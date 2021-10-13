#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifdef PYTHON
#include <qtpython.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pyrun_clicked()
{
    qtPython();
    settext();

}
