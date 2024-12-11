// SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <qqmlintegration.h>
#include <qqmlregistration.h>

#include "AcbfBinary.h"
#include "AcbfReference.h"
#include "AcbfStyle.h"
#include "AcbfIdentifiedObjectModel.h"

class IdentifiedObjectModelForeign : public QObject
{
    Q_OBJECT
    QML_FOREIGN(AdvancedComicBookFormat::IdentifiedObjectModel);
    QML_NAMED_ELEMENT(IdentifiedObjectModel);
};

class ReferenceForeign : public QObject
{
    Q_OBJECT
    QML_FOREIGN(AdvancedComicBookFormat::Reference);
    QML_NAMED_ELEMENT(Reference);
    QML_UNCREATABLE("Don't attempt to create ACBF types directly, use the convenience functions on their container types for creating them")
};

class BinaryForeign : public QObject
{
    Q_OBJECT
    QML_FOREIGN(AdvancedComicBookFormat::Binary);
    QML_NAMED_ELEMENT(Binary);
    QML_UNCREATABLE("Don't attempt to create ACBF types directly, use the convenience functions on their container types for creating them")
};

class StyleForeign : public QObject
{
    Q_OBJECT
    QML_FOREIGN(AdvancedComicBookFormat::Style);
    QML_NAMED_ELEMENT(Style);
    QML_UNCREATABLE("Don't attempt to create ACBF types directly, use the convenience functions on their container types for creating them")
};

