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

#ifndef INDEXINGPROGRESS_HPP
#define INDEXINGPROGRESS_HPP

#include <QtCore/QString>
class QMutex;

//! Current status of the indexing process
/*! \ingroup parser */
class IndexingProgress
{
public:
    IndexingProgress();
    ~IndexingProgress();

    //! Initializes the status with the total document count
    void initialize(int expectedDocumentCount);

    //! Increments the processed document count
    /*! Thread-safe. */
    void incrementDoneDocument();

    //! Increments the failed document count and save the error message in the log
    /*! Thread-safe. */
    void incrementFailedDocument(const QString &error);

    //! Returns a string for the current status.
    /*! Thread-safe.<br/>String format "d/t done (XX%) [f has failed]" with <br/>
        d : documents processed (including failed documents)<br/>
        t : total expected documents <br/>
        f : count of failed documents
        */
    QString status() const;

    QString errorLog() const;

private:
    QMutex *_mutex;
    int _expectedDocumentCount;
    int _currentDocumentCount;
    int _failedDocuments;
    QString _errorLog;
};

#endif // INDEXINGPROGRESS_HPP
