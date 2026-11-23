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

std::vector<FrCurve> FrCurve::split(const std::vector<double>& parameters) {
    if (parameters.empty())
        throw FrException{L"Ќе указаны параметры разделени€ кривой"};

    const double start = startParameter();
    const double end = endParameter();
    std::vector<double> sortedParameters = parameters;
    for (double param : sortedParameters) {
        if (!std::isfinite(param) || param < start || param > end)
            throw FrException{L"ѕараметр разделени€ вне диапазона кривой",
                              Acad::eInvalidInput};
    }
    std::sort(sortedParameters.begin(), sortedParameters.end());
    sortedParameters.erase(
        std::unique(sortedParameters.begin(), sortedParameters.end()),
        sortedParameters.end());

    const bool closed = open(*this, AcDb::kForRead)->isClosed();
    if (!closed) {
        constexpr double endpointTolerance = 1e-6;
        const double totalLength = length();
        std::erase_if(sortedParameters, [&](double param) {
            const double along = distance(param);
            return along <= endpointTolerance || totalLength - along <= endpointTolerance;
        });
        if (sortedParameters.empty()) return {};
    }

    AcGeDoubleArray params;
    for (double param : sortedParameters) {
        params.append(param);
    }

    auto curve = open(*this, AcDb::kForWrite);

    AcDbVoidPtrArray segments;
    auto status = curve->getSplitCurves(params, segments);

    struct Cleanup {
        AcDbVoidPtrArray& segments;

        ~Cleanup() {
            for (int i = 0; i < segments.length(); ++i) {
                delete static_cast<AcDbCurve*>(segments[i]);
            }
        }
    };

    Cleanup cleanup{segments};

    FrCheckStatus(status, L"Ќе удалось разделить кривую");

    std::vector<FrCurve> result;
    result.reserve(segments.length());

    for (int i = 0; i < segments.length(); ++i) {
        auto* raw = static_cast<AcDbCurve*>(segments[i]);

        std::unique_ptr<AcDbCurve> owned{raw};
        segments[i] = nullptr;

        FrCurve piece{std::move(owned)};
        if (piece.length() > 0.0) result.push_back(std::move(piece));
    }

    if (result.empty())
        throw FrException{L"–азрезание не дало фрагментов ненулевой длины",
                          Acad::eDegenerateGeometry};
    if (!closed && result.size() == 1) return {};
    if (isInDatabase())
        FrCheckStatus(curve->erase(), L"Ќе удалось удалить исходную кривую");
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
