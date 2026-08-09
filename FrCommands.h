#pragma once
#include "FrExceptions.h"
#include "FrOutput.h"

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
	catch (const std::exception& exs) {
		FrAlert(L"Стандартная ошибка");
		FrPrint(exs.what());
	}
	catch (...) {
		FrAlert(L"Неизвестная ошибка");
	}
}