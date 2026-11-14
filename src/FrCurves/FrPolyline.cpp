#include "FrPolyline.h"

#include <FrExceptions.h>
#include <Open.h>
#include <dbobjptr.h>
#include <dbobjptr2.h>

void FrPolyline::addVertex(const AcGePoint3d& point, size_t index) {
    auto line = open(*this, AcDb::kForWrite);

    FrCheckStatus(line->addVertexAt(index, {point.x, point.y}),
                  L"Не удалось добавить вершину");
}

void FrPolyline::addVertex(const AcGePoint3d& point) {
    auto line = open(*this, AcDb::kForWrite);

    FrCheckStatus(line->addVertexAt(line->numVerts(), {point.x,point.y}),
                  L"Не удалось добавить вершину");
}

void FrPolyline::removeVertex(size_t index) {
    auto line = open(*this, AcDb::kForWrite);

    FrCheckStatus(line->removeVertexAt(index), L"Не удалось удалить вершину");
}

void FrPolyline::setVertex(size_t index, const AcGePoint3d& point) {
    auto line = open(*this, AcDb::kForWrite);

    FrCheckStatus(line->setPointAt(index, {point.x, point.y}),
                  L"Не удалось удалить вершину");
}

void FrPolyline::setElevation(double elevation) {
    auto line = open(*this, AcDb::kForWrite);

    line->setElevation(elevation);
}

void FrPolyline::setClose() {
    auto line = open(*this, AcDb::kForWrite);
    line->setClosed(true);
}

void FrPolyline::setOpen() {
    auto line = open(*this, AcDb::kForWrite);
    line->setClosed(false);
}

AcGePoint3d FrPolyline::vertex(size_t index) const { 
    auto line = open(*this, AcDb::kForRead);
    AcGePoint3d pt;

    FrCheckStatus(line->getPointAt(index, pt), L"Не удалось получить точку");
    return pt;
}

size_t FrPolyline::vertexCount() const { 
    auto line = open(*this, AcDb::kForRead);
    return line->numVerts();
}

bool FrPolyline::closed() const { 
    auto line = open(*this, AcDb::kForRead);
    return line->isClosed();
}
