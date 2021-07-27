#include <QtWidgets>
#include "mainwindow.h"
#include "qappinfo.h"

static MainWindow * gMainWindow = Q_NULLPTR;

MainWindow::MainWindow()
{
    tabSrcWidget = new MyTabWidget(this);
    setCentralWidget(tabSrcWidget);
    tabSrcWidget->setContentsMargins(4, 4, 4, 4);
    setAcceptDrops(true);

    getStyleList();

    createActions();
    createStatusBar();
    createDockWindows();

    readSettings();

    connect(tabSrcWidget, &MyTabWidget::openFileRequest, this, &MainWindow::openFileAt);
    connect(tabSrcWidget, &MyTabWidget::dragTabRequest,  this, &MainWindow::dragTab);

    connect(fileListWidget, &QListWidget::itemClicked, this, &MainWindow::fileListWidgetItemClicked);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest, this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);

    gMainWindow = this;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        setCurrentFile(QString());
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            openFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About"), tr("Application: <b>MyIDE Qt Application </b><br>Version: <b>1.0</b>"));
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/Resource/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
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
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit", QIcon(":/Resource/exit.png"));
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);

    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/Resource/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cutAct, &QAction::triggered, getTabTextEdit(), &QTextBrowser::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/Resource/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(copyAct, &QAction::triggered, getTabTextEdit(), &QTextBrowser::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/Resource/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(pasteAct, &QAction::triggered, getTabTextEdit(), &QTextBrowser::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();

#endif // !QT_NO_CLIPBOARD

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    int stylecount = styleNameList.count();
    if (stylecount > 0)
    {
        QMenu *themeMenu = viewMenu->addMenu(tr("&Theme"));

        int defaultTheme = 0;
        themeGroup = new QActionGroup(this);

        for (int i = 0; i < stylecount; i ++)
        {
            QString stylename = styleNameList.at(i);
            const QIcon themeIcon = QIcon::fromTheme(stylename);
            QAction * themeAct = themeMenu->addAction(themeIcon, stylename, this, &MainWindow::applyTheme);
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

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon::fromTheme("help-about", QIcon(":/Resource/about.png"));
    QAction *aboutAct = helpMenu->addAction(aboutIcon, tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(getTabTextEdit(), &QTextBrowser::copyAvailable, cutAct, &QAction::setEnabled);
    connect(getTabTextEdit(), &QTextBrowser::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QString a = QCoreApplication::organizationName();
    QString b = QCoreApplication::applicationName();

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

bool MainWindow::maybeSave()
{
    if (!getTabTextEdit()->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << getTabTextEdit()->toPlainText();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    getTabTextEdit()->document()->setModified(false);
    setWindowModified(false);

    setWindowTitle("MyIDE 1.0");
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (getTabTextEdit()->document()->isModified())
            save();
    }
}
#endif

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

void MainWindow::fileListWidgetItemClicked(QListWidgetItem * item)
{
    if (item == Q_NULLPTR)
        return;

    QString filepath = item->data(Qt::UserRole).toString();
    if (filepath == "")
        return;

    QString fileName = item->text();


    int selTab = getTabIndex(fileName);
    if (selTab != -1)
        tabSrcWidget->setCurrentIndex(selTab);
    else
        showOnlyTabPanel(filepath);
}

MainWindow * MainWindow::instance()
{
    return gMainWindow;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else event->ignore();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData * mimeData = event->mimeData();
    if(mimeData->hasUrls()) {
        for (const QUrl& url : mimeData->urls()) {
            openFile(url.toLocalFile());
        }
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }
}

void MainWindow::openFile(QString fileName)
{
    return openFileAt(fileName, -1);
}

void MainWindow::openFileAt(QString fileName, int tabIndex)
{
    QFile file(fileName);
    if(! file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,tr("Error"), tr("Cannot open file %1:\n%2").arg(fileName).arg(file.errorString()));
        return ;
    }

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);

    auto browser = new QTextBrowser(this);
    auto tabWidget = qobject_cast<MyTabWidget*>(centralWidget());
    Q_ASSERT(tabWidget);
    auto index = tabWidget->insertTab(tabIndex,browser,QFileInfo(fileName).fileName());
    tabWidget->setCurrentIndex(index);

    auto highlighter = new Highlighter(browser->document());

    browser->setAcceptDrops(false);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    browser->setSource(QUrl::fromLocalFile(fileName));
    browser->setPlainText(in.readAll());
    QGuiApplication::restoreOverrideCursor();

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/Resource/open.png"));
    QListWidgetItem * pItem = new QListWidgetItem(openIcon, strippedName(fileName), fileListWidget);
    pItem->setData(Qt::UserRole, fileName);
    fileListWidget->addItem(pItem);

    statusBar()->showMessage(tr("File loaded"), 2000);

    file.close();
}

void MainWindow::showOnlyTabPanel(QString fileName)
{
    QFile file(fileName);
    if(! file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,tr("Error"), tr("Cannot open file %1:\n%2").arg(fileName).arg(file.errorString()));
        return ;
    }

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);

    auto browser = new QTextBrowser(this);
    auto tabWidget = qobject_cast<MyTabWidget*>(centralWidget());
    Q_ASSERT(tabWidget);
    auto index = tabWidget->addTab(browser,QFileInfo(fileName).fileName());
    tabWidget->setCurrentIndex(index);

    auto highlighter = new Highlighter(browser->document());

    browser->setAcceptDrops(false);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    browser->setSource(QUrl::fromLocalFile(fileName));
    browser->setPlainText(in.readAll());
    QGuiApplication::restoreOverrideCursor();

    statusBar()->showMessage(tr("File loaded"), 2000);

    file.close();
}


void MainWindow::dragTab(int tabIndex)
{
    //if(!isTabMovable(tabIndex)) return;
    auto tabWidget = qobject_cast<MyTabWidget*>(centralWidget());
    Q_ASSERT(tabWidget);
    auto browser = qobject_cast<QTextBrowser*>(tabWidget->widget(tabIndex));
    Q_ASSERT(browser);

    auto drag = new QDrag(this);
    auto mimeData = new QMimeData;
    QPixmap thumbnail = windowHandle()->screen()->grabWindow(browser->winId());
    mimeData->setUrls({browser->source()});
    drag->setMimeData(mimeData);
    drag->setPixmap(thumbnail.scaled(200,200));

    auto dragAction = drag->exec(Qt::LinkAction);
    int currentIndex = tabWidget->indexOf(browser);
    qDebug() << "removed tab source" << tabIndex;
    qDebug() << "removed tab current" << currentIndex;
    if (dragAction==Qt::LinkAction) {
        tabWidget->removeTabActually(currentIndex);
    } else if (dragAction==Qt::IgnoreAction) {
        if(QProcess::startDetached(qApp->applicationFilePath(),
                                   {"-x",QString::number(QCursor::pos().x()),
                                    "-y",QString::number(QCursor::pos().y()),
                                   browser->source().toLocalFile()})) {
            tabWidget->removeTabActually(currentIndex);
        }
    } else {
        return;
    }
    if(tabWidget->count()==0) {
        qApp->closeAllWindows();
    }
}

QTextBrowser * MainWindow::getTabTextEdit()
{
    MyTabWidget * tabWidget = qobject_cast<MyTabWidget*>(centralWidget());

    static QTextBrowser *  srcEditor;

    int tabIndex = tabWidget->currentIndex();
    if (tabIndex == -1)
    {
        srcEditor = new QTextBrowser(this);
        srcEditor->hide();
        return srcEditor;
    }

    srcEditor = qobject_cast<QTextBrowser*>(tabWidget->widget(tabIndex));
    srcEditor->show();
    return srcEditor;
}

int MainWindow::getTabIndex(QString name)
{
    int index = -1;

    int count = tabSrcWidget->count();
    for (int i = 0; i < count; i ++)
    {
        QString tabname = tabSrcWidget->tabText(i);
        if (tabname == name)
        {
            index = i;
            break;
        }
    }


    return index;
}
