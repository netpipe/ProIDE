#include "qappinfo.h"

static QString  strAppDir;

QAppInfo::QAppInfo()
{
    strAppDir = "";
}

QString QAppInfo::getAppDirectory()
{
    if (strAppDir == "")
        strAppDir = qApp->applicationDirPath();

    return strAppDir;
}
