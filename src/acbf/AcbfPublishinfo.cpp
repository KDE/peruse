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
#include <QXmlStreamReader>

#include <acbf_debug.h>

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
    static const int typeId = qRegisterMetaType<PublishInfo*>("PublishInfo*");
    Q_UNUSED(typeId);
}

PublishInfo::~PublishInfo() = default;

void PublishInfo::toXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement(QStringLiteral("publish-info"));

    writer->writeStartElement(QStringLiteral("publisher"));
    writer->writeCharacters(d->publisher);
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("publish-date"));
    writer->writeAttribute(QStringLiteral("value"), d->publishDate.toString(QStringLiteral("yyyy-MM-dd")));
    writer->writeCharacters(d->publishDate.toString(QStringLiteral("MMMM d yyyy")));
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("city"));
    writer->writeCharacters(d->city);
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("isbn"));
    writer->writeCharacters(d->isbn);
    writer->writeEndElement();

    writer->writeStartElement(QStringLiteral("license"));
    writer->writeCharacters(d->license);
    writer->writeEndElement();

    writer->writeEndElement();
}

bool PublishInfo::fromXml(QXmlStreamReader *xmlReader)
{
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("publisher"))
        {
            setPublisher(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("publish-date"))
        {
            QString date = xmlReader->attributes().value(QStringLiteral("value")).toString();
            if(date.isEmpty() || !QDate::fromString(date, Qt::ISODate).isValid()) {
                date = xmlReader->readElementText();
                setPublishDate(QDate::fromString(date));
            } else {
                setPublishDate(QDate::fromString(date, Qt::ISODate));
                xmlReader->skipCurrentElement();
            }
        }
        else if(xmlReader->name() == QStringLiteral("city"))
        {
            setCity(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("isbn"))
        {
            setIsbn(xmlReader->readElementText());
        }
        else if(xmlReader->name() == QStringLiteral("license"))
        {
            setLicense(xmlReader->readElementText());
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
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created publisher information for" << publisher();
    return !xmlReader->hasError();
}

QString PublishInfo::publisher() const
{
    return d->publisher;
}

void PublishInfo::setPublisher(const QString& publisher)
{
    d->publisher = publisher;
    emit publisherChanged();
}

QDate PublishInfo::publishDate() const
{
    if (d->publishDate.isValid()) {
        return d->publishDate;
    } else {
        return QDate().currentDate();
    }
}

void PublishInfo::setPublishDate(const QDate& publishDate)
{
    d->publishDate = publishDate;
    emit publishDateChanged();
}

void PublishInfo::setPublishDateFromInts(const int &year, const int &month, const int &day)
{
    QDate date(year, month, day);
    setPublishDate(date);
}

QString PublishInfo::city() const
{
    return d->city;
}

void PublishInfo::setCity(const QString& city)
{
    d->city = city;
    emit cityChanged();
}

QString PublishInfo::isbn() const
{
    return d->isbn;
}

void PublishInfo::setIsbn(const QString& isbn)
{
    d->isbn = isbn;
    emit isbnChanged();
}

QString PublishInfo::license() const
{
    return d->license;
}

void PublishInfo::setLicense(const QString& license)
{
    d->license = license;
    emit licenseChanged();
}
