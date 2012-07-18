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

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QThreadPool>

#include <iostream>
using namespace std;

#include "indexer.hpp"

////////////////////////////////////////////////////////////////////////////////
Indexer::Indexer(QCoreApplication &application) :
    QObject(&application), _app(application)
{
    _optionManager.setCommandFormat("indexer [options]");
    Database::configureOptions(_optionManager);

    _optionManager.addOption('c', "Max document count to process, default is 1000", Option::Int, "maxdocs-to-process");
    _optionManager.addOption('p', "Max threads to use, default is all available cores except one", Option::Int, "maxthreads");

    _optionManager.parse(_app.arguments());
    if(_optionManager.isSet('h'))
    {
        _optionManager.displayHelp();
        exit(0);
    }
    if(_optionManager.isSet('v'))
    {
        cout << _app.applicationVersion().toLocal8Bit().constData() << endl;
        exit(0);
    }

    _database.configureDatabase(_optionManager);
}

////////////////////////////////////////////////////////////////////////////////
void Indexer::run()
{
    int maxDocumentCount = 1000;
    int tmp = _optionManager.optionValue('c').toInt();
    if(tmp != 0)
        maxDocumentCount = tmp;

    try
    {
        _documents = _database.getDocuments(maxDocumentCount);
        _indexingProgress.initialize(_documents.size());

        if(!_optionManager.isSet('q'))
        {
            _timer.setInterval(500);
            connect(&_timer, SIGNAL(timeout()), this, SLOT(refreshProgress()));
            _timer.start();
        }

        QThreadPool *threadPool = QThreadPool::globalInstance();
        QList<AbstractParser*> parsers;

        int maxThreads = threadPool->maxThreadCount();
        tmp = _optionManager.optionValue('t').toInt();
        if(tmp > 0 && tmp < maxThreads)
            maxThreads = tmp;
        if(_documents.size() < maxThreads)
            maxThreads = _documents.size();

        for(int i=0; i<maxThreads; ++i)
        {
            AbstractParser *parser = ParserFactory::newParser(_documents.mostWaitingType(), _indexingProgress, _documents, _optionManager, _database);
            parsers.append(parser);
            threadPool->start(parser);
        }
        threadPool->waitForDone();
        _timer.stop();

        if(!_indexingProgress.errorLog().isEmpty() && !_optionManager.isSet('q'))
            cerr << _indexingProgress.errorLog().toLocal8Bit().constData() << endl;
    }
    catch(const Exception &e)
    {
        if(!_optionManager.isSet('q'))
            cerr << e.message().toLocal8Bit().constData() << endl;
    }


    _app.quit();
}

////////////////////////////////////////////////////////////////////////////////
void Indexer::refreshProgress() const
{
    cout << "\r" << _indexingProgress.status().toLocal8Bit().constData() << flush;
}

