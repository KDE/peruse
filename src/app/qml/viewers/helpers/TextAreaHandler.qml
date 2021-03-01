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
import QtQuick.Shapes 1.15
import org.kde.kirigami 2.7 as Kirigami

Item {
    id: component
    property double multiplier
    property int offsetX
    property int offsetY
    property var textArea

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
    Text {
        anchors.fill: parent
        textFormat: Text.StyledText
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        fontSizeMode: Text.Fit
        property QtObject style: root.model.acbfData.styleSheet.style("text-area", textArea.type, textArea.inverted);
        color: style.color
        minimumPixelSize: 10
        font {
            pixelSize: 250
            family: root.model.firstAvailableFont(style.fontFamily)
            italic: style.fontStyle === "italic"
//             weight: style.fontWeight
//             stretch: style.fontStretch
        }
        text: {
            var allText = "";
            for (var i = 0; i < textArea.paragraphs.length; ++i) {
                allText += "<p>" + textArea.paragraphs[i] + "</p>";
            }
            //console.log("Constructing " + textArea.paragraphs.length + " paragraphs of text for " + textArea.bounds + allText + textArea.paragraphs);
            return allText;
        }
    }
}
