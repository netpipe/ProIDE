QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS plugins ENCRYPTION2

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lxml2

SOURCES += \
    CustomCtrl/mytabbar.cpp \
    CustomCtrl/mytabwidget.cpp \
    highlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    mdichild.cpp \
    plugin/downloadmanager.cpp \
    plugin/ftp-server/dataconnection.cpp \
    plugin/ftp-server/debuglogdialog.cpp \
    plugin/ftp-server/ftpcommand.cpp \
    plugin/ftp-server/ftpcontrolconnection.cpp \
    plugin/ftp-server/ftpgui.cpp \
    plugin/ftp-server/ftplistcommand.cpp \
    plugin/ftp-server/ftpretrcommand.cpp \
    plugin/ftp-server/ftpserver.cpp \
    plugin/ftp-server/ftpstorcommand.cpp \
    plugin/ftp-server/sslserver.cpp \
    plugin/qonsole.cpp \
    qappinfo.cpp \
    qhlwidget.cpp

HEADERS += \
    CBP2MAKEFILE.h \
    CustomCtrl/mytabbar.h \
    CustomCtrl/mytabwidget.h \
    database.h \
    highlighter.h \
    mainwindow.h \
    mdichild.h \
    plugin/downloadmanager.h \
    plugin/ftp-server/CSslSocket/csslsocket.h \
    plugin/ftp-server/dataconnection.h \
    plugin/ftp-server/debuglogdialog.h \
    plugin/ftp-server/ftpcommand.h \
    plugin/ftp-server/ftpcontrolconnection.h \
    plugin/ftp-server/ftpgui.h \
    plugin/ftp-server/ftplistcommand.h \
    plugin/ftp-server/ftpretrcommand.h \
    plugin/ftp-server/ftpserver.h \
    plugin/ftp-server/ftpstorcommand.h \
    plugin/ftp-server/sslserver.h \
    plugin/qonsole.h \
    qappinfo.h \
    qhlwidget.h

FORMS += \
    mainwindow.ui \
    plugin/ftp-server/debuglogdialog.ui \
    plugin/ftp-server/ftpgui.ui \
    plugin/qonsole.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    MyIDE.qrc

DISTFILES += \
    plugin/ftp-server/deployment.pri
