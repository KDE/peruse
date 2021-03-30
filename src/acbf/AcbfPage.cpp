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

#include "AcbfPage.h"
#include "AcbfBody.h"
#include "AcbfFrame.h"
#include "AcbfJump.h"
#include "AcbfTextarea.h"
#include "AcbfTextlayer.h"

#include <QHash>
#include <QXmlStreamReader>
#include <QTimer>
#include <QtGlobal>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class Page::Private
{
public:
    Private()
        : isCoverPage(false)
    {
        jumpsUpdateTimer.setSingleShot(true);
        jumpsUpdateTimer.setInterval(0);
    }
    QString id;
    QString bgcolor;
    QString transition;
    QHash<QString, QString> title;
    QString imageHref;
    QHash<QString, Textlayer*> textLayers;
    QList<Frame*> frames;
    QList<Jump*> jumps;
    QTimer jumpsUpdateTimer;
    bool isCoverPage;
};

Page::Page(Document* parent)
    : InternalReferenceObject(InternalReferenceObject::ReferenceTarget, parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<Page*>("Page*");
    Q_UNUSED(typeId);

    QObject::connect(&d->jumpsUpdateTimer, &QTimer::timeout, &d->jumpsUpdateTimer, [this]() {
        Q_EMIT jumpsChanged();
    });

    connect(this, &Page::idChanged, this, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Page::bgcolorChanged, this, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Page::transitionChanged, this, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Page::imageHrefChanged, this, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Page::textLayerLanguagesChanged, this, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Page::framePointStringsChanged, this, &InternalReferenceObject::propertyDataChanged);
    connect(this, &Page::jumpsChanged, this, &InternalReferenceObject::propertyDataChanged);
}

Page::~Page() = default;

void Page::toXml(QXmlStreamWriter* writer)
{
    if(d->isCoverPage) {
        writer->writeStartElement(QStringLiteral("coverpage"));
    }
    else {
        writer->writeStartElement(QStringLiteral("page"));
    }
    if(!d->id.isEmpty()) {
        writer->writeAttribute(QStringLiteral("id"), id());
    }

    if(!d->bgcolor.isEmpty()) {
        writer->writeAttribute(QStringLiteral("bgcolor"), d->bgcolor);
    }
    if(!d->transition.isEmpty()) {
        writer->writeAttribute(QStringLiteral("transition"), d->transition);
    }

    QHashIterator<QString, QString> titles(d->title);
    while(titles.hasNext()) {
        titles.next();
        writer->writeStartElement(QStringLiteral("title"));
        writer->writeAttribute(QStringLiteral("lang"), titles.key());
        writer->writeCharacters(titles.value());
        writer->writeEndElement();
    }

    writer->writeStartElement(QStringLiteral("image"));
    writer->writeAttribute(QStringLiteral("href"), d->imageHref);
    writer->writeEndElement();

    for(Textlayer* layer : d->textLayers.values()) {
        layer->toXml(writer);
    }

     for(Frame* frame : d->frames) {
         frame->toXml(writer);
     }

     for(Jump* jump : d->jumps) {
         jump->toXml(writer);
     }

    writer->writeEndElement();
}

