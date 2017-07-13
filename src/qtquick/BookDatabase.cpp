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

#include "BookDatabase.h"

#include "CategoryEntriesModel.h"

#include <QDebug>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

class BookDatabase::Private {
public:
    Private() {
        db = QSqlDatabase::addDatabase("QSQLITE");
        dbfile = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/library.sqlite";
        db.setDatabaseName(dbfile);
    }

    QSqlDatabase db;
    QString dbfile;

    bool prepareDb() {
        if (!db.open()) {
            qDebug() << "Failed to open the book database file" << dbfile << db.lastError();
            return false;
        }

        QStringList tables = db.tables();
        if (tables.contains("books", Qt::CaseInsensitive))
            return true;

        QSqlQuery q;
        if (!q.exec(QLatin1String("create table books(filename varchar primary key, filetitle varchar, title varchar, series varchar, author varchar, publisher varchar, created datetime, lastOpenedTime datetime, totalPages integer, currentPage integer, thumbnail varchar)"))) {
            qDebug() << "Database could not create the table books";
            return false;
        }

        return true;
    }

    void closeDb() {
        db.close();
    }
};

BookDatabase::BookDatabase(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

BookDatabase::~BookDatabase()
{
    delete d;
}

QList<BookEntry*> BookDatabase::loadEntries()
{
    if(!d->prepareDb()) {
        return QList<BookEntry*>();
    }

    QList<BookEntry*> entries;
    QSqlQuery allEntries("SELECT filename, filetitle, title, series, author, publisher, created, lastOpenedTime, totalPages, currentPage, thumbnail FROM books");
    while(allEntries.next())
    {
        BookEntry* entry = new BookEntry();
        entry->filename = allEntries.value(0).toString();
        entry->filetitle = allEntries.value(1).toString();
        entry->title = allEntries.value(2).toString();
        entry->series = allEntries.value(3).toString();
        entry->author = allEntries.value(4).toString();
        entry->publisher = allEntries.value(5).toString();
        entry->created = allEntries.value(6).toDateTime();
        entry->lastOpenedTime = allEntries.value(7).toDateTime();
        entry->totalPages = allEntries.value(8).toInt();
        entry->currentPage = allEntries.value(9).toInt();
        entry->thumbnail = allEntries.value(10).toString();
        entries.append(entry);
    }

    d->closeDb();
    return entries;
}

void BookDatabase::addEntry(BookEntry* entry)
{
    if(!d->prepareDb()) {
        return;
    }
    qDebug() << "Adding newly discovered book to the database" << entry->filename;

    QSqlQuery newEntry;
    newEntry.prepare("INSERT INTO books (filename, filetitle, title, series, author, publisher, created, lastOpenedTime, totalPages, currentPage, thumbnail) "
                     "VALUES (:filename, :filetitle, :title, :series, :author, :publisher, :created, :lastOpenedTime, :totalPages, :currentPage, :thumbnail)");
    newEntry.bindValue(":filename", entry->filename);
    newEntry.bindValue(":filetitle", entry->filetitle);
    newEntry.bindValue(":title", entry->title);
    newEntry.bindValue(":series", entry->series);
    newEntry.bindValue(":author", entry->author);
    newEntry.bindValue(":publisher", entry->publisher);
    newEntry.bindValue(":publisher", entry->publisher);
    newEntry.bindValue(":created", entry->created);
    newEntry.bindValue(":lastOpenedTime", entry->lastOpenedTime);
    newEntry.bindValue(":totalPages", entry->totalPages);
    newEntry.bindValue(":currentPage", entry->currentPage);
    newEntry.bindValue(":thumbnail", entry->thumbnail);
    newEntry.exec();

    d->closeDb();
}
