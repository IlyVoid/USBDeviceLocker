#include <QApplication>
#include "gui/MainWindow.h"
#include "./class/main.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}