bool Page::fromXml(QXmlStreamReader *xmlReader, const QString& xmlData)
{
    setId(xmlReader->attributes().value(QStringLiteral("id")).toString());
    setBgcolor(xmlReader->attributes().value(QStringLiteral("bgcolor")).toString());
    setTransition(xmlReader->attributes().value(QStringLiteral("transition")).toString());
    while(xmlReader->readNextStartElement())
    {
        if(xmlReader->name() == QStringLiteral("title"))
        {
            d->title[xmlReader->attributes().value(QStringLiteral("lang")).toString()] = xmlReader->readElementText();
        }
        else if(xmlReader->name() == QStringLiteral("image"))
        {
            /**
             * There are some acbf files out there that have backslashes in their
             * image href. This is probably a mistake from windows users, but not proper XML.
             * We should thus replace those with forward slashes so the image can be loaded.
             */
            QString href = xmlReader->attributes().value(QStringLiteral("href")).toString();
            setImageHref(href.replace(QStringLiteral("\\"), QStringLiteral("/"))); 
            xmlReader->skipCurrentElement();
        }
        else if(xmlReader->name() == QStringLiteral("text-layer"))
        {
            Textlayer* newLayer = new Textlayer(this);
            if(!newLayer->fromXml(xmlReader, xmlData)) {
                return false;
            }
            d->textLayers[newLayer->language()] = newLayer;
        }
        else if(xmlReader->name() == QStringLiteral("frame"))
        {
            Frame* newFrame = new Frame(this);
            if(!newFrame->fromXml(xmlReader)) {
                return false;
            }
            d->frames.append(newFrame);

            // Frames have no child elements, so we need to force the reader to go to the next one.
            xmlReader->readNext();
        }
         else if(xmlReader->name() == QStringLiteral("jump"))
         {
             Jump* newJump = new Jump(this);
             if(!newJump->fromXml(xmlReader)) {
                 return false;
             }
             // Jumps have no child elements, so we need to force the reader to go to the next one.
             addJump(newJump, -1);
             xmlReader->readNext();
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
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created page for image" << d->imageHref;
    return !xmlReader->hasError();
}

QString Page::id() const
{
    return d->id;
}

void Page::setId(const QString& newId)
{
    if (d->id != newId) {
        d->id = newId;
        Q_EMIT idChanged();
    }
}

QString Page::bgcolor() const
{
    if (d->bgcolor.isEmpty()) {
        Document* document = qobject_cast<Document*>(parent());
        if (document && document->body()) {
            return document->body()->bgcolor();
        }
    }
    return d->bgcolor;
}

void Page::setBgcolor(const QString& newColor)
{
    d->bgcolor = newColor;
    emit bgcolorChanged();
}

QString Page::transition() const
{
    return d->transition;
}

void Page::setTransition(const QString& transition)
{
    d->transition = transition;
    emit transitionChanged();
}

QStringList Page::availableTransitions()
{
    return {
        QStringLiteral("fade"), // (old page fades out into background, then new page fades in)
        QStringLiteral("blend"), // (new page blends in the image while old page blends out)
        QStringLiteral("scroll_right"), // (screen scrolls to the right to a new page; reversed behavior applies when moving to previous page)
        QStringLiteral("scroll_down"), // (screen scrolls down to a new page; reversed behavior applies when moving to previous page)
        QStringLiteral("none") // (no transition animation happens)
    };
}

QStringList Page::titleForAllLanguages() const
{
    return d->title.values();
}

QString Page::title(const QString& language) const
{
    if (d->title.count()==0) {
        return "";
    }
    if (!d->title.keys().contains(language)) {
        d->title.values().at(0);
    }

    QString title = d->title.value(language);

    if (title.isEmpty()) {
        title = d->title.values().at(0);
    }

    return title;
}

void Page::setTitle(const QString& title, const QString& language)
{
    if(title.isEmpty())
    {
        d->title.remove(language);
    }
    else
    {
        d->title[language] = title;
    }
    emit titlesChanged();
}

QString Page::imageHref() const
{
    return d->imageHref;
}

void Page::setImageHref(const QString& imageHref)
{
    d->imageHref = imageHref;
}

QList<Textlayer *> Page::textLayersForAllLanguages() const
{
    return d->textLayers.values();
}

Textlayer * Page::textLayer(const QString& language) const
{
    if (!d->textLayers.keys().contains("") && language == QString() && d->textLayers.count() > 0) {
        return d->textLayers.values().at(0);
    }
    return d->textLayers.value(language);
}

void Page::setTextLayer(Textlayer* textlayer, const QString& language)
{
    if(textlayer)
    {
        d->textLayers[language] = textlayer;
        Q_EMIT textLayerAdded(textlayer);
    }
    else
    {
        Textlayer* layer = d->textLayers.take(language);
        if (layer) {
            layer->deleteLater();
        }
    }
    emit textLayerLanguagesChanged();
}

void Page::addTextLayer(const QString &language)
{
    Textlayer* textLayer = new Textlayer(this);
    textLayer->setLanguage(language);
    setTextLayer(textLayer, language);
}

void Page::removeTextLayer(const QString &language)
{
    setTextLayer(nullptr, language);
}

void Page::duplicateTextLayer(const QString &languageFrom, const QString &languageTo)
{
    Textlayer* to = new Textlayer(this);
    to->setLanguage(languageTo);
    if (d->textLayers[languageFrom]) {
        Textlayer* from = d->textLayers[languageFrom];
        to->setBgcolor(from->bgcolor());
        for (int i=0; i<from->textareaPointStrings().size(); i++) {
            to->addTextarea(i);
            to->textarea(i)->setBgcolor(from->textarea(i)->bgcolor());
            to->textarea(i)->setInverted(from->textarea(i)->inverted());
            to->textarea(i)->setTransparent(from->textarea(i)->transparent());
            to->textarea(i)->setTextRotation(from->textarea(i)->textRotation());
            to->textarea(i)->setType(from->textarea(i)->type());
            to->textarea(i)->setParagraphs(from->textarea(i)->paragraphs());
            for (int p=0; p<from->textarea(i)->pointCount(); p++) {
                to->textarea(i)->addPoint(from->textarea(i)->point(p));
            }
        }
    }
    setTextLayer(to);
}

QStringList Page::textLayerLanguages() const
{
    if (d->textLayers.isEmpty()) {
        return QStringList();
    }
    return d->textLayers.keys();
}

QList<Frame *> Page::frames() const
{
    return d->frames;
}

Frame * Page::frame(int index) const
{
    return d->frames.at(index);
}

int Page::frameIndex(Frame* frame) const
{
    return d->frames.indexOf(frame);
}

void Page::addFrame(Frame* frame, int index)
{
    if(index > -1 && d->frames.count() < index) {
        d->frames.insert(index, frame);
    }
    else {
        d->frames.append(frame);
    }
    Q_EMIT frameAdded(frame);
    emit framePointStringsChanged();
}

void Page::removeFrame(Frame* frame)
{
    d->frames.removeAll(frame);
    emit framePointStringsChanged();
}

void Page::removeFrame(int index)
{
    removeFrame(frame(index));
}

void Page::addFrame(int index)
{
    Frame* frame = new Frame(this);
    addFrame(frame, index);
}

bool Page::swapFrames(int swapThis, int withThis)
{
    if(swapThis > -1 && withThis > -1) {
        d->frames.swapItemsAt(swapThis, withThis);
        emit framePointStringsChanged();
        return true;
    }
    return false;
}

QStringList Page::framePointStrings()
{
    QStringList frameList;
    for (int i=0; i<d->frames.size(); i++) {
        QStringList framePoints;
        for (int p=0; p< frame(i)->pointCount(); p++) {
            framePoints.append(QString("%1,%2").arg(frame(i)->point(p).x()).arg(frame(i)->point(p).y()));
        }
        frameList.append(framePoints.join(" "));
    }

    return frameList;
}

QObjectList Page::jumps() const
{
    QObjectList jumpsList;

    for(int i = 0; i < d->jumps.size(); i++) {
        jumpsList.append(d->jumps.at(i));
    }

    return jumpsList;
}

Jump* Page::jump(int index) const
{
    return d->jumps.at(index);
}

int Page::jumpIndex(Jump* jump) const
{
    return d->jumps.indexOf(jump);
}

void Page::addJump(Jump* jump, int index)
{
    QObject::connect(jump, &Jump::pointCountChanged, &d->jumpsUpdateTimer, QOverload<>::of(&QTimer::start));
    QObject::connect(jump, &Jump::boundsChanged, &d->jumpsUpdateTimer, QOverload<>::of(&QTimer::start));
    QObject::connect(jump, &Jump::pageIndexChanged, &d->jumpsUpdateTimer, QOverload<>::of(&QTimer::start));
    QObject::connect(jump, &QObject::destroyed, &d->jumpsUpdateTimer, [this, jump]() {
        d->jumps.removeAll(jump);
        d->jumpsUpdateTimer.start();
    });

    if(index > -1 && index < d->jumps.count()) {
        d->jumps.insert(index, jump);
    } else {
        d->jumps.append(jump);
    }
    Q_EMIT jumpAdded(jump);
    emit jumpsChanged();
}

void Page::addJump(int pageIndex, int index)
{
    Jump* jump = new Jump(this);
    jump->setPageIndex(pageIndex);
    addJump(jump, index);
}

void Page::removeJump(Jump* jump)
{
    d->jumps.removeAll(jump);
    emit jumpsChanged();
}

void Page::removeJump(int index)
{
    removeJump(jump(index));
}

bool Page::swapJumps(int swapThis, int withThis)
{
    if(swapThis > -1 && withThis > -1) {
        d->jumps.swapItemsAt(swapThis, withThis);
        emit jumpsChanged();
        return true;
    }
    return false;
}

bool Page::isCoverPage() const
{
    return d->isCoverPage;
}

void Page::setIsCoverPage(bool isCoverPage)
{
    d->isCoverPage = isCoverPage;
}

int Page::localIndex()
{
    Document* document = qobject_cast<Document*>(parent());
    if (document && document->body()) {
        return document->body()->pageIndex(this);
    }
    return -1;
}
