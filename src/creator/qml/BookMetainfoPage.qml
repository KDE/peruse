// SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

import "metainfoeditors"
/**
 * @brief Page with form to edit the comic metadata.
 *
 * Most metadata entries are quite simple.
 *
 * Others, like Author, need a dedicated entry editor (AuthorEntryEditor).
 */
FormCard.FormCardPage {
    id: root

    property string categoryName: "bookMetaInfo"
    property QtObject model

    title: i18nc("title text for the book meta information editor sheet", "Edit Meta Information")

    actions: saveAction

    Kirigami.Action {
        id: saveAction;
        text: i18nc("Saves the book to a file on disk", "Save Book");
        icon.name: "document-save";
        onTriggered: {
            // Ensure there's a default language entry.
            if (root.model.acbfData.metaData.bookInfo.languageEntryList.indexOf("") === -1) {
                root.model.acbfData.metaData.bookInfo.addLanguage("");
            }
            root.model.saveBook();
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormTextFieldDelegate {
            id: defaultTitle

            label: i18nc("@label:textfield", "Title")
            text: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.title("") : "";
            onTextChanged: {
                root.model.acbfData.metaData.bookInfo.setTitle(defaultTitle.text, "");
                root.model.setDirty();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: defaultAnnotation

            label: i18nc("@label:textfield", "Annotation")
            text: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.annotation("").join("\n\n") : "";
            onTextChanged: {
                root.model.acbfData.metaData.bookInfo.setAnnotation(defaultAnnotation.text.split("\n\n"), "");
                root.model.setDirty();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: keywords

            label: i18nc("@label:textfield", "Keywords")
            placeholderText: i18nc("@info:placeholder", "Write a comma separated list of keywords.");

            text: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.keywords("").join(", ") : "";
            onTextChanged: {
                const keywords = defaultKeywords.text.split(",").map(word => word.trim())
                root.model.acbfData.metaData.bookInfo.setKeywords(keywords, "");
                root.model.setDirty();
            }
        }

        FormCard.FormSpinBoxDelegate {
            id: versionSpinBox

            readonly property int decimals: 2;
            readonly property real realValue: value / 100;

            label: i18nc("Label for the document version spinbox", "Document Version:")
            from:0;
            to: 100 * 100;
            stepSize: 100;

            validator: DoubleValidator {
                bottom: Math.min(versionSpinBox.from, versionSpinBox.to)
                top: Math.max(versionSpinBox.from, versionSpinBox.to)
            }

            textFromValue: function(value, locale) {
                return Number(value / 100).toLocaleString(locale, 'f', decimals)
            }

            valueFromText: function(text, locale) {
                return Number.fromLocaleString(locale, text) * 100
            }
            value: root.model.acbfData.metaData.documentInfo.version * 100;

            onFocusChanged: {
                if (root.model.acbfData.metaData.documentInfo.version * 100 !== value) {
                    root.model.acbfData.metaData.documentInfo.version = value/100;
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Authors")
    }

    FormCard.FormCard {
        Repeater {
            id: authorRepeater;
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.authorNames : 0;
            delegate: FormCard.AbstractFormDelegate {
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.Label {
                        text: modelData.length > 0 ? modelData : i18nc("The text used in place of an author's name if one has not been set", "(unnamed)")
                        Layout.fillWidth: true
                    }

                    Controls.Button {
                        id: editAuthorButton
                        icon.name: "document-edit-symbolic"
                        text: i18nc("@action:button", "Edit author")
                        display: Controls.Button.IconOnly
                        onClicked: {
                            const window = root.Controls.ApplicationWindow.window.pageStack.pushDialogLayer(authorEditor, {
                                index: model.index,
                            });
                        }
                    }

                    Controls.Button {
                        id: removeAuthorButton
                        icon.name: "list-remove-symbolic"
                        text: i18nc("@action:button", "Remove author")
                        display: Controls.Button.IconOnly
                        onClicked: {
                            // When removing, set the model dirty first, and then remove the entry to avoid reference errors.
                            root.model.setDirty();
                            root.model.acbfData.metaData.bookInfo.removeAuthor(index);
                        }
                    }
                }

                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }

        FormCard.FormTextFieldDelegate {
            id: newAuthorField

            label: i18nc("@label:textfield", "New author (nickname)")
            onAccepted: addAuthor()
            function addAuthor(): void {
                if (text.trim().length > 0) {
                    // Just add an author where only the nickname is defined
                    root.model.acbfData.metaData.bookInfo.addAuthor("", "", "", "", "", text, [""], [""]);
                    root.model.setDirty();
                    text = "";
                }
            }

            trailing: Controls.Button {
                icon.name: "list-add-symbolic"
                text: i18nc("@action:button", "Add new author")
                display: Controls.Button.IconOnly
                onClicked: newAuthorField.addAuthor();
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Genres")
    }

    FormCard.FormCard {
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.genres : 0;
            delegate: FormCard.AbstractFormDelegate {
                id: genreDelegate

                required property var modelData

                contentItem: ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.fillWidth: true

                        Controls.Label {
                            text: genreDelegate.modelData;
                            Layout.fillWidth: true
                        }

                        Controls.Button {
                            icon.name: "list-remove-symbolic"
                            text: i18nc("@action:button", "Remove genre")
                            display: Controls.Button.IconOnly
                            onClicked: {
                                root.model.setDirty();
                                root.model.acbfData.metaData.bookInfo.removeGenre(genreDelegate.modelData);
                            }
                        }
                    }

                    Controls.Slider {
                        from: 0;
                        to: 100;
                        stepSize: 1.0;
                        width: genreText.width;
                        value: root.model.acbfData.metaData.bookInfo.genrePercentage(genreDelegate.modelData);
                        onValueChanged: {
                            if(value > 0 && value !== root.model.acbfData.metaData.bookInfo.genrePercentage(genreDelegate.modelData)) {
                                root.model.acbfData.metaData.bookInfo.setGenre(genreDelegate.modelData, value);
                                root.model.setDirty();
                            }
                        }

                        Layout.fillWidth: true
                    }
                }

                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }

        FormCard.AbstractFormDelegate {
            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Controls.ComboBox {
                    id: addGenreCombo

                    model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.availableGenres().filter(checkGenreInUse) : 0;
                    onAccepted: addGenre();
                    Layout.fillWidth: true

                    function addGenre(): void {
                        root.model.acbfData.metaData.bookInfo.setGenre(currentText);
                        root.model.setDirty();
                        currentIndex = 0;
                    }

                    function checkGenreInUse(genre: string): bool {
                        return root.model.acbfData.metaData.bookInfo.genres.indexOf(genre) === -1;
                    }
                }

                Controls.Button {
                    id: addGenreButton
                    icon.name: "list-add-symbolic"
                    onClicked: addGenreCombo.addGenre();
                }
            }
            background: null
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Characters")
    }

    FormCard.FormCard {
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.characters : 0;
            delegate: FormCard.AbstractFormDelegate {
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.TextField {
                        text: modelData
                        onEditingFinished: root.model.acbfData.metaData.bookInfo.characters[index] = text;
                        Layout.fillWidth: true
                    }

                    Controls.Button {
                        id: removeCharacterButton
                        icon.name: "list-remove-symbolic"
                        onClicked: {
                            root.model.setDirty();
                            root.model.acbfData.metaData.bookInfo.removeCharacter(modelData);
                        }
                    }
                }
                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }

        FormCard.AbstractFormDelegate {
            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Layout.fillWidth: true

                Controls.TextField {
                    id: newCharacterField
                    placeholderText: i18nc("placeholder text for the add new character text entry", "Write to add new character");
                    onAccepted: addCharacter();
                    Layout.fillWidth: true

                    function addCharacter(): void {
                        if (text !== "") {
                            root.model.acbfData.metaData.bookInfo.addCharacter(text);
                            root.model.setDirty();
                            text = "";
                        }
                    }
                }

                Controls.Button {
                    id: addCharacterButton;
                    icon.name: "list-add-symbolic"
                    onClicked: newCharacterField.addCharacter();
                }
            }
            background: null
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Sequence")
    }

    FormCard.FormCard {
        Repeater {
            id: sequenceListRepeater;
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.sequenceCount : 0;
            delegate: FormCard.AbstractFormDelegate {
                id: serieDelegate

                function updateSeries(): void {
                    root.model.acbfData.metaData.bookInfo.sequence(modelData).title = seriesTextField.text;
                    if (numberField.value !== root.model.acbfData.metaData.bookInfo.sequence(modelData).number) {
                        root.model.acbfData.metaData.bookInfo.sequence(modelData).number = numberField.value;
                    }
                    if (volumeField.value !== root.model.acbfData.metaData.bookInfo.sequence(modelData).volume) {
                        root.model.acbfData.metaData.bookInfo.sequence(modelData).volume = volumeField.value;
                    }
                    root.model.setDirty();
                }

                contentItem: ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.fillWidth: true

                        Controls.TextField {
                            id: seriesTextField;
                            text: root.model.acbfData.metaData.bookInfo.sequence(modelData).title;
                            onEditingFinished: serieDelegate.updateSeries();
                        }

                        Controls.Button {
                            icon.name: "list-remove-symbolic"
                            onClicked: {
                                root.model.setDirty();
                                root.model.acbfData.metaData.bookInfo.removeSequence(index);
                            }
                        }
                    }

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.fillWidth: true

                        Controls.Label {
                            id: sequenceNumberLabel;
                            text: i18nc("Label for sequence number","Number:");
                        }
                        Controls.SpinBox {
                            id: numberField;
                            value : root.model.acbfData.metaData.bookInfo.sequence(modelData).number;
                            onValueChanged: serieDelegate.updateSeries();
                            from: 0;
                            to: 99999;
                            editable: true;
                            Layout.fillWidth: true
                        }
                    }

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.fillWidth: true

                        Controls.Label {
                            id: sequenceVolumeLabel
                            text: i18nc("Label for sequence volume","Volume:");
                        }

                        Controls.SpinBox {
                            id: volumeField
                            value : root.model.acbfData.metaData.bookInfo.sequence(modelData).volume;
                            onValueChanged: serieDelegate.updateSeries();
                            from: 0;
                            to: 99999;
                            editable: true;
                        }
                    }
                }
                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }

        FormCard.FormTextFieldDelegate {
            id: addNewSerieDelegate

            label: i18nc("@label:textfield", "New serie");
            onAccepted: addSequence();

            function addSequence(): void {
                if(text !== "") {
                    root.model.acbfData.metaData.bookInfo.addSequence(0, text);
                    root.model.setDirty();
                    text = "";
                }
            }

            trailing: Controls.Button {
                icon.name: "list-add-symbolic"
                onClicked: addNewSerieDelegate.addSequence();
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Database References")
    }

    FormCard.FormCard {
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.databaseRefCount : 0;
            delegate: FormCard.AbstractFormDelegate {
                id: referenceDelegate

                Layout.fillWidth: true

                function updateDatabaseRef(): void {
                    root.model.acbfData.metaData.bookInfo.databaseRef(modelData).reference = referenceTextField.text
                    root.model.acbfData.metaData.bookInfo.databaseRef(modelData).dbname = databaseNameField.text
                    root.model.acbfData.metaData.bookInfo.databaseRef(modelData).type = referenceTypeField.text
                    root.model.setDirty();
                }

                contentItem: ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing
                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.fillWidth: true

                        Controls.Label {
                            text: i18nc("@label:textfield", "Reference:")
                        }

                        Controls.TextField {
                            id: referenceTextField
                            text: root.model.acbfData.metaData.bookInfo.databaseRef(modelData).reference;
                            onEditingFinished: referenceDelegate.updateDatabaseRef();
                            Layout.fillWidth: true
                        }

                        Controls.Button {
                            id: removeReferenceButton
                            icon.name: "list-remove-symbolic"
                            onClicked: {
                                root.model.setDirty();
                                root.model.acbfData.metaData.bookInfo.removeDatabaseRef(index);
                            }
                        }
                    }

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.fillWidth: true

                        Controls.Label {
                            text: i18nc("@label:textfield", "Database name:")
                        }

                        Controls.TextField {
                            id: databaseNameField;
                            text: root.model.acbfData.metaData.bookInfo.databaseRef(modelData).dbname;
                            onEditingFinished: referenceDelegate.updateDatabaseRef();
                            Layout.fillWidth: true
                        }
                    }

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.fillWidth: true

                        Controls.Label {
                            text: i18nc("@label:textfield", "Reference type:")
                        }

                        Controls.TextField {
                            id: referenceTypeField
                            text: root.model.acbfData.metaData.bookInfo.databaseRef(modelData).type;
                            onEditingFinished: referenceDelegate.updateDatabaseRef();
                        }
                    }
                }

                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }


        FormCard.AbstractFormDelegate {
            id: addReferenceDelegate

            function addReference(): void {
                if (addReferenceField.text === "" || addDatabaseNameField.text === "") {
                    return;
                }
                root.model.acbfData.metaData.bookInfo.addDatabaseRef(addReferenceField.text, addDatabaseNameField.text);
                root.model.setDirty();
                addReferenceField.text = "";
                addDatabaseNameField.text = "";
            }

            background: null
            contentItem: ColumnLayout {
                spacing: Kirigami.Units.smallSpacing

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.Label {
                        text: i18nc("@label:textfield", "Reference:")
                    }

                    Controls.TextField {
                        id: addReferenceField
                        onAccepted: addDatabaseNameField.forceActiveFocus();
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.Label {
                        text: i18nc("@label:textfield", "Database name:")
                    }

                    Controls.TextField {
                        id: addDatabaseNameField
                        onAccepted: addReferenceDelegate.addReference();
                        Layout.fillWidth: true
                    }

                    Controls.Button {
                        icon.name: "list-add-symbolic"
                        onClicked: addReferenceDelegate.addReference();
                    }
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Content Ratings")
    }

    FormCard.FormCard {
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.bookInfo.contentRatingCount : 0;
            delegate: FormCard.AbstractFormDelegate {
                id: ratingDelegate

                function updateRating(): void {
                    root.model.acbfData.metaData.bookInfo.contentRating(modelData).rating = ratingNameField.text
                    root.model.acbfData.metaData.bookInfo.contentRating(modelData).type = systemNameField.text
                    root.model.setDirty();
                }

                contentItem: ColumnLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Layout.fillWidth: true


                    Controls.TextField {
                        id: ratingNameField;
                        text: root.model.acbfData.metaData.bookInfo.contentRating(modelData).rating;
                        placeholderText: i18nc("placeholder text for the add content rating text entry", "Write to add rating label.");
                        onEditingFinished: ratingDelegate.updateRating();
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        spacing: Kirigami.Units.smallSpacing

                        Layout.fillWidth: true

                        Controls.TextField {
                            id: systemNameField
                            text: root.model.acbfData.metaData.bookInfo.contentRating(modelData).type
                            placeholderText: i18nc("placeholder text for the add content rating system text entry", "Write to add rating system.");
                            onEditingFinished: ratingDelegate.updateRating();
                            Layout.fillWidth: true
                        }

                        Controls.Button {
                            id: removeRatingButton
                            icon.name: "list-remove-symbolic"
                            onClicked: {
                                root.model.setDirty();
                                root.model.acbfData.metaData.bookInfo.removeContentRating(index);
                            }
                        }
                    }
                }

                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }

        FormCard.AbstractFormDelegate {
            id: addRatingDelegate

            function addRating(): void {
                if (addRatingField.text === "" || addSystemField.text === "") {
                    return;
                }
                root.model.acbfData.metaData.bookInfo.addContentRating(addRatingField.text, addSystemField.text);
                root.model.setDirty();
                addRatingField.text = "";
                addSystemField.text = "";
            }

            contentItem: ColumnLayout {
                spacing: Kirigami.Units.smallSpacing

                Layout.fillWidth: true

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Layout.fillWidth: true

                    Controls.Label {
                        text: i18nc("@label:textfield", "Review title");
                    }

                    Controls.TextField {
                        id: addRatingField
                        onEditingFinished: addSystemField.forceActiveFocus();
                        Layout.fillWidth: true
                    }
                }

                RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Layout.fillWidth: true

                    Controls.Label {
                        text: i18nc("@label:textfield", "Description");
                    }

                    Controls.TextField {
                        id: addSystemField;
                        Layout.fillWidth: true
                        onEditingFinished: addRatingDelegate.addRating();
                    }

                    Controls.Button {
                        id: addRatingButton;
                        icon.name: "list-add-symbolic"
                        onClicked: addRatingDelegate.addRating();
                    }
                }
            }
            background: null
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Publisher Info")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: publisher
            label: i18nc("Label for publisher", "Publisher:")
            placeholderText: i18nc("placeholder text for the publisher entry", "Write to add publisher")
            text: root.model.acbfData? root.model.acbfData.metaData.publishInfo.publisher: "";
            onEditingFinished: {
                if (root.model.acbfData && text !=="") {
                    root.model.acbfData.metaData.publishInfo.publisher = text
                    root.model.setDirty();
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormDateTimeDelegate {
            text: i18nc("Label for publishing date", "Publishing Date:")
            dateTimeDisplay: FormCard.FormDateTimeDelegate.DateTimeDisplay.Date
            initialValue: root.model.acbfData.metaData.publishInfo.publishDate
            onValueChanged: {
                root.model.acbfData.metaData.publishInfo.publishDate = value;
                root.model.setDirty();
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: city

            label: i18nc("Label for city", "City:")
            text: root.model.acbfData? root.model.acbfData.metaData.publishInfo.city: "";
            onEditingFinished: {
                if (root.model.acbfData && text !=="") {
                    root.model.acbfData.metaData.publishInfo.city = text ;
                    root.model.setDirty();
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: isbn

            label: i18nc("Label for ISBN", "ISBN:");
            text: root.model.acbfData? root.model.acbfData.metaData.publishInfo.isbn: "";
            onEditingFinished: {
                if (root.model.acbfData && text !=="") {
                    root.model.acbfData.metaData.publishInfo.isbn = text
                    root.model.setDirty();
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: license

            label: i18nc("Label for license", "License:");
            text: root.model.acbfData? root.model.acbfData.metaData.publishInfo.license: "";
            onEditingFinished: {
                if (root.model.acbfData && text !=="") {
                    root.model.acbfData.metaData.publishInfo.license = text
                    root.model.setDirty();
                }
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Document Authors");
    }

    FormCard.FormCard {
        Repeater {
            id: docAuthorRepeater;
            model: root.model.acbfData ? root.model.acbfData.metaData.documentInfo.authorNames : 0;
            delegate: FormCard.AbstractFormDelegate {
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.Label {
                        text: modelData.length > 0 ? modelData : i18nc("The text used in place of an author's name if one has not been set", "(unnamed)");
                        Layout.fillWidth: true
                    }

                    Controls.Button {
                        icon.name: "document-edit-symbolic"
                        onClicked: {
                            const window = root.Controls.ApplicationWindow.window.pageStack.pushDialogLayer(docAuthorEditor, {
                                index: model.index,
                            });
                        }
                    }

                    Controls.Button {
                        id: removeDocAuthorButton;
                        icon.name: "list-remove-symbolic"
                        onClicked: {
                            // When removing, set the model dirty first, and then remove the entry to avoid reference errors.
                            root.model.setDirty();
                            root.model.acbfData.metaData.documentInfo.removeAuthor(index);
                        }
                    }
                }
                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }

        FormCard.FormTextFieldDelegate {
            id: addAuthorDelegate

            label: i18nc("@label:textfield", "New author (nickname)")
            onAccepted: addAuthor()

            function addAuthor(): void {
                if(text !== "") {
                    // Just add an author where only the nickname is defined
                    root.model.acbfData.metaData.documentInfo.addAuthor("", "", "", "", "", text, [""], [""]);
                    root.model.setDirty();
                    text = "";
                }
            }

            trailing: Controls.Button {
                icon.name: "list-add-symbolic"
                onClicked: addAuthorDelegate.addAuthor();
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Document Sources");
    }

    FormCard.FormCard {
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.documentInfo.source : 0;
            FormCard.AbstractFormDelegate {
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.TextField {
                        id: sourceText;
                        text: modelData
                        onEditingFinished: root.model.acbfData.metaData.documentInfo.sources[index] = text;
                        Layout.fillWidth: true
                    }

                    Controls.Button {
                        id: removeSourceButton
                        icon.name: "list-remove-symbolic"
                        onClicked: {
                            root.model.setDirty();
                            root.model.acbfData.metaData.documentInfo.removeSource(index);
                        }
                    }
                }
                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }

        FormCard.FormTextFieldDelegate {
            id: addDocumentSourceDelegate

            label: i18nc("@label:textfield", "New document source")
            onAccepted: addEntry();

            function addEntry(): void {
                if (text !== "") {
                    root.model.acbfData.metaData.documentInfo.source.push(text);
                    root.model.setDirty();
                    text = "";
                }
            }

            trailing: Controls.Button {
                icon.name: "list-add-symbolic"
                onClicked: addDocumentSourceDelegate.addEntry();
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Document History")
    }

    FormCard.FormCard {
        Repeater {
            model: root.model.acbfData ? root.model.acbfData.metaData.documentInfo.history : 0;
            delegate: FormCard.AbstractFormDelegate {
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.TextField {
                        id: historyText;
                        text: modelData;
                        onEditingFinished: root.model.acbfData.metaData.documentInfo.history[index] = text;
                    }

                    Controls.Button {
                        id: removeHistoryButton;
                        icon.name: "list-remove";
                        onClicked: {
                            root.model.setDirty();
                            root.model.acbfData.metaData.documentInfo.removeHistoryLine(index);
                        }
                    }
                }

                background: Item {
                    FormCard.FormDelegateSeparator {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                    }
                }
            }
        }

        FormCard.FormTextFieldDelegate {
            id: addDocumentInfoDelegate

            label: i18nc("@label:textfield", "New document info")
            onAccepted: addEntry();

            function addEntry(): void {
                if (text !== "") {
                    root.model.acbfData.metaData.documentInfo.history.push(text);
                    root.model.setDirty();
                    text = "";
                }
            }

            trailing: Controls.Button {
                icon.name: "list-add-symbolic"
                onClicked: addDocumentInfoDelegate.addEntry();
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "General Page Background Color")
    }

    FormCard.FormCard {
        FormCard.FormColorDelegate {
            color: root.model.acbfData.body.bgcolor !== ""? root.model.acbfData.body.bgcolor: "#ffffff";
            onColorChanged: {
                root.model.acbfData.body.bgcolor = color;
            }
        }
    }

    Component {
        id: authorEditor
        AuthorEntryEditor {
            bookinfo: root.model.acbfData.metaData.bookInfo
            onSave: {
                root.model.acbfData.metaData.bookInfo.setAuthor(index, activity, language, firstName, middleName, lastName, nickName, homePage, email);
                root.model.setDirty();
            }
        }
    }

    Component {
        id: docAuthorEditor
        AuthorEntryEditor {
            bookinfo: root.model.acbfData.metaData.documentInfo
            onSave: {
                root.model.acbfData.metaData.documentInfo.setAuthor(index, activity, language, firstName, middleName, lastName, nickName, homePage, email);
                root.model.setDirty();
            }
        }
    }
}
