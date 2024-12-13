/*
    SPDX-FileCopyrightText: 2020 Dan Leinir Turthra Jensen <admin@leinir.dk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

import QtQuick 2.12
import QtQuick.Controls 2.12 as QtControls
import QtQuick.Layouts 1.12 as QtLayouts

import org.kde.kirigami 2.13 as Kirigami

Kirigami.OverlaySheet {
    id: component

    property alias title: headerLabel.text
    property alias text: messageLabel.text
    property list<QtObject> actions

    showCloseButton: true
    header: Kirigami.Heading {
        id: headerLabel
        QtLayouts.Layout.fillWidth: true
        elide: Text.ElideRight
    }
    // This is a TextEdit rather than a label, because the QQC Label control
    // does not support text selection (and it's very useful to be able to
    // select error texts for searchy purposes)
    contentItem: TextEdit {
        id: messageLabel
        QtLayouts.Layout.preferredWidth: Kirigami.Units.gridUnit * 10
        QtLayouts.Layout.margins: Kirigami.Units.largeSpacing
        wrapMode: Text.Wrap
        readOnly: true
        selectByMouse: true
        color: Kirigami.Theme.textColor
        selectedTextColor: Kirigami.Theme.highlightedTextColor
        selectionColor: Kirigami.Theme.highlightColor
    }
    footer: QtLayouts.RowLayout {
        Item { QtLayouts.Layout.fillWidth: true }
        Repeater {
            model: component.actions;
            QtControls.Button {
                action: modelData
                Connections {
                    target: action
                    onTriggered: component.close()
                }
            }
        }
    }
}
