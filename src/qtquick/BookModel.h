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

#ifndef BOOKMODEL_H
#define BOOKMODEL_H

#include <QAbstractListModel>
/**
 * \brief Base Class to handle books, their pages and their metadata
 * 
 * BookModel is an QAbstractListModel, holding the pages as a list of objects.
 * 
 * It also holds metadata for the following entries as Q Properties:
 * 
 * - filename.
 * - author
 * - publisher
 * - title
 * - page count.
 * - current page.
 * - acbf data
 * - processing
 * 
 * The book model in turn is extended by ArchiveBookModel and FolderBookModel
 * to provide specialised functionality for archives(zip, rar, cbz, cbr) with
 * a book and Folders with a book and a description file.
 */
class BookModel : public QAbstractListModel
{
    Q_OBJECT
    /**
     * \brief The filename of the archive that describes this book.
     */
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    /**
     * \brief The main author of this book.
     */
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    /**
     * \brief the name of the publisher of this book.
     */
    Q_PROPERTY(QString publisher READ publisher WRITE setPublisher NOTIFY publisherChanged)
    /**
     * \brief The title of the book.
     */
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    /**
     * \brief The page count of the book.
     */
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
    /**
     * \brief The page currently being read of the book.
     */
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    /**
     * The Advanced Comic Book Format data management instance associated with this book
     * This may be null
     */
    Q_PROPERTY(QObject* acbfData READ acbfData NOTIFY acbfDataChanged)
    /**
     * \brief Whether or not the book is still being processed.
     */
    Q_PROPERTY(bool processing READ processing WRITE setProcessing NOTIFY processingChanged)
    /**
     * \brief When processing, this string will commonly be set to something informational
     */
    Q_PROPERTY(QString processingDescription READ processingDescription WRITE setProcessingDescription NOTIFY processingDescriptionChanged)
public:
    explicit BookModel(QObject* parent = nullptr);
    ~BookModel() override;

    /**
     * Extra roles for the page data access.
     */
    enum Roles {
        UrlRole = Qt::UserRole + 1, // This allows access to the resource location of the page.
        TitleRole, // This allows access to the title of the page, if it has one.
    };

    /**
     * \brief This gives names for the Roles enum.
     */
    QHash<int, QByteArray> roleNames() const override;
    /**
     * \brief Access the data inside the BookModel.
     * @param index The QModelIndex at which you wish to access the data.
     * @param role An enumerator of the type of data you want to access.
     * Is extended by the Roles enum.
     * 
     * @return a QVariant with the page data.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    /**
     * @param parent The QModel index of the parent, not used here.
     * @returns the number of total pages there are in the Book.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * \brief add a page to this book.
     * @param url The resource location of the page as an url.
     * @param title The title of the page. This is shown in a table of contents.
     */
    virtual void addPage(QString url, QString title);

    /**
     * @brief removePage
     * Remove this page from the book.
     * @param index the index of the page to be removed.
     */
    virtual void removePage(int pageNumber);

    /**
     * \brief remove all pages from the book.
     */
    virtual void clearPages();

    /**
     * @return the filename of the file that describes this book.
     */
    QString filename() const;
    /**
     * \brief set the filename of the file that describes this book.
     */
    virtual void setFilename(QString newFilename);
    /**
     * \brief Fires when the filename is changed via setfilename.
     */
    Q_SIGNAL void filenameChanged();
    /**
     * @returns the main author of the book as a QString.
     */
    virtual QString author() const;
    /**
     * \brief set the main author of the book as a single string.
     * @param newAuthor The new name associated with the author
     * as a single string.
     */
    virtual void setAuthor(QString newAuthor);
    /**
     * \brief Fires when the author has changed via setAuthor.
     */
    Q_SIGNAL void authorChanged();
    /**
     * @return the name of the publisher as a QString.
     */
    virtual QString publisher() const;
    /**
     * \brief Set the name of the publisher.
     * @param newPublisher String that describes the publisher's name.
     */
    virtual void setPublisher(QString newPublisher);
    /**
     * \brief Fires when publisher's name has changed with setPublisher.
     */
    Q_SIGNAL void publisherChanged();
    /**
     * @return The proper title of the book as a Qstring.
     */
    virtual QString title() const;
    /**
     * \brief Set the title of the book.
     * @param newTitle A QString describing the new title.
     */
    virtual void setTitle(QString newTitle);
    /**
     * \brief Fires when the book's title has changed via SetTitle
     */
    Q_SIGNAL void titleChanged();
    /**
     * @return the total pages in the book as an int.
     */
    virtual int pageCount() const;
    /**
     * \brief Fires when the page count has changed, via for example pages
     * being added or removed.
     */
    Q_SIGNAL void pageCountChanged();

    /**
     * @return the number of the current page being viewed as an int.
     */
    int currentPage() const;
    /**
     * \brief Set the current page.
     * @param newCurrentPage Int with the index of the page to switch to.
     * @param updateFilesystem If this is set to false, the attributes do not get written back to the filesystem. Useful for when the information is first filled out
     */
    virtual void setCurrentPage(int newCurrentPage, bool updateFilesystem = true);
    /**
     * \brief Fires when the current page has changed.
     */
    Q_SIGNAL void currentPageChanged();

    /**
     * @return an object with the acbf data, might be null.
     */
    QObject* acbfData() const;
    /**
     * This is used by subclasses who want to create one such. Until this is called
     * with a valid object, acbfData is null. This function causes BookModel to take
     * ownership of the object. It will further delete any previous objects set as
     * acbfData.
     */
    void setAcbfData(QObject* obj);
    /**
     * \brief Fires when the ACBF data has changed.
     */
    Q_SIGNAL void acbfDataChanged();

    /**
     * @return Whether or not the any processing is currently going on
     */
    bool processing() const;
    /**
     * \brief Set whether it is processing or done.
     * @param processing Whether this model is being processed.
     */
    void setProcessing(bool processing);
    /**
     * \brief Fires when the state of processing has changed.
     */
    Q_SIGNAL void processingChanged();
    /**
     * \brief Fires when the book is done loading, and informs whether it was
     * successful.
     * @param success Whether the book's loading was successful
     * TODO: This isn't triggered by anything right now?
     */
    Q_SIGNAL void loadingCompleted(bool success);

    /**
     * \brief A human readable description of what is currently happening when processing is ongoing
     * \return The description of what's going on
     */
    QString processingDescription() const;
    /**
     * \brief Set the human readable description of what is currently happening
     * \param description The description of what's going on
     */
    void setProcessingDescription(const QString& description);
    /**
     * \brief Fires when the description of what's going on changes
     */
    Q_SIGNAL void processingDescriptionChanged();

    /**
     * @brief Swap the two pages at the specified indices
     * 
     * @param swapThisIndex The index of the first page to be swapped
     * @param withThisIndex The index of the page you want the first to be swapped with
     */
    Q_INVOKABLE virtual void swapPages(int swapThisIndex, int withThisIndex);
private:
    class Private;
    Private* d;
};

#endif//BOOKMODEL_H
