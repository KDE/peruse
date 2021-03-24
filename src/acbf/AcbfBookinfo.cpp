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

#include <QHash>
#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class BookInfo::Private
{
public:
    Private()
        : coverPage(nullptr)
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
    bool rightToLeft = false;
};

BookInfo::BookInfo(Metadata* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<BookInfo*>("BookInfo*");
    Q_UNUSED(typeId);
    d->coverPage = new Page(metadata()->document());
    d->coverPage->setIsCoverPage(true);
}

BookInfo::~BookInfo() = default;

Metadata * BookInfo::metadata()
{
    return qobject_cast<Metadata*>(parent());
}

void BookInfo::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement(QStringLiteral("book-info"));

    for(Author* author : d->author) {
        author->toXml(writer);
    }

    QHashIterator<QString, QString> titles(d->title);
    while(titles.hasNext()) {
        titles.next();
        writer->writeStartElement(QStringLiteral("book-title"));
        writer->writeAttribute(QStringLiteral("lang"), titles.key());
        writer->writeCharacters(titles.value());
        writer->writeEndElement();
    }

    QHashIterator<QString, int> genres(d->genre);
    while(genres.hasNext()) {
        genres.next();
        writer->writeStartElement(QStringLiteral("genre"));
        writer->writeAttribute(QStringLiteral("match"), QString::number(genres.value()));
        writer->writeCharacters(genres.key());
        writer->writeEndElement();
    }

    writer->writeStartElement("characters");
    writer->writeCharacters("");
    for(const QString& character : d->characters) {
        writer->writeStartElement("name");
        writer->writeCharacters(character);
        writer->writeEndElement();
    }
    writer->writeEndElement();

    QHashIterator<QString, QStringList> annotations(d->annotation);
    while(annotations.hasNext()) {
        annotations.next();
        writer->writeStartElement(QStringLiteral("annotation"));
        writer->writeAttribute(QStringLiteral("lang"), annotations.key());
        for(const QString& paragraph : annotations.value()) {
            writer->writeStartElement(QStringLiteral("p"));
            writer->writeCharacters("");  // to ensure we close the tag correctly and don't end up with a <p />
            writer->device()->write(paragraph.toUtf8().constData(), paragraph.toUtf8().length());
            writer->writeEndElement();
        }
        writer->writeEndElement();
    }

    QHashIterator<QString, QStringList> keywords(d->keywords);
    while(keywords.hasNext()) {
        keywords.next();
        writer->writeStartElement(QStringLiteral("keywords"));
        writer->writeAttribute(QStringLiteral("lang"), keywords.key());
        writer->writeCharacters(keywords.value().join(','));
        writer->writeEndElement();
    }

    d->coverPage->toXml(writer);

    writer->writeStartElement(QStringLiteral("languages"));
    for(Language* language : d->languages) {
        language->toXml(writer);
    }
    writer->writeEndElement();

    for(Sequence* sequence : d->sequence) {
        sequence->toXml(writer);
    }
    for(DatabaseRef* ref : d->databaseRef) {
        ref->toXml(writer);
    }
    for(ContentRating* rating : d->contentRating) {
        rating->toXml(writer);
    }
    
    //ACBF 1.2
    // Therefore only write this one when it is useful.
    if (d->rightToLeft) {
        writer->writeStartElement("reading-direction");
        writer->writeCharacters("RTL");
        writer->writeEndElement();
    }

    writer->writeEndElement();
}

