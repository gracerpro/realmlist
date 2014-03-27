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