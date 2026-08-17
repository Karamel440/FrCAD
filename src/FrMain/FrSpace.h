#pragma once
#include <aced.h>

#include <FrObject.h>
#include <FrEntity.h>

class FrSpace : public FrObject {
	friend class FrRefDatabase;

	FrSpace(AcDbObjectId id)
		: FrObject{ id } {}

public:
	using NativeType = AcDbBlockTableRecord;

	void append(FrEntity& ent);

	std::wstring name() const;
	AcDbObjectId id() const;
};
