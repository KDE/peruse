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

ContentQuery::ContentQuery(QObject* parent)
    : QObject(parent), d(new Private)
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
    if(!d->mimeTypes.isEmpty())
        return d->mimeTypes;

    return d->mimeTypesForType(d->type);
}

void ContentQuery::setType(ContentQuery::Type type)
{
    if(type == d->type)
        return;

    d->type = type;
    emit typeChanged();
}

void ContentQuery::setSearchString(const QString& searchString)
{
    if(searchString == d->searchString)
        return;

    d->searchString = searchString;
    emit searchStringChanged();
}

void ContentQuery::setLocations(const QStringList& locations)
{
    if(locations == d->locations)
        return;

    d->locations = locations;
    emit locationsChanged();
}

void ContentQuery::setMimeTypes(const QStringList& mimeTypes)
{
    if(mimeTypes == d->mimeTypes)
        return;

    d->mimeTypes = mimeTypes;
    emit mimeTypesChanged();
}

QStringList ContentQuery::Private::mimeTypesForType(ContentQuery::Type type)
{
    QStringList result;
    QMimeDatabase mimeDatabase;

    //TODO: Find a better solution for this.
    switch(type) {
        case ContentQuery::Video:
            result << QStringLiteral("video/x-matroska");
            result << QStringLiteral("video/mp4");
            result << QStringLiteral("video/mpeg");
            result << QStringLiteral("video/ogg");
            result << QStringLiteral("video/quicktime");
            result << QStringLiteral("video/webm");
            result << QStringLiteral("video/x-ms-wmv");
            result << QStringLiteral("video/x-msvideo");
            result << QStringLiteral("video/x-ogm+ogg");
            result << QStringLiteral("video/x-theora+ogg");
            break;
        case ContentQuery::Audio:
            result << QStringLiteral("audio/aac");
            result << QStringLiteral("audio/flac");
            result << QStringLiteral("audio/mp2");
            result << QStringLiteral("audio/mp4");
            result << QStringLiteral("audio/mpeg");
            result << QStringLiteral("audio/ogg");
            result << QStringLiteral("audio/webm");
            result << QStringLiteral("audio/x-opus+ogg");
            result << QStringLiteral("audio/x-ms-wma");
            result << QStringLiteral("audio/x-vorbis+ogg");
            result << QStringLiteral("audio/x-wav");
            break;
        case ContentQuery::Documents:
            result << QStringLiteral("application/vnd.oasis.opendocument.text");
            result << QStringLiteral("application/vnd.oasis.opendocument.spreadsheet");
            result << QStringLiteral("application/vnd.oasis.opendocument.presentation");
            result << QStringLiteral("application/vnd.ms-word");
            result << QStringLiteral("application/vnd.ms-excel");
            result << QStringLiteral("application/vnd.ms-powerpoint");
            result << QStringLiteral("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.xml");
            result << QStringLiteral("application/vnd.openxmlformats-officedocument.wordprocessingml.document.xml");
            result << QStringLiteral("application/vnd.openxmlformats-officedocument.presentationml.presentation.xml");
            result << QStringLiteral("text/plain");
            result << QStringLiteral("application/pdf");
            break;
        case ContentQuery::Images:
            for(const auto& item : QImageReader::supportedMimeTypes())
            {
                result << QString::fromUtf8(item);
            }
            break;
        case ContentQuery::Comics:
            result << QStringLiteral("application/x-cbz");
            result << QStringLiteral("application/x-cbr");
            result << QStringLiteral("application/x-cb7");
            result << QStringLiteral("application/x-cbt");
            result << QStringLiteral("application/x-cba");
            result << QStringLiteral("application/vnd.comicbook+zip");
            result << QStringLiteral("application/vnd.comicbook+rar");
            result << QStringLiteral("application/vnd.ms-htmlhelp");
            result << QStringLiteral("image/vnd.djvu");
            result << QStringLiteral("image/x-djvu");
            result << QStringLiteral("application/epub+zip");
            result << QStringLiteral("application/pdf");
            break;
        default:
            break;
    }

    return result;
}
