/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "AcbfDocument.h"
#include "AcbfBody.h"
#include "AcbfMetadata.h"

#include <QDebug>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class Document::Private {
public:
    Private()
        : metaData(0)
        , body(0)
    {}
    Metadata* metaData;
    Body* body;
};

Document::Document(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
    d->metaData = new Metadata(this);
    d->body = new Body(this);
}

Document::~Document()
{
    delete d;
}

QString Document::toXml()
{
    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("ACBF");
    writer.writeAttribute("xmlns", "http://www.fictionbook-lib.org/xml/acbf/1.0");
    d->metaData->toXml(&writer);
    d->body->toXml(&writer);
    writer.writeEndElement();
    writer.writeEndDocument();

    return output;
}

bool Document::fromXml(QString xmlDocument)
{
    QXmlStreamReader xmlReader(xmlDocument);
    if(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "ACBF" && xmlReader.namespaceUri().startsWith("http://www.fictionbook-lib.org/xml/acbf/"))
        {
            while(xmlReader.readNextStartElement())
            {
                if(xmlReader.name() == "body")
                {
                    if(!d->body->fromXml(&xmlReader)) {
                        break;
                    }
                }
                else if(xmlReader.name() == "meta-data")
                {
                    if(!d->metaData->fromXml(&xmlReader)) {
                        break;
                    }
                }
                else
                {
                    qWarning() << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader.name();
                    xmlReader.skipCurrentElement();
                }
            }
        }
    }
    if (xmlReader.hasError()) {
        qWarning() << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader.name() << "(" << xmlReader.lineNumber() << ":" << xmlReader.columnNumber() << ") The reported error was:" << xmlReader.errorString();
    }
    return !xmlReader.hasError();
}

Metadata * Document::metaData()
{
    return d->metaData;
}

Body * Document::body()
{
    return d->body;
}
