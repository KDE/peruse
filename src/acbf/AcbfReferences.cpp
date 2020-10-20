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
#include <QTimer>
#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class References::Private {
public:
    Private(References* qq)
        : q(qq)
    {}
    References* q;
    QMultiHash<QString, Reference*> referencesById;
    QObjectList references;

    void addReference(Reference* reference, bool emitListChangeSignal = true) {
        referencesById.insert(reference->id(), reference);
        references << reference;
        QObject::connect(reference, &Reference::languageChanged, q, &References::referencesChanged);
        QObject::connect(reference, &Reference::paragraphsChanged, q, &References::referencesChanged);
        QObject::connect(reference, &Reference::idChanged, q, [this, reference](){
            QMutableHashIterator<QString, Reference*> iterator(referencesById);
            while(iterator.findNext(reference)) {
                iterator.remove();
            }
            referencesById.insert(reference->id(), reference);
            Q_EMIT q->referencesChanged();
        });
        QObject::connect(reference, &QObject::destroyed, q, [this, reference](){
            referencesById.remove(referencesById.key(reference));
            references.removeAll(reference);
            Q_EMIT q->referencesChanged();
        });
        Q_EMIT q->referenceAdded(reference);
        if (emitListChangeSignal) {
            Q_EMIT q->referencesChanged();
        }
    }
};

References::References(Document* parent)
    : QObject(parent)
    , d(new Private(this))
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
            d->addReference(newReference, false);
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

Reference* References::addReference(const QString& id, const QStringList& paragraphs, const QString& language)
{
    Reference* ref = new Reference(this);
    ref->setId(id);
    ref->setParagraphs(paragraphs);
    ref->setLanguage(language);
    d->addReference(ref);
    return ref;
}

QStringList References::referenceIds() const
{
    return d->referencesById.keys();
}

QObjectList References::references() const
{
    return d->references;
}

int References::referenceIndex(Reference* reference) const
{
    return d->references.indexOf(reference);
}

void References::swapReferences(QObject* swapThis, QObject* withThis)
{
    int first = d->references.indexOf(swapThis);
    int second = d->references.indexOf(withThis);
    swapReferencesByIndex(first, second);
}

void References::swapReferencesByIndex(int swapThis, int withThis)
{
    QObject* first = d->references[swapThis];
    QObject* second = d->references[withThis];
    d->references[swapThis] = second;
    d->references[withThis] = first;
    Q_EMIT referencesChanged();
    QTimer::singleShot(100, this, &References::referencesChanged);
}
