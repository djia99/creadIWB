// CButtonDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CButtonDlg.h"
#include "afxdialogex.h"


// CButtonDlg 대화 상자

IMPLEMENT_DYNAMIC(CButtonDlg, CDialogEx)

CButtonDlg::CButtonDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BTN_DLG, pParent),
	m_nButtonId(0),
	m_pathNormal(L""),
	m_pathHover(L""),
	m_bIsTracking(FALSE),
	m_width(0),
	m_height(0),
	m_bSelected(FALSE),
	m_clr(RGB(0,0,255)),
	m_bInk(FALSE),
	m_pToolTip(NULL),
	m_bPen(FALSE)
{
}

CButtonDlg::~CButtonDlg()
{
	if (m_pToolTip)	delete m_pToolTip;
}

void CButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CButtonDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CButtonDlg 메시지 처리기
void CButtonDlg::SetImage(UINT nButtonId, HWND pParent, std::wstring pathNormal, std::wstring pathHover, std::wstring pathSelect, int width, int height, COLORREF clr, BOOL bInk) {
	m_nButtonId = nButtonId;
	m_pParent = pParent;
	m_pathNormal = pathNormal;
	m_pathHover = pathHover;
	m_pathSelect = pathSelect;
	m_width = width;
	m_height = height;
	m_clr = clr;
	m_bInk = bInk;

}

void CButtonDlg::SetPen(BOOL b) {
	m_bPen = b;
}

void CButtonDlg::SetColor(COLORREF clr, BOOL bInkUse) {
	m_clr = clr;
	m_bInkUse = bInkUse;
}

BOOL CButtonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);


	m_imgNormal = Gdiplus::Image::FromFile(m_pathNormal.c_str());
	m_imgHover = Gdiplus::Image::FromFile(m_pathHover.c_str());
	m_imgSelect = Gdiplus::Image::FromFile(m_pathSelect.c_str());
	if (m_imgNormal)
	{
		if (m_width == 0 || m_height == 0) {
			if (m_bInk) {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal, m_clr, m_bInk, m_bPen, (m_bInkUse) ? 255 : 100);
			}
			else {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal);
			}
			
		}
		else {
			g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal, m_width, m_height);
		}
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


int CButtonDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;



	return 0;
}


void CButtonDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	

	if (!m_bIsTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bIsTracking = _TrackMouseEvent(&tme);
	}

	//::SendMessage(m_pParent, MSG_CREAD_BUTTON, WM_MOUSEMOVE, m_nButtonId);
	//::PostMessage(m_pParent, MSG_CREAD_BUTTON, WM_MOUSEMOVE, m_nButtonId);
	CDialogEx::OnMouseMove(nFlags, point);
}






//=============================================================================
LRESULT CButtonDlg::OnMouseHover(WPARAM wparam, LPARAM lparam)
//=============================================================================
{
	//RECT rt;
	//GetWindowRect(&rt);
	//TRACE(_T("\n>>> windowrect: %d,%d,%d, %d <<<<\n"),rt.left, rt.top, rt.right, rt.bottom);
	//ScreenToClient(&rt);
	//TRACE(_T("\n>>> clie: %d,%d,%d, %d <<<<\n"), rt.left, rt.top, rt.right, rt.bottom);

	m_bIsHovering = TRUE;
	Invalidate();
	DeleteToolTip();

	// Create a new Tooltip with new Button Size and Location
	SetToolTipText(m_tooltext);

	if (m_pToolTip != NULL)
	{

		if (::IsWindow(m_pToolTip->m_hWnd))
		{
			m_pToolTip->SetDelayTime(200);
			CRect rt;
			GetClientRect(rt);
			m_pToolTip->SetToolRect(this, 1, rt);

			//Display ToolTip
			m_pToolTip->Update();

		}
	}


	BOOL bInkUse = m_bInkUse;

	// 펜 타입 일 경우 : 선택되지 않은 상태면 배경 투명
	if (m_bSelected == FALSE && m_bPen) {
		bInkUse = FALSE;
	}

	if (m_width == 0 || m_height == 0) {
		if (m_bInk) {
			g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgHover, m_clr, m_bInk, m_bPen, (bInkUse) ? 255 : 100);
		}
		else {
			g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgHover);
		}
	}
	else {
		g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgHover, m_width, m_height);
	}
	return 0;
}


