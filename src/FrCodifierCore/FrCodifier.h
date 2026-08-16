#pragma once
#include <map>
#include <filesystem>

#include "FrFactory.h"
#include "FrCodifierEnums.h"
#include "FrCodRule.h"

namespace fs = std::filesystem;

class FrCodifier {

	using FrRuleMap = std::map<FrObjectCode, FrCodRule>;

	FrRuleMap rules_;

	FrRuleMap loadConfig(
		const fs::path& path
	);

	fs::path findJson();

public:
	FrCodifier()
		: rules_(loadConfig(findJson())) {
	}

	FrCodifier(fs::path path)
		:rules_(loadConfig(path)) {
	}

	FrFactory factory(FrRefDatabase& db) const;
	const FrCodRule getRule(FrObjectCode code);

};
