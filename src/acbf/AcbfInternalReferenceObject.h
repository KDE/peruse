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

#ifndef ACBFINTERNALREFERENCEOBJECT_H
#define ACBFINTERNALREFERENCEOBJECT_H

#include <QObject>
#include <memory>

#include "acbf_export.h"

namespace AdvancedComicBookFormat
{
class InternalReference;
/**
 * \brief To be able to use your class for internal references, inherit from this class
 */
class ACBF_EXPORT InternalReferenceObject : public QObject
{
    Q_OBJECT
    /**
     * Whether this object can be a target, an origin, or both
     */
    Q_PROPERTY(int supportedReferenceType READ supportedReferenceType CONSTANT)
    /**
     * A list of the internal links going out of this Reference (that is, objects to which are links
     * in the paragraphs in this instance)
     * @see AdvancedComicBookFormat::InternalReference
     */
    Q_PROPERTY(QObjectList forwardReferences READ forwardReferences NOTIFY forwardReferencesChanged)
    /**
     * A list of the internal links going into this Reference (that is, objects which contain paragraphs
     * in which there are links that point to this instance)
     */
    Q_PROPERTY(QObjectList backReferences READ backReferences NOTIFY backReferencesChanged)
    /**
     * The index of this object in the local list it is a part of
     */
    Q_PROPERTY(int localIndex READ localIndex NOTIFY localIndexChanged)
    /**
     * The ACBF type name of this object (this will usually be either Reference, Binary, or Textarea).
     * This might be used for more granular identification of the object, if the reference type is
     * not sufficient.
     */
    Q_PROPERTY(QString objectType READ objectType CONSTANT)
public:
    enum SupportedReferenceType {
        ReferenceUnknownType = 0,
        ReferenceOrigin = 1,
        ReferenceTarget = 2,
        ReferenceOriginAndTarget = ReferenceOrigin + ReferenceTarget
    };
    Q_ENUM(SupportedReferenceType)
    explicit InternalReferenceObject(SupportedReferenceType supportedReferenceType, QObject *parent = nullptr);
    virtual ~InternalReferenceObject();

    SupportedReferenceType supportedReferenceType() const;

    QObjectList forwardReferences() const;
    Q_SIGNAL void forwardReferencesChanged();
    void updateForwardReferences();
    QObjectList backReferences() const;
    Q_SIGNAL void backReferencesChanged();
    QString objectType() const;
    /**
     * Function called by other classes which contain links to this reference
     */
    void registerBackReference(InternalReference *ref);

    /**
     * This is the index of this object in the local list the object is a part of. Specifically,
     * - Reference instances are in an ordered list inside the References main object
     * - Binary instances are in an ordered list inside the Data main object
     * Two objects can consequently have the same local index, but two objects inside the same
     * main object can't.
     */
    virtual int localIndex() = 0;
    Q_SIGNAL void localIndexChanged();

    /**
     * This should be fired whenever any of the properties change (so the container model can be updated)
     */
    Q_SIGNAL void propertyDataChanged();

private:
    class Private;
    std::unique_ptr<Private> d;
};
};

#endif // ACBFINTERNALREFERENCEOBJECT_H
