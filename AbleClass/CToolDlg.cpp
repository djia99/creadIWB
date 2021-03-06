// CToolDlg.cpp: 구현 파일
//


#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CToolDlg.h"
#include "afxdialogex.h"




// CToolDlg 대화 상자

IMPLEMENT_DYNAMIC(CToolDlg, CDialogEx)

CToolDlg::CToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOOL_MENU, pParent),
	//nButtonSize(78),
	//nColorSize(46),
	//nLeftOffset(26)
	nLeftOffset(36),
	m_bInkMode(FALSE),
	m_bPen(TRUE),
	m_bClick(FALSE),
	m_bDrag(FALSE)
{

}

CToolDlg::~CToolDlg()
{
}

void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CToolDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_WINDOWPOSCHANGING()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CToolDlg::OnCreadButton)
END_MESSAGE_MAP()


// CToolDlg 메시지 처리기
void CToolDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
}

int CToolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;




	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetImgToolMenuBg().c_str());
	if (m_pBack)
	{
		g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack);
	}

	m_pBackInk = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"tool_menu/bg_v_ink.png").c_str());

	int nBtnImageSize = g_pSkin->m_toolBtn.right;
	int nBtnColorSize = g_pSkin->m_toolBtnColor.right;
	int nPadding = g_pSkin->m_toolBtnPadding;
	int nToolBtnOffsetY = g_pSkin->m_toolBtnOffsetY;


	m_btnPen.SetToolTipText(_T("일반펜"), TRUE);
	m_btnPen.SetPen(TRUE);
	m_btnPen.SetImage(BTN_PEN, m_hWnd, g_pSkin->GetSource(L"tool_menu/pen_trans_n.png"), g_pSkin->GetSource(L"tool_menu/pen_trans_n.png"), g_pSkin->GetSource(L"tool_menu/pen_trans_n.png"), 0, 0, m_color[0].clr, TRUE);

	if (m_btnPen.Create(IDD_BTN_DLG, this)) {
		m_btnPen.MoveWindow(nLeftOffset, nToolBtnOffsetY, nBtnImageSize, nBtnImageSize);
		m_btnPen.ShowWindow(SW_SHOW);
	}
	


	nLeftOffset += (nBtnImageSize + nPadding);
	m_btnHPen.SetToolTipText(_T("형광펜"), TRUE);
	m_btnHPen.SetPen(TRUE);
	m_btnHPen.SetImage(BTN_HPEN, m_hWnd, g_pSkin->GetSource(L"tool_menu/pen2_trans_n.png"), g_pSkin->GetSource(L"tool_menu/pen2_trans_n.png"), g_pSkin->GetSource(L"tool_menu/pen2_trans_n.png"), 0, 0, m_color[0].clr, TRUE);
	if (m_btnHPen.Create(IDD_BTN_DLG, this)) {
		m_btnHPen.MoveWindow(nLeftOffset, nToolBtnOffsetY, nBtnImageSize, nBtnImageSize);
		m_btnHPen.ShowWindow(SW_SHOW);
	}

	//std::wstring strColorNormal = g_pSkin->GetSource(L"tool_menu/color%02d_n.png";
	//std::wstring strColorHover = g_pSkin->GetSource(L"tool_menu/color%02d_c.png";
	//std::string strColorNormalTest = "./images/tool_menu/color%d_n.png";


	TCHAR bufColorNormal[50];
	TCHAR bufColorHover[50];

	TCHAR bufPenSizeNormal[50];
	TCHAR bufPenSizeHover[50];



	GetColorInfoFromDatabase();


	

	if (g_pSkin->m_version == SKIN_VER_122) {
		nLeftOffset += (nBtnImageSize + nBtnColorSize * 0.5);
		int nLeftInkOffset = nLeftOffset;

		for (int i = 0; i < 6; ++i) {
			std::swprintf(bufColorNormal, sizeof(bufColorNormal), g_pSkin->GetSource(L"tool_menu/color_empty_n.png").c_str());
			std::swprintf(bufColorHover, sizeof(bufColorHover), g_pSkin->GetSource(L"tool_menu/color_empty_o.png").c_str());


			m_btnColor[i].SetToolTipText(_T("색상 선택"), TRUE);
			m_btnColor[i].SetImage(BTN_COLOR1 + i, m_hWnd, bufColorNormal, bufColorHover, bufColorHover, 0, 0, m_color[i].clr, TRUE);

			if (i < 3) {
				nLeftInkOffset = nLeftOffset + i * (nBtnColorSize + nPadding);
				if (m_btnColor[i].Create(IDD_BTN_DLG, this)) {
					m_btnColor[i].MoveWindow(nLeftInkOffset, g_pSkin->m_toolBtnColor.top, nBtnColorSize, nBtnColorSize);
					m_btnColor[i].ShowWindow(SW_SHOW);
				}
			}
			else {
				nLeftInkOffset = nLeftOffset + (i - 3) * (nBtnColorSize + nPadding);
				if (m_btnColor[i].Create(IDD_BTN_DLG, this)) {
					m_btnColor[i].MoveWindow(nLeftInkOffset, g_pSkin->m_toolBtnColor.top + nBtnColorSize, nBtnColorSize, nBtnColorSize);
					m_btnColor[i].ShowWindow(SW_SHOW);
				}
			}
		}

		nLeftOffset = nLeftInkOffset + (nBtnColorSize + nPadding);

		int penTextWidth = 53;
		int penTextHeight = 31;

		m_btnPenSizeText1.SetToolTipText(_T("작은 사이즈 펜"), TRUE);
		m_btnPenSizeText1.SetImage(BTN_PENSIZE_TEXT1, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_1_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_1_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_1_v.png"));
		if (m_btnPenSizeText1.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText1.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top + 36, penTextWidth, penTextHeight);
			m_btnPenSizeText1.ShowWindow(SW_SHOW);
		}

		m_btnPenSizeText2.SetToolTipText(_T("보통 사이즈 펜"), TRUE);
		m_btnPenSizeText2.SetImage(BTN_PENSIZE_TEXT2, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_2_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_2_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_2_v.png"));
		if (m_btnPenSizeText2.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText2.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top + 36, penTextWidth, penTextHeight);
			m_btnPenSizeText2.ShowWindow(SW_HIDE);
		}

		m_btnPenSizeText3.SetToolTipText(_T("큰 사이즈 펜"), TRUE);
		m_btnPenSizeText3.SetImage(BTN_PENSIZE_TEXT3, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_3_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_3_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_3_v.png"));
		if (m_btnPenSizeText3.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText3.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top + 36, penTextWidth, penTextHeight);
			m_btnPenSizeText3.ShowWindow(SW_HIDE);
		}

		m_btnPenSizeText4.SetToolTipText(_T("큰 사이즈 펜"), TRUE);
		m_btnPenSizeText4.SetImage(BTN_PENSIZE_TEXT4, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_4_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_4_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_4_v.png"));
		if (m_btnPenSizeText4.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText4.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top + 36, penTextWidth, penTextHeight);
			m_btnPenSizeText4.ShowWindow(SW_HIDE);
		}


		m_btnPenSizeText5.SetToolTipText(_T("큰 사이즈 펜"), TRUE);
		m_btnPenSizeText5.SetImage(BTN_PENSIZE_TEXT5, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_5_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_5_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_5_v.png"));
		if (m_btnPenSizeText5.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText5.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top + 36, penTextWidth, penTextHeight);
			m_btnPenSizeText5.ShowWindow(SW_HIDE);
		}

		m_btnPenSizeText6.SetToolTipText(_T("큰 사이즈 펜"), TRUE);
		m_btnPenSizeText6.SetImage(BTN_PENSIZE_TEXT6, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_6_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_6_v.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_6_v.png"));
		if (m_btnPenSizeText6.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText6.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top + 36, penTextWidth, penTextHeight);
			m_btnPenSizeText6.ShowWindow(SW_HIDE);
		}


		//nLeftOffset += (48);
		m_btnPenSizeUp.SetToolTipText(_T("펜 사이즈 UP"), TRUE);
		m_btnPenSizeUp.SetImage(BTN_PENSIZE_UP, m_hWnd, g_pSkin->GetSource(L"tool_menu/pen_size_up_n_v.png"), g_pSkin->GetSource(L"tool_menu/pen_size_up_o_v.png"), g_pSkin->GetSource(L"tool_menu/pen_size_up_o_v.png"));
		if (m_btnPenSizeUp.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeUp.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top, 53, 52);
			m_btnPenSizeUp.ShowWindow(SW_SHOW);
		}

		m_btnPenSizeDown.SetToolTipText(_T("펜 사이즈 DOWN"), TRUE);
		m_btnPenSizeDown.SetImage(BTN_PENSIZE_DOWN, m_hWnd, g_pSkin->GetSource(L"tool_menu/pen_size_down_n_v.png"), g_pSkin->GetSource(L"tool_menu/pen_size_down_o_v.png"), g_pSkin->GetSource(L"tool_menu/pen_size_down_o_v.png"));
		if (m_btnPenSizeDown.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeDown.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top + 52, 53, 51);
			m_btnPenSizeDown.ShowWindow(SW_SHOW);
		}



		nLeftOffset += (nBtnColorSize + nBtnColorSize*0.5);
		m_btnEraser.SetToolTipText(_T("선 지우개"), TRUE);
		m_btnEraser.SetImage(BTN_ERASER, m_hWnd, g_pSkin->GetSource(L"tool_menu/eraser_n.png"), g_pSkin->GetSource(L"tool_menu/eraser_o.png"), g_pSkin->GetSource(L"tool_menu/eraser_o.png"));
		if (m_btnEraser.Create(IDD_BTN_DLG, this)) {
			m_btnEraser.MoveWindow(nLeftOffset, nToolBtnOffsetY, nBtnImageSize, nBtnImageSize);
			m_btnEraser.ShowWindow(SW_SHOW);
		}

		nLeftOffset += (nBtnImageSize + nPadding);
		m_btnWastebasket.SetToolTipText(_T("전체 지우기"), TRUE);
		m_btnWastebasket.SetImage(BTN_WASTEBASKET, m_hWnd, g_pSkin->GetSource(L"tool_menu/wastebasket_n.png"), g_pSkin->GetSource(L"tool_menu/wastebasket_o.png"), g_pSkin->GetSource(L"tool_menu/wastebasket_o.png"));
		if (m_btnWastebasket.Create(IDD_BTN_DLG, this)) {
			m_btnWastebasket.MoveWindow(nLeftOffset, nToolBtnOffsetY, nBtnImageSize, nBtnImageSize);
			m_btnWastebasket.ShowWindow(SW_SHOW);
		}

		nLeftOffset += (nBtnImageSize + nPadding);
		m_btnReturn.SetToolTipText(_T("작업 취소"), TRUE);
		m_btnReturn.SetImage(BTN_RETURN, m_hWnd, g_pSkin->GetSource(L"tool_menu/return_n.png"), g_pSkin->GetSource(L"tool_menu/return_o.png"), g_pSkin->GetSource(L"tool_menu/return_o.png"));
		if (m_btnReturn.Create(IDD_BTN_DLG, this)) {
			m_btnReturn.MoveWindow(nLeftOffset, nToolBtnOffsetY, nBtnImageSize, nBtnImageSize);
			m_btnReturn.ShowWindow(SW_SHOW);
		}

		nLeftOffset += (nBtnImageSize + nPadding);
		m_btnClose.SetToolTipText(_T("잉크 툴"), TRUE);
		m_btnClose.SetImage(BTN_CLOSE, m_hWnd, g_pSkin->GetSource(L"tool_menu/palette_ink_hpen.png"), g_pSkin->GetSource(L"tool_menu/palette_ink_pen.png"), g_pSkin->GetSource(L"tool_menu/palette_ink_pen.png"), 0, 0, m_color[0].clr, TRUE);
		if (m_btnClose.Create(IDD_BTN_DLG, this)) {
			m_btnClose.MoveWindow(nLeftOffset + g_pSkin->m_toolBtnClose.left, g_pSkin->m_toolBtnClose.top, g_pSkin->m_toolBtnClose.right, g_pSkin->m_toolBtnClose.bottom);
			m_btnClose.ShowWindow(SW_SHOW);
		}

	}
	else {
		nLeftOffset += (nBtnImageSize + nPadding);
		for (int i = 0; i < 6; ++i) {
			std::swprintf(bufColorNormal, sizeof(bufColorNormal), g_pSkin->GetSource(L"tool_menu/color_empty_n.png").c_str());
			std::swprintf(bufColorHover, sizeof(bufColorHover), g_pSkin->GetSource(L"tool_menu/color_empty_o.png").c_str());


			m_btnColor[i].SetToolTipText(_T("색상 선택"), TRUE);
			m_btnColor[i].SetImage(BTN_COLOR1 + i, m_hWnd, bufColorNormal, bufColorHover, bufColorHover, 0, 0, m_color[i].clr, TRUE);

			if (m_btnColor[i].Create(IDD_BTN_DLG, this)) {
				m_btnColor[i].MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top, nBtnColorSize, nBtnColorSize);
				m_btnColor[i].ShowWindow(SW_SHOW);
			}
			nLeftOffset += (nBtnColorSize + nPadding);
		}

		m_btnPenSizeText1.SetToolTipText(_T("작은 사이즈 펜"), TRUE);
		m_btnPenSizeText1.SetImage(BTN_PENSIZE_TEXT1, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_1.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_1.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_1.png"));
		if (m_btnPenSizeText1.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText1.MoveWindow(nLeftOffset + 33, g_pSkin->m_toolBtnColor.top, 33, 45);
			m_btnPenSizeText1.ShowWindow(SW_SHOW);
		}

		m_btnPenSizeText2.SetToolTipText(_T("보통 사이즈 펜"), TRUE);
		m_btnPenSizeText2.SetImage(BTN_PENSIZE_TEXT2, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_2.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_2.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_2.png"));
		if (m_btnPenSizeText2.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText2.MoveWindow(nLeftOffset + 33, g_pSkin->m_toolBtnColor.top, 33, 45);
			m_btnPenSizeText2.ShowWindow(SW_HIDE);
		}

		m_btnPenSizeText3.SetToolTipText(_T("큰 사이즈 펜"), TRUE);
		m_btnPenSizeText3.SetImage(BTN_PENSIZE_TEXT3, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_3.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_3.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_3.png"));
		if (m_btnPenSizeText3.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText3.MoveWindow(nLeftOffset + 33, g_pSkin->m_toolBtnColor.top, 33, 45);
			m_btnPenSizeText3.ShowWindow(SW_HIDE);
		}

		m_btnPenSizeText4.SetToolTipText(_T("큰 사이즈 펜"), TRUE);
		m_btnPenSizeText4.SetImage(BTN_PENSIZE_TEXT4, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_4.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_4.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_4.png"));
		if (m_btnPenSizeText4.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText4.MoveWindow(nLeftOffset + 33, g_pSkin->m_toolBtnColor.top, 33, 45);
			m_btnPenSizeText4.ShowWindow(SW_HIDE);
		}


		m_btnPenSizeText5.SetToolTipText(_T("큰 사이즈 펜"), TRUE);
		m_btnPenSizeText5.SetImage(BTN_PENSIZE_TEXT5, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_5.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_5.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_5.png"));
		if (m_btnPenSizeText5.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText5.MoveWindow(nLeftOffset + 33, g_pSkin->m_toolBtnColor.top, 33, 45);
			m_btnPenSizeText5.ShowWindow(SW_HIDE);
		}

		m_btnPenSizeText6.SetToolTipText(_T("큰 사이즈 펜"), TRUE);
		m_btnPenSizeText6.SetImage(BTN_PENSIZE_TEXT6, m_hWnd, g_pSkin->GetSource(L"tool_menu/pensize_text_6.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_6.png"), g_pSkin->GetSource(L"tool_menu/pensize_text_6.png"));
		if (m_btnPenSizeText6.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeText6.MoveWindow(nLeftOffset + 33, g_pSkin->m_toolBtnColor.top, 33, 45);
			m_btnPenSizeText6.ShowWindow(SW_HIDE);
		}


		m_btnPenSizeDown.SetToolTipText(_T("펜 사이즈 DOWN"), TRUE);
		m_btnPenSizeDown.SetImage(BTN_PENSIZE_DOWN, m_hWnd, g_pSkin->GetSource(L"tool_menu/pen_size_down_n.png"), g_pSkin->GetSource(L"tool_menu/pen_size_down_o.png"), g_pSkin->GetSource(L"tool_menu/pen_size_down_o.png"));
		if (m_btnPenSizeDown.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeDown.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top, 48, 52);
			m_btnPenSizeDown.ShowWindow(SW_SHOW);
		}
		nLeftOffset += (48);
		m_btnPenSizeUp.SetToolTipText(_T("펜 사이즈 UP"), TRUE);
		m_btnPenSizeUp.SetImage(BTN_PENSIZE_UP, m_hWnd, g_pSkin->GetSource(L"tool_menu/pen_size_up_n.png"), g_pSkin->GetSource(L"tool_menu/pen_size_up_o.png"), g_pSkin->GetSource(L"tool_menu/pen_size_up_o.png"));
		if (m_btnPenSizeUp.Create(IDD_BTN_DLG, this)) {
			m_btnPenSizeUp.MoveWindow(nLeftOffset, g_pSkin->m_toolBtnColor.top, 57, 52);
			m_btnPenSizeUp.ShowWindow(SW_SHOW);
		}


		nLeftOffset += (57 + nPadding);
		m_btnEraser.SetToolTipText(_T("선 지우개"), TRUE);
		m_btnEraser.SetImage(BTN_ERASER, m_hWnd, g_pSkin->GetSource(L"tool_menu/eraser_n.png"), g_pSkin->GetSource(L"tool_menu/eraser_o.png"), g_pSkin->GetSource(L"tool_menu/eraser_o.png"));
		if (m_btnEraser.Create(IDD_BTN_DLG, this)) {
			m_btnEraser.MoveWindow(nLeftOffset, nToolBtnOffsetY, nBtnImageSize, nBtnImageSize);
			m_btnEraser.ShowWindow(SW_SHOW);
		}

		nLeftOffset += (nBtnImageSize + nPadding);
		m_btnWastebasket.SetToolTipText(_T("전체 지우기"), TRUE);
		m_btnWastebasket.SetImage(BTN_WASTEBASKET, m_hWnd, g_pSkin->GetSource(L"tool_menu/wastebasket_n.png"), g_pSkin->GetSource(L"tool_menu/wastebasket_o.png"), g_pSkin->GetSource(L"tool_menu/wastebasket_o.png"));
		if (m_btnWastebasket.Create(IDD_BTN_DLG, this)) {
			m_btnWastebasket.MoveWindow(nLeftOffset, nToolBtnOffsetY, nBtnImageSize, nBtnImageSize);
			m_btnWastebasket.ShowWindow(SW_SHOW);
		}

		nLeftOffset += (nBtnImageSize + nPadding);
		m_btnReturn.SetToolTipText(_T("작업 취소"), TRUE);
		m_btnReturn.SetImage(BTN_RETURN, m_hWnd, g_pSkin->GetSource(L"tool_menu/return_n.png"), g_pSkin->GetSource(L"tool_menu/return_o.png"), g_pSkin->GetSource(L"tool_menu/return_o.png"));
		if (m_btnReturn.Create(IDD_BTN_DLG, this)) {
			m_btnReturn.MoveWindow(nLeftOffset, nToolBtnOffsetY, nBtnImageSize, nBtnImageSize);
			m_btnReturn.ShowWindow(SW_SHOW);
		}

		m_btnClose.SetToolTipText(_T("판서 모드 종료"), TRUE);
		m_btnClose.SetImage(BTN_CLOSE, m_hWnd, g_pSkin->GetSource(L"tool_menu/closed_n.png"), g_pSkin->GetSource(L"tool_menu/closed_o.png"), g_pSkin->GetSource(L"tool_menu/closed_o.png"));
		if (m_btnClose.Create(IDD_BTN_DLG, this)) {
			m_btnClose.MoveWindow(nLeftOffset + g_pSkin->m_toolBtnClose.left, g_pSkin->m_toolBtnClose.top, g_pSkin->m_toolBtnClose.right, g_pSkin->m_toolBtnClose.bottom);
			m_btnClose.ShowWindow(SW_SHOW);
		}

	}

	return 0;
}


BOOL CToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();




	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CToolDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CToolDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnMouseMove(nFlags, point);
}

void CToolDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//GetParent()->SendMessage(LAYOUT_MESSAGE, IDD_TOOL_MENU, SW_HIDE);

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CToolDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (g_pSkin->m_version == SKIN_VER_122) {
	}
	else {
		GetParent()->SendMessage(LAYOUT_MESSAGE, IDD_TOOL_MENU, SW_HIDE);
	}
	
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CToolDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	RECT rt;
	::GetClientRect(GetParent()->m_hWnd, &rt);

	int offset = g_pSkin->m_toolBtnClose.top * 2 + g_pSkin->m_toolBtnClose.bottom;
	if (lpwndpos->y > rt.bottom - offset) lpwndpos->y = rt.bottom - offset;
	if (lpwndpos->x < rt.left) lpwndpos->x = rt.left;
	if (lpwndpos->x + lpwndpos->cx > rt.right) lpwndpos->x = rt.right - lpwndpos->cx;
	

}

