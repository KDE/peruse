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
#include <KNSCore/Engine>

#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QImageReader>
#include <QMimeDatabase>

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
        d->config.group("general").writeEntry("animate jump areas", true);
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
    QStringList recent = d->config.group("general").readEntry("recently opened", QStringList());
    QStringList actualRecent;
    while(recent.count() > 0) {
        QString current = recent.takeFirst();
        if(QFile::exists(current)) {
            actualRecent.append(current);
        }
    }
    return actualRecent;
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
        const QStringList locations = bookLocations();
        // First, get rid of all the entries which start with the newly added location, because that's silly
        QStringList newLocations;
        bool alreadyInThere = false;
        for(const QString& entry : locations) {
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
    const QStringList locations = KNSCore::Engine::configSearchLocations();
    QString knsrc;
    for (const QString& location : locations) {
        knsrc = QString::fromLocal8Bit("%1/peruse.knsrc").arg(location);
        if (QFile(knsrc).exists()) {
            break;
        }
    }
    if(qEnvironmentVariableIsSet("APPDIR"))
    {
        // Because appimage install happens into /app/usr...
        knsrc = knsrc.prepend("/usr").prepend(qgetenv("APPDIR"));
    }
    return knsrc;
}

bool PeruseConfig::animateJumpAreas() const
{
    return d->config.group("general").readEntry("animate jump areas", true);
}

void PeruseConfig::setAnimateJumpAreas(bool animate)
{
    bool animateJumpAreasCurrentVal = animateJumpAreas();

    if(animateJumpAreasCurrentVal != animate) {
        d->config.group("general").writeEntry("animate jump areas", animate);
        d->config.sync();
        emit animateJumpAreasChanged();
    }
}

QString PeruseConfig::homeDir() const
{
    return QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
}

void PeruseConfig::setFilesystemProperty(QString fileName, QString propertyName, QString value)
{
    KFileMetaData::UserMetaData data(fileName);
    if (propertyName == "rating") {
        data.setRating(value.toInt());
    } else if (propertyName == "tags") {
        data.setTags(value.split(","));
    } else if (propertyName == "comment") {
        data.setUserComment(value);
    } else {
        data.setAttribute(QString("peruse.").append(propertyName), value);
    }
}

QString PeruseConfig::getFilesystemProperty(QString fileName, QString propertyName)
{
    QString value;
    KFileMetaData::UserMetaData data(fileName);
    if (propertyName == "rating") {
        value = QString::number(data.rating());
    } else if (propertyName == "tags") {
        value = data.tags().join(",");
    } else if (propertyName == "comment") {
        value = data.userComment();
    } else if (propertyName == "bytes") {
        value = QString::number(QFileInfo(fileName).size());
    } else if (propertyName == "mimetype") {
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(fileName);
        value = mime.name();
    } else {
        value = data.attribute(QString("peruse.").append(propertyName));
    }
    return value;
}

QStringList PeruseConfig::supportedImageFormats() const
{
    QStringList formats;
    for (const QByteArray& format : QImageReader::supportedImageFormats()) {
        formats << format;
    }
    return formats;
}
