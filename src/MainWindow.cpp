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
#include "Application.h"
#include "MainWindow.h"
#include "resource.h"
#include "global.h"
#include "settings.h"
#include "AboutWindow.h"
#include <CommCtrl.h>
#include <tchar.h>

extern Application g_App;

static MainWindow* g_MainWindow;

BOOL MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


static inline INT_PTR GetSelectedIndexLvi(HWND hwndLvi) {
	return SendMessage(hwndLvi, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
}

void SetSelectedIndexLvi(HWND hwndParent, int index) {
	HWND hwndLvi = GetDlgItem(hwndParent, IDC_LSV_SERVER);
	ListView_SetItemState(hwndParent, index, LVIS_FOCUSED | LVIS_SELECTED, 0);
}

MainWindow::MainWindow(HINSTANCE hInst) {
	g_MainWindow = this;

	m_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDR_MAINFRAME), NULL, (DLGPROC)MainDlgProc);

	if (!m_hWnd) {
		throw TEXT("Create main window fail");
	}
}

MainWindow::~MainWindow() {
	g_MainWindow = NULL;
}

BOOL MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		g_MainWindow->m_hWnd = hWnd;
		g_MainWindow->OnInitDialog(lParam);
		return TRUE;
	case WM_COMMAND:
		int id, notifyCode;
		HWND hwndFrom;

		id         = LOWORD(wParam);
		notifyCode = HIWORD(wParam);
		hwndFrom   = (HWND)lParam;
		g_MainWindow->OnCommand(id, notifyCode, hwndFrom);
		return TRUE;
	case WM_NOTIFY:
		g_MainWindow->OnNotify((int)wParam, (LPNMHDR)lParam);
		break;
	case WM_GETMINMAXINFO:
		g_MainWindow->OnMinMaxInfo((LPMINMAXINFO)lParam);
		break;
	case WM_SIZE:
		int cx, cy;
		INT_PTR flags;

		cx = LOWORD(lParam);
		cy = HIWORD(lParam);
		flags = wParam;
		g_MainWindow->OnSize(cx, cy, flags);
		break;
	case WM_DROPFILES:
		g_MainWindow->OnDropFiles((HDROP)wParam);
		break;
	case WM_CLOSE:
		g_MainWindow->OnClose();
		return TRUE;
	case WM_DESTROY:
		g_MainWindow->OnDestroy();
		return TRUE;
	}

	return 0;
}

void MainWindow::OnCommand(int id, int notifyCode, HWND hwndFrom) {
	if (notifyCode == CBN_SELCHANGE && id == IDC_CB_CLIENT_DIR) {
		OnComboboxClientDirChangeSel();
	}

	switch (id) {
	case IDC_DEL_CLIENT_DIR:
		OnDelClientDir();
		break;
	case IDC_FIND_CLIENT_DIR:
		OnFindClientDir();
		break;
	case IDC_SERVER_ADD:
		OnAddServer();
		break;
	case IDC_SERVER_DEL:
		OnDelServer();
		break;
	case IDC_SERVER_DEL_ALL:
		OnDelAllServers();
		break;
	case IDC_SERVER_SET:
		OnChangeServer();
		break;
	case IDC_SERVER_CUR:
		OnSetCurrentServer();
		break;
	case IDC_SERVER_FROM_CLIENT:
		OnServerFromClient();
		break;
	case IDC_HELP_ABOUT:
		OnHelpAbout();
		break;
	case IDC_FILE_EXIT:
		OnClose();
		break;
	case IDC_FILE_RUN_WOW:
		OnFileRunWow();
		break;

	case IDC_LOCALE_ENUS:
	case IDC_LOCALE_RURU:
		ApplicationLocale locale;
		locale = (ApplicationLocale)(id - IDC_LOCALE_NULL);
		SetLocale(locale, true);
		break;
	}
}