LRESULT CToolDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {


	if (g_pSkin->m_version == SKIN_VER_122) {
		if (lParam == BTN_CLOSE) {
			if (wParam == WM_LBUTTONDOWN) {
				m_bClick = TRUE;
				m_bDrag = FALSE;
			}



			if (wParam == WM_LBUTTONUP) {
				if (m_bClick && m_bDrag == FALSE) {
					InkModeLayoutChange();
				}
				m_bDrag = FALSE;
				m_bClick = FALSE;
			}
		}
	}



	if (wParam == WM_LBUTTONDOWN) {
		if (lParam == BTN_CLOSE) {
			if (g_pSkin->m_version == SKIN_VER_122) {
			}
			else {
				((CAbleClassDlg*)GetParent())->SetBoardNon();
			}
		}
		else if (lParam == BTN_PEN) {
			//DoButtonLayoutChange(BTN_PEN, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenType(0);
		}
		else if (lParam == BTN_HPEN) {
			//DoButtonLayoutChange(BTN_HPEN, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenType(1);
		}
		else if (lParam == BTN_ERASER) {
			//DoButtonLayoutChange(BTN_ERASER, TRUE);
			((CAbleClassDlg*)GetParent())->SetBoardEraser();
		}
		else if (lParam == BTN_WASTEBASKET) {
			((CAbleClassDlg*)GetParent())->SetBoardClear();
		}
		else if (lParam == BTN_RETURN) {
			((CAbleClassDlg*)GetParent())->SetBoardUndo();
		}
		else if (lParam == BTN_COLOR1) {
			//DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenColor(0);
		}
		else if (lParam == BTN_COLOR2) {
			//DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenColor(1);
		}
		else if (lParam == BTN_COLOR3) {
			//DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenColor(2);
		}
		else if (lParam == BTN_COLOR4) {
			//DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenColor(3);
		}
		else if (lParam == BTN_COLOR5) {
			//DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenColor(4);
		}
		else if (lParam == BTN_COLOR6) {
			//DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenColor(5);
		}
		else if (lParam == BTN_PENSIZE1) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenSize(0);
		}
		else if (lParam == BTN_PENSIZE2) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenSize(1);
		}
		else if (lParam == BTN_PENSIZE3) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenSize(2);
		}
		else if (lParam == BTN_PENSIZE4) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenSize(3);
		}
		else if (lParam == BTN_PENSIZE5) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenSize(4);
		}
		else if (lParam == BTN_PENSIZE6) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetPenSize(5);
		}
		else if (lParam == BTN_PENSIZE_UP) {
			((CAbleClassDlg*)GetParent())->SetPenSizeUp();
			DoButtonLayoutChange(lParam, TRUE);
		}
		else if (lParam == BTN_PENSIZE_DOWN) {
			((CAbleClassDlg*)GetParent())->SetPenSizeDown();
			DoButtonLayoutChange(lParam, TRUE);
		}

	}

	return 0L;
}


