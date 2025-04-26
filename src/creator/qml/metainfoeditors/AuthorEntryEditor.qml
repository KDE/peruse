// SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

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
FormCard.FormCardPage {
    id: root;

    property int index: -1;
    property QtObject bookinfo: null;
    signal save();

    onIndexChanged: {
        root.author = bookinfo.getAuthor(index);
        activityField.model = root.author.availableActivities();
        activityField.currentIndex = activityField.indexOfValue(author.activity());
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

    title: i18nc("title text for the edit author sheet", "Edit Author")

    footer: Controls.ToolBar {
        contentItem: Controls.DialogButtonBox {
            standardButtons: Controls.Dialog.Cancel | Controls.Dialog.Save
            onAccepted: {
                root.save();
                root.closeDialog();
            }

            onRejected: root.closeDialog()
        }
    }

    FormCard.FormSectionText {
        text: i18nc("help text for the edit author sheet", "Please complete the information for this author.")
    }


    FormCard.FormHeader {
        title: i18nc("@title:group", "Name")
    }

    FormCard.FormCard {
        FormCard.FormTextFieldDelegate {
            id: firstNameField
            label: i18nc("@label:textfield", "First name");
        }

        FormCard.FormTextFieldDelegate {
            id: middleNameField
            label: i18nc("@label:textfield", "Middle name");
        }

        FormCard.FormTextFieldDelegate {
            id: lastNameField
            label: i18nc("@label:textfield", "Last name");
        }

        FormCard.FormTextFieldDelegate {
            id: nickNameField
            label: i18nc("@label:textfield", "Nickname");
        }
    }

    FormCard.FormCard {
        Layout.topMargin: Kirigami.Units.gridUnit

        FormCard.FormComboBoxDelegate {
            id: activityField
            text: i18nc("@label:listbox", "Activity");
        }

        FormCard.FormTextFieldDelegate {
            id: languageField
            label: i18nc("@label:textfield", "Language");
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Homepage addresses")
    }

    FormCard.FormCard {
        Repeater {
            model: root.author ? root.author.homePages : 0;

            delegate: FormCard.AbstractFormDelegate {
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.TextField {
                        text: modelData;
                        onEditingFinished: root.author.homePages[index] = text;
                        Layout.fillWidth: true
                    }

                    Controls.Button {
                        id: removeHomePageButton
                        icon.name: "list-remove-symbolic"
                        onClicked: root.author.removeHomePage(index);
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
            id: homePageField

            label: i18nc("@label:textfield", "New Homepage")
            onAccepted: addEntry();

            function addEntry(): void {
                if (text!=="") {
                    root.author.addHomePage(text);
                    text="";
                }
            }

            trailing: Controls.Button {
                id: addHomepageButton;
                icon.name: "list-add-symbolic"
                onClicked: homePageField.addEntry();
            }
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Email addresses")
    }

    FormCard.FormCard {
        Repeater {
            model: root.author ? root.author.emails : 0;
            delegate: FormCard.AbstractFormDelegate {
                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing

                    Controls.TextField {
                        text: modelData
                        onEditingFinished: root.author.emails[index] = text;
                        Layout.fillWidth: true
                    }

                    Controls.Button {
                        id: removeEmailButton

                        icon.name: "list-remove-symbolic"
                        onClicked: root.author.removeEmail(index);
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
            id: emailField

            label: i18nc("placeholder text for the email field", "New Email address");
            onAccepted: addEntry();
            function addEntry(): void {
                if (text!=="") {
                    root.author.addEmail(text);
                    text = "";
                }
            }

            trailing: Controls.Button {
                id: addEmailButton
                icon.name: "list-add-symbolic"
                onClicked: emailField.addEntry();
            }
        }
    }
}
