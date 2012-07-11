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

#include <QtCore/QStringList>

#include "preparserdatabase.hpp"

////////////////////////////////////////////////////////////////////////////////
PreparserDatabase::PreparserDatabase()
{
}

////////////////////////////////////////////////////////////////////////////////
void PreparserDatabase::addDocuments(const QStringList &documents, QString docTag)
{
    QString data;

    QString docTagValue;
    if(docTag.isEmpty())
        docTagValue = ",\n";
    else
        docTagValue = "," + Database::escapeCopyString(docTag) + "\n";

    foreach(QString uri, documents)
        data += Database::escapeCopyString(uri) + docTagValue;

    copy("files(uri, doc_tag)", data);
}
