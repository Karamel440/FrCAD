#include <dbobjptr.h>
#include <dbidmap.h>

#include "FrDatabase.h"
#include "FrEntity.h"
#include <FrLayer.h>

//common operations
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

std::vector<FrEntity> FrRefDatabase::clone(const std::vector<FrEntity>& ents) {
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



	std::vector<FrEntity> result;

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
		result.push_back(FrEntity{ pair.value() });
	}

	return result;
}

//FrLayer
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


//FrSpace
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

std::optional<FrSpace> FrRefDatabase::space(const std::wstring& name) const
{
	AcDbObjectPointer<AcDbBlockTable> blockTable{
		db_->blockTableId(),
		AcDb::kForRead
	};

	FrCheckStatus(
		blockTable.openStatus(),
		L"Не удалось открыть таблицу блоков"
	);

	AcDbObjectId id;
	auto es = blockTable->getAt(name.c_str(), id);

	if (es != Acad::eOk)
		return std::nullopt;

	FrSpace space{ id };

	return std::optional<FrSpace>(space);
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

	bt.close();

	AcDbIdMapping map;

	AcDbDatabase* sourceDb = space.id_.database();

	auto es = sourceDb->wblockCloneObjects(
		spaceId,
		db_->blockTableId(),
		map,
		AcDb::kDrcReplace
	);

	if (es != Acad::eOk)
		throw FrException{
		L"не удалось скопировать блок",
		es
	};

	AcDbIdPair pair;

	pair.setKey(space.id_);

	auto comp = map.compute(pair);

	if (!comp)
		throw FrException{
		L"Не удалось получить вставленную сущность"
	};

	if (pair.value() == AcDbObjectId::kNull)
		throw FrException{
		L"Пустая сущность"
	};

	if (!pair.isCloned())
		throw FrException{
		L"Сущность не склонирована"
	};

	return FrSpace{ pair.value() };
}

AcDbDatabase* FrDatabase::loadDatabase(const std::filesystem::path& fileToRead)
{
	AcDbDatabase* temp = new AcDbDatabase(Adesk::kFalse,Adesk::kTrue);
	
	auto es = 
		temp->readDwgFile(fileToRead.c_str());

	if (es != Acad::eOk) {
		delete temp;
		throw FrException{
		L"Не удалось прочитать файл!",
		es
		};
	}

	return temp;
}

//common for FrDatabase
void FrDatabase::read(std::filesystem::path& path)
{
	if (path.extension() != L".dwg")
		throw FrException{
		L"Неверное имя файла!"
	};

	FrCheckStatus(
		db_->readDwgFile(path.c_str()),
		L"Не удалось прочитать файл"
	);
}
