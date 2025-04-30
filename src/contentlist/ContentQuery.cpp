// SPDX-FileCopyrightText: 2018 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "ContentQuery.h"

#include <QMimeDatabase>
#include <QtGui/QImageReader>

using namespace Qt::StringLiterals;

class ContentQuery::Private
{
public:
    QStringList mimeTypesForType(Type type) const;

    Type type = Any;
    QString searchString;
    QStringList locations;
    QStringList mimeTypes;
};

ContentQuery::ContentQuery(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Private>())
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
        u"video/x-matroska"_s,
        u"video/mp4"_s,
        u"video/mpeg"_s,
        u"video/ogg"_s,
        u"video/quicktime"_s,
        u"video/webm"_s,
        u"video/x-ms-wmv"_s,
        u"video/x-msvideo"_s,
        u"video/x-ogm+ogg"_s,
        u"video/x-theora+ogg"_s,
    };
}

QStringList contentQueryAudio()
{
    return {u"audio/aac"_s,
            u"audio/flac"_s,
            u"audio/mp2"_s,
            u"audio/mp4"_s,
            u"audio/mpeg"_s,
            u"audio/ogg"_s,
            u"audio/webm"_s,
            u"audio/x-opus+ogg"_s,
            u"audio/x-ms-wma"_s,
            u"audio/x-vorbis+ogg"_s,
            u"audio/x-wav"_s};
}

QStringList contentQueryDocuments()
{
    return {u"application/vnd.oasis.opendocument.text"_s,
            u"application/vnd.oasis.opendocument.spreadsheet"_s,
            u"application/vnd.oasis.opendocument.presentation"_s,
            u"application/vnd.ms-word"_s,
            u"application/vnd.ms-excel"_s,
            u"application/vnd.ms-powerpoint"_s,
            u"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.xml"_s,
            u"application/vnd.openxmlformats-officedocument.wordprocessingml.document.xml"_s,
            u"application/vnd.openxmlformats-officedocument.presentationml.presentation.xml"_s,
            u"text/plain"_s,
            u"application/pdf"_s};
}

QStringList contentQueryImages()
{
    // only popylate once.
    static QStringList result;
    if (result.isEmpty()) {
        const auto items = QImageReader::supportedMimeTypes();
        for (const auto &item : items) {
            result << QString::fromUtf8(item);
        }
    }
    return result;
}

QStringList contentQueryComics()
{
    return {u"application/x-cbz"_s,
            u"application/x-cbr"_s,
            u"application/x-cb7"_s,
            u"application/x-cbt"_s,
            u"application/x-cba"_s,
            u"application/vnd.comicbook+zip"_s,
            u"application/vnd.comicbook+rar"_s,
            u"application/vnd.ms-htmlhelp"_s,
            u"image/vnd.djvu"_s,
            u"image/x-djvu"_s,
            u"application/epub+zip"_s,
            u"application/pdf"_s};
}
}
QStringList ContentQuery::Private::mimeTypesForType(ContentQuery::Type type) const
{
    switch (type) {
    case ContentQuery::Type::Video:
        return contentQueryVideo();
    case ContentQuery::Type::Audio:
        return contentQueryAudio();
    case ContentQuery::Type::Documents:
        return contentQueryDocuments();
    case ContentQuery::Type::Images:
        return contentQueryImages();
    case ContentQuery::Type::Comics:
        return contentQueryComics();
    case ContentQuery::Type::Any: /* do nothing */
    default:
        return {};
    }
}
