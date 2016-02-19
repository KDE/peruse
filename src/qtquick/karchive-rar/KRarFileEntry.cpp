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

#include "KRarFileEntry.h"

#include <QDebug>

#include <archive.h>
#include <archive_entry.h>

class KRarFileEntry::Private {
public:
    Private()
        : crc(0)
        , headerStart(0)
        , entry(0)
        , rar(0)
    {
    }
    unsigned long crc;
    qint64        headerStart;
    QString       path;
    struct archive_entry* entry;
    KRar* rar;
};

KRarFileEntry::KRarFileEntry(KRar* rar, const QString& name, int access, const QDateTime& date, const QString& user, const QString& group, const QString& symlink, const QString& path, qint64 start, qint64 uncompressedSize, struct archive_entry* entry)
    : KArchiveFile(rar, name, access, date, user, group, symlink, start, uncompressedSize)
    , d(new Private)
{
    d->path = path;
    d->entry = entry;
    d->rar = rar;
//     qDebug() << "New entry for file" << name << "in path" << path;
}

KRarFileEntry::~KRarFileEntry()
{
    delete d;
}

void KRarFileEntry::setHeaderStart(qint64 headerstart)
{
    d->headerStart = headerstart;
}
qint64 KRarFileEntry::headerStart() const
{
    return d->headerStart;
}

unsigned long KRarFileEntry::crc32() const
{
    return d->crc;
}

void KRarFileEntry::setCRC32(unsigned long crc32)
{
    d->crc = crc32;
}

const QString & KRarFileEntry::path() const
{
    return d->path;
}

QByteArray KRarFileEntry::data() const
{
//     qDebug() << "Attempting to grab data from" << name() << "in" << path();
    QByteArray data;

    struct archive* archive = archive_read_new();
    archive_read_support_format_rar(archive); // require a rar file...
    int r = archive_read_open_filename(archive, d->rar->fileName().toLocal8Bit(), 1024);
    if(r != ARCHIVE_OK)
    {
        qDebug() << "Failed to open archive!";
        return data;
    }
    QString pathname = QString("%1/%2").arg(path()).arg(name());
    struct archive_entry* entry;
    while (archive_read_next_header(archive, &entry) == ARCHIVE_OK)
    {
        QString thisName = archive_entry_pathname(entry);
        if (pathname == thisName)
        {
            data.resize(size());
            r = archive_read_data(archive, data.data(), size());
            if(r != size()) {
                qDebug() << "We got an error reading the data" << r << archive_error_string(archive);
            }
            break;
        }
        else
        {
            archive_read_data_skip(archive); // Skip this entry 
        }
    }
    archive_read_free(archive);
    return data;
}

QIODevice * KRarFileEntry::createDevice() const
{
    return 0;
}
