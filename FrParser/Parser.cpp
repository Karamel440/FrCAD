#include <Windows.h>
#include <string>
#include <vector>

#include "Parser.h"

std::wstring Parser::parse(std::optional<ZoneUpdate> update) {
    output = raw;
    normalizeRawInput(output);

    auto tokens = tokenaize();
    auto zones = createZones(tokens);

    if (update) {
        addZone(zones, update->found, update->year);
    }

    normalizeZones(zones);

    output = createString(zones);
    removeDoubleSigns(output);

    return output;
}


void replaceAll(std::wstring& str, std::wstring raw, std::wstring normal) {

	auto size = raw.size();
	auto pos = str.find(raw);

	while (pos != std::wstring::npos) {

		str.replace(pos, size, normal);

		pos = str.find(raw, pos + normal.size());
	}
}

void toLower(std::wstring& str) {
	CharLowerBuffW(str.data(), static_cast<DWORD>(str.size()));
}
