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

#ifndef ACBFREFERENCES_H
#define ACBFREFERENCES_H

#include <memory>

#include "AcbfDocument.h"
#include "AcbfReference.h"
#include <QObject>
#include <QXmlStreamReader>


namespace AdvancedComicBookFormat
{

class ACBF_EXPORT References : public QObject
{
    Q_OBJECT

public:
    explicit References(Document* parent = nullptr);
    ~References() override;
    
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
     * @param id - the id that is used to reference to this object.
     * @return the reference object referenced by this id.
     */
    Reference* reference(const QString& id) const;
    
    /**
     * @brief Add reference at ID.
     * 
     * @param id The id that will refer to this reference.
     * @param paragraphs The paragraphs comprising this reference.
     * @param language The language (optional, and 1.2 only)
     */
    void setReference(const QString& id, const QStringList& paragraphs, const QString& language = "");

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFREFERENCES_H
