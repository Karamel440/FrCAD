#include <FrCommands.h>
#include <aced.h>
#include <rxregsvc.h>

#include "Main.h"

namespace {
constexpr wchar_t commandGroup[] = L"FrTestJig";
constexpr wchar_t commandName[] = L"FrTestJigHELLO";

}  // namespace

extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode message,
                                           void* applicationId) {
    switch (message) {
        case AcRx::kInitAppMsg:
            acrxDynamicLinker->unlockApplication(applicationId);
            acrxRegisterAppMDIAware(applicationId);

            if (acedRegCmds->addCommand(commandGroup, commandName, commandName,
                                        ACRX_CMD_MODAL,
                                        FrCommand<&FrTestyC>) != Acad::eOk) {
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
