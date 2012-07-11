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

#ifndef TEST_GLOBAL_HPP
#define TEST_GLOBAL_HPP

#include "global.hpp"

#include <QtCore/QString>

#include "parser/documentlist.hpp"
#include "parser/tagfilter.hpp"

class TESTSUITE(TestGlobal) TestGlobal : public QObject
{
    Q_OBJECT

private slots:
    void documentList()
    {
        DocumentInfo invalid;
        QVERIFY(!invalid.isValid());

        DocumentList list;
        DocumentInfo document(ParserFactory::XML, 1, "path");
        QVERIFY(document.isValid());

        list.addDocument(ParserFactory::XML, 1, "path");
        QCOMPARE(list.size(), 1);

        DocumentInfo bufferDoc = list.takeDocument(ParserFactory::ODT);
        QVERIFY(!bufferDoc.isValid());
        bufferDoc = list.takeDocument(ParserFactory::XML);
        QVERIFY(bufferDoc.isValid());
        QCOMPARE(bufferDoc.sourceId(), 1);
        QCOMPARE(bufferDoc.uri(), QString("path"));

        QCOMPARE(list.size(), 0);
        bufferDoc = list.takeDocument(ParserFactory::XML);
        QVERIFY(!bufferDoc.isValid());

        list.addDocument(ParserFactory::XML, 1, "path");
        list.addDocument(ParserFactory::XML, 2, "path");
        list.addDocument(ParserFactory::ODT, 3, "path");
        QCOMPARE(list.size(), 3);
        QCOMPARE(list.sourceIds(), QList<int>() << 1 << 2 << 3);

        bufferDoc = list.takeDocument(ParserFactory::ODT);
        QVERIFY(bufferDoc.isValid());
        QCOMPARE(bufferDoc.sourceId(), 3);
        bufferDoc = list.takeDocument(ParserFactory::ODT);
        QVERIFY(!bufferDoc.isValid());

        QCOMPARE(list.mostWaitingType(), ParserFactory::XML);

        list.addDocument(ParserFactory::ODT, 4, "path");
        list.addDocument(ParserFactory::ODT, 5, "path");
        list.addDocument(ParserFactory::ODT, 6, "path");
        QCOMPARE(list.mostWaitingType(), ParserFactory::ODT);

        bufferDoc = list.takeDocument(ParserFactory::ODT);
        QVERIFY(bufferDoc.isValid());
        bufferDoc = list.takeDocument(ParserFactory::ODT);
        QVERIFY(bufferDoc.isValid());
        QCOMPARE(list.mostWaitingType(), ParserFactory::XML);
    }

    void tagfilter()
    {
        TagFilter filter1;

        QVERIFY(filter1.acceptTag("a"));
        QVERIFY(filter1.acceptTag("x"));

        filter1.setIgnoredTags(QStringList() << "a" << "b");
        QVERIFY(!filter1.acceptTag("a"));
        QVERIFY(!filter1.acceptTag("b"));
        QVERIFY(filter1.acceptTag("c"));

        filter1.setAcceptedTags(QStringList() << "a" << "b");
        QVERIFY(!filter1.acceptTag("a"));
        QVERIFY(!filter1.acceptTag("b"));

        TagFilter filter2(TagFilter::Ignore);
        filter2.setAcceptedTags(QStringList() << "a" << "b");
        QVERIFY(filter2.acceptTag("a"));
        QVERIFY(filter2.acceptTag("b"));
        QVERIFY(!filter2.acceptTag("c"));

        filter2.setIgnoredTags(QStringList() << "a" << "b");
        QVERIFY(filter2.acceptTag("a"));
        QVERIFY(filter2.acceptTag("b"));
    }

};

#endif // TEST_GLOBAL_HPP
