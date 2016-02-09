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

#include "ArchiveBookModel.h"
#include "ArchiveImageProvider.h"

#include <QDir>
#include <QQmlEngine>
#include <QMimeDatabase>
#include <QDebug>

#include <karchive.h>
#include <kzip.h>

class ArchiveBookModel::Private
{
public:
    Private()
        : engine(0)
        , archive(0)
    {}
    QQmlEngine* engine;
    KArchive* archive;
    ArchiveImageProvider* imageProvider;

    static int counter()
    {
        static int count = 0;
        return count++;
    }
};

ArchiveBookModel::ArchiveBookModel(QObject* parent)
    : BookModel(parent)
    , d(new Private)
{
}

ArchiveBookModel::~ArchiveBookModel()
{
    delete d;
}

QStringList recursiveEntries(const KArchiveDirectory* dir, const QString& dirName = "")
{
    QStringList entries = dir->entries();
    QStringList allEntries = entries;
    Q_FOREACH(const QString& entryName, entries)
    {
        const KArchiveEntry* entry = dir->entry(entryName);
        if(entry->isDirectory())
        {
            const KArchiveDirectory* subDir = static_cast<const KArchiveDirectory*>(entry);
            QStringList subEntries = recursiveEntries(subDir, dirName + entryName + QDir::separator());
            Q_FOREACH(const QString& subEntry, subEntries)
            {
                entries.append(dirName + entryName + QDir::separator() + subEntry);
            }
        }
    }
    return entries;
}

void ArchiveBookModel::setFilename(QString newFilename)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(newFilename);
    if(d->archive) {
        delete d->archive;
        // TODO clean up imageproviders...
    }
    d->archive = 0;
    if(mime.inherits("application/zip"))
    {
        d->archive = new KZip(newFilename);
    }
//     else if (mime.inherits("application/x-rar"))
//     {
//         archive = new KRar(newFilename);
//     }

    bool success = false;
    if(d->archive)
    {
        QString prefix = QString("archivebookpage%1").arg(QString::number(Private::counter()));
        if(d->archive->open(QIODevice::ReadOnly))
        {
            d->imageProvider = new ArchiveImageProvider();
            d->imageProvider->setArchiveBookModel(this);
            d->imageProvider->setPrefix(prefix);
            d->engine->addImageProvider(prefix, d->imageProvider);

            QStringList entries = recursiveEntries(d->archive->directory());
            entries.sort();
            QString undesired = QString("%1").arg(QDir::separator()).append("Thumbs.db");
            Q_FOREACH(const QString& entry, entries)
            {
                const KArchiveEntry* archEntry = d->archive->directory()->entry(entry);
                if(archEntry->isFile() && !entry.endsWith(undesired))
                {
                    addPage(QString("image://%1/%2").arg(prefix).arg(entry), entry.split(QDir::separator()).last());
                }
            }
        }
        success = true;
    }

//     QDir dir(newFilename);
//     if(dir.exists())
//     {
//         QFileInfoList entries = dir.entryInfoList(QDir::Files, QDir::Name);
//         Q_FOREACH(const QFileInfo& entry, entries)
//         {
//             addPage(QString("file://").append(entry.canonicalFilePath()), entry.fileName());
//         }
//     }
    BookModel::setFilename(newFilename);
    emit loadingCompleted(success);
}

QObject * ArchiveBookModel::qmlEngine() const
{
    return d->engine;
}

void ArchiveBookModel::setQmlEngine(QObject* newEngine)
{
    d->engine = qobject_cast<QQmlEngine*>(newEngine);
    emit qmlEngineChanged();
}

const KArchiveFile * ArchiveBookModel::archiveFile(const QString& filePath)
{
    if(d->archive)
    {
        return d->archive->directory()->file(filePath);
    }
    return 0;
}
