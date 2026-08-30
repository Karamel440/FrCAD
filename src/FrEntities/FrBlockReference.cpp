#include "FrBlockReference.h"

void FrBlockReference::setPosition(AcGePoint3d point){
	
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {

		auto ent = AcDbBlockReference::cast(entity());

		FrCheckStatus(
			ent->setPosition(point),
			L"Не удалось установить позицию блока"
		);
		return;
	}

	AcDbObjectPointer<AcDbBlockReference> ent{
		id(),
		AcDb::kForWrite
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть блок"
	);

	FrCheckStatus(
		ent->setPosition(point),
		L"Не удалось установить позицию блока"
	);
}

FrSpace FrBlockReference::space() const {
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		auto ent = AcDbBlockReference::cast(entity());
		auto btr = ent->blockTableRecord();

		return FrSpace{ btr };
	}
}