void MainWindow::OnNotify(int idCtrl, LPNMHDR lpNmhdr) {
	if (idCtrl == IDC_LSV_SERVER && lpNmhdr->code == LVN_ITEMCHANGED) {
		NM_LISTVIEW* pNmListView = (NM_LISTVIEW*)lpNmhdr;
		if (pNmListView->uNewState == 3) { // TODO: hack
			OnServerLviChangeSel();
		}
	}
}

void MainWindow::OnClose() {
	DestroyWindow(m_hWnd);
}

void MainWindow::OnDestroy() {
	PostQuitMessage(0);
}

void MainWindow::OnSize(int cx, int cy, INT_PTR flags) {
	RECT rc;
	POINT pt;
	HWND hwndLviLocale = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	GetWindowRect(hwndLviLocale, &rc);
	pt.x = rc.left;
	pt.y = rc.top;
	int localeHeight = rc.bottom - rc.top;
	ScreenToClient(m_hWnd, &pt);
	MoveWindow(hwndLviLocale, 0, pt.y, cx, localeHeight, TRUE);

	HWND hwndCbCLientDir = GetDlgItem(m_hWnd, IDC_CB_CLIENT_DIR);
	GetWindowRect(hwndCbCLientDir, &rc);
	MoveWindow(hwndCbCLientDir, 0, 28, cx, rc.bottom - rc.top, TRUE);

	HWND hwndBtnRunWow = GetDlgItem(m_hWnd, IDC_FILE_RUN_WOW);
	GetWindowRect(hwndBtnRunWow, &rc);
	MoveWindow(hwndBtnRunWow, cx - (rc.right - rc.left) - 2, 2, rc.right - rc.left, rc.bottom - rc.top, TRUE);

	HWND hwndEdtServerAddrDestr = GetDlgItem(m_hWnd, IDC_EDT_SERVER_DESCR);
	GetWindowRect(hwndEdtServerAddrDestr, &rc);
	pt.x = rc.left;
	pt.y = rc.top;
	ScreenToClient(m_hWnd, &pt);
	MoveWindow(hwndEdtServerAddrDestr, pt.x, pt.y, cx - pt.x - 5, rc.bottom - rc.top, TRUE);

	HWND hwndLviServer = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	MoveWindow(hwndLviServer, 0, 230, cx, cy - 120, TRUE);
}

int MainWindow::MessageBox(const TCHAR* message, int flags) {
	return ::MessageBox(m_hWnd, message, TEXT("Message"), flags);
}

static BOOL LoadCurrectClientDir(TCHAR* szClientPath, int size) {

// win32:
	// TODO:

// win64: HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Blizzard Entertainment\World of Warcraft
	szClientPath[0] = 0;
	HKEY hkResult = NULL;
	TCHAR szSubKey[] = TEXT("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft");
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_READ, &hkResult)) {
		return FALSE;
	}
	DWORD dwSize = size;
	if (ERROR_SUCCESS == RegQueryValueEx(hkResult, TEXT("InstallPath"), NULL, NULL, (LPBYTE)szClientPath, &dwSize)) {
		szClientPath[dwSize] = 0;
	}
	RegCloseKey(hkResult);
	CharLower(szClientPath);

	return TRUE;
}

void MainWindow::InitListviews() {
	LV_COLUMN col = { 0 };
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);

	col.mask = LVCF_WIDTH | LVCF_TEXT;
	col.cx = 200;
	col.pszText = TEXT("");
	ListView_InsertColumn(hwndLvi, 0, &col);
	ListView_InsertColumn(hwndLvi, 1, &col);
	ListView_SetExtendedListViewStyle(hwndLvi, LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);

	HWND hwndLviClient = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	col.cx = 100;
	ListView_InsertColumn(hwndLviClient, 0, &col);
	col.cx = 200;
	ListView_InsertColumn(hwndLviClient, 1, &col);
	ListView_SetExtendedListViewStyle(hwndLviClient, LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);
}

