/*
 * Copyright (C) 2021 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Shapes
import org.kde.kirigami as Kirigami
import org.kde.peruse as Peruse

Item {
    id: component
    /**
     * The ArchiveBookModel instance this textarea is contained within
     */
    property QtObject model

    /**
     * An instance of Peruse.IdentifiedObjectsModel pointed at the ACBF Document
     * this TextAreaHandler's Textarea comes from
     * \note It would be possible to create one here, but the model is expensive to
     * instantiate, and having a global one ensures that we just have the one
     * copy (which is also nice)
     */
    property QtObject identifiedObjects

    /**
     * The zoom ratio of the view port
     */
    property real multiplier: 1

    /**
     * Horizontal offset of the Textarea in the viewport
     */
    property int offsetX: 0

    /**
     * Vertical offset of the Textarea in the viewport
     */
    property int offsetY: 0

    /**
     * The Textarea object itself (an AdvancedComicBookFormat::Textarea object instance)
     */
    property QtObject textArea: null

    /**
     * Fired when the user clicks/taps on a link in the Textarea
     * \note To work out if this is every likely for a specific instance, use linkRects to check whether there are any links
     */
    signal linkActivated(string link);
    /**
     * Fired when the user hovers over a link in the Textarea
     * \note To work out if this is every likely for a specific instance, use linkRects to check whether there are any links
     */
    property alias hoveredLink: textViewer.hoveredLink
    /**
     * A list of rectangles (with item-local coordinates) covering the areas
     * of this item which contain links (that is, things which should be
     * interactible)
     */
    property alias linkRects: textViewer.linkRects

    property rect textareaRect: Qt.rect((component.multiplier * textArea.bounds.x) + component.offsetX,
                        (component.multiplier * textArea.bounds.y) + component.offsetY,
                        (component.multiplier * textArea.bounds.width),
                        (component.multiplier * textArea.bounds.height))
    x: textareaRect.x
    y: textareaRect.y
    width: textareaRect.width
    height: textareaRect.height
    Shape {
        ShapePath {
            id: textAreaOutlinePath
            strokeColor: "transparent"
            fillColor: component.textArea.bgcolor
            property var linePoint: component.textArea.point(0)
            startX: (linePoint.x * component.multiplier) - (component.x - component.offsetX)
            startY: (linePoint.y * component.multiplier) - (component.y - component.offsetY)
        }
        Instantiator {
            id: textAreaOutlineInstantiator
            model: component.textArea.pointCount - 1
            onObjectAdded: textAreaOutlinePath.pathElements.push(object)
            PathLine {
                property var linePoint: component.textArea.point(modelData + 1)
                x: (linePoint.x * component.multiplier) - (component.x - component.offsetX)
                y: (linePoint.y * component.multiplier) - (component.y - component.offsetY)
            }
        }
    }
    Peruse.TextViewerItem {
        id: textViewer
        anchors.fill: parent
        rotation: 360 - component.textArea.textRotation

        enabled: component.enabled
        paragraphs: component.textArea.paragraphs
        shape: component.textArea.points
        shapeOffset: Qt.point((component.multiplier * component.textArea.bounds.x), (component.multiplier * component.textArea.bounds.y))
        shapeMultiplier: component.multiplier
        style: component.model.acbfData.styleSheet.style("text-area", component.textArea.type, component.textArea.inverted);
        fontFamily: style ? component.model.firstAvailableFont(style.fontFamily) : ""

        onLinkActivated: {
            component.linkActivated(link);
        }
    }
    QQC2.ToolTip {
        text: textViewer.hoveredLink
        visible: textViewer.hoveredLink !== ""
    }
}
