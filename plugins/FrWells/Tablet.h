#pragma once
#include <optional>

#include <aced.h>

class Tablet {

	int nomenclature{};
	AcGePoint3d LL{};

	void calculate(const AcGePoint3d& pt) noexcept;

public:
	explicit Tablet(const AcGePoint3d& pt) {
		calculate(pt);
	}

	std::optional<int> getCell(const AcGePoint3d& point) const;

	int getNomenclature() const noexcept;

	const AcGePoint3d getCorner() const;

	bool contains(const AcGePoint3d& point) const noexcept;
};
