/*
 * Copyright (C) 2017 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef BOOKDATABASE_H
#define BOOKDATABASE_H

#include <QObject>

struct BookEntry;
/**
 * \brief A Class to hold a cache of known books to reduce the amount of time spent indexing.
 * 
 * BookDatabase handles holding the conversion between SQL entry and
 * BookEntry structs.
 * 
 * The BookEntry struct is defined in CategoryEntriesModel.
 */
class BookDatabase : public QObject
{
    Q_OBJECT
public:
    explicit BookDatabase(QObject* parent = nullptr);
    ~BookDatabase() override;

    /**
     * @return a list of all known books in the database.
     */
    QList<BookEntry*> loadEntries();
    /**
     * \brief Add a new book to the cache.
     * @param entry The entry to add.
     */
    void addEntry(BookEntry* entry);
    /**
     * @brief remove an entry by filename from the cache.
     * @param entry the entry to remove.
     */
    void removeEntry(BookEntry* entry);
    /**
     * @brief updateEntry update an entry by filename.
     * @param fileName the filename of the entry to update.
     * @param property the property/fieldname you wish to update.
     * @param value a QVariant with the value.
     */
    void updateEntry(QString fileName, QString property, QVariant value);
private:
    class Private;
    Private* d;
};

#endif//BOOKDATABASE_H
