#include "FrCod.h"
#include <json.hpp>
#include <fstream>
#include <FrExceptions.h>

using json = nlohmann::json;

using FrRuleMap =
std::map<FrObjectCode, FrCodRule>;

namespace fs = std::filesystem;

const fs::path& FrCodifier::findJson() {

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

	const auto& sewerGroup = config.at("sewer");
	const auto& rainSewer = sewerGroup.at("rainSewer");

	result.emplace(
		FrObjectCode::rainSewer,
		FrCodRule{
			rainSewer.at("layer").get<std::string>(),
			rainSewer.at("colorIndex").get<int>(),
			rainSewer.at("lineWeight").get<double>(),
			rainSewer.at("lineType").get<std::wstring>()
		}
	);

	return result;
}
