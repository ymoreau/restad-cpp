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

#ifndef STRINGFILTERS_HPP
#define STRINGFILTERS_HPP

#include <QtCore/QString>

//! Static class for filtering/formating strings
/*! \ingroup parser */
class StringFilters
{
public:
    static const int MaxTagNameLength;
    static const int MaxAttributeNameLength;
    static const int MaxDocTitleLength;
    static const int MaxDocumentTagLength;

    //! Cleans the string for a well formated name
    /*! \param maxLength Will truncate the string if different than -1 */
    static QString cleanName(QString name, int maxLength = -1);

private:
    StringFilters();
};

#endif // STRINGFILTERS_HPP