//=============================================================================
LRESULT CButtonDlg::OnMouseLeave(WPARAM wparam, LPARAM lparam)
//=============================================================================
{
	m_bIsTracking = FALSE;
	m_bIsHovering = FALSE;

	BOOL bInkUse = m_bInkUse;

	// 펜 타입 일 경우 : 선택되지 않은 상태면 배경 투명
	if (m_bSelected == FALSE && m_bPen) {
		bInkUse = FALSE;
	}

	if (m_bSelected) {
		if (m_width == 0 || m_height == 0) {
			//g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgHover);

			if (m_bInk) {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgSelect, m_clr, m_bInk, m_bPen, (bInkUse) ? 255 : 100);
			}
			else {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgSelect);
			}

		}
		else {
			g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgSelect, m_width, m_height);
		}
	}
	else {
		if (m_width == 0 || m_height == 0) {
			if (m_bInk) {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal, m_clr, m_bInk, m_bPen, (bInkUse) ? 255 : 100);
			}
			else {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal);
			}
			//g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal);
		}
		else {
			g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal, m_width, m_height);
		}
	}

	return 0;
}

void CButtonDlg::OnLButtonDown(UINT nFlags, CPoint point)
{

	//::SendMessage(m_pParent, MSG_CREAD_BUTTON, WM_LBUTTONDOWN, m_nButtonId);
	::PostMessage(m_pParent, MSG_CREAD_BUTTON, WM_LBUTTONDOWN, m_nButtonId);

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CButtonDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	::PostMessage(m_pParent, MSG_CREAD_BUTTON, WM_LBUTTONUP, m_nButtonId);
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CButtonDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	::PostMessage(m_pParent, MSG_CREAD_BUTTON, WM_LBUTTONDBLCLK, m_nButtonId);
	CDialogEx::OnLButtonDblClk(nFlags, point);
}

BOOL CButtonDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//if (pMsg->message == WM_LBUTTONDOWN) {
	//	::PostMessage(m_pParent, pMsg->message, pMsg->wParam, m_nButtonId);
	//}

	if (m_pToolTip != NULL)
	{
		if (::IsWindow(m_pToolTip->m_hWnd))
		{
			m_pToolTip->RelayEvent(pMsg);
		}
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}



void CButtonDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	GetParent()->PostMessage(WM_KEYDOWN, nChar, 0);

}


void CButtonDlg::SetSelect(BOOL bSelect) {
	m_bSelected = bSelect;
	BOOL bInkUse = m_bInkUse;

	// 펜 타입 일 경우 : 선택되지 않은 상태면 배경 투명
	if (m_bSelected == FALSE && m_bPen) {
		bInkUse = FALSE;
	}

	if (m_bSelected) {
		if (m_width == 0 || m_height == 0) {
			//g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgHover);
			if (m_bInk) {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgSelect, m_clr, m_bInk, m_bPen, (bInkUse)?255:100);
			}
			else {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgSelect);
			}
		}
		else {

			g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgSelect, m_width, m_height);
		}
	}
	else {
		if (m_width == 0 || m_height == 0) {
			//g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal);

			if (m_bInk) {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal, m_clr, m_bInk, m_bPen, (bInkUse) ? 255 : 100);
			}
			else {
				g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal);
			}
		}
		else {
			g_pSkin->UpdateLayeredWindowPng(m_pParent, m_hWnd, m_imgNormal, m_width, m_height);
		}
	}

}



//=============================================================================
// Set the tooltip with a string resource
//=============================================================================
void CButtonDlg::SetToolTipText(UINT nId, BOOL bActivate)
{
	// load string resource
	m_tooltext.LoadString(nId);

	// If string resource is not empty
	if (m_tooltext.IsEmpty() == FALSE)
	{
		SetToolTipText(m_tooltext, bActivate);
	}

}

//=============================================================================
// Set the tooltip with a CString
//=============================================================================
void CButtonDlg::SetToolTipText(CString spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText.IsEmpty()) return;


	// Initialize ToolTip
	InitToolTip();
	m_tooltext = spText;

	// If there is no tooltip defined then add it
	if (m_pToolTip->GetToolCount() == 0)
	{
		CRect rectBtn;
		rectBtn.left = 100;
		rectBtn.top = 100;
		rectBtn.right = 200;
		rectBtn.bottom = 200;

		//GetClientRect(rectBtn);
		m_pToolTip->AddTool(this, m_tooltext, rectBtn, 1);
	}

	// Set text for tooltip
	m_pToolTip->UpdateTipText(m_tooltext, this, 1);
	m_pToolTip->SetDelayTime(400);
	m_pToolTip->Activate(bActivate);
}

//=============================================================================
void CButtonDlg::InitToolTip()
//=============================================================================
{
	if (m_pToolTip == NULL)
	{
		m_pToolTip = new CToolTipCtrl;
		// Create ToolTip control
		m_pToolTip->Create(this);
		m_pToolTip->Activate(TRUE);
	}
}

//=============================================================================
void CButtonDlg::DeleteToolTip()
//=============================================================================
{
	// Destroy Tooltip incase the size of the button has changed.
	if (m_pToolTip != NULL)
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
}

