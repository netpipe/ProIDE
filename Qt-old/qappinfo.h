#ifndef QAPPINFO_H
#define QAPPINFO_H

#include <QApplication>
#include <QString>

class QAppInfo
{
public:
    QAppInfo();

    static QString getAppDirectory();
};

#endif // QAPPINFO_H
