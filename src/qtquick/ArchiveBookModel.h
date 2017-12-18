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
    Q_PROPERTY(bool hasUnsavedChanges READ hasUnsavedChanges NOTIFY hasUnsavedChangesChanged)
public:
    explicit ArchiveBookModel(QObject* parent = 0);
    ~ArchiveBookModel() override;

    void setFilename(QString newFilename) override;

    QString author() const override;
    void setAuthor(QString newAuthor) override;
    QString publisher() const override;
    void setPublisher(QString newPublisher) override;
    QString title() const override;
    void setTitle(QString newTitle) override;

    QObject* qmlEngine() const;
    void setQmlEngine(QObject* newEngine);
    Q_SIGNAL void qmlEngineChanged();

    bool readWrite() const;
    void setReadWrite(bool newReadWrite);
    Q_SIGNAL void readWriteChanged();

    bool hasUnsavedChanges() const;
    Q_INVOKABLE void setDirty(bool isDirty = true);
    Q_SIGNAL void hasUnsavedChangesChanged();

    /**
     * Saves the archive back to disk
     * @return True if the save was successful
     */
    Q_INVOKABLE bool saveBook();

    void addPage(QString url, QString title) override;
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
    Q_INVOKABLE void swapPages(int swapThisIndex, int withThisIndex) override;

    /**
     * Creates a new book in the folder, with the given title and cover.
     * A filename will be constructed to fit the title, and which does not already exist in the
     * directory.
     */
    Q_INVOKABLE QString createBook(QString folder, QString title, QString coverUrl);

    friend class ArchiveImageProvider;
protected:
    const KArchiveFile* archiveFile(const QString& filePath);

private:
    class Private;
    Private* d;
};

#endif//ARCHIVEBOOKMODEL_H
