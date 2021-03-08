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
    QList<QTextLayout*> layouts;

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

        QTextCharFormat boldFormat = baseFormat;
        boldFormat.setFontWeight(QFont::Bold);

        QTextCharFormat headingFormat = boldFormat;
        headingFormat.setFontPointSize(font.pointSize() + 6.0);

        QTextCharFormat italicFormat = baseFormat;
        italicFormat.setFontItalic(true);

        QTextCharFormat codeFormat;
        codeFormat.setFontFamily("Courier");
        codeFormat.setFontPointSize(qMax(4, font.pointSize() - 2));

        QHash<QString,QTextCharFormat> formatHash;
        formatHash["bold"] = boldFormat;
        formatHash["italic"] = italicFormat;
        formatHash["heading"] = headingFormat;
        formatHash["code"] = codeFormat;

        for (const QString& para : paragraphs) {

            int i = 0;
            QString text;
            QVector<QTextLayout::FormatRange> lineFormats;
            QTextLayout::FormatRange currentFormat;
            currentFormat.format = baseFormat;
            currentFormat.start = -1;
            currentFormat.length = -1;

            while (i < para.size()) {
                QChar c = para[i];
//                 if (c == '\\') {
//                     if (i + 1 < para.size() && para[i+1] == '\\') {
//                         text += '\\';
//                         i += 2;
//                         continue;
//                     }
// 
//                     int at = para.indexOf("{", i + 1);
//                     if (at != -1) {
//                         QString command = para.mid(i + 1, at - i - 1);
//                         if (formatHash.contains(command)) {
//                             currentFormat.format = formatHash[command];
//                             currentFormat.start = text.size();
//                             i = at + 1;
//                             continue;
//                         }
//                     }
//                 } else if (c == '}') {
//                     if (currentFormat.start != -1) {
//                         currentFormat.length = text.size() - currentFormat.start;
//                         lineFormats.append(currentFormat);
//                         currentFormat.start = -1;
//                         ++i;
//                         continue;
//                     }
//                 }

                text += c;
                ++i;
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

    bool attemptLayout() {
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
                            }
                        } else if (step == 4) {
                            // From bottom-leftmost to end
                            // find the leftmost x in that list of points
                            if (xLeft < i->x()) {
                                xLeft = i->x();
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
                    } else {
                        y += line.height();

                        // If the text is wider than the available space, move the
                        // text onto the next line if there is space.
                        if (line.naturalTextWidth() <= (xRight - xLeft) && y <= ymax) {
                            line = textLayout->createLine();
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

            if (y > ymax) {
                break;
            }
            if (!managedToFitEverything) {
                break;
            }
        }
        return managedToFitEverything;
    }

    void performLayout() {
        int pixelSize{2};
        margin = shapeMultiplier;
        if (paragraphs.count() > 0 && q->height() > (margin * 2) + pixelSize) {
            // Now attempt to do the text layouting, squeezing it upwards until it no longer fits
            // Cap it at the size of the polygon, divided by the number of paragraphs, minus our margin
            int maximumSize{(qFloor(q->height()) / paragraphs.count()) - margin * 2};
            font.setPixelSize(pixelSize);
            while (attemptLayout() && pixelSize < maximumSize) {
                ++pixelSize;
                font.setPixelSize(pixelSize);
            }
            font.setPixelSize(pixelSize - 1);
            attemptLayout();
        } else {
            layouts.clear();
        }
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
