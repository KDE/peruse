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

#ifndef BALOOCONTENTLISTER_H
#define BALOOCONTENTLISTER_H

#include "ContentListerBase.h"

#include <Baloo/QueryRunnable>

#include <QString>

class BalooContentLister : public ContentListerBase
{
    Q_OBJECT
public:
    explicit BalooContentLister(QObject* parent = nullptr);
    ~BalooContentLister() override;

    /**
     * @returns whether Baloo is enabled. If baloo is not available on the system, we cannot use it.
     */
    bool balooEnabled() const;

    /**
     * \brief Start a search.
     * @param queries  List of ContentQueries that the search should be limited to.
     */
    void startSearch(const QList<ContentQuery*>& queries) override;

private:
    class Private;
    Private* d;

    Q_SLOT void queryCompleted(Baloo::QueryRunnable* query);
    void queryResult(const ContentQuery* query, const QString& location, const QString& file);
};

#endif//BALOOCONTENTLISTER_H
