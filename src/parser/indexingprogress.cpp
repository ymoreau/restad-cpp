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

#include "indexingprogress.hpp"

////////////////////////////////////////////////////////////////////////////////
IndexingProgress::IndexingProgress() : _mutex(new QMutex)
{
}

////////////////////////////////////////////////////////////////////////////////
IndexingProgress::~IndexingProgress()
{
    delete _mutex;
}

////////////////////////////////////////////////////////////////////////////////
void IndexingProgress::initialize(int expectedDocumentCount)
{
    if(expectedDocumentCount < 1)
        return;

    _expectedDocumentCount = expectedDocumentCount;
    _currentDocumentCount = 0;
    _failedDocuments = 0;
}

////////////////////////////////////////////////////////////////////////////////
void IndexingProgress::incrementDoneDocument()
{
    QMutexLocker locker(_mutex);

    ++_currentDocumentCount;
}

////////////////////////////////////////////////////////////////////////////////
void IndexingProgress::incrementFailedDocument(const QString &error)
{
    QMutexLocker locker(_mutex);

    ++_currentDocumentCount;
    ++_failedDocuments;
    _errorLog += error + "\n";
}

////////////////////////////////////////////////////////////////////////////////
QString IndexingProgress::status() const
{
    QString status;
    QMutexLocker locker(_mutex);

    if(_expectedDocumentCount < 1)
        return QString();

    status = QString::number(_currentDocumentCount) + "/" + QString::number(_expectedDocumentCount)
            + " done (" + QString::number((int) ((double)_currentDocumentCount / _expectedDocumentCount * 100)) + "%)";
    if(_failedDocuments > 0)
        status += " " + QString::number(_failedDocuments) + " has failed";
    return status;
}

////////////////////////////////////////////////////////////////////////////////
QString IndexingProgress::errorLog() const
{
    return _errorLog;
}
