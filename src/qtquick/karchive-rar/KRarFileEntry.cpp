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

extern "C"
{
    #include <unarr.h>
}

class KRarFileEntry::Private {
public:
    Private()
        : crc(0)
        , headerStart(0)
        , archive(nullptr)
        , rar(nullptr)
    {
    }
    unsigned long crc;
    qint64        headerStart;
    QString       path;
    ar_archive* archive;
    KRar* rar;
};

KRarFileEntry::KRarFileEntry(KRar* rar, const QString& name, int access, const QDateTime& date, const QString& user, const QString& group, const QString& symlink, const QString& path, qint64 start, qint64 uncompressedSize, struct ar_archive_s* archive)
    : KArchiveFile(rar, name, access, date, user, group, symlink, start, uncompressedSize)
    , d(new Private)
{
    d->headerStart = start;
    d->path = path;
    d->rar = rar;
    d->archive = archive;
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

    ar_archive* archive = d->archive;
    QString pathname = QString("%1/%2").arg(path()).arg(name());
    if(ar_parse_entry_at(archive, d->headerStart))
    {
        data.resize(size());
        if(!ar_entry_uncompress(archive, data.data(), size()))
        {
            qDebug() << "We got an error reading the data attempting to read" << pathname << " - error will be reported by unarr, see above";// << r << archive_error_string(archive);
        }
    }
    return data;
}

QIODevice * KRarFileEntry::createDevice() const
{
    return nullptr;
}
