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

#include <QtCore/QRegExp>
#include <QtCore/QTextStream>
#include "../global/optionmanager.hpp"
#include <QDebug>

#include <iostream>

#include "database.hpp"

////////////////////////////////////////////////////////////////////////////////
void Database::configureOptions(OptionManager &optionManager)
{
    optionManager.addOption('t', "Database host name", Option::String, "dbhost");
    optionManager.addOption('d', "Database name", Option::String, "dbname");
    optionManager.addOption('u', "Database user name", Option::String, "dbuser");
    optionManager.addOption('w', "Database password", Option::String, "dbpassword");
    optionManager.addOption('x', "Database encoding", Option::String, "dbencoding");
}

////////////////////////////////////////////////////////////////////////////////
Database::Database() : _connection(0), _encoding(Database::None)
{
}

////////////////////////////////////////////////////////////////////////////////
Database::~Database()
{
    PQfinish(_connection);
}

////////////////////////////////////////////////////////////////////////////////
Database::Database(const QString &host, const QString &dbName, const QString &userName,
                   const QString &password) : _connection(0)
{
    set(host, dbName, userName, password);
}

////////////////////////////////////////////////////////////////////////////////
void Database::set(const QString &host, const QString &dbName, const QString &userName, const QString &password)
{
    QString parameters;

    if(!host.isEmpty())
    {
        QRegExp regex("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
        if(regex.exactMatch(host))
            parameters += "hostaddr=" + host + " ";
        else
            parameters += "host=" + host + " ";
    }

    if(!dbName.isEmpty())
        parameters += "dbname=" + dbName + " ";
    if(!userName.isEmpty())
        parameters += "user=" + userName + " ";
    if(!password.isEmpty())
        parameters += "password=" + password + " ";

    _connection = PQconnectdb(parameters.toAscii().constData());
    if(PQstatus(_connection) != CONNECTION_OK)
        throw DatabaseException(QString("Could not connect to the database '") + dbName + "' : " + PQerrorMessage(_connection));
}

////////////////////////////////////////////////////////////////////////////////
void Database::configureDatabase(OptionManager &optionManager)
{
    QString password = optionManager.optionValue('w').toString();
    if(password.isEmpty() && !optionManager.isSet('q')) // If no password and no silent-mode
    {
        std::cout << "Database password: " << std::flush;
        QTextStream in(stdin);
        in >> password;
        optionManager.setValue('w', password);
    }

    set(optionManager.optionValue('t').toString(),
                 optionManager.optionValue('d').toString(),
                 optionManager.optionValue('u').toString(),
                 optionManager.optionValue('w').toString());
    QString encoding = optionManager.optionValue('x').toString().toLower();
    if(encoding == "utf8" || encoding == "utf-8")
        _encoding = Database::Utf8;
}

////////////////////////////////////////////////////////////////////////////////
QString &Database::escapeCopyString(QString &string)
{
    string.replace("\n", " ");
    string.replace("\"", "\"\"");
    string.insert(0, '"');
    string.append('"');
    return string;
}

////////////////////////////////////////////////////////////////////////////////
QByteArray Database::escapeString(const QString &string)
{
    if(_connection == 0)
        throw DatabaseException("Database::escapeString : connection is null");

    QByteArray source = encodeStringForDatabase(string);
    QByteArray dest;
    // Recommanded by libpq doc : 2*source_size+1
    dest.resize(2 * source.size() + 1);

    int destSize, error;
    destSize = PQescapeStringConn(_connection, dest.data(), source.constData(), source.size(), &error);
    if(error != 0)
        throw DatabaseException(QString("Could not escape string: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));

    dest.resize(destSize);
    dest.insert(0, '\'');
    dest.append('\'');

    return dest;
}

////////////////////////////////////////////////////////////////////////////////
QString Database::encodeStringFromDatabase(const char *string) const
{
    if(_encoding == Database::Utf8)
        return QString::fromUtf8(string);

    return QString(string);
}

QByteArray Database::encodeStringForDatabase(const QString &string) const
{
    if(_encoding == Database::Utf8)
        return string.toUtf8();

    return string.toLocal8Bit();
}

////////////////////////////////////////////////////////////////////////////////
void Database::copy(const QString &tableTarget, const QString &data)
{
    if(_connection == 0)
        throw DatabaseException("Database::copy : connection is null");

    PGresult *result;
    QString query("COPY ");
    query += tableTarget + " FROM STDIN csv";

    result = PQexec(_connection, query.toAscii().constData());
    if(PQresultStatus(result) != PGRES_COPY_IN)
    {
        PQclear(result);
        throw DatabaseException(QString("COPY error: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    }
    PQclear(result);

    QByteArray rawData = encodeStringForDatabase(data);
    if(PQputCopyData(_connection, rawData.constData(), rawData.size()) <= 0)
        throw DatabaseException(QString("COPY sending error: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    if(PQputCopyEnd(_connection, NULL) <= 0)
        throw DatabaseException(QString("COPY ending error: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));

    result = PQgetResult(_connection);
    if(PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        PQclear(result);
        throw DatabaseException(QString("COPY error: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    }
    PQclear(result);
}
