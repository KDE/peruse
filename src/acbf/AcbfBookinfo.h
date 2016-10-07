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

#ifndef ACBFBOOKINFO_H
#define ACBFBOOKINFO_H

#include "AcbfMetadata.h"

#include <QHash>

namespace AdvancedComicBookFormat
{
class Author;
class Page;
class Language;
class Sequence;
class DatabaseRef;
class ContentRating;
class ACBF_EXPORT BookInfo : public QObject
{
    Q_OBJECT
public:
    explicit BookInfo(Metadata* parent = 0);
    virtual ~BookInfo();

    Metadata* metadata();

    void toXml(QXmlStreamWriter *writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QList<Author*> author();
    void addAuthor(Author* author);
    void removeAuthor(Author* author);

    QStringList titleForAllLanguages();
    QString title(QString language = "");
    void setTitle(QString title, QString language = "");

    QHash<QString, int> genre();
    void setGenre(QString genre, int matchPercentage = 100);
    void removeGenre(QString genre);
    static QStringList availableGenres();

    QStringList characters();
    void addCharacter(QString name);
    void removeCharacter(QString name);

    QHash<QString, QStringList> annotationsForAllLanguage();
    QStringList annotation(QString language = ""); // empty string means "default language", as (un)defined by the specification...
    void setAnnotation(QStringList annotation, QString language = "");

    QHash<QString, QStringList> keywordsForAllLanguage();
    QStringList keywords(QString language = "");
    void setKeywords(QStringList keywords, QString language = "");

    Page* coverpage();
    void setCoverpage(Page* newCover);

    QList<Language*> languages();
    void addLanguage(Language* language);
    void removeLanguage(Language* language);

    QList<Sequence*> sequence();
    void addSequence(Sequence* sequence);
    void removeSequence(Sequence* sequence);

    QList<DatabaseRef*> databaseRef();
    void addDatabaseRef(DatabaseRef* databaseRef);
    void removeDatabaseRef(DatabaseRef* databaseRef);

    QList<ContentRating*> contentRating();
    void addContentRating(ContentRating* contentRating);
    void removeContentRating(ContentRating* contentRating);
private:
    class Private;
    Private* d;
};
}

#endif//ACBFBOOKINFO_H
