#pragma once
#include <FrObject.h>

#include <aced.h>
#include <dbsymtb.h>

class FrStyle : public FrObject {

	FrStyle(AcDbObjectId id)
		:FrObject(id) {
	}

	friend class FrRefDatabase;

public:

	using NativeType = AcDbTextStyleTableRecord;
};