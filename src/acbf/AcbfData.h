/*
 * Copyright (C) 2017 Jesse Pullinen <jesse12p@gmail.com>
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

#ifndef ACBFDATA_H
#define ACBFDATA_H

#include <QObject>
#include <QXmlStreamReader>

#include "AcbfDocument.h"
#include "AcbfBinary.h"

namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Data : public QObject
{
    Q_OBJECT
public:
    explicit Data(Document* parent = 0);
    virtual ~Data();

    void toXml(QXmlStreamWriter *writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    Binary* binary(QString id);
private:
    class Private;
    Private* d;
};
}

#endif // ACBFDATA_H
