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
#pragma once

#include "project.h"
#include "tooltip.h"
#include "WindowImageManager.h"
#include "LocaleManager.h"
#include <Windows.h>

class MainWindow {
public:
	MainWindow(HINSTANCE hInst);
	~MainWindow();

	HWND GetWnd() const { return m_hWnd; };

	int MessageBox(const TCHAR* message, int flags = MB_ICONINFORMATION);

protected:
	void OnCommand(int id, int notifyCode, HWND hwndFrom);
	void OnClose();
	void OnDestroy();
	void OnDropFiles(HDROP hDrop);
	void OnInitDialog(LPARAM param);
	void OnMinMaxInfo(LPMINMAXINFO lpMinMaxInfo);
	void OnNotify(int idCtrl, LPNMHDR lpNmhdr);
	void OnSize(int cx, int cy, INT_PTR flags);

	void OnFileRunWow();

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

	void LoadLocaleText();
	void SetDlgItemLocaleText(const char* message, UINT controlId, const char* defaultText = NULL);
	void SetLocale(ApplicationLocale locale);

private:
	HWND m_hWnd;
	WowClient m_project;
	Tooltip   m_tooltip;
	WindowImageManager m_imageManager;

	INT_PTR GetSelectedRealmlist(TCHAR* realmlist, int bufferSize);

	void InitListviews();
	void SetImages();
	void SetTooltips();

	// Truncate the file path to file dir
	bool AddToClientDirCb(const TCHAR* szDir);
};