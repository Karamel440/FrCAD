#pragma once
#include <FrDatabase.h>
#include <FrPolyline.h>
#include <FrBlockReference.h>
#include <FrText.h>

enum class FrLineSign : int {
    mainSewer = 1,
    rainSewer = 2,

};

class FrMap {
    FrRefDatabase db_;

  public:
    FrMap(FrRefDatabase& db) : db_(db) {}
    FrMap() = delete;

    FrPolyline createLineSign();
    
};