#pragma once

#include "Application.h"
#include <string>

extern Application g_App;

// TODO: apply to program
typedef std::basic_string<TCHAR> AppString;

bool IsDirectory(const TCHAR* szDir);
TCHAR* ToDirectoryName(TCHAR* szDir);

bool IsFile(const TCHAR* file);