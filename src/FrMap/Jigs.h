#pragma once
#include <dbjig.h>
#include <dbpl.h>
#include <FrPolyline.h>

class FrPolylineJig: public AcEdJig {
    AcGePoint3d currentPt_;
    AcGePoint3d basePt_;
    AcDbPolyline* line_;

    public:
    FrPolylineJig(FrPolyline& line);

    DragStatus sampler() override;
    Adesk::Boolean update() override;
    AcDbEntity* entity() const override;

    void addNextVertex();
    void removeLastVertex();
};