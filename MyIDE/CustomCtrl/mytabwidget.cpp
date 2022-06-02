#include "mytabwidget.h"
#include "mytabbar.h"
#include <QDebug>

MyTabWidget::MyTabWidget(QWidget * parent)
    : QTabWidget(parent)
{
    auto mTabBar = new MyTabBar(this);
    setTabBar(mTabBar);
    connect(mTabBar,&MyTabBar::tabCloseRequested,this,&MyTabWidget::removeTabActually);
    mTabBar->setTabsClosable(true);
    setTabShape(QTabWidget::Rounded);
    connect(mTabBar,&MyTabBar::openFileRequest,this,&MyTabWidget::openFileRequest);
    connect(mTabBar,&MyTabBar::dragTabRequest,this,&MyTabWidget::dragTabRequest);

    QString strImageFile = QString(":/Resource/ProIDE.png");
    imgBack.load(strImageFile);
}


MyTabWidget::~MyTabWidget()
{

}

void MyTabWidget::removeTabActually(int index)
{
    widget(index)->deleteLater();
    removeTab(index);
}

void MyTabWidget::paintEvent(QPaintEvent *)
{
    if (count() == 0)
    {
        QPainter p(this);

        int w = imgBack.width();
        int h = imgBack.height();
        int x = (geometry().width() - w) / 2;
        int y = (geometry().height() - h) / 2;
        QRect rect = QRect(x, y, w, h);
        p.drawPixmap(rect, imgBack);
    }
}
