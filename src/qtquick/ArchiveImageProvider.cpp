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

#include <QIcon>

#include <AcbfDocument.h>
#include <AcbfBinary.h>
#include <AcbfData.h>

#include <qtquick_debug.h>

class ArchiveImageProvider::Private
{
public:
    Private()
        : bookModel(nullptr)
    {}
    ArchiveBookModel* bookModel;
    QString prefix;
};

ArchiveImageProvider::ArchiveImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
    , d(new Private)
{
}

ArchiveImageProvider::~ArchiveImageProvider()
{
    delete d;
}

QImage ArchiveImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(size)
    Q_UNUSED(requestedSize)
    QImage img;
    bool success = false;

    /*
     * In ACBF, image references starting with a '#' refer to files embedded
     * in the <data> section of the .acbf file.
     * see: http://acbf.wikia.com/wiki/Body_Section_Definition#Image
     * TODO: binary files can also handle fonts, and those cannot be loaded into a QImage.
     */
    if (id.startsWith('#')) {
        auto document = qobject_cast<AdvancedComicBookFormat::Document*>(d->bookModel->acbfData());

        if (document) {
            AdvancedComicBookFormat::Binary* binary = document->data()->binary(id.mid(1));

            if (binary) {
                success = img.loadFromData(binary->data());
            }
        }
    }

    if (!success) {
        const KArchiveFile* entry = d->bookModel->archiveFile(id);

        if(entry) {
            success = img.loadFromData(entry->data());
        }
    }

    if (!success) {
        QIcon oops = QIcon::fromTheme("unknown");
        img = oops.pixmap(oops.availableSizes().last()).toImage();
        qCDebug(QTQUICK_LOG) << "Failed to load image with id:" << id;
    }

    return img;
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
