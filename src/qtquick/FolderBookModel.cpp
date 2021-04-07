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

#include "FolderBookModel.h"
#include <QMimeDatabase>
#include <QDir>
#include <KFileMetaData/UserMetaData>

FolderBookModel::FolderBookModel(QObject* parent)
    : BookModel(parent)
{
}

FolderBookModel::~FolderBookModel()
{
}

void FolderBookModel::setFilename(QString newFilename)
{
    setProcessing(true);
    clearPages();

    QMimeDatabase mimeDb;
    QString mimeType = mimeDb.mimeTypeForFile(newFilename).name();
    QString currentPageFile;
    if(mimeType == "image/jpeg" || mimeType == "image/png") {
        QFileInfo file(newFilename);
        newFilename = file.absolutePath();
        currentPageFile = file.fileName();
    }

    QDir dir(newFilename);
    if(dir.exists())
    {
        QFileInfoList entries = dir.entryInfoList(QDir::Files, QDir::Name);
        QLatin1String undesired("thumbs.db");
        int i = 0;
        for(const QFileInfo& entry : qAsConst(entries))
        {
            if(entry.fileName().toLower() == undesired) {
                continue;
            }
            addPage(QString("file://").append(entry.canonicalFilePath()), entry.fileName());
            if(currentPageFile == entry.fileName()) {
                BookModel::setCurrentPage(i, false);
            }
            ++i;
        }
    }

    BookModel::setFilename(newFilename);

    // We might have opened this in the past, so reset the current page to the actual one
    // rather than the filename we tried to open. Recent File is the opened page, rather
    // than the directory (because it makes for much simpler code), so reset rather than
    // doing other magic.
    KFileMetaData::UserMetaData data(filename());
    if(data.hasAttribute("peruse.currentPage"))
        BookModel::setCurrentPage(data.attribute("peruse.currentPage").toInt(), false);

    emit loadingCompleted(true);
    setProcessing(false);
}

