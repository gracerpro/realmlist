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
#pragma once

#include "ImageFactory.h"
#include <Windows.h>
#include <map>

// key   -- resource_id
// value -- HBITMAP
typedef std::map<UINT, HBITMAP> ResourceBitmapStore;


class WindowImageManager {
public:
	WindowImageManager();
	~WindowImageManager();

	void LoadButtonImagesFromResource(HWND hwndParent, const WindowImageItem* arrImage, size_t count);

	HBITMAP GetButtonHBitmap(UINT resourceId) const;

protected:


private:
	ResourceBitmapStore m_buttonBitmaps;

};
