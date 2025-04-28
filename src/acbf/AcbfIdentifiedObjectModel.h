/*
 * Copyright (C) 2020 Dan Leinir Turthra Jense
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

#ifndef ACBFIDENTIFIEDOBJECTMODEL_H
#define ACBFIDENTIFIEDOBJECTMODEL_H

#include <QAbstractListModel>
#include <memory>

#include "acbf_export.h"

namespace AdvancedComicBookFormat
{

/**
 * \brief A model which exposes all the identified objects (that is, objects with an explicit ID) in a specific AdvancedComicBookFormat::Document
 */
class ACBF_EXPORT IdentifiedObjectModel : public QAbstractListModel
{
    Q_OBJECT
    /**
     * The AdvancedComicBookFormat::Document instance this model should represent objects of
     */
    Q_PROPERTY(QObject *document READ document WRITE setDocument NOTIFY documentChanged)
public:
    explicit IdentifiedObjectModel(QObject *parent = nullptr);
    virtual ~IdentifiedObjectModel();

    enum Roles {
        IdRole = Qt::UserRole + 1, ///< The ID of the object in question. This is supposed to be unique in the document
        OriginalIndexRole, ///< The position of the object inside its original container
        TypeRole, ///< The type of the object - see IdentifiedObjectTypes for the values this can return
        ObjectRole, ///< The underlying object (you can identify the specific type using the Type role)
    };
    enum IdentifiedObjectTypes {
        UnknownType = -1,
        ReferenceType = 0,
        BinaryType = 1,
        TextareaType = 2,
        FrameType = 3,
        PageType = 4,
        JumpType = 5,
    };
    Q_ENUM(IdentifiedObjectTypes)

    /**
     * \brief This gives names for the Roles enum.
     */
    QHash<int, QByteArray> roleNames() const override;
    /**
     * \brief Access the data inside the identified object model.
     * @param index The QModelIndex at which you wish to access the data.
     * @param role An enumerator of the type of data you want to access.
     * Is extended by the Roles enum.
     *
     * @return a QVariant with the requested data, or an invalid variant if no information was available.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /**
     * @param parent The QModel index of the parent, not used here.
     * @returns the number of total number of identified objects in this document.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QObject *document() const;
    void setDocument(QObject *document);
    Q_SIGNAL void documentChanged();

    /**
     * Get the first object from the model which has the given ID
     * \note If identifying for link purposes, don't include the #
     * @param id The ID of the object you wish to get
     * @return Either an object with the given ID, or null if none was found
     */
    Q_INVOKABLE QObject *objectById(const QString &id);

private:
    class Private;
    std::unique_ptr<Private> d;
};
};

#endif // ACBFIDENTIFIEDOBJECTMODEL_H
