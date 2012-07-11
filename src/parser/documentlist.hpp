/* *****************************************************************************
 * 
 * This file is part of the Restad project
 * https://sourceforge.net/projects/restad/
 * Author : Yoann Moreau (moreau.yo * at * gmail.com)
 *
 * Copyright (C) 2012 LIA (www.lia.univ-avignon.fr)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see 
 * <http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>.
 * 
 ******************************************************************************/

#ifndef DOCUMENTLIST_HPP
#define DOCUMENTLIST_HPP

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QRegExp>
class QMutex;

#include "../parser/parserfactory.hpp"

//! Data structure for document info
/*! \ingroup global */
class DocumentInfo
{
public:
    //! Initializes an empty (invalid) document-info
    DocumentInfo();
    DocumentInfo(ParserFactory::FileType fileType, int sourceId, const QString &uri, const QString &docSeparator = QString());

    //! Returns true if not empty
    bool isValid() const;

    ParserFactory::FileType fileType() const;
    int sourceId() const;
    QString uri() const;
    QString docSeparator() const;

private:
    ParserFactory::FileType _fileType;
    int _sourceId;
    QString _uri;
    QString _docSeparator;
};

//! Manages a document map
/*! \ingroup global */
class DocumentList
{
public:
    DocumentList();

    //! Adds a document to the list
    /*! The function does not prevent from adding a document more than once. */
    void addDocument(ParserFactory::FileType docType, int sourceId, const QString &uri, const QString &docSeparator = QString());

    //! Removes a document from the list with the given type and returns its info
    /*! Thread-safe.
        \return An empty document-info if nothing can be found for the given file type.
      */
    DocumentInfo takeDocument(ParserFactory::FileType fileType);

    //! Returns the list of the document files id
    QList<int> sourceIds() const;

    //! Returns document count
    int size() const;

    //! Returns the file type which has the highest total of documents in the list
    /*! Thread-safe. */
    ParserFactory::FileType mostWaitingType() const;

private:
    QMutex *_mutex;
    QList<DocumentInfo> _list;
    ParserFactory::TypeCountMap _typeCountMap;
};

#endif // DOCUMENTLIST_HPP
