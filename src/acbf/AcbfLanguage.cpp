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

#include "AcbfLanguage.h"

#include <QDebug>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class Language::Private
{
public:
    Private()
        : show(false)
    {}
    QString language;
    bool show;
};

Language::Language(BookInfo* parent)
    : QObject(parent)
    , d(new Private)
{
}

Language::~Language()
{
    delete d;
}

void Language::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement("text-layer");
    writer->writeAttribute("lang", d->language);
    writer->writeAttribute("show", d->show ? "true" : "false");
    writer->writeEndElement();
}

bool Language::fromXml(QXmlStreamReader *xmlReader)
{
    setShow(xmlReader->attributes().value("show").toString().toLower() == "true");
    setLanguage(xmlReader->attributes().value("lang").toString());
    return true;
}

void Language::setLanguage(QString language)
{
    d->language = language;
}

QString Language::language()
{
    return d->language;
}

void Language::setShow(bool show)
{
    d->show = show;
}

bool Language::show()
{
    return d->show;
}
