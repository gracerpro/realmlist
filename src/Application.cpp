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
#include <CommCtrl.h>

Application::Application() {
	m_hInst = NULL;
	m_pMainWindow = NULL;
	m_gdiplusToken = NULL;
}

Application::~Application() {
	delete m_pMainWindow;
}

bool Application::InitInstance(HINSTANCE hInst) {
	m_hInst = hInst;
	INITCOMMONCONTROLSEX icce;

	icce.dwSize = sizeof(icce);
	icce.dwICC  = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&icce);

	Gdiplus::Status status;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	status = Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	//Settings settings;
	// load locale...

	const TCHAR* locale = TEXT("ruRU");
	m_localeManager.SetLocaleDir(GetAppDir());
	if (!m_localeManager.SetLocale(locale)) {
		m_localeManager.SetLocale();
	}

	return status == Gdiplus::Ok;
}

int Application::Run(int cmdShow) {

	// Create main window
	try {
		m_pMainWindow = new MainWindow(m_hInst);
	}
	catch (LPCTSTR message) {
		MessageBox(NULL, message, TEXT("error"), MB_ICONERROR);
		return -1;
	}

	HWND hwndMain = m_pMainWindow->GetWnd();

	ShowWindow(hwndMain, cmdShow);
	UpdateWindow(hwndMain);

	// main message loop
	MSG msg;
	while (int status = GetMessage(&msg, 0, 0, 0)) {
		if (status == -1) {
			break;
		}
		if (!IsDialogMessage(hwndMain, &msg)) {
			DispatchMessage(&msg);
			TranslateMessage(&msg);
		}
	}

	return 0;
}

void Application::ExitInstance() {
	if (m_gdiplusToken) {
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}
}

const wchar_t* Application::L(const char* message) {
	const wchar_t* text = m_localeManager.GetText(message);
	if (!text || !text[0]) {
		return TEXT("");
	}

	return text;
}

const TCHAR* Application::GetAppDir() const {
	static TCHAR file[MAX_PATH];

	DWORD len = GetModuleFileName(NULL, file, MAX_PATH);
	if (!len) {
		return TEXT("");
	}

	TCHAR* p = &file[len - 1];
	while (p > file && !(*p == '/' || *p == '\\')) {
		--p;
	}
	*(p + 1) = 0;

	return file;
}