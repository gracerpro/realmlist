/*
* Realmlist -- manage your realmlists of World of Warcraft
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
#include "ImageFactory.h"


ImageFactory::ImageFactory() {
}

ImageFactory::~ImageFactory() {
}

template <typename T>
inline void ImageFactory::SafeRelease(T *&p)
{
	if (NULL != p)
	{
		p->Release();
		p = NULL;
	}
}

HBITMAP ImageFactory::LoadImageFromResource(HWND hwndParent, const WindowImageItem& imageItem)
{
	HBITMAP hBitmap;

	if (imageItem.format == ImageFormatBmp) {
		hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(imageItem.resourceId));
	}
	else if (imageItem.format == ImageFormatPng) {
		HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(imageItem.resourceId), RT_RCDATA);
		if (hResource) {
			DWORD resourceSize = SizeofResource(NULL, hResource);
			HGLOBAL hResourceData = LoadResource(NULL, hResource);
			if (hResourceData) {
				IStream* stream = NULL;

				void* resourceData = LockResource(hResourceData);

				HGLOBAL hData = GlobalAlloc(0, resourceSize);
				if (hData) {
					void* data = GlobalLock(hData);
					CopyMemory(data, resourceData, resourceSize);
					HRESULT hr = CreateStreamOnHGlobal(hData, FALSE, &stream);
					if (SUCCEEDED(hr)) {
						hBitmap = CreateDibFromPngStream(stream);
						SafeRelease(stream);
					}
					GlobalUnlock(hData);
					GlobalFree(hData);
				}

				UnlockResource(hResourceData);
				FreeResource(hResourceData);
			}
		}
	}
	else {
		throw "Unknown image format";
	}

	return hBitmap;
}

HBITMAP ImageFactory::CreateDibFromPngStream(IStream* stream) {
	HBITMAP hBitmap = NULL;

	Gdiplus::Bitmap bitmap(stream);
	if (bitmap.GetLastStatus() == Gdiplus::Ok) {
		COLORREF clr = GetSysColor(COLOR_BTNFACE);
		Gdiplus::Color backgroundColor(clr);
		bitmap.GetHBITMAP(backgroundColor, &hBitmap);
	}

	return hBitmap;
}
