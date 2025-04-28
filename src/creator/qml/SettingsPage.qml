// SPDX-FileCopyrightText: 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

import QtQuick

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard

/**
 * @brief Page with knobs and dials for configuring Peruse Creator.
 */
FormCard.FormCardPage {
    id: root

    readonly property string categoryName: "settingsPage"

    title: i18nc("@title", "Settings")
}
