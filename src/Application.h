#pragma once

#include <Windows.h>
#include "MainWindow.h"

class Application {
public:
	Application();
	~Application();

	HINSTANCE GetInstance() const { return m_hInst; };
	int InitInstance(HINSTANCE hInst);
	int Run(int cmdShow);

	const MainWindow& GetMainWindow() const { return *m_pMainWindow; };
	//MainWindow& GetMainWindow() { return m_MainWindow; };

private:
	HINSTANCE m_hInst;
	MainWindow* m_pMainWindow;
};