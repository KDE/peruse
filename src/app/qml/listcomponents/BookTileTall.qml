/*
 * SPDX-FileCopyrightText: 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
 * SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.14 as Kirigami

/**
 * @brief A button to select a book to read with a nice big thumbnail.
 */
FocusScope {
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

    Rectangle {
        id: stateIndicator

        anchors.fill: parent
        z: 1

        color: "transparent"
        opacity: 0.4

        radius: 3
    }

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
            Layout.margins: Kirigami.Units.largeSpacing
            Layout.preferredHeight: root.height - 2 * Kirigami.Units.largeSpacing
            Layout.preferredWidth: root.width - 2 * Kirigami.Units.largeSpacing

            Kirigami.Icon {
                id: coverImage;
                source: root.thumbnail === "Unknown role" ? "" : root.thumbnail;
                placeholder: "application-vnd.oasis.opendocument.text";
                fallback: "paint-unknown"
                Layout.fillHeight: true
                Layout.alignment: Qt.AlignHCenter
            }

            QtControls.Label {
                id: bookTitle;
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                elide: Text.ElideMiddle;
                horizontalAlignment: Text.AlignHCenter
                Layout.maximumWidth: root.width * 0.9
                Layout.minimumWidth: Layout.maximumWidth
                Layout.maximumHeight: root.author.length === 0 ? bookTitleSize.boundingRect.height * 2 : bookTitleSize.boundingRect.height
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.bottomMargin: root.author.length === 0 ? 0 : Kirigami.Units.smallSpacing
            }

            QtControls.Label {
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
                elide: Text.ElideMiddle;
                visible: root.author.length > 0
                text: visible ? root.author.join(', ') : ""
                horizontalAlignment: Text.AlignHCenter
                Layout.maximumWidth: root.width * 0.9
                Layout.minimumWidth: Layout.maximumWidth
                Layout.maximumHeight: root.author.length === 0 ? bookTitleSize.boundingRect.height * 2 : bookTitleSize.boundingRect.height
                Layout.alignment: Qt.AlignHCenter | Qt.AlignBottom
                Layout.bottomMargin: root.author.length === 0 ? 0 : Kirigami.Units.smallSpacing
            }
        }
    }
}
