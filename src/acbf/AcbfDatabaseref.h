/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef ACBFDATABASEREF_H
#define ACBFDATABASEREF_H

#include <memory>
#include "AcbfBookinfo.h"
/**
 * \brief Class to handle references to this book in catalogues and databases.
 * 
 * Books can have entries in databases and library catalogues. If such an entry
 * exists, the DatabaseRef object can hold it. Such entries are useful to figure
 * out if two stories are related, or to retrieve extra information about the book
 * such as collections it may be part of.
 * 
 * dbname is the name of the database.
 * type is the type of reference it is, such as IssueID, SeriesID, or just plain URL.
 * reference is the actual reference.
 */
namespace AdvancedComicBookFormat
{
class ACBF_EXPORT DatabaseRef : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString dbname READ dbname WRITE setDbname NOTIFY dbnameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString reference READ reference WRITE setReference NOTIFY referenceChanged)
public:
    explicit DatabaseRef(BookInfo* parent = nullptr);
    ~DatabaseRef() override;

    /**
     * \brief write the contents of this object into the xmlwriter.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a DatabaseRef section from the xml into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return the name of the database.
     */
    Q_INVOKABLE QString dbname() const;
    /**
     * \brief set the name of the database for this reference.
     * @param dbname - the name of the database.
     */
    Q_INVOKABLE void setDbname(const QString& dbname);
    /**
     * @brief fires when the dbname changes;
     */
    Q_SIGNAL void dbnameChanged();
    /**
     * @returns the type of reference the reference is.
     */
    Q_INVOKABLE QString type() const;
    /**
     * \brief set which type of reference the reference is.
     * @param type - the type of reference.
     */
    Q_INVOKABLE void setType(const QString& type);
    /**
     * @brief fires when the type of the reference changes.
     */
    Q_SIGNAL void typeChanged();
    /**
     * @returns the reference.
     */
    Q_INVOKABLE QString reference() const;
    /**
     * \brief set the reference as a string.
     * @param reference - the reference a QString.
     */
    Q_INVOKABLE void setReference(const QString& reference);
    /**
     * @brief fires when the reference that this object has of the book
     * is changed.
     */
    Q_SIGNAL void referenceChanged();
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFDATABASEREF_H
