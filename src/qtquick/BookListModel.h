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

#ifndef BOOKLISTMODEL_H
#define BOOKLISTMODEL_H

#include "CategoryEntriesModel.h"
#include <QQmlParserStatus>
/**
 * \brief Main catalogue model class.
 * 
 * BookListModel extends CategoryEntriesModel, and is the main model that
 * handles book entries and the different categories that books can be in.
 * 
 * It also extends QQmlParseStatus to ensure that the loading the cache of
 * books is postponed until the application UI has been painted at least once.
 * 
 * BookListModel keeps track of which books there are, how they can be sorted
 * and how far the reader is in reading a specific book.
 * 
 * It caches its entries in the BookDataBase.
 * 
 * ContentModel is the model used to enable searching the collection, it is
 * typically a ContentList.
 */
class BookListModel : public CategoryEntriesModel, public QQmlParserStatus
{
    Q_OBJECT
    /**
     * \brief count holds how many entries there are in the catalogue.
     */
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    /**
     * \brief The content model is an abstract list model that holds data to search through.
     */
    Q_PROPERTY(QObject* contentModel READ contentModel WRITE setContentModel NOTIFY contentModelChanged)
    /**
     * \brief The "newly added" category entries model manages the newly added entries.
     */
    Q_PROPERTY(QObject* newlyAddedCategoryModel READ newlyAddedCategoryModel NOTIFY newlyAddedCategoryModelChanged)
    /**
     * \brief The "title" category entries model manages the sorting of entries by title.
     */
    Q_PROPERTY(QObject* titleCategoryModel READ titleCategoryModel NOTIFY titleCategoryModelChanged)
    /**
     * \brief The "author" category entries model manages the sorting of entries by author.
     */
    Q_PROPERTY(QObject* authorCategoryModel READ authorCategoryModel NOTIFY authorCategoryModelChanged)
    /**
     * \brief The "series" category entries model managed the sorting of entry by series.
     */
    Q_PROPERTY(QObject* seriesCategoryModel READ seriesCategoryModel NOTIFY seriesCategoryModelChanged)
    /**
     * \brief The "publisher" category entries model managed the sorting of entry by publisher.
     */
    Q_PROPERTY(QObject* publisherCategoryModel READ publisherCategoryModel NOTIFY publisherCategoryModelChanged)
    /**
     * \brief The "keyword" category entries model managed the sorting of entry by keyword.
     */
    Q_PROPERTY(QObject* keywordCategoryModel READ keywordCategoryModel NOTIFY keywordCategoryModelChanged)
    /**
     * \brief The "folder" category entries model managed the sorting of entry by file system folder.
     */
    Q_PROPERTY(QObject* folderCategoryModel READ folderCategoryModel NOTIFY folderCategoryModelChanged)
    /**
     * \brief cacheLoaded holds whether the database cache has been loaded..
     */
    Q_PROPERTY(bool cacheLoaded READ cacheLoaded NOTIFY cacheLoadedChanged)
    Q_ENUMS(Grouping)
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit BookListModel(QObject* parent = nullptr);
    ~BookListModel() override;

    /**
     * Inherited from QmlParserStatus, not implemented.
     */
    void classBegin() override {};
    /**
     * \brief triggers the loading of the cache.
     * Inherited from QmlParserStatus
     */
    void componentComplete() override;

    /**
     * \brief Enum holding the different categories implemented.
     */
    enum Grouping {
        GroupByNone = 0,
        GroupByRecentlyAdded,
        GroupByRecentlyRead,
        GroupByTitle,
        GroupByAuthor,
        GroupByPublisher
    };

    /**
     * @return the contentModel. Used for searching.
     */
    QObject* contentModel() const;
    /**
     * \brief set the ContentModel.
     * @param newModel The new content model.
     */
    void setContentModel(QObject* newModel);
    /**
     * \brief Fires when the content model has changed.
     */
    Q_SIGNAL void contentModelChanged();

    /**
     * @returns how many entries there are in the catalogue.
     */
    int count() const;
    /**
     * \brief Fires when the count has changed.
     */
    Q_SIGNAL void countChanged();

    /**
     * @return The categoryEntriesModel that manages the sorting of entries by title.
     */
    QObject* titleCategoryModel() const;
    /**
     * \brief Fires when the titleCategoryModel has changed or finished initializing.
     */
    Q_SIGNAL void titleCategoryModelChanged();

    /**
     * @return The categoryEntriesModel that manages the recently added entries.
     */
    QObject* newlyAddedCategoryModel() const;
    /**
     * \brief Fires when the newlyAddedCategoryModel has changed or finished initializing.
     */
    Q_SIGNAL void newlyAddedCategoryModelChanged();

    /**
     * @return The categoryEntriesModel that manages the sorting of entries by author.
     */
    QObject* authorCategoryModel() const;
    /**
     * \brief Fires when the authorCategoryModel has changed or finished initializing.
     */
    Q_SIGNAL void authorCategoryModelChanged();

    /**
     * @return The categoryEntriesModel that manages the sorting of entries by series.
     */
    QObject* seriesCategoryModel() const;
    /**
     * \brief Fires when the seriesCategoryModel has changed or finished initializing.
     */
    Q_SIGNAL void seriesCategoryModelChanged();
    
    /**
     * Returns the leaf model representing the series the entry with the passed URL is a part of
     * Base assumption: A book is only part of one series. This is not always true, but not sure how
     * to sensibly represent that.
     * 
     * @param fileName the File Name of the entry to get the series of.
     */
    Q_INVOKABLE QObject* seriesModelForEntry(QString fileName);

    /**
     * @return The categoryEntriesModel that manages the sorting of entries by publisher.
     */
    QObject* publisherCategoryModel() const;
    /**
     * \brief Fires when the publisherCategoryModel has changed or finished initializing.
     */
    Q_SIGNAL void publisherCategoryModelChanged();
    /**
     * @return The categoryEntriesModel that manages the sorting of entries by keywords, names and genres.
     */
    QObject* keywordCategoryModel() const;
    /**
     * \brief Fires when the keywordCategoryModel has changed or finished initializing.
     */
    Q_SIGNAL void keywordCategoryModelChanged();
    /**
     * @return The categoryEntriesModel that manages the sorting of entries by folder.
     */
    QObject* folderCategoryModel() const;
    /**
     * \brief Fires when the folderCategoryModel has changed or finished initializing.
     */
    Q_SIGNAL void folderCategoryModelChanged();

    /**
     * @returns whether the cache is loaded from the database.
     */
    bool cacheLoaded() const;
    /**
     * \brief Fires when the cache is done loading.
     */
    Q_SIGNAL void cacheLoadedChanged();

    /**
     * \brief Update the data of a book at runtime
     * 
     * This is used in to update totalPages and currentPage.
     * 
     * @param fileName The filename to update the page for.
     * @param property The property to update, can be "currentPage" or
     * "totalPages".
     * @param value The value to set it to.
     */
    Q_INVOKABLE void setBookData(QString fileName, QString property, QString value);

    /**
     * Delete a book from the model, and optionally delete the entry from file storage.
     * @param fileName The filename of the book to remove.
     * @param deleteFile Whether to also delete the file from the disk.
     */
    Q_INVOKABLE void removeBook(QString fileName, bool deleteFile = false);

    /**
     * \brief A list of the files currently known by the applications
     * @returns a QStringList with paths to known books.
     */
    Q_INVOKABLE QStringList knownBookFiles() const;
private:
    class Private;
    Private* d;

    Q_SLOT void contentModelItemsInserted(QModelIndex index,int first, int last);
};

#endif//BOOKLISTMODEL_H
