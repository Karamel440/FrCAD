#pragma once
#include <aced.h>

void FrPrint(const std::wstring& message);
void FrPrint(const char* message);
void FrPrint(const Acad::ErrorStatus es);
void FrAlert(const std::wstring& message);
