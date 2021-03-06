// CWhiteBoardDlg.cpp: 구현 파일
//


#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CWhiteBoardDlg.h"
#include "afxdialogex.h"
#include "direct2dutility.h"
#include "animationutility.h"
#include "ShellItemsLoader.h"
#include "FlyerAnimation.h"
#include "MoverAnimation.h"
#include "PanAnimation.h"
#include "ImageThumbnailControl.h"

#include <rtscom_i.c>   // RTS GUID definitions
#include "CSyncEventHandlerRTS.h"
#include "lib/fmt/format.h"


CComPtr<IMFPMediaPlayer>	CWhiteBoardDlg::m_mediaPlayer = nullptr;
CComPtr<IMFPMediaPlayerCallback> CWhiteBoardDlg::m_mediaPlayerCallBack = nullptr;

IMPLEMENT_DYNAMIC(CWhiteBoardDlg, CDialogEx)

CWhiteBoardDlg::CWhiteBoardDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WHITEBOARD, pParent),
	BackgroundColor(0x000000),
	m_operationType(ImageOperationTypeNone),
	m_operationTypeEmptyBoard(ImageOperationTypePen),
	m_prevOperationType(ImageOperationTypeNone),
	m_bReversal(FALSE),
	//m_penColor(D2D1::ColorF(D2D1::ColorF::Red)),
	m_bIsDrawing(false),
	m_bIsEraser(false),
	m_pageMode(PAGE_LIST),
	m_itemMode(ITEM_VIEW),
	m_itemModeEmptyBoard(ITEM_EDIT),
	m_bIsListAnimation(false),
	//m_previousRenderTargetSize(D2D1::SizeF(0.0f, 0.0f)),
	m_lastAnimationSize(D2D1::SizeF(0.0f, 0.0f)),
	m_currentAnimation(MoveAround),
	m_strRootDir(L"\\PoDo"),
	m_strDocumentName(L"\\book2"),
	m_nCurrentPage(0),
	m_nAnimatePage(0),
	m_currentZoom(1.0),
	m_emptyBoardZoom(1.0),
	m_beforeZoom(1.0),
	m_currentTransX(0.0f),
	m_beforeTransX(0.0f),
	m_currentTransY(0.0f),
	m_beforeTransY(0.0f),
	m_currentAlpha(0.0f),
	m_beforeAlpha(0.0f),
	m_bIsModeChangeAnimation(false),
	m_bIsSlideAnimation(false),
	m_bIsZoomAnimation(false),
	m_bIsEmptyBoardAnimation(false),
	m_currentPanPoint(D2D1::Point2F(0, 0)),
	m_emptyBoardPanPoint(D2D1::Point2F(0, 0)),
	m_zoomMin(0.6f),
	m_zoomMax(100.0f),
	m_zoomStep(0.6f),
	m_zoomBox(D2D1::RectF(0, 0, 0, 0)),
	m_zoomBoxDrawing(D2D1::RectF(0, 0, 0, 0)),
	m_bIsZoomming(false),
	m_zoomDuration(0.6f),
	m_clippingBox(D2D1::RectF(0, 0, 0, 0)),
	m_clippingBoxDrawing(D2D1::RectF(0, 0, 0, 0)),
	m_bIsClipping(false),
	m_nEmptyBoardPage(0),
	m_bIsDrag(false),
	m_mouseDownPoint(D2D1::Point2F(0, 0)),
	m_mousePrevPoint(D2D1::Point2F(0, 0)),
	m_bIsBackgroundLoading(false),
	m_bFirstListLoading(true),
	m_autoIncrese(0),
#ifdef STYLUS_ON
	m_bUseDyniamcRenderer(true),
	m_pRealTimeStylus(nullptr),
	m_pDynamicRenderer(nullptr),
	m_pSyncEventHandlerRTS(nullptr),
#endif
	m_bEditorBlankVisibled(true),
	m_pageChanging(PAGE_NON),
	m_penType(PENTYPE_PEN),
	m_penId(0),
	m_bLandscape(false)
{

	for (int i = 0; i < 6; i++) {
		m_penColorPalette[i].clr = RGB(255, 0, 0);
		m_penColorPalette[i].b = true;
	}


	m_Pen = new CBoardPen(PENTYPE_PEN);
	m_HPen = new CBoardPen(PENTYPE_HPEN);
	m_PenEmptyBoard = new CBoardPen(PENTYPE_PEN);
	m_HPenEmptyBoard = new CBoardPen(PENTYPE_HPEN);

}

CWhiteBoardDlg::~CWhiteBoardDlg()
{
}

void CWhiteBoardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWhiteBoardDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYUP()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_TOUCH, OnTouch)
	ON_MESSAGE(WM_POINTERDOWN, OnPointerDown)
	ON_MESSAGE(WM_POINTERUPDATE, OnPointerUpdate)
	ON_MESSAGE(WM_POINTERUP, OnPointerUp)
	ON_MESSAGE(MY_WM_BEGINDRAW, &CWhiteBoardDlg::OnBeginDraw)
	ON_MESSAGE(MY_WM_VIDEOCLIP, &CWhiteBoardDlg::OnVideoClipClick)
END_MESSAGE_MAP()


// CWhiteBoardDlg 메시지 처리기


int CWhiteBoardDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// D2D automatically handles high DPI settings
	//SetProcessDPIAware();
	RegisterTouchInput();

	HRESULT hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		hr = SharedObject<AsyncLoaderSpace::AsyncLoaderHelper>::Create(&m_AsyncLoaderHelper);
	}
	if (SUCCEEDED(hr))
	{
		hr = SharedObject<ThumbnailLayoutManager>::Create(m_AsyncLoaderHelper, &m_thumbnailLayoutManager);
	}


	if (SUCCEEDED(hr))
	{
		hr = m_AsyncLoaderHelper->ConnectClient(this);
	}

	if (SUCCEEDED(hr))
	{
		CreateDeviceResources();
	}

	if (m_videoClip.Create(IDD_VIDEO_CLIP, this)) {
		m_videoClip.ShowWindow(SW_HIDE);
	}

	if (m_setDlg.Create(IDD_BOARD_SET, this)) {
		m_setDlg.SetWindowPos(&wndTop, 10, 10, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE);
		m_setDlg.ShowWindow(SW_HIDE);
	}


	return 0;
}




void CWhiteBoardDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	UnregisterTouchInput();
	DiscardDeviceResources();
}


HRESULT CWhiteBoardDlg::RegisterTouchInput() {
	HRESULT hr = S_OK;
	::RegisterTouchWindow(m_hWnd, NULL);
	
	//POINTER_PEN_INFO penInfo;
	//BOOL b = GetPointerPenInfo(0, &penInfo);
	//TRACE(L"\n>>> di(%d) (%d) <<<\n", b, penInfo.pressure);


	//int digi = GetSystemMetrics(SM_DIGITIZER);
	//int nInputs = GetSystemMetrics(SM_MAXIMUMTOUCHES);
	//int tabletPc = GetSystemMetrics(SM_TABLETPC);

	//POINTER_TOUCH_INFO contact;
	//BOOL bRet = TRUE;

	////
	//// assume a maximum of 10 contacts and turn touch feedback off
	////
	//InitializeTouchInjection(10, TOUCH_FEEDBACK_NONE);

	////
	//// initialize the touch info structure
	////
	//memset(&contact, 0, sizeof(POINTER_TOUCH_INFO));

	//contact.pointerInfo.pointerType = PT_TOUCH; //we're sending touch input
	//contact.pointerInfo.pointerId = 0;          //contact 0
	//contact.pointerInfo.ptPixelLocation.x = 640;
	//contact.pointerInfo.ptPixelLocation.y = 480;
	//contact.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;
	//contact.touchFlags = TOUCH_FLAG_NONE;
	//contact.touchMask = TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
	//contact.orientation = 90;
	//contact.pressure = 32000;

	//bRet = InjectTouchInput(1, &contact);

	//TRACE(L"\n>>> di(%d) (%d) (%d) <<<\n", digi, nInputs, contact.pressure);

	//const int maxTouchPoints = GetSystemMetrics(SM_MAXIMUMTOUCHES);

	//DWORD_PTR dwHwndTabletProperty =
	//	TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
	//	TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
	//	TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
	//	TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)

	//ATOM atom = ::GlobalAddAtom(MICROSOFT_TABLETPENSERVICE_PROPERTY);
	//SetProp(m_hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
	//GlobalDeleteAtom(atom);

	BOOL fEnabled = FALSE;
	SetWindowFeedbackSetting(m_hWnd,
		FEEDBACK_TOUCH_CONTACTVISUALIZATION,
		0, sizeof(fEnabled), &fEnabled);
	//SetWindowFeedbackSetting(m_hWnd,
	//	FEEDBACK_PEN_BARRELVISUALIZATION,
	//	0, sizeof(fEnabled), &fEnabled);
	//SetWindowFeedbackSetting(m_hWnd,
	//	FEEDBACK_PEN_TAP,
	//	0, sizeof(fEnabled), &fEnabled);
	//SetWindowFeedbackSetting(m_hWnd,
	//	FEEDBACK_TOUCH_TAP,
	//	0, sizeof(fEnabled), &fEnabled);
	//SetWindowFeedbackSetting(m_hWnd,
	//	FEEDBACK_TOUCH_DOUBLETAP,
	//	0, sizeof(fEnabled), &fEnabled);
	//SetWindowFeedbackSetting(m_hWnd,
	//	FEEDBACK_TOUCH_PRESSANDHOLD,
	//	0, sizeof(fEnabled), &fEnabled);
	//SetWindowFeedbackSetting(m_hWnd,
	//	FEEDBACK_PEN_PRESSANDHOLD,
	//	0, sizeof(fEnabled), &fEnabled);

	/*FEEDBACK_TYPE flagToTurnOff[] = {
		FEEDBACK_TOUCH_CONTACTVISUALIZATION,
		FEEDBACK_PEN_BARRELVISUALIZATION,
		FEEDBACK_PEN_TAP,
		FEEDBACK_PEN_DOUBLETAP,
		FEEDBACK_PEN_PRESSANDHOLD,
		FEEDBACK_PEN_RIGHTTAP,
		FEEDBACK_TOUCH_TAP,
		FEEDBACK_TOUCH_DOUBLETAP,
		FEEDBACK_TOUCH_PRESSANDHOLD ,
		FEEDBACK_TOUCH_RIGHTTAP ,
		FEEDBACK_GESTURE_PRESSANDTAP
	};

	const BOOL value = FALSE;
	for (int i = 0; i < 11; i++)
		SetWindowFeedbackSetting(m_hWnd, flagToTurnOff[i], 0, sizeof(value), &value);*/

#ifdef STYLUS_ON
	// Create RTS object
	m_pRealTimeStylus = nullptr;
	hr = CoCreateInstance(CLSID_RealTimeStylus, NULL, CLSCTX_ALL, IID_PPV_ARGS(&m_pRealTimeStylus));
	if (FAILED(hr))
	{
		return NULL;
	}

	//CoCreateInstance(CLSID_StrokeBuilder, NULL, CLSCTX_INPROC, IID_IStrokeBuilder, (VOID **)&m_pStrokeBuilder);


	// Attach RTS object to a window
	hr = m_pRealTimeStylus->put_HWND((HANDLE_PTR)m_hWnd);
	if (FAILED(hr))
	{
		m_pRealTimeStylus->Release();
		return NULL;
	}

	if (m_bUseDyniamcRenderer)
	{
		m_pDynamicRenderer = CreateDynamicRenderer(m_pRealTimeStylus);
	}

	// Create EventHandler object
	m_pSyncEventHandlerRTS = CSyncEventHandlerRTS::Create(m_pRealTimeStylus, m_pDynamicRenderer, this);
	if (m_pSyncEventHandlerRTS == NULL)
	{
		m_pRealTimeStylus->Release();
		m_pRealTimeStylus = NULL;

		m_pDynamicRenderer->Release();
		m_pDynamicRenderer = NULL;

		return FALSE;
	}


	// Enable RTS and DynamicRenderer
	if (!EnableRealTimeStylus(m_pRealTimeStylus, m_pDynamicRenderer))
	{
		m_pSyncEventHandlerRTS->Release();
		m_pSyncEventHandlerRTS = NULL;

		m_pRealTimeStylus->Release();
		m_pRealTimeStylus = NULL;

		m_pDynamicRenderer->Release();
		m_pDynamicRenderer = NULL;

		return FALSE;
	}
#endif



	return hr;
}

#ifdef STYLUS_ON
bool CWhiteBoardDlg::EnableRealTimeStylus(IRealTimeStylus* pRealTimeStylus, IDynamicRenderer* pDynamicRenderer)
{
	// Check input arguments
	if (pRealTimeStylus == NULL)
	{
		return NULL;
	}
	if (pDynamicRenderer == NULL)
	{
		//return false;
	}

	// Enable RTS object
	HRESULT hr = pRealTimeStylus->put_Enabled(TRUE);
	if (FAILED(hr))
	{
		return false;
	}


	if (pDynamicRenderer) {
		hr = pDynamicRenderer->put_Enabled(TRUE);
		if (FAILED(hr))
		{
			return false;
		}

		//Enable DynamicRenderer's auto-redraw feature
		hr = pDynamicRenderer->put_DataCacheEnabled(TRUE);
		if (FAILED(hr))
		{
			return false;
		}
	}
	return true;
}



IDynamicRenderer* CWhiteBoardDlg::CreateDynamicRenderer(IRealTimeStylus* pRealTimeStylus)
{
	if (pRealTimeStylus == NULL)
	{
		return NULL;
	}

	HWND hWnd = NULL;
	HRESULT hr = pRealTimeStylus->get_HWND((HANDLE_PTR*)&hWnd);
	if (FAILED(hr))
	{
		return NULL;
	}

	IDynamicRenderer* pDynamicRenderer = NULL;
	hr = CoCreateInstance(CLSID_DynamicRenderer, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pDynamicRenderer));
	if (FAILED(hr))
	{
		return NULL;
	}

	IStylusSyncPlugin* pSyncDynamicRenderer = NULL;
	hr = pDynamicRenderer->QueryInterface(&pSyncDynamicRenderer);
	if (FAILED(hr))
	{
		pDynamicRenderer->Release();
		return NULL;
	}


	hr = pRealTimeStylus->AddStylusSyncPlugin(
		0,                      // insert plugin at position 0 in the sync plugin list
		pSyncDynamicRenderer);  // plugin to be inserted - DynamicRenderer
	if (FAILED(hr))
	{
		ASSERT(SUCCEEDED(hr) && L"CreateDynamicRenderer: failed to add DynamicRenderer to the RealTimeStylus plugins");
		pDynamicRenderer->Release();
		pSyncDynamicRenderer->Release();
		return NULL;
	}

	// Attach DynamicRenderer to the same window RTS object is attached to
	hr = pDynamicRenderer->put_HWND((HANDLE_PTR)hWnd);
	if (FAILED(hr))
	{
		ASSERT(SUCCEEDED(hr) && L"CreateDynamicRenderer: failed to set window handle");
		pDynamicRenderer->Release();
		pSyncDynamicRenderer->Release();
		return NULL;
	}

	pSyncDynamicRenderer->Release();

	return pDynamicRenderer;
}

#endif


HRESULT CWhiteBoardDlg::UnregisterTouchInput() {
	HRESULT hr = S_OK;
	::UnregisterTouchWindow(m_hWnd);
#ifdef STYLUS_ON



	//if (m_pDynamicRenderer != nullptr) {
	//	m_pDynamicRenderer->Release();
	//	m_pDynamicRenderer = NULL;
	//}

	if (m_pRealTimeStylus != nullptr) {
		m_pRealTimeStylus->Release();
		m_pRealTimeStylus = NULL;
	}
	if (m_pSyncEventHandlerRTS != nullptr) {
		m_pSyncEventHandlerRTS->Release();
		m_pSyncEventHandlerRTS = NULL;
	}

#endif
	return hr;

}


BOOL CWhiteBoardDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	CRect rt;
	GetClientRect(&rt);
	LONG nCenter = static_cast<LONG>((rt.right - rt.left) * 0.5);
	m_videoClip.MoveWindow(static_cast<int>(nCenter - 15), rt.bottom - 800, 500, 400);
	m_videoClip.ShowWindow(SW_HIDE);
	m_videoClip.UpdateWindow();



	//::RegisterTouchWindow(m_hWnd, NULL);


	//LoadBook(L"\\book2");
	//IShellItem *pShellItemDir;
	//HRESULT hr = ::SHCreateItemInKnownFolder(FOLDERID_Documents, 0, nullptr, IID_PPV_ARGS(&pShellItemDir));
	//if (SUCCEEDED(hr)) {
	//	hr = SetCurrentLocationList(pShellItemDir, false);
	//	if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == hr)
	//	{
	//	}
	//}

	//SetCursor(theApp.LoadCursor(IDC_PEN_RM));
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CWhiteBoardDlg::CloseBook() {
	ResetBookData();
	//DiscardDeviceResources();
}

void CWhiteBoardDlg::LoadBook(std::wstring strDocumentName) {

	ResetZoomData();
	m_nCurrentPage = 0;
	m_nEmptyBoardPage = 0;


	if (g_database.GetSetting()->set_board_reversal == 1) {
		m_bReversal = TRUE;
	}
	else {
		m_bReversal = FALSE;
	}
	


	m_strDocumentName = strDocumentName;

	IShellItem *pShellItemDir;
	HRESULT hr = ::SHCreateItemInKnownFolder(FOLDERID_Documents, 0, nullptr, IID_PPV_ARGS(&pShellItemDir));
	if (SUCCEEDED(hr)) {
		hr = SetCurrentLocationList(pShellItemDir, false);
		if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == hr)
		{
		}
	}
	LoadDbData();
	GetColorInfoFromDatabase();


	if (g_database.GetSetting()->set_board_pressure == 1) {
		m_Pen->SetPressure(true);
		m_HPen->SetPressure(true);
		m_PenEmptyBoard->SetPressure(true);
		m_HPenEmptyBoard->SetPressure(true);
		//GetPen()->SetPressure(true);
	}
	else {
		m_Pen->SetPressure(false);
		m_HPen->SetPressure(false);
		m_PenEmptyBoard->SetPressure(false);
		m_HPenEmptyBoard->SetPressure(false);
		//GetPen()->SetPressure(false);
	}

	m_Pen->SetPentipStyle(static_cast<PENTIP_STYLE>(g_database.GetSetting()->set_board_penstyle));
	m_HPen->SetPentipStyle(static_cast<PENTIP_STYLE>(g_database.GetSetting()->set_board_penstyle));
	m_PenEmptyBoard->SetPentipStyle(static_cast<PENTIP_STYLE>(g_database.GetSetting()->set_board_penstyle));
	m_HPenEmptyBoard->SetPentipStyle(static_cast<PENTIP_STYLE>(g_database.GetSetting()->set_board_penstyle));


	if (m_pDynamicRenderer) {
		if (g_database.GetSetting()->set_board_pressure == 1) {
			m_bUseDyniamcRenderer = true;
		}
		else {
			m_bUseDyniamcRenderer = false;
		}
		SetDynamicRendererMode(TRUE);
	}

	//DoSpline();
}