void MainWindow::FillServerListView() {
	LV_ITEM item = { 0 };

	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	ServerList ServerList = m_project.LoadServers();
	for (size_t i = 0; i < ServerList.size(); ++i) {
		const stServer& server = ServerList[i];

		item.pszText = const_cast<TCHAR*>(server.address.c_str());
		item.mask = LVIF_TEXT;
		item.iItem = static_cast<int>(i);
		LRESULT index = ListView_InsertItem(hwndLvi, &item);
		TCHAR* pDescr = const_cast<TCHAR*>(server.description.c_str());
		ListView_SetItemText(hwndLvi, index, 1, pDescr);
	}
}

void MainWindow::OnInitDialog(LPARAM param) {
	Settings settings;

	InitListviews();

	m_tooltip.Create(m_hWnd);
	m_tooltip.Activate();
	ApplicationLocale locale = static_cast<ApplicationLocale>(settings.GetInt(TEXT("locale")));
	SetLocale(locale, false);

	FillServerListView();

	m_project.LoadClientDirList();
	ClientDirList list = m_project.GetClientPathList();
	for (ClientDirList::const_iterator iter = list.begin(); iter != list.end(); ++iter) {
		const AppString& clientPath  = *iter;
		AddToClientDirCb(clientPath.c_str());
	}

	TCHAR buf[1024];
	LoadCurrectClientDir(buf, 1024);
	if (m_project.AddClientDir(buf)) {
		AddToClientDirCb(buf);
	}

	int indexServer = settings.GetInt(TEXT("selected_server"));
	SetSelectedIndexLvi(m_hWnd, indexServer);

	int index = settings.GetInt(TEXT("selected_client_dir"));
	SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_SETCURSEL, index, 0);
	OnComboboxClientDirChangeSel();

	SetImages();

	RemoveUnknownLocaleMenuItems();

	DragAcceptFiles(m_hWnd, TRUE);
}

void MainWindow::RemoveUnknownLocaleMenuItems() {
	HMENU hMenu = GetMenu(m_hWnd);

	for (size_t i = LocaleNull + 1; i < LocaleMax; ++i) {
		ApplicationLocale locale = static_cast<ApplicationLocale>(i);
		AppString filePath = g_App.GetLocaleManager().GetLocaleFilePath(locale);
		if (!IsFile(filePath.c_str())) {
			UINT id = IDC_LOCALE_NULL + i;
			RemoveMenu(hMenu, id, 0);
		}
	}
}

void MainWindow::SetDlgItemLocaleText(const char* message, UINT controlId, const char* defaultText) {
	const TCHAR* text = g_App.L(message);
	if (text && text[0]) {
		SetDlgItemText(m_hWnd, controlId, text);
	}
}

