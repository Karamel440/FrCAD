#pragma once
#include <FrEntity.h>
#include <vector>

class FrCurve : public FrEntity {
  protected:
    FrCurve(AcDbObjectId id) : FrEntity{id} {}
    FrCurve(std::unique_ptr<AcDbCurve> curve) : FrEntity(std::move(curve)) {}

  public:
    using NativeType = AcDbCurve;

    std::vector<FrCurve> split();
};