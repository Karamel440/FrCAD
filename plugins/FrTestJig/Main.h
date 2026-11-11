#pragma once
#include <FrPolyline.h>
#include <FrSelection.h>
#include <FrMap.h>
#include <adslib.h>
#include <dbcurve.h>
#include <dbobjptr.h>
#include <dbpl.h>

#include "FrPolylineJig.h"
#include "dbray.h"

inline void FrJigTest() {
    auto pt = FrGetPoint(L"Выберете первую вершину полилинии:\n");

    FrPolyline line;
    line.addVertex(pt);
    line.addVertex(pt);

    FrPolylineJig jig(line);

    while (true) {
        auto status = jig.drag();

        if (status == AcEdJig::kNormal) {
            jig.addNextVertex();
            continue;
        }
        if (status == AcEdJig::kNull) {
            jig.removeVertex();

            FrDbCurrent().append(line);
            break;
        }
        break;
    }
}

void FrTesty() {
    ads_name ent;
    ads_point pt;

    if (acedEntSel(L"Выберете секущую полилинию:\n", ent, pt) != RTNORM) return;

    AcDbObjectId id;
    if (acdbGetObjectId(id, ent) != Acad::eOk) return;

    AcDbObjectPointer<AcDbPolyline> boundary(id, AcDb::kForRead);

    if (boundary.openStatus() != Acad::eOk) return;

    ads_name cutEnt;
    ads_point cutPt;

    if (acedEntSel(L"Выберете разрезаемую полилинию:\n", cutEnt, cutPt) !=
        RTNORM)
        return;

    AcDbObjectId cutId;
    if (acdbGetObjectId(cutId, cutEnt) != Acad::eOk) return;

    AcDbObjectPointer<AcDbPolyline> cut(cutId, AcDb::kForWrite);

    if (cut.openStatus() != Acad::eOk) return;

    AcGePoint3dArray pts;

    if (boundary->intersectWith(cut, AcDb::kOnBothOperands, pts) != Acad::eOk)
        return;

    AcGeDoubleArray params;

    for (const auto pt : pts) {
        double param;

        if (cut->getParamAtPoint(pt, param) != Acad::eOk) return;

        params.append(param);
    }
    AcDbVoidPtrArray curves;

    auto es = cut->getSplitCurves(params, curves);

    if (es != Acad::eOk) return;

    auto db = acdbHostApplicationServices()->workingDatabase();
    auto msid = db->currentSpaceId();

    AcDbObjectPointer<AcDbBlockTableRecord> ms(msid, AcDb::kForWrite);

    if (ms.openStatus() != Acad::eOk) return;

    std::vector<AcDbPolyline*> lines;
    for (const auto& curve : curves) {
        auto base = static_cast<AcDbCurve*>(curve);

        double endParam;
        base->getEndParam(endParam);

        double length;
        base->getDistAtParam(endParam, length);

        AcGePoint3d midpoint;
        base->getPointAtDist(length / 2.0, midpoint);

        AcDbRay ray;

        ray.setBasePoint(midpoint);
        ray.setUnitDir(AcGeVector3d(1, 0, 0));

        AcGePoint3dArray intersections;

        auto es =
            ray.intersectWith(boundary, AcDb::kOnBothOperands, intersections);

        bool inside = intersections.length() % 2 != 0;

        if (inside) {
            delete curve;
            continue;
        }
        auto* poly = AcDbPolyline::cast(base);

        if (!poly) {
            delete base;
            return;
        }

        lines.push_back(poly);
    }

    if (lines.empty()) return;

    for (const auto& line : lines) {
        if (ms->appendAcDbEntity(line) != Acad::eOk) return;
        line->close();
    }
    if (cut->erase() != Acad::eOk) return;
}

void FrTestyC() { 
    auto db = FrDbCurrent();
    FrMap map{db};

    map.createLineSign(FrLineSign::rainSewer);
}