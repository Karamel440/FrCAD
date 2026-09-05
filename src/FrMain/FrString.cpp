#include "FrString.h"
#include <Windows.h>

std::wstring toWstring(const std::string& string)
{
	int size = MultiByteToWideChar(
		CP_UTF8,
		0,
		string.data(),
		static_cast<int>(string.size()),
		nullptr,
		0
	);

	std::wstring result(size, L'\0');

	MultiByteToWideChar(
		CP_UTF8,
		0,
		string.data(),
		static_cast<int>(string.size()),
		result.data(),
		size
	);

	return result;
}

std::string toString(std::wstring_view str) {
    if (str.empty())
        return {};

    const int size = WideCharToMultiByte(
        CP_UTF8,
        0,
        str.data(),
        static_cast<int>(str.size()),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    std::string result(size, '\0');

    WideCharToMultiByte(
        CP_UTF8,
        0,
        str.data(),
        static_cast<int>(str.size()),
        result.data(),
        size,
        nullptr,
        nullptr
    );

    return result;
}