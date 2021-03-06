
// AbleClassDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "afxdialogex.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CBookDlg.h"
#include "CMainSettingDlg.h"

#include "CLicenseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAbleClassDlg 대화 상자
#define TIMER_REC_STOP (WM_USER + 2001)


CAbleClassDlg::CAbleClassDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABLECLASS_DIALOG, pParent),
	m_bTrackMouse(FALSE),
	m_viewMode(VIEW_MAIN),
	m_boardMenuVisible(TRUE),
	m_toolMenuVisible(TRUE),
	m_pBookDB(NULL),
	m_bIsRecording(FALSE),
	m_hwndRec(NULL),
	m_AbleRecInfo(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

CAbleClassDlg::~CAbleClassDlg()
{
	if (m_AbleRecInfo) {
		for (int i = 0; i < m_AbleRecInfo->Windows.size(); i++) {
			if (::IsWindow(m_AbleRecInfo->Windows[i])) {
				::SendMessage(m_AbleRecInfo->Windows[i], WM_CLOSE, 0, 0);
			}
		}

		delete m_AbleRecInfo;
		m_AbleRecInfo = nullptr;
	}
	CloseBookDB();
}



void CAbleClassDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAbleClassDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_MESSAGE(LAYOUT_MESSAGE, &CAbleClassDlg::OnLayoutMessage)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_EXITSIZEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


int CAbleClassDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (g_database.GetSetting()->set_ui_fullsize == 1) {
		ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
		ModifyStyle(WS_MAXIMIZEBOX, 0);
		ModifyStyle(WS_CAPTION, 0);
	}

	HRESULT hr = S_OK;
	// D2D automatically handles high DPI settings
	//SetProcessDPIAware();

//#ifdef _DEBUG
//	g_ctDriver = new CComTouchDriver(m_hWnd);
//#else
//	g_ctDriver = new (std::nothrow) CComTouchDriver(m_hWnd);
//#endif
//	if (g_ctDriver == NULL)
//	{
//		hr = S_FALSE;
//	}
//
//	if (SUCCEEDED(hr))
//	{
//		if (g_ctDriver->Initialize()) hr = S_OK;
//
//		// Disable UI feedback for penflicks 
//		SetTabletInputServiceProperties();
//
//		// Ready for handling WM_TOUCH messages
		//::RegisterTouchWindow(m_hWnd, 0);
//	}
//
//	if (FAILED(hr)) {
//		if (g_ctDriver)
//		{
//			delete g_ctDriver;
//			g_ctDriver = NULL;
//		}
//	}

	if (m_mainDlg.Create(IDD_MAIN_DLG, this)) {

	}
	if (m_mainMenuDlg.Create(IDD_MAIN_MENU, this)) {

	}
	if (m_mainUpDlg.Create(IDD_MAIN_UP, this)) {

	}

	if (m_board.Create(IDD_WHITEBOARD, this)) {

	}

	if (m_boardMenuDlg.Create(IDD_BOARD_MENU, this)) {
	}

	if (m_toolDlg.Create(IDD_TOOL_MENU, this)) {
	}


	if (m_bookMarkDlg.Create(IDD_BOOKMARK, this)) {
	}
	
	if (m_inkDlg.Create(IDD_INK, this)) {
	}

	if (m_bookDeleteDlg.Create(IDD_BOOK_DELETE, this)) {
	}

	if (m_editorDlg.Create(IDD_EDITOR_DLG, this)) {
	}

	if (m_editorToolDlg.Create(IDD_EDITOR_TOOL, this)) {
	}


	
	m_btnLogo.SetImage(IDD_BTN_DLG, m_hWnd, g_pSkin->GetSource(L"main_menu/podo_logo.png").c_str(), g_pSkin->GetSource(L"main_menu/podo_logo.png").c_str(), g_pSkin->GetSource(L"main_menu/podo_logo.png").c_str(), g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.bottom);
	if (m_btnLogo.Create(IDD_BTN_DLG, this)) {
		m_btnLogo.ShowWindow(SW_SHOW);
		m_btnLogo.UpdateWindow();
	}

	return 0;
}

// CAbleClassDlg 메시지 처리기

BOOL CAbleClassDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	ShowWindow(SW_MAXIMIZE);
	GetClientRect(&m_rtContext);
	LONG nCenter = static_cast<LONG>((m_rtContext.right - m_rtContext.left) * 0.5);


	if (g_pSkin->m_logoAlign == BS_CENTER) {
		m_btnLogo.MoveWindow(nCenter - g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.top, g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.bottom);
	}
	else {
		m_btnLogo.MoveWindow(0, g_pSkin->m_mainUpLogo.top, g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.bottom);
	}

	m_mainDlg.SetWindowPos(&wndBottom, 0, 0, m_rtContext.right, m_rtContext.bottom, SWP_HIDEWINDOW);
	m_board.SetWindowPos(&wndBottom, 0, 0, m_rtContext.right, m_rtContext.bottom, SWP_HIDEWINDOW);

	m_mainMenuDlg.MoveWindow(0, 0, g_pSkin->m_mainMenu.right, m_rtContext.bottom);
	m_mainMenuDlg.ShowWindow(SW_HIDE);
	//m_mainMenuDlg.UpdateWindow();

	m_mainUpDlg.MoveWindow(g_pSkin->m_mainMenu.right, 0, m_rtContext.right, g_pSkin->m_mainUpHeight);
	m_mainUpDlg.ShowWindow(SW_HIDE);
	//m_mainUpDlg.UpdateWindow();

	m_bookDeleteDlg.MoveWindow(m_rtContext.right/2 - 300, m_rtContext.bottom - 220, 687, 200);
	m_bookDeleteDlg.ShowWindow(SW_HIDE);

	m_boardMenuDlg.MoveWindow(m_rtContext.right - g_pSkin->m_boardMenu.right, 0, g_pSkin->m_boardMenu.right, m_rtContext.bottom);
	m_boardMenuDlg.ShowWindow(SW_HIDE);
	//m_boardMenuDlg.UpdateWindow();


	m_bookMarkDlg.SetWindowPos(&wndTop, m_rtContext.right - g_pSkin->m_bookMark.right, 0, g_pSkin->m_bookMark.right, g_pSkin->m_bookMark.bottom, SWP_HIDEWINDOW);
	m_bookMarkDlg.ShowWindow(SW_HIDE);
	//m_bookMarkDlg.UpdateWindow();
	//BOOL bCloak = TRUE;
	//::DwmSetWindowAttribute(m_bookMarkDlg.GetSafeHwnd(), DWMWA_CLOAK, &bCloak, sizeof(bCloak));

	
	m_toolDlg.MoveWindow(static_cast<int>(nCenter - g_pSkin->m_toolRect.right*0.5), m_rtContext.bottom - g_pSkin->m_toolRect.bottom, g_pSkin->m_toolRect.right, g_pSkin->m_toolRect.bottom);
	m_toolDlg.ShowWindow(SW_HIDE);
	m_toolDlg.UpdateWindow();
	

	m_inkDlg.MoveWindow(m_rtContext.right - g_pSkin->m_boardMenu.right - g_pSkin->m_inkRect.right, m_rtContext.bottom - g_pSkin->m_inkRect.bottom, g_pSkin->m_inkRect.right, g_pSkin->m_inkRect.bottom);
	m_inkDlg.ShowWindow(SW_HIDE);
	m_inkDlg.UpdateWindow();


	m_editorDlg.SetWindowPos(&wndBottom, 0, 0, m_rtContext.right, m_rtContext.bottom, SWP_HIDEWINDOW);

	m_editorToolDlg.MoveWindow(static_cast<int>(nCenter - 922 * 0.5), m_rtContext.bottom - 106, 922, 106);
	m_editorToolDlg.ShowWindow(SW_HIDE);
	m_editorToolDlg.UpdateWindow();

	SetViewChange();


	::SetFocus(m_board.m_hWnd);



	//ShowWindow(SW_SHOWMAXIMIZED);

	return FALSE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CAbleClassDlg::SetViewMode(UINT viewMode) {
	m_viewMode = viewMode;
}


