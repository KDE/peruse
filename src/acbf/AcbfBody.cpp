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

#include "AcbfBody.h"
#include "AcbfPage.h"

#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class Body::Private
{
public:
    Private() {}
    QString bgcolor;
    QList<Page*> pages;
};

Body::Body(Document* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Body*>("Body*");
    Q_UNUSED(typeId);
}

Body::~Body() = default;

Document * Body::document() const
{
    return qobject_cast<Document*>(parent());
}

void Body::toXml(QXmlStreamWriter *writer)
{
    writer->writeStartElement(QStringLiteral("body"));

    for(Page* page : d->pages) {
        page->toXml(writer);
    }

    writer->writeEndElement();
}

bool Body::fromXml(QXmlStreamReader *xmlReader, const QString& xmlData)
{
    setBgcolor(xmlReader->attributes().value(QStringLiteral("bgcolor")).toString());
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("page"))
        {
            Page* newPage = new Page(document());
            if(!newPage->fromXml(xmlReader, xmlData)) {
                return false;
            }
            d->pages.append(newPage);
        }
        else
        {
            qCWarning(ACBF_LOG) << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader->name();
            xmlReader->skipCurrentElement();
        }
    }
    if (xmlReader->hasError()) {
        qCWarning(ACBF_LOG) << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created body with" << d->pages.count() << "pages";
    return !xmlReader->hasError();
}

QString Body::bgcolor() const
{
    return d->bgcolor;
}

void Body::setBgcolor(const QString& newColor)
{
    d->bgcolor = newColor;
    emit bgcolorChanged();
}

QList<Page *> Body::pages() const
{
    return d->pages;
}

Page * Body::page(int index) const
{
    return d->pages.at(index);
}

int Body::pageIndex(Page* page) const
{
    return d->pages.indexOf(page);
}

void Body::addPage(Page* page, int index)
{
    if(index > -1 && d->pages.count() < index) {
        d->pages.insert(index, page);
    }
    else {
        d->pages.append(page);
    }
    Q_EMIT pageAdded(page);
    emit pageCountChanged();
}

void Body::removePage(Page* page)
{
    d->pages.removeAll(page);
    emit pageCountChanged();
}

bool Body::swapPages(Page* swapThis, Page* withThis)
{
    int index1 = d->pages.indexOf(swapThis);
    int index2 = d->pages.indexOf(withThis);
    if(index1 > -1 && index2 > -1) {
        d->pages.swapItemsAt(index1, index2);
        emit pageCountChanged();
        return true;
    }
    return false;
}

int Body::pageCount()
{
    return d->pages.size();
}