void MainWindow::LoadLocaleText() {
	// buttons
	SetDlgItemLocaleText("server_addr", IDC_STC_SERVER_ADDR);
	SetDlgItemLocaleText("server_descr_col", IDC_STC_SERVER_DESCR);
	SetDlgItemLocaleText("client_dir", IDC_STC_CLIENT_DIR);
	SetDlgItemLocaleText("m_main_window_from_client", IDC_SERVER_FROM_CLIENT);

	// menu
	HMENU hMenu = GetMenu(m_hWnd);
	if (hMenu) {
		// TODO: do function...
		ModifyMenu(hMenu, 0, MF_BYPOSITION, 0, g_App.L("common_file"));
		ModifyMenu(hMenu, 1, MF_BYPOSITION, 1, g_App.L("server_addr"));
		ModifyMenu(hMenu, 2, MF_BYPOSITION, 2, g_App.L("m_client_dir"));
		ModifyMenu(hMenu, 3, MF_BYPOSITION, 3, g_App.L("common_help"));
		HMENU hSubMenu = GetSubMenu(hMenu, 3);
		int posLocalization = 1;
		ModifyMenu(hSubMenu, posLocalization, MF_BYPOSITION, 0, g_App.L("common_language"));
		ModifyMenu(hMenu, IDC_LOCALE_ENUS, 0, IDC_LOCALE_ENUS, g_App.L("common_language_enus"));
		ModifyMenu(hMenu, IDC_LOCALE_RURU, 0, IDC_LOCALE_RURU, g_App.L("common_language_ruru"));

		ModifyMenu(hMenu, IDC_FILE_RUN_WOW, 0, IDC_FILE_RUN_WOW, g_App.L("m_main_window_run_wow"));
		ModifyMenu(hMenu, IDC_FILE_EXIT, 0, IDC_FILE_EXIT, g_App.L("m_main_window_exit"));

		ModifyMenu(hMenu, IDC_SERVER_ADD, 0, IDC_SERVER_ADD, g_App.L("common_add"));
		ModifyMenu(hMenu, IDC_SERVER_DEL, 0, IDC_SERVER_DEL, g_App.L("common_del"));
		ModifyMenu(hMenu, IDC_SERVER_SET, 0, IDC_SERVER_SET, g_App.L("common_change"));
		ModifyMenu(hMenu, IDC_SERVER_CUR, 0, IDC_SERVER_CUR, g_App.L("m_server_write"));
		ModifyMenu(hMenu, IDC_SERVER_DEL_ALL, 0, IDC_SERVER_DEL_ALL, g_App.L("common_del_all"));
		ModifyMenu(hMenu, IDC_SERVER_FROM_CLIENT, 0, IDC_SERVER_FROM_CLIENT, g_App.L("m_main_window_from_client"));

		ModifyMenu(hMenu, IDC_DEL_CLIENT_DIR, 0, IDC_DEL_CLIENT_DIR, g_App.L("common_del"));
		ModifyMenu(hMenu, IDC_FIND_CLIENT_DIR, 0, IDC_FIND_CLIENT_DIR, g_App.L("m_client_dir_find"));

		ModifyMenu(hMenu, IDC_HELP_ABOUT, 0, IDC_HELP_ABOUT, g_App.L("m_about"));
	}

	// ListView columns
	LV_COLUMN col = { 0 };
	col.mask = LVCF_TEXT;

	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	col.pszText = const_cast<LPWSTR>(g_App.L("server_addr"));
	ListView_SetColumn(hwndLvi, 0, &col);
	col.pszText = const_cast<LPWSTR>(g_App.L("server_descr_col"));
	ListView_SetColumn(hwndLvi, 1, &col);

	HWND hwndLviClient = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	col.pszText = (PWCHAR)g_App.L("localization");
	ListView_SetColumn(hwndLviClient, 0, &col);
	col.pszText = (PWCHAR)g_App.L("server_addr");
	ListView_SetColumn(hwndLviClient, 1, &col);

	// set main title
	SetWindowText(m_hWnd, g_App.L("main_window_title"));
}

void MainWindow::SetTooltips() {
	m_tooltip.AddTooltip(IDC_DEL_CLIENT_DIR, m_hWnd, g_App.L("tt_client_dir_del"));
	m_tooltip.AddTooltip(IDC_FIND_CLIENT_DIR, m_hWnd, g_App.L("tt_client_dir_find"));

	m_tooltip.AddTooltip(IDC_SERVER_CUR, m_hWnd, g_App.L("tt_server_addr_write"));
	m_tooltip.AddTooltip(IDC_SERVER_ADD, m_hWnd, g_App.L("tt_server_add"));
	m_tooltip.AddTooltip(IDC_SERVER_DEL, m_hWnd, g_App.L("tt_server_del"));
	m_tooltip.AddTooltip(IDC_SERVER_SET, m_hWnd, g_App.L("tt_server_change"));

	m_tooltip.AddTooltip(IDC_FILE_RUN_WOW, m_hWnd, g_App.L("tt_run_wow"));
}

