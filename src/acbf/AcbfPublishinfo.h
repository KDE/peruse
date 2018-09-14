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

#ifndef ACBFPUBLISHINFO_H
#define ACBFPUBLISHINFO_H

#include <memory>

#include "AcbfMetadata.h"

#include <QDate>
/**
 * \brief Class to handle the publishing info.
 * 
 * The Publishing info in ACBF is about who published the book,
 * when, where, the ISBN number, and which license it was published
 * under.
 */
namespace AdvancedComicBookFormat
{
class ACBF_EXPORT PublishInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString publisher READ publisher WRITE setPublisher NOTIFY publisherChanged)
    Q_PROPERTY(QDate publishDate READ publishDate WRITE setPublishDate NOTIFY publishDateChanged)
    Q_PROPERTY(QString city READ city WRITE setCity NOTIFY cityChanged)
    Q_PROPERTY(QString isbn READ isbn WRITE setIsbn NOTIFY isbnChanged)
    Q_PROPERTY(QString license READ license WRITE setLicense NOTIFY licenseChanged)
public:
    explicit PublishInfo(Metadata* parent = nullptr);
    ~PublishInfo() override;

    /**
     * \brief Write the publishingInfo into the xml writer.
     */
    void toXml(QXmlStreamWriter *writer);
    /**
     * \brief load the publishing info into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return the name of the publisher.
     */
    QString publisher() const;
    /**
     * \brief set the name of the publisher.
     */
    void setPublisher(const QString& publisher);
    /**
     * @brief fires when the publisher has changed.
     */
    Q_SIGNAL void publisherChanged();

    /**
     * @return the publishing date as a QDate object.
     */
    QDate publishDate() const;
    /**
     * \brief set the publishing date.
     * @param publishDate - the publishingdate as a QDate object.
     */
    void setPublishDate(const QDate& publishDate);
    /**
     * @brief a function to work around strangeness surrounding qml date.
     * @param year - publishing year.
     * @param month - publishing month, from 1 to 12
     * @param day - publishing day.
     */
    Q_INVOKABLE void setPublishDateFromInts(const int& year, const int& month, const int& day);
    /**
     * @brief fires when the date has changed.
     */
    Q_SIGNAL void publishDateChanged();

    /**
     * @return the name of the city the work was published in.
     */
    QString city() const;
    /**
     * \brief set the name of the city the work was published in.
     * @param city - the name of the city as a QString.
     */
    void setCity(const QString& city = QString());
    /**
     * @brief fires when the city has changed.
     */
    Q_SIGNAL void cityChanged();

    /**
     * @return the ISBN number as a QString.
     */
    QString isbn() const;
    /**
     * \brief set the ISBN number.
     * @param isbn - the ISBN number as a QString. ISBN numbers should be registered.
     */
    void setIsbn(const QString& isbn = QString());
    /**
     * @brief fires when the usbn has changed.
     */
    Q_SIGNAL void isbnChanged();

    /**
     * @return the name of the license the comic was published under.
     */
    QString license() const;
    /**
     * \brief set the License that the work was published under.
     * @param license - the name of the license as a QString.
     */
    void setLicense(const QString& license = QString());
    /**
     * @brief fires when the license has changed.
     */
    Q_SIGNAL void licenseChanged();
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFPUBLISHINFO_H