void CAbleClassDlg::SetViewChange() {
	if (m_viewMode == VIEW_MAIN) {
		m_mainDlg.ShowWindow(SW_SHOWNORMAL);
		m_mainDlg.UpdateWindow();
		m_mainMenuDlg.ShowWindow(SW_SHOWNORMAL);
		m_mainMenuDlg.UpdateWindow();
		m_mainUpDlg.ShowWindow(SW_SHOWNORMAL);
		m_mainUpDlg.UpdateWindow();
		m_btnLogo.ShowWindow(SW_SHOW);
		m_btnLogo.UpdateWindow();

		m_board.ShowWindow(SW_HIDE);
		m_boardMenuDlg.ShowWindow(SW_HIDE);
		m_bookMarkDlg.ShowWindow(SW_HIDE);
		m_toolDlg.ShowWindow(SW_HIDE);
		m_inkDlg.ShowWindow(SW_HIDE);

		m_editorDlg.ShowWindow(SW_HIDE);
		m_editorToolDlg.ShowWindow(SW_HIDE);
}
	else if (m_viewMode == VIEW_WHITEBOARD) {
		m_mainDlg.ShowWindow(SW_HIDE);
		m_mainMenuDlg.ShowWindow(SW_HIDE);
		m_mainUpDlg.ShowWindow(SW_HIDE);
		m_btnLogo.ShowWindow(SW_HIDE);

		m_board.ShowWindow(SW_SHOWNORMAL);
		m_boardMenuDlg.ShowWindow(SW_SHOWNORMAL);
		m_boardMenuDlg.UpdateWindow();
		m_bookMarkDlg.ShowWindow(SW_SHOWNORMAL);
		m_bookMarkDlg.UpdateWindow();

		m_editorDlg.ShowWindow(SW_HIDE);
		m_editorToolDlg.ShowWindow(SW_HIDE);
	}
	else if (m_viewMode == VIEW_EDIT) {
		m_mainDlg.ShowWindow(SW_HIDE);
		m_mainMenuDlg.ShowWindow(SW_HIDE);
		m_mainUpDlg.ShowWindow(SW_HIDE);
		m_btnLogo.ShowWindow(SW_HIDE);

		m_board.ShowWindow(SW_HIDE);
		m_boardMenuDlg.ShowWindow(SW_HIDE);
		m_bookMarkDlg.ShowWindow(SW_HIDE);
		m_toolDlg.ShowWindow(SW_HIDE);
		m_inkDlg.ShowWindow(SW_HIDE);

		m_editorDlg.ShowWindow(SW_SHOW);
		m_editorToolDlg.ShowWindow(SW_SHOW);
	}
}

void CAbleClassDlg::SetWhiteBoardViewChange() {
	if (m_board.GetPageMode() == PAGE_ITEM) {
		if (m_board.IsOperationTypePen()) {
			m_toolDlg.ShowWindow(SW_SHOWNORMAL);
			m_toolDlg.UpdateWindow();

			if (g_pSkin->m_version == SKIN_VER_122) {
				m_inkDlg.ShowWindow(SW_HIDE);
			}
			else {
				m_inkDlg.ShowWindow(SW_SHOWNORMAL);
				m_inkDlg.UpdateWindow();
			}

			SetBoardMenuButtonStyle(IDC_BTN_PEN, TRUE);
			SetPenColor(m_board.GetPen()->GetColorIndex());
			SetPenSize(m_board.GetPen()->GetSizeIndex());
			SetPenType(m_board.GetPenType());
			//SetPenColor(m_board.GetCurrentPenColorIndex());
			//SetPenSize(m_board.GetCurrentPenSizeIndex());
			//SetPenAlpha(m_board.GetCurrentPenAlphaIndex());
		}
		else {
			SetBoardMenuButtonStyle(IDC_BTN_PEN, FALSE);
			m_toolDlg.ShowWindow(SW_HIDE);
			m_inkDlg.ShowWindow(SW_HIDE);
		}
	}
	else if (m_board.GetPageMode() == PAGE_EMPTYBOARD) {
		if (m_board.IsOperationTypePen()) {

			m_toolDlg.ShowWindow(SW_SHOWNORMAL);
			m_toolDlg.UpdateWindow();

			if (g_pSkin->m_version == SKIN_VER_122) {
				m_inkDlg.ShowWindow(SW_HIDE);
			}
			else {
				m_inkDlg.ShowWindow(SW_SHOWNORMAL);
				m_inkDlg.UpdateWindow();
			}

			SetBoardMenuButtonStyle(IDC_BTN_PEN, TRUE);
			SetPenColor(m_board.GetPen()->GetColorIndex());
			SetPenSize(m_board.GetPen()->GetSizeIndex());
			SetPenType(m_board.GetPenType());
		}
		else {
			SetBoardMenuButtonStyle(IDC_BTN_PEN, FALSE);
			m_toolDlg.ShowWindow(SW_HIDE);
			m_inkDlg.ShowWindow(SW_HIDE);
		}
		//SetPenColor(m_board.GetCurrentPenColorIndex());
		//SetPenSize(m_board.GetCurrentPenSizeIndex());
		//SetPenAlpha(m_board.GetCurrentPenAlphaIndex());
	}

}



