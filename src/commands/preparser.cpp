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
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDebug>

#include <iostream>
using namespace std;

#include "preparser.hpp"

////////////////////////////////////////////////////////////////////////////////
Preparser::Preparser(QCoreApplication &application) :
    QObject(&application), _app(application)
{
    _optionManager.setCommandFormat("preparser [options] path-to-explore");
    Database::configureOptions(_optionManager);

    _optionManager.addOption('e', "Filter by extensions. Example \"*.xml *.html\"", Option::String, "preparsing-extensions");
    _optionManager.addOption('m', "Document tag for multiple document files (the name of the tag enclosing one document)",
                             Option::String, "document-separator");
    _optionManager.addOption('r', "Recursive listing of the files", Option::Flag, "preparsing-recursive");

    QStringList args;
    _optionManager.parse(_app.arguments(), &args, 1);
    if(_optionManager.isSet('h'))
    {
        _optionManager.displayHelp();
        return;
    }
    if(_optionManager.isSet('v'))
    {
        cout << _app.applicationVersion().toStdString();
        return;
    }
    _dirToExplore = args.at(0);

    if(!QFile::exists(_dirToExplore))
        throw Exception(_dirToExplore + " not found");

    _database.configureDatabase(_optionManager);
}

////////////////////////////////////////////////////////////////////////////////
void Preparser::run()
{
    QStringList files;
    QDir dir(_dirToExplore);

    QString extensionFilter = _optionManager.optionValue('e').toString();
    if(extensionFilter.isEmpty())
        extensionFilter = "*";
    QStringList filters(extensionFilter);

    QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);

    if(_optionManager.isSet('r')) // Recursing
    {
        QStringList subDirs = Preparser::absolutePath(dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));
        while(!subDirs.isEmpty())
        {
            QDir subDir(subDirs.takeFirst());
            subDirs << Preparser::absolutePath(subDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot));
            fileInfoList << subDir.entryInfoList(filters, QDir::Files | QDir::NoSymLinks);
        }
    }

    files << Preparser::absolutePath(fileInfoList);

    try
    {
        _database.addDocuments(files, _optionManager.optionValue('m').toString());
    }
    catch(const DatabaseException &e)
    {
        cerr << e.message().toStdString() << endl;
    }

    _app.quit();
}

////////////////////////////////////////////////////////////////////////////////
QStringList Preparser::absolutePath(const QFileInfoList &list)
{
    QStringList result;
    foreach(const QFileInfo &fileInfo, list)
        result << fileInfo.absoluteFilePath();
    return result;
}
