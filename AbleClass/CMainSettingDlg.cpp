// CMainSettingDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CMainSettingDlg.h"
#include "CLicenseDlg.h"
#include "afxdialogex.h"


// CMainSettingDlg 대화 상자

IMPLEMENT_DYNAMIC(CMainSettingDlg, CDialogEx)

CMainSettingDlg::CMainSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_SETTING_DLG, pParent),
	m_bFullSizeUpdate(FALSE),
	m_currentColorDlgItemNumber(0),
	strAutoColorText(L"사용안함"),
	strOtherText(L"다른 색상"),
	strDocColorsText(L"저장")
{
	for (int i = 0; i < 6; i++) {
		m_color[i].clr = RGB(255, 0, 0);
		m_color[i].b = false;
	}
	m_backColor = CreateSolidBrush(RGB(250, 250, 250));

}

CMainSettingDlg::~CMainSettingDlg()
{
	::DeleteObject(m_backColor);
}

void CMainSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_VERSION, m_staticVersion);
	DDX_Control(pDX, IDC_CHK_FULLSIZE, m_chkFullSize);
	DDX_Control(pDX, IDC_CHK_PRESSURE, m_chkBoardPressure);
	DDX_Control(pDX, IDC_CHK_REVERSAL, m_chkBoardReversal);
	DDX_Control(pDX, IDC_CHK_PENTYPE1, m_chkPenStyle1);
	DDX_Control(pDX, IDC_CHK_PENTYPE2, m_chkPenStyle2);
	DDX_Control(pDX, IDC_CHK_PENTYPE3, m_chkPenStyle3);
	DDX_Control(pDX, IDC_CHK_PENTYPE1_CLONE, m_chkPenStyle1Clone);
	DDX_Control(pDX, IDC_CHK_PENTYPE2_CLONE, m_chkPenStyle2Clone);
	DDX_Control(pDX, IDC_CHK_PENTYPE3_CLONE, m_chkPenStyle3Clone);
	DDX_Control(pDX, IDC_BTN_COLOR1, m_btnColor[0]);
	DDX_Control(pDX, IDC_BTN_COLOR2, m_btnColor[1]);
	DDX_Control(pDX, IDC_BTN_COLOR3, m_btnColor[2]);
	DDX_Control(pDX, IDC_BTN_COLOR4, m_btnColor[3]);
	DDX_Control(pDX, IDC_BTN_COLOR5, m_btnColor[4]);
	DDX_Control(pDX, IDC_BTN_COLOR6, m_btnColor[5]);
	DDX_Control(pDX, IDC_COLORPICKER_BTN, m_colorPicker);
	DDX_Control(pDX, IDC_BTN_LICENSE, m_btnLicense);
	DDX_Control(pDX, IDC_REC_PATH, m_editRecPath);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	
	
}


BEGIN_MESSAGE_MAP(CMainSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHK_FULLSIZE, &CMainSettingDlg::OnBnClickedChkFullsize)
	ON_BN_CLICKED(IDC_CHK_PRESSURE, &CMainSettingDlg::OnBnClickedChkPressure)
	ON_BN_CLICKED(IDC_CHK_PENTYPE1, &CMainSettingDlg::OnBnClickedChkPenStyle1)
	ON_BN_CLICKED(IDC_CHK_PENTYPE2, &CMainSettingDlg::OnBnClickedChkPenStyle2)
	ON_BN_CLICKED(IDC_CHK_PENTYPE3, &CMainSettingDlg::OnBnClickedChkPenStyle3)
	ON_BN_CLICKED(IDC_CHK_PENTYPE1_CLONE, &CMainSettingDlg::OnBnClickedChkPenStyle1Clone)
	ON_BN_CLICKED(IDC_CHK_PENTYPE2_CLONE, &CMainSettingDlg::OnBnClickedChkPenStyle2Clone)
	ON_BN_CLICKED(IDC_CHK_PENTYPE3_CLONE, &CMainSettingDlg::OnBnClickedChkPenStyle3Clone)
	ON_BN_CLICKED(IDC_BTN_COLOR1, &CMainSettingDlg::OnBnClickedColor1)
	ON_BN_CLICKED(IDC_BTN_COLOR2, &CMainSettingDlg::OnBnClickedColor2)
	ON_BN_CLICKED(IDC_BTN_COLOR3, &CMainSettingDlg::OnBnClickedColor3)
	ON_BN_CLICKED(IDC_BTN_COLOR4, &CMainSettingDlg::OnBnClickedColor4)
	ON_BN_CLICKED(IDC_BTN_COLOR5, &CMainSettingDlg::OnBnClickedColor5)
	ON_BN_CLICKED(IDC_BTN_COLOR6, &CMainSettingDlg::OnBnClickedColor6)
	ON_BN_CLICKED(IDOK, &CMainSettingDlg::OnBnClickedOk)
	ON_MESSAGE(WM_COLOR_UPDATE, &CMainSettingDlg::OnColorUpdate)
	ON_BN_CLICKED(IDC_BTN_LICENSE, &CMainSettingDlg::OnBnClickedBtnLicense)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BTN_SAVE_PATH, &CMainSettingDlg::OnBnClickedBtnSavePath)
