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

#include "AcbfAuthor.h"

#include <QDebug>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class Author::Private
{
public:
    Private() {}
    QString activity;
    QString language;
    QString firstName;
    QString middleName;
    QString lastName;
    QString nickName;
    QString homePage;
    QString email;
};

Author::Author(Metadata* parent)
    : QObject(parent)
    , d(new Private())
{
    qRegisterMetaType<Author*>("Author*");
}

Author::~Author() = default;

QString Author::displayName() const
{
    if(!d->nickName.isEmpty()) {
        return d->nickName;
    }
    else if(!d->firstName.isEmpty() || !d->middleName.isEmpty() || !d->lastName.isEmpty()) {
        return QStringLiteral("%1 %2 %3").arg(d->firstName).arg(d->middleName).arg(d->lastName).simplified();
    }
    else if(!d->email.isEmpty()) {
        return d->email;
    }
    else if(!d->homePage.isEmpty()) {
        return d->homePage;
    }
    return QLatin1String("");
}

void Author::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement(QStringLiteral("author"));
    if(!d->activity.isEmpty()) {
        writer->writeAttribute(QStringLiteral("activity"), d->activity);
    }
    if(!d->language.isEmpty()) {
        writer->writeAttribute(QStringLiteral("lang"), d->language);
    }

    writer->writeTextElement(QStringLiteral("first-name"), d->firstName);
    writer->writeTextElement(QStringLiteral("middle-name"), d->middleName);
    writer->writeTextElement(QStringLiteral("last-name"), d->lastName);
    writer->writeTextElement(QStringLiteral("nickname"), d->nickName);
    writer->writeTextElement(QStringLiteral("home-page"), d->homePage);
    writer->writeTextElement(QStringLiteral("email"), d->email);

    writer->writeEndElement();  
}

bool Author::fromXml(QXmlStreamReader *xmlReader)
{
    setActivity(xmlReader->attributes().value(QStringLiteral("activity")).toString());
    setLanguage(xmlReader->attributes().value(QStringLiteral("lang")).toString());
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("first-name"))
        {
            setFirstName(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("middle-name"))
        {
            setMiddleName(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("last-name"))
        {
            setLastName(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("nickname"))
        {
            setNickName(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("home-page"))
        {
            setHomePage(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("email"))
        {
            setEmail(xmlReader->readElementText());
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
    qDebug() << Q_FUNC_INFO << "Created author" << firstName() << lastName() << nickName() << "responsible for" << activity() << "for language" << language();
    return !xmlReader->hasError();
}

QString Author::activity() const
{
    return d->activity;
}

void Author::setActivity(const QString& activity)
{
    d->activity = activity;
}

QStringList Author::availableActivities()
{
    return {
        QStringLiteral("Writer"), // (story was written by)
        QStringLiteral("Adapter"), // (in case the comic book story is adaptation of story written by someone else)
        QStringLiteral("Artist"), // (art was drawn by)
        QStringLiteral("Penciller"), // (penciller, inker, colorist and leterrer attribute value may be used in case art was created in collaboration by a group of different artists)
        QStringLiteral("Inker"),
        QStringLiteral("Colorist"),
        QStringLiteral("Letterer"), // (in case texts are handwritten)
        QStringLiteral("CoverArtist"), // (created comic book cover)
        QStringLiteral("Photographer"), // /new in 1.1/
        QStringLiteral("Editor"),
        QStringLiteral("Assistant Editor"), // /new in 1.1/
        QStringLiteral("Translator"),
        QStringLiteral("Other") // /new in 1.1/
    };
}

QString Author::language() const
{
    return d->language;
}

void Author::setLanguage(const QString& language)
{
    d->language = language;
}

QString Author::firstName() const
{
    return d->firstName;
}

void Author::setFirstName(const QString& name)
{
    d->firstName = name;
}

QString Author::middleName() const
{
    return d->middleName;
}

void Author::setMiddleName(const QString& name)
{
    d->middleName = name;
}

QString Author::lastName() const
{
    return d->lastName;
}

void Author::setLastName(const QString& name)
{
    d->lastName = name;
}

QString Author::nickName() const
{
    return d->nickName;
}

void Author::setNickName(const QString& name)
{
    d->nickName = name;
}

QString Author::homePage() const
{
    return d->homePage;
}

void Author::setHomePage(const QString& homepage)
{
    d->homePage = homepage;
}

QString Author::email() const
{
    return d->email;
}

void Author::setEmail(const QString& email)
{
    d->email = email;
}
