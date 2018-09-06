/*
 * Copyright 2018  Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
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

#include "AcbfJump.h"
#include <QDebug>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class Jump::Private
{
public:
    Private()
        : pageIndex(0)
    {}
    QList<QPoint> points;
    int pageIndex;
};

Jump::Jump(Page* parent)
    : QObject(parent)
    , d(new Private)
{
}

Jump::~Jump() = default;

void Jump::toXml(QXmlStreamWriter* writer) {
    writer->writeStartElement(QStringLiteral("jump"));
    
    QStringList points;
    Q_FOREACH(const QPoint& point, d->points) {
        points << QStringLiteral("%1,%2").arg(QString::number(point.x())).arg(QString::number(point.y()));
    }
    writer->writeAttribute(QStringLiteral("points"), points.join(' '));
    writer->writeAttribute(QStringLiteral("page"), QString::number(d->pageIndex));
    
    writer->writeEndElement();
}

bool Jump::fromXml(QXmlStreamReader *xmlReader)
{
    setPageIndex(xmlReader->attributes().value(QStringLiteral("page")).toInt());

    QStringList points = xmlReader->attributes().value(QStringLiteral("points")).toString().split(' ');
    Q_FOREACH(const QString& point, points) {
        QStringList elements = point.split(',');
        if(elements.length() == 2)
        {
            addPoint(QPoint(elements.at(0).toInt(), elements.at(1).toInt()));
        }
        else
        {
            qWarning() << "Failed to construct one of the points for a jump. Attempted to handle the point" << point << "in the data" << points;
            return false;
        }
    }

    if (xmlReader->hasError()) {
        qWarning() << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qDebug() << Q_FUNC_INFO << "Created a jump with " << points.count() << "points, to page " << d->pageIndex;

    return !xmlReader->hasError();
}

QList<QPoint> Jump::points() const
{
    return d->points;
}

QPoint Jump::point(int index) const
{
    return d->points.at(index);
}

int Jump::pointIndex(const QPoint& point) const
{
    return d->points.indexOf(point);
}

void Jump::addPoint(const QPoint& point, int index)
{
    if(index > -1 && d->points.count() < index) {
        d->points.insert(index, point);
    }
    else {
        d->points.append(point);
    }
}

void Jump::removePoint(const QPoint& point)
{
    d->points.removeAll(point);
}

bool Jump::swapPoints(const QPoint& swapThis, const QPoint& withThis)
{
    int index1 = d->points.indexOf(swapThis);
    int index2 = d->points.indexOf(withThis);
    if(index1 > -1 && index2 > -1) {
        d->points.swap(index1, index2);
        return true;
    }
    return false;
}

int Jump::pageIndex() const
{
    return d->pageIndex;
}

void Jump::setPageIndex(const int& pageNumber)
{
    d->pageIndex = pageNumber;
}
