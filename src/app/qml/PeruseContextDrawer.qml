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

import QtQuick 2.12
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12 as QtControls

import org.kde.kirigami 2.7

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
    property Item topContent: pageStack.layers.depth > 1
        ? (pageStack.layers.currentItem.contextualTopItems ? pageStack.layers.currentItem.contextualTopItems : null)
        : (pageStack.currentItem && pageStack.currentItem.contextualTopItems ? pageStack.currentItem.contextualTopItems : null);

    /**
     * actions: list<Action>
     * This can be any type of object that a ListView can accept as model. 
     * It expects items compatible with either QAction or Kirigami Action
     */
    property var actions: pageStack.layers.depth > 1
        ? pageStack.layers.currentItem.contextualActions
        : (pageStack.currentItem ? pageStack.currentItem.contextualActions : null)
    enabled: actions !== undefined && actions !== null && actions.length > 0;
    edge: Qt.RightEdge
    drawerOpen: false

    //list items go to edges, have their own padding
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 0

    handleVisible: applicationWindow == undefined || applicationWindow().wideScreen == true ? false : applicationWindow().controlsVisible

    contentItem: QtControls.StackView {
        id: sidebarStack;
        implicitWidth: Units.gridUnit * 20
        initialItem: sidebarPage;
    }
    Component {
        id: sidebarPage;
        Item {
            id: localRoot;
            implicitWidth: Units.gridUnit * 20
            property Item topContent: root.topContent;
            property var actions: root.actions;
            property int level: 0
            Item {
                id: topContainer;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    bottom: menu.top;
                }
                children: localRoot.topContent;
            }
            Column {
                id: menu
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }
                height: childrenRect.height;
                Item {
                    height: localRoot.level === 0 ? heading.height : 0;
                    visible: height > 0;
                    width: menu.width
                    Heading {
                        id: heading
                        anchors {
                            left: parent.left
                            right: parent.right
                            margins: Units.largeSpacing
                        }
                        elide: Text.ElideRight
                        level: 2
                        text: root.title
                    }
                }
                Repeater {
                    model: localRoot.actions;
                    delegate: Item {
                        width: menu.width;
                        height: listItemHeader.visible ? listItemHeader.height : (listItem.visible ? listItem.height : (modelData.trigger === undefined ? Units.largeSpacing : 0));
                        Item {
                            id: listItemHeader;
                            anchors { top: parent.top; left: parent.left; }
                            visible: modelData.trigger === undefined && heading.text !== "";
                            width: menu.width
                            height: heading.height;
                            Heading {
                                id: heading
                                anchors {
                                    left: parent.left
                                    right: parent.right
                                    margins: Units.largeSpacing
                                }
                                elide: Text.ElideRight
                                level: 2
                                text: model && model.text ? model.text : (modelData.text ? modelData.text : "")
                            }
                        }
                        BasicListItem {
                            id: listItem;
                            checked: modelData.checked
                            icon: modelData.iconName
                            supportsMouseEvents: true
                            separatorVisible: false
                            label: model ? (model.tooltip ? model.tooltip : model.text) : (modelData.tooltip ? modelData.tooltip : modelData.text)
                            enabled: model && model.enabled !== undefined ? model.enabled : (modelData.enabled !== undefined ? modelData.enabled : true)
                            visible: model && model.visible !== undefined ? model.visible : (modelData.visible !== undefined ? modelData.visible : true)
                            opacity: enabled ? 1.0 : 0.6
                            onClicked: {
                                if (modelData.children!==undefined && modelData.children.length > 0) {
                                    sidebarStack.push(sidebarPage, { actions: modelData.children, "level": level + 1, topContent: null });
                                } else if (modelData && modelData.trigger !== undefined) {
                                    modelData.trigger();
                                // assume the model is a list of QAction or Action
                                } else if (menu.model.length > index) {
                                    menu.model[index].trigger();
                                } else {
                                    console.warning("Don't know how to trigger the action")
                                }
                            }
                            Icon {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.rightMargin: Settings.isMobile ? 0 : Units.gridUnit
                                height: Units.iconSizes.smallMedium
                                selected: listItem.checked || listItem.pressed
                                width: height
                                source: "go-next"
                                visible: modelData.children!==undefined && modelData.children.length > 0
                            }
                        }
                    }
                }
                BasicListItem {
                    visible: level > 0
                    supportsMouseEvents: true
                    icon: "go-previous"
                    label: typeof i18n !== "undefined" ? i18n("Back") : "Back"
                    onClicked: sidebarStack.pop()
                }
            }
        }
    }
}
