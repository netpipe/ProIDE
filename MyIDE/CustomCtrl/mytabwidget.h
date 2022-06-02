#ifndef MYTABWIDGET_H
#define MYTABWIDGET_H

#include <QTabWidget>
#include <QPainter>

class MyTabWidget : public QTabWidget
{
    Q_OBJECT

public:
    MyTabWidget(QWidget * parent = Q_NULLPTR);
    ~MyTabWidget();
    void removeTabActually(int index);

    void paintEvent(QPaintEvent *)			override;

signals:
    void openFileRequest(QString/*fileName*/,int/*tabIndex*/);
    void dragTabRequest(int/*tabIndex*/);

private:
    QPixmap imgBack;

};

#endif // MYTABWIDGET_H
