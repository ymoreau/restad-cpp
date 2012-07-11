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

#include "tagfilter.hpp"

////////////////////////////////////////////////////////////////////////////////
TagFilter::TagFilter(FilterMode mode) : _mutex(new QMutex())
{
    _mode = mode;
}

////////////////////////////////////////////////////////////////////////////////
void TagFilter::setAcceptedTags(const QStringList &tags)
{
    _acceptedTags = tags;
}

////////////////////////////////////////////////////////////////////////////////
void TagFilter::setIgnoredTags(const QStringList &tags)
{
    _ignoredTags = tags;
}

////////////////////////////////////////////////////////////////////////////////
QStringList TagFilter::acceptedTags() const
{
    return _acceptedTags;
}

////////////////////////////////////////////////////////////////////////////////
QStringList TagFilter::ignoredTags() const
{
    return _ignoredTags;
}

bool TagFilter::acceptTag(const QString &tag) const
{
    QMutexLocker locker(_mutex);

    if(_mode == Accept)
    {
        if(_ignoredTags.contains(tag, Qt::CaseInsensitive))
            return false;
        return true;
    }

    if(_acceptedTags.contains(tag, Qt::CaseInsensitive))
        return true;

    return false;
}
