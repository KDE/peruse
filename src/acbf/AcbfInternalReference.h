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

#ifndef ACBFINTERNALREFERENCE_H
#define ACBFINTERNALREFERENCE_H

#include <QObject>
#include <memory>

#include <AcbfInternalReferenceObject.h>

namespace AdvancedComicBookFormat
{
/**
 * \brief A reference from one bit of text the book (either in a Reference, or in a TextArea) to some referenceable object
 *
 * This is a container for easily determining the origin and destination of internal links in the book. This is used
 * in cases where links are done inside paragraphs of text, rather than through a Jump.
 *
 * Note that the from position is the precise one, as ACBF does not have granular targeting (internal links are done
 * on an id basis, and only a top level objects of the types Reference and Binary can have IDs).
 */
class ACBF_EXPORT InternalReference : public QObject
{
    Q_OBJECT
    /**
     * The top level origin of the link (this will either be a Reference or a TextArea).
     * To determine the granular location, use the paragraph and character properties.
     */
    Q_PROPERTY(QObject *from READ from CONSTANT)
    /**
     * The index of the paragraph in the origin where the link can be found.
     */
    Q_PROPERTY(int paragraph READ paragraph CONSTANT)
    /**
     * The position of the link inside the paragraph. This is the
     * position of the first character of the anchor element (that is, the position of the
     * opening \< in \<a href...), not the text of the link.
     */
    Q_PROPERTY(int character READ character CONSTANT)
    /**
     * The destination for the reference (this will be either a Reference or a TextArea).
     */
    Q_PROPERTY(QObject *to READ to CONSTANT)
public:
    explicit InternalReference(InternalReferenceObject *from, int paragraph, int character, InternalReferenceObject *to, QObject *parent = nullptr);
    virtual ~InternalReference();

    QObject *from() const;
    int paragraph() const;
    int character() const;
    QObject *to() const;

private:
    class Private;
    std::unique_ptr<Private> d;
};

};

#endif // ACBFINTERNALREFERENCE_H