HRESULT CWhiteBoardDlg::CreateDeviceIndependentResources()
{
	HRESULT hr = Direct2DUtility::GetD2DFactory(&m_d2dFactory);

	if (SUCCEEDED(hr))
	{
		hr = Direct2DUtility::GetDWriteFactory(&m_dWriteFactory);
	}


	//if (m_pageMode == PAGE_ITEM) {

	if (SUCCEEDED(hr))
	{
		float dashes[] = { 1.0f, 2.0f, 1.0f, 1.0f, 2.0f, 1.0f };
		// Stroke Style with custom Dash Style
		hr = m_d2dFactory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_CUSTOM,
				0.0f),
			dashes,
			ARRAYSIZE(dashes),
			&strokeStyleCustomOffsetZero);
	}

	static const wchar_t initialText[] = L"화면 영역";


	// Create animation objects
	if (SUCCEEDED(hr))
	{
		hr = AnimationUtility::GetAnimationManager(PAGE_ITEM, &m_animationManager);
	}

	if (SUCCEEDED(hr))
	{
		hr = AnimationUtility::GetTransitionLibrary(PAGE_ITEM, &m_transitionLibrary);
	}


	//}
	//else 
	//if (m_pageMode == PAGE_LIST) {

		static const wchar_t initialTextList[] = L"화면 영역";

		if (SUCCEEDED(hr))
		{
			// Create text format
			hr = m_dWriteFactory->CreateTextFormat(
				L"맑은 고딕",
				nullptr,
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				12,
				L"ko-KR",
				&m_textFormatList);


			hr = m_dWriteFactory->CreateTextFormat(
				L"맑은 고딕",
				nullptr,
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				80,
				L"ko-KR",
				&m_tfMessage);
		}

		//if (SUCCEEDED(hr))
		//{
		//	// Create an arbitary text layout object
		//	// Width and height will be determined later.
		//	hr = m_dWriteFactory->CreateTextLayout(
		//		initialTextList,
		//		ARRAYSIZE(initialTextList) - 1,
		//		m_textFormatList,
		//		50,
		//		50,
		//		&m_textLayoutList);
		//}

		//if (SUCCEEDED(hr))
		//{
		//	hr = m_textLayoutList->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
		//}

		//if (SUCCEEDED(hr))
		//{
		//	hr = m_textLayoutList->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		//}

		//if (SUCCEEDED(hr))
		//{
		//	hr = m_textLayoutList->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		//}


		if (SUCCEEDED(hr))
		{
			// Initialize animation controller with flyer animation
			hr = SharedObject<FlyerAnimation>::Create(&m_animationController);
		}

		if (SUCCEEDED(hr))
		{

			m_renderingParametersList.textLayout = m_textLayoutList;
			m_renderingParametersList.textFormat = m_textFormatList;
		}
	//}


	return hr;
}

//
//  This method creates resources which are bound to a particular Direct2D render target
//
HRESULT CWhiteBoardDlg::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (m_renderTarget == nullptr)
	{
		if (SUCCEEDED(hr))
		{
			RECT rect;
			if (::GetClientRect(m_hWnd, &rect))
			{
				D2D1_SIZE_U size = D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top);

				// Create a Direct2D render target.
				hr = m_d2dFactory->CreateHwndRenderTarget(
					D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties(m_hWnd, size),
					&m_renderTarget);
			}

			hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_bgBrush);
			hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_paintBrush);
			hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 0.4f), &m_textBrush);
			
		}

		
		//m_renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

		//if (m_pageMode == PAGE_LIST) {
			D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();

			if (SUCCEEDED(hr))
			{
				// Update thumbnail layout manager with current render target size
				m_thumbnailLayoutManager->SetBoundaryPadding(Direct2DUtility::GetRectForNoDPI(D2D1::RectF(100, 50, 100, 50)), 1);
				m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height));
			}

			if (SUCCEEDED(hr))
			{
				CreateThumbnailCells(true);

				if (SUCCEEDED(hr))
				{
					hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_solidBrushList);
				}

				// Set current render parameters
				m_renderingParametersList.solidBrush = m_solidBrushList;
				m_renderingParametersList.renderTarget = m_renderTarget;

				// Make sure rendering parameters are updated in all thumbnails if they've already been created
				//for (auto thumbnail = m_thumbnailControls.begin(); thumbnail != m_thumbnailControls.end(); thumbnail++)
				//{
				//	(*thumbnail)->SetRenderingParameters(m_renderingParametersList);
				//}

				// Start background loading of thumbnails
				hr = m_AsyncLoaderHelper->StartBackgroundLoading();
			}


			//hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, L"./images/main_menu/bg.bmp", 0, 0, &m_splashBitmap);
		//}
	}

	return hr;
}

//
// Discard any Direct2D resources which are no longer bound to a particular Direct2D render target
//
HRESULT CWhiteBoardDlg::DiscardDeviceResources()
{
	for (auto thumbnail = m_thumbnailControls.begin(); thumbnail != m_thumbnailControls.end(); thumbnail++)
	{
		(*thumbnail)->DiscardResources();
	}


	for (auto emptyThumbnail = m_emptyBoard.begin(); emptyThumbnail != m_emptyBoard.end(); emptyThumbnail++)
	{
		(*emptyThumbnail)->DiscardResources();
	}


	HRESULT hr = RemoveAllItems();
	if (SUCCEEDED(hr))
	{
		hr = m_AsyncLoaderHelper->Shutdown();
	}


	if (m_mediaPlayer)
	{
		m_mediaPlayer->Shutdown();
		m_mediaPlayer = nullptr;
	}

	// Discard local Direct2D resources
	//m_foregroundGradientBrushLeft = nullptr;
	//m_foregroundGradientBrushRight = nullptr;
	//m_solidBrush = nullptr;
	//m_renderTarget = nullptr;
	//m_bgBrush = nullptr;
	//m_solidBrushList = nullptr;
	//m_renderTarget = nullptr;
	m_renderingParametersList.renderTarget = nullptr;
	m_renderingParametersList.solidBrush = nullptr;
	m_renderingParametersList.textFormat = nullptr;
	m_renderingParametersList.textLayout = nullptr;

	return S_OK;
}




void CWhiteBoardDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	HRESULT hr = S_OK;


	if (m_renderTarget)
	{
		// Capture previous size
		//m_previousRenderTargetSize = m_renderTarget->GetSize();

		// 화면의 사이즈 cx, cy m_renderTarget->GetSize() 적용
		// 화면의 사이즈는 실제 사이즈이고 m_renderTarget 의 사이즈는 DPI 적용된 사이즈
		// 화면 사이즈를 바로 적용하면 dpi 사이즈가 적용된 m_renderTarget 사이즈와 다름
		// 화면 사이즈를 m_renderTarget 사이즈로 변환 후 사용
		m_renderTarget->Resize(D2D1::SizeU(cx, cy));
		D2D1_SIZE_F rederTargetSize = m_renderTarget->GetSize();
		m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, rederTargetSize.width, rederTargetSize.height));

		if (m_pageMode == PAGE_LIST) {
			SetupAnimationList(MoveAround, m_renderTarget->GetSize());
		}
		else if (m_pageMode == PAGE_ITEM) {
			m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);
			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else if (m_pageMode == PAGE_EMPTYBOARD) {
			//m_thumbnailLayoutManager->SetVisibleIndex(m_emptyBoard, m_nEmptyBoardPage);
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
	}

}


//TODO : OnPaint
void CWhiteBoardDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	HRESULT hr = CreateDeviceResources();

	if (m_pageMode == PAGE_LIST) {
		if (m_bIsListAnimation) {
			hr = AnimationUtility::UpdateAnimationManagerTime(PAGE_LIST);
			OnPaintList();

			bool isBusy = false;
			AnimationUtility::IsAnimationManagerBusy(PAGE_LIST, &isBusy);
			if (!isBusy)
			{
				m_bIsListAnimation = false;
				if (m_renderTarget->GetSize().width != m_lastAnimationSize.width ||
					m_renderTarget->GetSize().height != m_lastAnimationSize.height)
				{ 
					SetupAnimationList(MoveAround, m_renderTarget->GetSize());
				}
			
			}
			else {
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else {
			OnPaintList();
		}
	}
	else if (m_pageMode == PAGE_ITEM) {
		if (m_bIsModeChangeAnimation || m_bIsSlideAnimation || m_bIsZoomAnimation) {
			hr = AnimationUtility::UpdateAnimationManagerTime(PAGE_ITEM);

			bool isBusy = false;
			AnimationUtility::IsAnimationManagerBusy(PAGE_ITEM, &isBusy);
			if (!isBusy)
			{
				if (/*m_bIsModeChangeAnimation || */m_bIsSlideAnimation) {
					m_AsyncLoaderHelper->StartFullImageLoad(AsyncLoaderSpace::FullSize, m_nCurrentPage);
				}


				//if (m_bIsZoomAnimation) {
					//if (m_currentZoom > 2.0f) {
					//	m_AsyncLoaderHelper->StartFullImageLoad(AsyncLoaderSpace::XxhdpiSize, m_nCurrentPage);
					//}
				//}

				m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);
				if (m_bIsZoomAnimation) {
					m_bIsZoomAnimation = false;
					if (m_currentZoom != 1.0f && m_currentZoom < 1.1f) {
						Sleep(300);
						m_zoomDuration = 1.0f;
						DoReduce();
					}
				}

				m_bIsModeChangeAnimation = false;
				m_bIsSlideAnimation = false;
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
			else {
				if (m_bIsModeChangeAnimation) OnPaintModeChangeAnimation();
				if (m_bIsSlideAnimation) OnPaintSlideAnimation();
				if (m_bIsZoomAnimation) OnPaintZoomAnimation();
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else {
			OnPaintItem();
		}

#ifdef STYLUS_ON
		if (GetOperationType() != ImageOperationTypeEraser) {
			//m_pDynamicRenderer->Refresh();
		}
#endif

	}
	else if (m_pageMode == PAGE_EMPTYBOARD) {
		//if (m_bIsEmptyBoardAnimation || m_bIsZoomAnimation) {
		//	hr = AnimationUtility::UpdateAnimationManagerTime(PAGE_ITEM);

		//	bool isBusy = false;
		//	AnimationUtility::IsAnimationManagerBusy(PAGE_ITEM, &isBusy);
		//	if (!isBusy)
		//	{
		//		m_bIsEmptyBoardAnimation = false;
		//		m_bIsZoomAnimation = false;
		//		::InvalidateRect(m_hWnd, nullptr, FALSE);
		//	}
		//	else {
		//		if (m_bIsZoomAnimation) OnPaintEmptyBoardAnimation();
		//		if (m_bIsEmptyBoardAnimation) OnPaintEmptyBoardAnimation();
		//		::InvalidateRect(m_hWnd, nullptr, FALSE);
		//	}
		//}
		//else {
			OnPaintEmptyBoard();
		//}
		
	}
	

	if (m_mediaPlayer)
	{
		m_mediaPlayer->UpdateVideo();
	}

}




void CWhiteBoardDlg::OnPaintList() {
	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		m_renderTarget->BeginDraw();
		m_renderTarget->Clear(D2D1::ColorF(BackgroundColor));
		//m_renderTarget->FillRectangle(D2D1::RectF(0, 0, m_renderTarget->GetSize().width, m_renderTarget->GetSize().height), m_bgBrush);
		m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		if (m_bIsListAnimation)
		{
			DrawAnimatedThumbnailCells();
		}
		else
		{
			int currentIndex;
			int endIndex;
			int nCurrentPageStartIndex;
			int nCurrentPageEndIndex;
			hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);
			if (SUCCEEDED(hr))
			{
				while (currentIndex < endIndex)
				{
					m_thumbnailControls[currentIndex]->Draw();
					currentIndex++;
				}
			}
		}


	}

	hr = m_renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}
}


void CWhiteBoardDlg::OnPaintItem() {

	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{


		m_renderTarget->BeginDraw();
		m_renderTarget->Clear(D2D1::ColorF(BackgroundColor));


		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(m_currentZoom, m_currentZoom, GetCenter());
		D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(m_currentPanPoint.x, m_currentPanPoint.y);
		m_renderTarget->SetTransform(scale * translation);

		if (m_nCurrentPage < static_cast<UINT>(m_thumbnailControls.size())) {
			m_thumbnailControls[m_nCurrentPage]->DrawDynamicLoad(IsItemEditMode());
			if (GetOperationType() == ImageOperationTypeZoom) {
				OnPaintItemDrawZoomBox();
			}
			else if (GetOperationType() == ImageOperationTypeClipping) {
				OnPaintItemDrawClippingBox();
			}
		}
	}

	hr = m_renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}
	

}


void CWhiteBoardDlg::OnPaintItemDrawZoomBox() {
	if (m_bIsZoomming) {
		D2D1_RECT_F imageRect;
		m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);

		D2D1_COLOR_F savedColor = m_paintBrush->GetColor();
		float savedOpacity = m_paintBrush->GetOpacity();
		float lineWidth = 1.0f / m_currentZoom;
		m_paintBrush->SetColor(D2D1::ColorF(D2D1::ColorF::YellowGreen));
		m_paintBrush->SetOpacity(0.10f);

		D2D1_RECT_F selectionRect = Direct2DUtility::FixRect(m_zoomBoxDrawing);
		// Wash out unneeded areas
		m_renderTarget->FillRectangle(D2D1::RectF(imageRect.left, imageRect.top, imageRect.right, selectionRect.top), m_paintBrush);
		m_renderTarget->FillRectangle(D2D1::RectF(imageRect.left, selectionRect.top, selectionRect.left, selectionRect.bottom), m_paintBrush);
		m_renderTarget->FillRectangle(D2D1::RectF(selectionRect.right, selectionRect.top, imageRect.right, selectionRect.bottom), m_paintBrush);
		m_renderTarget->FillRectangle(D2D1::RectF(imageRect.left, selectionRect.bottom, imageRect.right, imageRect.bottom), m_paintBrush);


		m_paintBrush->SetOpacity(1);
		m_paintBrush->SetColor(D2D1::ColorF(D2D1::ColorF::YellowGreen));
			// The boundary box of the clipping rectangle
		m_renderTarget->DrawRectangle(m_zoomBoxDrawing, m_paintBrush, lineWidth, strokeStyleCustomOffsetZero);
		m_paintBrush->SetColor(savedColor);
		m_paintBrush->SetOpacity(savedOpacity);
	}

}


void CWhiteBoardDlg::OnPaintItemDrawClippingBox() {
	if (m_bIsClipping) {
		D2D1_RECT_F imageRect;
		m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);

		D2D1_COLOR_F savedColor = m_paintBrush->GetColor();
		float savedOpacity = m_paintBrush->GetOpacity();
		float lineWidth = 1.0f / m_currentZoom;
		m_paintBrush->SetColor(D2D1::ColorF(D2D1::ColorF::HotPink));
		m_paintBrush->SetOpacity(0.10f);

		D2D1_RECT_F selectionRect = Direct2DUtility::FixRect(m_clippingBoxDrawing);
		// Wash out unneeded areas
		m_renderTarget->FillRectangle(D2D1::RectF(imageRect.left, imageRect.top, imageRect.right, selectionRect.top), m_paintBrush);
		m_renderTarget->FillRectangle(D2D1::RectF(imageRect.left, selectionRect.top, selectionRect.left, selectionRect.bottom), m_paintBrush);
		m_renderTarget->FillRectangle(D2D1::RectF(selectionRect.right, selectionRect.top, imageRect.right, selectionRect.bottom), m_paintBrush);
		m_renderTarget->FillRectangle(D2D1::RectF(imageRect.left, selectionRect.bottom, imageRect.right, imageRect.bottom), m_paintBrush);


		m_paintBrush->SetOpacity(1);
		m_paintBrush->SetColor(D2D1::ColorF(D2D1::ColorF::HotPink));
		// The boundary box of the clipping rectangle
		m_renderTarget->DrawRectangle(m_clippingBoxDrawing, m_paintBrush, lineWidth, strokeStyleCustomOffsetZero);
		m_paintBrush->SetColor(savedColor);
		m_paintBrush->SetOpacity(savedOpacity);
	}

}

HRESULT CWhiteBoardDlg::OnPaintModeChangeAnimation() {
	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		m_renderTarget->BeginDraw();
		m_renderTarget->Clear(D2D1::ColorF(BackgroundColor));
		//m_renderTarget->FillRectangle(D2D1::RectF(0, 0, m_renderTarget->GetSize().width, m_renderTarget->GetSize().height), m_bgBrush);
		//m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		//D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(1, 1, GetCenter());
		//D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(0, 0);
		//m_renderTarget->SetTransform(scale * translation);

		if (m_bIsModeChangeAnimation)
		{

			D2D1_RECT_F rect;

			DOUBLE left = 0;
			m_varRectLeft->GetValue(&left);
			DOUBLE top = 0;
			m_varRectTop->GetValue(&top);
			DOUBLE right = 0;
			m_varRectRight->GetValue(&right);
			DOUBLE bottom = 0;
			m_varRectBottom->GetValue(&bottom);

			rect.left = static_cast<FLOAT>(left);
			rect.top = static_cast<FLOAT>(top);
			rect.right = static_cast<FLOAT>(right);
			rect.bottom = static_cast<FLOAT>(bottom);
			m_thumbnailControls[m_nCurrentPage]->SetDrawingRect(rect);
			m_thumbnailControls[m_nCurrentPage]->DrawDynamicLoad(IsItemEditMode());
		}
	}

	hr = m_renderTarget->EndDraw();

	if (m_bIsBackgroundLoading) {
		m_bIsBackgroundLoading = false;
		//m_AsyncLoaderHelper->StartFullImageLoad(m_nCurrentPage);
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}
	return hr;
}




