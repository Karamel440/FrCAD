#include "FrCurve.h"

#include <FrExceptions.h>
#include <Open.h>

double FrCurve::startParameter() const {
    auto curve = open(*this, AcDb::kForRead);
    double param;

    FrCheckStatus(curve->getStartParam(param),
                  L"не удалось получить стартовый параметр");
    return param;
}

double FrCurve::endParameter() const {
    auto curve = open(*this, AcDb::kForRead);
    double param;

    FrCheckStatus(curve->getEndParam(param),
                  L"не удалось получить конечный параметр");
    return param;
}

double FrCurve::parameter(const AcGePoint3d& pt) const {
    auto curve = open(*this, AcDb::kForRead);
    double param;

    FrCheckStatus(curve->getParamAtPoint(pt, param),
                  L"не удалось получить параметр");
    return param;
}

double FrCurve::parameter(double distance) const { 
    auto curve = open(*this, AcDb::kForRead);
    double param;

    FrCheckStatus(curve->getParamAtDist(distance, param),
                  L"не удалось получить параметр");
    return param;

}

double FrCurve::distance(double parameter) const {
    auto curve = open(*this, AcDb::kForRead);
    double dist;

    FrCheckStatus(curve->getDistAtParam(parameter,dist),
                  L"не удалось получить расстояние");
    return dist;
}

double FrCurve::length() const {
    auto curve = open(*this, AcDb::kForRead);

    double endParam;
    double length;

    FrCheckStatus(curve->getEndParam(endParam),
                  L"Не удалось получить конечный параметр");

    FrCheckStatus(curve->getDistAtParam(endParam, length),
                  L"Не удалось получить длину кривой");

    return length;
}

AcGePoint3d FrCurve::point(double parameter) const {
    auto curve = open(*this, AcDb::kForRead);
    AcGePoint3d pt;

    FrCheckStatus(curve->getPointAtParam(parameter, pt),
                  L"не удалось получить точку");
    return pt;
}

AcGePoint3d FrCurve::pointAtDistance(double distance) const {
    auto curve = open(*this, AcDb::kForRead);
    AcGePoint3d pt;

    FrCheckStatus(curve->getPointAtDist(distance, pt),
                  L"не удалось получить точку");
    return pt;
}

AcGePoint3d FrCurve::closestPoint(const AcGePoint3d& pt) const {
    auto curve = open(*this, AcDb::kForRead);

    AcGePoint3d result;

    FrCheckStatus(curve->getClosestPointTo(pt, result),
                  L"не удалось получить ближайшую точку");
    return result;
}
