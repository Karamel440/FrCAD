#include "FrPolyline.h"

#include <FrExceptions.h>
#include <Open.h>
#include <dbobjptr.h>
#include <dbobjptr2.h>

#include <numbers>

void FrPolyline::addVertex(const AcGePoint3d& point, size_t index) {
    auto line = open(*this, AcDb::kForWrite);

    FrCheckStatus(line->addVertexAt(index, {point.x, point.y}),
                  L"Не удалось добавить вершину");
}

void FrPolyline::addVertex(const AcGePoint3d& point) {
    auto line = open(*this, AcDb::kForWrite);

    FrCheckStatus(line->addVertexAt(line->numVerts(), {point.x, point.y}),
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

size_t FrPolyline::segment(double parameter) const {
    auto start = startParameter();
    auto end = endParameter();

    if (!std::isfinite(parameter) || parameter < start || parameter > end) {
        throw FrException{L"Параметр вне диапазона кривой"};
    }

    if (parameter == end) {
        return segmentsCount() - 1;
    }

    return static_cast<size_t>(std::floor(parameter));
}

double FrPolyline::segmentAngle(size_t segmentIndex) const {
    auto pl = open(*this, AcDb::kForRead);

    if (segmentIndex >= segmentsCount()) {
        throw FrException{L"Некорректный индекс сегмента"};
    }

    auto index = static_cast<unsigned int>(segmentIndex);

    if (pl->segType(index) != AcDbPolyline::kLine) {
        throw FrException{L"Сегмент не является прямым"};
    }

    AcGeLineSeg3d lineSeg;

    FrCheckStatus(pl->getLineSegAt(index, lineSeg),
                  L"Не удалось получить сегмент");

    AcGeVector3d direction = lineSeg.endPoint() - lineSeg.startPoint();

    return std::atan2(direction.y, direction.x) * 180.0 / std::numbers::pi;
}

double FrPolyline::segmentLength(size_t segmentIndex) const {
    if (segmentIndex >= segmentsCount())
        throw FrException{L"Некорректный индекс сегмента"};

    return distance(static_cast<double>(segmentIndex + 1)) -
           distance(static_cast<double>(segmentIndex));
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

size_t FrPolyline::segmentsCount() const {
    size_t count = vertexCount();

    if (count < 2) return 0;

    return closed() ? count : count - 1;
}

size_t FrPolyline::closestSegment(const AcGePoint3d& pt) const {
    auto closest = closestPoint(pt);
    auto param = parameter(closest);

    return segment(param);
}

bool FrPolyline::closed() const {
    auto line = open(*this, AcDb::kForRead);
    return line->isClosed();
}

bool FrPolyline::isArc(size_t segmentIndex) const {
    if (segmentIndex >= segmentsCount())
        throw FrException{L"Некорректный индекс сегмента"};

    auto pl = open(*this, AcDb::kForRead);

    return pl->segType(static_cast<unsigned int>(segmentIndex)) ==
           AcDbPolyline::kArc;
}
