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

#include <QtCore/QSettings>
#include <QtCore/QRegExp>
#include <QtCore/QFile>

#include "optionmanager.hpp"
#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
bool Option::isNameValid(const QChar &name)
{
    return name.isLetter() && name.isLower();
}

////////////////////////////////////////////////////////////////////////////////
bool Option::isLongNameValid(const QString &name)
{
    QRegExp regex("^(\\w|-|_)*$");
    return regex.exactMatch(name);
}

////////////////////////////////////////////////////////////////////////////////
Option::Option(QString description, ArgValueType valueType, QVariant value, QString longName)
{
    if(!Option::isLongNameValid(longName))
        throw Exception(QString("Long name '") + longName + "' is not valid");

    _longName = longName.toLower();
    _description = description;
    _valueType = valueType;
    _value = value;
}

////////////////////////////////////////////////////////////////////////////////
void Option::clear()
{
    _value = QVariant();
}

////////////////////////////////////////////////////////////////////////////////
bool Option::isFlag() const
{
    return _valueType == Option::Flag;
}

////////////////////////////////////////////////////////////////////////////////
bool Option::parseValue(const QString &strValue)
{
    if(_valueType == Option::Flag)
        return false;

    bool isOk = true;

    if(_valueType == Option::String)
    {
        _value = strValue;
    }
    else if(_valueType == Option::Int)
    {
        int tempValue = strValue.toInt(&isOk);
        if(!isOk)
            return false;
        _value = tempValue;
    }
    else
        return false;

    return true;
}

