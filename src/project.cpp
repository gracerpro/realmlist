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
#include "project.h"
#include "global.h"
#include "settings.h"
#include <fstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <WinVer.h>
#include <winsock.h>

WowClient::WowClient() {
	m_selectedClientDirIndex = -1;
}

WowClient::~WowClient() {

}

bool WowClient::AddClientDir(const AppString szClientDir) {
	if (szClientDir.empty()) {
		return false;
	}
	if (std::find(m_clientDirList.begin(), m_clientDirList.end(), szClientDir) != m_clientDirList.end()) {
		return false;
	}
	if (!IsDir(szClientDir.c_str())) {
		return false;
	}
	m_clientDirList.push_back(szClientDir);

	return true;
}

bool WowClient::DelClientDir(size_t index) {
	ClientDirList::iterator iter = m_clientDirList.begin();
	std::advance(iter, index);

	if (iter == m_clientDirList.end()) {
		return false;
	}
	m_clientDirList.erase(iter);

	return true;
}

static TCHAR* GetExeDir(TCHAR* dir) {
	GetModuleFileName(NULL, dir, MAX_PATH);
	TCHAR* p = &dir[_tcslen(dir)];
	while (p > dir && !(*p == '\\' || *p == '/')) {
		--p;
	}
	*(++p) = 0;

	return dir;
}

const TCHAR* WowClient::GetRealmlistFilePath() const {
	static TCHAR fileName[MAX_PATH];

	GetExeDir(fileName);
	_tcscat(fileName, TEXT("realmlist.txt"));

	return fileName;
}

const TCHAR* WowClient::GetClientDirFilePath() const {
	static TCHAR fileName[MAX_PATH];

	GetExeDir(fileName);
	_tcscat(fileName, TEXT("clientdir.txt"));

	return fileName;
}

void WowClient::SaveServer() {
	std::basic_fstream<TCHAR> stream;

	std::string fileName = ToString(GetRealmlistFilePath());
	stream.open(fileName, std::ios_base::out | std::ios_base::trunc);
	if (!stream.is_open()) {
		return;
	}

	ServerList::const_iterator iter = m_serverList.begin();
	for ( ; iter != m_serverList.end(); ++iter) {
		const stServer& server = *iter;

		stream << server.address << ';' << server.description << '\n';
	}

	stream.close();
}

void WowClient::SaveClientDirList() {
	std::string fileName = ToString(GetClientDirFilePath());
	std::basic_ofstream<TCHAR> ostream(fileName, std::ios_base::out | std::ios_base::trunc);

	if (!ostream.is_open()) {
		return;
	}

	ClientDirList::const_iterator iter = m_clientDirList.begin();
	for ( ; iter != m_clientDirList.end(); ++iter) {
		const AppString& dir = *iter;

		ostream << dir << '\n';
	}

	ostream.close();
}

ServerList& WowClient::LoadServers() {
	std::string fileName = ToString(GetRealmlistFilePath());
	std::basic_ifstream<TCHAR> stream(fileName, std::ios::in);

	m_serverList.clear();

	if (stream.is_open()) {
		TCHAR line[1024];
		while (stream.getline(line, 1024)) {
			stServer server;
			TCHAR* pDelim = _tcschr(line, ';');
			if (pDelim) {
				ptrdiff_t nameLen = pDelim - line;
				server.address.assign(line, nameLen);
				server.description.assign(line + nameLen + 1);
			}
			else {

			}
			m_serverList.push_back(server);
		}
		stream.close();
	}
	else {
		std::basic_ofstream<TCHAR> ostream(fileName, std::ios_base::out | std::ios_base::trunc);
		if (ostream.is_open()) {
			ostream.close();
		}
	}

	return m_serverList;
}

size_t WowClient::LoadClientDirList() {
	std::string fileName = ToString(GetClientDirFilePath());
	std::basic_ifstream<TCHAR> stream(fileName, std::ios::in);

	if (stream.is_open()) {
		TCHAR line[1024];
		while (stream.getline(line, 1024)) {
			if (line[0])
				m_clientDirList.push_back(line);
		}
		stream.close();
	}
	else {
		std::basic_ofstream<TCHAR> ostream(fileName, std::ios_base::out | std::ios_base::trunc);
		if (ostream.is_open()) {
			ostream.close();
		}
	}

	return m_serverList.size();
}

