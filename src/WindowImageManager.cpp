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
#include "WindowImageManager.h"


WindowImageManager::WindowImageManager() {

}

WindowImageManager::~WindowImageManager() {
	for (auto iter = m_buttonBitmaps.begin(); iter != m_buttonBitmaps.end(); ++iter) {
		HBITMAP hBitmap = (*iter).second;
		DeleteObject(hBitmap);
	}
}

void WindowImageManager::LoadButtonImagesFromResource(HWND hwndParent, const WindowImageItem* arrImage, size_t count) {
	ImageFactory imageFactory;

	HBITMAP hBitmap = NULL;
	for (size_t i = 0; i < count; ++i) {
		const WindowImageItem& item = arrImage[i];

		HWND hwndBtn = GetDlgItem(hwndParent, item.controlId);
		if (!hwndBtn) {
			continue;
		}

		LONG style = GetWindowLong(hwndBtn, GWL_STYLE);
		style |= BS_BITMAP;
		SetWindowLong(hwndBtn, GWL_STYLE, style);

		auto iterFind = m_buttonBitmaps.find(item.resourceId);
		if (iterFind != m_buttonBitmaps.end()) {
			hBitmap = (*iterFind).second;
		}
		else {
			hBitmap = imageFactory.LoadImageFromResource(hwndParent, item);
		}

		if (hBitmap) {
			SendMessage(hwndBtn, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
			m_buttonBitmaps[item.resourceId] = hBitmap;
		}
	}
}

HBITMAP WindowImageManager::GetButtonHBitmap(UINT resourceId) const {
	auto iterFind = m_buttonBitmaps.find(resourceId);
	if (iterFind != m_buttonBitmaps.end()) {
		return (*iterFind).second;
	}

	return NULL;
}