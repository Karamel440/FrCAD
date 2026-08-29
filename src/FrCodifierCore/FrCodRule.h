#pragma once
#include <string>
#include "FrCodifierEnums.h"
#include <FrString.h>

#include <magic_enum.hpp>

struct RawInput {
	std::string layer;
	std::string layerCode;
	std::string layerTag;
	
	std::string objectCode;

	int colorIndex;

	double lineWeight;
	std::string lineType;

};

struct LayerInfo {
	std::wstring layerName;
	FrLayerTag tag;
};

struct ViewInfo {
	int colorIndex;
	std::wstring lineType;
	AcDb::LineWeight lineWeight;
};

class FrCodRule
{
	LayerCode layer_;
	ViewInfo view_;

public:
	FrCodRule(const RawInput& input) {
		
		auto code = magic_enum::enum_cast<LayerCode>(input.layerCode);

		if (!code) {
			throw FrException{
				L"Не удалось распарсить слои"
			};
		}

		layer_ = *code;

		view_.colorIndex = input.colorIndex;
		view_.lineType = toWstring(input.lineType);
		view_.lineWeight = static_cast<AcDb::LineWeight>(input.lineWeight * 100);
	}
			
	LayerCode layerCode() const {
		return layer_;
	}
	ViewInfo view() const {
		return view_;
	}

	int colorIndex() const {
		return view_.colorIndex;
	}

	const std::wstring& lineType() const {
		return view_.lineType;
	}
	AcDb::LineWeight lineWeight() const {
		return view_.lineWeight;
	}
};

