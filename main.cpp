#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindow window;
    window.resize(1680, 400);

    window.show();
    window.start();

    bool ok = app.exec(); 

    return ok;
}
