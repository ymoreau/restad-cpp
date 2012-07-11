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

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <CuteTest.h>
#include <iostream>
#include "global/exception.hpp"

#define QVERIFY_THROW(expression, ExpectedExceptionType) \
do \
{ \
    bool caught = false; \
    try { expression; } \
    catch (ExpectedExceptionType const&) { caught = true; } \
    catch (...) {} \
    QVERIFY2(caught, #expression " : " #ExpectedExceptionType); \
    break; \
} while(0)

#define QVERIFY_NOTHROW(expression) \
do \
{ \
    bool caught = false; \
    try { expression; } \
    catch (const Exception &e) { caught = true; std::cerr <<__FILE__<<":"<<__LINE__<<" "<< e.message().toStdString() << std::endl; } \
    QVERIFY2(!caught, #expression); \
    break; \
} while(0)

#endif // GLOBAL_HPP
