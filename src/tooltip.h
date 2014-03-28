#pragma once

#include <Windows.h>

class Tooltip {
public:
	Tooltip();
	~Tooltip();

	HWND Create(HWND hwndParent);
	void Activate();

	bool AddTooltip(int idCtrl, HWND hwndParent, TCHAR* text);
	bool AddTooltip(HWND hwndCtrl, TCHAR* text);

private:
	HWND m_hWnd; // tooltip handle
};