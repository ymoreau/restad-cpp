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

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

#include "documentlist.hpp"

////////////////////////////////////////////////////////////////////////////////
DocumentInfo::DocumentInfo()
{
    _fileType = ParserFactory::Invalid;
    _sourceId = -1;
}

////////////////////////////////////////////////////////////////////////////////
DocumentInfo::DocumentInfo(ParserFactory::FileType fileType, int sourceId, const QString &uri, const QString &docSeparator)
{
    _fileType = fileType;
    _sourceId = sourceId;
    _uri = uri;
    _docSeparator = docSeparator;
}

////////////////////////////////////////////////////////////////////////////////
bool DocumentInfo::isValid() const
{
    return _fileType != ParserFactory::Invalid;
}

////////////////////////////////////////////////////////////////////////////////
ParserFactory::FileType DocumentInfo::fileType() const
{
    return _fileType;
}

////////////////////////////////////////////////////////////////////////////////
int DocumentInfo::sourceId() const
{
    return _sourceId;
}

////////////////////////////////////////////////////////////////////////////////
QString DocumentInfo::uri() const
{
    return _uri;
}

////////////////////////////////////////////////////////////////////////////////
QString DocumentInfo::docSeparator() const
{
    return _docSeparator;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DocumentList::DocumentList() : _mutex(new QMutex)
{
    ParserFactory::initializeTypeCountMap(_typeCountMap);
}

////////////////////////////////////////////////////////////////////////////////
void DocumentList::addDocument(ParserFactory::FileType fileType, int sourceId, const QString &uri, const QString &docSeparator)
{
    if(fileType == ParserFactory::Invalid)
        return;

    _list.append(DocumentInfo(fileType, sourceId, uri, docSeparator));
    ++(_typeCountMap[fileType]);
}

////////////////////////////////////////////////////////////////////////////////
DocumentInfo DocumentList::takeDocument(ParserFactory::FileType fileType)
{
    DocumentInfo doc;

    if(fileType == ParserFactory::Invalid)
        return doc;

    QMutexLocker locker(_mutex);

    if(_list.size() < 1)
        return doc;

    QList<DocumentInfo>::iterator it;
    for(it = _list.begin(); it != _list.end(); ++it)
    {
        if((*it).fileType() == fileType)
        {
            doc = *it;
            --(_typeCountMap[fileType]);
            _list.erase(it);
            return doc;
        }
    }

    return doc;
}

////////////////////////////////////////////////////////////////////////////////
QList<int> DocumentList::sourceIds() const
{
    QList<int> ids;
    ids.reserve(_list.size());
    foreach(const DocumentInfo &info, _list)
        ids.append(info.sourceId());
    return ids;
}

////////////////////////////////////////////////////////////////////////////////
int DocumentList::size() const
{
    return _list.size();
}

////////////////////////////////////////////////////////////////////////////////
ParserFactory::FileType DocumentList::mostWaitingType() const
{
    int maxCount = 0;
    ParserFactory::FileType mostWaiting = ParserFactory::Invalid;

    QMutexLocker locker(_mutex);

    foreach(ParserFactory::FileType type, _typeCountMap.keys())
    {
        if(_typeCountMap.value(type, -1) > maxCount)
        {
            maxCount = _typeCountMap.value(type);
            mostWaiting = type;
        }
    }

    return mostWaiting;
}