void CToolDlg::ChangePenSizeText() {
	

	if (m_bInkMode) {
		m_btnPenSizeText1.ShowWindow(SW_HIDE);
		m_btnPenSizeText2.ShowWindow(SW_HIDE);
		m_btnPenSizeText3.ShowWindow(SW_HIDE);
		m_btnPenSizeText4.ShowWindow(SW_HIDE);
		m_btnPenSizeText5.ShowWindow(SW_HIDE);
		m_btnPenSizeText6.ShowWindow(SW_HIDE);
	}
	else {
		int nPenSizeIndex = ((CAbleClassDlg*)GetParent())->GetPenSizeIndex();
		if (nPenSizeIndex == 0) {
			m_btnPenSizeText1.ShowWindow(SW_SHOW);
			m_btnPenSizeText2.ShowWindow(SW_HIDE);
			m_btnPenSizeText3.ShowWindow(SW_HIDE);
			m_btnPenSizeText4.ShowWindow(SW_HIDE);
			m_btnPenSizeText5.ShowWindow(SW_HIDE);
			m_btnPenSizeText6.ShowWindow(SW_HIDE);
		}
		else if (nPenSizeIndex == 1) {
			m_btnPenSizeText1.ShowWindow(SW_HIDE);
			m_btnPenSizeText2.ShowWindow(SW_SHOW);
			m_btnPenSizeText3.ShowWindow(SW_HIDE);
			m_btnPenSizeText4.ShowWindow(SW_HIDE);
			m_btnPenSizeText5.ShowWindow(SW_HIDE);
			m_btnPenSizeText6.ShowWindow(SW_HIDE);
		}
		else if (nPenSizeIndex == 2) {
			m_btnPenSizeText1.ShowWindow(SW_HIDE);
			m_btnPenSizeText2.ShowWindow(SW_HIDE);
			m_btnPenSizeText3.ShowWindow(SW_SHOW);
			m_btnPenSizeText4.ShowWindow(SW_HIDE);
			m_btnPenSizeText5.ShowWindow(SW_HIDE);
			m_btnPenSizeText6.ShowWindow(SW_HIDE);
		}
		else if (nPenSizeIndex == 3) {
			m_btnPenSizeText1.ShowWindow(SW_HIDE);
			m_btnPenSizeText2.ShowWindow(SW_HIDE);
			m_btnPenSizeText3.ShowWindow(SW_HIDE);
			m_btnPenSizeText4.ShowWindow(SW_SHOW);
			m_btnPenSizeText5.ShowWindow(SW_HIDE);
			m_btnPenSizeText6.ShowWindow(SW_HIDE);
		}
		else if (nPenSizeIndex == 4) {
			m_btnPenSizeText1.ShowWindow(SW_HIDE);
			m_btnPenSizeText2.ShowWindow(SW_HIDE);
			m_btnPenSizeText3.ShowWindow(SW_HIDE);
			m_btnPenSizeText4.ShowWindow(SW_HIDE);
			m_btnPenSizeText5.ShowWindow(SW_SHOW);
			m_btnPenSizeText6.ShowWindow(SW_HIDE);
		}
		else if (nPenSizeIndex == 5) {
			m_btnPenSizeText1.ShowWindow(SW_HIDE);
			m_btnPenSizeText2.ShowWindow(SW_HIDE);
			m_btnPenSizeText3.ShowWindow(SW_HIDE);
			m_btnPenSizeText4.ShowWindow(SW_HIDE);
			m_btnPenSizeText5.ShowWindow(SW_HIDE);
			m_btnPenSizeText6.ShowWindow(SW_SHOW);
		}

	}
}



