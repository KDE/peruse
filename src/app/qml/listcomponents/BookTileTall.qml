/*
 * SPDX-FileCopyrightText: 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.14 as Kirigami

/**
 * @brief A button to select a book to read with a nice big thumbnail.
 */
QQC2.AbstractButton {
    id: root;
    property bool selected: false;
    property alias title: bookTitle.text;
    property var author: [];
    property string filename;
    property int categoryEntriesCount;
    property string currentPage;
    property string totalPages;
    property double progress: currentPage / totalPages;
    property string thumbnail;
    property bool pressIndicator: false;
    signal bookSelected(string filename, int currentPage);
    /// FIXME This signal will also forward the MouseEvent, but the type is not recognised, so we can't
    /// add it to the signature. Certainly would be nice if that were possible, though, right?
    /// @see https://bugreports.qt.io/browse/QTBUG-41441
    signal pressAndHold();

    enabled: visible;
    clip: true;

    ColumnLayout {
        anchors.fill: parent
        TapHandler {
            onTapped: root.bookSelected(root.filename, root.currentPage)
            onLongPressed: root.pressAndHold(mouse)
        }

        Kirigami.Icon {
            id: coverImage;
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 4
            source: root.thumbnail === "Unknown role" ? "" : root.thumbnail;
            placeholder: "application-vnd.oasis.opendocument.text";
            fallback: "paint-unknown"
        }
        QQC2.Label {
            id: bookTitle;
            elide: Text.ElideMiddle;
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }

        QQC2.Label {
            function getCombinedName(stringList) {
                var combined = "";
                for (var i = 0; i < stringList.length; ++i) {
                    if (combined.length > 0 && i == stringList.length - 1) {
                        combined += i18nc("The last item in a list of author names when there is more than one", ", and %1", stringList[i]);
                    }
                    else if (combined.length > 0) {
                        combined += i18nc("An item in a list of authors (but not the last)", ", %1", stringList[i]);
                    }
                    else {
                        combined += i18nc("The first author in a list of authors", "by %1", stringList[i]);
                    }
                }
                return combined;
            }
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.9
            elide: Text.ElideMiddle;
            text: root.author.length > 0 ? getCombinedName(root.author) : i18nc("Author name used when there are no known authors for a book", "by an unknown author");
            horizontalAlignment: Text.AlignHCenter
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
            Layout.bottomMargin: Kirigami.Units.smallSpacing
        }
    }
}
/*






    MouseArea {
        anchors.fill: parent;
        onClicked: root.bookSelected(root.filename, root.currentPage);
        onPressAndHold: root.pressAndHold(mouse);
//         onPressed: root.pressIndicator ? pressIndicatorAnimation.start():0;
//         onReleased: {pressIndicatorAnimation.stop(); pressIndicator.width = 0;pressIndicator.height = 0;}

        TextMetrics {
            id: bookTitleSize
            font: bookTitle.font
            text: bookTitle.text
        }

        ColumnLayout {
            spacing: 0;
            anchors {
                fill: parent;
                margins: Kirigami.Units.largeSpacing
            }
            Item {
                id: bookCover;
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
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
                Item {
                    anchors {
                        right: tileBg.right
                        bottom: tileBg.bottom
                        rightMargin: - Kirigami.Units.largeSpacing
                        bottomMargin: - Kirigami.Units.largeSpacing
                    }
                    width: bookTitleSize.boundingRect.height * 2
                    height: width
                    visible: root.progress > 0
                    Rectangle {
                        anchors.fill: parent
                        radius: width / 2
                        opacity: .9
                        color: Kirigami.Theme.activeBackgroundColor
                    }
                    QtControls.Label {
                        text: i18nc("A percentage of progress", "%1\%", Math.floor(100 * root.progress))
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: Kirigami.Theme.activeTextColor
                    }
                }
            }

            QtControls.Label {
                id: bookTitle;
                elide: Text.ElideMiddle;
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.bottomMargin: Kirigami.Units.smallSpacing
            }

            QtControls.Label {
                function getCombinedName(stringList) {
                    var combined = "";
                    for (var i = 0; i < stringList.length; ++i) {
                        if (combined.length > 0 && i == stringList.length - 1) {
                            combined += i18nc("The last item in a list of author names when there is more than one", ", and %1", stringList[i]);
                        }
                        else if (combined.length > 0) {
                            combined += i18nc("An item in a list of authors (but not the last)", ", %1", stringList[i]);
                        }
                        else {
                            combined += i18nc("The first author in a list of authors", "by %1", stringList[i]);
                        }
                    }
                    return combined;
                }
                elide: Text.ElideMiddle;
                text: root.author.length > 0 ? getCombinedName(root.author) : i18nc("Author name used when there are no known authors for a book", "by an unknown author");
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
                Layout.maximumHeight: bookTitleSize.boundingRect.height
                Layout.minimumHeight: Layout.maximumHeight
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.bottomMargin: Kirigami.Units.smallSpacing
            }
        }
    }
}*/