void CAbleClassDlg::SetBoardMenuButtonStyle(UINT id, BOOL bSelect) {
	m_boardMenuDlg.SetButtonStyle(id, bSelect);
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CAbleClassDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		//CRect rect;
		//GetClientRect(&rect);
		//if (g_ctDriver != NULL) {
		//	g_ctDriver->RenderInitialState(rect.Width(), rect.Height());
		//}

		

	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CAbleClassDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





void CAbleClassDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);



	//if (m_viewMode == VIEW_WHITEBOARD) {
		ResizeLayout(cx, cy);
	//}

}


void CAbleClassDlg::ResizeLayout(int cx, int cy) {

	m_rtContext.right = cx;
	m_rtContext.bottom = cy;

	if (::IsWindow(m_mainDlg.m_hWnd) && ::IsWindow(m_board.m_hWnd)) {


		m_mainDlg.SetWindowPos(&wndBottom, 0, 0, m_rtContext.right, m_rtContext.bottom, SWP_NOZORDER);
		m_mainMenuDlg.MoveWindow(0, 0, g_pSkin->m_mainMenu.right, m_rtContext.bottom);
		m_mainUpDlg.MoveWindow(g_pSkin->m_mainMenu.right, 0, m_rtContext.right, g_pSkin->m_mainUpHeight);
		m_bookDeleteDlg.MoveWindow(m_rtContext.right / 2 - 300, m_rtContext.bottom - 220, 687, 200);

		m_board.SetWindowPos(&wndBottom, 0, 0, m_rtContext.right, m_rtContext.bottom, SWP_NOZORDER);
		int nCenterX = static_cast<int>(cx * 0.5);
		m_boardMenuDlg.MoveWindow(m_rtContext.right - g_pSkin->m_boardMenu.right, 0, g_pSkin->m_boardMenu.right, m_rtContext.bottom);
		m_bookMarkDlg.MoveWindow(m_rtContext.right - g_pSkin->m_bookMark.right, 0, g_pSkin->m_bookMark.right, g_pSkin->m_bookMark.bottom);
		m_toolDlg.MoveWindow(static_cast<int>(nCenterX - g_pSkin->m_toolRect.right * 0.5), m_rtContext.bottom - g_pSkin->m_toolRect.bottom, g_pSkin->m_toolRect.right, g_pSkin->m_toolRect.bottom);
		m_inkDlg.MoveWindow(m_rtContext.right - g_pSkin->m_boardMenu.right - g_pSkin->m_inkRect.right, m_rtContext.bottom - g_pSkin->m_inkRect.bottom, g_pSkin->m_inkRect.right, g_pSkin->m_inkRect.bottom);


		m_editorDlg.SetWindowPos(&wndBottom, 0, 0, m_rtContext.right, m_rtContext.bottom, SWP_NOZORDER);
		m_editorToolDlg.MoveWindow(static_cast<int>(nCenterX - 922 * 0.5), m_rtContext.bottom - 106, 922, 106);

		if (m_viewMode == VIEW_MAIN) {
			m_mainMenuDlg.UpdateWindow();
			m_mainUpDlg.UpdateWindow();
			m_bookDeleteDlg.UpdateWindow();

			if (g_pSkin->m_logoAlign == BS_CENTER) {
				m_btnLogo.SetWindowPos(&wndTop, cx * 0.5 - g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.top, g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.bottom, SWP_SHOWWINDOW);
			}
			else {
				m_btnLogo.SetWindowPos(&wndTop, 0, g_pSkin->m_mainUpLogo.top, g_pSkin->m_mainUpLogo.right, g_pSkin->m_mainUpLogo.bottom, SWP_SHOWWINDOW);
			}
			
			m_btnLogo.UpdateWindow();
		}
		else if (m_viewMode == VIEW_WHITEBOARD) {
			m_board.UpdateWindow();
			m_boardMenuDlg.UpdateWindow();
			m_pVisualMainMenu->SetOffsetX(static_cast<float>(cx - g_pSkin->m_boardMenu.right));
			m_pVisualMainMenu->SetOffsetY(0.0f);
			m_bookMarkDlg.UpdateWindow();
			m_pVisualBookMark->SetOffsetX(static_cast<float>(cx - g_pSkin->m_bookMark.right));
			m_pVisualBookMark->SetOffsetY(0.0f);
			m_toolDlg.UpdateWindow();
			m_pVisualToolMenu->SetOffsetX(static_cast<float>(nCenterX - g_pSkin->m_toolRect.right * 0.5f));
			m_pVisualToolMenu->SetOffsetY(static_cast<float>(m_rtContext.bottom - g_pSkin->m_toolRect.bottom));
			m_inkDlg.UpdateWindow();
			m_pDeviceLayer->Commit();
		}
		else if (m_viewMode == VIEW_EDIT) {
			m_editorDlg.UpdateWindow();
			m_editorToolDlg.UpdateWindow();
		}




	}


	if (::IsWindow(m_boardMenuDlg.m_hWnd)/* && m_viewMode == VIEW_WHITEBOARD*/) {
	}

}


LRESULT CAbleClassDlg::OnLayoutMessage(WPARAM wParam, LPARAM lParam) {
	LRESULT hr = 0L;



	if (wParam == IDD_BOARD_MENU) {
		if (lParam == SW_HIDE) {
			DoTrans(IDD_BOARD_MENU, SW_HIDE);
		}
		else if (lParam == SW_SHOW) {
			DoTrans(IDD_BOARD_MENU, SW_SHOW);
		}
		
	}
	else if (wParam == IDD_TOOL_MENU) {

		if (lParam == SW_HIDE) {
			DoTransTool();
		}

	}

	return hr;
}


void CAbleClassDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// compositionvisual click test
	//m_pVisualBookMark->



	//TOUCHINPUT pInput;
	//pInput.dwID = 1;
	//pInput.x = point.x;
	//pInput.y = point.y;
	//g_ctDriver->StrokeBegin(&pInput);
	//DoTrans();

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CAbleClassDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	//TOUCHINPUT pInput;
	//pInput.dwID = 1;
	//pInput.x = point.x;
	//pInput.y = point.y;
	//g_ctDriver->StrokeEnd(&pInput);

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CAbleClassDlg::OnMouseMove(UINT nFlags, CPoint point)
{

	//if (nFlags & MK_LBUTTON) {
	//	TOUCHINPUT pInput;
	//	pInput.dwID = 1;
	//	pInput.x = point.x;
	//	pInput.y = point.y;
	//	//g_ctDriver->StrokeDrawing(&pInput);
	//}


	CDialogEx::OnMouseMove(nFlags, point);
}



VOID CAbleClassDlg::SetTabletInputServiceProperties()
{
	DWORD_PTR dwHwndTabletProperty =
		TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
		TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
		TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
		TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)

	ATOM atom = ::GlobalAddAtom(MICROSOFT_TABLETPENSERVICE_PROPERTY);
	SetProp(m_hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
	GlobalDeleteAtom(atom);

}


//BOOL CAbleClassDlg::OnTouchInput(CPoint pt, int nInputNumber, int nInputsCount, PTOUCHINPUT pInput) {
//	TRACE(_T("\n>>>>>>>> main touch <<<<<<<<<<\n"));
	//POINT ptInputs;
	//for (int i = 0; i < nInputsCount; i++)
	//{
	//	ptInputs.x = pInput[i].x / 100;
	//	ptInputs.y = pInput[i].y / 100;
	//	::ScreenToClient(m_hWnd, &ptInputs);
	//	pInput[i].x = ptInputs.x;
	//	pInput[i].y = ptInputs.y;
	//	//g_ctDriver->ProcessInputEvent(&pInput[i]);
	//}
	//g_ctDriver->ProcessChanges();
	//m_boardMenuDlg.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	//m_boardMenuDlg.Invalidate(TRUE);

	//if ((pInput->dwFlags & TOUCHEVENTF_UP) && (pInput->dwID != MOUSE_CURSOR_ID))
	//{
	//	//m_toolDlg.Invalidate();
	//}

//	return FALSE;
//}


void CAbleClassDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	//if (g_ctDriver)
	//{
	//	delete g_ctDriver;
	//}
}




/****************************************************************************
/ 화면 View UI 설정
// Dialog init
// layerd window control
****************************************************************************/

HRESULT CAbleClassDlg::InitLayer() {

	HRESULT hr = S_OK;
	if (_d3d11DeviceLayer) return hr;

	if (SUCCEEDED(hr))
	{
		hr = CreateD3D11Device();
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateDCompositionDevice();
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateDCompositionRenderTarget();
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateDCompositionVisualTree();
	}

	if (SUCCEEDED(hr))
	{
		hr = CreateTransforms();
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pDeviceLayer->Commit();
	}

	return hr;

}



HRESULT CAbleClassDlg::CreateD3D11Device()
{
	HRESULT hr = S_OK;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
	};

	D3D_FEATURE_LEVEL featureLevelSupported;

	for (int i = 0; i < sizeof(driverTypes) / sizeof(driverTypes[0]); ++i)
	{
		CComPtr<ID3D11Device> d3d11Device;
		CComPtr<ID3D11DeviceContext> d3d11DeviceContext;

		hr = D3D11CreateDevice(
			nullptr,
			driverTypes[i],
			NULL,
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,
			NULL,
			0,
			D3D11_SDK_VERSION,
			&d3d11Device,
			&featureLevelSupported,
			&d3d11DeviceContext);

		if (SUCCEEDED(hr))
		{
			_d3d11DeviceLayer = d3d11Device.Detach();
			_d3d11DeviceContextLayer = d3d11DeviceContext.Detach();
			break;
		}
	}

	return hr;
}

HRESULT CAbleClassDlg::CreateDCompositionDevice()
{
	HRESULT hr = (_d3d11DeviceLayer == nullptr) ? E_UNEXPECTED : S_OK;

	CComPtr<IDXGIDevice> dxgiDevice;

	if (SUCCEEDED(hr))
	{
		hr = _d3d11DeviceLayer->QueryInterface(&dxgiDevice);
	}

	if (SUCCEEDED(hr))
	{
		hr = DCompositionCreateDevice(dxgiDevice, __uuidof(IDCompositionDevice), reinterpret_cast<void **>(&m_pDeviceLayer));
	}

	return hr;
}

HRESULT CAbleClassDlg::CreateDCompositionRenderTarget()
{
	HRESULT hr = ((m_pDeviceLayer == nullptr) || (GetSafeHwnd() == NULL)) ? E_UNEXPECTED : S_OK;

	if (SUCCEEDED(hr))
	{
		hr = m_pDeviceLayer->CreateTargetForHwnd(GetSafeHwnd(), TRUE, &m_pHwndRenderTargetLayer);
	}

	return hr;
}

HRESULT CAbleClassDlg::CreateDCompositionVisualTree()
{
	HRESULT hr = ((m_pDeviceLayer == nullptr) || (GetSafeHwnd() == NULL)) ? E_UNEXPECTED : S_OK;

	if (SUCCEEDED(hr))
	{
		// Create the root visual.
		hr = m_pDeviceLayer->CreateVisual(&m_pRootVisualLayer);
	}
	
	if (SUCCEEDED(hr))
	{
		// Make the visual the root of the tree.
		hr = m_pHwndRenderTargetLayer->SetRoot(m_pRootVisualLayer);
	}


	// visual main menu
	if (SUCCEEDED(hr))
	{
		hr = m_pDeviceLayer->CreateVisual(&m_pVisualMainMenu);
		hr = m_pDeviceLayer->CreateSurfaceFromHwnd(m_boardMenuDlg.GetSafeHwnd(), &m_pMainMenuSurface);
		hr = m_pVisualMainMenu->SetContent(m_pMainMenuSurface);
		hr = m_pRootVisualLayer->AddVisual(m_pVisualMainMenu, FALSE, NULL);

	}


	// visual book mark
	if (SUCCEEDED(hr))
	{
		hr = m_pDeviceLayer->CreateVisual(&m_pVisualBookMark);
		hr = m_pDeviceLayer->CreateSurfaceFromHwnd(m_bookMarkDlg.GetSafeHwnd(), &m_pBookMarkSurface);
		hr = m_pVisualBookMark->SetContent(m_pBookMarkSurface);
		hr = m_pRootVisualLayer->AddVisual(m_pVisualBookMark, TRUE, m_pVisualMainMenu);
	}



	if (SUCCEEDED(hr))
	{
		hr = m_pDeviceLayer->CreateVisual(&m_pVisualToolMenu);
		hr = m_pDeviceLayer->CreateSurfaceFromHwnd(m_toolDlg.GetSafeHwnd(), &m_pToolMenuSurface);
		hr = m_pVisualToolMenu->SetContent(m_pToolMenuSurface);
		//hr = m_pRootVisualLayer->AddVisual(m_pVisualToolMenu, TRUE, NULL);
	}


	


	//IDCompositionRectangleClip* clipRect;
	//m_pDeviceLayer->CreateRectangleClip(&clipRect);
	//
	//if (SUCCEEDED(hr))
	//{
	//	clipRect->SetLeft(static_cast<float>(m_rtContext.left));
	//	clipRect->SetTop(static_cast<float>(m_rtContext.top));
	//	clipRect->SetRight(static_cast<float>(m_rtContext.right));
	//	clipRect->SetBottom(static_cast<float>(m_rtContext.bottom));
	//	hr = m_pVisualMainMenu->SetClip(clipRect);
	//}


	return hr;
}

