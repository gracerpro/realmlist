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
#include "LocaleManager.h"
#include <fstream>
#include <cctype>


const TCHAR* LocaleManager::DEFAULT_LOCALE = TEXT("enUS");


LocaleManager::LocaleManager() :
m_localeName(LocaleManager::DEFAULT_LOCALE)
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

bool LocaleManager::SetLocale(const TCHAR* szLocele) {
	m_localeName = szLocele;

	AppString fileName = m_localeDir;
	fileName += TEXT("locale_");
	fileName += szLocele;
	fileName += TEXT(".txt");

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

			int writenLen = MultiByteToWideChar(CP_UTF8, 0, localeMessage, strlen(localeMessage), wszLocaleMessage, lineSize);
			wszLocaleMessage[writenLen] = 0;
			m_messages[message] = wszLocaleMessage;
		}
	} while (stream.good());

	stream.close();

	return true;
}

const wchar_t* LocaleManager::GetText(const char* message, ...) {
	auto iterFind = m_messages.find(message);
	if (iterFind != m_messages.end()) {
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