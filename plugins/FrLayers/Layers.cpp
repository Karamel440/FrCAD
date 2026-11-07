#include "cad.h"
#include <FrDatabase.h>
#include <FrSelection.h>
#include <FrEntity.h>
#include <cwchar>

static AcDbObjectIdArray getLayers() {
    auto db = cad::db::current();
    AcDbObjectIdArray layers;

    cad::db::collectRecords<AcDbLayerTable>(
        db.get(),
        &AcDbDatabase::getLayerTable,
        layers
    );

    return layers;
}

static int getPrefix(const AcString& name) {
    const ACHAR* text = name.kACharPtr();

    if (text[0] < L'0' || text[0] > L'9')
        return -1;

    if (text[1] < L'0' || text[1] > L'9')
        return -1;

    return (text[0] - L'0') * 10 + (text[1] - L'0');
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

CAD_COMMAND(FrLayersIsolate) {
    auto entity = cad::db::open<AcDbEntity>(
        cad::ed::select()
    );

    AcDbObjectId selectedLayer = entity->layerId();
    AcDbObjectIdArray layers = getLayers();

    for (const AcDbObjectId& id : layers) {
        auto layer = cad::db::open<AcDbLayerTableRecord>(
            id,
            AcDb::kForWrite
        );
        
        AcString name;
        layer->getName(name);

        if (std::wcschr(name.kACharPtr(), L'|'))
            continue;
        layer->setIsOff(id != selectedLayer);
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

CAD_COMMAND(FrLayersOnCabels) {
    AcDbObjectIdArray layers = getLayers();

    for (const AcDbObjectId& id : layers) {
        auto layer = cad::db::open<AcDbLayerTableRecord>(
            id,
            AcDb::kForWrite
        );

        AcString name;
        layer->getName(name);

        if (std::wcschr(name.kACharPtr(), L'|'))
            continue;

        layer->setIsOff(!isBaseLayer(getPrefix(name)));
    }
}

CAD_COMMAND(FrLayersOnWells) {
    AcDbObjectIdArray layers = getLayers();

    for (const auto& id : layers) {
        auto layer = cad::db::open<AcDbLayerTableRecord>(id, AcDb::kForWrite);

        AcString layerName;
        layer->getName(layerName);

        if (std::wcschr(layerName.kACharPtr(),L'|'))
            continue;

       int prefix = getPrefix(layerName);

        bool onLayers =
            prefix == 44 ||
            prefix == 45 ||
            prefix == 18 ||
            prefix == 02 ||
            layerName == L"0";
        layer->setIsOff(!onLayers);
    }
}