// SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "FilesystemContentLister.h"

#include <KFileMetaData/UserMetaData>

#include <QCoreApplication>
#include <QDateTime>
#include <QDirIterator>
#include <QMimeDatabase>
#include <QRunnable>
#include <QThreadPool>
#include <QTimer>
#include <QVariantHash>

#include "ContentQuery.h"

class FileSystemSearcher : public QObject, public QRunnable
{
    Q_OBJECT
public:
    FileSystemSearcher(ContentQuery *query)
        : QObject()
    {
        m_query = query;
    }

    void run() override
    {
        QMimeDatabase mimeDb;

        auto locations = m_query->locations();
        if (locations.isEmpty())
            locations.append(QDir::homePath());

        for (const auto &location : locations) {
            QDirIterator it(location, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                auto filePath = it.next();

                if (it.fileInfo().isDir())
                    continue;

                if (!m_query->mimeTypes().isEmpty()) {
                    QString mimeType = mimeDb.mimeTypeForFile(filePath).name();
                    if (!m_query->mimeTypes().contains(mimeType))
                        continue;
                }

                auto metadata = ContentListerBase::metaDataForFile(filePath);

                emit fileFound(filePath, metadata);
            }
        }

        emit finished(this);
    }

Q_SIGNALS:
    void fileFound(const QString &path, const QVariantMap &metaData);
    void finished(FileSystemSearcher *searcher);

private:
    ContentQuery *m_query;
};

class FilesystemContentLister::Private
{
public:
    Private()
    {
    }

    QList<FileSystemSearcher *> runnables;
};

FilesystemContentLister::FilesystemContentLister(QObject *parent)
    : ContentListerBase(parent)
    , d(new Private)
{
}

FilesystemContentLister::~FilesystemContentLister()
{
    QThreadPool::globalInstance()->waitForDone();
    delete d;
}

void FilesystemContentLister::startSearch(const QList<ContentQuery *> &queries)
{
    for (const auto &query : queries) {
        auto runnable = new FileSystemSearcher{query};
        connect(runnable, &FileSystemSearcher::fileFound, this, &FilesystemContentLister::fileFound);
        connect(runnable, &FileSystemSearcher::finished, this, &FilesystemContentLister::queryFinished);

        d->runnables.append(runnable);
    }

    if (!d->runnables.isEmpty())
        QThreadPool::globalInstance()->start(d->runnables.first());
}

void FilesystemContentLister::queryFinished(QRunnable *runnable)
{
    d->runnables.removeAll(static_cast<FileSystemSearcher *>(runnable));

    if (!d->runnables.isEmpty()) {
        QThreadPool::globalInstance()->start(d->runnables.first());
    } else {
        emit searchCompleted();
    }
}

// This needs to be included since we define a QObject subclass here in the C++ file.
#include "FilesystemContentLister.moc"
