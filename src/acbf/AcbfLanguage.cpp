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

#include <QXmlStreamReader>

#include <acbf_debug.h>

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

Language::~Language() = default;

void Language::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement(QStringLiteral("text-layer"));
    writer->writeAttribute(QStringLiteral("lang"), d->language);
    writer->writeAttribute(QStringLiteral("show"), d->show ? QStringLiteral("true") : QStringLiteral("false"));
    writer->writeEndElement();
}

bool Language::fromXml(QXmlStreamReader *xmlReader)
{
    setShow(xmlReader->attributes().value(QStringLiteral("show")).toString().toLower() == QStringLiteral("true"));
    setLanguage(xmlReader->attributes().value(QStringLiteral("lang")).toString());
    xmlReader->skipCurrentElement();
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created language" << language();
    return true;
}

void Language::setLanguage(const QString& language)
{
    if (d->language != language) {
        d->language = language;
        Q_EMIT languageChanged();
    }
}

QString Language::language() const
{
    return d->language;
}

void Language::setShow(bool show)
{
    if (d->show != show) {
        d->show = show;
        Q_EMIT showChanged();
    }
}

bool Language::show() const
{
    return d->show;
}
