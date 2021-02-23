/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef PERUSECONFIG_H
#define PERUSECONFIG_H

#include <QObject>
/**
 * \brief Class to handle reading and writing the configuration.
 * 
 * Peruse will store the data in 'peruserc' at the QStandardPath
 * for configuration.
 */
class PeruseConfig : public QObject
{
    Q_OBJECT
    Q_ENUMS(ZoomMode)
    /**
     * \brief recently opened represents the recently opened books.
     */
    Q_PROPERTY(QStringList recentlyOpened READ recentlyOpened NOTIFY recentlyOpenedChanged)
    /**
     * \brief book locations is where Peruse will look for books, whether they have been removed, added, etc.
     */
    Q_PROPERTY(QStringList bookLocations READ bookLocations NOTIFY bookLocationsChanged)
    /**
     * \brief new stuff location is for the location of the KNewStuff config for the GHNS protocol.
     */
    Q_PROPERTY(QString newstuffLocation READ newstuffLocation NOTIFY newstuffLocationChanged)
    /**
     * \brief boolean representing whether or not we should animate jumps on the page
     */
    Q_PROPERTY(bool animateJumpAreas READ animateJumpAreas WRITE setAnimateJumpAreas NOTIFY animateJumpAreasChanged)
public:
    /**
     * \brief Enum holding the preferred zoom mode.
     */
    enum ZoomMode {
        ZoomFull = 0,
        ZoomFitWidth = 1,
        ZoomFitHeight = 2
    };

    explicit PeruseConfig(QObject* parent = nullptr);
    ~PeruseConfig() override;

    /**
     * \brief Add a book to the recently opened list.
     * @param path the path/filename of the newly opened book.
     */
    Q_INVOKABLE void bookOpened(QString path);
    /**
     * @return a list of recently opened files.
     */
    QStringList recentlyOpened() const;
    /**
     * \brief Fires when the list of recently opened files has changed.
     */
    Q_SIGNAL void recentlyOpenedChanged();

    /**
     * \brief Add a folder to the book locations.
     * @param location path to the folder to add.
     */
    Q_INVOKABLE void addBookLocation(const QString& location);
    /**
     * \brief Remove a folder from the book locations.
     * @param location path of the folder to remove.
     */
    Q_INVOKABLE void removeBookLocation(const QString& location);
    /**
     * @return a QStringList with paths to all the folders to check for books.
     */
    QStringList bookLocations() const;
    /**
     * \brief Fires when the book locations to check have changed.
     */
    Q_SIGNAL void bookLocationsChanged();

    /**
     * \brief Holds url to the peruse's KNewStuff configuration fle, to
     * make it easy to retrieve.
     */
    QString newstuffLocation() const;
    /**
     * \brief Fires when the location to the KNewStuff config is changed.
     */
    Q_SIGNAL void newstuffLocationChanged();

    /**
     * @return a boolean representing  value of animateJumpAreas property
     */
    bool animateJumpAreas() const;

    /**
     * \biref sets the animateJumpAreas property value to the passed parameter
     * @param animate the new value for animateJumpAreas property
     */
    void setAnimateJumpAreas(bool animate);

    /**
     * \biref Fires when the animateJumpAreas property gets changed
     */
    Q_SIGNAL void animateJumpAreasChanged();

    /**
     * \brief Fires when there is an config error message to show.
     * @param message The Error message to show.
     */
    Q_SIGNAL void showMessage(QString message);

    // This should go somewhere more sensible, really... like a static on Qt. or something :P
    Q_INVOKABLE QString homeDir() const;
    /**
     * Creates a KFileMetaData::UserMetaData for this file, property and value so the information is not lost when files are moved around outside of Peruse
     */
    Q_INVOKABLE void setFilesystemProperty(QString fileName, QString propertyName, QString value);
    /**
     * @brief getFilesystemProperty
     * @param fileName file name of the file to get data from.
     * @param propertyName value of the proper to get data from.
     * @return the value of the property.
     */
    Q_INVOKABLE QString getFilesystemProperty(QString fileName, QString propertyName);

    /**
     * Convenience function which simply forwards the information from
     * QImageReader::supportedImageFormats() in a QML-friendly format.
     * @return A list of image formats by short name - see the documentation for QImageReader::supportedImageFormats() for further details
     */
    Q_INVOKABLE QStringList supportedImageFormats() const;
private:
    class Private;
    Private* d;
};

#endif//PERUSECONFIG_H
