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