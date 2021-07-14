#ifndef QHLWIDGET_H
#define QHLWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QLabel;
class QPushButton;
class QPlainTextEdit;
QT_END_NAMESPACE

class QHlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QHlWidget(QWidget *parent = nullptr);
    ~QHlWidget();

private:
    void initialize();
    void search(QString search, QPlainTextEdit *edit);

private slots:
    void onHighlightToken();
    void onReplace();
    void onUndo();

private:
    QLineEdit * lineFind;
    QLineEdit * lineReplace;
    QLabel *    lblFind;
    QLabel *    lblReplace;
    QPushButton * btnHigh;
    QPushButton * btnReplace;
    QPushButton * btnUndo;
    QWidget *   parentWidget;

    QString undobuffer;
    QString undobuffer2;
    QString findbuf;
    QString replacebuf;

    bool isFirstTime = true;
    bool colorundo = false;
    bool replace = false;
};

#endif // QHLWIDGET_H
