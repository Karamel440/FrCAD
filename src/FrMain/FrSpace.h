#pragma once
#include <aced.h>

#include <FrObject.h>
#include <FrEntity.h>

class FrSpace : public FrObject {
	friend class FrRefDatabase;

	FrSpace(AcDbObjectId id)
		: FrObject{ id } {}

public:
	using NativeType = AcDbBlockTableRecord;

	void append(FrEntity& ent);

	template <typename T>
	requires std::derived_from<T, FrEntity>
	std::vector<T> entities() const {
		
		AcDbObjectPointer<AcDbBlockTableRecord> space{
	id_,
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

			es = iter->getEntityId(id);

			if (es != Acad::eOk)
				continue;

			if (id.isNull()) {
				continue;
			}

			FrObject obj{ id };

			if (!obj.is<T>()) {
				continue;
			}

			result.push_back(obj.as<T>());
		}
		return result;
	}

	std::wstring name() const;
	AcDbObjectId id() const;
};
