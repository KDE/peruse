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

#ifndef KRAR_H
#define KRAR_H

#include <karchive.h>

/**
 * KRar is a class for reading archives in the rar format. Writing
 * is not supported.
 * @short A class for reading rar archives.
 * @author Dan Leinir Turthra Jensen <admin@leinir.dk>
 */
class KRar : public KArchive
{
public:
    /**
     * Creates an instance that operates on the given filename.
     * using the compression filter associated to given mimetype.
     *
     * @param filename is a local path (e.g. "/home/leinir/boop.rar")
     */
    KRar(const QString &filename);

    /**
     * Creates an instance that operates on the given device.
     * The device can be compressed (KFilterDev) or not (QFile, etc.).
     * @warning Do not assume that giving a QFile here will decompress the file,
     * in case it's compressed!
     * @param dev the device to access
     */
    KRar(QIODevice *dev);

    /**
     * If the rar file is still opened, then it will be
     * closed automatically by the destructor.
     */
    ~KRar() override;

protected:
    /*
     * Writing is not supported by this class, will always fail.
     * @return always false
     */
    bool doPrepareWriting(const QString &name, const QString &user, const QString &group, qint64 size,
                          mode_t perm, const QDateTime &atime, const QDateTime &mtime, const QDateTime &ctime) override;

    /*
     * Writing is not supported by this class, will always fail.
     * @return always false
     */
    bool doFinishWriting(qint64 size) override;

    /*
     * Writing is not supported by this class, will always fail.
     * @return always false
     */
    bool doWriteDir(const QString &name, const QString &user, const QString &group,
                    mode_t perm, const QDateTime &atime, const QDateTime &mtime, const QDateTime &ctime) override;

    bool doWriteSymLink(const QString &name, const QString &target,
                        const QString &user, const QString &group, mode_t perm,
                        const QDateTime &atime, const QDateTime &mtime, const QDateTime &ctime) override;

    /**
     * Opens the archive for reading.
     * Parses the directory listing of the archive
     * and creates the KArchiveDirectory/KArchiveFile entries.
     *
     */
    bool openArchive(QIODevice::OpenMode mode) override;
    bool closeArchive() override;

protected:
    void virtual_hook(int id, void *data) override;

private:
    class Private;
    Private* d;
};

#endif//KRAR_H
