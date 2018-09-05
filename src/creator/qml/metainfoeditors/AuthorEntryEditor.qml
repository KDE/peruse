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

import org.kde.kirigami 2.1 as Kirigami
import QtQuick.Controls 2.2 as QtControls

Kirigami.OverlaySheet {
    id: root;
    property int index: -1;
    property QtObject bookinfo: null;
    signal save();

    onIndexChanged: {
        root.author = bookinfo.getAuthor(index);
        activityField.model = root.author.availableActivities();
        activityField.currentIndex = activityField.find(author.activity());
        languageField.text = root.author.language();
        firstNameField.text = root.author.firstName();
        middleNameField.text = root.author.middleName();
        lastNameField.text = root.author.lastName();
        nickNameField.text = root.author.nickName();
        if (root.author.homePages.count > 0) {
            homePageField.text = root.author.homePages()[0];
        }
        if (root.author.emails.count > 0) {
            emailField.text = root.author.emails()[0];
        }
    }
    property QtObject author: null;

    property alias activity: activityField.currentText;
    property alias language: languageField.text;
    property alias firstName: firstNameField.text;
    property alias middleName: middleNameField.text;
    property alias lastName: lastNameField.text;
    property alias nickName: nickNameField.text;
    property alias homePage: homePageField.text;
    property alias email: emailField.text;

    Column {
        height: childrenRect.height;
        spacing: Kirigami.Units.smallSpacing;
        Kirigami.Heading {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("title text for the edit author sheet", "Edit Author");
            QtControls.Button {
                id: saveButton;
                anchors {
                    right: parent.right;
                    leftMargin: Kirigami.Units.smallSpacing;
                }
                contentItem: Kirigami.Icon {
                    source: "dialog-ok";
                }
                height: parent.height;
                width: height;
                onClicked: {
                    root.save();
                    root.close();
                }
            }
        }
        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("help text for the edit author sheet", "Please coplete the information for this author.");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        Item {
            width: parent.width;
            height: Kirigami.Units.largeSpacing;
        }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the activity field", "Activity:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.ComboBox {
            id: activityField;
            width: parent.width - Kirigami.Units.smallSpacing;
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the language field", "Language:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.TextField {
            id: languageField;
            width: parent.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the language field", "Language");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the first name field", "First name:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.TextField {
            id: firstNameField;
            width: parent.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the first name field", "First Name");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the middle name field", "Middle name:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.TextField {
            id: middleNameField;
            width: parent.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the middle name field", "Middle Name");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the last name field", "Last name:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.TextField {
            id: lastNameField;
            width: parent.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the last name field", "Last Name");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the nickname field", "Nickname:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.TextField {
            id: nickNameField;
            width: parent.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the nickname field", "Nickname");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the homepage field", "Homepage address:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.TextField {
            id: homePageField;
            width: parent.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the homepage field", "Homepage");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }

        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the email field", "Email address:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        QtControls.TextField {
            id: emailField;
            width: parent.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the email field", "Email address");
        }
        Item { width: parent.width; height: Kirigami.Units.smallSpacing; }
    }
}
