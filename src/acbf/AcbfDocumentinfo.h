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
/**
 * \brief Class to handle the DocumentInfo section.
 * 
 * DocumentInfo, in ACBF is the data about the ACBF file itself.
 * It contains the ACBF author, creation date, original source, version
 * number and a version history.
 * 
 * Sources is a stringlist, which is useful when the ACBF encompasses several
 * pages of a webcomic, for example.
 */
namespace AdvancedComicBookFormat
{
class Author;
class ACBF_EXPORT DocumentInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList authorNames READ authorNames NOTIFY authorsChanged)
    Q_PROPERTY(QStringList source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(float version READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QStringList history READ history WRITE setHistory NOTIFY historyChanged)
public:
    explicit DocumentInfo(Metadata* parent = nullptr);
    ~DocumentInfo() override;

    Metadata* metadata() const;

    /**
     * \brief write the documentinfo into the XML writer.
     */
    void toXml(QXmlStreamWriter *writer);
    /**
     * \brief load the DocumentInfo into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader, const QString& xmlData);

    /**
     * \brief the list of authors that worked on this specific acbf.
     * @return a list of author objects.
     */
    QList<Author*> author() const;
    /**
     * \brief add an Author object to the list of authors.
     * @param author - an author object.
     */
    void addAuthor(Author* author);
    /**
     * \brief remove an Author object from the list of authors.
     * @param author - an author object.
     */
    void removeAuthor(Author* author);

    /**
     * @return The list of authors that worked on this book as
     * a stringlist of names.
     */
    QStringList authorNames() const;

    /**
     * \brief get an author object by index.
     * @param index - the index of the author.
     */
    Q_INVOKABLE Author* getAuthor(int index) const;

    /**
     * \brief add an author to the list.
     * @param activity - the role this author played.
     * @param language - the language of the author in language code, country
     * code format joined by a dash (not an underscore).
     * @param firstName - the given name of the author.
     * @param middleName - the middle name(s) of the author as a string.
     * @param lastName - the family name of the author.
     * @param nickName - the nickname of the author.
     * @param homePages - a homepage url to associate with this author.
     * @param emails - an email address to associate with this author.
     */
    Q_INVOKABLE void addAuthor(QString activity, QString language, QString firstName, QString middleName, QString lastName, QString nickName, QStringList homePages, QStringList emails);
    /**
     * \brief make changes to an author in the list.
     * @param index - The index of this author in the author list.
     * @param activity - the role this author played.
     * @param language - the language of the author in language code, country
     * code format joined by a dash (not an underscore).
     * @param firstName - the given name of the author.
     * @param middleName - the middle name(s) of the author as a string.
     * @param lastName - the family name of the author.
     * @param nickName - the nickname of the author.
     * @param homePages - a homepage url to associate with this author.
     * @param emails - an email address to associate with this author.
     */
    Q_INVOKABLE void setAuthor(int index, QString activity, QString language, QString firstName, QString middleName, QString lastName, QString nickName, QStringList homePages, QStringList emails);

    /**
     * \brief remove an author in the list.
     * @param index - the index of the author to remove.
     */
    Q_INVOKABLE void removeAuthor(int index);
    /**
     * \brief triggers when the authors list changes.
     */
    Q_SIGNAL void authorsChanged();

    /**
     * @return a QDate with the creation date of this file.
     */
    QDate creationDate() const;
    /**
     * \brief set the creation date of this file.
     * @param creationDate - a QDate object holding the creation date.
     */
    void setCreationDate(const QDate& creationDate);

    /**
     * @return a list of sources that this ACBF is an adaptation of.
     */ 
    QStringList source() const;
    /**
     * \brief set the list of source that this ACBF is an adaptation of.
     * @param source - a stringlist of sources.
     */
    void setSource(const QStringList& source);

    /**
     * @brief remove the source  by index.
     * @param index - index of the source to remove.
     */
    Q_INVOKABLE void removeSource(int index);
    /**
     * @brief fires when the source stringlist is changed.
     */
    Q_SIGNAL void sourceChanged();

    /**
     * @returns the unique id of this ACBF document, used for cataloguing purposes.
     */
    QString id() const;
    /**
     * \brief set the unique ID for this file used for cataloguing purposes.
     * @param id - a unique ID. Can be any kind of string, a UUID is recommended as default.
     */
    void setId(const QString& id);

    /**
     * @return the version of this document as a float.
     */
    float version() const;
    /**
     * \brief set the version of this document
     * @param version - the version as a floating point number.
     */
    void setVersion(const float& version);
    /**
     * @brief fires when the version of the document changes.
     */
    Q_SIGNAL void versionChanged();
    /**
     * @return a list of history entries as a stringlist.
     */
    QStringList history() const;
    /**
     * \brief set the whole history list as a single stringlist.
     * @param history - the whole history of the document as a stringlist.
     */
    void setHistory(const QStringList& history);
    /**
     * \brief add a single entry to the document history.
     * @param historyLine - a single entry in the document.
     */
    void addHistoryLine(const QString& historyLine);
    /**
     * @brief remove the history line by index.
     * @param index - index of the line to remove.
     */
    Q_INVOKABLE void removeHistoryLine(int index);
    /**
     * @brief fires when the history stringlist changes.
     */
    Q_SIGNAL void historyChanged();
private:
    class Private;
    Private* d;
};
}

#endif//ACBFDOCUMENTINFO_H
