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
#include <vector>
#include "agg.h"
#include "world.h"
#include "animation.h"
#include "localevent.h"
#include "button.h"
#include "cursor.h"
#include "config.h"
#include "castle.h"
#include "dialog.h"
#include "heroes.h"
#include "localevent.h"
#include "background.h"
#include "tools.h"
#include "text.h"
#include "payment.h"

Dialog::answer_t Castle::DialogBuyBuilding(building_t build, bool buttons)
{
    const std::string &system = (H2Config::EvilInterface() ? "SYSTEME.ICN" : "SYSTEM.ICN");

    Cursor::Hide();

	const std::string & building_description =
			(DWELLING_MONSTER1 |
			 DWELLING_MONSTER2 |
			 DWELLING_MONSTER3 |
			 DWELLING_MONSTER4 |
			 DWELLING_MONSTER5 |
			 DWELLING_MONSTER6 |
			 DWELLING_UPGRADE2 |
			 DWELLING_UPGRADE3 |
			 DWELLING_UPGRADE4 |
			 DWELLING_UPGRADE5 |
			 DWELLING_UPGRADE6 |
			 DWELLING_UPGRADE7) & build ?
			 std::string("The " + GetStringBuilding(build, race) + " produces " + Monster::String(Monster::Monster(race, build)) + ".") :
			 GetDescriptionBuilding(build, race);
	
	u8 height_description = Text::height(BOXAREA_WIDTH, building_description, Font::BIG);

	std::vector<building_t> requires;
	Castle::GetBuildingRequires(race, build, requires);
	
	std::string string_requires;

	if(requires.size())
	{
		std::vector<building_t>::const_iterator it1 = requires.begin();
		std::vector<building_t>::const_iterator it2 = requires.end();

		while(it1 != it2)
		{
			if(! (building & *it1))
			{
				string_requires += GetStringBuilding(*it1, race);

				if(it1 + 1 < it2) string_requires += ", ";
			}
		
			++it1;
		}
	}
	
	u8 height_requires = string_requires.empty() ? 0 : Text::height(BOXAREA_WIDTH, string_requires, Font::BIG);


	PaymentConditions::BuyBuilding paymentBuild(race, build);
	const u8 & valid_resource = paymentBuild.GetValidItems();
    
	Dialog::Box box(60 + 30 + height_description + 15 + (height_requires ? 30 + height_requires : 0) + (7 == valid_resource ? 150 : (3 < valid_resource ? 100 : 50)), buttons);

    const Rect & box_rt = box.GetArea();

    std::string str;

    switch(race)
    {
        case Race::KNGT: str = "CSTLKNGT.ICN"; break;
        case Race::BARB: str = "CSTLBARB.ICN"; break;
        case Race::SORC: str = "CSTLSORC.ICN"; break;
        case Race::WRLK: str = "CSTLWRLK.ICN"; break;
        case Race::WZRD: str = "CSTLWZRD.ICN"; break;
        case Race::NECR: str = "CSTLNECR.ICN"; break;
        default: return Dialog::CANCEL;
    }

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Button *button1 = NULL;
    Button *button2 = NULL;

    Point dst_pt;
    if(buttons)
    {
		dst_pt.x = box_rt.x;
        dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 1).h();
        button1 = new Button(dst_pt, system, 1, 2);
        dst_pt.x = box_rt.x + box_rt.w - AGG::GetICN(system, 3).w();
        dst_pt.y = box_rt.y + box_rt.h + BUTTON_HEIGHT - AGG::GetICN(system, 3).h();
        button2 = new Button(dst_pt, system, 3, 4);
    }

	u8 index = 0;
	
	// sprite
	switch(build)
	{
		case BUILD_MAGEGUILD1:
		case BUILD_MAGEGUILD2:
		case BUILD_MAGEGUILD3:
		case BUILD_MAGEGUILD4:
		case BUILD_MAGEGUILD5:	index = 0; break;
		case BUILD_THIEVESGUILD:index = 1; break;
		case BUILD_TAVERN:		index = 2; break;
		case BUILD_SHIPYARD:	index = 3; break;
		case BUILD_WELL:		index = 4; break;
		case BUILD_TENT:		index = 5; break;
		case BUILD_CASTLE:		index = 6; break;
		case BUILD_STATUE:		index = 7; break;
		case BUILD_LEFTTURRET:	index = 8; break;
		case BUILD_RIGHTTURRET:	index = 9; break;
		case BUILD_MARKETPLACE:	index = 10; break;
		case BUILD_WEL2:		index = 11; break;
		case BUILD_MOAT:		index = 12; break;
		case BUILD_SPEC:		index = 13; break;
		case BUILD_CAPTAIN:		index = 15; break;
		case DWELLING_MONSTER1: index = 19; break;
		case DWELLING_MONSTER2: index = 20; break;
		case DWELLING_MONSTER3: index = 21; break;
		case DWELLING_MONSTER4: index = 22; break;
		case DWELLING_MONSTER5: index = 23; break;
		case DWELLING_MONSTER6: index = 24; break;
		case DWELLING_UPGRADE2: index = 25; break;
		case DWELLING_UPGRADE3: index = 26; break;
		case DWELLING_UPGRADE4: index = 27; break;
		case DWELLING_UPGRADE5: index = 28; break;
		case DWELLING_UPGRADE6: index = 29; break;
		case DWELLING_UPGRADE7: index = 30; break;
		default: return Dialog::CANCEL;
	}

    const Sprite & window_icons = AGG::GetICN("CASLWIND.ICN", 0);
    Rect src_rt(5, 2, 137, 72);
	dst_pt.x = box_rt.x + (box_rt.w - src_rt.w) / 2;
	dst_pt.y = box_rt.y + 12;
	display.Blit(window_icons, src_rt, dst_pt);

	const Sprite & building_icons = AGG::GetICN(str, index);
	dst_pt.x = box_rt.x + (box_rt.w - building_icons.w()) / 2;
	dst_pt.y = box_rt.y + 13;
	display.Blit(building_icons, dst_pt);

    const std::string & building_name = GetStringBuilding(build, race);
	dst_pt.x = box_rt.x + (box_rt.w - Text::width(building_name, Font::SMALL)) / 2;
	dst_pt.y = box_rt.y + 70;
    Text(dst_pt.x, dst_pt.y, building_name, Font::SMALL, true);

	src_rt.x = box_rt.x;
	src_rt.y = box_rt.y + 100;
	src_rt.w = BOXAREA_WIDTH;
	src_rt.h = 200;
    TextBox(src_rt, building_description, Font::BIG, true);

	if(height_requires)
	{
		str = "Requires:";
		dst_pt.x = box_rt.x + (box_rt.w - Text::width(str, Font::BIG)) / 2;
		dst_pt.y = box_rt.y + 100 + height_description + 20;
		Text(dst_pt.x, dst_pt.y, str, Font::BIG, true);

		src_rt.x = box_rt.x;
		src_rt.y = box_rt.y + 100 + height_description + 35;
		src_rt.w = BOXAREA_WIDTH;
		src_rt.h = 200;
    	TextBox(src_rt, string_requires, Font::BIG, true);
	}

	index = 2 < valid_resource ? box_rt.w / 3 : box_rt.w / valid_resource;
	src_rt.y = height_requires ? box_rt.y + 100 + height_description + 35 + height_requires + 45 : box_rt.y + 100 + height_description + 50;
	u8 count = 0;
	
	if(paymentBuild.wood)
	{
		const Sprite & sprite = AGG::GetICN("RESOURCE.ICN", 0);
		dst_pt.x = box_rt.x + index / 2 + count * index - sprite.w() / 2;
		dst_pt.y = src_rt.y - sprite.h();
		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, paymentBuild.wood);
		dst_pt.x = box_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
		dst_pt.y = src_rt.y + 2;
		Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

		++count;
	}
	
	if(paymentBuild.ore)
	{
		const Sprite & sprite = AGG::GetICN("RESOURCE.ICN", 2);
		dst_pt.x = box_rt.x + index / 2 + count * index - sprite.w() / 2;
		dst_pt.y = src_rt.y - sprite.h();
		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, paymentBuild.ore);
		dst_pt.x = box_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
		dst_pt.y = src_rt.y + 2;
		Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

		++count;
	}

	if(paymentBuild.mercury)
	{
		const Sprite & sprite = AGG::GetICN("RESOURCE.ICN", 1);
		dst_pt.x = box_rt.x + index / 2 + count * index - sprite.w() / 2;
		dst_pt.y = src_rt.y - sprite.h();
		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, paymentBuild.mercury);
		dst_pt.x = box_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
		dst_pt.y = src_rt.y + 2;
		Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

		++count;
	}
    
	if(2 < count){ count = 0; src_rt.y += 50; }
	if(paymentBuild.sulfur)
	{
		const Sprite & sprite = AGG::GetICN("RESOURCE.ICN", 3);
		dst_pt.x = box_rt.x + index / 2 + count * index - sprite.w() / 2;
		dst_pt.y = src_rt.y - sprite.h();
		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, paymentBuild.sulfur);
		dst_pt.x = box_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
		dst_pt.y = src_rt.y + 2;
		Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

		++count;
	}
	
	if(2 < count){ count = 0; src_rt.y += 50; }
	if(paymentBuild.crystal)
	{
		const Sprite & sprite = AGG::GetICN("RESOURCE.ICN", 4);
		dst_pt.x = box_rt.x + index / 2 + count * index - sprite.w() / 2;
		dst_pt.y = src_rt.y - sprite.h();
		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, paymentBuild.crystal);
		dst_pt.x = box_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
		dst_pt.y = src_rt.y + 2;
		Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

		++count;
	}
	
	if(2 < count){ count = 0; src_rt.y += 50; }
	if(paymentBuild.gems)
	{
		const Sprite & sprite = AGG::GetICN("RESOURCE.ICN", 5);
		dst_pt.x = box_rt.x + index / 2 + count * index - sprite.w() / 2;
		dst_pt.y = src_rt.y - sprite.h();
		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, paymentBuild.gems);
		dst_pt.x = box_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
		dst_pt.y = src_rt.y + 2;
		Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);

		++count;
	}

	if(2 < count){ count = 0; src_rt.y += 50; }
	if(paymentBuild.gold)
	{
		const Sprite & sprite = AGG::GetICN("RESOURCE.ICN", 6);
		if(! count) index = box_rt.w;
		dst_pt.x = box_rt.x + index / 2 + count * index - sprite.w() / 2;
		dst_pt.y = src_rt.y - sprite.h();
		display.Blit(sprite, dst_pt);

		str.clear();
		String::AddInt(str, paymentBuild.gold);
		dst_pt.x = box_rt.x + index / 2 + count * index - Text::width(str, Font::SMALL) / 2;
		dst_pt.y = src_rt.y + 2;
		Text(dst_pt.x, dst_pt.y, str, Font::SMALL, true);
	}
	
	display.Flip();
    Cursor::Show();

    le.ResetKey();

    // message loop
    bool exit = false;
    Dialog::answer_t result = Dialog::ZERO;

    while(!exit)
    {
        le.HandleEvents();

        if(!buttons && !le.MouseRight()) exit = true;

        if(button1) le.MousePressLeft(*button1) ? button1->Press() : button1->Release();
        if(button2) le.MousePressLeft(*button2) ? button2->Press() : button2->Release();

        if(button1 && le.MouseClickLeft(*button1)){ exit = true; result = Dialog::OK; }
        if(button2 && le.MouseClickLeft(*button2)){ exit = true; result = Dialog::CANCEL; }

        if(le.KeyPress(SDLK_RETURN)){ exit = true; result = Dialog::OK; }

        if(le.KeyPress(SDLK_ESCAPE)){ exit = true; result = Dialog::CANCEL; }
    }
    
    le.ResetKey();

    Cursor::Hide();

    if(button1) delete button1;
    if(button2) delete button2;

    Cursor::Show();

    return result;
}

