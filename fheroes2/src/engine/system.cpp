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

#include "system.h"

#include <cctype>
#include <cstdlib>
 
#include <fstream>
#include <locale>
#include <sstream>

#include <SDL.h>

#if defined(__MINGW32CE__) || defined(__MINGW32__)
#include <windows.h>
#include <shellapi.h>
#endif

#if ! defined(__MINGW32CE__)
#include <unistd.h>
#endif

#if defined(__MINGW32CE__)
#undef Shell_NotifyIcon
extern "C" {
    BOOL WINAPI Shell_NotifyIcon(DWORD, PNOTIFYICONDATAW);
}

// wincommon/SDL_sysevents.c
extern HICON screen_icn;
extern HINSTANCE SDL_Instance;
extern HWND SDL_Window;
#endif

#if defined(__WIN32__)
#include <io.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#if defined(__SYMBIAN32__)
#define SEPARATOR '\\'
#elif defined(__WIN32__)
#define SEPARATOR '\\'
#else
#define SEPARATOR '/'
#endif

int System::MakeDirectory(const std::string & path)
{
#if defined(__SYMBIAN32__)
    return mkdir(path.c_str(), S_IRWXU);
#elif defined(__WIN32__)
    return mkdir(path.c_str());
#else
    return mkdir(path.c_str(), S_IRWXU);
#endif
}

std::string System::ConcatePath(const std::string & str1, const std::string & str2)
{
    return std::string(str1 + SEPARATOR + str2);
}

std::string System::GetDirname(const std::string & str)
{
    if(str.size())
    {
        size_t pos = str.rfind(SEPARATOR);

        if(std::string::npos == pos)
            return std::string(".");
        else
        if(pos == 0)
            return std::string("./");
        else
        if(pos == str.size() - 1)
            return GetDirname(str.substr(0, str.size() - 1));
        else
            return str.substr(0, pos);
    }

    return str;
}

std::string System::GetBasename(const std::string & str)
{
    if(str.size())
    {
        size_t pos = str.rfind(SEPARATOR);

        if(std::string::npos == pos ||
            pos == 0) return str;
        else
        if(pos == str.size() - 1)
            return GetBasename(str.substr(0, str.size() - 1));
        else
            return str.substr(pos + 1);
    }

    return str;
}

const char* System::GetEnvironment(const char* name)
{
#if defined(__MINGW32CE__) || defined(__MINGW32__)
    return SDL_getenv(name);
#else
    return getenv(name);
#endif
}

int System::SetEnvironment(const char* name, const char* value)
{
#if defined(__MINGW32CE__) || defined(__MINGW32__)
    std::string str(std::string(name) + "=" + std::string(value));
    // SDL 1.2.12 (char *)
    return SDL_putenv(const_cast<char *>(str.c_str()));
#else
    return setenv(name, value, 1);
#endif
}

char* System::SetLocale(const char* locale)
{
#if ! defined(__MINGW32CE__)
    return setlocale(LC_ALL, locale);
#else
    return NULL;
#endif
}

int System::GetCommandOptions(int argc, char* const argv[], const char* optstring)
{
#if defined(__MINGW32CE__)
    return -1;
#else
    return getopt(argc, argv, optstring);
#endif
}

char* System::GetOptionsArgument(void)
{
#if defined(__MINGW32CE__)
    return NULL;
#else
    return optarg;
#endif
}

size_t System::GetMemoryUsage(void)
{
#if defined __SYMBIAN32__
    return 0;
#elif defined __WIN32__
    static MEMORYSTATUS ms;

    ZeroMemory(&ms, sizeof(ms));
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);

    return (ms.dwTotalVirtual - ms.dwAvailVirtual);
#elif defined __LINUX__
    unsigned int size = 0;
    std::ostringstream os;
    os << "/proc/" << getpid() << "/statm";

    std::ifstream fs(os.str().c_str());
    if(fs.is_open())
    {
        fs >> size;
        fs.close();
    }

    return size * getpagesize();
#else
    return 0;
#endif
}

std::string System::GetTime(void)
{
    time_t raw;
    struct tm* tmi;
    char buf [13] = { 0 };

    std::time(&raw);
    tmi = std::localtime(&raw);

    std::strftime(buf, sizeof(buf) - 1, "%X", tmi);

    return std::string(buf);
}

