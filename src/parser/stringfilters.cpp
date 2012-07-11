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

#include <QtCore/QRegExp>

#include "stringfilters.hpp"


const int StringFilters::MaxTagNameLength = 256;
const int StringFilters::MaxAttributeNameLength = 256;
const int StringFilters::MaxDocTitleLength = 512;
const int StringFilters::MaxDocumentTagLength = 256;

////////////////////////////////////////////////////////////////////////////////
QString StringFilters::cleanName(QString name, int maxLength)
{
    QString result = name.toLower();
    result.replace(QRegExp("\\s+"), "_");
    if(maxLength > 0)
        result.truncate(maxLength);
    return result;
}
