#pragma once
#include <map>
#include <filesystem>

#include "FrFactory.h"
#include "FrCodifierEnums.h"
#include "FrCodRule.h"

namespace fs = std::filesystem;

class FrFactory;

class FrCodifier {

	using FrRuleMap = std::map<FrObjectCode, FrCodRule>;
	using FrLayerMap = std::map<LayerCode, LayerInfo>;

	FrRuleMap rules_;
	FrLayerMap layers_;

	FrRuleMap loadObjectConfig(
		const fs::path& path
	);

	FrLayerMap loadLayerConfig(
		const fs::path& path
	);

	fs::path findObjectJson() const;
	fs::path findLayerJson() const;

public:
	FrCodifier()
		: rules_(loadObjectConfig(findObjectJson())),
		layers_(loadLayerConfig(findLayerJson()))
	{
	}

	explicit FrCodifier(fs::path path)
		:rules_(loadObjectConfig(path)) {
	}

	FrFactory factory(FrRefDatabase& db) const;
	const FrCodRule getRule(FrObjectCode code) const;

};
