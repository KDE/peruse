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

#include "PreviewImageProvider.h"

#include <kiconloader.h>
#include <kio/previewjob.h>

#include <QCoreApplication>
#include <QDir>
#include <QTimer>
#include <QIcon>
#include <QMimeDatabase>
#include <QMutex>
#include <QThreadPool>
#include <QDebug>

class PreviewImageProvider::Private
{
public:
    Private() {};
};

PreviewImageProvider::PreviewImageProvider()
    : QQuickAsyncImageProvider()
    , d(new Private)
{
    qRegisterMetaType<KFileItem>("KFileItem");
}

PreviewImageProvider::~PreviewImageProvider()
{
    delete d;
}

class PreviewResponse : public QQuickImageResponse
{
    public:
        PreviewResponse(const QString &id, const QSize &requestedSize)
        {
            m_runnable = new PreviewRunnable(id, requestedSize);
            m_runnable->setAutoDelete(false);
            connect(m_runnable, &PreviewRunnable::done, this, &PreviewResponse::handleDone, Qt::QueuedConnection);
            connect(this, &QQuickImageResponse::finished, m_runnable, &QObject::deleteLater,  Qt::QueuedConnection);
            QThreadPool::globalInstance()->start(m_runnable);
        }

        void handleDone(QImage image) {
            m_image = image;
            Q_EMIT finished();
        }

        QQuickTextureFactory *textureFactory() const override
        {
            return QQuickTextureFactory::textureFactoryForImage(m_image);
        }

        void cancel() override
        {
            m_runnable->abort();
        }

        PreviewRunnable* m_runnable{nullptr};
        QImage m_image;
};

QQuickImageResponse * PreviewImageProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
    // We sometimes get malformed IDs (that is, extra slashes at the start), so fix those up
    QString adjustedId{id};
    while (adjustedId.startsWith("//")) {
        adjustedId = adjustedId.mid(1);
    }
    PreviewResponse* response = new PreviewResponse(adjustedId, requestedSize);
    return response;
}

class PreviewRunnable::Private {
public:
    Private() {}
    QString id;
    QSize requestedSize;

    bool abort{false};
    QMutex abortMutex;
    bool isAborted() {
        QMutexLocker locker(&abortMutex);
        return abort;
    }

    QImage preview;
    QPointer<KIO::PreviewJob> job{nullptr};
    QString mimetype;
};

PreviewRunnable::PreviewRunnable(const QString& id, const QSize& requestedSize)
    : d(new Private)
{
    d->id = id;
    d->requestedSize = requestedSize;
}

PreviewRunnable::~PreviewRunnable()
{
    abort();
    delete d;
}

void PreviewRunnable::run()
{

    QSize ourSize(KIconLoader::SizeEnormous, KIconLoader::SizeEnormous);
    if(d->requestedSize.width() > 0 && d->requestedSize.height() > 0)
    {
        ourSize = d->requestedSize;
    }

    if(QFile(d->id).exists())
    {
        QMimeDatabase db;
        QList<QMimeType> mimetypes = db.mimeTypesForFileName(d->id);
        if(mimetypes.count() > 0)
        {
            d->mimetype = mimetypes.first().name();
        }

        if(!d->isAborted()) {
            static QStringList allPlugins{KIO::PreviewJob::availablePlugins()};
            d->job = new KIO::PreviewJob(KFileItemList() << KFileItem(QUrl::fromLocalFile(d->id), d->mimetype, 0), ourSize, &allPlugins);
            d->job->setIgnoreMaximumSize(true);
            d->job->setScaleType(KIO::PreviewJob::ScaledAndCached);
            connect(d->job, &KIO::PreviewJob::gotPreview, this, &PreviewRunnable::updatePreview);
            connect(d->job, &KIO::PreviewJob::failed, this, &PreviewRunnable::fallbackPreview);
            connect(d->job, &KIO::PreviewJob::finished, this, &PreviewRunnable::finishedPreview);
            d->job->start();

            QTimer* breaker = new QTimer();
            breaker->moveToThread(thread());
            breaker->setParent(this);
            breaker->setSingleShot(true);
            breaker->setInterval(3000);
            connect(breaker, &QTimer::timeout, this, [this](){
                if (!d->isAborted()) {
                    abort();
                }
            });
            QTimer::singleShot(0, breaker, [breaker](){ breaker->start(); });
        } else {
            finishedPreview(nullptr);
        }
    }
    else
    {
        finishedPreview(nullptr);
    }
}

void PreviewRunnable::abort()
{
    if (d->job) {
        QMutexLocker locker(&d->abortMutex);
        d->abort = true;
        locker.unlock();
        d->job->kill();
    }
}

void PreviewRunnable::fallbackPreview(const KFileItem& item)
{
    KIO::PreviewJob* previewJob = qobject_cast<KIO::PreviewJob*>(sender());
    if(previewJob)
    {
        QMimeDatabase db;
        QIcon mimeIcon = QIcon::fromTheme(db.mimeTypeForName(item.mimetype()).iconName());
        QSize actualSize = mimeIcon.actualSize(d->requestedSize);
        d->preview = mimeIcon.pixmap(actualSize).toImage();
    }
}

void PreviewRunnable::updatePreview(const KFileItem&, const QPixmap& p)
{
    KIO::PreviewJob* previewJob = qobject_cast<KIO::PreviewJob*>(sender());
    if(previewJob)
    {
        d->preview = p.toImage();
    }
}

void PreviewRunnable::finishedPreview(KJob* /*job*/)
{
    if(d->isAborted()) {
        if (d->preview.isNull()) {
            QMimeDatabase db;
            QIcon mimeIcon = QIcon::fromTheme(db.mimeTypeForName(d->mimetype).iconName());
            QSize actualSize = mimeIcon.actualSize(d->requestedSize);
            d->preview = mimeIcon.pixmap(actualSize).toImage();
        }
    } else {
        if(d->requestedSize.width() > 0 && d->requestedSize.height() > 0) {
            d->preview = d->preview.scaled(d->requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
    Q_EMIT done(d->preview);
}
