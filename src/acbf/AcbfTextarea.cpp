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

#include "AcbfTextarea.h"

#include <QXmlStreamReader>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class Textarea::Private
{
public:
    Private()
        : textRotation(0)
        , type ("speech")
        , inverted(false)
        , transparent(false)
    {}
    Textlayer* parent;
    QString id;
    QString bgcolor;
    QList<QPoint> points;
    int textRotation;
    QString type;
    bool inverted;
    bool transparent;
    QStringList paragraphs;
};

Textarea::Textarea(Textlayer* parent)
    : InternalReferenceObject(InternalReferenceObject::ReferenceOriginAndTarget, parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Textarea*>("Textarea*");
    Q_UNUSED(typeId);
    d->parent = parent;
    connect(this, &Textarea::pointCountChanged, this, &Textarea::boundsChanged);

    connect(this, &Textarea::idChanged, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Textarea::bgcolorChanged, &InternalReferenceObject::propertyDataChanged);
    // Don't forward pointsCountChanged, as boundsChanged already fires from that
    connect(this, &Textarea::boundsChanged, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Textarea::textRotationChanged, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Textarea::typeChanged, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Textarea::invertedChanged, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Textarea::transparentChanged, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Textarea::paragraphsChanged, &InternalReferenceObject::propertyDataChanged);
}

Textarea::~Textarea() = default;

void Textarea::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement(QStringLiteral("text-area"));
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
    if(d->textRotation != 0) {
        writer->writeAttribute(QStringLiteral("text-rotation"), QString::number(d->textRotation));
    }
    if(!d->type.isEmpty()) {
        writer->writeAttribute(QStringLiteral("type"), d->type);
    }
    if(d->inverted) {
        // because the default is false, no need to write it otherwise...
        writer->writeAttribute(QStringLiteral("inverted"), QStringLiteral("true"));
    }
    if(d->transparent) {
        // because the default is false, no need to write it otherwise...
        writer->writeAttribute(QStringLiteral("transparent"), QStringLiteral("true"));
    }

    for(const QString& paragraph : d->paragraphs) {
        writer->writeStartElement(QStringLiteral("p"));
        writer->writeCharacters("");  // to ensure we close the tag correctly and don't end up with a <p />
        writer->device()->write(paragraph.toUtf8().constData(), paragraph.toUtf8().length());
        writer->writeEndElement();
    }

    writer->writeEndElement();
}

bool Textarea::fromXml(QXmlStreamReader *xmlReader, const QString& xmlData)
{
    setId(xmlReader->attributes().value(QStringLiteral("id")).toString());
    setBgcolor(xmlReader->attributes().value(QStringLiteral("bgcolor")).toString());
    setTextRotation(xmlReader->attributes().value(QStringLiteral("text-rotation")).toInt());
    setType(xmlReader->attributes().value(QStringLiteral("type")).toString());
    setInverted(xmlReader->attributes().value(QStringLiteral("inverted")).toString().toLower() == QStringLiteral("true"));
    setTransparent(xmlReader->attributes().value(QStringLiteral("transparent")).toString().toLower() == QStringLiteral("true"));

    QVector<QStringRef> points = xmlReader->attributes().value(QStringLiteral("points")).split(' ');
    for(QStringRef point : points) {
        QVector<QStringRef> elements = point.split(',');
        if(elements.length() == 2)
        {
            addPoint(QPoint(elements.at(0).toInt(), elements.at(1).toInt()));
        }
        else
        {
            qCWarning(ACBF_LOG) << "Failed to construct one of the points for a text-area. Attempted to handle the point" << point << "in the data" << points;
            return false;
        }
    }

    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("p"))
        {
            int startPoint = xmlReader->characterOffset();
            int endPoint{startPoint};
            while(xmlReader->readNext()) {
                if (xmlReader->isEndElement() && xmlReader->name() == QStringLiteral("p")) {
                    endPoint = xmlReader->characterOffset();
                    break;
                }
            }
            d->paragraphs.append(xmlData.mid(startPoint, endPoint - startPoint - 4));
        }
        else
        {
            qCWarning(ACBF_LOG) << Q_FUNC_INFO << "currently unsupported subsection in text-area:" << xmlReader->name();
            xmlReader->skipCurrentElement();
        }
    }
    if (xmlReader->hasError()) {
        qCWarning(ACBF_LOG) << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created a text area of type" << type() << "with the paragraphs" << d->paragraphs;
    return !xmlReader->hasError();
}

