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

#ifndef ACBFSEQUENCE_H
#define ACBFSEQUENCE_H

#include <memory>

#include "AcbfBookinfo.h"
/**
 * \brief Class to handle the sequence information.
 *
 * A comic book can be part of several collections or series.
 * ACBF can handle multiple sequences, with numbers and volumes.
 *
 */
namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Sequence : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
public:
    explicit Sequence(BookInfo *parent = nullptr);
    ~Sequence() override;

    /**
     * \brief Write the sequence into the xml writer.
     */
    void toXml(QXmlStreamWriter *writer);
    /**
     * \brief load the sequence element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @returns the name of the series or sequence.
     */
    QString title() const;
    /**
     * \brief set the name of the sequence.
     * @param title - the name of the sequence as a QString.
     */
    void setTitle(const QString &title);

    Q_SIGNAL void titleChanged();

    /**
     * @returns the volume number.
     */
    int volume() const;
    /**
     * \brief set the volume number.
     * @param volume - The volume number, this is optional
     * if set to 0, this is not saved
     */
    void setVolume(int volume = 0);

    Q_SIGNAL void volumeChanged();

    /**
     * @return the number that this work takes up in the sequence.
     */
    int number() const;
    /**
     * \brief set the number in the sequence.
     * @param number - an interger number that indicates where it is
     * part of the series.
     */
    void setNumber(int number);

    Q_SIGNAL void numberChanged();

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFSEQUENCE_H
