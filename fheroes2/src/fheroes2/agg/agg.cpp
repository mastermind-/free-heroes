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

#include "agg/agg.h"
 
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "engine.h"
#include "font.h"
#include "system.h"
#include "game/game.h"
#include "gui/text.h"
#include "system/dir.h"
#include "system/settings.h"
#include "resource/artifact.h"

#ifdef WITH_ZLIB
#include "image/images_pack.h"
#include "zzlib.h"
#endif

#define FATSIZENAME	15

namespace AGG
{	
    class FAT
    {
    public:
	FAT() : crc(0), offset(0), size(0) {}

	u32		crc;
	u32		offset;
	u32		size;

	std::string	Info(void) const;
    };

    class File
    {
    public:
	File();
	~File();

	bool			Open(const std::string &);
	bool			isGood(void) const;
	const std::string &	Name(void) const;
	const FAT &		Fat(const std::string & key);

	const std::vector<u8> &	Read(const std::string & key);

    private:
	std::string			filename;
	std::map<std::string, FAT>	fat;
	u32				count_items;
	std::ifstream			stream;
	std::string			key;
	std::vector<u8>			body;
    };

    struct icn_cache_t
    {
	icn_cache_t() : sprites(NULL), reflect(NULL), count(0) {}

	Sprite*		sprites;
	Sprite*		reflect;
	u32		count;
    };

    struct til_cache_t
    {
	til_cache_t() : sprites(NULL),  count(0) {}

	Surface*	sprites;
	u32		count;
    };

    struct fnt_cache_t
    {
	Surface		sfs[4]; /* small_white, small_yellow, medium_white, medium_yellow */
    };

    struct loop_sound_t
    {
	loop_sound_t(int w, int c) : sound(w), channel(c) {}

	bool operator==(int m82) const { return m82 == sound; }

	int		sound;
	int		channel;
    };

    File					heroes2_agg;
    File					heroes2x_agg;

    std::vector<icn_cache_t>			icn_cache;
    std::vector<til_cache_t>			til_cache;

    std::map<int, std::vector<u8> >		wav_cache;
    std::map<int, std::vector<u8> >		mid_cache;
    std::vector<loop_sound_t>			loop_sounds;
    std::map<u32, fnt_cache_t>			fnt_cache;

    bool					memlimit_usage = true;

#ifdef WITH_TTF
    SDL::Font*			fonts; /* small, medium */

    void			LoadTTFChar(u32);
    Surface			GetFNT(u32, u32);
#endif

    const std::vector<u8> &	GetWAV(int m82);
    const std::vector<u8> &	GetMID(int xmi);

    void			LoadWAV(int m82, std::vector<u8> &);
    void			LoadMID(int xmi, std::vector<u8> &);

    bool			LoadExtICN(int icn, u32, bool);
    bool			LoadAltICN(int icn, u32, bool);
    bool			LoadOrgICN(Sprite &, int icn, u32, bool);
    bool			LoadOrgICN(int icn, u32, bool);
    void			LoadICN(int icn, u32, bool reflect = false);
    void			SaveICN(int icn);

    bool			LoadAltTIL(int til, u32 max);
    bool			LoadOrgTIL(int til, u32 max);
    void			LoadTIL(int til);

    void			LoadFNT(void);
    void			ShowError(void);

    bool			CheckMemoryLimit(void);
    u32				ClearFreeObjects(void);

    bool			ReadDataDir(void);
    const std::vector<u8> &	ReadICNChunk(int icn, u32);
    const std::vector<u8> &	ReadChunk(const std::string &);
}

/*AGG::File constructor */
AGG::File::File(void) : count_items(0)
{
}

bool AGG::File::Open(const std::string & fname)
{
    filename = fname;
    stream.open(filename.c_str(), std::ios::binary);

    if(! stream.is_open())
    {
	DEBUG(DBG_ENGINE, DBG_WARN, "error read file: " << filename << ", skipping...");
	return false;
    }

    stream.seekg(0, std::ios_base::end);
    const u32 size = stream.tellg();
    stream.seekg(0, std::ios_base::beg);

    count_items = StreamBase::getLE16(stream);
    DEBUG(DBG_ENGINE, DBG_INFO, "load: " << filename << ", count items: " << count_items);

    char buf[FATSIZENAME + 1];
    buf[FATSIZENAME] = 0;

    for(u32 ii = 0; ii < count_items; ++ii)
    {
	if(! stream.good())
	{
	    DEBUG(DBG_ENGINE, DBG_WARN, "stream error");
	    return false;
	}

        const u32 pos = stream.tellg();

        stream.seekg(size - FATSIZENAME * (count_items - ii), std::ios_base::beg);
        stream.read(buf, FATSIZENAME);

        FAT & f = fat[std::string(buf)];

        stream.seekg(pos, std::ios_base::beg);

	f.crc = StreamBase::getLE32(stream);
	f.offset = StreamBase::getLE32(stream);
	f.size = StreamBase::getLE32(stream);
    }

    return true;
}

AGG::File::~File()
{
    stream.close();
}

bool AGG::File::isGood(void) const
{
    return stream.good() && count_items;
}

/* get AGG file name */
const std::string & AGG::File::Name(void) const
{
    return filename;
}

/* get FAT element */
const AGG::FAT & AGG::File::Fat(const std::string & key)
{
    return fat[key];
}

/* dump FAT */
std::string AGG::FAT::Info(void) const
{
    std::ostringstream os;

    os << "crc: " << crc << ", offset: " << offset << ", size: " << size;
    return os.str();
}

/* read element to body */
const std::vector<u8> & AGG::File::Read(const std::string & str)
{
    if(key != str)
    {
	std::map<std::string, FAT>::const_iterator it = fat.find(str);

	if(it != fat.end())
	{
	    const FAT & f = (*it).second;
	    key = str;
	    body.resize(f.size);

	    if(f.size)
	    {
		DEBUG(DBG_ENGINE, DBG_TRACE, key << ":\t" << f.Info());

		stream.seekg(f.offset, std::ios_base::beg);
		stream.read(reinterpret_cast<char*>(&body[0]), f.size);
	    }
	}
	else
	if(body.size())
	{
	    body.clear();
	    key.clear();
	}
    }

    return body;
}

