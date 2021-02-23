/*
 * Copyright (C) 2021 Mahmoud Ahmed Khalil
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

/**
 * @brief a component that should handle jumps
 * 
 * JumpHandler takes in the Jump object, offset from the image, and the zooming
 * ratio. It draws itself on the screen according to the Jump object's dimensions
 */

Item {
    id: component;
    
    x: offsetX + (widthMultiplier * jumpObject.bounds.x) + (widthMultiplier * jumpObject.bounds.width / 2) - markerRadius;
    y: offsetY + (heightMultiplier * jumpObject.bounds.y) + (heightMultiplier * jumpObject.bounds.height / 2) - markerRadius;
    width: markerRadius * 2;
    height: markerRadius * 2;
    
    /**
     * Passed jump object from the model
     */
    property QtObject jumpObject;
    
    /**
     * Horizontal offset of the jump in the view port
     */
    property int offsetX;
    
    /**
     * Vertical offset of the jump in the view port
     */
    property int offsetY;
    
    /**
     * Horizontal zoom ratio of the view port 
     */
    property real widthMultiplier;
    
    /**
     * Vertical zoom ratio of the view port
     */
    property real heightMultiplier;
    
    /**
     * Zoom ratio for the radius of the jump marker, which should be 
     * the minimum of the width, and height zoom ratios
     */
    property real radiusMultiplier: Math.min(widthMultiplier, heightMultiplier);
    
    /**
     * The radius of the jump marker
     */
    property real markerRadius: radiusMultiplier * Kirigami.Units.gridUnit * 1.5;
    
    /**
     * A boolean flag indicating whether the marker should blink or not
     */
    property bool blinking: hovered? false : peruseConfig.animateJumpAreas;
    
    /**
     * Boolean flag indicating whether or not we should highlight the Jump marker
     */
    property bool highlightMarker: highlightingTimer.highlightingState;
    
    property color highlightingColor: Kirigami.Theme.highlightColor;
    property color dimmingColor: Qt.darker(Kirigami.Theme.highlightColor, 2);
    
    /**
     * Boolean flag that listens to the actual hovering event
     */
    property bool hovered: jumpHoverEventHandler.hovered;
    
    /**
     * Gets fired whenever the Handler is clicked/tapped
     */
    signal activated();

    Shape {
        id: jumpMarker;
        anchors.fill: parent;
        
        ShapePath {
            id: jumpMarkerBody;
            
            fillColor: Qt.rgba(dimmingColor.r, dimmingColor.g, dimmingColor.b, 0.5);
            strokeColor: Qt.rgba(dimmingColor.r, dimmingColor.g, dimmingColor.b, 0.5);
            strokeWidth: 1;
            capStyle: ShapePath.RoundCap;
            
            PathAngleArc {
                centerX: component.width / 2;
                centerY: component.height / 2;
                radiusX: jumpHoverEventHandler.hovered? markerRadius - (radiusMultiplier * Kirigami.Units.smallSpacing) : markerRadius - (radiusMultiplier * Kirigami.Units.smallSpacing * 2);
                radiusY: jumpHoverEventHandler.hovered? markerRadius - (radiusMultiplier * Kirigami.Units.smallSpacing) : markerRadius - (radiusMultiplier * Kirigami.Units.smallSpacing * 2);
                
                startAngle: -180;
                sweepAngle: 360;
            }
        }
        
        ShapePath {
            id: jumpMarkerBorder;
            fillColor: "transparent";
            strokeColor: highlightMarker? highlightingColor : Qt.rgba(dimmingColor.r, dimmingColor.g, dimmingColor.b, 0.3); Behavior on strokeColor { ColorAnimation { duration: Kirigami.Units.longDuration; easing.type: Easing.InOutQuad; } }
            strokeWidth: Kirigami.Units.largeSpacing;
            capStyle: ShapePath.RoundCap;
            
            PathAngleArc {
                centerX: component.width / 2;
                centerY: component.height / 2;
                radiusX: jumpHoverEventHandler.hovered? markerRadius + (radiusMultiplier * Kirigami.Units.smallSpacing) : markerRadius;
                radiusY: jumpHoverEventHandler.hovered? markerRadius + (radiusMultiplier * Kirigami.Units.smallSpacing) : markerRadius;
                
                startAngle: -180;
                sweepAngle: 360;
            }
        }
    }
    
    Timer {
        id: highlightingTimer;
        interval: Kirigami.Units.veryLongDuration;
        repeat: true;
        running: (component.visible && blinking);
        onTriggered: {
            highlightingState = !highlightingState;
        }
        
        onRunningChanged: {
            highlightingState = component.hovered;
        }
        
        property bool highlightingState: false;
    }
    
    TapHandler {
        id: jumpTapEventHandler;
        
        onTapped: {
            activated();
        }
    }
    
    HoverHandler {
        id: jumpHoverEventHandler;
    }
}
