/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef ACBFMETADATA_H
#define ACBFMETADATA_H

#include "AcbfDocument.h"

class QXmlStreamWriter;
class QXmlStreamReader;
namespace AdvancedComicBookFormat
{
class BookInfo;
class PublishInfo;
class DocumentInfo;
class ACBF_EXPORT Metadata : public QObject {
    Q_OBJECT
    Q_PROPERTY(AdvancedComicBookFormat::BookInfo* bookInfo READ bookInfo NOTIFY bookInfoChanged)
public:
    explicit Metadata(Document* parent = 0);
    ~Metadata() override;

    Document* document();

    void toXml(QXmlStreamWriter *writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    BookInfo* bookInfo();
    Q_SIGNAL void bookInfoChanged();
    PublishInfo* publishInfo();
    DocumentInfo* documentInfo();
private:
    class Private;
    Private* d;
};
}

Q_DECLARE_METATYPE(AdvancedComicBookFormat::Metadata*)

#endif//ACBFMETADATA_H
