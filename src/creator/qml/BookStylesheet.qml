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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QtControls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.peruse as Peruse
import org.kde.peruse.acbf as Acbf

/**
 * @brief the page shows basic information about the book
 */
Kirigami.ScrollablePage {
    id: root

    property string categoryName: "bookStylesheet"
    property QtObject model

    signal requestCategoryChange(string categoryName)

    title: i18nc("title of the page which lets the user manage the book's stylesheet", "Stylesheet")
    actions: [
        Kirigami.Action {
            id: saveBookAction
            text: i18nc("Saves the book to a file on disk", "Save Book")
            icon.name: "document-save-symbolic"
            onTriggered: root.model.saveBook();
            enabled: root.model?.hasUnsavedChanges ?? false
        },
        Kirigami.Action {
            id: addStyleAction
            text: i18nc("Creates a new, empty stylesheet entry and lets the user edit it", "Add Style...")
            icon.name: "list-add-font-symbolic"
            onTriggered: {
                const newStyle = root.model.acbfData.styleSheet.addStyle();
                editStyleSheet.editStyle(newStyle);
            }
        }
    ]

    FormCard.FormCardDialog {
        id: editStyleSheet

        property Acbf.Style styleObject: null;

        function editStyle(styleObject: Acbf.Style): void {
            editStyleSheet.styleObject = styleObject;
            styleElement.currentIndex = styleElement.indexOfValue(styleObject.element);
            if (styleElement.currentIndex === -1) {
                styleElement.currentIndex = 0;
            }
            inverted.checked = styleObject.inverted;
            color.color = styleObject.color;
            type.currentIndex = type.indexOfValue(styleObject.type);
            if (type.currentIndex === -1) {
                type.currentIndex = 0;
            }
            open();
        }

        title: i18nc("@title:dialog", "Edit Style")

        standardButtons: QtControls.Dialog.Cancel | QtControls.Dialog.Save
        implicitWidth: Math.min(parent.width - Kirigami.Units.gridUnit * 2, Kirigami.Units.gridUnit * 25)

        onRejected: close()
        onAccepted: {
            styleObject.element = styleElement.currentValue;
            styleObject.inverted = inverted.checked;
            styleObject.color = color.color;
            close();
        }

        FormCard.FormComboBoxDelegate {
            id: styleElement
            text: i18nc("Label for the style element input field", "Element");
            model: ["*", "text-area", "emphasis", "strong"]
        }

        FormCard.FormSwitchDelegate {
            id: inverted
            text: i18nc("@option:check", "Apply to style elements which request an inverted scheme");
        }

        FormCard.FormComboBoxDelegate {
            id: type
            text: i18nc("@option:check", "Sub-element selector type");
            model: [i18n("None"), "code", "letter", "commentary", "formal", "heading", "audio", "thought", "sign"]
        }

        FormCard.FormColorDelegate {
            id: color
            text: i18nc("@option:check", "Color");
        }
    }

    ListView {
        id: stylesList

        model: root.model.acbfData.styleSheet.styles
        delegate: Delegates.RoundedItemDelegate {
            id: listItem
            onClicked: {
                editStyleSheet.editStyle(modelData);
            }
            icon.name: "font-select-symbolic"

            contentItem: Delegates.SubtitleContentItem {
                itemDelegate: listItem
                subtitle: modelData.string
            }

            text: {
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
        }

        Kirigami.PlaceholderMessage {
            text: i18nc("@info:placeholder", "No Styling Information")
            explanation: i18nc("@info:placeholder", "You can add such data by creating a new entry, and then adding data to that entry from some existing file on your system. That data will be imported into your book, and leaves the external file otherwise untouched.")
            visible: stylesList.count === 0
            width: parent.width - Kirigami.Units.gridUnit * 4
            anchors.centerIn: parent
            helpfulAction: addStyleAction
        }
    }
}
