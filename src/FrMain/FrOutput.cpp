#include "FrOutput.h"
#include <acestext.h>
#include <Windows.h>

void FrPrint(const std::wstring& message) {
	acutPrintf(L"\n%s", message.c_str());
}

void FrPrint(const char* message) {
    if (!message) return;

    // source_location and exception messages contain narrow strings.
    UINT codePage = CP_UTF8;
    int size = MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, message,
                                   -1, nullptr, 0);
    if (size == 0) {
        codePage = CP_ACP;
        size = MultiByteToWideChar(codePage, 0, message, -1, nullptr, 0);
    }
    if (size == 0) return;

    std::wstring wide(size, L'\0');
    if (MultiByteToWideChar(codePage, 0, message, -1, wide.data(), size) == 0)
        return;
    wide.pop_back();
    FrPrint(wide);
}

void FrAlert(const std::wstring& message) {
	acedAlert(message.c_str());
}

void FrPrint(const Acad::ErrorStatus es) {
	acutPrintf(L"\n%s", acadErrorStatusText(es));
}
