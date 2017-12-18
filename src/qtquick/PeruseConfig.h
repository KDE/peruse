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

class PeruseConfig : public QObject
{
    Q_OBJECT
    Q_ENUMS(ZoomMode)
    Q_PROPERTY(QStringList recentlyOpened READ recentlyOpened NOTIFY recentlyOpenedChanged)
    Q_PROPERTY(QStringList bookLocations READ bookLocations NOTIFY bookLocationsChanged)
    Q_PROPERTY(QString newstuffLocation READ newstuffLocation NOTIFY newstuffLocationChanged)
public:
    enum ZoomMode {
        ZoomFull = 0,
        ZoomFitWidth = 1,
        ZoomFitHeight = 2
    };

    explicit PeruseConfig(QObject* parent = nullptr);
    ~PeruseConfig() override;

    Q_INVOKABLE void bookOpened(QString path);
    QStringList recentlyOpened() const;
    Q_SIGNAL void recentlyOpenedChanged();

    Q_INVOKABLE void addBookLocation(const QString& location);
    Q_INVOKABLE void removeBookLocation(const QString& location);
    QStringList bookLocations() const;
    Q_SIGNAL void bookLocationsChanged();

    QString newstuffLocation() const;
    Q_SIGNAL void newstuffLocationChanged();

    Q_SIGNAL void showMessage(QString message);

    // This should go somewhere more sensible, really... like a static on Qt. or something :P
    Q_INVOKABLE QString homeDir() const;
    Q_INVOKABLE void setFilesystemProperty(QString fileName, QString propertyName, QString value);
private:
    class Private;
    Private* d;
};

#endif//PERUSECONFIG_H
