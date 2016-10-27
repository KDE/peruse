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

import QtQuick 2.2
import QtQuick.Controls 1.4 as QtControls

import org.kde.kirigami 1.0 as Kirigami

import "metainfoeditors"

Kirigami.Page {
    id: root;
    title: i18nc("title text for the book meta information editor sheet", "Edit Meta Information");
    property QtObject model;

    actions {
        main: saveAndCloseAction;
    }
    Kirigami.Action {
        id: saveAndCloseAction;
        text: i18nc("Saves the remaining unsaved edited fields and closes the metainfo editor", "Save and Close Editor");
        iconName: "dialog-ok";
        onTriggered: {
            root.model.setDirty();
            pageStack.pop();
        }
    }

    Column {
        id: contentColumn;
        width: root.width - (root.leftPadding + root.rightPadding);
        height: childrenRect.height;
        spacing: Kirigami.Units.smallSpacing;
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the book title", "Titles");
        }
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.titleLanguages : 0;
            delegate: LanguageTextEntryEditor {
                width: contentColumn.width;
                title: modelData === "" ? i18nc("label text for the the book title with no language (default)", "Default title") : modelData;
                text: root.model.acbfData.metaData.bookInfo.title(modelData);
                onSaveRequested: {
                    root.model.acbfData.metaData.bookInfo.setTitle(text, modelData);
                    root.model.setDirty();
                }
                onRemoveRequested: {
                    root.model.acbfData.metaData.bookInfo.setTitle("", modelData);
                    root.model.setDirty();
                }
                removePossible: title === modelData;
            }
        }
        QtControls.Button {
            anchors.right: parent.right;
            width: parent.with;
            iconName: "list-add";
            text: i18nc("Text on the button for adding new titles", "Add a title in another language");
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the genre list", "Genres");
        }
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.genres : 0;
            delegate: Item {
                width: parent.width;
                height: childrenRect.height;
                QtControls.TextField {
                    id: genreText;
                    width: parent.width - removeGenreButton.width - Kirigami.Units.smallSpacing;
                    text: modelData;
                    QtControls.Button {
                        id: removeGenreButton;
                        anchors {
                            left: parent.right;
                            leftMargin: Kirigami.Units.smallSpacing;
                        }
                        iconName: "list-remove";
                        height: parent.height;
                        width: height;
                        onClicked: {
                            root.model.acbfData.metaData.bookInfo.removeGenre(modelData);
                            root.model.setDirty();
                        }
                    }
                }
                QtControls.Slider {
                    anchors {
                        top: genreText.bottom;
                        topMargin: Kirigami.Units.smallSpacing;
                    }
                    minimumValue: 0;
                    maximumValue: 100;
                    stepSize: 1.0;
                    width: genreText.width;
                    value: root.model.acbfData.metaData.bookInfo.genrePercentage(modelData);
                    onValueChanged: {
                        if(value > 0 && value !== root.model.acbfData.metaData.bookInfo.genrePercentage(modelData)) {
                            root.model.acbfData.metaData.bookInfo.setGenre(modelData, value);
                            root.model.setDirty();
                        }
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addCharacterButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the add new genre text entry", "Write to add new genre");
            QtControls.Button {
                id: addGenreButton;
                anchors {
                    left: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                iconName: "list-add";
                height: parent.height;
                width: height;
                onClicked: {
                    if(parent.text !== "") {
                        root.model.acbfData.metaData.bookInfo.setGenre(parent.text);
                        root.model.setDirty();
                        parent.text = "";
                    }
                }
            }
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the character list", "Characters");
        }
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.characters : 0;
            delegate: QtControls.TextField {
                width: parent.width - removeCharacterButton.width - Kirigami.Units.smallSpacing;
                text: modelData;
                QtControls.Button {
                    id: removeCharacterButton;
                    anchors {
                        left: parent.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    iconName: "list-remove";
                    height: parent.height;
                    width: height;
                    onClicked: {
                        root.model.acbfData.metaData.bookInfo.removeCharacter(modelData);
                        root.model.setDirty();
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addCharacterButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the add new character text entry", "Write to add new character");
            QtControls.Button {
                id: addCharacterButton;
                anchors {
                    left: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                iconName: "list-add";
                height: parent.height;
                width: height;
                onClicked: {
                    if(parent.text !== "") {
                        root.model.acbfData.metaData.bookInfo.addCharacter(parent.text);
                        root.model.setDirty();
                        parent.text = "";
                    }
                }
            }
        }
    }
}
