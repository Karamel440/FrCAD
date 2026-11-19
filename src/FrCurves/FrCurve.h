#pragma once
#include <FrEntity.h>

#include <vector>

class FrCurve : public FrEntity {
  protected:
    FrCurve(AcDbObjectId id) : FrEntity{id} {}
    FrCurve(std::unique_ptr<AcDbCurve> curve) : FrEntity(std::move(curve)) {}

  public:
    using NativeType = AcDbCurve;

    double startParameter() const;
    double endParameter() const;

    double parameter(const AcGePoint3d& pt) const;
    double parameter(double distance) const;

    AcGePoint3d perpendicular(double parameter, double distance) const;
    AcGePoint3d perpendicular(const AcGePoint3d& pt, double distance) const;

    double distance(double parameter) const;
    double length() const;

    /// <summary>
    /// Разрезает исходную кривую на набор кривых.
    /// </summary>
    /// <returns>Набор кривых из исходной</returns>
    std::vector<FrCurve> split();
    std::vector<FrCurve> split(const std::vector<double>& parameters);
    std::vector<FrCurve> split(std::vector<AcGePoint3d> points);

    AcGePoint3d point(double parameter) const;
    AcGePoint3d pointAtDistance(double distance) const;

    AcGePoint3d closestPoint(const AcGePoint3d& pt) const;
};