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
#include <gdiplus.h>


enum ImageFormat {
	ImageFormatNull,
	ImageFormatBmp,
	ImageFormatPng,
};


struct WindowImageItem {
	UINT controlId;
	UINT resourceId;
	ImageFormat format;
};


class ImageFactory
{
public:
	ImageFactory();
	~ImageFactory();

	HBITMAP LoadImageFromResource(HWND hwndParent, const WindowImageItem& imageItem);

protected:
	HBITMAP CreateDibFromPngStream(IStream* stream);

	template <typename T>
	inline void SafeRelease(T *&p);

private:

};

