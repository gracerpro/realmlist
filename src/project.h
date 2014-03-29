#pragma once

#include "global.h"
#include <string>
#include <array>
#include <vector>
#include <tchar.h>

struct stRealmlist {
	AppString name;
	AppString description;

	bool operator == (const stRealmlist realmlist) const {
		return name == realmlist.name;
	}
	bool operator == (AppString realmlist) const {
		return name == realmlist;
	}
};

struct stClientRealmlist {
	AppString locale;
	AppString currentRealmlist;

	bool operator == (const stClientRealmlist realmlist) const {
		return (currentRealmlist == realmlist.currentRealmlist) && (locale == realmlist.locale);
	}
};

enum eWowVersion {
	WOW_VERSION_NULL,
	WOW_VERSION_335A,
	WOW_VERSION_434
};

typedef std::vector<AppString>   ClientDirList;
typedef std::vector<stRealmlist>       RealmlistList;
typedef std::vector<stClientRealmlist> LocaleRealmlistList;

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

	size_t LoadRealmlist();
	void SaveRealmlist();
	bool AddRealmlist(const stRealmlist& realmlist);
	bool DelRealmlist(const AppString& realmlist);
	void DelAllRealmlist();
	bool ChangeRealmlist(const AppString realmlistName, stRealmlist& realmlistNew);
	bool SetCurrectRealmlist(size_t indexClientDir, AppString locale, const AppString realmlist);

	size_t LoadLocaleRealmlist(LocaleRealmlistList& list, size_t clientDirIndex);

	eWowVersion GetWowVersion() const;

	const TCHAR* GetRealmlistFilePath() const;
	const TCHAR* GetClientDirFilePath() const;

	const ClientDirList& GetClientPathList() const { return m_clientDirList; };
	size_t GetClientDirCount() const { return m_clientDirList.size(); };

	const RealmlistList& GetRealmlistList() const { return m_realmlistList; };

	const LocaleRealmlistList& GetLocaleRealmlistList() const { return m_localeRealmlistList; };
	const size_t GetLocaleRealmlistCount() const { return m_localeRealmlistList.size(); };

private:
	ClientDirList       m_clientDirList;
	RealmlistList       m_realmlistList;
	LocaleRealmlistList m_localeRealmlistList;

	size_t m_selectedClientDirIndex;
};