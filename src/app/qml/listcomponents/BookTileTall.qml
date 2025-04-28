// SPDX-FileCopyrightText: 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick
import QtQuick.Controls as QtControls
import QtQuick.Layouts
import QtQuick.Effects

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates
import org.kde.peruse as Peruse

/**
 * @brief A button to select a book to read with a nice big thumbnail.
 */
Delegates.RoundedItemDelegate {
    id: root;

    required property int index
    required property string title
    required property string filename
    required property string thumbnail
    required property int categoryEntriesCount
    required property int currentPage
    required property int totalPages
    required property var author

    readonly property double progress: currentPage / totalPages;

    property bool selected: false;
    signal bookSelected(string filename, int currentPage);

    TapHandler {
        acceptedButtons: Qt.RightButton
        onTapped: root.pressAndHold();
    }

    TapHandler {
        onTapped: root.bookSelected(root.filename, root.currentPage);
    }

    TextMetrics {
        id: bookTitleSize
        font: bookTitle.font
        text: bookTitle.text
    }

    SystemPalette {
        id: myPalette
    }

    contentItem: ColumnLayout {
        spacing: 0;

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: root.width - 2 * Kirigami.Units.largeSpacing

            MultiEffect {
                source: coverImage
                enabled: !Kirigami.Settings.isMobile // don't use drop shadow on mobile

                shadowColor: myPalette.shadow
                autoPaddingEnabled: true
                shadowBlur: 1.0
                shadowEnabled: true
                shadowVerticalOffset: 3
                shadowHorizontalOffset: 1

                anchors.fill: coverImage
            }

            Kirigami.Icon {
                id: coverImage

                width: root.width - 2 * Kirigami.Units.largeSpacing
                height: root.width - 2 * Kirigami.Units.largeSpacing

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    margins: Kirigami.Settings.isMobile ? 0 : Kirigami.Units.largeSpacing
                }

                source: root.thumbnail === "Unknown role" ? "" : root.thumbnail;
                placeholder: "application-vnd.oasis.opendocument.text";
                fallback: "paint-unknown"
            }

            QtControls.Label {
                text: root.currentPage > 0 ? i18nc("A percentage of progress", "%1%", Math.floor(100 * root.progress)) : i18nc("should be keep short, inside a label. Will be in uppercase", "New")
                color: "white"
                padding: 3
                visible: root.totalPages > 0

                background: Rectangle {
                    color: Kirigami.Theme.highlightColor
                    radius: height
                }

                anchors {
                    right: coverImage.right
                    top: coverImage.top
                }
            }
        }

        QtControls.Label {
            id: bookTitle;

            text: root.title
            elide: Text.ElideMiddle;
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        QtControls.Label {
            function getCombinedName(stringList) {
                let combined = "";
                for (var i = 0; i < stringList.length; ++i) {
                    if (combined.length > 0 && i == stringList.length - 1) {
                        combined += i18nc("The last item in a list of author names when there is more than one", ", and %1", stringList[i]);
                    }
                    else if (combined.length > 0) {
                        combined += i18nc("An item in a list of authors (but not the last)", ", %1", stringList[i]);
                    }
                    else {
                        combined += stringList[i];
                    }
                }
                return combined;
            }
            elide: Text.ElideMiddle;
            text: root.author.length > 0 ? getCombinedName(root.author) : i18nc("Author name used when there are no known authors for a book", "Unknown author");
            opacity: 0.7
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            Layout.maximumHeight: bookTitleSize.boundingRect.height
            Layout.minimumHeight: Layout.maximumHeight
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }
    }
}
