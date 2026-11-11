#pragma once
#include <FrBlockReference.h>
#include <FrDatabase.h>
#include <FrPolyline.h>
#include <FrText.h>

#include <magic_enum/magic_enum.hpp>
#include <glaze/forward.hpp>
#include <string>

enum class FrLineSign : int { mainSewer, rainSewer };

namespace glz {

template <typename T>
    requires std::is_enum_v<T>
struct meta<T> {
    static constexpr auto keys = magic_enum::enum_names<T>();
    static constexpr auto value = magic_enum::enum_values<T>();
};

}  // namespace glz

struct FrLineRule {
    std::string layer;
    int color;
    double lineweight = 0.2;
    std::string linetype = "Continuous";
};

class FrMap {
    FrRefDatabase db_;
    std::unordered_map<FrLineSign, FrLineRule> lineRules;

  public:
    FrMap(FrRefDatabase& db) : db_(db) {}
    FrMap() = delete;

    FrPolyline createLineSign(FrLineSign sign);
    void setLineSign(FrLineSign sign, FrPolyline& line);
};