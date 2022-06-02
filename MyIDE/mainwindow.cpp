#include <QtWidgets>
#include "mainwindow.h"
#include "qappinfo.h"
#include "compileDock/compiler.h"
#include "compileDock/asmparser.h"
#include "DockCtrl/dockindock.h"
#include "DockCtrl/dockindockmanager.h"
#include "DockCtrl/perspectives.h"
#include "DockCtrl/DockingStateReader.h"
#include <set>
#include <QDesktopWidget>

using namespace ads;
using namespace std;

static MainWindow * gMainWindow = Q_NULLPTR;
static CDockAreaWidget* previousDockWidget = Q_NULLPTR;
static QtAdsUtl::PerspectivesManager * m_perspectivesManager;

static const char message[] =
    "<p><b>ProIDE v1.0</b></p>"

    "<p>Qt Code::Blocks Compatible IDE - Interactive Development Environment</p>"

    "<p>first version - codename Artemus Basic loading and saving to CBP files impliment CBP2MAKE2</p> "
    "<p>As a programmer I had started out learning from blodshed devc++ and visual basic 6 using dialup mostly waiting until"
    "much later to get into linux because of hardware modem prices. thanks to codeblocks wxsmith and QT creator for linux"
    "we can enjoy the flow of an actual devleopment environment and rapid prototyping gui's. </p>"
    "<p>I like to have an IDE for every language and GUI API."
    "eventually I think a GUI editor will be implimented even if its just using GTK or WX"
    "possibly different plugins. we will have bash script support so alot of plugins can just be scripts.</p>"

    "<a href='http://www.trolltech.com'>http://cppcheck.sourceforge.net/</a> could possibly include this into the main code but gcc should be suffice";

MainWindow::MainWindow()
{
    m_perspectivesManager = new QtAdsUtl::PerspectivesManager("persist" );

    setAcceptDrops(true);

    setMinimumSize(1240, 660);
    setWindowTitle(tr("ProIDE 1.0"));

    qApp->setWindowIcon(QIcon(":/Resource/ProIDE.ico"));
    setWindowIcon(QIcon(":/Resource/ProIDE.ico"));

    dockManager = new QtAdsUtl::DockInDockWidget(this, true, m_perspectivesManager);
    setCentralWidget(dockManager);

    getStyleList();

    createActions();
    createStatusBar();
    createDockWindows();
    updateMenus();

    readSettings();        

    // Create an example editor
    addWelcomeTab();

    connect(fileListWidget, &QListWidget::itemClicked, this, &MainWindow::fileListWidgetItemClicked);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest, this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);

    aboutForm = new QAboutForm(this);

    gMainWindow = this;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    //if (maybeSave())
    {
        QString fileName = QString("%1/Noname%2").arg(QAppInfo::getAppDirectory()).arg(getNewFileCount() + 1);
        if (isExistListWidget(fileName))
            return;

        QString fName = strippedName(fileName);
        setCurrentFile(QString());
        auto browser = new QTextEdit(this);
        connect(browser, &QTextEdit::selectionChanged, this, &MainWindow::updateMenus);

        if (!isExistListWidget(fName))
        {
            const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/Resource/open.png"));
            QListWidgetItem * pItem = new QListWidgetItem(openIcon, fName, fileListWidget);
            pItem->setData(Qt::UserRole, fileName);
            fileListWidget->addItem(pItem);

            // Create an example editor
            QTextEdit* txtedit = new QTextEdit();
            txtedit->setPlaceholderText("Please enter your text here ...");

            previousDockWidget = dockManager->addTabWidget(txtedit, fName, openIcon, previousDockWidget);
        }
    }
}

