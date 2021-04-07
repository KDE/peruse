/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "ComicCoverImageProvider.h"

#include <KRar.h>
#include <KZip>
#include <karchive.h>
#include <karchivefile.h>
#include <kiconloader.h>

#include <QIcon>
#include <QMimeDatabase>
#include <QMutex>
#include <QThreadPool>

#include <qtquick_debug.h>

class ComicCoverImageProvider::Private {
public:
    Private() {
        // We'll just stick with a 100MiB cache for now - something to expose later maybe?
        imageCache = new KImageCache("peruse-comiccover", 104857600);
    }
    ~Private() {
        delete imageCache;
    }
    KImageCache* imageCache;
};

ComicCoverImageProvider::ComicCoverImageProvider()
    : QQuickAsyncImageProvider()
    , d(new Private)
{
}

ComicCoverImageProvider::~ComicCoverImageProvider()
{
    delete d;
}

class ComicCoverResponse : public QQuickImageResponse
{
    public:
        ComicCoverResponse(const QString &id, const QSize &requestedSize, KImageCache* imageCache)
        {
            m_runnable = new ComicCoverRunnable(id, requestedSize, imageCache);
            m_runnable->setAutoDelete(false);
            connect(m_runnable, &ComicCoverRunnable::done, this, &ComicCoverResponse::handleDone, Qt::QueuedConnection);
            connect(this, &QQuickImageResponse::finished, m_runnable, &QObject::deleteLater,  Qt::QueuedConnection);
            QThreadPool::globalInstance()->start(m_runnable);
        }

        void handleDone(QImage image) {
            m_image = image;
            Q_EMIT finished();
        }

        QQuickTextureFactory *textureFactory() const override
        {
            return QQuickTextureFactory::textureFactoryForImage(m_image);
        }

        void cancel() override
        {
            m_runnable->abort();
        }

        ComicCoverRunnable* m_runnable{nullptr};
        QImage m_image;
};

QQuickImageResponse * ComicCoverImageProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
    ComicCoverResponse* response = new ComicCoverResponse(id, requestedSize, d->imageCache);
    return response;
}


class ComicCoverRunnable::Private {
public:
    Private() {}
    QString id;
    QSize requestedSize;
    KImageCache* imageCache;

    bool abort{false};
    QMutex abortMutex;
    bool isAborted() {
        QMutexLocker locker(&abortMutex);
        return abort;
    }

    QStringList entries;
    void filterImages(QStringList& entries)
    {
        /// Sort case-insensitive, then remove non-image entries.
        QMap<QString, QString> entryMap;
        for(const QString& entry : qAsConst(entries)) {
            if (entry.endsWith(QLatin1String(".gif"), Qt::CaseInsensitive) ||
                    entry.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive) ||
                    entry.endsWith(QLatin1String(".jpeg"), Qt::CaseInsensitive) ||
                    entry.endsWith(QLatin1String(".png"), Qt::CaseInsensitive)) {
                entryMap.insert(entry.toLower(), entry);
            }
        }
        entries = entryMap.values();
    }
    void getArchiveFileList(QStringList& entries, const QString& prefix, const KArchiveDirectory *dir)
    {
        /// Recursively list all files in the ZIP archive into 'entries'.
        for (const QString& entry : dir->entries()) {
            const KArchiveEntry *e = dir->entry(entry);
            if (e->isDirectory()) {
            getArchiveFileList(entries, prefix + entry + '/',
                static_cast<const KArchiveDirectory*>(e));
            } else if (e->isFile()) {
                entries.append(prefix + entry);
            }
        }
    }
};

ComicCoverRunnable::ComicCoverRunnable(const QString& id, const QSize& requestedSize, KImageCache* imageCache)
    : d(new Private)
{
    d->id = id;
    d->requestedSize = requestedSize;
    d->imageCache = imageCache;
}

ComicCoverRunnable::~ComicCoverRunnable()
{
    abort();
}

void ComicCoverRunnable::abort()
{
    QMutexLocker locker(&d->abortMutex);
    d->abort = true;
}

void ComicCoverRunnable::run()
{
    QSize ourSize(KIconLoader::SizeEnormous, KIconLoader::SizeEnormous);
    if(d->requestedSize.width() > 0 && d->requestedSize.height() > 0)
    {
        ourSize = d->requestedSize;
    }

    QImage img;
    if (d->imageCache->findImage(d->id, &img)) {
        KArchive* archive = nullptr;
        QMimeDatabase db;
        db.mimeTypeForFile(d->id, QMimeDatabase::MatchContent);
        const QMimeType mime = db.mimeTypeForFile(d->id, QMimeDatabase::MatchContent);
        if(!d->isAborted() && (mime.inherits("application/x-cbr") || mime.inherits("application/x-rar"))) {
            archive = new KRar(d->id);
        } else if(!d->isAborted() && (mime.inherits("application/x-cbz") || mime.inherits("application/zip"))) {
            archive = new KZip(d->id);
        }
        // FIXME: This goes elsewhere - see below
        // If this code seems familiar, it is adapted from kio-extras/thumbnail/comiccreator.cpp
        // The reason being that this code should be removed once our karchive-rar functionality is merged into
        // karchive proper.
        if(!d->isAborted() && archive && archive->open(QIODevice::ReadOnly)) {
            // Get the archive's directory.
            const KArchiveDirectory* cArchiveDir = archive->directory();
            if (!d->isAborted() && cArchiveDir) {
                QStringList entries;
                // Get and filter the entries from the archive.
                d->getArchiveFileList(entries, QString(), cArchiveDir);
                d->filterImages(entries);
                if (!d->isAborted() && !entries.isEmpty()) {
                    // Extract the cover file.
                    const KArchiveFile *coverFile = static_cast<const KArchiveFile*>(cArchiveDir->entry(entries[0]));
                    if (!d->isAborted() && coverFile) {
                        bool success = img.loadFromData(coverFile->data());
                        if(!d->isAborted() && !success) {
                            QIcon oops = QIcon::fromTheme("unknown");
                            img = oops.pixmap(oops.availableSizes().last()).toImage();
                            qCDebug(QTQUICK_LOG) << "Failed to load image with id:" << d->id;
                        }
                    }
                }
            }
        }
        d->imageCache->insertImage(d->id, img);
    }
    Q_EMIT done(img.scaled(ourSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
