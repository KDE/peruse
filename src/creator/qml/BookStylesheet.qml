/*
 * Copyright (C) 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.12 as QtControls

import org.kde.kirigami 2.13 as Kirigami

import org.kde.peruse 0.1 as Peruse
/**
 * @brief the page shows basic information about the book
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "bookStylesheet";
    property QtObject model;
    signal requestCategoryChange(string categoryName);
    title: i18nc("title of the page which lets the user manage the book's stylesheet", "Stylesheet");
    actions {
        main: saveBookAction;
        right: addStyleAction;
    }
    Kirigami.Action {
        id: saveBookAction;
        text: i18nc("Saves the book to a file on disk", "Save Book");
        iconName: "document-save";
        onTriggered: root.model.saveBook();
        enabled: root.model ? root.model.hasUnsavedChanges : false;
    }
    Kirigami.Action {
        id: addStyleAction;
        text: i18nc("Creates a new, empty stylesheet entry and lets the user edit it", "Add Style...");
        iconName: "list-add-font";
        onTriggered: {
            var newStyle = root.model.acbfData.styleSheet.addStyle();
            editStyleSheet.editStyle(newStyle);
        }
    }

    Kirigami.OverlaySheet {
        id: editStyleSheet;
        property QtObject styleObject: null;
        function editStyle(styleObject) {
            editStyleSheet.styleObject = styleObject;
            styleElement.text = styleObject.element;
            open();
        }
        showCloseButton: true
        header: RowLayout {
            Kirigami.Heading {
                text: i18nc("title text for a sheet which lets the user edit a binary entry", "Edit Style");
                Layout.fillWidth: true;
                elide: Text.ElideRight;
            }
            QtControls.ToolButton {
                icon.name: "document-save";
                text: i18nc("label for a button which updates the style entry with the new details", "OK");
                onClicked: {
                    editStyleSheet.styleObject.element = styleElement.text;
                    editStyleSheet.close();
                }
            }
        }
        Kirigami.FormLayout {
            QtControls.TextField {
                id: styleElement;
                Layout.fillWidth: true;
                Kirigami.FormData.label: i18nc("Label for the style element input field", "Element");
                placeholderText: i18nc("Placeholder text for the style element input field", "Enter the name of the element you want to style");
            }
        }
    }

    ListView {
        id: stylesList;
        Layout.fillWidth: true;
        model: root.model.acbfData.styleSheet.styles;
        header: ColumnLayout {
            width: stylesList.width - Kirigami.Units.largeSpacing * 4;
            Item { height: Kirigami.Units.largeSpacing; Layout.fillWidth: true; }
            RowLayout {
                Layout.fillWidth: true;
                Item { height: Kirigami.Units.gridUnit; Layout.fillWidth: true; Layout.minimumWidth: Kirigami.Units.largeSpacing * 2; }
                Kirigami.AbstractCard {
                    header: Kirigami.Heading {
                        text: stylesList.count === 0
                            ? i18nc("title text for a card which informs the user there are no styles defined, and what those are", "No Styling Information")
                            : i18nc("title text for a card which informs the user what styles are", "Styling Information");
                        Layout.fillWidth: true;
                        elide: Text.ElideRight;
                    }
                    contentItem: QtControls.Label {
                        Layout.fillWidth: true;
                        wrapMode: Text.Wrap;
                        text: stylesList.count === 0
                            ? i18nc("Help text for the stylesheet page, when there are no styles defined", "There is no embedded data in your book yet. You can add such data by creating a new entry, and then adding data to that entry from some existing file on your system. That data will be imported into your book, and leaves the external file otherwise untouched.")
                            : i18nc("Help text for the stylesheet page, when there is already styles defined in the book", "You can add such new binary data entries by creating a new entry, and then adding data to that entry from some existing file on your system. That data will be imported into your book, and leaves the external file otherwise untouched.");
                    }
                }
            }
            Item { height: Kirigami.Units.largeSpacing; Layout.fillWidth: true; }
        }
        delegate: Kirigami.BasicListItem {
            id: listItem;
            height: Kirigami.Units.iconSizes.huge + Kirigami.Units.smallSpacing * 2;
            supportsMouseEvents: true;
            onClicked: {
                editStyleSheet.editStyle(modelData);
            }
            icon: "font-select-symbolic";
            label: {
                if (modelData.inverted === true) {
                    if (modelData.type === "") {
                        return i18nc("The title of a style entry which has a type, and which represents the inverted version of that style", "%1 (inverted)", modelData.element);
                    } else {
                        return i18nc("The title of a style entry which does not have type, and which represents the inverted version of that style", "%1 (sub type: %2, inverted)", modelData.element, modelData.type);
                    }
                } else {
                    if (modelData.type === "") {
                        return i18nc("The title of a style entry which has a type", "%1", modelData.element);
                    } else {
                        return i18nc("The title of a style entry which does not have type", "%1 (sub type: %2)", modelData.element, modelData.type);
                    }
                }
            }
            subtitle: modelData.string;
        }
        Rectangle {
            id: processingBackground;
            anchors.fill: parent;
            opacity: root.model && root.model.processing ? 0.5 : 0;
            Behavior on opacity { NumberAnimation { duration: mainWindow.animationDuration; } }
            MouseArea {
                anchors.fill: parent;
                enabled: parent.opacity > 0;
                onClicked: { }
            }
        }
        QtControls.BusyIndicator {
            anchors {
                horizontalCenter: processingBackground.horizontalCenter;
                top: parent.top
                topMargin: x;
            }
            running: processingBackground.opacity > 0;
            visible: running;
        }
    }
}
