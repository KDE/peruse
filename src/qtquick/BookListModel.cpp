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
#include <QUrl>

#include "CategoryEntriesModel.h"

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QMimeDatabase>

class BookListModel::Private {
public:
    Private()
        : contentModel(0)
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
    if(!d->newlyAddedCategoryModel)
    {
        d->newlyAddedCategoryModel = new CategoryEntriesModel(this);
        emit newlyAddedCategoryModelChanged();
    }
    if(!d->authorCategoryModel)
    {
        d->authorCategoryModel = new CategoryEntriesModel(this);
        emit authorCategoryModelChanged();
    }
    if(!d->seriesCategoryModel)
    {
        d->seriesCategoryModel = new CategoryEntriesModel(this);
        emit seriesCategoryModelChanged();
    }
    if(!d->folderCategoryModel)
    {
        d->folderCategoryModel = new CategoryEntriesModel(this);
        emit folderCategoryModel();
    }

    int newRow = d->entries.count();
    beginInsertRows(QModelIndex(), newRow, newRow + (last - first));
    for(int i = first; i < last + 1; ++i)
    {
        QVariant filename = d->contentModel->data(d->contentModel->index(first, 0, index), Qt::UserRole + 1);
        BookEntry* entry = new BookEntry();
        entry->filename = filename.toString();
        QStringList splitName = entry->filename.split(QDir::separator());
        entry->filetitle = splitName.takeLast();
        entry->series = splitName.takeLast(); // hahahaheuristics (dumb assumptions about filesystems, go!)
        // just in case we end up without a title... using complete basename here,
        // as we would rather have "book one. part two" and the odd "book one - part two.tar"
        QFileInfo fileinfo(entry->filename);
        entry->title = fileinfo.completeBaseName();

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
        }

        d->entries.append(entry);

        append(entry);
        d->authorCategoryModel->addCategoryEntry(entry->author, entry);
        d->seriesCategoryModel->addCategoryEntry(entry->series, entry);
        d->newlyAddedCategoryModel->append(entry, CreatedRole);
        QUrl url(entry->filename.left(entry->filename.lastIndexOf(QDir::separator())));
        d->folderCategoryModel->addCategoryEntry(url.path().mid(1), entry);
        d->folderCategoryModel->append(entry);
    }
    endInsertRows();
    emit countChanged();
    qApp->processEvents();
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

QObject * BookListModel::folderCategoryModel() const
{
    return d->folderCategoryModel;
}

int BookListModel::count() const
{
    return d->entries.count();
}
