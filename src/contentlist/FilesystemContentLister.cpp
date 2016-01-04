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
#include <QCoreApplication>

#include <QDirIterator>
#include <QMimeDatabase>
#include <QVariantHash>
#include <QDebug>

class FilesystemContentLister::Private
{
public:
    Private() {}
    QString searchString;
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

            if(QFileInfo(filePath).isDir())
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
                emit fileFound(filePath, QVariantHash());
            }
            qApp->processEvents();
        }
    }

    emit searchCompleted();
}
