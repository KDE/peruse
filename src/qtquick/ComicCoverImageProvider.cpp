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
#include <karchive.h>
#include <karchivefile.h>

#include <QIcon>
#include <QMimeDatabase>

#include <qtquick_debug.h>

class ComicCoverImageProvider::Private {
public:
    Private() {}

    QStringList entries;
    void filterImages(QStringList& entries)
    {
        /// Sort case-insensitive, then remove non-image entries.
        QMap<QString, QString> entryMap;
        Q_FOREACH(const QString& entry, entries) {
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
        Q_FOREACH (const QString& entry, dir->entries()) {
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

ComicCoverImageProvider::ComicCoverImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
    , d(new Private)
{
}

ComicCoverImageProvider::~ComicCoverImageProvider()
{
    delete d;
}

QImage ComicCoverImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(size)
    Q_UNUSED(requestedSize)
    QImage img;

    KArchive* archive = nullptr;
    QMimeDatabase db;
    db.mimeTypeForFile(id, QMimeDatabase::MatchContent);
    const QMimeType mime = db.mimeTypeForFile(id, QMimeDatabase::MatchContent);
    if(mime.inherits("application/x-cbr") || mime.inherits("application/x-rar")) {
        archive = new KRar(id);
    }
    // FIXME: This goes elsewhere - see below
    // If this code seems familiar, it is adapted from kio-extras/thumbnail/comiccreator.cpp
    // The reason being that this code should be removed once our karchive-rar functionality is merged into
    // karchive proper.
    if(archive && archive->open(QIODevice::ReadOnly)) {
        // Get the archive's directory.
        const KArchiveDirectory* cArchiveDir = archive->directory();
        if (cArchiveDir) {
            QStringList entries;
            // Get and filter the entries from the archive.
            d->getArchiveFileList(entries, QString(), cArchiveDir);
            d->filterImages(entries);
            if (!entries.isEmpty()) {
                // Extract the cover file.
                const KArchiveFile *coverFile = static_cast<const KArchiveFile*>(cArchiveDir->entry(entries[0]));
                if (coverFile) {
                    bool success = img.loadFromData(coverFile->data());
                    if(!success) {
                        QIcon oops = QIcon::fromTheme("unknown");
                        img = oops.pixmap(oops.availableSizes().last()).toImage();
                        qCDebug(QTQUICK_LOG) << "Failed to load image with id:" << id;
                    }
                }
            }
        }
    }
    return img;
}
