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
 */
namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Jump : public QObject
{
    Q_OBJECT

public:
    explicit Jump(Page* parent = nullptr);
    ~Jump() override;
    
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
    QPoint point(int index) const;
    /**
     * @param point - a point from the points list.
     * @return the index of the given point.
     */
    int pointIndex(const QPoint& point) const;

    /**
     * \brief add a point to the points list.
     * @param point - the point to add. Coordinates should be in pixels.
     * @param index - the index to add it at. If afterIndex is larger than zero,
     * the insertion will happen at that index
     */
    void addPoint(const QPoint& point, int index = -1);
    /**
     * \brief remove a point from the list.
     * @param point - point to remove from the list.
     */
    void removePoint(const QPoint& point);
    /**
     * \brief Swap two points in the list.
     * @param swapThis - the first points to swap.
     * @param withThis - the second points to swap.
     */
    bool swapPoints(const QPoint& swapThis, const QPoint& withThis);
    
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

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFJUMP_H
