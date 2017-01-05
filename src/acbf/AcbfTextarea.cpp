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

#include <QDebug>
#include <QXmlStreamReader>

using namespace AdvancedComicBookFormat;

class Textarea::Private
{
public:
    Private()
        : textRotation(0)
    {}
    QString bgcolor;
    QList<QPoint> points;
    int textRotation;
    QString type;
    bool inverted;
    bool transparent;
    QStringList paragraphs;
};

Textarea::Textarea(Textlayer* parent)
    : QObject(parent)
    , d(new Private)
{
}

Textarea::~Textarea()
{
    delete d;
}

void Textarea::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement("text-area");

    QStringList points;
    Q_FOREACH(const QPoint& point, d->points) {
        points << QString("%1,%2").arg(QString::number(point.x())).arg(QString::number(point.y()));
    }
    writer->writeAttribute("points", points.join(' '));

    if(!d->bgcolor.isEmpty()) {
        writer->writeAttribute("bgcolor", d->bgcolor);
    }
    if(d->textRotation != 0) {
        writer->writeAttribute("text-rotation", QString::number(d->textRotation));
    }
    if(!d->type.isEmpty()) {
        writer->writeAttribute("type", d->type);
    }
    if(d->inverted) {
        // because the default is false, no need to write it otherwise...
        writer->writeAttribute("inverted", "true");
    }
    if(d->transparent) {
        // because the default is false, no need to write it otherwise...
        writer->writeAttribute("transparent", "true");
    }

    Q_FOREACH(const QString& paragraph, d->paragraphs) {
        writer->writeStartElement("p");
        writer->writeCharacters(paragraph);
        writer->writeEndElement();
    }

    writer->writeEndElement();
}

bool Textarea::fromXml(QXmlStreamReader *xmlReader)
{
    setBgcolor(xmlReader->attributes().value("bgcolor").toString());
    setTextRotation(xmlReader->attributes().value("text-rotation").toInt());
    setType(xmlReader->attributes().value("type").toString());
    setInverted(xmlReader->attributes().value("inverted").toString().toLower() == "true");
    setTransparent(xmlReader->attributes().value("transparent").toString().toLower() == "true");

    QStringList points = xmlReader->attributes().value("points").toString().split(' ');
    Q_FOREACH(const QString& point, points) {
        QStringList elements = point.split(',');
        if(elements.length() == 2)
        {
            addPoint(QPoint(elements.at(0).toInt(), elements.at(1).toInt()));
        }
        else
        {
            qWarning() << "Failed to construct one of the points for a text-area. Attempted to handle the point" << point << "in the data" << points;
            return false;
        }
    }

    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == "p")
        {
            d->paragraphs.append(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "currently unsupported subsection in text-area:" << xmlReader->name();
            xmlReader->skipCurrentElement();
        }
    }
    if (xmlReader->hasError()) {
        qWarning() << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qDebug() << Q_FUNC_INFO << "Created a text area of type" << type();
    return !xmlReader->hasError();
}

QList<QPoint> Textarea::points()
{
    return d->points;
}

QPoint Textarea::point(int index)
{
    return d->points.at(index);
}

int Textarea::pointIndex(QPoint point)
{
    return d->points.indexOf(point);
}

void Textarea::addPoint(QPoint point, int index)
{
    if(index > -1 && d->points.count() < index) {
        d->points.insert(index, point);
    }
    else {
        d->points.append(point);
    }
}

void Textarea::removePoint(QPoint point)
{
    d->points.removeAll(point);
}

bool Textarea::swapPoints(QPoint swapThis, QPoint withThis)
{
    int index1 = d->points.indexOf(swapThis);
    int index2 = d->points.indexOf(withThis);
    if(index1 > -1 && index2 > -1) {
        d->points.swap(index1, index2);
        return true;
    }
    return false;
}

QString Textarea::bgcolor()
{
    return d->bgcolor;
}

void Textarea::setBgcolor(QString newColor)
{
    d->bgcolor = newColor;
}

void Textarea::setTextRotation(int rotation)
{
    d->textRotation = rotation;
}

int Textarea::textRotation()
{
    return d->textRotation;
}

QString Textarea::type()
{
    return d->type.isEmpty() ? "speech" : d->type;
}

void Textarea::setType(QString type)
{
    d->type = type;
}

QStringList Textarea::availableTypes()
{
    QStringList types;
    types << "speech"; // (character is speaking, text is centered)
    types << "commentary"; // (accompanying commentary, text is aligned to left)
    types << "formal"; // (text alignment is - justify)
    types << "letter"; // (rendered in handwriting font)
    types << "code"; // (rendered in monospace font)
    types << "heading"; // (e.g. chapter title)
    types << "audio"; // (speech emanating from an audio device, e.g., television or radio speaker, telephone, walkie-talkie, etc.)
    types << "thought";
    types << "sign"; // (any kind of sign/writing, text is centered)
    return types;
}

bool Textarea::inverted()
{
    return d->inverted;
}

void Textarea::setInverted(bool inverted)
{
    d->inverted = inverted;
}

bool Textarea::transparent()
{
    return d->transparent;
}

void Textarea::setTransparent(bool transparent)
{
    d->transparent = transparent;
}

QStringList Textarea::paragraphs()
{
    return d->paragraphs;
}

void Textarea::setParagraphs(QStringList paragraphs)
{
    d->paragraphs = paragraphs;
}
