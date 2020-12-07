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

import org.kde.kirigami 2.13 as Kirigami
/**
 * @brief The page on which Peruse Creator opens.
 * 
 * This page gives an introduction to peruse and has options for:
 * - Opening the last opened archive by either Peruse or Peruse Creator.
 * - Opening an existing comic.
 * - Creating a blank comic.
 * - Creating a comic archive from a selection of images.
 */
Kirigami.ScrollablePage {
    id: root;
    property string categoryName: "welcomePage";
    title: i18nc("title of the welcome page", "Welcome");

    ListView {
        id: startupChoices
        header: Column {
            height: titleContainer.height + instructionsCard.height + Kirigami.Units.largeSpacing * 5;
            width: startupChoices.width;
            spacing: Kirigami.Units.largeSpacing
            Item { height: Kirigami.Units.largeSpacing; width: Kirigami.Units.largeSpacing; }
            Item {
                id: titleContainer;
                anchors.horizontalCenter: parent.horizontalCenter
                width: startupChoices.width - Kirigami.Units.largeSpacing * 4
                height: appNameLabel.height + appDescriptionLabel.height + Kirigami.Units.largeSpacing;
                Kirigami.Heading {
                    id: appNameLabel;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        bottom: parent.verticalCenter;
                    }
                    text: i18nc("The application's name", "Peruse Creator");
                    horizontalAlignment: Text.AlignHCenter;
                }
                QtControls.Label {
                    id: appDescriptionLabel;
                    anchors {
                        top: parent.verticalCenter;
                        left: parent.left;
                        right: parent.right;
                    }
                    text: i18nc("application subtitle", "Comic Book Creation Tool");
                    horizontalAlignment: Text.AlignHCenter;
                }
                Rectangle {
                    anchors.centerIn: parent;
                    height: 1;
                    color: Kirigami.Theme.textColor;
                    width: appDescriptionLabel.paintedWidth;
                }
            }
            Kirigami.Card {
                id: instructionsCard
                width: startupChoices.width - Kirigami.Units.largeSpacing * 4
                anchors.horizontalCenter: parent.horizontalCenter
                contentItem: QtControls.Label {
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                    padding: Kirigami.Units.smallSpacing;
                    text: i18nc("Longer introduction text used on the welcome page", "Welcome to Peruse Creator, a tool designed to assist you in creating comic book archives which can be read with any cbz capable comic book reader app. You can either create entirely new comic book archives from scratch, create one from a set of pictures, or editing existing archives. Once you have created them, you can even publish them directly to the online comic book archive at the KDE Store from within the application, or just share the files with your friends.");
                }
            }
            Item { height: Kirigami.Units.largeSpacing; width: Kirigami.Units.largeSpacing; }
        }
        Component.onCompleted: {
            startupChoices.model.append( {
                text: i18nc("@action:button open existing comic book archive", "Open Existing..."),
                subtitle: "",
                icon: "document-open",
                script: " mainWindow.openOther()"
            });
            startupChoices.model.append( {
                text: i18nc("@action:button create a new, empty comic book archive", "Create Blank"),
                subtitle: "",
                icon: "document-new",
                script: "mainWindow.changeCategory(createNewBookPage)"
            });
            startupChoices.model.append( {
                text: i18nc("@action:button create a new comic book archive by copying in a bunch of pictures", "Create from Images..."),
                subtitle: "",
                icon: "folder-open",
                script: ""
            });
            if(peruseConfig.recentlyOpened.length > 0) {
                for(var i = 0; i < peruseConfig.recentlyOpened.length; ++i) {
                    if(peruseConfig.recentlyOpened[i].toLowerCase().slice(-4) === ".cbz") {
                        var recentBook = peruseConfig.recentlyOpened[i];
                        startupChoices.model.append({
                            text: i18nc("@action:button continue working on the most recently opened comic book archive", "Continue working on %1", recentBook.split('/').pop()),
                            subtitle: recentBook,
                            icon: "image://comiccover/" + recentBook,
                            script: "mainWindow.openBook(\"" + recentBook + "\")"
                        });
                    }
                }
            }
        }
        model: ListModel { }
        delegate: Kirigami.BasicListItem {
            height: Kirigami.Units.gridUnit * 3
            text: model.text
            subtitle: model.subtitle
            icon: model.icon
            onClicked: eval(model.script);
        }
    }
}
