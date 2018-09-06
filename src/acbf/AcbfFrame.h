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

#include <QObject>
#include <memory>

#include "AcbfPage.h"

#include <QPoint>
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
class ACBF_EXPORT Frame : public QObject
{
    Q_OBJECT

public:
    explicit Frame(Page* parent = nullptr);
    ~Frame() override;
    
    /**
     * \brief Write the frame into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a frame element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * @return a list of points that encompasses the frame.
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

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif // ACBFFRAME_H
