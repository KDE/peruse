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

#include "BookListModel.h"

#include "CategoryEntriesModel.h"

#include <kio/deletejob.h>

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QMimeDatabase>
#include <QUrl>

class BookListModel::Private {
public:
    Private()
        : contentModel(0)
        , titleCategoryModel(0)
        , newlyAddedCategoryModel(0)
        , authorCategoryModel(0)
        , seriesCategoryModel(0)
        , folderCategoryModel(0)
    {};
    ~Private()
    {
        qDeleteAll(entries);
    }
    QList<BookEntry*> entries;

    QAbstractListModel* contentModel;
    CategoryEntriesModel* titleCategoryModel;
    CategoryEntriesModel* newlyAddedCategoryModel;
    CategoryEntriesModel* authorCategoryModel;
    CategoryEntriesModel* seriesCategoryModel;
    CategoryEntriesModel* folderCategoryModel;
};

BookListModel::BookListModel(QObject* parent)
    : CategoryEntriesModel(parent)
    , d(new Private)
{
}

BookListModel::~BookListModel()
{
    delete d;
}

void BookListModel::setContentModel(QObject* newModel)
{
    if(d->contentModel)
    {
        d->contentModel->disconnect(this);
    }
    d->contentModel = qobject_cast<QAbstractListModel*>(newModel);
    if(d->contentModel)
    {
        connect(d->contentModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(contentModelItemsInserted(QModelIndex,int, int)));
    }
    emit contentModelChanged();
}

QObject * BookListModel::contentModel() const
{
    return d->contentModel;
}

void BookListModel::contentModelItemsInserted(QModelIndex index, int first, int last)
{
    if(!d->titleCategoryModel)
    {
        d->titleCategoryModel = new CategoryEntriesModel(this);
        connect(this, SIGNAL(entryDataUpdated(BookEntry*)), d->titleCategoryModel, SIGNAL(entryDataUpdated(BookEntry*)));
        connect(this, SIGNAL(entryRemoved(BookEntry*)), d->titleCategoryModel, SIGNAL(entryRemoved(BookEntry*)));
        emit titleCategoryModelChanged();
    }
    if(!d->newlyAddedCategoryModel)
    {
        d->newlyAddedCategoryModel = new CategoryEntriesModel(this);
        connect(this, SIGNAL(entryDataUpdated(BookEntry*)), d->newlyAddedCategoryModel, SIGNAL(entryDataUpdated(BookEntry*)));
        connect(this, SIGNAL(entryRemoved(BookEntry*)), d->newlyAddedCategoryModel, SIGNAL(entryRemoved(BookEntry*)));
        emit newlyAddedCategoryModelChanged();
    }
    if(!d->authorCategoryModel)
    {
        d->authorCategoryModel = new CategoryEntriesModel(this);
        connect(this, SIGNAL(entryDataUpdated(BookEntry*)), d->authorCategoryModel, SIGNAL(entryDataUpdated(BookEntry*)));
        connect(this, SIGNAL(entryRemoved(BookEntry*)), d->authorCategoryModel, SIGNAL(entryRemoved(BookEntry*)));
        emit authorCategoryModelChanged();
    }
    if(!d->seriesCategoryModel)
    {
        d->seriesCategoryModel = new CategoryEntriesModel(this);
        connect(this, SIGNAL(entryDataUpdated(BookEntry*)), d->seriesCategoryModel, SIGNAL(entryDataUpdated(BookEntry*)));
        connect(this, SIGNAL(entryRemoved(BookEntry*)), d->seriesCategoryModel, SIGNAL(entryRemoved(BookEntry*)));
        emit seriesCategoryModelChanged();
    }
    if(!d->folderCategoryModel)
    {
        d->folderCategoryModel = new CategoryEntriesModel(this);
        connect(this, SIGNAL(entryDataUpdated(BookEntry*)), d->folderCategoryModel, SIGNAL(entryDataUpdated(BookEntry*)));
        connect(this, SIGNAL(entryRemoved(BookEntry*)), d->folderCategoryModel, SIGNAL(entryRemoved(BookEntry*)));
        emit folderCategoryModel();
    }

    int newRow = d->entries.count();
    beginInsertRows(QModelIndex(), newRow, newRow + (last - first));
    for(int i = first; i < last + 1; ++i)
    {
        QVariant filename = d->contentModel->data(d->contentModel->index(first, 0, index), Qt::UserRole + 1);
        BookEntry* entry = new BookEntry();
        entry->filename = filename.toString();
        QStringList splitName = entry->filename.split("/");
        if (!splitName.isEmpty())
            entry->filetitle = splitName.takeLast();
        if(!splitName.isEmpty())
            entry->series = splitName.takeLast(); // hahahaheuristics (dumb assumptions about filesystems, go!)
        // just in case we end up without a title... using complete basename here,
        // as we would rather have "book one. part two" and the odd "book one - part two.tar"
        QFileInfo fileinfo(entry->filename);
        entry->title = fileinfo.completeBaseName();

        if(entry->filename.toLower().endsWith("cbr")) {
            entry->thumbnail = QString("image://comiccover/").append(entry->filename);
        }
#ifdef USE_PERUSE_PDFTHUMBNAILER
        else if(entry->filename.toLower().endsWith("pdf")) {
            entry->thumbnail = QString("image://pdfcover/").append(entry->filename);
        }
#endif
        else {
            entry->thumbnail = QString("image://preview/").append(entry->filename);
        }

        QVariantHash metadata = d->contentModel->data(d->contentModel->index(first, 0, index), Qt::UserRole + 2).toHash();
        QVariantHash::const_iterator it = metadata.constBegin();
        for (; it != metadata.constEnd(); it++) {
            if(it.key() == QLatin1String("author"))
            { entry->author = it.value().toString().trimmed(); }
            else if(it.key() == QLatin1String("title"))
            { entry->title = it.value().toString().trimmed(); }
            else if(it.key() == QLatin1String("publisher"))
            { entry->publisher = it.value().toString().trimmed(); }
            else if(it.key() == QLatin1String("created"))
            { entry->created = it.value().toDateTime(); }
            else if(it.key() == QLatin1String("currentPage"))
            { entry->currentPage = it.value().toInt(); }
            else if(it.key() == QLatin1String("totalPages"))
            { entry->totalPages = it.value().toInt(); }
        }

        d->entries.append(entry);

        append(entry);
        d->titleCategoryModel->addCategoryEntry(entry->title.left(1).toUpper(), entry);
        d->authorCategoryModel->addCategoryEntry(entry->author, entry);
        d->seriesCategoryModel->addCategoryEntry(entry->series, entry);
        d->newlyAddedCategoryModel->append(entry, CreatedRole);
        QUrl url(entry->filename.left(entry->filename.lastIndexOf("/")));
        d->folderCategoryModel->addCategoryEntry(url.path().mid(1), entry);
        d->folderCategoryModel->append(entry);
    }
    endInsertRows();
    emit countChanged();
    qApp->processEvents();
}

