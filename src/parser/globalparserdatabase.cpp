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
#include <QDebug>

#include "../parser/stringfilters.hpp"

#include "globalparserdatabase.hpp"

////////////////////////////////////////////////////////////////////////////////
GlobalParserDatabase::GlobalParserDatabase() : _mutex(new QMutex)
{
}

////////////////////////////////////////////////////////////////////////////////
void GlobalParserDatabase::refreshTagNames()
{
    if(_connection == 0)
        throw DatabaseException("GlobalParserDatabase::refreshTagNames : connection is null");

    _tagNames.clear();

    QByteArray query = "SELECT id_tag_name, tag_name FROM tag_names";
    PGresult *result = PQexec(_connection, query.constData());

    if(PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        PQclear(result);
        throw DatabaseException(QString("Could not get tag names: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    }

    // Fetch results
    bool ok;
    for(int i=0; i<PQntuples(result); ++i)
    {
        QString idStr(PQgetvalue(result, i, 0));
        int id = idStr.toInt(&ok);
        if(!ok)
        {
            PQclear(result);
            throw DatabaseException(QString("Could not convert id_tag_name to int '") + idStr + "'");
        }

        _tagNames.insert(PQgetvalue(result, i, 1), id);
    }

    PQclear(result);
}

////////////////////////////////////////////////////////////////////////////////
void GlobalParserDatabase::refreshAttributeNames()
{
    if(_connection == 0)
        throw DatabaseException("GlobalParserDatabase::refreshAttributeNames : connection is null");

    _attributeNames.clear();

    QByteArray query = "SELECT id_attribute_name, attribute_name FROM attribute_names";
    PGresult *result = PQexec(_connection, query.constData());

    if(PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        PQclear(result);
        throw DatabaseException(QString("Could not get attribute names: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    }

    // Fetch results
    bool ok;
    for(int i=0; i<PQntuples(result); ++i)
    {
        QString idStr(PQgetvalue(result, i, 0));
        int id = idStr.toInt(&ok);
        if(!ok)
        {
            PQclear(result);
            throw DatabaseException(QString("Could not convert id_attribute_name to int '") + idStr + "'");
        }

        _attributeNames.insert(PQgetvalue(result, i, 1), id);
    }

    PQclear(result);
}

////////////////////////////////////////////////////////////////////////////////
DocumentList GlobalParserDatabase::getDocuments(int count) const
{
    if(_connection == 0)
        throw DatabaseException("GlobalParserDatabase::getDocuments : connection is null");

    DocumentList list;

    QString query = "SELECT id_file, uri, doc_tag FROM files WHERE status=" + QString::number(Database::Waiting)
            + " FOR UPDATE";
    if(count > 0)
        query += " LIMIT " + QString::number(count);

    // Exec SELECT
    PGresult *result = PQexec(_connection, query.toAscii().constData());
    if(PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        PQclear(result);
        throw DatabaseException(QString("Could not get documents: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    }

    // Fetch results
    bool ok;
    int nTuples = PQntuples(result);
    for(int i=0; i<nTuples; ++i)
    {
        QString idStr(PQgetvalue(result, i, 0));
        int id = idStr.toInt(&ok);
        if(!ok)
        {
            PQclear(result);
            throw DatabaseException(QString("Could not convert id_doc to int '") + idStr + "'");
        }

        list.addDocument(ParserFactory::fileType(PQgetvalue(result, i, 1)), id, PQgetvalue(result, i, 1), PQgetvalue(result, i, 2));
    }
    PQclear(result);

    if(list.size() > 0)
    {
        // Update the document status
        query = "UPDATE files SET status=" + QString::number(Database::Processing) + ", processing_start=CURRENT_TIMESTAMP WHERE id_file IN (";
        QString sep = "";
        foreach(int id, list.sourceIds())
        {
            query += sep + QString::number(id);
            sep = ",";
        }
        query += ")";

        result = PQexec(_connection, query.toAscii().constData());
        if(PQresultStatus(result) != PGRES_COMMAND_OK)
        {
            PQclear(result);
            throw DatabaseException(QString("Could not update document status: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
        }
        PQclear(result);
    }

    return list;
}

////////////////////////////////////////////////////////////////////////////////
int GlobalParserDatabase::generateDocumentId() const
{
    QMutexLocker locker(_mutex);

    if(_connection == 0)
        throw DatabaseException("GlobalParserDatabase::generateDocumentId : connection is null");

    QByteArray query = "SELECT nextval('docs_id_doc_seq'::regclass)";
    PGresult *result = PQexec(_connection, query.constData());

    if(PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        PQclear(result);
        throw DatabaseException(QString("Could not get document id: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    }

    if(PQntuples(result) != 1)
    {
        PQclear(result);
        throw DatabaseException("No result for document id...");
    }

    // Get the result
    bool ok;
    QString idStr(PQgetvalue(result, 0, 0));
    int id = idStr.toInt(&ok);
    if(!ok)
    {
        PQclear(result);
        throw DatabaseException(QString("Could not convert generated document id to int '") + idStr + "'");
    }

    PQclear(result);
    return id;
}

////////////////////////////////////////////////////////////////////////////////
int GlobalParserDatabase::tagNameId(const QString &tagName)
{
    QString cleanName = StringFilters::cleanName(tagName);

    QMutexLocker locker(_mutex);

    int tries = 0;
    while(!_tagNames.contains(cleanName))
    {
        QString error = addTagName(cleanName);
        ++tries;
        if(tries > 2 && !error.isEmpty())
            throw DatabaseException("Can not add tag name '" + cleanName + "' : " + error);
    }

    return _tagNames.value(cleanName);
}

////////////////////////////////////////////////////////////////////////////////
int GlobalParserDatabase::attributeNameId(const QString &attributeName)
{
    QString cleanName = StringFilters::cleanName(attributeName);

    QMutexLocker locker(_mutex);

    int tries = 0;
    while(!_attributeNames.contains(cleanName))
    {
        QString error = addAttributeName(cleanName);
        ++tries;
        if(tries > 2 && !error.isEmpty())
            throw DatabaseException("Can not add attribute name '" + cleanName + "' : " + error);
    }

    return _attributeNames.value(cleanName);
}

////////////////////////////////////////////////////////////////////////////////
void GlobalParserDatabase::cancelFile(int idFile)
{
    QMutexLocker locker(_mutex);

    if(_connection == 0)
        throw DatabaseException("DataManager::cancelFile : connection is null");

    QString query = "UPDATE files SET status=" + QString::number(Database::Waiting) + " WHERE id_file=" + QString::number(idFile);
    PGresult *result = PQexec(_connection, encodeStringForDatabase(query).constData());
    if(PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        PQclear(result);
        throw DatabaseException(QString("Could not update (cancel) the file status: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    }
    PQclear(result);
}

////////////////////////////////////////////////////////////////////////////////
QString GlobalParserDatabase::addTagName(const QString &tagName)
{
    if(_connection == 0)
        throw DatabaseException("GlobalParserDatabase::addTagName : connection is null");

    QByteArray query = "INSERT INTO tag_names(tag_name) VALUES('" + encodeStringForDatabase(tagName) + "')";
    PGresult *result = PQexec(_connection, query.constData());

    QString error = "";
    if(PQresultStatus(result) != PGRES_COMMAND_OK)
        error = encodeStringFromDatabase(PQerrorMessage(_connection));
    PQclear(result);

    refreshTagNames();

    return error;
}

////////////////////////////////////////////////////////////////////////////////
QString GlobalParserDatabase::addAttributeName(const QString &attributeName)
{
    if(_connection == 0)
        throw DatabaseException("GlobalParserDatabase::addAttributeName : connection is null");

    QByteArray query = "INSERT INTO attribute_names(attribute_name) VALUES('" + encodeStringForDatabase(attributeName) + "')";
    PGresult *result = PQexec(_connection, query.constData());

    QString error = "";
    if(PQresultStatus(result) != PGRES_COMMAND_OK)
        error = encodeStringFromDatabase(PQerrorMessage(_connection));
    PQclear(result);

    refreshAttributeNames();

    return error;
}
