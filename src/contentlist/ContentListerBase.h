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

#ifndef CONTENTLISTERBASE_H
#define CONTENTLISTERBASE_H

#include <QObject>
#include <QSet>
#include <QString>

class ContentQuery;
/**
 * \brief Class to handle the search.
 * 
 * This class can be extended to handle other search engines,
 * such as baloo and the file system content lister.
 * 
 * By default it only searches the KFileMetaData available to it.
 */
class ContentListerBase : public QObject
{
    Q_OBJECT
public:
    explicit ContentListerBase(QObject* parent = nullptr);
    ~ContentListerBase() override;

    /**
     * \brief Start a search.
     * @param queries  List of ContentQueries that the search should be limited to.
     */
    Q_SLOT virtual void startSearch(const QList<ContentQuery*>& queries);

    /**
     * \brief Fires when a matching file is found.
     */
    Q_SIGNAL void fileFound(const QString& filePath, const QVariantMap& metadata);
    /**
     * \brief Fires when the search was completed.
     */
    Q_SIGNAL void searchCompleted();

    /**
     * @return the available metadata for the filepath so that it can be searched.
     */
    static QVariantMap metaDataForFile(const QString& file);

protected:
    friend class ContentList;
    QSet<QString> knownFiles;
};

#endif//CONTENTLISTERBASE_H
