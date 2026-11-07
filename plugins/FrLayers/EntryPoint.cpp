#include <aced.h>
#include <rxregsvc.h>
#include <FrCommands.h>
#include "Layers.h"

extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt) {
    switch (msg) {
    case AcRx::kInitAppMsg: {
        acrxDynamicLinker->unlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);

        acedRegCmds->addCommand(
            L"FRLAYERS_COMMANDS",
            L"FRLAYERSONALL",
            L"FrLayersOnAll",
            ACRX_CMD_MODAL,
            &FrCommand<FrLayersOnAll>
        );

        acedRegCmds->addCommand(
            L"FRLAYERS_COMMANDS",
            L"FRLAYERSISOLATE",
            L"FrLayersIsolate",
            ACRX_CMD_MODAL,
            &FrCommand<FrLayersIsolate>
        );

        acedRegCmds->addCommand(
            L"FRLAYERS_COMMANDS",
            L"FRLAYERSOFFSELECTED",
            L"FrLayersOffSelected",
            ACRX_CMD_MODAL,
               &FrCommand<FrLayersOffSelected>);

        acedRegCmds->addCommand(
            L"FRLAYERS_COMMANDS",
            L"FrLayersOffUnder",
            L"FrLayersOffUnder",
            ACRX_CMD_MODAL,
            &FrCommand<FrLayersOffUnder>
        );

        acedRegCmds->addCommand(
            L"FRLAYERS_COMMANDS",
            L"FrLayersOnCabels",
            L"FrLayersOnCabels",
            ACRX_CMD_MODAL,
            &FrCommand<FrLayersOnCabels>
        );

        acedRegCmds->addCommand(
            L"FRLAYERS_COMMANDS",
            L"FrLayersOnWells",
            L"FrLayersOnWells",
            ACRX_CMD_MODAL,
            &FrCommand<FrLayersOnWells>
        );

        acutPrintf(L"\nPlugin FrLayers loaded");
        break;
    }
    
    case AcRx::kUnloadAppMsg: {
        // ”дал€й группы команд
        acedRegCmds->removeGroup(L"FRLAYERS_COMMANDS");
        break;
    }
    }
    return AcRx::kRetOK;
}