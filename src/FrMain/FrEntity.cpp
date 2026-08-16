#include <string>

#include <dbobjptr2.h>

#include "FrEntity.h"
#include "FrExceptions.h"
#include "FrLayer.h"

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
		return;
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

FrLayer FrEntity::layer() const {
	if (ent_) {
		return FrLayer{ ent_->layerId() };
	}

	AcDbObjectPointer<AcDbEntity> entity{
		id_,
		AcDb::kForRead
	};

	FrCheckStatus(
		entity.openStatus(),
		L"Не удалось открыть сущность"
	);

	return FrLayer{ entity->layerId() };
}
