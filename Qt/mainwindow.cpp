#include <QtWidgets>
#include "mainwindow.h"
#include "qappinfo.h"
#include "CBP2MAKEFILE.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QSound>

QString mediadir = "./Resource/";

static MainWindow * gMainWindow = Q_NULLPTR;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    splitterValue = 0;

    mdiArea=new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setTabShape(QTabWidget::Rounded);
    mdiArea->setViewMode(QMdiArea::TabbedView);
    mdiArea->setTabsClosable(true);

    splitter = new QSplitter(mdiArea);

    ui->stackedWidget = new QStackedWidget(this);
    ui->stackedWidget->addWidget(mdiArea);
    ui->stackedWidget->addWidget(splitter);

 //   ui->stackedWidget->setCurrentIndex(1);
   // ui->tab_3->focusWidget(ui->stackedWidget);
    setCentralWidget(ui->stackedWidget);

    getStyleList();

    createActions();
    createDockWindows();
    createStatusBar();
    updateMenus();

    connect(mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenus);
    connect(fileListWidget, &QListWidget::itemClicked, this, &MainWindow::fileListWidgetItemClicked);

    readSettings();

    setWindowTitle(tr("ProIDE 1.0"));
    setUnifiedTitleAndToolBarOnMac(true);

    ui->stackedWidget->setCurrentWidget(mdiArea);

    gMainWindow = this;

  //  mdiArea->viewport()->installEventFilter(this);
    QPixmap oPixmap(32,32);
    oPixmap.load ( mediadir + "smoking.png");

    QIcon oIcon( oPixmap );

    trayIcon = new QSystemTrayIcon(oIcon);

    QAction *quit_action = new QAction( "Exit", trayIcon );
    connect( quit_action, SIGNAL(triggered()), this, SLOT(on_exit()) );

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction( quit_action );

    trayIcon->setContextMenu( trayIconMenu);
    trayIcon->setVisible(true);
    //trayIcon->showMessage("Test Message", "Text", QSystemTrayIcon::Information, 1000);
    //trayIcon->show();


    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));



    e.show();

    //QSound::play( mediadir + "phone.wav");

  //  CBP2MAKE("./test.cbp");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMessage()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon();
    trayIcon->showMessage(tr("QSatisfy"), tr("Will you smoke now..."), icon, 100);
}
void MainWindow::on_exit()
{
    this->close();
    QApplication::quit();
}

MainWindow * MainWindow::instance()
{
    return gMainWindow;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
 { //https://www.qtcentre.org/threads/15713-Displaying-text-on-a-MDI-area-background
//     if (obj == mdiArea->viewport()) {
//         if (event->type() == QEvent::Paint) {
//             //loggerSingleton::Instance()->addDebugString("Found paint event for mdiArea...",0);
//            // this->updateMessagesAndLogs();
//             //QPaintEvent *paintEvent = static_cast<QPaintEvent*>(event);
//             QPainter painter(mdiArea);
//             painter.setPen(Qt::blue);
//             painter.setFont(QFont("Arial", 30));
//             painter.drawText(10,10,"Test 123...");

//             QRectF rectangle(10.0, 20.0, 80.0, 60.0);
//             int startAngle = 30 * 16;
//             int spanAngle = 120 * 16;
//             painter.drawPie(rectangle, startAngle, spanAngle);
//             painter.end();
//             return true;
//        }
//    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newFile()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

bool MainWindow::openFile(const QString &fileName)
{
    if (QMdiSubWindow *existing = findMdiChild(fileName)) {
        mdiArea->setActiveSubWindow(existing);
        return true;
    }

    const bool succeeded = loadFile(fileName);
    if (succeeded)
        statusBar()->showMessage(tr("File loaded"), 2000);
    return succeeded;
}

bool MainWindow::loadFile(const QString &fileName)
{
    MdiChild *child = createMdiChild();
    const bool succeeded = child->loadFile(fileName);
    if (succeeded)
    {
        if (!isExistListWidget(fileName))
        {
            const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/Resource/open.png"));
            QListWidgetItem * pItem = new QListWidgetItem(openIcon, child->strippedName(fileName), fileListWidget);
            pItem->setData(Qt::UserRole, fileName);
            fileListWidget->addItem(pItem);
        }

        auto highlighter = new Highlighter(child->document());

        child->show();
    }
    else
        child->close();
    MainWindow::prependToRecentFiles(fileName);
    return succeeded;
}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings &settings)
{
    QStringList result;
    const int count = settings.beginReadArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        result.append(settings.value(fileKey()).toString());
    }
    settings.endArray();
    return result;
}

static void writeRecentFiles(const QStringList &files, QSettings &settings)
{
    const int count = files.size();
    settings.beginWriteArray(recentFilesKey());
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        settings.setValue(fileKey(), files.at(i));
    }
    settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const int count = settings.beginReadArray(recentFilesKey());
    settings.endArray();
    return count > 0;
}

