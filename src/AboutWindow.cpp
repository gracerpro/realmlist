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
#include "AboutWindow.h"
#include "resource.h"
#include "Application.h"

extern Application g_App;

AboutWindow::AboutWindow(HWND hwndParent) {
	m_hwndParent = hwndParent;
}

AboutWindow::~AboutWindow() {

}

static BOOL WINAPI AboutWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		SetWindowText(hWnd, g_App.L("wnd_about_title"));
		return TRUE;
	case WM_COMMAND:
		int id = LOWORD(wParam);
		if (id == IDOK || id == IDCANCEL) {
			return EndDialog(hWnd, id);
		}
		break;

	}

	return FALSE;
}

INT_PTR AboutWindow::DoModal() {
	return DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), m_hwndParent, (DLGPROC)AboutWindowProc);
}