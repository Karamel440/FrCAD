#include <FrCommands.h>
#include <aced.h>
#include <rxregsvc.h>

#include "Main.h"
#include "Nomenclature.h"
#include "Window.h"

extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt) {
    switch (msg) {
        case AcRx::kInitAppMsg: {
            acrxDynamicLinker->unlockApplication(pkt);
            acrxRegisterAppMDIAware(pkt);

            acedRegCmds->addCommand(L"FRWELLS", L"FrWellsTest", L"FrWellsTest",
                                    ACRX_CMD_MODAL, FrCommand<&FrWellsTest>);

            acedRegCmds->addCommand(L"FRWELLS", L"FrWellsExcel",
                                    L"FrWellsExcel", ACRX_CMD_MODAL,
                                    FrCommand<&FrWellsExcel>);

            acedRegCmds->addCommand(L"FRWELLS", L"FrWellsClear",
                                    L"FrWellsClear", ACRX_CMD_MODAL,
                                    FrCommand<&FrWellsClear>);

            acedRegCmds->addCommand(L"FRWELLS", L"FrWellsWind", L"FrWellsWind",
                                    ACRX_CMD_MODAL, FrCommand<&FrWellsWind>);

            acutPrintf(L"\nFRWELLS loaded.");
            break;
        }
        case AcRx::kUnloadAppMsg: {
            acedRegCmds->removeGroup(L"FRWELLS");
            break;
        }
    }
    return AcRx::kRetOK;
}