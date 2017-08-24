/*
 * Copyright (C) 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "PropertyContainer.h"

PropertyContainer::PropertyContainer::PropertyContainer(QObject* parent)
    : QObject(parent)
{
    m_name = "unnamed";
}


PropertyContainer::PropertyContainer(QString name, QObject* parent)
    : QObject(parent)
    , m_name(name)
{
}

PropertyContainer::~PropertyContainer()
{
}

void PropertyContainer::writeProperty(QString name, QVariant value)
{
    setProperty(name.toLatin1(), value);
}

QVariant PropertyContainer::readProperty(QString name)
{
    return property(name.toLatin1());
}

QString PropertyContainer::name()
{
    return m_name;
}
