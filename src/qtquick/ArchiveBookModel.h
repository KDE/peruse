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
    Q_PROPERTY(QStringList fileEntries READ fileEntries NOTIFY fileEntriesChanged)
    Q_PROPERTY(QStringList fileEntriesToDelete READ fileEntriesToDelete NOTIFY fileEntriesToDeleteChanged)
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
     * Whether or not this model should function in read/write mode. As this is potentially very expensive,
     * this option is disabled by default and must be set explicitly to true.
     * @return Whether or not the model is read/write (true) or in read-only mode (false)
     */
    bool readWrite() const;
    /**
     * Sets the readWrite option
     * @see readWrite()
     * @param newReadWrite Whether or not the model should be read/write (true) or in read-only mode (false)
     */
    void setReadWrite(bool newReadWrite);
    /**
     * Fired when the read/write property changes
     */
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
     * A list of every file contained within the archive, not just the pages
     * @return A list of files relative to the archive root
     */
    QStringList fileEntries() const;
    /**
     * Fired when the contents of the archive change
     */
    Q_SIGNAL void fileEntriesChanged();

    /**
     * \brief Whether or not a specific file entry is referenced somewhere in the ACBF document.
     * @param fileEntry The archive filename for the entry you want checked
     * @return Whether the file is referenced or not. 0 if not, 1 if fully matched, 2 if partially matched
     * @see fileEntries()
     * @see markArchiveFileForDeletion(QString,bool)
     */
    Q_INVOKABLE int fileEntryReferenced(const QString& fileEntry) const;

    /**
     * \brief Whether or not an entry is a directory in the archive (as opposed to a file)
     * This becomes useful for distinguishing what should be done for things that are not actually
     * files (and consequently not really directly useful in a book, as you can't simply link to
     * a directory in the archive)
     * @param fileEntry The entry you wish to check
     * @return True if the entry passed in is a directory
     */
    Q_INVOKABLE bool fileEntryIsDirectory(const QString& fileEntry) const;

    /**
     * @brief The list of files currently marked for deletion
     * @return A list of files marked for deletion on the next save action
     */
    QStringList fileEntriesToDelete() const;
    /**
     * Fired whenever the list of file entries which should be deleted changes
     */
    Q_SIGNAL void fileEntriesToDeleteChanged();
    /**
     * \brief Mark a file for removal from the archive
     * When saving the book, files marked for deletion will not be included in the new archive.
     *
     * @param archiveFile The filename of the file to be removed
     * @param markForDeletion Whether the archive file should be deleted or not
     */
    Q_INVOKABLE void markArchiveFileForDeletion(const QString& archiveFile, bool markForDeletion = true);

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
     * @note This does not remove the file pointed to by the page
     * @see markArchiveFileForDeletion(QString,bool)
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

    /**
     * Get the preview URL for an acbf item with the given ID.
     * @note If you are requesting a preview for an entry in the ACBF data, prepend the ID with a # symbol
     * @param id The ID of the item to get a preview URL for
     * @return The preview URL for the item with the given ID (this is NOT checked for validity, only constructed)
     */
    Q_INVOKABLE QString previewForId(const QString& id) const;

    /**
     * Get the family name for a font by its filename as stored in the ACBF data, or in the archive.
     * The font will be loaded on the first call of this function, and removed when the model
     * is destructed.
     * @param fontFileName The filename, in the acbf document or archive, of the font you wish to load
     * @return The first family name in the font, or an empty string if none was found
     */
    Q_INVOKABLE QString fontFamilyName(const QString& fontFileName);

    /**
     * Goes through a list of font families (such as that in a stylesheet) and returns the first one
     * available in the system. This function will also attempt to load any fonts that are in the
     * acbf data, or in the archive (using the fontFamilyName(QString) function)
     * @param fontList The ordered list of fonts to attempt to locate, in a fail-through fashion
     * @param The first font in the list that's available on the system. If none is found, an empty string is returned.
     */
    Q_INVOKABLE QString firstAvailableFont(const QStringList& fontList);

    friend class ArchiveImageRunnable;
protected:
    const KArchiveFile* archiveFile(const QString& filePath) const;
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
