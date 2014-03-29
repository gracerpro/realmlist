#include "project.h"
#include "global.h"
#include "settings.h"
#include <fstream>
#include <iostream>
#include <fstream>
#include <WinVer.h>

WowClient::WowClient() {
	m_selectedClientDirIndex = -1;
}

WowClient::~WowClient() {

}

bool WowClient::AddClientDir(const AppString szClientDir) {
	if (szClientDir.empty()) {
		return false;
	}
	if (m_clientDirList.end() != std::find(m_clientDirList.begin(), m_clientDirList.end(), szClientDir)) {
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
	_tcscat_s(fileName, MAX_PATH, TEXT("realmlist.txt"));

	return fileName;
}

const TCHAR* WowClient::GetClientDirFilePath() const {
	static TCHAR fileName[MAX_PATH];

	GetExeDir(fileName);
	_tcscat_s(fileName, MAX_PATH, TEXT("clientdir.txt"));

	return fileName;
}

void WowClient::SaveRealmlist() {
	std::basic_ofstream<TCHAR> ostream(GetRealmlistFilePath(), std::ios_base::out | std::ios_base::trunc);

	if (!ostream.is_open()) {
		return;
	}

	RealmlistList::const_iterator iter = m_realmlistList.begin();
	for ( ; iter != m_realmlistList.end(); ++iter) {
		const stRealmlist& realmlist = *iter;

		ostream << realmlist.name << ';' << realmlist.description << '\n';
	}

	ostream.close();
}

void WowClient::SaveClientDirList() {
	std::basic_ofstream<TCHAR> ostream(GetClientDirFilePath(), std::ios_base::out | std::ios_base::trunc);

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

size_t WowClient::LoadRealmlist() {
	std::basic_ifstream<TCHAR> stream(GetRealmlistFilePath());

	if (stream.is_open()) {
		TCHAR line[1024];
		while (stream.getline(line, 1024)) {
			stRealmlist realmlist;
			TCHAR* pDelim = _tcschr(line, ';');
			if (pDelim) {
				ptrdiff_t nameLen = pDelim - line;
				realmlist.name.assign(line, nameLen);
				realmlist.description.assign(line + nameLen + 1);
			}
			else {

			}
			m_realmlistList.push_back(realmlist);
		}
		stream.close();
	}
	else {
		std::basic_ofstream<TCHAR> ostream(GetRealmlistFilePath(), std::ios_base::out | std::ios_base::trunc);
		if (ostream.is_open()) {
			ostream.close();
		}
	}

	return m_realmlistList.size();
}

size_t WowClient::LoadClientDirList() {
	std::basic_ifstream<TCHAR> stream(GetClientDirFilePath());

	if (stream.is_open()) {
		TCHAR line[1024];
		while (stream.getline(line, 1024)) {
			m_clientDirList.push_back(line);
		}
		stream.close();
	}
	else {
		std::basic_ofstream<TCHAR> ostream(GetRealmlistFilePath(), std::ios_base::out | std::ios_base::trunc);
		if (ostream.is_open()) {
			ostream.close();
		}
	}

	return m_realmlistList.size();
}

bool WowClient::AddRealmlist(const stRealmlist& realmlist) {
	RealmlistList::const_iterator findIter = std::find(m_realmlistList.begin(), m_realmlistList.end(), realmlist);
	if (findIter != m_realmlistList.end()) {
		return false;
	}
	m_realmlistList.push_back(realmlist);

	return true;
}

bool WowClient::DelRealmlist(const AppString& realmlist) {
	RealmlistList::const_iterator findIter = std::find(m_realmlistList.begin(), m_realmlistList.end(), realmlist);
	if (findIter == m_realmlistList.end()) {
		return false;
	}
	m_realmlistList.erase(findIter);

	return true;
}

void WowClient::DelAllRealmlist() {
	m_realmlistList.clear();
}

bool WowClient::ChangeRealmlist(const AppString realmlistName, stRealmlist& realmlistNew) {
	RealmlistList::iterator findIter = std::find(m_realmlistList.begin(), m_realmlistList.end(), realmlistName);
	if (findIter == m_realmlistList.end()) {
		return false;
	}
	stRealmlist& changedRealmlist = *findIter;

	if (realmlistName == realmlistNew.name) {
		changedRealmlist.description = realmlistNew.description;
		return true;
	}

	RealmlistList::iterator existedRealmlistIter = std::find(m_realmlistList.begin(), m_realmlistList.end(), realmlistNew);
	if (existedRealmlistIter != m_realmlistList.end()) {
		return false;
	}
	changedRealmlist.name = realmlistNew.name;
	changedRealmlist.description = realmlistNew.description;

	return true;
}

eWowVersion WowClient::GetWowVersion() const {
	if (m_selectedClientDirIndex >= m_clientDirList.size()) {
		return WOW_VERSION_NULL;
	}

	eWowVersion res = WOW_VERSION_NULL;

	AppString exeFile = m_clientDirList[m_selectedClientDirIndex] + TEXT("WoW.exe");
	std::basic_ifstream<TCHAR> stream(exeFile.c_str(), std::ios::binary | std::ios::ate);
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
	std::basic_ofstream<TCHAR> stream(fileName.c_str(), std::ios::out | std::ios::trunc);
	if (stream.is_open()) {
		stream << realmlistData;
		stream.close();
	}
}

static bool WriteRealmlistToLocaleFile(AppString& clientDir, eWowVersion wowVersion,
	AppString realmlist, AppString locale) {

	AppString realmlistFileName = clientDir + TEXT("Data\\") + locale + TEXT("\\realmlist.wtf");
	AppString fileExe, data;

	if (!IsFile(realmlistFileName.c_str())) {
		return false;
	}
	fileExe = clientDir + TEXT("WoW.exe");
	switch (wowVersion) {
	case WOW_VERSION_335A:
		data = TEXT("set realmlist ");
		data += realmlist;
		WriteRealmlistToFile(realmlistFileName, data);
		break;
	case WOW_VERSION_434:
		data = TEXT("set realmlist ");
		data += realmlist;
		data += TEXT("\n");
		data += TEXT("set patchlist localhost");
		WriteRealmlistToFile(realmlistFileName, data);
		break;
	default:
		return false;
	}

	return true;
}

bool WowClient::SetCurrectRealmlist(size_t clientDirindex, AppString locale, const AppString realmlist) {
	if (realmlist.empty() || locale.empty()) {
		return false;
	}
	if (clientDirindex >= m_clientDirList.size()) {
		return false;
	}
	RealmlistList::const_iterator findIter = std::find(m_realmlistList.begin(), m_realmlistList.end(), realmlist);
	if (findIter == m_realmlistList.end()) {
		return false;
	}

	AppString& clientDir = m_clientDirList[clientDirindex];

	eWowVersion wowVersion = GetWowVersion();
	if (wowVersion == WOW_VERSION_NULL) {
		return false;
	}

	return WriteRealmlistToLocaleFile(clientDir, wowVersion, realmlist, locale);
}

void WowClient::SetSelectedClientDir(size_t index) {
	m_selectedClientDirIndex = index;
}

void WowClient::Save() {
	SaveClientDirList();
	SaveRealmlist();
}

static AppString GetRealmlistFromFile(AppString& file) {
	std::basic_ifstream<TCHAR> stream(file, std::ios::in);
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

size_t WowClient::LoadLocaleRealmlist(LocaleRealmlistList& list, size_t clientDirIndex) {
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

		stClientRealmlist clietntRealmlist;
		clietntRealmlist.locale = locale;
		clietntRealmlist.currentRealmlist = GetRealmlistFromFile(fileRealmlist);

		list.push_back(clietntRealmlist);
	}

	return list.size();
}