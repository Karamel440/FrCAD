#pragma once
#include <aced.h>
#include <stdexcept>
#include <filesystem>
#include <optional>
#include <dbobjptr2.h>
#include <dbapserv.h>

#include "FrEntity.h"
#include "FrExceptions.h"

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

	std::optional<FrLayer> layer(const std::wstring& layerName);

	std::vector<FrLayer> layers() const;

	FrLayer createLayer (const std::wstring& layerName);

	template <typename T>
	std::vector<T> entities() const {
		AcDbObjectPointer<AcDbBlockTableRecord> space{
	db_->currentSpaceId(),
	AcDb::kForRead
		};
		 Acad::ErrorStatus es =
			space.openStatus();

			FrCheckStatus(
				es,
				L"Не удалось открыть ms"
			);

		AcDbBlockTableRecordIterator* rawIter = nullptr;

		es = space->newIterator(rawIter);

		std::unique_ptr<AcDbBlockTableRecordIterator> iter{
			rawIter
		};

		FrCheckStatus(
			es,
			L"Не удалось создать итератор"
		);

		std::vector<T> result;

		for (; !iter->done(); iter->step()) {
			AcDbObjectId id;

			using NativeType = typename T::NativeType;

			es = iter->getEntityId(id);

			if (es != Acad::eOk)
				continue;

			AcRxClass* objectClass = id.objectClass();

			if (!objectClass ||
				!objectClass->isDerivedFrom(NativeType::desc()))
			{
				continue;
			}



			result.emplace_back(T{ id });
		}
		return result;
	}

	void append(FrEntity& ent);

	void saveAs(const std::filesystem::path& path);
};

class FrDatabase : public FrRefDatabase {
public:
	~FrDatabase() {
		delete db_;
	}

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

	void read(std::filesystem::path& path);
};

inline FrRefDatabase FrDbCurrent() {
	return FrRefDatabase{
		acdbHostApplicationServices()->workingDatabase()
	};
}
