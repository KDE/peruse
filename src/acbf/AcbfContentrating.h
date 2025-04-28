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

#ifndef ACBFCONTENTRATING_H
#define ACBFCONTENTRATING_H

#include <memory>

#include "AcbfBookinfo.h"
/**
 * \brief Class to handle the content rating of the work.
 *
 * Because there are multiple ContentRating systems over various
 * countries, ACBF can handle multiple content rating objects, and
 * has entries for handling which system said rating is in.
 *
 * This is used in the BookInfo object.
 */
namespace AdvancedComicBookFormat
{
class ACBF_EXPORT ContentRating : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString rating READ rating WRITE setRating NOTIFY ratingChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
public:
    explicit ContentRating(BookInfo *parent = nullptr);
    ~ContentRating() override;

    /**
     * \brief write this content rating to the xml writer.
     */
    void toXml(QXmlStreamWriter *writer);
    /**
     * \brief load a content-rating section from the xml into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return the system that the content rating belongs to.
     */
    Q_INVOKABLE QString type() const;
    /**
     * \brief set the system that defines this rating.
     * @param type - the name of the system.
     */
    Q_INVOKABLE void setType(const QString &type);
    /**
     * @brief fires when the type changes.
     */
    Q_SIGNAL void typeChanged();

    /**
     * @return the rating label as a string.
     */
    Q_INVOKABLE QString rating() const;
    /**
     * \brief set the rating.
     * @param rating - the name of the rating label as a QString.
     */
    Q_INVOKABLE void setRating(const QString &rating);
    /**
     * @brief fires when the rating changes.
     */
    Q_SIGNAL void ratingChanged();

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFCONTENTRATING_H
