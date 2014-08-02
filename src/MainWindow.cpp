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
	HWND hwndLvi = GetDlgItem(hwndParent, IDC_LSV_REALMLIST);
	ListView_SetItemState(hwndParent, index, LVIS_FOCUSED | LVIS_SELECTED, 0);
}

MainWindow::MainWindow(HINSTANCE hInst) {
	g_MainWindow = this;

	m_hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDR_MAINFRAME), NULL, (DLGPROC)MainDlgProc);

	if (!m_hWnd) {
		throw TEXT("Не удалось создать главное окно");
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
	case IDC_REALMLIST_ADD:
		OnAddRealmlist();
		break;
	case IDC_REALMLIST_DEL:
		OnDelRealmlist();
		break;
	case IDC_REALMLIST_DEL_ALL:
		OnDelAllRealmlist();
		break;
	case IDC_REALMLIST_SET:
		OnChangeRealmlist();
		break;
	case IDC_REALMLIST_CUR:
		OnSetCurrentRealmlist();
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
	}
}

void MainWindow::OnNotify(int idCtrl, LPNMHDR lpNmhdr) {
	if (idCtrl == IDC_LSV_REALMLIST && lpNmhdr->code == LVN_ITEMCHANGED) {
		NM_LISTVIEW* pNmListView = (NM_LISTVIEW*)lpNmhdr;
		if (pNmListView->uNewState == 3) { // TODO: hack
			OnRealmlistLviChangeSel();
		}
	}
}

void MainWindow::OnClose() {
	DestroyWindow(m_hWnd);
}

void MainWindow::OnDestroy() {
	Settings settings;

	// save selected client dir
//	settings.SetParam(TEXT("selected_client_dir"), TEXT(""));
	// save selected realmlist
//	settings.SetParam(TEXT("selected_realmlist"), TEXT(""));

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

	HWND hwndEdtRealmlistDestr = GetDlgItem(m_hWnd, IDC_EDT_REALMLIST_DESCR);
	GetWindowRect(hwndEdtRealmlistDestr, &rc);
	pt.x = rc.left;
	pt.y = rc.top;
	ScreenToClient(m_hWnd, &pt);
	MoveWindow(hwndEdtRealmlistDestr, pt.x, pt.y, cx - pt.x - 5, rc.bottom - rc.top, TRUE);

	HWND hwndLviRealmlist = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	MoveWindow(hwndLviRealmlist, 0, 230, cx, cy - 120, TRUE);
}

int MainWindow::MessageBox(const TCHAR* message, int flags) {
	return ::MessageBox(m_hWnd, message, TEXT("Realmlist"), flags);
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

void MainWindow::OnInitDialog(LPARAM param) {
	LV_COLUMN col = {0};
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);

	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.cx = 200;
	col.pszText = TEXT("Реалмлист");
	col.cchTextMax = static_cast<int>(_tcslen(col.pszText));
	ListView_InsertColumn(hwndLvi, 0, &col);
	col.pszText = TEXT("Описание (сервер)");
	col.cchTextMax = static_cast<int>(_tcslen(col.pszText));
	ListView_InsertColumn(hwndLvi, 1, &col);

	ListView_SetExtendedListViewStyle(hwndLvi, LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);

	HWND hwndLviClient = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	col.cx = 100;
	col.pszText = TEXT("Локализация");
	col.cchTextMax = static_cast<int>(_tcslen(col.pszText));
	ListView_InsertColumn(hwndLviClient, 0, &col);
	col.cx = 200;
	col.pszText = TEXT("Реалмлист");
	col.cchTextMax = static_cast<int>(_tcslen(col.pszText));
	ListView_InsertColumn(hwndLviClient, 1, &col);

	ListView_SetExtendedListViewStyle(hwndLviClient, LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);

	TCHAR buf[1024];
	LV_ITEM item = {0};

	m_project.LoadRealmlist();
	RealmlistList realmlistList = m_project.GetRealmlistList();
	for (size_t i = 0; i < realmlistList.size(); ++i) {
		const stRealmlist& realmlist = realmlistList[i];

		item.pszText = const_cast<TCHAR*>(realmlist.name.c_str());
		item.mask = LVIF_TEXT;
		item.iItem = static_cast<int>(i);
		LRESULT index = ListView_InsertItem(hwndLvi, &item);
		TCHAR* pDescr = const_cast<TCHAR*>(realmlist.description.c_str());
		ListView_SetItemText(hwndLvi, index, 1, pDescr);
	}
	Settings settings;

	int indexRealmlist = settings.GetInt(TEXT("selected_realmlist"));
	SetSelectedIndexLvi(m_hWnd, indexRealmlist);

	m_project.LoadClientDirList();
	ClientDirList list = m_project.GetClientPathList();
	for (ClientDirList::const_iterator iter = list.begin(); iter != list.end(); ++iter) {
		const AppString& clientPath  = *iter;
		AddToClientDirCb(clientPath.c_str());
	}
	LoadCurrectClientDir(buf, 1024);
	if (m_project.AddClientDir(buf)) {
		AddToClientDirCb(buf);
	}

	int index = settings.GetInt(TEXT("selected_client_dir"));
	SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_SETCURSEL, index, 0);
	OnComboboxClientDirChangeSel();

	m_tooltip.Create(NULL);

	m_tooltip.AddTooltip(IDC_ADD_CLIENT_DIR, m_hWnd, TEXT("Добавить путь до клиента WoW в список"));
	m_tooltip.AddTooltip(IDC_DEL_CLIENT_DIR, m_hWnd, TEXT("Удалить путь до клиента WoW из списка"));
	m_tooltip.AddTooltip(IDC_FIND_CLIENT_DIR, m_hWnd, TEXT("Найти клиент Wow и добавить путь к нему в список"));

	m_tooltip.AddTooltip(IDC_REALMLIST_CUR, m_hWnd, TEXT("Установить реалмлист в выбранном клиенте и локализации\nЗаписать в файл realmlist.wpf"));
	m_tooltip.AddTooltip(IDC_REALMLIST_ADD, m_hWnd, TEXT("Добавить реалмлист в список"));
	m_tooltip.AddTooltip(IDC_REALMLIST_DEL, m_hWnd, TEXT("Удалить реалмлист из списка"));
	m_tooltip.AddTooltip(IDC_REALMLIST_SET, m_hWnd, TEXT("Изменить название реалмлиста"));

	m_tooltip.AddTooltip(IDC_FILE_RUN_WOW, m_hWnd, TEXT("Запустить WoW"));

	m_tooltip.Activate();

	SetImages();

	DragAcceptFiles(m_hWnd, TRUE);
	// Load locale title
	// SetWindowText(title);
}

