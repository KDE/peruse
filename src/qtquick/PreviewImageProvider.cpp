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
#include <QIcon>
#include <QMimeDatabase>
#include <QThreadPool>
#include <QDebug>

class PreviewImageProvider::Private
{
public:
    Private() {};
    QThreadPool pool;
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
        PreviewResponse(const QString &id, const QSize &requestedSize, QThreadPool *pool)
        {
            m_runnable = new PreviewRunnable(id, requestedSize);
            connect(m_runnable, &PreviewRunnable::done, this, &PreviewResponse::handleDone);
            pool->start(m_runnable);
        }

        void handleDone(QImage image) {
            m_image = image;
            emit finished();
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
    PreviewResponse* response = new PreviewResponse(id, requestedSize, &d->pool);
    return response;
}

class PreviewRunnable::Private {
public:
    Private() {}
    QString id;
    QSize requestedSize;

    bool abort{false};

    QImage preview;
    bool jobCompletion{false};
    KIO::PreviewJob* job{nullptr};
};

PreviewRunnable::PreviewRunnable(const QString& id, const QSize& requestedSize)
    : d(new Private)
{
    d->id = id;
    d->requestedSize = requestedSize;
}

void PreviewRunnable::run()
{
    QImage image;

    QSize ourSize(KIconLoader::SizeEnormous, KIconLoader::SizeEnormous);
    if(d->requestedSize.width() > 0 && d->requestedSize.height() > 0)
    {
        ourSize = d->requestedSize;
    }

    if(QFile(d->id).exists())
    {
        QMimeDatabase db;
        QList<QMimeType> mimetypes = db.mimeTypesForFileName(d->id);
        QString mimetype;
        if(mimetypes.count() > 0)
        {
            mimetype = mimetypes.first().name();
        }

        if(!d->abort) {
            static QStringList allPlugins{KIO::PreviewJob::availablePlugins()};
            d->job = new KIO::PreviewJob(KFileItemList() << KFileItem(QUrl::fromLocalFile(d->id), mimetype, 0), ourSize, &allPlugins);
            d->job->setIgnoreMaximumSize(true);
            d->job->setScaleType(KIO::PreviewJob::ScaledAndCached);
            connect(d->job, SIGNAL(gotPreview(KFileItem,QPixmap)), SLOT(updatePreview(KFileItem,QPixmap)));
            connect(d->job, SIGNAL(failed(KFileItem)), SLOT(fallbackPreview(KFileItem)));
            connect(d->job, SIGNAL(finished(KJob*)), SLOT(finishedPreview(KJob*)));

            d->jobCompletion = false;
            if(d->job->exec())
            {
                // Do not access the job after this point! As we are requesting that
                // it be deleted in finishedPreview(), don't expect it to be around.
                while(!d->jobCompletion) {
                    // Let's let the job do its thing and whatnot...
                    qApp->processEvents();
                }
                if(!d->preview.isNull())
                {
                    if(d->requestedSize.width() > 0 && d->requestedSize.height() > 0)
                    {
                        image = d->preview.scaled(d->requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                    }
                    else
                    {
                        image = d->preview;
                    }
                }
            }
        }
    }
    else
    {
        image = QImage(ourSize, QImage::Format_ARGB32);
    }

    Q_EMIT done(image);
}

void PreviewRunnable::abort()
{
    if (d->job) {
        d->abort = true;
        d->job->kill();
    }
}

void PreviewRunnable::fallbackPreview(const KFileItem& item)
{
    KIO::PreviewJob* previewJob = qobject_cast<KIO::PreviewJob*>(sender());
    if(previewJob)
    {
        QMimeDatabase db;
        QImage preview = QIcon::fromTheme(db.mimeTypeForName(item.mimetype()).iconName()).pixmap(d->requestedSize).toImage();
        d->preview = preview;
        d->jobCompletion = true;
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
    d->jobCompletion = true;
}