bool WowClient::AddServer(const stServer& server) {
	ServerList::const_iterator findIter = std::find(m_serverList.begin(), m_serverList.end(), server);
	if (findIter != m_serverList.end()) {
		return false;
	}
	m_serverList.push_back(server);

	return true;
}

bool WowClient::DelServer(const AppString& server) {
	ServerList::iterator findIter = std::find(m_serverList.begin(), m_serverList.end(), server);
	if (findIter == m_serverList.end()) {
		return false;
	}
	m_serverList.erase(findIter);

	return true;
}

void WowClient::DelAllServers() {
	m_serverList.clear();
}

bool WowClient::ChangeServer(const AppString serverAddr, stServer& serverNew) {
	ServerList::iterator findIter = std::find(m_serverList.begin(), m_serverList.end(), serverAddr);
	if (findIter == m_serverList.end()) {
		return false;
	}
	stServer& changedServer = *findIter;

	if (serverAddr == serverNew.address) {
		changedServer.description = serverNew.description;
		return true;
	}

	ServerList::iterator existedRealmlistIter = std::find(m_serverList.begin(), m_serverList.end(), serverNew);
	if (existedRealmlistIter != m_serverList.end()) {
		return false;
	}
	changedServer.address = serverNew.address;
	changedServer.description = serverNew.description;

	return true;
}

eWowVersion WowClient::GetWowVersion() const {
	if (m_selectedClientDirIndex >= m_clientDirList.size()) {
		return WOW_VERSION_NULL;
	}

	eWowVersion res = WOW_VERSION_NULL;

	AppString exeFile = m_clientDirList[m_selectedClientDirIndex] + TEXT("WoW.exe");
	std::string fileName = ToString(exeFile);
	std::basic_ifstream<TCHAR> stream(fileName, std::ios::binary | std::ios::ate);
	if (stream.is_open()) {
		std::streamoff size = stream.tellg();
		stream.close();
		if (size < 8 * 1024 * 1024) {
			res = WOW_VERSION_335A;
		}
		if (size < 11 * 1024 * 1024) {
			res = WOW_VERSION_434;
		}
	}
	/*	FILEMUIINFO info;
			DWORD verHandle;

			DWORD size = GetFileVersionInfoSize(fileExe.c_str(), &verHandle);
			char* version = new char[size];
			GetFileVersionInfo(fileExe.c_str(), verHandle, size, version);

			VS_FIXEDFILEINFO* ffi;
			UINT len;
			VerQueryValue(version, TEXT("\\"), (LPVOID*)&ffi, &len);
			if (len) {

			}
			delete[] version;*/

	return res;
}

static void WriteRealmlistToFile(AppString fileName, AppString& realmlistData) {
	std::string fileName2 = ToString(fileName);
	std::basic_ofstream<TCHAR> stream(fileName2, std::ios::out | std::ios::trunc);
	if (stream.is_open()) {
		stream << realmlistData;
		stream.close();
	}
}

static bool WriteRealmlistToLocaleFile(AppString& clientDir, eWowVersion wowVersion,
	AppString serverAddr, AppString locale) {

	AppString realmlistFileName = clientDir + TEXT("Data\\") + locale + TEXT("\\realmlist.wtf");
	AppString fileExe, data;

	if (!IsFile(realmlistFileName.c_str())) {
		return false;
	}
	fileExe = clientDir + TEXT("WoW.exe");
	switch (wowVersion) {
	case WOW_VERSION_335A:
		data = TEXT("set realmlist ");
		data += serverAddr;
		WriteRealmlistToFile(realmlistFileName, data);
		break;
	case WOW_VERSION_434:
		data = TEXT("set realmlist ");
		data += serverAddr;
		data += TEXT("\n");
		data += TEXT("set patchlist localhost");
		WriteRealmlistToFile(realmlistFileName, data);
		break;
	default:
		return false;
	}

	return true;
}

