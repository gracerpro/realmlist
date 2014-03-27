#include "global.h"
#include "Application.h"
#include <Windows.h>

Application g_App;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	g_App.InitInstance(hInstance);
	g_App.Run(nCmdShow);

	return 0;
}