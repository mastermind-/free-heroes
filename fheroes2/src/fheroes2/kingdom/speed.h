/***************************************************************************
 *   Copyright (C) 2006 by Andrey Afletdinov                               *
 *   afletdinov@mail.dc.baikal.ru                                          *
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
#ifndef H2SPEED_H
#define H2SPEED_H

#include <string>
#include "gamedefs.h"

namespace Speed
{
    typedef enum {
	CRAWLING,
        VERYSLOW,
        SLOW,
        AVERAGE,
        FAST,
        VERYFAST,
        ULTRAFAST,
        BLAZING,
        INSTANT
    } speed_t;

    const std::string & String(u8 speed);
    u8 Move(u8 speed);

    inline speed_t& operator++ (speed_t& speed){ return speed = ( INSTANT == speed ? INSTANT : speed_t(speed + 1)); };
    inline speed_t& operator-- (speed_t& speed){ return speed = ( CRAWLING == speed ? CRAWLING : speed_t(speed - 1)); };
};

#endif
