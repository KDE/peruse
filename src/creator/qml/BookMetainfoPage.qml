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
import QtQuick.Controls 2.2 as QtControls

import org.kde.kirigami 2.1 as Kirigami

import "metainfoeditors"
/**
 * @brief Page with form to edit the comic metadata.
 * 
 * Most metadata entries are quite simple.
 * 
 * Others, like Author, need a dedicated entry editor (AuthorEntryEditor).
 */
Kirigami.ScrollablePage {
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
            // Save the default title/annotation/keywords.
            root.model.acbfData.metaData.bookInfo.setTitle(defaultTitle.text, "");
            root.model.acbfData.metaData.bookInfo.setAnnotation(defaultAnnotation.text.split("\n\n"), "");
            var keywords = defaultKeywords.text.split(",")
            for (var i in keywords) {
                keywords[i] = keywords[i].trim();
            }
            root.model.acbfData.metaData.bookInfo.setKeywords(keywords, "");

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
            text: i18nc("label text for the edit field for the book title", "Title");
        }

        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            id: defaultTitle;
            width: parent.width;
            placeholderText: i18nc("placeholder text for default title text-input", "Write to add default title");
            text:root.model.acbfData ? root.model.acbfData.metaData.bookInfo.title("") : "";
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the default annotation", "Annotation");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextArea {
            id: defaultAnnotation;
            width: parent.width;
            placeholderText: i18nc("placeholder text for default annotiation text-area", "Write to add default annotation");
            text:root.model.acbfData ? root.model.acbfData.metaData.bookInfo.annotation("").join("\n\n") : "";
        }
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the keyword list", "Keywords");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            id: defaultKeywords;
            width: parent.width;
            placeholderText: i18nc("placeholder text for the add new keyword text entry", "Write a comma seperated list of keywords.");
            text:root.model.acbfData ? root.model.acbfData.metaData.bookInfo.keywords("").join(", ") : "";
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the author list", "Authors (" + authorRepeater.count + ")");
        }
        Repeater {
            id: authorRepeater;
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.authorNames : 0;
            delegate: QtControls.Label {
                width: parent.width - removeAuthorButton.width - Kirigami.Units.smallSpacing;
                text: modelData.length > 0 ? modelData : "(unnamed)";
                QtControls.Button {
                    id: editAuthorButton;
                    anchors {
                        right: removeAuthorButton.left;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "document-edit";
                    }
                    height: parent.height;
                    width: height;
                    onClicked: {
                        authorEditor.index = model.index;
                        authorEditor.open();
                    }
                }
                QtControls.Button {
                    id: removeAuthorButton;
                    anchors {
                        left: parent.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "list-remove";
                    }
                    height: parent.height;
                    width: height;
                    onClicked: {
                        root.model.acbfData.metaData.bookInfo.removeAuthor(modelData);
                        root.model.setDirty();
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addAuthorButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the add new author text entry", "Write to add new author (nickname)");
            QtControls.Button {
                id: addAuthorButton;
                anchors {
                    left: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "list-add";
                }
                height: parent.height;
                width: height;
                onClicked: {
                    if(parent.text !== "") {
                        // Just add an author where only the nickname is defined
                        root.model.acbfData.metaData.bookInfo.addAuthor("", "", "", "", "", parent.text, [""], [""]);
                        root.model.setDirty();
                        parent.text = "";
                    }
                }
            }
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
                        contentItem: Kirigami.Icon {
                            source: "list-remove";
                        }
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
                    from: 0;
                    to: 100;
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
                contentItem: Kirigami.Icon {
                    source: "list-add";
                }
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
                onEditingFinished: root.model.acbfData.metaData.bookInfo.characters[index] = text;
                QtControls.Button {
                    id: removeCharacterButton;
                    anchors {
                        left: parent.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "list-remove";
                    }
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
                contentItem: Kirigami.Icon {
                    source: "list-add";
                }
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

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the sequence list", "Sequence");
        }
        Repeater {
            id: sequenceListRepeater;
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.sequenceCount : 0;
            delegate: Item {
                width: parent.width;
                height: childrenRect.height;

                function updateSeries() {
                    root.model.acbfData.metaData.bookInfo.sequence(modelData).title = seriesTextField.text;
                    root.model.acbfData.metaData.bookInfo.sequence(modelData).number = numberField.value;
                    root.model.acbfData.metaData.bookInfo.sequence(modelData).volume = volumeField.value;
                    root.model.setDirty();
                }

                QtControls.TextField {
                    id: seriesTextField;
                    width: parent.width - removeSequenceButton.width - Kirigami.Units.smallSpacing;
                    text: root.model.acbfData.metaData.bookInfo.sequence(modelData).title;
                    onEditingFinished: parent.updateSeries();
                }
                QtControls.SpinBox {
                    anchors {
                        top: seriesTextField.bottom;
                        topMargin: Kirigami.Units.smallSpacing;
                    }
                    value : root.model.acbfData.metaData.bookInfo.sequence(modelData).number;
                    width : (seriesTextField.width+Kirigami.Units.smallSpacing)/2;
                    id: numberField;
                    onValueChanged: parent.updateSeries();
                }
                QtControls.SpinBox {
                    anchors {
                        left: numberField.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                        top: seriesTextField.bottom;
                        topMargin: Kirigami.Units.smallSpacing;
                    }
                    value : root.model.acbfData.metaData.bookInfo.sequence(modelData).volume;
                    width : (seriesTextField.width/2)-(Kirigami.Units.smallSpacing*1.5);
                    id: volumeField;
                    onValueChanged: parent.updateSeries();
                }
                QtControls.Button {
                    id: removeSequenceButton;
                    anchors {
                        left: seriesTextField.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "list-remove";
                    }
                    height: seriesTextField.height;
                    width: height;
                    onClicked: {
                        root.model.acbfData.metaData.bookInfo.removeSequence(modelData);
                        root.model.setDirty();
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addSequenceButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the add new series text entry", "Write to add new series");
            QtControls.Button {
                id: addSequenceButton;
                anchors {
                    left: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "list-add";
                }
                height: parent.height;
                width: height;
                onClicked: {
                    if(parent.text !== "") {
                        root.model.acbfData.metaData.bookInfo.addSequence(0, parent.text);
                        root.model.setDirty();
                        parent.text = "";
                    }
                }
            }
        }

        AuthorEntryEditor {
            id: authorEditor;
            bookinfo: root.model.acbfData.metaData.bookInfo;
            onSave: {
                root.model.acbfData.metaData.bookInfo.setAuthor(index, activity, language, firstName, middleName, lastName, nickName, [homePage], [email]);
                root.model.setDirty();
            }
        }
    }
}