void RedrawInfoDwelling(const Point & pt, const Castle & castle, const Castle::building_t & build)
{
    std::string icnsprite;

    switch(castle.GetRace())
    {
        case Race::BARB: icnsprite = "CSTLBARB.ICN"; break;
        case Race::KNGT: icnsprite = "CSTLKNGT.ICN"; break;
        case Race::NECR: icnsprite = "CSTLNECR.ICN"; break;
        case Race::SORC: icnsprite = "CSTLSORC.ICN"; break;
        case Race::WRLK: icnsprite = "CSTLWRLK.ICN"; break;
        case Race::WZRD: icnsprite = "CSTLWZRD.ICN"; break;
	default: return;
    }

    const Sprite & sprite_allow = AGG::GetICN("TOWNWIND.ICN", 11);
    const Sprite & sprite_deny  = AGG::GetICN("TOWNWIND.ICN", 12);
    const Sprite & sprite_money = AGG::GetICN("TOWNWIND.ICN", 13);
    
    Point dst_pt(pt);

    bool allowBuyBuilding = castle.AllowBuyBuilding(build);

    PaymentConditions::BuyBuilding paymentBuild(castle.GetRace(), build);
                
    // indicator
    dst_pt.x = pt.x + 115;
    dst_pt.y = pt.y + 40;
    if(castle.isBuild(build)) display.Blit(sprite_allow, dst_pt);
    else
    if(1 == paymentBuild.GetValidItems() && paymentBuild.gold) display.Blit(sprite_money, dst_pt);
    else
    if(! allowBuyBuilding) display.Blit(sprite_deny, dst_pt);

    // status bar
    if(!castle.isBuild(build))
    {
	dst_pt.x = pt.x - 1;
	dst_pt.y = pt.y + 57;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", allowBuyBuilding ? 1 : 2), dst_pt);
    }

    // name
    const std::string & stringBuilding = Castle::GetStringBuilding(build, castle.GetRace());
    dst_pt.x = pt.x + 68 - Text::width(stringBuilding, Font::SMALL) / 2;
    dst_pt.y = pt.y + 58;
    Text(dst_pt.x, dst_pt.y, stringBuilding, Font::SMALL, true);

}

