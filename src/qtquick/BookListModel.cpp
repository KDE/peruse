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

#include "BookListModel.h"

#include "BookDatabase.h"
#include "CategoryEntriesModel.h"
#include "ArchiveBookModel.h"

#include "AcbfAuthor.h"
#include "AcbfSequence.h"
#include "AcbfBookinfo.h"

#include <kio/deletejob.h>
#include <KFileMetaData/UserMetaData>

#include <QCoreApplication>
#include <QDir>
#include <QMimeDatabase>
#include <QTimer>
#include <QUrl>

#include <qtquick_debug.h>

class BookListModel::Private {
public:
    Private()
        : contentModel(nullptr)
        , titleCategoryModel(nullptr)
        , newlyAddedCategoryModel(nullptr)
        , authorCategoryModel(nullptr)
        , seriesCategoryModel(nullptr)
        , publisherCategoryModel(nullptr)
        , keywordCategoryModel(nullptr)
        , folderCategoryModel(nullptr)
        , cacheLoaded(false)
    {
        db = new BookDatabase();
    };
    ~Private()
    {
        qDeleteAll(entries);
        db->deleteLater();
    }
    QList<BookEntry*> entries;

    QAbstractListModel* contentModel;
    CategoryEntriesModel* titleCategoryModel;
    CategoryEntriesModel* newlyAddedCategoryModel;
    CategoryEntriesModel* authorCategoryModel;
    CategoryEntriesModel* seriesCategoryModel;
    CategoryEntriesModel* publisherCategoryModel;
    CategoryEntriesModel* keywordCategoryModel;
    CategoryEntriesModel* folderCategoryModel;

    BookDatabase* db;
    bool cacheLoaded;

    void initializeSubModels(BookListModel* q) {
        if(!titleCategoryModel)
        {
            titleCategoryModel = new CategoryEntriesModel(q);
            connect(q, &CategoryEntriesModel::entryDataUpdated, titleCategoryModel, &CategoryEntriesModel::entryDataUpdated);
            connect(q, &CategoryEntriesModel::entryRemoved, titleCategoryModel, &CategoryEntriesModel::entryRemoved);
            emit q->titleCategoryModelChanged();
        }
        if(!newlyAddedCategoryModel)
        {
            newlyAddedCategoryModel = new CategoryEntriesModel(q);
            connect(q, &CategoryEntriesModel::entryDataUpdated, newlyAddedCategoryModel, &CategoryEntriesModel::entryDataUpdated);
            connect(q, &CategoryEntriesModel::entryRemoved, newlyAddedCategoryModel, &CategoryEntriesModel::entryRemoved);
            emit q->newlyAddedCategoryModelChanged();
        }
        if(!authorCategoryModel)
        {
            authorCategoryModel = new CategoryEntriesModel(q);
            connect(q, &CategoryEntriesModel::entryDataUpdated, authorCategoryModel, &CategoryEntriesModel::entryDataUpdated);
            connect(q, &CategoryEntriesModel::entryRemoved, authorCategoryModel, &CategoryEntriesModel::entryRemoved);
            emit q->authorCategoryModelChanged();
        }
        if(!seriesCategoryModel)
        {
            seriesCategoryModel = new CategoryEntriesModel(q);
            connect(q, &CategoryEntriesModel::entryDataUpdated, seriesCategoryModel, &CategoryEntriesModel::entryDataUpdated);
            connect(q, &CategoryEntriesModel::entryRemoved, seriesCategoryModel, &CategoryEntriesModel::entryRemoved);
            emit q->seriesCategoryModelChanged();
        }
        if(!publisherCategoryModel)
        {
            publisherCategoryModel = new CategoryEntriesModel(q);
            connect(q, &CategoryEntriesModel::entryDataUpdated, publisherCategoryModel, &CategoryEntriesModel::entryDataUpdated);
            connect(q, &CategoryEntriesModel::entryRemoved, publisherCategoryModel, &CategoryEntriesModel::entryRemoved);
            emit q->publisherCategoryModelChanged();
        }
        if(!keywordCategoryModel)
        {
            keywordCategoryModel = new CategoryEntriesModel(q);
            connect(q, &CategoryEntriesModel::entryDataUpdated, keywordCategoryModel, &CategoryEntriesModel::entryDataUpdated);
            connect(q, &CategoryEntriesModel::entryRemoved, keywordCategoryModel, &CategoryEntriesModel::entryRemoved);
            emit q->keywordCategoryModelChanged();
        }
        if(!folderCategoryModel)
        {
            folderCategoryModel = new CategoryEntriesModel(q);
            connect(q, &CategoryEntriesModel::entryDataUpdated, folderCategoryModel, &CategoryEntriesModel::entryDataUpdated);
            connect(q, &CategoryEntriesModel::entryRemoved, folderCategoryModel, &CategoryEntriesModel::entryRemoved);
            emit q->folderCategoryModel();
        }
    }

