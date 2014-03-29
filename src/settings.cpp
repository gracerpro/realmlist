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
#include "settings.h"
#include <Windows.h>

Settings::Settings() {

}

Settings::~Settings() {


}

static const TCHAR regSettingsPath[] = TEXT("Software\\Realmlist");

bool Settings::SetParam(const SettingString name, const SettingString value) {
	DWORD dwDisposition = 0;
	HKEY hKey = NULL;

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, regSettingsPath, 0, NULL,
		0, KEY_WRITE, NULL, &hKey, &dwDisposition)) {

		const BYTE* data = reinterpret_cast<const BYTE*>(value.c_str());
		DWORD cbData = static_cast<DWORD>(name.size());
		if (ERROR_SUCCESS == RegSetValueEx(hKey, name.c_str(), 0, REG_SZ, data, cbData)) {
			return true;
		}
	}

	return false;
}

bool Settings::GetParam(const SettingString name, SettingString& value, SettingString defaultValue) {
	DWORD dwDisposition = 0;
	HKEY hKey = NULL;

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, regSettingsPath, 0, NULL,
		0, KEY_READ, NULL, &hKey, &dwDisposition)) {

		const BYTE* data = reinterpret_cast<const BYTE*>(value.c_str());
		DWORD cbData = static_cast<DWORD>(name.size());
		if (ERROR_SUCCESS == RegSetValueEx(hKey, name.c_str(), 0, REG_SZ, data, cbData)) {
			return true;
		}
	}

	value = defaultValue;

	return true;
}

int Settings::GetInt(const SettingString name) {
	SettingString value;

	GetParam(name, value, TEXT("0"));

	return _tstoi(name.c_str());
}