void MainWindow::SetImages() {
	WindowImageItem arrImage[] = {
		{ IDC_FILE_RUN_WOW, IDR_BITMAP_WOW16, ImageFormatBmp },
		{ IDC_SERVER_SET, IDR_OK16, ImageFormatPng },
		{ IDC_SERVER_ADD, IDR_ADD16, ImageFormatPng },
		{ IDC_SERVER_DEL, IDR_DELETE16, ImageFormatPng },
		{ IDC_DEL_CLIENT_DIR, IDR_DELETE16, ImageFormatPng },
		{ IDC_SERVER_CUR, IDR_WRITE16, ImageFormatPng },
	};
	m_imageManager.LoadButtonImagesFromResource(m_hWnd, arrImage, sizeof(arrImage) / sizeof(arrImage[0]));

	HMENU hMenu = GetMenu(m_hWnd);
	for (size_t i = 0; i < sizeof(arrImage) / sizeof(arrImage[0]); ++i) {
		HBITMAP hBitmap = m_imageManager.GetButtonHBitmap(arrImage[i].resourceId);
		if (hBitmap) {
			SetMenuItemBitmaps(hMenu, arrImage[i].controlId, MF_BYCOMMAND, hBitmap, NULL);
		}
	}
}

void MainWindow::OnDelClientDir() {
	LRESULT index = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	if (index == CB_ERR) {
		MessageBox(g_App.L("client_dir_not_select"));
		return;
	}
	if (m_project.DelClientDir(index)) {
		TCHAR currentDir[MAX_PATH];
		TCHAR deletedDir[MAX_PATH];
		GetDlgItemText(m_hWnd, IDC_CB_CLIENT_DIR, currentDir, MAX_PATH);
		SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETLBTEXT, index, (LPARAM)deletedDir);
		if (!_tcscmp(currentDir, deletedDir)) {
			SetDlgItemText(m_hWnd, IDC_CB_CLIENT_DIR, TEXT(""));
		}
		SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_DELETESTRING, index, 0);
		OnComboboxClientDirChangeSel();
	}
}

void MainWindow::OnFindClientDir() {
	OPENFILENAME ofn = {0};
	TCHAR szFilePath[MAX_PATH];

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hInstance = g_App.GetInstance();
	szFilePath[0] = 0;
	ofn.lpstrFile = szFilePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.hwndOwner = m_hWnd;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrFilter = TEXT("WoW.exe file\0wow.exe\0");
	ofn.nFilterIndex = 1;

	if (!GetOpenFileName(&ofn)) {
		return;
	}
	ToDirectoryName(ofn.lpstrFile);
	if (m_project.AddClientDir(ofn.lpstrFile)) {
		AddToClientDirCb(ofn.lpstrFile);
	}
}

void MainWindow::OnAddServer() {
	stServer server;
	TCHAR buf[1024];

	if (!GetDlgItemText(m_hWnd, IDC_EDT_SERVER_ADDR, buf, 1024)) {
		MessageBox(g_App.L("server_not_write"));
		return;
	}
	server.address = buf;

	GetDlgItemText(m_hWnd, IDC_EDT_SERVER_DESCR, buf, 1024);
	server.description = buf;

	if (!m_project.AddServer(server)) {
		MessageBox(g_App.L("server_add_fail"));
		return;
	}
	// TODO: function
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	LV_ITEM item = {0};

	item.pszText = const_cast<TCHAR*>(server.address.c_str());
	item.mask = LVIF_TEXT;
	item.iItem = ListView_GetItemCount(hwndLvi);
	LRESULT index = ListView_InsertItem(hwndLvi, &item);
	TCHAR* pDescr = const_cast<TCHAR*>(server.description.c_str());
	ListView_SetItemText(hwndLvi, index, 1, pDescr);
}

void MainWindow::OnDelServer() {
	TCHAR serverAddr[1024];

	INT_PTR index = GetSelectedServer(serverAddr, 1024);
	if (index == -1) {
		MessageBox(g_App.L("server_not_select"));
		return;
	}
	if (IDOK != MessageBox(g_App.L("confirm_delete"), MB_OKCANCEL | MB_ICONINFORMATION)) {
		return;
	}
	if (m_project.DelServer(serverAddr)) {
		HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
		ListView_DeleteItem(hwndLvi, index);
	}
}

