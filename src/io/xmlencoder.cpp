/***************************************************************************
 *   Copyright (C) 2007 by Thorsten Wissmann                               *
 *   E-Mail: kollegstufe@thorsten-wissmann.de                              *
 *   Homepage: www.thorsten-wissmann.de                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "xmlencoder.h"
#include <QString>
#include <QChar>

QByteArray xmlEncoder::s_szReturnByteArray;

xmlEncoder::xmlEncoder()
{
}


xmlEncoder::~xmlEncoder()
{
}


char* xmlEncoder::xmlCodeToString(char* string)
{
    QString returnString = QString::fromLocal8Bit(string);
    returnString.replace(QString("&quot;"), QString("\""));
    returnString.replace(QString("&apos;"), QString("\'"));
    returnString.replace(QString("&lt;"), QString("<"));
    returnString.replace(QString("&gt;"), QString(">"));
    returnString.replace(QString("&nbsp;"), QString(" "));
    returnString.replace(QString("&auml;"), QString::fromLocal8Bit("??"));
    returnString.replace(QString("&amp;"), QString("&"));
    returnString.replace(QString("&auml;"), QString::fromLocal8Bit("??"));
    returnString.replace(QString("&ouml;"), QString::fromLocal8Bit("??"));
    returnString.replace(QString("&uuml;"), QString::fromLocal8Bit("??"));
    returnString.replace(QString("&Auml;"), QString::fromLocal8Bit("??"));
    returnString.replace(QString("&Ouml;"), QString::fromLocal8Bit("??"));
    returnString.replace(QString("&Uuml;"), QString::fromLocal8Bit("??"));
    returnString.replace(QString("&szlig;"), QString::fromLocal8Bit("??"));
    
    s_szReturnByteArray = returnString.toLocal8Bit();
    char* output = s_szReturnByteArray.data();
//     if(returnString.contains("ABCDEFGHI"))
//     {
//         qDebug("outputstring: \"%s\"", output);
//     }
    return output;
}

char* xmlEncoder::stringToXmlCode(char* string)
{
    QString returnString = QString::fromLocal8Bit(string);
    returnString.replace(QString("&"), QString("&amp;"));
    returnString.replace(QString("\""), QString("&quot;"));
    returnString.replace(QString("\'"), QString("&apos;"));
    returnString.replace(QString("<"), QString("&lt;"));
    returnString.replace(QString(">"), QString("&gt;"));
    returnString.replace(QString::fromLocal8Bit("??"), QString("&auml;"));
    returnString.replace(QString::fromLocal8Bit("??"), QString("&ouml;"));
    returnString.replace(QString::fromLocal8Bit("??"), QString("&uuml;"));
    returnString.replace(QString::fromLocal8Bit("??"), QString("&Auml;"));
    returnString.replace(QString::fromLocal8Bit("??"), QString("&Ouml;"));
    returnString.replace(QString::fromLocal8Bit("??"), QString("&Uuml;"));
    returnString.replace(QString::fromLocal8Bit("??"), QString("&szlig;"));
    s_szReturnByteArray = returnString.toLocal8Bit();
    return (char*)s_szReturnByteArray.data();
}


