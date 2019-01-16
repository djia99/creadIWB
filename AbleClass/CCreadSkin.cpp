#include "stdafx.h"
#include "CCreadSkin.h"



CCreadSkin::CCreadSkin() :
	m_dir(L"./images/"),
	m_mainMenu(0,0,0,0),
	m_mainMenuBtn(0, 0, 0, 0),
	m_boardMenu(0,0,0,0),
	m_version(001002001),
	m_imgToolMenuBg(L"tool_menu/bg.png"),
	m_screenWidth(1024),
	m_screenHeight(768),
	m_ratio(1.f)
{
	GetScreenSize();
	Init();
}


CCreadSkin::~CCreadSkin()
{
}


void CCreadSkin::SetDir(std::wstring dir) {
	m_dir = dir;
}

std::wstring CCreadSkin::GetSource(std::wstring strFile) {
	std::wstring resource = m_dir;
	resource.append(strFile);
	return resource;
}

void CCreadSkin::GetScreenSize() {
	m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	m_screenHeight = GetSystemMetrics(SM_CYSCREEN);
	if (m_screenHeight <= 1080) {
		m_ratio = 0.7f;
	}
}


void CCreadSkin::Init() {


	////////////////////////////////////////////////////////
	// v1.2.2 small size version
	// v1.2.2
	// version : 001002002
	////////////////////////////////////////////////////////
	m_version = 001002002;
	SetDir(L"./images/v1.2.2/");
	m_logoAlign = BS_LEFT;
	SetMainThumbnail(CRect(0, 0, 290, 210));
	SetMainMenu(CRect(50, 20, 230 * GetScreenRatio(), 200));
	SetMainMenuBtn(CRect(0, 0, 204 * GetScreenRatio(), 190 * GetScreenRatio()));
	SetBoardMenu(CRect(0, 0, 94 * GetScreenRatio(), 200));
	SetBookMark(CRect(0, 0, 150 * GetScreenRatio(), 100 * GetScreenRatio()));
	SetBookMarkArrow(CRect(50, 15, 22, 22));
	// main up
	m_mainUpHeight = 240;
	m_mainUpAlign = BS_RIGHT;
	SetMainUpBookList(CRect(800, 120, 136, 44));
	SetMainUpVideoList(CRect(600, 120, 136, 44));
	SetMainUpLogo(CRect(0, 1, 289 * GetScreenRatio(), 202 * GetScreenRatio()));
	SetMainUpExit(CRect(340, 30, 125, 124));
	m_fontSize = 14;
	// tool
	SetToolRect(CRect(0, 0, 800, 146));
	SetToolBtn(CRect(0, 0, 73, 72));
	SetToolBtnColor(CRect(0, 16, 48, 48));
	//SetToolBtnClose(CRect(31, 38, 72, 69));
	SetToolBtnClose(CRect(55, 37, 57, 57));
	m_toolBtnPadding = 4;
	m_toolBtnOffsetY = 35;
	m_imgToolMenuBg = L"tool_menu/bg_v.png";
	// ink
	SetInkRect(CRect(0, 0, 82, 372));
	SetInkRectSmall(CRect(0, 0, 72, 69));
	SetInkColor(CRect(6, 2, 43, 42));
	SetInkGone(CRect(11, 6, 58, 44));
	////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////
	//// v2s small size version
	// v1.2.1
	// version : 001002001
	////////////////////////////////////////////////////////
	//m_version = 001002001;
	//SetDir(L"./images/v1.2.1/");
	//m_logoAlign = BS_LEFT;
	//SetMainThumbnail(CRect(0, 0, 290, 210));
	//SetMainMenu(CRect(50, 20, 230, 200));
	//SetMainMenuBtn(CRect(0, 0, 204, 190));
	//SetBoardMenu(CRect(0, 0, 94, 200));
	//SetBookMark(CRect(0, 0, 150, 100));
	//SetBookMarkArrow(CRect(50, 15, 22, 22));
	//// main up
	//m_mainUpHeight = 240;
	//m_mainUpAlign = BS_RIGHT;
	//SetMainUpBookList(CRect(800, 120, 136, 44));
	//SetMainUpVideoList(CRect(600, 120, 136, 44));
	//SetMainUpLogo(CRect(0, 1, 289, 202));
	//SetMainUpExit(CRect(340, 30, 125, 124));
	//m_fontSize = 14;
	//// tool
	//SetToolRect(CRect(0, 0, 912, 158));
	//SetToolBtn(CRect(0, 0, 73, 72));
	//SetToolBtnColor(CRect(0, 64, 53, 52));
	//SetToolBtnClose(CRect(46, 1, 40, 37));
	//m_toolBtnPadding = 4;
	//m_toolBtnOffsetY = 54;
	//m_imgToolMenuBg = L"tool_menu/bg.png";
	//// ink
	//SetInkRect(CRect(0, 0, 82, 372));
	//SetInkRectSmall(CRect(0, 0, 72, 69));
	//SetInkColor(CRect(6, 2, 43, 42));
	//SetInkGone(CRect(11, 6, 58, 44));
	////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////
	//// v2
	//// v2.0
	//// version : 001002000
	////////////////////////////////////////////////////////
	//m_version = 001002000;
	//SetDir(L"./images/v1.2.0/");
	//m_logoAlign = BS_LEFT;
	//SetMainThumbnail(CRect(0, 0, 290, 210));
	//SetMainMenu(CRect(40, 20, 380, 200));
	//SetMainMenuBtn(CRect(0, 0, 350, 330));
	//SetBoardMenu(CRect(0, 0, 163, 200));
	//SetBookMark(CRect(0, 0, 236, 149));
	//SetBookMarkArrow(CRect(50, 15, 37, 39));
	////main up
	//m_mainUpHeight = 340;
	//m_mainUpAlign = BS_RIGHT;
	//SetMainUpBookList(CRect(1050, 270, 235, 59));
	//SetMainUpVideoList(CRect(815, 270, 235, 59));
	//SetMainUpLogo(CRect(0, 20, 478, 310));
	//SetMainUpExit(CRect(580, 40, 177, 177));
	//m_fontSize = 26;
	//// tool
	//SetToolRect(CRect(0, 0, 1374, 273));
	//SetToolBtn(CRect(0, 0, 117, 117));
	//SetToolBtnColor(CRect(0, 72, 84, 83));
	//SetToolBtnClose(CRect(50, 2, 70, 64));
	//m_toolBtnPadding = 22;
	//m_toolBtnOffsetY = 90;
	//m_imgToolMenuBg = L"tool_menu/bg.png";
	//// ink
	//SetInkRect(CRect(0, 0, 127, 630));
	//SetInkRectSmall(CRect(0, 0, 108, 106));
	//SetInkColor(CRect(6, 20, 68, 67));
	//SetInkGone(CRect(8, 20, 44, 43));
	////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////
	// v1
	////////////////////////////////////////////////////////
	//m_version = 001001000;
	//SetDir(L"./images/");
	//m_logoAlign = BS_CENTER;
	//SetMainThumbnail(CRect(0, 0, 290, 210));
	//SetMainMenu(CRect(30, 6, 230, 200));
	//SetMainMenuBtn(CRect(0, 0, 194, 163));
	//SetBoardMenu(CRect(0, 0, 92, 200));
	//SetBookMark(CRect(0, 0, 118, 85));
	//SetBookMarkArrow(CRect(50, 15, 23, 19));
	//// main up
	//m_mainUpHeight = 200;
	//m_mainUpAlign = BS_LEFT;
	//SetMainUpBookList(CRect(84, 88, 152, 58));
	//SetMainUpVideoList(CRect(244, 88, 152, 58));
	//SetMainUpLogo(CRect(0, 40, 181, 100));
	//SetMainUpExit(CRect(340, 30, 80, 80));
	//m_fontSize = 14;
	//// tool
	//SetToolRect(CRect(0, 0, 720, 180));
	//SetToolBtn(CRect(0, 0, 71, 71));
	//SetToolBtnColor(CRect(0, 82, 38, 38));
	//SetToolBtnClose(CRect(50, 10, 17, 17));
	//m_toolBtnPadding = 8;
	//m_toolBtnOffsetY = 90;
	//m_imgToolMenuBg = L"tool_menu/bg.png";
	//// ink
	//SetInkRect(CRect(0, 0, 67, 377));
	//SetInkRectSmall(CRect(0, 0, 67, 66));
	//SetInkColor(CRect(2, 2, 46, 44));
	//SetInkGone(CRect(2, 6, 22, 21));
	////////////////////////////////////////////////////////
}




