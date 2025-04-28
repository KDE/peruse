// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.peruse as Peruse
import org.kde.kirigamiaddons.formcard as FormCard

FormCard.FormCardPage {
    id: root

    property Peruse.bookEntry bookEntry
    property string comment: peruseConfig.getFilesystemProperty(root.bookEntry.filename, "comment")
    property var tags: peruseConfig.getFilesystemProperty(root.bookEntry.filename, "tags").split(",")
    property int rating: peruseConfig.getFilesystemProperty(root.bookEntry.filename, "rating")

    Kirigami.Icon {
        id: coverImage;

        source: root.bookEntry.thumbnail
        placeholder: "application-vnd.oasis.opendocument.text"
        fallback: "paint-unknown"

        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: Kirigami.Units.iconSizes.enormous
        Layout.preferredWidth: Kirigami.Units.iconSizes.enormous
        Layout.bottomMargin: Kirigami.Units.gridUnit
        Layout.topMargin: Kirigami.Units.gridUnit
    }

    Kirigami.Heading {
        text: root.bookEntry.title
        horizontalAlignment: Qt.AlignHCenter

        Layout.fillWidth: true
        Layout.bottomMargin: Kirigami.Units.gridUnit
    }

    FormCard.FormCard {
        FormCard.FormTextDelegate {
            text: i18nc("@label", "Author(s):")
            description: root.bookEntry.author.length > 0 ? root.bookEntry.author.join(', ') : i18nc("@info:placeholder", "Unknown")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@label", "Publisher:")
            description: root.bookEntry.publisher.length > 0 ? root.bookEntry.publisher : i18nc("@info:placeholder", "Unknown")
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextDelegate {
            text: i18nc("@label", "Location:")
            description: root.bookEntry.filename
        }
    }

    FormCard.FormHeader {
        title: i18nc("@title:group", "File Metadata")
    }

    FormCard.FormCard {
        FormCard.AbstractFormDelegate {
            id: ratingDelegate

            text: i18nc("@label", "Rating:")
            background: null
            contentItem: ColumnLayout {
                spacing: Kirigami.Units.smallSpacing

                Controls.Label {
                    Layout.fillWidth: true
                    text: ratingDelegate.text
                }

                Row {
                    id: ratingRow;

                    spacing: Kirigami.Settings.isMobile ? Kirigami.Units.smallSpacing : Math.round(Kirigami.Units.smallSpacing / 4)

                    Repeater{
                        model: [1, 3, 5, 7, 9]

                        Controls.AbstractButton {
                            activeFocusOnTab: true
                            width: height
                            height: Kirigami.Units.iconSizes.smallMedium
                            text: i18n("Set rating to %1", ratingTo)
                            property int ratingTo: {
                                if (root.rating === modelData + 1) {
                                    return modelData;
                                } else if (root.rating === modelData) {
                                    return modelData - 1;
                                } else {
                                    return modelData + 1;
                                }
                            }
                            contentItem: Kirigami.Icon {
                                source: root.rating > modelData ? "rating" : (root.rating < modelData ? "rating-unrated" : "rating-half")
                                width: parent.width
                                height: parent.height
                                color: (parent.focusReason == Qt.TabFocusReason || parent.focusReason == Qt.BacktabFocusReason) && parent.activeFocus ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                            }
                            onClicked: {
                                root.rating = ratingTo;
                                peruseConfig.setFilesystemProperty(root.bookEntry.filename, "rating", ratingTo)
                            }
                        }
                    }
                }
            }
        }

        FormCard.FormDelegateSeparator {}

        FormCard.FormTextFieldDelegate {
            id: tagsDelegate

            label: i18nc("@label", "Tags:")
            text: root.tags
            placeholderText: i18nc("Placeholder tag field", "(No tags)");
            onEditingFinished: {
                const tags = text.split(",");
                for (var i in tags) {
                    tags[i] = tags[i].trim();
                }
                root.tags = tags;
                peruseConfig.setFilesystemProperty(root.bookEntry.filename, "tags", tags.join(","))
            }
        }
    }
}
