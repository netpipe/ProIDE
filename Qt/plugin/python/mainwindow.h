#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void qtPython();

    //in mainwindow.h
    signals:
    void changeTextSignal();

    private slots:
    void settext();



private slots:
    void on_pyrun_clicked();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
