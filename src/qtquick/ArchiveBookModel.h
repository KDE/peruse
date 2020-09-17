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
#include <QMutex>
/**
 * \brief Class to hold pages and metadata for archive based books.
 * 
 * In particular, ArchiveBookModel handles CBZ and CBR files, reads
 * potential metadata and holds that into the acbfdata object.
 * 
 * ArchiveBookModel extends BookModel, which handles the functions for
 * setting the current page, and returning basic metadata.
 */
class KArchiveFile;
class ArchiveBookModel : public BookModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* qmlEngine READ qmlEngine WRITE setQmlEngine NOTIFY qmlEngineChanged)
    Q_PROPERTY(bool readWrite READ readWrite WRITE setReadWrite NOTIFY readWriteChanged)
    Q_PROPERTY(bool hasUnsavedChanges READ hasUnsavedChanges NOTIFY hasUnsavedChangesChanged)
public:
    explicit ArchiveBookModel(QObject* parent = nullptr);
    ~ArchiveBookModel() override;

    /**
     * \brief Set the filename that points to the archive that describes this book.
     */
    void setFilename(QString newFilename) override;

    /**
     * The author name will be either the default bookmodel author name, or
     * if ACBF data is available, the first authorname in the list of ACBF authors.
     * 
     * @return the author name as a QString.
     */
    QString author() const override;
    /**
     * \brief Set the main author's nickname.
     * 
     * If there is no ACBF data, this will set the author to BookModel's author.
     * If there is ACBF data, this will set the nickname entry on the name of the
     * first possible author.
     * 
     * Preferably authors should be added by editing the author list in the bookinfo
     * of the ACBF metadata this book holds.
     * 
     * @param newAuthor The main author's nickname.
     */
    void setAuthor(QString newAuthor) override;
    /**
     * @return the name of the publisher as a QString.
     */
    QString publisher() const override;
    /**
     * \brief Set the name of the publisher.
     * @param newPublisher QString with the name of the publisher.
     */
    void setPublisher(QString newPublisher) override;
    /**
     * @return The proper title of this book as a QString.
     */
    QString title() const override;
    /**
     * \brief Set the default title of this book.
     * @param newTitle The default title of this book as a QString.
     */
    void setTitle(QString newTitle) override;

    /**
     * @return a QQmlEngine associated with this book.
     * TODO: What is the QML engine and what is its purpose?
     * Used in the cbr.qml
     */
    QObject* qmlEngine() const;
    /**
     * \brief Set the QML engine on this book.
     * @param newEngine A QQmlEngine object.
     */
    void setQmlEngine(QObject* newEngine);
    /**
     * \brief Fires when a new QQmlEngine is set on this book.
     */
    Q_SIGNAL void qmlEngineChanged();

    /**
     * TODO: What is this? Only used in book.qml once?
     */
    bool readWrite() const;
    void setReadWrite(bool newReadWrite);
    Q_SIGNAL void readWriteChanged();

    /**
     * @return whether the book has been modified and has unsaved changes.
     * 
     * Used in PeruseCreator to determine whether to enable the save dialog.
     */
    bool hasUnsavedChanges() const;
    /**
     * \brief Set that the book has been modified.
     * @param isDirty whether the book has been modified.
     */
    Q_INVOKABLE void setDirty(bool isDirty = true);
    /**
     * \brief Fires when there are unsaved changes.
     */
    Q_SIGNAL void hasUnsavedChangesChanged();

    /**
     * \brief Saves the archive back to disk
     * @return True if the save was successful
     */
    Q_INVOKABLE bool saveBook();

    /**
     * \brief add a page to this book.
     * 
     * This adds it to the ACBF metadata too.
     * 
     * @param url The resource location of the page as an url.
     * @param title The title of the page. This is shown in a table of contents.
     */
    void addPage(QString url, QString title) override;

    /**
     * @brief removePage
     * remove the given page from the book by number.
     * @param pageNumber the number of the page to remove.
     */
    Q_INVOKABLE void removePage(int pageNumber) override;
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
     * 
     * @param folder the path to the folder to create this book in.
     * @param title The title of the book.
     * @param coverUrl A resource location pointing at the image that will be the coverpage.
     */
    Q_INVOKABLE QString createBook(QString folder, QString title, QString coverUrl);

    friend class ArchiveImageRunnable;
protected:
    const KArchiveFile* archiveFile(const QString& filePath);
    QMutex archiveMutex;

private:
    class Private;
    /**
     * @brief loadComicInfoXML
     * Loads ComicInfo.xml, this is an old file metadata type used by comicrack, and since then
     * written by other editors, amongst which a callibre plugin.
     * @param xmlDocument string with the archive value.
     * @param acbfData a pointer pointing to a acbfDocument.
     * @param entries a list of image entries, sorted.
     * @param filename the file name of the document, necessary for writing data to kfilemetadata.
     * @return whether the reading was successful.
     */
    bool loadComicInfoXML(QString xmlDocument, QObject* acbfData, QStringList entries, QString filename);
    /**
     * @brief loads CoMet xmls, https://www.denvog.com/comet/comet-specification/
     * @param xmlDocument string with the archive value.
     * @param acbfData a pointer pointing to a acbfDocument.
     * @param entries a list of image entries, sorted.
     * @param filename the file name of the document, necessary for writing data to kfilemetadata.
     * @return whether the reading was successful.
     */
    bool loadCoMet(QStringList xmlDocuments, QObject* acbfData, QStringList entries, QString filename);
    Private* d;
};

#endif//ARCHIVEBOOKMODEL_H