void MainWindow::SetImages() {
	WindowImageItem arrImage[] = {
		{ IDC_FILE_RUN_WOW, IDR_BITMAP_WOW16, ImageFormatBmp },
		{ IDC_REALMLIST_SET, IDR_OK16, ImageFormatPng },
		{ IDC_REALMLIST_ADD, IDR_ADD16, ImageFormatPng },
		{ IDC_REALMLIST_DEL, IDR_DELETE16, ImageFormatPng },
		{ IDC_DEL_CLIENT_DIR, IDR_DELETE16, ImageFormatPng },
		{ IDC_REALMLIST_CUR, IDR_WRITE16, ImageFormatPng },
	};
	m_imageManager.LoadButtonImagesFromResource(m_hWnd, arrImage, sizeof(arrImage) / sizeof(arrImage[0]));
}

void MainWindow::OnDelClientDir() {
	LRESULT index = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	if (index == CB_ERR) {
		MessageBox(TEXT("Не выбран путь к клиенту"));
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

void MainWindow::OnAddRealmlist() {
	stRealmlist realmlist;
	TCHAR buf[1024];

	if (!GetDlgItemText(m_hWnd, IDC_EDT_REALMLIST, buf, 1024)) {
		MessageBox(TEXT("Введите реалмлист"));
		return;
	}
	realmlist.name = buf;

	GetDlgItemText(m_hWnd, IDC_EDT_REALMLIST_DESCR, buf, 1024);
	realmlist.description = buf;

	if (!m_project.AddRealmlist(realmlist)) {
		MessageBox(TEXT("Не удалось добавить реалмлист в список. Возможно он уже присутствует в нем."));
		return;
	}
	// TODO: function
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	LV_ITEM item = {0};

	item.pszText = const_cast<TCHAR*>(realmlist.name.c_str());
	item.mask = LVIF_TEXT;
	item.iItem = ListView_GetItemCount(hwndLvi);
	LRESULT index = ListView_InsertItem(hwndLvi, &item);
	TCHAR* pDescr = const_cast<TCHAR*>(realmlist.description.c_str());
	ListView_SetItemText(hwndLvi, index, 1, pDescr);
}

void MainWindow::OnDelRealmlist() {
	TCHAR realmlist[1024];

	INT_PTR index = GetSelectedRealmlist(realmlist, 1024);
	if (index == -1) {
		MessageBox(TEXT("Не выбран реалмлист"));
		return;
	}
	if (IDOK != MessageBox(L"Подтвердите удаление", MB_OKCANCEL | MB_ICONINFORMATION)) {
		return;
	}
	if (m_project.DelRealmlist(realmlist)) {
		HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
		ListView_DeleteItem(hwndLvi, index);
	}
}

void MainWindow::OnDelAllRealmlist() {
	if (IDOK == MessageBox(L"Подтвердите удаление всех реалмлистов", MB_ICONQUESTION | MB_OKCANCEL)) {
		m_project.DelAllRealmlist();
		SendDlgItemMessage(m_hWnd, IDC_LSV_REALMLIST, LVM_DELETEALLITEMS, 0, 0);
	}
}

void MainWindow::OnChangeRealmlist() {
	stRealmlist realmlist;
	TCHAR buf[1024];
	TCHAR realmlistName[255];

	if (!GetDlgItemText(m_hWnd, IDC_EDT_REALMLIST, buf, 1024)) {
		MessageBox(TEXT("Введите реалмлист"));
		return;
	}
	realmlist.name = buf;

	GetDlgItemText(m_hWnd, IDC_EDT_REALMLIST_DESCR, buf, 1024);
	realmlist.description = buf;

	// TODO: function
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	LV_ITEM item = {0};

	INT_PTR index = GetSelectedRealmlist(realmlistName, 255);
	if (index == -1) {
		MessageBox(TEXT("Выберите реалмлист"));
		return;
	}
	if (!m_project.ChangeRealmlist(realmlistName, realmlist)) {
		MessageBox(TEXT("Не удалось изменить реалмлист. Возможно он уже присутствует в списке."));
		return;
	}

	TCHAR* pName = const_cast<TCHAR*>(realmlist.name.c_str());
	ListView_SetItemText(hwndLvi, index, 0, pName);
	TCHAR* pDescr = const_cast<TCHAR*>(realmlist.description.c_str());
	ListView_SetItemText(hwndLvi, index, 1, pDescr);
}

void MainWindow::OnSetCurrentRealmlist() {
	HWND hwndLviRealmlist = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	LRESULT realmlistIndex = GetSelectedIndexLvi(hwndLviRealmlist);
	if (realmlistIndex == -1) {
		MessageBox(TEXT("Выберите реалмлист"));
		return;
	}
	HWND hwndLviLocale = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	LRESULT localeIndex = GetSelectedIndexLvi(hwndLviLocale);
	if (localeIndex == -1) {
		MessageBox(TEXT("Выберите локализацию клиента"));
		return;
	}
	LRESULT selectedClientWowDir = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	if (selectedClientWowDir == CB_ERR) {
		MessageBox(TEXT("Не выбран путь к клиенту WoW"));
		return;
	}

	TCHAR realmlist[200], locale[100];
	ListView_GetItemText(hwndLviLocale, localeIndex, 0, locale, 100);
	ListView_GetItemText(hwndLviRealmlist, realmlistIndex, 0, realmlist, 200);
	if (m_project.SetCurrectRealmlist(selectedClientWowDir, locale, realmlist)) {
		ListView_SetItemText(hwndLviLocale, localeIndex, 1, realmlist);
	}
}

void MainWindow::OnComboboxClientDirChangeSel() {
	INT_PTR index = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	m_project.SetSelectedClientDir(index);

	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	ListView_DeleteAllItems(hwndLvi);

	LocaleRealmlistList list;
	m_project.LoadLocaleRealmlist(list, index);

	for (size_t i = 0; i < list.size(); ++i) {
		stClientRealmlist& cocaleRealmlist = list[i];

		LV_ITEM item = {0};

		item.pszText = const_cast<TCHAR*>(cocaleRealmlist.locale.c_str());
		item.mask = LVIF_TEXT;
		item.iItem = static_cast<int>(i);
		LRESULT index = ListView_InsertItem(hwndLvi, &item);
		TCHAR* pDescr = const_cast<TCHAR*>(cocaleRealmlist.currentRealmlist.c_str());
		ListView_SetItemText(hwndLvi, index, 1, pDescr);
	}
}

INT_PTR MainWindow::GetSelectedRealmlist(TCHAR* realmlist, int bufferSize) {
	realmlist[0] = 0;
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	INT_PTR index = GetSelectedIndexLvi(hwndLvi);
	if (index < 0) {
		return -1;
	}
	ListView_GetItemText(hwndLvi, index, 0, realmlist, bufferSize);

	return index;
}

void MainWindow::OnRealmlistLviChangeSel() {
	TCHAR realmlist[200];
	TCHAR description[200];
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);

	INT_PTR index = GetSelectedIndexLvi(hwndLvi);
	if (index < 0) {
		return;
	}
	ListView_GetItemText(hwndLvi, index, 0, realmlist, 200);
	ListView_GetItemText(hwndLvi, index, 1, description, 200);
	SetDlgItemText(m_hWnd, IDC_EDT_REALMLIST, realmlist);
	SetDlgItemText(m_hWnd, IDC_EDT_REALMLIST_DESCR, description);
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
		MessageBox(TEXT("Не выбран путь к клиенту"));
		return;
	}
	SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETLBTEXT, index, (LPARAM)wowExePath);
	ToDirectoryName(wowExePath);

	_tcscat_s(wowExePath, TEXT("WoW.exe"));

	if (!CreateProcess(wowExePath, TEXT(""), NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &sti, &pi)) {
		TCHAR message[MAX_PATH + 100];
		_tcscpy_s(message, TEXT("Не удалось запустить WoW\n"));
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
	if (!wcsstr(szFilePath, L"wow.exe")) {
		MessageBox(L"Имя файла не содержит WoW.exe");
		return;
	}
	ToDirectoryName(szFilePath);
	if (!m_project.AddClientDir(szFilePath)) {
		// TODO: add messages storage
		MessageBox(TEXT("Не удалось добавить путь к клиенту WoW в список.\nДиректория не существует или она уже включена в список"));
		return;
	}

	AddToClientDirCb(szFilePath);
}

void MainWindow::OnMinMaxInfo(LPMINMAXINFO lpMinMaxInfo) {
	lpMinMaxInfo->ptMinTrackSize.x = 350;
	lpMinMaxInfo->ptMinTrackSize.y = 350;
}