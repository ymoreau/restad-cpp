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

#ifndef GLOBALPARSERDATABASE_HPP
#define GLOBALPARSERDATABASE_HPP

#include <QtCore/QHash>
class QMutex;

typedef QHash<QString, int> NameMap;

#include "../global/database.hpp"
#include "../parser/documentlist.hpp"

//! %Database for global (thread-safe) parser use.
/*! \ingroup parser */
class GlobalParserDatabase : public Database
{
public:
    GlobalParserDatabase();

    //! Refresh the buffer map of the tag names
    /*! \warning Not thread-safe !
        \throw DatabaseException If a connection error occurs */
    void refreshTagNames();

    //! Refresh the buffer map of the attribute names
    /*! \warning Not thread-safe !
        \throw DatabaseException If a connection error occurs */
    void refreshAttributeNames();

    //! Gets count documents with the Waiting status
    /*! \warning Not thread-safe !
        \param count The max count of documents to get, set to -1 for no limit
        \throw DatabaseException If a connection error occurs */
    DocumentList getDocuments(int count) const;

    //! Returns an available document id
    /*! Thread-safe.
        \throw DatabaseException If a connection error occurs */
    int generateDocumentId() const;

    //! Returns the tag name id
    /*! Thread-safe.
        It will add the tag name if unknown and then cache again the buffer map.
        \throw DatabaseException If a connection error occurs */
    int tagNameId(const QString &tagName);

    //! Returns the attribute name id
    /*! Thread-safe.
        It will add the attribute name if unknown and then cache again the buffer map.
        \throw DatabaseException If a connection error occurs */
    int attributeNameId(const QString &attributeName);

    //! Update the file status to 'waiting'
    /*! Thread-safe.
        \throw DatabaseException If a connection error occurs */
    void cancelFile(int idFile, bool toError);

private:
    QMutex *_mutex;
    NameMap _tagNames;
    NameMap _attributeNames;

    //! Adds the tag name to the database
    /*! It will refresh the cache after adding the tag name.
        \throw DatabaseException If a connection error occurs */
    QString addTagName(const QString &tagName);

    //! Adds the attribute name to the database
    /*! It will refresh the cache after adding the attribute name.
        \throw DatabaseException If a connection error occurs */
    QString addAttributeName(const QString &attributeName);
};

#endif // GLOBALPARSERDATABASE_HPP
