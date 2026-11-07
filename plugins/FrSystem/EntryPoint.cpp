#include <aced.h>
#include <rxregsvc.h>
#include <FrCommands.h>
#include "system.h"

extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt) {
    switch (msg) {
    case AcRx::kInitAppMsg: {
        acrxDynamicLinker->unlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);

        acedRegCmds->addCommand(
            L"FRSYSTEMGROUP",
            L"FrSystemClearAll",
            L"FrSystemClearAll",
            ACRX_CMD_MODAL,
            &FrCommand<FrSystemClearAll>
        );

        acedRegCmds->addCommand(
            L"FRSYSTEMGROUP",
            L"FrSystemSort",
            L"FrSystemSort",
            ACRX_CMD_MODAL,
            &FrCommand<FrSystemSort>
        );

        acedRegCmds->addCommand(
            L"FRSYSTEMGROUP",
            L"setFrStandardVars",
            L"setFrStandardVars",
            ACRX_CMD_MODAL,
            &FrCommand<setFrStandardVars>
        );

        acutPrintf(L"\nPlugin FrSystem loaded");
        break;
    }
    case AcRx::kUnloadAppMsg: {
        acedRegCmds->removeGroup(L"FRSYSTEMGROUP");
        break;
    }
    }
    return AcRx::kRetOK;
}