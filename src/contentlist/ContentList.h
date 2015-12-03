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

#ifndef CONTENTLISTBASE_H
#define CONTENTLISTBASE_H

#include <QAbstractListModel>

class ContentList : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ContentList(QObject* parent = 0);
    virtual ~ContentList();

    Q_INVOKABLE static QString getMimetype(QString filePath);

    Q_SLOT void addLocation(QString path);
    Q_SLOT void addMimetype(QString mimetype);
    Q_SLOT void setSearchString(const QString& searchString);
    Q_SLOT void startSearch();

    enum Roles {
        FilenameRole = Qt::UserRole + 1,
        MetadataRole
    };

    virtual QHash<int, QByteArray> roleNames() const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    Q_SLOT void fileFound(const QString& filePath, const QVariantHash& metaData);
private:
    class Private;
    Private* d;
};

#endif//CONTENTLISTBASE_H
