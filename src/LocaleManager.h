/*
* Realmlist -- manage your realmlists of World of Warcraft
* Copyright (C) 2014 SlaFF

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* his program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "global.h"
#include <Windows.h>
#include <string>
#include <map>

/*
Simple locale messages manager
Length of each message less or empty then 1024 letters

*/

class LocaleManager {
public:
	LocaleManager();
	~LocaleManager();

	void SetLocaleDir(const TCHAR* dir);
	/*
	Find localization file by name: "locale_" + szLocele + ".txt"
	and load the map ("English message" = "Localization message")

	@param szLocele (enUS, ruRU etc.)
	@return true if localization was load success
	*/
	bool SetLocale(const TCHAR* szLocele = DEFAULT_LOCALE);
	/**
	@param message Always in English!
	@return localized text or "" if it not found
	*/
	const wchar_t* GetText(const char* message, ...);


	static const TCHAR* DEFAULT_LOCALE;

protected:


private:
	AppString m_localeName;
	AppString m_localeDir;
	std::map<std::string, std::wstring> m_messages;
};