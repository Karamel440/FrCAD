#pragma once
#include <aced.h>
#include <string>

void FrPrint(const std::wstring& message);
void FrPrint(const char* message);
void FrPrint(const Acad::ErrorStatus es);
void FrAlert(const std::wstring& message);