u32 AGG::ClearFreeObjects(void)
{
    u32 total = 0;

    // wav cache
    for(std::map<int, std::vector<u8> >::iterator
	it = wav_cache.begin(); it != wav_cache.end(); ++it)
	total += (*it).second.size();

    DEBUG(DBG_ENGINE, DBG_INFO, "WAV" << " " << "memory: " << total);
    total = 0;

    // mus cache
    for(std::map<int, std::vector<u8> >::iterator
	it = mid_cache.begin(); it != mid_cache.end(); ++it)
	total += (*it).second.size();

    DEBUG(DBG_ENGINE, DBG_INFO, "MID" << " " << "memory: " << total);
    total = 0;

#ifdef WITH_TTF
    // fnt cache
    for(std::map<u32, fnt_cache_t>::iterator
	it = fnt_cache.begin(); it != fnt_cache.end(); ++it)
    {
	total += (*it).second.sfs[0].GetMemoryUsage();
	total += (*it).second.sfs[1].GetMemoryUsage();
	total += (*it).second.sfs[2].GetMemoryUsage();
	total += (*it).second.sfs[3].GetMemoryUsage();
    }

    DEBUG(DBG_ENGINE, DBG_INFO, "FNT" << " " << "memory: " << total);
    total = 0;
#endif

    // til cache
    for(std::vector<til_cache_t>::iterator
	it = til_cache.begin(); it != til_cache.end(); ++it)
    {
	til_cache_t & tils = *it;

	for(u32 jj = 0; jj < tils.count; ++jj)
	    if(tils.sprites)
		total += tils.sprites[jj].GetMemoryUsage();
    }
    DEBUG(DBG_ENGINE, DBG_INFO, "TIL" << " " << "memory: " << total);
    total = 0;

    // icn cache
    u32 used = 0;

    for(std::vector<icn_cache_t>::iterator
	it = icn_cache.begin(); it != icn_cache.end(); ++it)
    {
	icn_cache_t & icns = (*it);

	for(u32 jj = 0; jj < icns.count; ++jj)
	{
	    if(icns.sprites)
	    {
		Sprite & sprite1 = icns.sprites[jj];

		if(! sprite1.isRefCopy())
		{
		    total += sprite1.GetMemoryUsage();
		    Surface::FreeSurface(sprite1);
		}
		else
		    used += sprite1.GetMemoryUsage();
	    }

	    if(icns.reflect)
	    {
		Sprite & sprite2 = icns.reflect[jj];

		if(! sprite2.isRefCopy())
		{
		    total += sprite2.GetMemoryUsage();
		    Surface::FreeSurface(sprite2);
		}
		else
		    used += sprite2.GetMemoryUsage();
	    }
	}
    }

    DEBUG(DBG_ENGINE, DBG_INFO, "ICN" << " " << "memory: " << used);

    return total;
}

bool AGG::CheckMemoryLimit(void)
{
    Settings & conf = Settings::Get();

    // memory limit trigger
    if(conf.ExtPocketLowMemory() && 0 < conf.MemoryLimit() && memlimit_usage)
    {
	u32 usage = System::GetMemoryUsage();

	if(0 < usage && conf.MemoryLimit() < usage)
	{
    	    VERBOSE("MemoryLimit: " << "settings: " << conf.MemoryLimit() << ", game usage: " << usage);
    	    const u32 freemem = ClearFreeObjects();
    	    VERBOSE("MemoryLimit: " << "free " << freemem);

    	    usage = System::GetMemoryUsage();

    	    if(conf.MemoryLimit() < usage + (300 * 1024))
    	    {
        	VERBOSE("MemoryLimit: " << "settings: " << conf.MemoryLimit() << ", too small");
        	// increase + 300Kb
        	conf.SetMemoryLimit(usage + (300 * 1024));
        	VERBOSE("MemoryLimit: " << "settings: " << "increase limit on 300kb, current value: " << conf.MemoryLimit());
    	    }

	    return true;
	}
    }

    return false;
}

/* read data directory */
bool AGG::ReadDataDir(void)
{
    Settings & conf = Settings::Get();
    ListFiles aggs = conf.GetListFiles("data", ".agg");
    const std::string & other_data = conf.GetDataParams();

    if(other_data.size() && other_data != "data")
	aggs.Append(conf.GetListFiles(other_data, ".agg"));

    // not found agg, exit
    if(0 == aggs.size()) return false;

    // attach agg files
    for(ListFiles::const_iterator
	it = aggs.begin(); it != aggs.end(); ++it)
    {
	std::string lower = StringLower(*it);
	if(std::string::npos != lower.find("heroes2.agg") && !heroes2_agg.isGood()) heroes2_agg.Open(*it);
	if(std::string::npos != lower.find("heroes2x.agg") && !heroes2x_agg.isGood()) heroes2x_agg.Open(*it);
    }

    if(heroes2x_agg.isGood()) conf.SetPriceLoyaltyVersion();

    return heroes2_agg.isGood();
}

const std::vector<u8> & AGG::ReadChunk(const std::string & key)
{
    if(heroes2x_agg.isGood())
    {
	const std::vector<u8> & buf = heroes2x_agg.Read(key);
	if(buf.size()) return buf;
    }

    return heroes2_agg.Read(key);
}


