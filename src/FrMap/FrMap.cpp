#include "FrMap.h"

#include <FrString.h>
#include <FrSelection.h>

#include <cmath>
#include <glaze/json.hpp>
#include <unordered_map>

#include "Jigs.h"

FrPolyline FrMap::createLineSign(FrLineSign sign) {
    auto error = glz::read_file_json(lineRules, "D:/Macro/Fr/config/LayerObjects.json",
                                     std::string{});

    if (error)
        throw FrException(L"Не удалось прочитать правила линейных объектов");

    auto rule = lineRules.at(sign);

    FrPolyline line;
    auto layer = db_.layer(toWstring(rule.layer));

    if (!layer) {
        layer = db_.createLayer(toWstring(rule.layer));
    }

    line.setLayer(*layer);
    line.setColor(rule.color);

    AcDb::LineWeight weight =
        static_cast<AcDb::LineWeight>(std::lround(rule.lineweight * 100.0));

    line.setLineWeight(weight);
    line.setLinetype(toWstring(rule.linetype));

    auto pt = FrGetPoint(L"Выберете первую точку:\n");

    line.addVertex(pt);
    line.addVertex(pt);

    FrPolylineJig jig(line);

    while (true) {
        auto status = jig.drag();

        if (status == AcEdJig::kNormal) {
            jig.addNextVertex();
            continue;
        }
        jig.removeLastVertex();
        break;
    }
    db_.append(line);
    return line;
}
