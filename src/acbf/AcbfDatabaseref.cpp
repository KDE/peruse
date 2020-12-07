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

#include "AcbfDatabaseref.h"

#include <QXmlStreamWriter>

#include <acbf_debug.h>

using namespace AdvancedComicBookFormat;

class DatabaseRef::Private
{
public:
    Private() {}
    QString dbname;
    QString type;
    QString reference;
};

DatabaseRef::DatabaseRef(BookInfo* parent)
    : QObject(parent)
    , d(new Private)
{
    static const int typeId = qRegisterMetaType<DatabaseRef*>("DatabaseRef*");
    Q_UNUSED(typeId);
}

DatabaseRef::~DatabaseRef() = default;

void DatabaseRef::toXml(QXmlStreamWriter* writer)
{
    writer->writeStartElement(QStringLiteral("databaseref"));
    writer->writeAttribute(QStringLiteral("dbname"), d->dbname);
    if(!d->type.isEmpty()) {
        writer->writeAttribute(QStringLiteral("type"), d->type);
    }
    writer->writeCharacters(d->reference);
    writer->writeEndElement();
}

bool DatabaseRef::fromXml(QXmlStreamReader *xmlReader)
{
    setDbname(xmlReader->attributes().value(QStringLiteral("dbname")).toString());
    setType(xmlReader->attributes().value(QStringLiteral("type")).toString());
    setReference(xmlReader->readElementText(QXmlStreamReader::IncludeChildElements));
    qCDebug(ACBF_LOG) << Q_FUNC_INFO << "Created a database reference for the database" << dbname() << "with reference" << reference();
    return true;
}

QString DatabaseRef::dbname() const
{
    return d->dbname;
}

void DatabaseRef::setDbname(const QString& dbname)
{
    d->dbname = dbname;
    emit dbnameChanged();
}

QString DatabaseRef::type() const
{
    return d->type;
}

void DatabaseRef::setType(const QString& type)
{
    d->type = type;
    emit typeChanged();
}

QString DatabaseRef::reference() const
{
    return d->reference;
}

void DatabaseRef::setReference(const QString& reference)
{
    d->reference = reference;
    emit referenceChanged();
}