    void addEntry(BookListModel* q, BookEntry* entry) {
        entries.append(entry);
        q->append(entry);
        titleCategoryModel->addCategoryEntry(entry->title.left(1).toUpper(), entry);
        for (int i=0; i<entry->author.size(); i++) {
            authorCategoryModel->addCategoryEntry(entry->author.at(i), entry);
        }
        for (int i=0; i<entry->series.size(); i++) {
            seriesCategoryModel->addCategoryEntry(entry->series.at(i), entry, SeriesRole);
        }
        if (newlyAddedCategoryModel->indexOfFile(entry->filename) == -1) {
            newlyAddedCategoryModel->append(entry, CreatedRole);
        }
        publisherCategoryModel->addCategoryEntry(entry->publisher, entry);
        QUrl url(entry->filename.left(entry->filename.lastIndexOf("/")));
        folderCategoryModel->addCategoryEntry(url.path().mid(1), entry);
        if (folderCategoryModel->indexOfFile(entry->filename) == -1) {
            folderCategoryModel->append(entry);
        }
        for (int i=0; i<entry->genres.size(); i++) {
            keywordCategoryModel->addCategoryEntry(QString("Genre/").append(entry->genres.at(i)), entry, GenreRole);
        }
        for (int i=0; i<entry->characters.size(); i++) {
            keywordCategoryModel->addCategoryEntry(QString("Characters/").append(entry->characters.at(i)), entry, GenreRole);
        }
        for (int i=0; i<entry->keywords.size(); i++) {
            keywordCategoryModel->addCategoryEntry(QString("Keywords/").append(entry->keywords.at(i)), entry, GenreRole);
        }

    }

    void loadCache(BookListModel* q) {
        QList<BookEntry*> entries = db->loadEntries();
        if(entries.count() > 0)
        {
            initializeSubModels(q);
        }
        int i = 0;
        for(BookEntry* entry : entries)
        {
            /*
             * This might turn out a little slow, but we should avoid having entries
             * that do not exist. If we end up with slowdown issues when loading the
             * cache this would be a good place to start investigating.
             */
            if (QFileInfo::exists(entry->filename)) {
                addEntry(q, entry);
                if(++i % 100 == 0) {
                    emit q->countChanged();
                    qApp->processEvents();
                }
            } else {
                db->removeEntry(entry);
            }
        }
        cacheLoaded = true;
        emit q->cacheLoadedChanged();
    }
};

BookListModel::BookListModel(QObject* parent)
    : CategoryEntriesModel(parent)
    , d(new Private)
{
}

BookListModel::~BookListModel()
{
    delete d;
}

void BookListModel::componentComplete()
{
    QTimer::singleShot(0, this, [this](){ d->loadCache(this); });
}

bool BookListModel::cacheLoaded() const
{
    return d->cacheLoaded;
}

void BookListModel::setContentModel(QObject* newModel)
{
    if(d->contentModel)
    {
        d->contentModel->disconnect(this);
    }
    d->contentModel = qobject_cast<QAbstractListModel*>(newModel);
    if(d->contentModel)
    {
        connect(d->contentModel, &QAbstractItemModel::rowsInserted, this, &BookListModel::contentModelItemsInserted);
    }
    emit contentModelChanged();
}

