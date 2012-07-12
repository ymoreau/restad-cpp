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

#ifndef PARSERFACTORY_HPP
#define PARSERFACTORY_HPP

#include <QtCore/QHash>

#include "../parser/abstractparser.hpp"

//! Factory for the parsers
class ParserFactory
{
public:
    //! File types to parse
    // Do not forget to update initializeTypeCountMap() if changing this enum
    enum FileType
    {
        Invalid = 0,
        XML     = 1,
        ODT     = 2
    };

    //! Map for counting each file type total files
    typedef QHash<FileType, int> TypeCountMap;

    //! Initializes the map to zero for each file type
    static void initializeTypeCountMap(TypeCountMap &map);

    //! Returns the parser for the given file type
    /*! \throw Exception if file type is unknown or has no parser */
    static AbstractParser *newParser(FileType fileType, IndexingProgress &indexingProgress, DocumentList &documentList,
                                      OptionManager &optionManager, GlobalParserDatabase &globalDatabase);

    //! Returns the file type depending on the file extension
    static FileType fileType(const QString &fileName);

private:
    ParserFactory();
};

#endif // PARSERFACTORY_HPP
