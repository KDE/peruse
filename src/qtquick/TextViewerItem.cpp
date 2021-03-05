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
#include <QTimer>
#include <private/qquicktextnode_p.h>

class TextViewerItem::Private {
public:
    Private(TextViewerItem* qq)
        : q(qq)
    {}
    ~Private() {
        qDeleteAll(layouts);
    }
    TextViewerItem* q;
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

                // We use lines at the top, middle, and bottom of the planned
                // location for the text.
                QLineF topLine = QLineF(0, y, q->width(), y);
                QLineF centerLine = topLine;
                centerLine.translate(0, lineHeight / 2);
                QLineF bottomLine = topLine;
                bottomLine.translate(0, lineHeight);

                // Obtain all x-coordinates where intersections occur.
                QVector<qreal> xCoords;

                for (int i = 0; (i+1) < shapePolygon.size(); ++i) {
                    QLineF pLine = QLineF(shapePolygon[i], shapePolygon[i+1]);
                    QPointF p;
                    if (pLine.intersects(topLine, &p) == QLineF::BoundedIntersection) {
                        xCoords.append(p.x());
                    }
//                     if (pLine.intersects(centerLine, &p) == QLineF::BoundedIntersection) {
//                         xCoords.append(p.x());
//                     }
//                     if (pLine.intersects(bottomLine, &p) == QLineF::BoundedIntersection) {
//                         xCoords.append(p.x());
//                     }
                }

                // If intersections occurred, sort them and use the innermost
                // x-coordinates as horizontal delimiters to mark the area in
                // which text can be written.
                qreal left;
                qreal right;

                if (xCoords.size() > 0 && (xCoords.size() % 2) == 0) {
                    std::sort(xCoords.begin(), xCoords.end());

                    left = xCoords[xCoords.size()/2 - 1] + margin;
                    right = xCoords[xCoords.size()/2] - margin;

                    line.setPosition(QPointF(left, y));
                    line.setLineWidth(right - left);

                    y += line.height();

                    // If the text is wider than the available space, move the
                    // text onto the next line if there is space.
                    if (line.naturalTextWidth() <= (right - left) && y <= ymax)
                        line = textLayout->createLine();
                } else {
                    y += lineHeight;
                }

                // Break if there isn't enough space for another line.
                if (y > ymax) {
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

    void positionLayouts() {
        // Now attempt to do the text layouting, squeezing it upwards as long as it fits
        int pixelSize{10};
        font.setPixelSize(pixelSize);
        while (attemptLayout() && pixelSize < 150) {
            ++pixelSize;
            font.setPixelSize(pixelSize);
        }
        font.setPixelSize(pixelSize - 1);
        attemptLayout();
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
    }
};

TextViewerItem::TextViewerItem(QQuickItem* parent)
    : QQuickItem(parent)
    , d(new Private(this))
{
    setFlag(ItemHasContents, true);

    connect(this, &TextViewerItem::shapeChanged, &QQuickItem::polish);
    connect(this, &TextViewerItem::shapeOffsetChanged, &QQuickItem::polish);
    connect(this, &TextViewerItem::shapeMultiplierChanged, &QQuickItem::polish);
    connect(this, &TextViewerItem::paragraphsChanged, &QQuickItem::polish);
    connect(this, &TextViewerItem::styleChanged, &QQuickItem::polish);
    connect(this, &TextViewerItem::fontFamilyChanged, &QQuickItem::polish);

    connect(this, &QQuickItem::heightChanged, &QQuickItem::polish);
    connect(this, &QQuickItem::widthChanged, &QQuickItem::polish);
    connect(this, &QQuickItem::xChanged, &QQuickItem::polish);
    connect(this, &QQuickItem::yChanged, &QQuickItem::polish);
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
    d->buildPolygon();
    d->adjustFormats();
    d->positionLayouts();
    update();
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
    polish();
}
