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

#ifndef ACBFBODY_H
#define ACBFBODY_H

#include <memory>

#include "acbf_export.h"

#include <QDate>
#include <QObject>

class QXmlStreamWriter;
class QXmlStreamReader;
/**
 * \brief Class to handle the body section of ACBF.
 * 
 * ACBF's body section holds all the pages. Beyond that,
 * it has a bgcolor. The presence of the body section
 * is mandatory.
 * 
 * This class can load and save the body section.
 * It also holds the page objects and allows
 * ordering/adding/removing them.
 */
namespace AdvancedComicBookFormat
{
class Page;
class Document;
class ACBF_EXPORT Body : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString bgcolor READ bgcolor WRITE setBgcolor NOTIFY bgcolorChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
public:
    explicit Body(Document* parent = nullptr);
    ~Body();

    Document* document() const;

    /**
     * \brief write body data into the XMLWriter.
     */
    void toXml(QXmlStreamWriter *writer);
    
    /**
     * \brief Load data from the xml into this body object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader, const QString& xmlData);

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
    void setBgcolor(const QString& newColor);
    /**
     * @brief fires when the background color changes.
     */
    Q_SIGNAL void bgcolorChanged();

    /**
     * @return a QList of all the pages stored currently.
     */
    QList<Page*> pages() const;

    /**
     * @brief Fires when a new page is added to the list of pages
     * @param The newly added page
     */
    Q_SIGNAL void pageAdded(Page* newPage);

    /**
     * @param index - the index of the page.
     * @return the page object at the given index.
     */
    Q_INVOKABLE Page* page(int index) const;

    /**
     * @param page - The page for which to return the index.
     * @return index of the page given, will return -1 if the page isn't in this document.
     */
    int pageIndex(Page* page) const;

    /**
     * Add a page to the list, optionally at a specified position in the list
     * @param page The page to add to the list
     * @param index The position at which to insert the page (if -1, the page will be appended)
     */
    void addPage(Page* page, int index = -1);

    /**
     * \brief remove the given page object from this body.
     * @param page - the page to remove.
     */
    void removePage(Page* page);
    /**
     * \brief Swap two pages in the list.
     * @param swapThis - the first page to swap.
     * @param withThis - the second page to swap.
     */
    bool swapPages(Page* swapThis, Page* withThis);
    /**
     * @brief pageCount
     * @return
     */
    int pageCount();
    /**
     * @brief pageCountChanged
     */
    Q_SIGNAL void pageCountChanged();

private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFBODY_H