bool System::IsFile(const std::string & name, bool writable)
{
    struct stat fs;

    if(stat(name.c_str(), &fs) || !S_ISREG(fs.st_mode))
        return false;

#if defined(ANDROID)
    return writable ? 0 == access(name.c_str(), W_OK) : true;
#else
    return writable ? 0 == access(name.c_str(), W_OK) : S_IRUSR & fs.st_mode;
#endif
}

bool System::IsDirectory(const std::string & name, bool writable)
{
    struct stat fs;

    if(stat(name.c_str(), &fs) || !S_ISDIR(fs.st_mode))
        return false;

    return writable ? 0 == access(name.c_str(), W_OK) : S_IRUSR & fs.st_mode;
}

int System::Unlink(const std::string & file)
{
    return unlink(file.c_str());
}

int System::CreateTrayIcon(bool fl)
{
#if defined(__MINGW32CE__) && defined(ID_ICON)
    NOTIFYICONDATA nid = {0};
    nid.cbSize =  sizeof(nid);
    nid.uID = ID_ICON;
    nid.hWnd = SDL_Window;

    if(fl)
    {
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	nid.uCallbackMessage = WM_USER;
	nid.hIcon = ::LoadIcon(SDL_Instance, MAKEINTRESOURCE(ID_ICON));
	return Shell_NotifyIcon(NIM_ADD, &nid);
    }

    return Shell_NotifyIcon(NIM_DELETE, &nid);
#endif
    return 0;
}

void System::PowerManagerOff(bool fl)
{
#if defined(__MINGW32CE__)
    // power manager control
    const wchar_t lpGlobalSubKeyPM[] = TEXT("System\\CurrentControlSet\\Control\\Power\\Timeouts");
    const wchar_t lpNamePM[] = TEXT("BattSuspendTimeout");
    static DWORD origValuePM = 0;

    HKEY   hKey;

    if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpGlobalSubKeyPM, 0, KEY_ALL_ACCESS, &hKey))
    {
        DWORD dwType = REG_DWORD;
	DWORD value = 0;

	// save orig value
	if(fl)
	{
    	    DWORD valueLen = sizeof(origValuePM);

    	    if(ERROR_SUCCESS == RegQueryValueEx(hKey, lpNamePM, 0, &dwType, (LPBYTE) &origValuePM, &valueLen))
		RegSetValueEx(hKey, lpNamePM, 0, dwType, (const BYTE*) &value, sizeof(value));
	}
	else
	if(origValuePM)
	    RegSetValueEx(hKey, lpNamePM, 0, dwType, (const BYTE*) &origValuePM, sizeof(origValuePM));

        RegCloseKey(hKey);
    }

    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("PowerManager/ReloadActivityTimeouts"));

    if(hEvent != NULL)
    {
        SetEvent(hEvent);
        CloseHandle(hEvent);
    }

    // backlight control
    const wchar_t lpGlobalSubKeyBL[] = TEXT("\\ControlPanel\\Backlight");
    const wchar_t lpNameBL[] = TEXT("BatteryTimeout");
    static DWORD origValueBL = 0;

    if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, lpGlobalSubKeyBL, 0, KEY_ALL_ACCESS, &hKey))
    {
        DWORD dwType = REG_DWORD;
	DWORD value = 0;

	// save orig value
	if(fl)
	{
    	    DWORD valueLen = sizeof(origValueBL);

    	    if(ERROR_SUCCESS == RegQueryValueEx(hKey, lpNameBL, 0, &dwType, (LPBYTE) &origValueBL, &valueLen))
		RegSetValueEx(hKey, lpNameBL, 0, dwType, (const BYTE*) &value, sizeof(value));
	}
	else
	if(origValueBL)
	    RegSetValueEx(hKey, lpNameBL, 0, dwType, (const BYTE*) &origValueBL, sizeof(origValueBL));

        RegCloseKey(hKey);
    }

    hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("BackLightChangeEvent"));

    if(hEvent != NULL)
    {
        SetEvent(hEvent);
        CloseHandle(hEvent);
    }
#endif
}

bool System::isRunning(void)
{
#if defined(__MINGW32CE__)
    SetEnvironment("DEBUG_VIDEO", "1");
    SetEnvironment("DEBUG_VIDEO_GAPI", "1");

    HWND hwnd = FindWindow(NULL, L"SDL_app");

    if(hwnd)
    {
        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
    }

    return hwnd;
#endif

    return false;
}

int System::ShellCommand(const char* cmd)
{
#if defined(__MINGW32CE__)
    return cmd ? 0 : -1;
#else
    return system(cmd);
#endif
}
