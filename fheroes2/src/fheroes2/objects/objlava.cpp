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
#include "objlava.h"

bool ObjLava::isPassable(u16 icn, u8 index, u16 direct)
{
    switch(icn)
    {
	case ICN::OBJNLAV2:
	    // volcano
	    if((129 < index && index < 133) || (78 < index && index < 82))
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else return true;

	case ICN::OBJNLAV3:
	    // volcano
	    if(243 < index && index < 247)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else return true;

	case ICN::OBJNLAVA:
	    // crater
	    if((1 < index && index < 10) || (11 < index && index < 18)) return false;
	    else
	    // lava pool
	    if((17 < index && index < 21) || (26 < index && index < 45) || (45 < index && index < 49) ||
		( 49 < index && index < 64) || (64 < index && index < 74)) return false;
	    else
	    // volcano
	    if((75 < index && index < 78) || 88 == index || 98 == index)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // obelisk
	    if(110 == index) return false;
	    else
	    // daemon cave
	    if(113 < index && index < 116)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));
	    else
	    // sign
	    if(117 == index) return false;
	    else
	    // saw mill
	    if(119 < index && index < 122) return false;
	    else
	    if(121 < index && index < 127)
		return Direction::UNKNOWN == direct || (direct & (Direction::LEFT | Direction::RIGHT | DIRECTION_BOTTOM_ROW));

	    else return true;

	default: break;
    }

    return false;
}
