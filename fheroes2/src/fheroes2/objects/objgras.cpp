/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
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

#include "icn.h"
#include "objgras.h"
#include <iostream>

bool ObjGrass::isPassable(u16 icn, u8 index, u16 direct)
{
    switch(icn)
    {
	case ICN::OBJNGRA2:
	    // hill fort
	    if(1 < index && index < 5)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // halfling hole
	    if(5 < index && index < 9)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // tree city
	    if(20 < index && index < 23)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // wind mill
	    if(55 == index || 59 == index)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // archer house
	    if(84 == index) return false;
	    else
	    // goblin hut
	    if(92 == index) return false;
	    else
	    // dwarf cottadge
	    if(114 == index) return false;
	    else
	    // oracul
	    if(124 < index && index < 127)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // obelisk
	    if(129 == index) return false;
	    else return true;	    

	case ICN::OBJNGRAS:
	    // abandoned mines
	    if(3 == index)
		return false;
	    else
	    if(4 < index && index < 8)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // faerie ring
	    if(29 < index && index < 32)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // rock
	    if((32 < index && index < 35) || (36 < index && index < 39) || (39 < index && index < 42) ||
		43 == index || 45 == index || 47 == index || 49 == index)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // lake
	    if((53 < index && index < 59) || (64 < index && index < 69)) return false;
	    else
	    if((58 < index && index < 64) || (68 < index && index < 73) || (72 < index && index < 76))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // mound
	    if((76 < index && index < 79) || (148 < index && index < 151))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // trees
	    if((82 < index && index < 86) || (88 < index && index < 91) || 93 == index)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // shrub
	    if((94 < index && index < 98) || (98 < index && index < 102) || (102 < index && index < 105) ||
		(105 < index && index < 108) || (108 < index && index < 111) || 112 == index ||
		(113 < index && index < 117) || (120 < index && index < 124) || (124 < index && index < 128) ||
		(128 < index && index < 132) || (134 < index && index < 137) || (138 < index && index < 141) ||
		142 == index || 144 == index || 146 == index || 148 == index)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
		return true;

	default: break;
    }

    return false;
}
