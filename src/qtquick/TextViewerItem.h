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

#ifndef TEXTVIEWERITEM_H
#define TEXTVIEWERITEM_H

#include <QQuickItem>

/**
 * A QQuickItem which fits a set of paragraphs of text with a given style into an arbitrary polygon
 * Particularly, this is designed to render data from AdvancedComicBookFormat::Textarea instances.
 */
class TextViewerItem : public QQuickItem
{
    Q_OBJECT
    /**
     * A list of paragraphs which should be displayed inside the polygon
     * These are expected to be in the format returned by AdvancedComicBookFormat::Textarea::paragraphs()
     */
    Q_PROPERTY(QStringList paragraphs READ paragraphs WRITE setParagraphs NOTIFY paragraphsChanged)
    /**
     * A list of points which make up the polygon the text should fit inside.
     * This is expected to be in the format returned by AdvancedComicBookFormat::Textarea::points()
     */
    Q_PROPERTY(QVariantList shape READ shape WRITE setShape NOTIFY shapeChanged)
    /**
     * The offset of the shape relative to the page (useful when the item is nested inside others,
     * such as our own TextAreaHandler component).
     */
    Q_PROPERTY(QPoint shapeOffset READ shapeOffset WRITE setShapeOffset NOTIFY shapeOffsetChanged)
    /**
     * The zoom ratio of the view port
     */
    Q_PROPERTY(double shapeMultiplier READ shapeMultiplier WRITE setShapeMultiplier NOTIFY shapeMultiplierChanged)
    /**
     * The style which should be used to render the text in this item. This must be
     * an instance of AdvancedComicBookFormat::Style.
     */
    Q_PROPERTY(QObject* style READ style WRITE setStyle NOTIFY styleChanged)
    /**
     * The specific font family to be used for the text. This is expected to exist on the system
     * and works best in conjunction with the ArchiveBookModel::fontFamilyName(QString) function.
     */
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontFamilyChanged)
    /**
     * This is a list of all the active rects in the item (in essence, anywhere there is an anchor
     * in one of the paragraphs, there will be a corresponding rect in this list).
     */
    Q_PROPERTY(QVariantList linkRects READ linkRects NOTIFY linkRectsChanged)
    /**
     * The address for the link currently underneath the pointer (empty if none).
     */
    Q_PROPERTY(QString hoveredLink READ hoveredLink NOTIFY hoveredLinkChanged)
public:
    explicit TextViewerItem(QQuickItem *parent = nullptr);
    virtual ~TextViewerItem();

    QStringList paragraphs() const;
    void setParagraphs(const QStringList& newParagraphs);
    Q_SIGNAL void paragraphsChanged();

    QVariantList shape() const;
    void setShape(const QVariantList& newShape);
    Q_SIGNAL void shapeChanged();

    QPoint shapeOffset() const;
    void setShapeOffset(const QPoint& newShapeOffset);
    Q_SIGNAL void shapeOffsetChanged();

    double shapeMultiplier() const;
    void setShapeMultiplier(double newShapeMultiplier);
    Q_SIGNAL void shapeMultiplierChanged();

    QObject* style() const;
    void setStyle(QObject* newStyle);
    Q_SIGNAL void styleChanged();

    QString fontFamily() const;
    void setFontFamily(const QString& newFontFamily);
    Q_SIGNAL void fontFamilyChanged();

    QVariantList linkRects() const;
    Q_SIGNAL void linkRectsChanged();

    QString hoveredLink() const;
    Q_SIGNAL void hoveredLinkChanged();
    Q_SIGNAL void linkHovered(const QString& link);
    Q_SIGNAL void linkActivated(const QString& link);
protected:
    void updatePolish() override;
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void hoverLeaveEvent(QHoverEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:
    class Private;
    Private* d;
};

#endif
