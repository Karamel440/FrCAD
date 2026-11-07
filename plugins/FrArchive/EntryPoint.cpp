#include <aced.h>
#include <rxregsvc.h>
#include "Recolor.h"
#include <FrCommands.h>

extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt) {
    switch (msg) {
    case AcRx::kInitAppMsg: {
        acrxDynamicLinker->unlockApplication(pkt);
        acrxRegisterAppMDIAware(pkt);

        acedRegCmds->addCommand(L"ARCHIVEGROUP", L"FrArchiveRecolor",
            L"FrArchiveRecolor", ACRX_CMD_MODAL, &FrCommand<FrArchiveRecolor>);

        acedRegCmds->addCommand(L"ARCHIVEGROUP", L"FrArchiveMakeArchive",
            L"FrArchiveMakeArchive", ACRX_CMD_MODAL, &FrCommand<FrArchiveMakeArchive>);

        acedRegCmds->addCommand(L"ARCHIVEGROUP", L"FrArchiveMake",
            L"FrArchiveMake", ACRX_CMD_MODAL, &FrCommand<FrArchiveMake>);

        acedRegCmds->addCommand(L"ARCHIVEGROUP", L"FrTest",
            L"FrTest", ACRX_CMD_MODAL, &FrCommand<FrTest>);

        acedRegCmds->addCommand(L"ARCHIVEGROUP", L"FrArchiveRecolorDwg",
            L"FrArchiveRecolorDwg", ACRX_CMD_MODAL, &FrCommand<FrArchiveRecolorDwg>);

        acutPrintf(L"\nFrArchive loaded");
        break;
    }
    case AcRx::kUnloadAppMsg: {
        acedRegCmds->removeGroup(L"ARCHIVEGROUP");
        break;
    }
    }
    return AcRx::kRetOK;
}