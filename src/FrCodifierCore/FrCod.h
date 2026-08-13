#pragma once
#include <map>
#include <optional>
#include <filesystem>
#include <FrDatabase.h>

namespace fs = std::filesystem;

enum class FrObjectCode {
	rainSewer,
	mainSewer,
	pressureSewer
};

struct FrCodRule
{
	std::string layer_;
	int colorIndex_;
	double lineWeight_;
	std::string lineType_;

	FrCodRule(
		std::string layer,
		int colorIndex,
		double lineWeight,
		std::string lineType) :
		layer_(layer),
		colorIndex_(colorIndex),
		lineWeight_(lineWeight),
		lineType_(lineType)
	{ }

};

class FrCodifier {
	
	using FrRuleMap = std::map<FrObjectCode, FrCodRule>;

	FrRuleMap rules_;

	FrRuleMap loadConfig(
		const fs::path& path
	);

	fs::path findJson();

public:
	FrCodifier()
		: rules_(loadConfig(findJson())) { }

	FrCodifier(fs::path path)
		:rules_(loadConfig(path)) { }
	
	const FrCodRule& getRule(FrObjectCode code);


};

class FrModifier {
	FrRefDatabase* db_;
	FrCodifier* cod_;

public:
	FrModifier(FrRefDatabase* db, FrCodifier* cod)
		:db_(db),cod_(cod) { }

	void apply(FrEntity& ent, FrObjectCode code);
};
