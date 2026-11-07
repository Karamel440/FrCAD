#include "Recolor.h"
#include <FrDatabase.h>
#include <FrEntity.h>
#include <FrPolyline.h>
#include <FrSpace.h>
#include <FrUtils.h>
#include <FrOutput.h>
#include <FrSelection.h>
#include <FrText.h>
#include <FrBlockReference.h>

namespace  fs = std::filesystem;

std::vector<fs::path> checkFiles(const std::vector<fs::path>& files) {
	
	std::vector<fs::path> failed;
	for (const auto& file : files) {
		
		try {
			auto db = cad::db::read(file.c_str());
		}
		catch (cad::core::Exception&) {
			failed.push_back(file);
		}
	}
	return failed;
}

CAD_COMMAND(FrRecolorArchive) {
	namespace fs = std::filesystem;

	fs::path archiveFolder = cad::ed::folder();
	std::vector<fs::path> dwgForRecolor;

	for (const auto& entry : fs::recursive_directory_iterator(archiveFolder)) {
		
		if (!entry.is_regular_file() 
			|| entry.path().extension() != L".dwg"
			|| entry.path().stem().wstring().length() != 8) 
			continue;

		dwgForRecolor.push_back(entry.path());
	}

	std::vector<fs::path>failedDwg;

	for (const auto& dwg : dwgForRecolor) {
		try {
			auto newDb = cad::db::read(dwg.c_str());
			
			cad::db::recolor(255, newDb.get());

			fs::path recoloredName = archiveFolder / (dwg.stem().wstring() + L" белый" + dwg.extension().wstring());
			cad::db::saveAs(newDb, recoloredName);
		}
		catch (const cad::core::Exception&) {
			failedDwg.push_back(dwg);
		}
		
		if (!failedDwg.empty()) {
			std::wstring message = L"Файлы,которые не удалось покрасить:\n\n";

			for (const auto& dwg : failedDwg) {
				message += dwg.filename().wstring();
				message += L"\n";
			}
			cad::out::alert(message);
		}

	}
}

CAD_COMMAND(FrArchiveMakeArchive) {

	auto path = cad::ed::folder(L"Выберете папку архива");

	fs::path selectedFolder{ path };

	std::optional<fs::path> boundaryFile;
	std::vector<fs::path> dwgsForJoin;

	for (const fs::directory_entry& entry :
		fs::recursive_directory_iterator(selectedFolder)) {
		if (!entry.is_regular_file() ||
			entry.path().extension() != L".dwg" ||
			entry.path().stem().wstring().size() != 8 ||
			entry.path().stem().wstring().find_first_not_of(L"1234567890") != std::wstring::npos)
			continue;

		dwgsForJoin.push_back(entry.path());
	}

	std::vector<fs::path> failedFiles = checkFiles(dwgsForJoin);
	
	std::wstring message = L"Не удалось обработать планшеты под номерами:\n";

	if (!failedFiles.empty()) {
		for (const auto& file : failedFiles) {
			message += L"\n";
			message += file.filename().stem().wstring();
		}
		cad::out::alert(message);
		return;
	}

	for (const auto& entry :
		fs::directory_iterator(selectedFolder)) {
		
		if (!entry.is_regular_file() ||
			entry.path().extension() != L".dwg")
			continue;

		if (boundaryFile) {
			cad::out::alert
			(
				L"В папке найдено несколько DWG-файлов границ. "
				L"Будет использован первый найденный файл."
			);
			break;
		}
			boundaryFile = entry.path();
	}
		
	auto archiveDwg = std::make_unique<AcDbDatabase> (true,true);

	for (auto& dwg : dwgsForJoin) {;
	
	auto tempDwg = cad::db::create();
	
	auto file = cad::db::read(dwg.c_str());
	
	auto ids = cad::db::entities<AcDbObject>(file);

	cad::ops::clone(ids, archiveDwg.get(),file.get());
	}

	if (boundaryFile) {
		auto temp = cad::db::create();

		auto tempFile = cad::db::read(boundaryFile.value().c_str());

		auto ids = cad::db::entities<AcDbPolyline>(temp);

		auto plineIds = cad::ops::clone(ids, archiveDwg.get());

		auto layerId = cad::db::layer(temp, L"18_Зарамочное оформление");

		cad::ops::setLayer(ids, layerId);

		for (const auto& id : plineIds) {
			auto pline = cad::db::open<AcDbPolyline>(id, AcDb::kForWrite);
			pline->setColorIndex(2);
			pline->setLinetype(L"M5_004_A");
			pline->setLineWeight(AcDb::kLnWt040);
		}
	}

	fs::path outputPath =
		selectedFolder / L"архив.dwg";

	cad::db::saveAs(archiveDwg.get(), outputPath);
}

