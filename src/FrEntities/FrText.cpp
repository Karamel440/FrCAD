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

		if (ent->horizontalMode() == AcDb::kTextLeft &&
			ent->verticalMode() == AcDb::kTextBase)
		{
			FrCheckStatus(
				ent->setPosition(pos),
				L"Не удалось установить позицию текста"
			);
		}
		else
		{
			FrCheckStatus(
				ent->setAlignmentPoint(pos),
				L"Не удалось установить позицию текста"
			);
		}
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

	if (ent->horizontalMode() == AcDb::kTextLeft &&
		ent->verticalMode() == AcDb::kTextBase)
	{
		FrCheckStatus(
			ent->setPosition(pos),
			L"Не удалось установить позицию текста"
		);
	}
	else
	{
		FrCheckStatus(
			ent->setAlignmentPoint(pos),
			L"Не удалось установить позицию текста"
		);
	}
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

void FrText::setJustify(FrJustify just) {
	
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		auto ent = AcDbText::cast(entity());

		AcGePoint3d point;

		if (ent->horizontalMode() == AcDb::kTextLeft &&
			ent->verticalMode() == AcDb::kTextBase)
		{
			point = ent->position();
		}
		else
		{
			point = ent->alignmentPoint();
		}

		switch (just)
		{
		case FrJustify::Left:
		{
			ent->setHorizontalMode(AcDb::kTextLeft);
			ent->setVerticalMode(AcDb::kTextBase);
			ent->setPosition(point);
			break;
		}
			
		case FrJustify::center:
		{
			ent->setHorizontalMode(AcDb::kTextCenter);
			ent->setVerticalMode(AcDb::kTextVertMid);
			ent->setAlignmentPoint(point);
		}
			break;
		default:
			throw FrException{
				L"Не удалось выставить выравнивание"
			};
		}
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

	AcGePoint3d point;

	if (ent->horizontalMode() == AcDb::kTextLeft &&
		ent->verticalMode() == AcDb::kTextBase)
	{
		point = ent->position();
	}
	else
	{
		point = ent->alignmentPoint();
	}


	switch (just)
	{
	case FrJustify::Left:
	{
		ent->setHorizontalMode(AcDb::kTextLeft);
		ent->setVerticalMode(AcDb::kTextBase);
		ent->setPosition(point);
		break;
	}

	case FrJustify::center:
	{
		ent->setHorizontalMode(AcDb::kTextCenter);
		ent->setVerticalMode(AcDb::kTextVertMid);
		ent->setAlignmentPoint(point);
	}
	break;
	default:
		throw FrException{
			L"Не удалось выставить выравнивание"
		};
	}
}

void FrText::setStyle(const FrStyle& style) {
	
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		auto text = AcDbText::cast(entity());
		
		FrCheckStatus(
			text->setTextStyle(style.id()),
			L"Не удалось установить стиль тексту"
		);
		return;
	}

	AcDbObjectPointer<AcDbText> text{
		id(),
		AcDb::kForWrite
	};

	FrCheckStatus(
		text.openStatus(),
		L"Не удалось открыть текст"
	);

	FrCheckStatus(
		text->setTextStyle(style.id()),
		L"Не удалось установить стиль тексту"
	);
}

void FrText::setElevation(double units) {

	auto pos = position();
	pos.z = units;

	setPosition(pos);
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

		if (ent->horizontalMode() == AcDb::kTextLeft &&
			ent->verticalMode() == AcDb::kTextBase)
		{
			return ent->position();
		}

		return ent->alignmentPoint();
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForRead
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	if (ent->horizontalMode() == AcDb::kTextLeft &&
		ent->verticalMode() == AcDb::kTextBase)
	{
		return ent->position();
	}

	return ent->alignmentPoint();
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

FrJustify FrText::justify() const {
	if (id() == AcDbObjectId::kNull && !entity())
		throw FrException{ L"Не валидный объект" };

	if (entity()) {
		auto ent = AcDbText::cast(entity());

		auto hor = ent->horizontalMode();
		auto vert = ent->verticalMode();

		if (hor == AcDb::kTextLeft &&
			vert == AcDb::kTextBase)
			return FrJustify::Left;

		else if (hor == AcDb::kTextCenter &&
			vert == AcDb::kTextVertMid)
			return FrJustify::center;

		throw FrException{
			L"Неизвестное выравнивание"
		};
	}

	AcDbObjectPointer<AcDbText> ent{
		id(),
		AcDb::kForRead
	};

	FrCheckStatus(
		ent.openStatus(),
		L"Не удалось открыть текст"
	);

	auto hor = ent->horizontalMode();
	auto vert = ent->verticalMode();

	if (hor == AcDb::kTextLeft &&
		vert == AcDb::kTextBase)
		return FrJustify::Left;

	else if (hor == AcDb::kTextCenter &&
		vert == AcDb::kTextBase)
		return FrJustify::center;

	throw FrException{
			L"Неизвестное выравнивание"
	};
}
