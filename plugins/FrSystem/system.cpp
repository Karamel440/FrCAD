#include "system.h"

void setFrStandardVars()
{
    cad::sys::set(L"INSUNITS", 6);        // meters

    cad::sys::set(L"CMDECHO", 0);
    cad::sys::set(L"FILEDIA", 1);
    cad::sys::set(L"CMDDIA", 1);
    cad::sys::set(L"PROXYNOTICE", 1);

    cad::sys::set(L"PICKFIRST", 1);
    cad::sys::set(L"PICKADD", 2);

    cad::sys::set(L"LTSCALE", 1.0);
    cad::sys::set(L"CELTSCALE", 1.0);
    cad::sys::set(L"PSLTSCALE", 1);
    cad::sys::set(L"MSLTSCALE", 1);

    cad::sys::set(L"CLAYER", L"0");
    cad::sys::set(L"CELTYPE", L"Continuous");
    cad::sys::set(L"CECOLOR", L"7");
    cad::sys::set(L"CELWEIGHT", 20);
    cad::sys::set(L"TEXTSTYLE", L"Standard");

    cad::out::println(L"Стандартные системные переменные выставлены");
}

CAD_COMMAND(FrSystemClearAll)
{
    cad::sys::QuietGuard quiet;
    cad::sys::PropsGuard props;

    auto db = cad::db::current();

    for (int pass = 0; pass < 3; ++pass) {
        AcDbObjectIdArray ids;

        cad::db::collectBlockRecords(db.get(), ids);

        cad::db::collectRecords<AcDbLayerTable>(
            db.get(),
            &AcDbDatabase::getLayerTable,
            ids
        );


        cad::db::collectRecords<AcDbLinetypeTable>(
            db.get(),
            &AcDbDatabase::getLinetypeTable,
            ids
        );

        cad::db::collectRecords<AcDbTextStyleTable>(
            db.get(),
            &AcDbDatabase::getTextStyleTable,
            ids
        );

        cad::db::collectRecords<AcDbDimStyleTable>(
            db.get(),
            &AcDbDatabase::getDimStyleTable,
            ids
        );

        cad::db::collectRecords<AcDbRegAppTable>(
            db.get(),
            &AcDbDatabase::getRegAppTable,
            ids
        );

        if (ids.isEmpty())
            break;

        Acad::ErrorStatus es = db->purge(ids);

        if (es != Acad::eOk) {
            throw cad::core::Exception(
                cad::core::ErrorType::AcadError,
                L"FrClearAll",
                es
            );
        }

        if (ids.isEmpty())
            break;

        cad::ops::tryErase(ids);
    }

    cad::out::println(L"Очистка завершена");
}

 int layerNumber(const ACHAR* name) noexcept {
        if (!name ||
            name[0] < L'0' || name[0] > L'9' ||
            name[1] < L'0' || name[1] > L'9')
        {
            return -1;
        }

        return (name[0] - L'0') * 10 +
            (name[1] - L'0');
    }

 namespace {

     bool sameText(const ACHAR* left, const ACHAR* right) noexcept {
         return left &&
             right &&
             std::wcscmp(left, right) == 0;
     }

     bool isTopPolyline(const AcDbEntity& entity) noexcept {
         return sameText(entity.linetype(), L"M2_001") ||
             sameText(entity.linetype(), L"M5_001");
     }

     bool isSpecialHatchLayer(const ACHAR* layerName) noexcept {
         static constexpr const ACHAR* layers[] = {
             L"50_Заливка камер и шахт на канализации",
             L"51_Заливка камер на водопроводе",
             L"52_Заливка камер на теплосети",
             L"53_Заливка камер на газопроводе",
             L"54_Заливка камер на трубопроводах спецназначения",
             L"55_Заливка камер на телефонной канализации",
             L"56_Заливка габаритов кабельных колодцев"
         };

         for (const ACHAR* name : layers) {
             if (sameText(layerName, name))
                 return true;
         }

         return false;
     }

     bool isTopHatch(const AcDbEntity& entity) noexcept {
         const Adesk::UInt16 color = entity.colorIndex();

         return color == 140 ||
             color == 7 ||
             (
                 color == 255 &&
                 isSpecialHatchLayer(entity.layer())
                 );
     }

     bool isTopBlock(AcDbEntity* entity) {
         AcDbBlockReference* block =
             AcDbBlockReference::cast(entity);

         if (!block)
             return false;

         auto blockRecord =
             cad::db::open<AcDbBlockTableRecord>(
                 block->blockTableRecord()
             );

         AcString blockName;

         const Acad::ErrorStatus es =
             blockRecord->getName(blockName);

         if (es != Acad::eOk) {
             throw cad::core::Exception(
                 cad::core::ErrorType::AcadError,
                 L"FrSystemSort: block name",
                 es
             );
         }

         return sameText(blockName.kACharPtr(), L"M2_075") ||
             sameText(blockName.kACharPtr(), L"M5_075");
     }

     Acad::ErrorStatus moveTop(
         AcDbSortentsTable* table,
         const AcDbObjectIdArray& ids)
     {
         if (ids.isEmpty())
             return Acad::eOk;

         return table->moveToTop(ids);
     }

     Acad::ErrorStatus moveBottom(
         AcDbSortentsTable* table,
         const AcDbObjectIdArray& ids)
     {
         if (ids.isEmpty())
             return Acad::eOk;

         return table->moveToBottom(ids);
     }

 }

 CAD_COMMAND(FrSystemSort) {
     auto db = cad::db::current();

     auto currentSpace =
         cad::db::open<AcDbBlockTableRecord>(
             db->currentSpaceId()
         );

     AcDbObjectIdArray entityIds;

     AcDbBlockTableRecordIterator* iterator = nullptr;

     Acad::ErrorStatus es =
         currentSpace->newIterator(iterator);

     if (es != Acad::eOk || !iterator) {
         delete iterator;

         throw cad::core::Exception(
             cad::core::ErrorType::AcadError,
             L"FrSystemSort: current space iterator",
             es
         );
     }

     for (
         iterator->start();
         !iterator->done();
         iterator->step()
         ) {
         AcDbObjectId id;

         if (iterator->getEntityId(id) == Acad::eOk &&
             !id.isNull())
         {
             entityIds.append(id);
         }
     }

     delete iterator;

     AcDbObjectIdArray topPolylines;
     AcDbObjectIdArray otherPolylines;

     AcDbObjectIdArray texts;

     AcDbObjectIdArray allBlocks;
     AcDbObjectIdArray topBlocks;

     AcDbObjectIdArray topHatches;
     AcDbObjectIdArray otherHatches;

     AcDbObjectIdArray images;

     for (const AcDbObjectId& id : entityIds) {
         auto entity = cad::db::open<AcDbEntity>(id);

         const AcRxClass* entityClass = entity->isA();

         if (!entityClass)
             continue;

         const ACHAR* type = entityClass->dxfName();

         if (sameText(type, L"LWPOLYLINE")) {
             if (isTopPolyline(*entity))
                 topPolylines.append(id);
             else
                 otherPolylines.append(id);
         }
         else if (sameText(type, L"TEXT")) {
             texts.append(id);
         }
         else if (sameText(type, L"INSERT")) {
             allBlocks.append(id);

             if (isTopBlock(entity.object()))
                 topBlocks.append(id);
         }
         else if (sameText(type, L"HATCH")) {
             if (isTopHatch(*entity))
                 topHatches.append(id);
             else
                 otherHatches.append(id);
         }
         else if (sameText(type, L"IMAGE")) {
             images.append(id);
         }
     }

     AcDbSortentsTable* sortTable = nullptr;

     es = currentSpace->getSortentsTable(
         sortTable,
         AcDb::kForWrite,
         true
     );

     if (es != Acad::eOk) {
         throw cad::core::Exception(
             cad::core::ErrorType::AcadError,
             L"FrSystemSort: getSortentsTable",
             es
         );
     }

     if (!sortTable) {
         throw cad::core::Exception(
             cad::core::ErrorType::NullPointer,
             L"FrSystemSort: sortTable"
         );
     }

     // Точная последовательность старого KSORT.
     es = moveBottom(sortTable, otherPolylines);

     if (es == Acad::eOk)
         es = moveTop(sortTable, topHatches);

     if (es == Acad::eOk)
         es = moveBottom(sortTable, images);

     if (es == Acad::eOk)
         es = moveTop(sortTable, allBlocks);

     if (es == Acad::eOk)
         es = moveTop(sortTable, topBlocks);

     if (es == Acad::eOk)
         es = moveTop(sortTable, texts);

     if (es == Acad::eOk)
         es = moveTop(sortTable, topPolylines);

     if (es == Acad::eOk)
         es = moveBottom(sortTable, otherHatches);

     // Изображения снова опускаются после штриховок,
     // чтобы гарантированно оказаться в самом низу.
     if (es == Acad::eOk)
         es = moveBottom(sortTable, images);

     const Acad::ErrorStatus closeStatus =
         sortTable->close();

     if (es != Acad::eOk) {
         throw cad::core::Exception(
             cad::core::ErrorType::AcadError,
             L"FrSystemSort: changing draw order",
             es
         );
     }

     if (closeStatus != Acad::eOk) {
         throw cad::core::Exception(
             cad::core::ErrorType::AcadError,
             L"FrSystemSort: close sort table",
             closeStatus
         );
     }

     cad::out::println(
         L"FrSystemSort: порядок отрисовки установлен."
     );
 }
