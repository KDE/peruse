/*
 * Copyright (C) 2017 Jesse Pullinen <jesse12p@gmail.com>
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

#include "AcbfBinary.h"

#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "AcbfData.h"

using namespace AdvancedComicBookFormat;

class Binary::Private {
public:
    Private() {}
    QString id;
    QString contentType;
    QByteArray data;
};

Binary::Binary(Data* parent)
    : InternalReferenceObject(InternalReferenceObject::ReferenceTarget, parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Binary*>("Binary*");
    Q_UNUSED(typeId);
}

Binary::~Binary() = default;

void Binary::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement(QStringLiteral("binary"));

    writer->writeAttribute(QStringLiteral("id"), id());
    writer->writeAttribute(QStringLiteral("contentType"), contentType());
    writer->writeCharacters(QString::fromLatin1(data().toBase64()));

    writer->writeEndElement();
}

bool Binary::fromXml(QXmlStreamReader* xmlReader)
{
    setId(xmlReader->attributes().value(QStringLiteral("id")).toString());
    setContentType(xmlReader->attributes().value(QStringLiteral("content-type")).toString());
    setData(QByteArray::fromBase64(xmlReader->readElementText().toLatin1()));

    return !xmlReader->hasError();
}

QString Binary::id() const
{
    return d->id;
}

void Binary::setId(const QString& newId)
{
    d->id = newId;
}

QString Binary::contentType() const
{
    return d->contentType;
}

void Binary::setContentType(const QString& newContentType)
{
    d->contentType = newContentType;
}

QByteArray Binary::data() const
{
    return d->data;
}

void Binary::setData(const QByteArray& newData)
{
    d->data = newData;
}
