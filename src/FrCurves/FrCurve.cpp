#include "FrCurve.h"

#include <FrExceptions.h>
#include <Open.h>
#include <dbpl.h>

#include <algorithm>
#include <cmath>
#include <vector>

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

AcGePoint3d FrCurve::perpendicular(double parameter, double distance) const {
    auto curve = open(*this, AcDb::kForRead);

    AcGePoint3d pt;

    FrCheckStatus(curve->getPointAtParam(parameter, pt),
                  L"Ќе удалось получить точку");

    AcGeVector3d tangent;

    FrCheckStatus(curve->getFirstDeriv(parameter, tangent),
                  L"Ќе удалось получить касательную");

    AcGeVector3d perpendicular{-tangent.y, tangent.x, 0.0};

    perpendicular.normalize();

    return pt + perpendicular * distance;
}

AcGePoint3d FrCurve::perpendicular(const AcGePoint3d& pt,
                                   double distance) const {
    return perpendicular(parameter(pt), distance);
}

double FrCurve::distance(double parameter) const {
    auto curve = open(*this, AcDb::kForRead);
    double dist;

    FrCheckStatus(curve->getDistAtParam(parameter,dist),
                  L"не удалось получить рассто€ние");
    return dist;
}

double FrCurve::length() const {
    auto curve = open(*this, AcDb::kForRead);

    double endParam;
    double length;

    FrCheckStatus(curve->getEndParam(endParam),
                  L"Ќе удалось получить конечный параметр");

    FrCheckStatus(curve->getDistAtParam(endParam, length),
                  L"Ќе удалось получить длину кривой");

    return length;
}

std::vector<FrCurve> FrCurve::split(std::vector<double>& parameters) {
    if (parameters.empty())
        throw FrException{L"ѕереданы некорректные параметры"};

    const auto start = startParameter();
    const auto end = endParameter();
    constexpr auto tolerance = 1e-8;
    const auto closed = open<>(*this, AcDb::kForRead)->isClosed();

    std::sort(parameters.begin(), parameters.end());

    AcGeDoubleArray params;

    for (auto& param : parameters) {
        if (!closed && (param <= start + tolerance || param >= end - tolerance))
            continue;

        params.append(param);
    }

    AcGeVoidPointerArray curves;

    std::vector<FrCurve> result;

    if (params.isEmpty()) return {};

    try {
        auto curve = open<>(*this, AcDb::kForWrite);

        FrCheckStatus(curve->getSplitCurves(params, curves),
                      L"Ќе удалось разделить кривую");

        result.reserve(curves.length());

        for (int i{}; i < curves.length(); i++) {
            std::unique_ptr<AcDbCurve> ptr{static_cast<AcDbCurve*>(curves[i])};
            curves[i] = nullptr;

            FrCurve piece{std::move(ptr)};
            if (piece.length() > 0.0) result.push_back(std::move(piece));
        }
    } catch (...) {
        for (int i{}; i < curves.length(); i++) {
            delete static_cast<AcDbCurve*>(curves[i]);
        }
        throw;
    }
    

    if (!result.empty() && isInDatabase()) erase();

    return result;
}

bool FrCurve::contains(const AcGePoint3d& point) const {
    constexpr double tolerance = 1e-6;

    auto curve = open(*this, AcDb::kForRead);
    auto* poly = AcDbPolyline::cast(curve.operator->());

    if (!poly || !poly->isClosed() || poly->numVerts() < 3)
        throw FrException{L" рива€ не €вл€етс€ замкнутой полилинией"};

    if (poly->hasBulges())
        throw FrException{L"√раница не должна содержать дуги"};

    bool inside = false;
    const int count = poly->numVerts();

    for (int i = 0; i < count; ++i) {
        AcGePoint3d a, b;

       FrCheckStatus(poly->getPointAt(i, a),
                      L"Ќе удалось получить вершину полилинии");

        FrCheckStatus(poly->getPointAt((i + 1) % count, b),
                      L"Ќе удалось получить вершину полилинии");

        const double dx = b.x - a.x;
        const double dy = b.y - a.y;
        const double len2 = dx * dx + dy * dy;

        // ѕроверка принадлежности точки отрезку
        if (len2 > 0.0) {
            const double t = std::clamp(
                ((point.x - a.x) * dx + (point.y - a.y) * dy) / len2, 0.0, 1.0);

            const double x = a.x + t * dx;
            const double y = a.y + t * dy;

            const double dist2 =
                (point.x - x) * (point.x - x) + (point.y - y) * (point.y - y);

            if (dist2 <= tolerance * tolerance) return true;
        }

        // Ray Casting
        if ((a.y > point.y) != (b.y > point.y)) {
            const double t = (point.y - a.y) / (b.y - a.y);

            const double x = a.x + t * dx;

            if (x > point.x) inside = !inside;
        }
    }

    return inside;
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
