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
#include "global.h"


bool IsDirectory(const TCHAR* szDir) {
	return (GetFileAttributes(szDir) & FILE_ATTRIBUTE_DIRECTORY) > 0;
}

TCHAR* ToDirectoryName(TCHAR* szDir) {
	if (!szDir) {
		return false;
	}

	TCHAR* p = &szDir[_tcslen(szDir)];
	while (p > szDir && !(*p == '/' || *p == '\\')) {
		--p;
	}
	*(++p) = 0;

	return szDir;
}

bool IsFile(const TCHAR* file) {
	return GetFileAttributes(file) != -1;
}

bool IsDir(const TCHAR* file) {
	DWORD res = GetFileAttributes(file);

	return (res != -1) && (res & FILE_ATTRIBUTE_DIRECTORY);
}

TCHAR* trim(TCHAR* str) {
	TCHAR* res = str;

	while (*res && (*res == ' ' || *res == '\n' || *res == '\t' || *res == '\r')) {
		++res;
	}

	TCHAR* pEnd = &res[_tcslen(res) - 1];
	while (pEnd > res && (*pEnd == ' ' || *pEnd == '\n' || *pEnd == '\t' || *pEnd == '\r')) {
		--pEnd;
	}
	*(pEnd + 1) = 0;

	return res;
}
