#include "mytabbar.h"
#include <QUrl>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QDebug>

MyTabBar::MyTabBar(QWidget *parent)
    : QTabBar(parent)
{
    setMovable(true);
    setAcceptDrops(true);
}

MyTabBar::~MyTabBar()
{}

void MyTabBar::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
    else event->ignore();
}

void MyTabBar::dropEvent(QDropEvent *event)
{
    const QMimeData * mimeData = event->mimeData();
    if(mimeData->hasUrls()) {
        for (const QUrl& url : mimeData->urls()) {
            emit openFileRequest(url.toLocalFile(),tabAt(event->pos()));
        }
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }
}

void MyTabBar::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::MiddleButton) {
        emit tabCloseRequested(tabAt(event->pos()));
    }

    QTabBar::mousePressEvent(event);
}

void MyTabBar::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()==Qt::LeftButton) {
        if(!geometry().contains(event->pos())) {
            emit dragTabRequest(currentIndex());
        }
    }

    QTabBar::mouseMoveEvent(event);
}
