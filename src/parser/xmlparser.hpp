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

#ifndef XMLPARSER_HPP
#define XMLPARSER_HPP

#include <QXmlDefaultHandler>
#include <QtCore/QString>
#include <QtCore/QStack>
#include <QtCore/QHash>

#include "../parser/abstractparser.hpp"

#include "../parser/documentlist.hpp"

class DocumentInfo;

//! Parser for XML files
/*! \ingroup parser */
class XMLParser : public AbstractParser, public QXmlDefaultHandler
{
public:
    //! Initializes the parser
    /*! \throw DatabaseException if initialization of database fails */
    XMLParser(IndexingProgress &indexingProgress, DocumentList &documentList, OptionManager &optionManager,
              GlobalParserDatabase &globalDatabase, TagFilter *tagFilter = 0);

    virtual ~XMLParser();

    virtual void run();

    // QXmlDefaultHandler functions
    virtual bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
    virtual bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    virtual bool characters(const QString &ch);
    virtual bool fatalError(const QXmlParseException &exception);

    //! Starts a new document
    void initDocument();

private:
    DocumentInfo _currentDocument;
    bool _isParsing;
    QString _docTag;
    int _depth;
    QStack<Tag> _tagStack;
    QString _errorMessage;
    QHash<QString, QHash<int, int> > _tagNamesCount; // map[tag-name][parent-id] -> count == tag_order_position
};

#endif // XMLPARSER_HPP
