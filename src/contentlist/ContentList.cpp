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

#include "ContentList.h"
#include "FilesystemContentLister.h"

#ifdef BALOO_FOUND
    #include "BalooContentLister.h"
#endif

#include <QDebug>
#include <QMimeDatabase>
#include <QTimer>

struct ContentEntry {
    QString filename;
    QVariantHash metadata;
};

class ContentList::Private {
public:
    Private()
        : actualContentList(0)
    {}
    QList<ContentEntry*> entries;
    ContentListerBase* actualContentList;
};

ContentList::ContentList(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
#ifdef BALOO_FOUND
    BalooContentLister* baloo = new BalooContentLister(this);
    if(baloo->balooEnabled())
    {
        d->actualContentList = baloo;
        qDebug() << "Baloo support enabled";
    }
    else
    {
        baloo->deleteLater();
        d->actualContentList = new FilesystemContentLister(this);
        qDebug() << "Baloo is disabled for the system, use the filesystem scraper";
    }
#else
    d->actualContentList = new FilesystemContentLister(this);
#endif
    connect(d->actualContentList, SIGNAL(fileFound(QString,QVariantHash)), this, SLOT(fileFound(QString,QVariantHash)));
    connect(d->actualContentList, SIGNAL(searchCompleted()), this, SIGNAL(searchCompleted()));
}

ContentList::~ContentList()
{
    delete d;
}

QString ContentList::getMimetype(QString filePath)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath);
    return mime.name();
}

void ContentList::addLocation(QString path)
{
    d->actualContentList->addLocation(path);
}

void ContentList::addMimetype(QString mimetype)
{
    d->actualContentList->addMimetype(mimetype);
}

void ContentList::setSearchString(const QString& searchString)
{
    d->actualContentList->setSearchString(searchString);
}

void ContentList::startSearch()
{
    QTimer::singleShot(1, d->actualContentList, SLOT(startSearch()));
}

void ContentList::setKnownFiles(QStringList knownFiles)
{
    d->actualContentList->setKnownFiles(knownFiles);
}

void ContentList::fileFound(const QString& filePath, const QVariantHash& metadata)
{
    ContentEntry* entry = new ContentEntry();
    entry->filename = filePath;
    entry->metadata = metadata;

    int newRow = d->entries.count();
    beginInsertRows(QModelIndex(), newRow, newRow);
    d->entries.append(entry);
    endInsertRows();
}

QHash<int, QByteArray> ContentList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilenameRole] = "filename";
    roles[MetadataRole] = "metadata";
    return roles;
}

QVariant ContentList::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        const ContentEntry* entry = d->entries[index.row()];
        switch(role)
        {
            case FilenameRole:
                result.setValue(entry->filename);
                break;
            case MetadataRole:
                result.setValue(entry->metadata);
                break;
            default:
                result.setValue(QString("Unknown role"));
                break;
        }
    }
    return result;
}

int ContentList::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}
