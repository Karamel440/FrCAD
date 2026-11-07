#include "system.h"

#include <FrExceptions.h>
#include <FrOutput.h>
#include <acutmem.h>
#include <adscodes.h>
#include <dbapserv.h>
#include <dbents.h>
#include <dbobjptr.h>
#include <sorttab.h>
#include <cwchar>
#include <memory>

namespace {
    resbuf systemValue(int value) {
        resbuf result{};
        result.restype = RTSHORT;
        result.resval.rint = static_cast<short>(value);
        return result;
    }

    resbuf systemValue(double value) {
        resbuf result{};
        result.restype = RTREAL;
        result.resval.rreal = value;
        return result;
    }

    resbuf systemValue(const ACHAR* value) {
        resbuf result{};
        result.restype = RTSTR;
        result.resval.rstring = const_cast<ACHAR*>(value);
        return result;
    }

    void setSystemVar(const ACHAR* name, const resbuf& value) {
        if (acedSetVar(name, &value) != RTNORM)
            throw FrException{ L"Не удалось установить " + std::wstring{ name } };
    }

    template <typename T>
    void setSystemVar(const ACHAR* name, T value) {
        setSystemVar(name, systemValue(value));
    }

    class ScopedSystemVar {
        const ACHAR* name_;
        resbuf previous_{};

    public:
        ScopedSystemVar(const ACHAR* name, const resbuf& value) : name_(name) {
            if (acedGetVar(name_, &previous_) != RTNORM)
                throw FrException{ L"Не удалось прочитать " + std::wstring{ name_ } };

            if (acedSetVar(name_, &value) != RTNORM) {
                if (previous_.restype == RTSTR)
                    acutDelString(previous_.resval.rstring);
                throw FrException{ L"Не удалось установить " + std::wstring{ name_ } };
            }
        }

        ~ScopedSystemVar() {
            acedSetVar(name_, &previous_);
            if (previous_.restype == RTSTR)
                acutDelString(previous_.resval.rstring);
        }

        ScopedSystemVar(const ScopedSystemVar&) = delete;
        ScopedSystemVar& operator=(const ScopedSystemVar&) = delete;
    };

    AcDbDatabase& currentDatabase() {
        auto* db = acdbHostApplicationServices()->workingDatabase();
        if (!db)
            throw FrException{ L"Нет активного чертежа" };
        return *db;
    }

    void collectRecords(AcDbObjectId tableId, AcDbObjectIdArray& ids, bool isBlockTable = false) {
        AcDbObjectPointer<AcDbSymbolTable> table{ tableId, AcDb::kForRead };
        FrCheckStatus(table.openStatus(), L"Не удалось открыть таблицу чертежа");

        AcDbSymbolTableIterator* raw = nullptr;
        const auto status = table->newIterator(raw);
        std::unique_ptr<AcDbSymbolTableIterator> records{ raw };
        FrCheckStatus(status, L"Не удалось получить записи таблицы");

        for (; !records->done(); records->step()) {
            AcDbObjectId id;
            if (records->getRecordId(id) != Acad::eOk || id.isNull())
                continue;
            if (isBlockTable) {
                AcDbObjectPointer<AcDbBlockTableRecord> block{ id, AcDb::kForRead };
                if (block.openStatus() != Acad::eOk || block->isLayout() ||
                    block->isFromExternalReference() || block->isDependent())
                    continue;
            }
            ids.append(id);
        }
    }
}


void setFrStandardVars()
{
    setSystemVar(L"INSUNITS", 6);        // meters

    setSystemVar(L"CMDECHO", 0);
    setSystemVar(L"FILEDIA", 1);
    setSystemVar(L"CMDDIA", 1);
    setSystemVar(L"PROXYNOTICE", 1);

    setSystemVar(L"PICKFIRST", 1);
    setSystemVar(L"PICKADD", 2);

    setSystemVar(L"LTSCALE", 1.0);
    setSystemVar(L"CELTSCALE", 1.0);
    setSystemVar(L"PSLTSCALE", 1);
    setSystemVar(L"MSLTSCALE", 1);

    setSystemVar(L"CLAYER", L"0");
    setSystemVar(L"CELTYPE", L"Continuous");
    setSystemVar(L"CECOLOR", L"7");
    setSystemVar(L"CELWEIGHT", 20);
    setSystemVar(L"TEXTSTYLE", L"Standard");

    FrPrint(L"Стандартные системные переменные выставлены");
}

