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
    QMultiHash<QString, Reference*> referencesById;
    QObjectList references;
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

    for(QObject* reference : d->references) {
        qobject_cast<Reference*>(reference)->toXml(writer);
    }
    writer->writeEndElement();
}

bool References::fromXml(QXmlStreamReader *xmlReader)
{
    qDeleteAll(d->references);
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("reference"))
        {
            Reference* newReference = new Reference(this);
            if(!newReference->fromXml(xmlReader)) {
                return false;
            }
            d->referencesById.insert(newReference->id(), newReference);
            d->references << newReference;
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
    Q_EMIT referencesChanged();

    return !xmlReader->hasError();
}

Reference* References::reference(const QString& id) const
{
    return d->referencesById.value(id);
}

Reference* AdvancedComicBookFormat::References::addReference(const QString& id, const QStringList& paragraphs, const QString& language)
{
    Reference* ref = new Reference(this);
    ref->setId(id);
    ref->setParagraphs(paragraphs);
    ref->setLanguage(language);
    connect(ref, &QObject::destroyed, this, [this, ref](){
        d->referencesById.remove(d->referencesById.key(ref));
        d->references.removeAll(ref);
        Q_EMIT referencesChanged();
    });
    connect(ref, &Reference::idChanged, this, [this, ref](){
        QMutableHashIterator<QString, Reference*> iterator(d->referencesById);
        while(iterator.findNext(ref)) {
            iterator.remove();
        }
        d->referencesById.insert(ref->id(), ref);
        Q_EMIT referencesChanged();
    });
    d->referencesById.insert(ref->id(), ref);
    d->references << ref;
    Q_EMIT referencesChanged();
    return ref;
}

QObjectList AdvancedComicBookFormat::References::references() const
{
    return d->references;
}