HRESULT CWhiteBoardDlg::OnPaintSlideAnimation() {

	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		m_renderTarget->BeginDraw();
		m_renderTarget->Clear(D2D1::ColorF(BackgroundColor));
		//m_renderTarget->FillRectangle(D2D1::RectF(0, 0, m_renderTarget->GetSize().width, m_renderTarget->GetSize().height), m_bgBrush);
		m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		if (m_bIsSlideAnimation)
		{
			D2D1_RECT_F rect;
			m_thumbnailControls[m_nAnimatePage]->GetDrawingRect(&rect);

			FLOAT alphaCurrent;
			FLOAT alphaAnimate;
			D2D1_POINT_2F point;
			if (SUCCEEDED(m_thumbnailControls[m_nAnimatePage]->GetAnimationPoint(&point, &alphaAnimate)))
			{
				alphaCurrent = 1.0f - alphaAnimate;
				m_renderTarget->SetTransform(D2D1::Matrix3x2F::Translation(point.x - rect.left, point.y - rect.top));
				m_thumbnailControls[m_nAnimatePage]->SetAlpha(alphaAnimate);
				m_thumbnailControls[m_nCurrentPage]->SetAlpha(alphaCurrent);
				m_thumbnailControls[m_nAnimatePage]->DrawDynamicLoad(IsItemEditMode());
				m_thumbnailControls[m_nCurrentPage]->DrawDynamicLoad(IsItemEditMode());
			}
		}
		if (m_nCurrentPage == 0) {
			//OnPaintTextMessage();
		}
	}
	hr = m_renderTarget->EndDraw();


	if (m_bIsBackgroundLoading) {
		m_bIsBackgroundLoading = false;
		//m_AsyncLoaderHelper->StartFullImageLoad(m_nCurrentPage);
	}


	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}
	return hr;
}


void CWhiteBoardDlg::OnPaintTextMessage() {
	HRESULT hr = S_OK;

	if (m_tlMessage) {
		
		//D2D1_POINT_2F pt = D2D1::Point2F(m_renderTarget->GetSize().width/2, m_renderTarget->GetSize().height/2);
		D2D1_POINT_2F pt = D2D1::Point2F(0, 0);
		m_renderTarget->DrawTextLayout(
			pt,
			m_tlMessage,
			m_textBrush,
			D2D1_DRAW_TEXT_OPTIONS_CLIP);
	}
}


HRESULT CWhiteBoardDlg::OnPaintZoomAnimation() {

	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		m_renderTarget->BeginDraw();
		m_renderTarget->Clear(D2D1::ColorF(BackgroundColor));

		D2D1_MATRIX_3X2_F originalTransform;
		m_renderTarget->GetTransform(&originalTransform);

		DOUBLE valueX = 0;
		m_varAniTransX->GetValue(&valueX);
		DOUBLE valueY = 0;
		m_varAniTransY->GetValue(&valueY);
		DOUBLE fZoom = 0.0f;
		m_varAniZoom->GetValue(&fZoom);

		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(fZoom, fZoom, GetCenter());
		D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(valueX, valueY);
		m_renderTarget->SetTransform(scale * translation);

		if (m_bIsZoomAnimation)
		{
			m_thumbnailControls[m_nCurrentPage]->DrawDynamicLoad(IsItemEditMode());
		}

		m_renderTarget->SetTransform(originalTransform);
	}

	hr = m_renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}
	return hr;
}


HRESULT CWhiteBoardDlg::OnPaintEmptyBoardAnimation() {

	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		m_renderTarget->BeginDraw();
		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(m_emptyBoardZoom, m_emptyBoardZoom, GetCenter());
		D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(m_emptyBoardPanPoint.x, m_emptyBoardPanPoint.y);
		m_renderTarget->SetTransform(scale * translation);

		D2D1_RECT_F rect;
		DOUBLE alpha = 0;
		if (m_varAniAlpha) m_varAniAlpha->GetValue(&alpha);
		m_emptyBoard[m_nEmptyBoardPage]->DrawEmptyBoard(IsItemEditMode());

	}

	hr = m_renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}
	return hr;
}




void CWhiteBoardDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	CDialogEx::OnRButtonDown(nFlags, point);
}


//BOOL CWhiteBoardDlg::OnTouchInput(CPoint pt, int nInputNumber, int nInputsCount, PTOUCHINPUT pInput) {
//
//	TRACE(_T("\n>>>>>>>> touch <<<<<<<<<<\n"));
//
//	if (pInput->dwFlags & TOUCHEVENTF_DOWN) {
//	}
//
//	//if ((pInput->dwFlags & TOUCHEVENTF_UP) && (pInput->dwID != MOUSE_CURSOR_ID))
//	//{
//	//	//m_toolDlg.Invalidate();
//	//}
//
//	return TRUE;
//}


void CWhiteBoardDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	PostMessage(MY_WM_BEGINDRAW, 0, 0);

	if (IsOperationTypePen()) return;

	//return;
	// Capture the mouse cursor
	SetCapture();
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);

	InputProcessDown(mousePosition);

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CWhiteBoardDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	//if (m_bIsDrawing) {
	//	PostMessage(MY_WM_BEGINDRAW, 0, 0);
	//}

	if (IsOperationTypePen()) return;
	//return;
	HRESULT hr = S_OK;
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);
	::ReleaseCapture();

	InputProcessUp(mousePosition);

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CWhiteBoardDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	//if (nFlags == 1) return;

	if (m_bIsDrawing) {
		PostMessage(MY_WM_BEGINDRAW, 0, 0);
		//OnBeginDraw(0, 0);
	}



	if (IsOperationTypePen()) return;
	//return;
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);
	//UpdateMouseCursor(mousePosition);
	InputProcessMove(mousePosition);

	CDialogEx::OnMouseMove(nFlags, point);
}


void CWhiteBoardDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (!IsWindowVisible()) {
		return;
	}
	/*
	// key map 
	// 일반 모드, 필기 모드 
	1. 이전 페이지, 펜 종류 - A
	2. 다음 페이지, 펜 색상 - D
	3. 확대 축소, 이동 모드 - W
	4. 영역 확대, 선지우개 - X
	5. 필기 모드, 화면 모드 전환 - I
	6. 5번과 동일
	7. 반전 모드 - ctrl + r
	8. 전체페이지 - ctrl + a
	9. 잉크보드 켜기 - ctrl + n
	10. 영역이미지 캡쳐 - ctrl + t
	11. 녹화 - ctrl + g
	12. 빈칸(가리개) 열기 - ctrl + b
	13. 종료 - Ctrl + x
	*/


	if (0x8000 == (GetKeyState(VK_CONTROL) & 0x8000)) {
		if (nChar == 'R' || nChar == 'r') {
			if (GetPageMode() != PAGE_LIST) {
				m_bReversal ^= 1;

				DoReversal();
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}

		if (nChar == 'A' || nChar == 'a') {
			((CAbleClassDlg*)GetParent())->SetListMode();
			((CAbleClassDlg*)GetParent())->ResetFocus();
		}

		if (nChar == 'N' || nChar == 'n') {
			((CAbleClassDlg*)GetParent())->SetEmptyBoard();
			((CAbleClassDlg*)GetParent())->ResetFocus();
		}

		if (nChar == 'T' || nChar == 't') {
			((CAbleClassDlg*)GetParent())->SetBoardClipping();
			((CAbleClassDlg*)GetParent())->ResetFocus();
		}

		if (nChar == 'G' || nChar == 'g') {
			((CAbleClassDlg*)GetParent())->SetBoardRecord();
			((CAbleClassDlg*)GetParent())->ResetFocus();
		}

		if (nChar == 'B' || nChar == 'b') {
			((CAbleClassDlg*)GetParent())->SetBoardBlank();
			((CAbleClassDlg*)GetParent())->ResetFocus();
		}


		if (nChar == 'Q' || nChar == 'q') {
			((CAbleClassDlg*)GetParent())->SetBoardClose();
			((CAbleClassDlg*)GetParent())->ResetFocus();
		}
	}
	else {
		if (nChar == 'A' || nChar == 'a') {
			if (IsItemEditMode()) {
				if (GetOperationType() == ImageOperationTypePen) {
					SetPenToggle();
				}
				((CAbleClassDlg*)GetParent())->SetPenType(GetPenType());

			}
			else {
				// 기존 페이지 지동
				PreviousPage();
				// 임시로 반전으로 
				//if (GetPageMode() != PAGE_LIST) {
				//	m_bReversal ^= 1;

				//	DoReversal();


				//	::InvalidateRect(m_hWnd, nullptr, FALSE);
				//}
			}

		}
		if (nChar == 'D' || nChar == 'd') {
			if (IsItemEditMode()) {
				//TRACE(L"\n>>> %d <<<", m_nPenColorIndex);
				//m_nPenColorIndex = m_nPenColorIndex % 6;
				GetPen()->SetColor(NextPenColor());
				((CAbleClassDlg*)GetParent())->SetPenColor(GetPen()->GetColorIndex());
				//SetPenColor(m_nPenColorIndex+1);
			}
			else {
				NextPage();
			}
		}
		if (nChar == 'W' || nChar == 'w') {
			if (IsItemEditMode()) {
				DoMove();
			}
			else {
				DoReduce();
			}
		}
		if (nChar == 'X' || nChar == 'x') {
			if (IsItemEditMode()) {
				((CAbleClassDlg*)GetParent())->SetBoardEraser();
			}
			else {
				if (GetOperationType() != ImageOperationTypeZoom) {
					//DoZoom();
					((CAbleClassDlg*)GetParent())->SetBoardZoom();
				}
				else {
					SetOperationType(m_prevOperationType);
					if (m_prevOperationType == ImageOperationTypeZoom) {
						DoView();
					}
					else {
						::InvalidateRect(m_hWnd, nullptr, FALSE);
					}


				}
			}
		}
		if (nChar == 'I' || nChar == 'i') {
			if (IsItemEditMode()) {
				//DoView();
				//((CAbleClassDlg*)GetParent())->SetWhiteBoardViewChange();
				((CAbleClassDlg*)GetParent())->SetBoardNon();
			}
			else {
				//DoPen();
				//((CAbleClassDlg*)GetParent())->SetWhiteBoardViewChange();
				((CAbleClassDlg*)GetParent())->SetBoardPen();
			}
		}
	}

	

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CWhiteBoardDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}

//
// Navigate to the previous image
//
bool CWhiteBoardDlg::PreviousPage()
{
	if (GetCurrentPage() == 0) {
		return true;
	}

	bool isPrevImage = false;
	if (m_pageMode == PAGE_ITEM) {


		if (m_nCurrentPage < 1) {
			//return isPrevImage;
		}

		if (m_bIsSlideAnimation || m_bIsModeChangeAnimation || m_bIsZoomAnimation) {
			AbandonAnimation();
			m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);
			
		}



		ResetZoomData();
		if (m_nCurrentPage > 0) {
			//m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);
			m_nAnimatePage = m_nCurrentPage--;
		}

		PrintPageNumber();



		//m_AsyncLoaderHelper->StartFullImageLoad(m_nCurrentPage);
		m_bIsBackgroundLoading = true;
		m_bIsSlideAnimation = true;
		m_thumbnailLayoutManager->SetSlideAnimation(m_thumbnailControls, SlidePrevious);
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	else if (m_pageMode == PAGE_EMPTYBOARD) {
		
		if (m_nEmptyBoardPage > 0) m_nEmptyBoardPage--;
		PrintPageNumber();
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	return isPrevImage;
}



//
// Navigate to the next image
//
bool CWhiteBoardDlg::NextPage()
{
	ShowMessageAnimation(L"fdsfsdfjdsklfj dsljfldsjfld");

	bool isNextImage = false;
	if (m_pageMode == PAGE_ITEM) {
		if (m_nCurrentPage == m_thumbnailControls.size() - 1) {
			return true;
		}


		if (m_bIsSlideAnimation || m_bIsModeChangeAnimation || m_bIsZoomAnimation) {
			AbandonAnimation();
			m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);

		}

		ResetZoomData();

		if (m_nCurrentPage < m_thumbnailControls.size()-1) {
			//m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);
			m_nAnimatePage = m_nCurrentPage++;
		}


		PrintPageNumber();



		//m_AsyncLoaderHelper->StartFullImageLoad(m_nCurrentPage);
		m_bIsSlideAnimation = true;
		m_bIsBackgroundLoading = true;
		m_thumbnailLayoutManager->SetSlideAnimation(m_thumbnailControls, SlideNext);
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	else if (m_pageMode == PAGE_EMPTYBOARD) {

		if (m_nEmptyBoardPage < m_emptyBoard.size() - 1) {
			m_nEmptyBoardPage++;
		} else {
			AddEmptyBoardItem();
			m_nEmptyBoardPage = m_emptyBoard.size() - 1;
		}

		PrintPageNumber();
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	
	return isNextImage;
}






void CWhiteBoardDlg::UpdateMouseCursor(D2D1_POINT_2F mousePosition)
{
	bool isHit = false;// IsImageHit(mousePosition);

}

BOOL CWhiteBoardDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return FALSE;
}

BOOL CWhiteBoardDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE)) return TRUE;

	if (pMsg->wParam == VK_LEFT)
	{

	}
	else if (pMsg->wParam == VK_RIGHT) {
	}
	else if (pMsg->wParam == VK_UP) {
	}
	else if (pMsg->wParam == VK_DOWN) {
	}
	else {
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CWhiteBoardDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_pageMode == PAGE_ITEM || m_pageMode == PAGE_EMPTYBOARD)
	{
		if (zDelta > 100) {
			ZoomIn();


		}
		else {
			ZoomOut();
		}
	}
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}




void CWhiteBoardDlg::ZoomIn() {
	if (GetPageMode() == PAGE_EMPTYBOARD)
		return;

	if (m_bIsZoomAnimation) {
		DOUBLE fZoom;
		m_varAniZoom->GetValue(&fZoom);
		m_currentZoom = fZoom;
		//m_zoomDuration = 0.4f;
		//m_bIsZoomAnimation = false;
		//AnimationUtility::AbandonAllStoryboards(PAGE_ITEM);
		//return;
		AbandonAnimation();
	}

	if (m_currentZoom < m_zoomMax)
	{
		m_beforeZoom = m_currentZoom;
		m_currentZoom = std::min<float>(m_zoomMax, m_currentZoom * (1.0f + m_zoomStep));
	}

	SetZoomAnimation();
	::InvalidateRect(m_hWnd, nullptr, FALSE);
	
}


void CWhiteBoardDlg::ZoomOut() {
	if (GetPageMode() == PAGE_EMPTYBOARD)
		return;

	if (m_bIsZoomAnimation) {
		DOUBLE fZoom;
		m_varAniZoom->GetValue(&fZoom);
		//m_beforeZoom = fZoom;
		m_currentZoom = fZoom;
		//m_zoomDuration = 0.4f;
		//m_bIsZoomAnimation = false;
		AbandonAnimation();
		//AnimationUtility::AbandonAllStoryboards(PAGE_ITEM);

		//return;
	}

	if (m_currentZoom > m_zoomMin)
	{
		m_beforeZoom = m_currentZoom;
		m_currentZoom = std::max<float>(m_zoomMin, m_currentZoom * (1.0f-m_zoomStep));
	}

	SetZoomAnimation();
	::InvalidateRect(m_hWnd, nullptr, FALSE);
	
}


float CWhiteBoardDlg::GetZoomming(FLOAT width, FLOAT height) {
	float fZoomValue = 0.0f;

	D2D1_RECT_F rect;
	HRESULT hr = m_thumbnailControls[m_nCurrentPage]->GetClipRect(&rect);		// 실제 이미지 크기

	D2D1_RECT_F imageRect;
	m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);	// 화면상 이미지 크기 (축소 또는 확대값 반영)

	float imageWidth = (imageRect.right - imageRect.left)/* * m_currentZoom*/;
	float imageHeight = (imageRect.bottom - imageRect.top)/* * m_currentZoom*/;
	float clientWidth = m_renderTarget->GetSize().width;								// 모니터 화면 출력 크기
	float clientHeight = m_renderTarget->GetSize().height;



	float fZoomedHeight = width * (clientHeight / clientWidth);


	if (height < fZoomedHeight) {
		fZoomValue = Direct2DUtility::GetRectWidth(rect) / width;
		if (clientWidth > imageWidth) {
			fZoomValue *= (clientWidth / imageWidth);
		}
		else {
			fZoomValue += (imageWidth / clientWidth);
		}
	}
	else {
		fZoomValue = Direct2DUtility::GetRectHeight(rect) / height;
		if (clientHeight > imageHeight) {
			fZoomValue *= (clientHeight / imageHeight);
		}
		else {
			fZoomValue *= (imageHeight / clientHeight);
		}
	}

	return fZoomValue;
}

bool CWhiteBoardDlg::CheckOutOfBoundary() {


	D2D1_RECT_F imageRect;
	m_thumbnailControls[m_nCurrentPage]->GetDrawingRect(&imageRect);

	D2D1::Matrix3x2F originalTransform;
	m_renderTarget->GetTransform(&originalTransform);
	D2D1_POINT_2F transPointLeftTop = originalTransform.TransformPoint(D2D1::Point2F(imageRect.left, imageRect.top));
	D2D1_POINT_2F transPointRightBottom = originalTransform.TransformPoint(D2D1::Point2F(imageRect.right, imageRect.bottom));
	
/*

	D2D1_RECT_F DrawingRect;
	DrawingRect.left = DrawingRect.left */

	//>> trans(616.00), (384.00) <<<<<<<
	//	c:\job\code\ableclassbind\ableclass\cwhiteboarddlg.cpp(1608) : atlTraceGeneral -
	//	>> trans(484.65), (342.00) <<<<<<<
	//	c : \job\code\ableclassbind\ableclass\cwhiteboarddlg.cpp(1608) : atlTraceGeneral -
	//	>> trans(1051.35), (391.20) <<<<<<<
	//TRACE(_T("\n>>trans(%.02f),(%.02f) - image (%.02f) (%.02f) <<<<<<<\n"),
	//	transPoint.x, transPoint.y, imageRect.left, imageRect.top
	//);

	//float transX;
	//float transY;

	//D2D1_POINT_2F upperLeft = transform.TransformPoint(D2D1::Point2F(, rect.top));

	//D2D1_RECT_F imageRect;
	//m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);

	//float imageWidth = (imageRect.right - imageRect.left) * m_currentZoom;
	//float imageHeight = (imageRect.bottom - imageRect.top) * m_currentZoom;

	//float clientWidth = m_renderTarget->GetSize().width;
	//float clientHeight = m_renderTarget->GetSize().height;

	//float boundaryLeft = (clientWidth / 2) - (imageWidth / 2);
	//float buondaryRight = boundaryLeft * -1;
	//float boundaryTop = (clientHeight / 2) - (imageHeight / 2);
	//float boundaryBottom = boundaryTop * -1;

	//if (rect.left > renderTargetSize.width || rect.right < 0 || rect.top > renderTargetSize.height || rect.bottom < 0) {
	//	return true;
	//}
	return false;
}



