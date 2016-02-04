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

#include "CategoryModel.h"
#include <QDir>
#include <QDebug>

#include "PropertyContainer.h"

struct CategoryEntry
{
public:
    CategoryEntry()
        : entries(0)
    {}
    QString name;
    CategoryEntriesModel* entries;
    QString thumbnailUrl;
};

class CategoryModel::Private
{
public:
    Private() {}
    ~Private()
    {
        qDeleteAll(entries);
    }
    QList<CategoryEntry*> entries;
};

CategoryModel::CategoryModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

CategoryModel::~CategoryModel()
{
    delete d;
}

QHash<int, QByteArray> CategoryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[CategoryNameRole] = "categoryName";
    roles[EntryCountRole] = "entryCount";
    roles[ThumbnailUrlRole] = "thumbnailUrl";
    roles[EntriesModelRole] = "entriesModel";
    return roles;
}

QVariant CategoryModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        const CategoryEntry* entry = d->entries[index.row()];
        switch(role)
        {
            case CategoryNameRole:
                result.setValue(entry->name);
                break;
            case EntryCountRole:
                result.setValue(entry->entries->rowCount(QModelIndex()));
                break;
            case ThumbnailUrlRole:
                result.setValue(entry->thumbnailUrl);
                break;
            case EntriesModelRole:
                result.setValue(entry->entries);
                break;
            default:
                result.setValue(QString("Unknown role"));
                break;
        }
    }
    return result;
}

int CategoryModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}

void CategoryModel::addCategoryEntry(const QString& categoryName, BookEntry* bookEntry)
{
    CategoryEntry* entry = 0;
    QStringList splitName = categoryName.split(QDir::separator());
    QString actualName = splitName.takeFirst();
    Q_FOREACH(CategoryEntry* existingEntry, d->entries)
    {
        if(existingEntry->name == actualName)
        {
            entry = existingEntry;
            break;
        }
    }
    if(!entry) {
        entry = new CategoryEntry();
        entry->name = actualName;
        entry->thumbnailUrl = ""; // find useful thumbnail... probably first book in category?
        entry->entries = new CategoryEntriesModel(this);

        int insertionIndex = 0;
        for(; insertionIndex < d->entries.count(); ++insertionIndex)
        {
            if(QString::localeAwareCompare(d->entries.at(insertionIndex)->name, entry->name) > 0)
            {
                break;
            }
        }
        beginInsertRows(QModelIndex(), insertionIndex, insertionIndex);
        d->entries.insert(insertionIndex, entry);
        endInsertRows();
    }
    entry->entries->append(bookEntry);

    if(splitName.count() > 0)
    {
        entry->entries->addCategoryEntry(splitName.join(QDir::separator()), bookEntry);
    }
}

QObject * CategoryModel::get(int index)
{
    PropertyContainer* obj = new PropertyContainer("book", this);
    if(index > -1 && index < rowCount())
    {
        CategoryEntry* catEntry = d->entries.at(index);
        obj->setProperty("title", catEntry->name);
        obj->setProperty("categoryEntriesCount", catEntry->entries->rowCount());
        obj->setProperty("entriesModel", QVariant::fromValue(catEntry->entries));
    }
    return obj;
}