void FrSystemClearAll()
{
    ScopedSystemVar cmdecho{ L"CMDECHO", systemValue(0) };
    ScopedSystemVar filedia{ L"FILEDIA", systemValue(0) };
    ScopedSystemVar cmddia{ L"CMDDIA", systemValue(0) };
    ScopedSystemVar proxyNotice{ L"PROXYNOTICE", systemValue(0) };
    ScopedSystemVar layer{ L"CLAYER", systemValue(L"0") };
    ScopedSystemVar linetype{ L"CELTYPE", systemValue(L"Continuous") };
    ScopedSystemVar lineweight{ L"CELWEIGHT", systemValue(20) };
    ScopedSystemVar color{ L"CECOLOR", systemValue(L"7") };
    ScopedSystemVar style{ L"TEXTSTYLE", systemValue(L"Standard") };

    auto& db = currentDatabase();
    for (int pass = 0; pass < 3; ++pass) {
        AcDbObjectIdArray ids;
        collectRecords(db.blockTableId(), ids, true);
        collectRecords(db.layerTableId(), ids);
        collectRecords(db.linetypeTableId(), ids);
        collectRecords(db.textStyleTableId(), ids);
        collectRecords(db.dimStyleTableId(), ids);
        collectRecords(db.regAppTableId(), ids);

        if (ids.isEmpty())
            break;

        FrCheckStatus(db.purge(ids), L"FrSystemClearAll: не удалось очистить чертёж");
        if (ids.isEmpty())
            break;

        for (const auto& id : ids) {
            AcDbObjectPointer<AcDbObject> object{ id, AcDb::kForWrite };
            if (object.openStatus() == Acad::eOk)
                object->erase();
        }
    }

    FrPrint(L"Очистка завершена");
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

         AcDbObjectPointer<AcDbBlockTableRecord> blockRecord{
             block->blockTableRecord(), AcDb::kForRead
         };
         FrCheckStatus(blockRecord.openStatus(), L"FrSystemSort: block record");

         AcString blockName;

         const Acad::ErrorStatus es =
             blockRecord->getName(blockName);

         if (es != Acad::eOk) {
             throw FrException{ L"FrSystemSort: block name", es };
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

 void FrSystemSort() {
     auto& db = currentDatabase();
     AcDbObjectPointer<AcDbBlockTableRecord> currentSpace{
         db.currentSpaceId(), AcDb::kForRead
     };
     FrCheckStatus(currentSpace.openStatus(), L"FrSystemSort: current space");

     AcDbObjectIdArray entityIds;

     AcDbBlockTableRecordIterator* iterator = nullptr;

     Acad::ErrorStatus es =
         currentSpace->newIterator(iterator);

     if (es != Acad::eOk || !iterator) {
         delete iterator;

         throw FrException{ L"FrSystemSort: current space iterator", es };
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
         AcDbObjectPointer<AcDbEntity> entity{ id, AcDb::kForRead };
         FrCheckStatus(entity.openStatus(), L"FrSystemSort: open entity");

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
         throw FrException{ L"FrSystemSort: getSortentsTable", es };
     }

     if (!sortTable) {
         throw FrException{ L"FrSystemSort: sortTable" };
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
         throw FrException{ L"FrSystemSort: changing draw order", es };
     }

     if (closeStatus != Acad::eOk) {
         throw FrException{ L"FrSystemSort: close sort table", closeStatus };
     }

     FrPrint(
         L"FrSystemSort: порядок отрисовки установлен."
     );
 }
