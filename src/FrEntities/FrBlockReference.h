#pragma once
#include <FrEntity.h>
#include "FrSpace.h"

class FrBlockReference : public FrEntity {
	
	friend class FrObject;

	FrBlockReference(AcDbObjectId id)
		:FrEntity(id) { }
public:
	
	using NativeType = AcDbBlockReference;

	FrBlockReference(FrSpace& space)
		: FrEntity(
			std::make_unique<AcDbBlockReference>(
				AcGePoint3d::kOrigin,
				space.id()
			)
		) { }

	FrBlockReference(FrSpace& space, AcGePoint3d position)
		: FrEntity(
			std::make_unique<AcDbBlockReference>(
				position,
				space.id()
			)
		) 
	{ }

	void setPosition(AcGePoint3d point);
	void setRotation(double degree);
	void setElevation(double units);

	bool isReference() const;

	AcGePoint3d position() const;
	FrSpace space() const;
	double elevation() const;
};
