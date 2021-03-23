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

#include "AcbfMetadata.h"
#include "AcbfBookinfo.h"
#include "AcbfPublishinfo.h"
#include "AcbfDocumentinfo.h"

#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class Metadata::Private
{
public:
    Private()
        : bookInfo(nullptr)
        , publishInfo(nullptr)
        , documentInfo(nullptr)
    {}
    BookInfo* bookInfo;
    PublishInfo* publishInfo;
    DocumentInfo* documentInfo;
};

Metadata::Metadata(Document* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Metadata*>("Metadata*");
    Q_UNUSED(typeId);
    d->bookInfo = new BookInfo(this);
    d->publishInfo = new PublishInfo(this);
    d->documentInfo = new DocumentInfo(this);
}

Metadata::~Metadata() = default;

Document * Metadata::document() const
{
    return qobject_cast<Document*>(parent());
}

void Metadata::toXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement(QStringLiteral("meta-data"));
    d->bookInfo->toXml(writer);
    d->publishInfo->toXml(writer);
    d->documentInfo->toXml(writer);
    writer->writeEndElement();
}

bool Metadata::fromXml(QXmlStreamReader *xmlReader, const QString& xmlData)
{
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("book-info"))
        {
            if(!d->bookInfo->fromXml(xmlReader, xmlData)) {
                return false;
            }
        }
        else if(xmlReader->name() == QStringLiteral("publish-info"))
        {
            if(!d->publishInfo->fromXml(xmlReader)) {
                return false;
            }
        }
        else if(xmlReader->name() == QStringLiteral("document-info"))
        {
            if(!d->documentInfo->fromXml(xmlReader, xmlData)) {
                return false;
            }
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
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created meta information section";
    return !xmlReader->hasError();
}

DocumentInfo * Metadata::documentInfo() const
{
    return d->documentInfo;
}

BookInfo * Metadata::bookInfo() const
{
    return d->bookInfo;
}

PublishInfo * Metadata::publishInfo() const
{
    return d->publishInfo;
}
