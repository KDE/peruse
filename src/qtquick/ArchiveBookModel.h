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

#ifndef ARCHIVEBOOKMODEL_H
#define ARCHIVEBOOKMODEL_H

#include "BookModel.h"

class KArchiveFile;
class ArchiveBookModel : public BookModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* qmlEngine READ qmlEngine WRITE setQmlEngine NOTIFY qmlEngineChanged)
public:
    explicit ArchiveBookModel(QObject* parent = 0);
    virtual ~ArchiveBookModel();

    virtual void setFilename(QString newFilename);

    QObject* qmlEngine() const;
    void setQmlEngine(QObject* newEngine);
    Q_SIGNAL void qmlEngineChanged();

    friend class ArchiveImageProvider;
protected:
    const KArchiveFile* archiveFile(const QString& filePath);

private:
    class Private;
    Private* d;
};

#endif//ARCHIVEBOOKMODEL_H