void MainWindow::OnDelAllServers() {
	if (IDOK == MessageBox(g_App.L("confirm_delete"), MB_ICONQUESTION | MB_OKCANCEL)) {
		m_project.DelAllServers();
		SendDlgItemMessage(m_hWnd, IDC_LSV_SERVER, LVM_DELETEALLITEMS, 0, 0);
	}
}

void MainWindow::OnChangeServer() {
	stServer server;
	TCHAR buf[1024];
	TCHAR serverAddr[255];

	if (!GetDlgItemText(m_hWnd, IDC_EDT_SERVER_ADDR, buf, 1024)) {
		MessageBox(g_App.L("server_not_write"));
		return;
	}
	server.address = buf;

	GetDlgItemText(m_hWnd, IDC_EDT_SERVER_DESCR, buf, 1024);
	server.description = buf;

	// TODO: function
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	LV_ITEM item = {0};

	INT_PTR index = GetSelectedServer(serverAddr, 255);
	if (index == -1) {
		MessageBox(g_App.L("server_not_select"));
		return;
	}
	if (!m_project.ChangeServer(serverAddr, server)) {
		MessageBox(g_App.L("server_change_fail"));
		return;
	}

	TCHAR* pName = const_cast<TCHAR*>(server.address.c_str());
	ListView_SetItemText(hwndLvi, index, 0, pName);
	TCHAR* pDescr = const_cast<TCHAR*>(server.description.c_str());
	ListView_SetItemText(hwndLvi, index, 1, pDescr);
}

void MainWindow::OnSetCurrentServer() {
	HWND hwndLviServer = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	LRESULT serverIndex = GetSelectedIndexLvi(hwndLviServer);
	if (serverIndex == -1) {
		MessageBox(g_App.L("server_not_select"));
		return;
	}
	HWND hwndLviLocale = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	LRESULT localeIndex = GetSelectedIndexLvi(hwndLviLocale);
	if (localeIndex == -1) {
		MessageBox(g_App.L("localization_not_select"));
		return;
	}
	LRESULT selectedClientWowDir = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	if (selectedClientWowDir == CB_ERR) {
		MessageBox(g_App.L("client_dir_not_select"));
		return;
	}

	TCHAR serverAddr[200], locale[100];
	ListView_GetItemText(hwndLviLocale, localeIndex, 0, locale, 100);
	ListView_GetItemText(hwndLviServer, serverIndex, 0, serverAddr, 200);
	if (m_project.SetCurrectServer(selectedClientWowDir, locale, serverAddr)) {
		ListView_SetItemText(hwndLviLocale, localeIndex, 1, serverAddr);
	}
}

void MainWindow::OnServerFromClient() {
	HWND hwndLviClient = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	LRESULT localeIndex = GetSelectedIndexLvi(hwndLviClient);
	if (localeIndex == -1) {
		MessageBox(g_App.L("localization_not_select"));
		return;
	}

	TCHAR serverAddr[200];
	ListView_GetItemText(hwndLviClient, localeIndex, 1, serverAddr, 200);

	SetDlgItemText(m_hWnd, IDC_EDT_SERVER_ADDR, serverAddr);
	OnAddServer();
}

void MainWindow::OnComboboxClientDirChangeSel() {
	INT_PTR index = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	m_project.SetSelectedClientDir(index);

	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	ListView_DeleteAllItems(hwndLvi);

	LocaleServerList list;
	m_project.LoadLocaleServerAddr(list, index);

	for (size_t i = 0; i < list.size(); ++i) {
		stClientServer& localeServer = list[i];

		LV_ITEM item = {0};

		item.pszText = const_cast<TCHAR*>(localeServer.locale.c_str());
		item.mask = LVIF_TEXT;
		item.iItem = static_cast<int>(i);
		LRESULT index = ListView_InsertItem(hwndLvi, &item);
		TCHAR* pDescr = const_cast<TCHAR*>(localeServer.currentServer.c_str());
		ListView_SetItemText(hwndLvi, index, 1, pDescr);
	}
}

