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

#ifndef PREVIEWIMAGEPROVIDER_H
#define PREVIEWIMAGEPROVIDER_H

#include <QQuickImageProvider>

/**
 * \brief Get file previews using KIO::PreviewJob
 *
 * NOTE: As this task is potentially heavy, make sure to mark any Image using this provider asynchronous
 */
class KFileItem;
class KJob;
class PreviewImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    explicit PreviewImageProvider(QObject* parent = nullptr);
    ~PreviewImageProvider() override;

    /**
     * \brief Get an image.
     * 
     * @param id The source of the image.
     * @param size The size of the original image.
     * @param requestedSize The required size of the final image.
     * 
     * @return a QPixmap.
     */
    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;

    /**
     *\brief Get an icon associated with the mimetype of the image as a fallback.
     * 
     * @param p Pointer to pixmap to write this fallback into.
     */
    Q_SLOT void updatePreview(const KFileItem&, const QPixmap& p);
    /**
     *\brief Get an icon associated with the mimetype of the image as a fallback.
     * 
     * @param item The image to write a fallback for.
     */
    Q_SLOT void fallbackPreview(const KFileItem& item);
    /**
     * \brief Set whether the preview generation is finished.
     * 
     * @param job The job to mark finished.
     */
    Q_SLOT void finishedPreview(KJob* job);
private:
    class Private;
    Private* d;
};

#endif//PREVIEWIMAGEPROVIDER_H
