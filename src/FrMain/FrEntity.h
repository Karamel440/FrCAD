#pragma once
#include <optional>
#include <aced.h>
#include <dbents.h>


class FrLayer {
	AcDbObjectId id_;
	explicit FrLayer(AcDbObjectId id) : id_(id) {}

	friend class FrRefDatabase;
	friend class FrEntity;

public:
	bool isReference() const;
	std::wstring name() const;
	void off() const;
	void on() const;
	void freeze() const;
	void unfreeze() const;
	void erase() const;
};

class FrEntity {
	AcDbObjectId id_  = AcDbObjectId::kNull;
	std::unique_ptr<AcDbEntity> ent_;

	friend class FrRefDatabase;

protected:
	virtual ~FrEntity();
	explicit FrEntity(std::unique_ptr<AcDbEntity> entity) : ent_(std::move(entity)) {}
	explicit FrEntity(AcDbObjectId id) : id_(id) {}

	AcDbEntity* entity() noexcept {
		return ent_.get();
	}

	AcDbObjectId id() const noexcept {
		return id_;
	}

public:

	void setLayer(const FrLayer& layer);
	void setColor(int index);
	void setLineWeight(AcDb::LineWeight weight);

	FrLayer layer() const;
	int color() const;
	AcDb::LineWeight lineWeight() const;
};

