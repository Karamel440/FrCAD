#include <numbers>

#include <dbobjptr.h>

#include "FrText.h"

void FrText::setContent(const std::wstring& text) {
	
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		
		auto ent = AcDbText::cast(entity());
		
		FrCheckStatus(
			ent->setTextString(text.c_str()),
			L"Не удалось установить текст текста"
		);
		return;
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForWrite
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	FrCheckStatus(
		ent->setTextString(text.c_str()),
		L"Не удалось установить текст текста"
	);
}

void FrText::setPosition(AcGePoint3d pos) {

	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {

		auto ent = AcDbText::cast(entity());

		FrCheckStatus(
			ent->setPosition(pos),
			L"Не удалось установить позицию текста"
		);
		return;
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForWrite
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	FrCheckStatus(
		ent->setPosition(pos),
		L"Не удалось установить позицию текста"
	);
}

void FrText::setRotation(double degrees) {

	const double radians = degrees * std::numbers::pi / 180.0;

	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {

		auto ent = AcDbText::cast(entity());

		FrCheckStatus(
			ent->setRotation(radians),
			L"Не удалось установить поворот текста"
		);
		return;
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForWrite
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	FrCheckStatus(
		ent->setRotation(radians),
		L"Не удалось установить поворот текста"
	);
}

void FrText::setHeight(double units) {
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {

		auto ent = AcDbText::cast(entity());

		FrCheckStatus(
			ent->setHeight(units),
			L"Не удалось установить высоту текста"
		);
		return;
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForWrite
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	FrCheckStatus(
		ent->setHeight(units),
		L"Не удалось установить высоту текста"
	);

}


std::wstring FrText::content() const
{
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		auto ent = AcDbText::cast(entity());

		return ent->textString();
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForRead
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	return ent->textString();
}

AcGePoint3d FrText::position() const
{
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		auto ent = AcDbText::cast(entity());

		return ent->position();
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForRead
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	return ent->position();
}

double FrText::rotation() const {
	
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		auto ent = AcDbText::cast(entity());

		return ent->rotation() * 180.0 / std::numbers::pi;
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForRead
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	return ent->rotation() * 180.0 / std::numbers::pi;
}