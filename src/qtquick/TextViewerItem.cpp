/*
 * Copyright (C) 2021 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "TextViewerItem.h"

#include "AcbfStyle.h"

#include <QCursor>
#include <QFontMetrics>
#include <qmath.h>
#include <QTimer>
#include <private/qquicktextnode_p.h>

class TextViewerItem::Private {
public:
    Private(TextViewerItem* qq)
        : q(qq)
    {
        throttle = new QTimer(qq);
        throttle->setInterval(1);
        throttle->setSingleShot(true);
        QObject::connect(throttle, &QTimer::timeout, qq, &QQuickItem::polish);
    }
    ~Private() {
        qDeleteAll(layouts);
    }
    TextViewerItem* q;
    QTimer* throttle{nullptr};
    QStringList paragraphs;
    QList<QPoint> shape;
    QPoint shapeOffset{0, 0};
    double shapeMultiplier{1};
    AdvancedComicBookFormat::Style* style{nullptr};
    QString fontFamily;
    int margin{2};

    QPolygonF shapePolygon;
    QFont font;
    QStringList internalParagraphs;
    QList<QVector<QTextLayout::FormatRange> > formats;
    // First is the index in formats, and second is the index is the list at that index
    QHash<QPair<int,int>, QList<QRectF>> anchorRects;
    QList<QTextLayout*> layouts;

    // State tracker for making sure that if the user moves outside of the anchor they originally
    // clicked on, we don't actually suggest it was clicked
    QPair<int, int> clickedAnchor{-1, -1};
    QString hoveredLink;

    QString fromHtmlEscaped(QString html) {
        html.replace("&quot;", "\"", Qt::CaseInsensitive);
        html.replace("&gt;", ">", Qt::CaseInsensitive);
        html.replace("&lt;", "<", Qt::CaseInsensitive);
        html.replace("&amp;", "&", Qt::CaseInsensitive);
        return html;
    }

    void adjustFormats() {
        internalParagraphs.clear();
        formats.clear();

        font = QFont(fontFamily);

        if (style) {
            const QString fontStyle(style->fontStyle().toLower());
            if (fontStyle == QStringLiteral("normal")) {
                font.setStyle(QFont::StyleNormal);
            } else if (fontStyle == QStringLiteral("italic")) {
                font.setStyle(QFont::StyleItalic);
            } else if (fontStyle == QStringLiteral("oblique")) {
                font.setStyle(QFont::StyleOblique);
            }

            font.setItalic(style->fontStyle().toLower() == QStringLiteral("italic"));
            const QString fontWeight(style->fontWeight().toLower());
            if (fontWeight == QStringLiteral("normal")) {
                font.setWeight(400);
            } else if (fontWeight == QStringLiteral("bold")) {
                font.setWeight(700);
            } else if (fontWeight == QStringLiteral("bolder")) {
                font.setWeight(900);
            } else if (fontWeight == QStringLiteral("lighter")) {
                font.setWeight(100);
            } else if (QString::number(fontWeight.toInt()) == fontWeight) {
                font.setWeight(fontWeight.toInt());
            }
            const QString fontStretch(style->fontStretch().toLower());
            if (fontStretch == QStringLiteral("")) {
            } else if (fontStretch == QStringLiteral("ultra-condensed")) {
                font.setStretch(QFont::UltraCondensed);
            } else if (fontStretch == QStringLiteral("extra-condensed")) {
                font.setStretch(QFont::ExtraCondensed);
            } else if (fontStretch == QStringLiteral("condensed")) {
                font.setStretch(QFont::Condensed);
            } else if (fontStretch == QStringLiteral("semi-condensed")) {
                font.setStretch(QFont::SemiCondensed);
            } else if (fontStretch == QStringLiteral("normal")) {
                font.setStretch(QFont::Unstretched);
            } else if (fontStretch == QStringLiteral("semi-expanded")) {
                font.setStretch(QFont::SemiExpanded);
            } else if (fontStretch == QStringLiteral("expanded")) {
                font.setStretch(QFont::Expanded);
            } else if (fontStretch == QStringLiteral("extra-expanded")) {
                font.setStretch(QFont::ExtraExpanded);
            } else if (fontStretch == QStringLiteral("ultra-expanded")) {
                font.setStretch(QFont::UltraExpanded);
            } else if (QString::number(fontStretch.toInt()) == fontStretch) {
                font.setStretch(fontStretch.toInt());
            }
        }

        QTextCharFormat baseFormat;
        if (style) {
            baseFormat.setForeground(QColor(style->color()));
        }
        baseFormat.setFont(font);

        for (const QString& para : paragraphs) {

            int i = 0;
            QString text;
            QVector<QTextLayout::FormatRange> lineFormats;
            QTextLayout::FormatRange currentFormat;
            currentFormat.format = baseFormat;
            currentFormat.start = -1;
            currentFormat.length = -1;

            static const QLatin1String strongTag{"strong"};
            static const QLatin1String emTag{"emphasis"};
            static const QLatin1String strikethroughTag{"strikethrough"};
            static const QLatin1String subTag{"sub"};
            static const QLatin1String supTag{"sup"};
            static const QLatin1String aTag{"a"};
            static const QLatin1String commentaryTag{"commentary"};
            static const QLatin1String codeTag{"code"};
            static const QLatin1String invertedTag{"inverted"};

            while (i < para.size()) {
                QChar c = para[i];
                /*
                 * Paragraphs in ACBF can contain a highly specific subset of html
                 * See the documentation for AdvancedComicBookFormat::Textarea for
                 * details, but here is a quick rundown of just the elements:
                <p>
                    <strong>...</strong>
                    <emphasis>...</emphasis>
                    <strikethrough>...</strikethrough>
                    <sub>...</sub>
                    <sup>...</sup>
                    <a href="">...</a>
                    <!-- the following are deprecated, but we probably still need to handle them -->
                    <commentary>...</commentary>
                    <code>...</code>
                    <inverted>...</inverted>
                </p>
                */
                if (c == '<') {
                    // Start of some kind of formatting tag
                    int tagEnd = para.indexOf('>', i + 1);
                    // Skip past end tags, because we kind of already handle those...
                    if (i + 1 < para.size() && para[i+1] != '/') {
                        QTextCharFormat format = currentFormat.format;
                        // We're starting a new tag, let's see which that is...
                        const QString thisIsStarting = para.midRef(i + 1, tagEnd - i - 1).left(para.indexOf(" ")).trimmed().toString();
                        if (thisIsStarting == strongTag) {
                            format.setFontWeight(QFont::Bold);
                        } else if (thisIsStarting == emTag) {
                            format.setFontItalic(true);
                        } else if (thisIsStarting == strikethroughTag) {
                            format.setFontStrikeOut(true);
                        } else if (thisIsStarting == subTag) {
                            format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
                        } else if (thisIsStarting == supTag) {
                            format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
                        } else if (thisIsStarting == aTag) {
                            format.setAnchor(true);
                            // Technically, a is the only thing to allow any further parameters, and it allows exactly
                            // one, so... let's just make some assumptions here for simplicity's sake
                            const QString parameters = para.mid(i + thisIsStarting.length() + 2, tagEnd - i - thisIsStarting.length() - 2);
                            if (parameters.toLower().startsWith("href=\"")) {
                                format.setAnchorHref(parameters.mid(6, parameters.length() - 7));
                            }
                        } else if (thisIsStarting == commentaryTag) {
                        } else if (thisIsStarting == codeTag) {
                        } else if (thisIsStarting == invertedTag) {
                        }
                        currentFormat.length = para.indexOf(QLatin1String("</%1>").arg(thisIsStarting), text.size()) - text.size() - thisIsStarting.length() - 2;
                        currentFormat.format = format;
                        currentFormat.start = text.size();
                        lineFormats.append(currentFormat);
                        currentFormat.start = -1;
                    }
                    i = tagEnd + 1;
                } else if (c == '&') {
                    int entityEnd = para.indexOf(';', i);
                    if (entityEnd > 0) {
                        text += fromHtmlEscaped(para.mid(i, entityEnd - i + 1));
                        i = entityEnd + 1;
                    } else {
                        ++i;
                    }
                } else {
                    text += c;
                    ++i;
                }
            }

            if (currentFormat.start != -1) {
                currentFormat.length = text.size() - currentFormat.start;
                currentFormat.format = baseFormat;
                lineFormats.append(currentFormat);
            }

            internalParagraphs.append(text);
            formats.append(lineFormats);
        }
    }

    bool attemptLayout(bool debug = false) {
        bool managedToFitEverything{true};
        QFontMetricsF fm(font);
        qreal lineHeight = fm.height();
        qreal averageCharWidth = fm.averageCharWidth();

        qreal y = margin;
        qreal ymax = shapePolygon.boundingRect().height() - margin * 2;

        qDeleteAll(layouts);
        layouts.clear();

        for (int p = 0; p < internalParagraphs.size(); ++p) {
            // Use a separate text layout for each paragraph in the document.
            QTextLayout *textLayout = new QTextLayout(internalParagraphs[p], font);
            QTextOption option = QTextOption(Qt::AlignCenter);
            option.setWrapMode(QTextOption::WordWrap);
            textLayout->setTextOption(option);
            textLayout->setFormats(formats[p]);
            textLayout->beginLayout();

            QTextLine line = textLayout->createLine();

            while (line.isValid()) {
                QPolygonF intersection = shapePolygon.intersected(QPolygonF(QRectF(0, y, q->width(), lineHeight)));
                if (intersection.size() > 0) {
                    qreal xLeft{0};
                    qreal xRight{q->width()};
                    // find leftmost and rightmost x values along top and bottom lines,
                    // and pick the innermost of those as the left/right bits of our current line
                    QVector<qreal> topAndBottom{y, y + lineHeight};
                    QVector<QPointF> cornerPoints{QPolygonF(intersection.boundingRect())};
                    if (debug) qDebug() << "intersection:" << intersection;
                    int step{0};
                    for (const qreal& position : topAndBottom) {
                        QVector<qreal> coords;
                        for (const QPointF& point : intersection) {
                            if (point.y() == position) {
                                coords.append(point.x());
                            }
                        }
                        if (coords.count() > 0) {
                            std::sort(coords.begin(), coords.end());
                            if (xLeft < coords.first()) {
                                xLeft = coords.first();
                            }
                            if (xRight > coords.last()) {
                                xRight = coords.last();
                            }
                            cornerPoints[(step == 0) ? 0 : 3] = QPointF(coords.first(), position);
                            cornerPoints[(step == 0) ? 1 : 2] = QPointF(coords.last(), position);
                        }
                        coords.clear();
                        ++step;
                    }

                    // At this point it's entirely possible that we might still have an overlap, if the polygon
                    // we're fitting things into has bits poking /in/ rather than out along the sides, so...
                    // maybe we want to try and figure this out. Maybe store the four innermost corner points,
                    // and then find them on each side, and pick the innermost x coord from that list of points
                    // on the polygon.

                    // First, we've no idea of where the polygon actually starts, so let's make sure it's the top-left corner
                    int firstPointPosition{intersection.indexOf(cornerPoints[0])};
                    if (firstPointPosition > 0) {
                        QPolygonF snippet{intersection.mid(0, firstPointPosition)};
                        intersection.remove(0, firstPointPosition);
                        intersection.append(snippet);
                    }
                    QPolygonF::const_iterator i;
                    step = 0;
                    for (i = intersection.constBegin(); i != intersection.constEnd(); ++i) {
                        if (step == 1) {
                            // From top-rightmost to bottom-rightmost
                            // find the leftmost x in that list of points
                            if (xRight > i->x()) {
                                xRight = i->x();
                                if (debug) qDebug() << "Discovered new right hand innermost";
                            }
                        } else if (step == 4) {
                            // From bottom-leftmost to end
                            // find the leftmost x in that list of points
                            if (xLeft < i->x()) {
                                xLeft = i->x();
                                if (debug) qDebug() << "Discovered new left hand innermost";
                            }
                        }
                        if (*i == cornerPoints[step]) {
                            ++step;
                        }
                    }
                    xLeft = xLeft - margin;
                    xRight = xRight - margin;
                    // we now have our true xLeft and xRight

                    line.setPosition(QPointF(xLeft, y));
                    line.setLineWidth(xRight - xLeft);

                    // Does it fit the first string here...
                    // Would be kind of nice to just be able to ask QTextLine whether it is able to
                    // fit the any part of the text it's requested to fit before wrapping
                    if (line.width() < averageCharWidth * (line.textLength() + 1)) {
                        // We can't actually fit the first word here, so... let's push the line one pixel and try again
                        y += 1;
                        if (debug) qDebug() << "Could not fit the line, move down a pixel and try again";
                    } else {
                        y += line.height();

                        // If the text is wider than the available space, move the
                        // text onto the next line if there is space.
                        if (line.naturalTextWidth() <= (xRight - xLeft)) {
                            line = textLayout->createLine();
                        } else {
                            line = QTextLine();
                        }
                    }
                } else {
                    y += 1;
                }

                // Break if there isn't enough space for another line.
                if (y + lineHeight > ymax && line.isValid()) {
                    break;
                }
            }

            // This puts whatever overflow we have on the bottom right hand corner of the item
            if (line.isValid()) {
                managedToFitEverything = false;
                line.setPosition(QPointF(shapePolygon.boundingRect().width(), shapePolygon.boundingRect().height()));
            }

            textLayout->endLayout();
            layouts.append(textLayout);

            if (!managedToFitEverything) {
                break;
            }
            if (y + lineHeight > ymax) {
                break;
            }
        }
        return managedToFitEverything;
    }

    // This sets the font size for everything (including our "many" stored character formats)
    void setFontSize(int size) {
        font.setPixelSize(size);
        for (QVector<QTextLayout::FormatRange>& formatRange : formats) {
            for (QTextLayout::FormatRange& format : formatRange) {
                QFont tempFont = format.format.font();
                tempFont.setPixelSize(size);
                format.format.setFont(tempFont);
            }
        }
    }

    bool sizeAccepted(int size) {
        setFontSize(size);
        return attemptLayout();
    }

    int findMaxSize(int searchMin, int searchMax) {
        int largestAccepted{searchMin};
        int searchSpace = searchMax - searchMin;
        if (searchSpace > 1) {
            int middle = searchMin + searchSpace / 2;
            if (sizeAccepted(middle)) {
                largestAccepted = findMaxSize(middle, searchMax);
            } else if (searchMin < middle) {
                largestAccepted = findMaxSize(searchMin, middle - 1);
            }
        }
        return largestAccepted;
    }

    void updateAnchorRects() {
        int layoutIndex{0};
        anchorRects.clear();
        for (const QTextLayout* layout : layouts) {
            int formatIndex{0};
            for (const QTextLayout::FormatRange& format : layout->formats()) {
                if (!format.format.anchorHref().isEmpty()) {
                    QList<QRectF> rects;
                    // get all the lines, so we can store all the bounding rects for this anchor...
                    int textPos = 0;
                    while (textPos < format.length) {
                        QTextLine currentLine = layout->lineForTextPosition(format.start + textPos);
                        if (currentLine.isValid()) {
                            QPointF topLeft(currentLine.cursorToX(format.start + textPos), currentLine.y());
                            QSizeF size(currentLine.width() - (currentLine.x() - topLeft.x()), currentLine.height());
                            rects << QRectF(topLeft, size);
                            textPos += currentLine.textLength();
                        } else {
                            ++textPos;
                        }
                    }
                    anchorRects[QPair<int, int>(layoutIndex, formatIndex)] = rects;
                }
                ++formatIndex;
            }
            ++layoutIndex;
        }
        Q_EMIT q->linkRectsChanged();
    }

    /**
     * Find the identifier pair (used to find formats in the anchorRects variable)
     * which identifies the format for the given local position. If there is none,
     * the function will return a pair with the values (-1, -1), which would both
     * be invalid positions in the formats list.
     * @param localPos The position to identify a pair for
     * @return An identifying pair, or an invalid pair if none was found
     */
    QPair<int, int> getAnchor(const QPointF& localPos) {
        QPair<int, int> anchor{-1, -1};
        QHash<QPair<int, int>, QList<QRectF>>::const_iterator i;
        for (i = anchorRects.constBegin(); i != anchorRects.constEnd(); ++i) {
            for (const QRectF& rect : i.value()) {
                if (rect.contains(localPos)) {
                    anchor = i.key();
                    break;
                }
            }
            if (anchor.first > -1 && anchor.second > -1) {
                break;
            }
        }
        return anchor;
    }

    void performLayout() {
        bool debugLayout{false};
        int pixelSize{2};
        margin = shapeMultiplier;
        if (paragraphs.count() > 0 && q->height() > (margin * 2) + pixelSize) {
            // Now attempt to do the text layouting, squeezing it upwards until it no longer fits
            // Cap it at the size of the polygon, divided by the number of paragraphs, minus our margin
            int maximumSize{(qFloor(shapePolygon.boundingRect().height()) / paragraphs.count()) - margin * 2};
            int bestSize = findMaxSize(pixelSize, maximumSize);
            setFontSize(bestSize);
            bool layoutSuccessful = attemptLayout(debugLayout);
            if (debugLayout) {
                qDebug() << "Layout was successful?" << layoutSuccessful << "for the paragraphs" << internalParagraphs;
                for (QTextLayout* layout : layouts) {
                    qDebug() << layout->lineCount() << "layouts at size" << pixelSize - 1 << "to fit into" << shapePolygon.boundingRect() << "the following text:" << layout->text();
                    qDebug() << shapePolygon;
                    for (int i = 0; i < layout->lineCount(); ++i) {
                        QTextLine line = layout->lineAt(i);
                        qDebug() << line.position() << line.width();
                    }
                }
            }
        } else {
            layouts.clear();
        }
        updateAnchorRects();
    }

    void buildPolygon() {
        // Convert the polygon into something both scaled correctly, and at the right place
        shapePolygon.clear();
        QVector<QPointF> points;
        for (const QPoint& point : shape) {
            int x = (point.x() * shapeMultiplier) - shapeOffset.x();
            int y = (point.y() * shapeMultiplier) - shapeOffset.y();
            points << QPointF(x, y);
        }
        shapePolygon = QPolygonF(points);
        QTransform transform;
        transform.rotate(360 - q->rotation());
        shapePolygon = transform.map(shapePolygon);
        q->transform();
    }
};

