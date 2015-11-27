/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick 2.1
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0
import QtQuick.Window 2.1

import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

ApplicationWindow {
    id: mainWindow;
    property int animationDuration: 200;

    SystemPalette { id: sysPal; }

    toolBar: ToolBar {
        RowLayout {
            PlasmaComponents.ToolButton {
                iconName: "format-justify-fill"
                onClicked: mainWindow.toggleSidebar();
            }
            PlasmaComponents.ToolButton {
                iconName: "draw-arrow-back"
                text: "Back"
                enabled: view.depth > 1

                onClicked: goUp();
            }
        }
    }

    leftSidebar: ColumnLayout {
        spacing: 0
        PlasmaExtras.Heading {
            text: "Navigation"
            font.bold: true
            level: 2
        }
        PlasmaComponents.ToolButton {
            text: "All Comics"
            iconName: "system-search"
            Layout.fillWidth: true
            onClicked: {
                view.clear()
                view.push({
                    item: allComics,
                    properties: { focus: true }
                })
                mainWindow.toggleSidebar();
            }
        }
    }
    mainItem: StackView {
        id: view
        focus: true

        delegate: StackViewDelegate {
            pushTransition: StackViewTransition {
                PropertyAnimation {
                    duration: mainWindow.animationDuration;
                    target: enterItem
                    property: "x"
                    from: exitItem.height
                    to: 0
                }
            }
            popTransition: StackViewTransition {
                PropertyAnimation {
                    duration: mainWindow.animationDuration;
                    target: exitItem
                    property: "x"
                    from: 0
                    to: enterItem.height
                }
            }
        }
    }
}
