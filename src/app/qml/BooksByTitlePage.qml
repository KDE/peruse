// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only or LGPL-3.0-only or LicenseRef-KDE-Accepted-LGPL

pragma ComponentBehavior: Bound

import QtQuick

import org.kde.kitemmodels as KItemModels
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.delegates as Delegates

import org.kde.peruse as Peruse

Kirigami.ScrollablePage {
    id: root

    readonly property string categoryName: "bookshelfTitle"
    property alias model: sortFilterModel.sourceModel

    signal bookSelected(string filename, int currentPage)

    title: i18nc("Title of the page with books grouped by the title start letters", "Group by Title")

    actions: Kirigami.Action {
        displayComponent: Kirigami.SearchField {
            onTextChanged: sortFilterModel.filterString = text;
        }
    }

    ListView {
        id: list

        currentIndex: -1

        model: KItemModels.KSortFilterProxyModel {
            id: sortFilterModel

            filterRoleName: 'title'
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        section {
            property: 'title'
            criteria: ViewSection.FirstCharacter
            delegate: Kirigami.ListSectionHeader {
                required property string section
                width: ListView.view.width
                text: section
            }
        }

        delegate: Delegates.RoundedItemDelegate {
            id: bookDelegate

            required property int index
            required property string title
            required property string currentPage
            required property string filename
            required property list<string> author

            text: title

            contentItem: Delegates.SubtitleContentItem {
                itemDelegate: bookDelegate
                subtitle: bookDelegate.author.join(', ')
            }

            onClicked: root.bookSelected(filename, currentPage)
        }

        Kirigami.PlaceholderMessage {
            id: placeholderMessage
            width: parent.width - Kirigami.Units.gridUnit * 4
            visible: list.count === 0
            anchors.centerIn: parent
            text: sortFilterModel.filterString.length > 0 ? i18nc("Placeholder Text when there are no comics in the library that match the filter", "No matches") : i18nc("@info:placeholder", "No authors available")
        }
    }
}