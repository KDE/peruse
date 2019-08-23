/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "PDFCoverImageProvider.h"

#include <QCoreApplication>
#include <QDir>
#include <QIcon>
#include <QMimeDatabase>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

#include <qtquick_debug.h>

class PDFCoverImageProvider::Private {
public:
    Private() {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        thumbDir = QDir(path);
        QString subpath("thumbcache");
        if(!thumbDir.exists(subpath)) {
            thumbDir.mkpath(subpath);
        }
        thumbDir.cd(subpath);
    }
    QDir thumbDir;
};

PDFCoverImageProvider::PDFCoverImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap, QQmlImageProviderBase::ForceAsynchronousImageLoading)
    , d(new Private)
{
}

PDFCoverImageProvider::~PDFCoverImageProvider()
{
    delete d;
}

QPixmap PDFCoverImageProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(size)
    Q_UNUSED(requestedSize)
    QPixmap img;

    QMimeDatabase db;
    db.mimeTypeForFile(id, QMimeDatabase::MatchContent);
    const QMimeType mime = db.mimeTypeForFile(id, QMimeDatabase::MatchContent);
    if(mime.inherits("application/pdf")) {
        //-sOutputFile=FILENAME.png FILENAME
        QString outFile = QString("%1/%2.png").arg(d->thumbDir.absolutePath()).arg(QUrl(id).toString().replace("/", "-").replace(":", "-"));
        if(!QFile::exists(outFile)) {
            // then we've not already generated a thumbnail, try to make one...
            QProcess thumbnailer;
            QStringList args;
            args << "-sPageList=1" << "-dLastPage=1" << "-dSAFER" << "-dBATCH" << "-dNOPAUSE" << "-dQUIET" << "-sDEVICE=png16m" << "-dGraphicsAlphaBits=4" << "-r300";
            args << QString("-sOutputFile=%1").arg(outFile) << id;
            QString gsApp;
            #ifdef Q_OS_WIN
                #ifdef __MINGW32__
                    gsApp = qApp->applicationDirPath() + "/gsc.exe";
                #else
                    gsApp = qApp->applicationDirPath();
                    #ifdef Q_OS_WIN64
                        gsApp += "/gswin64c.exe";
                    #else
                        gsApp += "/gswin32c.exe";
                    #endif
                #endif
            #else
                gsApp = "gs";
            #endif
            thumbnailer.start(gsApp, args);
            thumbnailer.waitForFinished();
        }
        bool success = false;
        // Now, does it exist this time?
        if(QFile::exists(outFile)) {
            success = img.load(outFile);
        }
        if(!success) {
            QIcon oops = QIcon::fromTheme("unknown");
            img = oops.pixmap(oops.availableSizes().last());
            qCDebug(QTQUICK_LOG) << "Failed to load image with id" << id << "from thumbnail file" << outFile;
        }
    }


    return img;
}
