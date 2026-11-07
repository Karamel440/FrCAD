#include <aced.h>
#include <rxregsvc.h>
#include <Windows.h>
#include <FrCommands.h>

#include "Plugins.h"

extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt) {
    switch (msg) {
        case AcRx::kInitAppMsg: {
            acrxDynamicLinker->unlockApplication(pkt);
            acrxRegisterAppMDIAware(pkt);

            acutPrintf(L"\nFrCore loaded");

            try {
                const std::wstring corePath = getCorePath();

                SetDllDirectoryW(std::filesystem::path{ corePath }.parent_path().c_str());

                if (!loadFrPlugins(corePath)) {
                    FrPrint(L"Failed to load one or more Fr plugins");
                }
            } catch (const FrException& ex) {
                FrPrint(L"Failed to initialize plugin system: " +
                                  ex.message());
                SetDllDirectoryW(nullptr);
            } catch (...) {
                FrPrint(
                    L"Failed to initialize plugin system: unknown error");
                SetDllDirectoryW(nullptr);
            }
            SetDllDirectoryW(nullptr);

            break;
        }
        case AcRx::kUnloadAppMsg: {
            unloadFrPlugins();

            acedRegCmds->removeGroup(L"COREGROUP");

            FrPrint(L"\nFrCore unloaded");
            break;
        }
    }

    return AcRx::kRetOK;
}