TextViewerItem::TextViewerItem(QQuickItem* parent)
    : QQuickItem(parent)
    , d(new Private(this))
{
    setFlag(ItemHasContents, true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    // Because that's what ACBF wants from us, so default that one
    setTransformOrigin(QQuickItem::TopLeft);

    connect(this, &TextViewerItem::shapeChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &TextViewerItem::shapeOffsetChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &TextViewerItem::shapeMultiplierChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &TextViewerItem::paragraphsChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &TextViewerItem::styleChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &TextViewerItem::fontFamilyChanged, d->throttle, QOverload<>::of(&QTimer::start));

    connect(this, &QQuickItem::heightChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &QQuickItem::widthChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &QQuickItem::xChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &QQuickItem::yChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &QQuickItem::rotationChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &QQuickItem::enabledChanged, d->throttle, QOverload<>::of(&QTimer::start));
    connect(this, &QQuickItem::enabledChanged, d->throttle, QOverload<>::of(&QTimer::start));
}

TextViewerItem::~TextViewerItem()
{
    delete d;
}

QStringList TextViewerItem::paragraphs() const
{
    return d->paragraphs;
}

void TextViewerItem::setParagraphs(const QStringList& newParagraphs)
{
    d->paragraphs = newParagraphs;
    Q_EMIT paragraphsChanged();
}

QVariantList TextViewerItem::shape() const
{
    QVariantList list;
    for (const QPoint& point : d->shape) {
        list << point;
    }
    return list;
}

void TextViewerItem::setShape(const QVariantList& newShape)
{
    d->shape.clear();
    for (const QVariant& point : newShape) {
        d->shape << point.toPoint();
    }
    Q_EMIT shapeChanged();
}

QPoint TextViewerItem::shapeOffset() const
{
    return d->shapeOffset;
}

void TextViewerItem::setShapeOffset(const QPoint& newShapeOffset)
{
    if (d->shapeOffset != newShapeOffset) {
        d->shapeOffset = newShapeOffset;
        Q_EMIT shapeOffsetChanged();
    }
}

double TextViewerItem::shapeMultiplier() const
{
    return d->shapeMultiplier;
}

void TextViewerItem::setShapeMultiplier(double newShapeMultiplier)
{
    if (d->shapeMultiplier != newShapeMultiplier) {
        d->shapeMultiplier = newShapeMultiplier;
        if (qIsNaN(d->shapeMultiplier)) {
            // At least let's try and make sure that things are reasonable sane
            d->shapeMultiplier = 1;
        }
        Q_EMIT shapeMultiplierChanged();
    }
}

QObject * TextViewerItem::style() const
{
    return d->style;
}

void TextViewerItem::setStyle(QObject* newStyle)
{
    if (d->style != newStyle) {
        d->style = qobject_cast<AdvancedComicBookFormat::Style*>(newStyle);
        Q_EMIT styleChanged();
    }
}

QString TextViewerItem::fontFamily() const
{
    return d->fontFamily;
}

void TextViewerItem::setFontFamily(const QString& newFontFamily)
{
    if (d->fontFamily != newFontFamily) {
        d->fontFamily = newFontFamily;
        Q_EMIT fontFamilyChanged();
    }
}

QVariantList TextViewerItem::linkRects() const
{
    QVariantList rects;
    QHash<QPair<int, int>, QList<QRectF>>::const_iterator i;
    for (i = d->anchorRects.constBegin(); i != d->anchorRects.constEnd(); ++i) {
        for (const QRectF& rect : i.value()) {
            rects << rect;
        }
    }
    return rects;
}

QString TextViewerItem::hoveredLink() const
{
    return d->hoveredLink;
}

void TextViewerItem::updatePolish()
{
    if (isEnabled()) {
        d->buildPolygon();
        d->adjustFormats();
        d->performLayout();
        update();
    }
}

QSGNode * TextViewerItem::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* data)
{
    Q_UNUSED(data)
    QQuickTextNode *n = static_cast<QQuickTextNode *>(node);
    if (!n)
        n = new QQuickTextNode(this);
    n->removeAllChildNodes();
    for (QTextLayout* layout : d->layouts) {
        n->addTextLayout(QPoint(0, 0), layout);
    }
    return n;
}

void TextViewerItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    Q_UNUSED(newGeometry)
    Q_UNUSED(oldGeometry)
    d->throttle->start();
}

void TextViewerItem::hoverMoveEvent(QHoverEvent* event)
{
    QPair<int, int> anchor = d->getAnchor(event->pos());
    // Only really need one of the point's items to be greater than -1 to know there's an anchor, no need to check more
    if (anchor.first > -1) {
        setCursor(Qt::PointingHandCursor);
        QTextLayout::FormatRange format = d->formats.value(anchor.first).value(anchor.second);
        if (d->hoveredLink != format.format.anchorHref()) {
            d->hoveredLink = format.format.anchorHref();
            Q_EMIT hoveredLinkChanged();
            Q_EMIT linkHovered(d->hoveredLink);
        }
    } else {
        if (!d->hoveredLink.isEmpty()) {
            d->hoveredLink.clear();
            Q_EMIT hoveredLinkChanged();
        }
        unsetCursor();
    }
}

void TextViewerItem::hoverLeaveEvent(QHoverEvent* event)
{
    if (!d->hoveredLink.isEmpty()) {
        d->hoveredLink.clear();
        Q_EMIT hoveredLinkChanged();
        unsetCursor();
        event->accept();
    }
}

void TextViewerItem::mousePressEvent(QMouseEvent* event)
{
    d->clickedAnchor = d->getAnchor(event->localPos());
    event->accept();
}

void TextViewerItem::mouseReleaseEvent(QMouseEvent* event)
{
    QPair<int, int> eventAnchor = d->getAnchor(event->localPos());
    if (eventAnchor.first > -1 && eventAnchor == d->clickedAnchor) {
        QTextLayout::FormatRange format = d->formats.value(eventAnchor.first).value(eventAnchor.second);
        Q_EMIT linkActivated(format.format.anchorHref());
        event->accept();
    }
}
