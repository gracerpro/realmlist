#include "global.h"
#include "Application.h"
#include <Windows.h>

// type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'

Application g_App;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	g_App.InitInstance(hInstance);
	g_App.Run(nCmdShow);

	return 0;
}