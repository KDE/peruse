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

#include "AcbfDocumentinfo.h"
#include "AcbfAuthor.h"

#include <QDebug>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class DocumentInfo::Private
{
public:
    Private() {}
    QList<Author*> author;
    QDate creationDate;
    QStringList source;
    QString id;
    QString version;
    QStringList history;
};

DocumentInfo::DocumentInfo(Metadata* parent)
    : QObject(parent)
    , d(new Private)
{
    qRegisterMetaType<DocumentInfo*>("DocumentInfo*");
}

DocumentInfo::~DocumentInfo() = default;

Metadata * DocumentInfo::metadata() const
{
    return qobject_cast<Metadata*>(parent());
}

void DocumentInfo::toXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement(QStringLiteral("document-info"));

    Q_FOREACH(Author* author, d->author) {
        author->toXml(writer);
    }

    writer->writeStartElement(QStringLiteral("creation-date"));
    writer->writeCharacters(d->creationDate.toString(QStringLiteral("MMMM d yyyy")));
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("source"));
    Q_FOREACH(const QString& source, d->source) {
        writer->writeStartElement(QStringLiteral("p"));
        writer->writeCharacters(source);
        writer->writeEndElement();
    }
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("id"));
    writer->writeCharacters(d->id);
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("version"));
    writer->writeCharacters(d->version);
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("history"));
    Q_FOREACH(const QString& history, d->history) {
        writer->writeStartElement(QStringLiteral("p"));
        writer->writeCharacters(history);
        writer->writeEndElement();
    }
    writer->writeEndElement();

    writer->writeEndElement();
}

bool DocumentInfo::fromXml(QXmlStreamReader *xmlReader)
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
        else if(xmlReader->name() == QStringLiteral("creation-date"))
        {
            QString date = xmlReader->attributes().value(QStringLiteral("value")).toString();
            if(date.isEmpty()) {
                date = xmlReader->readElementText();
            } else {
                xmlReader->skipCurrentElement();
            }
            setCreationDate(QDate::fromString(date));
        }
        else if(xmlReader->name() == QStringLiteral("source"))
        {
            while(xmlReader->readNextStartElement()) {
                if(xmlReader->name() == QStringLiteral("p")) {
                    d->source.append(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
                }
                else {
                    xmlReader->skipCurrentElement();
                }
            }
        }
        else if(xmlReader->name() == QStringLiteral("id"))
        {
            setId(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("version"))
        {
            setVersion(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("history"))
        {
            while(xmlReader->readNextStartElement()) {
                if(xmlReader->name() == QStringLiteral("p")) {
                    d->history.append(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
                }
                else {
                    xmlReader->skipCurrentElement();
                }
            }
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader->name();
            xmlReader->skipCurrentElement();
        }
    }
    if (xmlReader->hasError()) {
        qWarning() << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qDebug() << Q_FUNC_INFO << "Created document information for a document created on" << creationDate() << "at version" << version();
    return !xmlReader->hasError();
}

QList<Author *> DocumentInfo::author() const
{
    return d->author;
}

void DocumentInfo::addAuthor(Author* author)
{
    d->author.append(author);
}

void DocumentInfo::removeAuthor(Author* author)
{
    d->author.removeAll(author);
}

QDate DocumentInfo::creationDate() const
{
    return d->creationDate;
}

void DocumentInfo::setCreationDate(const QDate& creationDate)
{
    d->creationDate = creationDate;
}

QStringList DocumentInfo::source() const
{
    return d->source;
}

void DocumentInfo::setSource(const QStringList& source)
{
    d->source = source;
}

QString DocumentInfo::id() const
{
    return d->id;
}

void DocumentInfo::setId(const QString& id)
{
    d->id = id;
}

QString DocumentInfo::version() const
{
    return d->version;
}

void DocumentInfo::setVersion(const QString& version)
{
    d->version = version;
}

QStringList DocumentInfo::history() const
{
    return d->history;
}

void DocumentInfo::setHistory(const QStringList& history)
{
    d->history = history;
}

void DocumentInfo::addHistoryLine(const QString& historyLine)
{
    d->history.append(historyLine);
}
