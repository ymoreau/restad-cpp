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

#ifndef TEST_OPTIONMANAGER_HPP
#define TEST_OPTIONMANAGER_HPP

#include "global.hpp"

#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <iostream>
#include "global/optionmanager.hpp"

class TESTSUITE(TestOptionManager) TestOptionManager : public QObject
{
    Q_OBJECT

private slots:
    void optionValueParsing()
    {
        Option voidOpt;
        QVERIFY(!voidOpt.parseValue("whatever"));

        Option opt("", Option::Int);
        QVERIFY(!opt.parseValue("not a number"));
        QVERIFY(opt.parseValue("14"));
        QVERIFY(!opt.parseValue("12.6"));
    }
    void initializing()
    {
        OptionManager optionManager;
        QVERIFY_NOTHROW(optionManager.addOption('a', ""));
        QVERIFY_THROW(optionManager.addOption('a', ""), Exception);
        QVERIFY_THROW(optionManager.addOption('h', ""), Exception);

        QVERIFY_THROW(optionManager.addOption('-', ""), Exception);
        QVERIFY_THROW(optionManager.addOption('x', "", Option::Flag, "wrong name"), Exception);
    }
    void argsParsing()
    {
        OptionManager optionManager;

        QStringList args;
        args << "program-name" << "-hv";
        QVERIFY_NOTHROW(optionManager.parse(args));
        QVERIFY(optionManager.isSet('h'));
        QVERIFY(!optionManager.isSet('v'));
        optionManager.clear();

        args.clear();
        args << "program-name" << "-vh";
        QVERIFY_NOTHROW(optionManager.parse(args));
        QVERIFY(!optionManager.isSet('h'));
        QVERIFY(optionManager.isSet('v'));
        QVERIFY(!optionManager.isSet('a'));
        optionManager.clear();

        QVERIFY_NOTHROW(optionManager.addOption('a', "Flag"));
        QVERIFY(!optionManager.isSet('a'));
        QVERIFY_NOTHROW(optionManager.addOption('b', "String", Option::String));
        QVERIFY_NOTHROW(optionManager.addOption('c', "Int", Option::Int));

        args.clear();
        args << "program-name" << "whatever";
        QVERIFY_NOTHROW(optionManager.parse(args));

        args.clear();
        args << "program-name" << "-a" << "whatever";
        QVERIFY_NOTHROW(optionManager.parse(args));
        optionManager.clear();

        args.clear();
        args << "program-name" << "-c" << "whatever";
        QVERIFY_THROW(optionManager.parse(args), Exception);

        args.clear();
        args << "program-name" << "-ba" << "whatever";
        QVERIFY_THROW(optionManager.parse(args), Exception);

        args.clear();
        args << "program-name" << "-a" << "-b" << "string" << "-c" << "10";
        QVERIFY_NOTHROW(optionManager.parse(args));
        optionManager.clear();

        QStringList finalArgs, buffList;

        args.clear();
        args << "program-name" << "whatever";
        finalArgs << "whatever";
        QVERIFY_NOTHROW(optionManager.parse(args, &buffList));
        QCOMPARE(buffList, finalArgs);
        optionManager.clear();

        args.clear();
        args << "program-name" << "-v";
        QVERIFY_NOTHROW(optionManager.parse(args, &buffList, 2));
        QCOMPARE(buffList, finalArgs);
        optionManager.clear();

        args.clear();
        args << "program-name" << "-a" << "-b" << "string" << "-c" << "10";
        QVERIFY_NOTHROW(optionManager.parse(args));
        QVERIFY(optionManager.isSet('a'));
        QCOMPARE(optionManager.optionValue('b'), QVariant("string"));
        QCOMPARE(optionManager.optionValue('c'), QVariant(10));
        QVERIFY(!optionManager.isSet('b'));
        QVERIFY(!optionManager.isSet('z'));
        QCOMPARE(optionManager.optionValue('a'), QVariant());
        optionManager.clear();

        // Testing the clear() function
        QVERIFY(!optionManager.isSet('a'));
        QCOMPARE(optionManager.optionValue('b'), QVariant());
        QCOMPARE(optionManager.optionValue('c'), QVariant());

        args.clear();
        args << "program-name" << "-b" << "string" << "whatever" << "10";
        finalArgs.clear();
        finalArgs << "whatever" << "10";
        buffList.clear();
        QVERIFY_NOTHROW(optionManager.parse(args, &buffList));
        QCOMPARE(optionManager.optionValue('b'), QVariant("string"));
        QCOMPARE(buffList, finalArgs);
        optionManager.clear();

        args.clear();
        args << "program-name" << "-b" << "string" << "whatever";
        buffList.clear();
        QVERIFY_THROW(optionManager.parse(args, &buffList, 2), Exception);

        /*
        QString item;
        foreach(item, buffList)
            std::cerr << "  " << item.toStdString() << std::endl;
            */
    }
    void fileParsing()
    {
        OptionManager optionManager;
        QVERIFY_NOTHROW(optionManager.addOption('a', "Flag"));
        QVERIFY_NOTHROW(optionManager.addOption('b', "String", Option::String));
        QVERIFY_NOTHROW(optionManager.addOption('c', "Int", Option::Int));
        QVERIFY_NOTHROW(optionManager.addOption('d', "Int", Option::Int));

        QString filename = "./test_buffer_file";

        QVERIFY_THROW(optionManager.parse("/hope/no/one/will/ever/create/this/file"), Exception);

        QFile file(filename);
        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
        QTextStream stream(&file);
        stream << "# Comment\n" << "a=true\n" << "b = string  \n" << "\n\n# Comment\n  # Comment\n"
               << "c= 10\n" << "d=40\n" << "ignored" << "spam=true";
        stream.flush();
        file.close();

        QVERIFY_NOTHROW(optionManager.parse(filename));
        QVERIFY(optionManager.isSet('a'));
        QCOMPARE(optionManager.optionValue('b'), QVariant("string"));
        QCOMPARE(optionManager.optionValue('c'), QVariant(10));
        optionManager.clear();

        optionManager.setValue('d', "10");
        optionManager.setValue('b', "another string");
        QVERIFY_NOTHROW(optionManager.parse(filename)); // Parsing file will NOT overwrite values
        QVERIFY(optionManager.isSet('a'));
        QCOMPARE(optionManager.optionValue('d'), QVariant(10));
        QCOMPARE(optionManager.optionValue('b'), QVariant("another string"));
        optionManager.clear();

        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
        stream << "a=10\n";
        stream.flush();
        file.close();

        QVERIFY_THROW(optionManager.parse(filename), Exception);

        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
        stream << "c=not-a-number\n";
        stream.flush();
        file.close();

        QVERIFY_THROW(optionManager.parse(filename), Exception);
        optionManager.clear();

        QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
        stream << "b=\"I got spaces\"\n" << "b=\"I got spaces too\"\n";
        stream.flush();
        file.close();

        QVERIFY_NOTHROW(optionManager.parse(filename)); // The INI parsing only keeps the last value
        QCOMPARE(optionManager.optionValue('b'), QVariant("I got spaces too"));
        optionManager.clear();

        QStringList args;
        args << "program-name" << "-f" << filename;
        QVERIFY_NOTHROW(optionManager.parse(args));
        QCOMPARE(optionManager.optionValue('b'), QVariant("I got spaces too"));

        QVERIFY(QFile::remove(filename));
    }
};

#endif // TEST_OPTIONMANAGER_HPP
