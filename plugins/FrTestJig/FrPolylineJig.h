#pragma once
#include <FrPolyline.h>

#include "dbents.h"
#include "dbjig.h"
#include "dbmain.h"
#include "dbpl.h"
#include "gept2dar.h"

class FrPolylineJig : public AcEdJig {
    AcDbPolyline* line_;
    AcGePoint3d currentPt_;
    AcGePoint3d basePt_;

  public:
    FrPolylineJig(FrPolyline& line) : line_(AcDbPolyline::cast(line.entity())) {
        auto es = line_->getPointAt(0, basePt_);

        if (es != Acad::eOk) throw FrException(L"Не удалось создать полилинию");
        currentPt_ = basePt_;
    }

    AcDbEntity* entity() const override { return line_; }

    DragStatus sampler() override {
        AcGePoint3d newPt;

        setUserInputControls(kNullResponseAccepted);
        auto status = acquirePoint(newPt, basePt_);

        if (status != kNormal) return status;

        if (currentPt_.isEqualTo(newPt)) return kNoChange;

        currentPt_ = newPt;

        return kNormal;
    }

    bool update() override {
        auto index = line_->numVerts() - 1;
        AcGePoint2d pt(currentPt_.x, currentPt_.y);

        return line_->setPointAt(index, pt) == Acad::eOk;
    }

    void addNextVertex() {
        auto index = line_->numVerts() - 1;
        AcGePoint2d pt(currentPt_.x, currentPt_.y);

        auto es = line_->addVertexAt(index, pt);
        if (es != Acad::eOk) throw FrException(L"Не удалось добавить вершину");

        basePt_ = currentPt_;
    }

    void removeVertex() {
        auto index = line_->numVerts() - 1;

        auto es = line_->removeVertexAt(index);
        if (es != Acad::eOk) throw FrException(L"Не удалось удалить вершину");
    }
};