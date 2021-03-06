// CEditorSetDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CEditorSetDlg.h"
#include "afxdialogex.h"
#include "CEditorDlg.h"


// CEditorSetDlg 대화 상자

IMPLEMENT_DYNAMIC(CEditorSetDlg, CDialogEx)

CEditorSetDlg::CEditorSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDITOR_SET, pParent),
	m_bIsBoardMenuVisible(TRUE),
	m_strMessage(L"Color"),
	m_alpha(255),
	m_bDeleteMode(FALSE)
{
	m_pFont = new Gdiplus::Font(L"맑은 고딕", 12, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	m_pBrush = new Gdiplus::SolidBrush(Gdiplus::Color::AntiqueWhite);
}

CEditorSetDlg::~CEditorSetDlg()
{
}

void CEditorSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLORPICKER_BTN, m_colorPicker);
}


BEGIN_MESSAGE_MAP(CEditorSetDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CEditorSetDlg::OnCreadButton)
	ON_MESSAGE(WM_COLOR_UPDATE, &CEditorSetDlg::OnColorUpdate)
END_MESSAGE_MAP()



int CEditorSetDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);


	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"edit_menu/setting_bg.png").c_str());
	if (m_pBack)
	{
		UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack, m_alpha);
	}

	m_btnDelete.SetImage(BTN_DELETE, m_hWnd, g_pSkin->GetSource(L"main_menu/book_delete_btn_del_n.png").c_str(), g_pSkin->GetSource(L"main_menu/book_delete_btn_del_o.png").c_str(), g_pSkin->GetSource(L"main_menu/book_delete_btn_del_o.png").c_str(), 53, 50);
	if (m_btnDelete.Create(IDD_BTN_DLG, this)) {
		m_btnDelete.MoveWindow(150, 10, 53, 50);
		m_btnDelete.ShowWindow(SW_SHOW);
	}

	m_btnClose.SetImage(BTN_CLOSE, m_hWnd, g_pSkin->GetSource(L"main_menu/book_delete_btn_close_n.png").c_str(), g_pSkin->GetSource(L"main_menu/book_delete_btn_close_o.png").c_str(), g_pSkin->GetSource(L"main_menu/book_delete_btn_close_o.png").c_str());
	if (m_btnClose.Create(IDD_BTN_DLG, this)) {
		m_btnClose.MoveWindow(200, 10, 53, 50);
		m_btnClose.ShowWindow(SW_HIDE);
	}


	m_colorPicker.SetParent(this);

	return 0;
}


LRESULT CEditorSetDlg::OnColorUpdate(WPARAM wParam, LPARAM lParam) {
	COLORREF color = m_colorPicker.GetUpdateColor();
	return 0L;
}



LRESULT CEditorSetDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONDOWN) {
		if (lParam == BTN_DELETE) {
			((CEditorDlg*)GetParent())->DeleteSelectedItem();
			//COLORREF color = 20;
			//CArray<COLORREF, COLORREF> colors;
			//colors.Add(COLORREF(RGB(255,0,0)));
			//colors.Add(COLORREF(RGB(0, 255, 0)));
			//colors.Add(COLORREF(RGB(0, 0, 255)));
			//colors.Add(80);
			//colors.Add(110);
			//colors.Add(150);
			//colors.Add(220);
			//colors.Add(210);
			//colors.Add(780);
			//CString strAutoColorText = L"자동 ";
			//CString strOtherText = L"other color";
			//CString strDocColorsText = L"저장";
			//CList<COLORREF, COLORREF> lstDocColors;

			//COLORREF colorAutomatic = 0;
			//int nColumns = 5;

			//CMFCColorPopupMenu* pPopup = new CMFCColorPopupMenu(&m_colorPicker, colors, color, strAutoColorText, strOtherText, strDocColorsText, lstDocColors, nColumns, colorAutomatic);
			//if (pPopup->Create(this, 0, 0, NULL)) {
			//	//pPopup->ShowWindow(SW_SHOW);
			//	//pPopup->MoveWindow(0, 0, 200, 200);
			//	//pPopup->UpdateWindow();
			//}








			//if (AfxMessageBox(_T("교재를 삭제하시겠습니까?"), MB_OKCANCEL) == IDOK) {
			//	((CAbleClassDlg*)GetParent())->DoDeleteBook();
			//}

			//PROPVARIANT* value;
			//UI_SWATCHCOLORTYPE colorType = static_cast<UI_SWATCHCOLORTYPE>(::PropVariantToUInt32WithDefault(*value, UI_SWATCHCOLORTYPE_NOCOLOR));

			//if (UI_SWATCHCOLORTYPE_RGB == colorType)
			//{
			//}
		}
		else if (lParam == BTN_CLOSE) {
			//((CAbleClassDlg*)GetParent())->SetDeleteBookMode();
		}
	}
	return 0L;
}

//void CEditorSetDlg::OnLButtonDown(UINT nFlags, CPoint point)
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


void CEditorSetDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.


}


void CEditorSetDlg::UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, BYTE alpha) {

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


	Gdiplus::RectF R(10, 6, pImage->GetWidth(), pImage->GetHeight());
	Gdiplus::StringFormat SF;
	SF.SetAlignment(Gdiplus::StringAlignmentNear);
	SF.SetLineAlignment(Gdiplus::StringAlignmentNear);
	g.DrawString(m_strMessage.c_str(), -1, m_pFont, R, &SF, m_pBrush);

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA }; // AlphaBlend() 
	::UpdateLayeredWindow(hWnd, hdc, NULL, &sz, hMemDC, &pt, 0x00000000, &bf, ULW_ALPHA);


	::SelectObject(hMemDC, hOldBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hWnd, hdc);

	DeleteObject(hBitmap);
}


void CEditorSetDlg::SetDeleteMode(BOOL mode) {
	m_bDeleteMode = mode;

	if (m_bDeleteMode) {
		m_strMessage = L"선택 항목 삭제";
	}
	else {
		m_strMessage = L"Color";
	}

	if (m_pBack)
	{
		UpdateLayeredWindowPng(GetParent()->m_hWnd, m_hWnd, m_pBack, m_alpha);
	}
}