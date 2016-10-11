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

#include "PeruseConfig.h"

#include <KFileMetaData/UserMetaData>
#include <KConfig>
#include <KConfigGroup>

#include <QTimer>

class PeruseConfig::Private
{
public:
    Private()
        : config("peruserc")
    {};
    KConfig config;
};

PeruseConfig::PeruseConfig(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
    QStringList locations = d->config.group("general").readEntry("book locations", QStringList());
    if(locations.count() < 1)
    {
        locations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        locations << QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
        locations << QString("%1/comics").arg(QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).first());
        d->config.group("general").writeEntry("book locations", locations);
        d->config.sync();
    }
}

PeruseConfig::~PeruseConfig()
{
    delete d;
}

void PeruseConfig::bookOpened(QString path)
{
    QStringList recent = recentlyOpened();

    int i = recent.indexOf(path);
    if(i == 0)
    {
        // This is already first, don't do work we don't need to, because that's just silly
        return;
    }
    else
    {
        recent.removeAll(path);
        recent.prepend(path);
    }
    d->config.group("general").writeEntry("recently opened", recent);
    d->config.sync();
    emit recentlyOpenedChanged();
}

QStringList PeruseConfig::recentlyOpened() const
{
    return d->config.group("general").readEntry("recently opened", QStringList());
}

void PeruseConfig::addBookLocation(const QString& location)
{
    if(location.startsWith("file://"))
    {
#ifdef Q_OS_WIN
        QString newLocation = location.mid(8);
#else
        QString newLocation = location.mid(7);
#endif
        QStringList locations = bookLocations();
        // First, get rid of all the entries which start with the newly added location, because that's silly
        QStringList newLocations;
        bool alreadyInThere = false;
        Q_FOREACH(QString entry, locations) {
            if(!entry.startsWith(newLocation))
            {
                newLocations.append(entry);
            }
            if(newLocation.startsWith(entry))
            {
                alreadyInThere = true;
            }
        }
        if(alreadyInThere)
        {
            // Don't be silly, don't add a new location if it's already covered by something more high level...
            emit showMessage("Attempted to add a new location to the list of search folders which is a sub-folder to something already in the list.");
            return;
        }
        newLocations.append(newLocation);
        d->config.group("general").writeEntry("book locations", newLocations);
        d->config.sync();
        emit bookLocationsChanged();
    }
}

void PeruseConfig::removeBookLocation(const QString& location)
{
    QStringList locations = bookLocations();
    locations.removeAll(location);
    d->config.group("general").writeEntry("book locations", locations);
    d->config.sync();
    QTimer::singleShot(100, this, SIGNAL(bookLocationsChanged()));
}

QStringList PeruseConfig::bookLocations() const
{
    QStringList locations = d->config.group("general").readEntry("book locations", QStringList());
    return locations;
}

QString PeruseConfig::newstuffLocation() const
{
    return QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation).last().append("/peruse.knsrc");
}

QString PeruseConfig::homeDir() const
{
    return QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
}

void PeruseConfig::setFilesystemProperty(QString fileName, QString propertyName, QString value)
{
    KFileMetaData::UserMetaData data(fileName);
    data.setAttribute(QString("peruse.").append(propertyName), value);
}
