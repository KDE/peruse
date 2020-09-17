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

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls

import org.kde.kirigami 2.7 as Kirigami
/**
 * @brief a special overlay sheet for editing the author information.
 * 
 * Authors can have the full names, nicknames and some contact information
 * like email address and homepage. They can also be assigned a role
 * from a list of predefined author activities.
 * 
 * Author is used in acbf for both the actual authors as well as the people
 * who handled generating the acbf document, which is why this is
 * a dedicated form.
 * 
 * TODO: Support input for multiple homepage and email addresses.
 */
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

    }
    property QtObject author: null;

    property alias activity: activityField.currentText;
    property alias language: languageField.text;
    property alias firstName: firstNameField.text;
    property alias middleName: middleNameField.text;
    property alias lastName: lastNameField.text;
    property alias nickName: nickNameField.text;
    property var homePage: root.author ? root.author.homePages : "";
    property var email: root.author ? root.author.emails : "";

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
                Keys.onReturnPressed: {
                        root.save();
                        root.close();
                    }
                onClicked: {
                    root.save();
                    root.close();
                }
            }
        }
        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("help text for the edit author sheet", "Please complete the information for this author.");
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
            //enabled: activity;
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
            text: i18nc("label for the homepage field", "Homepage addresses:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        Repeater {
            model: root.author ? root.author.homePages : 0;
            QtControls.TextField {
                width: parent.width - removeHomePageButton.width - Kirigami.Units.smallSpacing;
                text: modelData;
                onEditingFinished: root.author.homePages[index] = text;

                QtControls.Button {
                    id: removeHomePageButton;
                    anchors {
                        left: parent.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "list-remove";
                    }
                    height: parent.height;
                    width: height;
                    onClicked: root.author.removeHomePage(index);
                }
            }

        }
        QtControls.TextField {
            id: homePageField;
            width: parent.width - addHomepageButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the homepage field", "Add Homepage");
            Keys.onReturnPressed: addEntry();
            function addEntry() {
                if (text!=="") {
                    root.author.addHomePage(text);
                    text="";
                }
            }

            QtControls.Button {
                id: addHomepageButton;
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
        QtControls.Label {
            width: parent.width;
            height: paintedHeight;
            text: i18nc("label for the email field", "Email addresses:");
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
        }
        Repeater {
            model: root.author ? root.author.emails : 0;
            QtControls.TextField {
                width: parent.width - addEmailButton.width - Kirigami.Units.smallSpacing;
                text: modelData;
                onEditingFinished: root.author.emails[index] = text;

                QtControls.Button {
                    id: removeEmailButton;
                    anchors {
                        left: parent.right;
                        leftMargin: Kirigami.Units.smallSpacing;
                    }
                    contentItem: Kirigami.Icon {
                        source: "list-remove";
                    }
                    height: parent.height;
                    width: height;
                    onClicked: root.author.removeEmail(index);
                }
            }

        }
        QtControls.TextField {
            id: emailField;
            width: parent.width - addEmailButton.width - Kirigami.Units.smallSpacing;
            placeholderText: i18nc("placeholder text for the email field", "Add Email address");
            Keys.onReturnPressed: addEntry();
            function addEntry() {
                if (text!=="") {
                    root.author.addEmail(text);
                    text = "";
                }
            }

            QtControls.Button {
                id: addEmailButton;
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
    }
}
