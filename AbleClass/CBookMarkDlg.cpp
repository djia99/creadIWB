// CBookMarkDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CBookMarkDlg.h"
#include "afxdialogex.h"


// CBookMarkDlg 대화 상자

IMPLEMENT_DYNAMIC(CBookMarkDlg, CDialogEx)

CBookMarkDlg::CBookMarkDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BOOKMARK, pParent),
	m_bIsBoardMenuVisible(FALSE),
	m_strPage(L"000 / 001"),
	m_alpha(255)
{
	m_pFont = new Gdiplus::Font(L"맑은 고딕", g_pSkin->GetFontSize(), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	m_pBrush = new Gdiplus::SolidBrush(Gdiplus::Color::AntiqueWhite);
}

CBookMarkDlg::~CBookMarkDlg()
{
}

void CBookMarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBookMarkDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CBookMarkDlg 메시지 처리기


int CBookMarkDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	



	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"board_menu/bookmark.png").c_str());
	if (m_pBack)
	{
		UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack, m_alpha);
	}

	m_btnArrow.SetImage(1, m_hWnd, g_pSkin->GetSource(L"board_menu/bookmark_button.png"), g_pSkin->GetSource(L"board_menu/bookmark_button.png"), g_pSkin->GetSource(L"board_menu/bookmark_button.png"), g_pSkin->m_bookMarkArrow.right* g_pSkin->GetScreenRatio(), g_pSkin->m_bookMarkArrow.bottom* g_pSkin->GetScreenRatio());
	if (m_btnArrow.Create(IDD_BTN_DLG, this)) {
		m_btnArrow.MoveWindow(g_pSkin->m_bookMark.right*0.5, g_pSkin->m_bookMarkArrow.top* g_pSkin->GetScreenRatio(), g_pSkin->m_bookMarkArrow.right* g_pSkin->GetScreenRatio(), g_pSkin->m_bookMarkArrow.bottom* g_pSkin->GetScreenRatio());
		m_btnArrow.ShowWindow(SW_SHOW);
	}
	return 0;
}


void CBookMarkDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetBoardMenuVisible(m_bIsBoardMenuVisible = m_bIsBoardMenuVisible ^ 1);
	if (m_bIsBoardMenuVisible) {
		GetParent()->SendMessage(LAYOUT_MESSAGE, IDD_BOARD_MENU, SW_SHOW);
	} else {
		GetParent()->SendMessage(LAYOUT_MESSAGE, IDD_BOARD_MENU, SW_HIDE);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CBookMarkDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	
}

void CBookMarkDlg::SetBoardMenuVisible(BOOL b) {
	m_bIsBoardMenuVisible = b;
	if (m_bIsBoardMenuVisible) {
		m_alpha = 255;
	}
	else {
		m_alpha = 70;
	}

	if (m_pBack)
	{
		UpdateLayeredWindowPng(GetParent()->m_hWnd, m_hWnd, m_pBack, m_alpha);
	}
}


void CBookMarkDlg::UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, BYTE alpha) {

	if (pImage == NULL)
		return;

	if (!IsWindow(hWnd))
		return;

	HDC hdc = ::GetDC(hWnd);
	SIZE sz = { static_cast<INT>(pImage->GetWidth() * g_pSkin->GetScreenRatio()), static_cast<INT>(pImage->GetHeight() * g_pSkin->GetScreenRatio()) };

	RECT rt = { 0 };
	POINT pt = { 0 };
	::GetWindowRect(hWnd, &rt);

	pt.x = rt.left;
	pt.y = rt.top;
	::ScreenToClient(pParent, &pt);
	rt.left = pt.x;
	rt.top = pt.y;

	pt.x = 0;
	pt.y = 0;

	HDC hMemDC = CreateCompatibleDC(hdc);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, static_cast<INT>(pImage->GetWidth() * g_pSkin->GetScreenRatio()), static_cast<INT>(pImage->GetHeight() * g_pSkin->GetScreenRatio()));
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	Gdiplus::Graphics g(hMemDC);
	//g.Clear(Gdiplus::Color(0,0,0,0));
	//g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	//g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);




	g.DrawImage(pImage, 0, 0, static_cast<INT>(pImage->GetWidth() * g_pSkin->GetScreenRatio()), static_cast<INT>(pImage->GetHeight() * g_pSkin->GetScreenRatio()));


	Gdiplus::RectF R(4, 0, static_cast<INT>(pImage->GetWidth() * g_pSkin->GetScreenRatio()), static_cast<INT>(pImage->GetHeight() * g_pSkin->GetScreenRatio()));
	Gdiplus::StringFormat SF;
	SF.SetAlignment(Gdiplus::StringAlignmentCenter);
	SF.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	g.DrawString(m_strPage.c_str(), -1, m_pFont, R, &SF, m_pBrush);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA }; // AlphaBlend() 
	::UpdateLayeredWindow(hWnd, hdc, NULL, &sz, hMemDC, &pt, 0x00000000, &bf, ULW_ALPHA);


	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hWnd, hdc);

	DeleteObject(hBitmap);
}





void CBookMarkDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}