/* load manual ICN object */
bool AGG::LoadExtICN(int icn, u32 index, bool reflect)
{
    // for animation sprite need update count for ICN::AnimationFrame
    u32 count = 0;
    const Settings & conf = Settings::Get();

    switch(icn)
    {
	case ICN::BOAT12:		count = 1; break;
	case ICN::BATTLESKIP:
	case ICN::BATTLEWAIT:
	case ICN::BATTLEAUTO:
	case ICN::BATTLESETS:
	case ICN::BUYMAX:
	case ICN::BTNBATTLEONLY:
	case ICN::BTNGIFT:
	case ICN::BTNMIN:
	case ICN::BTNCONFIG:		count = 2; break;
	case ICN::FOUNTAIN:		count = 2; break;
	case ICN::TREASURE:		count = 2; break;
	case ICN::CSLMARKER:		count = 3; break;
	case ICN::TELEPORT1:
	case ICN::TELEPORT2:
	case ICN::TELEPORT3:		count = 8; break;
	case ICN::YELLOW_FONT:
	case ICN::YELLOW_SMALFONT:	count = 96; break;
	case ICN::ROUTERED:		count = 145; break;

	default: break;
    }

    // not modify sprite
    if(0 == count) return false;

    icn_cache_t & v = icn_cache[icn];
    DEBUG(DBG_ENGINE, DBG_TRACE, ICN::GetString(icn) << ", " << index);

    if(NULL == v.sprites)
    {
	v.sprites = new Sprite [count];
	v.reflect = new Sprite [count];
	v.count = count;
    }

    // simple modify
    if(index < count)
    {
	Sprite & sprite = reflect ? v.reflect[index] : v.sprites[index];

	memlimit_usage = false;

	switch(icn)
	{
	    case ICN::BTNBATTLEONLY:
		LoadOrgICN(sprite, ICN::BTNNEWGM, 2 + index, false);
		// clean
		GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 55, 14), 15, 13, sprite);
		GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 55, 14), 70, 13, sprite);
		GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 55, 14), 42, 28, sprite);
		// ba
		GetICN(ICN::BTNCMPGN, index).Blit(Rect(41, 28, 28, 14), 30, 13, sprite);
		// tt
		GetICN(ICN::BTNNEWGM, index).Blit(Rect(25, 13, 13, 14), 57, 13, sprite);
		GetICN(ICN::BTNNEWGM, index).Blit(Rect(25, 13, 13, 14), 70, 13, sprite);
		// le
		GetICN(ICN::BTNNEWGM, 6 + index).Blit(Rect(97, 21, 13, 14), 83, 13, sprite);
		GetICN(ICN::BTNNEWGM, 6 + index).Blit(Rect(86, 21, 13, 14), 96, 13, sprite);
		// on
		GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(44, 21, 31, 14), 40, 28, sprite);
		// ly
		GetICN(ICN::BTNHOTST, index).Blit(Rect(47, 21, 13, 13), 71, 28, sprite);
		GetICN(ICN::BTNHOTST, index).Blit(Rect(72, 21, 13, 13), 84, 28, sprite);
		break;

	    case ICN::BTNCONFIG:
		LoadOrgICN(sprite, ICN::SYSTEM, 11 + index, false);
		// config
		GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(30, 20, 80, 16), 8, 5, sprite);
		break;

	    case ICN::BTNGIFT:
		LoadOrgICN(sprite,
			(Settings::Get().ExtGameEvilInterface() ? ICN::TRADPOSE : ICN::TRADPOST),
			17 + index, false);
		// clean
		GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 72, 15), 6, 4, sprite);
		// G
		GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(94, 20, 15, 15), 20, 4, sprite);
		// I
		GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(86, 20, 9, 15), 36, 4, sprite);
		// F
		GetICN(ICN::BTNDCCFG, 4 + index).Blit(Rect(74, 20, 13, 15), 46, 4, sprite);
		// T
		GetICN(ICN::BTNNEWGM, index).Blit(Rect(25, 13, 13, 14), 60, 5, sprite);
		break;

	    case ICN::BTNMIN:
		// max
		LoadOrgICN(sprite, ICN::RECRUIT, index + 4, false);
		// clean
		GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 33, 15), 30, 4, sprite);
		// add: IN
		GetICN(ICN::APANEL, 4 + index).Blit(Rect(23, 20, 25, 15), 30, 4, sprite);
		break;

	    case ICN::BUYMAX:
		LoadOrgICN(sprite, ICN::WELLXTRA, index, false);
		// clean
		GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(10, 6, 52, 14), 6, 2, sprite);
		// max
		GetICN(ICN::RECRUIT, 4 + index).Blit(Rect(12, 6, 50, 12), 7, 3, sprite);
		break;

	    case ICN::BATTLESKIP:
		if(conf.PocketPC())
		    LoadOrgICN(sprite, ICN::TEXTBAR, index, false);
		else
		{
		    LoadOrgICN(sprite, ICN::TEXTBAR, 4 + index, false);
		    // clean
		    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(3, 8, 43, 14), 3, 1, sprite);
		    // skip
		    GetICN(ICN::TEXTBAR, index).Blit(Rect(3, 8, 43, 14), 3, 0, sprite);
		}
		break;

	    case ICN::BATTLEAUTO:
		LoadOrgICN(sprite, ICN::TEXTBAR, 0 + index, false);
		// clean
		GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(4, 8, 43, 13), 3, 10, sprite);
		//
		GetICN(ICN::TEXTBAR, 4 + index).Blit(Rect(5, 2, 40, 12), 4, 11, sprite);
		break;

	    case ICN::BATTLESETS:
		LoadOrgICN(sprite, ICN::TEXTBAR, 0 + index, false);
		// clean
		GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(4, 8, 43, 13), 3, 10, sprite);
		//
		GetICN(ICN::ADVBTNS, 14 + index).Blit(Rect(5, 5, 26, 26), 10, 6, sprite);
		break;

	    case ICN::BATTLEWAIT:
		if(conf.PocketPC())
		    LoadOrgICN(sprite, ICN::ADVBTNS, 8 + index, false);
		else
		{
		    LoadOrgICN(sprite, ICN::TEXTBAR, 4 + index, false);
		    // clean
		    GetICN(ICN::SYSTEM, 11 + index).Blit(Rect(3, 8, 43, 14), 3, 1, sprite);
		    // wait
		    Surface src(28, 28);
		    GetICN(ICN::ADVBTNS, 8 + index).Blit(Rect(5, 4, 28, 28), 0, 0, src);
		    Surface dst = Sprite::ScaleQVGA(src);
		    dst.Blit((sprite.w() - dst.w()) / 2, 2, sprite);
		}
		break;

	    case ICN::BOAT12:
	    {
		LoadOrgICN(sprite, ICN::ADVMCO, 28 + index, false);
		Surface dst = Sprite::ScaleQVGA(sprite);
		Surface::Swap(sprite, dst);
	    }
		break;

	    case ICN::CSLMARKER:
		// sprite: not allow build: complete, not today, all builds (white)
		LoadOrgICN(sprite, ICN::LOCATORS, 24, false);

		// sprite: not allow build: builds requires
		if(1 == index)
		    sprite.ChangeColorIndex(0x0A, 0xD6);
		else
		// sprite: not allow build: lack resources (green)
		if(2 == index)
		    sprite.ChangeColorIndex(0x0A, 0xDE);
		break;

	    default: break;
	}

	memlimit_usage = true;
    }

    // change color
    for(u32 ii = 0; ii < count; ++ii)
    {
	Sprite & sprite = reflect ? v.reflect[ii] : v.sprites[ii];

	switch(icn)
	{
	    case ICN::TELEPORT1:
		LoadOrgICN(sprite, ICN::OBJNMUL2, 116, false);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii / 2);
		break;

	    case ICN::TELEPORT2:
		LoadOrgICN(sprite, ICN::OBJNMUL2, 119, false);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii);
		break;

	    case ICN::TELEPORT3:
		LoadOrgICN(sprite, ICN::OBJNMUL2, 122, false);
		sprite.ChangeColorIndex(0xEE, 0xEE + ii);
		break;

	    case ICN::FOUNTAIN:
		LoadOrgICN(sprite, ICN::OBJNMUL2, 15, false);
		sprite.ChangeColorIndex(0xE8, 0xE8 - ii);
		sprite.ChangeColorIndex(0xE9, 0xE9 - ii);
		sprite.ChangeColorIndex(0xEA, 0xEA - ii);
		sprite.ChangeColorIndex(0xEB, 0xEB - ii);
		sprite.ChangeColorIndex(0xEC, 0xEC - ii);
		sprite.ChangeColorIndex(0xED, 0xED - ii);
		sprite.ChangeColorIndex(0xEE, 0xEE - ii);
		sprite.ChangeColorIndex(0xEF, 0xEF - ii);
		break;

	    case ICN::TREASURE:
		LoadOrgICN(sprite, ICN::OBJNRSRC, 19, false);
		sprite.ChangeColorIndex(0x0A, ii ? 0x00 : 0x0A);
		sprite.ChangeColorIndex(0xC2, ii ? 0xD6 : 0xC2);
		sprite.ChangeColorIndex(0x64, ii ? 0xDA : 0x64);
		break;

	    case ICN::ROUTERED:
		LoadOrgICN(sprite, ICN::ROUTE, ii, false);
		sprite.ChangeColorIndex(0x55, 0xB0);
		sprite.ChangeColorIndex(0x5C, 0xB7);
		sprite.ChangeColorIndex(0x60, 0xBB);
		break;

	    case ICN::YELLOW_FONT:
		LoadOrgICN(sprite, ICN::FONT, ii, false);
		sprite.ChangeColorIndex(0x0A, 0xDA);
		sprite.ChangeColorIndex(0x0B, 0xDA);
		sprite.ChangeColorIndex(0x0C, 0xDA);
		sprite.ChangeColorIndex(0x0D, 0xDA);
		sprite.ChangeColorIndex(0x0E, 0xDB);
		sprite.ChangeColorIndex(0x0F, 0xDB);
		sprite.ChangeColorIndex(0x10, 0xDB);
		sprite.ChangeColorIndex(0x11, 0xDB);
		sprite.ChangeColorIndex(0x12, 0xDB);
		sprite.ChangeColorIndex(0x13, 0xDB);
		sprite.ChangeColorIndex(0x14, 0xDB);
		break;

	    case ICN::YELLOW_SMALFONT:
		LoadOrgICN(sprite, ICN::SMALFONT, ii, false);
		sprite.ChangeColorIndex(0x0A, 0xDA);
		sprite.ChangeColorIndex(0x0B, 0xDA);
		sprite.ChangeColorIndex(0x0C, 0xDA);
		sprite.ChangeColorIndex(0x0D, 0xDA);
		sprite.ChangeColorIndex(0x0E, 0xDB);
		sprite.ChangeColorIndex(0x0F, 0xDB);
		sprite.ChangeColorIndex(0x10, 0xDB);
		sprite.ChangeColorIndex(0x11, 0xDB);
		sprite.ChangeColorIndex(0x12, 0xDB);
		sprite.ChangeColorIndex(0x13, 0xDB);
		sprite.ChangeColorIndex(0x14, 0xDB);
		break;

	    default: break;
	}
    }

    return true;
}

