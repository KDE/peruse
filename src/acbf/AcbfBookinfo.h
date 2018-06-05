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

#include <memory>

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
    Q_PROPERTY(QStringList authorNames READ authorNames NOTIFY authorsChanged)
    Q_PROPERTY(QStringList titleLanguages READ titleLanguages NOTIFY titleChanged)
    Q_PROPERTY(QStringList genres READ genres NOTIFY genresChanged)
    Q_PROPERTY(QStringList characters READ characters NOTIFY charactersChanged)
public:
    explicit BookInfo(Metadata* parent = nullptr);
    ~BookInfo() override;

    Metadata* metadata();

    void toXml(QXmlStreamWriter *writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QList<Author*> author();
    QStringList authorNames() const;
    Q_INVOKABLE Author* getAuthor(int index) const;
    Q_INVOKABLE void addAuthor(QString activity, QString language, QString firstName, QString middleName, QString lastName, QString nickName, QString homePage, QString email);
    Q_INVOKABLE void setAuthor(int index, QString activity, QString language, QString firstName, QString middleName, QString lastName, QString nickName, QString homePage, QString email);
    Q_INVOKABLE void removeAuthor(int index);
    void addAuthor(Author* author);
    void removeAuthor(Author* author);
    Q_SIGNAL void authorsChanged();

    Q_INVOKABLE QStringList titleForAllLanguages();
    Q_INVOKABLE QStringList titleLanguages();
    Q_INVOKABLE QString title(QString language = "");
    Q_INVOKABLE void setTitle(QString title, QString language = "");
    Q_SIGNAL void titleChanged();

    Q_INVOKABLE QHash<QString, int> genre();
    Q_INVOKABLE QStringList genres() const;
    Q_INVOKABLE int genrePercentage(QString genre) const;
    Q_INVOKABLE void setGenre(QString genre, int matchPercentage = 100);
    Q_INVOKABLE void removeGenre(QString genre);
    Q_SIGNAL void genresChanged();
    Q_INVOKABLE static QStringList availableGenres();

    Q_INVOKABLE QStringList characters();
    Q_INVOKABLE void addCharacter(QString name);
    Q_INVOKABLE void removeCharacter(QString name);
    Q_SIGNAL void charactersChanged();

    Q_INVOKABLE QList<QStringList> annotationsForAllLanguage();
    Q_INVOKABLE QStringList annotationLanguages();
    Q_INVOKABLE QStringList annotation(QString language = ""); // empty string means "default language", as (un)defined by the specification...
    Q_INVOKABLE void setAnnotation(QStringList annotation, QString language = "");

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
    std::unique_ptr<Private> d;
};
}

#endif//ACBFBOOKINFO_H
