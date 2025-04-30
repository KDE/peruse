// SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "ContentListerBase.h"

#include <QDateTime>
#include <QFileInfo>
#include <QVariantMap>

#include <KFileMetaData/UserMetaData>

ContentListerBase::ContentListerBase(QObject *parent)
    : QObject(parent)
{
}

ContentListerBase::~ContentListerBase()
{
}

void ContentListerBase::startSearch(const QList<ContentQuery *> &queries)
{
    Q_UNUSED(queries);
}

QVariantMap ContentListerBase::metaDataForFile(const QString &file)
{
    QVariantMap metadata;

    // TODO: This should include the same information for both the Baloo and
    // File searchers. Unfortunately, currently KFileMetaData does not seem able
    // to provide this. So this needs changes at a lower level.

    QFileInfo info(file);
    metadata["lastModified"] = info.lastModified();
    metadata["created"] = info.birthTime();
    metadata["lastRead"] = info.lastRead();

    KFileMetaData::UserMetaData data(file);
    if (data.hasAttribute("peruse.currentPage")) {
        int currentPage = data.attribute("peruse.currentPage").toInt();
        metadata["currentPage"] = QVariant::fromValue<int>(currentPage);
    }
    if (data.hasAttribute("peruse.totalPages")) {
        int totalPages = data.attribute("peruse.totalPages").toInt();
        metadata["totalPages"] = QVariant::fromValue<int>(totalPages);
    }
    if (!data.tags().isEmpty()) {
        metadata["tags"] = QVariant::fromValue<QStringList>(data.tags());
    }
    if (!data.userComment().isEmpty()) {
        metadata["comment"] = QVariant::fromValue<QString>(data.userComment());
    }
    metadata["rating"] = QVariant::fromValue<int>(data.rating());

    return metadata;
}
