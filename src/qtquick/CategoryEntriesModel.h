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

#ifndef CATEGORYENTRIESMODEL_H
#define CATEGORYENTRIESMODEL_H

#include <QAbstractListModel>
#include <QDateTime>

class CategoryEntriesModel;
struct BookEntry {
    BookEntry()
        : totalPages(0)
        , currentPage(0)
    {}
    QString filename;
    QString filetitle;
    QString title;
    QString series;
    QString author;
    QString publisher;
    QDateTime created;
    QDateTime lastOpenedTime;
    int totalPages;
    int currentPage;
};

class CategoryEntriesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit CategoryEntriesModel(QObject* parent = 0);
    virtual ~CategoryEntriesModel();

    enum Roles {
        FilenameRole = Qt::UserRole + 1,
        FiletitleRole,
        TitleRole,
        SeriesRole,
        AuthorRole,
        PublisherRole,
        CreatedRole,
        LastOpenedTimeRole,
        TotalPagesRole,
        CurrentPageRole,
        CategoryEntriesModelRole,
        CategoryEntryCountRole
    };

    virtual QHash<int, QByteArray> roleNames() const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    void append(BookEntry* entry, Roles compareRole = TitleRole);
    void addCategoryEntry(const QString& categoryName, BookEntry* entry);

    Q_INVOKABLE QObject* get(int index);
    // This is backwards... need to fox this to make get return the actual thing, not just a book, and create a getter for books...
    Q_INVOKABLE QObject* getEntry(int index);
    Q_INVOKABLE int indexOfFile(QString filename);
    Q_INVOKABLE bool indexIsBook(int index);

    Q_SIGNAL void entryDataUpdated(BookEntry* entry);
    Q_SLOT void entryDataChanged(BookEntry* entry);
    Q_SIGNAL void entryRemoved(BookEntry* entry);
    Q_SLOT void entryRemove(BookEntry* entry);
protected:
    QString name() const;
    void setName(const QString& newName);
private:
    class Private;
    Private* d;
};

#endif//CATEGORYENTRIESMODEL_H
