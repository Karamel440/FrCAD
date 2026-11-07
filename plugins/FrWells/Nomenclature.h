#pragma once
#include <FrBlockReference.h>
#include <FrDatabase.h>
#include <FrText.h>

#include "Tablet.h"

struct Well {
    std::optional<std::wstring> number{};
    AcGePoint3d pos{};

    std::optional<double> height{};
    bool ambiguousNumber = false;
};

std::vector<FrBlockReference> collectWells();
std::vector<FrText> collectNumbers();
std::vector<FrBlockReference> collectMarks(const FrRefDatabase& db);

std::vector<Well> match(std::vector<FrBlockReference>& wells,
                        std::vector<FrText>& numbers);

void matchXRef(std::vector<Well>& matched);

void markNumbers(std::vector<Well>& wells);

std::vector<Well> collectChangedNunmbers();