void CWhiteBoardDlg::DoItemMode(int page) {
	DoModeChange(PAGE_ITEM, page);

	

	//D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();
	//m_renderTarget->Clear(D2D1::ColorF(0x000000));
	//D2D1_RECT_F rect = { 0, 0, renderTargetSize.width, renderTargetSize.height };
	//D2D1_RECT_F bitmapRect = D2D1::RectF(0, 0, m_splashBitmap->GetSize().width, m_splashBitmap->GetSize().height);
	//m_renderTarget->DrawBitmap(m_splashBitmap, rect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, bitmapRect);


	int nLoopCount = 0;
	while (!m_AsyncLoaderHelper->CheckListLoadDone()) {
		Sleep(100);
		if (nLoopCount++ > 50) {
			//TRACE(_T("\n>>>>> time over <<<<<<<<\n"));
			break;		// 최대 대기 시간 4 sec
		}
	}

	SetModeChangeAnimation();
}


void CWhiteBoardDlg::DoListMode() {
	ResetZoomData();
	SetOperationType(ImageOperationTypeNone);

	DoModeChange(PAGE_LIST, m_nCurrentPage);
	if (m_bFirstListLoading) {
		D2D1_SIZE_F rederTargetSize = m_renderTarget->GetSize();
		m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, rederTargetSize.width, rederTargetSize.height));
		SetupAnimationList(MoveAround, m_renderTarget->GetSize());
		//TRACE(_T("\n>>>>>>> animaition pan (%.02f), (%.02f) <<<<<<<<<<<<\n "), rederTargetSize.width, rederTargetSize.height);
	}
}


void CWhiteBoardDlg::DoClipping() {
	if (m_operationType == ImageOperationTypeClipping) {
		SetOperationType(m_prevOperationType);
	}
	else {
		SetOperationType(ImageOperationTypeClipping);
	}
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CWhiteBoardDlg::DoPen() {
	SetItemMode(ITEM_EDIT);
	m_prevOperationType = ImageOperationTypePen;
	SetOperationType(ImageOperationTypePen);
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CWhiteBoardDlg::DoView() {
	SetItemMode(ITEM_VIEW);
	m_prevOperationType = ImageOperationTypeNone;
	SetOperationType(ImageOperationTypeNone);
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CWhiteBoardDlg::DoMove() {
	m_prevOperationType = ImageOperationTypeNone;
	SetOperationType(ImageOperationTypeNone);
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}
void CWhiteBoardDlg::DoZoom() {
	if (m_operationType == ImageOperationTypeZoom) {
		SetOperationType(m_prevOperationType);
	}
	else {
		SetOperationType(ImageOperationTypeZoom);
	}
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CWhiteBoardDlg::DoReduce() {
	m_currentZoom = 1.0f;
	m_currentTransX = 0;
	m_currentTransY = 0;
	m_currentPanPoint.x = 0;
	m_currentPanPoint.y = 0;

	if (m_bIsSlideAnimation || m_bIsModeChangeAnimation || m_bIsZoomAnimation) {
		AbandonAnimation();
	}

	SetZoomAnimation();
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}


void CWhiteBoardDlg::DoEraser() {
	//m_prevOperationType = ImageOperationTypeEraser;
	SetOperationType(ImageOperationTypeEraser);
#ifdef _DEBUG
	//::InvalidateRect(m_hWnd, nullptr, FALSE);
#endif
}

void CWhiteBoardDlg::DoClear() {
	//ComPtr<IImageOperation> clearOperation;
	//HRESULT hr = SharedObject<DrawGeometryOperation>::Create(&clearOperation);
	//ComPtr<IDrawGeometryOperation> drawGeometry;
	//if (SUCCEEDED(clearOperation.QueryInterface(&drawGeometry)))
	//{
	//	drawGeometry->SetClearOperation(true);
	//	GetCurrentThumbnailPtr()->PushImageOperation(clearOperation);
	//}

	GetCurrentThumbnailPtr()->ClearOperation();

#ifdef STYLUS_ON
	//for (int i = 0; i < 1000; i++)
	//	m_pDynamicRenderer->ReleaseCachedData(i);

#endif

	::InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CWhiteBoardDlg::DoUndo() {
	
	GetCurrentThumbnailPtr()->UndoImageOperation();
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}


void CWhiteBoardDlg::DoBlank() {
	if (m_bEditorBlankVisibled) {
		m_bEditorBlankVisibled = false;
	}
	else {
		m_bEditorBlankVisibled = true;
	}
	GetCurrentThumbnailPtr()->BlankEditorOperation(m_bEditorBlankVisibled);
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}


void CWhiteBoardDlg::DoEmptyBoardMode() {
	if (m_pageMode == PAGE_EMPTYBOARD) {
		//((CAbleClassDlg*)GetParent())->SetBoardMenuButtonStyle(IDC_BTN_BOARD, FALSE);

		DoModeChange(PAGE_ITEM, m_nCurrentPage);
		//SetModeChangeAnimation();
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	else {
		//((CAbleClassDlg*)GetParent())->SetBoardMenuButtonStyle(IDC_BTN_BOARD, TRUE);
		//ResetZoomData();
		if (m_emptyBoard.size() == 0) {
			AddEmptyBoardItem();
			m_nEmptyBoardPage = 0;
		}
		//m_itemMode = ITEM_EDIT;
		//SetOperationType(ImageOperationTypePen);
		DoModeChange(PAGE_EMPTYBOARD, 0);
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
}



void CWhiteBoardDlg::CreateThumbnailCells(bool resetPanPosition)
{
	//m_thumbnailLayoutManager->SetTextBoxHeight(m_textLayout->GetMaxHeight());
	m_thumbnailLayoutManager->CreateLayout(static_cast<int>(m_thumbnailControls.size()), resetPanPosition);
}







HRESULT CWhiteBoardDlg::SetCurrentLocationList(IShellItem* shellFolder, bool recursive)
{
	HRESULT hr = S_OK;

	// Capture currently visible thumbnails for animation of previous thumbnails
	//std::vector<ThumbnailCell> previousThumbnails;

	//if (m_previousRenderTargetSize.width > 0 || m_previousRenderTargetSize.height > 0)
	{
		//m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, m_previousRenderTargetSize.width, m_previousRenderTargetSize.height));
		m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, m_renderTarget->GetSize().width, m_renderTarget->GetSize().height));
		hr = m_thumbnailLayoutManager->UpdateVisibleThumbnailControls(m_thumbnailControls);
		
	}
	//else
	//{
	//	//hr = GetVisibleThumbnailCells(previousThumbnails);
	//}

	if (SUCCEEDED(hr))
	{
		hr = RemoveAllItems();
	}

	// Create render target if necessary. This is needed to set default bitmaps and setup flyer animation
	//if (SUCCEEDED(hr))
	//{
	//	if (!m_renderTarget)
	//	{
	//		hr = CreateDeviceResources();
	//	}
	//}

	// Get list of ShellItems for current directory
	std::vector<ComPtr<IShellItem> > shellItems;


	wchar_t * buffer;
	hr = shellFolder->GetDisplayName(SIGDN_FILESYSPATH, &buffer);
	m_MyDocument = buffer;
	m_MyDocument.append(m_strRootDir);
	m_MyDocument.append(L"\\");
	m_MyDocument.append(m_strDocumentName);

	::SHCreateItemFromParsingName(m_MyDocument.c_str(), nullptr, IID_PPV_ARGS(&shellFolder));
	::CoTaskMemFree(buffer);

	if (!shellFolder)
		return hr;


	// Ignore the returned HRESULT because it's possible no elements are found in this folder
	ShellItemsLoader::EnumerateFolderItems(shellFolder, FileTypeImage, recursive, shellItems);

	//ComPtr<IShellItem> shellItem;


	//BOOL bWide = FALSE;
	D2D1_RECT_F clipRect = D2D1::RectF(0, 0, 2480, 3508);
	if (m_bLandscape) clipRect = D2D1::RectF(0, 0, 3508, 2480);

	int i = 0;
	for (auto shellItem = shellItems.begin(); shellItem != shellItems.end(); shellItem++)
	{

		ThumbnailInfo info(*shellItem);
		info.fileType = FileTypeImage;
		ComPtr<IThumbnail> imageThumbnailControl;
		if (SUCCEEDED(SharedObject<ImageThumbnail>::Create(info, &imageThumbnailControl)))
		{
			//imageThumbnailControl->SetDefaultBitmap(m_defaultThumbnailBitmap);
			imageThumbnailControl->SetRenderingParameters(m_renderingParametersList);
			imageThumbnailControl->SetReversal(m_bReversal);
			imageThumbnailControl->SetClipRect(clipRect);
			imageThumbnailControl->SetParentWindow(this);

			m_thumbnailControls.push_back(imageThumbnailControl);

			if (SUCCEEDED(hr))
			{
				hr = m_AsyncLoaderHelper->ConnectItem(
					imageThumbnailControl,
					static_cast<unsigned int>(m_thumbnailControls.size()) - 1);
			}
		}


		if (imageThumbnailControl) imageThumbnailControl = nullptr;
	}

	if (SUCCEEDED(hr))
	{
		// Initialize thumbnail layout manager with new cells
		CreateThumbnailCells(true);
		hr = m_thumbnailLayoutManager->UpdateVisibleThumbnailControls(m_thumbnailControls);
	}

	//std::vector<ThumbnailCell> currentThumbnails;
	//if (SUCCEEDED(hr))
	//{
	//	// Get currently visible thumbnails
	//	hr = GetVisibleThumbnailCells(currentThumbnails);
	//}

	if (SUCCEEDED(hr))
	{
		// Initialize FlyIn animation
		//m_bIsListAnimation = false;
		//hr = SetupAnimationList(FlyIn, m_renderTarget->GetSize());
	}

	if (SUCCEEDED(hr))
	{
		// Setup the FlyIn animation
		//m_animationController->Setup(previousThumbnails, currentThumbnails, m_renderTarget->GetSize());
		//m_updatingFolder = true;
	}


	return hr;
}

HRESULT CWhiteBoardDlg::SetupAnimationList(AnimationType animationType, D2D1_SIZE_F size)
{
	HRESULT hr = S_OK;

	// Don't start another animation if one if already running or we are in slide show mode
	if (m_bIsListAnimation)
	{
		return hr;
	}


	//if (m_currentAnimation != animationType)
	{
		if (nullptr != m_animationController)
		{ 
			m_animationController = nullptr;
		}

		m_currentAnimation = animationType;

		//if (animationType == FlyIn)
		//{
		//	hr = SharedObject<FlyerAnimation>::Create(&m_animationController);
		//}
		//else if (animationType == MoveAround)
		//{
			hr = SharedObject<MoverAnimation>::Create(&m_animationController);
		//}
	}

	// Capture current size when starting the current animation. This allows us to check if a MoveAround
	// animation is needed after animation is complete
	m_lastAnimationSize = size;


	if (SUCCEEDED(hr))
	{

		if (animationType == MoveAround)
		{

			std::vector<ThumbnailCell> thumbnailCells;
			std::vector<ThumbnailCell> prevThumbnailCells;

			// Capture currently visible thumbnails
			hr = GetVisibleThumbnailCells(prevThumbnailCells);
			m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, m_renderTarget->GetSize().width, m_renderTarget->GetSize().height));
			// Update thumbnail cells
			CreateThumbnailCells(true);
			m_thumbnailLayoutManager->UpdateVisibleThumbnailControls(m_thumbnailControls);
			// Get updated thumbnail cells
			hr = GetVisibleThumbnailCells(thumbnailCells);
			if (SUCCEEDED(hr))
			{
				//TRACE(_T("\n>>>>>>> SetupAnimationList 222 <<<<<<<<<<<<\n "));

				hr = m_animationController->Setup(prevThumbnailCells, thumbnailCells, size);
			}
		}
	}

	m_bIsListAnimation = true;
	::InvalidateRect(m_hWnd, nullptr, FALSE);

	return hr;
}

HRESULT CWhiteBoardDlg::GetVisibleThumbnailCells(std::vector<ThumbnailCell> &cells)
{
	HRESULT hr = S_OK;
	int currentIndex;
	int endIndex;

	if (!m_thumbnailControls.empty())
	{
		int nCurrentPageStartIndex;
		int nCurrentPageEndIndex;
		hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);

		if (SUCCEEDED(hr))
		{
			for (; currentIndex < endIndex; currentIndex++)
			{
				D2D1_RECT_F rect;
				hr = m_thumbnailControls[currentIndex]->GetThumbnailRect(&rect);

				if (SUCCEEDED(hr))
				{
					cells.push_back(ThumbnailCell(m_thumbnailControls[currentIndex], rect));
				}
			}
		}
	}

	return hr;
}

HRESULT CWhiteBoardDlg::RemoveAllItems()
{
	m_AsyncLoaderHelper->ClearItems();
	m_thumbnailControls.clear();
	m_emptyBoard.clear();

	return S_OK;
}




void CWhiteBoardDlg::DrawAnimatedThumbnailCells()
{
	std::vector<AnimatedThumbnailCell> thumbnails;
	m_animationController->GetAnimatedThumbnailCells(thumbnails);

	int i = 0;
	for (auto thumbnail = thumbnails.begin(); thumbnail != thumbnails.end(); ++thumbnail)
	{
		D2D1_POINT_2F center;

		if (SUCCEEDED(m_animationController->GetAnimationPosition(thumbnail->cell.control, &center)))
		{
			//TRACE(_T("\n>>>>>>> animaition pan(%d) (%.02f), (%.02f) <<<<<<<<<<<<\n "), i++, center.x, center.y);

			float offsetX = Direct2DUtility::GetRectWidth(thumbnail->cell.position) / 2;
			float offsetY = Direct2DUtility::GetRectHeight(thumbnail->cell.position) / 2;

			D2D1_RECT_F position = { center.x - offsetX, center.y - offsetY, center.x + offsetX, center.y + offsetY };

			thumbnail->cell.control->SetThumbnailRect(position);
			thumbnail->cell.control->SetIsFullImage(false);
			thumbnail->cell.control->Draw();
		}
	}
}

float CWhiteBoardDlg::GetThumbnailSize()
{
	float thumbnailSize = 0;

	if (m_thumbnailLayoutManager)
	{
		m_thumbnailLayoutManager->GetThumbnailSize(&thumbnailSize);
	}

	return thumbnailSize;
}


//
// Sets the slide show mode specifying the image that should have the current focus when entering/leaving
// slideshow mode
//
void CWhiteBoardDlg::DoModeChange(UINT nMode, unsigned int imageIndex)
{
	AbandonAnimation();
	m_pageMode = nMode;

	if (nMode == PAGE_ITEM)
	{
		m_nCurrentPage = imageIndex;
		D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();
		m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height));
		m_thumbnailLayoutManager->SetSlideShowMode(true);
		m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, imageIndex);

		SetDynamicRendererMode(TRUE);
	}
	else if (nMode == PAGE_LIST)
	{
		m_nCurrentPage = imageIndex;
		// Make sure memory is released for stored full images
		for (auto thumbnail = m_thumbnailControls.begin(); thumbnail != m_thumbnailControls.end(); thumbnail++)
		{
			(*thumbnail)->ReleaseFullImage();
		}

		m_thumbnailLayoutManager->SetSlideShowMode(false);
		CreateThumbnailCells(true);

		D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();
		m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height));
		m_thumbnailLayoutManager->UpdateVisibleThumbnailControls(m_thumbnailControls);
	}
	else if (nMode == PAGE_EMPTYBOARD) {
		SetOperationType(ImageOperationTypePen);
		D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();
		m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height));
		m_thumbnailLayoutManager->SetSlideShowMode(true);
		m_thumbnailLayoutManager->SetVisibleIndex(m_emptyBoard, imageIndex);

		SetDynamicRendererMode(TRUE);
	}

	PrintPageNumber();

	::InvalidateRect(m_hWnd, nullptr, FALSE);
}




HRESULT CWhiteBoardDlg::AbandonAnimation() {
	HRESULT hr = S_OK;

	//TRACE(L"\n>> mode change anima end <<\n");

	m_bIsSlideAnimation = false;
	m_bIsModeChangeAnimation = false;
	m_bIsZoomAnimation = false;
	AnimationUtility::AbandonAllStoryboards(PAGE_ITEM);
	AnimationUtility::AbandonAllStoryboards(PAGE_LIST);
	return hr;

}


HRESULT CWhiteBoardDlg::ReleaseAnimation() {
	HRESULT hr = S_OK;
	AbandonAnimation();

	m_varAniTransX = nullptr;
	m_varAniTransY = nullptr;
	m_varAniZoom = nullptr;
	m_varRectLeft = nullptr;
	m_varRectTop = nullptr;
	m_varRectRight = nullptr;
	m_varRectBottom = nullptr;
	if (m_pageMode == PAGE_ITEM)
		m_thumbnailLayoutManager->ReleaseAnimation(m_thumbnailControls);
	return hr;

}


