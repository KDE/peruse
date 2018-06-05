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

namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Sequence : public QObject
{
    Q_OBJECT
public:
    explicit Sequence(BookInfo * parent = nullptr);
    ~Sequence() override;

    void toXml(QXmlStreamWriter* writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QString title() const;
    void setTitle(const QString& title);

    // Optional attribute - if set to 0, this is not saved
    int volume() const;
    void setVolume(int volume = 0);

    int number() const;
    void setNumber(int number);
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFSEQUENCE_H
