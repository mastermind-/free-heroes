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

#include <vector>
#include "agg.h"
#include "config.h"
#include "sprite.h"
#include "localevent.h"
#include "button.h"
#include "dialog.h"
#include "world.h"
#include "gamearea.h"
#include "cursor.h"
#include "radar.h"
#include "castle.h"
#include "heroes.h"
#include "splitter.h"
#include "game_statuswindow.h"
#include "maps_tiles.h"
#include "ground.h"
#include "error.h"
#include "game_selectfocus.h"
#include "route.h"
#include "game.h"

namespace Game
{
    void OpenCastle(Castle *castle, GameArea & areaMaps, Radar & radar);
    void OpenHeroes(Heroes *heroes, GameArea & areaMaps, Radar & radar);
    void FocusToCastle(Castle *castle, GameArea & areaMaps, Radar & radar);
    void FocusToHeroes(Heroes *hero, GameArea & areaMaps, Radar & radar);
};

Game::menu_t Game::StartGame(void)
{
    // Load maps
    world.LoadMaps(H2Config::GetFileMaps());

    GameArea areaMaps;
    const Rect area_pos(BORDERWIDTH, BORDERWIDTH, GameArea::GetRect().w * TILEWIDTH, GameArea::GetRect().h * TILEWIDTH);

    // cursor
    Cursor::Hide();

    Game::DrawInterface();

    // Create radar
    Radar radar;

    areaMaps.Redraw();
    radar.Redraw();

    // Create radar cursor
    radar.UpdatePosition();

    //
    const std::string &icnscroll = ( H2Config::EvilInterface() ? "SCROLLE.ICN" : "SCROLL.ICN" );
    const std::string &icnbtn = ( H2Config::EvilInterface() ? "ADVEBTNS.ICN" : "ADVBTNS.ICN" );

    Rect areaScrollLeft(0, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    Rect areaScrollRight(display.w() - BORDERWIDTH / 2, BORDERWIDTH / 2, BORDERWIDTH / 2, display.h() - BORDERWIDTH);
    Rect areaScrollTop(BORDERWIDTH / 2, 0, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    Rect areaScrollBottom(BORDERWIDTH / 2, display.h() - BORDERWIDTH / 2, (areaMaps.GetRect().w - 1) * TILEWIDTH, BORDERWIDTH / 2);
    Rect areaLeftPanel(display.w() - 2 * BORDERWIDTH - RADARWIDTH, 0, BORDERWIDTH + RADARWIDTH, display.h());

    Point pt_stw, pt_shu, pt_scu, pt_her, pt_act, pt_cas, pt_mag, pt_end, pt_inf, pt_opt ,pt_set, pt_shd, pt_scd;

    // coord button heroes scroll up
    pt_shu.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
    pt_shu.y = RADARWIDTH + 2 * BORDERWIDTH;

    // coord button castle scroll up
    pt_scu.x = display.w() - RADARWIDTH - BORDERWIDTH + 115 + AGG::GetICN(icnscroll, 0).w();
    pt_scu.y = RADARWIDTH + 2 * BORDERWIDTH;

    u8 icon_count = 0;

    // recalculate buttons coordinate
    switch(H2Config::GetVideoMode()){

        default:
	    // coord status windows
	    pt_stw.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    pt_stw.y = 392;
            // coord button next hero
            pt_her.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_her.y = 320;
            // coord button action
            pt_act.x = pt_her.x + AGG::GetICN(icnbtn, 0).w();
            pt_act.y = 320;
            // coord button castle
            pt_cas.x = pt_act.x + AGG::GetICN(icnbtn, 0).w();
            pt_cas.y = 320;
            // coord button magic
            pt_mag.x = pt_cas.x + AGG::GetICN(icnbtn, 0).w();
            pt_mag.y = 320;
            // coord button end tur
            pt_end.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_end.y = 320 + AGG::GetICN(icnbtn, 0).h();
            // coord button info
            pt_inf.x = pt_end.x + AGG::GetICN(icnbtn, 0).w();
            pt_inf.y = 320 + AGG::GetICN(icnbtn, 0).h();
            // coord button option
            pt_opt.x = pt_inf.x + AGG::GetICN(icnbtn, 0).w();
            pt_opt.y = 320 + AGG::GetICN(icnbtn, 0).h();
            // coord button settings
            pt_set.x = pt_opt.x + AGG::GetICN(icnbtn, 0).w();
            pt_set.y = 320 + AGG::GetICN(icnbtn, 0).h();
	    icon_count = 4;
            // coord button heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
            // coord button castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
	    break;
	
	case Display::MEDIUM:
	    // coord status windows
	    pt_stw.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    pt_stw.y = 488;
            // coord button next hero
            pt_her.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_her.y = 416;
            // coord button action
            pt_act.x = pt_her.x + AGG::GetICN(icnbtn, 0).w();
            pt_act.y = 416;
            // coord button castle
            pt_cas.x = pt_act.x + AGG::GetICN(icnbtn, 0).w();
            pt_cas.y = 416;
            // coord button magic
            pt_mag.x = pt_cas.x + AGG::GetICN(icnbtn, 0).w();
            pt_mag.y = 416;
            // coord button end tur
            pt_end.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_end.y = 416 + AGG::GetICN(icnbtn, 0).h();
            // coord button info
            pt_inf.x = pt_end.x + AGG::GetICN(icnbtn, 0).w();
            pt_inf.y = 416 + AGG::GetICN(icnbtn, 0).h();
            // coord button option
            pt_opt.x = pt_inf.x + AGG::GetICN(icnbtn, 0).w();
            pt_opt.y = 416 + AGG::GetICN(icnbtn, 0).h();
            // coord button settings
            pt_set.x = pt_opt.x + AGG::GetICN(icnbtn, 0).w();
            pt_set.y = 416 + AGG::GetICN(icnbtn, 0).h();
	    icon_count = 7;
            // coord button heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
            // coord button castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
    	    break;

	case Display::LARGE:
	case Display::XLARGE:
	    // coord status windows
	    pt_stw.x = display.w() - RADARWIDTH - BORDERWIDTH;
	    pt_stw.y = 520;
            // coord button next hero
            pt_her.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_her.y = 448;
            // coord button action
            pt_act.x = pt_her.x + AGG::GetICN(icnbtn, 0).w();
            pt_act.y = 448;
            // coord button castle
            pt_cas.x = pt_act.x + AGG::GetICN(icnbtn, 0).w();
            pt_cas.y = 448;
            // coord button magic
            pt_mag.x = pt_cas.x + AGG::GetICN(icnbtn, 0).w();
            pt_mag.y = 448;
            // coord button end tur
            pt_end.x = display.w() - RADARWIDTH - BORDERWIDTH;
            pt_end.y = 448 + AGG::GetICN(icnbtn, 0).h();
            // coord button info
            pt_inf.x = pt_end.x + AGG::GetICN(icnbtn, 0).w();
            pt_inf.y = 448 + AGG::GetICN(icnbtn, 0).h();
            // coord button option
            pt_opt.x = pt_inf.x + AGG::GetICN(icnbtn, 0).w();
            pt_opt.y = 448 + AGG::GetICN(icnbtn, 0).h();
            // coord button settings
            pt_set.x = pt_opt.x + AGG::GetICN(icnbtn, 0).w();
            pt_set.y = 448 + AGG::GetICN(icnbtn, 0).h();
	    icon_count = 8;
            // coord heroes scroll down
            pt_shd.x = display.w() - RADARWIDTH - BORDERWIDTH + 57;
            pt_shd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
            // coord castle scroll down
            pt_scd.x = display.w() - BORDERWIDTH - 15;
            pt_scd.y = RADARWIDTH + 2 * BORDERWIDTH + 32 * icon_count - 15;
	    break;
    }

    Button buttonScrollHeroesUp(pt_shu, icnscroll, 0, 1);
    Button buttonScrollCastleUp(pt_scu, icnscroll, 0, 1);
    Button buttonNextHero(pt_her, icnbtn, 0, 1);
    Button buttonMovement(pt_act, icnbtn, 2, 3);
    Button buttonKingdom(pt_cas, icnbtn, 4, 5);
    Button buttonSpell(pt_mag, icnbtn, 6, 7);
    Button buttonEndTur(pt_end, icnbtn, 8, 9);
    Button buttonAdventure(pt_inf, icnbtn, 10, 11);
    Button buttonFile(pt_opt, icnbtn, 12, 13);
    Button buttonSystem(pt_set, icnbtn, 14, 15);
    Button buttonScrollHeroesDown(pt_shd, icnscroll, 2, 3);
    Button buttonScrollCastleDown(pt_scd, icnscroll, 2, 3);

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();

    Game::SelectFocusCastles & selectCastles = SelectFocusCastles::Get();
    Game::SelectFocusHeroes  & selectHeroes = SelectFocusHeroes::Get();

    selectCastles.SetCastles(myCastles);
    selectCastles.Reset();

    selectHeroes.SetHeroes(myHeroes);
    selectHeroes.Reset();

    const std::vector<Rect> & coordsCastlesIcon = selectCastles.GetCoords();
    const std::vector<Rect> & coordsHeroesIcon = selectHeroes.GetCoords();

    // splitter
    Splitter splitHeroes(AGG::GetICN(icnscroll, 4), Rect(pt_shu.x + 3, pt_shu.y + 18, 10, pt_shd.y - pt_shu.y - 20), Splitter::VERTICAL);
    Splitter splitCastles(AGG::GetICN(icnscroll, 4), Rect(pt_scu.x + 3, pt_scu.y + 18, 10, pt_scd.y - pt_scu.y - 20), Splitter::VERTICAL);

    splitHeroes.SetRange(0, myHeroes.size() > icon_count ? myHeroes.size() - icon_count : 0);
    splitCastles.SetRange(0, myCastles.size() > icon_count ? myCastles.size() - icon_count : 0);

    // focus set to castle
    if(myCastles.size())
    {
	globalfocus.type = Game::CASTLE;
	globalfocus.center = (*myCastles.front()).GetCenter();
	selectCastles.Select(0);
    }
    else
    // focus set to heroes
    if(myHeroes.size())
    {
	globalfocus.type = Game::HEROES;
	globalfocus.center = (*myHeroes.front()).GetCenter();
	selectHeroes.Select(0);
    }
    // unknown focus
    else
    {
	Error::Warning("Game::StartGame: unknown game focus. exiting.");
	return QUITGAME;
    }

    selectCastles.Redraw();
    selectHeroes.Redraw();

    // center to focus
    areaMaps.Center(globalfocus.center);
    radar.UpdatePosition();

    // status window
    Game::StatusWindow statusWindow(pt_stw, myKingdom);
    statusWindow.Redraw();

    LocalEvent & le = LocalEvent::GetLocalEvent();

    Cursor::Show();
    display.Flip();

    u32 ticket = 0;

    // startgame loop
    while(le.HandleEvents())
    {
	// ESC
	if(le.KeyPress(SDLK_ESCAPE) && (Dialog::YES & Dialog::Message("", "Are you sure you want to quit?", Font::BIG, Dialog::YES|Dialog::NO))) return QUITGAME;

	// scroll area maps left
	if(le.MouseCursor(areaScrollLeft))
	{
	    Cursor::Hide();
	    Cursor::Set(Cursor::SCROLL_LEFT);
	    areaMaps.Scroll(GameArea::LEFT);
	    radar.UpdatePosition();
	    Cursor::Show();
	    display.Flip();
	    continue;
	}
	// scroll area maps right
	if(le.MouseCursor(areaScrollRight))
	{
	    Cursor::Hide();
	    Cursor::Set(Cursor::SCROLL_RIGHT);
	    areaMaps.Scroll(GameArea::RIGHT);
	    radar.UpdatePosition();
	    Cursor::Show();
	    display.Flip();
	    continue;
	}
	// scroll area maps top
	if(le.MouseCursor(areaScrollTop))
	{
	    Cursor::Hide();
	    Cursor::Set(Cursor::SCROLL_TOP);
	    areaMaps.Scroll(GameArea::TOP);
	    radar.UpdatePosition();
	    Cursor::Show();
	    display.Flip();
	    continue;
	}
	// scroll area maps bottom
	if(le.MouseCursor(areaScrollBottom)){
	    Cursor::Hide();
	    Cursor::Set(Cursor::SCROLL_BOTTOM);
	    areaMaps.Scroll(GameArea::BOTTOM);
	    radar.UpdatePosition();
	    Cursor::Show();
	    display.Flip();
	    continue;
	}

	// pointer cursor on left panel
	if(le.MouseCursor(areaLeftPanel)){ Cursor::Set(Cursor::POINTER); }
	else
	// cursor over game area
	if(le.MouseCursor(area_pos))
	{
	    const Point & mouse_coord = le.MouseCursor();
	    const u16 index_maps = Maps::GetIndexFromAreaPoint(mouse_coord);
	    const Maps::Tiles & tile = world.GetTiles(index_maps);
	    const Rect tile_pos(BORDERWIDTH + ((u16) (mouse_coord.x - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, BORDERWIDTH + ((u16) (mouse_coord.y - BORDERWIDTH) / TILEWIDTH) * TILEWIDTH, TILEWIDTH, TILEWIDTH);
	    const Castle * castle = NULL;
	    const Heroes * heroes = NULL;
	    u8 object = tile.GetObject();

	    switch(globalfocus.type)
	    {
		// focus from hero
		case Game::HEROES:
		{
		    if(NULL == (heroes = world.GetHeroes(globalfocus.center))) break;

		    const Route & path = (*heroes).GetPath();

		    switch(object)
		    {
			// focus from hero to monster
    			case MP2::OBJ_MONSTER:
    			    Cursor::Set(Cursor::FIGHT);

			    // FIXME heroes attack monster
			    if(le.MouseClickLeft(tile_pos))
			    {
				const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

				if(path.GetDestinationIndex() != new_dst_point)
				{
				    Cursor::Hide();
				    if(path.Length()) path.Hide();
				    const_cast<Route &>(path).Calculate(new_dst_point);
				    path.Show();
				    Cursor::Show();
				    display.Flip();
				}
				else
				{
				    const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

				    if(path.GetDestinationIndex() != new_dst_point)
				    {
					Cursor::Hide();
					if(path.Length()) path.Hide();
					const_cast<Route &>(path).Calculate(new_dst_point);
					path.Show();
					Cursor::Show();
					display.Flip();
				    }
				    else
				    {
				        // hero goto near monster
					if(path.Get().size() > 1)
					{
					    Cursor::Hide();
				    	    const_cast<Heroes &>(*heroes).Goto(path.Get().at(path.Get().size() - 2));
				    	    globalfocus.center = heroes->GetCenter();
    					    FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
				    	    const_cast<Route &>(path).Reset();
					    Cursor::Show();
					    display.Flip();
				    	}
				    	else
				    	{
					    Cursor::Hide();
				    	    const_cast<Route &>(path).Reset();
					    Cursor::Show();
					    display.Flip();
				    	}

				    	// FIXME: hero attack monster
				    	Error::Verbose("Game: FIXME: hero attack monster");
				    }
				}
			    }
			    else
			    if(le.MousePressRight(tile_pos))
			    {
				Monster::monster_t monster = Monster::Monster(tile);

				if(Monster::UNKNOWN > monster) Dialog::QuickInfo(Army::String(Army::GetSize(Monster::GetSize(tile))) + " of " + Monster::String(monster));
			    }
			    break;

			// focus from hero to castle
    			case MP2::OBJN_CASTLE:
    			    if(NULL != (castle = world.GetCastle(index_maps)))
    			    {
				if(H2Config::GetMyColor() == castle->GetColor())
				{
	    			    Cursor::Set(Cursor::CASTLE);

				    if(le.MouseClickLeft(tile_pos))
				    {
					if(selectHeroes.isSelected())
					{
					    selectHeroes.Reset();
					    selectHeroes.Redraw();
					}

					// is selected open dialog
					if(selectCastles.isSelected() && castle->GetCenter() == selectCastles.GetCenter(selectCastles.GetSelectIndex()))
					{
					    OpenCastle(const_cast<Castle *>(castle), areaMaps, radar);

					    statusWindow.Redraw();
					    display.Flip();
					}
					// select other castle
					else
					{
					    path.Hide();
					    FocusToCastle(const_cast<Castle *>(castle), areaMaps, radar);
					    display.Flip();
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*castle);
				    }
				}
				// over enemy castle
				else
				{
	    			    Cursor::Set(Cursor::FIGHT);

				    if(le.MouseClickLeft(tile_pos))
				    {
					const u16 new_dst_point = Maps::GetIndexFromAbsPoint(castle->GetCenter());

					if(path.GetDestinationIndex() != new_dst_point)
					{
					    Cursor::Hide();
					    if(path.Length()) path.Hide();
					    const_cast<Route &>(path).Calculate(new_dst_point);
					    path.Show();
					    Cursor::Show();
					    display.Flip();
					}
					else
					{
				    	    // hero goto near castle
					    if(path.Get().size() > 1)
					    {
						Cursor::Hide();
				    		const_cast<Heroes &>(*heroes).Goto(path.Get().at(path.Get().size() - 2));
				    		globalfocus.center = heroes->GetCenter();
    						FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
				    		const_cast<Route &>(path).Reset();
						Cursor::Show();
						display.Flip();
				    	    }
				    	    else
				    	    {
						Cursor::Hide();
				    		const_cast<Route &>(path).Reset();
						Cursor::Show();
						display.Flip();
				    	    }

				    	    // FIXME: hero attack castle
				    	    Error::Verbose("Game: FIXME: hero attack castle");
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*castle);
				    }
				}
    			    }
    			    break;

			// focus from hero to castle
    			case MP2::OBJ_CASTLE:
    			    if(NULL != (castle = world.GetCastle(index_maps)))
			    {
				if(H2Config::GetMyColor() == castle->GetColor())
				{
        			    Cursor::Set(Cursor::ACTION);

				    if(le.MouseClickLeft(tile_pos))
				    {
					const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

					if(path.GetDestinationIndex() != new_dst_point)
					{
					    // hero show path
					    Cursor::Hide();
					    if(path.Length()) path.Hide();
					    const_cast<Route &>(path).Calculate(new_dst_point);
					    path.Show();
					    Cursor::Show();
					    display.Flip();
					}
					else
					{
					    // heroes goto in castle
					    Cursor::Hide();
				    	    const_cast<Heroes &>(*heroes).Goto(Maps::GetIndexFromAreaPoint(mouse_coord));
				    	    const_cast<Route &>(path).Reset();
				    	    globalfocus.center = heroes->GetCenter();
    					    FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
					    Cursor::Show();
					    display.Flip();

    					    // castle dialog
    					    if(NULL != (castle = world.GetCastle(heroes->GetCenter())))
    					    {
						OpenCastle(const_cast<Castle *>(castle), areaMaps, radar);

						statusWindow.Redraw();
						display.Flip();
					    }
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*castle);
				    }
				}
				// over enemy castle
				else
				{
				    Cursor::Set(Cursor::FIGHT);

				    if(le.MouseClickLeft(tile_pos))
				    {
					const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

					if(path.GetDestinationIndex() != new_dst_point)
					{
					    Cursor::Hide();
					    if(path.Length()) path.Hide();
					    const_cast<Route &>(path).Calculate(new_dst_point);
					    path.Show();
					    Cursor::Show();
					    display.Flip();
					}
					else
					{
				    	    // hero goto near castle
					    if(path.Get().size() > 1)
					    {
						Cursor::Hide();
				    		const_cast<Heroes &>(*heroes).Goto(path.Get().at(path.Get().size() - 2));
				    		globalfocus.center = heroes->GetCenter();
    						FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
				    		const_cast<Route &>(path).Reset();
						Cursor::Show();
						display.Flip();
				    	    }
				    	    else
				    	    {
						Cursor::Hide();
				    		const_cast<Route &>(path).Reset();
						Cursor::Show();
						display.Flip();
				    	    }

				    	    // FIXME: hero attack castle
				    	    Error::Verbose("Game: FIXME: hero attack castle");
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*castle);
				    }
				}
			    }
        		    break;

			// focus from hero to hero
			case MP2::OBJ_HEROES:
			{
			    const Heroes * heroes2 = world.GetHeroes(index_maps);
    			    if(NULL != heroes2)
    			    {
				if(H2Config::GetMyColor() == heroes2->GetColor())
				{
				    Cursor::Set(heroes2->GetCenter() == selectHeroes.GetCenter(selectHeroes.GetSelectIndex()) ? Cursor::HEROES : Cursor::CHANGE);

				    if(le.MouseClickLeft(tile_pos))
				    {
					// is selected open dialog
					if(heroes2->GetCenter() == selectHeroes.GetCenter(selectHeroes.GetSelectIndex()))
					{
					    OpenHeroes(const_cast<Heroes *>(heroes2), areaMaps, radar);

					    statusWindow.Redraw();
					    display.Flip();
					}
					// heroes dialog meeting
					else
					{
					    const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

					    if(path.GetDestinationIndex() != new_dst_point)
					    {
						Cursor::Hide();
						if(path.Length()) path.Hide();
						const_cast<Route &>(path).Calculate(new_dst_point);
						path.Show();
						Cursor::Show();
						display.Flip();
					    }
					    else
					    {
				    		// hero goto near hero
						if(path.Get().size() > 1)
						{
						    Cursor::Hide();
				    		    const_cast<Heroes &>(*heroes).Goto(path.Get().at(path.Get().size() - 2));
				    		    globalfocus.center = heroes->GetCenter();
    						    FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
				    		    const_cast<Route &>(path).Reset();
						    Cursor::Show();
						    display.Flip();
				    		}
				    		else
				    		{
						    Cursor::Hide();
				    		    const_cast<Route &>(path).Reset();
						    Cursor::Show();
						    display.Flip();
				    		}

						// FIXME: hero meeting dialog
				    		Error::Verbose("Game: FIXME: hero meeting other hero");
					    }
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*heroes2);
				    }
				}
				// over enemy hero
				else
				{
				    Cursor::Set(Cursor::FIGHT);

				    if(le.MouseClickLeft(tile_pos))
				    {
					const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

					if(path.GetDestinationIndex() != new_dst_point)
					{
					    Cursor::Hide();
					    if(path.Length()) path.Hide();
					    const_cast<Route &>(path).Calculate(new_dst_point);
					    path.Show();
					    Cursor::Show();
					    display.Flip();
					}
					else
					{
				    	    // hero goto near monster
					    if(path.Get().size() > 1)
					    {
						Cursor::Hide();
				    	        const_cast<Heroes &>(*heroes).Goto(path.Get().at(path.Get().size() - 2));
				    		globalfocus.center = heroes->GetCenter();
    					        FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
				    		const_cast<Route &>(path).Reset();
					        Cursor::Show();
					        display.Flip();
				    	    }
				    	    else
				    	    {
					        Cursor::Hide();
				    		const_cast<Route &>(path).Reset();
					        Cursor::Show();
					        display.Flip();
				    	    }

				    	    // FIXME: hero attack other hero
				    	    Error::Verbose("Game: FIXME: hero attack other hero");
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*heroes2);
				    }
				}
    			    }
    			}
    			    break;

			// focus from hero to boat
    			//case MP2::OBJ_BOAT:
    			//		Cursor::Set(Cursor::BOAT);
			//    break;

			// focus from hero to object
    			case MP2::OBJ_TREASURECHEST:
    			    if(Maps::Ground::WATER != tile.GetGround())
    			    {
    			        Cursor::Set(Cursor::ACTION);

				if(le.MouseClickLeft(tile_pos))
				{
				    const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

				    // show path
				    if(path.GetDestinationIndex() != new_dst_point)
				    {
					Cursor::Hide();
					if(path.Length()) path.Hide();
					const_cast<Route &>(path).Calculate(new_dst_point);
					path.Show();
					Cursor::Show();
					display.Flip();
				    }
				    else
				    {
					const u16 dst_index = path.GetDestinationIndex();

				        //hero goto near object
					if(path.Get().size() > 1)
					{
					    Cursor::Hide();
				    	    const_cast<Heroes &>(*heroes).Goto(path.Get().at(path.Get().size() - 2));
				    	    globalfocus.center = heroes->GetCenter();
    					    FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
				    	    const_cast<Route &>(path).Reset();
					    Cursor::Show();
					    display.Flip();
				    	}
				    	else
				    	{
					    Cursor::Hide();
				    	    const_cast<Route &>(path).Reset();
					    Cursor::Show();
					    display.Flip();
				    	}

				    	// hero action
				    	const_cast<Heroes &>(*heroes).Action(dst_index);
				    }
				}
    			    }
    			    break;

			// focus from hero to object
    			case MP2::OBJ_ALCHEMYTOWER:
    			case MP2::OBJ_SIGN:
    			case MP2::OBJ_SKELETON:
    			case MP2::OBJ_DAEMONCAVE:
    			case MP2::OBJ_FAERIERING:
    			case MP2::OBJ_CAMPFIRE:
    			case MP2::OBJ_FOUNTAIN:
    			case MP2::OBJ_GAZEBO:
    			case MP2::OBJ_ANCIENTLAMP:
    			case MP2::OBJ_GRAVEYARD:
    			case MP2::OBJ_ARCHERHOUSE:
    			case MP2::OBJ_GOBLINHUNT:
    			case MP2::OBJ_DWARFCOTT:
    			case MP2::OBJ_PEASANTHUNT:
    			case MP2::OBJ_PEASANTHUNT2:
    			case MP2::OBJ_DRAGONCITY:
    			case MP2::OBJ_LIGHTHOUSE:
    			case MP2::OBJ_WATERMILL:
    			case MP2::OBJ_MINES:
			case MP2::OBJ_OBELISK:
			case MP2::OBJ_OASIS:
			case MP2::OBJ_RESOURCE:
			case MP2::OBJ_SAWMILL:
			case MP2::OBJ_ORACLE:
			case MP2::OBJ_SHRINE1:
			case MP2::OBJ_DERELICTSHIP:
			case MP2::OBJ_DESERTTENT:
			case MP2::OBJ_STONELIGHTS:
			case MP2::OBJ_WAGONCAMP:
			case MP2::OBJ_WINDMILL:
			case MP2::OBJ_ARTIFACT:
			case MP2::OBJ_WATCHTOWER:
			case MP2::OBJ_TREEHOUSE:
			case MP2::OBJ_TREECITY:
			case MP2::OBJ_RUINS:
			case MP2::OBJ_FORT:
    			case MP2::OBJ_TRADINGPOST:
    			case MP2::OBJ_ABANDONEDMINE:
    			case MP2::OBJ_STANDINGSTONES:
    			case MP2::OBJ_IDOL:
    			case MP2::OBJ_TREEKNOWLEDGE:
    			case MP2::OBJ_DOCTORHUNT:
    			case MP2::OBJ_TEMPLE:
    			case MP2::OBJ_HILLFORT:
    			case MP2::OBJ_HALFLINGHOLE:
    			case MP2::OBJ_MERCENARYCAMP:
    			case MP2::OBJ_SHRINE2:
    			case MP2::OBJ_SHRINE3:
    			case MP2::OBJ_PIRAMID:
    			case MP2::OBJ_CITYDEAD:
    			case MP2::OBJ_EXCAVATION:
    			case MP2::OBJ_SPHINX:
    			case MP2::OBJ_WAGON:
    			case MP2::OBJ_ARTESIANSPRING:
    			case MP2::OBJ_TROLLBRIDGE:
    			case MP2::OBJ_WITCHHUNT:
    			case MP2::OBJ_XANADU:
    			case MP2::OBJ_CAVE:
    			case MP2::OBJ_LEANTO:
    			case MP2::OBJ_MAGICWELL:
    			case MP2::OBJ_MAGICGARDEN:
    			case MP2::OBJ_OBSERVATIONTOWER:
    			case MP2::OBJ_FREEMANFOUNDRY:

				Cursor::Set(Cursor::ACTION);

				if(le.MouseClickLeft(tile_pos))
				{
				    const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

				    if(path.GetDestinationIndex() != new_dst_point)
				    {
					Cursor::Hide();
					if(path.Length()) path.Hide();
					const_cast<Route &>(path).Calculate(new_dst_point);
					path.Show();
					Cursor::Show();
					display.Flip();
				    }
				    else
				    {
					const u16 dst_index =path.GetDestinationIndex();

				        //hero goto near object
					if(path.Get().size() > 1)
					{
					    Cursor::Hide();
				    	    const_cast<Heroes &>(*heroes).Goto(path.Get().at(path.Get().size() - 2));
				    	    globalfocus.center = heroes->GetCenter();
    					    FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
				    	    const_cast<Route &>(path).Reset();
					    Cursor::Show();
					    display.Flip();
				    	}
				    	else
				    	{
					    Cursor::Hide();
				    	    const_cast<Route &>(path).Reset();
					    Cursor::Show();
					    display.Flip();
				    	}

				    	// hero action
				    	const_cast<Heroes &>(*heroes).Action(dst_index);
				    }
                        	}
			    break;

			default:
                            if(Maps::Ground::WATER != tile.GetGround() && tile.isPassable())
                            {
                            	// FIXME: select MOVE cursor or MOVE2 or MOVE3 or MOVE4
                            	Cursor::Set(Cursor::MOVE);

				if(le.MouseClickLeft(tile_pos))
				{
				    const u16 new_dst_point = Maps::GetIndexFromAreaPoint(mouse_coord);

				    if(path.GetDestinationIndex() != new_dst_point)
				    {
					// hero show path
					Cursor::Hide();
					if(path.Length()) path.Hide();
					const_cast<Route &>(path).Calculate(new_dst_point);
					path.Show();
					Cursor::Show();
					display.Flip();
				    }
				    else
				    {
					// hero goto
					Cursor::Hide();
				        const_cast<Heroes &>(*heroes).Goto(Maps::GetIndexFromAreaPoint(mouse_coord));
				        const_cast<Route &>(path).Reset();
				        globalfocus.center = heroes->GetCenter();
    					FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);
					Cursor::Show();
					display.Flip();
				    }
                        	}
                    	    }
                            else
                            {
                                Cursor::Set(Cursor::POINTER);
                            }
			    break;
		    }
		}
		break;

		// focus from boat
    		case Game::BOAT:
		{
    		    if(NULL == (heroes = world.GetHeroes(index_maps))) break;

		    const Route & path = (*heroes).GetPath();

		    switch(object)
		    {
			// focus from boat to castle
    			case MP2::OBJN_CASTLE:
    			case MP2::OBJ_CASTLE:
    			    if(NULL != (castle = world.GetCastle(index_maps)))
			    {
				if((*castle).GetColor() == H2Config::GetMyColor())
				{
			    	    Cursor::Set(Cursor::CASTLE);

				    if(le.MouseClickLeft(tile_pos))
				    {
					// is selected open dialog
				        if(castle->GetCenter() == selectCastles.GetCenter(selectCastles.GetSelectIndex()))
					{
					    OpenCastle(const_cast<Castle *>(castle), areaMaps, radar);

					    statusWindow.Redraw();
					    display.Flip();
					}
					// select other castle
					else
					{
					    path.Hide();
					    FocusToCastle(const_cast<Castle *>(castle), areaMaps, radar);
					    display.Flip();
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*castle);
				    }
				}
				// over enemy castle
				else
				{
			    	    Cursor::Set(Cursor::POINTER);

				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*castle);
				    }
				}
    			    }
    			    break;

			// focus from boat to hero
			//case MP2::OBJ_HEROES:
    			//    if(NULL != (heroes = world.GetHeroes(index_maps)) && (*heroes).GetColor() == H2Config::GetMyColor())
    			//			Cursor::Set(Cursor::HEROES);
        		//    break;

			// focus from boat to boat
    			case MP2::OBJ_BOAT:
    			    if(NULL != (heroes = world.GetHeroes(index_maps)))
					Cursor::Set((*heroes).GetColor() == H2Config::GetMyColor() ? Cursor::HEROES : Cursor::FIGHT);
    			    break;

			// focus from boat to object
    			case MP2::OBJ_TREASURECHEST:
			    if(Maps::Ground::WATER == tile.GetGround())
					Cursor::Set(Cursor::REDBOAT);
			    break;

			// focus from boat to object
    			case MP2::OBJ_SHIPWRECK:
    			case MP2::OBJ_WHIRLPOOL:
    			case MP2::OBJ_BUOY:
    			case MP2::OBJ_BOTTLE:
    			case MP2::OBJ_SHIPWRECKSURVIROR:
    			case MP2::OBJ_FLOTSAM:
    			case MP2::OBJ_MAGELLANMAPS:
					Cursor::Set(Cursor::REDBOAT);
			    break;

			// focus from boat to object
    			case MP2::OBJ_COAST:
					Cursor::Set(Cursor::ANCHOR);
			    break;

			default:
			    Cursor::Set(Maps::Ground::WATER == tile.GetGround() ? Cursor::BOAT : Cursor::POINTER);
			    break;
		    }
		}
		break;

		// focus from castle
		case Game::CASTLE:
		{
		    switch(object)
		    {
			// focus from castle to castle
    			case MP2::OBJN_CASTLE:
    			case MP2::OBJ_CASTLE:
    			    if(NULL != (castle = world.GetCastle(index_maps)))
    			    {
    				if(castle->GetColor() == H2Config::GetMyColor())
    				{
    			    	    Cursor::Set(Cursor::CASTLE);

				    if(le.MouseClickLeft(tile_pos))
				    {
					// is selected open dialog
					if(castle->GetCenter() == selectCastles.GetCenter(selectCastles.GetSelectIndex()))
					{
					    OpenCastle(const_cast<Castle *>(castle), areaMaps, radar);

					    statusWindow.Redraw();
					    display.Flip();
					}
					// select other castle
					else
					{
					    if(HEROES == globalfocus.type || BOAT == globalfocus.type)
					    {
						const Route & path = (*world.GetHeroes(globalfocus.center)).GetPath();
						path.Hide();
					    }
					    FocusToCastle(const_cast<Castle *>(castle), areaMaps, radar);
					    display.Flip();
					}
				    }
				    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*castle);
				    }
				}
				// over enemy castle
				else
				{
    			    	    Cursor::Set(Cursor::POINTER);

				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*castle);
				    }
				}
			    }
			    break;

			// focus from castle to heroes
			case MP2::OBJ_HEROES:
    			    if(NULL != (heroes = world.GetHeroes(index_maps)))
    			    {
    				if(heroes->GetColor() == H2Config::GetMyColor())
    				{
    			    	    Cursor::Set(Cursor::HEROES);

				    if(le.MouseClickLeft(tile_pos))
				    {
					if(selectCastles.isSelected())
					{
					    selectCastles.Reset();
					    selectCastles.Redraw();
					}

					// is selected open dialog
					if(selectHeroes.isSelected() && heroes->GetCenter() == selectHeroes.GetCenter(selectHeroes.GetSelectIndex()))
					{
					    OpenHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);

					    statusWindow.Redraw();
					    display.Flip();
					}
					// select other hero
					else
					{
					    if(HEROES == globalfocus.type || BOAT == globalfocus.type)
					    {
						const Route & path = (*world.GetHeroes(globalfocus.center)).GetPath();
						path.Hide();
					    }
    					    FocusToHeroes(const_cast<Heroes *>(heroes), areaMaps, radar);

					    const Route & path = (*world.GetHeroes(globalfocus.center)).GetPath();
					    if(path.Length())
					    {
						Cursor::Hide();
						path.Show();
						Cursor::Show();
					    }

					    display.Flip();
					}
        			    }
        			    else
				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*heroes);
				    }
        			}
        			else
        			{
    			    	    Cursor::Set(Cursor::POINTER);

				    if(le.MousePressRight(tile_pos))
				    {
					Dialog::QuickInfo(*heroes);
				    }
        			}
        		    }
        		    break;

			// focus from castle to boat
    			//case MP2::OBJ_BOAT:
    			//    if(NULL != (heroes = world.GetHeroes(index_maps)) && (*heroes).GetColor() == H2Config::GetMyColor())
    			//			Cursor::Set(Cursor::HEROES);
        		//    break;

			default:
			    // default cursor
			    Cursor::Set(Cursor::POINTER);
			    break;
		    }
		}
		break;
	    }

	    if(le.MouseRight())
	    {
		if(H2Config::Debug()) tile.DebugInfo(index_maps);

		const std::string & info = (MP2::OBJ_ZERO == object || MP2::OBJ_EVENT == object ?
		    Maps::Ground::String(tile.GetGround()) : MP2::StringObject(object));

		Dialog::QuickInfo(info);
	    }

	// end cursor over game area
	}


	// draw push buttons
	le.MousePressLeft(buttonScrollHeroesUp) ? buttonScrollHeroesUp.Press() : buttonScrollHeroesUp.Release();
	le.MousePressLeft(buttonScrollCastleUp) ? buttonScrollCastleUp.Press() : buttonScrollCastleUp.Release();
	le.MousePressLeft(buttonNextHero) ? buttonNextHero.Press() : buttonNextHero.Release();
	le.MousePressLeft(buttonMovement) ? buttonMovement.Press() : buttonMovement.Release();
	le.MousePressLeft(buttonKingdom) ? buttonKingdom.Press() : buttonKingdom.Release();
	le.MousePressLeft(buttonSpell) ? buttonSpell.Press() : buttonSpell.Release();
	le.MousePressLeft(buttonEndTur) ? buttonEndTur.Press() : buttonEndTur.Release();
	le.MousePressLeft(buttonAdventure) ? buttonAdventure.Press() : buttonAdventure.Release();
	le.MousePressLeft(buttonFile) ? buttonFile.Press() : buttonFile.Release();
	le.MousePressLeft(buttonSystem) ? buttonSystem.Press() : buttonSystem.Release();
	le.MousePressLeft(buttonScrollHeroesDown) ? buttonScrollHeroesDown.Press() : buttonScrollHeroesDown.Release();
	le.MousePressLeft(buttonScrollCastleDown) ? buttonScrollCastleDown.Press() : buttonScrollCastleDown.Release();

	// point radar
	if(le.MouseCursor(radar.GetRect()) &&
	    (le.MouseClickLeft(radar.GetRect()) ||
		le.MousePressLeft(radar.GetRect())))
	{
	    Rect prev(areaMaps.GetRect());
	    areaMaps.CenterFromRadar(le.MouseCursor());
	    if(prev != areaMaps.GetRect()){
		Cursor::Hide();
		radar.UpdatePosition();
		Cursor::Show();
		display.Flip();
	    }
	}
	
	// click Scroll Heroes Up
	if((le.MouseWheelUp(selectHeroes.GetMaxRect()) ||
	    le.MouseWheelUp(splitHeroes.GetRect()) ||
	    le.MouseClickLeft(buttonScrollHeroesUp))  && selectHeroes.Prev())
	{
	    Cursor::Hide();
	    selectHeroes.Redraw();
	    splitHeroes.Backward();
	    Cursor::Show();
	    display.Flip();
	}

	// click Scroll Castle Up
	if((le.MouseWheelUp(selectCastles.GetMaxRect()) ||
	    le.MouseWheelUp(splitCastles.GetRect()) ||
	    le.MouseClickLeft(buttonScrollCastleUp)) && selectCastles.Prev())
	{
	    Cursor::Hide();
	    selectCastles.Redraw();
	    splitCastles.Backward();
	    Cursor::Show();
	    display.Flip();
	}

	// click Scroll Heroes Down
	if((le.MouseWheelDn(selectHeroes.GetMaxRect()) ||
	    le.MouseWheelDn(splitHeroes.GetRect()) ||
	    le.MouseClickLeft(buttonScrollHeroesDown)) && 
	    myHeroes.size() > selectHeroes.GetCoords().size() &&
	    selectHeroes.Next())
	{
	    Cursor::Hide();
	    selectHeroes.Redraw();
	    splitHeroes.Forward();
	    Cursor::Show();
	    display.Flip();
	}

	// click Scroll Castle Down
	if((le.MouseWheelDn(selectCastles.GetMaxRect()) ||
	    le.MouseWheelDn(splitCastles.GetRect())  ||
	    le.MouseClickLeft(buttonScrollCastleDown)) &&
	    myCastles.size() > selectCastles.GetCoords().size() &&
	    selectCastles.Next())
	{
	    Cursor::Hide();
	    selectCastles.Redraw();
	    splitCastles.Forward();
	    Cursor::Show();
	    display.Flip();
	}

        // move splitter cursor castle
        if(le.MousePressLeft(splitCastles.GetRect()) && myCastles.size() > icon_count && le.MouseCursor(splitCastles.GetRect()))
        {
    	    u32 seek = (le.MouseCursor().y - splitCastles.GetRect().y) * 100 / splitCastles.GetStep();

            if(seek > myCastles.size() - 1) seek = myCastles.size() - 1;

            Cursor::Hide();

            splitCastles.Move(seek);
	    selectCastles.SetTop(seek);
	    selectCastles.Redraw();

            Cursor::Show();
            display.Flip();
	}

        // move splitter cursor heroes
        if(le.MousePressLeft(splitHeroes.GetRect()) && myHeroes.size() > icon_count && le.MouseCursor(splitHeroes.GetRect()))
        {
    	    u32 seek = (le.MouseCursor().y - splitHeroes.GetRect().y) * 100 / splitHeroes.GetStep();

            if(seek > myHeroes.size() - 1) seek = myHeroes.size() - 1;

            Cursor::Hide();

            splitHeroes.Move(seek);
	    selectHeroes.SetTop(seek);
	    selectHeroes.Redraw();

            Cursor::Show();
            display.Flip();
	}

	// click Heroes Icons
	if(u8 count_icons = coordsHeroesIcon.size())
	{
	    for(u8 ii = 0; ii < count_icons; ++ii)
	    {
		const Rect & coord = coordsHeroesIcon[ii];
		const u8 icons_index = selectHeroes.GetTopIndex() + ii;

		// left click (center to hero)
		if(le.MouseClickLeft(coord) && icons_index < myHeroes.size())
		{
		    //double click
		    if(icons_index == selectHeroes.GetCursorIndex())
		    {
			const Point & center = selectHeroes.GetCenter(ii);
			Heroes *hero = const_cast<Heroes *>(world.GetHeroes(center.x, center.y));

			// open hero
			if(hero && H2Config::GetMyColor() == (*hero).GetColor())
			{
			    if(selectCastles.isSelected())
			    {
				selectCastles.Reset();
				selectCastles.Redraw();
			    }

			    OpenHeroes(hero, areaMaps, radar);

			    statusWindow.Redraw();
			    display.Flip();
			}
		    }
		    else
		    {
			if(selectCastles.isSelected())
			{
			    selectCastles.Reset();
			    selectCastles.Redraw();
			}

			const Point & center = selectHeroes.GetCenter(ii);
			Heroes *hero = const_cast<Heroes *>(world.GetHeroes(center.x, center.y));

			// open hero
			if(hero && H2Config::GetMyColor() == (*hero).GetColor())
			{
			    if(HEROES == globalfocus.type || BOAT == globalfocus.type)
			    {
				const Route & path = (*world.GetHeroes(globalfocus.center)).GetPath();
				path.Hide();
			    }
			    FocusToHeroes(hero, areaMaps, radar);

			    const Route & path = (*world.GetHeroes(globalfocus.center)).GetPath();
			    if(path.Length())
			    {
				Cursor::Hide();
				path.Show();
				Cursor::Show();
			    }

			    display.Flip();
			}
		    }
		}
		else
		// right click (show info heroes)
		if(le.MousePressRight(coord) && icons_index < myHeroes.size())
		{
		    const Point & center = selectHeroes.GetCenter(ii);
		    const Heroes *hero = world.GetHeroes(center.x, center.y);
		    if(hero)
		    {
			Cursor::Hide();
			Dialog::QuickInfo(*hero);
			Cursor::Show();
			display.Flip();
		    }
		}
	    }
	}

	// click Castle Icons
	if(u8 count_icons = coordsCastlesIcon.size())
	{
	    for(u8 ii = 0; ii < count_icons; ++ii)
	    {
		const Rect & coord = coordsCastlesIcon[ii];
		const u8 icons_index = selectCastles.GetTopIndex() + ii;

		// left click (center to castle)
		if(le.MouseClickLeft(coord) && icons_index < myCastles.size())
		{
		    // double click
		    if(icons_index == selectCastles.GetCursorIndex())
		    {
			const Point & center = selectCastles.GetCenter(ii);
			Castle *castle = const_cast<Castle *>(world.GetCastle(center.x, center.y));

			// open castle
			if(castle && H2Config::GetMyColor() == (*castle).GetColor())
			{
			    if(selectHeroes.isSelected())
			    {
				selectHeroes.Reset();
				selectHeroes.Redraw();
			    }

			    OpenCastle(castle, areaMaps, radar);

			    statusWindow.Redraw();
			    display.Flip();
			}
		    }
		    else
		    {
			if(selectHeroes.isSelected())
			{
			    selectHeroes.Reset();
			    selectHeroes.Redraw();
			}

			const Point & center = selectCastles.GetCenter(ii);
			Castle *castle = const_cast<Castle *>(world.GetCastle(center.x, center.y));

			if(castle && H2Config::GetMyColor() == (*castle).GetColor())
			{
			    if(HEROES == globalfocus.type || BOAT == globalfocus.type)
			    {
				const Route & path = (*world.GetHeroes(globalfocus.center)).GetPath();
				path.Hide();
			    }
			    FocusToCastle(castle, areaMaps, radar);
			    display.Flip();
			}
		    }
		}
		else
		// right click (show info castle)
		if(le.MousePressRight(coord) && icons_index < myCastles.size())
		{
		    const Point & center = selectCastles.GetCenter(ii);
		    Castle *castle = const_cast<Castle *>(world.GetCastle(center.x, center.y));
		    
		    if(castle)
		    {
			Cursor::Hide();
			Dialog::QuickInfo(*castle);
			Cursor::Show();
			display.Flip();
		    }
		}
	    }
	}

	// click Next Hero
	if(le.MouseClickLeft(buttonNextHero))
	{
	}

	// click Continue Movement
	if(le.MouseClickLeft(buttonMovement))
	{
	}

	// click Kingdom Summary
	if(le.MouseClickLeft(buttonKingdom))
	{
	}

	// click Cast Spell
	if(le.MouseClickLeft(buttonSpell))
	{
	}

	// click End Turn
	if(le.MouseClickLeft(buttonEndTur))
	{
	    Cursor::Hide();
	    world.NextDay();

	    Color::color_t human = H2Config::GetMyColor();

	    // AI move
	    for(Color::color_t color = Color::BLUE; color != Color::GRAY; ++color) if(color != human)
		const_cast<Kingdom &>(world.GetKingdom(color)).AITurns(statusWindow);

	    statusWindow.SetState(Game::StatusWindow::DAY);
	    statusWindow.Redraw();
	    Cursor::Show();
	    display.Flip();
	}

        // click AdventureOptions
	if(le.MouseClickLeft(buttonAdventure))
	{
	    switch(Dialog::AdventureOptions())
	    {
		default:
		    break;
	    }
        }

	// click FileOptions
	if(le.MouseClickLeft(buttonFile))
	{
	    Game::menu_t result = Dialog::FileOptions();
	    
	    switch(result){

		case Game::NEWGAME:
		case Game::LOADGAME:
		case Game::QUITGAME:
		    return result;

		case Game::SAVEGAME:
		    break;

		default:
		    break;
	    }
	}

	// click SystemOptions
	if(le.MouseClickLeft(buttonSystem)&& Dialog::OK == Dialog::SystemOptions())
	{
	    // Change and save system settings
	}
	
	// click StatusWindow
	if(le.MouseClickLeft(statusWindow.GetRect()))
	{
	    Cursor::Hide();
	    statusWindow.NextState();
	    statusWindow.Redraw();
	    Cursor::Show();
	    display.Flip();
	}

	// right info
	if(le.MousePressRight(radar.GetRect())) Dialog::Message("World Map", "A miniature view of the known world. Left click to move viewing area.", Font::BIG);
	if(le.MousePressRight(buttonNextHero)) Dialog::Message("Next Hero", "Select the next Hero.", Font::BIG);
	if(le.MousePressRight(buttonMovement)) Dialog::Message("Continue Movement", "Continue the Hero's movement along the current path.", Font::BIG);
	if(le.MousePressRight(buttonKingdom)) Dialog::Message("Kingdom Summary", "View a Summary of your Kingdom.", Font::BIG);
	if(le.MousePressRight(buttonSpell)) Dialog::Message("Cast Spell", "Cast an adventure spell.", Font::BIG);
	if(le.MousePressRight(buttonEndTur)) Dialog::Message("End Turn", "End your turn and left the computer take its turn.", Font::BIG);
	if(le.MousePressRight(buttonAdventure)) Dialog::Message("Adventure Options", "Bring up the adventure options menu.", Font::BIG);
	if(le.MousePressRight(buttonFile)) Dialog::Message("File Options", "Bring up the file options menu, alloving you to load menu, save etc.", Font::BIG);
	if(le.MousePressRight(buttonSystem)) Dialog::Message("System Options", "Bring up the system options menu, alloving you to customize your game.", Font::BIG);
	if(le.MousePressRight(statusWindow.GetRect())) Dialog::Message("Status Window", "This window provides information on the status of your hero or kingdom, and shows the date. Left click here to cycle throungh these windows.", Font::BIG);
	
        // animation
        if(!(++ticket % 50)) // FIXME: speed animation low
        {
            Cursor::Hide();
            areaMaps.RedrawAnimation();
            Cursor::Show();
            display.Flip();
        }
    }

    return QUITGAME;
}

