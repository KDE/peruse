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

#ifndef PROPERTYCONTAINER_H
#define PROPERTYCONTAINER_H

#include <QObject>
#include <QVariant>

/**
 * The only purpose of this class is to expose the dynamic property
 * system of Qt to QML, so we can set and get properties on a generic
 * object. It is a little bit of a hack, but QML deliberately does
 * not have access to this (according to the developers).
 */
class PropertyContainer : public QObject
{
    Q_OBJECT
public:
    explicit PropertyContainer(QObject* parent = nullptr);
    explicit PropertyContainer(QString name, QObject* parent = nullptr);
    ~PropertyContainer() override;

    // As QObject already as setProperty and property() functions, we must
    // name ours differently
    Q_INVOKABLE void writeProperty(QString name, QVariant value);
    Q_INVOKABLE QVariant readProperty(QString name);

    Q_INVOKABLE QString name();
private:
    QString m_name;
};

#endif // PROPERTYCONTAINER_H
