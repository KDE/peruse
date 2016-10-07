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

#include "AcbfBookinfo.h"
#include "AcbfAuthor.h"
#include "AcbfContentrating.h"
#include "AcbfDatabaseref.h"
#include "AcbfLanguage.h"
#include "AcbfPage.h"
#include "AcbfSequence.h"

#include <QDebug>
#include <QHash>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class BookInfo::Private
{
public:
    Private()
        : coverPage(0)
    {}
    QList<Author*> author;
    QHash<QString, QString> title;
    QHash<QString, int> genre;
    QList<QString> characters;
    QHash<QString, QStringList> annotation;
    QHash<QString, QStringList> keywords;
    Page* coverPage;
    QList<Language*> languages;
    QList<Sequence*> sequence;
    QList<DatabaseRef*> databaseRef;
    QList<ContentRating*> contentRating;
};

BookInfo::BookInfo(Metadata* parent)
    : QObject(parent)
    , d(new Private)
{
    d->coverPage = new Page(metadata()->document());
    d->coverPage->setIsCoverPage(true);
}

BookInfo::~BookInfo()
{
    delete d;
}

Metadata * BookInfo::metadata()
{
    return qobject_cast<Metadata*>(parent());
}

void BookInfo::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement("book-info");

    Q_FOREACH(Author* author, d->author) {
        author->toXml(writer);
    }

    QHashIterator<QString, QString> titles(d->title);
    while(titles.hasNext()) {
        titles.next();
        writer->writeStartElement("book-title");
        writer->writeAttribute("lang", titles.key());
        writer->writeCharacters(titles.value());
        writer->writeEndElement();
    }

    QHashIterator<QString, int> genres(d->genre);
    while(genres.hasNext()) {
        genres.next();
        writer->writeStartElement("genre");
        writer->writeAttribute("match", QString::number(genres.value()));
        writer->writeCharacters(genres.key());
        writer->writeEndElement();
    }

    writer->writeStartElement("character");
    writer->writeCharacters("");
    Q_FOREACH(const QString& character, d->characters) {
        writer->writeStartElement("name");
        writer->writeCharacters(character);
        writer->writeEndElement();
    }
    writer->writeEndElement();

    QHashIterator<QString, QStringList> annotations(d->annotation);
    while(annotations.hasNext()) {
        annotations.next();
        writer->writeStartElement("annotation");
        writer->writeAttribute("lang", annotations.key());
        Q_FOREACH(const QString& paragraph, annotations.value()) {
            writer->writeStartElement("p");
            writer->writeCharacters(paragraph);
            writer->writeEndElement();
        }
        writer->writeEndElement();
    }

    QHashIterator<QString, QStringList> keywords(d->keywords);
    while(keywords.hasNext()) {
        keywords.next();
        writer->writeStartElement("keywords");
        writer->writeAttribute("lang", keywords.key());
        writer->writeCharacters(keywords.value().join(','));
        writer->writeEndElement();
    }

    d->coverPage->toXml(writer);

    writer->writeStartElement("languages");
    Q_FOREACH(Language* language, d->languages) {
        language->toXml(writer);
    }
    writer->writeEndElement();

    Q_FOREACH(Sequence* sequence, d->sequence) {
        sequence->toXml(writer);
    }
    Q_FOREACH(DatabaseRef* ref, d->databaseRef) {
        ref->toXml(writer);
    }
    Q_FOREACH(ContentRating* rating, d->contentRating) {
        rating->toXml(writer);
    }

    writer->writeEndElement();
}

