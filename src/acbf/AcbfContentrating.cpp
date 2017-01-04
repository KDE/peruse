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

#include "AcbfContentrating.h"

#include <QXmlStreamWriter>
#include <QDebug>

using namespace AdvancedComicBookFormat;

class ContentRating::Private
{
public:
    Private() {}
    QString type;
    QString rating;
};

ContentRating::ContentRating(BookInfo* parent)
    : QObject(parent)
    , d(new Private)
{
}

ContentRating::~ContentRating()
{
    delete d;
}

void ContentRating::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement("content-rating");
    writer->writeAttribute("type", d->type);
    writer->writeCharacters(d->rating);
    writer->writeEndElement();
}

bool ContentRating::fromXml(QXmlStreamReader *xmlReader)
{
    setType(xmlReader->attributes().value("type").toString());
    setRating(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
    qDebug() << Q_FUNC_INFO << "Created a content rating for" << type() << "with rating" << rating();
    return true;
}

QString ContentRating::type()
{
    return d->type;
}

void ContentRating::setType(QString type)
{
    d->type = type;
}

QString ContentRating::rating()
{
    return d->rating;
}

void ContentRating::setRating(QString rating)
{
    d->rating = rating;
}
