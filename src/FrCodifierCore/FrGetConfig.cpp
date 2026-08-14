#include "FrCod.h"
#include <json.hpp>
#include <fstream>
#include <FrExceptions.h>
#include <magic_enum.hpp>

using json = nlohmann::json;

using FrRuleMap =
std::map<FrObjectCode, FrCodRule>;

namespace fs = std::filesystem;

fs::path FrCodifier::findJson() {

}

const FrCodRule& FrCodifier::getRule(FrObjectCode code)
{
	return rules_.at(code);
}

FrRuleMap FrCodifier::loadConfig(
	const fs::path& path) {

	std::ifstream stream{ path };

	if (!stream.is_open())
		throw FrException{
		L"Не удалось прочитать файл конфигурации"
	};

	json config;
	stream >> config;

	

	FrRuleMap result;

	for (const auto& [groupName, group] : config.items()) {
		for (const auto [objectName, rule] : group.items()) {

			auto code =
				magic_enum::enum_cast<FrObjectCode>(objectName);

			auto layer =
				rule.at("layer").get<std::string>();

			auto color =
				rule.at("colorIndex").get<int>();

			auto lineWeight =
				rule.at("lineWeight").get<double>();

			auto lineType =
				rule.at("lineType").get<std::string>();

			result.emplace(
				*code,
				FrCodRule{
					layer,
					color,
					lineWeight,
					lineType
				}
			);
		}
	}
	return result;
}
