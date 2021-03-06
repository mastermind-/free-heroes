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

#include <cstdlib>
#include <iostream>
#include <string>

#include "engine.h"
#include "system.h"
#include "zzlib.h"
 
#include "agg/agg.h"
#include "game/game.h"
#include "gui/cursor.h"
#include "image/images_pack.h"
#include "system/gamedefs.h"
#include "system/settings.h"
#include "system/dir.h"
#include "test/test.h"

void LoadZLogo(void);
void SetVideoDriver(const std::string &);
void SetTimidityEnvPath(const Settings &);
void SetLangEnvPath(const Settings &);
void InitHomeDir(void);
void ReadConfigs(void);

int PrintHelp(const char *basename)
{
    VERBOSE("Usage: " << basename << " [OPTIONS]");
#ifndef BUILD_RELEASE
    VERBOSE("  -d\tdebug mode");
#endif
    VERBOSE("  -h\tprint this help and exit");

    return EXIT_SUCCESS;
}

std::string GetCaption(void)
{
    return std::string("Free Heroes II, version: " + Settings::GetVersion());
}

int main(int argc, char **argv)
{
	Settings & conf = Settings::Get();
	int test = 0;

	DEBUG(DBG_ALL, DBG_INFO, "Free Heroes II, " + conf.GetVersion());

	conf.SetProgramPath(argv[0]);

	InitHomeDir();
	ReadConfigs();

	// getopt
	{
	    int opt;
	    while((opt = System::GetCommandOptions(argc, argv, "hest:d:")) != -1)
    		switch(opt)
                {
#ifndef BUILD_RELEASE
                    case 't':
			test = GetInt(System::GetOptionsArgument());
			break;

                    case 'd':
                	conf.SetDebug(System::GetOptionsArgument() ? GetInt(System::GetOptionsArgument()) : 0);
                	break;
#endif
                    case '?':
                    case 'h': return PrintHelp(argv[0]);

                    default:  break;
		}
	}

	if(conf.SelectVideoDriver().size()) SetVideoDriver(conf.SelectVideoDriver());

	// random init
	Rand::Init();
        if(conf.Music()) SetTimidityEnvPath(conf);

	u32 subsystem = INIT_VIDEO | INIT_TIMER;

        if(conf.Sound() || conf.Music())
            subsystem |= INIT_AUDIO;
#ifdef WITH_AUDIOCD
        if(conf.MusicCD())
            subsystem |= INIT_CDROM | INIT_AUDIO;
#endif
	if(SDL::Init(subsystem))
#ifndef ANDROID
	try
#endif
	{
	    std::atexit(SDL::Quit);

	    if(conf.Unicode()) SetLangEnvPath(conf);

	    if(Mixer::isValid())
	    {
		Mixer::SetChannels(8);
                Mixer::Volume(-1, Mixer::MaxVolume() * conf.SoundVolume() / 10);
                Music::Volume(Mixer::MaxVolume() * conf.MusicVolume() / 10);
                if(conf.Music())
		{
		    Music::SetFadeIn(3000);
		}
	    }
	    else
	    if(conf.Sound() || conf.Music())
	    {
		conf.ResetSound();
		conf.ResetMusic();
	    }

	    if(0 == conf.VideoMode().w || 0 == conf.VideoMode().h)
	    	conf.SetAutoVideoMode();

            Display::SetVideoMode(conf.VideoMode().w, conf.VideoMode().h, conf.DisplayFlags());

	    Display::HideCursor();
	    Display::SetCaption(GetCaption().c_str());

    	    //Ensure the mouse position is updated to prevent bad initial values.
    	    LocalEvent::Get().GetMouseCursor();

#ifdef WITH_ZLIB
    	    ZSurface zicons;
	    if(zicons.Load(_ptr_08067830.width, _ptr_08067830.height, _ptr_08067830.bpp, _ptr_08067830.pitch,
    		_ptr_08067830.rmask, _ptr_08067830.gmask, _ptr_08067830.bmask, _ptr_08067830.amask, _ptr_08067830.zdata, sizeof(_ptr_08067830.zdata)))
	    Display::SetIcons(zicons);
#endif

            DEBUG(DBG_GAME, DBG_INFO, conf.String());
            DEBUG(DBG_GAME|DBG_ENGINE, DBG_INFO, Display::GetInfo());

	    // read data dir
	    if(! AGG::Init())
		return EXIT_FAILURE;

	    atexit(&AGG::Quit);

#ifdef WITH_ZLIB
	    LoadZLogo();
#endif

	    // init cursor
	    Cursor::Get().SetThemes(Cursor::POINTER);

	    // init game data
	    Game::Init();

	    // goto main menu
	    int rs = (test ? Game::TESTING : Game::MAINMENU);

	    while(rs != Game::QUITGAME)
	    {
		switch(rs)
		{
	    		case Game::MAINMENU:       rs = Game::MainMenu();		break;
	    		case Game::NEWGAME:        rs = Game::NewGame();		break;
	    		case Game::LOADGAME:       rs = Game::LoadGame();		break;
	    		case Game::HIGHSCORES:     rs = Game::HighScores(true);		break;
	    		case Game::CREDITS:        rs = Game::Credits();		break;
	    		case Game::NEWSTANDARD:    rs = Game::NewStandard();		break;
	    		case Game::NEWCAMPAIN:     rs = Game::NewCampain();		break;
	    		case Game::NEWMULTI:       rs = Game::NewMulti();		break;
			case Game::NEWHOTSEAT:     rs = Game::NewHotSeat();		break;
#ifdef NETWORK_ENABLE
		        case Game::NEWNETWORK:     rs = Game::NewNetwork();		break;
#endif
		        case Game::NEWBATTLEONLY:  rs = Game::NewBattleOnly();		break;
	    		case Game::LOADSTANDARD:   rs = Game::LoadStandard();		break;
	    		case Game::LOADCAMPAIN:    rs = Game::LoadCampain();		break;
	    		case Game::LOADMULTI:      rs = Game::LoadMulti();		break;
	    		case Game::SCENARIOINFO:   rs = Game::ScenarioInfo();		break;
	    		case Game::SELECTSCENARIO: rs = Game::SelectScenario();		break;
			case Game::STARTGAME:      rs = Game::StartGame();      	break;
		        case Game::TESTING:        rs = Game::Testing(test);		break;

	    		default: break;
		}
	    }
	}
#ifndef ANDROID
	catch(Error::Exception)
	{
	    VERBOSE(std::endl << conf.String());
	}
#endif
	return EXIT_SUCCESS;
}

