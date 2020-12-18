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

#ifndef COMICCOVERIMAGEPROVIDER_H
#define COMICCOVERIMAGEPROVIDER_H

#include <QQuickAsyncImageProvider>
#include <QRunnable>

#include <kimagecache.h>
#include <memory.h>

/**
 * \brief Get file previews of Comic Book Archives 
 * 
 * TODO This should go into a thumbnailer later, once karchive-rar is merged into KArchive
 *
 * NOTE: As this task is potentially heavy, make sure to mark any Image using this provider asynchronous
 */
class ComicCoverImageProvider : public QQuickAsyncImageProvider
{
public:
    explicit ComicCoverImageProvider();
    ~ComicCoverImageProvider() override;

    /**
     * \brief Get an image.
     * 
     * @param id The source of the image.
     * @param requestedSize The required size of the final image, unused.
     * 
     * @return an asynchronous image response
     */
    QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;
private:
    class Private;
    Private* d;
};

/**
 * \brief A worker class which does the bulk of the work for PreviewImageProvider
 */
class ComicCoverRunnable : public QObject, public QRunnable {
    Q_OBJECT;
public:
    explicit ComicCoverRunnable(const QString &id, const QSize &requestedSize, KImageCache* imageCache);
    virtual ~ComicCoverRunnable();

    void run() override;

    /**
     * Request that the preview worker abort what it's doing
     */
    Q_SLOT void abort();

    /**
     * \brief Emitted once the preview has been retrieved (successfully or not)
     * @param image The preview image in the requested size (possibly a placeholder)
     */
    Q_SIGNAL void done(QImage image);
private:
    class Private;
    std::unique_ptr<Private> d;
};

#endif//COMICCOVERIMAGEPROVIDER_H
