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
#include <memory>

/**
 * \brief Class for handling ACBF style authors.
 * 
 * ACBF style authors have their names split up into
 * first/middle/last and nickname. This is because
 * traditionally archives use a lastname, first name
 * format to catalogue the entry, while in the west
 * their proper name is firstname/lastname.
 * 
 * Nickname is particularly necessary for comics because
 * many artists use a nom de plume of sorts.
 * 
 * activity is similar to EPUB role, but where EPUB uses
 * MARC relators, ACBF has its own limited list of activities.
 * These are available as a stringlist from
 * the available activities function.
 * 
 * Authors also have two contact options
 * available(homepage and email), the official xsd
 * allows multiple of these.
 * 
 * Authors can also have a language assigned.
 * This is relevant for translator in particular, but
 * the official xsd does not invalidate any other with
 * the attribute.
 * 
 * Authors should, at minimum have a name, which is either
 * a nickname, or a first and lastname.
 * 
 */

namespace AdvancedComicBookFormat
{
class ACBF_EXPORT Author : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList homePages READ homePages WRITE setHomePages NOTIFY homePagesChanged)
    Q_PROPERTY(QStringList emails READ emails WRITE setEmails NOTIFY emailsChanged)
public:
    // Author is used in both Bookinfo and Documentinfo, and as such it is parented to Metadata
    explicit Author(Metadata* parent = nullptr);
    ~Author();

    /**
     * Write the data from this author object into the XmlStream.
     */
    void toXml(QXmlStreamWriter* writer);
    
    /**
     * \brief Load the data from the acbf xml into this author object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader);

    /**
     * \brief convenience function to put together the different parts
     * of the author's name into a single string.
     * @return a single string for the author's name.
     */
    Q_INVOKABLE QString displayName() const;

    /**
     * @return The activity this author performed on the book.
     */
    Q_INVOKABLE QString activity() const;
    
    /**
     * \brief Set the activity this author performed on the book.
     * @param activity - the activity as a string, should be an entry
     * of availableActivities().
     */
    Q_INVOKABLE void setActivity(const QString& activity);
    
    /**
     * @return A list of all the approved activities.
     */
    Q_INVOKABLE static QStringList availableActivities();

    /**
     * \brief The language this author worked in for this book. Activity dependent (writer, translator, letterer...)
     * @return The language in xs:language style language-COUNTRY format.
     */
    Q_INVOKABLE QString language() const;

    /**
     * \brief Set the language used by this author, in particular necessary for translator.
     * 
     * @param language should be xs:language, which is BCP 47 minus the script.
     */
    Q_INVOKABLE void setLanguage(const QString& language = QString());

    /**
     * @return the given name of this author as a QString.
     */
    Q_INVOKABLE QString firstName() const;
    
    /**
     * \brief Set the given name of this author.
     * @param name - the given name as a string.
     */
    Q_INVOKABLE void setFirstName(const QString& name);
    
    /**
     * @return the middles name(s) of this author as a QString.
     */
    Q_INVOKABLE QString middleName() const;
    
    /**
     * \brief Set the middles name(s) of this author.
     * @param name - the middle name(s) as a string.
     */
    Q_INVOKABLE void setMiddleName(const QString& name);
    
    /**
     * @return the family name of this author as a QString.
     */
    Q_INVOKABLE QString lastName() const;
    
    /**
     * \brief Set the family name of this author.
     * @param name - the family name as a string.
     */
    Q_INVOKABLE void setLastName(const QString& name);
    
    /**
     * @return the nick name of this author as a QString.
     */
    Q_INVOKABLE QString nickName() const;
    
    /**
     * \brief set the nick name of this author.
     * @param name - the nickname as a string.
     */
    Q_INVOKABLE void setNickName(const QString& name);
    
    /**
     * @return the homepages associated with this author as a QStringList.
     */
    Q_INVOKABLE QStringList homePages() const;
    
    /**
     * \brief Add a homepage associated with this author.
     * @param homepage - the url of the homepage as a string.
     */
    Q_INVOKABLE void addHomePage(const QString& homepage);
    /**
     * @brief remove homepage by index.
     * @param index - index of the homepage to remove.
     */
    Q_INVOKABLE void removeHomePage(const int& index);
    /**
     * \brief Set the homepages associated with this author.
     * @param homepages - homepages as a string.
     */
    Q_INVOKABLE void setHomePages(const QStringList& homepages);
    /**
     * @brief fires when the homepage list changes.
     */
    Q_SIGNAL void homePagesChanged();
    
    /**
     * @return The email adresses associated with this author as a QStringList.
     */
    Q_INVOKABLE QStringList emails() const;
    
    /**
     * \brief Set the email adress associated with this author.
     * @param email - email as a string.
     */
    Q_INVOKABLE void addEmail(const QString& email);
    /**
     * @brief remove email by index.
     * @param index - index of the email to remove.
     */
    Q_INVOKABLE void removeEmail(const int& index);
    /**
     * \brief Add an email adresses associated with this author.
     * @param emails - email dresses as a stringlist.
     */
    Q_INVOKABLE void setEmails(const QStringList& emails);
    /**
     * @brief fires when the email list changes.
     */
    Q_SIGNAL void emailsChanged();
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFAUTHORINFO_H
