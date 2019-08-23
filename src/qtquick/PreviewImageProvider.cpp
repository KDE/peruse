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
#include <QDebug>

class PreviewImageProvider::Private
{
public:
    Private() {};
    // Yes, we might use the KFileItem here, but we have a one-to-one equivalence between jobs and previews here anyway, so...
    QHash<KIO::PreviewJob*, QPixmap> previews;
    QHash<KJob*, bool> jobCompletion;
};

PreviewImageProvider::PreviewImageProvider(QObject* parent)
    : QObject(parent)
    , QQuickImageProvider(QQuickImageProvider::Pixmap, QQmlImageProviderBase::ForceAsynchronousImageLoading)
    , d(new Private)
{
    qRegisterMetaType<KFileItem>("KFileItem");
}

PreviewImageProvider::~PreviewImageProvider()
{
    delete d;
}

QPixmap PreviewImageProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    QPixmap image;

    QSize ourSize(KIconLoader::SizeEnormous, KIconLoader::SizeEnormous);
    if(requestedSize.width() > 0 && requestedSize.height() > 0)
    {
        ourSize = requestedSize;
    }

    if(QFile(id).exists())
    {
        QMimeDatabase db;
        QList<QMimeType> mimetypes = db.mimeTypesForFileName(id);
        QString mimetype;
        if(mimetypes.count() > 0)
        {
            mimetype = mimetypes.first().name();
        }

        const QStringList* allPlugins = new QStringList(KIO::PreviewJob::availablePlugins());
        KIO::PreviewJob* job = new KIO::PreviewJob(KFileItemList() << KFileItem(QUrl::fromLocalFile(id), mimetype, 0), ourSize, allPlugins);
        job->setIgnoreMaximumSize(true);
        job->setScaleType(KIO::PreviewJob::ScaledAndCached);
        connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)), SLOT(updatePreview(KFileItem,QPixmap)));
        connect(job, SIGNAL(failed(KFileItem)), SLOT(fallbackPreview(KFileItem)));
        connect(job, SIGNAL(finished(KJob*)), SLOT(finishedPreview(KJob*)));

        connect(job, &QObject::destroyed, [job,this](){d->jobCompletion.remove(job);});

        d->jobCompletion[job] = false;
        if(job->exec())
        {
            // Do not access the job after this point! As we are requesting that
            // it be deleted in finishedPreview(), don't expect it to be around.
            while(!d->jobCompletion[job]) {
                // Let's let the job do its thing and whatnot...
                qApp->processEvents();
            }
            if(!d->previews[job].isNull())
            {
                if(requestedSize.width() > 0 && requestedSize.height() > 0)
                {
                    image = d->previews[job].scaled(requestedSize);
                }
                else
                {
                    image = d->previews[job];
                }
            }
        }
        d->previews.remove(job);
        delete allPlugins;
    }
    else
    {
        image = QPixmap(ourSize);
    }

    if(size)
    {
        *size = ourSize;
    }
    return image;
}

void PreviewImageProvider::fallbackPreview(const KFileItem& item)
{
    KIO::PreviewJob* previewJob = qobject_cast<KIO::PreviewJob*>(sender());
    if(previewJob)
    {
        QMimeDatabase db;
        QPixmap preview = QIcon::fromTheme(db.mimeTypeForName(item.mimetype()).iconName()).pixmap(128);
        d->previews[previewJob] = preview;
        d->jobCompletion[previewJob] = true;
    }
}

void PreviewImageProvider::updatePreview(const KFileItem&, const QPixmap& p)
{
    KIO::PreviewJob* previewJob = qobject_cast<KIO::PreviewJob*>(sender());
    if(previewJob)
    {
        d->previews[previewJob] = p;
    }
}

void PreviewImageProvider::finishedPreview(KJob* job)
{
    d->jobCompletion[job] = true;
}
