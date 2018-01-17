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

#include "BalooContentLister.h"

#include <Baloo/IndexerConfig>
#include <Baloo/File>
#include <KFileMetaData/PropertyInfo>
#include <KFileMetaData/UserMetaData>

#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QFileInfo>
#include <QProcess>
#include <QThreadPool>
#include <QMimeDatabase>

#include "ContentQuery.h"

class BalooContentLister::Private
{
public:
    Private(BalooContentLister* qq) : q(qq) {}

    BalooContentLister* q = nullptr;

    Baloo::QueryRunnable* createQuery(ContentQuery* contentQuery, const QString& location = QString{});

    QStringList knownFiles;
    QStringList locations;
    QString searchString;
    QList<Baloo::QueryRunnable*> queries;
    QList<QString> queryLocations;

    QMimeDatabase mimeDatabase;
};

BalooContentLister::BalooContentLister(QObject* parent)
    : ContentListerBase(parent)
    , d(new Private(this))
{
}

BalooContentLister::~BalooContentLister()
{
    QThreadPool::globalInstance()->waitForDone();
    delete d;
}

bool BalooContentLister::balooEnabled() const
{
    Baloo::IndexerConfig config;
    bool result = config.fileIndexingEnabled();

    if(result)
    {
        // It would be terribly nice with a bit of baloo engine exporting, so
        // we can ask the database about whether or not it is accessible...
        // But, this is a catch-all check anyway, so we get a complete "everything's broken"
        // result if anything is broken... guess it will do :)
        QProcess statuscheck;
        statuscheck.start("balooctl", QStringList() << "status");
        statuscheck.waitForFinished();
        QString output = statuscheck.readAll();
        if(statuscheck.exitStatus() == QProcess::CrashExit || statuscheck.exitCode() != 0)
        {
            result = false;
        }
    }

    return result;
}

void BalooContentLister::startSearch(const QList<ContentQuery*>& queries)
{
    for(const auto& query : queries)
    {
        for(const auto& location : query->locations())
        {
            d->queries.append(d->createQuery(query, location));
        }

        if(query->locations().isEmpty())
            d->queries.append(d->createQuery(query));
    }

    if(!d->queries.empty())
    {
        QThreadPool::globalInstance()->start(d->queries.first());
    }
}

void BalooContentLister::queryCompleted(Baloo::QueryRunnable* query)
{
    d->queries.removeAll(query);
    if(d->queries.empty())
    {
        emit searchCompleted();
    }
    else
    {
        QThreadPool::globalInstance()->start(d->queries.first());
    }
}

void BalooContentLister::queryResult(const ContentQuery* query, const QString& location, const QString& file)
{
    if(d->knownFiles.contains(file)) {
        return;
    }

    // wow, this isn't nice... why is baloo not limiting searches like it's supposed to?
    if(!file.startsWith(location)) {
        return;
    }

    // Like the one above, this is also not nice: apparently Baloo can return results to
    // files that no longer exist on the file system. So we have to check manually whether
    // the results provided are actually sensible results...
    if(!QFile::exists(file)) {
        return;
    }

    // It would be nice if Baloo could do mime type filtering on its own...
    auto mimeType = d->mimeDatabase.mimeTypeForFile(file).name();
    if(!query->mimeTypes().isEmpty() && !query->mimeTypes().contains(mimeType))
        return;

    auto metadata = metaDataForFile(file);

    Baloo::File balooFile(file);
    balooFile.load();
    KFileMetaData::PropertyMap properties = balooFile.properties();
    KFileMetaData::PropertyMap::const_iterator it = properties.constBegin();
    for (; it != properties.constEnd(); it++)
    {
        KFileMetaData::PropertyInfo propInfo(it.key());
        metadata[propInfo.name()] = it.value();
    }

    emit fileFound(file, metadata);
}

Baloo::QueryRunnable* BalooContentLister::Private::createQuery(ContentQuery* contentQuery, const QString& location)
{
    auto balooQuery = Baloo::Query{};
    if(!location.isEmpty())
        balooQuery.setIncludeFolder(location);

    switch(contentQuery->type())
    {
        case ContentQuery::Audio:
            balooQuery.setType("Audio");
            break;
        case ContentQuery::Documents:
            balooQuery.setType("Document");
            break;
        case ContentQuery::Images:
            balooQuery.setType("Image");
            break;
        case ContentQuery::Video:
            balooQuery.setType("Video");
            break;
        default:
            break;
    }

    if(!contentQuery->searchString().isEmpty())
        balooQuery.setSearchString(contentQuery->searchString());

    auto runnable = new Baloo::QueryRunnable{balooQuery};
    connect(runnable, &Baloo::QueryRunnable::queryResult, [this, contentQuery, location](QRunnable*, const QString& file) {
        q->queryResult(contentQuery, location, file);
    });
    connect(runnable, &Baloo::QueryRunnable::finished, q, &BalooContentLister::queryCompleted);

    return runnable;
}
