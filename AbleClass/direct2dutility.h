//===================================================================================
// Copyright (c) Microsoft Corporation.  All rights reserved.                        
//                                                                                   
// THIS CODE AND INFORMATION IS PROVIDED 'AS IS' WITHOUT WARRANTY                    
// OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT                       
// LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND                          
// FITNESS FOR A PARTICULAR PURPOSE.                                                 
//===================================================================================

#pragma once
#include <algorithm>
#include <d2d1.h>
#include <wincodecsdk.h>

#include <shellscalingapi.h>

		//
		// This class provides various utility functions to work with 
		// Direct2D and related APIs such as WIC and DirectWrite
		//
class Direct2DUtility
{
public:
	enum {
		HIT_NON = 0,
		HIT_IN = 1,
		HIT_LEFT,
		HIT_RIGHT,
	};

	static HRESULT LoadBitmapFromFile(ID2D1RenderTarget *renderTarget, const wchar_t *uri, unsigned int destinationWidth, unsigned int destinationHeight, ID2D1Bitmap **bitmap);
	static HRESULT LoadBitmapFromResource(ID2D1RenderTarget *renderTarget, const wchar_t *resourceName, const wchar_t *resourceType, unsigned int destinationWidth, unsigned int destinationHeight, ID2D1Bitmap **bitmap);
	static HRESULT SaveBitmapToFile(IWICBitmap* bitmap, const wchar_t *uriOriginalFile, const wchar_t *uriUpdatedFile = nullptr);
	static HRESULT DecodeImageFromThumbCache(IShellItem *shellItem, ID2D1RenderTarget* renderTarget, unsigned int thumbnailSize, ID2D1Bitmap **bitmap);
	static HRESULT GetD2DFactory(__out ID2D1Factory** factory);
	static HRESULT GetWICFactory(__out IWICImagingFactory** factory);
	static HRESULT GetDWriteFactory(__out IDWriteFactory** factory);

	static D2D1_POINT_2F GetMousePositionForCurrentDpi(LPARAM lParam)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}

		return D2D1::Point2F(
			static_cast<int>(static_cast<short>(LOWORD(lParam))) * 96 / newDpiX,
			static_cast<int>(static_cast<short>(HIWORD(lParam))) * 96 / newDpiY);
	}

	static D2D1_POINT_2F GetMousePositionForCurrentDpi(float x, float y)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}
		return D2D1::Point2F(x * 96 / newDpiX, y * 96 / newDpiY);
	}

	static D2D1_POINT_2F GetPositionForCurrentDPI(POINTS location)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}

		return D2D1::Point2F(location.x * 96 / newDpiX, location.y * 96 / newDpiY);
	}

	static D2D1_RECT_F GetRectForCurrentDPI(const D2D1_RECT_F &rect)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}

		return D2D1::RectF(rect.left * newDpiX/96, rect.top * newDpiY/96, rect.right * newDpiX/ 96, rect.bottom *  newDpiY / 96);
	}
	static D2D1_RECT_F GetRectForCurrentDPI(const D2D1_RECT_U &rect)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}

		return D2D1::RectF(rect.left * newDpiX / 96, rect.top * newDpiY / 96, rect.right * newDpiX / 96, rect.bottom *  newDpiY / 96);
	}
	static D2D1_RECT_F GetRectForNoDPI(const D2D1_RECT_F &rect)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}
		return D2D1::RectF(rect.left * 96 / newDpiX, rect.top * 96 / newDpiY, rect.right * 96 / newDpiX, rect.bottom * 96 / newDpiY);
	}
	static D2D1_RECT_F GetRectForNoDPI(const D2D1_RECT_U &rect)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}
		return D2D1::RectF(rect.left * 96.f / static_cast<FLOAT>(newDpiX), rect.top * 96.f / static_cast<FLOAT>(newDpiY),
			rect.right * 96.f / static_cast<FLOAT>(newDpiX), rect.bottom * 96.f / static_cast<FLOAT>(newDpiY));
	}

	static float ScaleValueForCurrentDPI(float value)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}

		return value * newDpiX / 96;
	}

	static float ScaleValueForNoDPI(float value)
	{
		HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
		UINT newDpiX = 96;
		UINT newDpiY = 96;
		if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
		{
		}

		return value * 96 / newDpiX;
	}

	inline static int HitTest(const D2D1_RECT_F& rect, const D2D1_POINT_2F& point)
	{
		
		if (rect.left <= point.x && rect.right >= point.x && rect.top <= point.y && rect.bottom >= point.y) {
			return HIT_IN;
		}
		else {
			if (point.x > rect.right) {
				return HIT_RIGHT;
			}
			else if (point.x < rect.left) {
				return HIT_LEFT;
			}
			else {
				return HIT_NON;
			}
		}
	}

	inline static float GetRectWidth(const D2D1_RECT_F& rect)
	{
		return std::abs(rect.right - rect.left);
	}

	inline static float GetRectHeight(const D2D1_RECT_F& rect)
	{
		return std::abs(rect.bottom - rect.top);
	}

	// Get the mid point of a rectangle
	inline static D2D1_POINT_2F GetMidPoint(const D2D1_RECT_F& rect)
	{
		return D2D1::Point2F(
			rect.left + (rect.right - rect.left) / 2.0f,
			rect.top + (rect.bottom - rect.top) / 2.0f);
	}

	// Make sure the left top corner is smaller than right bottom one
	inline static D2D1_RECT_F FixRect(const D2D1_RECT_F& rect)
	{
		return D2D1::RectF(
			std::min<float>(rect.left, rect.right),
			std::min<float>(rect.top, rect.bottom),
			std::max<float>(rect.left, rect.right),
			std::max<float>(rect.top, rect.bottom));
	}

private:
	Direct2DUtility();
	~Direct2DUtility();
};

