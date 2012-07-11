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

#ifndef TAGFILTER_HPP
#define TAGFILTER_HPP

#include <QtCore/QStringList>
class QMutex;

//! Filter for tags
class TagFilter
{
public:
    //! Defines how filter handles the unspecifed tags
    enum FilterMode
    {
        Accept, //!< Filter will accept unspecified tags by default
        Ignore //!< Filter will ignore unspecified tags by default
    };

    TagFilter(FilterMode mode = Accept);

    void setAcceptedTags(const QStringList &tags);
    void setIgnoredTags(const QStringList &tags);
    QStringList acceptedTags() const;
    QStringList ignoredTags() const;

    //! Returns true if tag is accepted (or not ignored)
    /*! Thread-safe. */
    bool acceptTag(const QString &tag) const;

private:
    QMutex *_mutex;
    FilterMode _mode;
    QStringList _acceptedTags;
    QStringList _ignoredTags;
};

#endif // TAGFILTER_HPP
