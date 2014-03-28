#include <Windows.h>

class AboutWindow {
public:
	AboutWindow(HWND hwndParent);
	~AboutWindow();

	int DoModal();

private:
	HWND m_hwndParent;
};