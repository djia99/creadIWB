// CInkDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CInkDlg.h"
#include "afxdialogex.h"


// CInkDlg 대화 상자

IMPLEMENT_DYNAMIC(CInkDlg, CDialogEx)

CInkDlg::CInkDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INK, pParent),
	m_bIsGone(FALSE)
{

}

CInkDlg::~CInkDlg()
{
}

void CInkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInkDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CInkDlg::OnCreadButton)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CInkDlg 메시지 처리기
void CInkDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
}


int CInkDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	GetColorInfoFromDatabase();

	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"ink_menu/bg.png").c_str());
	if (m_pBack)
	{
		g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack);
	}

	m_pBackGone = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"ink_menu/open.png").c_str());
	if (m_pBackGone)
	{
		//g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBackGone);
	}

	




	TCHAR bufColorNormal[50];
	TCHAR bufColorHover[50];
	int nOffsetY = g_pSkin->m_inkColor.bottom + g_pSkin->m_inkColor.top;

	for (int i = 0; i < 6; ++i) {
		std::swprintf(bufColorNormal, sizeof(bufColorNormal), g_pSkin->GetSource(L"ink_menu/color_empty_n.png").c_str());
		std::swprintf(bufColorHover, sizeof(bufColorHover), g_pSkin->GetSource(L"ink_menu/color_empty_o.png").c_str());


		m_btnInkColor[i].SetImage(BTN_INK_COLOR1 + i, m_hWnd, bufColorNormal, bufColorHover, bufColorHover, 0, 0, m_color[i].clr, TRUE);
		if (m_btnInkColor[i].Create(IDD_BTN_DLG, this)) {
			m_btnInkColor[i].MoveWindow(g_pSkin->m_inkRect.right * 0.5 - g_pSkin->m_inkColor.right * 0.5 - g_pSkin->m_inkColor.left, nOffsetY, g_pSkin->m_inkColor.right, g_pSkin->m_inkColor.bottom);
			m_btnInkColor[i].ShowWindow(SW_SHOW);
		}

		nOffsetY += g_pSkin->m_inkColor.bottom;
	}

	nOffsetY += g_pSkin->m_inkGone.top;

	m_btnGone.SetImage(BTN_GONE, m_hWnd, g_pSkin->GetSource(L"ink_menu/closed.png").c_str(), g_pSkin->GetSource(L"ink_menu/closed.png").c_str(), g_pSkin->GetSource(L"ink_menu/closed.png").c_str());
	if (m_btnGone.Create(IDD_BTN_DLG, this)) {
		m_btnGone.MoveWindow(g_pSkin->m_inkRect.right * 0.5 - g_pSkin->m_inkGone.right * 0.5 - g_pSkin->m_inkGone.left, nOffsetY, g_pSkin->m_inkGone.right, g_pSkin->m_inkGone.bottom);
		m_btnGone.ShowWindow(SW_SHOW);
	}
	


	return 0;
}


BOOL CInkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CInkDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}


void CInkDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CInkDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);

	CDialogEx::OnLButtonDown(nFlags, point);
}



LRESULT CInkDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONDOWN) {
		if (lParam == BTN_GONE) {
	
			LayoutChange();
		}
		else if (lParam == BTN_INK_COLOR1) {
			((CAbleClassDlg*)GetParent())->SetPenColor(0);
		} 
		else if (lParam == BTN_INK_COLOR2) {
			((CAbleClassDlg*)GetParent())->SetPenColor(1);
		}
		else if (lParam == BTN_INK_COLOR3) {
			((CAbleClassDlg*)GetParent())->SetPenColor(2);
		}
		else if (lParam == BTN_INK_COLOR4) {
			((CAbleClassDlg*)GetParent())->SetPenColor(3);
		}
		else if (lParam == BTN_INK_COLOR5) {
			((CAbleClassDlg*)GetParent())->SetPenColor(4);
		}
		else if (lParam == BTN_INK_COLOR6) {
			((CAbleClassDlg*)GetParent())->SetPenColor(5);
		}
	}

	return 0L;
}

void CInkDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LayoutChange();

	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CInkDlg::LayoutChange() {
	if (m_bIsGone) {
		m_bIsGone = FALSE;
		RECT rt;
		GetWindowRect(&rt);

		g_pSkin->UpdateLayeredWindowPng(GetParent()->m_hWnd, m_hWnd, m_pBack);
		SetWindowPos(NULL, rt.left, rt.top - g_pSkin->m_inkRect.bottom, g_pSkin->m_inkRect.right, g_pSkin->m_inkRect.bottom, SWP_SHOWWINDOW | SWP_NOZORDER);

		for (int i = 0; i < 6; ++i) {
			m_btnInkColor[i].ShowWindow(SW_SHOW);
		}
		m_btnGone.ShowWindow(SW_SHOW);
	}
	else {
		m_bIsGone = TRUE;
		RECT rt;
		GetWindowRect(&rt);
		for (int i = 0; i < 6; ++i) {
			m_btnInkColor[i].ShowWindow(SW_HIDE);
		}
		m_btnGone.ShowWindow(SW_HIDE);

		g_pSkin->UpdateLayeredWindowPng(GetParent()->m_hWnd, m_hWnd, m_pBackGone, 160);
		SetWindowPos(NULL, rt.left, rt.bottom - g_pSkin->m_inkRectSmall.bottom, g_pSkin->m_inkRectSmall.right, g_pSkin->m_inkRectSmall.bottom, SWP_SHOWWINDOW | SWP_NOZORDER);
	}

	RedrawWindow();
}


void CInkDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);
	RECT rt;
	::GetClientRect(GetParent()->m_hWnd, &rt);

	if (lpwndpos->y > rt.bottom - g_pSkin->m_inkRectSmall.bottom) lpwndpos->y = rt.bottom - g_pSkin->m_inkRectSmall.bottom;
	if (lpwndpos->x > rt.right - g_pSkin->m_inkRectSmall.right) lpwndpos->x = rt.right - g_pSkin->m_inkRectSmall.right;

}



void CInkDlg::GetColorInfoFromDatabase() {
	for (int i = 0; i < 6; i++) {
		m_color[i].clr = g_database.GetSetting()->set_ink_color[i];
		m_color[i].b = (g_database.GetSetting()->set_ink_color_on[i] == 1) ? true : false;

		if (m_color[i].b) {
			m_btnInkColor[i].SetColor(m_color[i].clr, TRUE);

			if (::IsWindow(m_btnInkColor[i].GetSafeHwnd())) {
				m_btnInkColor[i].EnableWindow(TRUE);
			}

		}
		else {
			m_btnInkColor[i].SetColor(RGB(0, 255, 0), FALSE);
			if (::IsWindow(m_btnInkColor[i].GetSafeHwnd())) {
				m_btnInkColor[i].EnableWindow(FALSE);
			}
		}
	}
}