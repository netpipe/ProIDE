QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

windows {
        # MinGW
        *-g++* {
                QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
        }
        # MSVC
        *-msvc* {
                QMAKE_CXXFLAGS += /utf-8
        }
}

SOURCES += \
    CustomCtrl/QCodeEditor.cpp \
    CustomCtrl/QLineNumberArea.cpp \
    CustomCtrl/QSourceHighlite/languagedata.cpp \
    CustomCtrl/QSourceHighlite/qsourcehighliter.cpp \
    CustomCtrl/QSourceHighlite/qsourcehighliterthemes.cpp \
    CustomCtrl/mytabbar.cpp \
    CustomCtrl/mytabwidget.cpp \
    DockCtrl/DockAreaTabBar.cpp \
    DockCtrl/DockAreaTitleBar.cpp \
    DockCtrl/DockAreaWidget.cpp \
    DockCtrl/DockComponentsFactory.cpp \
    DockCtrl/DockContainerWidget.cpp \
    DockCtrl/DockFocusController.cpp \
    DockCtrl/DockManager.cpp \
    DockCtrl/DockOverlay.cpp \
    DockCtrl/DockSplitter.cpp \
    DockCtrl/DockWidget.cpp \
    DockCtrl/DockWidgetTab.cpp \
    DockCtrl/DockingStateReader.cpp \
    DockCtrl/ElidingLabel.cpp \
    DockCtrl/FloatingDockContainer.cpp \
    DockCtrl/FloatingDragPreview.cpp \
    DockCtrl/IconProvider.cpp \
    DockCtrl/ads_globals.cpp \
    DockCtrl/dockindock.cpp \
    DockCtrl/dockindockmanager.cpp \
    DockCtrl/linux/FloatingWidgetTitleBar.cpp \
    DockCtrl/perspectiveactions.cpp \
    DockCtrl/perspectives.cpp \
    compileDock/asmparser.cpp \
    compileDock/compiler.cpp \
    highlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    qaboutform.cpp \
    qappinfo.cpp \
    qhlwidget.cpp

HEADERS += \
    CustomCtrl/QCodeEditor.h \
    CustomCtrl/QLineNumberArea.h \
    CustomCtrl/QSourceHighlite/languagedata.h \
    CustomCtrl/QSourceHighlite/qsourcehighliter.h \
    CustomCtrl/QSourceHighlite/qsourcehighliterthemes.h \
    CustomCtrl/mytabbar.h \
    CustomCtrl/mytabwidget.h \
    DockCtrl/DockAreaTabBar.h \
    DockCtrl/DockAreaTitleBar.h \
    DockCtrl/DockAreaTitleBar_p.h \
    DockCtrl/DockAreaWidget.h \
    DockCtrl/DockComponentsFactory.h \
    DockCtrl/DockContainerWidget.h \
    DockCtrl/DockFocusController.h \
    DockCtrl/DockManager.h \
    DockCtrl/DockOverlay.h \
    DockCtrl/DockSplitter.h \
    DockCtrl/DockWidget.h \
    DockCtrl/DockWidgetTab.h \
    DockCtrl/DockingStateReader.h \
    DockCtrl/ElidingLabel.h \
    DockCtrl/FloatingDockContainer.h \
    DockCtrl/FloatingDragPreview.h \
    DockCtrl/IconProvider.h \
    DockCtrl/ads_globals.h \
    DockCtrl/dockindock.h \
    DockCtrl/dockindockmanager.h \
    DockCtrl/linux/FloatingWidgetTitleBar.h \
    DockCtrl/perspectiveactions.h \
    DockCtrl/perspectives.h \
    compileDock/asmparser.h \
    compileDock/compiler.h \
    highlighter.h \
    mainwindow.h \
    qaboutform.h \
    qappinfo.h \
    qhlwidget.h

FORMS += \
    qaboutform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    MyIDE.qrc
