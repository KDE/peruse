/*
 * Copyright 2018  Wolthera van Hövell tot Westerflier <griffinvalley@gmail.com>
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

#ifndef ACBFFRAME_H
#define ACBFFRAME_H

#include <memory>

#include "AcbfInternalReferenceObject.h"
#include "AcbfPage.h"

#include <QPoint>
#include <QRect>
/**
 * \brief a Class to handle comic panels.
 *
 * Frames are a polygon of points, which describe
 * a panel on a comic book page image in pixels.
 *
 * This can be used to give improved navigation on
 * smaller screens.
 *
 * Frames also have a background color which can be used
 * to enhance the reading experience by setting the background
 * color to the frame color.
 */
namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Frame : public InternalReferenceObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString bgcolor READ bgcolor WRITE setBgcolor NOTIFY bgcolorChanged)
    Q_PROPERTY(int pointCount READ pointCount NOTIFY pointCountChanged)
    Q_PROPERTY(QRect bounds READ bounds NOTIFY boundsChanged)
public:
    explicit Frame(Page *parent = nullptr);
    ~Frame() override;

    /**
     * \brief Write the frame into the xml writer.
     */
    void toXml(QXmlStreamWriter *writer);
    /**
     * \brief load a frame element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return The ID of this frame as a QString.
     * Used to identify it from other parts of the
     * ACBF document.
     */
    QString id() const;

    /**
     * \brief Set the ID for this frame.
     * This is used to reference this element from
     * other parts of the ACBF document.
     * @param newId - The new ID as a string.
     */
    void setId(const QString &newId);
    Q_SIGNAL void idChanged();

    /**
     * @return a list of points that encompasses the frame.
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
    Q_INVOKABLE int pointIndex(const QPoint &point) const;

    /**
     * \brief add a point to the points list.
     * @param point - the point to add. Coordinates should be in pixels.
     * @param index - the index to add it at. If afterIndex is larger than zero,
     * the insertion will happen at that index
     */
    Q_INVOKABLE void addPoint(const QPoint &point, int index = -1);
    /**
     * \brief remove a point from the list.
     * @param point - point to remove from the list.
     */
    Q_INVOKABLE void removePoint(const QPoint &point);
    /**
     * \brief Swap two points in the list.
     * @param swapThis - the first points to swap.
     * @param withThis - the second points to swap.
     */
    bool swapPoints(const QPoint &swapThis, const QPoint &withThis);
    /**
     * @brief set the points based on a top left and bottom right.
     * @param topLeft the topleft corner of the rect.
     * @param bottomRight the bottomright corner of the rect.
     */
    Q_INVOKABLE void setPointsFromRect(const QPoint &topLeft, const QPoint &bottomRight);

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
    void setBgcolor(const QString &newColor = QString());
    /**
     * @brief fires when the background color changes.
     */
    Q_SIGNAL void bgcolorChanged();

    /**
     * The position of this Frame in the list of Frames in the
     * parent Page instance.
     * @return The instance's position
     */
    int localIndex() override;

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFFRAME_H
