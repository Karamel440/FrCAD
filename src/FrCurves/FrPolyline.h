#pragma once
#include <FrEntity.h>
#include <FrObject.h>
#include <aced.h>

#include "FrCurve.h"

class FrPolyline : public FrCurve {
    friend class FrRefDatabase;
    friend class FrObject;

    FrPolyline(AcDbObjectId id) : FrCurve{id} {}

  public:
    FrPolyline(): FrCurve(std::make_unique<AcDbPolyline>()) {}

    using NativeType = AcDbPolyline;

    void addVertex(const AcGePoint3d& point);
    void removeVertex(size_t index);
    void SetElevation(double elevation);
};