QObject * BookListModel::contentModel() const
{
    return d->contentModel;
}

void BookListModel::contentModelItemsInserted(QModelIndex index, int first, int last)
{
    d->initializeSubModels(this);
    int newRow = d->entries.count();
    beginInsertRows(QModelIndex(), newRow, newRow + (last - first));
    int role = d->contentModel->roleNames().key("filePath");
    for(int i = first; i < last + 1; ++i)
    {
        QVariant filePath = d->contentModel->data(d->contentModel->index(first, 0, index), role);
        BookEntry* entry = new BookEntry();
        entry->filename = filePath.toUrl().toLocalFile();
        QStringList splitName = entry->filename.split("/");
        if (!splitName.isEmpty())
            entry->filetitle = splitName.takeLast();
        if(!splitName.isEmpty()) {
            entry->series = QStringList(splitName.takeLast()); // hahahaheuristics (dumb assumptions about filesystems, go!)
            entry->seriesNumbers = QStringList("0");
            entry->seriesVolumes = QStringList("0");
        }
        // just in case we end up without a title... using complete basename here,
        // as we would rather have "book one. part two" and the odd "book one - part two.tar"
        QFileInfo fileinfo(entry->filename);
        entry->title = fileinfo.completeBaseName();

        if(entry->filename.toLower().endsWith("cbr") || entry->filename.toLower().endsWith("cbz")) {
            entry->thumbnail = QString("image://comiccover/").append(entry->filename);
        }
#ifdef USE_PERUSE_PDFTHUMBNAILER
        else if(entry->filename.toLower().endsWith("pdf")) {
            entry->thumbnail = QString("image://pdfcover/").append(entry->filename);
        }
#endif
        else {
            entry->thumbnail = QString("image://preview/").append(entry->filename);
        }

        KFileMetaData::UserMetaData data(entry->filename);
        entry->rating = data.rating();
        entry->comment = data.userComment();
        entry->tags = data.tags();

        QVariantHash metadata = d->contentModel->data(d->contentModel->index(first, 0, index), Qt::UserRole + 2).toHash();
        QVariantHash::const_iterator it = metadata.constBegin();
        for (; it != metadata.constEnd(); it++) {
            if(it.key() == QLatin1String("author"))
            { entry->author = it.value().toStringList(); }
            else if(it.key() == QLatin1String("title"))
            { entry->title = it.value().toString().trimmed(); }
            else if(it.key() == QLatin1String("publisher"))
            { entry->publisher = it.value().toString().trimmed(); }
            else if(it.key() == QLatin1String("created"))
            { entry->created = it.value().toDateTime(); }
            else if(it.key() == QLatin1String("currentPage"))
            { entry->currentPage = it.value().toInt(); }
            else if(it.key() == QLatin1String("totalPages"))
            { entry->totalPages = it.value().toInt(); }
            else if(it.key() == QLatin1String("comments"))
            { entry->comment = it.value().toString();}
            else if(it.key() == QLatin1Literal("tags"))
            { entry->tags = it.value().toStringList();}
            else if(it.key() == QLatin1String("rating"))
            { entry->rating = it.value().toInt();}
        }
        // ACBF information is always preferred for CBRs, so let's just use that if it's there
        QMimeDatabase db;
        QString mimetype = db.mimeTypeForFile(entry->filename).name();
        if(mimetype == "application/x-cbz" || mimetype == "application/x-cbr" || mimetype == "application/vnd.comicbook+zip" || mimetype == "application/vnd.comicbook+rar") {
            ArchiveBookModel* bookModel = new ArchiveBookModel(this);
            bookModel->setFilename(entry->filename);

            AdvancedComicBookFormat::Document* acbfDocument = qobject_cast<AdvancedComicBookFormat::Document*>(bookModel->acbfData());
            if(acbfDocument) {
                for(AdvancedComicBookFormat::Sequence* sequence : acbfDocument->metaData()->bookInfo()->sequence()) {
                    if (!entry->series.contains(sequence->title())) {
                        entry->series.append(sequence->title());
                        entry->seriesNumbers.append(QString::number(sequence->number()));
                        entry->seriesVolumes.append(QString::number(sequence->volume()));
                    } else {
                        int series = entry->series.indexOf(sequence->title());
                        entry->seriesNumbers.replace(series, QString::number(sequence->number()));
                        entry->seriesVolumes.replace(series, QString::number(sequence->volume()));
                    }

                }
                for(AdvancedComicBookFormat::Author* author : acbfDocument->metaData()->bookInfo()->author()) {
                    entry->author.append(author->displayName());
                }
                entry->description = acbfDocument->metaData()->bookInfo()->annotation("");
                entry->genres = acbfDocument->metaData()->bookInfo()->genres();
                entry->characters = acbfDocument->metaData()->bookInfo()->characters();
                entry->keywords = acbfDocument->metaData()->bookInfo()->keywords("");
            }

            if (entry->author.isEmpty()) {
                entry->author.append(bookModel->author());
            }
            entry->title = bookModel->title();
            entry->publisher = bookModel->publisher();
            entry->totalPages = bookModel->pageCount();
            bookModel->deleteLater();
        }

        d->addEntry(this, entry);
        d->db->addEntry(entry);
    }
    endInsertRows();
    emit countChanged();
    qApp->processEvents();
}

