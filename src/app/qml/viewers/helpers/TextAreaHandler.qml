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

import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Shapes 1.15
import org.kde.kirigami 2.7 as Kirigami
import org.kde.peruse 0.1 as Peruse

Item {
    id: component
    property QtObject model
    property real multiplier: 1
    property int offsetX: 0
    property int offsetY: 0
    property QtObject textArea: null
    signal linkActivated(string link);

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
        id: textEdit
        anchors.fill: parent
        rotation: 360 - component.textArea.textRotation

        enabled: component.enabled
        paragraphs: component.textArea.paragraphs
        shape: component.textArea.points
        shapeOffset: Qt.point((component.multiplier * component.textArea.bounds.x), (component.multiplier * component.textArea.bounds.y))
        shapeMultiplier: component.multiplier
        style: component.model.acbfData.styleSheet.style("text-area", component.textArea.type, component.textArea.inverted);
        fontFamily: style ? component.model.firstAvailableFont(style.fontFamily) : ""

        function onLinkActivated(link) {
            component.linkActivated(link);
        }
    }
    QQC2.ToolTip {
        text: textEdit.hoveredLink
        visible: textEdit.hoveredLink !== ""
    }
}
