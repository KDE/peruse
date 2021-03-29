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
/**
 * \brief Class to handle the textlayer element.
 * 
 * ACBF textlayers are groupings of textareas
 * according to a specific language.
 * 
 * They also have a default textarea background color.
 */
namespace AdvancedComicBookFormat
{
class Textarea;
class ACBF_EXPORT Textlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString bgcolor READ bgcolor WRITE setBgcolor NOTIFY bgcolorChanged)
    Q_PROPERTY(QStringList textareaPointStrings READ textareaPointStrings NOTIFY textareaPointStringsChanged)
    Q_PROPERTY(QObjectList textareas READ textareas NOTIFY textareasChanged)
public:
    explicit Textlayer(Page* parent = nullptr);
    ~Textlayer() override;

    /**
     * \brief Write the textlayer into the xml writer.
     */
    void toXml(QXmlStreamWriter* writer);
    /**
     * \brief load a textlayer element into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader, const QString& xmlData);

    /**
     * @returns the language for this text-layer.
     */
    QString language() const;
    /**
     * \brief set the language for this text-layer.
     * @param language - the language of the entry in language code, country
     * code format joined by a dash (not an underscore).
     */
    void setLanguage(const QString& language);
    /**
     * @brief fires when the language changes.
     */
    Q_SIGNAL void languageChanged();

    /**
     * @return the background color as a QString.
     * 
     * It should be an 8bit per channel rgb hexcode.
     */
    QString bgcolor() const;
    /**
     * \brief set the background color.
     * 
     * @param newColor - a String with an 8bit per channel rgb hexcode (#ff00ff, or the like)
     */
    void setBgcolor(const QString& newColor = QString());
    /**
     * @brief fires when the background color changes.
     */
    Q_SIGNAL void bgcolorChanged();

    /**
     * @returns a list of textareas in this page.
     */
    QObjectList textareas() const;
    Q_SIGNAL void textareaAdded(Textarea* newArea);
    /**
     * Fires when textareas changes
     */
    Q_SIGNAL void textareasChanged();
    int textAreaIndex(Textarea* textarea);
    /**
     * @param index - index of the textarea.
     * @return the textarea of that index.
     */
    Q_INVOKABLE Textarea* textarea(int index) const;
    /**
     * @param textarea - the textarea you want to index of.
     * @returns the index of the given textarea.
     */
    Q_INVOKABLE int textareaIndex(Textarea* textarea) const;

    /**
     * \brief add a textarea to the list of frames.
     * @param textarea - the textarea to add.
     * @param index - the index to add it at. If afterIndex is larger than
     * zero, the insertion will happen at that index
     */
    void addTextarea(Textarea* textarea, int index = -1);
    /**
     * @brief add a text area at index.
     * @param index - the index to add it at. If afterIndex is larger than
     * zero, the insertion will happen at that index
     */
    Q_INVOKABLE void addTextarea(int index = -1);
    /**
     * \brief remove the given textarea from the framelist.
     * @param textarea - the textarea to remove.
     */
    void removeTextarea(Textarea* textarea);
    /**
     * @brief addTextarea
     * @param index to remove the textarea at.
     */
    Q_INVOKABLE void removeTextarea(int index);
    /**
     * \brief Swap two textareas in the list.
     * @param swapThis - the first textarea to swap.
     * @param withThis - the second textarea to swap.
     */
    Q_INVOKABLE bool swapTextareas(int swapThis, int withThis);
    /**
     * @brief a list with the textareas.
     * This is a hack to ensure that qml repeaters and listviews update
     * properly when the textareas are swapped.
     * @return a QStringList with textareas and their points.
     */
    QStringList textareaPointStrings();
    /**
     * @brief textareaCountChanged
     */
    Q_SIGNAL void textareaPointStringsChanged();
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFTEXTLAYER_H
