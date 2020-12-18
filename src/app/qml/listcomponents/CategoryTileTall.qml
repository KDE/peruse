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
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.14 as Kirigami

/**
 * @brief A button to select a category to show the categories and books inside.
 * 
 * It distinguishes itself from a book by drawing two rectangles behind the thumbnail,
 * to indicate 'multiple books'.
 */
FocusScope {
    id: root;
    property bool selected: false;
    property alias count: categoryCount.text;
    property alias title: categoryTitle.text
    property QtObject entriesModel;
    property int neededHeight: categoryImage.height + categoryTitle.height + Kirigami.Units.largeSpacing;
    visible: height > 0;
    enabled: visible;
    clip: true;
    MouseArea {
        anchors.fill: parent;
        onClicked: {
            applicationWindow().pageStack.push(bookshelf, { focus: true, headerText: root.title, model: root.entriesModel })
        }

        TextMetrics {
            id: categoryTitleSize
            font: categoryTitle.font
            text: categoryTitle.text
        }

        ColumnLayout {
            spacing: 0;
            anchors {
                fill: parent;
                margins: Kirigami.Units.largeSpacing
            }
            Item {
                id: categoryImage;
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                Rectangle {
                    anchors.centerIn: coverImage;
                    width: tileBg.width;
                    height: tileBg.height;
                    color: root.selected ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor;
                    border {
                        width: 2;
                        color: root.selected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor;
                    }
                    radius: 2;
                    rotation: 16;
                    Rectangle {
                        anchors {
                            fill: parent;
                            margins: Kirigami.Units.smallSpacing;
                        }
                        color: root.selected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor;
                    }
                }
                Rectangle {
                    anchors.centerIn: coverImage;
                    width: tileBg.width;
                    height: tileBg.height;
                    color: root.selected ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor;
                    border {
                        width: 2;
                        color: root.selected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor;
                    }
                    radius: 2;
                    rotation: 8;
                    Rectangle {
                        anchors {
                            fill: parent;
                            margins: Kirigami.Units.smallSpacing;
                        }
                        color: root.selected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor;
                    }
                }
                Rectangle {
                    id: tileBg;
                    anchors.centerIn: coverImage;
                    width: Math.max(coverImage.paintedWidth, Kirigami.Units.iconSizes.large) + Kirigami.Units.smallSpacing * 2;
                    height: Math.max(coverImage.paintedHeight, Kirigami.Units.iconSizes.large) + Kirigami.Units.smallSpacing * 2;
                    color: root.selected ? Kirigami.Theme.highlightColor : Kirigami.Theme.backgroundColor;
                    border {
                        width: 2;
                        color: root.selected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor;
                    }
                    radius: 2;
                }
                Kirigami.Icon {
                    id: coverImage;
                    anchors {
                        fill: parent;
                        margins: Kirigami.Units.largeSpacing;
                    }
                    source: root.entriesModel ? root.entriesModel.get(0).readProperty("thumbnail") : "";
                    placeholder: "application-vnd.oasis.opendocument.text";
                    fallback: "folder-documents-symbolic"
                }
                Rectangle {
                    anchors {
                        fill: categoryCount;
                        margins: -Kirigami.Units.smallSpacing;
                    }
                    radius: height / 2;
                    color: Kirigami.Theme.highlightColor;
                }
                QtControls.Label {
                    id: categoryCount;
                    anchors {
                        bottom: tileBg.bottom;
                        right: tileBg.right;
                    }
                    height: paintedHeight;
                    width: paintedWidth;
                    color: Kirigami.Theme.highlightedTextColor;
                }
            }

            QtControls.Label {
                id: categoryTitle;
                elide: Text.ElideMiddle;
                horizontalAlignment: Text.AlignHCenter
                Layout.maximumHeight: categoryTitleSize.boundingRect.height
                Layout.minimumHeight: Layout.maximumHeight
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.bottomMargin: Kirigami.Units.smallSpacing
            }
        }
    }
}
