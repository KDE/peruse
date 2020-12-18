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

#ifndef FILTERPROXY_H
#define FILTERPROXY_H

#include <QSortFilterProxyModel>

#include <memory>
/**
 * \brief a Filter proxy for handling search with.
 * 
 * Extends QSortFilterProxyModel, is caseInsensitive.
 */
class FilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
    Q_PROPERTY(bool filterBoolean READ filterBoolean WRITE setFilterBoolean NOTIFY filterBooleanChanged)
    Q_PROPERTY(int filterInt READ filterInt WRITE setFilterInt NOTIFY filterIntChanged)
    /**
     * If you for some reason need to toggle the filter off, you can turn the integer filtering off
     * by toggling this to false. Setting the filterInt property will set this to true.
     */
    Q_PROPERTY(bool filterIntEnabled READ filterIntEnabled WRITE setFilterIntEnabled NOTIFY filterIntEnabledChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit FilterProxy(QObject* parent = nullptr);
    ~FilterProxy() override;

    void setFilterString(const QString &string);
    QString filterString() const;
    Q_SIGNAL void filterStringChanged();

    void setFilterBoolean(const bool &value);
    bool filterBoolean() const;
    Q_SIGNAL void filterBooleanChanged();

    void setFilterInt(const int &value);
    int filterInt() const;
    Q_SIGNAL void filterIntChanged();

    void setFilterIntEnabled(const bool &value);
    bool filterIntEnabled() const;
    Q_SIGNAL void filterIntEnabledChanged();

    int count() const;
    Q_SIGNAL void countChanged();

    Q_INVOKABLE int sourceIndex(const int &filterIndex);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    class Private;
    std::unique_ptr<Private> d;
};

#endif//FILTERPROXY_H
