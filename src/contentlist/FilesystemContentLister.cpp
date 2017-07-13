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

#include "FilesystemContentLister.h"

#include <KFileMetaData/UserMetaData>

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDirIterator>
#include <QMimeDatabase>
#include <QTimer>
#include <QVariantHash>

class FilesystemContentLister::Private
{
public:
    Private() {}
    QString searchString;
    QStringList knownFiles;
    QStringList locations;
    QStringList mimetypes;
};

FilesystemContentLister::FilesystemContentLister(QObject* parent)
    : ContentListerBase(parent)
    , d(new Private)
{
}

FilesystemContentLister::~FilesystemContentLister()
{
    delete d;
}

void FilesystemContentLister::addLocation(QString path)
{
    d->locations.append(path);
}

void FilesystemContentLister::addMimetype(QString mimetype)
{
    d->mimetypes.append(mimetype);
}

void FilesystemContentLister::setSearchString(const QString& searchString)
{
    d->searchString = searchString;
}

void FilesystemContentLister::setKnownFiles(QStringList knownFiles)
{
    d->knownFiles = knownFiles;
}

void FilesystemContentLister::startSearch()
{
    QMimeDatabase mimeDb;
    bool useThis(false);

    qDebug() << "Searching in" << d->locations;
    Q_FOREACH(const QString& folder, d->locations)
    {
        QDirIterator it(folder, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString filePath = it.next();
            if(d->knownFiles.contains(filePath)) {
                continue;
            }

            QFileInfo info(filePath);

            if(info.isDir())
            {
                qApp->processEvents();
                continue;
            }
            useThis = false;
            QString mimetype = mimeDb.mimeTypeForFile(filePath, QMimeDatabase::MatchExtension).name();
//             qDebug() << useThis << mimetype << filePath;
            Q_FOREACH(const QString& type, d->mimetypes)
            {
                if(type == mimetype) {
                    useThis = true;
                    break;
                }
            }

            if(useThis)
            {
                QVariantHash metadata;
                metadata["created"] = info.created();

                KFileMetaData::UserMetaData data(filePath);
                if (data.hasAttribute("peruse.currentPage")) {
                    int currentPage = data.attribute("peruse.currentPage").toInt();
                    metadata["currentPage"] = QVariant::fromValue<int>(currentPage);
                }
                if (data.hasAttribute("peruse.totalPages")) {
                    int totalPages = data.attribute("peruse.totalPages").toInt();
                    metadata["totalPages"] = QVariant::fromValue<int>(totalPages);
                }

                emit fileFound(filePath, metadata);
            }
            qApp->processEvents();
        }
    }
    // This ensures that, should we decide to search more stuff later, we can do so granularly
    d->locations.clear();

    // Not entirely happy about this, but it makes things not break...
    // Previously, the welcome page in Peruse would end up unpopulated because a signal
    // was unreceived from the main window upon search completion (and consequently
    // application readiness)
    QTimer::singleShot(0, this, SIGNAL(searchCompleted()));
}
