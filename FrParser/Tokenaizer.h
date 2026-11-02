#pragma once
#include <string>
#include <vector>

enum class TokenFlag { 
	KK, 
	NO, 
	Separator, 
	Number, 
	FreeText, 
	YearMarker 
};

struct Token {
	
	TokenFlag flag{};
	std::wstring value{};
};