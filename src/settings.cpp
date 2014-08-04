/*
 * WowServerManager -- manage your servers of World of Warcraft
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

bool Settings::SetParam(const TCHAR* name, const BYTE* data, DWORD size, DWORD type) {
	DWORD dwDisposition = 0;
	HKEY hKey = NULL;

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, regSettingsPath, 0, NULL,
		0, KEY_WRITE, NULL, &hKey, &dwDisposition)) {

		if (ERROR_SUCCESS == RegSetValueEx(hKey, name, 0, type, data, size)) {
			return true;
		}
		RegCloseKey(hKey);
	}

	return false;
}

bool Settings::GetParam(const TCHAR* name, BYTE* value, DWORD size, DWORD type) {
	DWORD dwDisposition = 0;
	HKEY hKey = NULL;

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, regSettingsPath, 0, NULL,
		0, KEY_READ, NULL, &hKey, &dwDisposition)) {

		DWORD cbData = size;
		if (ERROR_SUCCESS == RegQueryValueEx(hKey, name, 0, &type, value, &cbData)) {
			return true;
		}
		RegCloseKey(hKey);
	}

	return false;
}

int Settings::GetInt(const TCHAR* name, int defaultValue) {
	DWORD dw;

	if (!GetParam(name, reinterpret_cast<BYTE*>(&dw), sizeof(DWORD), REG_DWORD)) {
		dw = defaultValue;
	}

	return dw;
}

bool Settings::SetInt(const TCHAR* name, int value) {
	return SetParam(name, reinterpret_cast<const BYTE*>(&value), sizeof(DWORD), REG_DWORD);
}