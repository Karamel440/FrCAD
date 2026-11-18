#include <FrPolyline.h>
#include <FrSelection.h>
#include <FrOutput.h>
#include <FrText.h>
#include "main.h"

void FrTestSegments() {
    auto texts = FrSelect<FrText>();
    auto polylines = FrSelect<FrPolyline>();

    if (texts.empty() || polylines.empty()) return;

    auto& text = texts[0];
    auto& pl = polylines[0];

    auto pt = FrGetPoint(L"Выберите точку на полилинии:\n");

    auto seg = pl.closestSegment(pt);
    double angle = pl.segmentAngle(seg);

    if (angle > 90.0) {
        angle -= 180.0;
    } else if (angle < -90.0) {
        angle += 180.0;
    }

    text.setRotation(angle);

    auto pt2 = FrGetPoint(L"Выберите точку вставки текста:\n");

    text.setPosition(pt2);
}