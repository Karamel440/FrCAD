#pragma once
#include <FrString.h>

#include <OpenXLSX/OpenXLSX.hpp>
#include <filesystem>
#include <string>

#include "Nomenclature.h"

namespace fs = std::filesystem;
using namespace OpenXLSX;

class FrExcel {
    fs::path path_;
    XLDocument doc;
    XLWorkbook workbook;

  public:
    FrExcel(const fs::path& excel)
        : path_((excel.extension() == L".xls" || excel.extension() == L".XLS")
                    ? convertToXLSX(excel)
                    : excel),
          doc(toString(path_.native())),
          workbook(doc.workbook()) {}

    fs::path convertToXLSX(const fs::path& file);

    fs::path convertToXLS(const fs::path& directory);
    bool isHeader(const XLRow& row);
    std::optional<XLRow> findWell(const Well& well);
    void changeWell(XLRow& row, std::optional<double> height, int year);
    void processWells(const std::vector<Well>& wells);
};
fs::path findExplication(int nomenclature, const fs::path& archive);
