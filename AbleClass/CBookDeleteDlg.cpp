// CBookDeleteDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CBookDeleteDlg.h"
#include "afxdialogex.h"
#include "AbleClassDlg.h"


// CBookDeleteDlg 대화 상자

IMPLEMENT_DYNAMIC(CBookDeleteDlg, CDialogEx)

CBookDeleteDlg::CBookDeleteDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BOOK_DELETE, pParent),
	m_bIsBoardMenuVisible(TRUE),
	m_strMessage(L"삭제할 항목을 선택하세요"),
	m_alpha(255)
{
	m_pFont = new Gdiplus::Font(L"맑은 고딕", 22, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	m_pBrush = new Gdiplus::SolidBrush(Gdiplus::Color::AntiqueWhite);
}

CBookDeleteDlg::~CBookDeleteDlg()
{
}

void CBookDeleteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBookDeleteDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CBookDeleteDlg::OnCreadButton)
END_MESSAGE_MAP()




int CBookDeleteDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;





	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	m_pBack = Gdiplus::Image::FromFile(L"./images/main_menu/book_delete_bg.png");
	if (m_pBack)
	{
		UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack, m_alpha);
	}

	m_btnDelete.SetImage(BTN_DELETE, m_hWnd, L"./images/main_menu/book_delete_btn_del_n.png", L"./images/main_menu/book_delete_btn_del_o.png", L"./images/main_menu/book_delete_btn_del_o.png");
	if (m_btnDelete.Create(IDD_BTN_DLG, this)) {
		m_btnDelete.MoveWindow(450, 56, 106, 100);
		m_btnDelete.ShowWindow(SW_SHOW);
	}

	m_btnClose.SetImage(BTN_CLOSE, m_hWnd, L"./images/main_menu/book_delete_btn_close_n.png", L"./images/main_menu/book_delete_btn_close_o.png", L"./images/main_menu/book_delete_btn_close_o.png");
	if (m_btnClose.Create(IDD_BTN_DLG, this)) {
		m_btnClose.MoveWindow(560, 56, 106, 100);
		m_btnClose.ShowWindow(SW_SHOW);
	}

	return 0;
}



LRESULT CBookDeleteDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONDOWN) {
		if (lParam == BTN_DELETE) {
			if (AfxMessageBox(_T("교재를 삭제하시겠습니까?"), MB_OKCANCEL) == IDOK) {
				((CAbleClassDlg*)GetParent())->DoDeleteBook();
			}
		}
		else if (lParam == BTN_CLOSE) {
			((CAbleClassDlg*)GetParent())->SetDeleteBookMode();
		}
	}
	return 0L;
}

//void CBookDeleteDlg::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	if (m_bIsBoardMenuVisible) {
//		m_alpha = 90;
//		m_bIsBoardMenuVisible = FALSE;
//		GetParent()->SendMessage(LAYOUT_MESSAGE, IDD_BOARD_MENU, SW_HIDE);
//	}
//	else {
//		m_alpha = 255;
//		m_bIsBoardMenuVisible = TRUE;
//		GetParent()->SendMessage(LAYOUT_MESSAGE, IDD_BOARD_MENU, SW_SHOW);
//	}
//
//	//if (m_pBack)
//	//{
//	//	UpdateLayeredWindowPng(GetParent()->m_hWnd, m_hWnd, m_pBack, m_alpha);
//	//}
//
//
//	CDialogEx::OnLButtonDown(nFlags, point);
//}


void CBookDeleteDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.


}


void CBookDeleteDlg::UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, BYTE alpha) {

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


	Gdiplus::RectF R(20, 0, pImage->GetWidth(), pImage->GetHeight());
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



