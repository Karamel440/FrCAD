#include <cwctype>

#include "Parser.h"
#include "Tokenaizer.h"

namespace {

bool isKK(const std::wstring& raw, size_t pos) {
	return raw.compare(pos, 4, L"к.к.") == 0;
}

bool isNO(const std::wstring& raw, size_t pos) {
	return raw.compare(pos, 4, L"н.о.") == 0 && (pos < 3 || raw.compare(pos - 3, 3, L"тр.") != 0);
}

bool isNumber(const std::wstring& raw, size_t pos) {
	return raw[pos] >= L'0' && raw[pos] <= L'9';
}

bool isSign(const std::wstring& raw, size_t pos) {
    const auto ch = raw[pos];

    return 
		ch == L',' || 
		ch == L';' || 
		ch == L'-' || 
		ch == L'\u2013' ||
        ch == L'\u2014';                                             
}

bool isYearMarker(const std::wstring& raw, size_t pos) {
    return 
		raw.compare(pos, 1, L"г") == 0
	    && (pos == 0 || raw[pos - 1] == L' ' || std::iswdigit(raw[pos - 1])) &&
           (raw[pos + 1] == L',' || raw[pos + 1] == L'.' || std::iswspace(raw[pos + 1]) ||pos + 1 == raw.size());
}
} // namespace

std::vector<Token> Parser::tokenaize() {

	std::vector<Token> result;

	if (output.empty())
		return result;

	size_t pos{};

	while (pos < output.size()) {

		if (isKK(output, pos)) {
			Token token;
			token.flag = TokenFlag::KK;
			token.value = L"к.к.";
			pos += 4;
			result.push_back(token);
		}

		else if (isNO(output, pos)) {
			Token token;
			token.flag = TokenFlag::NO;
			token.value = L"н.о.";
			pos += 4;
			result.push_back(token);
		}

		else if (isNumber(output, pos)) {

			std::size_t start = pos;

			while (pos < output.size() && std::iswdigit(output[pos])) {
				++pos;
			}

			Token token;
			token.flag = TokenFlag::Number;
			token.value = output.substr(start, pos - start);

			result.push_back(token);
		}

		else if (isSign(output, pos)) {

			Token token;
			token.flag = TokenFlag::Separator;
			token.value = output[pos];
			result.push_back(token);
			pos++;
		}

		else if (isYearMarker(output, pos)) {

			Token token;
			token.flag = TokenFlag::YearMarker;
			token.value = L"г.";

			pos + 1 < output.size() && output[pos + 1] == L'.' ? pos += 2 : pos += 1;

			result.push_back(token);
		}

		else {

			Token token;
			token.flag = TokenFlag::FreeText;
			auto start = pos;

			while (
				pos < output.size() 
				&& !isKK(output, pos) 
				&& !isNO(output, pos) 
				&& !isNumber(output, pos) 
				&& !isSign(output, pos)
			       && !isYearMarker(output, pos)) {

				++pos;
			}

			token.value = output.substr(start, pos - start);
			result.push_back(token);
		}
	}
	return result;
}
