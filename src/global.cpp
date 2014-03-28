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