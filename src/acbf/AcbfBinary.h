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

#ifndef ACBFBINARY_H
#define ACBFBINARY_H

#include <memory>

#include "AcbfInternalReferenceObject.h"
#include "acbf_export.h"

class QXmlStreamReader;
class QXmlStreamWriter;

/**
 * \brief Class for handling the embedded data in ACBF
 * 
 * ACBF allows for embedding images and fonts as Base64 bytearrays.
 * 
 * The images are used to allow ACBF to be standalone.
 * 
 * The fonts are used to indicate the appropriate style
 * for text areas.
 * 
 * This class holds the bytearray and mimetype,
 * handling reading and loading from the xml.
 * 
 * It does not convert the bytearrays
 * to the appropriate object.
 */

namespace AdvancedComicBookFormat
{
class Data;
class ACBF_EXPORT Binary : public InternalReferenceObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString contentType READ contentType WRITE setContentType NOTIFY contentTypeChanged)
    Q_PROPERTY(int size READ size NOTIFY dataChanged)
public:
    explicit Binary(Data* parent = nullptr);
    ~Binary() override;

    /**
     *\brief Load binary data into xml.
     */
    void toXml(QXmlStreamWriter *writer);

    /**
     * \brief Load binary data from xml.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return The ID of this binary data element as a QString.
     * Used to identify it from other parts of the
     * ACBF document.
     */
    QString id() const;

    /**
     * \brief Set the ID for this binary element.
     * This is used to reference this element from
     * other parts of the ACBF document.
     * @param newId - The new ID as a string.
     */
    void setId(const QString& newId);
    Q_SIGNAL void idChanged();

    /**
     * @return the mimetype of the binary data as a QString. If one was not defined, the default is application/octet-stream
     */
    QString contentType() const;

    /**
     * \brief Indicate the mimetype of the binary data.
     * @param newContentType - the mimetype in string format.
     */
    void setContentType(const QString& newContentType);
    Q_SIGNAL void contentTypeChanged();

    /**
     * @return The binary data as a QByteArray.
     */
    QByteArray data() const;
    /**
     * @return The size of the binary data
     */
    int size() const;

    /**
     * \brief Set the binary data to store in this element.
     * 
     * @param newData - This should be a QByteArray.
     */
    void setData(const QByteArray& newData);
    /**
     * \brief Set the binary data to store in this element from a file.
     * Note: This will be read immediately and added with no further checks.
     * @param fileName The filename of a file on disk. If the file does not exist, the data will be set to empty.
     */
    Q_INVOKABLE void setDataFromFile(const QString& fileName);
    /**
     * Fired whenever the data contents of the binary changes
     */
    Q_SIGNAL void dataChanged();

    /**
     * The position of this binary in the list of Binary instances in the
     * parent Data instance.
     * @return The instance's position
     */
    int localIndex() override;
private:
    class Private;
    std::unique_ptr<Private> d;
};
}
Q_DECLARE_METATYPE(AdvancedComicBookFormat::Binary*)

#endif // ACBFBINARY_H
