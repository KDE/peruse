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

#ifndef ACBFTEXTAREA_H
#define ACBFTEXTAREA_H

#include <memory>

#include "AcbfTextlayer.h"

#include <QPoint>
#include <QRect>

/**
 * \brief Class to handle the text areas in ACBF
 * 
 * ACBF text areas are a collection of points, some text
 * and some extra information on how to render that text if necessary.
 */

namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Textarea : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString bgcolor READ bgcolor WRITE setBgcolor NOTIFY bgcolorChanged)
    Q_PROPERTY(int pointCount READ pointCount NOTIFY pointCountChanged)
    Q_PROPERTY(QRect bounds READ bounds NOTIFY boundsChanged)
    Q_PROPERTY(int textRotation READ textRotation WRITE setTextRotation NOTIFY textRotationChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)
    Q_PROPERTY(bool transparent READ transparent WRITE setTransparent NOTIFY transparentChanged)
    Q_PROPERTY(QStringList paragraphs READ paragraphs WRITE setParagraphs NOTIFY paragraphsChanged)

public:
    explicit Textarea(Textlayer* parent = nullptr);
    ~Textarea() override;

    /**
     * @return the types that a text-area can be, like speech or commentary.
     */
    static QStringList availableTypes();

    /**
     * \brief Write the textarea into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a textarea element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return a list of points that encompasses the textarea.
     */
    QList<QPoint> points() const;
    /**
     * @param index - the index of the desired point.
     * @return a point for an index.
     */
    Q_INVOKABLE QPoint point(int index) const;
    /**
     * @param point - a point from the points list.
     * @return the index of the given point.
     */
    Q_INVOKABLE int pointIndex(const QPoint& point) const;

    /**
     * \brief add a point to the points list.
     * @param point - the point to add. Coordinates should be in pixels.
     * @param index - the index to add it at. If afterIndex is larger than zero,
     * the insertion will happen at that index
     */
    Q_INVOKABLE void addPoint(const QPoint& point, int index = -1);
    /**
     * \brief remove a point from the list.
     * @param point - point to remove from the list.
     */
    Q_INVOKABLE void removePoint(const QPoint& point);
    /**
     * \brief Swap two points in the list.
     * @param swapThis - the first points to swap.
     * @param withThis - the second points to swap.
     */
    bool swapPoints(const QPoint& swapThis, const QPoint& withThis);
    /**
     * @brief set the points based on a top left and bottom right.
     * @param topLeft the topleft corner of the rect.
     * @param bottomRight the bottomright corner of the rect.
     */
    Q_INVOKABLE void setPointsFromRect(const QPoint& topLeft, const QPoint& bottomRight);

    int pointCount() const;
    /**
     * @brief fires when the point counts changes.
     */
    Q_SIGNAL void pointCountChanged();
    /**
     * @brief convenience function to get the ractangle of the points.
     * @return the bounds of the frame.
     */
    QRect bounds() const;
    /**
     * @brief fires when the bounds change, which happens when the point count changes.
     */
    Q_SIGNAL void boundsChanged();
    /**
     * @return the background color as a QString.
     * 
     * It should be an 8bit per channel rgb hexcode.
     */
    QString bgcolor() const;
    /**
     * \brief set the background color.
     * 
     * @param newColor - a String with an 8bit per channel rgb hexcode (#ff00ff, or the like)
     */
    void setBgcolor(const QString& newColor = QString());
    /**
     * @brief fires when the background color changes.
     */
    Q_SIGNAL void bgcolorChanged();

    /**
     * @return the text rotation in degrees.
     */
    int textRotation() const;
    /**
     * \brief set the text rotation.
     * @param rotation - the text rotation in degrees.
     */
    void setTextRotation(int rotation = 0);
    /**
     * @brief fires when the text rotation is set.
     */
    Q_SIGNAL void textRotationChanged();

    /**
     * @return the type of the text area. This determines how it is styled.
     */
    QString type() const;
    /**
     * \brief set the type of the text area.
     * @param type - this should be an entry from the availableTypes(),
     * it will change the way how the text is styled. The default is "speech".
     */
    void setType(const QString& type = QStringLiteral("speech"));
    /**
     * @brief fires when the type is set.
     */
    Q_SIGNAL void typeChanged();

    /**
     * @return whether the text should use the 'inverted' colorscheme.
     */
    bool inverted() const;
    /**
     * \brief set whether the text should use the inverted colorscheme.
     * @param inverted - whether to do so.
     */
    void setInverted(bool inverted = false);
    /**
     * @brief fires when inverted is set.
     */
    Q_SIGNAL void invertedChanged();
    /**
     * @return whether to use the background color when overlaying the text.
     */
    bool transparent() const;
    /**
     * \brief set whether to use the background color when overlaying the text.
     * @param transparent - false means to use the background color, true means
     * to have the textare background transparent.
     */
    void setTransparent(bool transparent = false);
    /**
     * @brief fires when transparent is set.
     */
    Q_SIGNAL void transparentChanged();

    /**
     * @returns a list of paragraphs.
     * 
     * Contains allowed sub-elements: strong, emphasis, strikethrough
     * sub, sup, a (with mandatory href attribute only)
     * Can also contain deprecated sub-elements (superceded by...): code (type option code),
     * inverted (textarea option inverted)
     */
    QStringList paragraphs() const;
    /**
     * \brief set the list of paragraphs for this textarea.
     * @param paragraphs - a list of paragraphs. Can contain sub-elements:
     * strong, emphasis, strikethrough, sub, sup, a (with mandatory href attribute only)
     */
    void setParagraphs(const QStringList& paragraphs);
    /**
     * @brief fires when the paragraphs are set.
     */
    Q_SIGNAL void paragraphsChanged();
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFTEXTAREA_H
