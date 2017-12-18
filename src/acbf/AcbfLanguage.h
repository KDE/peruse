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

#ifndef ACBFLANGUAGEINFO_H
#define ACBFLANGUAGEINFO_H

#include "AcbfBookinfo.h"

namespace AdvancedComicBookFormat
{
// Due to an oddity in the naming of certain elements in the ACBF standard, this class
// actually represents a text-layer sub-element in the languages subelement of book-info
class ACBF_EXPORT Language : public QObject
{
    Q_OBJECT
public:
    explicit Language(BookInfo* parent = nullptr);
    ~Language() override;

    void toXml(QXmlStreamWriter* writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    void setLanguage(QString language);
    QString language();

    void setShow(bool show);
    bool show();
private:
    class Private;
    Private* d;
};
}

#endif//ACBFLANGUAGEINFO_H
