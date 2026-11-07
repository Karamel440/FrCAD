#include "Recolor.h"
#include <filesystem>
#include <optional>
#include <vector>
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
			FrDatabase db{ file };
		}
		catch (const FrException&) {
			failed.push_back(file);
		}
	}
	return failed;
}

void FrArchiveMakeArchive() {

	auto path = FrSelectFolder(L"Выберете папку архива");

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
		FrAlert(message);
		return;
	}

	for (const auto& entry :
		fs::directory_iterator(selectedFolder)) {
		
		if (!entry.is_regular_file() ||
			entry.path().extension() != L".dwg")
			continue;

		if (boundaryFile) {
			FrAlert
			(
				L"В папке найдено несколько DWG-файлов границ. "
				L"Будет использован первый найденный файл."
			);
			break;
		}
			boundaryFile = entry.path();
	}
		
	FrDatabase archiveDwg;

	for (const auto& dwg : dwgsForJoin) {
		FrDatabase file{ dwg };
		auto entities = file.entities<FrEntity>();
		if (!entities.empty())
			archiveDwg.clone(entities);
	}

	if (boundaryFile) {
		FrDatabase boundary{ *boundaryFile };
		auto lines = boundary.entities<FrPolyline>();
		if (!lines.empty()) {
			auto layer = archiveDwg.layer(L"18_Зарамочное оформление");
			if (!layer)
				layer = archiveDwg.createLayer(L"18_Зарамочное оформление");

			for (auto& line : archiveDwg.clone(lines)) {
				line.setLayer(*layer);
				line.setColor(2);
				line.setLineWeight(AcDb::kLnWt040);
				AcDbObjectPointer<AcDbEntity> entity{ line.id(), AcDb::kForWrite };
				FrCheckStatus(entity.openStatus(), L"Не удалось открыть полилинию границы");
				entity->setLinetype(L"M5_004_A");
			}
		}
	}

	archiveDwg.saveAs(selectedFolder / L"архив.dwg");
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