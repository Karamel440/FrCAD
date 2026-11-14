#pragma once
#include <FrEntity.h>
#include <dbobjptr.h>

#include <optional>

template <typename T>
class Open {
    std::optional<AcDbObjectPointer<T>> opened_;
    T* ptr_ = nullptr;

  public:
    Open(const FrEntity& ent, AcDb::OpenMode mode, std::wstring message) {
        if (ent.ent_) {
            ptr_ = T::cast(ent.ent_.get());
        } else {
            opened_.emplace(ent.id_, mode);
            FrCheckStatus(opened_->openStatus(), message);
            ptr_ = opened_->object();
        }
        if (!ptr_) throw FrException{L"Пустой объект или несовместимый тип"};
    }

    Open(const Open&) = delete;
    Open& operator=(const Open&) = delete;

    T* operator->() const noexcept { return ptr_; }
};

template <typename E>
auto open(const E& ent, AcDb::OpenMode mode) {
    return Open<typename E::NativeType>(ent, mode,
                                        L"Не удалось открыть объект");
}