QObject * BookListModel::titleCategoryModel() const
{
    return d->titleCategoryModel;
}

QObject * BookListModel::newlyAddedCategoryModel() const
{
    return d->newlyAddedCategoryModel;
}

QObject * BookListModel::authorCategoryModel() const
{
    return d->authorCategoryModel;
}

QObject * BookListModel::seriesCategoryModel() const
{
    return d->seriesCategoryModel;
}

QObject * BookListModel::seriesModelForEntry(QString fileName)
{
    for(BookEntry* entry : d->entries)
    {
        if(entry->filename == fileName)
        {
            return d->seriesCategoryModel->leafModelForEntry(entry);
        }
    }
    return nullptr;
}

QObject *BookListModel::publisherCategoryModel() const
{
    return d->publisherCategoryModel;
}

QObject *BookListModel::keywordCategoryModel() const
{
    return d->keywordCategoryModel;
}

QObject * BookListModel::folderCategoryModel() const
{
    return d->folderCategoryModel;
}

int BookListModel::count() const
{
    return d->entries.count();
}

void BookListModel::setBookData(QString fileName, QString property, QString value)
{
    for(BookEntry* entry : d->entries)
    {
        if(entry->filename == fileName)
        {
            if(property == "totalPages")
            {
                entry->totalPages = value.toInt();
                d->db->updateEntry(entry->filename, property, QVariant(value.toInt()));
            }
            else if(property == "currentPage")
            {
                entry->currentPage = value.toInt();
                d->db->updateEntry(entry->filename, property, QVariant(value.toInt()));
            }
            else if(property == "rating")
            {
                entry->rating = value.toInt();
                d->db->updateEntry(entry->filename, property, QVariant(value.toInt()));
            }
            else if(property == "tags")
            {
                entry->tags = value.split(",");
                d->db->updateEntry(entry->filename, property, QVariant(value.split(",")));
            }
            else if(property == "comment") {
                entry->comment = value;
                d->db->updateEntry(entry->filename, property, QVariant(value));
            }
            emit entryDataUpdated(entry);
            break;
        }
    }
}

void BookListModel::removeBook(QString fileName, bool deleteFile)
{
    if(deleteFile) {
        KIO::DeleteJob* job = KIO::del(QUrl::fromLocalFile(fileName), KIO::HideProgressInfo);
        job->start();
    }

    for(BookEntry* entry : d->entries)
    {
        if(entry->filename == fileName)
        {
            emit entryRemoved(entry);
            d->db->removeEntry(entry);
            delete entry;
            break;
        }
    }
}

QStringList BookListModel::knownBookFiles() const
{
    QStringList files;
    for(BookEntry* entry : d->entries) {
        files.append(entry->filename);
    }
    return files;
}