HRESULT CWhiteBoardDlg::SetModeChangeAnimation() {
	HRESULT hr = S_OK;

	if (m_thumbnailControls.size() <= 0) return hr;

	m_AsyncLoaderHelper->StartFullImageLoad(AsyncLoaderSpace::FullSize, m_nCurrentPage);

	PrintPageNumber();

	m_bIsModeChangeAnimation = true;
	m_bIsBackgroundLoading = true;


	m_varRectLeft = nullptr;
	m_varRectTop = nullptr;
	m_varRectRight = nullptr;
	m_varRectBottom = nullptr;


	D2D1_RECT_F thumbnailRect;
	D2D1_RECT_F DrawingRect;
	m_thumbnailControls[m_nCurrentPage]->GetThumbnailRect(&thumbnailRect);
	m_thumbnailControls[m_nCurrentPage]->GetDrawingRect(&DrawingRect);


	hr = m_animationManager->CreateAnimationVariable(thumbnailRect.left, &m_varRectLeft);
	hr = m_animationManager->CreateAnimationVariable(thumbnailRect.top, &m_varRectTop);
	hr = m_animationManager->CreateAnimationVariable(thumbnailRect.right, &m_varRectRight);
	hr = m_animationManager->CreateAnimationVariable(thumbnailRect.bottom, &m_varRectBottom);

	float TransformationAnimationDuration = 0.6f;

	ComPtr<IUIAnimationStoryboard> storyboard;
	ComPtr<IUIAnimationTransition> transitionLeft;
	ComPtr<IUIAnimationTransition> transitionTop;
	ComPtr<IUIAnimationTransition> transitionRight;
	ComPtr<IUIAnimationTransition> transitionBottom;
	ComPtr<IUIAnimationTransition> transitionLeft2;
	ComPtr<IUIAnimationTransition> transitionTop2;
	ComPtr<IUIAnimationTransition> transitionRight2;
	ComPtr<IUIAnimationTransition> transitionBottom2;


	if (SUCCEEDED(hr))
	{
		// Initialize storyboard
		hr = m_animationManager->CreateStoryboard(&storyboard);
	}

	if (SUCCEEDED(hr))
	{

		float fSmoothLeft;
		float fSmoothTop;
		float fSmoothRight;
		float fSmoothBottom;


		if (thumbnailRect.left < DrawingRect.left) {
			fSmoothLeft = thumbnailRect.left + (DrawingRect.left - thumbnailRect.left) * 0.95f;
		}
		else {
			fSmoothLeft = thumbnailRect.left - (thumbnailRect.left - DrawingRect.left) * 0.95f;
		}

		if (thumbnailRect.right < DrawingRect.right) {
			fSmoothRight = thumbnailRect.right + (DrawingRect.right - thumbnailRect.right) * 0.95f;
		}
		else {
			fSmoothRight = thumbnailRect.right - (thumbnailRect.right - DrawingRect.right) * 0.95f;
		}


		if (thumbnailRect.top < DrawingRect.top) {
			fSmoothTop = thumbnailRect.top + (DrawingRect.top - thumbnailRect.top) * 0.95f;
		}
		else {
			fSmoothTop = thumbnailRect.top - (thumbnailRect.top - DrawingRect.top) * 0.95f;
		}

		if (thumbnailRect.bottom < DrawingRect.bottom) {
			fSmoothBottom = thumbnailRect.bottom + (DrawingRect.bottom - thumbnailRect.bottom) * 0.95f;
		}
		else {
			fSmoothBottom = thumbnailRect.bottom - (thumbnailRect.bottom - DrawingRect.bottom) * 0.95f;
		}



		// Create rotation transition
		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
			TransformationAnimationDuration,
			fSmoothLeft,
			0.4,
			0.6,
			&transitionLeft);
		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
			TransformationAnimationDuration,
			fSmoothTop,
			0.4,
			0.6,
			&transitionTop);
		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
			TransformationAnimationDuration,
			fSmoothRight,
			0.4,
			0.6,
			&transitionRight);
		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
			TransformationAnimationDuration,
			fSmoothBottom,
			0.4,
			0.6,
			&transitionBottom);


		// Create rotation transition
		hr = m_transitionLibrary->CreateSmoothStopTransition(
			0.8f,
			DrawingRect.left,
			&transitionLeft2);
		hr = m_transitionLibrary->CreateSmoothStopTransition(
			0.8f,
			DrawingRect.top,
			&transitionTop2);
		hr = m_transitionLibrary->CreateSmoothStopTransition(
			0.8f,
			DrawingRect.right,
			&transitionRight2);
		hr = m_transitionLibrary->CreateSmoothStopTransition(
			0.8f,
			DrawingRect.bottom,
			&transitionBottom2);


	}

	if (SUCCEEDED(hr))
	{
		hr = storyboard->AddTransition(m_varRectLeft, transitionLeft);
		hr = storyboard->AddTransition(m_varRectTop, transitionTop);
		hr = storyboard->AddTransition(m_varRectRight, transitionRight);
		hr = storyboard->AddTransition(m_varRectBottom, transitionBottom);
		hr = storyboard->AddTransition(m_varRectLeft, transitionLeft2);
		hr = storyboard->AddTransition(m_varRectTop, transitionTop2);
		hr = storyboard->AddTransition(m_varRectRight, transitionRight2);
		hr = storyboard->AddTransition(m_varRectBottom, transitionBottom2);
	}

	if (SUCCEEDED(hr))
	{
		 //Tag the storyboard for priority comparison

		hr = storyboard->SetTag(
			NULL,
			0
		);
		if (SUCCEEDED(hr))
		{
			hr = storyboard->SetLongestAcceptableDelay(
				0.1
			);
		}
	}

	AnimationUtility::ScheduleStoryboard(PAGE_ITEM, storyboard);
	return hr;

}


// 테스트용
HRESULT CWhiteBoardDlg::SetZoomAnimation()
{

	if (m_currentZoom > 2.0f) {
		m_AsyncLoaderHelper->StartFullImageLoad(AsyncLoaderSpace::XxhdpiSize, m_nCurrentPage);
	}

	HRESULT hr = S_OK;
	//float TransformationAnimationDuration = 0.2f;
	m_bIsZoomAnimation = true;

	m_varAniTransX = nullptr;
	m_varAniTransY = nullptr;
	m_varAniZoom = nullptr;
	
	
	float smoothZ = 0;
	float smoothX = 0;
	float smoothY = 0;

	// zoom in
	if (m_currentZoom > m_beforeZoom) {
		smoothZ = m_currentZoom;
		smoothX = m_currentTransX;
		smoothY = m_currentTransY;
	}
	// zoom out
	else {
		smoothZ = m_beforeZoom;
		smoothX = m_beforeTransX;
		smoothY = m_beforeTransY;
	}


	hr = m_animationManager->CreateAnimationVariable(m_beforeTransX, &m_varAniTransX);
	hr = m_animationManager->CreateAnimationVariable(m_beforeTransY, &m_varAniTransY);
	hr = m_animationManager->CreateAnimationVariable(m_beforeZoom, &m_varAniZoom);
	m_beforeTransX = m_currentTransX;
	m_beforeTransY = m_currentTransY;
	m_beforeZoom = m_currentZoom;

	ComPtr<IUIAnimationStoryboard> storyboard;
	ComPtr<IUIAnimationTransition> transitionX;
	ComPtr<IUIAnimationTransition> transitionY;
	ComPtr<IUIAnimationTransition> transitionZoom;

	if (SUCCEEDED(hr))
	{
		// Initialize storyboard
		hr = m_animationManager->CreateStoryboard(&storyboard);

	}

	if (SUCCEEDED(hr))
	{


		// Create rotation transition
		hr = m_transitionLibrary->CreateSmoothStopTransition(
			1,
			m_currentZoom,
			&transitionZoom);
		transitionZoom->SetInitialVelocity(smoothZ * 2);

		hr = m_transitionLibrary->CreateSmoothStopTransition(
			1,
			m_currentTransX,
			&transitionX);
		transitionX->SetInitialVelocity(smoothX * 2);


		hr = m_transitionLibrary->CreateSmoothStopTransition(
			1,
			m_currentTransY,
			&transitionY);
		transitionY->SetInitialVelocity(smoothY * 2);


		
	}

	if (SUCCEEDED(hr))
	{
		hr = storyboard->AddTransition(m_varAniTransX, transitionX);
		hr = storyboard->AddTransition(m_varAniTransY, transitionY);
		hr = storyboard->AddTransition(m_varAniZoom, transitionZoom);
	}

	//if (SUCCEEDED(hr))
	//{
	//	// Tag the storyboard for priority comparison

	//	hr = storyboard->SetTag(
	//		NULL,
	//		0
	//	);
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = storyboard->SetLongestAcceptableDelay(
	//			0.1
	//		);
	//	}
	//}


	
	m_zoomDuration = 0.6f;

	hr = AnimationUtility::ScheduleStoryboard(PAGE_ITEM, storyboard);
	return hr;
}


// 원본
//HRESULT CWhiteBoardDlg::SetZoomAnimation()
//{
//
//
//	if (m_currentZoom > 2.0f) {
//		m_AsyncLoaderHelper->StartFullImageLoad(AsyncLoaderSpace::XxhdpiSize, m_nCurrentPage);
//	}
//
//	HRESULT hr = S_OK;
//	//float TransformationAnimationDuration = 0.2f;
//	m_bIsZoomAnimation = true;
//
//	m_varAniTransX = nullptr;
//	m_varAniTransY = nullptr;
//	m_varAniZoom = nullptr;
//
//	float fSmoothZoomValue;
//	float fSmoothXValue;
//	float fSmoothYValue;
//
//	if (m_beforeZoom < m_currentZoom) {			// 확대
//		fSmoothZoomValue = m_beforeZoom + (m_currentZoom - m_beforeZoom) * 0.9f;
//
//		if (m_beforeTransX < m_currentTransX) {
//			fSmoothXValue = m_beforeTransX + (m_currentTransX - m_beforeTransX) * 0.9f;
//		}
//		else {
//			fSmoothXValue = m_beforeTransX - (m_beforeTransX - m_currentTransX) * 0.9f;
//		}
//
//		if (m_beforeTransY < m_currentTransY) {
//			fSmoothYValue = m_beforeTransY + (m_currentTransY - m_beforeTransY) * 0.9f;
//		}
//		else {
//			fSmoothYValue = m_beforeTransY - (m_beforeTransY - m_currentTransY) * 0.9f;
//		}
//	}
//	else { // 축소
//		fSmoothZoomValue = m_currentZoom + 0.1;
//		fSmoothXValue = m_currentTransX;
//		fSmoothYValue = m_currentTransY;
//	}
//
//
//
//
//	//if (m_beforeZoom > m_currentZoom) {			// 축소
//	//	fSmoothZoomValue = m_currentZoom + 0.05;
//	//	fSmoothXValue = m_currentTransX;
//	//	fSmoothYValue = m_currentTransY;
//	//}
//	//else {
//	//	fSmoothZoomValue = m_currentZoom - m_currentZoom*0.1;
//
//	//	if (m_beforeTransX > m_currentTransX) {
//	//		fSmoothXValue = m_currentTransX - m_currentTransX * 0.1;
//	//	}
//	//	else {
//	//		fSmoothXValue = m_currentTransX + m_currentTransX * 0.1;
//	//	}
//
//	//	if (m_beforeTransY > m_currentTransY) {
//	//		fSmoothYValue = m_currentTransY - m_currentTransY * 0.1;
//	//	}
//	//	else {
//	//		fSmoothYValue = m_currentTransY - m_currentTransY * 0.1;
//	//	}
//	//}
//
//	hr = m_animationManager->CreateAnimationVariable(m_beforeTransX, &m_varAniTransX);
//	hr = m_animationManager->CreateAnimationVariable(m_beforeTransY, &m_varAniTransY);
//	hr = m_animationManager->CreateAnimationVariable(m_beforeZoom, &m_varAniZoom);
//	m_beforeTransX = m_currentTransX;
//	m_beforeTransY = m_currentTransY;
//	m_beforeZoom = m_currentZoom;
//
//	ComPtr<IUIAnimationStoryboard> storyboard;
//	ComPtr<IUIAnimationTransition> transitionX;
//	ComPtr<IUIAnimationTransition> transitionY;
//	ComPtr<IUIAnimationTransition> transitionZoom;
//	ComPtr<IUIAnimationTransition> transitionX2;
//	ComPtr<IUIAnimationTransition> transitionY2;
//	ComPtr<IUIAnimationTransition> transitionZoom2;
//
//	if (SUCCEEDED(hr))
//	{
//		// Initialize storyboard
//		hr = m_animationManager->CreateStoryboard(&storyboard);
//
//	}
//
//	if (SUCCEEDED(hr))
//	{
//
//		// Create rotation transition
//		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
//			m_zoomDuration,
//			fSmoothZoomValue,
//			0.5,
//			0.5,
//			&transitionZoom);
//
//		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
//			m_zoomDuration,
//			fSmoothXValue,
//			0.5,
//			0.5,
//			&transitionX);
//
//		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
//			m_zoomDuration,
//			fSmoothYValue,
//			0.5,
//			0.5,
//			&transitionY);
//
//		hr = m_transitionLibrary->CreateSmoothStopTransition(
//			m_zoomDuration + 0.1,
//			m_currentZoom,
//			&transitionZoom2);
//
//		hr = m_transitionLibrary->CreateSmoothStopTransition(
//			m_zoomDuration + 0.1,
//			m_currentTransX,
//			&transitionX2);
//
//		hr = m_transitionLibrary->CreateSmoothStopTransition(
//			m_zoomDuration + 0.1,
//			m_currentTransY,
//			&transitionY2);
//
//
//	}
//
//	if (SUCCEEDED(hr))
//	{
//		hr = storyboard->AddTransition(m_varAniTransX, transitionX);
//		hr = storyboard->AddTransition(m_varAniTransY, transitionY);
//		hr = storyboard->AddTransition(m_varAniZoom, transitionZoom);
//		hr = storyboard->AddTransition(m_varAniTransX, transitionX2);
//		hr = storyboard->AddTransition(m_varAniTransY, transitionY2);
//		hr = storyboard->AddTransition(m_varAniZoom, transitionZoom2);
//	}
//
//	//if (SUCCEEDED(hr))
//	//{
//	//	// Tag the storyboard for priority comparison
//
//	//	hr = storyboard->SetTag(
//	//		NULL,
//	//		0
//	//	);
//	//	if (SUCCEEDED(hr))
//	//	{
//	//		hr = storyboard->SetLongestAcceptableDelay(
//	//			0.1
//	//		);
//	//	}
//	//}
//
//
//
//	m_zoomDuration = 0.6f;
//
//	hr = AnimationUtility::ScheduleStoryboard(PAGE_ITEM, storyboard);
//	return hr;
//}

HRESULT CWhiteBoardDlg::SetEmptyBoardAnimation()
{
	HRESULT hr = S_OK;
	m_bIsEmptyBoardAnimation = true;

	m_beforeAlpha = 0.f;
	m_currentAlpha = 1.f;
	m_emptyBoardPanPoint.x = 0;
	m_emptyBoardPanPoint.y = 0;


	m_varAniAlpha = nullptr;
	hr = m_animationManager->CreateAnimationVariable(m_beforeAlpha, &m_varAniAlpha);
	m_beforeAlpha = m_currentAlpha;

	ComPtr<IUIAnimationStoryboard> storyboard;
	ComPtr<IUIAnimationTransition> transAlpha;

	if (SUCCEEDED(hr))
	{
		hr = m_animationManager->CreateStoryboard(&storyboard);
	}

	if (SUCCEEDED(hr))
	{

		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
			1.0f,
			m_currentAlpha,
			0.5,
			0.5,
			&transAlpha);
	}

	if (SUCCEEDED(hr))
	{
		hr = storyboard->AddTransition(m_varAniAlpha, transAlpha);
	}

	if (SUCCEEDED(hr))
	{
		hr = storyboard->SetTag(
			NULL,
			0
		);
		if (SUCCEEDED(hr))
		{
			hr = storyboard->SetLongestAcceptableDelay(
				0.1
			);
		}
	}
	AnimationUtility::ScheduleStoryboard(PAGE_ITEM, storyboard);
	return hr;
}


D2D1_POINT_2F CWhiteBoardDlg::GetAbsolutePosition(D2D1_POINT_2F mousePosition)
{
	//TRACE(L"\n >>> 11111111  GetAbsolutePosition (%.02f, %.02f) <<<<\n", mousePosition.x, mousePosition.y);
	// First transform back the point (disregarding current translation and scale)
	mousePosition = RemoveRenderingTransformations(mousePosition);

	//TRACE(L"\n >>> 2222222 GetAbsolutePosition (%.02f, %.02f) <<<<\n", mousePosition.x, mousePosition.y);
	// Translate to an absolute point within the image current drawing rect
	D2D1_POINT_2F absPoint;
	GetCurrentThumbnailPtr()->TranslateToAbsolutePoint(mousePosition, &absPoint);

	//TRACE(L"\n >>> 33333333  GetAbsolutePosition (%.02f, %.02f) <<<<\n", absPoint.x, absPoint.y);

	D2D1_RECT_F rect;
	GetCurrentThumbnailPtr()->GetDrawingRect(&rect);

	// Scale to actual point relative to the original bitmap
	float scale;
	GetCurrentThumbnailPtr()->GetScale(&scale);


	// 이미지 영역 안에서만 그리기 해제
	//return AdjustToClipRect(
	//	D2D1::Point2F(
	//		scale * (absPoint.x - rect.left),
	//		scale * (absPoint.y - rect.top)));
	return D2D1::Point2F(scale * (absPoint.x - rect.left), scale * (absPoint.y - rect.top));
}


D2D1_RECT_F CWhiteBoardDlg::GetScreenRect(D2D1_RECT_F rect)
{
	D2D1_RECT_F rectDrawing;
	GetCurrentThumbnailPtr()->GetDrawingRect(&rectDrawing);

	// Scale to actual point relative to the original bitmap
	float scale;
	GetCurrentThumbnailPtr()->GetScale(&scale);
	return D2D1::RectF(scale * (rect.left + rectDrawing.left), scale * (rect.top + rectDrawing.top), scale * (rect.right + rectDrawing.left), scale * (rect.bottom + rectDrawing.top));
}


D2D1_POINT_2F CWhiteBoardDlg::AdjustToClipRect(D2D1_POINT_2F absPoint)
{
	D2D1_RECT_F clipRect;
	m_thumbnailControls[m_nCurrentPage]->GetClipRect(&clipRect);

	return D2D1::Point2F(
		std::max<FLOAT>(clipRect.left, std::min<FLOAT>(clipRect.right, absPoint.x)),
		std::max<FLOAT>(clipRect.top, std::min<FLOAT>(clipRect.bottom, absPoint.y)));
}

D2D1_POINT_2F CWhiteBoardDlg::RemoveRenderingTransformations(D2D1_POINT_2F mousePosition)
{
	float zoom = (m_pageMode == PAGE_ITEM) ? m_currentZoom : m_emptyBoardZoom;
	D2D1_POINT_2F panPoint = (m_pageMode == PAGE_ITEM) ? m_currentPanPoint : m_emptyBoardPanPoint;
	mousePosition = D2D1::Matrix3x2F::Translation(-panPoint.x, -panPoint.y).TransformPoint(mousePosition);
	mousePosition = D2D1::Matrix3x2F::Scale(1 / zoom, 1 / zoom, GetCenter()).TransformPoint(mousePosition);

	return mousePosition;
}



int CWhiteBoardDlg::IsImageHit(D2D1_POINT_2F mousePosition)
{
	if (GetPageMode() != PAGE_ITEM) {
		return Direct2DUtility::HIT_IN;
	}

	mousePosition = RemoveRenderingTransformations(mousePosition);
	int isHit = Direct2DUtility::HIT_NON;

	if (m_nCurrentPage < static_cast<UINT>(m_thumbnailControls.size())) {
		m_thumbnailControls[m_nCurrentPage]->ContainsPoint(mousePosition, &isHit);
	}

	return isHit;
}




