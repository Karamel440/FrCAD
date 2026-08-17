#include <dbobjptr.h>
#include <dbidmap.h>

#include "FrDatabase.h"
#include "FrEntity.h"
#include <FrLayer.h>

std::optional<FrLayer> FrRefDatabase::layer(const std::wstring& layerName) {
	AcDbObjectPointer<AcDbLayerTable> lt{
		db_->layerTableId(),
		AcDb::kForRead
	};
	
	Acad::ErrorStatus es =
		lt.openStatus();
	if (es != Acad::eOk)
		throw FrException{
		L"Ошибка при открытии таблицы слоев",
		es
	};

	AcDbObjectId id;
	es = lt->getAt(layerName.c_str(), id);

	if (es == Acad::eKeyNotFound)
		return std::nullopt;

	if (es != Acad::eOk)
		throw FrException{
		L"Ошибка при получении слоя",
		es
	};

	return FrLayer{ id };
}

std::vector<FrLayer> FrRefDatabase::layers() const {
	AcDbObjectPointer<AcDbLayerTable> lt{
		db_->layerTableId(),
		AcDb::kForRead
	};

	Acad::ErrorStatus es =
		lt.openStatus();
	if (es != Acad::eOk)
		throw FrException{
		L"Ошибка при открытии таблицы слоев",
		es
	};

	AcDbLayerTableIterator* it = nullptr;
	es = lt->newIterator(it);

	std::unique_ptr<AcDbLayerTableIterator> iter{
		it
	};

	FrCheckStatus(
		es,
		L"Ошибка при создании итератора таблицы слоёв"
	);


	std::vector<FrLayer> result;

	for (; !iter->done(); iter->step()) {
		AcDbObjectId id;

		const auto es = iter->getRecordId(id);
		if (es != Acad::eOk)
			continue;

		result.emplace_back(FrLayer{ id });
	}
	

	return result;
}

FrLayer FrRefDatabase::createLayer(const std::wstring& layerName) {
	AcDbObjectPointer<AcDbLayerTable> lt{
		db_->layerTableId(),
		AcDb::kForRead
	};

	Acad::ErrorStatus es =
		lt.openStatus();
	if (es != Acad::eOk)
		throw FrException{
		L"Ошибка при открытии таблицы слоев",
		es
	};

	AcDbObjectPointer<AcDbLayerTableRecord> ltr;
	es = ltr.create();
	if (es != Acad::eOk)
		throw FrException{
		L"Ошибка при создании объекта слоя",
		es
	};

	FrCheckStatus(
		ltr->setName(layerName.c_str()),
		L"Не удалось присвоить имя слою"
	);

	FrCheckStatus(
		lt->upgradeOpen(),
		L"Не удалось открыть запись в таблице слоев"
	);
	
	AcDbObjectId id;

	FrCheckStatus(
		lt->add(id, ltr.object()),
		L"Не удалось добавить слой в таблицу слоев"
	);
	
	return FrLayer{ id };
}

void FrRefDatabase::append(FrEntity& ent) {
	if (!ent.ent_)
		throw FrException{
		L"Сущность уже добавлена в бд"
	};

		AcDbObjectPointer<AcDbBlockTableRecord> space{
			db_->currentSpaceId(),
			AcDb::kForWrite
		};
		
		Acad::ErrorStatus es = space.openStatus();
		if (es != Acad::eOk)
			throw FrException{
			L"Не удалось открыть model space",
			es
		};

		AcDbObjectId id;
		AcDbEntity* entity = ent.ent_.get();

		es = space->appendAcDbEntity(id, entity);
		if (es != Acad::eOk)
			throw FrException{
			L"Не удалось добавить сущность в бд",
			es
		};
		ent.ent_.release();
		ent.id_ = id;

		entity->close();
}

void FrRefDatabase::saveAs(const std::filesystem::path& path) {

	FrCheckStatus(
		db_->saveAs(path.wstring().c_str()),
		L"Не удалось сохранить файл по выбранному пути"
	);
}

FrSpace FrRefDatabase::modelSpace() const
{
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

	return FrSpace{ modelSpaceId };
}

FrSpace FrRefDatabase::createSpace(const std::wstring& name) {
	AcDbObjectPointer<AcDbBlockTable> bt{
		db_->blockTableId(),
		AcDb::kForWrite
	};

	FrCheckStatus(
		bt.openStatus(),
		L"Не удалось открыть пространство"
	);

	AcDbObjectId id;
	if (bt->getAt(name.c_str(), id) == Acad::eOk)
		throw FrException{
		L"В бд уже есть такой блок"
	};

	AcDbObjectPointer<AcDbBlockTableRecord> space;

	FrCheckStatus(
		space.create(),
		L"Не удалось создать пространство"
	);

	FrCheckStatus(
		space->setName(name.c_str()),
		L"Не удалось задать имя пространства"
	);

	AcDbObjectId spaceId;

	FrCheckStatus(
		bt->add(spaceId, space.object()),
		L"Не удалось добавить пространство в таблицу блоков"
	);

	return FrSpace{ spaceId };
}

FrSpace FrRefDatabase::cloneSpace(const FrSpace& space) {
	AcDbObjectPointer<AcDbBlockTable> bt{
		db_->blockTableId(),
		AcDb::kForRead
	};

	FrCheckStatus(
		bt.openStatus(),
		L"Не удалось открыть запись"
	);

	AcDbObjectId id;
	if (bt->getAt(space.name().c_str(), id) == Acad::eOk)
		throw FrException{
		L"В бд уже есть такой блок"
	};

	AcDbObjectIdArray spaceId;
	spaceId.append(space.id_);
	
	AcDbIdMapping map;

	AcDbDatabase* sourceDb = space.id_.database();

	sourceDb->wblockCloneObjects(
		spaceId,
		db_->blockTableId(),
		map,
		AcDb::kDrcReplace
	);

	AcDbIdPair pair;
	
	pair.setKey(space.id_);
	if (!map.compute(pair) ||
		pair.value() == AcDbObjectId::kNull ||
		!pair.isCloned())
		throw FrException{
		L"Не удалось клонировать блок"
	};

	return FrSpace{ pair.value() };
}
