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

class TextViewerItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QStringList paragraphs READ paragraphs WRITE setParagraphs NOTIFY paragraphsChanged)
    Q_PROPERTY(QVariantList shape READ shape WRITE setShape NOTIFY shapeChanged)
    Q_PROPERTY(QPoint shapeOffset READ shapeOffset WRITE setShapeOffset NOTIFY shapeOffsetChanged)
    Q_PROPERTY(double shapeMultiplier READ shapeMultiplier WRITE setShapeMultiplier NOTIFY shapeMultiplierChanged)
    Q_PROPERTY(QObject* style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontFamilyChanged)
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

    QString hoveredLink() const;
    Q_SIGNAL void hoveredLinkChanged();
    Q_SIGNAL void linkHovered(const QString& link);
    Q_SIGNAL void linkActivated(const QString& link);
protected:
    void updatePolish() override;
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

private:
    class Private;
    Private* d;
};

#endif
