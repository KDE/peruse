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
#include "AcbfBookinfo.h"
#include "AcbfData.h"
#include "AcbfReferences.h"
#include "AcbfStyleSheet.h"

#include <QXmlStreamReader>

#include <acbf_debug.h>
#include <QBuffer>

using namespace AdvancedComicBookFormat;

class Document::Private {
public:
    Private()
        : metaData(nullptr)
        , body(nullptr)
        , data(nullptr)
    {}
    Metadata* metaData;
    Body* body;
    Data* data;
    References* references;
    StyleSheet* cssStyleSheet;
};

Document::Document(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<QObjectList>("QObjectList");
    Q_UNUSED(typeId);
    d->metaData = new Metadata(this);
    d->body = new Body(this);
    d->data = new Data(this);
    d->references = new References(this);
    d->cssStyleSheet = new StyleSheet(this);
}

Document::~Document() = default;

QString Document::toXml()
{
    QByteArray bytes;
    QBuffer output(&bytes);
    output.open(QIODevice::WriteOnly);
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement(QStringLiteral("ACBF"));
    writer.writeAttribute(QStringLiteral("xmlns"), QStringLiteral("http://www.acbf.info/xml/acbf/1.2"));
    d->metaData->toXml(&writer);
    d->cssStyleSheet->toXml(&writer);
    d->body->toXml(&writer);
    d->data->toXml(&writer);
    d->references->toXml(&writer);
    writer.writeEndElement();
    writer.writeEndDocument();

    return QString{bytes};
}

bool Document::fromXml(QString xmlDocument)
{
    QXmlStreamReader xmlReader(xmlDocument);
    if(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == QStringLiteral("ACBF")
            && (xmlReader.namespaceUri().startsWith(QStringLiteral("http://www.fictionbook-lib.org/xml/acbf/"))
                || xmlReader.namespaceUri().startsWith(QStringLiteral("http://www.acbf.info/xml/acbf/"))
            ))
        {
            while(xmlReader.readNextStartElement())
            {
                if(xmlReader.name() == QStringLiteral("meta-data"))
                {
                    if(!d->metaData->fromXml(&xmlReader, xmlDocument)) {
                        break;
                    }
                }
                else if(xmlReader.name() == QStringLiteral("body"))
                {
                    if(!d->body->fromXml(&xmlReader, xmlDocument)) {
                        break;
                    }
                }
                else if(xmlReader.name() == QStringLiteral("data"))
                {
                    if(!d->data->fromXml(&xmlReader)) {
                        break;
                    }
                }
                else if(xmlReader.name() == QStringLiteral("references"))
                {
                    if(!d->references->fromXml(&xmlReader, xmlDocument)) {
                        break;
                    }
                }
                else if(xmlReader.name() == QStringLiteral("style"))
                {
                    if(!d->cssStyleSheet->fromXml(&xmlReader, xmlDocument)) {
                        break;
                    }
                }
                else
                {
                    qCWarning(ACBF_LOG) << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader.name();
                    xmlReader.skipCurrentElement();
                }
            }
            // Ensure that all the internal forward references are up to date, by running through all the paragraphs in all the places
            // That is, run through all objects, check if they're referenceable, and update their forward references if they are
            std::function<void(const QObject* parent)> updateAllForwardReferences;
            updateAllForwardReferences = [&updateAllForwardReferences](const QObject *parent) {
                for (QObject *child : parent->children()) {
                    InternalReferenceObject* refObj = qobject_cast<InternalReferenceObject*>(child);
                    if (refObj) {
                        refObj->updateForwardReferences();
                    }
                    updateAllForwardReferences(child);
                }
            };
            updateAllForwardReferences(this);
        }
        else {
            qCWarning(ACBF_LOG) << Q_FUNC_INFO << "not an ACBF XML document";
            return false;
        }
    }
    if (xmlReader.hasError()) {
        qCWarning(ACBF_LOG) << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader.name() << "(" << xmlReader.lineNumber() << ":" << xmlReader.columnNumber() << ") The reported error was:" << xmlReader.errorString();
    }
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Completed ACBF document creation for" << d->metaData->bookInfo()->title();
    return !xmlReader.hasError();
}

Metadata * Document::metaData() const
{
    return d->metaData;
}

Body * Document::body() const
{
    return d->body;
}

Data * Document::data() const
{
    return d->data;
}

References * Document::references() const
{
    return d->references;
}

StyleSheet * Document::styleSheet() const
{
    return d->cssStyleSheet;
}

QObject * Document::objectByID(const QString& id) const
{
    QObject* obj{nullptr};
    // We could introspect for id here, but that would be much more expensive, so let's not
    for (QObject* child : d->references->references()) {
        Reference* ref = qobject_cast<Reference*>(child);
        if (ref->id() == id) {
            obj = child;
            break;
        }
    }
    if (!obj) {
        for (QObject* child : d->data->binaries()) {
            Binary* ref = qobject_cast<Binary*>(child);
            if (ref->id() == id) {
                obj = child;
                break;
            }
        }
    }
    return obj;
}