END_MESSAGE_MAP()



BOOL CMainSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	InitColorPalette();
	SetDlgItem();
	CheckCert();


	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, 255, LWA_ALPHA);


	m_staticVersion.SetWindowTextW(g_pSkin->GetVersion().c_str());


	HINSTANCE	hInstResource = NULL;

	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"main_menu/setting_bg.bmp").c_str());
	if (m_pBack)
	{
		Gdiplus::Bitmap* pBitmap = static_cast<Gdiplus::Bitmap*>(m_pBack->Clone());
		Gdiplus::Status status = pBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &m_hBitmap);
	}

	m_btnOk.LoadStdImage(g_pSkin->GetSource(L"main_menu/setting_btn_ok_n.png").c_str());
	m_btnOk.LoadAltImage(g_pSkin->GetSource(L"main_menu/setting_btn_ok_o.png").c_str());
	m_btnOk.EnableToggle(TRUE);
	m_btnOk.SetToolTipText(_T("적용"));

	m_btnCancel.LoadStdImage(g_pSkin->GetSource(L"main_menu/setting_btn_cancel_n.png").c_str());
	m_btnCancel.LoadAltImage(g_pSkin->GetSource(L"main_menu/setting_btn_cancel_n.png").c_str());
	m_btnCancel.EnableToggle(TRUE);
	m_btnCancel.SetToolTipText(_T("닫기"));


	((CComboBox*)GetDlgItem(IDC_COMBO_LANGUAGE))->SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CMainSettingDlg::SetDlgItem()
{
	if (g_database.GetSetting()->set_ui_fullsize == 1) {
		m_chkFullSize.SetCheck(BST_CHECKED);
	}
	else {
		m_chkFullSize.SetCheck(BST_UNCHECKED);
	}


	if (g_database.GetSetting()->set_board_pressure == 1) {
		m_chkBoardPressure.SetCheck(BST_CHECKED);
	}
	else {
		m_chkBoardPressure.SetCheck(BST_UNCHECKED);
	}


	if (g_database.GetSetting()->set_board_reversal == 1) {
		m_chkBoardReversal.SetCheck(BST_CHECKED);
	}
	else {
		m_chkBoardReversal.SetCheck(BST_UNCHECKED);
	}
	


	for (int i = 0; i < 6; i++) {
		m_color[i].clr = g_database.GetSetting()->set_ink_color[i];
		m_color[i].b = (g_database.GetSetting()->set_ink_color_on[i] == 1) ? true : false;
		m_btnColor[i].SetColor(m_color[i].clr, static_cast<BOOL>(m_color[i].b));

		m_btnColor[i].SetColorButton(TRUE);
		m_btnColor[i].EnableToggle(FALSE);
	}


	m_chkPenStyle1.LoadStdImage(g_pSkin->GetSource(L"main_menu/shape_1_o.png").c_str());
	m_chkPenStyle1.LoadAltImage(g_pSkin->GetSource(L"main_menu/shape_1_o.png").c_str());
	m_chkPenStyle1.EnableToggle(FALSE);

	m_chkPenStyle2.LoadStdImage(g_pSkin->GetSource(L"main_menu/shape_2_o.png").c_str());
	m_chkPenStyle2.LoadAltImage(g_pSkin->GetSource(L"main_menu/shape_2_o.png").c_str());
	m_chkPenStyle2.EnableToggle(FALSE);

	m_chkPenStyle3.LoadStdImage(g_pSkin->GetSource(L"main_menu/shape_3_o.png").c_str());
	m_chkPenStyle3.LoadAltImage(g_pSkin->GetSource(L"main_menu/shape_3_o.png").c_str());
	m_chkPenStyle3.EnableToggle(FALSE);


	PENTIP_STYLE penStyle = static_cast<PENTIP_STYLE>(g_database.GetSetting()->set_board_penstyle);
	//TRACE(L"\n>>> pen style (%d) <<<\n", penStyle);


	// rec path
	m_editRecPath.SetWindowTextW(g_database.GetSetting()->root_tcolumn2.c_str());

	SetPenStyleDlgItem(penStyle);
	UpdatePenStyleDlgItem();
}



