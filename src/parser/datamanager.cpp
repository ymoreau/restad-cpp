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

#include "../parser/globalparserdatabase.hpp"
#include "../parser/stringfilters.hpp"

#include "datamanager.hpp"

////////////////////////////////////////////////////////////////////////////////
DataManager::DataManager(GlobalParserDatabase &globalDatabase) :
    _globalDatabase(globalDatabase)
{
    _sourceId = 0;
    _docId = 0;
}

////////////////////////////////////////////////////////////////////////////////
void DataManager::startDocument(int sourceId)
{
    _sourceId = sourceId;
    _docId = _globalDatabase.generateDocumentId();
    _tagCount = 0;
    _docTitle.clear();
    _text.clear();
    _tagsBuffer.clear();
    _attributesBuffer.clear();
}

////////////////////////////////////////////////////////////////////////////////
void DataManager::flushDocument(bool doValidateSource)
{
    if(_docId < 0)
        return;

    if(_connection == 0)
        throw DatabaseException("DataManager::flushDocument : connection is null");

    // Start the transaction
    PGresult *result = PQexec(_connection, "BEGIN");
    if(PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        PQclear(result);
        throw DatabaseException(QString("Could not start transaction: ") + encodeStringFromDatabase(PQerrorMessage(_connection)));
    }
    PQclear(result);

    // Send the doc data
    QByteArray query = "INSERT INTO docs(id_doc,id_file,title,text) VALUES(" + QString::number(_docId).toAscii() + ","
            + QString::number(_sourceId).toAscii() + "," + Database::escapeString(_docTitle)
            + ",E" + Database::escapeString(_text) + ")"; // E'text' is for postgresql strings with backslashed chars like newline

    result = PQexec(_connection, query.constData());
    if(PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        PQclear(result);
        QString error = encodeStringFromDatabase(PQerrorMessage(_connection));
        PQexec(_connection, "ROLLBACK");
        throw DatabaseException(QString("Could not add the document data: ") + error);
    }
    PQclear(result);

    copy("tags(id_doc,id_tag,id_tag_name,tag_order_position,parent_id,starting_offset,ending_offset)", _tagsBuffer);
    copy("tag_attributes(id_doc,id_tag,id_attribute_name,attribute_value)", _attributesBuffer);

    if(doValidateSource)
        validateSource(true);

    PQexec(_connection, "COMMIT");
}

////////////////////////////////////////////////////////////////////////////////
void DataManager::validateSource(bool rollbackOnError)
{
    if(_sourceId < 0)
        return;

    if(_connection == 0)
        throw DatabaseException("DataManager::validateSource : connection is null");

    // Set the document status
    QString query = "UPDATE files SET status=" + QString::number(Database::Parsed) + " WHERE id_file=" + QString::number(_sourceId);
    PGresult *result = PQexec(_connection, encodeStringForDatabase(query).constData());
    if(PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        PQclear(result);
        QString error = encodeStringFromDatabase(PQerrorMessage(_connection));
        if(rollbackOnError)
            PQexec(_connection, "ROLLBACK");
        throw DatabaseException(QString("Could not update the file status: ") + error);
    }
    PQclear(result);
}

////////////////////////////////////////////////////////////////////////////////
int DataManager::generateTagId()
{
    return ++_tagCount;
}

////////////////////////////////////////////////////////////////////////////////
int DataManager::tagNameId(const QString &tagName)
{
    return _globalDatabase.tagNameId(tagName);
}

////////////////////////////////////////////////////////////////////////////////
int DataManager::currentOffset() const
{
    return _text.size();
}

////////////////////////////////////////////////////////////////////////////////
void DataManager::addTag(const Tag &tag)
{
    _tagsBuffer += QString::number(_docId) + "," + QString::number(tag.idTag) + "," + QString::number(tag.idTagName) + ","
            + QString::number(tag.orderPosition) + ",";
    if(tag.parentId < 0)
        _tagsBuffer += "";
    else
        _tagsBuffer += QString::number(tag.parentId);
    _tagsBuffer += "," + QString::number(tag.startingOffset) + "," + QString::number(tag.endingOffset) + "\n";
}

////////////////////////////////////////////////////////////////////////////////
void DataManager::addAttribute(int idTag, const QString &attributeName, QString attributeValue)
{
    QString attName = StringFilters::cleanName(attributeName, StringFilters::MaxAttributeNameLength);
    _attributesBuffer += QString::number(_docId) + "," + QString::number(idTag) + ","
            + QString::number(_globalDatabase.attributeNameId(attName)) + "," + Database::escapeCopyString(attributeValue) + "\n";
}

////////////////////////////////////////////////////////////////////////////////
void DataManager::addText(const QString &text)
{
    _text += text;
}

////////////////////////////////////////////////////////////////////////////////
void DataManager::cancelCurrentFile(bool toError)
{
    if(_sourceId < 0)
        return;

    _globalDatabase.cancelFile(_sourceId, toError);
    _sourceId = -1;
    _docId = -1;
}
