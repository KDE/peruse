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

#include <QDir>

class BookDatabase::Private {
public:
    Private() {
        db = QSqlDatabase::addDatabase("QSQLITE");

        QDir location{QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)};
        if(!location.exists())
            location.mkpath(".");

        dbfile = location.absoluteFilePath("library.sqlite");
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
        if (!q.exec(QLatin1String("create table books(filename varchar primary key, filetitle varchar, title varchar, series varchar, author varchar, publisher varchar, created datetime, lastOpenedTime datetime, totalPages integer, currentPage integer, thumbnail varchar, description varchar, comment varchar, tags varchar, rating integer, seriesVolumes varchar, seriesNumbers varchar, genres varchar, keywords varchar, characters varchar)"))) {
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
    QSqlQuery allEntries("SELECT filename, filetitle, title, series, author, publisher, created, lastOpenedTime, totalPages, currentPage, thumbnail, description, comment, tags, rating, seriesNumbers, seriesVolumes, genres, keywords, characters FROM books");
    while(allEntries.next())
    {
        BookEntry* entry = new BookEntry();
        entry->filename = allEntries.value(0).toString();
        entry->filetitle = allEntries.value(1).toString();
        entry->title = allEntries.value(2).toString();
        entry->series = allEntries.value(3).toString().split(",", QString::SkipEmptyParts);
        entry->author = allEntries.value(4).toString().split(",", QString::SkipEmptyParts);
        entry->publisher = allEntries.value(5).toString();
        entry->created = allEntries.value(6).toDateTime();
        entry->lastOpenedTime = allEntries.value(7).toDateTime();
        entry->totalPages = allEntries.value(8).toInt();
        entry->currentPage = allEntries.value(9).toInt();
        entry->thumbnail = allEntries.value(10).toString();
        entry->description = allEntries.value(11).toString().split("\n", QString::SkipEmptyParts);
        entry->comment = allEntries.value(12).toString();
        entry->tags = allEntries.value(13).toString().split(",", QString::SkipEmptyParts);
        entry->rating = allEntries.value(14).toInt();
        entry->seriesNumbers = allEntries.value(15).toString().split(",", QString::SkipEmptyParts);
        entry->seriesVolumes = allEntries.value(16).toString().split(",", QString::SkipEmptyParts);
        entry->genres = allEntries.value(17).toString().split(",", QString::SkipEmptyParts);
        entry->keywords = allEntries.value(18).toString().split(",", QString::SkipEmptyParts);
        entry->characters = allEntries.value(19).toString().split(",", QString::SkipEmptyParts);
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
    newEntry.prepare("INSERT INTO books (filename, filetitle, title, series, author, publisher, created, lastOpenedTime, totalPages, currentPage, thumbnail, description, comment, tags, rating, seriesNumbers, seriesVolumes, genres, keywords, characters) "
                     "VALUES (:filename, :filetitle, :title, :series, :author, :publisher, :created, :lastOpenedTime, :totalPages, :currentPage, :thumbnail, :description, :comment, :tags, :rating, :seriesNumbers, :seriesVolumes, :genres, :keywords, :characters)");
    newEntry.bindValue(":filename", entry->filename);
    newEntry.bindValue(":filetitle", entry->filetitle);
    newEntry.bindValue(":title", entry->title);
    newEntry.bindValue(":series", entry->series.join(","));
    newEntry.bindValue(":author", entry->author.join(","));
    newEntry.bindValue(":publisher", entry->publisher);
    newEntry.bindValue(":publisher", entry->publisher);
    newEntry.bindValue(":created", entry->created);
    newEntry.bindValue(":lastOpenedTime", entry->lastOpenedTime);
    newEntry.bindValue(":totalPages", entry->totalPages);
    newEntry.bindValue(":currentPage", entry->currentPage);
    newEntry.bindValue(":thumbnail", entry->thumbnail);
    newEntry.bindValue(":description", entry->description.join("\n"));
    newEntry.bindValue(":comment", entry->comment);
    newEntry.bindValue(":tags", entry->tags.join(","));
    newEntry.bindValue(":rating", entry->rating);
    newEntry.bindValue(":seriesNumbers", entry->seriesNumbers.join(","));
    newEntry.bindValue(":seriesVolumes", entry->seriesVolumes.join(","));
    newEntry.bindValue(":genres", entry->genres.join(","));
    newEntry.bindValue(":keywords", entry->keywords.join(","));
    newEntry.bindValue(":characters", entry->characters.join(","));
    newEntry.exec();

    d->closeDb();
}

void BookDatabase::removeEntry(BookEntry* entry)
{
    if(!d->prepareDb()) {
        return;
    }
    qDebug() << "Removing book from the database" << entry->filename;

    QSqlQuery removeEntry;
    removeEntry.prepare("DELETE FROM books WHERE filename='"+entry->filename+"';");
    removeEntry.exec();

    d->closeDb();
}
