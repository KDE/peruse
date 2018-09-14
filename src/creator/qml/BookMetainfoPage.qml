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
                        // When removing, set the model dirty first, and then remove the entry to avoid reference errors.
                        root.model.setDirty();
                        root.model.acbfData.metaData.bookInfo.removeAuthor(index);
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addAuthorButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the add new author text entry", "Write to add new author (nickname)");
            Keys.onReturnPressed: addAuthor();
            function addAuthor() {
                if(text !== "") {
                    // Just add an author where only the nickname is defined
                    root.model.acbfData.metaData.bookInfo.addAuthor("", "", "", "", "", text, [""], [""]);
                    root.model.setDirty();
                    text = "";
                }
            }

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
                onClicked: parent.addAuthor();
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
                QtControls.Label {
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
                            root.model.setDirty();
                            root.model.acbfData.metaData.bookInfo.removeGenre(modelData);
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
        QtControls.ComboBox {
            width: parent.width - addGenreButton.width - Kirigami.Units.smallSpacing;
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.availableGenres().filter(checkGenreInUse) : 0;
            Keys.onReturnPressed: addGenre();
            function addGenre() {
                root.model.acbfData.metaData.bookInfo.setGenre(currentText);
                root.model.setDirty();
                currentIndex=0;
            }
            function checkGenreInUse (genre) {
                return root.model.acbfData.metaData.bookInfo.genres.indexOf(genre) === -1;
            }

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
                onClicked: parent.addGenre();
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
                        root.model.setDirty();
                        root.model.acbfData.metaData.bookInfo.removeCharacter(modelData);
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addCharacterButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the add new character text entry", "Write to add new character");
            Keys.onReturnPressed: addCharacter();
            function addCharacter() {
                if(text !== "") {
                    root.model.acbfData.metaData.bookInfo.addCharacter(text);
                    root.model.setDirty();
                    text = "";
                }
            }

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
                onClicked: parent.addCharacter();
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
                    if (numberField.value !== root.model.acbfData.metaData.bookInfo.sequence(modelData).number) {
                        root.model.acbfData.metaData.bookInfo.sequence(modelData).number = numberField.value;
                    }
                    if (volumeField.value !== root.model.acbfData.metaData.bookInfo.sequence(modelData).volume) {
                        root.model.acbfData.metaData.bookInfo.sequence(modelData).volume = volumeField.value;
                    }
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
                        root.model.setDirty();
                        root.model.acbfData.metaData.bookInfo.removeSequence(index);
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addSequenceButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the add new series text entry", "Write to add new series");
            Keys.onReturnPressed:addSequence();
            function addSequence() {
                if(text !== "") {
                    root.model.acbfData.metaData.bookInfo.addSequence(0, text);
                    root.model.setDirty();
                    text = "";
                }
            }

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
                onClicked: parent.addSequence();
            }
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the database reference list", "Database References");
        }
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.databaseRefCount : 0;
            delegate: Item {
                width: parent.width;
                height: childrenRect.height;

                function updateDatabaseRef() {
                    root.model.acbfData.metaData.bookInfo.databaseRef(modelData).reference = referenceTextField.text
                    root.model.acbfData.metaData.bookInfo.databaseRef(modelData).dbname = databaseNameField.text
                    root.model.acbfData.metaData.bookInfo.databaseRef(modelData).type = referenceTypeField.text
                    root.model.setDirty();
                }

                QtControls.TextField {
                    id: referenceTextField;
                    width: parent.width - removeReferenceButton.width - Kirigami.Units.smallSpacing;
                    text: root.model.acbfData.metaData.bookInfo.databaseRef(modelData).reference;
                    onEditingFinished: parent.updateDatabaseRef();
                }
                QtControls.TextField {
                    anchors {
                        top: referenceTextField.bottom;
                        topMargin: Kirigami.Units.smallSpacing;
                    }
                    width : (referenceTextField.width+Kirigami.Units.smallSpacing)/2;
                    id: databaseNameField;
                    text: root.model.acbfData.metaData.bookInfo.databaseRef(modelData).dbname;
                    onEditingFinished: parent.updateDatabaseRef();
                }
                QtControls.TextField {
                    anchors {
                        left: databaseNameField.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                        top: referenceTextField.bottom;
                        topMargin: Kirigami.Units.smallSpacing;
                    }
                    width : (referenceTextField.width/2)-(Kirigami.Units.smallSpacing*1.5);
                    id: referenceTypeField;
                    text: root.model.acbfData.metaData.bookInfo.databaseRef(modelData).type;
                    placeholderText: i18nc("placeholder text for the add reference type text entry", "Write to add reference type");
                    onEditingFinished: parent.updateDatabaseRef();
                }
                QtControls.Button {
                    id: removeReferenceButton;
                    anchors {
                        left: referenceTextField.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "list-remove";
                    }
                    height: referenceTextField.height;
                    width: height;
                    onClicked: {
                        root.model.setDirty();
                        root.model.acbfData.metaData.bookInfo.removeDatabaseRef(index);
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        Item {
             width: parent.width;
             height: childrenRect.height;
            function addReference() {
                if(addReferenceField.text !== "" && addDatabaseNameField.text !== "") {
                    root.model.acbfData.metaData.bookInfo.addDatabaseRef(addReferenceField.text, addDatabaseNameField.text);
                    root.model.setDirty();
                    addReferenceField.text = "";
                    addDatabaseNameField.text = "";
                }
            }

            QtControls.TextField {
                id: addReferenceField
                width: parent.width - addReferenceButton.width - Kirigami.Units.smallSpacing;
                placeholderText: i18nc("placeholder text for the add new reference text entry", "Write to add new reference");
                Keys.onReturnPressed: parent.addReference();
            }
            QtControls.TextField {
                id: addDatabaseNameField
                anchors {
                    top: addReferenceField.bottom;
                    topMargin: Kirigami.Units.smallSpacing;
                }
                width: parent.width - addReferenceButton.width - Kirigami.Units.smallSpacing;
                placeholderText: i18nc("placeholder text for the add databasename text entry", "Write to add database name for new reference.");
                Keys.onReturnPressed: parent.addReference();
            }
            QtControls.Button {
                id: addReferenceButton;
                anchors {
                    left: addReferenceField.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "list-add";
                }
                height: addReferenceField.height;
                width: height;
                onClicked: parent.addReference();
            }
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the edit field for the content rating list", "Content Ratings");
        }
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.contentRatingCount : 0;
            delegate: Item {
                width: parent.width;
                height: childrenRect.height;

                function updateRating() {
                    root.model.acbfData.metaData.bookInfo.contentRating(modelData).rating = ratingNameField.text
                    root.model.acbfData.metaData.bookInfo.contentRating(modelData).type = systemNameField.text
                    root.model.setDirty();
                }

                QtControls.TextField {
                    width : (parent.width-removeRatingButton.width+Kirigami.Units.smallSpacing)/2;
                    id: ratingNameField;
                    text: root.model.acbfData.metaData.bookInfo.contentRating(modelData).rating;
                    onEditingFinished: parent.updateRating();
                }
                QtControls.TextField {
                    anchors {
                        left: ratingNameField.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    width : ((parent.width-removeRatingButton.width)/2)-(Kirigami.Units.smallSpacing*1.5);
                    id: systemNameField;
                    text: root.model.acbfData.metaData.bookInfo.contentRating(modelData).type;
                    placeholderText: i18nc("placeholder text for the add reference type text entry", "Write to add reference type");
                    onEditingFinished: parent.updateRating();
                }
                QtControls.Button {
                    id: removeRatingButton;
                    anchors {
                        left: systemNameField.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "list-remove";
                    }
                    height: systemNameField.height;
                    width: height;
                    onClicked: {
                        root.model.setDirty();
                        root.model.acbfData.metaData.bookInfo.removeContentRating(index);
                    }
                }
            }
        }
        Item {
            width: parent.width;
            height: childrenRect.height;
            function addRating() {
                if(addRatingField.text !== "" && addSystemField.text !== "") {
                    root.model.acbfData.metaData.bookInfo.addContentRating(addRatingField.text, addSystemField.text);
                    root.model.setDirty();
                    addRatingField.text = "";
                    addSystemField.text = "";
                }
            }
                QtControls.TextField {
                    width : (parent.width-addRatingButton.width+Kirigami.Units.smallSpacing)/2;
                    id: addRatingField;
                    placeholderText: i18nc("placeholder text for the add content rating text entry", "Write to add rating label.");
                    onEditingFinished: parent.addRating();
                }
                QtControls.TextField {
                    anchors {
                        left: addRatingField.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    width : ((parent.width-addRatingButton.width)/2)-(Kirigami.Units.smallSpacing*1.5);
                    id: addSystemField;
                    placeholderText: i18nc("placeholder text for the add content rating system text entry", "Write to add rating system.");
                    onEditingFinished: parent.addRating();
                }
                QtControls.Button {
                    id: addRatingButton;
                    anchors {
                        left: addSystemField.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "list-add";
                    }
                    height: addSystemField.height;
                    width: height;
                    onClicked: parent.addRating();
                }
            }
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the form for the publishing info list", "Publisher Info");
        }
        QtControls.TextField {
            width : parent.width;
            id: publisher;
            placeholderText: i18nc("placeholder text for the publisher entry", "Write to add publisher");
            text: root.model.acbfData? root.model.acbfData.metaData.publishInfo.publisher: "";
            onEditingFinished: {
                if (root.model.acbfData && text !=="") {
                    root.model.acbfData.metaData.publishInfo.publisher = text
                    root.model.setDirty();
                }
            }
        }
        Item {
            width : parent.width;
            id: publishingDate;
            height: childrenRect.height;
            property date publishingDate: root.model.acbfData.metaData.publishInfo.publishDate;
            function changePublishDate() {
                root.model.acbfData.metaData.publishInfo.setPublishDateFromInts(pdYear.value, (pdMonth.currentIndex+1), pdDate.value);
                root.model.setDirty();
            }
            QtControls.SpinBox {
                id: pdYear
                width: (parent.width-(Kirigami.Units.smallSpacing*2))/3;
                value: parent.publishingDate.getFullYear();
                onValueChanged: parent.changePublishDate();
                editable: true;
                from: 0;
                to: 9999;
            }
            QtControls.ComboBox {
                id: pdMonth
                anchors {
                    left: pdYear.right;
                    margins: Kirigami.Units.smallSpacing;
                }
                model: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
                width: (parent.width-(Kirigami.Units.smallSpacing*2))/3;
                currentIndex: parent.publishingDate.getMonth();
                displayText: Qt.locale().monthName(currentText, Locale.LongFormat);
                onActivated: parent.changePublishDate();
                delegate: QtControls.ItemDelegate {
                    text:Qt.locale().monthName(modelData, Locale.LongFormat);
                }
            }
            QtControls.SpinBox {
                id: pdDate
                anchors {
                    left: pdMonth.right;
                    margins: Kirigami.Units.smallSpacing;
                }
                width: (parent.width-(Kirigami.Units.smallSpacing*2))/3;
                height: pdMonth.height;
                from: 1;
                to: 31;
                editable: true;
                value: parent.publishingDate.getDate();
                onValueChanged: parent.changePublishDate();
            }
        }
        QtControls.TextField {
            width : parent.width;
            id: city;
            placeholderText: i18nc("placeholder text for the publishing city entry", "Write to add city");
            text: root.model.acbfData? root.model.acbfData.metaData.publishInfo.city: "";
            onEditingFinished: {
                if (root.model.acbfData && text !=="") {
                    root.model.acbfData.metaData.publishInfo.city = text ;
                    root.model.setDirty();
                }
            }
        }
        QtControls.TextField {
            width : parent.width;
            id: isbn;
            placeholderText: i18nc("placeholder text for the publishing isbn entry", "Write to add isbn");
            text: root.model.acbfData? root.model.acbfData.metaData.publishInfo.isbn: "";
            onEditingFinished: {
                if (root.model.acbfData && text !=="") {
                    root.model.acbfData.metaData.publishInfo.isbn = text
                    root.model.setDirty();
                }
            }
        }
        QtControls.TextField {
            width : parent.width;
            id: license;
            placeholderText: i18nc("placeholder text for the publishing license entry", "Write to add license");
            text: root.model.acbfData? root.model.acbfData.metaData.publishInfo.license: "";
            onEditingFinished: {
                if (root.model.acbfData && text !=="") {
                    root.model.acbfData.metaData.publishInfo.license = text
                    root.model.setDirty();
                }
            }
        }
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the form for the document info list", "Document Authors");
        }

        Repeater {
            id: docAuthorRepeater;
            model: root.model.acbfData ? root.model.acbfData.metaData.documentInfo.authorNames : 0;
            delegate: QtControls.Label {
                width: parent.width - removeDocAuthorButton.width - Kirigami.Units.smallSpacing;
                text: modelData.length > 0 ? modelData : "(unnamed)";
                QtControls.Button {
                    id: removeDocAuthorButton;
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
                        // When removing, set the model dirty first, and then remove the entry to avoid reference errors.
                        root.model.setDirty();
                        root.model.acbfData.metaData.documentInfo.removeAuthor(index);
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addDocAuthorButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the add new author text entry", "Write to add new author (nickname)");
            Keys.onReturnPressed: addAuthor();
            function addAuthor() {
                if(text !== "") {
                    // Just add an author where only the nickname is defined
                    root.model.acbfData.metaData.documentInfo.addAuthor("", "", "", "", "", text, [""], [""]);
                    root.model.setDirty();
                    text = "";
                }
            }

            QtControls.Button {
                id: addDocAuthorButton;
                anchors {
                    left: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "list-add";
                }
                height: parent.height;
                width: height;
                onClicked: parent.addAuthor();
            }
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the form for the sources list", "Document Source");
        }

        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.documentInfo.source : 0;
            delegate: Item {
                width: parent.width;
                height: childrenRect.height;
                QtControls.TextField {
                    id: sourceText;
                    width: parent.width - removeSourceButton.width - Kirigami.Units.smallSpacing;
                    text: modelData;
                    onEditingFinished: root.model.acbfData.metaData.documentInfo.sources[index] = text;
                    QtControls.Button {
                        id: removeSourceButton;
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
                            root.model.setDirty();
                            root.model.acbfData.metaData.documentInfo.removeSource(index);
                        }
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addSourceButton.width - Kirigami.Units.smallSpacing;
            Keys.onReturnPressed: addEntry();
            function addEntry() {
                if (text !== "") {
                    root.model.acbfData.metaData.documentInfo.source.push(text);
                    root.model.setDirty();
                    text = "";
                }
            }

            QtControls.Button {
                id: addSourceButton;
                anchors {
                    left: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "list-add";
                }
                height: parent.height;
                width: height;
                onClicked: parent.addEntry();
            }
        }

        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight + Kirigami.Units.smallSpacing * 2;
            text: i18nc("label text for the form for the history list", "Document History");
        }

        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.documentInfo.history : 0;
            delegate: Item {
                width: parent.width;
                height: childrenRect.height;
                QtControls.TextField {
                    id: historyText;
                    width: parent.width - removeHistoryButton.width - Kirigami.Units.smallSpacing;
                    text: modelData;
                    onEditingFinished: root.model.acbfData.metaData.documentInfo.history[index] = text;
                    QtControls.Button {
                        id: removeHistoryButton;
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
                            root.model.setDirty();
                            root.model.acbfData.metaData.documentInfo.removeHistoryLine(index);
                        }
                    }
                }
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        QtControls.TextField {
            width: parent.width - addHistoryButton.width - Kirigami.Units.smallSpacing;
            Keys.onReturnPressed: addEntry();
            function addEntry() {
                if (text !== "") {
                    root.model.acbfData.metaData.documentInfo.history.push(text);
                    root.model.setDirty();
                    text = "";
                }
            }

            QtControls.Button {
                id: addHistoryButton;
                anchors {
                    left: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "list-add";
                }
                height: parent.height;
                width: height;
                onClicked: parent.addEntry();
            }
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
        Item {
            width: parent.width;
            height: childrenRect.height;
            QtControls.Label {
                id: versionLabel;
                height: versionSpinBox.height;
                text: i18nc("Label for the document version spinbox","Document Version");
            }
            QtControls.SpinBox {
                id: versionSpinBox;
                anchors {
                    top: versionLabel.top;
                    left: versionLabel.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }

                width: parent.width - (Kirigami.Units.smallSpacing*2) - versionLabel.width - addHistoryButton.width;
                value: root.model.acbfData.metaData.documentInfo.version;

                onValueChanged: {
                    if (root.model.acbfData.metaData.documentInfo.version!==value) {
                        root.model.acbfData.metaData.documentInfo.version = value;
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
