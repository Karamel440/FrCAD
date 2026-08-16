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

