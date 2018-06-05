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

#ifndef ACBFTEXTLAYER_H
#define ACBFTEXTLAYER_H

#include <memory>

#include "AcbfPage.h"

namespace AdvancedComicBookFormat
{
class Textarea;
class ACBF_EXPORT Textlayer : public QObject
{
    Q_OBJECT
public:
    explicit Textlayer(Page* parent = nullptr);
    ~Textlayer() override;

    void toXml(QXmlStreamWriter* writer);
    bool fromXml(QXmlStreamReader *xmlReader);

    QString language() const;
    void setLanguage(const QString& language);

    QString bgcolor() const;
    void setBgcolor(const QString& newColor = QString());

    QList<Textarea*> textareas() const;
    Textarea* textarea(int index) const;
    int textareaIndex(Textarea* textarea) const;

    // If afterIndex is larger than zero, the insertion will happen at that index
    void addTextarea(Textarea* textarea, int index = -1);
    void removeTextarea(Textarea* textarea);
    bool swapTextareas(Textarea* swapThis, Textarea* withThis);
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFTEXTLAYER_H
