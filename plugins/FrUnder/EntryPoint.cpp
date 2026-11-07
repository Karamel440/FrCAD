#include <aced.h>
#include <rxregsvc.h>
#include "Under.h"
#include <FrCommands.h>


extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt) {
    switch (msg) {
    case AcRx::kInitAppMsg: {
        acrxDynamicLinker->unlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);
        
        acedRegCmds->addCommand(
            L"FRUNDER",
            L"FrUnderRainSewer",
            L"FrUnderRainSewer",
            ACRX_CMD_MODAL,
            &FrCommand<FrUnderRainSewer>
            );

        acedRegCmds->addCommand(
            L"FRUNDER",
            L"FrUnderUnderWater",
            L"FrUnderUnderWater",
            ACRX_CMD_MODAL,
            &FrCommand<FrUnderUnderWater>
        );

        acutPrintf(L"\nFrUnder loaded");
        break;
    }
    case AcRx::kUnloadAppMsg: {
        acedRegCmds->removeGroup(
            L"FRUNDER"
        );
        break;
    }
    }
    return AcRx::kRetOK;
}