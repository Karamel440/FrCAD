#pragma once
#include <Open.h>

#include "FrCurve.h"

class FrCircle : public FrCurve {
    friend class FrRefDatabase;
    friend class FrObject;
    template <typename T>
    friend class Open;

  protected:
    FrCircle(AcDbObjectId id);

  public:
    using NativeType = AcDbCircle;

    FrCircle();
    explicit FrCircle(const AcGePoint3d& center);
    explicit FrCircle(double radius);
    FrCircle(const AcGePoint3d& center, double radius);

    void setRadius(double radius);
    void setCenter(const AcGePoint3d& center);

    double radius() const;
    AcGePoint3d center() const;
};