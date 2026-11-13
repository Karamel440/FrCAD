#include "FrPolyline.h"

#include <FrExceptions.h>
#include <dbobjptr.h>
#include <dbobjptr2.h>

void FrPolyline::addVertex(const AcGePoint3d& point) {
    if (id() != AcDbObjectId::kNull) {
        AcDbSmartObjectPointer<AcDbPolyline> pline{id(), AcDb::kForWrite};

        FrCheckStatus(pline.openStatus(),
                      L"Ошибка при открытии указателя на entity");

        FrCheckStatus(pline->addVertexAt(pline->numVerts(),
                                         AcGePoint2d{point.x, point.y}),
                      L"Не удалось добавить вершину полилинии");
        return;
    }

    if (!entity()) throw FrException{L"Некорректный объект"};

    AcDbPolyline* pline = AcDbPolyline::cast(entity());

    if (!pline) throw FrException{L"Некорректный объект"};

    FrCheckStatus(
        pline->addVertexAt(pline->numVerts(), AcGePoint2d{point.x, point.y}),
        L"Не удалось добавить вершину");
}

void FrPolyline::SetElevation(double elevation) {
    if (id() == AcDbObjectId::kNull && !entity())
        throw FrException{L"Не валидный объект"};

    if (id() != AcDbObjectId::kNull) {
        AcDbSmartObjectPointer<AcDbPolyline> line{id(), AcDb::kForWrite};
        line->setElevation(elevation);
        return;
    }
    auto* line = AcDbPolyline::cast(entity());
    line->setElevation(elevation);
}