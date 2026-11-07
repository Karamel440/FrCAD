#include "Under.h"
#include <filesystem>
#include <FrEntity.h>
#include <FrPolyline.h>
#include <FrDatabase.h>
#include <FrCodifier.h>
#include <FrFactory.h>
#include <FrSelection.h>

void FrUnderRainSewer() {

	FrCodifier cod{
	std::filesystem::path{ LR"(D:\Macro\FrConfig.json)" }
	};

	auto db = FrDbCurrent();
	auto factory = cod.factory(db);

	auto line = factory.createLineObject(FrObjectCode::rainSewer);
	auto firstPoint = FrGetPoint();
	auto secondPoint = FrGetPoint(firstPoint, L"\nВыберете вторую точку: ");

	line.addVertex(firstPoint);
	line.addVertex(secondPoint);

	db.append(line);
}

void FrUnderUnderWater() {
	FrCodifier cod{
	std::filesystem::path{ LR"(D:\Macro\FrConfig.json)" }
	};

	auto db = FrDbCurrent();
	auto factory = cod.factory(db);

	auto line = factory.createLineObject(FrObjectCode::underWater);
	auto firstPoint = FrGetPoint();
	auto secondPoint = FrGetPoint(firstPoint,L"\nВыберете вторую точку: ");

	line.addVertex(firstPoint);
	line.addVertex(secondPoint);

	db.append(line);
}