void MainWindow::prependToRecentFiles(const QString &fileName)
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList oldRecentFiles = readRecentFiles(settings);
    QStringList recentFiles = oldRecentFiles;
    recentFiles.removeAll(fileName);
    recentFiles.prepend(fileName);
    if (oldRecentFiles != recentFiles)
        writeRecentFiles(recentFiles, settings);

    setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
    recentFileSubMenuAct->setVisible(visible);
    recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

    const QStringList recentFiles = readRecentFiles(settings);
    const int count = qMin(int(MaxRecentFiles), recentFiles.size());
    int i = 0;
    for ( ; i < count; ++i) {
        const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
        recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
        recentFileActs[i]->setData(recentFiles.at(i));
        recentFileActs[i]->setVisible(true);
    }
    for ( ; i < MaxRecentFiles; ++i)
        recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
    if (const QAction *action = qobject_cast<const QAction *>(sender()))
        openFile(action->data().toString());
}

void MainWindow::save()
{
    if (activeMdiChild() && activeMdiChild()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    MdiChild *child = activeMdiChild();
    if (child && child->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        MainWindow::prependToRecentFiles(child->currentFile());
    }
}

#ifndef QT_NO_CLIPBOARD
void MainWindow::cut()
{
    if (activeMdiChild())
        activeMdiChild()->cut();
}

void MainWindow::copy()
{
    if (activeMdiChild())
        activeMdiChild()->copy();
}

void MainWindow::paste()
{
    if (activeMdiChild())
        activeMdiChild()->paste();
}
#endif

void MainWindow::about()
{
    QMessageBox::about(this, tr("About ProIDE"), tr("Application: <b>ProIDE</b> Qt Application <br>Version: <b>1.0</b>"));
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != nullptr);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
#ifndef QT_NO_CLIPBOARD
    pasteAct->setEnabled(hasMdiChild);
#endif
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    windowMenuSeparatorAct->setVisible(hasMdiChild);

#ifndef QT_NO_CLIPBOARD
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
#endif
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(windowMenuSeparatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    windowMenuSeparatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        QMdiSubWindow *mdiSubWindow = windows.at(i);
        MdiChild *child = qobject_cast<MdiChild *>(mdiSubWindow->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action = windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
            mdiArea->setActiveSubWindow(mdiSubWindow);
        });
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
    }
}

MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    child->setWindowFlag(Qt::WindowCloseButtonHint);
    child->setWindowIcon(QIcon(":/Resource/new.png"));
    QMdiSubWindow * window = mdiArea->addSubWindow(child, Qt::WindowCloseButtonHint);

#ifndef QT_NO_CLIPBOARD
    connect(child, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(child, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif

    return child;
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/Resource/new.png"));
    newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/Resource/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/Resource/save.png"));
    saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
    fileMenu->addAction(saveAsAct);

    fileMenu->addSeparator();

    QMenu *recentMenu = fileMenu->addMenu(tr("Recent..."));
    connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
    recentFileSubMenuAct = recentMenu->menuAction();

    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
        recentFileActs[i]->setVisible(false);
    }

    recentFileSeparator = fileMenu->addSeparator();

    setRecentFilesVisible(MainWindow::hasRecentFiles());

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), qApp, &QApplication::closeAllWindows);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAct);

#ifndef QT_NO_CLIPBOARD
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/Resource/cut.png"));
    cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/Resource/copy.png"));
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/Resource/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);
#endif

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    int stylecount = styleNameList.count();
    if (stylecount > 0)
    {
        QMenu *themeMenu = viewMenu->addMenu(tr("&Theme     "));

        int defaultTheme = 0;
        themeGroup = new QActionGroup(this);

        for (int i = 0; i < stylecount; i ++)
        {
            QString stylename = styleNameList.at(i);
            QAction * themeAct = themeMenu->addAction(stylename, this, &MainWindow::applyTheme);
            themeAct->setCheckable(true);
            QString tip = QString("Apply %1 theme for application.").arg(stylename);
            themeAct->setStatusTip(tip);
            if (stylename == "UBUNTU")
            {
                themeAct->setChecked(true);
                defaultTheme = i;
            }

            themeGroup->addAction(themeAct);
        }

        applyTheme(defaultTheme);
    }

    QMenu *splitMenu = viewMenu->addMenu(tr("&Split Windows     "));
    {
        QAction * splitVertAct = splitMenu->addAction(tr("Split Vertical "), this, &MainWindow::splitWindow);
        splitVertAct->setCheckable(true);

        QAction * splitHorzAct = splitMenu->addAction(tr("Split Horizental "), this, &MainWindow::splitWindow);
        splitHorzAct->setCheckable(true);

        QAction * splitDelAct = splitMenu->addAction(tr("Split None "), this, &MainWindow::splitWindow);
        splitDelAct->setCheckable(true);
        splitDelAct->setChecked(true);

        splitGroup = new QActionGroup(this);
        splitGroup->addAction(splitVertAct);
        splitGroup->addAction(splitHorzAct);
        splitGroup->addAction(splitDelAct);
    }

    windowMenu = menuBar()->addMenu(tr("&Window"));
    connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

    closeAct = new QAction(tr("Cl&ose"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, &QAction::triggered,
            mdiArea, &QMdiArea::closeActiveSubWindow);

    closeAllAct = new QAction(tr("Close &All"), this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);

    windowMenuSeparatorAct = new QAction(this);
    windowMenuSeparatorAct->setSeparator(true);

    updateWindowMenu();

    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry();
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

MdiChild *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return nullptr;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return nullptr;
}

