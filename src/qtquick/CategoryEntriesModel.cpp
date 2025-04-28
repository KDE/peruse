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

#include "CategoryEntriesModel.h"

#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>

#include <KFileMetaData/UserMetaData>

class CategoryEntriesModel::Private {
public:
    Private(CategoryEntriesModel* qq)
        : q(qq)
    {};
    ~Private() = default;
    CategoryEntriesModel* q;
    QString name;
    QList<BookEntry> entries;
    QList<CategoryEntriesModel *> categoryModels;
};

bool operator==(const BookEntry &b1, const BookEntry &b2) noexcept
{
    return b1.filename == b2.filename;
}

CategoryEntriesModel::CategoryEntriesModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
    connect(this, &CategoryEntriesModel::entryDataUpdated, this, &CategoryEntriesModel::entryDataChanged);
    connect(this, &CategoryEntriesModel::entryRemoved, this, &CategoryEntriesModel::entryRemove);
}

CategoryEntriesModel::~CategoryEntriesModel()
{
    delete d;
}

QHash<int, QByteArray> CategoryEntriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilenameRole] = "filename";
    roles[FiletitleRole] = "filetitle";
    roles[TitleRole] = "title";
    roles[GenreRole] = "genres";
    roles[KeywordRole] = "keywords";
    roles[SeriesRole] = "series";
    roles[SeriesNumbersRole] = "seriesNumber";
    roles[SeriesVolumesRole] = "seriesVolume";
    roles[AuthorRole] = "author";
    roles[PublisherRole] = "publisher";
    roles[CreatedRole] = "created";
    roles[LastOpenedTimeRole] = "lastOpenedTime";
    roles[TotalPagesRole] = "totalPages";
    roles[CurrentPageRole] = "currentPage";
    roles[CategoryEntriesModelRole] = "categoryEntriesModel";
    roles[CategoryEntriesCountRole] = "categoryEntriesCount";
    roles[ThumbnailRole] = "thumbnail";
    roles[DescriptionRole] = "description";
    roles[CommentRole] = "comment";
    roles[TagsRole] = "tags";
    roles[RatingRole] = "rating";
    roles[TypeRole] = "type";
    return roles;
}

QVariant CategoryEntriesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() <= -1) {
        return {};
    }

    if (index.row() < d->categoryModels.count()) {
        CategoryEntriesModel* model = d->categoryModels[index.row()];
        switch(role) {
        case Qt::DisplayRole:
        case TitleRole:
            return model->name();
        case TypeRole:
            return "category";
        case CategoryEntriesCountRole:
            return model->bookCount();
        case CategoryEntriesModelRole:
            return QVariant::fromValue(model);
        default:
            return "Unknown role category";
        }
    }

    const BookEntry &entry = d->entries[index.row() - d->categoryModels.count()];
    switch(role) {
    case Qt::DisplayRole:
    case TypeRole:
        return "book";
    case FilenameRole:
        return entry.filename;
    case FiletitleRole:
        return entry.filetitle;
    case TitleRole:
        return entry.title;
    case GenreRole:
        return entry.genres;
    case KeywordRole:
        return entry.keywords;
    case CharacterRole:
        return entry.characters;
    case SeriesRole:
        return entry.series;
    case SeriesNumbersRole:
        return entry.seriesNumbers;
    case SeriesVolumesRole:
        return entry.seriesVolumes;
    case AuthorRole:
        return entry.author;
    case PublisherRole:
        return entry.publisher;
    case CreatedRole:
        return entry.created;
    case LastOpenedTimeRole:
        return entry.lastOpenedTime;
    case TotalPagesRole:
        return entry.totalPages;
    case CurrentPageRole:
        return entry.currentPage;
    case CategoryEntriesModelRole:
        // Nothing, if we're not equipped with one such...
        return {};
    case CategoryEntriesCountRole:
        return 0;
    case ThumbnailRole:
        return entry.thumbnail;
    case DescriptionRole:
        return entry.description;
    case CommentRole:
        return entry.comment;
    case TagsRole:
        return entry.tags;
    case RatingRole:
        return entry.rating;
    default:
        return QString("Unknown role");
    }
}

int CategoryEntriesModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->categoryModels.count() + d->entries.count();
}

int CategoryEntriesModel::count() const
{
    return rowCount();
}