void CWhiteBoardDlg::AddEmptyBoardItem() {
	HRESULT hr = S_OK;
	ThumbnailInfo info;
	info.m_fileName = L"";
	info.fileType = FileTypeImage;
	ComPtr<IThumbnail> emptyBoardItem;
	D2D1_RECT_F drawingRect;
	drawingRect.left = 900;
	drawingRect.top = 1;
	drawingRect.right = 1600;
	drawingRect.bottom = 1017;

	//drawingRect.left = 0.5f * (m_renderTarget->GetSize().width - 2480);
	//drawingRect.right = drawingRect.left + 2480;
	//drawingRect.top = 0.5f * (m_renderTarget->GetSize().height - 3508);
	//drawingRect.bottom = drawingRect.top + 3508;

	if (SUCCEEDED(SharedObject<ImageThumbnail>::Create(info, &emptyBoardItem)))
	{
		emptyBoardItem->SetRenderingParameters(m_renderingParametersList);
		emptyBoardItem->SetParentWindow(this);
		emptyBoardItem->SetOffsetIndexRect(0);
		emptyBoardItem->SetDrawingRect(drawingRect);
		m_emptyBoard.push_back(emptyBoardItem);
	}
	if (emptyBoardItem) emptyBoardItem = nullptr;



}


void CWhiteBoardDlg::OnPaintEmptyBoard() {
	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		m_renderTarget->BeginDraw();
		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(m_emptyBoardZoom, m_emptyBoardZoom, GetCenter());
		D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(m_emptyBoardPanPoint.x, m_emptyBoardPanPoint.y);
		m_renderTarget->SetTransform(scale * translation);

		if (m_nEmptyBoardPage < m_emptyBoard.size())
			m_emptyBoard[m_nEmptyBoardPage]->DrawEmptyBoard(TRUE);
		else {
			m_emptyBoard[0]->DrawEmptyBoard(TRUE);
		}
	}

	hr = m_renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}
}



ComPtr<IThumbnail> CWhiteBoardDlg::GetCurrentThumbnailPtr() {
	if (m_pageMode == PAGE_ITEM) {
		return m_thumbnailControls[m_nCurrentPage];
	}
	else if (m_pageMode == PAGE_EMPTYBOARD) {
		if (m_nEmptyBoardPage < m_emptyBoard.size()) {
			return m_emptyBoard[m_nEmptyBoardPage];
		}
		
	}

	return nullptr;

}


void CWhiteBoardDlg::SetPenColor(int index) {
	GetPen()->SetColor(index);
	//switch (index) {
	//case 1:
	//	m_penColor = D2D1::ColorF(D2D1::ColorF::Red);
	//	break;
	//case 2:
	//	m_penColor = D2D1::ColorF(D2D1::ColorF(0xfff361));
	//	break;
	//case 3:
	//	m_penColor = D2D1::ColorF(D2D1::ColorF(0x00ff2a));
	//	break;
	//case 4:
	//	m_penColor = D2D1::ColorF(D2D1::ColorF::Blue);
	//	break;
	//case 5:
	//	m_penColor = D2D1::ColorF(D2D1::ColorF::White);
	//	break;
	//case 6:
	//	m_penColor = D2D1::ColorF(D2D1::ColorF::Black);
	//	break;
	//default:
	//	m_penColor = D2D1::ColorF(D2D1::ColorF::Red);
	//	break;
	//}
}


//float CWhiteBoardDlg::GetPenSize() {
//	return GetPen()->GetSizeIndex();
//}

float CWhiteBoardDlg::GetPenSizeDynamicRender() {
	float scale = GetCurrentThumbnailPtr()->GetImageScale();

	if (GetPageMode() == PAGE_EMPTYBOARD) {
		return (GetPen()->GetSizeValue() / scale * 52.5);
	}
	else return (GetPen()->GetSizeValue() / scale * (m_currentZoom) * 52.5);
}



void CWhiteBoardDlg::SetPenSizeUp() {
	GetPen()->SetSizeUp();
}

void CWhiteBoardDlg::SetPenSizeDown() {
	GetPen()->SetSizeDown();
}


void CWhiteBoardDlg::SetPenSize(int index) {
	GetPen()->SetSize(index);
//
//	switch (index) {
//	case 1:
//		m_penSize = 1;
//		break;
//	case 2:
//		m_penSize = 2;
//		break;
//	case 3:
//		m_penSize = 4;
//		break;
//	case 4:
//		m_penSize = 6;
//		break;
//	case 5:
//		m_penSize = 8;
//		break;
//	case 6:
//		m_penSize = 10;
//		break;
//	default:
//		m_penSize = 14;
//		break;
//	}
}


void CWhiteBoardDlg::SetPenType(int index) {
	SetOperationType(ImageOperationTypePen);
	m_penType = index;
}

//void CWhiteBoardDlg::SetPenAlpha(int index) {
	//SetOperationType(ImageOperationTypePen);
	//m_nPenAlphaIndex = index;
	//GetPen()->SetAlpha()

	//switch (index) {
	//case PEN_ALPHA:
	//	m_penAlpha = 1.0f;
	//	break;
	//case HPEN_ALPHA:
	//	m_penAlpha = 0.5f;
	//	break;
	//default:
	//	m_penAlpha = 1.f;
	//	break;
	//}
//}



LRESULT CWhiteBoardDlg::OnPointerDown(WPARAM wParam, LPARAM lParam) {
	return 0L;
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(&pt);
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);

	InputProcessDown(mousePosition);

	return 0L;
}

LRESULT CWhiteBoardDlg::OnPointerUpdate(WPARAM wParam, LPARAM lParam) {
	//AfxMessageBox(L"포인트 업 ", MB_OK);

	return 0L;
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(&pt);
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);

	InputProcessMove(mousePosition);

	return 0L;
}

LRESULT CWhiteBoardDlg::OnPointerUp(WPARAM wParam, LPARAM lParam) {
	return 0L;
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(&pt);
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);

	InputProcessUp(mousePosition);

	return 0L;
}

LRESULT CWhiteBoardDlg::OnTouch(WPARAM wParam, LPARAM lParam) {
	//AfxMessageBox(L"터치 ");

	return 0L;
	//if (m_autoIncrese > 100000) m_autoIncrese = 0;
	//return DefWindowProc(WM_TOUCH, wParam, lParam);
	BOOL bHandled = FALSE;
	UINT cInputs = LOWORD(wParam);
	PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
	if (pInputs) {
		if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT))) {
			//for (UINT i = 0; i < cInputs; i++) {
				//TOUCHINPUT ti = pInputs[i];
			TOUCHINPUT ti = pInputs[0];
				POINT pt;
				pt.x = ti.x / 100;
				pt.y = ti.y / 100;
				ScreenToClient(&pt);

				D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);


				if (ti.dwFlags & TOUCHEVENTF_DOWN) {
					InputProcessDown(mousePosition);
				}
				else if (ti.dwFlags & TOUCHEVENTF_UP) {
					InputProcessUp(mousePosition);
				}
				else if (ti.dwFlags & TOUCHEVENTF_MOVE) {
					InputProcessMove(mousePosition);
				}
				else {
					InputProcessMove(mousePosition);
				}
			//}
			bHandled = TRUE;
		}
		else {
			/* handle the error here */
		}
		delete[] pInputs;
	}
	else {
		/* handle the error here, probably out of memory */
	}
	if (bHandled) {
		// if you handled the message, close the touch input handle and return
		CloseTouchInputHandle((HTOUCHINPUT)lParam);
		return 0;
	}
	else {
		return 0L;
	}
}