void MainWindow::open()
{
    //if (maybeSave())
    {
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

void MainWindow::restoreState()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load docking state", "", "XML Files (*.xml)");
    if (!fileName.isEmpty())
        restoreDockingState(fileName);
}

void MainWindow::saveState()
{
    QFileDialog dialog(this, "Save docking state", "", "XML Files (*.xml)");
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.selectFile("ProIDEDockingState.xml");
    if (dialog.exec() == QDialog::Accepted)
        saveDockingState(dialog.selectedFiles().first());
}

void MainWindow::about()
{
    //aboutForm->show();
    QString content = tr(message);
    QMessageBox::about(this, tr("About ProIDE"), content);
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

    // Restore/save docking state main menu items

    QAction *loadStateAct = new QAction("Load State...", this);
    connect(loadStateAct, &QAction::triggered, this, &MainWindow::restoreState);
    fileMenu->addAction(loadStateAct);

    QAction *saveStateAct = new QAction("Save State...", this);
    connect(saveStateAct, &QAction::triggered, this, &MainWindow::saveState);
    fileMenu->addAction(saveStateAct);

    fileMenu->addSeparator();


    const QIcon exitIcon = QIcon::fromTheme("application-exit", QIcon(":/Resource/exit.png"));
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);

    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/Resource/cut.png"));
    cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/Resource/copy.png"));
    copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/Resource/paste.png"));
    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();

#endif // !QT_NO_CLIPBOARD

    QMenu *buildMenu = menuBar()->addMenu(tr("&Build"));
    QToolBar *buildToolBar = addToolBar(tr("Build"));

    const QIcon compileIcon = QIcon::fromTheme("edit-paste", QIcon(":/Resource/compile.png"));
    QAction *compileAct = new QAction(compileIcon, tr("&Compile"), this);
    compileAct->setStatusTip(tr("Compile current source file"));
    compileAct->setShortcut(QKeySequence(tr("F7", "Compile")));
    connect(compileAct, &QAction::triggered, this, &MainWindow::onBuildCompile);
    buildMenu->addAction(compileAct);
    buildToolBar->addAction(compileAct);

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
            //const QIcon themeIcon = QIcon::fromTheme(stylename);
            //QAction * themeAct = themeMenu->addAction(themeIcon, stylename, this, &MainWindow::applyTheme);
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

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon::fromTheme("help-about", QIcon(":/Resource/about.png"));
    QAction *aboutAct = helpMenu->addAction(aboutIcon, tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(getTabTextEdit(), &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(getTabTextEdit(), &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
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
    try {
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
    catch(...)
    {

    }

    return false;
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

#ifndef QT_NO_CLIPBOARD
void MainWindow::cut()
{
    getTabTextEdit()->cut();
}

void MainWindow::copy()
{
    getTabTextEdit()->copy();
}

void MainWindow::paste()
{
     getTabTextEdit()->paste();
}
#endif

void MainWindow::updateMenus()
{
#ifndef QT_NO_CLIPBOARD
    if (getTabTextEdit() != Q_NULLPTR)
        pasteAct->setEnabled(true);
    else
        pasteAct->setEnabled(false);
    bool hasSelection = (getTabTextEdit()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
#endif
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    getTabTextEdit()->document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(tr("ProIDE 1.0"));
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

    dock = new QDockWidget(tr("Compiled Assembly Editor"), this);
    compiledEdiorWidget = new QCodeEditor(dock);
    dock->setWidget(compiledEdiorWidget);
    addDockWidget(Qt::BottomDockWidgetArea, dock);
}

void MainWindow::fileListWidgetItemClicked(QListWidgetItem * item)
{
    if (item == Q_NULLPTR)
        return;

    QString filepath = item->data(Qt::UserRole).toString();
    if (filepath == "")
        return;

    QString fileName = item->text();

    if (getTabIndex(fileName) == -1)
        openFile(filepath);
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
    if (isExistListWidget(fileName))
        return;

    QFile file(fileName);
    if(! file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this,tr("Error"), tr("Cannot open file %1:\n%2").arg(fileName).arg(file.errorString()));
        return ;
    }

    QString fName = QFileInfo(fileName).fileName();

    setCurrentFile(fileName);

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);

    auto browser = new QTextEdit(this);
    connect(browser, &QTextEdit::selectionChanged, this, &MainWindow::updateMenus);

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QString content = in.readAll();
    QGuiApplication::restoreOverrideCursor();


    auto highlighter = new Highlighter(browser->document());

    browser->setAcceptDrops(false);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    browser->setPlainText(content);
    QGuiApplication::restoreOverrideCursor();

    if (!isExistListWidget(fName))
    {
        const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/Resource/open.png"));
        QListWidgetItem * pItem = new QListWidgetItem(openIcon, strippedName(fileName), fileListWidget);
        pItem->setData(Qt::UserRole, fileName);
        fileListWidget->addItem(pItem);

        previousDockWidget = dockManager->addTabWidget(browser, fName, openIcon, previousDockWidget);
    }

    statusBar()->showMessage(tr("File loaded"), 2000);

    file.close();
}

bool MainWindow::isExistListWidget(QString filename)
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

int MainWindow::getFileListItemIndex(QString filename)
{
    int row = -1;

    QString name = QFileInfo(filename).fileName();

    int count = fileListWidget->count();
    for (int i = 0; i < count; i ++)
    {
        QString listItemName = fileListWidget->item(i)->text();
        if (listItemName == name)
        {
            row = i;
            break;
        }
    }

    return row;
}

int MainWindow::getTabIndex(QString name)
{
    int index = -1;

    for (auto widget : dockManager->getManager()->getWidgetsInGUIOrder())
    {
        index ++;
        QString widgetname = widget->objectName();
        if (widgetname == name)
        {
            CDockWidget* dockWidget = (CDockWidget *)widget;
            dockWidget->setAsCurrentTab();
            return index;
        }
    }

    return -1;
}

int MainWindow::getNewFileCount()
{
    int index = 0;

    for (auto widget : dockManager->getManager()->getWidgetsInGUIOrder())
    {
        QString widgetname = widget->objectName();
        int found = widgetname.indexOf("Noname");
        if (found == 0)
            index ++;
    }

    return index;
}

QTextEdit * MainWindow::getTabTextEdit()
{
    static QTextEdit* srcEditor = Q_NULLPTR;

    for (auto widget : dockManager->getManager()->getWidgetsInGUIOrder())
    {
        CDockWidget* dockWidget = (CDockWidget *)widget;
        QString name = dockWidget->objectName();
        if (name != "Welcome to ProIDE")
        {
            if (dockWidget != NULL && dockWidget->isCurrentTab() && dockWidget->isTabbed())
            {
                srcEditor = (QTextEdit*)dockWidget->widget();
                srcEditor->show();
                return srcEditor;
            }
        }
    }

    srcEditor = new QTextEdit(this);
    srcEditor->hide();

    return srcEditor;
}

void MainWindow::addWelcomeTab()
{
    // Create an example editor
    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/Resource/open.png"));
    QListWidgetItem * pItem = new QListWidgetItem(openIcon, "Welcome to ProIDE", fileListWidget);
    fileListWidget->addItem(pItem);

    QLabel* lblLogo = new QLabel();
    lblLogo->setAlignment(Qt::AlignCenter);
    lblLogo->setContentsMargins(0, 0, 0, 50);

    QString strImageFile = QString(":/Resource/ProIDE.png");
    lblLogo->setPixmap(QPixmap(strImageFile));

    previousDockWidget = dockManager->addTabWidget(lblLogo, "Welcome to ProIDE", previousDockWidget);
}

void MainWindow::addNewFileTab(const QString &fileNamePath, const QString &fName)
{
    if (!isExistListWidget(fName)) // just in case
    {
        const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/Resource/open.png"));
        QListWidgetItem * pItem = new QListWidgetItem(openIcon, fName, fileListWidget);
        pItem->setData(Qt::UserRole, fileNamePath);
        fileListWidget->addItem(pItem);

        // Create an example editor
        QTextEdit* txtedit = new QTextEdit();
        txtedit->setPlaceholderText("Please enter your text here ...");

        previousDockWidget = dockManager->addTabWidget(txtedit, fName, openIcon, previousDockWidget);
    }
}

void MainWindow::saveDockingState(const QString &stateFileName)
{
    QFile file(stateFileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Save state failed",
                              "Failed to open xml-file for writting!\nError: " + file.errorString());
    }
    else
    {
        QXmlStreamWriter xml(&file);
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        xml.writeStartElement("ProIDEDockingState");
        xml.writeStartElement("FileList");

        // Save file list
        for (int i = 0; i < fileListWidget->count(); ++i)
        {
            QListWidgetItem* lwItem = fileListWidget->item(i);
            if (lwItem->data(Qt::UserRole).isNull() && lwItem->text() == "Welcome to ProIDE")
                xml.writeTextElement("File", "Welcome to ProIDE");
            else
            {
                QString fileName = lwItem->data(Qt::UserRole).toString();
                if (QFileInfo::exists(fileName) || fileName.indexOf("Noname") < 0)
                    xml.writeTextElement("File", fileName);
                else
                    xml.writeTextElement("File", strippedName(fileName));
            }
        }

        xml.writeEndElement();

        // Save adv. docking system state
        dockManager->getManager()->saveStateToWriter(xml, 0);

        xml.writeEndElement();
        xml.writeEndDocument();
    }
}

void MainWindow::restoreDockingState(const QString &stateFileName)
{
    //
    // Remove current tabs/files

    if (dockManager)
    {
        dockManager->deleteLater();
        dockManager = nullptr;
        previousDockWidget = nullptr;
    }

    fileListWidget->clear();

    //
    // Create/open new ones

    dockManager = new QtAdsUtl::DockInDockWidget(this, true, m_perspectivesManager);
    setCentralWidget(dockManager);

    QFile file(stateFileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Restore state failed",
                              "Failed to open xml-file for reading.\nError: " + file.errorString());
    }
    else
    {
        ads::CDockingStateReader xml(&file);
        if (xml.readNextStartElement() && xml.name() == "ProIDEDockingState" &&
            xml.readNextStartElement() && xml.name() == "FileList")
        {
            QStringList saveStateFileNames;

            // Read file list
            while (xml.readNextStartElement())
            {
                if (xml.name() == "File")
                {
                    QString fileName = xml.readElementText();
                    saveStateFileNames.push_back(fileName);
                }
                else
                    xml.skipCurrentElement();
            }

            // Create/open files/tabs
            for (const QString& fileName : saveStateFileNames)
            {
                if (fileName == "Welcome to ProIDE")
                {
                    addWelcomeTab();
                }
                else if (QFile::exists(fileName))
                {
                    openFileAt(fileName, -1);
                }
                else
                {
                    QString fName = strippedName(fileName);
                    addNewFileTab(fileName, fName);
                }
            }

            // Restore dock widgets state
            dockManager->getManager()->restoreStateFromReader(xml);

            // Make the 1st dock area as the area for adding new tabs/files.
            previousDockWidget = dockManager->getManager()->dockArea(0);
        }
        else
        {
            QMessageBox::critical(this, "Restore state failed",
                                  "Error while reading docking state file.");
        }
    }
}

QString MainWindow::getActivateFilePath()
{
    return curFile;
}

void MainWindow::onBuildCompile()
{
    const QString source = getTabTextEdit()->toPlainText();
    const auto compilerName = "g++";
    const bool intelSyntax = false;
    const QString args = "";
    const QStringList argsList = [&args]() -> QStringList
    {
            if (!args.isEmpty()) {
                return args.split(QLatin1Char(' '));
            }
            return {};
    }();

    const Compiler compiler(std::move(compilerName));
    const QString currentFile = getActivateFilePath();

    std::pair<QString, bool> out = compiler.compileToAsm(source, argsList, intelSyntax, currentFile);

    if (out.second) {
        QString demangled = AsmParser::demangle(std::move(out.first));
        const QString cleanAsm = AsmParser().process(demangled);
        compiledEdiorWidget->setPlainText(cleanAsm);
    } else {
        compiledEdiorWidget->setPlainText(QStringLiteral("<Compilation Failed>\n") + out.first);
    }
}
