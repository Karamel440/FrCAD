#include "FrFactory.h"
#include <string>
#include "FrString.h"

void FrFactory::applyRule(FrEntity& entity, const FrCodRule& rule) const {
}

FrPolyline FrFactory::createLineObject(FrObjectCode code) const
{
	auto rule = cod_.getRule(code);
	FrPolyline line;
	
	applyRule(line, rule);
	return line;
}