u32 Castle::OpenTown(void)
{
    // cursor
    Cursor::Hide();

    Dialog::FrameBorder background;

    const Point & cur_pt(background.GetArea());
    Point dst_pt(cur_pt);

    display.Blit(AGG::GetICN("CASLWIND.ICN", 0), dst_pt);

    // hide captain options
    if(! (building & BUILD_CAPTAIN))
    {
	dst_pt.x = 530;
	dst_pt.y = 163;
	const Rect rect(dst_pt, 110, 84);
	dst_pt.x += cur_pt.x;
	dst_pt.y += cur_pt.y;
		
	display.Blit(AGG::GetICN("STONEBAK.ICN", 0), rect, dst_pt);
    }

    // draw castle sprite
    dst_pt.x = cur_pt.x + 460;
    dst_pt.y = cur_pt.y + 0;
    DrawImageCastle(dst_pt);

    //
    std::string message;
    
    switch(race)
    {
        case Race::BARB: message = "CSTLBARB.ICN"; break;
        case Race::KNGT: message = "CSTLKNGT.ICN"; break;
        case Race::NECR: message = "CSTLNECR.ICN"; break;
        case Race::SORC: message = "CSTLSORC.ICN"; break;
        case Race::WRLK: message = "CSTLWRLK.ICN"; break;
        case Race::WZRD: message = "CSTLWZRD.ICN"; break;
	default: return 0;
    }

    u8 index = 0;

    // dwelling 1
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 3;
    display.Blit(AGG::GetICN(message, 19), dst_pt);
    const Rect rectDwelling1(dst_pt, 135, 57);
    const std::string & stringDwelling1 = GetStringBuilding(DWELLING_MONSTER1, race);
    bool allowBuyBuildDwelling1 = AllowBuyBuilding(DWELLING_MONSTER1);
    RedrawInfoDwelling(dst_pt, *this, DWELLING_MONSTER1);

    // dwelling 2
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 3;
    bool allowUpgrade2 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER2)) && building & DWELLING_MONSTER2;
    index = allowUpgrade2  ? 25 : 20;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling2(dst_pt, 135, 57);
    bool allowBuyBuildDwelling2 = (allowUpgrade2 && (DWELLING_MONSTER2 & building) ? AllowBuyBuilding(DWELLING_UPGRADE2) : AllowBuyBuilding(DWELLING_MONSTER2));
    const std::string & stringDwelling2 = GetStringBuilding(allowUpgrade2 && (DWELLING_MONSTER2 & building) ? DWELLING_UPGRADE2 : DWELLING_MONSTER2, race);
    // status bar
    if((allowUpgrade2 && !(DWELLING_UPGRADE2 & building)) ||
       (!(DWELLING_MONSTER2 & building)))
    {
		dst_pt.x = cur_pt.x + 149;
		dst_pt.y = cur_pt.y + 60;
		display.Blit(AGG::GetICN("CASLXTRA.ICN", allowBuyBuildDwelling2 ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 216 - Text::width(stringDwelling2, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 61;
    Text(dst_pt.x, dst_pt.y, stringDwelling2, Font::SMALL, true);

    // dwelling 3
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 3;
    bool allowUpgrade3 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER3)) && building & DWELLING_MONSTER3;
    index = allowUpgrade3 ? 26 : 21;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling3(dst_pt, 135, 57);
    bool allowBuyBuildDwelling3 = (allowUpgrade3 && (DWELLING_MONSTER3 & building) ? AllowBuyBuilding(DWELLING_UPGRADE3) : AllowBuyBuilding(DWELLING_MONSTER3));
    const std::string & stringDwelling3 = GetStringBuilding(allowUpgrade3 && (DWELLING_MONSTER3 & building) ? DWELLING_UPGRADE3 : DWELLING_MONSTER3, race);
    // status bar
    if((allowUpgrade3 && !(DWELLING_UPGRADE3 & building)) ||
       (!(DWELLING_MONSTER3 & building)))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 60;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", allowBuyBuildDwelling3 ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 364 - Text::width(stringDwelling3, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 61;
    Text(dst_pt.x, dst_pt.y, stringDwelling3, Font::SMALL, true);

    // dwelling 4
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 78;
    bool allowUpgrade4 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER4)) && building & DWELLING_MONSTER4;
    index = allowUpgrade4 ? 27 : 22;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling4(dst_pt, 135, 57);
    bool allowBuyBuildDwelling4 = (allowUpgrade4 && (DWELLING_MONSTER4 & building) ? AllowBuyBuilding(DWELLING_UPGRADE4) : AllowBuyBuilding(DWELLING_MONSTER4));
    const std::string & stringDwelling4 = GetStringBuilding(allowUpgrade4 && (DWELLING_MONSTER4 & building) ? DWELLING_UPGRADE4 : DWELLING_MONSTER4, race);
    // status bar
    if((allowUpgrade4 && !(DWELLING_UPGRADE4 & building)) ||
       (!(DWELLING_MONSTER4 & building)))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", allowBuyBuildDwelling4 ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 74 - Text::width(stringDwelling4, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 136;
    Text(dst_pt.x, dst_pt.y, stringDwelling4, Font::SMALL, true);

    // dwelling 5
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 78;
    bool allowUpgrade5 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER5)) && building & DWELLING_MONSTER5;
    index = allowUpgrade5 ? 28 : 23;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling5(dst_pt, 135, 57);
    bool allowBuyBuildDwelling5 = (allowUpgrade5 && (DWELLING_MONSTER5 & building) ? AllowBuyBuilding(DWELLING_UPGRADE5) : AllowBuyBuilding(DWELLING_MONSTER5));
    const std::string & stringDwelling5 = GetStringBuilding(allowUpgrade5 && (DWELLING_MONSTER5 & building) ? DWELLING_UPGRADE5 : DWELLING_MONSTER5, race);
    // status bar
    if((allowUpgrade5 && !(DWELLING_UPGRADE5 & building)) ||
       (!(DWELLING_MONSTER5 & building)))
    {
	dst_pt.x = cur_pt.x + 149;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", allowBuyBuildDwelling5 ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 216 - Text::width(stringDwelling5, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 136;
    Text(dst_pt.x, dst_pt.y, stringDwelling5, Font::SMALL, true);

    // dwelling 6
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 78;
    bool allowUpgrade6 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_MONSTER6)) && building & DWELLING_UPGRADE6;
    bool allowUpgrade7 = Monster::AllowUpgrade(Monster::Monster(race, DWELLING_UPGRADE6)) && building & DWELLING_MONSTER6;
    index = allowUpgrade7 ? 30 :
           (allowUpgrade6 ? 29 : 24);
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectDwelling6(dst_pt, 135, 57);
    bool allowBuyBuildDwelling6 = (allowUpgrade7 && (DWELLING_UPGRADE6 & building) ?
	AllowBuyBuilding(DWELLING_UPGRADE7) :
	(allowUpgrade6 && (DWELLING_MONSTER6 & building) ? AllowBuyBuilding(DWELLING_UPGRADE6) :
	AllowBuyBuilding(DWELLING_MONSTER6)));
    const std::string & stringDwelling6 = GetStringBuilding(
	allowUpgrade7 && (DWELLING_UPGRADE6 & building) ? DWELLING_UPGRADE7 :
	(allowUpgrade6 && (DWELLING_MONSTER6 & building) ? DWELLING_UPGRADE6 : DWELLING_MONSTER6), race);
    // status bar
    if((allowUpgrade7 && !(DWELLING_UPGRADE7 & building)) ||
       (allowUpgrade6 && !(DWELLING_UPGRADE6 & building)) ||
       (!(DWELLING_MONSTER6 & building)))
    {
	dst_pt.x = cur_pt.x + 293;
	dst_pt.y = cur_pt.y + 135;
	display.Blit(AGG::GetICN("CASLXTRA.ICN", allowBuyBuildDwelling6 ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 364 - Text::width(stringDwelling6, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 136;
    Text(dst_pt.x, dst_pt.y, stringDwelling6, Font::SMALL, true);

    // mage guild
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 158;
    display.Blit(AGG::GetICN(message, 0), dst_pt);
    const Rect rectMageGuild(dst_pt, 135, 57);
    std::string stringMageGuild = GetStringBuilding(BUILD_MAGEGUILD1, race);
    bool allowBuyBuildMageGuild = false;
    building_t nextLevelMageGuild = BUILD_MAGEGUILD1;
    // status bar
    if(!(BUILD_MAGEGUILD5 & building))
    {
	dst_pt.x = cur_pt.x + 5;
	dst_pt.y = cur_pt.y + 215;

	switch(GetLevelMageGuild())
	{
	    case 0:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD1);
		stringMageGuild += ", Level 1";
		nextLevelMageGuild = BUILD_MAGEGUILD1;
		break;
	    case 1:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD2);
		stringMageGuild += ", Level 2";
		nextLevelMageGuild = BUILD_MAGEGUILD2;
		break;
	    case 2:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD3);
		stringMageGuild += ", Level 3";
		nextLevelMageGuild = BUILD_MAGEGUILD3;
		break;
	    case 3:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD4);
		stringMageGuild += ", Level 4";
		nextLevelMageGuild = BUILD_MAGEGUILD4;
		break;
	    case 4:
		allowBuyBuildMageGuild = AllowBuyBuilding(BUILD_MAGEGUILD5);
		stringMageGuild += ", Level 5";
		nextLevelMageGuild = BUILD_MAGEGUILD5;
		break;
	    default: break;
	}

	display.Blit(AGG::GetICN("CASLXTRA.ICN", allowBuyBuildMageGuild ? 1 : 2), dst_pt);
    }
    // name
    dst_pt.x = cur_pt.x + 74 - Text::width(stringMageGuild, Font::SMALL) / 2;
    dst_pt.y = cur_pt.y + 216;
    Text(dst_pt.x, dst_pt.y, stringMageGuild, Font::SMALL, true);

    // tavern
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 158;
    const Rect rectTavern(dst_pt, 135, 57);
    bool allowBuyBuildTavern = AllowBuyBuilding(BUILD_TAVERN);
    const std::string & stringTavern = GetStringBuilding(BUILD_TAVERN, race);
    if(Race::NECR == race)
	display.FillRect(0, 0, 0, Rect(dst_pt, 135, 57));
    else
    {
    	display.Blit(AGG::GetICN(message, 2), dst_pt);
	RedrawInfoDwelling(dst_pt, *this, BUILD_TAVERN);
    }

    // thieves guild
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 158;
    display.Blit(AGG::GetICN(message, 1), dst_pt);
    const Rect rectThievesGuild(dst_pt, 135, 57);
    bool allowBuyBuildThievesGuild = AllowBuyBuilding(BUILD_THIEVESGUILD);
    const std::string & stringThievesGuild = GetStringBuilding(BUILD_THIEVESGUILD, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_THIEVESGUILD);

    // shipyard
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(message, 3), dst_pt);
    const Rect rectShipyard(dst_pt, 135, 57);
    bool allowBuyBuildShipyard = AllowBuyBuilding(BUILD_SHIPYARD);
    const std::string & stringShipyard = GetStringBuilding(BUILD_SHIPYARD, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_SHIPYARD);

    // statue
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(message, 7), dst_pt);
    const Rect rectStatue(dst_pt, 135, 57);
    bool allowBuyBuildStatue = AllowBuyBuilding(BUILD_STATUE);
    const std::string & stringStatue = GetStringBuilding(BUILD_STATUE, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_STATUE);

    // marketplace
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 233;
    display.Blit(AGG::GetICN(message, 10), dst_pt);
    const Rect rectMarketplace(dst_pt, 135, 57);
    bool allowBuyBuildMarketplace = AllowBuyBuilding(BUILD_MARKETPLACE);
    const std::string & stringMarketplace = GetStringBuilding(BUILD_MARKETPLACE, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_MARKETPLACE);

    // well
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(message, 4), dst_pt);
    const Rect rectWell(dst_pt, 135, 57);
    bool allowBuyBuildWell = AllowBuyBuilding(BUILD_WELL);
    const std::string & stringWell = GetStringBuilding(BUILD_WELL, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_WELL);

    // wel2
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(message, 11), dst_pt);
    const Rect rectWel2(dst_pt, 135, 57);
    bool allowBuyBuildWel2 = AllowBuyBuilding(BUILD_WEL2);
    const std::string & stringWel2 = GetStringBuilding(BUILD_WEL2, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_WEL2);

    // spec
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 308;
    display.Blit(AGG::GetICN(message, 13), dst_pt);
    const Rect rectSpec(dst_pt, 135, 57);
    bool allowBuyBuildSpec = AllowBuyBuilding(BUILD_SPEC);
    const std::string & stringSpec = GetStringBuilding(BUILD_SPEC, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_SPEC);

    // left turret
    dst_pt.x = cur_pt.x + 6;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(message, 8), dst_pt);
    const Rect rectLTurret(dst_pt, 135, 57);
    bool allowBuyBuildLTurret = AllowBuyBuilding(BUILD_LEFTTURRET);
    const std::string & stringLTurret = GetStringBuilding(BUILD_LEFTTURRET, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_LEFTTURRET);

    // right turret
    dst_pt.x = cur_pt.x + 150;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(message, 9), dst_pt);
    const Rect rectRTurret(dst_pt, 135, 57);
    bool allowBuyBuildRTurret = AllowBuyBuilding(BUILD_RIGHTTURRET);
    const std::string & stringRTurret = GetStringBuilding(BUILD_RIGHTTURRET, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_RIGHTTURRET);

    // moat
    dst_pt.x = cur_pt.x + 294;
    dst_pt.y = cur_pt.y + 388;
    display.Blit(AGG::GetICN(message, 12), dst_pt);
    const Rect rectMoat(dst_pt, 135, 57);
    bool allowBuyBuildMoat = AllowBuyBuilding(BUILD_MOAT);
    const std::string & stringMoat = GetStringBuilding(BUILD_MOAT, race);
    RedrawInfoDwelling(dst_pt, *this, BUILD_MOAT);

    // captain
    switch(race)
    {
        case Race::BARB: message = "CSTLCAPB.ICN"; break;
        case Race::KNGT: message = "CSTLCAPK.ICN"; break;
        case Race::NECR: message = "CSTLCAPN.ICN"; break;
        case Race::SORC: message = "CSTLCAPS.ICN"; break;
        case Race::WRLK: message = "CSTLCAPW.ICN"; break;
        case Race::WZRD: message = "CSTLCAPZ.ICN"; break;
	default: return 0;
    }
    dst_pt.x = cur_pt.x + 444;
    dst_pt.y = cur_pt.y + 165;
    index = building & BUILD_CAPTAIN ? 1 : 0;
    display.Blit(AGG::GetICN(message, index), dst_pt);
    const Rect rectCaptain(dst_pt, AGG::GetICN(message, index).w(), AGG::GetICN(message, index).h());
    bool allowBuyBuildCaptain = AllowBuyBuilding(BUILD_CAPTAIN);
    const std::string & stringCaptain = GetStringBuilding(BUILD_CAPTAIN, race);

    // first hero
    const Heroes * hero1 = world.GetFreemanHeroes(race);
    dst_pt.x = cur_pt.x + 443;
    dst_pt.y = cur_pt.y + 260;
    const Rect rectHero1(dst_pt, 102, 93);
    if(hero1)
    {
	Heroes::heroes_t hero1_name = (*hero1).GetHeroes();
	message = 10 > hero1_name ? "PORT000" : "PORT00";
	String::AddInt(message, hero1_name);
	message += ".ICN";
	const Sprite & hero1_sprite = AGG::GetICN(message, 0);
	display.Blit(hero1_sprite, dst_pt);
    }
    else
	display.FillRect(0, 0, 0, rectHero1);

    // second hero
    const Heroes * hero2 = world.GetFreemanHeroes();
    dst_pt.x = cur_pt.x + 443;
    dst_pt.y = cur_pt.y + 362;
    const Rect rectHero2(dst_pt, 102, 94);
    if(hero2)
    {
	Heroes::heroes_t hero2_name = (*hero2).GetHeroes();
	message = 10 > hero2_name ? "PORT000" : "PORT00";
	String::AddInt(message, hero2_name);
	message += ".ICN";
	const Sprite & hero2_sprite = AGG::GetICN(message, 0);
	display.Blit(hero2_sprite, dst_pt);
    }
    else
	display.FillRect(0, 0, 0, rectHero2);

    // bottom bar
    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 461;
    Dialog::StatusBar statusBar(dst_pt, AGG::GetICN("CASLBAR.ICN", 0), Font::BIG);
    statusBar.Clear("Castle Options");

    // redraw resource panel
    RedrawResourcePanel();

    // button exit
    dst_pt.x = cur_pt.x + 554;
    dst_pt.y = cur_pt.y + 428;
    Button buttonExit(dst_pt, "SWAPBTN.ICN", 0, 1);

    display.Flip();
    Cursor::Show();

    LocalEvent & le = LocalEvent::GetLocalEvent();
   
    le.ResetKey();

    // message loop
    bool exit = false;

    while(!exit)
    {
        le.HandleEvents();

        le.MousePressLeft(buttonExit) ? buttonExit.Press() : buttonExit.Release();

        if(le.MouseClickLeft(buttonExit) || le.KeyPress(SDLK_RETURN) || le.KeyPress(SDLK_ESCAPE)){ exit = true; }

	// click left
	if(le.MouseClickLeft(rectDwelling1) && allowBuyBuildDwelling1 &&
		Dialog::OK == DialogBuyBuilding(DWELLING_MONSTER1, true));
	else
	if(le.MouseClickLeft(rectDwelling2) && allowBuyBuildDwelling2 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2, true))
	{
		BuyBuilding(allowUpgrade2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2);
		return allowUpgrade2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2;
	}
	else
	if(le.MouseClickLeft(rectDwelling3) && allowBuyBuildDwelling3 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3, true))
	{
		BuyBuilding(allowUpgrade3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3);
		return allowUpgrade3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3;
	}
	else
	if(le.MouseClickLeft(rectDwelling4) && allowBuyBuildDwelling4 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4, true))
	{
		BuyBuilding(allowUpgrade4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4);
		return allowUpgrade4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4;
	}
	else
	if(le.MouseClickLeft(rectDwelling5) && allowBuyBuildDwelling5 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5, true))
	{
		BuyBuilding(allowUpgrade5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5);
		return allowUpgrade5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5;
	}
	else
	if(le.MouseClickLeft(rectDwelling6) && allowBuyBuildDwelling6 &&
		Dialog::OK == DialogBuyBuilding(allowUpgrade7 ? DWELLING_UPGRADE7 : (allowUpgrade6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6), true))
	{
		BuyBuilding(allowUpgrade7 ? DWELLING_UPGRADE7 : (allowUpgrade6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6));
		return allowUpgrade7 ? DWELLING_UPGRADE7 : (allowUpgrade6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6);
	}
	else
	if(le.MouseClickLeft(rectMageGuild) && allowBuyBuildMageGuild && Dialog::OK == DialogBuyBuilding(nextLevelMageGuild, true))
	{
		BuyBuilding(nextLevelMageGuild);
		return nextLevelMageGuild;
	}
	else
	if(Race::NECR != race && le.MouseClickLeft(rectTavern) && allowBuyBuildTavern && Dialog::OK == DialogBuyBuilding(BUILD_TAVERN, true))
	{
		BuyBuilding(BUILD_TAVERN);
		return BUILD_TAVERN;
	}
	else
	if(le.MouseClickLeft(rectThievesGuild) && allowBuyBuildThievesGuild && Dialog::OK == DialogBuyBuilding(BUILD_THIEVESGUILD, true))
	{
		BuyBuilding(BUILD_THIEVESGUILD);
		return BUILD_THIEVESGUILD;
	}
	else
	if(le.MouseClickLeft(rectShipyard) && allowBuyBuildShipyard && Dialog::OK == DialogBuyBuilding(BUILD_SHIPYARD, true))
	{
		BuyBuilding(BUILD_SHIPYARD);
		return BUILD_SHIPYARD;
	}
	else
	if(le.MouseClickLeft(rectStatue) && allowBuyBuildStatue && Dialog::OK == DialogBuyBuilding(BUILD_STATUE, true))
	{
		BuyBuilding(BUILD_STATUE);
		return BUILD_STATUE;
	}
	else
	if(le.MouseClickLeft(rectMarketplace) && allowBuyBuildMarketplace && Dialog::OK == DialogBuyBuilding(BUILD_MARKETPLACE, true))
	{
		BuyBuilding(BUILD_MARKETPLACE);
		return BUILD_MARKETPLACE;
	}
	else
	if(le.MouseClickLeft(rectWell) && allowBuyBuildWell && Dialog::OK == DialogBuyBuilding(BUILD_WELL, true))
	{
		BuyBuilding(BUILD_WELL);
		return BUILD_WELL;
	}
	else
	if(le.MouseClickLeft(rectWel2) && allowBuyBuildWel2 && Dialog::OK == DialogBuyBuilding(BUILD_WEL2, true))
	{
		BuyBuilding(BUILD_WEL2);
		return BUILD_WEL2;
	}
	else
	if(le.MouseClickLeft(rectSpec) && allowBuyBuildSpec && Dialog::OK == DialogBuyBuilding(BUILD_SPEC, true))
	{
		BuyBuilding(BUILD_SPEC);
		return BUILD_SPEC;
	}
	else
	if(le.MouseClickLeft(rectLTurret) && allowBuyBuildLTurret && Dialog::OK == DialogBuyBuilding(BUILD_LEFTTURRET, true))
	{
		BuyBuilding(BUILD_LEFTTURRET);
		return BUILD_LEFTTURRET;
	}
	else
	if(le.MouseClickLeft(rectRTurret) && allowBuyBuildRTurret && Dialog::OK == DialogBuyBuilding(BUILD_RIGHTTURRET, true))
	{
		BuyBuilding(BUILD_RIGHTTURRET);
		return BUILD_RIGHTTURRET;
	}
	else
	if(le.MouseClickLeft(rectMoat) && allowBuyBuildMoat && Dialog::OK == DialogBuyBuilding(BUILD_MOAT, true))
	{
		BuyBuilding(BUILD_MOAT);
		return BUILD_MOAT;
	}
	else
	if(le.MouseClickLeft(rectCaptain) && allowBuyBuildCaptain && Dialog::OK == DialogBuyBuilding(BUILD_CAPTAIN, true))
	{
		BuyBuilding(BUILD_CAPTAIN);
		return BUILD_CAPTAIN;
	}

	// press right
	if(le.MousePressRight(rectDwelling1)) DialogBuyBuilding(DWELLING_MONSTER1, false);
	else
	if(le.MousePressRight(rectDwelling2)) DialogBuyBuilding(allowUpgrade2 ? DWELLING_UPGRADE2 : DWELLING_MONSTER2, false);
	else
	if(le.MousePressRight(rectDwelling3)) DialogBuyBuilding(allowUpgrade3 ? DWELLING_UPGRADE3 : DWELLING_MONSTER3, false);
	else
	if(le.MousePressRight(rectDwelling4)) DialogBuyBuilding(allowUpgrade4 ? DWELLING_UPGRADE4 : DWELLING_MONSTER4, false);
	else
	if(le.MousePressRight(rectDwelling5)) DialogBuyBuilding(allowUpgrade5 ? DWELLING_UPGRADE5 : DWELLING_MONSTER5, false);
	else
	if(le.MousePressRight(rectDwelling6)) DialogBuyBuilding(allowUpgrade7 ? DWELLING_UPGRADE7 : (allowUpgrade6 ? DWELLING_UPGRADE6 : DWELLING_MONSTER6), false);
	else
	if(le.MousePressRight(rectMageGuild)) DialogBuyBuilding(nextLevelMageGuild, false);
	else
	if(Race::NECR != race && le.MousePressRight(rectTavern)) DialogBuyBuilding(BUILD_TAVERN, false);
	else
	if(le.MousePressRight(rectThievesGuild)) DialogBuyBuilding(BUILD_THIEVESGUILD, false);
	else
	if(le.MousePressRight(rectShipyard)) DialogBuyBuilding(BUILD_SHIPYARD, false);
	else
	if(le.MousePressRight(rectStatue)) DialogBuyBuilding(BUILD_STATUE, false);
	else
	if(le.MousePressRight(rectMarketplace)) DialogBuyBuilding(BUILD_MARKETPLACE, false);
	else
	if(le.MousePressRight(rectWell)) DialogBuyBuilding(BUILD_WELL, false);
	else
	if(le.MousePressRight(rectWel2)) DialogBuyBuilding(BUILD_WEL2, false);
	else
	if(le.MousePressRight(rectSpec)) DialogBuyBuilding(BUILD_SPEC, false);
	else
	if(le.MousePressRight(rectLTurret)) DialogBuyBuilding(BUILD_LEFTTURRET, false);
	else
	if(le.MousePressRight(rectRTurret)) DialogBuyBuilding(BUILD_RIGHTTURRET, false);
	else
	if(le.MousePressRight(rectMoat)) DialogBuyBuilding(BUILD_MOAT, false);
	else
	if(le.MousePressRight(rectCaptain)) DialogBuyBuilding(BUILD_CAPTAIN, false);


        // status info
	if(le.MouseCursor(rectDwelling1))
	{
	    if(DWELLING_MONSTER1 & building)
			statusBar.ShowMessage(stringDwelling1 + " is already built");
	    else
	    if(allowBuyBuildDwelling1)
			statusBar.ShowMessage("Build " + stringDwelling1);
	    else
			statusBar.ShowMessage("Cannot build " + stringDwelling1);
	}
	else
	if(le.MouseCursor(rectDwelling2))
	{
	    if((allowUpgrade2 && (DWELLING_UPGRADE2 & building)) ||
	       (!allowUpgrade2 && (DWELLING_MONSTER2 & building)))
			statusBar.ShowMessage(stringDwelling2 + " is already built");
	    else
	    if(allowBuyBuildDwelling2)
			statusBar.ShowMessage("Build " + stringDwelling2);
	    else
			statusBar.ShowMessage("Cannot build " + stringDwelling2);
	}
	else
	if(le.MouseCursor(rectDwelling3))
	{
	    if((allowUpgrade3 && (DWELLING_UPGRADE3 & building)) ||
	       (!allowUpgrade3 && (DWELLING_MONSTER3 & building)))
			statusBar.ShowMessage(stringDwelling3 + " is already built");
	    else
	    if(allowBuyBuildDwelling3)
			statusBar.ShowMessage("Build " + stringDwelling3);
	    else
			statusBar.ShowMessage("Cannot build " + stringDwelling3);
	}
	else
	if(le.MouseCursor(rectDwelling4))
	{
	    if((allowUpgrade4 && (DWELLING_UPGRADE4 & building)) ||
	       (!allowUpgrade4 && (DWELLING_MONSTER4 & building)))
			statusBar.ShowMessage(stringDwelling4 + " is already built");
	    else
	    if(allowBuyBuildDwelling4)
			statusBar.ShowMessage("Build " + stringDwelling4);
	    else
			statusBar.ShowMessage("Cannot build " + stringDwelling4);
	}
	else
	if(le.MouseCursor(rectDwelling5))
	{
	    if((allowUpgrade5 && (DWELLING_UPGRADE5 & building)) ||
	       (!allowUpgrade5 && (DWELLING_MONSTER5 & building)))
			statusBar.ShowMessage(stringDwelling5 + " is already built");
	    else
	    if(allowBuyBuildDwelling5)
			statusBar.ShowMessage("Build " + stringDwelling5);
	    else
			statusBar.ShowMessage("Cannot build " + stringDwelling5);
	}
	else
	if(le.MouseCursor(rectDwelling6))
	{
	    if((allowUpgrade7 && (DWELLING_UPGRADE7 & building)) ||
	       (!allowUpgrade7 && allowUpgrade6 && (DWELLING_UPGRADE6 & building)) ||
	       (!allowUpgrade6 && (DWELLING_MONSTER6 & building)))
			statusBar.ShowMessage(stringDwelling6 + " is already built");
	    else
	    if(allowBuyBuildDwelling6)
			statusBar.ShowMessage("Build " + stringDwelling6);
	    else
			statusBar.ShowMessage("Cannot build " + stringDwelling6);
	}
	else
	if(le.MouseCursor(rectMageGuild))
	{
	    if(BUILD_MAGEGUILD5 & building)
			statusBar.ShowMessage(stringMageGuild + " is already built");
	    else
	    if(allowBuyBuildMageGuild)
			statusBar.ShowMessage("Build " + stringMageGuild);
	    else
			statusBar.ShowMessage("Cannot build " + stringMageGuild);
	}
	else
	if(Race::NECR != race && le.MouseCursor(rectTavern))
	{
	    if(BUILD_TAVERN & building)
			statusBar.ShowMessage(stringTavern + " is already built");
	    else
	    if(allowBuyBuildTavern)
			statusBar.ShowMessage("Build " + stringTavern);
	    else
			statusBar.ShowMessage("Cannot build " + stringTavern);
	}
	else
	if(le.MouseCursor(rectThievesGuild))
	{
	    if(BUILD_THIEVESGUILD & building)
			statusBar.ShowMessage(stringThievesGuild + " is already built");
	    else
	    if(allowBuyBuildThievesGuild)
			statusBar.ShowMessage("Build " + stringThievesGuild);
	    else
			statusBar.ShowMessage("Cannot build " + stringThievesGuild);
	}
	else
	if(le.MouseCursor(rectShipyard))
	{
	    if(BUILD_SHIPYARD & building)
			statusBar.ShowMessage(stringShipyard + " is already built");
	    else
	    if(allowBuyBuildShipyard)
			statusBar.ShowMessage("Build " + stringShipyard);
	    else
			statusBar.ShowMessage("Cannot build " + stringShipyard);
	}
	else
	if(le.MouseCursor(rectStatue))
	{
	    if(BUILD_STATUE & building)
			statusBar.ShowMessage(stringStatue + " is already built");
	    else
	    if(allowBuyBuildStatue)
			statusBar.ShowMessage("Build " + stringStatue);
	    else
			statusBar.ShowMessage("Cannot build " + stringStatue);
	}
	else
	if(le.MouseCursor(rectMarketplace))
	{
	    if(BUILD_MARKETPLACE & building)
			statusBar.ShowMessage(stringMarketplace + " is already built");
	    else
	    if(allowBuyBuildMarketplace)
			statusBar.ShowMessage("Build " + stringMarketplace);
	    else
			statusBar.ShowMessage("Cannot build " + stringMarketplace);
	}
	else
	if(le.MouseCursor(rectWell))
	{
	    if(BUILD_WELL & building)
			statusBar.ShowMessage(stringWell + " is already built");
	    else
	    if(allowBuyBuildWell)
			statusBar.ShowMessage("Build " + stringWell);
	    else
			statusBar.ShowMessage("Cannot build " + stringWell);
	}
	else
	if(le.MouseCursor(rectWel2))
	{
	    if(BUILD_WEL2 & building)
			statusBar.ShowMessage(stringWel2 + " is already built");
	    else
	    if(allowBuyBuildWel2)
			statusBar.ShowMessage("Build " + stringWel2);
	    else
			statusBar.ShowMessage("Cannot build " + stringWel2);
	}
	else
	if(le.MouseCursor(rectSpec))
	{
	    if(BUILD_SPEC & building)
			statusBar.ShowMessage(stringSpec + " is already built");
	    else
	    if(allowBuyBuildSpec)
			statusBar.ShowMessage("Build " + stringSpec);
	    else
			statusBar.ShowMessage("Cannot build " + stringSpec);
	}
	else
	if(le.MouseCursor(rectLTurret))
	{
	    if(BUILD_LEFTTURRET & building)
			statusBar.ShowMessage(stringLTurret + " is already built");
	    else
	    if(allowBuyBuildLTurret)
			statusBar.ShowMessage("Build " + stringLTurret);
	    else
			statusBar.ShowMessage("Cannot build " + stringLTurret);
	}
	else
	if(le.MouseCursor(rectRTurret))
	{
	    if(BUILD_RIGHTTURRET & building)
			statusBar.ShowMessage(stringRTurret + " is already built");
	    else
	    if(allowBuyBuildRTurret)
			statusBar.ShowMessage("Build " + stringRTurret);
	    else
			statusBar.ShowMessage("Cannot build " + stringRTurret);
	}
	else
	if(le.MouseCursor(rectMoat))
	{
	   	if(BUILD_MOAT & building)
			statusBar.ShowMessage(stringMoat + " is already built");
	   	else
	   	if(allowBuyBuildMoat)
			statusBar.ShowMessage("Build " + stringMoat);
	   	else
			statusBar.ShowMessage("Cannot build " + stringMoat);
	}
	else
	if(le.MouseCursor(rectCaptain))
	{
		if(BUILD_CAPTAIN & building)
			statusBar.ShowMessage(stringCaptain + " is already built");
		else
		if(allowBuyBuildCaptain)
			statusBar.ShowMessage("Build " + stringCaptain);
		else
    			statusBar.ShowMessage("Cannot build " + stringCaptain);
	}
	else
	if(hero1 && le.MouseCursor(rectHero1))
	{
	    statusBar.ShowMessage("Recrut " + (*hero1).GetName() + " the " + Race::String((*hero1).GetRace()));
	}
	else
	if(hero2 && le.MouseCursor(rectHero2))
	{
	    statusBar.ShowMessage("Recrut " + (*hero2).GetName() + " the " + Race::String((*hero2).GetRace()));
	}
	else
	// clear all
	if(! statusBar.isEmpty()) statusBar.Clear("Castle Options");
    }

    le.ResetKey();

    Cursor::Show();

	return 0;
}
