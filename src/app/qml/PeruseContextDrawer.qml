/*
 *   Copyright 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.0 as QtControls
import org.kde.kirigami 1.0

// Modified version of the ContextDrawer component found in the Plasma Components
// In addition to the original drawer, this will allow you to optionally insert an item
// at the top of the menu, which can be any item, but originally designed for the thumbnail
// navigation system for comic book pages found in Peruse.
OverlayDrawer {
    id: root

    /**
     * title: string
     * A title for the action list that will be shown to the user when opens the drawer
     */
    property string title: typeof i18n !== "undefined" ? i18n("Actions") : "Actions"

    // This can be any list of objects which can be a child of a column
    property Item topContent: pageStack.currentItem && pageStack.currentItem.contextualTopItems ? pageStack.currentItem.contextualTopItems : null;

    /**
     * actions: list<Action>
     * This can be any type of object that a ListView can accept as model. 
     * It expects items compatible with either QAction or Kirigami Action
     */
    property var actions: pageStack.currentItem ? pageStack.currentItem.contextualActions : null
    enabled: menu.count > 0
    edge: Qt.RightEdge

    handleVisible: applicationWindow() ? applicationWindow().controlsVisible : true

    Connections {
        target: pageStack
        onCurrentItemChanged: {
            actions = pageStack.currentItem.contextualActions
        }
    }

    contentItem: Item {
        implicitWidth: Units.gridUnit * 20
        Item {
            id: topContainer;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: parent.height - menu.contentHeight;
            children: root.topContent;
        }
        ListView {
            id: menu
            interactive: contentHeight > height
            model: {
                if (typeof root.actions == "undefined") {
                    return null;
                }
                if (root.actions.length == 0) {
                    return null;
                } else {
                    return root.actions[0].text !== undefined &&
                        root.actions[0].trigger !== undefined ?
                            root.actions :
                            root.actions[0];
                }
            }
            anchors {
                top: topContainer.bottom;
                left: parent.left;
                right: parent.right;
            }
            height: (menu.count > 0 && parent.height > 1 && parent.height > menu.contentHeight) ? menu.contentHeight : parent.height;
            header: Item {
                height: heading.height
                width: menu.width
                Heading {
                    id: heading
                    anchors {
                        left: parent.left
                        margins: Units.largeSpacing
                    }
                    width: root.width
                    elide: Text.ElideRight
                    level: 2
                    text: root.title
                }
            }
            delegate: BasicListItem {
                property Item drawerRoot: root;
                property Item listViewItem: menu;
                checked: modelData.checked
                icon: modelData.iconName
                supportsMouseEvents: true
                label: model ? model.text : modelData.text
                enabled: model ? model.enabled : modelData.enabled
                visible: model ? model.visible : modelData.visible
                opacity: enabled ? 1.0 : 0.6
                onClicked: {
                    if (modelData && modelData.trigger !== undefined) {
                        modelData.trigger();
                    // assume the model is a list of QAction or Action
                    } else if (listViewItem.model.length > index) {
                        listViewItem.model[index].trigger();
                    } else {
                        console.warning("Don't know how to trigger the action")
                    }
                    drawerRoot.close();
                }
            }
        }
    }
}
