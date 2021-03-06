// CMainUpDlg.cpp: 구현 파일
//


#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CMainUpDlg.h"
#include "afxdialogex.h"


// CMainUpDlg 대화 상자

IMPLEMENT_DYNAMIC(CMainUpDlg, CDialogEx)

CMainUpDlg::CMainUpDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_UP, pParent)
{

}

CMainUpDlg::~CMainUpDlg()
{
}

void CMainUpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainUpDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CMainUpDlg::OnCreadButton)
END_MESSAGE_MAP()


// CMainUpDlg 메시지 처리기


int CMainUpDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;



	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	CRect rt;
	GetWindowRect(&rt);

	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"main_menu/menu_up_bg.png").c_str());
	if (m_pBack)
	{
		g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack, rt.Width(), rt.Height());
	}

	
	m_btnBookList.SetImage(BTN_BOOK_LIST, m_hWnd, g_pSkin->GetSource(L"main_menu/book_list_c.png"), g_pSkin->GetSource(L"main_menu/book_list_n.png"), g_pSkin->GetSource(L"main_menu/book_list_n.png"));
	if (m_btnBookList.Create(IDD_BTN_DLG, this)) {
		if (g_pSkin->m_mainUpAlign == BS_LEFT) {
			m_btnBookList.MoveWindow(g_pSkin->m_mainUpBookList.left, g_pSkin->m_mainUpBookList.top, g_pSkin->m_mainUpBookList.right, g_pSkin->m_mainUpBookList.bottom);
		}
		else {
			m_btnBookList.MoveWindow(rt.Width() - g_pSkin->m_mainUpBookList.left, g_pSkin->m_mainUpBookList.top, g_pSkin->m_mainUpBookList.right, g_pSkin->m_mainUpBookList.bottom);
		}
		m_btnBookList.ShowWindow(SW_SHOW);
	}
	m_btnVideoList.SetImage(BTN_VIDEO_LIST, m_hWnd, g_pSkin->GetSource(L"main_menu/video_list_n.png"), g_pSkin->GetSource(L"main_menu/video_list_c.png"), g_pSkin->GetSource(L"main_menu/video_list_c.png"));
	if (m_btnVideoList.Create(IDD_BTN_DLG, this)) {
		if (g_pSkin->m_mainUpAlign == BS_LEFT) {
			m_btnVideoList.MoveWindow(g_pSkin->m_mainUpVideoList.left, g_pSkin->m_mainUpVideoList.top, g_pSkin->m_mainUpVideoList.right, g_pSkin->m_mainUpVideoList.bottom);
		}
		else {
			m_btnVideoList.MoveWindow(rt.Width() - g_pSkin->m_mainUpVideoList.left, g_pSkin->m_mainUpVideoList.top, g_pSkin->m_mainUpVideoList.right, g_pSkin->m_mainUpVideoList.bottom);
		}

		m_btnVideoList.ShowWindow(SW_SHOW);
	}

	//m_btnLogo.SetImage(BTN_LOGO, m_hWnd, g_pSkin->GetSource(L"main_menu/podo_logo.png").c_str(), g_pSkin->GetSource(L"main_menu/podo_logo.png").c_str(), g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.bottom);
	//if (m_btnLogo.Create(IDD_BTN_DLG, this)) {
	//	m_btnLogo.MoveWindow(rt.Width()*0.5- g_pSkin->m_mainUpLogo.left, g_pSkin->m_mainUpLogo.top, g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.bottom);
	//	m_btnLogo.ShowWindow(SW_SHOW);
	//	m_btnLogo.UpdateWindow();
	//}

	m_btnExit.SetImage(BTN_EXIT, m_hWnd, g_pSkin->GetSource(L"main_menu/closed_icon.png"), g_pSkin->GetSource(L"main_menu/closed_icon_o.png"), g_pSkin->GetSource(L"main_menu/closed_icon_o.png"));
	if (m_btnExit.Create(IDD_BTN_DLG, this)) {
		m_btnExit.MoveWindow(rt.Width() - g_pSkin->m_mainUpExit.left, g_pSkin->m_mainUpExit.top, g_pSkin->m_mainUpExit.right, g_pSkin->m_mainUpExit.bottom);
		m_btnExit.ShowWindow(SW_SHOW);
		//m_btnExit.UpdateWindow();
	}

	return 0;
}


void CMainUpDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}


void CMainUpDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (::IsWindow(m_btnExit.m_hWnd)) {

		//m_btnLogo.SetWindowPos(&wndTop, cx*0.5 - 308, 10, 308, 171, SWP_NOZORDER | SWP_NOSIZE);
		//m_btnLogo.SetWindowPos(&wndTop, cx*0.5 - g_pSkin->m_mainUpLogo.left, g_pSkin->m_mainUpLogo.top, g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.bottom, SWP_NOZORDER);
		//m_btnLogo.UpdateWindow();
		if (g_pSkin->m_mainUpAlign == BS_LEFT) {
		}
		else {
			m_btnBookList.SetWindowPos(&wndTop, cx - g_pSkin->m_mainUpBookList.left, g_pSkin->m_mainUpBookList.top, g_pSkin->m_mainUpBookList.right, g_pSkin->m_mainUpBookList.bottom, SWP_NOZORDER | SWP_NOSIZE);
			m_btnBookList.UpdateWindow();
			m_btnVideoList.SetWindowPos(&wndTop, cx - g_pSkin->m_mainUpVideoList.left, g_pSkin->m_mainUpVideoList.top, g_pSkin->m_mainUpVideoList.right, g_pSkin->m_mainUpVideoList.bottom, SWP_NOZORDER | SWP_NOSIZE);
			m_btnVideoList.UpdateWindow();
		}

		m_btnExit.SetWindowPos(&wndTop, cx - g_pSkin->m_mainUpExit.left, g_pSkin->m_mainUpExit.top, g_pSkin->m_mainUpExit.right, g_pSkin->m_mainUpExit.bottom, SWP_NOZORDER | SWP_NOSIZE);
		m_btnExit.UpdateWindow();
		g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack, cx, cy);
	}
}


BOOL CMainUpDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CMainUpDlg::OnEraseBkgnd(CDC* pDC)
{
	//CDialog::OnEraseBkgnd(pDC);
	return FALSE;
}



BOOL CMainUpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



LRESULT CMainUpDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONUP) {
		if (lParam == BTN_EXIT) {

			GetParent()->SendMessage(WM_CLOSE, 0, 0);
		}
	}

	if (wParam == WM_LBUTTONDOWN) {
		if (lParam == BTN_BOOK_LIST) {
		}
		else if (lParam == BTN_VIDEO_LIST) {
			ShellExecute(NULL, L"open", g_database.GetSetting()->root_tcolumn2.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
	return 0L;
}