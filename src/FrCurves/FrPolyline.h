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

    void addVertex(const AcGePoint3d& point,size_t index);
    void removeVertex(size_t index);
    void SetElevation(double elevation);
    void setClose() const; 
    void setOpen() const;

    size_t numberOfVertex() const;
    bool closed() const;
};
