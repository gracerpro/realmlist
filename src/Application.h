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
#pragma once

#include "MainWindow.h"
#include "LocaleManager.h"
#include <Windows.h>
#include <gdiplus.h>

class Application {
public:
	Application();
	~Application();

	HINSTANCE GetInstance() const { return m_hInst; };
	bool InitInstance(HINSTANCE hInst);
	int Run(int cmdShow);
	void ExitInstance();

	const TCHAR* GetAppDir() const;

	const MainWindow& GetMainWindow() const { return *m_pMainWindow; };

	LocaleManager& GetLocaleManager() { return m_localeManager; };

	/*
	@param message always in an English
	*/
	const wchar_t* L(const char* message);

private:
	HINSTANCE m_hInst;
	ULONG_PTR m_gdiplusToken;
	MainWindow* m_pMainWindow;
	LocaleManager m_localeManager;
};