void CMainSettingDlg::OnBnClickedChkFullsize()
{
	//if (m_chkFullSize.GetCheck() == BST_CHECKED) {
	//	static_cast<CAbleClassDlg*>(GetParent())->SetFullSize(TRUE);
	//}
	//else {
	//	static_cast<CAbleClassDlg*>(GetParent())->SetFullSize(FALSE);
	//}
}


void CMainSettingDlg::OnBnClickedChkPressure()
{
	UpdatePenStyleDlgItem();
	
}

void CMainSettingDlg::OnBnClickedChkPenStyle1()
{
	SetPenStyleDlgItem(PS_NORMAL);
}

void CMainSettingDlg::OnBnClickedChkPenStyle2()
{
	SetPenStyleDlgItem(PS_H_EXPAND);
}

void CMainSettingDlg::OnBnClickedChkPenStyle3()
{
	SetPenStyleDlgItem(PS_V_EXPAND);
}

void CMainSettingDlg::OnBnClickedChkPenStyle1Clone()
{
	SetPenStyleDlgItem(PS_NORMAL);
}

void CMainSettingDlg::OnBnClickedChkPenStyle2Clone()
{
	SetPenStyleDlgItem(PS_H_EXPAND);
}

void CMainSettingDlg::OnBnClickedChkPenStyle3Clone()
{
	SetPenStyleDlgItem(PS_V_EXPAND);
}

void CMainSettingDlg::OnBnClickedColor1()
{
	m_currentColorDlgItemNumber = 0;
	LoadColorPalette();
}


void CMainSettingDlg::OnBnClickedColor2()
{
	m_currentColorDlgItemNumber = 1;
	LoadColorPalette();
}

void CMainSettingDlg::OnBnClickedColor3()
{
	m_currentColorDlgItemNumber = 2;
	LoadColorPalette();
}

void CMainSettingDlg::OnBnClickedColor4()
{
	m_currentColorDlgItemNumber = 3;
	LoadColorPalette();
}

void CMainSettingDlg::OnBnClickedColor5()
{
	m_currentColorDlgItemNumber = 4;
	LoadColorPalette();
}
void CMainSettingDlg::OnBnClickedColor6()
{
	m_currentColorDlgItemNumber = 5;
	LoadColorPalette();
}



void CMainSettingDlg::UpdatePenStyleDlgItem() {
	if (m_chkBoardPressure.GetCheck() == BST_CHECKED) {
		m_chkPenStyle1Clone.EnableWindow(FALSE);
		m_chkPenStyle2Clone.EnableWindow(FALSE);
		m_chkPenStyle3Clone.EnableWindow(FALSE);
	}
	else {
		m_chkPenStyle1Clone.EnableWindow(TRUE);
		m_chkPenStyle2Clone.EnableWindow(TRUE);
		m_chkPenStyle3Clone.EnableWindow(TRUE);
	}
}

