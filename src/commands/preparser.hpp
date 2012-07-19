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

#ifndef PREPARSER_HPP
#define PREPARSER_HPP

#include <QtCore/QObject>
#include <QtCore/QFileInfoList>
class QCoreApplication;

#include "../global/exception.hpp"
#include "../global/optionmanager.hpp"
#include "../preparser/preparserdatabase.hpp"

//! Manages the preparsing command
/*! \ingroup commands */
class Preparser : public QObject
{
    Q_OBJECT
public:
    /*! \throw Exception from OptionManager
        \throw DatabaseException from Database */
    explicit Preparser(QCoreApplication &application);

public slots:
    void run();

private:
    static QStringList absolutePath(const QFileInfoList &list);

    QCoreApplication &_app;
    bool _doRun;
    OptionManager _optionManager;
    PreparserDatabase _database;
    QString _dirToExplore;
};

#endif // PREPARSER_HPP
