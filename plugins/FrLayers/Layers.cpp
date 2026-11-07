#include "Layers.h"
#include <FrDatabase.h>
#include <FrSelection.h>
#include <adscodes.h>
#include <cwctype>
#include <string_view>

static int getPrefix(std::wstring_view name) {
    if (name.size() < 2 || name[0] < L'0' || name[0] > L'9' ||
        name[1] < L'0' || name[1] > L'9')
        return -1;

    return (name[0] - L'0') * 10 + (name[1] - L'0');
}

static bool isBaseLayer(int prefix) {
    return prefix == 2
        || prefix == 7
        || prefix == 18
        || prefix == 37
        || prefix == 38
        || prefix == 41
        || prefix == 43
        || prefix == 46;
}

void FrLayersOnAll() {

    for (const auto& layer : FrDbCurrent().layers()) {
        layer.on();
    }
}

void FrLayersIsolate() {
    ads_name entity;
    ads_point point;
    const int status = acedEntSel(L"\nВыберите объект: ", entity, point);
    if (status == RTCAN)
        throw FrCancel{};
    if (status != RTNORM)
        throw FrException{ L"Не удалось выбрать объект" };

    AcDbObjectId id;
    FrCheckStatus(acdbGetObjectId(id, entity), L"Не удалось получить ID объекта");
    const auto selectedLayer = FrEntity{ id }.layer().name();

    for (const auto& layer : FrDbCurrent().layers()) {
        if (layer.isReference())
            continue;
        if (layer.name() == selectedLayer)
            layer.on();
        else
            layer.off();
    }
}

void FrLayersOffSelected() {
	auto selected = FrSelect<FrEntity>();
	if (selected.size() != 1) {
		throw FrException{
			L"Нужно выбрать один объект"
		};
	}
	auto layer = selected.front().layer();
	layer.off();
}
    


bool isUnderLayer(std::wstring_view name) noexcept {
    if (name.size() < 3 ||
        !std::iswdigit(name[0]) ||
        !std::iswdigit(name[1]) ||
        name[2] != L'_')
    {
        return false;
    }

    const int number =
        (name[0] - L'0') * 10 +
        (name[1] - L'0');

    return
        (number >= 30 && number <= 43) ||
        number == 46 ||
        (number >= 50 && number <= 57);
}

void FrLayersOffUnder() {
    for (const auto& layer : FrDbCurrent().layers()) {
        if (layer.isReference())
            continue;

        const auto name = layer.name();

        if (isUnderLayer(name))
            layer.off();
        else
            layer.on();
    }
} 

void FrLayersOnCabels() {
    for (const auto& layer : FrDbCurrent().layers()) {
        if (layer.isReference())
            continue;
        if (isBaseLayer(getPrefix(layer.name())))
            layer.on();
        else
            layer.off();
    }
}

void FrLayersOnWells() {
    for (const auto& layer : FrDbCurrent().layers()) {
        if (layer.isReference())
            continue;

        const auto name = layer.name();
        const int prefix = getPrefix(name);
        if (prefix == 44 || prefix == 45 || prefix == 18 || prefix == 2 || name == L"0")
            layer.on();
        else
            layer.off();
    }
}
