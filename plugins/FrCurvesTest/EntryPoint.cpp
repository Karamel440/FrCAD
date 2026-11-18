#include <FrCommands.h>
#include <aced.h>
#include <rxregsvc.h>

#include "main.h"

namespace {
constexpr wchar_t commandGroup[] = L"FrCurvesTest";
constexpr wchar_t commandName[] = L"FrCurvesTestHELLO";

void hello() { acutPrintf(L"\nFrCurvesTest is loaded."); }
}  // namespace

extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode message,
                                           void* applicationId) {
    switch (message) {
        case AcRx::kInitAppMsg:
            acrxDynamicLinker->unlockApplication(applicationId);
            acrxRegisterAppMDIAware(applicationId);

            if (acedRegCmds->addCommand(
                    commandGroup, commandName, commandName, ACRX_CMD_MODAL,
                    FrCommand<&FrTestSegments>) != Acad::eOk) {
                return AcRx::kRetError;
            }
            break;
        case AcRx::kUnloadAppMsg:
            acedRegCmds->removeGroup(commandGroup);
            break;
        default:
            break;
    }

    return AcRx::kRetOK;
}
