// CBoardSetDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CBoardSetDlg.h"
#include "afxdialogex.h"


// CBoardSetDlg 대화 상자

IMPLEMENT_DYNAMIC(CBoardSetDlg, CDialogEx)

CBoardSetDlg::CBoardSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BOARD_SET, pParent),
	m_bIsBoardMenuVisible(TRUE),
	m_strMessage(L"Color"),
	m_alpha(255),
	m_bDeleteMode(FALSE)
{
	m_pFont = new Gdiplus::Font(L"맑은 고딕", 12, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	m_pBrush = new Gdiplus::SolidBrush(Gdiplus::Color::AntiqueWhite);
}
CBoardSetDlg::~CBoardSetDlg()
{
}

void CBoardSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBoardSetDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CBoardSetDlg::OnCreadButton)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



int CBoardSetDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);


	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"board_menu/setting_bg.png").c_str());
	if (m_pBack)
	{
		UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack, m_alpha);
	}


	m_btnClose.SetImage(BTN_CLOSE, m_hWnd, g_pSkin->GetSource(L"board_menu/closed.png").c_str(), g_pSkin->GetSource(L"board_menu/closed.png").c_str(), g_pSkin->GetSource(L"board_menu/closed.png").c_str());
	if (m_btnClose.Create(IDD_BTN_DLG, this)) {
		m_btnClose.MoveWindow(250, 5, 53, 50);
		m_btnClose.ShowWindow(SW_SHOW);
	}


	return 0;
}





LRESULT CBoardSetDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONDOWN) {
		if (lParam == BTN_DELETE) {
		}
		else if (lParam == BTN_CLOSE) {
			//((CAbleClassDlg*)GetParent())->SetDeleteBookMode();
		}
	}
	return 0L;
}



void CBoardSetDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.


}


void CBoardSetDlg::UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, BYTE alpha) {

	if (pImage == NULL)
		return;

	if (!IsWindow(hWnd))
		return;

	HDC hdc = ::GetDC(hWnd);
	SIZE sz = { pImage->GetWidth() , pImage->GetHeight() };

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
	HBITMAP hBitmap = CreateCompatibleBitmap(hdc, pImage->GetWidth(), pImage->GetHeight());
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	Gdiplus::Graphics g(hMemDC);
	//g.Clear(Gdiplus::Color(0,0,0,0));
	//g.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	//g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);


	g.DrawImage(pImage, 0, 0, pImage->GetWidth(), pImage->GetHeight());


	Gdiplus::RectF R(10, 0, pImage->GetWidth(), pImage->GetHeight());
	Gdiplus::StringFormat SF;
	SF.SetAlignment(Gdiplus::StringAlignmentNear);
	SF.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	g.DrawString(m_strMessage.c_str(), -1, m_pFont, R, &SF, m_pBrush);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA }; // AlphaBlend() 
	::UpdateLayeredWindow(hWnd, hdc, NULL, &sz, hMemDC, &pt, 0x00000000, &bf, ULW_ALPHA);


	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hWnd, hdc);

	DeleteObject(hBitmap);
}


//void CBoardSetDlg::SetDeleteMode(BOOL mode) {
//	m_bDeleteMode = mode;
//
//	if (m_bDeleteMode) {
//		m_strMessage = L"선택 항목 삭제";
//	}
//	else {
//		m_strMessage = L"Color";
//	}
//
//	if (m_pBack)
//	{
//		UpdateLayeredWindowPng(GetParent()->m_hWnd, m_hWnd, m_pBack, m_alpha);
//	}
//}

void CBoardSetDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//TRACE("\n>>>>>> key down <<<\n");
	GetParent()->SendMessage(WM_KEYDOWN, nChar, 0);
	//CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}
