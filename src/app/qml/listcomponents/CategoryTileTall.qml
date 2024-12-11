// SPDX-FileCopyrightText: 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Controls as QtControls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.peruse as Peruse

/**
 * @brief A button to select a category to show the categories and books inside.
 * 
 * It distinguishes itself from a book by drawing two rectangles behind the thumbnail,
 * to indicate 'multiple books'.
 */
Delegates.RoundedItemDelegate {
    id: root

    required property int index
    required property string title
    required property Peruse.CategoryEntriesModel categoryEntriesModel
    required property int categoryEntriesCount
    required property bool selected

    onClicked: applicationWindow().pageStack.push(bookshelf, { focus: true, headerText: root.title, model: root.categoryEntriesModel })

    TextMetrics {
        id: categoryTitleSize

        font: categoryTitle.font
        text: categoryTitle.text
    }

    contentItem: ColumnLayout {
        spacing: 0;
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
                source: root.categoryEntriesModel ? root.categoryEntriesModel.get(0).readProperty("thumbnail") : "";
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
                text: root.categoryEntriesCount
                height: paintedHeight;
                width: paintedWidth;
                color: Kirigami.Theme.highlightedTextColor;
            }
        }

        QtControls.Label {
            id: categoryTitle;

            text: root.title
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
