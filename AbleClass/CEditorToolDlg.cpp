// CEditorToolDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CEditorToolDlg.h"
#include "afxdialogex.h"


// CEditorToolDlg 대화 상자

IMPLEMENT_DYNAMIC(CEditorToolDlg, CDialogEx)

CEditorToolDlg::CEditorToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDITOR_TOOL, pParent),
	nButtonSize(78),
	nLeftOffset(18)
{

}

CEditorToolDlg::~CEditorToolDlg()
{
}

void CEditorToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditorToolDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_WINDOWPOSCHANGING()
	ON_MESSAGE(MSG_CREAD_BUTTON, &CEditorToolDlg::OnCreadButton)
END_MESSAGE_MAP()




void CEditorToolDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
}

int CEditorToolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;




	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	m_pBack = Gdiplus::Image::FromFile(L"./images/edit_menu/tool_bg.png");
	if (m_pBack)
	{
		g_pSkin->UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack);
	}

	int nBtnImageSize = 71;
	int nPaddingTop = 15;
	int nPadding = 18;

	nLeftOffset += nButtonSize;
	m_btnPrevious.SetToolTipText(_T("이전 페이지"), TRUE);
	m_btnPrevious.SetImage(BTN_PREVIOUS, m_hWnd, L"./images/edit_menu/before_n.png", L"./images/edit_menu/before_c.png", L"./images/edit_menu/before_c.png");
	if (m_btnPrevious.Create(IDD_BTN_DLG, this)) {
		m_btnPrevious.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnPrevious.ShowWindow(SW_SHOW);
	}

	nLeftOffset += nButtonSize;
	m_btnNext.SetToolTipText(_T("다음 페이지"), TRUE);
	m_btnNext.SetImage(BTN_NEXT, m_hWnd, L"./images/edit_menu/next_n.png", L"./images/edit_menu/next_c.png", L"./images/edit_menu/next_c.png");
	if (m_btnNext.Create(IDD_BTN_DLG, this)) {
		m_btnNext.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnNext.ShowWindow(SW_SHOW);
	}

	nLeftOffset += nButtonSize;
	nLeftOffset += nPadding;
	m_btnVideo.SetToolTipText(_T("영상 삽입"), TRUE);
	m_btnVideo.SetImage(BTN_VIDEO, m_hWnd, L"./images/edit_menu/video_n.png", L"./images/edit_menu/video_c.png", L"./images/edit_menu/video_c.png");
	if (m_btnVideo.Create(IDD_BTN_DLG, this)) {
		m_btnVideo.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnVideo.ShowWindow(SW_SHOW);
	}
	nLeftOffset += nButtonSize;
	m_btnAudio.SetToolTipText(_T("소리 삽입"), TRUE);
	m_btnAudio.SetImage(BTN_AUDIO, m_hWnd, L"./images/edit_menu/sound_n.png", L"./images/edit_menu/sound_c.png", L"./images/edit_menu/sound_c.png");
	if (m_btnAudio.Create(IDD_BTN_DLG, this)) {
		m_btnAudio.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnAudio.ShowWindow(SW_SHOW);
	}
	nLeftOffset += nButtonSize;
	m_btnPicture.SetToolTipText(_T("이미지 삽입"), TRUE);
	m_btnPicture.SetImage(BTN_PICTURE, m_hWnd, L"./images/edit_menu/picture_n.png", L"./images/edit_menu/picture_c.png", L"./images/edit_menu/picture_c.png");
	if (m_btnPicture.Create(IDD_BTN_DLG, this)) {
		m_btnPicture.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnPicture.ShowWindow(SW_SHOW);
	}
	nLeftOffset += nButtonSize;
	nLeftOffset += nPadding;
	m_btnMemo.SetToolTipText(_T("메모 삽입"), TRUE);
	m_btnMemo.SetImage(BTN_MEMO, m_hWnd, L"./images/edit_menu/memo_n.png", L"./images/edit_menu/memo_c.png", L"./images/edit_menu/memo_c.png");
	if (m_btnMemo.Create(IDD_BTN_DLG, this)) {
		m_btnMemo.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnMemo.ShowWindow(SW_SHOW);
	}

	nLeftOffset += nButtonSize;
	m_btnBlank.SetToolTipText(_T("빈칸(가리개) 삽입"), TRUE);
	m_btnBlank.SetImage(BTN_BLANK, m_hWnd, L"./images/edit_menu/hidden_n.png", L"./images/edit_menu/hidden_c.png", L"./images/edit_menu/hidden_c.png");
	if (m_btnBlank.Create(IDD_BTN_DLG, this)) {
		m_btnBlank.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnBlank.ShowWindow(SW_SHOW);
	}
	nLeftOffset += nButtonSize;
	nLeftOffset += nPadding;
	m_btnSave.SetToolTipText(_T("변경한 내용을 저장합니다"), TRUE);
	m_btnSave.SetImage(BTN_SAVE, m_hWnd, L"./images/edit_menu/save_n.png", L"./images/edit_menu/save_c.png", L"./images/edit_menu/save_c.png");
	if (m_btnSave.Create(IDD_BTN_DLG, this)) {
		m_btnSave.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnSave.ShowWindow(SW_SHOW);
	}

	nLeftOffset += nButtonSize;
	nLeftOffset += nPadding;
	m_btnExit.SetToolTipText(_T("편집 모드 종료"), TRUE);
	m_btnExit.SetImage(BTN_EXIT, m_hWnd, L"./images/edit_menu/exit_n.png", L"./images/edit_menu/exit_c.png", L"./images/edit_menu/exit_c.png");
	if (m_btnExit.Create(IDD_BTN_DLG, this)) {
		m_btnExit.MoveWindow(nLeftOffset, nPaddingTop, nBtnImageSize, nBtnImageSize);
		m_btnExit.ShowWindow(SW_SHOW);
	}



	return 0;
}


BOOL CEditorToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void CEditorToolDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnLButtonDown(nFlags, point);
}

void CEditorToolDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnMouseMove(nFlags, point);
}

void CEditorToolDlg::OnLButtonUp(UINT nFlags, CPoint point)
{

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CEditorToolDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CEditorToolDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanging(lpwndpos);

	RECT rt;
	::GetClientRect(GetParent()->m_hWnd, &rt);

	if (lpwndpos->y > rt.bottom - 100) lpwndpos->y = rt.bottom - 100;
}

LRESULT CEditorToolDlg::OnCreadButton(WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_LBUTTONUP) {
		if (lParam == BTN_PREVIOUS) {
			((CAbleClassDlg*)GetParent())->SetEditorPreviousPage();
		}
		else if (lParam == BTN_NEXT) {
			((CAbleClassDlg*)GetParent())->SetEditorNextPage();
		}
		else if (lParam == BTN_VIDEO) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetEditorVideoInsert();
		}
		else if (lParam == BTN_AUDIO) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetEditorAudioInsert();
		}
		else if (lParam == BTN_PICTURE) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetEditorPictureInsert();
		}
		else if (lParam == BTN_MEMO) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetEditorMemoInsert();
		}
		else if (lParam == BTN_BLANK) {
			DoButtonLayoutChange(lParam, TRUE);
			((CAbleClassDlg*)GetParent())->SetEditorBlankInsert();
		}
		else if (lParam == BTN_SAVE) {
			((CAbleClassDlg*)GetParent())->SaveEditorBook();
		}
		else if (lParam == BTN_EXIT) {
			((CAbleClassDlg*)GetParent())->CloseEditorBook();
		}
	}

	return 0L;
}



