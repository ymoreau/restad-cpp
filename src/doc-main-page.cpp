/* *****************************************************************************
 * 
 * This file is a part of the Restad project
 * https://sourceforge.net/projects/restad/
 * Author : Yoann Moreau (moreau.yo * at * gmail.com)
 *
 * Copyright (C) 2012 LIA (www.lia.univ-avignon.fr)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 ******************************************************************************/

/*!
  \mainpage
  Restad (Relational Storage for Tagged Documents) is an indexing-querying tool for tagged documents
  using a relational database for storage and querying.
  It actually uses PostgreSQL because it's "the world's most advanced open source database".
  Restad has been developped by the <a href="http://www.lia.univ-avignon.fr/">Computer Science lab of the university of Avignon.</a>

  \section dwnlds Download and install
  To download the tool, see the <a href="https://sourceforge.net/projects/restad/files/">download page</a>.
  See \ref install for details.

  \section uses How to use Restad
  Restad provides tools to index the documents in the database and SQL functions to query the documents for
  full-text search on semi-structured data. First you will need to install the tools and set up the database,
  see \ref install.

  For now, indexing tools only works on local files, that means you need to run the command
  on the system hosting the files. However the database can be a distant server. See \ref indexing.

  Restad does not provide any interface to query the database because there a lot of good tools to connect to
  and query a PostgreSQL database. The work is done by the database server, so Restad is how efficient as
  your PostgreSQL server is, and will always make the most of PostgreSQL updates. See details about
  database schema and functions in \ref querying.

  \section dvlpers Contribute to the project
  Restad is open-source and anyone can contribute or fork and edit code to fit to special needs.
  See \ref developers.

  Project website : http://restad.sourceforge.net <br/>
  Project on sourceforge : https://sourceforge.net/projects/restad <br/>
  See the last news on the blog : https://sourceforge.net/p/restad/blog

  \author Yoann Moreau (moreau.yo * at * gmail.com)
  \version 0.1.0

  */

//--------------------------------------------------------------------------------------------------
// Users doc
//--------------------------------------------------------------------------------------------------
/*!
  \page install Install guide

  To use Restad you need a <a href="http://wiki.postgresql.org/wiki/Detailed_installation_guides">PostgreSQL</a> server.
  To compile the Restad commands you will need Qt4 and PostgreSQL libpq C library.

  \section install-commands Install the restad commands
  \subsection debian Ubuntu/Debian
  Restad needs the following packages :
  \code
libpq-dev qt4-dev-tools
  \endcode

  In the root directory, run
  \code
qmake -config release
make
sudo make install
  \endcode
  The binary files are procuced in bin directory and installed in /usr/bin.


  \section install-database Set up the database
  <ul>
    <li><a href="http://www.postgresql.org/docs/8.4/static/sql-createdatabase.html">Create a database</a></li>
    <li>Set the <a href="http://www.postgresql.org/docs/8.4/static/textsearch.html">full-text</a>
        <a href="http://www.postgresql.org/docs/8.4/static/textsearch-intro.html#TEXTSEARCH-INTRO-CONFIGURATIONS">configuration</a></li>
    <li>Create tables using the <b>sql/database.sql</b> file</li>
    <li>Create functions using the <b>sql/restad.sql</b> file</li>
  </ul>

  */

/*!
  \page indexing How to index
  <p><i>command</i> -h will display all available options.<br/>
  <i>command</i> -v will display the version.</p>

  At first, Restad needs to list the documents in the database. Then it will parse and index the listed
  files.

  \section options Command options
  The restad commands can use a config file to read options, using INI format. Options given as command line arguments
  will overwrite the config file values. See option-parsing for more details.

  Global options of the commands :
  \verbatim
-h           Display help
-v           Display version

-f string    Config file

-q           Silent mode

-d string    Database name
-t string    Database host name
-u string    Database user name
-w string    Database password
-x string    Database encoding
  \endverbatim

  The database encoding is used to encode properly the parsed text, and display the database messages
  in the console.

  \section preparsing Add files to the database
  This step is called preparsing, the command <b>restad-preparser</b> will list the files and add them
  to the database. You can specify here if a file contains more than one document by giving the tag
  surrounding every document.

  %Preparser command format :
  \verbatim
preparser [options] path-to-explore

  -e string    Filter by extensions. Example "*.xml *.html"
  -m string    Document tag for multiple document files (the name of the tag enclosing one document)
  -r           Recursive listing of the files
  \endverbatim

  \section indexing Parse and index documents
  This tool will connect to the database, get back files to process (i.e. file path) and start parsing
  them using as much threads as it can. Maximum threads number is the number of cores of the system except
  one, or the maximum number specified with the option <i>-m</i>. It will get 1000 documents by default and
  process them all before quit. You can specify the number of documents to process with the option <i>-c</i>.

  %Indexer command format :
  \verbatim
indexer [options]

  -c int       Max document count to process, default is 1000
  -p int       Max threads to use, default is all available cores except one
  \endverbatim

  */

/*!
  \page querying How to query
  The SQL functions have not changed (yet) since prototype has been released, you can see the
  doc here <a href="https://github.com/ymoreau/Restad/wiki/Restad-SQL-functions">Restad-SQL-functions</a>.

  \section db-schema Database schema
  \image html database.png Database schema

  */

//--------------------------------------------------------------------------------------------------
// Developpers doc
//--------------------------------------------------------------------------------------------------

// Code groups, to sort classes in the doc
/*!
  \defgroup global Global code used by all commands
  \defgroup exceptions Exceptions of the project
  \defgroup commands Code managing the command line interfaces
  \defgroup preparser Code for the preparsing task
  \defgroup parser Code for the parsing/indexing task
  */

/*!
  \page developers Developer guide
  Restad uses Qt and its qmake/project-file system.
    - The restad.pro file is managing the whole project, but each command has its own project file (in src dir).
    - global.pri is included by all other project file
    - Most of the source files are included by src.pri,
      because they may be used in the final bin file or in the test bin file. Any class which is used by
      more than one project file must be included in src.pri instead of the project file. A class in src.pri
      must include classes which are also in src.pri or it will get linking errors.

  Code documentation is generated by Doxygen, the config file is Doxyfile. Simply run doxygen in the
  project root dir and everything is created/updated in doc/html.

  The project has some test units, see \ref tests.

  If you want to contribute, you can check \ref todo.
  The project uses Git. You can browse the code here https://sourceforge.net/p/restad/code/ .
  The classes of the project are grouped in subdirs, which are represented as modules in this doc, see
  <a href="./modules.html">Modules section</a>.
  Here is a raw class diagram used as a draw of the project, not up to date, not true UML but gives an overview of classes.
  \image html class_diagram.png Class diagram

  */

/*!
  \page tests Tests
  Restad project uses the Qt and CuteTest (https://bitbucket.org/mayastudios/cutetest/overview)
  for testing, you'll need to install both, and specify in test/test.pro the path for CuteTest
  includes and libraries.
  You will need to compile with qmake CONFIG+=debug CONFIG-=release for the test part.

  To test Restad with a database, a Ruby script is doing the job (test/test_index.rb), it needs the pg gem
  and an empty database set up with Restad schema. Just pass a restad-config file as argument and run the
  script into the target directory (i.e. where the Restad bin files are).

  */