bool AGG::LoadAltICN(int icn, u32 index, bool reflect)
{
#ifdef WITH_XML
    const std::string prefix_images_icn = System::ConcatePath(System::ConcatePath("files", "images"), StringLower(ICN::GetString(icn)));
    const std::string xml_spec = Settings::GetLastFile(prefix_images_icn, "spec.xml");

    // parse spec.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_icn = NULL;

    if(doc.LoadFile(xml_spec.c_str()) &&
	NULL != (xml_icn = doc.FirstChildElement("icn")))
    {
	int count, ox, oy;
	xml_icn->Attribute("count", &count);
	icn_cache_t & v = icn_cache[icn];

	if(NULL == v.sprites)
	{
	    v.count = count;
	    v.sprites = new Sprite [v.count];
	    v.reflect = new Sprite [v.count];
	}

	// find current image
	const TiXmlElement *xml_sprite = xml_icn->FirstChildElement("sprite");
	int index1 = index;
	int index2 = 0;

	for(; xml_sprite && index2 != index1; xml_sprite = xml_sprite->NextSiblingElement("sprite"))
	    xml_sprite->Attribute("index", &index2);

	if(xml_sprite && index2 == index1)
	{
	    xml_sprite->Attribute("ox", &ox);
	    xml_sprite->Attribute("oy", &oy);
	    std::string name(xml_spec);
	    StringReplace(name, "spec.xml", xml_sprite->Attribute("name"));

	    Sprite & sp1 = v.sprites[index];
	    Sprite & sp2 = v.reflect[index];

	    if(! sp1.isValid() && System::IsFile(name) && sp1.Load(name.c_str()))
	    {
		sp1.SetOffset(ox, oy);
		DEBUG(DBG_ENGINE, DBG_TRACE, xml_spec << ", " << index);
		if(!reflect) return sp1.isValid();
	    }

	    if(reflect && sp1.isValid() && ! sp2.isValid())
	    {
		sp2 = Surface::Reflect(sp1, 2);
		sp2.SetOffset(ox, oy);
		return sp2.isValid();
	    }
	}

	DEBUG(DBG_ENGINE, DBG_WARN, "broken xml file: " <<  xml_spec);
    }
#endif

    return false;
}

void AGG::SaveICN(int icn)
{
#ifdef WITH_XML
#ifdef WITH_DEBUG
    const std::string images_dir = Settings::GetWriteableDir("images");

    if(images_dir.size())
    {
	icn_cache_t & v = icn_cache[icn];

        const std::string icn_lower = StringLower(ICN::GetString(icn));
	const std::string icn_dir = System::ConcatePath(images_dir, icn_lower);

	if(! System::IsDirectory(icn_dir))
		System::MakeDirectory(icn_dir);

	if(System::IsDirectory(icn_dir, true))
	{
	    const std::string stats_file = System::ConcatePath(icn_dir, "stats.xml");
	    bool need_save = false;
	    TiXmlDocument doc;
	    TiXmlElement* icn_element = NULL;

	    if(doc.LoadFile(stats_file.c_str()))
		icn_element = doc.FirstChildElement("icn");

	    if(! icn_element)
	    {
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
		doc.LinkEndChild(decl);
    
		icn_element = new TiXmlElement("icn");
		icn_element->SetAttribute("name", icn_lower.c_str());
		icn_element->SetAttribute("count", v.count);

		doc.LinkEndChild(icn_element);
		need_save = true;
	    }

	    for(u32 index = 0; index < v.count; ++index)
	    {

		const Sprite & sp = v.sprites[index];

		if(sp.isValid())
		{
		    std::ostringstream sp_name;
		    sp_name << std::setw(3) << std::setfill('0') << index;
#ifndef WITH_IMAGE
    		    sp_name << ".bmp";
#else
    		    sp_name << ".png";
#endif
		    const std::string image_full = System::ConcatePath(icn_dir, sp_name.str());

		    if(! System::IsFile(image_full))
		    {
			sp.Save(image_full);

			TiXmlElement* sprite_element = new TiXmlElement("sprite");
			sprite_element->SetAttribute("index", index);
			sprite_element->SetAttribute("name", sp_name.str().c_str());
			sprite_element->SetAttribute("ox", sp.x());
			sprite_element->SetAttribute("oy", sp.y());

			icn_element->LinkEndChild(sprite_element);

			need_save = true;
		    }
		}
	    }

	    if(need_save)
		doc.SaveFile(stats_file.c_str());
	}
    }
#endif
#endif
}

