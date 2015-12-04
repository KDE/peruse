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

#include "CategoryEntriesModel.h"

class CategoryEntriesModel::Private {
public:
    Private() {};
    ~Private()
    {
        // No deleting the entries - this is done by the master BookListModel already, so do that at your own risk
    }
    QList<BookEntry*> entries;
};

CategoryEntriesModel::CategoryEntriesModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

CategoryEntriesModel::~CategoryEntriesModel()
{
    delete d;
}

QHash<int, QByteArray> CategoryEntriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilenameRole] = "filename";
    roles[FiletitleRole] = "filetitle";
    roles[TitleRole] = "title";
    roles[SeriesRole] = "series";
    roles[AuthorRole] = "author";
    roles[PublisherRole] = "publisher";
    roles[LastOpenedTimeRole] = "lastOpenedTime";
    roles[TotalPagesRole] = "totalPages";
    roles[CurrentPageRole] = "currentPage";
    return roles;
}

QVariant CategoryEntriesModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        const BookEntry* entry = d->entries[index.row()];
        switch(role)
        {
            case FilenameRole:
                result.setValue(entry->filename);
                break;
            case FiletitleRole:
                result.setValue(entry->filetitle);
                break;
            case TitleRole:
                result.setValue(entry->title);
                break;
            case SeriesRole:
                result.setValue(entry->series);
                break;
            case AuthorRole:
                result.setValue(entry->author);
                break;
            case PublisherRole:
                result.setValue(entry->publisher);
                break;
            case LastOpenedTimeRole:
                result.setValue(entry->lastOpenedTime);
                break;
            case TotalPagesRole:
                result.setValue(entry->totalPages);
                break;
            case CurrentPageRole:
                result.setValue(entry->currentPage);
                break;
            default:
                result.setValue(QString("Unknown role"));
                break;
        }
    }
    return result;
}

int CategoryEntriesModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}

void CategoryEntriesModel::append(BookEntry* entry)
{
    int insertionIndex = 0;
    for(; insertionIndex < d->entries.count(); ++insertionIndex)
    {
        if(QString::localeAwareCompare(d->entries.at(insertionIndex)->title, entry->title) > 0)
        {
            break;
        }
    }
    beginInsertRows(QModelIndex(), insertionIndex, insertionIndex);
    d->entries.insert(insertionIndex, entry);
    endInsertRows();
}
