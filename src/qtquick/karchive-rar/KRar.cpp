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

#include "KRar.h"
#include <QDir>
#include "KRarFileEntry.h"

#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QDebug>

#include <KFilterDev>

#include <archive.h>
#include <archive_entry.h>

class KRar::Private {
public:
    Private()
        : archive(0)
    {}
    struct archive* archive;
    QList<KRarFileEntry*> files;
};

KRar::KRar(const QString& filename)
    : KArchive(filename)
    , d(new Private)
{
}

KRar::KRar(QIODevice* dev)
    : KArchive(dev)
    , d(new Private)
{
}

KRar::~KRar()
{
    if (isOpen()) {
        close();
    }
    delete d;
}

bool KRar::doPrepareWriting(const QString& /*name*/, const QString& /*user*/, const QString& /*group*/, qint64 /*size*/, mode_t /*perm*/, const QDateTime& /*atime*/, const QDateTime& /*mtime*/, const QDateTime& /*ctime*/)
{
    return false;
}

bool KRar::doFinishWriting(qint64 /*size*/)
{
    return false;
}

bool KRar::doWriteDir(const QString& /*name*/, const QString& /*user*/, const QString& /*group*/, mode_t /*perm*/, const QDateTime& /*atime*/, const QDateTime& /*mtime*/, const QDateTime& /*ctime*/)
{
    return false;
}

bool KRar::doWriteSymLink(const QString& /*name*/, const QString& /*target*/, const QString& /*user*/, const QString& /*group*/, mode_t /*perm*/, const QDateTime& /*atime*/, const QDateTime& /*mtime*/, const QDateTime& /*ctime*/)
{
    return false;
}

bool KRar::openArchive(QIODevice::OpenMode mode)
{
    // First clear the file list, because we don't like leftovers
    if(d->archive)
    {
        closeArchive();
    }
    // Open archive

    if (mode == QIODevice::WriteOnly) {
        return true;
    }
    if (mode != QIODevice::ReadOnly && mode != QIODevice::ReadWrite) {
        //qWarning() << "Unsupported mode " << mode;
        return false;
    }

    QIODevice *dev = device();
    if (!dev) {
        return false;
    }

    d->archive = archive_read_new();
    archive_read_support_format_rar(d->archive); // require a rar file...
    int r = archive_read_open_filename(d->archive, fileName().toLocal8Bit(), 1024);
    if(r != ARCHIVE_OK) {
        return false;
    }

    // Iterate through all entries and get a KRarFileEntry out of them
    struct archive_entry* entry;
    while (archive_read_next_header(d->archive, &entry) == ARCHIVE_OK) {
        QString pathname(archive_entry_pathname(entry));
        int splitPos = pathname.lastIndexOf("/");
        QString path = pathname.left(splitPos);
        QString name = pathname.mid(splitPos + 1);
        QDateTime mtime = QDateTime::fromTime_t(archive_entry_mtime(entry));
        bool isDir = archive_entry_filetype(entry) == AE_IFDIR;
        quint64 start = archive_read_header_position(d->archive);
        quint64 size = archive_entry_size(entry);

        KArchiveEntry* kaentry = 0;
        if(isDir)
        {
            QString path = QDir::cleanPath(pathname);
            const KArchiveEntry *ent = rootDir()->entry(path);
            if (ent && ent->isDirectory()) {
//                 qDebug() << "Directory already exists, NOT going to add it again";
                kaentry = 0;
            } else {
                kaentry = new KArchiveDirectory(this, name, 0755, mtime, rootDir()->user(), rootDir()->group(), QString());
//                 qDebug() << "KArchiveDirectory created, name=" << name;
            }
        }
        else
        {
            KRarFileEntry* fileEntry = new KRarFileEntry(this, name, 0100644, mtime, rootDir()->user(), rootDir()->group(), "", path, start, size, entry);
            kaentry = fileEntry;
            d->files.append(fileEntry);
        }

        if(kaentry)
        {
            if(splitPos > 0)
            {
                // Ensure container directory exists, create otherwise
                KArchiveDirectory *tdir = findOrCreate(path);
                tdir->addEntry(kaentry);
            }
            else
            {
                rootDir()->addEntry(kaentry);
            }
        }

        archive_read_data_skip(d->archive);  // Note 2
    }

    return true;
}

bool KRar::closeArchive()
{
    // Close the archive
    int r = archive_read_free(d->archive);  // Note 3
    d->archive = 0;
    if (r != ARCHIVE_OK) {
        qDeleteAll(d->files);
        d->files.clear();
        return false;
    }
    return true;
}

struct archive * KRar::archive() const
{
    return d->archive;
}

void KRar::virtual_hook(int id, void* data)
{
    KArchive::virtual_hook(id, data);
}
