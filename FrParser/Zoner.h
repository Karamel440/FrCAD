#pragma once
#include <vector>

#include "Tokenaizer.h"

enum class ZoneTag {
	KKzone,
	NOzone,
	Freezone
};

struct Zone {
	ZoneTag tag{};
	std::vector<Token> tokens;
};

