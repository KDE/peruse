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

    Metadata* metadata();

    void toXml(QXmlStreamWriter *writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QList<Author*> author();
    void addAuthor(Author* author);
    void removeAuthor(Author* author);

    QDate creationDate();
    void setCreationDate(QDate creationDate);

    QStringList source();
    void setSource(QStringList source);

    QString id();
    void setId(QString id);

    QString version();
    void setVersion(QString version);

    QStringList history();
    void setHistory(QStringList history);
    void addHistoryLine(QString historyLine);
private:
    class Private;
    Private* d;
};
}

#endif//ACBFDOCUMENTINFO_H
