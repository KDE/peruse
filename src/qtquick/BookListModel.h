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

#ifndef BOOKMODEL_H
#define BOOKMODEL_H

#include <QAbstractListModel>

class BookModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* contentModel READ contentModel WRITE setContentModel NOTIFY contentModelChanged)
    Q_ENUMS(Grouping)
public:
    explicit BookModel(QObject* parent = 0);
    virtual ~BookModel();

    enum Grouping {
        GroupByNone = 0,
        GroupByRecentlyAdded,
        GroupByRecentlyRead,
        GroupByTitle,
        GroupByAuthor,
        GroupByPublisher
    };

    enum Roles {
        FilenameRole = Qt::UserRole + 1,
        TitleRole,
        AuthorRole,
        PublisherRole,
        LastOpenedTimeRole,
        TotalPagesRole,
        CurrentPageRole
    };

    virtual QHash<int, QByteArray> roleNames() const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    QObject* contentModel() const;
    void setContentModel(QObject* newModel);
    Q_SIGNAL void contentModelChanged();
private:
    class Private;
    Private* d;

    Q_SLOT void contentModelItemsInserted(QModelIndex index,int first, int last);
};

#endif//BOOKMODEL_H
