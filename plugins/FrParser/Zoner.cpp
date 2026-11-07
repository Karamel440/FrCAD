#include "Parser.h"
#include "Zoner.h"

namespace {
Zone createKKZone(const std::vector<Token>& tokens, size_t& index) {
    Zone result;

    if (index >= tokens.size()) {
        return result;
    }

    result.tag = ZoneTag::KKzone;
    for (; index < tokens.size(); index++) {

        if (tokens[index].flag == TokenFlag::KK && result.tokens.empty()) {
            result.tokens.push_back(tokens[index]);
        }

        else if (!result.tokens.empty() && result.tokens.front().flag == TokenFlag::KK &&
                 tokens[index].flag == TokenFlag::Number &&
                 (tokens[index].value.size() == 2 || tokens[index].value.size() == 4)) {
            result.tokens.push_back(tokens[index]);
        }

        else if (tokens[index].flag == TokenFlag::NO) {
            break;
        }

        else if (tokens[index].flag == TokenFlag::FreeText && tokens[index].value != L" ") {
            break;
        }

        else if (tokens[index].flag == TokenFlag::FreeText) {
            result.tokens.push_back(tokens[index]);
        }

        else if (tokens[index].flag == TokenFlag::Separator && tokens[index].value == L";") {
            break;
        }

        else if (tokens[index].flag == TokenFlag::Separator) {
            result.tokens.push_back(tokens[index]);
        }

        else if (tokens[index].flag == TokenFlag::Number &&
                 (tokens[index].value.size() != 2 && tokens[index].value.size() != 4)) {

            break;
        }
    }
    return result;
}

Zone createNOZone(const std::vector<Token>& tokens, size_t& index) {
    Zone result;
    result.tag = ZoneTag::NOzone;

    if (index >= tokens.size()) {
        return result;
    }

    for (; index < tokens.size(); index++) {

        if (tokens[index].flag == TokenFlag::NO && result.tokens.empty()) {
            result.tokens.push_back(tokens[index]);
        }

        else if (tokens[index].flag == TokenFlag::NO) {
            continue;
        }

        else if (tokens[index].flag == TokenFlag::Separator && tokens[index].value != L";") {
            result.tokens.push_back(tokens[index]);
        }

        else if (tokens[index].flag == TokenFlag::KK ||
                 (tokens[index].flag == TokenFlag::Separator && tokens[index].value == L";") ||
                 (tokens[index].flag == TokenFlag::FreeText && tokens[index].value != L" ")) {
            break;
        }

        else if (tokens[index].flag == TokenFlag::Number &&
                 result.tokens.front().flag == TokenFlag::NO &&
                 (tokens[index].value.size() == 2 || tokens[index].value.size() == 4)) {

            result.tokens.push_back(tokens[index]);
        }

        else if (tokens[index].flag == TokenFlag::Number &&
                 (tokens[index].value.size() != 2 && tokens[index].value.size() != 4)) {

            break;
        }
    }
    return result;
}

Zone createFreeZone(const std::vector<Token>& tokens, size_t& index) {

    Zone result;
    result.tag = ZoneTag::Freezone;

    if (index >= tokens.size()) {
        return result;
    }

    while (
        index < tokens.size() && 
        tokens[index].flag != TokenFlag::KK &&
        tokens[index].flag != TokenFlag::NO && 
        tokens[index].flag != TokenFlag::YearMarker) {

        result.tokens.push_back(tokens[index]);
        ++index;
    }
    return result;
}
} // namespace

std::vector<Zone> Parser::createZones(const std::vector<Token>& tokens) {

	std::vector<Zone> result;
	size_t pos{};
	
	while (pos < tokens.size()) {

		if (tokens[pos].flag == TokenFlag::KK) {
			auto zone = createKKZone(tokens, pos);
			result.push_back(zone);
		}

		else if (tokens[pos].flag == TokenFlag::NO) {
			auto zone = createNOZone(tokens, pos);
			result.push_back(zone);
		}

		else if (tokens[pos].flag == TokenFlag::YearMarker) {
			++pos;
			continue;
		}

		else {
			auto zone = createFreeZone(tokens, pos);
			result.push_back(zone);
		}
	}
	return result;
}

void Parser::addZone(std::vector<Zone>& zones, bool status, int year) {
   
    if ( status ) {
        
        std::erase_if(zones, [](const Zone& zone) {
            return zone.tag == ZoneTag::NOzone;
        });

        bool hasKK = false;

        for (auto& zone : zones) {
            if (zone.tag == ZoneTag::KKzone) {
                Token token{TokenFlag::Number, std::to_wstring(year)};
                zone.tokens.push_back(token);
                hasKK = true;
                break;
            }
        }
        if (!hasKK) {
            Zone KK;
            KK.tag = ZoneTag::KKzone;

            Token kk{TokenFlag::KK, L"ê.ê."};
            Token num{TokenFlag::Number, std::to_wstring(year)};

            KK.tokens.push_back(kk);
            KK.tokens.push_back(num);
            zones.push_back(KK);
        }
    }

    else {
        bool hasNO = false;

        for (auto& zone : zones) {
            if (zone.tag == ZoneTag::NOzone) {
                Token token{TokenFlag::Number, std::to_wstring(year)};
                zone.tokens.push_back(token);
                hasNO = true;
                break;
            }
        }

        if (!hasNO) {
            Zone NO;
            NO.tag = ZoneTag::NOzone;

            Token no{TokenFlag::NO, L"í.î."};
            Token num{TokenFlag::Number, std::to_wstring(year)};

            NO.tokens.push_back(no);
            NO.tokens.push_back(num);
            zones.push_back(NO);
        }
    }
}

