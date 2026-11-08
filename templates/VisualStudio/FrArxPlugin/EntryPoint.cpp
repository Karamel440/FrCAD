#include <aced.h>
#include <rxregsvc.h>

namespace {
constexpr wchar_t commandGroup[] = L"$safeprojectname$";
constexpr wchar_t commandName[] = L"$safeprojectname$HELLO";

void hello() {
    acutPrintf(L"\n$safeprojectname$ is loaded.");
}
}

extern "C" AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode message, void* applicationId) {
    switch (message) {
    case AcRx::kInitAppMsg:
        acrxDynamicLinker->unlockApplication(applicationId);
        acrxRegisterAppMDIAware(applicationId);

        if (acedRegCmds->addCommand(commandGroup, commandName, commandName,
                                   ACRX_CMD_MODAL, hello) != Acad::eOk) {
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
