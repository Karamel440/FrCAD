#include "blocks.h"

CAD_COMMAND(FrBlockImportAll)
{
    namespace fs = std::filesystem;

    fs::path blockPath = cad::io::findFile(L"M5_120.dwg");
    fs::path blockFolder = blockPath.parent_path();

    auto db = cad::db::current();

    int ok = 0;
    int failed = 0;

    for (const auto& entry : fs::directory_iterator(blockFolder)) {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() != L".dwg")
            continue;

        fs::path filePath = entry.path();
        std::wstring blockName = filePath.stem().wstring();

        try {
            cad::ops::importBlock(
                blockName.c_str(),
                filePath.c_str(),
                db);

            ++ok;
        }
        catch (const cad::core::Exception& e) {
            ++failed;

            cad::out::println(
                L"Не удалось импортировать: " +
                filePath.filename().wstring()
            );

            cad::out::println(e.where());
        }
    }

    cad::out::println(
        L"Импорт завершён. Успешно: " +
        std::to_wstring(ok) +
        L", ошибок: " +
        std::to_wstring(failed)
    );
}