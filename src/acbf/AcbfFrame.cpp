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

#include "AcbfFrame.h"
#include <QDebug>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class Frame::Private
{
public:
    Private()
    {}
    QString bgcolor;
    QList<QPoint> points;
};

Frame::Frame(Page* parent)
    : QObject(parent)
    , d(new Private)
{
}

Frame::~Frame() = default;

void Frame::toXml(QXmlStreamWriter* writer) {
    writer->writeStartElement(QStringLiteral("frame"));
    
    QStringList points;
    Q_FOREACH(const QPoint& point, d->points) {
        points << QStringLiteral("%1,%2").arg(QString::number(point.x())).arg(QString::number(point.y()));
    }
    writer->writeAttribute(QStringLiteral("points"), points.join(' '));
    if(!d->bgcolor.isEmpty()) {
        writer->writeAttribute(QStringLiteral("bgcolor"), d->bgcolor);
    }
    writer->writeEndElement();
}

bool Frame::fromXml(QXmlStreamReader *xmlReader)
{
    setBgcolor(xmlReader->attributes().value(QStringLiteral("bgcolor")).toString());

    QStringList points = xmlReader->attributes().value(QStringLiteral("points")).toString().split(' ');

    Q_FOREACH(const QString& point, points) {
        QStringList elements = point.split(',');
        if(elements.length() == 2)
        {
            addPoint(QPoint(elements.at(0).toInt(), elements.at(1).toInt()));
        }
        else
        {
            qWarning() << "Failed to construct one of the points for a frame. Attempted to handle the point" << point << "in the data" << points;
            return false;
        }
    }

    if (xmlReader->hasError()) {
        qWarning() << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qDebug() << Q_FUNC_INFO << "Created a frame with " << points.count() << "points";

    return !xmlReader->hasError();
}

QList<QPoint> Frame::points() const
{
    return d->points;
}

QPoint Frame::point(int index) const
{
    return d->points.at(index);
}

int Frame::pointIndex(const QPoint& point) const
{
    return d->points.indexOf(point);
}

void Frame::addPoint(const QPoint& point, int index)
{
    if(index > -1 && d->points.count() < index) {
        d->points.insert(index, point);
    }
    else {
        d->points.append(point);
    }
}

void Frame::removePoint(const QPoint& point)
{
    d->points.removeAll(point);
}

bool Frame::swapPoints(const QPoint& swapThis, const QPoint& withThis)
{
    int index1 = d->points.indexOf(swapThis);
    int index2 = d->points.indexOf(withThis);
    if(index1 > -1 && index2 > -1) {
        d->points.swap(index1, index2);
        return true;
    }
    return false;
}

QString Frame::bgcolor() const
{
    return d->bgcolor;
}

void Frame::setBgcolor(const QString& newColor)
{
    d->bgcolor = newColor;
}
