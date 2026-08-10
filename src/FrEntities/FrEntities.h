#pragma once
#include <aced.h>
#include <dbents.h>
#include <FrEntity.h>

class FrPolyline : public FrEntity {
	friend class FrRefDatabase;
	friend class FrObject;

	FrPolyline(AcDbObjectId id)
		:FrEntity{ id } {
	}
public:
	FrPolyline() noexcept
		:FrEntity(std::make_unique<AcDbPolyline>()) {
	}

	using NativeType = AcDbPolyline;

	void addVertex(const AcGePoint3d& point);
	void removeVertex(size_t index);
	void SetElevation(double elevation);

};

