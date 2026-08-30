#pragma once
#include <FrEntity.h>
#include "FrSpace.h"

class FrBlockReference : public FrEntity {
	
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

	void setPosition(AcGePoint3d point);
	void setRotation(double degree);

	AcGePoint3d position() const;
	FrSpace space() const;

};
