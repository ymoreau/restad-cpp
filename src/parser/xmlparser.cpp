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

#include <QtCore/QDebug>
#include <QXmlSimpleReader>

#include "../global/exception.hpp"

#include "../parser/stringfilters.hpp"
#include "../parser/tagfilter.hpp"
#include "../parser/indexingprogress.hpp"

#include "xmlparser.hpp"

////////////////////////////////////////////////////////////////////////////////
XMLParser::XMLParser(IndexingProgress &indexingProgress, DocumentList &documentList, OptionManager &optionManager,
                     GlobalParserDatabase &globalDatabase, TagFilter *tagFilter) :
    AbstractParser(indexingProgress, documentList, optionManager, globalDatabase, tagFilter)
{
}

////////////////////////////////////////////////////////////////////////////////
XMLParser::~XMLParser()
{
}

////////////////////////////////////////////////////////////////////////////////
void XMLParser::run()
{
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);

    _currentDocument = _documentList.takeDocument(ParserFactory::XML);
    while(_currentDocument.isValid())
    {
        QIODevice *dev = AbstractParser::readFile(_currentDocument.uri());
        if(!dev->open(QIODevice::ReadOnly))
        {
            _indexingProgress.incrementFailedDocument(QString("idFile[") + QString::number(_currentDocument.sourceId())
                                                      + "] " + _currentDocument.uri() + ": can not open file");
        }

        QXmlInputSource *source = new QXmlInputSource(dev);

        try
        {
            _docTag = _currentDocument.docSeparator();

            _isParsing = false;
            if(_docTag.isEmpty()) // Not a multiple doc file
                initDocument();

            if(!reader.parse(source))
            {
                _dataManager.cancelCurrentFile();
                _indexingProgress.incrementFailedDocument(QString("idFile[") + QString::number(_currentDocument.sourceId())
                                                          + "] " + _currentDocument.uri() + ": " + _errorMessage);
            }
            else
            {
                if(_docTag.isEmpty())
                    _dataManager.flushDocument(true);
                else
                    _dataManager.validateSource(false);
                _indexingProgress.incrementDoneDocument();

            }
        }
        catch(const DatabaseException &e)
        {
            try { _dataManager.cancelCurrentFile(); } catch(const DatabaseException &) {} // Ignore a database error
            _indexingProgress.incrementFailedDocument(QString("idFile[") + QString::number(_currentDocument.sourceId())
                                                      + "] " + _currentDocument.uri() + ": " + e.message());
        }

        delete dev;
        delete source;

        _currentDocument = _documentList.takeDocument(ParserFactory::XML);
    }
}

////////////////////////////////////////////////////////////////////////////////
bool XMLParser::startElement(const QString &, const QString &, const QString &qName, const QXmlAttributes &atts)
{
    QString name = StringFilters::cleanName(qName, StringFilters::MaxTagNameLength);
    if(_tagFilter && !_tagFilter->acceptTag(name))
        return true;

    //! \todo search for doc title

    if(!_isParsing && name == _docTag)
    {
        try
        {
            initDocument();
        }
        catch(const DatabaseException &e)
        {
            _indexingProgress.incrementFailedDocument(QString("idFile[") + QString::number(_currentDocument.sourceId())
                                                      + "] " + _currentDocument.uri() + ": " + e.message());
            return false;
        }
    }

    if(!_isParsing)
        return true;

    try
    {
        ++_depth;

        // Add the tag to the stack
        int parentId = -1;
        if(!_tagStack.isEmpty())
            parentId = _tagStack.top().idTag;

        int tagId = _dataManager.generateTagId();

        // Computes the tag order
        if(!_tagNamesCount.contains(name))
            _tagNamesCount[name][parentId] = 1;
        int tagOrder = _tagNamesCount[name].value(parentId, 1);
        _tagNamesCount[name][parentId] = tagOrder + 1;

        _tagStack.push(Tag(tagId, name, _dataManager.tagNameId(name), parentId, tagOrder, _dataManager.currentOffset()));

        // Add the attributes
        for(int i=0; i<atts.count(); ++i)
            _dataManager.addAttribute(tagId, atts.qName(i), atts.value(i));
    }
    catch(const DatabaseException &e)
    {
        _errorMessage = e.message();
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool XMLParser::endElement(const QString &, const QString &, const QString &qName)
{
    QString name = StringFilters::cleanName(qName, StringFilters::MaxTagNameLength);
    if(_tagFilter && !_tagFilter->acceptTag(name))
        return true;

    if(!_isParsing)
        return true;

    Tag tag = _tagStack.pop();
    if(tag.name != StringFilters::cleanName(qName, StringFilters::MaxTagNameLength))
    {
        _errorMessage = "Unexpected ending tag '" + StringFilters::cleanName(qName, StringFilters::MaxTagNameLength)
                + "', expected '" + tag.name + "' (depth: " + QString::number(_depth) + ")";
        return false;
    }

    tag.endingOffset = _dataManager.currentOffset();
    _dataManager.addTag(tag);

    --_depth;

    if(name == _docTag) // End of document
    {
        _isParsing = false;

        try
        {
            _dataManager.flushDocument(false);
        }
        catch(const DatabaseException &e)
        {
            _indexingProgress.incrementFailedDocument(QString("idFile[") + QString::number(_currentDocument.sourceId())
                                                      + "] " + _currentDocument.uri() + ": " + e.message());
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool XMLParser::characters(const QString &ch)
{
    if(!_isParsing)
        return true;

    _dataManager.addText(ch);
    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool XMLParser::fatalError(const QXmlParseException &exception)
{
    _errorMessage = QString("line") + QString::number(exception.lineNumber()) + ": " + exception.message();
    return false;
}

////////////////////////////////////////////////////////////////////////////////
void XMLParser::initDocument()
{
    _depth = 0;
    _tagStack.clear();
    _tagNamesCount.clear();
    _dataManager.startDocument(_currentDocument.sourceId());
    _isParsing = true;
}

