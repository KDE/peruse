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
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString publisher READ publisher WRITE setPublisher NOTIFY publisherChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
public:
    explicit BookModel(QObject* parent = 0);
    virtual ~BookModel();

    enum Roles {
        UrlRole = Qt::UserRole + 1,
        TitleRole,
    };

    virtual QHash<int, QByteArray> roleNames() const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual void addPage(QString url, QString title);

    QString filename() const;
    virtual void setFilename(QString newFilename);
    Q_SIGNAL void filenameChanged();
    virtual QString author() const;
    void setAuthor(QString newAuthor);
    Q_SIGNAL void authorChanged();
    virtual QString publisher() const;
    void setPublisher(QString newPublisher);
    Q_SIGNAL void publisherChanged();
    virtual int pageCount() const;
    Q_SIGNAL void pageCountChanged();

private:
    class Private;
    Private* d;
};

#endif//BOOKMODEL_H
