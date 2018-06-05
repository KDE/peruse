#ifndef PERUSE_HELPERS_H
#define PERUSE_HELPERS_H

#include <QString>

class QQmlEngine;
class QApplication;

namespace PeruseHelpers {
    int getMaxTextureSize();
    int init(QString& path, QApplication& app);
}

#endif
