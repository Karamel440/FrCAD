#pragma once
#include <optional>
#include <aced.h>
#include <dbents.h>
#include <FrExceptions.h>

class FrObject {
protected:
	AcDbObjectId id_ = AcDbObjectId::kNull;

public:
	using NativeType = AcDbObject;

	virtual ~FrObject() = default;

	FrObject() = default;

	explicit FrObject(AcDbObjectId id)
		: id_(id) {
		
	}

	template<typename T>
		requires std::derived_from<T, FrObject>
	bool is() const {
		if (id_ != AcDbObjectId::kNull) {
			AcRxClass* objectClass = id_.objectClass();

			return objectClass &&
				objectClass->isDerivedFrom(T::NativeType::desc());
		}

		return dynamic_cast<const T*>(this) != nullptr;
	}

	template<typename T>
		requires std::derived_from<T, FrObject>
	T as() const {
		if (id_.isNull() || !is<T>())
			throw FrException{
				L"Невозможно преобразовать объект к текущему типу"
		};

		return T{ id_ };
	}
};

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
