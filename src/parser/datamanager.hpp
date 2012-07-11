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

#ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include "../global/database.hpp"
class GlobalParserDatabase;
class Tag;

//! Manages the data buffers and interface for the parser
/*! \ingroup parser
    This class is used by the parser and should only call local or thread-safe functions.
*/
class DataManager : public Database
{
public:
    DataManager(GlobalParserDatabase &globalDatabase);

    //! Initializes the buffers for a new document
    /*! \throw DatabaseException If a connection error occurs */
    void startDocument(int sourceId);

    //! Sends the current document data to the database
    /*! \param validateSource If set, update the file status to 'parsed'.
        \throw DatabaseException If a connection error occurs */
    void flushDocument(bool validateSource = false);

    //! Validates the current source file status
    /*! \param rollbackOnError Will exec a rollback query if an error occurs.
        \throw DatabaseException If a connection error occurs */
    void validateSource(bool rollbackOnError = false);

    int generateTagId();

    /*! \throw DatabaseException If a connection error occurs */
    int tagNameId(const QString &tagName);

    //! Returns the offset of the text
    int currentOffset() const;

    //! Adds tag data to the buffers
    void addTag(const Tag &tag);
    //! Adds attribute data to the buffers
    void addAttribute(int idTag, const QString &attributeName, QString attributeValue);
    //! Adds text to the buffer
    void addText(const QString &text);

    //! Changes the status of the current file
    /*! \throw DatabaseException If a connection error occurs */
    void cancelCurrentFile();

//private:
    GlobalParserDatabase &_globalDatabase;
    int _tagCount;
    int _docId;
    int _sourceId;
    QString _docTitle;
    QString _text;
    QString _tagsBuffer;
    QString _attributesBuffer;
};

#endif // DATAMANAGER_HPP
