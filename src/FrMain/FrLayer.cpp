#include <string>

#include <dbents.h>
#include <dbobjptr.h>

#include <FrLayer.h>
#include <FrExceptions.h>

std::wstring FrLayer::name() const {
	if (id_ == AcDbObjectId::kNull)
		throw FrException{ L"Некорректный слой" };

	AcDbObjectPointer<AcDbLayerTableRecord> layer{
		id_,
		AcDb::kForRead
	};

	FrCheckStatus(
		layer.openStatus(),
		L"Не удалось открыть слой"
	);

	return layer->name().kwszPtr();
}

bool FrLayer::isReference() const {
	if (id_ == AcDbObjectId::kNull)
		throw FrException{ L"Некорректный слой" };

	AcDbObjectPointer<AcDbLayerTableRecord> layer{
		id_,
		AcDb::kForRead
	};

	FrCheckStatus(
		layer.openStatus(),
		L"Не удалось открыть слой"
	);

	return layer->isDependent();
}

void FrLayer::off() const {
	AcDbObjectPointer<AcDbLayerTableRecord> layer{
		id_,
		AcDb::kForWrite
	};


	FrCheckStatus(
		layer.openStatus(),
		L"Не удалось открыть слой"
	);

	layer->setIsOff(true);
}

void FrLayer::on() const {
	AcDbObjectPointer<AcDbLayerTableRecord> layer{
		id_,
		AcDb::kForWrite
	};

	FrCheckStatus(
		layer.openStatus(),
		L"Не удалось открыть слой"
	);

	layer->setIsOff(false);
}

void FrLayer::freeze() const {
	AcDbObjectPointer<AcDbLayerTableRecord> layer{
		id_,
		AcDb::kForWrite
	};

	FrCheckStatus(
		layer.openStatus(),
		L"Не удалось открыть слой"
	);

	layer->setIsFrozen(true);
}

void FrLayer::unfreeze() const {
	AcDbObjectPointer<AcDbLayerTableRecord> layer{
		id_,
		AcDb::kForWrite
	};

	FrCheckStatus(
		layer.openStatus(),
		L"Не удалось открыть слой"
	);

	layer->setIsFrozen(false);
}