#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(MyIDE);

   // QApplication app(argc, argv);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();


//    QApplication::setOrganizationName("MyIDE");
//    QApplication::setApplicationName("MyIDE");
//    QApplication::setApplicationVersion("1.0");

//    MainWindow mainWin;
//    mainWin.show();

    return a.exec();
}

