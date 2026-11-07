#pragma once
#include <FrSelection.h>

#include <filesystem>
#include <vector>

#include "Exel.h"
#include "Nomenclature.h"
#include "Tablet.h"

namespace fs = std::filesystem;

inline void FrWellsTest() {
    auto pt = FrGetPoint(L"Выберете точку внутри планшета:\n");

    Tablet tab{pt};

    auto wells = collectWells();
    auto nums = collectNumbers();

    auto matchedWells = match(wells, nums);
    // Подпись за границей планшета может относиться к колодцу внутри него.
    std::erase_if(matchedWells,
                  [&tab](const Well& well) { return !tab.contains(well.pos); });
    matchXRef(matchedWells);

    markNumbers(matchedWells);
}

inline void FrWellsExcel() {
    auto wells = collectChangedNunmbers();

    if (wells.empty()) throw FrException{L"Нет колодцев!"};

    Tablet tab{wells.back().pos};

    auto exp = findExplication(tab.getNomenclature(), FrDbCurrent().folder());

    FrExcel doc{exp};
    doc.processWells(wells);
}

inline void FrWellsClear() {
    for (auto& text : FrDbCurrent().entities<FrText>()) {
        if (text.layer().name() == L"Обнаружено" ||
            text.layer().name() == L"Не обнаружено")
            text.erase();
    }
}