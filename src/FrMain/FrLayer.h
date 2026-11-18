#pragma once
#include <aced.h>
#include <string>

class FrLayer {
	AcDbObjectId id_;
	explicit FrLayer(AcDbObjectId id) : id_(id) {}

	friend class FrRefDatabase;
	friend class FrEntity;

public:
	std::wstring name() const;
	bool isReference() const;
	void off() const;
	void on() const;
	void freeze() const;
	void unfreeze() const;
	void erase() const;
};