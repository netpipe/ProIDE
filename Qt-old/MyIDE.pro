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
    plugin/QRCode/QrCode.cpp \
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
    plugin/gl/oglwidget.cpp \
    plugin/qonsole.cpp \
    plugin/quazip/JlCompress.cpp \
    plugin/quazip/qioapi.cpp \
    plugin/quazip/quaadler32.cpp \
    plugin/quazip/quacrc32.cpp \
    plugin/quazip/quagzipfile.cpp \
    plugin/quazip/quaziodevice.cpp \
    plugin/quazip/quazip.cpp \
    plugin/quazip/quazipdir.cpp \
    plugin/quazip/quazipfile.cpp \
    plugin/quazip/quazipfileinfo.cpp \
    plugin/quazip/quazipnewinfo.cpp \
    plugin/quazip/unzip.c \
    plugin/quazip/zip.c \
    plugin/zlib/adler32.c \
    plugin/zlib/compress.c \
    plugin/zlib/crc32.c \
    plugin/zlib/deflate.c \
    plugin/zlib/gzclose.c \
    plugin/zlib/gzlib.c \
    plugin/zlib/gzread.c \
    plugin/zlib/gzwrite.c \
    plugin/zlib/infback.c \
    plugin/zlib/inffast.c \
    plugin/zlib/inflate.c \
    plugin/zlib/inftrees.c \
    plugin/zlib/trees.c \
    plugin/zlib/uncompr.c \
    plugin/zlib/zutil.c \
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
    plugin/QRCode/QrCode.hpp \
    plugin/dbus/dbushandler.h \
    plugin/downloadmanager.h \
    plugin/email.h \
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
    plugin/gl/oglwidget.h \
    plugin/python/qtpython.h \
    plugin/qonsole.h \
    plugin/quazip/JlCompress.h \
    plugin/quazip/crypt.h \
    plugin/quazip/ioapi.h \
    plugin/quazip/quaadler32.h \
    plugin/quazip/quachecksum32.h \
    plugin/quazip/quacrc32.h \
    plugin/quazip/quagzipfile.h \
    plugin/quazip/quaziodevice.h \
    plugin/quazip/quazip.h \
    plugin/quazip/quazip_global.h \
    plugin/quazip/quazipdir.h \
    plugin/quazip/quazipfile.h \
    plugin/quazip/quazipfileinfo.h \
    plugin/quazip/quazipnewinfo.h \
    plugin/quazip/unzip.h \
    plugin/quazip/zip.h \
    plugin/smtp.h \
    plugin/zlib/crc32.h \
    plugin/zlib/deflate.h \
    plugin/zlib/gzguts.h \
    plugin/zlib/inffast.h \
    plugin/zlib/inffixed.h \
    plugin/zlib/inflate.h \
    plugin/zlib/inftrees.h \
    plugin/zlib/trees.h \
    plugin/zlib/zconf.h \
    plugin/zlib/zlib.h \
    plugin/zlib/zutil.h \
    qappinfo.h \
    qhlwidget.h

FORMS += \
    mainwindow.ui \
    plugin/ftp-server/debuglogdialog.ui \
    plugin/ftp-server/ftpgui.ui \
    plugin/python/mainwindow.ui \
    plugin/qonsole.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    MyIDE.qrc

DISTFILES += \
    plugin/ftp-server/deployment.pri
