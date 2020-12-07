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

#ifndef KRARFILEENTRY_H
#define KRARFILEENTRY_H

#include "KRar.h"
/**
 * A KRarFileEntry represents a file in a rar archive.
 */
class KRarFileEntry : public KArchiveFile
{
public:
    /**
     * Creates a new rar file entry. Do not call this, KRar takes care of it.
     */
    KRarFileEntry(KRar *rar, const QString &name, int access, const QDateTime &date,
                  const QString &user, const QString &group, const QString &symlink,
                  const QString &path, qint64 start, qint64 uncompressedSize, struct ar_archive_s* archive);

    /**
     * Destructor. Do not call this.
     */
    ~KRarFileEntry() override;

    /// Header start: only used when writing, consequently not used (as we don't support writing)
    void setHeaderStart(qint64 headerstart);
    qint64 headerStart() const;

    /// CRC: only used when writing, consequently not used (as we don't support writing)
    unsigned long crc32() const;
    void setCRC32(unsigned long crc32);

    /// Name with complete path - KArchiveFile::name() is the filename only (no path)
    const QString &path() const;

    /**
     * @return the content of this file.
     * Call data() with care (only once per file), this data isn't cached.
     */
    QByteArray data() const override;

    /**
     * This method returns a QIODevice to read the file contents.
     * This is obviously for reading only.
     * Note that the ownership of the device is being transferred to the caller,
     * who will have to delete it.
     * The returned device auto-opens (in readonly mode), no need to open it.
     */
    QIODevice *createDevice() const override;

private:
    class Private;
    Private *const d;
};

#endif//KRARFILEENTRY_H

