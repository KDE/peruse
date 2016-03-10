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

#include "qmlplugin.h"

#include "ArchiveBookModel.h"
#include "BookListModel.h"
#include "BookModel.h"
#include "ComicCoverImageProvider.h"
#include "FolderBookModel.h"
#include "PeruseConfig.h"
#include "PreviewImageProvider.h"
#include "FilterProxy.h"
#include "PropertyContainer.h"

#include <QQmlEngine>
#include <QtQml/qqml.h>

void QmlPlugins::initializeEngine(QQmlEngine *engine, const char *)
{
    engine->addImageProvider("preview", new PreviewImageProvider());
    engine->addImageProvider("comiccover", new ComicCoverImageProvider());
}

void QmlPlugins::registerTypes(const char *uri)
{
    qmlRegisterType<BookListModel>(uri, 0, 1, "BookListModel");
    qmlRegisterType<BookModel>(uri, 0, 1, "BookModel");
    qmlRegisterType<ArchiveBookModel>(uri, 0, 1, "ArchiveBookModel");
    qmlRegisterType<FolderBookModel>(uri, 0, 1, "FolderBookModel");
    qmlRegisterType<PeruseConfig>(uri, 0, 1, "Config");
    qmlRegisterType<PropertyContainer>(uri, 0, 1, "PropertyContainer");
    qmlRegisterType<FilterProxy>(uri, 0, 1, "FilterProxy");
}
