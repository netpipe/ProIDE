#include "qaboutform.h"
#include "ui_qaboutform.h"

QAboutForm::QAboutForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QAboutForm)
{
    Qt::WindowFlags flags = Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::CustomizeWindowHint;
    setWindowFlags(flags);

    ui->setupUi(this);

    //setStyleSheet("QWidget{background-image: url(:/Resource/ProIDE.png);}");
}

QAboutForm::~QAboutForm()
{
    delete ui;
}

void QAboutForm::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return || event->key() == Qt::Key_Escape)
    {
        hide();
        return;
    }
}
