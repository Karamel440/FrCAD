#pragma once
#include <string>

void replaceAll(std::wstring& str, std::wstring raw, std::wstring normal);
void toLower(std::wstring& str);

class Parser {

	std::wstring raw{};
	std::wstring output{};

	void removeDoubleSigns(std::wstring & str);

	void normalizeRawInput(std::wstring & raw);

public:
	Parser(const std::wstring input)
		:raw{ input } {
	}

	Parser(const wchar_t* input)
		:raw{ std::wstring{input} } {
	}

	std::wstring parse();
};