void CategoryEntriesModel::append(const BookEntry &entry, Roles compareRole)
{
    int insertionIndex = 0;
    if(compareRole == UnknownRole) {
        // If we don't know what order to sort by, literally just append the entry
        insertionIndex = d->entries.count();
    }
    else {
        int seriesOne = -1; int seriesTwo = -1;
        if(compareRole == SeriesRole) {
            seriesOne = entry.series.indexOf(name());
            if (entry.series.contains(name(), Qt::CaseInsensitive) && seriesOne == -1){
                for (int s=0; s<entry.series.size();s++) {
                    if (QString::compare(name(), entry.series.at(s), Qt::CaseInsensitive)) {
                        seriesOne = s;
                    }
                }
            }
        }
        for(; insertionIndex < d->entries.count(); ++insertionIndex)
        {
            if(compareRole == SeriesRole) {
                seriesTwo = d->entries.at(insertionIndex).series.indexOf(name());
                if ( d->entries.at(insertionIndex).series.contains(name(), Qt::CaseInsensitive) && seriesTwo == -1){
                    for (int s=0; s< d->entries.at(insertionIndex).series.size();s++) {
                        if (QString::compare(name(), d->entries.at(insertionIndex).series.at(s), Qt::CaseInsensitive)) {
                            seriesTwo = s;
                        }
                    }
                }
            }
            if(compareRole == CreatedRole)
            {
                if(entry.created <= d->entries.at(insertionIndex).created) {
                    continue;
                }
                break;
            }
            else if((seriesOne>-1 && seriesTwo>-1)
                    && entry.seriesNumbers.count() > -1 && entry.seriesNumbers.count() > seriesOne
                    && d->entries.at(insertionIndex).seriesNumbers.count() > -1 && d->entries.at(insertionIndex).seriesNumbers.count() > seriesTwo
                    && entry.seriesNumbers.at(seriesOne).toInt() > 0
                    && d->entries.at(insertionIndex).seriesNumbers.at(seriesTwo).toInt() > 0)
            {
                if (entry.seriesVolumes.count() > -1 && entry.seriesVolumes.count() > seriesOne
                        && d->entries.at(insertionIndex).seriesVolumes.count() > -1 && d->entries.at(insertionIndex).seriesVolumes.count() > seriesTwo
                        && entry.seriesVolumes.at(seriesOne).toInt() >= d->entries.at(insertionIndex).seriesVolumes.at(seriesTwo).toInt()
                        && entry.seriesNumbers.at(seriesOne).toInt() > d->entries.at(insertionIndex).seriesNumbers.at(seriesTwo).toInt())
                {continue;}
                break;
            }
            else
            {
                if(QString::localeAwareCompare(d->entries.at(insertionIndex).title, entry.title) > 0) {
                    break;
                }
            }
        }
    }
    beginInsertRows(QModelIndex(), insertionIndex, insertionIndex);
    d->entries.insert(insertionIndex, entry);
    Q_EMIT countChanged();
    endInsertRows();
}

void CategoryEntriesModel::appendFakeBook(const BookEntry &book, CategoryEntriesModel::Roles compareRole )
{
    append(book, compareRole);
}

void CategoryEntriesModel::clear()
{
    beginResetModel();
    d->entries.clear();
    endResetModel();
}

const QString &CategoryEntriesModel::name() const
{
    return d->name;
}

void CategoryEntriesModel::setName(const QString& newName)
{
    d->name = newName;
}

CategoryEntriesModel *CategoryEntriesModel::leafModelForEntry(const BookEntry &entry)
{
    CategoryEntriesModel* model(nullptr);
    if(d->categoryModels.count() == 0) {
        if(d->entries.contains(entry)) {
            model = this;
        }
    } else {
        for(CategoryEntriesModel* testModel : std::as_const(d->categoryModels)) {
            model = testModel->leafModelForEntry(entry);
            if(model) {
                break;
            }
        }
    }
    return model;
}

