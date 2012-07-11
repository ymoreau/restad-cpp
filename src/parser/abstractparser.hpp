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

#ifndef ABSTRACTPARSER_HPP
#define ABSTRACTPARSER_HPP

#include <QtCore/QRunnable>

#include "../parser/datamanager.hpp"
class IndexingProgress;
class DocumentList;
class OptionManager;
class GlobalParserDatabase;
class TagFilter;
class QIODevice;

//! Class holding data for a tag
class Tag
{
public:
    Tag();
    Tag(int idTag, const QString &name, int idTagName, int parentId, int orderPosition, int startingOffset);

    int idTag;
    QString name;
    int idTagName;
    int parentId;
    int orderPosition;
    int startingOffset;
    int endingOffset;
};

//! Abstract class for parsers
/*! \ingroup parser
  Everything inside this class should call local data member functions or thread-safe functions only !
  */
class AbstractParser : public QRunnable
{
public:
    //! Initializes the parser
    /*! \throw DatabaseException if initialization of database fails */
    AbstractParser(IndexingProgress &indexingProgress, DocumentList &documentList, const OptionManager &optionManager,
                   GlobalParserDatabase &globalDatabase, TagFilter *tagFilter = 0);
    virtual ~AbstractParser();

    virtual void run() = 0;

    //! Returns the file interface, caller must delete the returned object.
    static QIODevice *readFile(const QString &path);

protected:
    IndexingProgress &_indexingProgress;
    DocumentList &_documentList;
    DataManager _dataManager;
    TagFilter *_tagFilter;
};

#endif // ABSTRACTPARSER_HPP