INT_PTR MainWindow::GetSelectedServer(TCHAR* serverUrl, int bufferSize) {
	serverUrl[0] = 0;
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	INT_PTR index = GetSelectedIndexLvi(hwndLvi);
	if (index < 0) {
		return -1;
	}
	ListView_GetItemText(hwndLvi, index, 0, serverUrl, bufferSize);

	return index;
}

void MainWindow::OnServerLviChangeSel() {
	TCHAR serverAddr[200];
	TCHAR description[200];
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);

	INT_PTR index = GetSelectedIndexLvi(hwndLvi);
	if (index < 0) {
		return;
	}
	ListView_GetItemText(hwndLvi, index, 0, serverAddr, 200);
	ListView_GetItemText(hwndLvi, index, 1, description, 200);
	SetDlgItemText(m_hWnd, IDC_EDT_SERVER_ADDR, serverAddr);
	SetDlgItemText(m_hWnd, IDC_EDT_SERVER_DESCR, description);
}

bool MainWindow::AddToClientDirCb(const TCHAR* szDir) {
	TCHAR dir[MAX_PATH];

	wcscpy_s(dir, szDir);
	HWND hwndClientPathCb = GetDlgItem(m_hWnd, IDC_CB_CLIENT_DIR);
	LRESULT index = SendMessage(hwndClientPathCb, CB_ADDSTRING, 0, (LPARAM)szDir);
	SendMessage(hwndClientPathCb, CB_SETCURSEL, index, 0);
	OnComboboxClientDirChangeSel();

	return true;
}

void MainWindow::OnHelpAbout() {
	AboutWindow dlg(m_hWnd);

	dlg.DoModal();
}

void MainWindow::OnFileRunWow() {
	STARTUPINFO sti = {0};
	PROCESS_INFORMATION pi = {0};
	TCHAR wowExePath[MAX_PATH];

	INT_PTR index = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	if (index == -1) {
		MessageBox(g_App.L("client_dir_not_select"));
		return;
	}
	SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETLBTEXT, index, (LPARAM)wowExePath);
	ToDirectoryName(wowExePath);

	_tcscat_s(wowExePath, TEXT("WoW.exe"));

	if (!CreateProcess(wowExePath, TEXT(""), NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &sti, &pi)) {
		TCHAR message[MAX_PATH + 100];
		_tcscpy_s(message, g_App.L("run_wow_fail"));
		_tcscat_s(message, wowExePath);
		MessageBox(message, MB_ICONWARNING);
	}
}

void MainWindow::OnDropFiles(HDROP hDrop) {
	TCHAR szFilePath[MAX_PATH];

	szFilePath[0] = 0;
	if (!DragQueryFile(hDrop, 0, szFilePath, MAX_PATH)) {
		return;
	}
	CharLower(szFilePath);
	if (!wcsstr(szFilePath, TEXT("wow.exe"))) {
		MessageBox(g_App.L("file_name_not_contain_wow_exe"));
		return;
	}
	ToDirectoryName(szFilePath);
	if (!m_project.AddClientDir(szFilePath)) {
		MessageBox(g_App.L("client_dir_add_fail"));
		return;
	}

	AddToClientDirCb(szFilePath);
}

void MainWindow::OnMinMaxInfo(LPMINMAXINFO lpMinMaxInfo) {
	lpMinMaxInfo->ptMinTrackSize.x = 350;
	lpMinMaxInfo->ptMinTrackSize.y = 350;
}

void MainWindow::SetLocale(ApplicationLocale locale, bool bUserSelect) {
	g_App.GetLocaleManager().LoadLocale(locale);

	if (bUserSelect) {
		Settings settings;

		settings.SetInt(TEXT("locale"), locale);
	}

	LoadLocaleText();
	SetTooltips();

	UINT id = IDC_LOCALE_NULL + locale;
	HMENU hMenu = GetMenu(m_hWnd);
	CheckMenuRadioItem(hMenu, IDC_LOCALE_START, IDC_LOCALE_END, id, 0);

	DrawMenuBar(m_hWnd);
}