const std::vector<u8> & AGG::ReadICNChunk(int icn, u32 index)
{
    // hard fix artifact "ultimate stuff" sprite for loyalty version
    if(ICN::ARTIFACT == icn &&
	Artifact(Artifact::ULTIMATE_STAFF).IndexSprite64() == index && heroes2x_agg.isGood())
    {
	return heroes2x_agg.Read(ICN::GetString(icn));
    }

    return ReadChunk(ICN::GetString(icn));
}

struct ICNHeader
{
    ICNHeader() : offsetX(0), offsetY(0), width(0), height(0), type(0), offsetData(0) {}

    u16 offsetX;
    u16 offsetY;
    u16 width;
    u16 height;
    u8 type;
    u32 offsetData;
};

StreamBuf & operator>> (StreamBuf & st, ICNHeader & icn)
{
    icn.offsetX =  st.getLE16();
    icn.offsetY = st.getLE16();
    icn.width = st.getLE16();
    icn.height = st.getLE16();
    icn.type = st.get();
    icn.offsetData = st.getLE32();

    return st;
}

bool AGG::LoadOrgICN(Sprite & sp, int icn, u32 index, bool reflect)
{
    const std::vector<u8> & body = ReadICNChunk(icn, index);

    if(body.size())
    {
	// loading original
	DEBUG(DBG_ENGINE, DBG_TRACE, ICN::GetString(icn) << ", " << index);

	StreamBuf st(body);

	u32 count = st.getLE16();
	u32 blockSize = st.getLE32();
	u32 sizeData = 0;

	if(index) st.skip(index * 13);

	ICNHeader header1;
	st >> header1;

	if(index + 1 != count)
	{
	    ICNHeader header2;
	    st >> header2;
	    sizeData = header2.offsetData - header1.offsetData;
	}
	else
	    sizeData = blockSize - header1.offsetData;

	sp.Set(header1.width, header1.height, false);
	sp.SetOffset(header1.offsetX, header1.offsetY);
	Sprite::DrawICN(icn, &body[6 + header1.offsetData], sizeData, reflect, sp);
	Sprite::AddonExtensionModify(sp, icn, index);

	return true;
    }

    DEBUG(DBG_ENGINE, DBG_WARN, "error: " << ICN::GetString(icn));

    return false;
}

bool AGG::LoadOrgICN(int icn, u32 index, bool reflect)
{
    icn_cache_t & v = icn_cache[icn];

    if(NULL == v.sprites)
    {
	const std::vector<u8> & body = ReadChunk(ICN::GetString(icn));

	if(body.size())
	{
	    v.count = StreamBase::getLE16(& body[0]);
	    v.sprites = new Sprite [v.count];
	    v.reflect = new Sprite [v.count];
	}
	else
	    return false;
    }

    Sprite & sp = reflect ? v.reflect[index] : v.sprites[index];

    return LoadOrgICN(sp, icn, index, reflect);
}

/* load ICN object */
void AGG::LoadICN(int icn, u32 index, bool reflect)
{
    icn_cache_t & v = icn_cache[icn];

    // need load
    if((reflect && (!v.reflect || (index < v.count && !v.reflect[index].isValid()))) ||
	(!reflect && (!v.sprites || (index < v.count && !v.sprites[index].isValid()))))
    {
	const Settings & conf = Settings::Get();

	// load from images dir
	if(! conf.UseAltResource() ||
	    ! LoadAltICN(icn, index, reflect))
	{
	    // load modify sprite
	    if(! LoadExtICN(icn, index, reflect))
	    {
		//load origin sprite
		if(! LoadOrgICN(icn, index, reflect))
		    Error::Except(__FUNCTION__, "load icn");
	    }
	}

	// pocketpc: scale sprites
	if(Settings::Get().QVGA() && ICN::NeedMinify4PocketPC(icn, index))
	{
	    Sprite & sp = reflect ? v.reflect[index] : v.sprites[index];
	    sp.ScaleQVGA();
	}
    }
}

/* return ICN sprite */
Sprite AGG::GetICN(int icn, u32 index, bool reflect)
{
    Sprite result;

    if(icn < static_cast<int>(icn_cache.size()))
    {
	icn_cache_t & v = icn_cache[icn];

	// out of range?
	if(v.count && index >= v.count)
	{
	    DEBUG(DBG_ENGINE, DBG_WARN, ICN::GetString(icn) << ", " << "out of range: " << index);
	    index = 0;
	}

	// need load?
	if(0 == v.count || ((reflect && (!v.reflect || !v.reflect[index].isValid())) || (!v.sprites || !v.sprites[index].isValid())))
	{
	    CheckMemoryLimit();
	    LoadICN(icn, index, reflect);
	}

	result = reflect ? v.reflect[index] : v.sprites[index];

	// invalid sprite?
	if(! result.isValid())
	{
	    DEBUG(DBG_ENGINE, DBG_INFO, "invalid sprite: " << ICN::GetString(icn) << ", index: " << index << ", reflect: " << (reflect ? "true" : "false"));
	}
    }

    return result;
}

/* return count of sprites from specific ICN */
u32 AGG::GetICNCount(int icn)
{
    if(icn_cache[icn].count == 0) AGG::GetICN(icn, 0);
    return icn_cache[icn].count;
}


int AGG::PutICN(const Sprite & sprite, bool init_reflect)
{
    icn_cache_t v;

    v.count = 1;
    v.sprites = new Sprite[1];
    v.sprites[0] = sprite;

    if(init_reflect)
    {
	v.reflect = new Sprite[1];
        v.reflect[0] = Surface::Reflect(sprite, 2);
	v.reflect[0].SetOffset(sprite.x(), sprite.y());
    }

    icn_cache.push_back(v);
    return icn_cache.size() - 1;
}

