#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(MyIDE);

    QApplication app(argc, argv);

    QApplication::setOrganizationName("MyIDE");
    QApplication::setApplicationName("MyIDE");
    QApplication::setApplicationVersion("1.0");

    MainWindow mainWin;
    mainWin.show();

    return app.exec();
}

