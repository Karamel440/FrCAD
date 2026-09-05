#pragma once
#include <string>

std::wstring toWstring(const std::string& string);
std::string toString(std::wstring_view str);