HRESULT CAbleClassDlg::CreateTransforms()
{
	RECT rcClient;
	::GetClientRect(GetSafeHwnd(), &rcClient);
	int nCenterX = static_cast<int>((rcClient.right - rcClient.left) * 0.5);

	// Create a translate transform for the control child visual.
	HRESULT hr = m_pDeviceLayer->CreateTranslateTransform(&m_pControlTranslateTransform);
	if (SUCCEEDED(hr))
	{
		// Set the offset of x-axis of the control child visual
		hr = m_pVisualMainMenu->SetOffsetX(static_cast<float>(rcClient.right- g_pSkin->m_boardMenu.right));
		hr = m_pVisualMainMenu->SetOffsetY(0.0f);
		hr = m_pControlTranslateTransform->SetOffsetX(0.0f);
		hr = m_pControlTranslateTransform->SetOffsetY(0.0f);
		hr = m_pVisualMainMenu->SetTransform(m_pControlTranslateTransform);

		// Create a rotate transform.
		//hr = m_pDeviceLayer->CreateRotateTransform(&m_pRotateTransform);
		//hr = m_pRotateTransform->SetAngle(0.0f);
		//hr = m_pRotateTransform->SetCenterX(rcClient.right / 2.0f);
		//hr = m_pRotateTransform->SetCenterY(rcClient.bottom / 2.0f);
		//IDCompositionTransform *pTransformArray[] = { m_pRotateTransform };
		//hr = m_pDeviceLayer->CreateTransformGroup(pTransformArray, 1, &m_pTransformGroup);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pVisualBookMark->SetOffsetX(static_cast<float>(rcClient.right - g_pSkin->m_bookMark.right));
		hr = m_pVisualBookMark->SetOffsetY(0.0f);
	}


	hr = m_pDeviceLayer->CreateTranslateTransform(&m_transTool);

	//Create a transform group (rotate, scale, skew) for the video child visual
	if (SUCCEEDED(hr))
	{
		m_pVisualToolMenu->SetOffsetX(static_cast<float>(nCenterX - g_pSkin->m_toolRect.right * 0.5f));
		m_pVisualToolMenu->SetOffsetY(static_cast<float>(m_rtContext.bottom - g_pSkin->m_toolRect.bottom));

		hr = m_transTool->SetOffsetX(0.0f);
		hr = m_transTool->SetOffsetY(0.0f);
		hr = m_pVisualToolMenu->SetTransform(m_transTool);

	}


	


	return hr;
}


HRESULT CAbleClassDlg::DoTrans(UINT target, UINT show) {
	


	HRESULT hr = S_FALSE;
	IDCompositionAnimation* pAnimation = NULL;
	BOOL bCloak = TRUE;

	if (target == IDD_BOARD_MENU) {
		if (show == SW_HIDE) {
			m_boardMenuVisible = FALSE;
			::DwmSetWindowAttribute(m_boardMenuDlg.GetSafeHwnd(), DWMWA_CLOAK, &bCloak, sizeof(bCloak));
			hr = SetTranslation(0.0f, static_cast<float>(g_pSkin->m_boardMenu.right), 0.0f, 0.0f, 0.0f, 0.4f, m_pControlTranslateTransform);
			m_boardMenuDlg.ShowWindow(SW_HIDE);
		}
		else if (show == SW_SHOW) {
			m_boardMenuVisible = TRUE;
			hr = SetTranslation(static_cast<float>(g_pSkin->m_boardMenu.right), 0.0f, 0.0f, 0.0f, 0.0f, 0.4f, m_pControlTranslateTransform);
			m_boardMenuDlg.ShowWindow(SW_SHOW);

			SetTimer(IDD_BOARD_MENU, 600, NULL);
		}



		if (SUCCEEDED(hr))
		{
			hr = m_pDeviceLayer->Commit();
		}
	}
	return hr;
}


HRESULT CAbleClassDlg::DoTransTool() {
	HRESULT hr = S_OK;
	IDCompositionAnimation* pAnimation = NULL;

	RECT rt;
	m_toolDlg.GetWindowRect(&rt);
	POINT pt;
	pt.x = rt.left;
	pt.y = rt.top;
	::ScreenToClient(m_hWnd, &pt);


	m_pVisualToolMenu->SetOffsetX(pt.x);
	m_pVisualToolMenu->SetOffsetY(pt.y);

	hr = m_pRootVisualLayer->AddVisual(m_pVisualToolMenu, TRUE, NULL);
	

	RECT mainRt;
	GetClientRect(&mainRt);


	float fDestToolX;
	float fDestToolY;

	if (m_toolMenuVisible) {
		m_toolMenuVisible = FALSE;
		fDestToolX = (mainRt.right - mainRt.left) * 0.5 - ((rt.right - rt.left) * 0.5) - pt.x;
		fDestToolY = mainRt.bottom - pt.y - 40;
	}
	else {
		m_toolMenuVisible = TRUE;
		fDestToolX = (mainRt.right - mainRt.left) * 0.5 - ((rt.right - rt.left) * 0.5) - pt.x;
		fDestToolY = mainRt.bottom - pt.y - (rt.bottom - rt.top);
	}

	BOOL bCloak = TRUE;
	::DwmSetWindowAttribute(m_toolDlg.GetSafeHwnd(), DWMWA_CLOAK, &bCloak, sizeof(bCloak));

	hr = SetTranslation(0.0f, fDestToolX, 0.0f, fDestToolY, 0.0f, 0.2f, m_transTool);

	if (SUCCEEDED(hr))
	{
		hr = m_pDeviceLayer->Commit();
	}


	SetTimer(IDD_TOOL_MENU, 200, NULL);

	return hr;
}


