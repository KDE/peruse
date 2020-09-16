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

#include "AcbfReferences.h"
#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class References::Private {
public:
    Private() {}
    QHash<QString, Reference*> references;
};

References::References(Document* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<References*>("References*");
    Q_UNUSED(typeId);
}

References::~References() = default;

void References::toXml(QXmlStreamWriter* writer) {
    writer->writeStartElement(QStringLiteral("references"));

    for(Reference* reference : d->references) {
        reference->toXml(writer);
    }
    writer->writeEndElement();
}

bool References::fromXml(QXmlStreamReader *xmlReader)
{

    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("reference"))
        {
            Reference* newReference = new Reference(this);
            if(!newReference->fromXml(xmlReader)) {
                return false;
            }
            d->references.insert(newReference->id(), newReference);
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

    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created reference section with" << d->references.count() << "references";

    return !xmlReader->hasError();
}

Reference* References::reference(const QString& id) const
{
    return d->references.value(id);
}

void References::setReference(const QString& id, const QStringList& paragraphs, const QString& language) {
    Reference* ref = new Reference(this);
    ref->setId(id);
    ref->setParagraphs(paragraphs);
    ref->setLanguage(language);
    d->references.insert(ref->id(), ref);
}
