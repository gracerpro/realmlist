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
#include "tooltip.h"
#include <CommCtrl.h>

Tooltip::Tooltip() {
	m_hWnd = NULL;
}

Tooltip::~Tooltip() {

}

HWND Tooltip::Create(HWND hwndParent) {
	m_hWnd = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
      TTS_ALWAYSTIP | WS_POPUP | TTS_NOPREFIX,
      CW_USEDEFAULT, 0,
      CW_USEDEFAULT, 0,
	  hwndParent, 0, GetModuleHandle(NULL), NULL);

	SendMessage(m_hWnd, TTM_SETMAXTIPWIDTH, 0, 300);

	return m_hWnd;
}

void Tooltip::Activate() {
	SendMessage(m_hWnd, TTM_ACTIVATE, TRUE, 0);
}

bool Tooltip::AddTooltip(int idCtrl, HWND hwndParent, const TCHAR* text) {
	TOOLINFO tt = {0};

	HWND hwndCtrl = GetDlgItem(hwndParent, idCtrl);
	if (!hwndCtrl) {
		return false;
	}
	tt.cbSize = sizeof(TOOLINFO);
#ifdef UNICODE
	tt.cbSize -= sizeof( void*);
#endif
	tt.uId = (UINT_PTR)hwndCtrl;
	tt.hwnd = hwndParent;
	tt.uFlags = TTF_SUBCLASS | TTF_IDISHWND;

	if (SendMessage(m_hWnd, TTM_GETTOOLINFO, 0, (LPARAM)&tt)) {
		tt.lpszText = (LPWSTR)text;
		SendMessage(m_hWnd, TTM_UPDATETIPTEXT, 0, (LPARAM)&tt);
		return true;
	}
	tt.lpszText = (LPWSTR)text;

	SendMessage(m_hWnd, TTM_ADDTOOL, 0, LPARAM(&tt));

	return true;
}