/*
 * Copyright 2018 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ContentQuery.h"

#include <QMimeDatabase>
#include <QtGui/QImageReader>

class ContentQuery::Private
{
public:
    QStringList mimeTypesForType(Type type);

    Type type = Any;
    QString searchString;
    QStringList locations;
    QStringList mimeTypes;
};

ContentQuery::ContentQuery(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

ContentQuery::~ContentQuery()
{
}

ContentQuery::Type ContentQuery::type() const
{
    return d->type;
}

QString ContentQuery::searchString() const
{
    return d->searchString;
}

QStringList ContentQuery::locations() const
{
    return d->locations;
}

QStringList ContentQuery::mimeTypes() const
{
    if (!d->mimeTypes.isEmpty())
        return d->mimeTypes;

    return d->mimeTypesForType(d->type);
}

void ContentQuery::setType(ContentQuery::Type type)
{
    if (type == d->type)
        return;

    d->type = type;
    emit typeChanged();
}

void ContentQuery::setSearchString(const QString &searchString)
{
    if (searchString == d->searchString)
        return;

    d->searchString = searchString;
    emit searchStringChanged();
}

void ContentQuery::setLocations(const QStringList &locations)
{
    if (locations == d->locations)
        return;

    d->locations = locations;
    emit locationsChanged();
}

void ContentQuery::setMimeTypes(const QStringList &mimeTypes)
{
    if (mimeTypes == d->mimeTypes)
        return;

    d->mimeTypes = mimeTypes;
    emit mimeTypesChanged();
}

namespace
{
QStringList contentQueryVideo()
{
    return {
        QStringLiteral("video/x-matroska"),
        QStringLiteral("video/mp4"),
        QStringLiteral("video/mpeg"),
        QStringLiteral("video/ogg"),
        QStringLiteral("video/quicktime"),
        QStringLiteral("video/webm"),
        QStringLiteral("video/x-ms-wmv"),
        QStringLiteral("video/x-msvideo"),
        QStringLiteral("video/x-ogm+ogg"),
        QStringLiteral("video/x-theora+ogg"),
    };
}

QStringList contentQueryAudio()
{
    return {QStringLiteral("audio/aac"),
            QStringLiteral("audio/flac"),
            QStringLiteral("audio/mp2"),
            QStringLiteral("audio/mp4"),
            QStringLiteral("audio/mpeg"),
            QStringLiteral("audio/ogg"),
            QStringLiteral("audio/webm"),
            QStringLiteral("audio/x-opus+ogg"),
            QStringLiteral("audio/x-ms-wma"),
            QStringLiteral("audio/x-vorbis+ogg"),
            QStringLiteral("audio/x-wav")};
}

QStringList contentQueryDocuments()
{
    return {QStringLiteral("application/vnd.oasis.opendocument.text"),
            QStringLiteral("application/vnd.oasis.opendocument.spreadsheet"),
            QStringLiteral("application/vnd.oasis.opendocument.presentation"),
            QStringLiteral("application/vnd.ms-word"),
            QStringLiteral("application/vnd.ms-excel"),
            QStringLiteral("application/vnd.ms-powerpoint"),
            QStringLiteral("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.xml"),
            QStringLiteral("application/vnd.openxmlformats-officedocument.wordprocessingml.document.xml"),
            QStringLiteral("application/vnd.openxmlformats-officedocument.presentationml.presentation.xml"),
            QStringLiteral("text/plain"),
            QStringLiteral("application/pdf")};
}

QStringList contentQueryImages()
{
    // only popylate once.
    static QStringList result;
    if (result.isEmpty()) {
        for (const auto &item : QImageReader::supportedMimeTypes()) {
            result << QString::fromUtf8(item);
        }
    }
    return result;
}

QStringList contentQueryComics()
{
    return {QStringLiteral("application/x-cbz"),
            QStringLiteral("application/x-cbr"),
            QStringLiteral("application/x-cb7"),
            QStringLiteral("application/x-cbt"),
            QStringLiteral("application/x-cba"),
            QStringLiteral("application/vnd.comicbook+zip"),
            QStringLiteral("application/vnd.comicbook+rar"),
            QStringLiteral("application/vnd.ms-htmlhelp"),
            QStringLiteral("image/vnd.djvu"),
            QStringLiteral("image/x-djvu"),
            QStringLiteral("application/epub+zip"),
            QStringLiteral("application/pdf")};
}
}
QStringList ContentQuery::Private::mimeTypesForType(ContentQuery::Type type)
{
    QStringList ret{};
    switch (type) {
    case ContentQuery::Type::Video:
        ret = contentQueryVideo();
        break;
    case ContentQuery::Type::Audio:
        ret = contentQueryAudio();
        break;
    case ContentQuery::Type::Documents:
        ret = contentQueryDocuments();
        break;
    case ContentQuery::Type::Images:
        ret = contentQueryImages();
        break;
    case ContentQuery::Type::Comics:
        ret = contentQueryComics();
        break;
    case ContentQuery::Type::Any: /* do nothing */
        break;
    }
    return ret;
}
