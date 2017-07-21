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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4 as QtControls

import org.kde.kirigami 2.1 as Kirigami

Kirigami.Page {
    id: root;
    property string categoryName: "welcomePage";
    title: i18nc("title of the welcome page", "Welcome");

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - root.topPadding;

        Item {
            id: titleContainer;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: appNameLabel.height + appDescriptionLabel.height + Kirigami.Units.largeSpacing;
            Kirigami.Heading {
                id: appNameLabel;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.verticalCenter;
                }
                text: "Peruse Creator";
                horizontalAlignment: Text.AlignHCenter;
            }
            Kirigami.Label {
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

        Item {
            id: actionsContainer;
            anchors {
                top: titleContainer.bottom;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            Item {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.verticalCenter;
                    margins: Kirigami.Units.largeSpacing;
                }
                Kirigami.Label {
                    anchors.fill: parent;
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;
                    text: i18nc("Longer introduction text used on the welcome page", "Welcome to Peruse Creator, a tool designed to assist you in creating comic book archives which can be read with any cbz capable comic book reader app. You can either create entirely new comic book archives from scratch, create one from a set of pictures, or editing existing archives. Once you have created them, you can even publish them directly to the online comic book archive at the KDE Store from within the application, or just share the files with your friends.");
                }
            }
            Item {
                anchors {
                    top: parent.verticalCenter;
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                }
                Item {
                    id: continueLast;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                        bottom: parent.verticalCenter;
                    }
                    QtControls.Button {
                        anchors.centerIn: parent;
                        iconName: "go-next";
                        text: i18nc("Button to continue working on the most recently opened comic book archive", "Continue %1").arg(continueLast.mostRecentBook.split('/').pop());
                        onClicked: mainWindow.openBook(continueLast.mostRecentBook);
                    }
                    property string mostRecentBook: "";
                    Component.onCompleted: {
                        if(peruseConfig.recentlyOpened.length > 0) {
                            for(var i = 0; i < peruseConfig.recentlyOpened.length; ++i) {
                                if(peruseConfig.recentlyOpened[i].toLowerCase().slice(-4) === ".cbz") {
                                    continueLast.mostRecentBook = peruseConfig.recentlyOpened[i];
                                    break;
                                }
                            }
                        }
                    }
                    visible: mostRecentBook.length > 0;
                }
                Item {
                    anchors {
                        top: parent.top;
                        left: continueLast.visible ? parent.horizontalCenter : parent.left;
                        right: parent.right;
                        bottom: parent.verticalCenter;
                    }
                    QtControls.Button {
                        anchors.centerIn: parent;
                        iconName: "document-open";
                        text: i18nc("Button to open existing comic book archive", "Open Existing...");
                        onClicked: mainWindow.openOther();
                    }
                }
                Item {
                    anchors {
                        top: parent.verticalCenter;
                        left: parent.left;
                        right: parent.horizontalCenter;
                        bottom: parent.bottom;
                    }
                    QtControls.Button {
                        anchors.centerIn: parent;
                        iconName: "document-new";
                        text: i18nc("Button to create a new, empty comic book archive", "Create Blank");
                        onClicked: mainWindow.createNew();
                    }
                }
                Item {
                    anchors {
                        top: parent.verticalCenter;
                        left: parent.horizontalCenter;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    QtControls.Button {
                        anchors.centerIn: parent;
                        iconName: "folder-open";
                        text: i18nc("Button to create a new comic book archive by copying in a bunch of pictures", "Create From Images...");
                    }
                }
            }
        }
    }
}
