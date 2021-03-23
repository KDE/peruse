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

#ifndef ACBFREFERENCE_H
#define ACBFREFERENCE_H

#include <memory>

#include "AcbfInternalReferenceObject.h"
#include "acbf_export.h"

class QXmlStreamWriter;
class QXmlStreamReader;

namespace AdvancedComicBookFormat
{
class References;
/**
 * \brief a Class to handle a single ACBF reference.
 * 
 * A reference is a bit of text that can be referred to
 * with an anchor in any of the other paragraphs. This
 * can be used for footnotes, like translation notes
 * or author's notes.
 * 
 * You refer to a reference by using the ID.
 */
class ACBF_EXPORT Reference : public InternalReferenceObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QStringList paragraphs READ paragraphs WRITE setParagraphs NOTIFY paragraphsChanged)

public:
    explicit Reference(References* parent = nullptr);
    ~Reference() override;

    /**
     * \brief Write the reference into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a reference element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader, const QString& xmlData);

    /**
     * @return The ID of this reference data element as a QString.
     * Used to identify it from other parts of the
     * ACBF document.
     */
    QString id() const;
    /**
     * \brief Set the ID for this reference element.
     * This is used to reference this element from
     * other parts of the ACBF document.
     * @param newId - The new ID as a string.
     */
    void setId(const QString& newId);
    Q_SIGNAL void idChanged();

    /**
     * @returns the language for this reference.
     */
    QString language() const;
    /**
     * \brief set the language for this reference.
     * @param language - the language of the entry in language code, country
     * code format joined by a dash (not an underscore).
     */
    void setLanguage(const QString& language);
    Q_SIGNAL void languageChanged();

    /**
     * @returns a list of paragraphs.
     * 
     * Contains allowed sub-elements: strong, emphasis, strikethrough
     * sub, sup, a (with mandatory href attribute only)
     * Can also contain deprecated sub-elements (superceeded by...): code (type option code),
     * inverted (textarea option inverted)
     */
    QStringList paragraphs() const;
    /**
     * \brief set the list of paragraphs for this reference.
     * @param paragraphs - a list of paragraphs. Can contain sub-elements:
     * strong, emphasis, strikethrough, sub, sup, a (with mandatory href attribute only)
     */
    void setParagraphs(const QStringList& paragraphs);
    Q_SIGNAL void paragraphsChanged();

    /**
     * The position of this reference in the list of Reference instances in the
     * parent References instance.
     * @return The instance's position
     */
    int localIndex() override;
private:
    class Private;
    std::unique_ptr<Private> d;
};
}
Q_DECLARE_METATYPE(AdvancedComicBookFormat::Reference*)

#endif // ACBFREFERENCE_H
