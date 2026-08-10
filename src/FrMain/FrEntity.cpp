#include "FrEntity.h"
#include "FrExceptions.h"
#include <dbobjptr2.h>

FrEntity::~FrEntity() = default;

void FrEntity::setLayer(const FrLayer& layer)
{
	if (ent_) {
		Acad::ErrorStatus es =
			ent_->setLayer(layer.id_);
		if (es != Acad::eOk)
			throw FrException{ L"Произошла ошибка при настройке слоя" };
		return;
	}

	if (!id_.isNull()) {
		AcDbSmartObjectPointer<AcDbEntity> ent{
			id_,
			AcDb::kForWrite
		};

		Acad::ErrorStatus es = ent.openStatus();
		if (es != Acad::eOk)
			throw FrException{ L"Произошла ошибка при настройке слоя по id" };

		es = ent->setLayer(layer.id_);
		if (es != Acad::eOk)
			throw FrException{ L"Произошла ошибка при выставлении слоя по id" };
		return;
	}

	throw FrException{
	L"Сущность не содержит объекта"
	};
}

void FrEntity::setColor(int index) {
	if (!ent_ && id_ == AcDbObjectId::kNull)
		throw FrException{ L"Пустой объект" };

	if (id_ != AcDbObjectId::kNull) {
		AcDbObjectPointer<AcDbEntity> ent{
			id_,
			AcDb::kForWrite
		};
		FrCheckStatus(
			ent.openStatus(),
			L"Ошибка при открытии объекта"
		);

		FrCheckStatus(
			ent->setColorIndex(index),
			L"Не удалось установить цвет"
		);
		return;
	}

	FrCheckStatus(
		ent_->setColorIndex(index),
		L"Не удалось установить цвет"
	);
}

void FrEntity::setLineWeight(AcDb::LineWeight weight) {
	if (!ent_ && id_ == AcDbObjectId::kNull)
		throw FrException{ L"Пустой объект" };

	if (id_ != AcDbObjectId::kNull) {
		AcDbObjectPointer<AcDbEntity> ent{
			id_,
			AcDb::kForWrite
		};
		FrCheckStatus(
			ent.openStatus(),
			L"Ошибка при открытии объекта"
		);

		FrCheckStatus(
			ent->setLineWeight(weight),
			L"Не удалось установить вес линии"
		);
		return;
	}

	FrCheckStatus(
		ent_->setLineWeight(weight),
		L"Не удалось установить вес линии"
	);
}

void FrEntity::setLinetype(const std::wstring& name) {
	if (!ent_ && id_ == AcDbObjectId::kNull)
		throw FrException{ L"Пустой объект" };
	
	if (ent_) {
		FrCheckStatus(
			ent_->setLinetype(name.c_str()),
			L"Не удалось установить тип линии"
		);
	}

	AcDbObjectPointer<AcDbEntity> entity{
		id_,
		AcDb::kForWrite
	};

	FrCheckStatus(
		entity.openStatus(),
		L"Не удалось открыть сущность"
	);

	FrCheckStatus(
		entity->setLinetype(name.c_str()),
		L"Не удалось установить тип линии"
	);
}

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