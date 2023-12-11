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

#include "AcbfReference.h"
#include <QObject>
#include <QXmlStreamReader>

namespace AdvancedComicBookFormat
{

class Document;

/**
 * @brief Class to handle the ACBF references section.
 * 
 * The References object holds Reference objects, which
 * in turn have an id, language and a list of paragraphs.
 * 
 * A reference is a bit of text that can be referred to
 * with an anchor in any of the other paragraphs. This
 * can be used for footnotes, like translation notes
 * or author's notes.
 * 
 * Reference objects can be retrieved by using their ID.
 */
class ACBF_EXPORT References : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObjectList references READ references NOTIFY referencesChanged)
    Q_PROPERTY(QStringList referenceIds READ referenceIds NOTIFY referencesChanged)
public:
    explicit References(Document* parent = nullptr);
    ~References() override;


    /**
     * \brief Write the references into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a reference element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader, const QString& xmlData);

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
    Q_INVOKABLE AdvancedComicBookFormat::Reference* addReference(const QString& id, const QStringList& paragraphs, const QString& language = "");

    /**
     * Returns a list of all known IDs for references
     * @param A list of reference IDs
     */
    QStringList referenceIds() const;

    /**
     * Returns the internal list of references
     * @return A list of reference objects
     */
    QObjectList references() const;

    /**
     * The position in the references object list of the reference passed to the function
     * @param reference The object you want to get the position of
     * @return The position of the object, or -1 if the object wasn't found
     */
    int referenceIndex(Reference* reference) const;

    /**
     * Fired whenever the list of references has changed
     */
    Q_SIGNAL void referencesChanged();

    /**
     * Fired when a new reference has been added to the list
     * @param reference The new reference
     */
    Q_SIGNAL void referenceAdded(QObject *referece);

    /**
     * Swap the two given references in the ordered list of references
     * (if either doesn't exist, this will fail quietly)
     * @param swapThis The first object, which will take the position of the second
     * @param withThis The second object, which will take the position of the first
     */
    Q_INVOKABLE void swapReferences(QObject *swapThis, QObject* withThis);
    /**
     * A convenience function for swapping reference positions directly by ID.
     * @param swapThis The index of the first object, which will take the position of the second
     * @param withThis The index of the second object, which will take the position of the first
     */
    Q_INVOKABLE void swapReferencesByIndex(int swapThis, int withThis);
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFREFERENCES_H