QString Textarea::id() const
{
    return d->id;
}

void Textarea::setId(const QString& newId)
{
    if (d->id != newId) {
        d->id = newId;
        Q_EMIT idChanged();
    }
}

QVariantList Textarea::points() const
{
    QVariantList list;
    for (const QPoint& point : d->points) {
        list << point;
    }
    return list;
}

QPoint Textarea::point(int index) const
{
    if (index < 0 || index >= d->points.count()) {
        return QPoint{};
    }
    return d->points.at(index);
}

int Textarea::pointIndex(const QPoint& point) const
{
    return d->points.indexOf(point);
}

void Textarea::addPoint(const QPoint& point, int index)
{
    if(index > -1 && d->points.count() < index) {
        d->points.insert(index, point);
    }
    else {
        d->points.append(point);
    }
    emit pointCountChanged();
}

void Textarea::removePoint(const QPoint& point)
{
    d->points.removeAll(point);
    emit pointCountChanged();
}

bool Textarea::swapPoints(const QPoint& swapThis, const QPoint& withThis)
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
void Textarea::setPointsFromRect(const QPoint &topLeft, const QPoint &bottomRight)
{
    QRect rect(topLeft, bottomRight);
    d->points.clear();
    d->points.append(rect.topLeft());
    d->points.append(rect.topRight());
    d->points.append(rect.bottomRight());
    d->points.append(rect.bottomLeft());
    emit pointCountChanged();
}

int Textarea::pointCount() const
{
    return d->points.size();
}

QRect Textarea::bounds() const
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

QString Textarea::bgcolor() const
{
    if (d->bgcolor.isEmpty()) {
        Textlayer* layer = qobject_cast<Textlayer*>(parent());
        if (layer) {
            return layer->bgcolor();
        }
    }
    return d->bgcolor;
}

void Textarea::setBgcolor(const QString& newColor)
{
    d->bgcolor = newColor;
    emit bgcolorChanged();
}

void Textarea::setTextRotation(int rotation)
{
    d->textRotation = rotation;
    emit textRotationChanged();
}

int Textarea::textRotation() const
{
    return d->textRotation;
}

QString Textarea::type() const
{
    return d->type.isEmpty() ? "speech" : d->type;
}

void Textarea::setType(const QString& type)
{
    d->type = type;
    emit typeChanged();
}

QStringList Textarea::availableTypes()
{
    return {
        QStringLiteral("speech"), // (character is speaking, text is centered)
        QStringLiteral("commentary"), // (accompanying commentary, text is aligned to left)
        QStringLiteral("formal"), // (text alignment is - justify)
        QStringLiteral("letter"), // (rendered in handwriting font)
        QStringLiteral("code"), // (rendered in monospace font)
        QStringLiteral("heading"), // (e.g. chapter title)
        QStringLiteral("audio"), // (speech emanating from an audio device, e.g., television or radio speaker, telephone, walkie-talkie, etc.)
        QStringLiteral("thought"),
        QStringLiteral("sign"), // (any kind of sign/writing, text is centered)
        QStringLiteral("sound"), // /new in 1.2/
    };
}

bool Textarea::inverted() const
{
    return d->inverted;
}

void Textarea::setInverted(bool inverted)
{
    d->inverted = inverted;
    emit invertedChanged();
}

bool Textarea::transparent() const
{
    return d->transparent;
}

void Textarea::setTransparent(bool transparent)
{
    d->transparent = transparent;
    emit transparentChanged();
}

QStringList Textarea::paragraphs() const
{
    return d->paragraphs;
}

void Textarea::setParagraphs(const QStringList& paragraphs)
{
    d->paragraphs = paragraphs;
    emit paragraphsChanged();
}

int Textarea::localIndex()
{
    if (d->parent) {
        return d->parent->textAreaIndex(this);
    }
    return -1;
}
