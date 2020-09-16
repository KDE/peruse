/*
 * Copyright 2018  Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
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

#include "AcbfReference.h"
#include "AcbfReferences.h"
#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class Reference::Private {
public:
    Private() {}
    QString id;
    QString language;
    QStringList paragraphs;
};

Reference::Reference(References* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Reference*>("Reference*");
    Q_UNUSED(typeId);
}

Reference::~Reference() = default;

void Reference::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement(QStringLiteral("reference"));
    writer->writeAttribute(QStringLiteral("id"), id());
    /* ACBF 1.2
    writer->writeAttribute(QStringLiteral("lang"), language());
    */
    for(const QString& paragraph : d->paragraphs) {
        writer->writeStartElement(QStringLiteral("p"));
        writer->writeCharacters(paragraph);
        writer->writeEndElement();
    }
    writer->writeEndElement();
}

bool Reference::fromXml(QXmlStreamReader *xmlReader)
{
    setId(xmlReader->attributes().value(QStringLiteral("id")).toString());
    setLanguage(xmlReader->attributes().value(QStringLiteral("lang")).toString());
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("p"))
        {
            d->paragraphs.append(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
        }
        else
        {
            qCWarning(ACBF_LOG) << Q_FUNC_INFO << "currently unsupported subsection in text-area:" << xmlReader->name();
            xmlReader->skipCurrentElement();
        }
    }
    if (xmlReader->hasError()) {
        qCWarning(ACBF_LOG) << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created a reference of with id" << id();
    return !xmlReader->hasError();
}

QString Reference::id() const
{
    return d->id;
}

void Reference::setId(const QString& newId)
{
    d->id = newId;
}

QString Reference::language() const
{
    return d->language;
}

void Reference::setLanguage(const QString& language)
{
    d->language = language;
}

QStringList Reference::paragraphs() const
{
    return d->paragraphs;
}

void Reference::setParagraphs(const QStringList& paragraphs)
{
    d->paragraphs = paragraphs;
}
