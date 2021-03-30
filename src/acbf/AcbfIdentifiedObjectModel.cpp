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

#include "AcbfIdentifiedObjectModel.h"
#include "AcbfBody.h"
#include "AcbfDocument.h"
#include "AcbfInternalReferenceObject.h"
#include "AcbfData.h"
#include "AcbfFrame.h"
#include "AcbfJump.h"
#include "AcbfReferences.h"
#include "AcbfTextarea.h"

using namespace AdvancedComicBookFormat;

class IdentifiedObjectModel::Private {
public:
    Private(IdentifiedObjectModel* qq)
        : q(qq)
    {}
    IdentifiedObjectModel* q{nullptr};
    Document* document{nullptr};
    QList<InternalReferenceObject*> identifiedObjects;

    void addAndConnectChild(InternalReferenceObject* child) {
        if (child) {
            int idx = identifiedObjects.count();
            q->beginInsertRows(QModelIndex(), idx, idx);
            identifiedObjects.append(child);
            q->endInsertRows();
            QObject::connect(child, &QObject::destroyed, q, [this, child](){
                int idx = identifiedObjects.indexOf(child);
                q->beginRemoveRows(QModelIndex(), idx, idx);
                identifiedObjects.removeOne(child);
                q->endRemoveRows();
                child->disconnect(q);
            });
            QObject::connect(child, &InternalReferenceObject::propertyDataChanged, q, [this, child]() {
                QModelIndex idx = q->index(identifiedObjects.indexOf(child));
                q->dataChanged(idx, idx);
            });

            // Some special handling for pages, because pages are special and potentially contain things, including some that can also have reference objects
            Page* page = qobject_cast<Page*>(child);
            if (page) {
                connect(page, &Page::jumpAdded, q, [this](QObject* child) { addAndConnectChild(qobject_cast<InternalReferenceObject*>(child)); });
                connect(page, &Page::jumpsChanged, q,  [this]() { q->dataChanged(q->index(0), q->index(identifiedObjects.count())); });
                for (QObject* obj: page->jumps()) {
                    addAndConnectChild(qobject_cast<InternalReferenceObject*>(obj));
                }
                connect(page, &Page::frameAdded, q, [this](QObject* child) { addAndConnectChild(qobject_cast<InternalReferenceObject*>(child)); });
                connect(page, &Page::framePointStringsChanged, q, [this]() { q->dataChanged(q->index(0), q->index(identifiedObjects.count())); });
                for (Frame* frame : page->frames()) {
                    addAndConnectChild(frame);
                }
                connect(page, &Page::textLayerAdded, q, [this](QObject* child) { connectTextLayer(qobject_cast<Textlayer*>(child)); });
                connect(page, &Page::textLayerLanguagesChanged, q, [this](){ q->dataChanged(q->index(0), q->index(identifiedObjects.count())); });
                for (Textlayer* textlayer : page->textLayersForAllLanguages()) {
                    connectTextLayer(textlayer);
                }
            }
        }
    }
    void connectTextLayer(Textlayer* textlayer) {
        connect(textlayer, &Textlayer::textareaAdded, q, [this](QObject* child) { addAndConnectChild(qobject_cast<InternalReferenceObject*>(child)); });
        connect(textlayer, &Textlayer::textareasChanged, q, [this](){ q->dataChanged(q->index(0), q->index(identifiedObjects.count())); });
        for (QObject* obj : textlayer->textareas()) {
            Textarea* textarea = qobject_cast<Textarea*>(obj);
            addAndConnectChild(textarea);
        }
    }
};

IdentifiedObjectModel::IdentifiedObjectModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
}

IdentifiedObjectModel::~IdentifiedObjectModel() = default;

QHash<int, QByteArray> IdentifiedObjectModel::roleNames() const
{
    static const QHash<int, QByteArray> roleNames{
        {IdRole, "id"},
        {OriginalIndexRole, "originalIndex"},
        {TypeRole, "type"},
        {ObjectRole, "object"}
    };
    return roleNames;
}

QVariant IdentifiedObjectModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if (checkIndex(index) && d->document) {
        InternalReferenceObject* object = d->identifiedObjects.value(index.row());
        if (object) {
            switch(role) {
                case IdRole:
                    data.setValue(object->property("id"));
                    break;
                case TypeRole:
                    if (qobject_cast<Reference*>(object)) {
                        data.setValue<int>(ReferenceType);
                    } else if (qobject_cast<Binary*>(object)) {
                        data.setValue<int>(BinaryType);
                    } else if (qobject_cast<Textarea*>(object)) {
                        data.setValue<int>(TextareaType);
                    } else if (qobject_cast<Frame*>(object)) {
                        data.setValue<int>(FrameType);
                    } else if (qobject_cast<Page*>(object)) {
                        data.setValue<int>(PageType);
                    } else if (qobject_cast<Jump*>(object)) {
                        data.setValue<int>(JumpType);
                    } else {
                        data.setValue<int>(UnknownType);
                    }
                    break;
                case OriginalIndexRole:
                    data.setValue<int>(object->localIndex());
                    break;
                case ObjectRole:
                    data.setValue<QObject*>(object);
                    break;
                default:
                    break;
            };
        }
    }
    return data;
}

int IdentifiedObjectModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return d->identifiedObjects.count();
}

QObject * IdentifiedObjectModel::document() const
{
    return d->document;
}

void IdentifiedObjectModel::setDocument(QObject* document)
{
    if (d->document != document) {
        beginResetModel();
        for (QObject* obj : d->identifiedObjects) {
            obj->disconnect(this);
        }
        d->identifiedObjects.clear();
        d->document = qobject_cast<Document*>(document);
        if (d->document) {
            std::function<void(const QObject* parent)> findAllIdentifiedObjects;
            findAllIdentifiedObjects = [&findAllIdentifiedObjects, this](const QObject *parent) {
                for (QObject *child : parent->children()) {
                    InternalReferenceObject* refObj = qobject_cast<InternalReferenceObject*>(child);
                    if (refObj) {
                        d->addAndConnectChild(refObj);
                    }
                    findAllIdentifiedObjects(child);
                }
            };
            findAllIdentifiedObjects(d->document);
            connect(d->document->data(), &Data::binaryAdded, this, [this](QObject* child){ d->addAndConnectChild(qobject_cast<InternalReferenceObject*>(child)); });
            connect(d->document->data(), &Data::binariesChanged, this, [this](){ dataChanged(index(0), index(d->identifiedObjects.count())); });
            connect(d->document->references(), &References::referenceAdded, this, [this](QObject* child){ d->addAndConnectChild(qobject_cast<InternalReferenceObject*>(child)); });
            connect(d->document->references(), &References::referencesChanged, this, [this](){ dataChanged(index(0), index(d->identifiedObjects.count())); });
            connect(d->document->body(), &Body::pageCountChanged, this, [this](){ dataChanged(index(0), index(d->identifiedObjects.count())); });
            connect(d->document->body(), &Body::pageAdded, this, [this](QObject* child) { d->addAndConnectChild(qobject_cast<InternalReferenceObject*>(child)); });
        }
        endResetModel();
        Q_EMIT documentChanged();
    }
}

QObject * IdentifiedObjectModel::objectById(const QString& id)
{
    QObject* identified{nullptr};
    static const char* idProp{"id"};
    for (InternalReferenceObject* object : d->identifiedObjects) {
        if ((object->supportedReferenceType() & InternalReferenceObject::ReferenceTarget) == InternalReferenceObject::ReferenceTarget) {
            if (object->property(idProp).toString() == id) {
                identified = object;
                break;
            }
        }
    }
    return identified;
}
