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
#include "LocaleManager.h"
#include <fstream>
#include <cctype>


const ApplicationLocale LocaleManager::DEFAULT_LOCALE = LocaleEnUS;


LocaleManager::LocaleManager() :
m_locale(LocaleNull)
{
}

LocaleManager::~LocaleManager() {

}

static char* trim(char* str) {
	char* strStart = str;
	while (isspace(static_cast<unsigned char>(*strStart))) {
		++strStart;
	}
	size_t len = strlen(strStart);
	if (!len) {
		return strStart;
	}
	char* strEnd = &strStart[len - 1];
	while (strEnd > strStart && isspace(static_cast<unsigned char>(*strEnd))) {
		--strEnd;
	}
	*(strEnd + 1) = 0;

	return strStart;
}

bool LocaleManager::LoadLocale(ApplicationLocale locale) {
	if (m_locale == locale) {
		return true;
	}
	if (locale == DEFAULT_LOCALE) {
		m_messages.clear();
	}

	auto& messages = locale == LocaleEnUS ? m_messagesDefault : m_messages;
	AppString fileName = GetLocaleFilePath(locale);

	std::ifstream stream;
	stream.open(fileName, std::ios::in);
	if (stream.fail()) {
		return false;
	}

	const int lineSize = 1024;
	char line[lineSize];
	wchar_t wszLocaleMessage[lineSize];
	do {
		line[0] = 0;
		stream.getline(line, lineSize);
		if (line[0] && line[0] != ';') {
			// trim key
			char* keyStart = line;
			char* keyEnd = strchr(keyStart, '=');
			if (!keyEnd) {
				continue;
			}
			*keyEnd = 0;
			char* valueStart = keyEnd + 1;

			char* message = trim(keyStart); // always enUS
			char* localeMessage = trim(valueStart); // utf8

			// TODO: replace \n\t\r\b\a etc.
			int writenLen = MultiByteToWideChar(CP_UTF8, 0, localeMessage, -1, wszLocaleMessage, lineSize);
			wszLocaleMessage[writenLen] = 0;
			messages[message] = wszLocaleMessage;
		}
	} while (stream.good());

	stream.close();

	m_locale = locale;

	return true;
}

bool LocaleManager::LoadDefaultLocale() {
	return LoadLocale(DEFAULT_LOCALE);
}

const wchar_t* LocaleManager::GetText(const char* message) const {
	auto iterFind = m_messages.find(message);
	if (iterFind != m_messages.end()) {
		return (*iterFind).second.c_str();
	}
	iterFind = m_messagesDefault.find(message);
	if (iterFind != m_messagesDefault.end()) {
		return (*iterFind).second.c_str();
	}

	return L"";
}

void LocaleManager::SetLocaleDir(const TCHAR* dir) {
	if (!dir || !dir[0]) {
		m_localeDir = TEXT("");
		return;
	}

	m_localeDir = dir;
}

const TCHAR* LocaleManager::GetLocaleName(ApplicationLocale locale) {
	switch (locale)
	{
	case LocaleEnUS: return TEXT("enUS");
	case LocaleRuRU: return TEXT("ruRU");
	case LocaleDeDE: return TEXT("deDE");
	case LocaleFrFR: return TEXT("frFR");
	case LocaleEnGB: return TEXT("enGB");
	case LocaleEsES: return TEXT("esES");
	case LocaleEsMX: return TEXT("esMX");
	case LocaleKoKR: return TEXT("koKR");
	case LocalePtBR: return TEXT("ptBR");
	case LocaleZhCN: return TEXT("zhCN");
	case LocaleZhTW: return TEXT("zhTW");
	default:
		return TEXT("null");
	}
}

AppString LocaleManager::GetLocaleFilePath(ApplicationLocale locale) {
	AppString filePath = m_localeDir;
	filePath += TEXT("locale_");
	filePath += GetLocaleName(locale);
	filePath += TEXT(".txt");

	return filePath;
}