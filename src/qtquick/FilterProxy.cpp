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

#include "FilterProxy.h"

#include <QTimer>

class FilterProxy::Private {
public:
    Private() {
        updateTimer.setInterval(1);
        updateTimer.setSingleShot(true);
    }
    bool filterBoolean{false};
    bool filterIntEnabled{false};
    int filterInt{INT_MIN}; // INT_MIN to ensure that we actually hold true to that thing where we said we'd change the filterIntEnabled thing as well...
    QTimer updateTimer;
};

FilterProxy::FilterProxy(QObject* parent)
    : QSortFilterProxyModel(parent)
    , d(new Private)
{
    connect(&d->updateTimer, &QTimer::timeout, this, [this](){
        Q_EMIT countChanged();
        sort(0);
    } );
    connect(this, &QAbstractItemModel::rowsInserted, this, [this](){ d->updateTimer.start(); });
    connect(this, &QAbstractItemModel::rowsRemoved, this, [this](){ d->updateTimer.start(); });
    connect(this, &QAbstractItemModel::dataChanged, this, [this](){ d->updateTimer.start(); });
    connect(this, &QAbstractItemModel::layoutChanged, this, [this](){ d->updateTimer.start(); });
    connect(this, &QAbstractItemModel::modelReset, this, [this](){ d->updateTimer.start(); });
    setDynamicSortFilter(true);
}

FilterProxy::~FilterProxy() = default;

void FilterProxy::setFilterString(const QString &string)
{
    QSortFilterProxyModel::setFilterFixedString(string);
    QSortFilterProxyModel::setFilterCaseSensitivity(Qt::CaseInsensitive);
    emit filterStringChanged();
}

QString FilterProxy::filterString() const
{
    return filterRegExp().pattern();
}

void FilterProxy::setFilterBoolean(const bool& value)
{
    d->filterBoolean = value;
    emit filterBooleanChanged();
}

bool FilterProxy::filterBoolean() const
{
    return d->filterBoolean;
}

bool FilterProxy::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (d->filterBoolean) {
        return sourceModel()->data(index, filterRole()).toBool();
    } else if (d->filterIntEnabled) {
        return (sourceModel()->data(index, filterRole()).toInt() == d->filterInt);
    } else {
        return sourceModel()->data(index, filterRole()).toString().contains(filterRegExp());
    }
}

int FilterProxy::count() const
{
    return rowCount();
}

int FilterProxy::filterInt() const
{
    return d->filterInt;
}

void FilterProxy::setFilterInt(const int& value)
{
    if (d->filterInt != value) {
        d->filterInt = value;
        setFilterIntEnabled(true);
        Q_EMIT filterIntChanged();
    }
}

bool FilterProxy::filterIntEnabled() const
{
    return d->filterIntEnabled;
}

void FilterProxy::setFilterIntEnabled(const bool& value)
{
    if (d->filterIntEnabled != value) {
        d->filterIntEnabled = value;
        Q_EMIT filterIntEnabledChanged();
    }
}

int FilterProxy::sourceIndex ( const int& filterIndex )
{
    int mappedIndex{-1};
    QModelIndex ourIndex = index(filterIndex, 0);
    if (ourIndex.isValid() && sourceModel()) {
        mappedIndex = mapToSource(ourIndex).row();
    }
    return mappedIndex;
}
