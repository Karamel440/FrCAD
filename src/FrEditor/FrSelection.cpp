#include <Windows.h>
#include <ShObjIdl.h>
#include <atlbase.h>

#include <acedads.h>

#include <FrSelection.h>

AcGePoint3d FrGetPoint(
    const std::wstring& message) {
    ads_point pt;

    int status = acedGetPoint(
        nullptr,
        message.c_str(),
        pt
    );
    if (status == RTCAN)
        throw FrCancel{};
    
    if (status != RTNORM)
        throw FrException{
        L"Попробуйте еще раз!"
    };

    return AcGePoint3d{
        pt[0],
        pt[1],
        pt[2]
    };
}

AcGePoint3d FrGetPoint(
    AcGePoint3d firstPoint,
    const std::wstring& message) {
    ads_point pt;
    ads_point point{
        firstPoint.x,
        firstPoint.y,
        firstPoint.z
    };

    int status = acedGetPoint(
        point,
        message.c_str(),
        pt
    );
    if (status == RTCAN)
        throw FrCancel{};

    if (status != RTNORM)
        throw FrException{
        L"Попробуйте еще раз!"
    };

    return AcGePoint3d{
        pt[0],
        pt[1],
        pt[2]
    };
}

fs::path FrSelectFolder(const std::wstring& prompt)
{
    auto result = CoInitializeEx(
        nullptr,
        COINIT_APARTMENTTHREADED
    );

    if (FAILED(result)) {
        throw FrException{
        L"Ошибка при инициализации диалога"
        };
    }

    fs::path string{};

    try {
        CComPtr<IFileOpenDialog> dialog = nullptr;

        auto res = CoCreateInstance(
            CLSID_FileOpenDialog,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&dialog)
        );

        if (FAILED(res)) {
            throw FrException{
            L"Ошибка при получении COM"
            };
        }

        FILEOPENDIALOGOPTIONS options{};

        result = dialog->GetOptions(&options);

        if (FAILED(result)) {
            throw FrException{
            L"Ошибка при получении настроек окна"
            };
        }

        result = dialog->SetTitle(prompt.c_str());

        if (FAILED(result)) {
            throw FrException{
            L"Ошибка при наименовании окна"
            };
        }

        options |= FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM;

        result = dialog->SetOptions(options);

        if (FAILED(result)) {
            throw FrException{
            L"Ошибка при настройке окна"
            };
        }

        auto autocad = adsw_acadMainWnd();

        result = dialog->Show(autocad);

        if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED))
            throw FrCancel{};

        if (FAILED(result)) {
            throw FrException{
            L"Ошибка при показе окна"
            };
        }


        CComPtr<IShellItem> folder;
        result = dialog->GetResult(&folder);

        if (FAILED(result)) {
            throw FrException{
            L"Ошибка при получении результата"
            };
        }

        CComHeapPtr<wchar_t> path;

        result = folder->GetDisplayName(
            SIGDN_FILESYSPATH,
            &path
        );

        if (FAILED(result)) {
            throw FrException{
            L"Ошибка при получении пути папки"
            };
        }
        string = path.m_pData;
    }
    catch (...) {
        CoUninitialize();
        throw;
    }

    CoUninitialize();
    return string;
}