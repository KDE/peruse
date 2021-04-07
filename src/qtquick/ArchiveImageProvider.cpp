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

#include "ArchiveImageProvider.h"
#include "ArchiveBookModel.h"

#include <karchive.h>
#include <karchivefile.h>

#include <QBuffer>
#include <QIcon>
#include <QImageReader>
#include <QPainter>
#include <QThreadPool>

#include <AcbfDocument.h>
#include <AcbfBinary.h>
#include <AcbfData.h>

#include <qtquick_debug.h>

class ArchiveImageProvider::Private
{
public:
    Private() {}

    ArchiveBookModel* bookModel{nullptr};
    QString prefix;
};

ArchiveImageProvider::ArchiveImageProvider()
    : QQuickAsyncImageProvider()
    , d(new Private)
{
}

ArchiveImageProvider::~ArchiveImageProvider()
{
    delete d;
}

class ArchiveImageResponse : public QQuickImageResponse
{
    public:
        ArchiveImageResponse(const QString &id, const QSize &requestedSize, ArchiveBookModel* bookModel, const QString& prefix)
        {
            m_runnable = new ArchiveImageRunnable(id, requestedSize, bookModel, prefix);
            m_runnable->setAutoDelete(false);
            connect(m_runnable, &ArchiveImageRunnable::done, this, &ArchiveImageResponse::handleDone, Qt::QueuedConnection);
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

        ArchiveImageRunnable* m_runnable{nullptr};
        QImage m_image;
};

QQuickImageResponse * ArchiveImageProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
    ArchiveImageResponse* response = new ArchiveImageResponse(id, requestedSize, d->bookModel, d->prefix);
    return response;
}

void ArchiveImageProvider::setArchiveBookModel(ArchiveBookModel* model)
{
    d->bookModel = model;
}

void ArchiveImageProvider::setPrefix(QString prefix)
{
    d->prefix = prefix;
}

QString ArchiveImageProvider::prefix() const
{
    return d->prefix;
}

class ArchiveImageRunnable::Private {
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

    ArchiveBookModel* bookModel{nullptr};
    QString prefix;

    QString errorString;
    bool loadImage(QImage *image, const QByteArray &data)
    {
        QBuffer b;
        b.setData(data);
        b.open(QIODevice::ReadOnly);
        QImageReader reader(&b, nullptr);
        bool success = reader.read(image);
        if (success) {
            errorString.clear();
        } else {
            errorString = reader.errorString();
        }
        return success;
    }
};

ArchiveImageRunnable::ArchiveImageRunnable(const QString& id, const QSize& requestedSize, ArchiveBookModel* bookModel, const QString& prefix)
    : d(new Private)
{
    d->id = id;
    d->requestedSize = requestedSize;
    d->bookModel = bookModel;
    d->prefix = prefix;
}

ArchiveImageRunnable::~ArchiveImageRunnable()
{
    abort();
    delete d;
}

void ArchiveImageRunnable::abort()
{
    QMutexLocker locker(&d->abortMutex);
    d->abort = true;
}

void ArchiveImageRunnable::run()//const QString& id, QSize* size, const QSize& requestedSize)
{
    QImage img;
    bool success = false;

    /*
     * In ACBF, image references starting with a '#' refer to files embedded
     * in the <data> section of the .acbf file.
     * see: http://acbf.wikia.com/wiki/Body_Section_Definition#Image
     * TODO: binary files can also handle fonts, and those cannot be loaded into a QImage.
     */
    if (d->id.startsWith('#')) {
        auto document = qobject_cast<AdvancedComicBookFormat::Document*>(d->bookModel->acbfData());

        if (document) {
            AdvancedComicBookFormat::Binary* binary = qobject_cast<AdvancedComicBookFormat::Binary*>(document->objectByID(d->id.mid(1)));

            if (!d->isAborted() && binary) {
                success = d->loadImage(&img, binary->data());
            }
        }
    }

    if (!d->isAborted() && !success) {
        QMutexLocker locker(&d->bookModel->archiveMutex);
        const KArchiveFile* entry = d->bookModel->archiveFile(d->id);

        if(!d->isAborted() && entry) {
            success = d->loadImage(&img, entry->data());
        }
    }

    if (!d->isAborted() && !success) {
        QIcon oops = QIcon::fromTheme("unknown");
        img = oops.pixmap(oops.availableSizes().last()).toImage();
        QPainter thing(&img);
        thing.drawText(img.rect(), Qt::AlignCenter | Qt::TextWordWrap, d->errorString);
        qCDebug(QTQUICK_LOG) << "Failed to load image with id:" << d->id << "and the error" << d->errorString;
    }

    Q_EMIT done(img);
}
