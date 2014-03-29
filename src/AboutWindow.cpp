#include "AboutWindow.h"
#include "resource.h"

AboutWindow::AboutWindow(HWND hwndParent) {
	m_hwndParent = hwndParent;
}

AboutWindow::~AboutWindow() {

}

static BOOL WINAPI AboutWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
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