// CMainMenuDlg.cpp: 구현 파일
//


#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CMainMenuDlg.h"
#include "afxdialogex.h"



// CMainMenuDlg 대화 상자

IMPLEMENT_DYNAMIC(CMainMenuDlg, CDialogEx)

CMainMenuDlg::CMainMenuDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_MENU, pParent)
{

}

CMainMenuDlg::~CMainMenuDlg()
{
}

void CMainMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainMenuDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CMainMenuDlg::OnCreadButton)
END_MESSAGE_MAP()


// CMainMenuDlg 메시지 처리기


int CMainMenuDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;



	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);


	int nBtnImageSizeWidth = g_pSkin->m_mainMenuBtn.right;
	int nBtnImageSizeHeight = g_pSkin->m_mainMenuBtn.bottom;

	int nOffsetY = g_pSkin->m_mainMenuBtn.bottom;

	CRect rt;
	GetWindowRect(&rt);

	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"main_menu/menu_bar_bg.png").c_str());
	if (m_pBack)
	{
		g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack , g_pSkin->m_mainMenu.right, 0/*g_pSkin->m_mainMenu.bottom*/);
	}


	m_btnAdd.SetToolTipText(_T("새로운 교재를 추가합니다"), TRUE);
	m_btnAdd.SetImage(BTN_ADD, m_hWnd, g_pSkin->GetSource(L"main_menu/add_icon_n.png"), g_pSkin->GetSource(L"main_menu/add_icon_c.png"), g_pSkin->GetSource(L"main_menu/add_icon_c.png"), nBtnImageSizeWidth, nBtnImageSizeHeight);
	if (m_btnAdd.Create(IDD_BTN_DLG, this)) {
		m_btnAdd.MoveWindow(0, nOffsetY, nBtnImageSizeWidth, nBtnImageSizeHeight);
		m_btnAdd.ShowWindow(SW_SHOW);
	}
	nOffsetY += nBtnImageSizeHeight;

	m_btnDel.SetToolTipText(_T("교재를 삭제합니다"), TRUE);
	m_btnDel.SetImage(BTN_DEL, m_hWnd, g_pSkin->GetSource(L"main_menu/delet_icon_n.png"), g_pSkin->GetSource(L"main_menu/delet_icon_o.png"), g_pSkin->GetSource(L"main_menu/delet_icon_o.png"), nBtnImageSizeWidth, nBtnImageSizeHeight);
	if (m_btnDel.Create(IDD_BTN_DLG, this)) {
		m_btnDel.MoveWindow(0, nOffsetY, nBtnImageSizeWidth, nBtnImageSizeHeight);
		m_btnDel.ShowWindow(SW_SHOW);
	}
	nOffsetY += nBtnImageSizeHeight;

	m_btnSort.SetToolTipText(_T("목록 표시 순서"), TRUE);
	m_btnSort.SetImage(BTN_SORT, m_hWnd, g_pSkin->GetSource(L"main_menu/roteA_icon_n.png"), g_pSkin->GetSource(L"main_menu/roteA_icon_o.png"), g_pSkin->GetSource(L"main_menu/roteA_icon_o.png"), nBtnImageSizeWidth, nBtnImageSizeHeight);
	if (m_btnSort.Create(IDD_BTN_DLG, this)) {
		m_btnSort.MoveWindow(0, nOffsetY, nBtnImageSizeWidth, nBtnImageSizeHeight);
		m_btnSort.ShowWindow(SW_SHOW);
	}
	nOffsetY += nBtnImageSizeHeight;

	m_btnSetting.SetToolTipText(_T("환경 설정"), TRUE);
	m_btnSetting.SetImage(BTN_SETTING, m_hWnd, g_pSkin->GetSource(L"main_menu/setting_icon_n.png"), g_pSkin->GetSource(L"main_menu/setting_icon_o.png"), g_pSkin->GetSource(L"main_menu/setting_icon_o.png"), nBtnImageSizeWidth, nBtnImageSizeHeight);
	if (m_btnSetting.Create(IDD_BTN_DLG, this)) {
		m_btnSetting.MoveWindow(0, nOffsetY, nBtnImageSizeWidth, nBtnImageSizeHeight);
		m_btnSetting.ShowWindow(SW_SHOW);
	}

	return 0;
}


void CMainMenuDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}


void CMainMenuDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
}


BOOL CMainMenuDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)) return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CMainMenuDlg::OnEraseBkgnd(CDC* pDC)
{
	CDialog::OnEraseBkgnd(pDC);
	return TRUE;
}



BOOL CMainMenuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



LRESULT CMainMenuDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONUP) {
		if (lParam == BTN_ADD) {

			((CAbleClassDlg*)GetParent())->AddBook();
		}
		else if (lParam == BTN_DEL) {
			((CAbleClassDlg*)GetParent())->SetDeleteBookMode();
		}
		else if (lParam == BTN_SETTING) {
			((CAbleClassDlg*)GetParent())->DoMainSetting();
		}

	}
	return 0L;
}