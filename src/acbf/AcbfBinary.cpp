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

#include <QFile>
#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "AcbfData.h"

using namespace AdvancedComicBookFormat;

class Binary::Private {
public:
    Private() {}
    Data *parent;

    QString id;
    QString contentType{QLatin1String{"application/octet-stream"}};
    QByteArray data;
};

Binary::Binary(Data* parent)
    : InternalReferenceObject(InternalReferenceObject::ReferenceTarget, parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Binary*>("Binary*");
    Q_UNUSED(typeId);
    d->parent = parent;
    // Hook up properties to the parent's global data change signal
    connect(this, &Binary::idChanged, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Binary::contentTypeChanged, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Binary::dataChanged, &InternalReferenceObject::propertyDataChanged);
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
    if (d->id != newId) {
        d->id = newId;
        Q_EMIT idChanged();
    }
}

QString Binary::contentType() const
{
    return d->contentType;
}

void Binary::setContentType(const QString& newContentType)
{
    if (d->contentType != newContentType) {
        d->contentType = newContentType;
        Q_EMIT contentTypeChanged();
    }
}

QByteArray Binary::data() const
{
    return d->data;
}

int Binary::size() const
{
    return d->data.size();
}

void Binary::setData(const QByteArray& newData)
{
    if (d->data != newData) {
        d->data = newData;
        Q_EMIT dataChanged();
    }
}

void AdvancedComicBookFormat::Binary::setDataFromFile(const QString& fileName)
{
    d->data.clear();
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        d->data = file.readAll();
        file.close();
    }
    Q_EMIT dataChanged();
}

int Binary::localIndex()
{
    if (d->parent) {
        return d->parent->binaryIndex(this);
    }
    return -1;
}
