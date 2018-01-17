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
#include <QQmlListProperty>
#include <QQmlParserStatus>

#include "ContentQuery.h"

class ContentList : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "queries")
    Q_PROPERTY(QQmlListProperty<ContentQuery> queries READ queries)
    Q_PROPERTY(bool autoSearch READ autoSearch WRITE setAutoSearch NOTIFY autoSearchChanged)
    Q_PROPERTY(bool cacheResults READ cacheResults WRITE setCacheResults NOTIFY cacheResultsChanged)
public:
    explicit ContentList(QObject* parent = nullptr);
    ~ContentList() override;

    enum Roles {
        FilenameRole = Qt::UserRole + 1,
        FilePathRole,
        MetadataRole
    };

    QQmlListProperty<ContentQuery> queries();

    bool autoSearch() const;
    bool cacheResults() const;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    void classBegin() override;
    void componentComplete() override;

    Q_SLOT void setAutoSearch(bool autoSearch);
    Q_SLOT void setCacheResults(bool cacheResults);

    Q_SLOT void setKnownFiles(const QStringList& results);
    Q_SLOT void startSearch();

    Q_SIGNAL void autoSearchChanged();
    Q_SIGNAL void cacheResultsChanged();
    Q_SIGNAL void searchCompleted();

    Q_INVOKABLE static QString getMimetype(QString filePath);

private:
    bool isComplete() const;
    Q_SLOT void fileFound(const QString& filePath, const QVariantMap& metaData);

    class Private;
    Private* d;
};

#endif//CONTENTLISTBASE_H