void CToolDlg::GetColorInfoFromDatabase() {
	for (int i = 0; i < 6; i++) {
		m_color[i].clr = g_database.GetSetting()->set_ink_color[i];
		m_color[i].b = (g_database.GetSetting()->set_ink_color_on[i] == 1) ? true : false;
		if (m_color[i].b) {
			m_btnColor[i].SetColor(m_color[i].clr, TRUE);

			if (::IsWindow(m_btnColor[i].GetSafeHwnd())) {
				m_btnColor[i].EnableWindow(TRUE);
			}
			
		}
		else {
			m_btnColor[i].SetColor(RGB(0, 255, 0), FALSE);
			if (::IsWindow(m_btnColor[i].GetSafeHwnd())) {
				m_btnColor[i].EnableWindow(FALSE);
			}
		}
	}
}


void CToolDlg::InkModeLayoutChange() {
	m_bInkMode ^= 1;

	if (m_bInkMode) {
		if (m_pBackInk)
		{
			g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBackInk, 100);
			m_btnPen.ShowWindow(SW_HIDE);
			m_btnHPen.ShowWindow(SW_HIDE);
			m_btnEraser.ShowWindow(SW_HIDE);
			m_btnWastebasket.ShowWindow(SW_HIDE);
			m_btnReturn.ShowWindow(SW_HIDE);
			for (int i = 0; i < 6; i++) { m_btnColor[i].ShowWindow(SW_HIDE); }
			m_btnPenSizeUp.ShowWindow(SW_HIDE);
			m_btnPenSizeDown.ShowWindow(SW_HIDE);
			m_btnPenSizeText1.ShowWindow(SW_HIDE);
			m_btnPenSizeText2.ShowWindow(SW_HIDE);
			m_btnPenSizeText3.ShowWindow(SW_HIDE);
			m_btnPenSizeText4.ShowWindow(SW_HIDE);
			m_btnPenSizeText5.ShowWindow(SW_HIDE);
			m_btnPenSizeText6.ShowWindow(SW_HIDE);

		}
	}
	else {
		if (m_pBack)
		{
			g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack);
			m_btnPen.ShowWindow(SW_SHOW);
			m_btnHPen.ShowWindow(SW_SHOW);
			m_btnEraser.ShowWindow(SW_SHOW);
			m_btnWastebasket.ShowWindow(SW_SHOW);
			m_btnReturn.ShowWindow(SW_SHOW);
			for (int i = 0; i < 6; i++) { m_btnColor[i].ShowWindow(SW_SHOW); }
			m_btnPenSizeUp.ShowWindow(SW_SHOW);
			m_btnPenSizeDown.ShowWindow(SW_SHOW);
			ChangePenSizeText();
		}
	}

	RedrawWindow();
}