void FrArchiveMake() {
	fs::path dict = FrSelectFolder(L"Выберете папку архива");

	FrDatabase arch;
	FrDatabase boundary;

	for (auto& entry : fs::directory_iterator{ dict }) {
		if (!entry.is_regular_file() ||
			entry.path().extension() != ".dwg" ||
			entry.path().stem() == L"Архив")
			continue;

		FrDatabase temp{ entry.path() };
		auto lines = boundary.clone(temp.entities<FrPolyline>());

		auto layer = boundary.layer(L"18_Зарамочное оформление");
		
		if (!layer)
			layer = boundary.createLayer(L"18_Зарамочное оформление");

		for (auto& line : lines) {
			line.setColor(2);
			line.setLayer(*layer);
			line.setLineWeight(AcDb::kLnWt040);
		}
	}

	for (auto& entry : fs::recursive_directory_iterator{ dict }) {
		if (!entry.is_regular_file() ||
			entry.path().extension() != L".dwg" ||
			entry.path().stem().wstring().size() != 8)
			continue;

		FrDatabase temp{ entry.path() };
		arch.clone(temp.entities<FrEntity>());
	}
	arch.clone(boundary.entities<FrPolyline>());
	arch.saveAs(dict / L"Архив.dwg");
}

void FrTest() {
	auto point = FrGetPoint(L"\nВведите первую вершину полилинии: ");
	auto second = FrGetPoint(point, L"\nВведите следующую вершину полилинии: ");

	FrPolyline line;
	
	line.addVertex(point);
	line.addVertex(second);
	line.setLineWeight(AcDb::kLnWt040);

	auto db = FrDbCurrent();

	auto space = db.createSpace(L"Test");
	space.append(line);

	for (auto& ent : space.entities<FrEntity>()) {
		ent.setColor(1);
	}

	FrBlockReference block{space};
	
	db.append(block);
}

void FrArchiveRecolorDwg() {
	
	auto number = FrGetInt(L"Введите индекс цвета\n");

	if (number < 0 ||
		number > 255)
		throw FrException{
		L"введите корректный индекс!"
	};

	auto db = FrDbCurrent();

	for (auto& space : db.spaces()) {
		for (auto& ent : space.entities<FrEntity>()) {
			ent.setColor(number);
		}
	}
}

void FrArchiveRecolor() {
	auto archiveDir = FrSelectFolder(L"Выберете папку архива");

	for (auto& entry : fs::recursive_directory_iterator{ archiveDir }) {
		if (!entry.is_regular_file() ||
			entry.path().stem().wstring().size() != 8 ||
			entry.path().stem().string().find_first_not_of("1234567890") != std::string::npos ||
			entry.path().extension() != L".dwg" &&
			entry.path().extension() != L".DWG")
			continue;

		FrDatabase file{ entry.path() };
		for (auto& space : file.spaces()) {
			for (auto& ent : space.entities<FrEntity>()) {
				ent.setColor(255);
			}
		}
		
		fs::path fileName = 
			entry.path().stem().wstring() + 
			L" белый" + 
			entry.path().extension().wstring();

		file.saveAs(archiveDir / fileName );
	}
}