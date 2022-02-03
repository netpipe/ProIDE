#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QStackedWidget>
#include "mdichild.h"
#include "qhlwidget.h"
#include "highlighter.h"
#include <QMenu>
#include <QSystemTrayIcon>

#define plugins
#include "plugin/qonsole.h"
#include "ui_qonsole.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#ifdef SOUND
#include <QMediaPlayer>
#endif
//#include "src/encryption/rsa/Rsa.h"
#include <QEvent>
#include <QThread>
#include <QDebug>
#include <QCryptographicHash>
#include <QGraphicsView>
#include <QTabWidget>
#include "plugin/downloadmanager.h"
#include "plugin/dbus/dbushandler.h"


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


    int adminftp=0;


#ifdef ENCRYPTION2
        //encryption
        QString encryptxor(QString test,QString key);
        QString decryptxor(QString string,QString key);

        QByteArray md5Checksum(QString stuff);
        QByteArray fileChecksum(const QString &fileName,QCryptographicHash::Algorithm hashAlgorithm);
        QString rot13( const QString & input );

        QString simplecrypt(QString string,QString key,QCryptographicHash::Algorithm hash);
        QString simpledecrypt(QString string,QString key,QCryptographicHash::Algorithm hash);

    #ifdef ENCRYPTION
        Rsa *rsaTester;
        BigInt m_e, m_n;
        QString aesKey;

    #endif

        QString encdec(QString ,int );
        QString encdec2(QString ,int );
        #ifdef ENCRYPTION
        QString rsaenc(QString input, Rsa *rsa = NULL);
        QString rsadec(QString input, Rsa *rsa);
        #endif
        QByteArray aesenc(QString input,QString,QString);
        QString aesdec(QByteArray input,QString,QString);

        QByteArray EncryptMsg(QString plainMsg,QString aeskey1,QString aeskey2);
        #ifdef ENCRYPTION
        QString DecryptMsg(QByteArray encryptedMsg, Rsa *rsa,QString aeskey1,QString aeskey2);
    #endif
        void GenerateQRCode(QString data,QGraphicsView *view);
    //    void EAN13(QString productname,QString country,QString ean,QGraphicsView *graphicsView);
        QString decodeqr(QString image);
#endif



public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool        openFile(const QString &fileName);

    QTextEdit * getActiveTextEdit();

    static MainWindow * instance();
    bool eventFilter(QObject *obj, QEvent *event);

    void on_exit();
    void showMessage();

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

    QStackedWidget *stackedWidget;

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

    void unCompress(QString filename , QString ofilename);
    void Compress(QString filename , QString ofilename);

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
        Qonsole e;
};

#endif