bool BookInfo::fromXml(QXmlStreamReader *xmlReader, const QString& xmlData)
{
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("author"))
        {
            Author* newAuthor = new Author(metadata());
            if(!newAuthor->fromXml(xmlReader)) {
                return false;
            }
            d->author.append(newAuthor);
        }
        else if(xmlReader->name() == QStringLiteral("book-title"))
        {
            QString language = xmlReader->attributes().value(QStringLiteral("lang")).toString();
            d->title[language] = xmlReader->readElementText(QXmlStreamReader::IncludeChildElements);
        }
        else if(xmlReader->name() == QStringLiteral("genre"))
        {
            int match = xmlReader->attributes().value(QStringLiteral("match")).toInt();
            d->genre[xmlReader->readElementText(QXmlStreamReader::IncludeChildElements)] = match;
        }
        else if(xmlReader->name() == QStringLiteral("characters"))
        {
            while(xmlReader->readNextStartElement()) {
                if(xmlReader->name() == QStringLiteral("name")) {
                    d->characters.append(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
                }
                else {
                    xmlReader->skipCurrentElement();
                }
            }
            qCDebug(ACBF_LOG) << "Created character entries, we now have" << d->characters.count() << "characters";
        }
        else if(xmlReader->name() == QStringLiteral("annotation"))
        {
            QString language = xmlReader->attributes().value(QStringLiteral("lang")).toString();
            QStringList paragraphs;
            while(xmlReader->readNextStartElement()) {
                if(xmlReader->name() == QStringLiteral("p")) {
                    int startPoint = xmlReader->characterOffset();
                    int endPoint{startPoint};
                    while(xmlReader->readNext()) {
                        if (xmlReader->isEndElement() && xmlReader->name() == QStringLiteral("p")) {
                            endPoint = xmlReader->characterOffset();
                            break;
                        }
                    }
                    paragraphs.append(xmlData.mid(startPoint, endPoint - startPoint - 4));
                }
                else {
                    xmlReader->skipCurrentElement();
                }
            }
            d->annotation[language] = paragraphs;
        }
        else if(xmlReader->name() == QStringLiteral("keywords"))
        {
            QString language = xmlReader->attributes().value(QStringLiteral("lang")).toString();
            d->keywords[language] = xmlReader->readElementText(QXmlStreamReader::IncludeChildElements).split(',');
        }
        else if(xmlReader->name() == QStringLiteral("coverpage"))
        {
            if(!d->coverPage->fromXml(xmlReader, xmlData)) {
                return false;
            }
        }
        else if(xmlReader->name() == QStringLiteral("languages"))
        {
            while(xmlReader->readNextStartElement()) {
                if(xmlReader->name() == QStringLiteral("text-layer")) {
                    Language* newLanguage = new Language(this);
                    newLanguage->fromXml(xmlReader);
                    d->languages.append(newLanguage);
                }
                else {
                    xmlReader->skipCurrentElement();
                }
            }
        }
        else if(xmlReader->name() == QStringLiteral("sequence"))
        {
            Sequence* newSequence = new Sequence(this);
            newSequence->fromXml(xmlReader);
            d->sequence.append(newSequence);
        }
        else if(xmlReader->name() == QStringLiteral("databaseref"))
        {
            DatabaseRef* newDatabaseRef = new DatabaseRef(this);
            newDatabaseRef->fromXml(xmlReader);
            d->databaseRef.append(newDatabaseRef);
        }
        else if(xmlReader->name() == QStringLiteral("content-rating"))
        {
            ContentRating* newContentRating = new ContentRating(this);
            newContentRating->fromXml(xmlReader);
            d->contentRating.append(newContentRating);
        }
        else if(xmlReader->name() == QStringLiteral("reading-direction"))
        {
            QString direction = xmlReader->readElementText(QXmlStreamReader::IncludeChildElements).toLower();
            if (direction=="rtl") {
                setRightToLeft(true);
            } else {
                setRightToLeft(false);
            }
        }
        else
        {
            qCWarning(ACBF_LOG) << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader->name();
            xmlReader->skipCurrentElement();
        }
    }
    if (xmlReader->hasError()) {
        qCWarning(ACBF_LOG) << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created book information for the book with the titles" << d->title.values();
    return !xmlReader->hasError();
}

QList<Author*> BookInfo::author()
{
    return d->author;
}

QStringList BookInfo::authorNames() const
{
    QStringList names;
    for(Author* author : d->author) {
        names.append(author->displayName());
    }
    return names;
}

Author * BookInfo::getAuthor(int index) const
{
    return d->author.at(index);
}

void BookInfo::addAuthor(QString activity, QString language, QString firstName, QString middleName, QString lastName, QString nickName, QStringList homePages, QStringList emails)
{
    Author* author = new Author(metadata());
    author->setActivity(activity);
    author->setLanguage(language);
    author->setFirstName(firstName);
    author->setMiddleName(middleName);
    author->setLastName(lastName);
    author->setNickName(nickName);
    author->setHomePages(homePages);
    author->setEmails(emails);
    d->author.append(author);
    emit authorsChanged();
}

