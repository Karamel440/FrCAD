#include "FrBounds.h"

#include <algorithm>

AcGePoint3d FrBounds::closestPoint(const AcGePoint3d& point) const
{
    return {
        std::clamp(point.x,min_.x,max_.x),
        std::clamp(point.y,min_.y,max_.y),
        point.z
    };
}
