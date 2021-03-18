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

#ifndef ACBFBOOKINFO_H
#define ACBFBOOKINFO_H

#include <memory>

#include "AcbfMetadata.h"

#include <QHash>

/**
 * \brief Class for handling the book metadata.
 * 
 * ACBF book-info is all the metadata that relates
 * to the story inside.
 * 
 * This class holds all the authors, titles, languages,
 * summaries, genres, keywords, hashtags and more.
 * 
 * It also handles adding and removing, as well as storing
 * and reading it from the xml.
 * 
 * ACBF can hold multiple authors per book. Authors have their own object.
 * 
 * ACBF can hold titles, annotations(summaries or descriptions) and
 * a list of comma separated keywords in several languages.
 * 
 * Annotations in particular are a stringlist of paragraphs.
 * 
 * The library handles retrieving title, keywords and annotation when there is no
 * language defined as follows: It checks if there is an entry for "", if not, tries
 * the entry for the first language object, and if that doesn't work, returns the first
 * value it can find.
 * 
 * ACBF's language support is further detailed in the Language object.
 * 
 * ACBF can have multiple genres, but they are limited to a list of keys, given
 * by availableGenres().
 * Genres can also indicate how much they apply on the given story, using
 * a match percentage. This allows noting that a story is 80% romance, and
 * 20% western for example.
 * 
 * ACBF can also hold character names. Character names are a type of tag, and
 * especially relevant with American style multiverses, but also in Creative
 * Commons stories because the permissive licenses allow for easier reusage of
 * existing characters.
 * 
 * The coverpage is defined in the book-info as a Page object.
 * 
 * Finally, ACBF can hold metadata about database references, sequences and
 * content rating. All these too have their own objects.
 */

namespace AdvancedComicBookFormat
{
class Author;
class Page;
class Language;
class Sequence;
class DatabaseRef;
class ContentRating;
class ACBF_EXPORT BookInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList authorNames READ authorNames NOTIFY authorsChanged)
    Q_PROPERTY(QStringList titleLanguages READ titleLanguages NOTIFY titleChanged)
    Q_PROPERTY(QStringList genres READ genres NOTIFY genresChanged)
    Q_PROPERTY(QStringList characters READ characters WRITE setCharacters NOTIFY charactersChanged)
    Q_PROPERTY(QObjectList languages READ languages NOTIFY languagesChanged)
    Q_PROPERTY(QStringList languageEntryList READ languageEntryList NOTIFY languagesChanged)
    Q_PROPERTY(int sequenceCount READ sequenceCount NOTIFY sequenceCountChanged)
    Q_PROPERTY(int databaseRefCount READ databaseRefCount NOTIFY databaseRefCountChanged)
    Q_PROPERTY(int contentRatingCount READ contentRatingCount NOTIFY contentRatingCountChanged)
    Q_PROPERTY(bool rightToLeft READ rightToLeft WRITE setRightToLeft NOTIFY rightToLeftChanged)