void LoadZLogo(void)
{
#ifdef BUILD_RELEASE
#ifdef WITH_ZLIB
    // SDL logo
    if(Settings::Get().ExtGameShowSDL())
    {
	Display & display = Display::Get();
    	ZSurface zlogo;

	if(zlogo.Load(_ptr_0806f690.width, _ptr_0806f690.height, _ptr_0806f690.bpp, _ptr_0806f690.pitch,
    		_ptr_0806f690.rmask, _ptr_0806f690.gmask, _ptr_0806f690.bmask, _ptr_0806f690.amask, _ptr_0806f690.zdata, sizeof(_ptr_0806f690.zdata)))
	{
	    // scale logo
	    if(Settings::Get().QVGA())
	    {
		Surface small = Sprite::ScaleQVGA(zlogo);
		Surface::Swap(zlogo, small);
	    }

	    const u32 black = zlogo.MapRGB(0, 0, 0);
	    const Point offset((display.w() - zlogo.w()) / 2, (display.h() - zlogo.h()) / 2);

	    int ii = 0;

	    while(ii < 250)
	    {
		zlogo.Blit(ii, offset.x, offset.y, display);
		display.Flip();
		display.Fill(black);
		ii += 10;
	    }

	    DELAY(500);

	    while(ii > 0)
	    {
		zlogo.Blit(ii, offset.x, offset.y, display);
		display.Flip();
		display.Fill(black);
		ii -= 10;
	    }
	}
    }
#endif
#endif
}

void ReadConfigs(void)
{
    Settings & conf = Settings::Get();
    ListFiles files = conf.GetListFiles("", "fheroes2.cfg");

    for(ListFiles::const_iterator
	it = files.begin(); it != files.end(); ++it)
    	if(System::IsFile(*it)) conf.Read(*it);
}

void InitHomeDir(void)
{
    const std::string & home = Settings::GetHomeDir();

    if(! home.empty())
    {
	const std::string home_maps  = System::ConcatePath(home, "maps");
	const std::string home_files = System::ConcatePath(home, "files");
	const std::string home_files_save = System::ConcatePath(home_files, "save");

	if(! System::IsDirectory(home))
	    System::MakeDirectory(home);

	if(System::IsDirectory(home, true) && ! System::IsDirectory(home_maps))
	    System::MakeDirectory(home_maps);

	if(System::IsDirectory(home, true) && ! System::IsDirectory(home_files))
	    System::MakeDirectory(home_files);

	if(System::IsDirectory(home_files, true) && ! System::IsDirectory(home_files_save))
	    System::MakeDirectory(home_files_save);
    }
}

void SetVideoDriver(const std::string & driver)
{
    System::SetEnvironment("SDL_VIDEODRIVER", driver.c_str());
}

void SetTimidityEnvPath(const Settings & conf)
{
    const std::string prefix_timidity = System::ConcatePath("files", "timidity");
    const std::string result = Settings::GetLastFile(prefix_timidity, "timidity.cfg");

    if(System::IsFile(result))
	System::SetEnvironment("TIMIDITY_PATH", System::GetDirname(result).c_str());
}

void SetLangEnvPath(const Settings & conf)
{
#ifdef WITH_TTF
    if(conf.ForceLang().size())
    {
	System::SetEnvironment("LANGUAGE", conf.ForceLang().c_str());
	System::SetEnvironment("LANG", conf.ForceLang().c_str());
    }

    const std::string & strtmp = conf.GetLangDir();

    System::SetLocale("en_US.UTF8");
    bindtextdomain(GETTEXT_PACKAGE, strtmp.c_str());
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
#endif
}
