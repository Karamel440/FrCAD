#pragma once
#include <filesystem>
#include "Main.h"

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>


void FrWellsWind();

class FrWellsWindow: public QDialog {

public:
	FrWellsWindow( QWidget* parent = nullptr )
		:QDialog( parent ) {

		setFixedSize( 300, 200 );
		setWindowTitle( QStringLiteral( "Анализ колодцев" ) );

		auto*  analizeButton = new QPushButton (QStringLiteral( "Запустить анализ" ), this);

		auto* excelButton = new QPushButton (QStringLiteral( "Перенести в эксель" ), this);

		analizeButton->setFixedSize( 180, 36 );
		excelButton->setFixedSize( 180, 36 );

		analizeButton->setStyleSheet(
			"background-color: #3478C5;"
			"color: white;"
			"border: none;"
			"border-radius: 5px;"
		);

		excelButton->setStyleSheet(
			"background-color: #0b9300;"
			"color: white;"
			"border: none;"
			"border-radius: 5px;"
		);

		auto* mainLayout = new QVBoxLayout( this );

		mainLayout->addWidget( analizeButton );
		mainLayout->addWidget( excelButton );
		mainLayout->setSpacing( 8 );
		mainLayout->setContentsMargins( 30, 30, 30, 30 );

		QObject::connect(
			analizeButton, &QPushButton::clicked,
			this, &QDialog::accept
		);

		QObject::connect(
			excelButton, &QPushButton::clicked,
			this, [this] {
				FrWellsExcel();
				FrWellsClear();

				reject();
			}
		);
	}
};