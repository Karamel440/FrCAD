#pragma once
#include <aced.h>

class FrBounds {
	
	AcGePoint3d min_;
	AcGePoint3d max_;
	
public:
	FrBounds(
		AcGePoint3d min,
		AcGePoint3d max
	) : min_{min},max_(max) { }

	FrBounds(AcDbExtents extents)
		:min_{extents.minPoint()}, max_{extents.maxPoint()} 
	{ }

	AcGePoint3d closestPoint(const AcGePoint3d& point) const;
};