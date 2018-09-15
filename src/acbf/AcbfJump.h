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

#ifndef ACBFJUMP_H
#define ACBFJUMP_H

#include <QObject>
#include <memory>

#include "AcbfPage.h"

#include <QPoint>
#include <QRect>
namespace AdvancedComicBookFormat
{
/**
 * \brief a Class to handle ACBF jumps.
 * 
 * Jumps are areas that point at a specific page.
 * 
 * This allows for a table of contents page that
 * can switch to a specific story by the user selecting
 * a jump area drawn around the icon for the specific
 * story and pointing at the beginning of the story.
 * 
 * Other uses included choose your own adventure style books.
 * 
 * Within ACBF, Jumps are areas defined by a polygon of points,
 * with an index pointing at the page to jump to.
 */
class ACBF_EXPORT Jump : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int pointCount READ pointCount NOTIFY pointCountChanged)
    Q_PROPERTY(QRect bounds READ bounds NOTIFY boundsChanged)
    Q_PROPERTY(int pageIndex READ pageIndex WRITE setPageIndex NOTIFY pageIndexChanged)

public:
    explicit Jump(Page* parent = nullptr);
    ~Jump() override;
    
    /**
     * \brief Write the jump into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a jump element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return a list of points that encompasses the jump.
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
     * @brief The page to jump to.
     * 
     * @return int that points at the index of the page to jump to.
     */
    int pageIndex() const;
    
    /**
     * @brief set the page to jump to.
     * 
     * @param pageNumber An integer for the index of the page in the
     * page list.
     */
    void setPageIndex(const int& pageNumber = 0);
    /**
     * @brief pageIndexChanged
     */
    Q_SIGNAL void pageIndexChanged();

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFJUMP_H
