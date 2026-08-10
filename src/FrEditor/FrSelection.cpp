#include <FrSelection.h>

AcGePoint3d FrGetPoint(
    const std::wstring& message = L"Выберите первую точку") {
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
    const std::wstring& message = L"Выберете вторую точку") {
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