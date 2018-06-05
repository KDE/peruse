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

#ifndef ACBFDATABASEREF_H
#define ACBFDATABASEREF_H

#include <memory>
#include "AcbfBookinfo.h"

namespace AdvancedComicBookFormat
{
class ACBF_EXPORT DatabaseRef : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseRef(BookInfo* parent = nullptr);
    ~DatabaseRef() override;

    void toXml(QXmlStreamWriter* writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QString dbname() const;
    void setDbname(const QString& dbname);

    QString type() const;
    void setType(const QString& type);

    QString reference() const;
    void setReference(const QString& reference);
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFDATABASEREF_H
