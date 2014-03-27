#include "MainWindow.h"
#include "resource.h"
#include "global.h"
#include "settings.h"
#include <CommCtrl.h>
#include <tchar.h>

static MainWindow* g_MainWindow;

BOOL MainDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


static inline int GetSelectedIndexLvi(HWND hwndParent) {
	return SendDlgItemMessage(hwndParent, IDC_LSV_REALMLIST, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
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
		g_MainWindow->OnNotify(wParam, (LPNMHDR)lParam);
		break;
	case WM_SIZE:
		int cx, cy, flags;

		cx = LOWORD(lParam);
		cy = HIWORD(lParam);
		flags = wParam;
		g_MainWindow->OnSize(cx, cy, flags);
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
	case IDC_BTN_ADD_CLIENT_DIR:
		OnAddClientDir();
		break;
	case IDC_BTN_DEL_CLIENT_DIR:
		OnDelClientDir();
		break;
	case IDC_BTN_FIND_CLIENT:
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
	case IDC_FILE_EXIT:
		OnClose();
		break;
	}
}

void MainWindow::OnNotify(int idCtrl, LPNMHDR lpNmhdr) {
	if (idCtrl == IDC_LSV_REALMLIST && lpNmhdr->code == LVN_ITEMCHANGED) {
		OnRealmlistLviChangeSel();
	}
}

void MainWindow::OnClose() {
	DestroyWindow(m_hWnd);
}

void MainWindow::OnDestroy() {
	m_project.Save();
	PostQuitMessage(0);
}

void MainWindow::OnSize(int cx, int cy, int flags) {
	RECT rc;
	HWND hwndLviLocale = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	GetWindowRect(hwndLviLocale, &rc);
	MoveWindow(hwndLviLocale, 0, 50, cx, rc.bottom - rc.top, TRUE);

	HWND hwndLviRealmlist = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	MoveWindow(hwndLviRealmlist, 0, 200, cx, cy - 120, TRUE);
}

int MainWindow::MessageBox(const TCHAR* message, int flags) {
	return ::MessageBox(m_hWnd, message, TEXT("Realmlist"), flags);
}

static BOOL LoadCurrectClientDir(TCHAR* szClientPath, int size) {

// win32:

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

	return TRUE;
}

void MainWindow::OnInitDialog(LPARAM param) {
	LV_ITEM item = {0};
	LV_COLUMN col = {0};
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);

	col.mask = LVCF_TEXT | LVCF_WIDTH;
	col.cx = 200;
	col.pszText = TEXT("Реалмлист");
	col.cchTextMax = _tcslen(col.pszText);
	ListView_InsertColumn(hwndLvi, 0, &col);
	col.pszText = TEXT("Описание (сервер)");
	col.cchTextMax = _tcslen(col.pszText);
	ListView_InsertColumn(hwndLvi, 1, &col);

	ListView_SetExtendedListViewStyle(hwndLvi, LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);

	HWND hwndLviClient = GetDlgItem(m_hWnd, IDC_LSV_CLIENT);
	col.cx = 100;
	col.pszText = TEXT("Локализация");
	col.cchTextMax = _tcslen(col.pszText);
	ListView_InsertColumn(hwndLviClient, 0, &col);
	col.cx = 200;
	col.pszText = TEXT("Реалмлист");
	col.cchTextMax = _tcslen(col.pszText);
	ListView_InsertColumn(hwndLviClient, 1, &col);

	ListView_SetExtendedListViewStyle(hwndLviClient, LVS_EX_FULLROWSELECT | LVS_EX_SINGLEROW);

	TCHAR buf[1024];

	m_project.LoadRealmlist();
	RealmlistList realmlistList = m_project.GetRealmlistList();
	RealmlistList::const_iterator iter = realmlistList.begin();
	int i = 0;
	for ( ; iter != realmlistList.end(); ++iter, ++i) {
		LV_ITEM item = {0};
		const stRealmlist& realmlist = *iter;

		item.pszText = const_cast<TCHAR*>(realmlist.name.c_str());
		item.mask = LVIF_TEXT;
		item.iItem = i;
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
		const WowClientString& clientPath  = *iter;
		AddToClientDirCb(clientPath.c_str());
	}
	LoadCurrectClientDir(buf, 1024);
	if (m_project.AddClientDir(buf)) {
		AddToClientDirCb(buf);
	}

	int index = settings.GetInt(TEXT("selected_client_dir"));
	SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_SETCURSEL, index, 0);
	OnComboboxClientDirChangeSel();
}

