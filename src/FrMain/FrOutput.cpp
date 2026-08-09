#include "FrOutput.h"
#include <acestext.h>

void FrPrint(const std::wstring& message) {
	acutPrintf(L"\n%s", message.c_str());
}

void FrPrint(const char* message) {
	acutPrintf(L"\n%hs", message);
}

void FrAlert(const std::wstring& message) {
	acedAlert(message.c_str());
}

void FrPrint(const Acad::ErrorStatus es) {
	acutPrintf(L"\n%s", acadErrorStatusText(es));
}
