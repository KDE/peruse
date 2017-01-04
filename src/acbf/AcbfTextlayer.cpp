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

#include "AcbfTextlayer.h"
#include "AcbfTextarea.h"

#include <QDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

using namespace AdvancedComicBookFormat;

class Textlayer::Private
{
public:
    Private() {}
    QString language;
    QString bgcolor;
    QList<Textarea*> textareas;
};

Textlayer::Textlayer(Page* parent)
    : QObject(parent)
    , d(new Private)
{
}

Textlayer::~Textlayer()
{
    delete d;
}

void Textlayer::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement("text-layer");
    if(!d->language.isEmpty()) {
        writer->writeAttribute("lang", d->language);
    }
    if(!d->bgcolor.isEmpty()) {
        writer->writeAttribute("bgcolor", d->bgcolor);
    }

    Q_FOREACH(Textarea* area, d->textareas) {
        area->toXml(writer);
    }

    writer->writeEndElement();
}

bool Textlayer::fromXml(QXmlStreamReader *xmlReader)
{
    setBgcolor(xmlReader->attributes().value("bgcolor").toString());
    setLanguage(xmlReader->attributes().value("lang").toString());
    while(xmlReader->readNext())
    {
        if(xmlReader->tokenType() == QXmlStreamReader::EndElement) {
            if(xmlReader->name() == "text-layer") {
                break;
            }
            else {
                continue;
            }
        }
        if(xmlReader->tokenType() == QXmlStreamReader::Characters) {
            continue;
        }
        if(xmlReader->name() == "text-area")
        {
            Textarea* newArea = new Textarea(this);
            if(!newArea->fromXml(xmlReader)) {
                return false;
            }
            d->textareas.append(newArea);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "currently unsupported subsection:" << xmlReader->name();
            xmlReader->skipCurrentElement();
        }
    }
    if (xmlReader->hasError()) {
        qWarning() << Q_FUNC_INFO << "Failed to read ACBF XML document at token" << xmlReader->name() << "(" << xmlReader->lineNumber() << ":" << xmlReader->columnNumber() << ") The reported error was:" << xmlReader->errorString();
    }
    qDebug() << Q_FUNC_INFO << "Created a text layer with" << d->textareas.count() << "text areas";
    return !xmlReader->hasError();
}

QString Textlayer::language()
{
    return d->language;
}

void Textlayer::setLanguage(QString language)
{
    d->language = language;
}

QString Textlayer::bgcolor()
{
    return d->bgcolor;
}

void Textlayer::setBgcolor(QString newColor)
{
    d->bgcolor = newColor;
}

QList<Textarea *> Textlayer::textareas()
{
    return d->textareas;
}

Textarea * Textlayer::textarea(int index)
{
    return d->textareas.at(index);
}

int Textlayer::textareaIndex(Textarea* textarea)
{
    return d->textareas.indexOf(textarea);
}

void Textlayer::addTextarea(Textarea* textarea, int index)
{
    if(index > -1 && d->textareas.count() < index) {
        d->textareas.insert(index, textarea);
    }
    else {
        d->textareas.append(textarea);
    }
}

void Textlayer::removeTextarea(Textarea* textarea)
{
    d->textareas.removeAll(textarea);
}

bool Textlayer::swapTextareas(Textarea* swapThis, Textarea* withThis)
{
    int index1 = d->textareas.indexOf(swapThis);
    int index2 = d->textareas.indexOf(withThis);
    if(index1 > -1 && index2 > -1) {
        d->textareas.swap(index1, index2);
        return true;
    }
    return false;
}