void CCreadSkin::UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, BYTE alpha)
{
	if (pImage == NULL)
		return;

	if (!IsWindow(hWnd))
		return;

	HDC hdc = ::GetDC(hWnd);
	SIZE sz = { pImage->GetWidth() , pImage->GetHeight() };

	RECT rt = { 0 };
	POINT pt = { 0 };
	::GetWindowRect(hWnd, &rt);
	//::ScreenToClient(hWnd, &rt);

	pt.x = rt.left;
	pt.y = rt.top;
	//TRACE(_T("\n>>> windowrect: %d,%d <<<<\n"), pt.x, pt.y);
	::ScreenToClient(pParent, &pt);
	rt.left = pt.x;
	rt.top = pt.y;
	//TRACE(_T("\n>>> clie: %d,%d <<<<\n"), pt.x, pt.y);

	pt.x = 0;
	pt.y = 0;

	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, pImage->GetWidth(), pImage->GetHeight());
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	Gdiplus::Graphics g(hMemDC);
	//g.Clear(Gdiplus::Color(0,0,0,0));
	//g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	//g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);


	g.DrawImage(pImage, 0, 0, pImage->GetWidth(), pImage->GetHeight());

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA }; // AlphaBlend() 

	::UpdateLayeredWindow(hWnd, hdc, NULL, &sz, hMemDC, &pt, RGB(255,255,255), &bf, ULW_ALPHA);



	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hWnd, hdc);

	DeleteObject(hBitmap);
}

