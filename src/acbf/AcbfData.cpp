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

#include "AcbfData.h"

#include <QString>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class Data::Private {
public:
    Private(Data* qq)
        : q(qq)
    {}
    Data* q;
    QMultiHash<QString, Binary*> binariesById;
    QObjectList binaries;

    void addBinary(Binary* binary, bool emitListChangeSignal = true) {
        binariesById.insert(binary->id(), binary);
        binaries << binary;
        Q_EMIT q->binaryAdded(binary);
        if (emitListChangeSignal) {
            Q_EMIT q->binariesChanged();
        }
    }
};

Data::Data(Document* parent)
    : QObject(parent)
    , d(new Private(this))
{
    static const int typeId = qRegisterMetaType<Data*>("Data*");
    Q_UNUSED(typeId);
}

Data::~Data() = default;

void Data::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement(QStringLiteral("data"));

    for(Binary* binary : d->binariesById) {
        binary->toXml(writer);
    }

    writer->writeEndElement();
}

bool Data::fromXml(QXmlStreamReader* xmlReader)
{
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("binary"))
        {
            Binary* newBinary = new Binary(this);
            if(!newBinary->fromXml(xmlReader)) {
                return false;
            }
            d->addBinary(newBinary, false);
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
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created data with" << d->binariesById.count() << "binaries";
    Q_EMIT binariesChanged();

    return !xmlReader->hasError();
}

Binary * Data::addBinary(const QString& id)
{
    Binary* newBinary = new Binary(this);
    newBinary->setId(id);
    d->addBinary(newBinary);
    return newBinary;
}


Binary* Data::binary(const QString& id) const
{
    return d->binariesById.value(id);
}

QStringList Data::binaryIds() const
{
    return d->binariesById.keys();
}

QObjectList Data::binaries() const
{
    return d->binaries;
}
