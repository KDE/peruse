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

#include "ContentList.h"
#include "FilesystemContentLister.h"

#ifdef BALOO_FOUND
    #include "BalooContentLister.h"
#endif

#include <QMimeDatabase>
#include <QSet>
#include <QTimer>
#include <QUrl>

struct ContentEntry {
    QString filename;
    QUrl filePath;
    QVariantMap metadata;
};

class ContentList::Private {
public:
    typedef QQmlListProperty<ContentQuery> QueryListProperty;

    Private()
        : actualContentList(nullptr)
    {}
    QList<ContentEntry*> entries;
    ContentListerBase* actualContentList;

    QList<ContentQuery*> queries;
    QueryListProperty listProperty;

    QSet<QString> knownFiles;

    bool autoSearch = false;
    bool cacheResults = false;
    bool completed = false;

    static void appendToList(QueryListProperty* property, ContentQuery* value);
    static ContentQuery* listValueAt(QueryListProperty* property, int index);
    static void clearList(QueryListProperty* property);
    static int countList(QueryListProperty* property);

    static QStringList cachedFiles;
};

QStringList ContentList::Private::cachedFiles;

ContentList::ContentList(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
#ifdef BALOO_FOUND
    BalooContentLister* baloo = new BalooContentLister(this);
    if(baloo->balooEnabled())
    {
        d->actualContentList = baloo;
    }
    else
    {
        baloo->deleteLater();
        d->actualContentList = new FilesystemContentLister(this);
    }
#else
    d->actualContentList = new FilesystemContentLister(this);
#endif
    connect(d->actualContentList, &ContentListerBase::fileFound, this, &ContentList::fileFound);
    connect(d->actualContentList, &ContentListerBase::searchCompleted, this, &ContentList::searchCompleted);

    d->listProperty = QQmlListProperty<ContentQuery>{this, &d->queries,
        &ContentList::Private::appendToList,
        &ContentList::Private::countList,
        &ContentList::Private::listValueAt,
        &ContentList::Private::clearList
    };
}

ContentList::~ContentList()
{
    delete d;
}

QQmlListProperty<ContentQuery> ContentList::queries()
{
    return d->listProperty;
}

bool ContentList::autoSearch() const
{
    return d->autoSearch;
}

bool ContentList::cacheResults() const
{
    return d->cacheResults;
}

QString ContentList::getMimetype(QString filePath)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(filePath);
    return mime.name();
}

void ContentList::startSearch()
{
    QTimer::singleShot(1, [this]() {
        d->actualContentList->startSearch(d->queries);
    });
}

void ContentList::fileFound(const QString& filePath, const QVariantMap& metaData)
{
    if(d->knownFiles.contains(filePath))
        return;

    auto fileUrl = QUrl::fromLocalFile(filePath);

    ContentEntry* entry = new ContentEntry();
    entry->filename = fileUrl.fileName();
    entry->filePath = fileUrl;
    entry->metadata = metaData;

    int newRow = d->entries.count();
    beginInsertRows(QModelIndex(), newRow, newRow);
    d->entries.append(entry);
    endInsertRows();

    if(d->cacheResults)
        Private::cachedFiles.append(filePath);
}

void ContentList::setAutoSearch(bool autoSearch)
{
    if(autoSearch == d->autoSearch)
        return;

    d->autoSearch = autoSearch;
    emit autoSearchChanged();
}

void ContentList::setCacheResults(bool cacheResults)
{
    if(cacheResults == d->cacheResults)
        return;

    d->cacheResults = cacheResults;

    if(d->cacheResults && d->completed && !Private::cachedFiles.isEmpty())
    {
        setKnownFiles(Private::cachedFiles);
    }

    emit cacheResultsChanged();
}

void ContentList::setKnownFiles(const QStringList& results)
{
    beginResetModel();
    d->entries.clear();
    d->knownFiles.clear();
    for(const auto& result : results)
    {
        auto entry = new ContentEntry{};
        auto url = QUrl::fromLocalFile(result);

        entry->filename = url.fileName();
        entry->filePath = url;
        entry->metadata = ContentListerBase::metaDataForFile(result);

        d->entries.append(entry);
        d->knownFiles.insert(result);
    }
    endResetModel();
}

QHash<int, QByteArray> ContentList::roleNames() const
{
    return {
        { FilenameRole, "filename"},
        { FilePathRole, "filePath"},
        { MetadataRole, "metadata"}
    };
}

QVariant ContentList::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        const ContentEntry* entry = d->entries[index.row()];
        switch(role)
        {
            case FilenameRole:
                result.setValue(entry->filename);
                break;
            case FilePathRole:
                result.setValue(entry->filePath);
                break;
            case MetadataRole:
                result.setValue(entry->metadata);
                break;
            default:
                break;
        }
    }
    return result;
}

int ContentList::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
}

void ContentList::classBegin()
{
}

void ContentList::componentComplete()
{
    d->completed = true;

    if(d->cacheResults && !Private::cachedFiles.isEmpty())
        setKnownFiles(Private::cachedFiles);

    if(d->autoSearch)
        d->actualContentList->startSearch(d->queries);
}

bool ContentList::isComplete() const
{
    return d->completed;
}

void ContentList::Private::appendToList(Private::QueryListProperty* property, ContentQuery* value)
{
    auto list = static_cast<QList<ContentQuery*>*>(property->data);
    auto model = static_cast<ContentList*>(property->object);
    list->append(value);
    if(model->autoSearch() && model->isComplete())
        model->startSearch();
}

ContentQuery* ContentList::Private::listValueAt(Private::QueryListProperty* property, int index)
{
    return static_cast<QList<ContentQuery*>*>(property->data)->at(index);
}

int ContentList::Private::countList(Private::QueryListProperty* property)
{
    return static_cast<QList<ContentQuery*>*>(property->data)->size();
}

void ContentList::Private::clearList(Private::QueryListProperty* property)
{
    auto list = static_cast<QList<ContentQuery*>*>(property->data);
    auto model = static_cast<ContentList*>(property->object);
    model->beginResetModel();
    list->clear();
    model->endResetModel();
}
