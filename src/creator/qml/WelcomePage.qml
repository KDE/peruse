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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

/**
 * @brief The page on which Peruse Creator opens.
 * 
 * This page gives an introduction to peruse and has options for:
 * - Opening the last opened archive by either Peruse or Peruse Creator.
 * - Opening an existing comic.
 * - Creating a blank comic.
 * - Creating a comic archive from a selection of images.
 */
FormCard.FormCardPage {
    id: root;

    property string categoryName: "welcomePage";

    title: i18nc("title of the welcome page", "Welcome");

    Kirigami.Icon {
        source: "peruse-creator"
        implicitWidth: Math.round(Kirigami.Units.iconSizes.huge * 1.5)
        implicitHeight: Math.round(Kirigami.Units.iconSizes.huge * 1.5)

        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: Kirigami.Units.gridUnit
    }

    Kirigami.Heading {
        text: i18nc("The application's name", "Peruse Creator")

        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: Kirigami.Units.largeSpacing
    }

    Kirigami.Heading {
        text: i18nc("application subtitle", "Comic Book Creation Tool")
        level: 3

        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: Kirigami.Units.largeSpacing
    }

    QQC2.Label {
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        padding: Kirigami.Units.largeSpacing
        text: i18nc("Longer introduction text used on the welcome page", "Welcome to Peruse Creator, a tool designed to assist you in creating comic book archives which can be read with any cbz capable comic book reader app. You can either create entirely new comic book archives from scratch, create one from a set of pictures, or editing existing archives. Once you have created them, you can even publish them directly to the online comic book archive at the KDE Store from within the application, or just share the files with your friends.")

        Layout.fillWidth: true
        Layout.maximumWidth: formcard.maximumWidth
        Layout.alignment: Qt.AlignHCenter
    }

    FormCard.FormCard {
        id: formcard
        FormCard.FormButtonDelegate {
            text: i18nc("@action:button open existing comic book archive", "Open Existing...")
            icon.name: "document-open"
            onClicked: mainWindow.openOther()
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormButtonDelegate {
            text: i18nc("@action:button create a new, empty comic book archive", "Create Blank")
            icon.name: "document-new"
            onClicked: mainWindow.changeCategory(createNewBookPage)
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "Recent books")
    }

    FormCard.FormCard {
        Repeater {
            id: recentRepeater

            model: peruseConfig.recentlyOpened.filter((book) => book.slice(-4) === ".cbz")

            ColumnLayout {
                id: recentBookDelegate

                required property int index
                required property var modelData

                Layout.fillWidth: true

                FormCard.FormDelegateSeparator { visible: recentBookDelegate.index > 0 }

                FormCard.FormButtonDelegate {
                    text: i18nc("@action:button continue working on the most recently opened comic book archive", "Continue working on %1", recentBookDelegate.modelData.split('/').pop())
                    description: recentBookDelegate.modelData
                    icon {
                        name: "image://comiccover/" + recentBookDelegate.modelData
                        width: Kirigami.Units.iconSizes.large
                        height: Kirigami.Units.iconSizes.large
                    }
                    onClicked: mainWindow.openBook(recentBookDelegate.modelData)
                }
            }
        }

        FormCard.FormPlaceholderMessageDelegate {
            text: i18nc("@info:placeholder", "There are no recent books")
            visible: recentRepeater.count === 0
        }
    }
}
