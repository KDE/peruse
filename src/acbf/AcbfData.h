/*
 * Copyright (C) 2017 Jesse Pullinen <jesse12p@gmail.com>
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

#ifndef ACBFDATA_H
#define ACBFDATA_H

#include <memory>

#include <QObject>
#include <QXmlStreamReader>

#include "AcbfDocument.h"
#include "AcbfBinary.h"
/**
 * \brief Class to handle the list of embedded data in an ACBF document.
 * 
 * ACBF allows embedding images and fonts as Base64 bytearrays.
 * They are stored in the data section of the file, with a binary
 * element each.
 * 
 * This class holds Binary objects which in turn hold the binary data.
 * This class cannot yet modify the entries or add and remove entries.
 */
namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Data : public QObject
{
    Q_OBJECT
public:
    explicit Data(Document* parent = nullptr);
    ~Data() override;

    /**
     * \brief writes the data section and its entries to the file.
     */
    void toXml(QXmlStreamWriter *writer);
    /**
     * \brief load the data section from the xml into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @param id - the id that is used to reference to this object.
     * @return the binary object referenced by this id.
     */
    Binary* binary(const QString& id) const;
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFDATA_H
