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

#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "../global/exception.hpp"

#include <QtCore/QString>
#include <libpq-fe.h>

class OptionManager;

//! Manages the database connection and queries
/*! \ingroup global
    This class uses the C libpq library as PosgreSQL API.
  */
class Database
{
public:
    //! The database encoding
    enum Encoding
    {
        None, //!< Unspecified, will use locale if detected
        Utf8
    };

    //! The status of the document in the database
    enum DocumentStatus
    {
        Waiting     = 0, //!< Document hasn't been indexed
        Parsed      = 1, //!< Document has been indexed
        Processing  = 2, //!< Document is currently being indexed
        Error       = 3  //!< Document can not be indexed because of bad data
    };

    //! Configures the option manager for database options (t, d, u, w, x)
    /*! \throw Exception If options can not be added */
    static void configureOptions(OptionManager &optionManager);

    Database();
    ~Database();

    //! Initializes the database connection
    /*! \throw DatabaseException If connection fails */
    Database(const QString &host, const QString &dbName, const QString &userName, const QString &password);

    //! Initializes the database connection
    /*! \throw DatabaseException If connection fails */
    void set(const QString &host, const QString &dbName, const QString &userName, const QString &password);

    //! Configures the database
    /*! \throw DatabaseException From Database::set() */
    void configureDatabase(OptionManager &optionManager);

protected:
    //! Escapes the copy delimiter and newline characters, then quotes the whole string
    static QString & escapeCopyString(QString &string);

    //! Escapes the string delimiter, then quotes the whole string
    QByteArray escapeString(const QString &string);

    //! Encodes a string sent by the database, depending on the database-encoding option
    QString encodeStringFromDatabase(const char *string) const;

    //! Encodes a string for SQL queries, depending on the database-encoding option
    QByteArray encodeStringForDatabase(const QString &string) const;

    //! Performs a COPY query
    /*! \throw DatabaseException */
    //! \todo send by blocks
    void copy(const QString &tableTarget, const QString &data);

    PGconn *_connection;
    Encoding _encoding;
};

#endif // DATABASE_HPP