QObject * BookListModel::titleCategoryModel() const
{
    return d->titleCategoryModel;
}

QObject * BookListModel::newlyAddedCategoryModel() const
{
    return d->newlyAddedCategoryModel;
}

QObject * BookListModel::authorCategoryModel() const
{
    return d->authorCategoryModel;
}

QObject * BookListModel::seriesCategoryModel() const
{
    return d->seriesCategoryModel;
}

QObject * BookListModel::seriesModelForEntry(BookEntry* entry)
{
    return d->seriesCategoryModel->leafModelForEntry(entry);
}

QObject * BookListModel::folderCategoryModel() const
{
    return d->folderCategoryModel;
}

int BookListModel::count() const
{
    return d->entries.count();
}

void BookListModel::setBookData(QString fileName, QString property, QString value)
{
    Q_FOREACH(BookEntry* entry, d->entries)
    {
        if(entry->filename == fileName)
        {
            if(property == "totalPages")
            {
                entry->totalPages = value.toInt();
            }
            else if(property == "currentPage")
            {
                entry->currentPage = value.toInt();
            }
            emit entryDataUpdated(entry);
            break;
        }
    }
}

void BookListModel::removeBook(QString fileName, bool deleteFile)
{
    if(deleteFile) {
        KIO::DeleteJob* job = KIO::del(QUrl::fromLocalFile(fileName), KIO::HideProgressInfo);
        job->start();
    }

    Q_FOREACH(BookEntry* entry, d->entries)
    {
        if(entry->filename == fileName)
        {
            emit entryRemoved(entry);
            delete entry;
            break;
        }
    }
}
