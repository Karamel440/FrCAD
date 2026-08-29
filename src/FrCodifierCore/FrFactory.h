#pragma once
#include <FrDatabase.h>
#include <FrEntity.h>
#include <FrPolyline.h>

#include "FrCodRule.h"
#include "FrCodifier.h"
#include "FrCodifierEnums.h"

class FrFactory {
    friend class FrCodifier;

   const FrCodifier& cod_;
    FrRefDatabase& db_;

    FrFactory(
        const FrCodifier& cod, FrRefDatabase& db)
        : cod_(cod), db_(db) {}

    void applyRule(
        FrEntity& entity,
        const FrCodRule& rule
    ) const;

public:
    FrPolyline createLineObject(
        FrObjectCode code
    ) const;

    FrFactory& operator=(FrFactory&&) = delete;
    FrFactory(FrFactory&&) = delete;

    FrFactory& operator=(const FrFactory&) = delete;
    FrFactory(const FrFactory&) = delete;

    ~FrFactory() = default;
};