/* open castle wrapper */
void Game::OpenCastle(Castle *castle, GameArea & areaMaps, Radar & radar)
{
    if(! castle) return;

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Castle *> & myCastles = myKingdom.GetCastles();
    SelectFocusCastles & selectCastles = SelectFocusCastles::Get();

    Dialog::answer_t result = Dialog::ZERO;

    while(Dialog::CANCEL != result)
    {
	FocusToCastle(castle, areaMaps, radar);

	Cursor::Hide();

	result = castle->OpenDialog();

	const u32 cursor_index = selectCastles.GetCursorIndex();

	if(Dialog::PREV == result)
	{
	     castle = cursor_index ? myCastles.at(cursor_index - 1) : myCastles.back();
	}
	else
	if(Dialog::NEXT == result)
	{
	     castle = myCastles.size() == cursor_index + 1 ? myCastles.front() : myCastles.at(cursor_index + 1);
	}

	Cursor::Show();
    }

    if(Heroes *hero = const_cast<Heroes *>(castle->GetHeroes()))
	    FocusToHeroes(hero, areaMaps, radar);

    areaMaps.Redraw();
}

/* focus to castle */
void Game::FocusToCastle(Castle *castle, GameArea & areaMaps, Radar & radar)
{
    Cursor::Hide();

    SelectFocusCastles & selectCastles = SelectFocusCastles::Get();
    SelectFocusHeroes & selectHeroes = SelectFocusHeroes::Get();

    selectCastles.Reset();
    selectHeroes.Reset();

    globalfocus.type = Game::CASTLE;
    globalfocus.center = castle->GetCenter();

    areaMaps.Center(globalfocus.center);
    radar.UpdatePosition();

    selectCastles.SelectFromCenter(castle->GetCenter());
    selectCastles.Redraw();
    selectHeroes.Redraw();

    Cursor::Show();
}