void MainWindow::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("Files"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    fileListWidget = new QListWidget(dock);
    dock->setWidget(fileListWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    dock = new QDockWidget(tr("Highlight"), this);
    hlWidget = new QHlWidget(dock);
    dock->setWidget(hlWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void MainWindow::getStyleList()
{
    stylePathList.clear();
    styleNameList.clear();

    QString strDir = QAppInfo::getAppDirectory();
    strDir += "/Resource/themes/";

    QDirIterator it(strDir, QStringList() << "*.qss", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        QString filepath = it.next().toLatin1();
        QFileInfo fileInfo(filepath);
        styleNameList.push_back(fileInfo.baseName().toUpper());
        stylePathList.push_back(filepath);
    }
}

void MainWindow::loadStyleSheet( QString sheet_name)
{
    QFile file(sheet_name);
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
}

void MainWindow::applyTheme(int index)
{
    if (themeGroup != Q_NULLPTR)
    {
        QString str = themeGroup->checkedAction()->text();
        for (int i = 0; i < styleNameList.count(); i ++)
        {
            QString tmp = styleNameList.at(i);
            if (!str.compare(tmp))
            {
                QString filepath = stylePathList.at(i);
                loadStyleSheet(filepath);
            }
        }
    }
}

bool MainWindow::isExistListWidget(const QString& filename)
{
    bool bOpened = false;

    QString name = QFileInfo(filename).fileName();

    int count = fileListWidget->count();
    for (int i = 0; i < count; i ++)
    {
        QString listItemName = fileListWidget->item(i)->text();
        if (listItemName == name)
        {
            fileListWidget->setCurrentRow(i);
            bOpened = true;
            break;
        }
    }

    return bOpened;
}

void MainWindow::fileListWidgetItemClicked(QListWidgetItem * item)
{
    if (item == Q_NULLPTR)
        return;

    QString filepath = item->data(Qt::UserRole).toString();
    if (filepath == "")
        return;

    QString fileName = item->text();

    if (QMdiSubWindow *existing = findMdiChild(fileName)) {
        mdiArea->setActiveSubWindow(existing);
        return;
    }

    openFile(filepath);
}

QTextEdit * MainWindow::getActiveTextEdit()
{
    QMdiSubWindow *existing = mdiArea->currentSubWindow();

    static QTextEdit * srcEditor;

    if (existing == Q_NULLPTR)
        return Q_NULLPTR;

    srcEditor = qobject_cast<QTextEdit*>(existing->widget());
    srcEditor->show();

    return srcEditor;
}


void MainWindow::splitWindow(int index)
{
    splitterValue = 0;

    if (splitGroup != Q_NULLPTR)
    {
        QString str = splitGroup->checkedAction()->text();

        if (str.indexOf(tr("Vertical")) != -1)
            splitterValue = 1;

        if (str.indexOf(tr("Horizental")) != -1)
            splitterValue = 2;
    }

    switchSplitter(splitterValue);
}

void MainWindow::switchSplitter(int type)
{
    QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();
    if (subWindows.isEmpty())
    {
        int count = splitter->count();
        if (count == 0)
            return;

        for (int i = 0; i < count; i ++)
        {
             MdiChild *child = (MdiChild *)splitter->widget(i);
             mdiArea->addSubWindow(child, Qt::WindowCloseButtonHint);
        }

        subWindows = mdiArea->subWindowList();
    }

    if (type > 0)
    {
        foreach (QMdiSubWindow *window, subWindows)
        {
            splitter->addWidget(window);
        }

        splitter->show();

        if (type == 2)
        {
            splitter->setOrientation(Qt::Horizontal);
        }
        else
        {
            splitter->setOrientation(Qt::Vertical);
        }

        ui->stackedWidget->setCurrentWidget(splitter);
    }
    else
    {
        foreach (QMdiSubWindow *window, subWindows)
            window->showMaximized();

        ui->stackedWidget->setCurrentWidget(mdiArea);
    }
}

void MainWindow::on_actioncompile_triggered()
{
    QString builddir="./test";
    system("make -f "+builddir.toLatin1()+"makefile");
}