////////////////////////////////////////////////////////////////////////////////
bool Option::setFlag(bool set)
{
    if(_valueType != Option::Flag)
        return false;

    _value = set;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
QString Option::longName() const
{
    return _longName;
}

////////////////////////////////////////////////////////////////////////////////
QString Option::description() const
{
    return _description;
}

////////////////////////////////////////////////////////////////////////////////
QString Option::valueTypeString() const
{
    if(_valueType == Option::Int)
        return "int";
    else if(_valueType == Option::String)
        return "string";
    return "";
}

////////////////////////////////////////////////////////////////////////////////
QVariant Option::value() const
{
    return _value;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
OptionManager::OptionManager(QString commandFormat) : _commandFormat(commandFormat)
{
    addOption('h', "Display help", Option::Flag);
    addOption('v', "Display version", Option::Flag);
    addOption('q', "Silent mode", Option::Flag);
    addOption('f', "Config file", Option::String);
}

////////////////////////////////////////////////////////////////////////////////
void OptionManager::setCommandFormat(const QString &commandFormat)
{
    _commandFormat = commandFormat;
}

////////////////////////////////////////////////////////////////////////////////
void OptionManager::addOption(const QChar &name, const Option &option)
{
    if(!Option::isNameValid(name))
        throw Exception(QString("Option name '") + name + "' is not a valid letter");
    if(_optionMap.contains(name))
        throw Exception(QString("Option name '") + name + "' is already known and can't be added");

    _optionMap.insert(name, option);
}

////////////////////////////////////////////////////////////////////////////////
void OptionManager::addOption(const QChar &name, const QString &description,
                              Option::ArgValueType valueType, QString longName)
{
    addOption(name, Option(description, valueType, QVariant(), longName));
}

////////////////////////////////////////////////////////////////////////////////
void OptionManager::clear()
{
    foreach(QChar key, _optionMap.keys())
        _optionMap[key].clear();
}

////////////////////////////////////////////////////////////////////////////////
void OptionManager::parse(const QStringList &args, QStringList *nonOptionArgs, int neededArgCount)
{
    bool waitingForValue = false;
    QChar optionWaiting;
    QString configFilePath;

    for(int i=1; i<args.count(); ++i) // Ignore first arg, it is program name
    {
        QString str = args.at(i);

        if(waitingForValue)
        {
            if(!_optionMap.contains(optionWaiting)) // Should have been checked already
                throw Exception("Intern error...");

            if(!_optionMap[optionWaiting].parseValue(str))
                throw Exception(QString("Error parsing the value '") + str + "' of '" + optionWaiting + "'");

            if(optionWaiting == 'f') // Current value is a file config path
                configFilePath = str;

            waitingForValue = false;
        }
        else if(str.at(0) == '-')
        {
            for(int j=1; j<str.size(); ++j)
            {
                QChar optionName = str.at(j);
                if(_optionMap.contains(optionName))
                {
                    if(_optionMap[optionName].isFlag())
                    {
                        if(!_optionMap[optionName].setFlag()) // Should never fail
                            throw Exception("Intern error...");
                    }
                    else if(j == str.size()-1) // Current char is the last
                    {
                        optionWaiting = optionName;
                        waitingForValue = true;
                    }
                    else // Option with value is in the middle of grouped options
                    {
                        throw Exception(QString("Error, option '") + optionName + "' expects a value, it can not be specified here");
                    }
                }
                else
                {
                    throw Exception(QString("Error, option name '") + optionName + "' is unknown");
                }

                if(optionName == 'v' || optionName == 'h')
                    return;
            }
        }
        else // Not an option nor an option value
        {
            if(nonOptionArgs)
                nonOptionArgs->append(str);
            // No failure, the string is just ignored
        }
    }

    if(waitingForValue)
        throw Exception(QString("Error, option '") + optionWaiting + "' expects a value");

    if(nonOptionArgs && neededArgCount > nonOptionArgs->size())
    {
        throw Exception(QString("Missing argument(s), expected ") + QString::number(neededArgCount)
                        + " got " + QString::number(nonOptionArgs->size()) + "\nusage: " + _commandFormat);
    }

    if(!configFilePath.isEmpty())
        parse(configFilePath);
}

////////////////////////////////////////////////////////////////////////////////
void OptionManager::parse(const QString &fileName)
{
    if(!QFile::exists(fileName))
        throw Exception(QString("Config file '") + fileName + "' not found");

    QSettings settings(fileName, QSettings::IniFormat);
    QString fileKey;
    QChar optionName;
    foreach(fileKey, settings.allKeys())
    {
        fileKey = fileKey.toLower();
        foreach(optionName, _optionMap.keys())
        {
            if(_optionMap[optionName].longName() == fileKey || QString(optionName) == fileKey)
            {
                if(!_optionMap[optionName].value().isValid())
                {
                    if(_optionMap[optionName].isFlag())
                    {
                        if(settings.value(fileKey).toString().toLower() != "true" && settings.value(fileKey).toString().toLower() != "false")
                            throw Exception(QString("Error, option '") + _optionMap[optionName].longName() + "' expects boolean value");

                        if(!_optionMap[optionName].setFlag(settings.value(fileKey).toBool())) // Should never fail
                            throw Exception("Intern error...");
                    }
                    else if(!_optionMap[optionName].parseValue(settings.value(fileKey).toString()))
                    {
                        throw Exception(QString("Error parsing option '") + _optionMap[optionName].longName() + "' value");
                    }
                }
                break;
            }
        }
    }

}

////////////////////////////////////////////////////////////////////////////////
bool OptionManager::setValue(const QChar &optionName, const QString &value)
{
    if(!_optionMap.contains(optionName))
        return false;
    return _optionMap[optionName].parseValue(value);
}

////////////////////////////////////////////////////////////////////////////////
bool OptionManager::isSet(const QChar &optionName) const
{
    if(!_optionMap.contains(optionName))
        return false;
    if(!_optionMap[optionName].isFlag())
        return false;
    return _optionMap[optionName].value().toBool();
}

////////////////////////////////////////////////////////////////////////////////
QVariant OptionManager::optionValue(const QChar &optionName) const
{
    if(!_optionMap.contains(optionName))
        return QVariant();
    if(_optionMap[optionName].isFlag())
        return QVariant();
    return _optionMap[optionName].value();
}

////////////////////////////////////////////////////////////////////////////////
void OptionManager::displayHelp() const
{
    cout << _commandFormat.toLocal8Bit().constData() << "\n\n";
    foreach(const QChar &key, _optionMap.keys())
    {
        QString valueDescription = _optionMap[key].valueTypeString();
        QString spaces(10 - valueDescription.size(), ' ');
        cout << "  -" << key.toAscii() << " " << valueDescription.toLocal8Bit().constData() << spaces.toLocal8Bit().constData()
                << _optionMap[key].description().toLocal8Bit().constData() << "\n";
    }
    cout << endl;
}
