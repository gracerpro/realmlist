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
#pragma once

#include "global.h"
#include <string>
#include <array>
#include <vector>
#include <tchar.h>

enum ServerStatus {
	ServerStatusNull,
	ServerStatusOnline,
	ServerStatusOffline,
};

struct stServer {
	AppString address;
	AppString description;
	ServerStatus status;

	stServer() {
		status = ServerStatusOffline;
	}

	bool operator == (const stServer server) const {
		return address == server.address;
	}
	bool operator == (AppString serverAddr) const {
		return address == serverAddr;
	}
};

struct stClientServer {
	AppString locale;
	AppString currentServer;

	bool operator == (const stClientServer server) const {
		return (currentServer == server.currentServer) && (locale == server.locale);
	}
};

enum eWowVersion {
	WOW_VERSION_NULL,
	WOW_VERSION_335A,
	WOW_VERSION_434
};

enum eWowPort {
	WOW_PORT = 3724
};

typedef std::vector<AppString>      ClientDirList;
typedef std::vector<stServer>       ServerList;
typedef std::vector<stClientServer> LocaleServerList;

class WowClient {
public:
	WowClient();
	~WowClient();

	void Save();

	size_t LoadClientDirList();
	void SaveClientDirList();
	bool AddClientDir(const AppString szClientDir);
	bool DelClientDir(size_t index);
	void SetSelectedClientDir(size_t index);

	ServerList& LoadServers();
	void SaveServer();
	bool AddServer(const stServer& server);
	bool DelServer(const AppString& server);
	void DelAllServers();
	bool ChangeServer(const AppString serverAddr, stServer& serverNew);
	bool SetCurrectServer(size_t indexClientDir, AppString locale, const AppString server);
	const stServer* Servers(size_t index) const;

	size_t LoadLocaleServerAddr(LocaleServerList& list, size_t clientDirIndex);

	// sets server.status
	// @return "online" or "offline"
	static ServerStatus GetServerStatus(const stServer& server);
	static AppString GetServerStatusName(ServerStatus status);

	eWowVersion GetWowVersion() const;

	const TCHAR* GetRealmlistFilePath() const;
	const TCHAR* GetClientDirFilePath() const;

	const ClientDirList& GetClientPathList() const { return m_clientDirList; };
	size_t GetClientDirCount() const { return m_clientDirList.size(); };

	const ServerList& GetServerList() const { return m_serverList; };

	const LocaleServerList& GetLocaleServerList() const { return m_LocaleServerList; };
	const size_t GetLocaleServerCount() const { return m_LocaleServerList.size(); };

private:
	ClientDirList       m_clientDirList;
	ServerList          m_serverList;
	LocaleServerList    m_LocaleServerList;
	size_t              m_selectedClientDirIndex;
};