bool AGG::LoadAltTIL(int til, u32 max)
{
#ifdef WITH_XML
    const std::string prefix_images_til = System::ConcatePath(System::ConcatePath("files", "images"), StringLower(TIL::GetString(til)));
    const std::string xml_spec = Settings::GetLastFile(prefix_images_til, "spec.xml");

    // parse spec.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_til = NULL;

    if(doc.LoadFile(xml_spec.c_str()) &&
	NULL != (xml_til = doc.FirstChildElement("til")))
    {
	int count, index;
	xml_til->Attribute("count", &count);
	til_cache_t & v = til_cache[til];

	if(NULL == v.sprites)
	{
	    v.count = count;
	    v.sprites = new Surface [v.count];
	}

	index = 0;
	for(const TiXmlElement*
	    xml_sprite = xml_til->FirstChildElement("sprite"); xml_sprite; ++index, xml_sprite = xml_sprite->NextSiblingElement("sprite"))
	{
	    xml_sprite->Attribute("index", &index);

	    if(index < count)
	    {
		Surface & sf = v.sprites[index];
		std::string name(xml_spec);
		StringReplace(name, "spec.xml", xml_sprite->Attribute("name"));

		if(System::IsFile(name))
		    sf.Load(name.c_str());
		else
		    DEBUG(DBG_ENGINE, DBG_TRACE, "load til" << ": " << name);

		if(! sf.isValid())
		    return false;
	    }
	}

	return true;
    }
    else
	DEBUG(DBG_ENGINE, DBG_WARN, "broken xml file: " << xml_spec);
#endif

    return false;
}

bool AGG::LoadOrgTIL(int til, u32 max)
{
    const std::vector<u8> & body = ReadChunk(TIL::GetString(til));

    if(body.size())
    {
	StreamBuf st(body);

	u32 count = st.getLE16();
	u32 width = st.getLE16();
	u32 height= st.getLE16();

	u32 tile_size = width * height;
	u32 body_size = 6 + count * tile_size;

	til_cache_t & v = til_cache[til];

	// check size
	if(body.size() == body_size && count <= max)
	{
	    for(u32 ii = 0; ii < count; ++ii)
		v.sprites[ii].Set(&body[6 + ii * tile_size], width, height, 1, false);

	    return true;
	}
	else
	{
	    DEBUG(DBG_ENGINE, DBG_WARN, "size mismach" << ", skipping...");
	}
    }

    return false;
}

/* load TIL object to AGG::Cache */
void AGG::LoadTIL(int til)
{
    til_cache_t & v = til_cache[til];

    if(! v.sprites)
    {
	DEBUG(DBG_ENGINE, DBG_INFO, TIL::GetString(til));
	u32 max = 0;

	switch(til)
	{
	    case TIL::CLOF32:	max = 4;   break;
    	    case TIL::GROUND32:	max = 432; break;
    	    case TIL::STON:	max = 36;  break;
	    default: break;
	}

	v.count = max * 4;  // rezerve for rotate sprites
	v.sprites = new Surface [v.count];

	const Settings & conf = Settings::Get();

	// load from images dir
	if(! conf.UseAltResource() || ! LoadAltTIL(til, max))
	{
	    if(! LoadOrgTIL(til, max))
		Error::Except(__FUNCTION__, "load til");
	}
    }
}

/* return TIL surface from AGG::Cache */
Surface AGG::GetTIL(int til, u32 index, u32 shape)
{
    Surface result;

    if(til < static_cast<int>(til_cache.size()))
    {
	til_cache_t & v = til_cache[til];

	if(0 == v.count) LoadTIL(til);
	u32 index2 = index;

	if(shape)
	{
	    switch(til)
	    {
		case TIL::STON:     index2 += 36 * (shape % 4); break;
		case TIL::CLOF32:   index2 += 4 * (shape % 4); break;
		case TIL::GROUND32: index2 += 432 * (shape % 4); break;
		default: break;
	    }
	}

	if(index2 >= v.count)
	{
	    DEBUG(DBG_ENGINE, DBG_WARN, TIL::GetString(til) << ", " << "out of range: " << index);
	    index2 = 0;
	}

	Surface & surface = v.sprites[index2];

	if(shape && ! surface.isValid())
	{
	    const Surface & src = v.sprites[index];

	    if(src.isValid())
		surface = Surface::Reflect(src, shape);
	    else
		DEBUG(DBG_ENGINE, DBG_WARN, "is NULL");
	}

	if(! surface.isValid())
	{
	    DEBUG(DBG_ENGINE, DBG_WARN, "invalid sprite: " << TIL::GetString(til) << ", index: " << index);
	}

	result = surface;
    }

    return result;
}

/* load 82M object to AGG::Cache in Audio::CVT */
void AGG::LoadWAV(int m82, std::vector<u8> & v)
{
#ifdef WITH_MIXER
    const Settings & conf = Settings::Get();

    if(conf.UseAltResource())
    {
	std::string name = StringLower(M82::GetString(m82));
	std::string prefix_sounds = System::ConcatePath("files", "sounds");

	// ogg
	StringReplace(name, ".82m", ".ogg");
	std::string sound = Settings::GetLastFile(prefix_sounds, name);
	v = LoadFileToMem(sound);

	if(v.empty())
	{
	    // find mp3
	    StringReplace(name, ".82m", ".mp3");
	    sound = Settings::GetLastFile(prefix_sounds, name);

	    v = LoadFileToMem(sound);
	}

	if(v.size())
	{
	    DEBUG(DBG_ENGINE, DBG_INFO, sound);
	    return;
	}
    }
#endif

    DEBUG(DBG_ENGINE, DBG_INFO, M82::GetString(m82));
    const std::vector<u8> & body = ReadChunk(M82::GetString(m82));

    if(body.size())
    {
#ifdef WITH_MIXER
	// create WAV format
	StreamBuf wavHeader(44);
	wavHeader.putLE32(0x46464952);		// RIFF
	wavHeader.putLE32(body.size() + 0x24);	// size
	wavHeader.putLE32(0x45564157);		// WAVE
	wavHeader.putLE32(0x20746D66);		// FMT
	wavHeader.putLE32(0x10);		// size_t
	wavHeader.putLE16(0x01);		// format
	wavHeader.putLE16(0x01);		// channels
	wavHeader.putLE32(22050);		// samples
	wavHeader.putLE32(22050);		// byteper
	wavHeader.putLE16(0x01);		// align
	wavHeader.putLE16(0x08);		// bitsper
	wavHeader.putLE32(0x61746164);		// DATA
	wavHeader.putLE32(body.size());		// size

	v.reserve(body.size() + 44);
	v.assign(wavHeader.data(), wavHeader.data() + 44);
	v.insert(v.begin() + 44, body.begin(), body.end());
#else
	Audio::Spec wav_spec;
	wav_spec.format = AUDIO_U8;
	wav_spec.channels = 1;
	wav_spec.freq = 22050;

	const Audio::Spec & hardware = Audio::GetHardwareSpec();

	Audio::CVT cvt;

	if(cvt.Build(wav_spec, hardware))
	{
	    const u32 size = cvt.len_mult * body.size();

	    cvt.buf = new u8[size];
	    cvt.len = body.size();

	    memcpy(cvt.buf, &body[0], body.size());

	    cvt.Convert();

	    v.assign(cvt.buf, cvt.buf + size - 1);

	    delete [] cvt.buf;
	    cvt.buf = NULL;
	}
#endif
    }
}

