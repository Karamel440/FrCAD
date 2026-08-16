#pragma once
#include <concepts>

#include <aced.h>

#include "FrExceptions.h"

class FrObject {
protected:
	AcDbObjectId id_ = AcDbObjectId::kNull;

public:
	using NativeType = AcDbObject;

	virtual ~FrObject() = default;

	FrObject() = default;

	explicit FrObject(AcDbObjectId id)
		: id_(id) {}

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
