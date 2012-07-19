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

#ifndef INDEXER_HPP
#define INDEXER_HPP

#include <QtCore/QObject>
#include <QtCore/QTimer>
class QCoreApplication;
class QThreadPool;

#include "../global/exception.hpp"
#include "../global/optionmanager.hpp"
#include "../parser/globalparserdatabase.hpp"
#include "../parser/indexingprogress.hpp"

//! Manages the indexing command
/*! \ingroup commands */
class Indexer : public QObject
{
    Q_OBJECT
public:
    /*! \throw Exception from OptionManager
        \throw DatabaseException from Database */
    explicit Indexer(QCoreApplication &application);

public slots:
    void run();
private slots:
    void update();
    void end() const;

private:
    QCoreApplication &_app;
    bool _doRun;
    QThreadPool *_threadPool;
    QTimer _timer;
    bool _firstLoop;
    OptionManager _optionManager;
    GlobalParserDatabase _database;
    DocumentList _documents;
    IndexingProgress _indexingProgress;
};

#endif // INDEXER_HPP
