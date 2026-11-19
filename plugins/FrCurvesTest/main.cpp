#include "main.h"

#include <FrOutput.h>
#include <FrPolyline.h>
#include <FrSelection.h>
#include <FrText.h>

void FrTestSegments() {
    FrPolyline line;

    AcGePoint3d pt1{0, 0, 0};
    AcGePoint3d pt2{10, 10, 0};

    line.addVertex(pt1);
    line.addVertex(pt2);

    double par1 = 0.25;
    double par2 = 0.75;

    auto db = FrDbCurrent();

    std::vector<double> params{par1, par2};

    db.append(line);

    auto curves = line.split(params);


    for (auto& curve : curves) {
        db.append(curve);
    }
}