#pragma once
#include <aced.h>
#include <stdexcept>
#include <filesystem>
#include <optional>
#include <dbobjptr2.h>
#include <dbapserv.h>
#include <vector>
#include <dbidmap.h>

#include "FrEntity.h"
#include "FrExceptions.h"
#include "FrLayer.h"
#include "FrSpace.h"
#include "FrStyle.h"

class FrRefDatabase {
protected:
	AcDbDatabase* db_;

public:
	FrRefDatabase() = delete;

	explicit FrRefDatabase(AcDbDatabase* db) : db_(db) {
		if (!db_)
			throw FrException {
			L"Ошибка конструктора FrRefDatabase"
		};
	}

	template <typename T = FrEntity>
		requires std::derived_from<T, FrEntity>
	std::vector<T> clone(const std::vector<T>& ents) {
		AcDbObjectIdArray ids;

		AcDbDatabase* tempDb = nullptr;

		for (const auto& ent : ents) {
			if (ent.id_ == AcDbObjectId::kNull)
				throw FrException{
				L"Не добавленная в бд сущность!"
			};

			if (ent.id_.isErased())
				throw FrException{
				L"стертая сущность!"
			};

			if (!tempDb)
				tempDb = ent.id_.database();

			if (tempDb == db_)
				throw FrException{
				L"Копирование в ту же бд запрещено!"
			};

			if (ent.id_.database() != tempDb)
				throw FrException{
				L"Сущности из разных бд!"
			};
			ids.append(ent.id_);
		}

		AcDbObjectPointer<AcDbBlockTable> blockTable{
			db_->blockTableId(),
			AcDb::kForRead
		};

		FrCheckStatus(
			blockTable.openStatus(),
			L"Не удалось открыть таблицу блоков"
		);

		AcDbObjectId modelSpaceId;

		FrCheckStatus(
			blockTable->getAt(
				ACDB_MODEL_SPACE,
				modelSpaceId
			),
			L"Не удалось получить Model Space"
		);

		blockTable.close();

		AcDbIdMapping map;

		auto es = tempDb->wblockCloneObjects(
			ids,
			modelSpaceId,
			map,
			AcDb::kDrcIgnore
		);

		if (es != Acad::eOk)
			throw FrException{
			L"Не удалось клонировать сущности",
			es
		};



		std::vector<T> result;

		for (const auto& ent : ents) {
			AcDbIdPair pair;
			pair.setKey(ent.id_);

			if (!map.compute(pair))
				throw FrException{
				L"Не найдены клоны объектов!"
			};

			if (pair.value() == AcDbObjectId::kNull)
				throw FrException{
				L"Некорректный объект"
			};
			result.push_back(
				FrObject{ pair.value() }.as<T>()
			);
		}

		return result;
	}


	std::optional<FrLayer> layer(const std::wstring& layerName);

	std::vector<FrLayer> layers() const;


	std::optional<FrSpace> space(const std::wstring& name) const;

	std::vector<FrSpace> spaces() const;

	FrSpace modelSpace() const;

	FrSpace createSpace(const std::wstring& name);

	FrSpace cloneSpace(const FrSpace& space);


	std::optional<FrStyle> style(const std::wstring& styleName);


	FrLayer createLayer (const std::wstring& layerName);

	template <typename T>
		requires std::derived_from<T, FrEntity>
	std::vector<T> entities() const {
		return modelSpace().entities<T>();
	}

	void append(FrEntity& ent);

	void saveAs(const std::filesystem::path& path);
};

class FrDatabase : public FrRefDatabase {

	static AcDbDatabase* loadDatabase(const std::filesystem::path& fileToRead);
	void read(std::filesystem::path& path);

public:
	~FrDatabase() {
		delete db_;
	}

	FrDatabase()
		:FrRefDatabase(new AcDbDatabase(Adesk::kTrue, Adesk::kTrue))
	{}

	explicit FrDatabase(const std::filesystem::path& fileToRead)
		:FrRefDatabase(loadDatabase(fileToRead)) {}

	FrDatabase(const FrDatabase&) = delete;
	FrDatabase& operator=(const FrDatabase&) = delete;

	FrDatabase(FrDatabase&& other) noexcept
		: FrRefDatabase(other.db_) {
		other.db_ = nullptr;
	}

	FrDatabase& operator=(FrDatabase&& other) noexcept {
		if (this != &other) {
			delete db_;

			db_ = other.db_;
			other.db_ = nullptr;
		}
		return *this;
	}
};

inline FrRefDatabase FrDbCurrent() {
	return FrRefDatabase{
		acdbHostApplicationServices()->workingDatabase()
	};
}