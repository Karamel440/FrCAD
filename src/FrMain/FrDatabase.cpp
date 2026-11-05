#include <dbobjptr.h>
#include <dbidmap.h>

#include "FrDatabase.h"
#include "FrEntity.h"
#include <FrLayer.h>

std::filesystem::path FrRefDatabase::path() const {
    const ACHAR* filename = nullptr;

    if (db_->getFilename(filename) != Acad::eOk || !filename || !*filename)
        throw FrException{L"Чертёж не имеет пути. Сохраните DWG."};

    return std::filesystem::path{filename};
}

std::filesystem::path FrRefDatabase::folder() const {
    return path().parent_path();
}

//common operations
void FrRefDatabase::append(FrEntity& ent) {
	return modelSpace().append(ent);
}

void FrRefDatabase::saveAs(const std::filesystem::path& path) {

	FrCheckStatus(
		db_->saveAs(path.wstring().c_str()),
		L"Не удалось сохранить файл по выбранному пути"
	);
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

std::vector<FrSpace> FrRefDatabase::spaces() const
{
	AcDbObjectPointer<AcDbBlockTable> blockTable{
		db_->blockTableId(),
		AcDb::kForRead
	};

	FrCheckStatus(
		blockTable.openStatus(),
		L"Не удалось открыть таблицу блоков"
	);

	AcDbBlockTableIterator* iter = nullptr;

	FrCheckStatus(
		blockTable->newIterator(iter),
		L"Не удалось создать итератор"
	);

	std::vector<FrSpace> result;

	for (iter->start(); !iter->done(); iter->step()) {
		AcDbObjectId id;
		
		auto es = iter->getRecordId(id);
		
		if (es != Acad::eOk)
			continue;

		AcDbObjectPointer<AcDbBlockTableRecord> rec{
			id,
			AcDb::kForRead
		};

		FrCheckStatus(
			rec.openStatus(),
			L"Не удалось открыть блок"
		);
		result.push_back(FrSpace{ id });
	}
	delete iter;
	return result;
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

std::optional<FrStyle> FrRefDatabase::style(const std::wstring& styleName)
{
	AcDbObjectPointer<AcDbTextStyleTable> st{
		db_->textStyleTableId(),
		AcDb::kForRead
	};
	
	FrCheckStatus(
		st.openStatus(),
		L"Не удалось открыть таблицу стилей"
	);
	
	AcDbObjectId id;
	
	auto status = st->getAt(styleName.c_str(), id);

	if (status == Acad::eKeyNotFound)
		return std::nullopt;

	FrCheckStatus(status, L"Не удалось получить стиль");

	return FrStyle{ id };
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
