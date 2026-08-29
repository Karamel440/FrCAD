#pragma once
#include <FrEntity.h>

class FrText : public FrEntity {

public:
	FrText()
		:FrEntity(std::make_unique<AcDbText>()) {
	}

	explicit FrText(const std::wstring& content)
		:FrEntity(
			std::make_unique<AcDbText>(
				AcGePoint3d::kOrigin,
				content.c_str()
			)
		)
	{
	}

	explicit FrText(AcGePoint3d pos)
		: FrEntity(
			std::make_unique<AcDbText>(
				pos,
				L""
			)
		)
	{
	}

	FrText(const std::wstring& content, AcGePoint3d pos)
		:FrEntity(
			std::make_unique<AcDbText>(
				pos, content.c_str()
			)
		)
	{
	}

	void setContent(const std::wstring& text);
	void setPosition(AcGePoint3d pos);
	void setRotation(double degrees);

	std::wstring content() const;
	AcGePoint3d position() const;
	double rotation() const;
};