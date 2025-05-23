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

#ifndef CATEGORYENTRIESMODEL_H
#define CATEGORYENTRIESMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <qqmlintegration.h>

class CategoryEntriesModel;
/**
 * \brief A struct for an Entry to the Book Database.
 */
struct BookEntry {
    Q_GADGET
    QML_VALUE_TYPE(bookEntry)

    Q_PROPERTY(QString filename MEMBER filename CONSTANT)
    Q_PROPERTY(QString filetitle MEMBER filetitle CONSTANT)
    Q_PROPERTY(QString title MEMBER title CONSTANT)
    Q_PROPERTY(QStringList genres MEMBER genres CONSTANT)
    Q_PROPERTY(QStringList keywords MEMBER keywords CONSTANT)
    Q_PROPERTY(QStringList characters MEMBER characters CONSTANT)
    Q_PROPERTY(QStringList series MEMBER series CONSTANT)
    Q_PROPERTY(QStringList seriesNumbers MEMBER seriesNumbers CONSTANT)
    Q_PROPERTY(QStringList seriesVolumes MEMBER seriesVolumes CONSTANT)
    Q_PROPERTY(QStringList author MEMBER author CONSTANT)
    Q_PROPERTY(QString publisher MEMBER publisher CONSTANT)
    Q_PROPERTY(QDateTime created MEMBER created CONSTANT)
    Q_PROPERTY(QDateTime lastOpenedTime MEMBER lastOpenedTime CONSTANT)
    Q_PROPERTY(int totalPages MEMBER totalPages CONSTANT)
    Q_PROPERTY(int currentPage MEMBER currentPage CONSTANT)
    Q_PROPERTY(QString thumbnail MEMBER thumbnail CONSTANT)
    Q_PROPERTY(QStringList description MEMBER description CONSTANT)
    Q_PROPERTY(QString comment MEMBER comment CONSTANT)
    Q_PROPERTY(QStringList tags MEMBER tags CONSTANT)
    Q_PROPERTY(int rating MEMBER rating CONSTANT)

public:
    explicit BookEntry()
        : totalPages(0)
        , currentPage(0)
        , rating(0)
    {
    }
    QString filename;
    QString filetitle;
    QString title;
    QStringList genres;
    QStringList keywords;
    QStringList characters;
    QStringList series;
    QStringList seriesNumbers;
    QStringList seriesVolumes;
    QStringList author;
    QString publisher;
    QDateTime created;
    QDateTime lastOpenedTime;
    int totalPages;
    int currentPage;
    QString thumbnail;
    QStringList description;
    QString comment;
    QStringList tags;
    int rating;
};

bool operator==(const BookEntry &a1, const BookEntry &a2) noexcept;

/**
 * \brief Model to handle the filter categories.
 *
 * This model in specific handles which categories there are
 * and which books are assigned to a category, if so, which.
 *
 * Used to handle sorting by author, title and so forth.
 * Is extended by BookListModel.
 *
 * categories and book entries are both in the same model
 * because there can be books that are not assigned categories.
 * Similarly, categories can contain categories, like in the case
 * of folder category.
 */
class CategoryEntriesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * \brief count holds how many entries there are in the model - equivalent to rowCount, except as a property
     */
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit CategoryEntriesModel(QObject *parent = nullptr);
    ~CategoryEntriesModel() override;

    /**
     * \brief Extra roles for the book entry access.
     */
    enum Roles {
        UnknownRole = Qt::UserRole,
        FilenameRole = Qt::UserRole + 1, /// For getting a string with the full path to the book.
        FiletitleRole, /// For getting a string with the basename of the book.
        TitleRole, /// For getting a string with the proper title of the book.
        SeriesRole, /// For getting a stringlist of series this book is part of.
        SeriesNumbersRole, /// For getting a stringlist of numbers, which represent the sequence number the book has within each series.
        SeriesVolumesRole, /// For getting a stringlist of numbers, which represent the volume number the book has within a series. This is optional.
        AuthorRole, /// For getting a stringlist of all the authors.
        PublisherRole, /// For getting a string with the publisher name.
        CreatedRole, /// For getting the creation date of the book as a QDateTime.
        LastOpenedTimeRole, /// For getting the last time the book was opened as a QDateTime.
        TotalPagesRole, /// For getting the total amount of pages in this book.
        CurrentPageRole, /// For getting the current page as an int.
        CategoryEntriesModelRole, /// For getting the model of this category.
        CategoryEntriesCountRole, /// For getting the an int with the number of books within this category.
        TypeRole,
        ThumbnailRole, /// For getting a thumbnail url for this book.
        DescriptionRole, /// For getting a stringlist with a book description.
        CommentRole, /// For getting a string with user assigned comment.
        TagsRole, /// For getting a stringlist with user assigned tags.
        RatingRole, /// For getting an int with the rating of the comic. This is gotten from KFileMeta and thus goes from 1-10 with 0 being no rating.
        GenreRole, /// For getting a stringlist with genres assigned to this book.
        KeywordRole, /// For getting a stringlist with keywords assigned to this book. Where tags are user assigned, keywords come from the book itself.
        CharacterRole /// For getting a stringlist with names of characters in this book.
    };
    Q_ENUMS(Roles)

    /**
     * @returns names for the extra roles defined.
     */
    QHash<int, QByteArray> roleNames() const override;
    /**
     * \brief Access the data inside the CategoryEntriesModel.
     * @param index The QModelIndex at which you wish to access the data.
     * @param role An enumerator of the type of data you want to access.
     * Is extended by the Roles enum.
     *
     * @return a QVariant with the book entry's data.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /**
     * @param parent The QModel index of the parent. This only counts for
     * tree like page structures, and thus defaults to a freshly constructed
     * QModelIndex. A wellformed QModelIndex will cause this function to return 0
     * @returns the number of total rows(bookentries and categories) there are.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @returns how many entries there are in the catalogue.
     */
    int count() const;
    /**
     * \brief Fires when the count has changed.
     */
    Q_SIGNAL void countChanged();

    /**
     * \brief Add a book entry to the CategoryEntriesModel.
     *
     * @param entry The BookEntry to add.
     * @param compareRole The role that determines the data to sort the entry into.
     * Defaults to the Book title.
     */
    Q_INVOKABLE void append(const BookEntry &entry, Roles compareRole = TitleRole);

    /**
     * \brief Add a book entry to the model, using a fake book
     *
     * @param book The fake book (such as returned by get(int))
     * @param compareRole The role that determines the data used to sort the entry (defaults to TitleRole)
     */
    Q_INVOKABLE void appendFakeBook(const BookEntry &book, Roles compareRole = TitleRole);

    /**
     * \brief Remove all entries from the model
     */
    Q_INVOKABLE void clear();

    /**
     * \brief Add a book entry to a category.
     *
     * This also adds it to the model's list of entries.
     */
    void addCategoryEntry(const QString &categoryName, const BookEntry &entry, Roles compareRole = TitleRole);

    /**
     * @param index an integer index pointing at the desired book.
     * @returns the BookEntry struct for the given index (owned by this model, do not delete)
     */
    Q_INVOKABLE BookEntry getBookEntry(int index) const;

    Q_INVOKABLE CategoryEntriesModel *getCategoryEntry(int index) const;
    /**
     * @return an entry object for the given filename. Used to get the recently
     * read books.
     * @param filename the filename associated with an entry object.
     */
    Q_INVOKABLE BookEntry bookFromFile(const QString &filename) const;
    /**
     * @return an entry index for the given filename.
     * @param filename the filename associated with an entry object.
     */
    Q_INVOKABLE int indexOfFile(const QString &filename) const;
    /**
     * @return whether the entry is a bookentry or a category entry.
     * @param index the index of the entry.
     */
    Q_INVOKABLE bool indexIsBook(int index) const;
    /**
     * @return an integer with the total books in the model.
     */
    int bookCount() const;

    /**
     * \brief Fires when a book entry is updated.
     * @param entry The updated entry
     *
     * Used in the BookListModel::setBookData()
     */
    Q_SIGNAL void entryDataUpdated(const BookEntry &entry);
    /**
     * \brief set a book entry as changed.
     * @param entry The changed entry.
     */
    Q_SLOT void entryDataChanged(const BookEntry &entry);
    /**
     * \brief Fires when a book entry is removed.
     * @param entry The removed entry
     */
    Q_SIGNAL void entryRemoved(const BookEntry &entry);
    /**
     * \brief Remove a book entry.
     * @param entry The entry to remove.
     */
    Q_SLOT void entryRemove(const BookEntry &entry);

    // This will iterate over all sub-models and find the model which contains the entry, or null if not found
    CategoryEntriesModel *leafModelForEntry(const BookEntry &entry);

protected:
    /**
     * @return the name of the model.
     */
    const QString &name() const;
    /**
     * \brief set the name of the model.
     * @param newName QString with the name.
     */
    void setName(const QString &newName);

private:
    class Private;
    Private *d;
};

#endif // CATEGORYENTRIESMODEL_H
