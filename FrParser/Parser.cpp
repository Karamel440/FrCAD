#include <Windows.h>
#include <string>
#include "Parser.h"
#include <vector>

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

std::wstring Parser::parse() {
	output = raw;
	normalizeRawInput(output);
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