void CMainSettingDlg::SetPenStyleDlgItem(PENTIP_STYLE ps) {
	if (ps == PS_NORMAL) {
		m_chkPenStyle1.SetCheck(BST_CHECKED);
		m_chkPenStyle2.SetCheck(BST_UNCHECKED);
		m_chkPenStyle3.SetCheck(BST_UNCHECKED);
		m_chkPenStyle1Clone.SetCheck(BST_CHECKED);
		m_chkPenStyle2Clone.SetCheck(BST_UNCHECKED);
		m_chkPenStyle3Clone.SetCheck(BST_UNCHECKED);
		
	}
	else if (ps == PS_H_EXPAND) {
		m_chkPenStyle1.SetCheck(BST_UNCHECKED);
		m_chkPenStyle2.SetCheck(BST_CHECKED);
		m_chkPenStyle3.SetCheck(BST_UNCHECKED);
		m_chkPenStyle1Clone.SetCheck(BST_UNCHECKED);
		m_chkPenStyle2Clone.SetCheck(BST_CHECKED);
		m_chkPenStyle3Clone.SetCheck(BST_UNCHECKED);
	}
	else if (ps == PS_V_EXPAND) {
		m_chkPenStyle1.SetCheck(BST_UNCHECKED);
		m_chkPenStyle2.SetCheck(BST_UNCHECKED);
		m_chkPenStyle3.SetCheck(BST_CHECKED);
		m_chkPenStyle1Clone.SetCheck(BST_UNCHECKED);
		m_chkPenStyle2Clone.SetCheck(BST_UNCHECKED);
		m_chkPenStyle3Clone.SetCheck(BST_CHECKED);
	}
}



void CMainSettingDlg::OnBnClickedOk()
{
	Save();
	CDialogEx::OnOK();
}


void CMainSettingDlg::Save() {
	int nFullSize = 0;
	int nBoardPressure = 0;
	int nPenStyle = 0;
	int nBoardReversal = 0;
	CString strRecPath = L"";


	if (m_chkFullSize.GetCheck() == BST_CHECKED) nFullSize = 1;
	else nFullSize = 0;

	if (m_chkBoardPressure.GetCheck() == BST_CHECKED) nBoardPressure = 1;
	else nBoardPressure = 0;

	if (m_chkBoardReversal.GetCheck() == BST_CHECKED) nBoardReversal = 1;
	else nBoardReversal = 0;


	if (m_chkPenStyle1Clone.GetCheck() == BST_CHECKED) nPenStyle = 0;
	else if (m_chkPenStyle2Clone.GetCheck() == BST_CHECKED) nPenStyle = 1;
	else if (m_chkPenStyle3Clone.GetCheck() == BST_CHECKED) nPenStyle = 2;


	if (g_database.GetSetting()->set_ui_fullsize != nFullSize) {
		m_bFullSizeUpdate = TRUE;
	}


	for (int i = 0; i < 6; i++) {
		g_database.GetSetting()->set_ink_color[i] = m_color[i].clr;
		g_database.GetSetting()->set_ink_color_on[i] = (m_color[i].b == true)?1:0;
	}

	m_editRecPath.GetWindowTextW(strRecPath);

	g_database.GetSetting()->root_tcolumn2 = strRecPath;
	g_database.GetSetting()->set_ui_fullsize = nFullSize;
	g_database.GetSetting()->set_board_pressure = nBoardPressure;
	g_database.GetSetting()->set_board_reversal = nBoardReversal;
	g_database.GetSetting()->set_board_penstyle = nPenStyle;
	g_database.sqlite_update_setting();

}



LRESULT CMainSettingDlg::OnColorUpdate(WPARAM wParam, LPARAM lParam) {
	COLORREF color = m_colorPicker.GetUpdateColor();
	if (m_currentColorDlgItemNumber < 6) {
		
		if (color == -1) {
			m_color[m_currentColorDlgItemNumber].clr = RGB(220, 220, 220);
			m_color[m_currentColorDlgItemNumber].b = false;
			m_btnColor[m_currentColorDlgItemNumber].SetColor(RGB(220, 220, 220), FALSE);
		}
		else {
			m_color[m_currentColorDlgItemNumber].clr = color;
			m_color[m_currentColorDlgItemNumber].b = true;
			m_btnColor[m_currentColorDlgItemNumber].SetColor(color, TRUE);
		}

	}
	return 0L;
}


