#pragma once
#include <string>
#include <optional>

#include "Tokenaizer.h"
#include "Zoner.h"

struct ZoneUpdate {
    bool found;
    int year;
};

std::wstring parse(std::optional<ZoneUpdate> update = std::nullopt);


void replaceAll(std::wstring& str, std::wstring raw, std::wstring normal);
void toLower(std::wstring& str);

class Parser {

	std::wstring raw{};
	std::wstring output{};

	void removeDoubleSigns(std::wstring & str);
    void normalizeRawInput(std::wstring& raw);
    void normalizeZones(std::vector<Zone>& zones);

	std::vector<Token> tokenaize();
    void addZone(std::vector<Zone>& zones, bool status, int year);
	std::vector<Zone> createZones(const std::vector<Token>& tokens);
	std::wstring createString(std::vector<Zone>& zones);

public:
	Parser(const std::wstring input)
		:raw{ input } {
	}

	Parser(const wchar_t* input)
		:raw{ std::wstring{input} } {
	}


	Parser(const std::wstring input,bool isFound,int year) : raw(input) {
		
	}

	std::wstring parse(std::optional<ZoneUpdate> update = std::nullopt);
};