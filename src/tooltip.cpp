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

bool Tooltip::AddTooltip(int idCtrl, HWND hwndParent, TCHAR* text) {
	TOOLINFO tt = {0};

	HWND hwndCtrl = GetDlgItem(hwndParent, idCtrl);
	if (!hwndCtrl) {
		return false;
	}
	tt.cbSize = sizeof(TOOLINFO);
#ifdef UNICODE
	tt.cbSize -= sizeof( void*);
#endif
	tt.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
	tt.uId = (UINT_PTR)hwndCtrl;
	tt.hwnd = hwndParent;
	tt.lpszText = text;
	tt.hinst = GetModuleHandle(NULL);
	GetClientRect(hwndCtrl, &tt.rect);

	SendMessage(m_hWnd, TTM_ADDTOOL, 0, LPARAM(&tt));

	return true;
}

bool Tooltip::AddTooltip(HWND hwndCtrl, TCHAR* text) {

	return true;
}