void CMainSettingDlg::InitColorPalette() {
	m_colorPicker.SetParent(this);

	m_colorPalette.Add(COLORREF(RGB(0, 0, 0)));
	m_colorPalette.Add(COLORREF(RGB(237, 28, 36)));
	m_colorPalette.Add(COLORREF(RGB(255, 127, 39)));
	m_colorPalette.Add(COLORREF(RGB(255, 242, 0)));
	m_colorPalette.Add(COLORREF(RGB(34, 177, 76)));

	m_colorPalette.Add(COLORREF(RGB(255, 255, 255)));
	m_colorPalette.Add(COLORREF(RGB(0, 162, 232)));
	m_colorPalette.Add(COLORREF(RGB(63, 72, 204)));
	m_colorPalette.Add(COLORREF(RGB(163, 73, 164)));
	m_colorPalette.Add(COLORREF(RGB(185, 122, 87)));

	m_colorPalette.Add(COLORREF(RGB(255, 174, 201)));
	m_colorPalette.Add(COLORREF(RGB(255, 201, 14)));
	m_colorPalette.Add(COLORREF(RGB(255, 0, 0)));
	m_colorPalette.Add(COLORREF(RGB(0, 0, 255)));
	m_colorPalette.Add(COLORREF(RGB(0, 255, 0)));

	m_colorPalette.Add(COLORREF(RGB(255, 255, 0)));
	m_colorPalette.Add(COLORREF(RGB(0, 255, 255)));
	m_colorPalette.Add(COLORREF(RGB(255, 0, 255)));
	m_colorPalette.Add(COLORREF(RGB(0, 0, 160)));
	m_colorPalette.Add(COLORREF(RGB(160, 0, 0)));


}

void CMainSettingDlg::LoadColorPalette() {
	COLORREF color = RGB(237, 28, 36);
	COLORREF colorAutomatic = 0;
	int nColumns = 5;

	POINT pt;
	GetCursorPos(&pt);


	CMFCColorPopupMenu* pPopup = new CMFCColorPopupMenu(&m_colorPicker, m_colorPalette, color, strAutoColorText.c_str(), strOtherText.c_str(), strDocColorsText.c_str(), m_lstDocColors, nColumns, colorAutomatic);
	if (pPopup->Create(this, pt.x, pt.y, NULL)) {
		pPopup->ShowWindow(SW_SHOW);
	}
}

void CMainSettingDlg::OnBnClickedBtnLicense()
{
	if (g_cert.CheckCert() == FALSE) {
		CLicenseDlg dlg;
		if (dlg.DoModal() == IDOK) {
		}
	}
}


BOOL CMainSettingDlg::CheckCert() {
	if (g_cert.CheckCert() == TRUE) {
		m_btnLicense.SetWindowTextW(L"제품키 적용 됨");
	}

	return TRUE;
}



BOOL CMainSettingDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);

	//CMyMemDC pDevC(pDC, CRect(0, 0, rect.right, rect.bottom));
	CMyMemDC pDevC(pDC, CRect(0, 0, 10, 10));

	if (m_hBitmap)
	{
		BITMAP bit;
		GetObject(m_hBitmap, sizeof(BITMAP), &bit);
		HBITMAP oldBitmap = (HBITMAP)pDevC->SelectObject(m_hBitmap);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), pDevC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);
		return TRUE;
	}
	else {
		return CDialogEx::OnEraseBkgnd(pDC);
	}

}

HBRUSH CMainSettingDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);


	if (pWnd->GetSafeHwnd() == GetDlgItem(IDC_CHK_FULLSIZE)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_CHK_REVERSAL)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_CHK_PRESSURE)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_1)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_2)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_3)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_4)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_5)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_6)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_7)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_8)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_9)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_10)->m_hWnd
		) {
		pDC->SetBkMode(TRANSPARENT);
		return m_backColor;
	}

	if (pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_VERSION)->m_hWnd) {
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}

	return hbr;
}


void CMainSettingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnMouseMove(nFlags, point);
}


void CMainSettingDlg::OnBnClickedBtnSavePath()
{
	ITEMIDLIST *pidlBrowse;
	TCHAR pPathName[MAX_PATH];
	BROWSEINFO BrInfo;
	memset(&BrInfo, 0, sizeof(BrInfo));

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;
	BrInfo.pszDisplayName = pPathName;
	BrInfo.lpszTitle = _T("녹화 파일 경로 선택");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	pidlBrowse = ::SHBrowseForFolder(&BrInfo);

	if (pidlBrowse != NULL)
	{
		SHGetPathFromIDList(pidlBrowse, pPathName);
		m_editRecPath.SetWindowTextW(pPathName);
	}
}
