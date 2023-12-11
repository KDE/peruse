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
#include "KRarFileEntry.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QDebug>

extern "C"
{
    #include <unarr.h>
}

class KRar::Private {
public:
    Private()
        : archive(nullptr)
        , stream(nullptr)
    {}
    ar_archive* archive;
    ar_stream* stream;
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

    d->stream = ar_open_file(fileName().toLocal8Bit());
    if (!d->stream)
    {
        qDebug() << "Failed to open" << fileName() << "into a stream for unarr";
        return false;
    }

    d->archive = ar_open_rar_archive(d->stream);
    if (!d->archive)
    {
        qDebug() << "Failed to open" << fileName() << "as a rar archive. Are we sure this is a rar archive?";
        return false;
    }

    // Iterate through all entries and get a KRarFileEntry out of them
    while (ar_parse_entry(d->archive)) {
        QString pathname(ar_entry_get_name(d->archive));
        int splitPos = pathname.lastIndexOf("/");
        QString path = pathname.left(splitPos);
        QString name = pathname.mid(splitPos + 1);
        QDateTime mtime = QDateTime::fromSecsSinceEpoch(ar_entry_get_filetime(d->archive));
        quint64 start = ar_entry_get_offset(d->archive);
        quint64 size = ar_entry_get_size(d->archive);
        // So, funny thing - unarr ignores directory entries in rar files entirely (see unarr/rar/rar.c:65)
        // Leaving the code in, in case we feel like reintroducing this at a later point in time
//         bool isDir = size < 1;//archive_entry_filetype(entry) == AE_IFDIR;

        KArchiveEntry* kaentry = nullptr;
//         if(isDir)
//         {
//             QString path = QDir::cleanPath(pathname);
//             const KArchiveEntry *ent = rootDir()->entry(path);
//             if (ent && ent->isDirectory()) {
//                 qDebug() << "Directory already exists, NOT going to add it again";
//                 kaentry = 0;
//             } else {
//                 kaentry = new KArchiveDirectory(this, name, 0755, mtime, rootDir()->user(), rootDir()->group(), QString());
//                 qDebug() << "KArchiveDirectory created, name=" << name;
//             }
//         }
//         else
//         {
            KRarFileEntry* fileEntry = new KRarFileEntry(this, name, 0100644, mtime, rootDir()->user(), rootDir()->group(), "", path, start, size, d->archive);
            kaentry = fileEntry;
            d->files.append(fileEntry);
//         }

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
    }

    return true;
}

bool KRar::closeArchive()
{
    ar_close_archive(d->archive);
    ar_close(d->stream);
    d->archive = nullptr;
    d->stream = nullptr;
    qDeleteAll(d->files);
    d->files.clear();
    return true;
}

void KRar::virtual_hook(int id, void* data)
{
    KArchive::virtual_hook(id, data);
}
