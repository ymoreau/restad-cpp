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
#include <QtCore/QThreadPool>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <iostream>
using namespace std;

#include "indexer.hpp"

////////////////////////////////////////////////////////////////////////////////
Indexer::Indexer(QCoreApplication &application) :
    QObject(&application), _app(application)
{
    _doRun = true;
    _optionManager.setCommandFormat("indexer [options]");
    Database::configureOptions(_optionManager);

    _optionManager.addOption('c', "Max document count to process, default is 1000", Option::Int, "maxdocs-to-process");
    _optionManager.addOption('p', "Max threads to use, default is all available cores except one", Option::Int, "maxthreads");
    _optionManager.addOption('l', "Loop until there is no more document to process", Option::Flag, "loop-indexing");
    _optionManager.addOption('o', "Append the parsing error log to a file", Option::String, "log-output-file");

    _optionManager.parse(_app.arguments());
    if(_optionManager.isSet('h'))
    {
        _optionManager.displayHelp();
        _doRun = false;
        return;
    }
    if(_optionManager.isSet('v'))
    {
        cout << _app.applicationVersion().toLocal8Bit().constData() << endl;
        _doRun = false;
        return;
    }

    _database.configureDatabase(_optionManager);
    _threadPool = QThreadPool::globalInstance();

    _timer.setInterval(500);
    connect(&_timer, SIGNAL(timeout()), SLOT(update()));
    _firstLoop = true;
}

////////////////////////////////////////////////////////////////////////////////
void Indexer::run()
{
    if(!_doRun)
    {
        _app.quit();
        return;
    }

    int maxDocumentCount = 1000;
    int tmp = _optionManager.optionValue('c').toInt();
    if(tmp != 0)
        maxDocumentCount = tmp;

    try
    {
        _documents = _database.getDocuments(maxDocumentCount);
        if(_documents.size() < 1)
        {
            if(_firstLoop && !_optionManager.isSet('q'))
                cout << "No document to process" << endl;

            QTimer::singleShot(0, this, SLOT(end()));
            return;
        }

        _indexingProgress.initialize(_documents.size());

        int maxThreads = _threadPool->maxThreadCount();
        tmp = _optionManager.optionValue('t').toInt();
        if(tmp > 0 && tmp < maxThreads)
            maxThreads = tmp;
        if(_documents.size() < maxThreads)
            maxThreads = _documents.size();

        for(int i=0; i<maxThreads; ++i)
        {
            AbstractParser *parser = ParserFactory::newParser(_documents.mostWaitingType(), _indexingProgress, _documents, _optionManager, _database);
            _threadPool->start(parser);
        }

        _timer.start();
    }
    catch(const Exception &e)
    {
        _timer.stop();
        if(!_optionManager.isSet('q'))
            cerr << e.message().toLocal8Bit().constData() << endl;
        QTimer::singleShot(0, this, SLOT(end()));
    }
}

////////////////////////////////////////////////////////////////////////////////
void Indexer::update()
{
    _firstLoop = false;

    if(!_optionManager.isSet('q'))
        cout << "\r" << _indexingProgress.status().toLocal8Bit().constData() << flush;

    if(_threadPool->activeThreadCount() < 1)
    {
        _timer.stop();
        if(!_optionManager.isSet('q'))
            cout << endl;

        if(_documents.size() > 0 && !_optionManager.isSet('q'))
            cerr << endl << "Threads stopped before processing all documents, " << _documents.size() << " left." << endl;

        if(_optionManager.isSet('l'))
            QTimer::singleShot(0, this, SLOT(run()));
        else
            QTimer::singleShot(0, this, SLOT(end()));
    }
}

////////////////////////////////////////////////////////////////////////////////
void Indexer::end() const
{
    if(!_indexingProgress.errorLog().isEmpty())
    {
        QString outputFile = _optionManager.optionValue('o').toString();
        if(!outputFile.isEmpty())
        {
            QFile file(outputFile);
            if(!file.open(QIODevice::Append))
            {
                if(!_optionManager.isSet('q'))
                    cerr << "Can not open the log file '" << outputFile.toAscii().constData() << "'" << endl;
            }
            else
            {
                QTextStream out(&file);
                out << _indexingProgress.errorLog() << "\n";
                file.close();
            }
        }
        else if(/*outputFile.isEmpty() IMPLICIT*/ !_optionManager.isSet('q'))
        {
            cerr << _indexingProgress.errorLog().toLocal8Bit().constData() << endl;
        }
    }

    _app.quit();
}