void BookInfo::setAuthor(int index, QString activity, QString language, QString firstName, QString middleName, QString lastName, QString nickName, QStringList homePages, QStringList emails)
{
    Author* author = d->author.at(index);
    author->setActivity(activity);
    author->setLanguage(language);
    author->setFirstName(firstName);
    author->setMiddleName(middleName);
    author->setLastName(lastName);
    author->setNickName(nickName);
    author->setHomePages(homePages);
    author->setEmails(emails);
    emit authorsChanged();
}

void BookInfo::removeAuthor(int index)
{
    d->author.removeAt(index);
    emit authorsChanged();
}

void BookInfo::addAuthor(Author* author)
{
    d->author.append(author);
    emit authorsChanged();
}

void BookInfo::removeAuthor(Author* author)
{
    d->author.removeAll(author);
    emit authorsChanged();
}

QStringList BookInfo::titleForAllLanguages()
{
    return d->title.values();
}

QStringList BookInfo::titleLanguages()
{
    return d->title.keys();
}

QString BookInfo::title(QString language)
{
    if (d->title.count()==0) {
        return "";
    }
    if (!d->title.keys().contains(language)) {
        language = "";
    }

    if(language.isEmpty() && d->title[language].isEmpty() && d->languages.count()>0) {
        language = d->languages.at(0)->language();
    }

    QString title = d->title.value(language);

    if (title.isEmpty()) {
        title = d->title.values().at(0);
    }

    return title;
}

void BookInfo::setTitle(QString title, QString language)
{
    // Don't allow removal of the default title, just everything else
    if(title.isEmpty() && !language.isEmpty())
    {
        d->title.remove(language);
    }
    else
    {
        d->title[language] = title;
    }
    emit titleChanged();
}

QHash<QString, int> BookInfo::genre()
{
    return d->genre;
}

QStringList BookInfo::genres() const
{
    return d->genre.keys();
}

int BookInfo::genrePercentage(QString genre) const
{
    return d->genre[genre];
}

void BookInfo::setGenre(QString genre, int matchPercentage)
{
    bool emitNewGenre = !d->genre.contains(genre);
    d->genre[genre] = matchPercentage;
    if(emitNewGenre)
    {
        emit genresChanged();
    }
}

void BookInfo::removeGenre(QString genre)
{
    d->genre.remove(genre);
    emit genresChanged();
}

QStringList BookInfo::availableGenres()
{
    return {
        QStringLiteral("adult"),
        QStringLiteral("adventure"),
        QStringLiteral("alternative"), // (abstract, underground ...)
        QStringLiteral("artbook"), // 1.2
        QStringLiteral("biography"), // (biographical comics)
        QStringLiteral("caricature"),
        QStringLiteral("children"),
        QStringLiteral("computer"), // (computers related)
        QStringLiteral("crime"),
        QStringLiteral("education"), // (education and science)
        QStringLiteral("fantasy"),
        QStringLiteral("history"), // (historical comics)
        QStringLiteral("horror"),
        QStringLiteral("humor"),
        QStringLiteral("manga"),
        QStringLiteral("military"), // (war ...)
        QStringLiteral("mystery"),
        QStringLiteral("non-fiction"),
        QStringLiteral("politics"),
        QStringLiteral("real_life"),
        QStringLiteral("religion"),
        QStringLiteral("romance"),
        QStringLiteral("science_fiction"),
        QStringLiteral("sports"),
        QStringLiteral("superhero"), // (e.g. Superman, Spiderman … or Super Villains)
        QStringLiteral("western"),
        QStringLiteral("other")
    };
}


QStringList BookInfo::characters()
{
    return d->characters;
}

void BookInfo::addCharacter(QString name)
{
    d->characters.append(name);
    emit charactersChanged();
}

void BookInfo::removeCharacter(QString name)
{
    d->characters.removeAll(name);
    emit charactersChanged();
}

void BookInfo::setCharacters(QStringList characters)
{
    d->characters = characters;
    emit charactersChanged();
}

QList<QStringList> BookInfo::annotationsForAllLanguage()
{
    return d->annotation.values();
}

QStringList BookInfo::annotationLanguages()
{
    return d->annotation.keys();
}