public:
    explicit BookInfo(Metadata* parent = nullptr);
    ~BookInfo() override;

    Metadata* metadata();

    /**
     * \brief write the whole book-info section to the XML writer.
     */
    void toXml(QXmlStreamWriter *writer);
    
    /**
     * \brief load the whole book-info section from the xml into this object.
     * @return True if the xmlReader encountered no errors.
     */
    bool fromXml(QXmlStreamReader *xmlReader, const QString& xmlData);

    /**
     * @return The list of authors that worked on this book as author objects.
     */
    QList<Author*> author();
    
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
     * \brief add an author to the list.
     * @param author - the author object to add.
     */
    void addAuthor(Author* author);
    /**
     * \brief remove an author to the list.
     * @param author - the author object to remove.
     */
    void removeAuthor(Author* author);
    /**
     * \brief triggers when the authors list changes.
     */
    Q_SIGNAL void authorsChanged();

    /**
     * \brief this holds a list of titles regardless of language.
     * @return a stringlist with all the titles.
     */
    Q_INVOKABLE QStringList titleForAllLanguages();
    /**
     * @return a list of the languages the titles are in.
     */
    Q_INVOKABLE QStringList titleLanguages();
    /**
     * \brief get the title for the given language.
     * @param language - the language of which to return the title for.
     * @return The title for the given language code.
     * When no language is supplied, returns english title.
     */
    Q_INVOKABLE QString title(QString language = "");
    /**
     * \brief set the title for the given language code.
     * @param title - the title as a QString
     * @param language - the language code in language code, country
     * code format joined by a dash (not an underscore).
     */
    Q_INVOKABLE void setTitle(QString title, QString language = "");
    
    /**
     * \brief triggers when a title is set for any language.
     */
    Q_SIGNAL void titleChanged();

    /**
     * @return returns a hash of genre keys and their match percentage.
     */
    Q_INVOKABLE QHash<QString, int> genre();
    /**
     * @return a list of strings for the genres.
     */
    Q_INVOKABLE QStringList genres() const;
    /**
     * @param genre - the genre for which to return the percentage.
     * @return an integer between 0 to 100 representing the match
     * percentage of the given genre.
     */
    Q_INVOKABLE int genrePercentage(QString genre) const;
    /**
     * \brief Set a genre and their match percentage.
     * @param genre - the name of the genre to add.
     * @param matchPercentage - the percentage of how much this genre matches.
     */
    Q_INVOKABLE void setGenre(QString genre, int matchPercentage = 100);
    /**
     * \brief remove the genre from the hashlist.
     * @param genre - the genre name to remove.
     */
    Q_INVOKABLE void removeGenre(QString genre);
    /**
     * \brief triggers when genres are set or removed.
     */
    Q_SIGNAL void genresChanged();
    /**
     * \brief the list of approved genre names.
     */
    Q_INVOKABLE static QStringList availableGenres();

    /**
     * @return character names as a stringlist.
     */
    Q_INVOKABLE QStringList characters();
    /**
     * \brief add a character to the characters list.
     * @param name - the name of the character to add.
     */
    Q_INVOKABLE void addCharacter(QString name);
    /**
     * \brief remove a character from the character list.
     * @param name - the name of the character to remove.
     */
    Q_INVOKABLE void removeCharacter(QString name);

    /**
     * @brief Replace the characters stringlist.
     * @param characters new string list to use.
     */
    Q_INVOKABLE void setCharacters(QStringList characters);
    /**
     * \brief this triggers when the character name list is changed.
     */
    Q_SIGNAL void charactersChanged();

    /**
     * @return a list of annotations, which in turn are stringlists.
     */
    Q_INVOKABLE QList<QStringList> annotationsForAllLanguage();
    /**
     * @return a list of languages the annotation is available in.
     */
    Q_INVOKABLE QStringList annotationLanguages();
    /**
     * @param language - the language for which to return the annotation.
     * @return the annotation for the given language
     * as a stringlist of paragraphs
     */
    Q_INVOKABLE QStringList annotation(QString language = ""); // empty string means "default language", as (un)defined by the specification...
    /**
     * \brief set an annotation for the given language.
     * @param annotation - A stringlist of paragraphs which make
     * up the annotation.
     * @param language - The language for which to set the annotation in
     * language code, country code format joined by a dash (not an underscore).
     */
    Q_INVOKABLE void setAnnotation(QStringList annotation, QString language = "");

    /**
     * @return a hashmap of languages and the keyword stringlists.
     */
    QHash<QString, QStringList> keywordsForAllLanguage();
    /**
     * @param language - the language for which to return the keywords for.
     * @return a stringlist of keywords in the given language.
     */
    Q_INVOKABLE QStringList keywords(QString language = "");
    /**
     * \brief set the list of keywords for the given language.
     * @param keywords - A stringlist of keywords
     * @param language - The language for which to set the annotation in
     * language code, country code format joined by a dash (not an underscore).
     */
    Q_INVOKABLE void setKeywords(QStringList keywords, QString language = "");

    /**
     * @return the coverpage as a page object.
     */
    Q_INVOKABLE Page* coverpage();
    /**
     * \brief set a cover page.
     * @param newCover A page object with the new cover.
     */
    void setCoverpage(Page* newCover);

    /**
     * @return a list of language objects for determining translations.
     */
    QObjectList languages();
    /**
     * Fired when the list of languages changes
     */
    Q_SIGNAL void languagesChanged();
    /**
     * \brief add a language to the list of translations.
     * @param language - language object to add.
     */
    void addLanguage(Language* language);

    /**
     * @brief languageEntryList
     * @return a list of languages for the language text layers.
     */
    QStringList languageEntryList();

    /**
     * \brief add a language to the list of translations.
     * @param language - language for which to add a language object.
     * @param show - whether to set the language object to 'show'.
     */
    Q_INVOKABLE void addLanguage(QString language="", bool show=false);
    /**
     * \brief remove a language from the translations.
     * @param language - language object to remove.
     */
    void removeLanguage(Language* language);
    /**
     * @brief removeLanguage
     * @param index
     */
    Q_INVOKABLE void removeLanguage(int index);

    /**
     * @return a list of sequence objects that describe the series and
     * collections this book is part of.
     */
    QList<Sequence*> sequence();
    /**
     * \brief add a sequence object to indicate this book is part of one.
     * @param sequence - the sequence object that describes this book's place in
     * a sequence.
     */
    void addSequence(Sequence* sequence);

    /**
     * @brief add a sequence from basic strings.
     *
     * @param number - integer representing the number within the sequence.
     * @param title - the of the sequence, optional.
     * @param volume - volume of the sequence, optional.
     */
    Q_INVOKABLE void addSequence(int number, QString title = QString(), int volume = 0);
    /**
     * \brief remove a sequence object from the list of sequences this book is
     * part of.
     * @param sequence - the sequence object that describes this book's place in
     * a sequence.
     */
    void removeSequence(Sequence* sequence);

    /**
     * @brief remove a sequence by index.
     * @param index - index of sequence to remove.
     */
    Q_INVOKABLE void removeSequence(int index);
    /**
     * @brief get a sequence by index.
     * @param index = index of the sequence to get.
     */
    Q_INVOKABLE Sequence* sequence(int index) const;

    /**
     * @brief get total amount of sequences.
     */
    Q_INVOKABLE int sequenceCount() const;

    /**
     * @brief fires when the sequence count changes.
     */
    Q_SIGNAL void sequenceCountChanged();
    /**
     * @returns a list of entries that this book has in various databases.
     */
    QList<DatabaseRef*> databaseRef();
    /**
     * \brief add a database entry that this book has.
     * @param databaseRef - a databaseRef object describing this work's place
     * in a database.
     */
    void addDatabaseRef(DatabaseRef* databaseRef);
    /**
     * @brief add a database entry that this book has.
     * @param reference the entry itself.
     * @param dbname the name of the database.
     * @param type the type of entry, optional.
     */
    Q_INVOKABLE void addDatabaseRef(QString reference, QString dbname, QString type="");
    /**
     * \brief remove a database entry that this book has.
     * @param databaseRef - a databaseRef object describing this work's place
     * in a database.
     */
    void removeDatabaseRef(DatabaseRef* databaseRef);
    /**
     * @brief removeDatabaseRef
     * @param index of the database reference to remove.
     */
    Q_INVOKABLE void removeDatabaseRef(int index);

    /**
     * @brief get databaseRef
     * @param index - index of the database ref to get.
     * @return databaseTef at index.
     */
    Q_INVOKABLE DatabaseRef* databaseRef(int index);
    /**
     * @brief databaseRefCount
     * @return amount of database references.
     */
    Q_INVOKABLE int databaseRefCount();
    /**
     * @brief fires when databaseRefCount changes
     */
    Q_SIGNAL void databaseRefCountChanged();

    /**
     * @returns a list of contentRating objects describing the audience for this
     * book.
     */
    QList<ContentRating*> contentRating();
    /**
     * \brief add a contentRating object to the contentratings.
     * @param contentRating - a contentRating object describing the label and
     * contentrating system.
     */
    void addContentRating(ContentRating* contentRating);
    /**
     * @brief add a contentRating object to the contentratings.
     * @param rating - The label of the rating.
     * @param type - the system the label belongs to.
     */
    Q_INVOKABLE void addContentRating(QString rating, QString type = QString());
    /**
     * \brief remove a contentRating object from the contentRatings.
     * @param contentRating - a contentRating object describing the label and
     * contentrating system.
     */
    void removeContentRating(ContentRating* contentRating);
    /**
     * @brief removeContentRating
     * @param index the entry to remove.
     */
    Q_INVOKABLE void removeContentRating(int index);
    /**
     * @brief gets the contentrating by index.
     * @param index - index of the content rating to get.
     * @return the content rating for the index.
     */
    Q_INVOKABLE ContentRating* contentRating(int index);
    /**
     * @brief contentRatingCount
     * @return the total amount of content ratings in this book.
     */
    Q_INVOKABLE int contentRatingCount();
    /**
     * @brief fires when contentRatingCount changes.
     */
    Q_SIGNAL void contentRatingCountChanged();
    /**
     * @brief The reading direction for this comic for adjusting the layout.
     * An ACBF 1.2 feature.
     * @return true when the reading direction is right to left.
     */
    bool rightToLeft() const;
    
    /**
     * @brief set the Reading direction for this comic. This indicates
     * how the UI will lay out the buttons and controls.
     * An ACBF 1.2 feature.
     * @param rtl a boolean indicating whether the reading direction is right to left.
     */
    void setRightToLeft(const bool& rtl = false);
    /**
     * @brief fires when right to left changes.
     */
    Q_SIGNAL void rightToLeftChanged();
private:
    class Private;
    std::unique_ptr<Private> d;
};
}

#endif//ACBFBOOKINFO_H
