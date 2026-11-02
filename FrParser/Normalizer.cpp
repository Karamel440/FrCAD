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

	for (const auto& var : notFind) {
		replaceAll(raw, var, L"н.о.");
	}

	for (const auto& var : found) {
		replaceAll(raw, var, L"к.к.");
	}

	removeDoubleSigns(raw);
}

void Parser::normalizeZones(std::vector<Zone>& zones) {

    std::vector<Zone> result{}; 

	std::vector<int> KKyears{};
    std::vector<int> NOyears{};
    Zone temp{};

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
            for (auto& token : zones[index].tokens) {
                temp.tag = ZoneTag::Freezone;
                temp.tokens.push_back(token);
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

    if (!temp.tokens.empty()) {
        result.push_back(temp);
    }

    //3.Расставить запятые между зонами
    size_t pos{};

    while (pos + 1 < result.size()) {
        result[pos].tokens.push_back({TokenFlag::Separator, L", "});
        pos++;
    }
    zones = result;

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