/* open heroes wrapper */
void Game::OpenHeroes(Heroes *hero, GameArea & areaMaps, Radar & radar)
{
    if(! hero) return;

    const Kingdom & myKingdom = world.GetMyKingdom();
    const std::vector<Heroes *> & myHeroes = myKingdom.GetHeroes();
    SelectFocusHeroes & selectHeroes = SelectFocusHeroes::Get();

    Dialog::answer_t result = Dialog::ZERO;

    while(Dialog::CANCEL != result)
    {
	FocusToHeroes(hero, areaMaps, radar);

	Cursor::Hide();

	result = hero->OpenDialog();

	const u32 cursor_index = selectHeroes.GetCursorIndex();

	if(Dialog::PREV == result)
	{
	     hero = cursor_index ? myHeroes.at(cursor_index - 1) : myHeroes.back();
	}
	else
	if(Dialog::NEXT == result)
	{
	     hero = myHeroes.size() == cursor_index + 1 ? myHeroes.front() : myHeroes.at(cursor_index + 1);
	}

	Cursor::Show();
    }
}

/* focus to heroes */
void Game::FocusToHeroes(Heroes *hero, GameArea & areaMaps, Radar & radar)
{
    Cursor::Hide();

    SelectFocusCastles & selectCastles = SelectFocusCastles::Get();
    SelectFocusHeroes & selectHeroes = SelectFocusHeroes::Get();

    selectCastles.Reset();
    selectHeroes.Reset();

    globalfocus.type = Game::HEROES;
    globalfocus.center = hero->GetCenter();

    areaMaps.Center(globalfocus.center);
    radar.UpdatePosition();

    selectHeroes.SelectFromCenter(hero->GetCenter());
    selectHeroes.Redraw();
    selectCastles.Redraw();

    Cursor::Show();
}
