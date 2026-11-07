#include "Nomenclature.h"

#include <FrBlockReference.h>
#include <FrDatabase.h>
#include <FrOutput.h>
#include <FrSelection.h>
#include <FrText.h>

#include "Exel.h"
#include "WellAssignment.h"

namespace {
constexpr wchar_t missingNumber[] = L"без номера!";
constexpr wchar_t ambiguousNumber[] = L"номер спорный";
}  // namespace

std::vector<FrBlockReference> collectWells() {
    auto db = FrDbCurrent();
    std::vector<FrBlockReference> result;

    for (auto& block : db.entities<FrBlockReference>()) {
        const auto name = block.space().name();

        if (name != L"M5_075" && name != L"M5_233_A") continue;

        result.push_back(std::move(block));
    }

    return result;
}

std::vector<FrText> collectNumbers() {
    auto db = FrDbCurrent();

    std::vector<FrText> result;

    for (auto& number : db.entities<FrText>()) {
        auto name = number.layer().name();
        if (name != L"45_Номера колодцев") continue;

        if (number.content().empty()) continue;

        result.push_back(std::move(number));
    }
    return result;
}

std::vector<FrBlockReference> collectMarks(const FrRefDatabase& db) {
    std::vector<FrBlockReference> result;

    for (auto& ent : db.entities<FrBlockReference>()) {
        const auto name = ent.space().name();

        if (name != L"M5_0_500" && !name.ends_with(L"|M5_0_500")) continue;

        result.push_back(std::move(ent));
    }

    return result;
}

/* Прежний алгоритм: каждый колодец независимо выбирал ближайшую подпись.
std::vector<Well> match(
    std::vector<FrBlockReference>& wells,
    std::vector<FrText>& numbers) {

    std::vector<Well> result;

    for (auto& well : wells) {

        Well res;
        res.pos = well.position();

        constexpr int epsilone = 3;

        double distance{ epsilone };
        FrText* nearest = nullptr;

        for (auto& number : numbers) {
            auto const closest =
                number.bounds().closestPoint(well.position());

            auto const dist =
                well.position().distanceTo(closest);

            if (dist < distance) {
                distance = dist;
                nearest = &number;
                continue;
            }
        }

        if (nearest) {
            res.number = nearest->content();
        }
        result.push_back(res);
    }
    return result;
}
*/

std::vector<Well> match(std::vector<FrBlockReference>& wells,
                        std::vector<FrText>& numbers) {
    std::vector<FrBounds> bounds;
    bounds.reserve(numbers.size());
    for (const auto& number : numbers) bounds.push_back(number.bounds());

    std::vector<Well> result(wells.size());
    std::vector<std::vector<well_assignment::Candidate>> candidates(
        wells.size());
    for (std::size_t i = 0; i < wells.size(); ++i) {
        auto& pos = result[i].pos;
        pos = wells[i].position();
        for (std::size_t j = 0; j < bounds.size(); ++j) {
            const double distance = pos.distanceTo(bounds[j].closestPoint(pos));
            if (distance <= well_assignment::searchRadius)
                candidates[i].push_back({j, distance});
        }
    }

    const auto assignments = well_assignment::solve(candidates, numbers.size());
    for (std::size_t i = 0; i < assignments.size(); ++i) {
        const auto& assignment = assignments[i];
        result[i].ambiguousNumber = assignment.ambiguous;
        if (assignment.label)
            result[i].number = numbers[*assignment.label].content();
    }
    return result;
}

void matchXRef(std::vector<Well>& matched) {
    auto db = FrDbCurrent();

    for (auto& space : db.spaces()) {
        if (!space.isReference()) continue;

        auto xrefDb = space.database();
        auto marks = collectMarks(xrefDb);

        for (auto& well : matched) {
            double distance = 0.55;
            FrBlockReference* nearest = nullptr;

            for (auto& mark : marks) {
                auto wellPos = well.pos;
                auto markPos = mark.position();

                wellPos.z = 0.0;
                markPos.z = 0.0;

                const auto dist = wellPos.distanceTo(markPos);

                if (dist < distance) {
                    distance = dist;
                    nearest = &mark;
                }
            }

            if (nearest) {
                well.height = nearest->elevation();
            }
        }
    }
}

void markNumbers(std::vector<Well>& wells) {
    auto db = FrDbCurrent();

    auto style = db.style(L"LIKE11");

    auto layerF = db.layer(L"Обнаружено");
    if (!layerF) layerF = db.createLayer(L"Обнаружено");

    auto layerNF = db.layer(L"Не обнаружено");
    if (!layerNF) layerNF = db.createLayer(L"Не обнаружено");

    for (auto& well : wells) {
        FrText text;

        if (well.number) {
            text.setContent(*well.number);
        }

        else {
            text.setContent(well.ambiguousNumber ? ambiguousNumber
                                                 : missingNumber);
        }

        text.setJustify(FrJustify::center);
        text.setPosition(well.pos);
        text.setHeight(0.5);

        if (style) text.setStyle(*style);

        if (well.height) {
            text.setColor(7);
            text.setLayer(*layerF);
            text.setElevation(*well.height);
        } else {
            text.setColor(1);
            text.setLayer(*layerNF);
        }
        db.append(text);
    }
}

std::vector<Well> collectChangedNunmbers() {
    auto db = FrDbCurrent();

    std::vector<Well> result;

    for (const auto& ent : db.entities<FrText>()) {
        Well well;

        const auto layer = ent.layer().name();
        if (layer != L"Обнаружено" && layer != L"Не обнаружено") continue;

        const auto number = ent.content();
        if (number == missingNumber || number == ambiguousNumber) continue;

        if (layer == L"Обнаружено") {
            well.height = ent.elevation();
        } else {
            well.height = std::nullopt;
        }

        well.number = number;
        well.pos = ent.position();
        result.push_back(well);
    }
    return result;
}
