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
}

Application::~Application() {
	delete m_pMainWindow;
}

int Application::InitInstance(HINSTANCE hInst) {
	m_hInst = hInst;
	INITCOMMONCONTROLSEX icce;

	icce.dwSize = sizeof(icce);
	icce.dwICC  = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&icce);

	return 0;
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