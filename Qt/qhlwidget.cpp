#include "qhlwidget.h"
#include "mainwindow.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QPlainTextEdit>
#include <QMessageBox>

QHlWidget::QHlWidget(QWidget *parent) :
    QWidget(parent)
{
    parentWidget = parent;
    initialize();
}

QHlWidget::~QHlWidget()
{


}

void QHlWidget::initialize()
{
    QGridLayout * mainLayout = new QGridLayout(this);

    lblFind = new QLabel(tr("Find:"));
    lblReplace = new QLabel(tr("Replace:"));

    lineFind = new QLineEdit();
    lineReplace = new QLineEdit();

    btnHigh = new QPushButton();
    btnHigh->setText(tr("Highlight"));
    connect(btnHigh, &QPushButton::clicked, this, &QHlWidget::onHighlightToken);

    btnReplace = new QPushButton();
    btnReplace->setText(tr("Replace"));
    connect(btnReplace, &QPushButton::clicked, this, &QHlWidget::onReplace);

    btnUndo = new QPushButton();
    btnUndo->setText(tr("Undo"));
    connect(btnUndo, &QPushButton::clicked, this, &QHlWidget::onUndo);

    mainLayout->addWidget(lblFind, 0, 0);
    mainLayout->addWidget(lineFind, 0, 1, 1, 3);
    mainLayout->addWidget(lblReplace, 1, 0);
    mainLayout->addWidget(lineReplace, 1, 1, 1, 3);
    mainLayout->addWidget(btnHigh, 2, 0);
    mainLayout->addWidget(btnReplace, 2, 1);
    mainLayout->addWidget(btnUndo, 2, 2);

    mainLayout->setRowMinimumHeight(0, 30);
    mainLayout->setRowMinimumHeight(1, 30);
    mainLayout->setRowMinimumHeight(2, 30);
    mainLayout->setRowStretch(3, 2);
    mainLayout->setContentsMargins(10, 10, 10, 10);
}

void QHlWidget::search(QString search, QTextEdit *edit)
{
    QString searchString = search.toLatin1();
    QTextDocument *document = edit->document();

    bool found = false;

    if (isFirstTime == false)
        document->undo();

    if (searchString.isEmpty()) {
        QMessageBox::information(this, tr("Empty Search Field"), "The search field is empty. Please enter a word and click Find.");
    }
    else {

        QTextCursor highlightCursor(document);
        QTextCursor cursor(document);

        cursor.beginEditBlock();

        QTextCharFormat plainFormat(highlightCursor.charFormat());
        QTextCharFormat colorFormat = plainFormat;
        colorFormat.setForeground(Qt::blue);

        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            highlightCursor = document->find(searchString, highlightCursor, QTextDocument::FindWholeWords);

            if (!highlightCursor.isNull()) {
                found = true;
                highlightCursor.movePosition(QTextCursor::WordRight,
                                       QTextCursor::KeepAnchor,1);
                highlightCursor.mergeCharFormat(colorFormat);
            }
        }

        cursor.endEditBlock();
        isFirstTime = false;

        if (found == false) {
            QMessageBox::information(this, tr("Word Not Found"), "Sorry, the word cannot be found.");
        }
    }
}

void QHlWidget::onHighlightToken()
{
    //if (MainWindow::instance() == NULL)
   //     return;

  //  isFirstTime = false;

  //  QTextEdit *plaineditptr = MainWindow::instance()->getActiveTextEdit();
  //  if (plaineditptr == Q_NULLPTR)
  //      return;

 //   undobuffer = plaineditptr->toPlainText();
 //   search(lineFind->text().toLatin1(), plaineditptr);
}

void QHlWidget::onReplace()
{
    isFirstTime = true;

    QTextEdit *plaineditptr = MainWindow::instance()->getActiveTextEdit();
    if (plaineditptr == Q_NULLPTR)
        return;

    undobuffer = plaineditptr->toPlainText();
    undobuffer2 = undobuffer;
    undobuffer.replace(lineFind->text().toLatin1(), lineReplace->text().toLatin1());
    plaineditptr->setPlainText(undobuffer);

    undobuffer = plaineditptr->toPlainText();

    search(lineReplace->text().toLatin1(), plaineditptr);

    replace = true;

    findbuf = lineFind->text();
    replacebuf = lineReplace->text();

    lineFind->setText(lineReplace->text());
}

void QHlWidget::onUndo()
{
    if(replace)
    {
        if(colorundo){
            lineFind->setText(findbuf);
            lineReplace->setText(replacebuf);
            colorundo = false;
        }

        QTextEdit *plaineditptr = MainWindow::instance()->getActiveTextEdit();
        if (plaineditptr == Q_NULLPTR)
            return;

        plaineditptr->setPlainText(undobuffer2);
        replace=false;
    }

    replace=false;
}