HRESULT CAbleClassDlg::SetTranslation(float beginPositionX, float endPositionX, float beginPositionY, float endPositionY, float beginTime, float endTime, IDCompositionTranslateTransform *translateTransform)
{
	CComPtr<IUIAnimationTransition2> acc;


	

	CComPtr<IDCompositionAnimation> offsetXAnimation;

	HRESULT hr = CreateTranslateAnimation(beginPositionX, endPositionX, beginTime, endTime, &offsetXAnimation);

	

	if (SUCCEEDED(hr))
	{
		hr = translateTransform->SetOffsetX(offsetXAnimation);
	}

	CComPtr<IDCompositionAnimation> offsetYAnimation;

	if (SUCCEEDED(hr))
	{
		hr = CreateTranslateAnimation(beginPositionY, endPositionY, beginTime, endTime, &offsetYAnimation);
	}

	if (SUCCEEDED(hr))
	{
		hr = translateTransform->SetOffsetY(offsetYAnimation);
	}

	return hr;
}


HRESULT CAbleClassDlg::CreateTranslateAnimation(float beginValue, float endValue, float beginTime, float endTime, IDCompositionAnimation **linearAnimation)
{
	HRESULT hr = (linearAnimation == nullptr) ? E_POINTER : S_OK;

	if (SUCCEEDED(hr))
	{
		*linearAnimation = nullptr;

		hr = (m_pDeviceLayer == nullptr) ? E_UNEXPECTED : S_OK;
	}

	CComPtr<IDCompositionAnimation> animation;

	if (SUCCEEDED(hr))
	{
		hr = m_pDeviceLayer->CreateAnimation(&animation);
	}



	// Ensures animation start value takes effect immediately
	if (SUCCEEDED(hr))
	{
		if (beginTime > 0.0)
		{
			hr = animation->AddCubic(0.0, beginValue, 0.0f, 0.0f, 0.0f);
		}
	}

	if (SUCCEEDED(hr))
	{
		//hr = animation->AddCubic(beginTime, beginValue, (endValue - beginValue) / (endTime - beginTime), 0.0f, 0.0f);
		hr = animation->AddCubic(beginTime, beginValue, (endValue - beginValue) / (endTime - beginTime) * 0.8, 0.0f, 0.0f);

		//hr = animation->AddCubic(endTime, endValue, (endValue - beginValue) / (endTime - beginTime) * 0.2, 0.0f, 0.0f);

		//TRACE(_T("\n>>>>> btime(%.02f) bValue(%.02f) th(%.02f) <<<<<\n"), beginTime, beginValue, (endValue - beginValue) / (endTime - beginTime));

	}


	//hr = animation->AddCubic(beginTime, beginValue, (endValue - beginValue) / (endTime - beginTime), 0.0f, 0.0f);


	if (SUCCEEDED(hr))
	{
		hr = animation->End(endTime, endValue);
	}

	if (SUCCEEDED(hr))
	{
		*linearAnimation = animation.Detach();
	}



	return hr;
}



void CAbleClassDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == IDD_BOARD_MENU) {
		KillTimer(IDD_BOARD_MENU);
		if (m_boardMenuVisible) {
			BOOL bCloak = FALSE;
			DwmSetWindowAttribute(m_boardMenuDlg.GetSafeHwnd(), DWMWA_CLOAK, &bCloak, sizeof(bCloak));
		}
	}


	if (nIDEvent == IDD_TOOL_MENU) {
		KillTimer(IDD_TOOL_MENU);
		//if (m_boardMenuVisible) {
			BOOL bCloak = FALSE;

			RECT rt;
			m_toolDlg.GetWindowRect(&rt);
			RECT mainRt;
			GetClientRect(&mainRt);
			int fDestToolX = (mainRt.right - mainRt.left) * 0.5 - ((rt.right - rt.left) * 0.5);
			int fDestToolY;
			
			if (m_toolMenuVisible) {
				fDestToolY = mainRt.bottom - (rt.bottom - rt.top);
			}
			else {
				fDestToolY = mainRt.bottom - 40;
			}

			m_toolDlg.SetWindowPos(NULL, fDestToolX, fDestToolY, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
			DwmSetWindowAttribute(m_toolDlg.GetSafeHwnd(), DWMWA_CLOAK, &bCloak, sizeof(bCloak));

			m_pRootVisualLayer->RemoveVisual(m_pVisualToolMenu);
			m_pDeviceLayer->Commit();
	}

	if (TIMER_REC_STOP == nIDEvent) {
		KillTimer(TIMER_REC_STOP);
		AfxMessageBox(L"녹화가 종료되었습니다");
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CAbleClassDlg::OnEraseBkgnd(CDC* pDC)
{

	return FALSE;
}


void CAbleClassDlg::OnExitSizeMove()
{

	CDialogEx::OnExitSizeMove();
}


//void CAbleClassDlg::OpenBook(int nIndex)
//{
	//SetViewMode(VIEW_WHITEBOARD);
	//InitLayer();
	//SetViewChange();

	//if (nIndex == 0) {
	//	m_board.LoadBook(L"\\book2");
	//}
	//else if (nIndex == 1) {
	//	m_board.LoadBook(L"\\book_india");
	//}
	//else if (nIndex == 2) {
	//	m_board.LoadBook(L"\\book_japan");
	//}
	//else if (nIndex == 3) {
	//	m_board.LoadBook(L"\\book_usa");
	//}
	//else {
	//	m_board.LoadBook(L"\\book2");
	//}
	////m_board.DoListMode();
	//m_board.DoItemMode(0);
	//ResetFocus();
//}




void CAbleClassDlg::OpenBook(std::wstring strBook, bool bLandscape)
{
#ifdef _DEBUG
	if (TRUE) {
#else
	if (g_cert.CheckCert()) {
#endif
		SetViewMode(VIEW_WHITEBOARD);
		InitLayer();
		SetViewChange();

		LoadBookDB(strBook + L"\\etc\\book.db");
		m_board.SetLandscape(bLandscape);
		m_board.LoadBook(strBook);
		m_board.DoItemMode(0);

		m_bookMarkDlg.SetBoardMenuVisible(FALSE);
		DoTrans(IDD_BOARD_MENU, SW_HIDE);

		m_toolDlg.GetColorInfoFromDatabase();
		m_inkDlg.GetColorInfoFromDatabase();
		ResetFocus();

	}
	else {
		if (AfxMessageBox(L"제품 등록 후 이용가능합니다. 제품 등록을 하시겠습니까?", MB_OKCANCEL) == IDOK) {
			CLicenseDlg dlg;
			if (dlg.DoModal()) {

			}
		}
	}

}


void CAbleClassDlg::CloseBook()
{
	m_board.CloseBook();
	SetViewMode(VIEW_MAIN);
	SetViewChange();
}


void CAbleClassDlg::AddBook()
{
	//const DWORD fileNameMaxLength = 1000 + 1;
	//const DWORD bufferSize = 1000 + 1;
	//TCHAR* filenamesBuffer = new TCHAR[bufferSize];
	//filenamesBuffer[0] = NULL;
	//filenamesBuffer[bufferSize - 1] = NULL;

	//CFileDialog dlg(TRUE, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES,
	//	//_T("문서파일 (*.doc,*.docx,*.xls,*.xlsx,*.ppt,*.pptx,*.pdf)|*.doc;*.docx;*.xls;*.xlsx;*.ppt;*.pptx;*.pdf;|이미지파일(*.png,*.jpg,*.bmp)|*.png;*.jpg;*.jpeg;*.bmp|All Files (*.*)|*.*||"), this);
	//	_T("문서파일 (*.doc,*.docx,*.ppt,*.pptx,*.pdf,*.hwp)|*.doc;*.docx;*.ppt;*.pptx;*.pdf;*.hwp;|이미지파일(*.png,*.jpg,*.bmp)|*.png;*.jpg;*.jpeg;*.bmp|All Files (*.*)|*.*||"), NULL);
	//dlg.m_ofn.lpstrFile = filenamesBuffer;
	//dlg.m_ofn.nMaxFile = bufferSize;
	//int key = 0;

	//if (dlg.DoModal() == IDOK)
	//{
	//	POSITION fileNamesPosition = dlg.GetStartPosition();
	///*	m_convertFile.strFilePath = dlg.GetNextPathName(fileNamesPosition);
	//	if (!m_convertFile.strFilePath.empty()) {
	//		m_convertFile.strFileName = ST_FILEDATA::GetFilename(m_convertFile.strFilePath);
	//		m_convertFile.strFileType = ST_FILEDATA::GetExtension(m_convertFile.strFileName);
	//		m_convertFile.strFileDisplayName = ST_FILEDATA::GetDisplayName(m_convertFile.strFileName);
	//		m_convertFile.result = 0;
	//		m_convertFile.format = ST_FILEDATA::GetFormat(m_convertFile.strFileType);
	//		m_editFilePath.SetWindowTextW(m_convertFile.strFilePath.c_str());
	//		m_editTitle.SetWindowTextW(m_convertFile.strFileDisplayName.c_str());

	//	}*/
	//}
	//delete[] filenamesBuffer;

	//return;

	//if (g_cert.CheckCert()) {
#ifdef _DEBUG
	if (1) {
#else
	if (g_cert.CheckCert()) {
#endif
		CBookDlg dlg;
		if (dlg.DoModal() == IDOK) {
		}

		if (dlg.IsExistNewItem()) {
			RedrawBookList();
		}
	}
	else {
		if (AfxMessageBox(L"제품 등록 후 이용가능합니다. 제품 등록을 하시겠습니까?", MB_OKCANCEL) == IDOK) {
			CLicenseDlg dlg;
			if (dlg.DoModal()) {

			}
		}
	}
}


void CAbleClassDlg::OpenEditor(std::wstring strBook, bool bLandscape)
{
#ifdef _DEBUG
	if (1) {
#else
	if (g_cert.CheckCert()) {
#endif
		SetViewMode(VIEW_EDIT);
		SetViewChange();

		LoadBookDB(strBook + L"\\etc\\book.db");
		m_editorDlg.SetLandscape(bLandscape);
		m_editorDlg.LoadBook(strBook);
		ResetFocus();

	}
	else {
		if (AfxMessageBox(L"제품 등록 후 이용가능합니다. 제품 등록을 하시겠습니까?", MB_OKCANCEL) == IDOK) {
			CLicenseDlg dlg;
			if (dlg.DoModal()) {

			}
		}
	}

}


void CAbleClassDlg::CloseEditorBook()
{

	m_editorDlg.DoClose();

	SetViewMode(VIEW_MAIN);
	SetViewChange();
}

void CAbleClassDlg::SaveEditorBook()
{
	m_editorDlg.DoSave();
}





void CAbleClassDlg::SetDeleteBookMode()
{
	if (m_mainDlg.m_bDeleteMode) {
		ShowBookDeleteDialog(SW_HIDE);
		m_mainDlg.SetDeleteBookMode();
	}
	else {
		ShowBookDeleteDialog(SW_SHOW);
		m_mainDlg.SetDeleteBookMode();
	}
}

void CAbleClassDlg::DoDeleteBook()
{
	m_mainDlg.DoDeleteBook();
}




void CAbleClassDlg::ShowBookDeleteDialog(int cmd) {
	m_bookDeleteDlg.ShowWindow(cmd);
}

void CAbleClassDlg::RedrawBookList()
{
	m_mainDlg.RedrawBookList();
}


void CAbleClassDlg::DbSelectBookData() {
	m_bookData.clear();
	if (m_pBookDB) {
		if (m_pBookDB->sqlite_select_book(&m_bookData) > 0) {
		}
	}

}


void CAbleClassDlg::DeleteBookData(std::vector<UINT>& vDeleteItem) {
	if (m_pBookDB) {
		if (vDeleteItem.size() > 0) {
			for (auto bookData = vDeleteItem.begin(); bookData != vDeleteItem.end(); bookData++)
			{

				UINT id = *bookData;
				m_pBookDB->sqlite_delete_book(id);
			}
		}
	}
}

void CAbleClassDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CAbleClassDlg::DoMainSetting() {
	CMainSettingDlg dlg;
	if (dlg.DoModal() == IDOK) {
		if (dlg.IsFullSizeUpdate())
			ChangeFullSize();
	}

}

void CAbleClassDlg::DoRecord() {

	//if (m_pipeLine.IsWorking()) {
	//	m_pipeLine.End();
	//}
	//else {
	//	m_pipeLine.Start(this);
	//}


	m_board.DoRecord();
	if (m_bIsRecording) {
		m_bIsRecording = FALSE;
		StopRecordSendMessage();
	}
	else {
		
		if (AfxMessageBox(L"녹화를 시작하시겠습니까?", MB_OKCANCEL) == IDOK) {
			m_bIsRecording = TRUE;
			StartRecordSendMessage();
		}
	}


	return;
}


void CAbleClassDlg::ChangeFullSize() {
	if (g_database.GetSetting()->set_ui_fullsize == 1) {
		ShowWindow(SW_HIDE);
		ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
		ModifyStyle(WS_MAXIMIZEBOX, 0);
		ModifyStyle(WS_CAPTION, 0);
		ShowWindow(SW_SHOWNOACTIVATE);
		ShowWindow(SW_SHOWMAXIMIZED);
	}
	else {
		ShowWindow(SW_HIDE);
		ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);
		ModifyStyle(0, WS_MAXIMIZEBOX);
		ModifyStyle(0, WS_CAPTION);
		ShowWindow(SW_SHOWNOACTIVATE);
		ShowWindow(SW_SHOWMAXIMIZED);

	}

}


BOOL __stdcall EnumProcessWindowsProc(HWND hwnd, LPARAM lParam)
{
	ProcessWindowsInfo *Info = reinterpret_cast<ProcessWindowsInfo*>(lParam);
	DWORD WindowProcessID;

	GetWindowThreadProcessId(hwnd, &WindowProcessID);

	if (WindowProcessID == Info->ProcessID) {
		Info->Windows.push_back(hwnd);
		//TRACE(L"\n>>>> hwnd (pid:%d) (%d) <<<<\n", WindowProcessID , hwnd);
	}
	
	return true;
}
void CAbleClassDlg::RunRecHandle() {

	if (m_AbleRecInfo) {
		bool isHandle = false;
		for (int i = 0; i < m_AbleRecInfo->Windows.size(); i++) {
			if (::IsWindow(m_AbleRecInfo->Windows[i])) {
				isHandle = true;
			}
		}
		if (isHandle == false) {
			delete m_AbleRecInfo;
			m_AbleRecInfo = nullptr;
		}
	}

	if (m_AbleRecInfo == nullptr) {

		GetProgramPath();

		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = _T("AbleRec.exe");
		ShExecInfo.lpDirectory = m_strExecutePath.c_str();
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;
		if (ShellExecuteEx(&ShExecInfo)) {
			//AfxMessageBox(m_strExecutePath.c_str());
			WaitForInputIdle(ShExecInfo.hProcess, INFINITE);
			//ProcessWindowsInfo Info(GetProcessId(ShExecInfo.hProcess));
			m_AbleRecInfo = new ProcessWindowsInfo(GetProcessId(ShExecInfo.hProcess));
			EnumWindows((WNDENUMPROC)EnumProcessWindowsProc, reinterpret_cast<LPARAM>(m_AbleRecInfo));

		}
	}


}

void CAbleClassDlg::StopRecordSendMessage() {
	if (m_AbleRecInfo) {
		bool isHandle = true;
		for (int i = 0; i < m_AbleRecInfo->Windows.size(); i++) {

			if (::IsWindow(m_AbleRecInfo->Windows[i])) {
				::PostMessage(m_AbleRecInfo->Windows[i], PROC_REC_STOP, 0, 0);
			}
			else {
				isHandle = false;
			}

		}

		if (isHandle == false) {
			delete m_AbleRecInfo;
	
			m_AbleRecInfo = nullptr;
		}

		SetTimer(TIMER_REC_STOP, 1000, NULL);
	}

}




void CAbleClassDlg::StartRecordSendMessage() {
	std::wstring wstrNow;
	std::wstring strRecPath = g_database.GetSetting()->root_tcolumn2;

	if (strRecPath.empty()) {
		AfxMessageBox(L"환경설정에서 녹화 파일 저장 위치를 지정하세요.\n- 녹화 실패\n");
		m_bIsRecording = FALSE;
		return;
	}

	auto now = std::chrono::system_clock::now();
	auto in_time_now = std::chrono::system_clock::to_time_t(now);
	std::tm tmNow;
	localtime_s(&tmNow, &in_time_now);

	wstrNow = fmt::format(L"{}/{:04}{:02}{:02}-{:02}{:02}{:02}.mp4", strRecPath.c_str(), tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
	std::replace(wstrNow.begin(), wstrNow.end(), L'\\', L'/');
	std::string sendMessage = g_global.ws2s(wstrNow);

	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = (sendMessage.length());
	cpd.lpData = (void*)sendMessage.c_str();

	RunRecHandle();

	if (m_AbleRecInfo) {
		bool isHandle = true;
		for (int i = 0; i < m_AbleRecInfo->Windows.size(); i++) {


			if (::IsWindow(m_AbleRecInfo->Windows[i])) {
				::SendMessage(m_AbleRecInfo->Windows[i], WM_COPYDATA, (WPARAM)AfxGetApp()->m_pMainWnd->GetSafeHwnd(), (LPARAM)&cpd);
				Sleep(500);
				::PostMessage(m_AbleRecInfo->Windows[i], PROC_REC_START, 0, 0);
			}
			else {
				isHandle = false;
			}
		}

		if (isHandle == false) {
			delete m_AbleRecInfo;
			m_AbleRecInfo = nullptr;
		}
	}

}

//std::wstring CAbleClassDlg::GetNow() {
//	auto now = std::chrono::system_clock::now();
//	auto in_time_now = std::chrono::system_clock::to_time_t(now);
//	std::tm tmNow;
//	localtime_s(&tmNow, &in_time_now);
//
//	return fmt::format(L"{}{:04}{:02}{:02}-{:02}{:02}{:02}.mp4", tmNow.tm_year + 1900, tmNow.tm_mon + 1, tmNow.tm_mday, tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec);
//}



void CAbleClassDlg::GetProgramPath()
{

	TCHAR szFullpath[MAX_PATH] = { 0 }, szDrive[256] = { 0 }, szDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFullpath, MAX_PATH);
	_tsplitpath(szFullpath, szDrive, szDir, NULL, NULL);
	m_strExecutePath = szDrive;
	m_strExecutePath.append(szDir);

#ifdef _DEBUG
	m_strExecutePath = L"C:\\job\\code\\AbleRec\\x64\\Release";
#endif
}