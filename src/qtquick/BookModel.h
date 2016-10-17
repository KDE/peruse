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

#ifndef BOOKMODEL_H
#define BOOKMODEL_H

#include <QAbstractListModel>

class BookModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY authorChanged)
    Q_PROPERTY(QString publisher READ publisher WRITE setPublisher NOTIFY publisherChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    /**
     * The Advanced Comic Book Format data management instance associated with this book
     * This may be null
     */
    Q_PROPERTY(QObject* acbfData READ acbfData NOTIFY acbfDataChanged)
    Q_PROPERTY(bool processing READ processing WRITE setProcessing NOTIFY processingChanged)
public:
    explicit BookModel(QObject* parent = 0);
    virtual ~BookModel();

    enum Roles {
        UrlRole = Qt::UserRole + 1,
        TitleRole,
    };

    virtual QHash<int, QByteArray> roleNames() const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    virtual void addPage(QString url, QString title);
    virtual void clearPages();

    QString filename() const;
    virtual void setFilename(QString newFilename);
    Q_SIGNAL void filenameChanged();
    virtual QString author() const;
    virtual void setAuthor(QString newAuthor);
    Q_SIGNAL void authorChanged();
    virtual QString publisher() const;
    virtual void setPublisher(QString newPublisher);
    Q_SIGNAL void publisherChanged();
    virtual QString title() const;
    virtual void setTitle(QString newTitle);
    Q_SIGNAL void titleChanged();
    virtual int pageCount() const;
    Q_SIGNAL void pageCountChanged();

    int currentPage() const;
    /**
     *
     * @param updateFilesystem If this is set to false, the attributes do not get written back to the filesystem. Useful for when the information is first filled out
     */
    virtual void setCurrentPage(int newCurrentPage, bool updateFilesystem = true);
    Q_SIGNAL void currentPageChanged();

    QObject* acbfData() const;
    /**
     * This is used by subclasses who want to create one such. Until this is called
     * with a valid object, acbfData is null. This function causes BookModel to take
     * ownership of the object. It will further delete any previous objects set as
     * acbfData.
     */
    void setAcbfData(QObject* obj);
    Q_SIGNAL void acbfDataChanged();

    // Whether or not the any processing is currently going on
    bool processing() const;
    void setProcessing(bool processing);
    Q_SIGNAL void processingChanged();
    Q_SIGNAL void loadingCompleted(bool success);

    /**
     * @brief Swap the two pages at the specified indices
     * 
     * @param swapThisIndex The index of the first page to be swapped
     * @param withThisIndex The index of the page you want the first to be swapped with
     */
    Q_INVOKABLE virtual void swapPages(int swapThisIndex, int withThisIndex);
private:
    class Private;
    Private* d;
};

#endif//BOOKMODEL_H
