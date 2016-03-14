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

#ifndef BOOKLISTMODEL_H
#define BOOKLISTMODEL_H

#include "CategoryEntriesModel.h"

class BookListModel : public CategoryEntriesModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QObject* contentModel READ contentModel WRITE setContentModel NOTIFY contentModelChanged)
    Q_PROPERTY(QObject* newlyAddedCategoryModel READ newlyAddedCategoryModel NOTIFY newlyAddedCategoryModelChanged)
    Q_PROPERTY(QObject* titleCategoryModel READ titleCategoryModel NOTIFY titleCategoryModelChanged)
    Q_PROPERTY(QObject* authorCategoryModel READ authorCategoryModel NOTIFY authorCategoryModelChanged)
    Q_PROPERTY(QObject* seriesCategoryModel READ seriesCategoryModel NOTIFY seriesCategoryModelChanged)
    Q_PROPERTY(QObject* folderCategoryModel READ folderCategoryModel NOTIFY folderCategoryModelChanged)
    Q_ENUMS(Grouping)
public:
    explicit BookListModel(QObject* parent = 0);
    virtual ~BookListModel();

    enum Grouping {
        GroupByNone = 0,
        GroupByRecentlyAdded,
        GroupByRecentlyRead,
        GroupByTitle,
        GroupByAuthor,
        GroupByPublisher
    };

    QObject* contentModel() const;
    void setContentModel(QObject* newModel);
    Q_SIGNAL void contentModelChanged();

    int count() const;
    Q_SIGNAL void countChanged();

    QObject* titleCategoryModel() const;
    Q_SIGNAL void titleCategoryModelChanged();

    QObject* newlyAddedCategoryModel() const;
    Q_SIGNAL void newlyAddedCategoryModelChanged();

    QObject* authorCategoryModel() const;
    Q_SIGNAL void authorCategoryModelChanged();

    QObject* seriesCategoryModel() const;
    Q_SIGNAL void seriesCategoryModelChanged();

    QObject* folderCategoryModel() const;
    Q_SIGNAL void folderCategoryModelChanged();

    // Update the data of a book at runtime - in particular, we need to update totalPages and currentPage
    Q_INVOKABLE void setBookData(QString fileName, QString property, QString value);
private:
    class Private;
    Private* d;

    Q_SLOT void contentModelItemsInserted(QModelIndex index,int first, int last);
};

#endif//BOOKLISTMODEL_H
