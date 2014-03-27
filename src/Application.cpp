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