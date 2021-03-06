/***************************************************************************
 *   Copyright (C) 2013 by Andrey Afletdinov <fheroes2@gmail.com>          *
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
#ifndef H2MAPS_ACTIONS_H
#define H2MAPS_ACTIONS_H

#include "kingdom/color.h"
#include "maps/position.h"
#include "resource/artifact.h"
#include "resource/resource.h"

class Heroes;
enum { ACTION_UNKNOWN = 0, ACTION_DEFAULT, ACTION_ACCESS, ACTION_MESSAGE, ACTION_RESOURCES, ACTION_ARTIFACT };

struct ActionMessage : public ObjectSimple
{
    std::string message;

    ActionMessage() : ObjectSimple(ACTION_MESSAGE) {}
    static bool Action(ActionMessage*, s32, Heroes &);
};

struct ActionDefault : public ObjectSimple
{
    bool        enabled;
    std::string message;

    ActionDefault() : ObjectSimple(ACTION_DEFAULT), enabled(true) {}
    static bool Action(ActionDefault*, s32, Heroes &);
};

struct ActionAccess : public ObjectSimple
{
    int         allowPlayers;
    bool        allowComputer;
    bool        cancelAfterFirstVisit;
    std::string message;

    ActionAccess() : ObjectSimple(ACTION_ACCESS), allowPlayers(Color::ALL), allowComputer(true), cancelAfterFirstVisit(false) {}
    static bool Action(ActionAccess*, s32, Heroes &);
};

struct ActionArtifact : public ObjectSimple
{
    Artifact    artifact;
    std::string message;

    ActionArtifact() : ObjectSimple(ACTION_ARTIFACT) {}
    static bool Action(ActionArtifact*, s32, Heroes &);
};

struct ActionResources : public ObjectSimple
{
    Funds       resources;
    std::string message;

    ActionResources() : ObjectSimple(ACTION_RESOURCES) {}
    static bool Action(ActionResources*, s32, Heroes &);
};

#endif
