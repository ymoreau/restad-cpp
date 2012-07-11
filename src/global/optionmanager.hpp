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

#ifndef OPTIONMANAGER_HPP
#define OPTIONMANAGER_HPP

#include "../global/exception.hpp"

#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QVariant>

//! Argument info
/*! \ingroup global */
class Option
{
public:
    enum ArgValueType {
        Flag, //!< Option is a flag with boolean value
        String,
        Int };

    static bool isNameValid(const QChar &name);
    static bool isLongNameValid(const QString &name);

    //! Initialize with the given values
    Option(QString description = QString(), ArgValueType valueType = Option::Flag,
             QVariant value = QVariant(), QString longName = QString());

    //! Clears the value
    void clear();

    bool isFlag() const;

    //! Stores the given value in the right type
    /*! \return False if the option does not have a value or if the string can't be parsed, true otherwise. */
    bool parseValue(const QString &strValue);

    //! Sets the option flag to true
    /*! \return False if the option is not flag type, true otherwise. */
    bool setFlag(bool set = true);

    QString longName() const;
    QString description() const;
    QString valueTypeString() const;
    QVariant value() const;

private:
    QString         _longName;
    QString         _description;
    ArgValueType    _valueType;
    QVariant        _value;
};

typedef QMap<QChar, Option> OptionMap;


//! Manages command and config-file options
/*! \ingroup global */
/*! \todo Sort options with groups and sort the help displayed */
class OptionManager
{
public:
    //! Defaults options : h -> help (flag), v -> version (string), f -> configfile (string)
    OptionManager(QString commandFormat = QString());

    //! Sets the command format message
    void setCommandFormat(const QString &commandFormat);

    //! Add the option to known options
    /*! \throw Exception If name or longName are not valid or if the option name is already known */
    void addOption(const QChar &name, const Option &option);

    //! Overloads addOption for convenience.
    /*! \throw Exception If name or longName are not valid or if the option name is already known */
    void addOption(const QChar &name, const QString &description, Option::ArgValueType valueType = Option::Flag,
                   QString longName = QString());

    //! Clears all values and set flags
    void clear();

    //! Parses the command line args and stores them in the map
    /*! If the config-file option (-f) is given, it will automatically parse the config-file after the args.
        \param nonOptionArgs A list to store the command line argument which are not options
        \param neededArgCount The expected count of command line non-option arguments
        \throw Exception If parsing fails */
    void parse(const QStringList &args, QStringList *nonOptionArgs = 0, int neededArgCount = 0);

    //! Parses the ini file from the given fileName and stores values in the map
    /*! \throw Exception If parsing fails */
    void parse(const QString &fileName);

    //! Sets the value for the specified option
    /*! \return False if option name is unknown or if value is not valid, true otherwise. */
    bool setValue(const QChar &optionName, const QString &value);

    /*! \return True if the option is a flag and is set, false otherwise. */
    bool isSet(const QChar &optionName) const;

    /*! \return An empty QVariant if option is unknown or does not have value (like flag options),
        the QVariant value otherwise. */
    QVariant optionValue(const QChar &optionName) const;

    //! Display the command line format and the available options
    void displayHelp() const;

private:
    OptionMap _optionMap;
    QString _commandFormat;
};


/*!
  \page option-parsing Option parsing
  \section intro Introduction
  Restad commands accept parameters from command-line or from a config file (INI format). Each parameter
  has a key of one letter, and a unique name (optional). Using the command line, you must specify a
  parameter with its one-letter key. Using the config file, you can use both key and unique name.

  Parameters can be flags, or values. A flag is actually like a boolean value, but its use is simpler.
  A value can be string or integer and can be empty or not. You can overwrite config-file parameters
  with the command-line.

  \section commandline Command line
  To set one or more flag(s) just give as command argument the one-letter key(s) after an hyphen "-".
  You can group flags like this :
  \verbatim
    $ restad-command -a -b -c
    $ restad-command -abc \endverbatim
  To set a value you must give the option key and then the value :
  \verbatim
    $ restad-command -d value \endverbatim
  To set a string value with spaces, enclose it with double-quotes :
  \verbatim
    $ restad-command -d "some words" \endverbatim
  You can group flags and one value, but only by putting the value key as the last character :
  \verbatim
    $ restad-command -abcd value
    $ restad-command -abdc value -> Error \endverbatim

  \section configfile Config file
  The config file uses the INI format. This means "key=value" text on each line. Empty lines or
  comment lines (using # at the begining of the line) have no effect on the parsing.
  You can use both one-letter key or long-name. Flag options must have a "true" or "false" value.
  Like command line options, to pass string value with spaces you must enclose it with double-quotes.
  \verbatim
# This is a comment
a=true
output=target-file
message = "I got spaces"
run = 10 \endverbatim
  */

#endif // OPTIONMANAGER_HPP