void CCreadSkin::UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, int width, int height)
{
	if (pImage == NULL)
		return;

	if (!IsWindow(hWnd))
		return;

	HDC hdc = ::GetDC(hWnd);


	CRect rt = { 0 };
	POINT pt = { 0 };
	::GetWindowRect(hWnd, &rt);

	SIZE sz = { rt.Width(), rt.Height() };
	//::ScreenToClient(hWnd, &rt);

	//TRACE(_T("\n>>> windowrect: %d,%d <<<<\n"), pt.x, pt.y);
	//rt.left = 0;
	//rt.top = 0;
	//rt.right = rt.left + width;
	//rt.bottom = rt.top + height;
	//TRACE(_T("\n>>> clie: %d,%d <<<<\n"), pt.x, pt.y);

	if (width <= 0) {
		width = pImage->GetWidth();
	}

	if (height <= 0) {
		height = pImage->GetHeight() * GetDpiRatio() * 2;
	}


	pt.x = 0;
	pt.y = 0;

	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rt.Width(), rt.Height());
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	Gdiplus::Graphics g(hMemDC);
	//g.Clear(Gdiplus::Color(0,0,0,0));
	//g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	//g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	g.DrawImage(pImage, 0, 0, width, height);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA }; // AlphaBlend() 
	::UpdateLayeredWindow(hWnd, hdc, NULL, &sz, hMemDC, &pt, 0x00000000, &bf, ULW_ALPHA);


	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hWnd, hdc);

	DeleteObject(hBitmap);
}




void CCreadSkin::UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, COLORREF clr, BOOL bInk, BOOL bPen, BYTE alpha)
{
	if (pImage == NULL)
		return;

	if (!IsWindow(hWnd))
		return;

	HDC hdc = ::GetDC(hWnd);
	SIZE sz = { pImage->GetWidth() , pImage->GetHeight() };

	RECT rt = { 0 };
	POINT pt = { 0 };
	::GetWindowRect(hWnd, &rt);
	//::ScreenToClient(hWnd, &rt);

	pt.x = rt.left;
	pt.y = rt.top;
	//TRACE(_T("\n>>> windowrect: %d,%d <<<<\n"), pt.x, pt.y);
	::ScreenToClient(pParent, &pt);
	rt.left = pt.x;
	rt.top = pt.y;
	//TRACE(_T("\n>>> clie: %d,%d <<<<\n"), pt.x, pt.y);

	pt.x = 0;
	pt.y = 0;

	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, pImage->GetWidth(), pImage->GetHeight());
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	Gdiplus::Graphics g(hMemDC);

	if (bInk) {
		//TRACE(L"\n>>> ink draw (%d)(%d) -rgb(%d,%d,%d)<<<<\n", pImage->GetWidth(), pImage->GetHeight(), GetRValue(clr), GetGValue(clr), GetBValue(clr));
		//Gdiplus::SolidBrush blackBrush(Gdiplus::Color(254, GetRValue(clr), GetGValue(clr), GetBValue(clr)));
		if (alpha == 255) {
			Gdiplus::SolidBrush blackBrush(Gdiplus::Color(254, GetRValue(clr), GetGValue(clr), GetBValue(clr)));

			if (bPen == TRUE) {
				g.FillRectangle(&blackBrush, 6, 6, pImage->GetWidth() - 22, pImage->GetHeight() - 22);
			}
			else {
				g.FillEllipse(&blackBrush, 2, 2, pImage->GetWidth() - pImage->GetWidth()*0.15, pImage->GetHeight() - pImage->GetHeight()*0.15);
			}
		}

	}

	g.DrawImage(pImage, 0, 0, pImage->GetWidth(), pImage->GetHeight());
	

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA }; // AlphaBlend() 

	::UpdateLayeredWindow(hWnd, hdc, NULL, &sz, hMemDC, &pt, RGB(255, 255, 255), &bf, ULW_ALPHA);



	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hWnd, hdc);

	DeleteObject(hBitmap);
}


float CCreadSkin::GetDpiRatio() {
	HMONITOR monitor = MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
	UINT newDpiX = 96;
	UINT newDpiY = 96;
	if (SUCCEEDED(GetDpiForMonitor(monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &newDpiX, &newDpiY)))
	{
	}
	return newDpiX/96;
}

