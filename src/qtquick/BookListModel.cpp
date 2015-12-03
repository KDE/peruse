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

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QDebug>
#include <QMimeDatabase>

#include "kfilemetadata/extractor.h"
#include "kfilemetadata/extractorcollection.h"
#include "kfilemetadata/propertyinfo.h"
#include "kfilemetadata/simpleextractionresult.h"

struct BookEntry {
    BookEntry()
        : totalPages(0)
        , currentPage(0)
    {}
    QString filename;
    QString filetitle;
    QString title;
    QString author;
    QString publisher;
    QDateTime lastOpenedTime;
    int totalPages;
    int currentPage;
};

class BookListModel::Private {
public:
    Private()
        : contentModel(0)
    {};
    ~Private()
    {
        qDeleteAll(entries);
    }
    QList<BookEntry*> entries;

    QAbstractListModel* contentModel;
};

BookListModel::BookListModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

BookListModel::~BookListModel()
{
    delete d;
}

QHash<int, QByteArray> BookListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[FilenameRole] = "filename";
    roles[FiletitleRole] = "filetitle";
    roles[TitleRole] = "title";
    roles[AuthorRole] = "author";
    roles[PublisherRole] = "publisher";
    roles[LastOpenedTimeRole] = "lastOpenedTime";
    roles[TotalPagesRole] = "totalPages";
    roles[CurrentPageRole] = "currentPage";
    return roles;
}

QVariant BookListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        const BookEntry* entry = d->entries[index.row()];
        switch(role)
        {
            case FilenameRole:
                result.setValue(entry->filename);
                break;
            case FiletitleRole:
                result.setValue(entry->filetitle);
                break;
            case TitleRole:
                result.setValue(entry->title);
                break;
            case AuthorRole:
                result.setValue(entry->author);
                break;
            case PublisherRole:
                result.setValue(entry->publisher);
                break;
            case LastOpenedTimeRole:
                result.setValue(entry->lastOpenedTime);
                break;
            case TotalPagesRole:
                result.setValue(entry->totalPages);
                break;
            case CurrentPageRole:
                result.setValue(entry->currentPage);
                break;
            default:
                result.setValue(QString("Unknown role"));
                break;
        }
    }
    return result;
}

int BookListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->entries.count();
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
        connect(d->contentModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(contentModelItemsInserted(QModelIndex,int, int)));
    }
    emit contentModelChanged();
}

QObject * BookListModel::contentModel() const
{
    return d->contentModel;
}

void BookListModel::contentModelItemsInserted(QModelIndex index, int first, int last)
{
    int newRow = d->entries.count();
    beginInsertRows(QModelIndex(), newRow, newRow + (last - first));
    for(int i = first; i < last + 1; ++i)
    {
        QVariant filename = d->contentModel->data(d->contentModel->index(first, 0, index), Qt::UserRole + 1);
        BookEntry* entry = new BookEntry();
        entry->filename = filename.toString();
        entry->filetitle = entry->filename.split(QDir::separator()).last();

        QMimeDatabase mimeDb;
        QString mimetype = mimeDb.mimeTypeForFile(entry->filename).name();

        KFileMetaData::ExtractorCollection extractors;
        QList<KFileMetaData::Extractor*> exList = extractors.fetchExtractors(mimetype);

        Q_FOREACH (KFileMetaData::Extractor* ex, exList)
        {
            KFileMetaData::SimpleExtractionResult result(entry->filename, mimetype, KFileMetaData::ExtractionResult::ExtractMetaData);
            ex->extract(&result);
            KFileMetaData::PropertyMap properties = result.properties();
            KFileMetaData::PropertyMap::const_iterator it = properties.constBegin();
            for (; it != properties.constEnd(); it++) {
                KFileMetaData::PropertyInfo propInfo(it.key());
                QString propName = propInfo.name();
                if(propName == QLatin1String("author"))
                {
                    entry->author = it.value().toString();
                }
                else if(propName == QLatin1String("title"))
                {
                    entry->title = it.value().toString();
                }
//                 qDebug() << KFileMetaData::PropertyInfo(it.key()).name() << " --> "
//                     << it.value().toString() << " (" << it.value().typeName() << ")\n";
            }
        }
        d->entries.append(entry);
    }
    endInsertRows();
}

