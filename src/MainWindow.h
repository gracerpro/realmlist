#pragma once

#include "project.h"
#include "tooltip.h"
#include <Windows.h>

class MainWindow {
public:
	MainWindow(HINSTANCE hInst);
	~MainWindow();

	HWND GetWnd() const { return m_hWnd; };

	int MessageBox(const TCHAR* message, int flags = MB_ICONINFORMATION);

protected:
	void OnInitDialog(LPARAM param);
	void OnCommand(int id, int notifyCode, HWND hwndFrom);
	void OnNotify(int idCtrl, LPNMHDR lpNmhdr);
	void OnClose();
	void OnDestroy();
	void OnSize(int cx, int cy, int flags);

	void OnAddClientDir();
	void OnDelClientDir();
	void OnFindClientDir();

	void OnAddRealmlist();
	void OnDelRealmlist();
	void OnDelAllRealmlist();
	void OnChangeRealmlist();
	void OnSetCurrentRealmlist();

	void OnHelpAbout();

	void OnComboboxClientDirChangeSel();
	void OnRealmlistLviChangeSel();

	friend BOOL MainDlgProc(HWND, UINT, WPARAM, LPARAM);

private:
	HWND m_hWnd;
	WowClient m_project;
	Tooltip   m_tooltip;

	int GetSelectedRealmlist(TCHAR* realmlist, size_t bufferSize);

	// Truncate the file path to file dir
	bool AddToClientDirCb(const TCHAR* szDir);
};