bool WowClient::SetCurrectServer(size_t clientDirindex, AppString locale, const AppString serverAddr) {
	if (serverAddr.empty() || locale.empty()) {
		return false;
	}
	if (clientDirindex >= m_clientDirList.size()) {
		return false;
	}
	ServerList::const_iterator findIter = std::find(m_serverList.begin(), m_serverList.end(), serverAddr);
	if (findIter == m_serverList.end()) {
		return false;
	}

	AppString& clientDir = m_clientDirList[clientDirindex];

	eWowVersion wowVersion = GetWowVersion();
	if (wowVersion == WOW_VERSION_NULL) {
		return false;
	}

	return WriteRealmlistToLocaleFile(clientDir, wowVersion, serverAddr, locale);
}

void WowClient::SetSelectedClientDir(size_t index) {
	m_selectedClientDirIndex = index;
}

void WowClient::Save() {
	SaveClientDirList();
	SaveServer();
}

static AppString GetServerAddrFromFile(AppString& file) {
	std::string fileName = ToString(file);
	std::basic_ifstream<TCHAR> stream(fileName, std::ios::in);
	AppString res;

	if (stream.good()) {
		TCHAR line[512];

		while (stream.getline(line, 512)) {
			const TCHAR setrealmlist[] = TEXT("set realmlist");
			const size_t setrealmlistLen = (sizeof(setrealmlist) / sizeof(TCHAR)) - 1;
			if (!_tcsncmp(line, setrealmlist, setrealmlistLen)) {
				res = trim(line + setrealmlistLen);
			}
		}
	}

	return res;
}

size_t WowClient::LoadLocaleServerAddr(LocaleServerList& list, size_t clientDirIndex) {
	static AppString arrLocaleName[] = {
		TEXT("deDE"),
		TEXT("enUS"),
		TEXT("esES"),
		TEXT("esMX"),
		TEXT("frFR"),
		TEXT("ptBR"),
		TEXT("ptPT"),
		TEXT("ruRU"),
		TEXT("zhCN"),
		TEXT("zhTW")
	};

	if (clientDirIndex >= m_clientDirList.size()) {
		return 0;
	}
	const AppString& dir = m_clientDirList[clientDirIndex];

	list.clear();
	for (size_t i = 0; i < sizeof(arrLocaleName) /  sizeof(arrLocaleName[0]); ++i) {
		AppString& locale = arrLocaleName[i];
		AppString fileRealmlist = dir + TEXT("Data\\") + locale + TEXT("\\realmlist.wtf");
		if (!IsFile(fileRealmlist.c_str())) {
			continue;
		}

		stClientServer clietntServer;
		clietntServer.locale = locale;
		clietntServer.currentServer = GetServerAddrFromFile(fileRealmlist);

		list.push_back(clietntServer);
	}

	return list.size();
}

AppString WowClient::GetServerStatusName(ServerStatus status) {
	AppString statusName = TEXT("offline");

	if (status == ServerStatusOnline) {
		statusName = TEXT("online");
	}

	return statusName;
}

ServerStatus WowClient::GetServerStatus(const stServer& server) {
	ServerStatus status = ServerStatusOffline;

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == SOCKET_ERROR) {
		return status;
	}

	const char* host;
#ifdef UNICODE
	std::string sAddress = ToString(server.address);
	host = sAddress.c_str();
#else
	host = serverAddr.c_str();
#endif
	u_long address = inet_addr(host);

	if (address == INADDR_NONE) {
		hostent* pHostent = gethostbyname(host);
		if (pHostent) {
			address = *((u_long*)pHostent->h_addr_list[0]);
		}
	}

	if (address != INADDR_NONE) {
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(WOW_PORT);
		addr.sin_addr.S_un.S_addr = address;

		if (connect(s, (const sockaddr*)&addr, sizeof(addr)) != SOCKET_ERROR) {
			status = ServerStatusOnline;
		}
	}

	closesocket(s);

	return status;
}

const stServer* WowClient::Servers(size_t index) const {
	if (index < 0 || index >= m_serverList.size()) {
		return NULL;
	}

	ServerList::const_iterator iterServer = m_serverList.begin();
	std::advance(iterServer, index);
	const stServer& server = *iterServer;

	return &server;
}
