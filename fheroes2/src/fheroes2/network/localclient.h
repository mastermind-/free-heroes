/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov                               *
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

#ifndef H2LOCALCLIENT_H
#define H2LOCALCLIENT_H

#include "gamedefs.h"

#ifdef WITH_NET

#include "network.h"

class FH2LocalClient : public FH2Client
{
public:
    static FH2LocalClient & Get(void);

    ~FH2LocalClient(){};

    bool Connect(const std::string &, u16);

    int Logout(void);
    int ConnectionChat(void);
    int ScenarioInfoDialog(void);
    int StartGame(void);

    std::string server;
    std::vector<Player> players;

private:
    FH2LocalClient();
};

#endif
#endif
