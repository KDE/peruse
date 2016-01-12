/*
 * Copyright (C) 2016 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "PeruseConfig.h"

#include <KConfig>
#include <KConfigGroup>

class PeruseConfig::Private
{
public:
    Private()
        : config("peruserc")
    {};
    KConfig config;
};

PeruseConfig::PeruseConfig(QObject* parent)
    : QObject(parent)
    , d(new Private)
{
}

PeruseConfig::~PeruseConfig()
{
    delete d;
}

void PeruseConfig::bookOpened(QString path)
{
    QStringList recent = recentlyOpened();

    int i = recent.indexOf(path);
    if(i == 0)
    {
        // This is already first, don't do work we don't need to, because that's just silly
        return;
    }
    else if(i > 0)
    {
        QString oldFirst = recent[0];
        recent[0] = recent[i];
        recent[i] = oldFirst;
    }
    else
    {
        recent[2] = recent[1];
        recent[1] = recent[0];
        recent[0] = path;
    }
    d->config.group("general").writeEntry("recently opened", recent);
    d->config.sync();
    emit recentlyOpenedChanged();
}

QStringList PeruseConfig::recentlyOpened() const
{
    return d->config.group("general").readEntry("recently opened", QStringList() << "" << "" << "");
}
