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

#ifndef ACBFAUTHORINFO_H
#define ACBFAUTHORINFO_H

#include "AcbfMetadata.h"

namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Author : public QObject
{
    Q_OBJECT
public:
    // Author is used in both Bookinfo and Documentinfo, and as such it is parented to Metadata
    explicit Author(Metadata* parent = 0);
    virtual ~Author();

    void toXml(QXmlStreamWriter* writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QString activity();
    void setActivity(QString activity);
    static QStringList availableActivities();

    // The language this author worked in for this book. Activity dependent (writer, translator, letterer...)
    QString language();
    void setLanguage(QString language = "");

    QString firstName();
    void setFirstName(QString name);
    QString middleName();
    void setMiddleName(QString name);
    QString lastName();
    void setLastName(QString name);
    QString nickName();
    void setNickName(QString name);
    QString homePage();
    void setHomePage(QString homepage);
    QString email();
    void setEmail(QString email);
private:
    class Private;
    Private* d;
};
}

#endif//ACBFAUTHORINFO_H
