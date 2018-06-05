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

#ifndef ACBFBODY_H
#define ACBFBODY_H

#include <memory>

#include "AcbfDocument.h"

#include <QDate>
class QXmlStreamWriter;
class QXmlStreamReader;

namespace AdvancedComicBookFormat
{
class Page;
class ACBF_EXPORT Body : public QObject
{
    Q_OBJECT
public:
    explicit Body(Document* parent = nullptr);
    ~Body();

    Document* document() const;

    void toXml(QXmlStreamWriter *writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QString bgcolor() const;
    void setBgcolor(const QString& newColor);

    QList<Page*> pages() const;
    Page* page(int index) const;
    int pageIndex(Page* page) const;

    // If afterIndex is larger than zero, the insertion will happen at that index
    void addPage(Page* page, int index = -1);
    void removePage(Page* page);
    bool swapPages(Page* swapThis, Page* withThis);

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFBODY_H
