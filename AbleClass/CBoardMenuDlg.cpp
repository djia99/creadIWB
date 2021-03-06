// CBoardMenuDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CBoardMenuDlg.h"
#include "afxdialogex.h"



// CBoardMenuDlg 대화 상자

IMPLEMENT_DYNAMIC(CBoardMenuDlg, CDialog)

CBoardMenuDlg::CBoardMenuDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_BOARD_MENU, pParent),
	m_bTrackMouse(FALSE),
	m_nButtonWidth(60)
{

}

CBoardMenuDlg::~CBoardMenuDlg()
{
	::DeleteObject(m_hBitmap);

}

void CBoardMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_ALL, m_btnAll);
	DDX_Control(pDX, IDC_BTN_NEXT, m_btnNext);
	DDX_Control(pDX, IDC_BTN_BEFORE, m_btnBefore);
	DDX_Control(pDX, IDC_BTN_EXPAND, m_btnExpand);
	DDX_Control(pDX, IDC_BTN_REDUCE, m_btnReduce);
	DDX_Control(pDX, IDC_BTN_CLIPPING, m_btnClipping);
	DDX_Control(pDX, IDC_BTN_PEN, m_btnPen);
	DDX_Control(pDX, IDC_BTN_BOARD, m_btnBoard);
	DDX_Control(pDX, IDC_BTN_REC, m_btnRec);
	DDX_Control(pDX, IDC_BTN_HIDDEN, m_btnHidden);
	DDX_Control(pDX, IDC_BTN_EXIT, m_btnExit);
}



BEGIN_MESSAGE_MAP(CBoardMenuDlg, CDialog)
	ON_WM_CREATE()
	//ON_WM_PAINT()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_BEFORE, &CBoardMenuDlg::OnBnClickedBtnBefore)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_EXIT, &CBoardMenuDlg::OnBnClickedBtnExit)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_NEXT, &CBoardMenuDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(IDC_BTN_EXPAND, &CBoardMenuDlg::OnBnClickedBtnExpand)
	ON_BN_CLICKED(IDC_BTN_CLIPPING, &CBoardMenuDlg::OnBnClickedBtnClipping)
	ON_BN_CLICKED(IDC_BTN_PEN, &CBoardMenuDlg::OnBnClickedBtnPen)
	ON_BN_CLICKED(IDC_BTN_REDUCE, &CBoardMenuDlg::OnBnClickedBtnReduce)
	ON_BN_CLICKED(IDC_BTN_ALL, &CBoardMenuDlg::OnBnClickedBtnAll)
	ON_BN_CLICKED(IDC_BTN_BOARD, &CBoardMenuDlg::OnBnClickedBtnBoard)
	ON_BN_CLICKED(IDC_BTN_HIDDEN, &CBoardMenuDlg::OnBnClickedBtnHidden)
	ON_BN_CLICKED(IDC_BTN_REC, &CBoardMenuDlg::OnBnClickedBtnRecord)
END_MESSAGE_MAP()


// CBoardMenuDlg 메시지 처리기
void CBoardMenuDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {

}


int CBoardMenuDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, 255, LWA_ALPHA);
	
	InitDialogSkin();

	//m_btnBefore.SetWindowPos(NULL, 10, 100, 40, 180, SWP_NOACTIVATE | SWP_NOZORDER);


	m_btnAll.LoadStdImage(g_pSkin->GetSource(L"board_menu/all_n.png").c_str());
	m_btnAll.LoadAltImage(g_pSkin->GetSource(L"board_menu/all_o.png").c_str());
	m_btnAll.EnableToggle(FALSE);
	m_btnAll.SetToolTipText(_T("목록 보기 (Ctrl + A)"));

	m_btnNext.LoadStdImage(g_pSkin->GetSource(L"board_menu/next_n.png").c_str());
	m_btnNext.LoadAltImage(g_pSkin->GetSource(L"board_menu/next_o.png").c_str());
	m_btnNext.EnableToggle(FALSE);
	m_btnNext.SetToolTipText(_T("다음 페이지 (D)"));

	m_btnBefore.LoadStdImage(g_pSkin->GetSource(L"board_menu/before_n.png").c_str());
	m_btnBefore.LoadAltImage(g_pSkin->GetSource(L"board_menu/before_o.png").c_str());
	m_btnBefore.EnableToggle(FALSE);
	m_btnBefore.SetToolTipText(_T("이전 페이지 (A)"));

	m_btnExpand.LoadStdImage(g_pSkin->GetSource(L"board_menu/expand_n.png").c_str());
	m_btnExpand.LoadAltImage(g_pSkin->GetSource(L"board_menu/expand_o.png").c_str());
	m_btnExpand.EnableToggle();
	m_btnExpand.SetToolTipText(_T("영역 확대 (W)"));

	m_btnReduce.LoadStdImage(g_pSkin->GetSource(L"board_menu/reduce_n.png").c_str());
	m_btnReduce.LoadAltImage(g_pSkin->GetSource(L"board_menu/reduce_o.png").c_str());
	m_btnReduce.EnableToggle(FALSE);
	m_btnReduce.SetToolTipText(_T("페이지 맞춤 (X)"));

	m_btnClipping.LoadStdImage(g_pSkin->GetSource(L"board_menu/screen_n.png").c_str());
	m_btnClipping.LoadAltImage(g_pSkin->GetSource(L"board_menu/screen_o.png").c_str());
	m_btnClipping.EnableToggle();
	m_btnClipping.SetToolTipText(_T("영역 이미지 캡처 후 판서 (Ctrl + T)"));

	m_btnPen.LoadStdImage(g_pSkin->GetSource(L"board_menu/pen_n.png").c_str());
	m_btnPen.LoadAltImage(g_pSkin->GetSource(L"board_menu/pen_o.png").c_str());
	m_btnPen.EnableToggle();
	m_btnPen.SetToolTipText(_T("판서 모드 켜기 (I)"));

	m_btnBoard.LoadStdImage(g_pSkin->GetSource(L"board_menu/board_n.png").c_str());
	m_btnBoard.LoadAltImage(g_pSkin->GetSource(L"board_menu/board_o.png").c_str());
	m_btnBoard.EnableToggle();
	m_btnBoard.SetToolTipText(_T("잉크 보드 켜기 (Ctrl + N)"));

	m_btnRec.LoadStdImage(g_pSkin->GetSource(L"board_menu/rec_n.png").c_str());
	m_btnRec.LoadAltImage(g_pSkin->GetSource(L"board_menu/rec_o.png").c_str());
	m_btnRec.EnableToggle(FALSE);
	m_btnRec.SetToolTipText(_T("녹화 모드 (Ctrl + G)"));

	m_btnHidden.LoadStdImage(g_pSkin->GetSource(L"board_menu/hidden_n.png").c_str());
	m_btnHidden.LoadAltImage(g_pSkin->GetSource(L"board_menu/hidden_o.png").c_str());
	m_btnHidden.SetSelect(TRUE);
	m_btnHidden.EnableToggle(FALSE);
	m_btnHidden.SetToolTipText(_T("빈칸 항목 열기 (Ctrl + B)"));

	m_btnExit.LoadStdImage(g_pSkin->GetSource(L"board_menu/exit_n.png").c_str());
	m_btnExit.LoadAltImage(g_pSkin->GetSource(L"board_menu/exit_o.png").c_str());
	m_btnExit.EnableToggle(FALSE);
	m_btnExit.SetToolTipText(_T("판서 종료 (Ctrl + Q)"));

	return 0;
}



BOOL CBoardMenuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	Resize();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CBoardMenuDlg::InitDialogSkin() {
	HINSTANCE	hInstResource = NULL;

	// Find correct resource handle
	//hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDB_BKGND), RT_BITMAP);

	//// Load bitmap In
	//m_hBitmap = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(IDB_MAIN_MENU_BG), IMAGE_BITMAP, 0, 0, 0);

	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"board_menu/bg.bmp").c_str());
	if (m_pBack)
	{
		Gdiplus::Bitmap* pBitmap = static_cast<Gdiplus::Bitmap*>(m_pBack->Clone());
		Gdiplus::Status status = pBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &m_hBitmap);
	}
}

void CBoardMenuDlg::OnMouseHover(UINT nFlags, CPoint point)
{
	CDialog::OnMouseHover(nFlags, point);
}


void CBoardMenuDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	//if (!m_bTrackMouse)
	//{
	//	TRACKMOUSEEVENT tme;
	//	tme.cbSize = sizeof(tme);
	//	tme.hwndTrack = m_hWnd;
	//	tme.dwFlags = TME_LEAVE | TME_HOVER;
	//	tme.dwHoverTime = 1;

	//	if (TrackMouseEvent(&tme))
	//	{
	//		m_bTrackMouse = TRUE;
	//	}
	//}
	CDialog::OnMouseMove(nFlags, point);
}


void CBoardMenuDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//BOOL bCloak = TRUE;
	//::DwmSetWindowAttribute(GetSafeHwnd(), DWMWA_CLOAK, &bCloak, sizeof(bCloak));
	//GetParent()->SendMessage(LAYOUT_MESSAGE, IDD_BOARD_MENU, SW_SHOWNOACTIVATE);

	//SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);

	//m_btnImage1 = Gdiplus::Image::FromFile(L"./images/board_menu/before_c.png");
	//m_pBack = Gdiplus::Image::FromFile(L"./images/board_menu/bg.bmp");
	//UpdateLayered_AlphaPNG(m_hWnd, m_pBack);

	CDialog::OnLButtonDown(nFlags, point);
}



