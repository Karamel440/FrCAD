#pragma once
#include "FrExceptions.h"
#include "FrOutput.h"
#include "FrString.h"

template <void (*Command)()>
void FrCommand() noexcept
{
	try {
		Command();
	}

	catch (const FrCancel&) {}

	catch (const FrException& ex) {
		FrAlert(ex.message());

		FrPrint(ex.message());

		const auto location = ex.where();

		FrPrint(location.file_name());
		FrPrint(location.function_name());

		if (ex.status()) {
			FrPrint(*ex.status());
		}
	}
	catch (const std::exception& ex) {
		FrAlert(toWstring(std::string{ ex.what() }));
	}

	catch (...) {
		FrAlert(L"Неизвестная ошибка");
	}
}
