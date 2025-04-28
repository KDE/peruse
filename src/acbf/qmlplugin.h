// SPDX-FileCopyrigthText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-only or LGPL-3.0-only or LicenseRef-KDE-Accepted-LGPL

#include <qqmlintegration.h>
#include "AcbfStyleSheet.h"
#include "AcbfStyle.h"

class AcbfStyleSheetForeign : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(StyleSheet)
    QML_FOREIGN(AdvancedComicBookFormat::StyleSheet)
};

class AcbfStyleForeign : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Style)
    QML_FOREIGN(AdvancedComicBookFormat::Style)
};