BOOL CBoardMenuDlg::OnEraseBkgnd(CDC* pDC)
{
	//CDialog::OnEraseBkgnd(pDC);
	CRect rect;
	GetClientRect(rect);


	CMyMemDC pDevC(pDC, rect);

	if (m_hBitmap)
	{
		BITMAP bit;
		GetObject(m_hBitmap, sizeof(BITMAP), &bit);

		(HBITMAP)pDevC->SelectObject(m_hBitmap);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), pDevC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);

		//pDevC->SelectObject(oldBitmap);
	}

	return TRUE;
}


void CBoardMenuDlg::OnBnClickedBtnExit()
{
	((CAbleClassDlg*)GetParent())->SetBoardClose();
	((CAbleClassDlg*)GetParent())->ResetFocus();

	//GetParent()->SendMessage(LAYOUT_MESSAGE, IDD_BOARD_MENU, SW_HIDE);

}


void CBoardMenuDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	Resize();
}

void CBoardMenuDlg::Resize() {

	if (::IsWindow(m_btnAll.m_hWnd)) {
		CRect rt;
		GetClientRect(&rt);

		UINT imageWidth = 0, imageHeight = 0;
		m_btnAll.GetImageSize(imageWidth, imageHeight);

		imageWidth = imageWidth * g_pSkin->GetScreenRatio();
		imageHeight = imageHeight * g_pSkin->GetScreenRatio();


		//if (imageHeight * 12 > rt.Height()) 
		//{
		//	imageWidth = static_cast<UINT>(imageWidth * 0.8);
		//	imageHeight = static_cast<UINT>(imageHeight * 0.8);
		//}


		int nOffsetX = 0;
		int nOffsetY = g_pSkin->m_bookMark.bottom + 10;

		nOffsetX = static_cast<int>((rt.Width() - imageWidth) * 0.65);

		m_btnAll.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		m_btnNext.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		m_btnBefore.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		nOffsetY += (imageHeight * 0.4);	// 단 나눔
		m_btnExpand.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		m_btnReduce.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		nOffsetY += (imageHeight * 0.4);	// 단 나눔
		m_btnPen.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		m_btnBoard.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		m_btnClipping.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		nOffsetY += (imageHeight * 0.4);	// 단 나눔
		m_btnRec.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		m_btnHidden.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
		nOffsetY += (imageHeight * 0.4);	// 단 나눔
		m_btnExit.MoveWindow(nOffsetX, nOffsetY, imageWidth, imageHeight);
		nOffsetY += (imageHeight);
	}

}


BOOL CBoardMenuDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)) return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}


void CBoardMenuDlg::OnBnClickedBtnNext()
{
	((CAbleClassDlg*)GetParent())->SetBoardNextImage();
	((CAbleClassDlg*)GetParent())->ResetFocus();

}


void CBoardMenuDlg::OnBnClickedBtnBefore()
{
	((CAbleClassDlg*)GetParent())->SetBoardPrevImage();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}

void CBoardMenuDlg::OnBnClickedBtnExpand()
{
	
	((CAbleClassDlg*)GetParent())->SetBoardZoom();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}



void CBoardMenuDlg::OnBnClickedBtnReduce()
{
	((CAbleClassDlg*)GetParent())->SetBoardReduce();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}


void CBoardMenuDlg::OnBnClickedBtnClipping()
{
	((CAbleClassDlg*)GetParent())->SetBoardClipping();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}


void CBoardMenuDlg::OnBnClickedBtnPen()
{
	
	((CAbleClassDlg*)GetParent())->SetBoardPen();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}



void CBoardMenuDlg::OnBnClickedBtnAll()
{
	
	((CAbleClassDlg*)GetParent())->SetListMode();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}


void CBoardMenuDlg::OnBnClickedBtnBoard()
{
	//SetButtonStyle(IDC_BTN_BOARD, TRUE);
	((CAbleClassDlg*)GetParent())->SetEmptyBoard();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}

void CBoardMenuDlg::OnBnClickedBtnHidden()
{
	//SetButtonStyle(IDC_BTN_BOARD, TRUE);
	((CAbleClassDlg*)GetParent())->SetBoardBlank();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}


void CBoardMenuDlg::OnBnClickedBtnRecord()
{
	((CAbleClassDlg*)GetParent())->SetBoardRecord();
	((CAbleClassDlg*)GetParent())->ResetFocus();
}

