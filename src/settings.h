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

#include <tchar.h>
#include <string>

typedef std::basic_string<TCHAR> SettingString;

class Settings {
public:
	Settings();
	~Settings();

	bool SetParam(const SettingString name, const SettingString value);
	bool GetParam(const SettingString name, SettingString& value, SettingString defaultValue = NULL);

	int GetInt(const SettingString name);

private:

};