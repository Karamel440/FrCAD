#include "FrBlockReference.h"

#include <numbers>

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

void FrBlockReference::setRotation(double degree) {
	
	const double radians = degree * std::numbers::pi / 180.0;

	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {

		auto ent = AcDbBlockReference::cast(entity());

		FrCheckStatus(
			ent->setRotation(radians),
			L"Не удалось установить поворот блока"
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
		ent->setRotation(radians),
		L"Не удалось установить поворот блока"
}


AcGePoint3d FrBlockReference::position() const {
	
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		auto ent = AcDbBlockReference::cast(entity());

		return ent->position();
	}

	AcDbObjectPointer<AcDbBlockReference> ent{
		id(),
		AcDb::kForRead
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть блок"
	);

	return ent->position();
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