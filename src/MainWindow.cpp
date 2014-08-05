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
#include <string.h>

extern Application g_App;

static MainWindow* g_MainWindow;

BOOL MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


static inline int GetSelectedIndexLvi(HWND hwndLvi) {
	return (int)SendMessage(hwndLvi, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
}

static void SetSelectedIndexLvi(HWND hwndLvi, int index) {
	ListView_SetItemState(hwndLvi, index, LVIS_FOCUSED | LVIS_SELECTED, 0);
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
		return g_MainWindow->OnNotify((int)wParam, (LPNMHDR)lParam);
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

static LONG OnCustumDraw(NMHDR* nmHdr) {
	if (nmHdr->idFrom != IDC_LSV_SERVER) {
		return CDRF_DODEFAULT;
	}

	NMLVCUSTOMDRAW* nmlvcd = (NMLVCUSTOMDRAW*)nmHdr;

	switch (nmlvcd->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		return CDRF_NOTIFYITEMDRAW;
	case CDDS_ITEMPREPAINT:
		COLORREF backColor;
		backColor = RGB(255, 255, 255);
		ServerList& serverList = g_MainWindow->GetProject().GetServerList();
		const stServer* pServer = g_MainWindow->GetProject().Servers(nmlvcd->nmcd.dwItemSpec);
		if (pServer) {
			if (pServer->status == ServerStatusOnline) {
				backColor = RGB(100, 255, 100);
			}
			if (pServer->status == ServerStatusOffline) {
				backColor = RGB(0xFA, 0xB2, 0xB0);
			}
		}
		nmlvcd->clrTextBk = backColor;
        return CDRF_NOTIFYSUBITEMDRAW;
	/*case (CDDS_ITEMPREPAINT | CDDS_ITEM | CDDS_SUBITEM):
		COLORREF clr = (nmlvcd->nmcd.dwItemSpec & 1) ? RGB(255,255,255) : RGB(230,230,230);
		return CDRF_NEWFONT;*/
	}

	return CDRF_DODEFAULT;
}

BOOL MainWindow::OnNotify(int idCtrl, LPNMHDR lpNmhdr) {
	if (idCtrl == IDC_LSV_SERVER) {
		if (lpNmhdr->code == LVN_ITEMCHANGED) {
			NM_LISTVIEW* pNmListView = (NM_LISTVIEW*)lpNmhdr;
			if (pNmListView->uNewState == 3) { // TODO: hack
				OnServerLviChangeSel();
			}
		}
		if (lpNmhdr->code == NM_CUSTOMDRAW) {
#ifdef _WIN64
			int index = DWLP_MSGRESULT;
#else
			int index = DWL_MSGRESULT;
#endif
			SetWindowLong(m_hWnd, index, OnCustumDraw(lpNmhdr));
			return TRUE;
		}
	}

	return 0;
}

void MainWindow::OnClose() {
	DestroyWindow(m_hWnd);
}

void MainWindow::OnDestroy() {
	m_project.Save();
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
	col.pszText = NULL;
	SendMessage(hwndLvi, LVM_INSERTCOLUMN, 0, (LPARAM)&col);
	SendMessage(hwndLvi, LVM_INSERTCOLUMN, 1, (LPARAM)&col);
	col.cx = 60;
	SendMessage(hwndLvi, LVM_INSERTCOLUMN, 2, (LPARAM)&col);
	SendMessage(hwndLvi, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);

	HWND hwndLviClient = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	col.cx = 100;
	SendMessage(hwndLviClient, LVM_INSERTCOLUMN, 0, (LPARAM)&col);
	col.cx = 200;
	SendMessage(hwndLviClient, LVM_INSERTCOLUMN, 1, (LPARAM)&col);
	SendMessage(hwndLviClient, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);
}

static DWORD WINAPI RetrieveServersStatusThread(LPVOID param) {
	ServerList& serverList = g_MainWindow->GetProject().GetServerList();
	HWND hwndLviServer = GetDlgItem(g_MainWindow->GetWnd(), IDC_LSV_SERVER);

	int i = 0;
	ServerList::iterator iter = serverList.begin();
	for (; iter != serverList.end(); ++iter) {
		stServer& server = *iter;
		ServerStatus status = WowClient::GetServerStatus(server);
		AppString statusName = WowClient::GetServerStatusName(status);
		LPWSTR pStatusName = const_cast<LPWSTR>(statusName.c_str());
		server.status = status;
		ListView_SetItemText(hwndLviServer, i, 2, pStatusName);
		SendMessage(hwndLviServer, LVM_REDRAWITEMS, i, i);
		++i;
		Sleep(1000);
	}

	ExitThread(0);
	return 0;
}

void MainWindow::FillServerListView() {
	const ServerList& serverList = m_project.LoadServers();
	ServerList::const_iterator iter = serverList.begin();
	for (; iter != serverList.end(); ++iter) {
		const stServer& server = *iter;
		AddServerToListView(server);
	}
}

void MainWindow::RetrieveServersStatus() {
	//static eventRetrieveServerStatus =
	DWORD dw = 0;
	CreateThread(NULL, 0, RetrieveServersStatusThread, 0, 0, &dw);
}

void MainWindow::OnInitDialog(LPARAM param) {
	Settings settings;

	InitListviews();

	m_tooltip.Create(m_hWnd);
	m_tooltip.Activate();
	ApplicationLocale locale = static_cast<ApplicationLocale>(settings.GetInt(TEXT("locale")));
	SetLocale(locale, false);

	FillServerListView();
	RetrieveServersStatus();

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
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	SetSelectedIndexLvi(hwndLvi, indexServer);

	int index = settings.GetInt(TEXT("selected_client_dir"));
	SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_SETCURSEL, index, 0);
	OnComboboxClientDirChangeSel();

	SetImages();

	RemoveUnknownLocaleMenuItems();

	DragAcceptFiles(m_hWnd, TRUE);
}

void MainWindow::RemoveUnknownLocaleMenuItems() {
	HMENU hMenu = GetMenu(m_hWnd);

	for (UINT i = LocaleNull + 1; i < LocaleMax; ++i) {
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

	HWND hwndLviClient = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	col.pszText = (PWCHAR)g_App.L("localization");
	SendMessage(hwndLviClient, LVM_SETCOLUMN, 0, (LPARAM)&col);
	col.pszText = (PWCHAR)g_App.L("server_addr");
	SendMessage(hwndLviClient, LVM_SETCOLUMN, 1, (LPARAM)&col);

	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	col.pszText = const_cast<LPWSTR>(g_App.L("server_addr"));
	SendMessage(hwndLvi, LVM_SETCOLUMN, 0, (LPARAM)&col);
	col.pszText = const_cast<LPWSTR>(g_App.L("server_descr_col"));
	SendMessage(hwndLviClient, LVM_SETCOLUMN, 1, (LPARAM)&col);
	col.pszText = const_cast<LPWSTR>(g_App.L("server_status"));
	SendMessage(hwndLviClient, LVM_SETCOLUMN, 2, (LPARAM)&col);

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
	m_tooltip.AddTooltip(IDC_SERVER_FROM_CLIENT, m_hWnd, g_App.L("tt_from_client"));

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
		if (0 == _tcscmp(currentDir, deletedDir)) {
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
	AddServerToListView(server);
}

void MainWindow::OnDelServer() {
	TCHAR serverAddr[1024];

	int index = GetSelectedServer(serverAddr, 1024);
	if (index == -1) {
		MessageBox(g_App.L("server_not_select"));
		return;
	}
	if (IDOK != MessageBox(g_App.L("confirm_delete"), MB_OKCANCEL | MB_ICONINFORMATION)) {
		return;
	}
	if (m_project.DelServer(serverAddr)) {
		HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
		SendMessage(hwndLvi, LVM_DELETEITEM, index, 0);
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

	int index = GetSelectedServer(serverAddr, 255);
	if (index == -1) {
		MessageBox(g_App.L("server_not_select"));
		return;
	}
	if (!m_project.ChangeServer(serverAddr, server)) {
		MessageBox(g_App.L("server_change_fail"));
		return;
	}

	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	TCHAR* pName = const_cast<TCHAR*>(server.address.c_str());
	ListView_SetItemText(hwndLvi, index, 0, pName);
	TCHAR* pDescr = const_cast<TCHAR*>(server.description.c_str());
	ListView_SetItemText(hwndLvi, index, 1, pDescr);
}

void MainWindow::OnSetCurrentServer() {
	HWND hwndLviServer = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	int serverIndex = GetSelectedIndexLvi(hwndLviServer);
	if (serverIndex == -1) {
		MessageBox(g_App.L("server_not_select"));
		return;
	}
	HWND hwndLviLocale = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	int localeIndex = GetSelectedIndexLvi(hwndLviLocale);
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
	int localeIndex = GetSelectedIndexLvi(hwndLviClient);
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
	LRESULT index = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	m_project.SetSelectedClientDir(index);

	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	SendMessage(hwndLvi, LVM_DELETEALLITEMS, 0, 0);

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

int MainWindow::GetSelectedServer(TCHAR* serverUrl, int bufferSize) {
	serverUrl[0] = 0;
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	int index = GetSelectedIndexLvi(hwndLvi);
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

	int index = GetSelectedIndexLvi(hwndLvi);
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

	wcscpy(dir, szDir);
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

	_tcscat(wowExePath, TEXT("WoW.exe"));

	if (!CreateProcess(wowExePath, NULL, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &sti, &pi)) {
		TCHAR message[MAX_PATH + 100];
		_tcscpy(message, g_App.L("run_wow_fail"));
		_tcscat(message, wowExePath);
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

/*
@param server Pointer to stServer struct from server list!
*/
void MainWindow::AddServerToListView(const stServer& server) {
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_SERVER);
	LV_ITEM item = {0};

	// DEBUG:
	// ServerStatus status = rand() % 2 == 0 ? ServerStatusOffline : ServerStatusOnline;
	// AppString statusName = WowClient::GetServerStatusName(status);
	// server->status = status;
	item.pszText = const_cast<TCHAR*>(server.address.c_str());
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = ListView_GetItemCount(hwndLvi);
	item.lParam = (LPARAM)&server;
	LRESULT index = ListView_InsertItem(hwndLvi, &item);
	TCHAR* pDescr = const_cast<TCHAR*>(server.description.c_str());
	ListView_SetItemText(hwndLvi, index, 1, pDescr);
	AppString statusName = WowClient::GetServerStatusName(ServerStatusOffline);
	TCHAR* pStatusName = const_cast<TCHAR*>(statusName.c_str());
	ListView_SetItemText(hwndLvi, index, 2, pStatusName);
}
