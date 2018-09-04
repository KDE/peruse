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

#ifndef FOLDERBOOKMODEL_H
#define FOLDERBOOKMODEL_H

#include "BookModel.h"
/**
 * \brief Class to handle a folder of images as a book.
 * 
 * This class will treat a folder of images as a book, with the alphabetical
 * order of the files determining the page order.
 */
class FolderBookModel : public BookModel
{
    Q_OBJECT
public:
    explicit FolderBookModel(QObject* parent = nullptr);
    ~FolderBookModel() override;

    /**
     * \brief Set the folder name of this book or a file describing
     * describing the images that belong to this book.
     */
    void setFilename(QString newFilename) override;
};

#endif//FOLDERBOOKMODEL_H