/* load XMI object */
void AGG::LoadMID(int xmi, std::vector<u8> & v)
{
    DEBUG(DBG_ENGINE, DBG_INFO, XMI::GetString(xmi));
    const std::vector<u8> & body = ReadChunk(XMI::GetString(xmi));

    if(body.size())
        v = Music::Xmi2Mid(body);
}

/* return CVT */
const std::vector<u8> & AGG::GetWAV(int m82)
{
    std::vector<u8> & v = wav_cache[m82];
    if(Mixer::isValid() && v.empty()) LoadWAV(m82, v);
    return v;
}

/* return MID */
const std::vector<u8> & AGG::GetMID(int xmi)
{
    std::vector<u8> & v = mid_cache[xmi];
    if(Mixer::isValid() && v.empty()) LoadMID(xmi, v);
    return v;
}

void AGG::LoadLOOPXXSounds(const std::vector<int> & vols)
{
    const Settings & conf = Settings::Get();

    if(conf.Sound())
    {
	// set volume loop sounds
	for(std::vector<int>::const_iterator
	    itv = vols.begin(); itv != vols.end(); ++itv)
	{
	    int vol = *itv;
	    int m82 = M82::GetLOOP00XX(std::distance(vols.begin(), itv));
	    if(M82::UNKNOWN == m82) continue;

	    // find loops
	    std::vector<loop_sound_t>::iterator itl = std::find(loop_sounds.begin(), loop_sounds.end(), m82);

	    if(itl != loop_sounds.end())
	    {
		// unused and free
		if(0 == vol)
		{
		    if(Mixer::isPlaying((*itl).channel))
		    {
			Mixer::Pause((*itl).channel);
			Mixer::Volume((*itl).channel, Mixer::MaxVolume() * conf.SoundVolume() / 10);
			Mixer::Stop((*itl).channel);
		    }
		    (*itl).sound = M82::UNKNOWN;
		}
		// used and set vols
		else
		if(Mixer::isPlaying((*itl).channel))
		{
		    Mixer::Pause((*itl).channel);
		    Mixer::Volume((*itl).channel, vol * conf.SoundVolume() / 10);
		    Mixer::Resume((*itl).channel);
		}
	    }
	    else
	    // new sound
	    if(0 != vol)
	    {
    		const std::vector<u8> & v = GetWAV(m82);
		int ch = Mixer::Play(&v[0], v.size(), -1, true);

		if(0 <= ch)
		{
		    Mixer::Pause(ch);
		    Mixer::Volume(ch, vol * conf.SoundVolume() / 10);
		    Mixer::Resume(ch);

		    // find unused
		    std::vector<loop_sound_t>::iterator itl = std::find(loop_sounds.begin(), loop_sounds.end(), static_cast<int>(M82::UNKNOWN));

		    if(itl != loop_sounds.end())
		    {
			(*itl).sound = m82;
			(*itl).channel = ch;
		    }
		    else
			loop_sounds.push_back(loop_sound_t(m82, ch));

		    DEBUG(DBG_ENGINE, DBG_INFO, M82::GetString(m82));
		}
	    }
	}
    }
}

/* wrapper Audio::Play */
void AGG::PlaySound(int m82)
{
    const Settings & conf = Settings::Get();

    if(conf.Sound())
    {
	DEBUG(DBG_ENGINE, DBG_INFO, M82::GetString(m82));
	const std::vector<u8> & v = AGG::GetWAV(m82);
	int ch = Mixer::Play(&v[0], v.size(), -1, false);
	Mixer::Pause(ch);
	Mixer::Volume(ch, Mixer::MaxVolume() * conf.SoundVolume() / 10);
	Mixer::Resume(ch);
    }
}

/* wrapper Audio::Play */
void AGG::PlayMusic(int mus, bool loop)
{
    const Settings & conf = Settings::Get();

    if(!conf.Music() || MUS::UNUSED == mus || MUS::UNKNOWN == mus || (Game::CurrentMusic() == mus && Music::isPlaying())) return;

    Game::SetCurrentMusic(mus);
    const std::string prefix_music = System::ConcatePath("files", "music");

    if(conf.MusicExt())
    {
	const std::string musname = Settings::GetLastFile(prefix_music, MUS::GetString(mus));

#ifdef WITH_MIXER
	std::string shortname = Settings::GetLastFile(prefix_music, MUS::GetString(mus, true));
	const char* filename = NULL;

	if(System::IsFile(musname))   filename = musname.c_str();
	else
	if(System::IsFile(shortname)) filename = shortname.c_str();
	else
	{
	    StringReplace(shortname, ".ogg", ".mp3");
	    if(System::IsFile(shortname)) filename = shortname.c_str();
	    else
		DEBUG(DBG_ENGINE, DBG_WARN, "error read file: " << musname << ", skipping...");
	}

	if(filename) Music::Play(filename, loop);
#else
	if(System::IsFile(musname) && conf.PlayMusCommand().size())
	{
	    const std::string run = conf.PlayMusCommand() + " " + musname;
	    Music::Play(run.c_str(), loop);
	}
#endif
	DEBUG(DBG_ENGINE, DBG_INFO, MUS::GetString(mus));
    }
    else
#ifdef WITH_AUDIOCD
    if(conf.MusicCD() && Cdrom::isValid())
    {
	Cdrom::Play(mus, loop);
	DEBUG(DBG_ENGINE, DBG_INFO, "cd track " << static_cast<int>(mus));
    }
    else
#endif
    if(conf.MusicMIDI())
    {
	int xmi = XMI::FromMUS(mus);
	if(XMI::UNKNOWN != xmi)
	{
#ifdef WITH_MIXER
	    const std::vector<u8> & v = GetMID(xmi);
	    if(v.size()) Music::Play(&v[0], v.size(), loop);
#else
	    if(conf.PlayMusCommand().size())
	    {
		const std::string file = Settings::GetLastFile(prefix_music, XMI::GetString(xmi));

		if(System::IsFile(file))
		{
		    const std::string run = conf.PlayMusCommand() + " " + file;
		    Music::Play(run.c_str(), loop);
		}
		else
		    SaveMemToFile(GetMID(xmi), file);
	    }
#endif
	}
	DEBUG(DBG_ENGINE, DBG_INFO, XMI::GetString(xmi));
    }
}

