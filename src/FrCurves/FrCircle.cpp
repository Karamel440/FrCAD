#include "FrCircle.h"

FrCircle::FrCircle(AcDbObjectId id) : FrCurve{id} {}

FrCircle::FrCircle() : FrCircle{AcGePoint3d{0.0, 0.0, 0.0}, 1.0} {}

FrCircle::FrCircle(const AcGePoint3d& center) : FrCircle{center, 1.0} {}

FrCircle::FrCircle(double radius)
    : FrCircle{AcGePoint3d{0.0, 0.0, 0.0}, radius} {}

FrCircle::FrCircle(const AcGePoint3d& center, double radius)
    : FrCurve{std::make_unique<AcDbCircle>(center, AcGeVector3d{0.0, 0.0, 1.0},
                                           radius)} {}

void FrCircle::setRadius(double radius) {
    auto circle = open<>(*this, AcDb::kForWrite);

    FrCheckStatus(circle->setRadius(radius), L"Не удалось установить радиус");
}

void FrCircle::setCenter(const AcGePoint3d& center) {
    auto circle = open<>(*this, AcDb::kForWrite);

    FrCheckStatus(circle->setCenter(center), L"Не удалось установить центр");
}

double FrCircle::radius() const {
    auto circle = open<>(*this, AcDb::kForRead);

    return circle->radius();
}

AcGePoint3d FrCircle::center() const {
    auto circle = open<>(*this, AcDb::kForRead);

    return circle->center();
}
