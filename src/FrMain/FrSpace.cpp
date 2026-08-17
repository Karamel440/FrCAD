#include <dbobjptr.h>

#include <FrExceptions.h>

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