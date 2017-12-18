/*
 * Copyright (C) 2015 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ACBFDOCUMENT_H
#define ACBFDOCUMENT_H

#include <QObject>
#include "acbf_export.h"

namespace AdvancedComicBookFormat
{
class Metadata;
class Body;
// class References;
class Data;
// class Stylesheet;
class ACBF_EXPORT Document : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Metadata* metaData READ metaData NOTIFY metaDataChanged)
public:
    explicit Document(QObject* parent = 0);
    ~Document() override;

    QString toXml();
    bool fromXml(QString xmlDocument);

    Metadata* metaData();
    Q_SIGNAL void metaDataChanged();

    Body* body();
    // References* references();
    Data* data();
    // Stylesheet* stylesheet();
private:
    class Private;
    Private* d;
};
}

#endif//ACBFDOCUMENT_H
