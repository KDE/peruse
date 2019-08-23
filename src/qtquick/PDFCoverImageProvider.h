/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef PDFCOVERIMAGEPROVIDER_H
#define PDFCOVERIMAGEPROVIDER_H

#include <QQuickImageProvider>

/**
 * \brief Get file previews of PDF files, where the thumbnailer isn't available...
 * 
 * NOTE: As this task is potentially heavy, make sure to mark any Image using this provider asynchronous
 */
class PDFCoverImageProvider : public QQuickImageProvider
{
public:
    explicit PDFCoverImageProvider();
    virtual ~PDFCoverImageProvider();

    /**
     * \brief Get an image.
     * 
     * @param id The source of the image.
     * @param size The size of the original image, unused.
     * @param requestedSize The required size of the final image, unused.
     * 
     * @return a QPixmap.
     */
    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;
private:
    class Private;
    Private* d;
};

#endif//PDFCOVERIMAGEPROVIDER_H
