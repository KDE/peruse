// SPDX-FileCopyrightText: 2018 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#ifndef CONTENTQUERY_H
#define CONTENTQUERY_H

#include <memory>

#include <QObject>
#include <qqmlintegration.h>

/**
 * Encapsulates searching parameters for files on the file system.
 */
class ContentQuery : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    /**
     * The type of files to search for.
     */
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    /**
     * A string that should be included in the file's file name.
     */
    Q_PROPERTY(QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged)
    /**
     * A list of directories. Only these directories and their subdirectories will be searched.
     */
    Q_PROPERTY(QStringList locations READ locations WRITE setLocations NOTIFY locationsChanged)
    /**
     * A list of mime type names to search for.
     *
     * Note that if this property has not explicitly been set, the list of mime types
     * is based on the type property.
     */
    Q_PROPERTY(QStringList mimeTypes READ mimeTypes WRITE setMimeTypes NOTIFY mimeTypesChanged)

public:
    /**
     * The type of files to search for.
     */
    enum Type {
        Any, ///< Do not limit results by any type.
        Video, ///< Only search for videos.
        Audio, ///< Only search for audio files.
        Documents, ///< Only search for documents.
        Images, ///< Only search for images.
        Comics, ///< Only search for comic books.
    };
    Q_ENUM(Type)

    /**
     * Constructor
     *
     * @param parent The QObject parent.
     */
    explicit ContentQuery(QObject *parent = nullptr);

    /**
     * Destructor
     */
    ~ContentQuery() override;

    /**
     * Get the type property.
     */
    Type type() const;
    /**
     * Get the searchString property.
     */
    QString searchString() const;
    /**
     * Get the locations property.
     */
    QStringList locations() const;
    /**
     * Get the mimeTypes property.
     */
    QStringList mimeTypes() const;

public Q_SLOTS:
    /**
     * Set the type property.
     *
     * \param type The new type.
     */
    void setType(Type type);
    /**
     * Set the searchString property.
     *
     * \param searchString The new search string.
     */
    void setSearchString(const QString &searchString);
    /**
     * Set the location property.
     *
     * \param location The new location.
     */
    void setLocations(const QStringList &location);
    /**
     * Set the mimeTypes property.
     *
     * \param mimeTypes The new list of mime types.
     */
    void setMimeTypes(const QStringList &mimeTypes);

Q_SIGNALS:
    /**
     * Emitted whenever the type property changes.
     */
    void typeChanged();
    /**
     * Emitted whenever the searchString property changes.
     */
    void searchStringChanged();
    /**
     * Emitted whenever the location property changes.
     */
    void locationsChanged();
    /**
     * Emitted whenever the mimeTypes property changes.
     */
    void mimeTypesChanged();

private:
    class Private;
    const std::unique_ptr<Private> d;
};

#endif // CONTENTQUERY_H