void MainWindow::OnAddClientDir() {
	TCHAR buf[1024];

	HWND hwndClientPathCb = GetDlgItem(m_hWnd, IDC_CB_CLIENT_DIR);
	UINT len = GetDlgItemText(m_hWnd, IDC_CB_CLIENT_DIR, buf, 1024);
	if (len > 0) {
		ToDirectoryName(buf);
	}
	else {
		LoadCurrectClientDir(buf, 1024);
	}
	if (m_project.AddClientDir(buf)) {
		AddToClientDirCb(buf);
	}
}

void MainWindow::OnDelClientDir() {
	LRESULT index = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	if (index == CB_ERR) {
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

	int index = GetSelectedRealmlist(realmlist, 1024);
	if (m_project.DelRealmlist(realmlist)) {
		HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
		ListView_DeleteItem(hwndLvi, index);
	}
}

void MainWindow::OnDelAllRealmlist() {
	m_project.DelAllRealmlist();
	SendDlgItemMessage(m_hWnd, IDC_LSV_REALMLIST, LVM_DELETEALLITEMS, 0, 0);
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

	int index = GetSelectedRealmlist(realmlistName, 255);
	if (index == -1) {
		MessageBox(TEXT("Выберите реалмлист"));
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
	TCHAR realmlist[200];

	int index = GetSelectedIndexLvi(m_hWnd);
	if (index == -1) {
		MessageBox(TEXT("Выберите реалмлист"));
		return;
	}
	LRESULT selectedClientWowDir = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
	if (selectedClientWowDir == CB_ERR) {
		MessageBox(TEXT("Не выбран путь к клиенту WoW"));
		return;
	}
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	ListView_GetItemText(hwndLvi, index, 0, realmlist, 200);
	m_project.SetCurrectRealmlist(selectedClientWowDir, realmlist);
}

void MainWindow::OnComboboxClientDirChangeSel() {
	int index = SendDlgItemMessage(m_hWnd, IDC_CB_CLIENT_DIR, CB_GETCURSEL, 0, 0);
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
		item.iItem = i;
		LRESULT index = ListView_InsertItem(hwndLvi, &item);
		TCHAR* pDescr = const_cast<TCHAR*>(cocaleRealmlist.currentRealmlist.c_str());
		ListView_SetItemText(hwndLvi, index, 1, pDescr);
	}
}

int MainWindow::GetSelectedRealmlist(TCHAR* realmlist, size_t bufferSize) {
	realmlist[0] = 0;
	int index = GetSelectedIndexLvi(m_hWnd);
	if (index < 0) {
		return -1;
	}

	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	ListView_GetItemText(hwndLvi, index, 0, realmlist, bufferSize);

	return index;
}

void MainWindow::OnRealmlistLviChangeSel() {
	TCHAR realmlist[200];
	TCHAR description[200];

	int index = GetSelectedIndexLvi(m_hWnd);
	if (index < 0) {
		return;
	}
	HWND hwndLvi = GetDlgItem(m_hWnd, IDC_LSV_REALMLIST);
	//realmlist[0] = 0;
	ListView_GetItemText(hwndLvi, index, 0, realmlist, 200);
	//description[0] = 0;
	ListView_GetItemText(hwndLvi, index, 1, description, 200);
	SetDlgItemText(m_hWnd, IDC_EDT_REALMLIST, realmlist);
	SetDlgItemText(m_hWnd, IDC_EDT_REALMLIST_DESCR, description);
}

bool MainWindow::AddToClientDirCb(const TCHAR* szDir) {
/*	if (!m_project.AddClientDir(szDir)) {
		return false;
	}*/

	HWND hwndClientPathCb = GetDlgItem(m_hWnd, IDC_CB_CLIENT_DIR);
	LRESULT index = SendMessage(hwndClientPathCb, CB_ADDSTRING, 0, (LPARAM)szDir);
	SendMessage(hwndClientPathCb, CB_SETCURSEL, index, 0);

	return true;
}