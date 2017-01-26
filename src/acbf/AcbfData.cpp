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
#include <QDebug>

using namespace AdvancedComicBookFormat;

class Data::Private {
public:
    Private() {}
    QHash<QString, Binary*> binaries;
};

Data::Data(Document* parent)
    : QObject(parent)
    , d(new Private)
{
    qRegisterMetaType<Data*>("Data*");
}

Data::~Data()
{
    delete d;
}

void Data::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement("data");

    Q_FOREACH(Binary* binary, d->binaries) {
        binary->toXml(writer);
    }

    writer->writeEndElement();
}

bool Data::fromXml(QXmlStreamReader* xmlReader)
{
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == "binary")
        {
            Binary* newBinary = new Binary(this);
            if(!newBinary->fromXml(xmlReader)) {
                return false;
            }
            d->binaries.insert(newBinary->id(), newBinary);
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
    qDebug() << Q_FUNC_INFO << "Created data with" << d->binaries.count() << "binaries";
    return !xmlReader->hasError();
}

Binary* Data::binary(QString id)
{
    return d->binaries.value(id);
}
