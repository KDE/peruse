// SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

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
    explicit ContentListerBase(QObject *parent = nullptr);
    ~ContentListerBase() override;

    /**
     * \brief Start a search.
     * @param queries  List of ContentQueries that the search should be limited to.
     */
    Q_SLOT virtual void startSearch(const QList<ContentQuery *> &queries);

    /**
     * \brief Fires when a matching file is found.
     */
    Q_SIGNAL void fileFound(const QString &filePath, const QVariantMap &metadata);
    /**
     * \brief Fires when the search was completed.
     */
    Q_SIGNAL void searchCompleted();

    /**
     * @return the available metadata for the filepath so that it can be searched.
     */
    static QVariantMap metaDataForFile(const QString &file);

protected:
    friend class ContentList;
    QSet<QString> knownFiles;
};

#endif // CONTENTLISTERBASE_H
