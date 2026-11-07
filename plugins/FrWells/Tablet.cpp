#include "Tablet.h"

void Tablet::calculate(const AcGePoint3d& point) noexcept {
    const int index4000X = floor(point.x / 4000);
    const int index4000Y = floor(point.y / 4000);

    const int corner4000X = index4000X * 4000;
    const int corner4000Y = index4000Y * 4000;

    const int offset4000X = point.x - corner4000X;
    const int offset4000Y = point.y - corner4000Y;

    const int index1000X = offset4000X / 1000;
    const int index1000Y = offset4000Y / 1000;

    const int corner1000X = index1000X * 1000;
    const int corner1000Y = index1000Y * 1000;

    const int offset1000X = offset4000X - corner1000X;
    const int offset1000Y = offset4000Y - corner1000Y;

    const int index250X = offset1000X / 250;
    const int index250Y = offset1000Y / 250;

    const int number1000 = 13 + index1000X - index1000Y * 4;

    const int number250 = 13 + index250X - index250Y * 4;

    // —бор номенклатуры

    nomenclature = (index4000Y + 1) * 1'000'000 + (index4000X + 1) * 10'000 +
                   number1000 * 100 + number250;

    // јбсолютный левый нижний угол планшета 250

    LL.x = corner4000X + corner1000X + index250X * 250;

    LL.y = corner4000Y + corner1000Y + index250Y * 250;
}

std::optional<int> Tablet::getCell(const AcGePoint3d& point) const {
    if (!contains(point)) return std::nullopt;

    constexpr int cellSize = 50.0;

    const int indexX = (point.x - LL.x) / cellSize;
    const int indexY = (point.y - LL.y) / cellSize;

    return 21 + indexX - indexY * 5;
}

int Tablet::getNomenclature() const noexcept { return nomenclature; }

const AcGePoint3d Tablet::getCorner() const { return LL; }

bool Tablet::contains(const AcGePoint3d& point) const noexcept {
    const AcGePoint3d rightHigherCorner{LL.x + 250.0, LL.y + 250.0, 0};

    return point.x < rightHigherCorner.x && point.y < rightHigherCorner.y &&
           point.x >= LL.x && point.y >= LL.y;
}