#ifdef WITH_TTF
void AGG::LoadTTFChar(u32 ch)
{
    const Settings & conf = Settings::Get();
    const RGBColor white = { 0xFF, 0xFF, 0xFF, 0x00 };
    const RGBColor yellow= { 0xFF, 0xFF, 0x00, 0x00 };
	    
    // small
    fonts[0].RenderUnicodeChar(fnt_cache[ch].sfs[0], ch, white, conf.FontSmallRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
    fonts[0].RenderUnicodeChar(fnt_cache[ch].sfs[1], ch, yellow, conf.FontSmallRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);

    // medium
    if(!(conf.QVGA() && !conf.Unicode()))
    {
	fonts[1].RenderUnicodeChar(fnt_cache[ch].sfs[2], ch, white, conf.FontNormalRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
	fonts[1].RenderUnicodeChar(fnt_cache[ch].sfs[3], ch, yellow, conf.FontNormalRenderBlended() ? SDL::Font::BLENDED : SDL::Font::SOLID);
    }

    DEBUG(DBG_ENGINE, DBG_TRACE, "0x" << std::hex << ch);
}

void AGG::LoadFNT(void)
{
    const Settings & conf = Settings::Get();

    if(! conf.Unicode())
    {
	DEBUG(DBG_ENGINE, DBG_INFO, "use bitmap fonts");
    }
    else
    if(fnt_cache.empty())
    {
	const std::string letters = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	std::vector<u16> unicode = StringUTF8_to_UNICODE(letters);

	for(std::vector<u16>::const_iterator
	    it = unicode.begin(); it != unicode.end(); ++it)
	    LoadTTFChar(*it);

	if(fnt_cache.empty())
	{
	    DEBUG(DBG_ENGINE, DBG_INFO, "use bitmap fonts");
	}
	else
	{
    	    DEBUG(DBG_ENGINE, DBG_INFO, "normal fonts " << conf.FontsNormal());
    	    DEBUG(DBG_ENGINE, DBG_INFO, "small fonts " << conf.FontsSmall());	
	    DEBUG(DBG_ENGINE, DBG_INFO, "preload english charsets");
	}
    }
}

u32 AGG::GetFontHeight(bool small)
{
    return small ? fonts[0].Height() : fonts[1].Height();
}

/* return letter sprite */
Surface AGG::GetUnicodeLetter(u32 ch, u32 ft)
{
    bool ttf_valid = fonts[0].isValid() && fonts[0].isValid();

    if(! ttf_valid)
        return GetLetter(ch, ft);

    if(!fnt_cache[ch].sfs[0].isValid()) LoadTTFChar(ch);

    switch(ft)
    {
	case Font::YELLOW_SMALL: return fnt_cache[ch].sfs[1];
	case Font::BIG:		 return fnt_cache[ch].sfs[2];
	case Font::YELLOW_BIG:	 return fnt_cache[ch].sfs[3];
	default: break;
    }

    return fnt_cache[ch].sfs[0];
}
#else
void AGG::LoadFNT(void)
{
    DEBUG(DBG_ENGINE, DBG_INFO, "use bitmap fonts");
}
#endif

Surface AGG::GetLetter(u32 ch, u32 ft)
{
    if(ch < 0x21) DEBUG(DBG_ENGINE, DBG_WARN, "unknown letter");

    switch(ft)
    {
	case Font::YELLOW_BIG:	return AGG::GetICN(ICN::YELLOW_FONT, ch - 0x20);
	case Font::YELLOW_SMALL:return AGG::GetICN(ICN::YELLOW_SMALFONT, ch - 0x20);
	case Font::BIG:		return AGG::GetICN(ICN::FONT, ch - 0x20);
	case Font::SMALL:	return AGG::GetICN(ICN::SMALFONT, ch - 0x20);

	default: break;
    }

    return AGG::GetICN(ICN::SMALFONT, ch - 0x20);
}

void AGG::ResetMixer(void)
{
    Mixer::Reset();
    loop_sounds.clear();
    loop_sounds.reserve(7);
}

void AGG::ShowError(void)
{
#ifdef WITH_ZLIB
    ZSurface zerr;
    if(zerr.Load(_ptr_080721d0.width, _ptr_080721d0.height, _ptr_080721d0.bpp, _ptr_080721d0.pitch,
            _ptr_080721d0.rmask, _ptr_080721d0.gmask, _ptr_080721d0.bmask, _ptr_080721d0.amask, _ptr_080721d0.zdata, sizeof(_ptr_080721d0.zdata)))
    {
        Display & display = Display::Get();
        LocalEvent & le = LocalEvent::Get();

        display.Fill(zerr.MapRGB(0, 0, 0));
        zerr.Blit((display.w() - zerr.w()) / 2, (display.h() - zerr.h()) / 2, display);
        display.Flip();

        while(le.HandleEvents() && !le.KeyPress() && !le.MouseClickLeft());
    }
#endif
}

bool AGG::Init(void)
{
    // read data dir
    if(! ReadDataDir())
    {
        DEBUG(DBG_ENGINE, DBG_WARN, "data files not found");
        ShowError();
        return false;
    }

#ifdef WITH_TTF
    Settings & conf = Settings::Get();
    const std::string prefix_fonts = System::ConcatePath("files", "fonts");
    const std::string font1 = Settings::GetLastFile(prefix_fonts, conf.FontsNormal());
    const std::string font2 = Settings::GetLastFile(prefix_fonts, conf.FontsSmall());

    fonts = new SDL::Font[2];

    if(conf.Unicode())
    {
	if(!fonts[1].Open(font1, conf.FontsNormalSize()) ||
	   !fonts[0].Open(font2, conf.FontsSmallSize())) conf.SetUnicode(false);
    }
#endif

    icn_cache.reserve(ICN::LASTICN + 256);
    icn_cache.resize(ICN::LASTICN);

    til_cache.resize(TIL::LASTTIL);

    // load font
    LoadFNT();

    return true;
}

void AGG::Quit(void)
{
    for(std::vector<icn_cache_t>::iterator
	it = icn_cache.begin(); it != icn_cache.end(); ++it)
    {
	icn_cache_t & icns = (*it);

	if(icns.sprites)
	{
	    int icn_num = std::distance(icn_cache.begin(), it);
	    if(icn_num < ICN::LASTICN &&
		Settings::Get().UseAltResource()) SaveICN(icn_num);
		delete [] icns.sprites;
	}
	icns.sprites = NULL;
	if(icns.reflect) delete [] icns.reflect;
	icns.reflect = NULL;
    }

    for(std::vector<til_cache_t>::iterator
	it = til_cache.begin(); it != til_cache.end(); ++it)
    {
	til_cache_t & tils = (*it);
	if(tils.sprites) delete [] tils.sprites;
    }

#ifdef WITH_TTF
    delete [] fonts;
#endif
}
