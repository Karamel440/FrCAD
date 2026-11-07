#pragma once
#include <string>

__declspec(dllexport) std::wstring parse(std::wstring raw);

__declspec(dllexport) std::wstring parse(std::wstring raw, bool found, int year);