void CategoryEntriesModel::addCategoryEntry(const QString& categoryName, const BookEntry &entry, Roles compareRole)
{
    if(categoryName.length() > 0)
    {
        static const QString splitString{"/"};
        int splitPos = categoryName.indexOf(splitString);
        QString desiredCategory{categoryName};
        if(splitPos > -1) {
            desiredCategory = categoryName.left(splitPos);
        }
        CategoryEntriesModel* categoryModel = nullptr;
        for(CategoryEntriesModel *existingModel : std::as_const(d->categoryModels))
        {
            if(QString::compare(existingModel->name(), desiredCategory, Qt::CaseInsensitive) == 0)
            {
                categoryModel = existingModel;
                break;
            }
        }
        if(!categoryModel)
        {
            categoryModel = new CategoryEntriesModel(this);
            connect(this, &CategoryEntriesModel::entryDataUpdated, categoryModel, &CategoryEntriesModel::entryDataUpdated);
            connect(this, &CategoryEntriesModel::entryRemoved, categoryModel, &CategoryEntriesModel::entryRemoved);
            categoryModel->setName(desiredCategory);

            int insertionIndex = 0;
            for(; insertionIndex < d->categoryModels.count(); ++insertionIndex)
            {
                if(QString::localeAwareCompare(d->categoryModels.at(insertionIndex)->name(), categoryModel->name()) > 0)
                {
                    break;
                }
            }
            beginInsertRows(QModelIndex(), insertionIndex, insertionIndex);
            d->categoryModels.insert(insertionIndex, categoryModel);
            endInsertRows();
        }
        if (categoryModel->indexOfFile(entry.filename) == -1) {
            categoryModel->append(entry, compareRole);
        }
        if(splitPos > -1)
            categoryModel->addCategoryEntry(categoryName.mid(splitPos + 1), entry);
    }
}

BookEntry CategoryEntriesModel::getBookEntry(int index) const
{
    if (index > -1 && index < d->entries.count()) {
        return d->entries.at(index);
    }
    return BookEntry{};
}

int CategoryEntriesModel::indexOfFile(const QString& filename) const
{
    int index = -1, i = 0;
    if(QFile::exists(filename))
    {
        for(const BookEntry &entry : d->entries) {
            if(entry.filename == filename)
            {
                index = i;
                break;
            }
            ++i;
        }
    }
    return index;
}

bool CategoryEntriesModel::indexIsBook(int index) const
{
    if(index < d->categoryModels.count() || index >= rowCount()) {
        return false;
    }
    return true;
}

int CategoryEntriesModel::bookCount() const
{
    return d->entries.count();
}

CategoryEntriesModel *CategoryEntriesModel::getCategoryEntry(int index) const
{
    if(index > d->categoryModels.count() - 1 && index < rowCount()) {
        return nullptr;
    }
    if (index >= 0 && index < d->categoryModels.count()) {
        return d->categoryModels.at(index);
    }

    return nullptr;
}

BookEntry CategoryEntriesModel::bookFromFile(const QString& filename) const
{
    auto obj = getBookEntry(indexOfFile(filename));
    if(obj.filename.isEmpty()) {
        if(QFileInfo::exists(filename)) {
            qWarning() << "bookFromFile" << filename;
            QFileInfo info(filename);
            obj.title = info.completeBaseName();
            obj.created = info.birthTime();

            KFileMetaData::UserMetaData data(filename);
            if (data.hasAttribute("peruse.currentPage")) {
                obj.currentPage = data.attribute("peruse.currentPage").toInt();
            }
            if (data.hasAttribute("peruse.totalPages")) {
                obj.totalPages = data.attribute("peruse.totalPages").toInt();
            }
            obj.rating = data.rating();
            if (!data.tags().isEmpty()) {
                obj.tags = data.tags();
            }
            if (!data.userComment().isEmpty()) {
                obj.comment = data.userComment();
            }
            obj.filename = filename;

            QString thumbnail;
            if(filename.toLower().endsWith("cbr") || filename.toLower().endsWith("cbz")) {
                thumbnail = QString("image://comiccover/").append(filename);
            }
#ifdef USE_PERUSE_PDFTHUMBNAILER
            else if(filename.toLower().endsWith("pdf")) {
                thumbnail = QString("image://pdfcover/").append(filename);
            }
#endif
            else {
                thumbnail = QString("image://preview/").append(filename);
            }
            obj.thumbnail = thumbnail;
        }
    }
    return obj;
}

void CategoryEntriesModel::entryDataChanged(const BookEntry &entry)
{
    int entryIndex = d->entries.indexOf(entry) + d->categoryModels.count();
    QModelIndex changed = index(entryIndex);
    dataChanged(changed, changed);
}

void CategoryEntriesModel::entryRemove(const BookEntry &entry)
{
    int listIndex = d->entries.indexOf(entry);
    if(listIndex > -1) {
        int entryIndex = listIndex + d->categoryModels.count();
        beginRemoveRows(QModelIndex(), entryIndex, entryIndex);
        d->entries.removeAll(entry);
        endRemoveRows();
    }
}
