#pragma once
#include <string>

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
	{
	}
};