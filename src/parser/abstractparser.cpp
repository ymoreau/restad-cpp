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

#include <QtCore/QThread>
#include <QtCore/QFile>

#include "../parser/indexingprogress.hpp"
#include "../parser/documentlist.hpp"
#include "../parser/globalparserdatabase.hpp"
#include "../parser/tagfilter.hpp"

#include "abstractparser.hpp"

////////////////////////////////////////////////////////////////////////////////
Tag::Tag()
{
    idTag = -1;
    idTagName = -1;
    parentId = -1;
    orderPosition = -1;
    startingOffset = -1;
    endingOffset = -1;
}

////////////////////////////////////////////////////////////////////////////////
Tag::Tag(int it, const QString &n, int itn, int pi, int op, int so)
{
    idTag = it;
    name = n;
    idTagName = itn;
    parentId = pi;
    orderPosition = op;
    startingOffset = so;
    endingOffset = -1;
}

////////////////////////////////////////////////////////////////////////////////
AbstractParser::AbstractParser(IndexingProgress &indexingProgress, DocumentList &documentList, const OptionManager &optionManager,
                               GlobalParserDatabase &globalDatabase, TagFilter *tagFilter) :
    _indexingProgress(indexingProgress), _documentList(documentList), _dataManager(globalDatabase), _tagFilter(tagFilter)
{
    _dataManager.configureDatabase(optionManager);
}

////////////////////////////////////////////////////////////////////////////////
AbstractParser::~AbstractParser()
{
}

////////////////////////////////////////////////////////////////////////////////
QIODevice *AbstractParser::readFile(const QString &path)
{
    //! \todo manage distant files
    return new QFile(path);
}
