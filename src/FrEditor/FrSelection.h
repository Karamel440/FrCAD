#pragma once
#include <aced.h>
#include <adscodes.h>
#include <FrEntity.h>
#include <FrDatabase.h>
#include <FrExceptions.h>

//template <typename T>
//requires std::derived_from<T, FrEntity>
//class FrSelectionSet {
//	AcDbObjectIdArray ids_;
//    std::vector<T> ents_;
//
//public:
//
//    using NativeType = typename T::NativeType;
//
//	FrSelectionSet(AcDbObjectIdArray ids)
// я       :ids_(ids) { 
//                
//        for (const auto& id : ids) {
//            if (is == AcDbObjectId::kNull)
//                continue;
//
//            FrObject obj{ id };
//            if(obj.is<T>)
//                ents_.emplace_back(T{ id });
//            throw FrException{
//                L"Выбраны не типовые объекты"
//            };
//        }
//    }
//
//   const std::vector<T>& entities() const {
//        return ents_;
//    }
//};

template <typename T = FrEntity>
requires std::derived_from<T,FrEntity>
std::vector<T> FrSelect() {

    std::vector<T> result;

    ads_name ss;
    const int status = acedSSGet(
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        ss
    );

    if (status == RTCAN)
    {
        throw FrCancel{};
    }
    
    if (status != RTNORM) {
        return {};
    }

    int length = 0;
    if (acedSSLength(ss, &length) != RTNORM) {
        acedSSFree(ss);
        return {};
    }

    for (int i{}; i < length; i++) {
        ads_name ent;

        if (acedSSName(ss, i, ent) != RTNORM)
            continue;

        AcDbObjectId id;

        if (acdbGetObjectId(id, ent) != Acad::eOk)
            continue;

        FrObject obj{ id };

        if (!obj.is<T>())
            continue;

        result.emplace_back(obj.as<T>());
    }
    acedSSFree(ss);
    return result;
}

AcGePoint3d FrGetPoint(
   const std::wstring& message = L"Выберете первую точку"
);

AcGePoint3d FrGetPoint(
    AcGePoint3d firstPoint,
    const std::wstring& message = L"Выберете первую точку"
);