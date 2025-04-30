// SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#ifndef FILESYSTEMCONTENTLISTER_H
#define FILESYSTEMCONTENTLISTER_H

#include <QRunnable>

#include "ContentListerBase.h"

class FilesystemContentLister : public ContentListerBase
{
    Q_OBJECT
public:
    explicit FilesystemContentLister(QObject *parent = nullptr);
    ~FilesystemContentLister() override;
    /**
     * \brief Start a search.
     * @param queries  List of ContentQueries that the search should be limited to.
     */
    void startSearch(const QList<ContentQuery *> &queries) override;

private:
    void queryFinished(QRunnable *runnable);

    class Private;
    Private *d;
};

#endif // FILESYSTEMCONTENTLISTER_H
