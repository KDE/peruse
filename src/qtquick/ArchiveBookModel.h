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
    Q_PROPERTY(bool readWrite READ readWrite WRITE setReadWrite NOTIFY readWriteChanged)
    Q_PROPERTY(bool hasUnsavedChanged READ hasUnsavedChanged NOTIFY hasUnsavedChangedChanged)
public:
    explicit ArchiveBookModel(QObject* parent = 0);
    virtual ~ArchiveBookModel();

    virtual void setFilename(QString newFilename) override;

    QObject* qmlEngine() const;
    void setQmlEngine(QObject* newEngine);
    Q_SIGNAL void qmlEngineChanged();

    bool readWrite() const;
    void setReadWrite(bool newReadWrite);
    Q_SIGNAL void readWriteChanged();

    bool hasUnsavedChanged() const;
    Q_SIGNAL void hasUnsavedChangedChanged();

    /**
     * Saves the archive back to disk
     * @return True if the save was successful
     */
    Q_INVOKABLE bool saveBook();

    virtual void addPage(QString url, QString title) override;
    /**
     * Adds a new page to the book archive on disk, by copying in the file
     * passed to the function. Optionally this can be done at a specific
     * position in the book.
     *
     * @param fileUrl     The URL of the file to copy into the archive
     * @param insertAfter The index to insert the new page after. If invalid, insertion will be at the end
     */
    Q_INVOKABLE void addPageFromFile(QString fileUrl, int insertAfter = -1);

    /**
     * @brief Swap the two pages at the specified indices
     *
     * This will change the order in the archive file as well (that is, renaming the files inside the archive)
     *
     * @param swapThisIndex The index of the first page to be swapped
     * @param withThisIndex The index of the page you want the first to be swapped with
     */
    Q_INVOKABLE virtual void swapPages(int swapThisIndex, int withThisIndex) override;

    friend class ArchiveImageProvider;
protected:
    const KArchiveFile* archiveFile(const QString& filePath);

private:
    class Private;
    Private* d;
};

#endif//ARCHIVEBOOKMODEL_H
