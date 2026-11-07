#include "Window.h"

#include <FrCommands.h>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "Main.h"

void FrWellsWind() {
    static int argc = 1;
    static char name[] = "FrWells";
    static char* argv[] = {name, nullptr};

    static QApplication application(argc, argv);

    FrWellsWindow window;

    while (window.exec() == QDialog::Accepted) {
        FrCommand<&FrWellsTest>();
        acedUpdateDisplay();
    }
}