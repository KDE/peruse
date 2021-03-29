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
#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class Frame::Private
{
public:
    Private()
    {}
    QString id;
    QString bgcolor;
    QList<QPoint> points;
};

Frame::Frame(Page* parent)
    : InternalReferenceObject(InternalReferenceObject::ReferenceTarget, parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Frame*>("Frame*");
    Q_UNUSED(typeId);
    connect(this, &Frame::pointCountChanged, this, &Frame::boundsChanged);

    connect(this, &Frame::idChanged, this, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Frame::bgcolorChanged, this, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Frame::boundsChanged, this, &InternalReferenceObject::propertyDataChanged);
}

Frame::~Frame() = default;

void Frame::toXml(QXmlStreamWriter* writer) {
    writer->writeStartElement(QStringLiteral("frame"));
    if(!d->id.isEmpty()) {
        writer->writeAttribute(QStringLiteral("id"), id());
    }

    QStringList points;
    for(const QPoint& point : d->points) {
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
    setId(xmlReader->attributes().value(QStringLiteral("id")).toString());
    setBgcolor(xmlReader->attributes().value(QStringLiteral("bgcolor")).toString());

    QVector<QStringRef> points = xmlReader->attributes().value(QStringLiteral("points")).split(' ');
    for(QStringRef point : points) {
        QVector<QStringRef> elements = point.split(',');
        if(elements.length() == 2)
        {
            addPoint(QPoint(elements.at(0).toInt(), elements.at(1).toInt()));
        }
        else
        {
            qCWarning(ACBF_LOG) << "Failed to construct one of the points for a frame. Attempted to handle the point" << point << "in the data" << points;
            return false;
        }
    }

    if (xmlReader->hasError()) {
        qCWarning(ACBF_LOG) << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created a frame with " << points.count() << "points";

    return !xmlReader->hasError();
}

QString Frame::id() const
{
    return d->id;
}

void Frame::setId(const QString& newId)
{
    if (d->id != newId) {
        d->id = newId;
        Q_EMIT idChanged();
    }
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
    emit pointCountChanged();
}

void Frame::removePoint(const QPoint& point)
{
    d->points.removeAll(point);
    emit pointCountChanged();
}

bool Frame::swapPoints(const QPoint& swapThis, const QPoint& withThis)
{
    int index1 = d->points.indexOf(swapThis);
    int index2 = d->points.indexOf(withThis);
    if(index1 > -1 && index2 > -1) {
        d->points.swapItemsAt(index1, index2);
        emit pointCountChanged();
        return true;
    }
    return false;
}

void Frame::setPointsFromRect(const QPoint &topLeft, const QPoint &bottomRight)
{
    QRect rect(topLeft, bottomRight);
    d->points.clear();
    d->points.append(rect.topLeft());
    d->points.append(rect.topRight());
    d->points.append(rect.bottomRight());
    d->points.append(rect.bottomLeft());
    emit pointCountChanged();
}

int Frame::pointCount() const
{
    return d->points.size();
}

QRect Frame::bounds() const
{
    // Would use QPolygon here, but that requires including QTGUI.
    if (d->points.size()==0) {
        return QRect();
    }
    QRect rect(d->points.at(0), d->points.at(1));
    for (int i = 2; i < d->points.size(); i++) {
        QPoint p = d->points.at(i);
        if (rect.left() > p.x()) {
            rect.setLeft(p.x());
        }
        if (rect.right() < p.x()) {
            rect.setRight(p.x());
        }
        if (rect.bottom() < p.y()) {
            rect.setBottom(p.y());
        }
        if (rect.top() > p.y()) {
            rect.setTop(p.y());
        }
    }
    return rect;
}

QString Frame::bgcolor() const
{
    return d->bgcolor;
}

void Frame::setBgcolor(const QString& newColor)
{
    d->bgcolor = newColor;
    emit bgcolorChanged();
}

int AdvancedComicBookFormat::Frame::localIndex()
{
    int idx{-1};
    Page* page = qobject_cast<Page*>(parent());
    if (page) {
        idx = page->frameIndex(this);
    }
    return idx;
}
