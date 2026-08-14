#include "FrCod.h"
#include <Windows.h>
#include <string>

std::wstring& toWstring(const std::string& string)
{
	int size = MultiByteToWideChar(
		CP_UTF8,
		0,
		string.data(),
		static_cast<int>(string.size()),
		nullptr,
		0
	);

	std::wstring result(size, L'\0');

	MultiByteToWideChar(
		CP_UTF8,
		0,
		string.data(),
		static_cast<int>(string.size()),
		result.data(),
		size
	);

	return result;
}

void FrModifier::apply(FrEntity& ent,FrObjectCode code)
{
	const auto& rule = cod_->getRule(code);

	const auto layerName = toWstring(rule.layer_);

	auto layer = db_->layer(layerName);

	if (!layer)
	layer = db_->createLayer(layerName);

	auto lineWeight =
		static_cast<AcDb::LineWeight>(
			static_cast<int>(rule.lineWeight_ * 100));

	ent.setLayer(*layer);
	ent.setColor(rule.colorIndex_);
	ent.setLineWeight(lineWeight);
	ent.setLinetype(toWstring(rule.lineType_));
}