QStringList BookInfo::annotation(QString language)
{
    if (d->annotation.isEmpty()) {
        return QStringList("");
    }
    if (!d->annotation.keys().contains(language)) {
        language = "";
    }

    if(language.isEmpty() && d->annotation.value(language).count()==0) {
        language = d->languages.at(0)->language();
    }

    QStringList annotation = d->annotation.value(language);

    if (annotation.count()==0) {
        annotation = d->annotation.values().at(0);
    }

    return annotation;
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
    if (d->keywords.isEmpty()) {
        return QStringList("");
    }

    if (!d->keywords.keys().contains(language)) {
        language = "";
    }

    if(language.isEmpty() && d->keywords.value(language).count()==0) {
        language = d->languages.at(0)->language();
    }

    QStringList keywords = d->keywords.value(language);

    if (keywords.count()==0) {
        keywords = d->keywords.values().at(0);
    }

    return keywords;
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

QObjectList BookInfo::languages()
{
    QObjectList list;
    for (Language* language : d->languages) {
        list << language;
    }
    return list;
}

void BookInfo::addLanguage(Language* language)
{
    d->languages.append(language);
    Q_EMIT languagesChanged();
}

QStringList BookInfo::languageEntryList()
{
    QStringList lang;
    for (int i=0; i<d->languages.size(); i++) {
        lang.append(d->languages.at(i)->language());
    }
    return lang;
}

void BookInfo::addLanguage(QString language, bool show)
{
    Language* lang = new Language(this);
    lang->setLanguage(language);
    lang->setShow(show);
    addLanguage(lang);
}

void BookInfo::removeLanguage(Language* language)
{
    d->languages.removeAll(language);
    Q_EMIT languagesChanged();
}

void BookInfo::removeLanguage(int index)
{
    removeLanguage(d->languages.at(index));
}

QList<Sequence *> BookInfo::sequence()
{
    return d->sequence;
}

void BookInfo::addSequence(Sequence* sequence)
{
    d->sequence.append(sequence);
    emit sequenceCountChanged();
}

void BookInfo::addSequence(int number, QString title, int volume)
{
    Sequence* sequence = new Sequence(this);
    sequence->setNumber(number);
    sequence->setTitle(title);
    sequence->setVolume(volume);
    addSequence(sequence);
}

void BookInfo::removeSequence(Sequence* sequence)
{
    d->sequence.removeAll(sequence);
    emit sequenceCountChanged();
}

void BookInfo::removeSequence(int index)
{
    removeSequence(sequence(index));
}

Sequence *BookInfo::sequence(int index) const
{
    return d->sequence.at(index);
}

int BookInfo::sequenceCount() const
{

    return d->sequence.size();
}

QList<DatabaseRef *> BookInfo::databaseRef()
{
    return d->databaseRef;
}


void BookInfo::addDatabaseRef(DatabaseRef* databaseRef)
{
    d->databaseRef.append(databaseRef);
    emit databaseRefCountChanged();
}

void BookInfo::addDatabaseRef(QString reference, QString dbname, QString type)
{
    DatabaseRef* dRef = new DatabaseRef(this);
    dRef->setReference(reference);
    dRef->setDbname(dbname);
    dRef->setType(type);
    addDatabaseRef(dRef);
}

void BookInfo::removeDatabaseRef(DatabaseRef* databaseRef)
{
    d->databaseRef.removeAll(databaseRef);
    emit databaseRefCountChanged();
}

void BookInfo::removeDatabaseRef(int index)
{
    removeDatabaseRef(databaseRef(index));
}

DatabaseRef *BookInfo::databaseRef(int index)
{
    return d->databaseRef.at(index);
}

int BookInfo::databaseRefCount()
{
    return d->databaseRef.size();
}

QList<ContentRating *> BookInfo::contentRating()
{
    return d->contentRating;
}

void BookInfo::addContentRating(ContentRating* contentRating)
{
    d->contentRating.append(contentRating);
    emit contentRatingCountChanged();
}

void BookInfo::addContentRating(QString rating, QString type)
{
    ContentRating* CR = new ContentRating(this);
    CR->setRating(rating);
    CR->setType(type);
    addContentRating(CR);
}

void BookInfo::removeContentRating(ContentRating* contentRating)
{
    d->contentRating.removeAll(contentRating);
    emit contentRatingCountChanged();
}

void BookInfo::removeContentRating(int index)
{
    removeContentRating(contentRating(index));
}

ContentRating *BookInfo::contentRating(int index)
{
    return d->contentRating.at(index);
}

int BookInfo::contentRatingCount()
{
    return d->contentRating.size();
}
bool BookInfo::rightToLeft() const
{
    return d->rightToLeft;
}

void BookInfo::setRightToLeft(const bool& rtl) {
    d->rightToLeft = rtl;
    emit rightToLeftChanged();
}
