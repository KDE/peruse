/*
 * Copyright (C) 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "AcbfInternalReferenceObject.h"
#include "AcbfInternalReference.h"
#include "AcbfReference.h"
#include "AcbfDocument.h"

#include <QVariant>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class InternalReferenceObject::Private
{
public:
    Private(InternalReferenceObject* qq) : q(qq) {}
    InternalReferenceObject* q;
    SupportedReferenceType supportedReferenceType{ReferenceUnknownType};
    QObjectList backReferences;
    QObjectList forwardReferences;

    Document* document() {
        QObject* parent = q;
        Document* doc{nullptr};
        while (!doc) {
            if (!parent) {
                break;
            }
            doc = qobject_cast<Document*>(parent);
            parent = parent->parent();
        }
        return doc;
    }
};

InternalReferenceObject::InternalReferenceObject(InternalReferenceObject::SupportedReferenceType supportedReferenceType, QObject* parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->supportedReferenceType = supportedReferenceType;
}

InternalReferenceObject::~InternalReferenceObject() = default;

InternalReferenceObject::SupportedReferenceType InternalReferenceObject::supportedReferenceType() const
{
    return d->supportedReferenceType;
}


QObjectList InternalReferenceObject::forwardReferences() const
{
    return d->forwardReferences;
}

void InternalReferenceObject::updateForwardReferences()
{
    QStringList paragraphs = property("paragraphs").toStringList();
    QXmlStreamReader reader;
    int paragraphIndex = 0;
    Document* document = d->document();
    if (document) {
        for (const QString& paragraph : paragraphs) {
            reader.clear();
            reader.addData(paragraph);
            while(reader.readNextStartElement())
            {
                if(reader.name() == QStringLiteral("a"))
                {
                    int characterOffset = reader.characterOffset();
                    QString destinationID = reader.attributes().value("href").toString();
                    QObject* destination = document->objectByID(destinationID);
                    InternalReference* internalReference{nullptr};
                    for (QObject* obj : d->forwardReferences) {
                        InternalReference* ref = qobject_cast<InternalReference*>(obj);
                        if (ref->paragraph() == paragraphIndex && ref->character() == characterOffset && ref->to() == destination) {
                            internalReference = ref;
                            break;
                        }
                    }
                    if (!internalReference) {
                        InternalReferenceObject* destinationObject = qobject_cast<InternalReferenceObject*>(destination);
                        internalReference = new InternalReference(this, paragraphIndex, characterOffset, destinationObject, document);
                        d->forwardReferences << internalReference;
                        connect(internalReference, &QObject::destroyed, this, [this, internalReference](){
                            d->forwardReferences.removeOne(internalReference);
                            Q_EMIT forwardReferencesChanged();
                        });
                        if (destinationObject && destinationObject->supportedReferenceType() & ReferenceOrigin) {
                            destinationObject->registerBackReference(internalReference);
                        }
                    }
                }
                else
                {
                    reader.skipCurrentElement();
                }
            }
            ++paragraphIndex;
        }
    }
}

QObjectList InternalReferenceObject::backReferences() const
{
    return d->backReferences;
}

void InternalReferenceObject::registerBackReference(InternalReference* ref)
{
    if (!d->backReferences.contains(ref)) {
        d->backReferences << ref;
        connect(ref, &QObject::destroyed, this, [this, ref](){
            d->backReferences.removeOne(ref);
            Q_EMIT backReferencesChanged();
        });
        Q_EMIT backReferencesChanged();
    }
}