BOOL CWhiteBoardDlg::InputProcessDown(D2D1_POINT_2F mousePosition, LONG pressure) {
#ifdef STYLUS_ON
	//return;
#endif


	int isHit = IsImageHit(mousePosition);

	//TRACE(L"\n>>>> key InputProcessDown (%d) page(%d) <<<<<<<<\n", isHit, m_nCurrentPage);

	if (m_pageMode == PAGE_EMPTYBOARD) {
		if (!m_bIsDrawing && GetOperationType() == ImageOperationTypePen && isHit == Direct2DUtility::HIT_IN) {
			{

				D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					D2D1_ALPHA_MODE_IGNORE
				);

				float dpiX, dpiY;
				m_renderTarget->GetDpi(&dpiX, &dpiY);

				D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties();
				props.pixelFormat = pixelFormat;
				props.dpiX = dpiX;
				props.dpiY = dpiY;
				//if (m_renderTargetBitmap) m_renderTargetBitmap = nullptr;
				if (m_renderTargetBitmap == nullptr) {
					m_renderTarget->CreateBitmap(D2D1::SizeU(m_renderTarget->GetPixelSize().width, m_renderTarget->GetPixelSize().height), props, &m_renderTargetBitmap);
				}
				D2D1_POINT_2U DestinationPoint = D2D1::Point2U(0, 0);
				D2D1_RECT_U SourceRect = D2D1::RectU(0, 0, m_renderTarget->GetPixelSize().width, m_renderTarget->GetPixelSize().height);
				m_renderTargetBitmap->CopyFromRenderTarget(&DestinationPoint, m_renderTarget, &SourceRect);

			}

			// 기존 할당 메모리 제거
			if (m_currentOperation) m_currentOperation = nullptr;

			m_bIsDrawing = true;
			HRESULT hr = SharedObject<DrawGeometryOperation>::Create(&m_currentOperation);
			ComPtr<IDrawGeometryOperation> drawGeometry;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&drawGeometry)))
			{
				drawGeometry->SetPenId(++m_penId);
				drawGeometry->SetBrushColor(D2D1::ColorF(RGB(GetBValue(GetPenColor()), GetGValue(GetPenColor()), GetRValue(GetPenColor()))));
				drawGeometry->SetBrushAlpha(GetPen()->GetAlphaValue());
				drawGeometry->SetStrokeSize(GetPen()->GetSizeValue());
				drawGeometry->SetPenStyleValue(GetPen()->GetPentipStyle(), GetPen()->GetPressure());
				drawGeometry->AppendPoint(m_renderTarget, GetAbsolutePosition(mousePosition), pressure);

				GetCurrentThumbnailPtr()->PushImageOperation(m_currentOperation);

			}
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else if (GetOperationType() == ImageOperationTypeEraser)
		{
			m_bIsEraser = true;
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
			if (SUCCEEDED(GetCurrentThumbnailPtr()->EraserContaionImageOperation(GetAbsolutePosition(mousePosition)))) {
			//if (SUCCEEDED(GetCurrentThumbnailPtr()->EraserContaionImageOperation(mousePosition))) {
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}

		}
		else {
			if (isHit == Direct2DUtility::HIT_IN) {
				m_bIsDrag = true;
			}
			else {
				if (isHit == Direct2DUtility::HIT_LEFT) {
					m_pageChanging = PAGE_PREV;
				}
				else {
					m_pageChanging = PAGE_NEXT;
				}
			}

			m_mouseDownPoint = mousePosition;
			m_mousePrevPoint = mousePosition;
		}
	}

	if (m_pageMode == PAGE_ITEM) {
		if (!m_bIsDrawing && GetOperationType() == ImageOperationTypePen && isHit == Direct2DUtility::HIT_IN) {
			//TRACE(_T("\n>>>>> lbutton down <<<<<\n"));

			// copy old render target
			{
				D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					D2D1_ALPHA_MODE_IGNORE
				);

				float dpiX, dpiY;
				m_renderTarget->GetDpi(&dpiX, &dpiY);

				D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties();
				props.pixelFormat = pixelFormat;
				props.dpiX = dpiX;
				props.dpiY = dpiY;

				if (m_renderTargetBitmap == nullptr) {
					m_renderTarget->CreateBitmap(D2D1::SizeU(m_renderTarget->GetPixelSize().width, m_renderTarget->GetPixelSize().height), props, &m_renderTargetBitmap);
				}
				D2D1_POINT_2U DestinationPoint = D2D1::Point2U(0, 0);
				D2D1_RECT_U SourceRect = D2D1::RectU(0, 0, m_renderTarget->GetPixelSize().width, m_renderTarget->GetPixelSize().height);
				m_renderTargetBitmap->CopyFromRenderTarget(&DestinationPoint, m_renderTarget, &SourceRect);
			}

			// 기존 할당 메모리 제거
			if (m_currentOperation)
			{
				m_currentOperation = nullptr;
			}

			m_bIsDrawing = true;
			HRESULT hr = SharedObject<DrawGeometryOperation>::Create(&m_currentOperation);
			ComPtr<IDrawGeometryOperation> drawGeometry;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&drawGeometry)))
			{

				drawGeometry->SetPenId(++m_penId);
				drawGeometry->SetBrushColor(D2D1::ColorF(RGB(GetBValue(GetPenColor()), GetGValue(GetPenColor()), GetRValue(GetPenColor()))));
				drawGeometry->SetBrushAlpha(GetPen()->GetAlphaValue());
				drawGeometry->SetStrokeSize(GetPen()->GetSizeValue());
				drawGeometry->SetPenStyleValue(GetPen()->GetPentipStyle(), GetPen()->GetPressure());

				drawGeometry->AppendPoint(m_renderTarget, GetAbsolutePosition(mousePosition), pressure);

				GetCurrentThumbnailPtr()->PushImageOperation(m_currentOperation);
				//PostMessage(MY_WM_BEGINDRAW, mousePosition.x, mousePosition.y);

			}
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else if (GetOperationType() == ImageOperationTypeEraser)
		{
			m_bIsEraser = true;
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
			if (SUCCEEDED(GetCurrentThumbnailPtr()->EraserContaionImageOperation(GetAbsolutePosition(mousePosition)))) {
			//if (SUCCEEDED(GetCurrentThumbnailPtr()->EraserContaionImageOperation(mousePosition))) {
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else if (GetOperationType() == ImageOperationTypeZoom && isHit == Direct2DUtility::HIT_IN)
		{
			m_zoomDuration = 0.8f;
			m_zoomBox.left = m_zoomBox.right = mousePosition.x;
			m_zoomBox.top = m_zoomBox.bottom = mousePosition.y;
			mousePosition = RemoveRenderingTransformations(mousePosition);
			m_zoomBoxDrawing.left = m_zoomBoxDrawing.right = mousePosition.x;
			m_zoomBoxDrawing.top = m_zoomBoxDrawing.bottom = mousePosition.y;
			m_bIsZoomming = true;
			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else if (GetOperationType() == ImageOperationTypeClipping && isHit == Direct2DUtility::HIT_IN && m_pageMode == PAGE_ITEM)
		{
			m_clippingBox.left = m_clippingBox.right = mousePosition.x;
			m_clippingBox.top = m_clippingBox.bottom = mousePosition.y;
			mousePosition = RemoveRenderingTransformations(mousePosition);
			m_clippingBoxDrawing.left = m_clippingBoxDrawing.right = mousePosition.x;
			m_clippingBoxDrawing.top = m_clippingBoxDrawing.bottom = mousePosition.y;
			m_bIsClipping = true;
			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else {

			if (isHit == Direct2DUtility::HIT_IN) {
				m_bIsDrag = true;
			}
			else {
				if (isHit == Direct2DUtility::HIT_LEFT) {
					m_pageChanging = PAGE_PREV;
				}
				else {
					m_pageChanging = PAGE_NEXT;
				}
			}
			
			m_mouseDownPoint = mousePosition;
			m_mousePrevPoint = mousePosition;
		}
	}



	//if (m_pageMode == PAGE_LIST) {
	//	int currentIndex;
	//	int endIndex;

	//	int nCurrentPageStartIndex;
	//	int nCurrentPageEndIndex;
	//	HRESULT hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);
	//	if (SUCCEEDED(hr))
	//	{
	//		// Determine if one of the visible thumbnail controls was clicked
	//		for (currentIndex; currentIndex < endIndex; currentIndex++)
	//		{
	//			D2D1_RECT_F rect;

	//			hr = m_thumbnailControls[currentIndex]->GetThumbnailRect(&rect);
	//			if (SUCCEEDED(hr))
	//			{
	//				if (Direct2DUtility::HitTest(rect, mousePosition) == Direct2DUtility::HIT_IN)
	//				{
	//					DoItemMode(currentIndex);
	//					//DoModeChange(PAGE_ITEM, currentIndex);
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}

	return (isHit == Direct2DUtility::HIT_IN);
}
void CWhiteBoardDlg::InputProcessMove(D2D1_POINT_2F mousePosition, LONG pressure) {
#ifdef STYLUS_ON
	//return;
#endif
	
	//TRACE(_T("\n>> pagemode (%d) <<<\n"), m_pageMode);

	if (m_pageMode == PAGE_EMPTYBOARD) {
		if (m_bIsDrawing) {
			ComPtr<IDrawGeometryOperation> drawGeometry;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&drawGeometry)))
			{
				drawGeometry->AppendPoint(m_renderTarget, GetAbsolutePosition(mousePosition), pressure);
			}
		}
		else if (m_bIsEraser) {


			// ink mode
			if (SUCCEEDED(GetCurrentThumbnailPtr()->EraserContaionImageOperation(GetAbsolutePosition(mousePosition)))) {
			//if (SUCCEEDED(GetCurrentThumbnailPtr()->EraserContaionImageOperation(mousePosition))) {
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else if (m_bIsDrag) {
			// Update pan based on delta
			D2D1_POINT_2F delta;
			delta.x = (mousePosition.x - m_mousePrevPoint.x);
			delta.y = (mousePosition.y - m_mousePrevPoint.y);

			if (!(delta.x == 0 && delta.y == 0))
			{
				m_emptyBoardPanPoint.x += delta.x;
				m_emptyBoardPanPoint.y += delta.y;


				m_mousePrevPoint = mousePosition;


				D2D1_SIZE_F renderSize = m_renderTarget->GetSize();
				float fSlideDistanceH = renderSize.width;
				float fSlideDistanceV = renderSize.height;


				if (std::abs(m_emptyBoardPanPoint.x) > fSlideDistanceH)
				{
					if (m_emptyBoardPanPoint.x > 0)
					{
					m_emptyBoardPanPoint.x = fSlideDistanceH;
					}
					else
					{
						m_emptyBoardPanPoint.x = -fSlideDistanceH;
					}
				}

				// 상단 이동 방지
				if (m_emptyBoardPanPoint.x > 0) {
					m_emptyBoardPanPoint.x = 0;
				}

				if (std::abs(m_emptyBoardPanPoint.y) > fSlideDistanceV)
				{
					if (m_emptyBoardPanPoint.y > 0)
					{
						m_emptyBoardPanPoint.y = fSlideDistanceV;
					}
					else
					{
						m_emptyBoardPanPoint.y = -fSlideDistanceV;
					}
				}

				// 좌측 이동 방지
				if (m_emptyBoardPanPoint.y > 0) {
					m_emptyBoardPanPoint.y = 0;
				}

				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else {
			EditorItemHitTest(GetAbsolutePosition(mousePosition), FALSE);
		}
	}

	if (m_pageMode == PAGE_ITEM) {

		if (GetOperationType() == ImageOperationTypePen) {
			//ShowCursor(TRUE);
		}

		if (m_bIsDrawing) {

			//TRACE(_T("\n>> pressure (%d) <<<\n"), pressure);

			//SetCursor(theApp.LoadCursor(IDC_PEN_RM));
			ComPtr<IDrawGeometryOperation> drawGeometry;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&drawGeometry)))
			{
				drawGeometry->AppendPoint(m_renderTarget, GetAbsolutePosition(mousePosition), pressure);
				//PostMessage(MY_WM_BEGINDRAW, mousePosition.x, mousePosition.y);
				//m_renderTarget->BeginDraw();
				//D2D1_MATRIX_3X2_F originalTransform;
				//m_renderTarget->GetTransform(&originalTransform);

				//D2D1_RECT_F						m_clipRect;			// 실제 이미지 크기 값
				//D2D1_RECT_F						m_drawingRect;		// 기본 출력 이미지 영역 좌표 값 (확대, 축소 미적용)
				//GetCurrentThumbnailPtr()->GetClipRect(&m_clipRect);
				//GetCurrentThumbnailPtr()->GetDrawingRect(&m_drawingRect);

				//D2D1_POINT_2F midPoint = Direct2DUtility::GetMidPoint(m_drawingRect);
				//float scale = GetCurrentThumbnailPtr()->GetImageScale();
				//D2D1_MATRIX_3X2_F transform =
				//	D2D1::Matrix3x2F::Translation(-m_clipRect.left, -m_clipRect.top) *
				//	D2D1::Matrix3x2F::Scale(1 / scale, 1 / scale) *
				//	D2D1::Matrix3x2F::Translation(m_drawingRect.left, m_drawingRect.top) *
				//	/*GetTransformationsReversed(midPoint) **/
				//	originalTransform;

				//m_renderTarget->SetTransform(transform);

				//drawGeometry->DrawToRenderTarget(m_renderTarget);

				//m_renderTarget->SetTransform(originalTransform);
				//m_renderTarget->EndDraw();


				//drawGeometry->EndGeometry(m_renderTarget);
				//::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else if (m_bIsEraser) {
			// ink 모드 일 경우 mouse 위치값을 변경
			if (SUCCEEDED(GetCurrentThumbnailPtr()->EraserContaionImageOperation(GetAbsolutePosition(mousePosition)))) {
			//TRACE(L"\n>>> 지우개 <<<<\n");
			//if (SUCCEEDED(GetCurrentThumbnailPtr()->EraserContaionImageOperation(mousePosition))) {
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else if (m_bIsZoomming)
		{
			m_zoomBox.right = mousePosition.x;
			m_zoomBox.bottom = mousePosition.y;
			mousePosition = RemoveRenderingTransformations(mousePosition);

			D2D1_RECT_F imageRect;
			m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);
			m_zoomBoxDrawing.right = std::max<FLOAT>(imageRect.left, std::min<FLOAT>(imageRect.right, mousePosition.x));
			m_zoomBoxDrawing.bottom = std::max<FLOAT>(imageRect.top, std::min<FLOAT>(imageRect.bottom, mousePosition.y));

			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else if (m_bIsClipping) {
			m_clippingBox.right = mousePosition.x;
			m_clippingBox.bottom = mousePosition.y;
			mousePosition = RemoveRenderingTransformations(mousePosition);

			D2D1_RECT_F imageRect;
			m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);
			m_clippingBoxDrawing.right = std::max<FLOAT>(imageRect.left, std::min<FLOAT>(imageRect.right, mousePosition.x));
			m_clippingBoxDrawing.bottom = std::max<FLOAT>(imageRect.top, std::min<FLOAT>(imageRect.bottom, mousePosition.y));

			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else if (m_bIsDrag) {
			// Update pan based on delta
			D2D1_POINT_2F delta;
			delta.x = (mousePosition.x - m_mousePrevPoint.x);
			delta.y = (mousePosition.y - m_mousePrevPoint.y);

			if (!(delta.x == 0 && delta.y == 0))
			{
				m_currentPanPoint.x += delta.x;
				m_currentPanPoint.y += delta.y;


				m_mousePrevPoint = mousePosition;


				D2D1_RECT_F imageRect;
				m_thumbnailControls[m_nCurrentPage]->GetDrawingRect(&imageRect);
				D2D1_SIZE_F renderSize = m_renderTarget->GetSize();

				float transedImageWidth = (imageRect.right - imageRect.left) * m_currentZoom;
				float transedImageHeight = (imageRect.bottom - imageRect.top) * m_currentZoom;
				float fSlideDistanceH;
				float fSlideDistanceV;

				if (transedImageWidth > renderSize.width) {
					//fSlideDistanceH = transedImageWidth*0.5;
					fSlideDistanceH = (transedImageWidth - renderSize.width) *0.5 + 40;
				}
				else {
					fSlideDistanceH = 0; //(/*renderSize.width - */transedImageWidth);
				}

				if (transedImageHeight > renderSize.height) {
					fSlideDistanceV = (transedImageHeight - renderSize.height) *0.5 + 40;// (transedImageHeight * 0.5 /* - renderSize.height*/);
				}
				else {
					fSlideDistanceV = (renderSize.height - transedImageHeight);
				}

				if (std::abs(m_currentPanPoint.x) > fSlideDistanceH)
				{
					if (m_currentPanPoint.x > 0)
					{
						m_currentPanPoint.x = fSlideDistanceH;
					}
					else
					{
						m_currentPanPoint.x = -fSlideDistanceH;
					}
				}
				if (std::abs(m_currentPanPoint.y) > fSlideDistanceV)
				{
					if (m_currentPanPoint.y > 0)
					{
						m_currentPanPoint.y = fSlideDistanceV;
					}
					else
					{
						m_currentPanPoint.y = -fSlideDistanceV;
					}
				}


				m_currentTransX = m_currentPanPoint.x;
				m_currentTransY = m_currentPanPoint.y;
				m_beforeTransX = m_currentTransX;
				m_beforeTransY = m_currentTransY;
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else {
			EditorItemHitTest(GetAbsolutePosition(mousePosition), FALSE);
		}
	}


	if (m_pageMode == PAGE_LIST) {

	

		bool redraw = false;
		int currentIndex;
		int endIndex;

		int nCurrentPageStartIndex;
		int nCurrentPageEndIndex;
		HRESULT hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);
		if (SUCCEEDED(hr))
		{


			// Determine if one of the visible thumbnail controls
			for (; currentIndex < endIndex; currentIndex++)
			{
				D2D1_RECT_F positionRect;

				hr = m_thumbnailControls[currentIndex]->GetThumbnailRect(&positionRect);
				if (SUCCEEDED(hr))
				{
					// Adjust for textbox
					//positionRect.bottom += m_textLayoutList->GetMaxHeight();
				}

				ThumbnailSelectionState selectionState = SelectionStateNone;
				if (SUCCEEDED(hr))
				{
					hr = m_thumbnailControls[currentIndex]->GetSelectionState(&selectionState);
				}

				if (SUCCEEDED(hr))
				{

					//if (currentIndex == 1) {
					//	TRACE(_T("\n>>>>>>>> pos(%.02f,%.02f,%.02f,%.02f) mouse(%.02f,%.02f) <<<<<\n"), 
					//		positionRect.left, positionRect.top, positionRect.right, positionRect.bottom,
					//		mousePosition.x, mousePosition.y);
					//}



					if (Direct2DUtility::HitTest(positionRect, mousePosition) == Direct2DUtility::HIT_IN)
					{
		

						if ((selectionState & SelectionStateHoverOnLeft) != SelectionStateHoverOnLeft)
						{
							selectionState = static_cast<ThumbnailSelectionState>(selectionState | SelectionStateHoverOnLeft);
							m_thumbnailControls[currentIndex]->SetSelectionState(selectionState);
							redraw = true;
						}
					}
					else
					{
						//	// Only toggle hover if necessary
						if ((selectionState & SelectionStateHoverOnLeft) == SelectionStateHoverOnLeft)
						{
							selectionState = static_cast<ThumbnailSelectionState>(selectionState & ~SelectionStateHoverOnLeft);
							m_thumbnailControls[currentIndex]->SetSelectionState(selectionState);
							redraw = true;
						}
						//	// Only toggle hover if necessary
						if ((selectionState & SelectionStateHoverOnRight) == SelectionStateHoverOnRight)
						{
							selectionState = static_cast<ThumbnailSelectionState>(selectionState & ~SelectionStateHoverOnRight);
							m_thumbnailControls[currentIndex]->SetSelectionState(selectionState);
							redraw = true;
						}
					}
				}
			}
		}
		if (redraw)
		{
			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		return;
	}

}
void CWhiteBoardDlg::InputProcessUp(D2D1_POINT_2F mousePosition, LONG pressure) {
#ifdef STYLUS_ON
	//return;

	
	//m_renderTarget->
	//m_pDynamicRenderer

#endif

	if (m_pageMode == PAGE_EMPTYBOARD) {
		if (m_bIsDrawing)
		{
			ComPtr<IDrawGeometryOperation> drawGeometry;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&drawGeometry)))
			{
				drawGeometry->AppendPoint(m_renderTarget, GetAbsolutePosition(mousePosition), pressure);
				//drawGeometry->EndGeometry(m_renderTarget, GetAbsolutePosition(mousePosition), pressure);
			}
			//m_currentOperation = nullptr;
			if (!m_bUseDyniamcRenderer) {
				//::InvalidateRect(m_hWnd, nullptr, FALSE);
				//PostMessage(MY_WM_BEGINDRAW, mousePosition.x, WM_LBUTTONUP);
			}
			else {
				//PostMessage(MY_WM_BEGINDRAW, mousePosition.x, WM_LBUTTONUP);
			}
			m_bIsDrawing = false;

		}
		else if (m_bIsEraser) {
			m_bIsEraser = false;
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else {
			if (m_bIsDrag) {
				m_bIsDrag = false;
			}

			if (m_pageChanging == PAGE_PREV) {
				m_pageChanging = PAGE_NON;
				PreviousPage();
			}
			if (m_pageChanging == PAGE_NEXT) {
				m_pageChanging = PAGE_NON;
				NextPage();

			}

			if (EditorItemHitTest(GetAbsolutePosition(mousePosition), TRUE) == Direct2DUtility::HIT_IN) {
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
	}


	if (m_pageMode == PAGE_ITEM) {
		if (m_bIsDrawing)
		{
			ComPtr<IDrawGeometryOperation> drawGeometry;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&drawGeometry)))
			{
				drawGeometry->AppendPoint(m_renderTarget, GetAbsolutePosition(mousePosition), pressure);
				drawGeometry->EndGeometry(m_renderTarget, GetAbsolutePosition(mousePosition), pressure);
			}
			if (!m_bUseDyniamcRenderer) {
				//::InvalidateRect(m_hWnd, nullptr, FALSE);
				//PostMessage(MY_WM_BEGINDRAW, mousePosition.x, WM_LBUTTONUP);
				//OnBeginDraw(0, 0);
			}
			else {
				//PostMessage(MY_WM_BEGINDRAW, mousePosition.x, WM_LBUTTONUP);

			}
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
			//m_currentOperation = nullptr;
			m_bIsDrawing = false;

		}
		else if (m_bIsEraser) {
			m_bIsEraser = false;
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else if (m_bIsZoomming) {
			if (Direct2DUtility::GetRectWidth(m_zoomBox) > 0 && Direct2DUtility::GetRectHeight(m_zoomBox) > 0)
			{
				D2D1_POINT_2F zoomRectStart;
				D2D1_POINT_2F zoomRectEnd;
				D2D1_POINT_2F zoomRectCenter;
				zoomRectStart.x = std::min<float>(m_zoomBox.left, mousePosition.x);
				zoomRectStart.y = std::min<float>(m_zoomBox.top, mousePosition.y);
				zoomRectEnd.x = std::max<float>(m_zoomBox.left, mousePosition.x);
				zoomRectEnd.y = std::max<float>(m_zoomBox.top, mousePosition.y);
				zoomRectStart = GetAbsolutePosition(zoomRectStart);
				zoomRectEnd = GetAbsolutePosition(zoomRectEnd);
				zoomRectCenter.x = (zoomRectEnd.x - zoomRectStart.x) * 0.5;
				zoomRectCenter.y = (zoomRectEnd.y - zoomRectStart.y) * 0.5;

				m_currentZoom = std::min<float>(m_zoomMax, GetZoomming(zoomRectEnd.x - zoomRectStart.x, zoomRectEnd.y - zoomRectStart.y));
				//HRESULT hr = CalculatePanBoundary();

				D2D1_RECT_F imageRect;
				m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);

				float imageWidth = (imageRect.right - imageRect.left) * m_currentZoom;
				float imageHeight = (imageRect.bottom - imageRect.top) * m_currentZoom;

				float clientWidth = m_renderTarget->GetSize().width;
				float clientHeight = m_renderTarget->GetSize().height;

				float boundaryLeft = (clientWidth / 2) - (imageWidth / 2);
				float buondaryRight = boundaryLeft * -1;
				float boundaryTop = (clientHeight / 2) - (imageHeight / 2);
				float boundaryBottom = boundaryTop * -1;

				D2D1_RECT_F realsize;
				m_thumbnailControls[m_nCurrentPage]->GetClipRect(&realsize);

				/*
				이미지를 확대 했을때 화면에 보여지는 이미지는 정중앙이 기본값
				확대 했을때의 이미지와 화면에 보여지는 이미지 간극차이를 boundary 값으로 계산
				실제로 확대했을때 화면에 보여지는 이미지 위치를 posx 값으로 실제 이미지 크기와 확대 이미지 크기 비율을 계산
				확대 했을때 이미지를 화면에 보여줄때 원하는 좌표로 boundary 값과 pos 값으로 이동
				posCenter 값은 화면에 보여줘야 하는 이미지 영역을 가운데 위치로 이동
				*/ 
				
				float posCenterX = clientWidth * 0.5 - (zoomRectEnd.x - zoomRectStart.x) * 0.5 * imageWidth / realsize.right;
				float posCenterY = clientHeight * 0.5 - (zoomRectEnd.y - zoomRectStart.y) * 0.5 * imageHeight / realsize.bottom;
				float posx = zoomRectStart.x * imageWidth / realsize.right - posCenterX;
				float posy = zoomRectStart.y * imageHeight / realsize.bottom - posCenterY;
				

				m_bIsZoomming = false;
				m_currentTransX = buondaryRight - posx;
				m_currentTransY = boundaryBottom - posy;
				m_currentPanPoint.x = m_currentTransX;
				m_currentPanPoint.y = m_currentTransY;



				SetZoomAnimation();


				::InvalidateRect(m_hWnd, nullptr, FALSE);
				SetOperationType(m_prevOperationType);

				if (GetItemMode() == ITEM_EDIT && GetOperationType() == ImageOperationTypePen) {
					((CAbleClassDlg*)GetParent())->SetBoardMenuButtonStyle(IDC_BTN_PEN, TRUE);
				}
				else {
					((CAbleClassDlg*)GetParent())->SetBoardMenuButtonStyle(IDC_BTN_EXPAND, FALSE);
				}

			}
			else {
				m_bIsZoomming = false;
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else if (m_bIsClipping) {
			if (Direct2DUtility::GetRectWidth(m_clippingBox) > 0 && Direct2DUtility::GetRectHeight(m_clippingBox) > 0)
			{
				m_clippingBox.right = mousePosition.x;
				m_clippingBox.bottom = mousePosition.y;
				m_clippingBox.left += 2;
				m_clippingBox.top += 2;
				m_clippingBox.right -= 2;
				m_clippingBox.bottom -= 2;

				m_clippingBox = Direct2DUtility::FixRect(m_clippingBox);

				// 빈칠판 모드 변경전 먼저 저장
				SetOperationType(m_prevOperationType);

				AddEmptyBoardItem();
				ULONG newItem = m_emptyBoard.size() - 1;
				m_emptyBoard[newItem]->SetClippingImage(m_renderTarget, Direct2DUtility::GetRectForCurrentDPI(m_clippingBox));
				m_nEmptyBoardPage = newItem;
				m_bIsClipping = false;

				SetEmptyBoardAnimation();
				//m_operationType = m_prevOperationType;
				//DoEmptyBoardMode();
				((CAbleClassDlg*)GetParent())->SetEmptyBoard();

				
				SetDlgVisible(SW_HIDE);


				if (GetItemMode() == ITEM_EDIT && GetOperationType() == ImageOperationTypePen) {
					((CAbleClassDlg*)GetParent())->SetBoardMenuButtonStyle(IDC_BTN_PEN, TRUE);
				}
				else {
					((CAbleClassDlg*)GetParent())->SetBoardMenuButtonStyle(IDC_BTN_CLIPPING, FALSE);
				}

				::InvalidateRect(m_hWnd, nullptr, FALSE);
				
				
			}
			else {
				m_bIsClipping = false;
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
		else {
			if (m_bIsDrag) {
				m_bIsDrag = false;
			}

			if (m_pageChanging == PAGE_PREV) {
				m_pageChanging = PAGE_NON;
				PreviousPage();
			}
			if (m_pageChanging == PAGE_NEXT) {
				m_pageChanging = PAGE_NON;
				NextPage();
		
			}

			if (EditorItemHitTest(GetAbsolutePosition(mousePosition), TRUE) == Direct2DUtility::HIT_IN) {
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}
	}

	if (m_pageMode == PAGE_LIST) {
		int currentIndex;
		int endIndex;

		int nCurrentPageStartIndex;
		int nCurrentPageEndIndex;
		HRESULT hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);
		if (SUCCEEDED(hr))
		{
			// Determine if one of the visible thumbnail controls was clicked
			for (currentIndex; currentIndex < endIndex; currentIndex++)
			{
				D2D1_RECT_F rect;

				hr = m_thumbnailControls[currentIndex]->GetThumbnailRect(&rect);
				if (SUCCEEDED(hr))
				{
					if (Direct2DUtility::HitTest(rect, mousePosition) == Direct2DUtility::HIT_IN)
					{
						DoItemMode(currentIndex);
						//DoModeChange(PAGE_ITEM, currentIndex);
						break;
					}
				}
			}
		}
	}
}




LRESULT CWhiteBoardDlg::OnBeginDraw(WPARAM wParam, LPARAM lParam) {
	LRESULT hr = 0L;
	//return hr;
	if (!m_bUseDyniamcRenderer || lParam == WM_LBUTTONUP) {
		if (GetOperationType() == ImageOperationTypePen) {
			if (m_currentOperation != nullptr) {
				ComPtr<IDrawGeometryOperation> drawGeometry;
				if (SUCCEEDED(m_currentOperation.QueryInterface(&drawGeometry)))
				{
					//if (m_bUseDyniamcRenderer && lParam == WM_LBUTTONUP) {
					//	for (int i = 0; i < 2; i++)
					//		m_pDynamicRenderer->ReleaseCachedData(i);
					//}

					//if (lParam == WM_LBUTTONUP) {
					//	TRACE(L"\n>>> lbutton up <<<<<\n");
					//}

					//if (SUCCEEDED(m_renderTarget->EndDraw())
					hr = CreateDeviceResources();

					if (FAILED(hr)) return 0L;


					m_renderTarget->BeginDraw();
					//m_renderTarget->Clear(D2D1::ColorF(BackgroundColor));

					D2D1_MATRIX_3X2_F originalTransform;
					if (GetPageMode() == PAGE_EMPTYBOARD) {
						D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(m_emptyBoardZoom, m_emptyBoardZoom, GetCenter());
						D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(m_emptyBoardPanPoint.x, m_emptyBoardPanPoint.y);
						m_renderTarget->SetTransform(scale * translation);
						m_renderTarget->GetTransform(&originalTransform);
					}
					else {
						m_renderTarget->GetTransform(&originalTransform);
					}



					//TRACE(L"\n>>> ori (%f,%f,%f,%f,%f,%f,) <<<\n",
					//	originalTransform.dx, originalTransform.dy,
					//	originalTransform.m11, originalTransform.m12,
					//	originalTransform.m21, originalTransform.m22
					//);


					D2D1_RECT_F						m_clipRect;			// 실제 이미지 크기 값
					D2D1_RECT_F						m_drawingRect;		// 기본 출력 이미지 영역 좌표 값 (확대, 축소 미적용)
					GetCurrentThumbnailPtr()->GetClipRect(&m_clipRect);
					GetCurrentThumbnailPtr()->GetDrawingRect(&m_drawingRect);

					D2D1_RECT_F rtSource = { 400, 500, 900, 1500 };
					//D2D1_RECT_F rtDest = { wParam - 40, lParam - 40, wParam + 40, lParam + 40 };

					D2D1_RECT_F rtDest = { 400, 500, 900, 1500};
					//drawGeometry->GetDrawingRect(&rtDest);
					//rtDest = GetScreenRect(rtDest);

					//TRACE(L"\n>>> rect(%.02f, %.02f, %.02f, %.02f) <<<\n", rtDest.left, rtDest.top, rtDest.right, rtDest.bottom);

					m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

					if (m_renderTargetBitmap) {
						m_renderTarget->DrawBitmap(m_renderTargetBitmap, D2D1::RectF(0, 0, m_renderTargetBitmap->GetSize().width, m_renderTargetBitmap->GetSize().height));
					}
					//m_renderTarget->DrawBitmap(m_renderTargetBitmap, rtDest, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rtSource);


					D2D1_POINT_2F midPoint = Direct2DUtility::GetMidPoint(m_drawingRect);
					float scale = GetCurrentThumbnailPtr()->GetImageScale();

					//TRACE(L"\n>>> scale (%f) <<<<<\n", scale);

					D2D1_MATRIX_3X2_F transform =
						D2D1::Matrix3x2F::Translation(-m_clipRect.left, -m_clipRect.top) *
						D2D1::Matrix3x2F::Scale(1 / scale, 1 / scale) *
						D2D1::Matrix3x2F::Translation(m_drawingRect.left, m_drawingRect.top) * originalTransform;
					/*GetTransformationsReversed(midPoint) **/

					//TRACE(L"\n>>>> (%.02f,%.02f) mid(%.02f,%.02f) scal(%.02f) (%.02f,%.02f) <<<<\n",
					//	m_clipRect.left, m_clipRect.top, midPoint.x, midPoint.y, scale, 
					//	m_drawingRect.left, m_drawingRect.top);

					m_renderTarget->SetTransform(transform);

					//drawGeometry->DrawToRenderTarget(m_renderTarget);
					drawGeometry->DynamicDrawToRenderTarget(m_renderTarget);
					m_renderTarget->SetTransform(originalTransform);
					hr = m_renderTarget->EndDraw();
					if (hr == D2DERR_RECREATE_TARGET)
					{
						hr = S_OK;
						DiscardDeviceResources();
					}
					//TRACE("\n>>>> draw (%d) <<<<\n", GetOperationType());

				}
			}
			//::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
		else {
			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
	}
	else {
	}
	//return hr;

	//m_pDynamicRenderer->Refresh();

	

	
	return hr;
}





void CWhiteBoardDlg::ShowMessageAnimation(std::wstring strMessage) {
	if (m_tlMessage)
		m_tlMessage = nullptr;

	if (m_tlMessage == nullptr) {
		D2D1_SIZE_F size = m_renderTarget->GetSize();

		HRESULT hr = m_dWriteFactory->CreateTextLayout(
			strMessage.c_str(),
			strMessage.length(),
			m_tfMessage,
			size.width,
			size.height,
			&m_tlMessage);

		hr = m_tlMessage->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
		hr = m_tlMessage->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		hr = m_tlMessage->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	//::InvalidateRect(m_hWnd, nullptr, FALSE);
}



void CWhiteBoardDlg::LoadDbData() {
	std::vector<CDataStructBook> vBook;
	((CAbleClassDlg*)GetParent())->GetBookData(vBook);

	if (m_currentOperation) m_currentOperation = nullptr;

	if (vBook.size() > 0) {
		for (auto bookData = vBook.begin(); bookData != vBook.end(); bookData++)
		{
			HRESULT hr = SharedObject<CEditorItemOperation>::Create(&m_currentOperation);

			ComPtr<IEditorItemOperation> editorItem;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&editorItem)))
			{
				D2D1_RECT_F rect;
				rect.left = (*bookData).book_left;
				rect.top = (*bookData).book_top;
				rect.right = (*bookData).book_right;
				rect.bottom = (*bookData).book_bottom;
				editorItem->SetType(static_cast<eItemType>((*bookData).book_type));
				editorItem->SetRect(m_renderTarget, rect);
				editorItem->SetBitmap(m_renderTarget);
				editorItem->SetColor((*bookData).book_color_bg, (*bookData).book_color_border);
				editorItem->SetFile((*bookData).book_path);

				int bookPage = (*bookData).book_page;
				if (bookPage > -1 && bookPage < m_thumbnailControls.size()) {
					m_thumbnailControls[bookPage]->PushImageOperation(m_currentOperation);
				}
				

			}
			m_currentOperation = nullptr;
		}		
	}
}


int CWhiteBoardDlg::EditorItemHitTest(D2D1_POINT_2F mousePoint, BOOL bClick) {
	int isHit = Direct2DUtility::HIT_NON;

	ComPtr<IImageOperation> hitOperation;


	if (GetCurrentThumbnailPtr()->EditorItemHistTest(mousePoint, bClick, &hitOperation) == S_OK) {

		SetCursor(LoadCursor(NULL, IDC_HAND));
		if (bClick) {
				if (hitOperation) {
					ComPtr<IEditorItemOperation> editorOperation;
					if (SUCCEEDED(hitOperation.QueryInterface(&editorOperation)))
					{
						isHit = Direct2DUtility::HIT_IN;

						eItemType etype = editorOperation->GetType();
						if (etype == EDITOR_VIDEO) {
							std::wstring file;
							file = editorOperation->GetFile();
							VideoClipDialog(SW_SHOW);
							MediaPlay(file);
						}
						else if (etype == EDITOR_AUDIO) {
							std::wstring file;
							file = editorOperation->GetFile();
							VideoClipDialog(SW_HIDE);
							MediaPlay(file);
						}
						else if (etype == EDITOR_BLANK) {
							editorOperation->SetVisible(SW_HIDE);
						}
					}
				}
			//}
			

		}
	}

	return isHit;
}



LRESULT CWhiteBoardDlg::OnVideoClipClick(WPARAM wParam, LPARAM lParam) {
	LRESULT hr = 0L;
	MediaStop();
	VideoClipDialog(SW_HIDE);
	return hr;
}

HRESULT CWhiteBoardDlg::MediaPlay(std::wstring file) {
	HRESULT hr = S_OK;


	std::wstring path = m_MyDocument;
	path.append(L"\\media\\");
	path.append(file);

	if (m_mediaPlayer == nullptr)
	{
		m_mediaPlayerCallBack = new MediaPlayerCallback();

		if (m_mediaPlayerCallBack == nullptr)
		{
			hr = E_OUTOFMEMORY;
		}

		if (SUCCEEDED(hr))
		{
			hr = MFPCreateMediaPlayer(NULL,
				FALSE,						// Start playback automatically
				MFP_OPTION_NONE,			// Flags
				m_mediaPlayerCallBack,		// Callback pointer
				m_videoClip.GetSafeHwnd(),		// Video window
				&m_mediaPlayer
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_mediaPlayer->CreateMediaItemFromURL(path.c_str(), FALSE, 0, NULL);

		}
	}
	else {
		m_mediaPlayer->ClearMediaItem();
		m_mediaPlayer->CreateMediaItemFromURL(path.c_str(), FALSE, 0, NULL);

		MFP_MEDIAPLAYER_STATE state = MFP_MEDIAPLAYER_STATE_EMPTY;
		hr = m_mediaPlayer->GetState(&state);

		if (SUCCEEDED(hr))
		{
			if (state == MFP_MEDIAPLAYER_STATE_PAUSED || state == MFP_MEDIAPLAYER_STATE_STOPPED)
			{
				hr = m_mediaPlayer->Play();
			}
			else if (state == MFP_MEDIAPLAYER_STATE_PLAYING)
			{
				VideoClipDialog(SW_HIDE);
				hr = m_mediaPlayer->Stop();
			}
		}
	}


	return hr;
}

void CWhiteBoardDlg::MediaStop() {
	MFP_MEDIAPLAYER_STATE state = MFP_MEDIAPLAYER_STATE_EMPTY;

	HRESULT hr = m_mediaPlayer->GetState(&state);

	if (SUCCEEDED(hr))
	{
		if (state == MFP_MEDIAPLAYER_STATE_PLAYING)
		{
			hr = m_mediaPlayer->Pause();
		}
	}
}

void CWhiteBoardDlg::MediaPlayerCallback::OnMediaPlayerEvent(_In_ MFP_EVENT_HEADER * pEventHeader)
{
	if (FAILED(pEventHeader->hrEvent))
	{
		return;
	}

	switch (pEventHeader->eEventType)
	{
	case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
		CWhiteBoardDlg::OnMediaItemCreated(MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader));
		break;

	case MFP_EVENT_TYPE_MEDIAITEM_SET:
		CWhiteBoardDlg::OnMediaItemSet(MFP_GET_MEDIAITEM_SET_EVENT(pEventHeader));
		break;
	}
}



void CWhiteBoardDlg::OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent)
{
	HRESULT hr = S_OK;

	// The media item was created successfully.

	if (m_mediaPlayer)
	{
		BOOL bHasVideo = FALSE, bIsSelected = FALSE;

		// Check if the media item contains video.
		hr = pEvent->pMediaItem->HasVideo(&bHasVideo, &bIsSelected);

		if (FAILED(hr)) {
			return;
		}

		//g_bHasVideo = bHasVideo && bIsSelected;

		// Set the media item on the player. This method completes asynchronously.
		hr = m_mediaPlayer->SetMediaItem(pEvent->pMediaItem);
	}
}

void CWhiteBoardDlg::OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT * /*pEvent*/)
{
	HRESULT hr = m_mediaPlayer->Play();

	if (FAILED(hr))
	{
	}
}

void CWhiteBoardDlg::VideoClipDialog(UINT visible) {
	if (visible == SW_SHOW) {

		CRect rt;
		GetClientRect(&rt);
		LONG nCenter = static_cast<LONG>((rt.right - rt.left) * 0.5);
		m_videoClip.MoveWindow(static_cast<int>(nCenter * 0.5), 150, nCenter, rt.bottom - 300);
		m_videoClip.ShowWindow(visible);
		m_videoClip.UpdateWindow();

	}
	else {
		m_videoClip.MoveWindow(0,0,1,1);
		m_videoClip.ShowWindow(visible);
		m_videoClip.UpdateWindow();
	}
	
	


}




void CWhiteBoardDlg::PrintPageNumber() {


	std::string s = fmt::format("{:03} / {:03}", GetCurrentPage() + 1, GetLastPage());

	((CAbleClassDlg*)GetParent())->SetBookMarkPage(g_global.s2ws(s));
}


void CWhiteBoardDlg::SetOperationType(ImageOperationType type) {
	if (type == ImageOperationTypeClipping) {
		SetDlgVisible(SW_SHOW, L"캡처할 영역을 선택하세요");
	} 
	else if (type == ImageOperationTypeZoom) {
		SetDlgVisible(SW_SHOW, L"확대할 영역을 선택하세요");
	}
	else SetDlgVisible(SW_HIDE);


	if (GetPageMode() == PAGE_ITEM) {
		m_operationType = type;
		SetDynamicRendererMode(TRUE);
	}
	else if (GetPageMode() == PAGE_EMPTYBOARD) {
		m_operationTypeEmptyBoard = type;
		SetDynamicRendererMode(TRUE);
	}


}


void CWhiteBoardDlg::ResetZoomData() {
	AnimationUtility::AbandonAllStoryboards(PAGE_ITEM);
	m_currentZoom = 1.0f;
	m_currentTransX = 0;
	m_currentTransY = 0;
	m_beforeZoom = 1.0f;
	m_beforeTransX = 0;
	m_beforeTransY = 0;
	m_currentPanPoint.x = 0;
	m_currentPanPoint.y = 0;
	m_beforeAlpha = 1.f;
	m_currentAlpha = 1.f;
}

void CWhiteBoardDlg::ResetBookData() {
	m_pageMode = PAGE_ITEM;
	m_itemMode = ITEM_VIEW;
	m_itemModeEmptyBoard = ITEM_EDIT;
	m_operationType = ImageOperationTypeNone;
	m_operationTypeEmptyBoard = ImageOperationTypePen;
	ResetZoomData();
	m_nEmptyBoardPage = 0;
	m_emptyBoard.clear();

}


// 현재 그리기 모드 인지 확인 

ImageOperationType CWhiteBoardDlg::GetOperationType() {
	if (GetPageMode() == PAGE_EMPTYBOARD) {
		return m_operationTypeEmptyBoard;
	}
	else {
		return m_operationType;
	}
}

bool CWhiteBoardDlg::IsOperationTypePen() {
	return (GetPageMode() == PAGE_LIST) ? false : (GetOperationType() == ImageOperationTypePen);
}




void CWhiteBoardDlg::GetColorInfoFromDatabase() {
	for (int i = 0; i < 6; i++) {
		m_penColorPalette[i].clr = g_database.GetSetting()->set_ink_color[i];
		m_penColorPalette[i].b = (g_database.GetSetting()->set_ink_color_on[i] == 1)?true:false;

	}
}

int CWhiteBoardDlg::NextPenColor() {

	int curPen = GetPen()->GetColorIndex();
	int selPen = 0;

	int i = 0;
	while (i++<6) {
		curPen++;
		if (curPen > 5) curPen = 0;

		if (m_penColorPalette[curPen].b == true) {
			selPen = curPen;
			break;
		}
	}

	return selPen;
}

void CWhiteBoardDlg::DoReversal() {

	for (auto thumbnail = m_thumbnailControls.begin(); thumbnail != m_thumbnailControls.end(); thumbnail++)
	{
		(*thumbnail)->SetReversal(m_bReversal);
	}
}


void CWhiteBoardDlg::SetDlgVisible(UINT visible, std::wstring strMessage) {
	//L"캡처할 영역을 선택해주세요"
	if (!strMessage.empty() && visible == SW_SHOW) {
		m_setDlg.SetMessage(strMessage);
	}
	m_setDlg.ShowWindow(visible);
	m_setDlg.UpdateWindow();
}



void CWhiteBoardDlg::DoSpline() {
	if (m_currentOperation) m_currentOperation = nullptr;

	m_bIsDrawing = true;
	HRESULT hr = SharedObject<DrawGeometryOperation>::Create(&m_currentOperation);
	ComPtr<IDrawGeometryOperation> drawGeometry;
	if (SUCCEEDED(m_currentOperation.QueryInterface(&drawGeometry)))
	{

		drawGeometry->SetBrushColor(D2D1::ColorF(RGB(GetBValue(GetPenColor()), GetGValue(GetPenColor()), GetRValue(GetPenColor()))));
		drawGeometry->SetBrushAlpha(GetPen()->GetAlphaValue());
		drawGeometry->SetStrokeSize(1);
		drawGeometry->SetPenStyleValue(GetPen()->GetPentipStyle(), GetPen()->GetPressure());
		m_thumbnailControls[0]->PushImageOperation(m_currentOperation);
		D2D1_POINT_2F mousePosition;

		drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1288.29, 1045.16), 1);
	
		drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1298.64, 1065.85), 1);
  
	  drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1315.89, 1121.04), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1336.58, 1162.43), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1353.83, 1186.58), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1364.18, 1210.73), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1367.63, 1234.87), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1367.63, 1269.36), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1364.18, 1300.41), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1353.83, 1331.45), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1346.93, 1365.95), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1346.93, 1390.09), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1346.93, 1414.24), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1346.93, 1438.38), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1346.93, 1455.63), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1346.93, 1497.02), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1343.48, 1528.07), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1340.03, 1562.56), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1329.68, 1610.85), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1319.34, 1652.24), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1312.44, 1686.74), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1312.44, 1707.43), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1305.54, 1790.22), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1305.54, 1824.71), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1305.54, 1873.00), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1315.89, 1907.50), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1319.34, 1935.09), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1322.78, 1962.69), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1326.23, 1990.28), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1326.23, 2017.88), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1333.13, 2038.57), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1336.58, 2066.17), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1340.03, 2090.31), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1343.48, 2104.11), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1346.93, 2121.36), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1346.93, 2138.60), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1350.38, 2145.50), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1350.38, 2159.30), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1353.83, 2166.20), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1353.83, 2173.10), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1353.83, 2176.55), 1);

	drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1353.83, 2180.00), 1);



		/*drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1300, 400), 1);
		drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1400, 700), 1);
		drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1350, 900), 1);
		drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1450, 1100), 1);
		drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1400, 1500), 1);
		drawGeometry->AppendPoint(m_renderTarget, D2D1::Point2F(1300.83, 2180.00), 1);*/
		drawGeometry->EndGeometry(m_renderTarget, D2D1::Point2F(1353.83, 2180.00), 1);
		m_currentOperation = nullptr;
		m_bIsDrawing = false;
	}
}



