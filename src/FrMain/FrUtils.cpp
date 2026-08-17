#include "FrUtils.h"
#include <Windows.h>
#include <filesystem>

#include <arxHeaders.h>

#include "FrExceptions.h"

namespace fs = std::filesystem;

fs::path currentPluginPath()
{
    auto func = &currentPluginPath;
    auto funcAddres = reinterpret_cast<LPCWSTR>(func);

    HMODULE module = nullptr;

    BOOL result = GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        funcAddres,
        &module
    );

    if (!result)
        throw FrException{
        L"Не удалось получить имя модуля"
    };

    std::wstring name;
    name.resize(1024);

    auto size = GetModuleFileNameW(
        module,
        name.data(),
        static_cast<DWORD>(name.size())
    );

    if (size == 0 ||
        size >= name.size())
        throw FrException{
        L"Ошибка пути"
    };

    name.resize(size);

    return fs::path(name);
}
