// CVideoClipDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CVideoClipDlg.h"
#include "afxdialogex.h"


// CVideoClipDlg 대화 상자

IMPLEMENT_DYNAMIC(CVideoClipDlg, CDialogEx)

CVideoClipDlg::CVideoClipDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIDEO_CLIP, pParent)
{

}

CVideoClipDlg::~CVideoClipDlg()
{
}

void CVideoClipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoClipDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CVideoClipDlg::OnCreadButton)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


int CVideoClipDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SetWindowLong(m_hWnd, GWL_EXSTYLE,
	//	GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	//m_pBack = Gdiplus::Image::FromFile(L"./images/edit_menu/clip.png");
	//if (m_pBack)
	//{
	//	g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack);
	//}

	return 0;
}


BOOL CVideoClipDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CVideoClipDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}


void CVideoClipDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CVideoClipDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnLButtonDown(nFlags, point);
}






LRESULT CVideoClipDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONDOWN) {
	}

	return 0L;
}

void CVideoClipDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	GetParent()->SendMessage(MY_WM_VIDEOCLIP, 0, 0);
	CDialogEx::OnLButtonDblClk(nFlags, point);
}



void CVideoClipDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);
	//RECT rt;
	//::GetClientRect(GetParent()->m_hWnd, &rt);

	//if (lpwndpos->y > rt.bottom - 66) lpwndpos->y = rt.bottom - 66;
	//if (lpwndpos->x > rt.right - 66) lpwndpos->x = rt.right - 66;

}





void CVideoClipDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	/*
	// key map
	// 일반 모드, 필기 모드
	1. 이전 페이지, 펜 종류 - A
	2. 다음 페이지, 펜 색상 - D
	3. 확대 축소, 이동 모드 - W
	4. 영역 확대, 선지우개 - X
	5. 필기 모드, 화면 모드 전환 - I
	6. 5번과 동일
	*/
	GetParent()->SendMessage(WM_KEYDOWN, nChar, 0);
	
}