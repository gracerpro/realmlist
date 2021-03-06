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

#include <Windows.h>

#define IDC_STATIC                -1
#define IDR_MAINFRAME             100

#define IDD_ABOUT                 101

#define IDC_LSV_SERVER            1001
#define IDC_CB_CLIENT_DIR         1002
#define IDC_EDT_SERVER_DESCR      1003
#define IDC_EDT_SERVER_ADDR       1004
#define IDC_LSV_CLIENT            1005
#define IDC_STC_SERVER_ADDR       1006
#define IDC_STC_SERVER_DESCR      1007
#define IDC_STC_CLIENT_DIR        1008

#define IDC_FILE_EXIT             10000
#define IDC_FILE_RUN_WOW          10001

#define IDC_SERVER_ADD            10100
#define IDC_SERVER_DEL            10101
#define IDC_SERVER_CUR            10102
#define IDC_SERVER_SET            10103
#define IDC_SERVER_DEL_ALL        10104
#define IDC_SERVER_FROM_CLIENT    10105

#define IDC_DEL_CLIENT_DIR        10051
#define IDC_FIND_CLIENT_DIR       10052

#define IDC_HELP_ABOUT            10300

// depend on ApplicationLocale defenotion, see LocaleManager.h
#define IDC_LOCALE_NULL           10400
#define IDC_LOCALE_ENUS           10401
#define IDC_LOCALE_RURU           10402
#define IDC_LOCALE_DEDE           10403
#define IDC_LOCALE_FRFR           10404
#define IDC_LOCALE_ENGB           10405
#define IDC_LOCALE_ESES           10406
#define IDC_LOCALE_ESMX           10407
#define IDC_LOCALE_KOKR           10408
#define IDC_LOCALE_PTBR           10409
#define IDC_LOCALE_ZHCN           10410
#define IDC_LOCALE_ZHTW           10411
#define IDC_LOCALE_START          IDC_LOCALE_ENUS
#define IDC_LOCALE_END            IDC_LOCALE_RURU


#define IDR_BITMAP_WOW16          100
#define IDR_OK16                  101
#define IDR_ADD16                 102
#define IDR_DELETE16              103
#define IDR_WRITE16               104
