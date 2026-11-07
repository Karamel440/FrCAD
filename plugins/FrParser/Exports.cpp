#include "Parser.h"

__declspec(dllexport) std::wstring parse(std::wstring raw) {
    Parser parser{raw};
    return parser.parse();
}

__declspec(dllexport) std::wstring parse(std::wstring raw, bool found, int year) {
    Parser parser{raw};
    return parser.parse(ZoneUpdate{found, year});
}


extern "C" __declspec(dllexport) 
int __stdcall parseVBA(
    const wchar_t* input, 
    int status, int year, 
    wchar_t* output, int capacity) {
    if (!input || capacity < 0 || (status != 0 && status != 1))
        return -1;

    try {
        auto result = parse(std::wstring{input}, status == 1, year);

        if (result.size() >= static_cast<std::size_t>(INT_MAX))
            return -1;

        int required = static_cast<int>(result.size()) + 1;

        // Возвращаем требуемую ёмкость, если буфера пока нет
        // или он слишком маленький.
        if (!output || capacity < required)
            return required;

        if (wcscpy_s(output, capacity, result.c_str()) != 0)
            return -1;

        return required;
    }
    catch (...) {
        return -1;
    }
}