bool BookInfo::fromXml(QXmlStreamReader *xmlReader)
{
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == "author")
        {
            Author* newAuthor = new Author(metadata());
            if(!newAuthor->fromXml(xmlReader)) {
                return false;
            }
            d->author.append(newAuthor);
        }
        else if(xmlReader->name() == "book-title")
        {
            QString language = xmlReader->attributes().value("lang").toString();
            d->title[language] = xmlReader->readElementText(QXmlStreamReader::IncludeChildElements);
        }
        else if(xmlReader->name() == "genre")
        {
            int match = xmlReader->attributes().value("match").toInt();
            d->genre[xmlReader->readElementText(QXmlStreamReader::IncludeChildElements)] = match;
        }
        else if(xmlReader->name() == "character")
        {
            while(xmlReader->readNextStartElement()) {
                if(xmlReader->name() == "name") {
                    d->characters.append(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
                }
                if(xmlReader->readNext() == QXmlStreamReader::EndElement && xmlReader->name() == "character") {
                    break;
                }
            }
        }
        else if(xmlReader->name() == "annotation")
        {
            QString language = xmlReader->attributes().value("lang").toString();
            QStringList paragraphs;
            while(xmlReader->readNextStartElement()) {
                if(xmlReader->name() == "p") {
                    paragraphs.append(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
                }
                if(xmlReader->readNext() == QXmlStreamReader::EndElement && xmlReader->name() == "character") {
                    break;
                }
            }
            d->annotation[language] = paragraphs;
        }
        else if(xmlReader->name() == "keywords")
        {
            QString language = xmlReader->attributes().value("lang").toString();
            d->keywords[language] = xmlReader->readElementText(QXmlStreamReader::IncludeChildElements).split(',');
        }
        else if(xmlReader->name() == "coverpage")
        {
            if(!d->coverPage->fromXml(xmlReader)) {
                return false;
            }
        }
        else if(xmlReader->name() == "languages")
        {
            while(xmlReader->readNextStartElement()) {
                if(xmlReader->name() == "text-layer") {
                    Language* newLanguage = new Language(this);
                    newLanguage->fromXml(xmlReader);
                    d->languages.append(newLanguage);
                }
                if(xmlReader->readNext() == QXmlStreamReader::EndElement && xmlReader->name() == "languages") {
                    break;
                }
            }
        }
        else if(xmlReader->name() == "sequence")
        {
            Sequence* newSequence = new Sequence(this);
            newSequence->fromXml(xmlReader);
            d->sequence.append(newSequence);
        }
        else if(xmlReader->name() == "databaseref")
        {
            DatabaseRef* newDatabaseRef = new DatabaseRef(this);
            newDatabaseRef->fromXml(xmlReader);
            d->databaseRef.append(newDatabaseRef);
        }
        else if(xmlReader->name() == "content-rating")
        {
            ContentRating* newContentRating = new ContentRating(this);
            newContentRating->fromXml(xmlReader);
            d->contentRating.append(newContentRating);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader->name();
            xmlReader->skipCurrentElement();
        }
        if(xmlReader->readNext() == QXmlStreamReader::EndElement && xmlReader->name() == "book-info") {
            break;
        }
    }
    if (xmlReader->hasError()) {
        qWarning() << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    return !xmlReader->hasError();
}

QList<Author *> BookInfo::author()
{
    return d->author;
}

void BookInfo::addAuthor(Author* author)
{
    d->author.append(author);
}

void BookInfo::removeAuthor(Author* author)
{
    d->author.removeAll(author);
}

QStringList BookInfo::titleForAllLanguages()
{
    return d->title.values();
}

QString BookInfo::title(QString language)
{
    return d->title.value(language);
}

void BookInfo::setTitle(QString title, QString language)
{
    d->title[language] = title;
}

QHash<QString, int> BookInfo::genre()
{
    return d->genre;
}

void BookInfo::setGenre(QString genre, int matchPercentage)
{
    d->genre[genre] = matchPercentage;
}

void BookInfo::removeGenre(QString genre)
{
    d->genre.remove(genre);
}

QStringList BookInfo::availableGenres()
{
    QStringList genres;
    genres << "science_fiction";
    genres << "fantasy";
    genres << "adventure";
    genres << "horror";
    genres << "mystery";
    genres << "crime";
    genres << "military"; // (war ...)
    genres << "real_life";
    genres << "superhero"; // (e.g. Superman, Spiderman … or Super Villains)
    genres << "humor";
    genres << "western";
    genres << "manga";
    genres << "politics";
    genres << "caricature";
    genres << "sports";
    genres << "history"; // (historical comics)
    genres << "biography"; // (biographical comics)
    genres << "education"; // (education and science)
    genres << "computer"; // (computers related)
    genres << "religion";
    genres << "romance";
    genres << "children";
    genres << "non-fiction";
    genres << "adult";
    genres << "alternative"; // (abstract, underground ...)
    genres << "other";
    return genres;
}

QStringList BookInfo::characters()
{
    return d->characters;
}

void BookInfo::addCharacter(QString name)
{
    d->characters.append(name);
}

void BookInfo::removeCharacter(QString name)
{
    d->characters.removeAll(name);
}

QHash<QString, QStringList> BookInfo::annotationsForAllLanguage()
{
    return d->annotation;
}

QStringList BookInfo::annotation(QString language)
{
    return d->annotation.value(language);
}

void BookInfo::setAnnotation(QStringList annotation, QString language)
{
    d->annotation[language] = annotation;
}

QHash<QString, QStringList> BookInfo::keywordsForAllLanguage()
{
    return d->keywords;
}

QStringList BookInfo::keywords(QString language)
{
    return d->keywords.value(language);
}

void BookInfo::setKeywords(QStringList keywords, QString language)
{
    d->keywords[language] = keywords;
}

Page * BookInfo::coverpage()
{
    return d->coverPage;
}

void BookInfo::setCoverpage(Page* newCover)
{
    d->coverPage = newCover;
}

QList<Language *> BookInfo::languages()
{
    return d->languages;
}

void BookInfo::addLanguage(Language* language)
{
    d->languages.append(language);
}

void BookInfo::removeLanguage(Language* language)
{
    d->languages.removeAll(language);
}

QList<Sequence *> BookInfo::sequence()
{
    return d->sequence;
}

void BookInfo::addSequence(Sequence* sequence)
{
    d->sequence.append(sequence);
}

void BookInfo::removeSequence(Sequence* sequence)
{
    d->sequence.removeAll(sequence);
}

QList<DatabaseRef *> BookInfo::databaseRef()
{
    return d->databaseRef;
}

void BookInfo::addDatabaseRef(DatabaseRef* databaseRef)
{
    d->databaseRef.append(databaseRef);
}

void BookInfo::removeDatabaseRef(DatabaseRef* databaseRef)
{
    d->databaseRef.removeAll(databaseRef);
}

QList<ContentRating *> BookInfo::contentRating()
{
    return d->contentRating;
}

void BookInfo::addContentRating(ContentRating* contentRating)
{
    d->contentRating.append(contentRating);
}

void BookInfo::removeContentRating(ContentRating* contentRating)
{
    d->contentRating.removeAll(contentRating);
}
