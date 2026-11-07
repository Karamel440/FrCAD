#include <aced.h>
#include <rxregsvc.h>
#include "blocks.h"
#include "Plugins.h"

extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt) {
	switch (msg) {
	case AcRx::kInitAppMsg: {
		 
		acrxDynamicLinker->unlockApplication(pkt);
		acrxRegisterAppMDIAware(pkt);

		acedRegCmds->addCommand(
			L"COREGROUP",
			L"FRBLOCKIMPORTALL",
			L"FRBLOCKIMPORTALL",
			ACRX_CMD_MODAL 
			/* | ACRX_CMD_SESSION*/,
			cmd_FrBlockImportAll);
		
		acutPrintf(L"\nFrCore loaded");

		try {
			const std::wstring corePath = getCorePath();

			SetDllDirectoryW(corePath.c_str());

			if (!loadFrPlugins(corePath)) {
				cad::out::println(L"Failed to load one or more Fr plugins");
			}
		}
		catch (const cad::core::Exception& ex) {
			cad::out::println(
				L"Failed to initialize plugin system: "
				+ std::wstring{ ex.where() }
			);
			SetDllDirectoryW(nullptr);
		}
		catch (...) {
			cad::out::println(
				L"Failed to initialize plugin system: unknown error"
			);
			SetDllDirectoryW(nullptr);
		}
		SetDllDirectoryW(nullptr);

		break;
	}
	case AcRx::kUnloadAppMsg: {
		unloadFrPlugins();

		acedRegCmds->removeGroup(L"COREGROUP");

		cad::out::println(L"\nFrCore unloaded");
		break;
	}
	}

	
	return AcRx::kRetOK;
}