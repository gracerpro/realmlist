#include <Windows.h>

class AboutWindow {
public:
	AboutWindow(HWND hwndParent);
	~AboutWindow();

	INT_PTR DoModal();

private:
	HWND m_hwndParent;
};