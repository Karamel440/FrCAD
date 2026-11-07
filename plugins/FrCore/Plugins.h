#pragma once
#include <cad.h>

std::wstring getCorePath();
bool loadFrPlugins(const std::wstring& pathToFrCore);
bool isFrPlugin(const std::filesystem::path& path);
void unloadFrPlugins();
