#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QStackedWidget>
#include "mdichild.h"
#include "qhlwidget.h"
#include "highlighter.h"


//#define plugins
#include "plugin/qonsole.h"
#include "ui_qonsole.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QListWidget;
class QListWidgetItem;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool        openFile(const QString &fileName);

    QTextEdit * getActiveTextEdit();

    static MainWindow * instance();
    bool eventFilter(QObject *obj, QEvent *event);


protected:
    void        closeEvent(QCloseEvent *event) override;

private slots:
    void        newFile();
    void        open();
    void        save();
    void        saveAs();
    void        updateRecentFileActions();
    void        openRecentFile();
#ifndef QT_NO_CLIPBOARD
    void        cut();
    void        copy();
    void        paste();
#endif
    void        applyTheme(int index);
    void        splitWindow(int index);
    void        about();
    void        updateMenus();
    void        updateWindowMenu();
    void        fileListWidgetItemClicked(QListWidgetItem * item);
    MdiChild *  createMdiChild();

    void on_actioncompile_triggered();

private:
    enum { MaxRecentFiles = 5 };

    void        createActions();
    void        createDockWindows();
    void        createStatusBar();
    void        getStyleList();
    void        loadStyleSheet( QString sheet_name);
    void        prependToRecentFiles(const QString &fileName);
    void        setRecentFilesVisible(bool visible);
    void        switchSplitter(int type);
    void        readSettings();
    void        writeSettings();

    bool        isExistListWidget(const QString& filename);
    bool        loadFile(const QString &fileName);

    static bool hasRecentFiles();

    MdiChild *      activeMdiChild() const;
    QMdiSubWindow * findMdiChild(const QString &fileName) const;

    QMdiArea *      mdiArea;

   // QStackedWidget *stackedWidget;

    QSplitter *     splitter;

    QMenu *         windowMenu;

    QAction *       newAct;
    QAction *       saveAct;
    QAction *       saveAsAct;
    QAction *       recentFileActs[MaxRecentFiles];
    QAction *       recentFileSeparator;
    QAction *       recentFileSubMenuAct;
#ifndef QT_NO_CLIPBOARD
    QAction *       cutAct;
    QAction *       copyAct;
    QAction *       pasteAct;
#endif
    QAction *       closeAct;
    QAction *       closeAllAct;
    QAction *       tileAct;
    QAction *       cascadeAct;
    QAction *       nextAct;
    QAction *       previousAct;
    QAction *       windowMenuSeparatorAct;

    QActionGroup *  themeGroup;
    QActionGroup *  splitGroup;
    QListWidget *   fileListWidget;
    QHlWidget *     hlWidget;

    QStringList     stylePathList;
    QStringList     styleNameList;

    int             splitterValue;
private:
    Ui::MainWindow *ui;
};

#endif
