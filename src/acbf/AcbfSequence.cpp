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

#include "AcbfSequence.h"

#include <QXmlStreamWriter>

using namespace AdvancedComicBookFormat;

class Sequence::Private
{
public:
    Private()
        : volume(0)
        , number(0)
    {}
    QString title;
    int volume;
    int number;
};

Sequence::Sequence(BookInfo* parent)
    : QObject(parent)
    , d(new Private)
{
}

Sequence::~Sequence()
{
    delete d;
}

void Sequence::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement("sequence");
    writer->writeAttribute("title", d->title);
    writer->writeAttribute("volume", QString::number(d->volume));
    writer->writeCharacters(QString::number(d->number));
    writer->writeEndElement();
}

bool Sequence::fromXml(QXmlStreamReader *xmlReader)
{
    setVolume(xmlReader->attributes().value("volume").toInt());
    setTitle(xmlReader->attributes().value("title").toString());
    setNumber(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements).toInt());
    return true;
}

QString Sequence::title()
{
    return d->title;
}

void Sequence::setTitle(QString title)
{
    d->title = title;
}

int Sequence::volume()
{
    return d->volume;
}

void Sequence::setVolume(int volume)
{
    d->volume = volume;
}

int Sequence::number()
{
    return d->number;
}

void Sequence::setNumber(int number)
{
    d->number = number;
}
