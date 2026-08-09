#pragma once
#include <source_location>
#include <string>
#include <aced.h>
#include <optional>

struct FrCancel
{

};

class FrException
{
    std::source_location location_;
    std::wstring message_;
    std::optional<Acad::ErrorStatus> status_;

public:
    FrException(
        const std::wstring& message,
        Acad::ErrorStatus status,
        std::source_location location = std::source_location::current())
        :
        location_(location),
        message_(std::move(message)),
        status_(status)
    {
    }

    FrException(
        std::wstring message,
        std::source_location location = std::source_location::current())
        :
        location_(location),
        message_(std::move(message))
    {
    }

    std::source_location where() const noexcept
    {
        return location_;
    }

    const std::wstring& message() const noexcept
    {
        return message_;
    }

    std::optional<Acad::ErrorStatus> status() const noexcept
    {
        return status_;
    }
};

inline void FrCheckStatus(
    Acad::ErrorStatus es,
    std::wstring_view message,
    std::source_location location = std::source_location::current())
{
    if (es != Acad::eOk) {
        throw FrException{
            std::wstring{message},
            es,
            location
        };
    }
}