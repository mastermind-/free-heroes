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

#include <string>
#include "text.h"
#include "cursor.h"
#include "display.h"
#include "castle.h"
#include "payment.h"
#include "kingdom.h"
#include "world.h"
#include "dialog.h"

void Dialog::StatusBar::ShowBuildMessage(bool isBuilt, bool allowBuild, const std::string & message, const Castle & castle, const u32 building)
{
    if(isBuilt)
	ShowMessage(message + " is already built");
    else
    {
	const PaymentConditions::BuyBuilding paymentBuild(castle.GetRace(), static_cast<Castle::building_t>(building));

	if(!castle.AllowBuild())
    	    ShowMessage("Cannot build. Already built here this turn.");
        else
        if(castle.AllowBuild() && paymentBuild > world.GetMyKingdom().GetFundsResource())
    	    ShowMessage("Cannot afford " + message);
        else
        if(Castle::BUILD_SHIPYARD == building && !castle.HaveNearlySea())
    	    ShowMessage("Cannot build " + message + " because castle is to far from water.");
        else
        if(!castle.AllowBuyBuilding(static_cast<Castle::building_t>(building)))
    	    ShowMessage("Cannot build " + message);
        else
    	    ShowMessage("Build " + message);
    }
}
 
void Dialog::StatusBar::ShowMessage(const std::string & message)
{
    if(message == status) return;

    if(! status.empty()) Clear();

    Cursor::Hide();

    Text(message, font, pos_pt.x + (sprite.w() - Text::width(message, font)) / 2,
         pos_pt.y + (sprite.h() - Text::height(message, font)) / 2);

    status = message;

    Cursor::Show();
}

void Dialog::StatusBar::Clear(void)
{
    bool localcursor = Cursor::Visible();

    if(localcursor) Cursor::Hide();

    display.Blit(sprite, pos_pt);

    status.clear();

    if(localcursor) Cursor::Show();
}

void Dialog::StatusBar::Clear(const std::string & message)
{
    if(message == status) return;

    bool localcursor = Cursor::Visible();

    if(localcursor) Cursor::Hide();

    display.Blit(sprite, pos_pt);

    status = message;

    Text(message, font, pos_pt.x + (sprite.w() - Text::width(message, font)) / 2,
         pos_pt.y + (sprite.h() - Text::height(message, font)) / 2);

    if(localcursor) Cursor::Show();
}

bool Dialog::StatusBar::isEmpty(void)
{
    return status.empty();
}
