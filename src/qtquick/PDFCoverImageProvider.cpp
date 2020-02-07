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

#include <kiconloader.h>

#include <QCoreApplication>
#include <QDir>
#include <QIcon>
#include <QMimeDatabase>
#include <QProcess>
#include <QStandardPaths>
#include <QThreadPool>
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
    QThreadPool pool;
};

PDFCoverImageProvider::PDFCoverImageProvider()
    : QQuickAsyncImageProvider()
    , d(new Private)
{
}

PDFCoverImageProvider::~PDFCoverImageProvider()
{
    delete d;
}

class PDFCoverResponse : public QQuickImageResponse
{
    public:
        PDFCoverResponse(const QString &id, const QSize &requestedSize, const QDir& thumbDir, QThreadPool *pool)
        {
            m_runnable = new PDFCoverRunnable(id, requestedSize, thumbDir);
            connect(m_runnable, &PDFCoverRunnable::done, this, &PDFCoverResponse::handleDone);
            pool->start(m_runnable);
        }

        void handleDone(QImage image) {
            m_image = image;
            emit finished();
        }

        QQuickTextureFactory *textureFactory() const override
        {
            return QQuickTextureFactory::textureFactoryForImage(m_image);
        }

        void cancel() override
        {
            m_runnable->abort();
        }

        PDFCoverRunnable* m_runnable{nullptr};
        QImage m_image;
};

QQuickImageResponse * PDFCoverImageProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
    PDFCoverResponse* response = new PDFCoverResponse(id, requestedSize, d->thumbDir, &d->pool);
    return response;
}

class PDFCoverRunnable::Private {
public:
    Private() {}
    QString id;
    QSize requestedSize;

    bool abort{false};

    QDir thumbDir;
};

PDFCoverRunnable::PDFCoverRunnable(const QString& id, const QSize& requestedSize, const QDir& thumbDir)
    : d(new Private)
{
    d->id = id;
    d->requestedSize = requestedSize;
    d->thumbDir = thumbDir;
}

void PDFCoverRunnable::abort()
{
    d->abort = true;
}

void PDFCoverRunnable::run()
{
    QImage img;

    QSize ourSize(KIconLoader::SizeEnormous, KIconLoader::SizeEnormous);
    if(d->requestedSize.width() > 0 && d->requestedSize.height() > 0)
    {
        ourSize = d->requestedSize;
    }

    QMimeDatabase db;
    db.mimeTypeForFile(d->id, QMimeDatabase::MatchContent);
    const QMimeType mime = db.mimeTypeForFile(d->id, QMimeDatabase::MatchContent);
    if(!d->abort && mime.inherits("application/pdf")) {
        //-sOutputFile=FILENAME.png FILENAME
        QString outFile = QString("%1/%2.png").arg(d->thumbDir.absolutePath()).arg(QUrl(d->id).toString().replace("/", "-").replace(":", "-"));
        if(!d->abort && !QFile::exists(outFile)) {
            // then we've not already generated a thumbnail, try to make one...
            QProcess thumbnailer;
            QStringList args;
            args << "-sPageList=1" << "-dLastPage=1" << "-dSAFER" << "-dBATCH" << "-dNOPAUSE" << "-dQUIET" << "-sDEVICE=png16m" << "-dGraphicsAlphaBits=4" << "-r300";
            args << QString("-sOutputFile=%1").arg(outFile) << d->id;
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
        if(!d->abort && QFile::exists(outFile)) {
            success = img.load(outFile);
        }
        if(!d->abort && !success) {
            QIcon oops = QIcon::fromTheme("application-pdf");
            img = oops.pixmap(oops.availableSizes().last()).toImage();
            qCDebug(QTQUICK_LOG) << "Failed to load image with id" << d->id << "from thumbnail file" << outFile;
        }
    }

    Q_EMIT done(img.scaled(ourSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
