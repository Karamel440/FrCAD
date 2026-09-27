#include "Parser.h"

extern "C" 
__declspec(dllexport)
int testParse(
    const wchar_t* input, wchar_t* output, int capacity) {

	Parser par{input};
	auto result = par.parse();

	int required = static_cast<int>(result.size() + 1);

	if (output == nullptr || capacity < required)
		return required;

	wcscpy_s(output, capacity, result.c_str());

	return required;
}