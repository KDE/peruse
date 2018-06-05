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

#ifndef ACBFDOCUMENTINFO_H
#define ACBFDOCUMENTINFO_H

#include <memory>

#include "AcbfMetadata.h"

#include <QDate>

namespace AdvancedComicBookFormat
{
class Author;
class ACBF_EXPORT DocumentInfo : public QObject
{
    Q_OBJECT
public:
    explicit DocumentInfo(Metadata* parent = nullptr);
    ~DocumentInfo() override;

    Metadata* metadata() const;

    void toXml(QXmlStreamWriter *writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QList<Author*> author() const;
    void addAuthor(Author* author);
    void removeAuthor(Author* author);

    QDate creationDate() const;
    void setCreationDate(const QDate& creationDate);

    QStringList source() const;
    void setSource(const QStringList& source);

    QString id() const;
    void setId(const QString& id);

    QString version() const;
    void setVersion(const QString& version);

    QStringList history() const;
    void setHistory(const QStringList& history);
    void addHistoryLine(const QString& historyLine);
private:
    class Private;
    Private* d;
};
}

#endif//ACBFDOCUMENTINFO_H
