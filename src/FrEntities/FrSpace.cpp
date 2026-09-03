#include <dbobjptr.h>

#include <FrExceptions.h>
#include <FrDatabase.h>

#include "FrSpace.h"

void FrSpace::append(FrEntity& ent)
{
    if (!ent.ent_) {
        throw FrException{
            L"Сущность уже добавлена в базу данных"
        };
    }

    AcDbObjectPointer<AcDbBlockTableRecord> space{
        id_,
        AcDb::kForWrite
    };

    FrCheckStatus(
        space.openStatus(),
        L"Не удалось открыть пространство"
    );

    AcDbObjectId entityId;
    AcDbEntity* native = ent.ent_.get();

    FrCheckStatus(
        space->appendAcDbEntity(entityId, native),
        L"Не удалось добавить сущность в пространство"
    );

    ent.ent_.release();
    ent.id_ = entityId;

    native->close();
}

std::wstring FrSpace::name() const
{
    AcDbObjectPointer<AcDbBlockTableRecord> space{
        id_,
        AcDb::kForRead
    };

    FrCheckStatus(
        space.openStatus(),
        L"Не удалось открыть пространство"
    );

    AcString name;

    FrCheckStatus(
        space->getName(name),
        L"Не удалось получить имя пространства"
    );

    return static_cast<const ACHAR*>(name);
}

AcDbObjectId FrSpace::id() const {
    return id_;
}

FrRefDatabase FrSpace::database() const {
    AcDbObjectPointer<AcDbBlockTableRecord> space{
        id_,
        AcDb::kForRead
    };

    FrCheckStatus(
        space.openStatus(),
        L"Не удалось открыть пространство"
    );

    if (space->isFromExternalReference()) {

        auto db = space->xrefDatabase();

        if (!db)
            throw FrException{
                L"Не удалось получить базу внешней ссылки"
        };

        return FrRefDatabase{ db };
    }

    return FrRefDatabase{ space->database() };
}

bool FrSpace::isModelSpace() const
{
    AcDbObjectPointer <AcDbBlockTable> bt{
        id_.database()->blockTableId(),
        AcDb::kForRead
    };

    FrCheckStatus(
        bt.openStatus(),
        L"Не удалось открыть таблицу блоков"
    );

    AcDbObjectId msId;

    FrCheckStatus(
        bt->getAt(ACDB_MODEL_SPACE, msId),
        L"Не удалось получить имя пространства модели"
    );
    
    return id_ == msId;
}

bool FrSpace::isLayout() const
{
    AcDbObjectPointer<AcDbBlockTableRecord> btr{
        id_,
        AcDb::kForRead
    };

    FrCheckStatus(
        btr.openStatus(),
        L"Не удалось открыть пространство"
    );

    return btr->isLayout();
}

bool FrSpace::isReference() const {
    AcDbObjectPointer<AcDbBlockTableRecord> btr{
        id_,
        AcDb::kForRead
    };

    FrCheckStatus(
        btr.openStatus(),
        L"Не удалось открыть пространство"
    );

    return btr->isFromExternalReference();
}
