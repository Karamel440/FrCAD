#pragma once
#include <FrEntity.h>
#include <FrObject.h>
#include <aced.h>

#include "FrCurve.h"

class FrPolyline : public FrCurve {
    friend class FrRefDatabase;
    friend class FrObject;

    explicit FrPolyline(AcDbObjectId id) : FrCurve{id} {}

  public:
    FrPolyline(): FrCurve(std::make_unique<AcDbPolyline>()) {}

    using NativeType = AcDbPolyline;

    void addVertex(const AcGePoint3d& point,size_t index);
    void addVertex(const AcGePoint3d& point);
    void removeVertex(size_t index);
    void setVertex(size_t index, const AcGePoint3d& point);

    size_t segment(double parameter) const;
    double segmentAngle(size_t segmentIndex) const;
    double segmentLength(size_t segmentIndex) const;

    void setElevation(double elevation);
    void setClose(); 
    void setOpen();

    AcGePoint3d vertex(size_t index) const;
    size_t vertexCount() const;
    size_t segmentsCount() const;
    size_t closestSegment(const AcGePoint3d& pt) const;

    bool closed() const;
    bool isArc(size_t segmentIndex) const;
};
