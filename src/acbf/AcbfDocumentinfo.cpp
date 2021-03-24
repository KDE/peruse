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

#include <QXmlStreamReader>
#include <QUuid>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class DocumentInfo::Private
{
public:
    Private() {}
    QList<Author*> author;
    QDate creationDate;
    QStringList source;
    QString id;
    float version;
    QStringList history;
};

DocumentInfo::DocumentInfo(Metadata* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<DocumentInfo*>("DocumentInfo*");
    Q_UNUSED(typeId);
}

DocumentInfo::~DocumentInfo() = default;

Metadata * DocumentInfo::metadata() const
{
    return qobject_cast<Metadata*>(parent());
}

void DocumentInfo::toXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement(QStringLiteral("document-info"));

    for(Author* author : d->author) {
        author->toXml(writer);
    }

    writer->writeStartElement(QStringLiteral("creation-date"));
    writer->writeCharacters(QDate::currentDate().toString(QStringLiteral("MMMM d yyyy")));
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("source"));
    for(const QString& source : d->source) {
        writer->writeStartElement(QStringLiteral("p"));
        writer->writeCharacters("");  // to ensure we close the tag correctly and don't end up with a <p />
        writer->device()->write(source.toUtf8().constData(), source.toUtf8().length());
        writer->writeEndElement();
    }
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("id"));
    writer->writeCharacters(id());
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("version"));
    writer->writeCharacters(QString::number(double(d->version)));
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("history"));
    for(const QString& history : d->history) {
        writer->writeStartElement(QStringLiteral("p"));
        writer->writeCharacters("");  // to ensure we close the tag correctly and don't end up with a <p />
        writer->device()->write(history.toUtf8().constData(), history.toUtf8().length());
        writer->writeEndElement();
    }
    writer->writeEndElement();

    writer->writeEndElement();
}

bool DocumentInfo::fromXml(QXmlStreamReader *xmlReader, const QString& xmlData)
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
                    int startPoint = xmlReader->characterOffset();
                    int endPoint{startPoint};
                    while(xmlReader->readNext()) {
                        if (xmlReader->isEndElement() && xmlReader->name() == QStringLiteral("p")) {
                            endPoint = xmlReader->characterOffset();
                            break;
                        }
                    }
                    d->source.append(xmlData.mid(startPoint, endPoint - startPoint - 4));
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
            setVersion(QString(xmlReader->readElementText()).toFloat());
        }
        else if(xmlReader->name() == QStringLiteral("history"))
        {
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
                    d->history.append(xmlData.mid(startPoint, endPoint - startPoint - 4));
                }
                else {
                    xmlReader->skipCurrentElement();
                }
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
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created document information for a document created on" << creationDate() << "at version" << version();
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

QStringList DocumentInfo::authorNames() const
{
    QStringList names;
    for(Author* author : d->author) {
        names.append(author->displayName());
    }
    return names;
}

Author *DocumentInfo::getAuthor(int index) const
{
    return d->author.at(index);
}

void DocumentInfo::addAuthor(QString activity, QString language, QString firstName, QString middleName, QString lastName, QString nickName, QStringList homePages, QStringList emails)
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

void DocumentInfo::setAuthor(int index, QString activity, QString language, QString firstName, QString middleName, QString lastName, QString nickName, QStringList homePages, QStringList emails)
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

void DocumentInfo::removeAuthor(int index)
{
    removeAuthor(d->author.at(index));
    emit authorsChanged();
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
    emit sourceChanged();
}

void DocumentInfo::removeSource(int index)
{
    d->source.removeAt(index);
    emit sourceChanged();
}

QString DocumentInfo::id() const
{
    // If ID is empty we ought to generate one.
    if (d->id.isEmpty()) {
        d->id = QUuid::createUuid().toString();
    }
    return d->id;
}

void DocumentInfo::setId(const QString& id)
{
    d->id = id;
}

float DocumentInfo::version() const
{
    return d->version;
}

void DocumentInfo::setVersion(const float& version)
{
    d->version = version;
    emit versionChanged();
}

QStringList DocumentInfo::history() const
{
    return d->history;
}

void DocumentInfo::setHistory(const QStringList& history)
{
    d->history = history;
    emit historyChanged();
}

void DocumentInfo::addHistoryLine(const QString& historyLine)
{
    d->history.append(historyLine);
    emit historyChanged();
}

void DocumentInfo::removeHistoryLine(int index)
{
    d->history.removeAt(index);
    emit historyChanged();
}
