#include "Plugins.h"

namespace fs = std::filesystem;

std::vector<std::filesystem::path> loadedFrPlugins;

std::wstring getCorePath()  {

	HMODULE module = nullptr;
	wchar_t buffer[1024];

	BOOL success = GetModuleHandleExW(
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		L"FrCore.arx",
		&module
	);

	if (!success)
		throw cad::core::Exception(L"FrCore.arx not found");

	DWORD length = GetModuleFileNameW(
		module,
		buffer,
		1024
	);

	return std::wstring{ buffer, length };

}

bool isFrPlugin(const std::filesystem::path& path) {
	auto fileName = path.filename().wstring();
	if (fileName.starts_with(L"Fr") &&
		path.extension() == ".arx" &&
		fileName != L"FrCore.arx")
		return true;
	return false;
}

bool loadFrPlugins(const std::wstring& pathToFrCore) {
	const fs::path coreDirectory =
		fs::path{ pathToFrCore }.parent_path();

	bool allLoaded = true;

	for (const fs::directory_entry& entry :
		fs::directory_iterator{ coreDirectory }) {

		if (!entry.is_regular_file())
			continue;

		const fs::path& pluginPath = entry.path();

		if (!isFrPlugin(pluginPath))
			continue;

		const bool loaded = acrxDynamicLinker->loadModule(
			pluginPath.c_str(),
			Adesk::kTrue,
			Adesk::kFalse
		);

		if (loaded)
			loadedFrPlugins.push_back(pluginPath);
		else
			allLoaded = false;
	}

	return allLoaded;
}

void unloadFrPlugins() {
	for (auto it = loadedFrPlugins.rbegin();
		it != loadedFrPlugins.rend();
		++it) {

		const bool unloaded = acrxDynamicLinker->unloadModule(
			it->c_str(),
			Adesk::kFalse
		);

		if (!unloaded) {
			const std::wstring fileName = it->filename().wstring();

			acutPrintf(
				L"\nFailed to unload plugin: %ls",
				fileName.c_str()
			);
		}
	}

	loadedFrPlugins.clear();
}
