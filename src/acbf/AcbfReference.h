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

#include <QObject>
#include <memory>

#include "acbf_export.h"
class QXmlStreamWriter;
class QXmlStreamReader;


namespace AdvancedComicBookFormat
{
/**
 * \brief a Class to handle a single ACBF reference.
 * 
 */
class References;
class ACBF_EXPORT Reference : public QObject
{
    Q_OBJECT

public:
    explicit Reference(References* parent = nullptr);
    ~Reference() override;
    
    /**
     * \brief Write the textarea into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a textarea element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);
    
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
    
    /**
     * @returns a list of paragraphs.
     * 
     * Contains allowed sub-elements: strong, emphasis, strikethrough
     * sub, sup, a (with mandatory href attribute only)
     * Can also contain deprecated sub-elements (superceded by...): code (type option code),
     * inverted (textarea option inverted)
     */
    QStringList paragraphs() const;
    /**
     * \brief set the list of paragraphs for this reference.
     * @param paragraphs - a list of paragraphs. Can contain sub-elements:
     * strong, emphasis, strikethrough, sub, sup, a (with mandatory href attribute only)
     */
    void setParagraphs(const QStringList& paragraphs);
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFREFERENCE_H
