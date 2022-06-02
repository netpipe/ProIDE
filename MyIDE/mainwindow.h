/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qhlwidget.h"
#include "highlighter.h"
#include "qaboutform.h"
#include "DockCtrl/DockManager.h"
#include "CustomCtrl/QCodeEditor.h"

#include <memory>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QListWidget;
class QListWidgetItem;
class QMenu;
class QTextEdit;
class QSessionManager;
QT_END_NAMESPACE

namespace QtAdsUtl
{
    class DockInDockWidget;
    class PerspectivesManager;
    class DockInDockManager;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    static MainWindow * instance();

    void openFile(QString fileName);
    void openFileAt(QString fileName, int tabIndex);

    QTextEdit * getTabTextEdit();

protected:
    bool isExistListWidget(QString filename);

    int  getFileListItemIndex(QString filename);

    QString getActivateFilePath();

    void closeEvent(QCloseEvent *event)         override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event)           override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void saveState();
    void restoreState();
    void about();
    void applyTheme(int index);
    void fileListWidgetItemClicked(QListWidgetItem * item);
    void onBuildCompile();
    void updateMenus();

#ifndef QT_NO_CLIPBOARD
    void cut();
    void copy();
    void paste();
#endif

#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    void createActions();
    void createDockWindows();
    void createStatusBar();
    void switchSplitter(int type);
    void getStyleList();
    void readSettings();
    void writeSettings();
    void loadStyleSheet( QString sheet_name);
    void setCurrentFile(const QString &fileName);

    bool maybeSave();
    bool saveFile(const QString &fileName);

    int  getNewFileCount();
    int  getTabIndex(QString name);

    QString strippedName(const QString &fullFileName);

    void addWelcomeTab();
    void addNewFileTab(const QString &fileName, const QString &fName);
    void saveDockingState(const QString &stateFileName);
    void restoreDockingState(const QString &stateFileName);

private:
    QAction *       copyAct;
    QAction *       cutAct;
    QAction *       pasteAct;
    QActionGroup *  splitGroup;
    QActionGroup *  themeGroup;
    QListWidget *   fileListWidget;
    QHlWidget *     hlWidget;
    QCodeEditor *   compiledEdiorWidget;

    QAboutForm *    aboutForm;

    QtAdsUtl::DockInDockWidget* dockManager;
    //std::unique_ptr<QtAdsUtl::PerspectivesManager> m_perspectivesManager;

    QString         curFile;
    QStringList     stylePathList;
    QStringList     styleNameList;
};


#endif
