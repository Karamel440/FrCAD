#include "Parser.h"
#include "Zoner.h"

#include <vector>
#include <string>
#include <algorithm>
#include <format>

const std::vector<std::wstring> notFind {
    L"не обнаружено. ",
    L"не обнаружено ",
    L"не обнаружен. ",
    L"не обнаружен ",

    L"необнаружено. ",
    L"необнаружено ",
    L"необнаружен. ",
    L"необнаружен ",

    L"не обн. ",
    L"не обн ",

    L"необн. ",
    L"необн ",

    L"необ. ",
    L"необ ",

    L"не обнаружено.",
    L"не обнаружено",
    L"не обнаружен.",
    L"не обнаружен",

    L"необнаружено.",
    L"необнаружено",
    L"необнаружен.",
    L"необнаружен",

    L"не обн.",
    L"не обн",

    L"необн.",
    L"необн",

    L"необ.",
    L"необ"
};

const std::vector<std::wstring> found {
    L"к. к. ",
    L"к .к. ",
    L"к . к. ",

    L"кк. ",
    L"кк ",
    L"к.к. ",
    L"к.к ",

    L"к. к.",
    L"к .к.",
    L"к . к.",

    L"кк.",
    L"кк",

    L"к.к"
};

void Parser::removeDoubleSigns(std::wstring& str) {

	while (str.find(L"..") != std::wstring::npos) {
		replaceAll(str, L"..", L".");
	}

	while (str.find(L"  ") != std::wstring::npos) {
		replaceAll(str, L"  ", L" ");
	}

	while (str.find(L",,") != std::wstring::npos) {
		replaceAll(str, L",,", L",");
	}
}

void Parser::normalizeRawInput(std::wstring& raw) {

	toLower(raw);
    removeDoubleSigns(raw);

	for (const auto& var : notFind) {
		replaceAll(raw, var, L"н.о.");
	}

	for (const auto& var : found) {
		replaceAll(raw, var, L"к.к.");
	}

    for (int i{}; i < raw.size(); i++) {
        if (raw[i] == L'\u00A0')
            replaceAll(raw, L"\u00A0", L" ");

        else if (raw[i] == L'\r')
            replaceAll(raw, L"\r", L" ");

        else if (raw[i] == L'\n')
            replaceAll(raw, L"\n", L" ");

        else if (raw[i] == L'\t')
            replaceAll(raw, L"\t", L" ");
    }

    removeDoubleSigns(raw);
}

void Parser::normalizeZones(std::vector<Zone>& zones) {

    std::vector<Zone> result{}; 

	std::vector<int> KKyears{};
    std::vector<int> NOyears{};
    std::vector<Zone> freeZones{};


	//1.Собрать информацию из всех зон
	for (size_t index{}; index < zones.size();index++) {
		if (zones[index].tag == ZoneTag::KKzone) {

            for (auto& token : zones[index].tokens) {
                
				if (token.flag != TokenFlag::Number)
                    continue;

				if (token.value.size() != 2 && token.value.size() != 4)
                    continue;

				if (token.value.size() == 2 && std ::stoi(token.value) <= 45) {
                    KKyears.push_back(std::stoi(token.value) + 2000);
                }
                else if (token.value.size() == 2 && std::stoi(token.value) > 45) {
                    KKyears.push_back(std::stoi(token.value) + 1900);
                }
                else {
                    KKyears.push_back(std::stoi(token.value));
                }
            }
        }

		if (zones[index].tag == ZoneTag::NOzone) {

            for (auto& token : zones[index].tokens) {

                if (token.flag != TokenFlag::Number)
                    continue;
				
				if (token.value.size() != 2 && token.value.size() != 4)
                    continue;

                if (token.value.size() == 2 && std::stoi(token.value) <= 45) {
                    NOyears.push_back(std::stoi(token.value) + 2000);
                }
                else if (token.value.size() == 2 && std::stoi(token.value) > 45) {
                    NOyears.push_back(std::stoi(token.value) + 1900);
                }
                else {
                    NOyears.push_back(std::stoi(token.value));
                }
            }
        }

		if (zones[index].tag == ZoneTag::Freezone) {
            Zone freeZone = zones[index];
            
            trimZone(freeZone);

            if (!freeZone.tokens.empty()) {
            freeZones.push_back(std::move(freeZone));
            }
        }


	}

    //2.Заново собрать все зоны по порядку
    if (!KKyears.empty()) {
        std::sort(KKyears.begin(), KKyears.end());

        std::wstring year = std::format(L"{:02}", KKyears.back() % 100);

        Zone kk{};
        kk.tag = ZoneTag::KKzone;
        kk.tokens.push_back({TokenFlag::KK, L"к.к."});
        kk.tokens.push_back({TokenFlag::Number, year});
        result.push_back(kk);
    }

    if (!NOyears.empty()) {
        std::sort(NOyears.begin(), NOyears.end());
        if (NOyears.front() != NOyears.back()) {
            std::wstring years =
                std::format(L"{:02}-{:02}", NOyears.front() % 100, NOyears.back() % 100);

            Zone no{};
            no.tag = ZoneTag::NOzone;
            no.tokens.push_back({TokenFlag::NO, L"н.о."});
            no.tokens.push_back({TokenFlag::Number, years});
            result.push_back(no);
        }

        else {
            std::wstring year =
                std::format(L"{:02}", NOyears.front() % 100);

           Zone no{};
            no.tag = ZoneTag::NOzone;
            no.tokens.push_back({TokenFlag::NO, L"н.о."});
            no.tokens.push_back({TokenFlag::Number, year});
            result.push_back(no);
        }
    }

    if (!freeZones.empty()) {
        for (const auto& zone : freeZones) {
            result.push_back(std::move(zone));
        }
    }

    //3.Расставить запятые между зонами
    size_t pos{};

    while (pos + 1 < result.size()) {
        result[pos].tokens.push_back({TokenFlag::Separator, L", "});
        pos++;
    }
    zones = result;

}

void Parser::trimZone(Zone& zone) {
    
    //1. Пройтись по началу зоны
    size_t i{};

    while (i < zone.tokens.size() &&
           (zone.tokens[i].flag == TokenFlag::Separator || zone.tokens[i].value == L" ")) {

        ++i;
    }

    zone.tokens.erase(zone.tokens.begin(), zone.tokens.begin() + i);

    i = 0;

    auto size = zone.tokens.size();

    if (size == 0)
        return;
    //2. пройтись по концу зоны
    while (i < zone.tokens.size() && 
        (zone.tokens[size - 1 - i].flag == TokenFlag::Separator || zone.tokens[size - 1 - i].value == L" ")) {

        i++;
    }

    zone.tokens.erase(zone.tokens.end() - i, zone.tokens.end());
}

std::wstring Parser::createString(std::vector<Zone>& zones) {

	std::wstring result;

	for (const auto& zone : zones) {
        for (auto token : zone.tokens) {
            result += token.value;
        }
    }

	return result;
}
