#pragma once
#include <optional>
#include <aced.h>
#include <dbents.h>
#include <FrExceptions.h>

class FrEntity : public FrObject {
	
	std::unique_ptr<AcDbEntity> ent_;

	friend class FrRefDatabase;
	friend class FrObject;

protected:
	
	explicit FrEntity(std::unique_ptr<AcDbEntity> entity) : ent_(std::move(entity)) {}
	explicit FrEntity(AcDbObjectId id) : FrObject(id) {}

	AcDbEntity* entity() noexcept {
		return ent_.get();
	}

	AcDbObjectId id() const noexcept {
		return id_;
	}

public:
	virtual ~FrEntity();

	using NativeType = AcDbEntity;

	FrEntity(const FrEntity&) = delete;
	FrEntity& operator=(const FrEntity&) = delete;

	FrEntity(FrEntity&&) noexcept = default;
	FrEntity& operator=(FrEntity&&) noexcept = default;

	void setLayer(const FrLayer& layer);
	void setColor(int index);
	void setLineWeight(AcDb::LineWeight weight);
	void setLinetype(const std::wstring& name);

	FrLayer layer() const;
	int color() const;
	AcDb::LineWeight lineWeight() const;
	std::wstring& Linetype() const;
};
