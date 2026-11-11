#include "Jigs.h"

FrPolylineJig::FrPolylineJig(FrPolyline& line)
    : line_(AcDbPolyline::cast(line.entity())) {
    auto es = line_->getPointAt(0, basePt_);

    if (es != Acad::eOk) throw FrException(L"Не удалось создать полилинию");
    currentPt_ = basePt_;
}



AcEdJig::DragStatus FrPolylineJig::sampler() {
    AcGePoint3d newPt;

    setUserInputControls(kNullResponseAccepted);
    auto status = acquirePoint(newPt, basePt_);

    if (status != kNormal) return status;
    if (currentPt_.isEqualTo(newPt)) return kNoChange;

    currentPt_ = newPt;

    return kNormal;
}

Adesk::Boolean FrPolylineJig::update() {
    auto index = line_->numVerts() - 1;
    AcGePoint2d pt(currentPt_.x, currentPt_.y);

    return line_->setPointAt(index, pt) == Acad::eOk;
}

AcDbEntity* FrPolylineJig::entity() const { return line_; }

void FrPolylineJig::addNextVertex() {
    auto index = line_->numVerts() - 1;
    AcGePoint2d pt(currentPt_.x, currentPt_.y);

    auto es = line_->addVertexAt(index, pt);
    if (es != Acad::eOk) throw FrException(L"Не удалось добавить вершину");

    basePt_ = currentPt_;
}

void FrPolylineJig::removeLastVertex() {
    auto index = line_->numVerts() - 1;

    auto es = line_->removeVertexAt(index);
    if (es != Acad::eOk) throw FrException(L"Не удалось удалить вершину");
}