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

#include "AcbfPublishinfo.h"

#include <QDate>
#include <QDebug>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class PublishInfo::Private
{
public:
    Private() {}
    QString publisher;
    QDate publishDate;
    QString city;
    QString isbn;
    QString license;
};

PublishInfo::PublishInfo(Metadata* parent)
    : QObject(parent)
    , d(new Private)
{
    qRegisterMetaType<PublishInfo*>("PublishInfo*");
}

PublishInfo::~PublishInfo()
{
    delete d;
}

void PublishInfo::toXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement("publish-info");

    writer->writeStartElement("publisher");
    writer->writeCharacters(d->publisher);
    writer->writeEndElement();

    writer->writeStartElement("publish-date");
    writer->writeAttribute("value", d->publishDate.toString("YYYY-MM-dd"));
    writer->writeCharacters(d->publishDate.toString("MMMM d yyyy"));
    writer->writeEndElement();

    writer->writeStartElement("city");
    writer->writeCharacters(d->city);
    writer->writeEndElement();

    writer->writeStartElement("isbn");
    writer->writeCharacters(d->isbn);
    writer->writeEndElement();

    writer->writeStartElement("license");
    writer->writeCharacters(d->license);
    writer->writeEndElement();

    writer->writeEndElement();
}

bool PublishInfo::fromXml(QXmlStreamReader *xmlReader)
{
    while(xmlReader->readNext())
    {
        if(xmlReader->tokenType() == QXmlStreamReader::EndElement) {
            if(xmlReader->name() == "publish-info") {
                break;
            }
            else {
                continue;
            }
        }
        if(xmlReader->tokenType() == QXmlStreamReader::Characters) {
            continue;
        }
        if(xmlReader->name() == "publisher")
        {
            setPublisher(xmlReader->readElementText());
        }
        else if(xmlReader->name() == "publish-date")
        {
            QString date = xmlReader->attributes().value("value").toString();
            if(date.isEmpty()) {
                date = xmlReader->readElementText();
            }
            setPublishDate(QDate::fromString(date));
        }
        else if(xmlReader->name() == "city")
        {
            setCity(xmlReader->readElementText());
        }
        else if(xmlReader->name() == "isbn")
        {
            setIsbn(xmlReader->readElementText());
        }
        else if(xmlReader->name() == "license")
        {
            setLicense(xmlReader->readElementText());
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
    qDebug() << Q_FUNC_INFO << "Created publisher information for" << publisher();
    return !xmlReader->hasError();
}

QString PublishInfo::publisher()
{
    return d->publisher;
}

void PublishInfo::setPublisher(QString publisher)
{
    d->publisher = publisher;
}

QDate PublishInfo::publishDate()
{
    return d->publishDate;
}

void PublishInfo::setPublishDate(QDate publishDate)
{
    d->publishDate = publishDate;
}

QString PublishInfo::city()
{
    return d->city;
}

void PublishInfo::setCity(QString city)
{
    d->city = city;
}

QString PublishInfo::isbn()
{
    return d->isbn;
}

void PublishInfo::setIsbn(QString isbn)
{
    d->isbn = isbn;
}

QString PublishInfo::license()
{
    return d->license;
}

void PublishInfo::setLicense(QString license)
{
    d->license = license;
}
