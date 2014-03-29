#pragma once

#include <windows.h>
#include <tchar.h>
#include <string>

// TODO: apply to program
typedef std::basic_string<TCHAR> AppString;

bool IsDirectory(const TCHAR* szDir);
TCHAR* ToDirectoryName(TCHAR* szDir);

bool IsFile(const TCHAR* file);
bool IsDir(const TCHAR* file);

TCHAR* trim(TCHAR* str);