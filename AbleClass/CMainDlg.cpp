// CMainDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CMainDlg.h"
#include "afxdialogex.h"

#include "FlyerAnimation.h"
#include "MoverAnimation.h"
#include "PanAnimation.h"
#include "ImageThumbnailControl.h"

// CMainDlg 대화 상자

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_DLG, pParent),
	m_lastAnimationSize(D2D1::SizeF(0.0f, 0.0f)),
	m_currentAnimation(MoveAround),
	m_strRootDir(L"\\PoDo"),
	m_frameIndex(1),
	m_pageCount(0),
	m_currentPage(1),
	m_beforePage(1),
	m_mouseOverPage(0),
	m_scale(1.f),
	m_hoverOnRect(D2D1::RectF(0,0,0,0)),
	m_bIsPageAnimation(false),
	m_fScroll(0.f),
	m_fScrollDistance(0.0f),
	m_previousPosition(0.f),
	m_nextPosition(0.f),
	m_bDeleteMode(false),
	m_bTouch(FALSE)
{
	memset(m_defaultBitmapSizeMap, 0, sizeof(m_defaultBitmapSizeMap));
}

CMainDlg::~CMainDlg()
{
	DiscardDeviceResources();
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_TOUCH, OnTouch)
	ON_MESSAGE(WM_POINTERDOWN, OnPointerDown)
	ON_MESSAGE(WM_POINTERUPDATE, OnPointerUpdate)
	ON_MESSAGE(WM_POINTERUP, OnPointerUp)
END_MESSAGE_MAP()


// CMainDlg 메시지 처리기


int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;


	::RegisterTouchWindow(GetSafeHwnd(), 0);

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


	return 0;
}


void CMainDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		m_renderTarget->BeginDraw();
		m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();

		// Paint background
		m_renderTarget->Clear(D2D1::ColorF(0x000000));
		D2D1_RECT_F rect = { 0, 0, renderTargetSize.width, renderTargetSize.height };
		m_renderTarget->DrawBitmap(m_backgroundBitmap, rect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, m_bitmapRect);

		//DrawGif();

		if (m_bIsHoverAnimation || m_bIsPageAnimation) {
			hr = AnimationUtility::UpdateAnimationManagerTime(PAGE_ITEM);

			bool isBusy = false;
			AnimationUtility::IsAnimationManagerBusy(PAGE_ITEM, &isBusy);
			if (!isBusy)
			{
				m_bIsHoverAnimation = false;

				if (m_bIsPageAnimation) {
					m_bIsPageAnimation = false;
					m_varAniTransY = nullptr;
					m_fScroll = 0;
					m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);
				}

			}
			else {
				if (m_bIsHoverAnimation) {
					RECT redrawRect = { m_hoverOnRect.left, m_hoverOnRect.top, m_hoverOnRect.right, m_hoverOnRect.bottom };
					::InvalidateRect(m_hWnd, &redrawRect, FALSE);
				}
				else if (m_bIsPageAnimation) {
					::InvalidateRect(m_hWnd, nullptr, FALSE);
				}
			}
			OnPaintListAnimation();
			
		}
		else {
			OnPaintList();
		}
		//m_renderTarget->BeginDraw();
		//m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		//D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();

		//// Paint background
		//m_renderTarget->Clear(D2D1::ColorF(0x000000));
		//D2D1_RECT_F rect = { 0, 0, renderTargetSize.width, renderTargetSize.height };
		//m_renderTarget->DrawBitmap(m_backgroundBitmap, rect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, m_bitmapRect);

		////DrawGif();

		//hr = AnimationUtility::UpdateAnimationManagerTime(PAGE_ITEM);

		//bool isBusy = false;
		//AnimationUtility::IsAnimationManagerBusy(PAGE_ITEM, &isBusy);
		//if (!isBusy)
		//{
		//	m_bIsHoverAnimation = false;
		//}
		//else {
		//	if (m_bIsHoverAnimation) {
		//		RECT redrawRect = { m_hoverOnRect.left, m_hoverOnRect.top, m_hoverOnRect.right, m_hoverOnRect.bottom };
		//		::InvalidateRect(m_hWnd, &redrawRect, FALSE);
		//	}
		//}

		//OnPaintList();
	}

	hr = m_renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}
}


void CMainDlg::OnPaintList() {
	HRESULT hr = S_OK;
	D2D1_SIZE_F size = m_renderTarget->GetSize();


	if (m_thumbnailControls.size() <= 0)
		return;


	int currentIndex;
	int endIndex;
	int nCurrentPageStartIndex;
	int nCurrentPageEndIndex;
	float startPointY = Direct2DUtility::ScaleValueForNoDPI(165);
	hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);

	
	D2D1_RECT_F rect;
	//m_thumbnailControls[currentIndex]->GetThumbnailRect(&rect);
	//m_previousPosition = rect.top;
	//m_thumbnailControls[nCurrentPageEndIndex]->GetThumbnailRect(&rect);
	//m_nextPosition = rect.top;


	if (nCurrentPageStartIndex >= m_thumbnailControls.size()) {
		nCurrentPageStartIndex = m_thumbnailControls.size() - 1;
	}

	if (SUCCEEDED(hr))
	{
		
		m_thumbnailControls[nCurrentPageStartIndex]->GetThumbnailRect(&rect);
		startPointY = rect.top - Direct2DUtility::ScaleValueForNoDPI(50);
	}

	D2D1_POINT_2F startPoint = D2D1::Point2F(size.width * 0.5, startPointY);
	D2D1_POINT_2F endPoint = D2D1::Point2F(size.width * 0.5, size.height- Direct2DUtility::ScaleValueForNoDPI(20));

	m_pGradientAppearBrush->SetStartPoint(startPoint);
	m_pGradientAppearBrush->SetEndPoint(endPoint);

	m_renderTarget->PushLayer(
		D2D1::LayerParameters(
			D2D1::InfiniteRect(),
			NULL,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			D2D1::IdentityMatrix(),
			1.0,
			m_pGradientAppearBrush,
			D2D1_LAYER_OPTIONS_NONE),
		m_pLayer
	);

	if (SUCCEEDED(hr))
	{
		D2D1_MATRIX_3X2_F oriTrans;
		m_renderTarget->GetTransform(&oriTrans);

		D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Translation(0, 0);
		m_renderTarget->SetTransform(transform);
		while (currentIndex < endIndex)
		{
			m_thumbnailControls[currentIndex++]->DrawBook(0);
		}

		m_renderTarget->SetTransform(oriTrans);
	}


	OnPaintPageIcon();

	m_renderTarget->PopLayer();
	
}

void CMainDlg::OnPaintListAnimation() {
	HRESULT hr = S_OK;
	D2D1_SIZE_F size = m_renderTarget->GetSize();

	int currentIndex;
	int endIndex;
	int nCurrentPageStartIndex;
	int nCurrentPageEndIndex;
	float startPointY = Direct2DUtility::ScaleValueForNoDPI(165);
	hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);


	if (m_thumbnailControls.size() <= 0) 
		return;

	if (SUCCEEDED(hr))
	{
		D2D1_RECT_F rect;
		m_thumbnailControls[nCurrentPageStartIndex]->GetThumbnailRect(&rect);
		startPointY = rect.top - Direct2DUtility::ScaleValueForNoDPI(50);
	}

	D2D1_POINT_2F startPoint = D2D1::Point2F(size.width * 0.5, startPointY);
	D2D1_POINT_2F endPoint = D2D1::Point2F(size.width * 0.5, size.height - Direct2DUtility::ScaleValueForNoDPI(20));

	m_pGradientAppearBrush->SetStartPoint(startPoint);
	m_pGradientAppearBrush->SetEndPoint(endPoint);

	m_renderTarget->PushLayer(
		D2D1::LayerParameters(
			D2D1::InfiniteRect(),
			NULL,
			D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
			D2D1::IdentityMatrix(),
			1.0,
			m_pGradientAppearBrush,
			D2D1_LAYER_OPTIONS_NONE),
		m_pLayer
	);

	if (SUCCEEDED(hr))
	{
		D2D1_MATRIX_3X2_F oriTrans;
		m_renderTarget->GetTransform(&oriTrans);


		DOUBLE valueY = 0;
		if (m_varAniTransY) m_varAniTransY->GetValue(&valueY);
		//m_fScroll = valueY;

		//TRACE(_T("\n>>>>>>> ani y value (%.0f) <<<<<<\n"), valueY);


		D2D1_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Translation(0, 0);
		m_renderTarget->SetTransform(transform);
		while (currentIndex < endIndex)
		{
			m_thumbnailControls[currentIndex++]->DrawBook(valueY);
		}

		m_renderTarget->SetTransform(oriTrans);
	}


	OnPaintPageIcon();

	m_renderTarget->PopLayer();

}


void CMainDlg::OnPaintPageIcon() {
	int nPageCount = m_pageCount;
	int nCurrentPageIndex = m_currentPage - 1;
	float pageIconSize = 10;
	float pageIconPadding = 20;
	D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();
	float vCenter = renderTargetSize.height * 0.5 - ((pageIconSize + pageIconPadding) * nPageCount) * 0.5;

	bool bSelected = false;


	D2D1_ELLIPSE ellipse = D2D1::Ellipse(
		D2D1::Point2F(renderTargetSize.width - 30, vCenter),
		pageIconSize, pageIconSize);

	D2D1_RECT_F rect;
	m_pageIconRect.clear();

	for (int i = 0; i < nPageCount; ++i) {
		bSelected = (i == nCurrentPageIndex) ? true : false;


		if (bSelected) {
			m_brushPageIcon->SetColor(D2D1::ColorF(D2D1::ColorF::White));
		}
		else {
			m_brushPageIcon->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
		}

		//if (i == m_mouseOverPage) {
		//	m_brushPageIcon->SetColor(D2D1::ColorF(D2D1::ColorF::Red));
		//}

		rect.left = ellipse.point.x - ellipse.radiusX;
		rect.right = ellipse.point.x + ellipse.radiusX;
		rect.top = ellipse.point.y - ellipse.radiusY;
		rect.bottom = ellipse.point.y + ellipse.radiusY;
		m_pageIconRect.push_back(rect);
		m_renderTarget->DrawEllipse(&ellipse, m_brushPageIconBorder);
		m_renderTarget->FillEllipse(&ellipse, m_brushPageIcon);
		ellipse.point.y += (pageIconSize + pageIconPadding);
	}
	

}


void CMainDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_renderTarget)
	{
		// 화면의 사이즈 cx, cy m_renderTarget->GetSize() 적용
		// 화면의 사이즈는 실제 사이즈이고 m_renderTarget 의 사이즈는 DPI 적용된 사이즈
		// 화면 사이즈를 바로 적용하면 dpi 사이즈가 적용된 m_renderTarget 사이즈와 다름
		// 화면 사이즈를 m_renderTarget 사이즈로 변환 후 사용
		m_renderTarget->Resize(D2D1::SizeU(cx, cy));
		D2D1_SIZE_F rederTargetSize = m_renderTarget->GetSize();
		m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, rederTargetSize.width, rederTargetSize.height));
		SetPageCount();
		m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
}


BOOL CMainDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return FALSE;
}


BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	IShellItem *pShellItemDir;
	HRESULT hr = ::SHCreateItemInKnownFolder(FOLDERID_Documents, 0, nullptr, IID_PPV_ARGS(&pShellItemDir));

	if (SUCCEEDED(hr)) {
		hr = SetCurrentLocationList(pShellItemDir, false);
		if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == hr)
		{
		}
	}


	SetPageCount();
	//SetTimer(1, 200, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}


HRESULT CMainDlg::CreateDeviceIndependentResources()
{
	HRESULT hr = Direct2DUtility::GetD2DFactory(&m_d2dFactory);

	if (SUCCEEDED(hr))
	{
		hr = Direct2DUtility::GetDWriteFactory(&m_dWriteFactory);
	}


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
			16,
			L"ko-KR",
			&m_textFormatList);
	}

	if (SUCCEEDED(hr))
	{
		// Create an arbitary text layout object
		// Width and height will be determined later.
		hr = m_dWriteFactory->CreateTextLayout(
			initialTextList,
			ARRAYSIZE(initialTextList) - 1,
			m_textFormatList,
			50,
			50,
			&m_textLayoutList);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_textLayoutList->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_textLayoutList->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_textLayoutList->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}


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



	// Create animation objects
	if (SUCCEEDED(hr))
	{
		hr = AnimationUtility::GetAnimationManager(PAGE_ITEM, &m_animationManager);
	}

	if (SUCCEEDED(hr))
	{
		hr = AnimationUtility::GetTransitionLibrary(PAGE_ITEM, &m_transitionLibrary);
	}


	return hr;
}


//
//  This method creates resources which are bound to a particular Direct2D render target
//
HRESULT CMainDlg::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (m_renderTarget == nullptr)
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

		if (SUCCEEDED(hr))
		{
			ComPtr<ID2D1Bitmap>	bookBox;
			ComPtr<ID2D1Bitmap>	bookBoxEditNormal;
			ComPtr<ID2D1Bitmap>	bookBoxEditOver;
			ComPtr<ID2D1Bitmap>	bookBoxPlayNormal;
			ComPtr<ID2D1Bitmap>	bookBoxPlayOver;
			hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"main_menu/book_bg.png").c_str(), 0, 0, &bookBox);
			hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"main_menu/book_bg_edit_n.png").c_str(), 0, 0, &bookBoxEditNormal);
			hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"main_menu/book_bg_edit_o.png").c_str(), 0, 0, &bookBoxEditOver);
			hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"main_menu/book_bg_play_n.png").c_str(), 0, 0, &bookBoxPlayNormal);
			hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"main_menu/book_bg_play_o.png").c_str(), 0, 0, &bookBoxPlayOver);

			/************************************************************************************
			 1 : 메인 box
			 2 : edit normal
			 3 : play normal
			 4 : edit over
			 5 : play over
				+---------------+
				|		1		|
				|---------------|
				|	2	|	3	| normal line
				|---------------|
				|	4	|	5	| over line
				+---------------+
			************************************************************************************/

			float dpiX, dpiY;
			m_renderTarget->GetDpi(&dpiX, &dpiY);
			D2D1_SIZE_U bitmapPixelSize = D2D1::SizeU(bookBox->GetSize().width, bookBox->GetSize().height + bookBoxEditNormal->GetSize().height + bookBoxEditOver->GetSize().height);
			D2D1_PIXEL_FORMAT fmt = bookBox->GetPixelFormat();
			D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties();
			props.pixelFormat = fmt;
			props.dpiX = dpiX;
			props.dpiY = dpiY;
			HRESULT hr = m_renderTarget->CreateBitmap(bitmapPixelSize, props, &m_defaultThumbnailBitmap);
			if (SUCCEEDED(hr))
			{

				D2D1_RECT_U bitmapSizeMap[6] = { 0, };

				/*Direct2DUtility::GetRectForNoDPI(*/
				bitmapSizeMap[0] = D2D1::RectU(0, 0, bookBox->GetSize().width, bookBox->GetSize().height);
				bitmapSizeMap[1] = D2D1::RectU(0, 0, bookBoxEditNormal->GetSize().width, bookBoxEditNormal->GetSize().height);
				bitmapSizeMap[2] = D2D1::RectU(0, 0, bookBoxPlayNormal->GetSize().width, bookBoxPlayNormal->GetSize().height);
				bitmapSizeMap[3] = D2D1::RectU(0, 0, bookBoxEditOver->GetSize().width, bookBoxEditOver->GetSize().height);
				bitmapSizeMap[4] = D2D1::RectU(0, 0, bookBoxPlayOver->GetSize().width, bookBoxPlayOver->GetSize().height);
				bitmapSizeMap[5] = D2D1::RectU(0, 0, 26, 26);			// 그림자 사이즈

				// 1번 copy
				D2D1_POINT_2U topleft = D2D1::Point2U(0, 0);
				hr = m_defaultThumbnailBitmap->CopyFromBitmap(&topleft, bookBox, &bitmapSizeMap[0]);
				// 2번 copy
				topleft = D2D1::Point2U(0, bookBox->GetSize().height);
				hr = m_defaultThumbnailBitmap->CopyFromBitmap(&topleft, bookBoxEditNormal, &bitmapSizeMap[1]);
				// 3번 copy
				topleft = D2D1::Point2U(bookBoxEditNormal->GetSize().width, bookBox->GetSize().height);
				hr = m_defaultThumbnailBitmap->CopyFromBitmap(&topleft, bookBoxPlayNormal, &bitmapSizeMap[2]);
				// 4번 copy
				topleft = D2D1::Point2U(0, bookBox->GetSize().height + bookBoxEditNormal->GetSize().height);
				hr = m_defaultThumbnailBitmap->CopyFromBitmap(&topleft, bookBoxEditOver, &bitmapSizeMap[3]);
				// 5번 copy
				topleft = D2D1::Point2U(bookBoxEditOver->GetSize().width, bookBox->GetSize().height + bookBoxEditNormal->GetSize().height);
				hr = m_defaultThumbnailBitmap->CopyFromBitmap(&topleft, bookBoxPlayOver, &bitmapSizeMap[4]);

				bitmapSizeMap[0] = D2D1::RectU(0, 0, bookBox->GetSize().width, bookBox->GetSize().height);
				bitmapSizeMap[1] = D2D1::RectU(0, bitmapSizeMap[0].bottom, bookBoxEditNormal->GetSize().width, bitmapSizeMap[0].bottom + bookBoxEditNormal->GetSize().height);
				bitmapSizeMap[2] = D2D1::RectU(bitmapSizeMap[1].right, bitmapSizeMap[0].bottom, bitmapSizeMap[1].right + bookBoxPlayNormal->GetSize().width, bitmapSizeMap[0].bottom + bookBoxPlayNormal->GetSize().height);
				bitmapSizeMap[3] = D2D1::RectU(0, bitmapSizeMap[1].bottom, bookBoxEditOver->GetSize().width, bitmapSizeMap[1].bottom + bookBoxEditOver->GetSize().height - 1);
				bitmapSizeMap[4] = D2D1::RectU(bitmapSizeMap[3].right, bitmapSizeMap[2].bottom, bitmapSizeMap[3].right + bookBoxPlayOver->GetSize().width, bitmapSizeMap[2].bottom + bookBoxPlayOver->GetSize().height);

				m_defaultBitmapSizeMap[0] = Direct2DUtility::GetRectForNoDPI(bitmapSizeMap[0]);
				m_defaultBitmapSizeMap[1] = Direct2DUtility::GetRectForNoDPI(bitmapSizeMap[1]);
				m_defaultBitmapSizeMap[2] = Direct2DUtility::GetRectForNoDPI(bitmapSizeMap[2]);
				m_defaultBitmapSizeMap[3] = Direct2DUtility::GetRectForNoDPI(bitmapSizeMap[3]);
				m_defaultBitmapSizeMap[4] = Direct2DUtility::GetRectForNoDPI(bitmapSizeMap[4]);
				m_defaultBitmapSizeMap[5] = D2D1::RectF(bitmapSizeMap[5].left, bitmapSizeMap[5].top, bitmapSizeMap[5].right, bitmapSizeMap[5].bottom);

		
			}

			hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"main_menu/bg.jpg").c_str(), 0, 0, &m_backgroundBitmap);
			if (SUCCEEDED(hr)) {
				m_bitmapRect.left = 0;
				m_bitmapRect.top = 0;
				m_bitmapRect.right = m_backgroundBitmap->GetSize().width;
				m_bitmapRect.bottom = m_backgroundBitmap->GetSize().height;
			}
			//hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"v2/main_menu/left_line.png", 0, 0, &m_bgItem1);
			//hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"v2/main_menu/blend01.png", 0, 0, &m_bgItem2);
			//hr = Direct2DUtility::LoadBitmapFromFile(m_renderTarget, g_pSkin->GetSource(L"v2/main_menu/light_big.png", 0, 0, &m_bgItem3);

		}

		D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();

		if (SUCCEEDED(hr))
		{
			// Update thumbnail layout manager with current render target size
			m_thumbnailLayoutManager->SetBoundaryAlign(ALIGN_LEFT);
			m_thumbnailLayoutManager->SetBoundaryPadding(Direct2DUtility::GetRectForNoDPI(D2D1::RectF(g_pSkin->m_mainMenu.right + g_pSkin->m_mainMenu.left, g_pSkin->m_mainUpHeight + g_pSkin->m_mainMenu.top, 50, 0)), 26);
			m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height));
			m_thumbnailLayoutManager->SetThumbnailSize(g_pSkin->m_mainThumbnailSize.right, g_pSkin->m_mainThumbnailSize.bottom);
		}

		if (SUCCEEDED(hr))
		{
			CreateThumbnailCells(false);

			if (SUCCEEDED(hr))
			{
				hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(RGB(64, 64, 64), 1.)), &m_solidBrushList);
				hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_brushPageIcon);
				hr = m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &m_brushPageIconBorder);
				
			}
			

			// Set current render parameters
			m_renderingParametersList.solidBrush = m_solidBrushList;
			m_renderingParametersList.renderTarget = m_renderTarget;


			// Start background loading of thumbnails
			hr = m_AsyncLoaderHelper->StartBackgroundLoading();
		}


		if (SUCCEEDED(hr))
		{
			m_renderTarget->CreateLayer(NULL, &m_pLayer);


			CComPtr<ID2D1GradientStopCollection> pGradientAppear;
			static const D2D1_GRADIENT_STOP gradientAppear[] =
			{
			{ 0.f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f) },
			{ 0.05f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f) },
			{ 0.95f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f) },
			{ 1.0f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f) },
			};
			hr = m_renderTarget->CreateGradientStopCollection(
				gradientAppear,
				4,
				&pGradientAppear);

			if (SUCCEEDED(hr))
			{
				hr = m_renderTarget->CreateLinearGradientBrush(
					D2D1::LinearGradientBrushProperties(
						D2D1::Point2F(renderTargetSize.width * 0.5, 0),
						D2D1::Point2F(renderTargetSize.width * 0.5, 100)),
					pGradientAppear,
					&m_pGradientAppearBrush);
			}



		/*	CComPtr<ID2D1GradientStopCollection> pGradientDisappear;
			static const D2D1_GRADIENT_STOP gradientDisappear[] =
			{
				{ 0.f,  D2D1::ColorF(D2D1::ColorF::Black, 1.0f) },
				{ 1.f,  D2D1::ColorF(D2D1::ColorF::White, 0.0f) },
			};
			hr = m_renderTarget->CreateGradientStopCollection(
				gradientDisappear,
				2,
				&pGradientDisappear);


			if (SUCCEEDED(hr))
			{
				hr = m_renderTarget->CreateLinearGradientBrush(
					D2D1::LinearGradientBrushProperties(
						D2D1::Point2F(renderTargetSize.width * 0.5, 0),
						D2D1::Point2F(renderTargetSize.width * 0.5, 100)),
					pGradientDisappear,
					&m_pGradientDisappearBrush);
			}*/
		}

	}

	return hr;

	
}

//
// Discard any Direct2D resources which are no longer bound to a particular Direct2D render target
//
HRESULT CMainDlg::DiscardDeviceResources()
{
	for (auto thumbnail = m_thumbnailControls.begin(); thumbnail != m_thumbnailControls.end(); thumbnail++)
	{
		(*thumbnail)->DiscardResources();
	}

	HRESULT hr = RemoveAllItems();
	if (SUCCEEDED(hr))
	{
		hr = m_AsyncLoaderHelper->Shutdown();
	}


	/*if (m_backgroundBitmap) m_backgroundBitmap = nullptr;
	m_solidBrushList = nullptr;
	m_renderTarget = nullptr;
	m_renderingParametersList.renderTarget = nullptr;
	m_renderingParametersList.solidBrush = nullptr;
	m_renderingParametersList.textFormat = nullptr;
	m_renderingParametersList.textLayout = nullptr;*/
	return S_OK;
}



void CMainDlg::CreateThumbnailCells(bool resetPanPosition)
{
	//m_thumbnailLayoutManager->SetTextBoxHeight(m_textLayout->GetMaxHeight());
	m_thumbnailLayoutManager->CreateLayout(static_cast<int>(m_thumbnailControls.size()), resetPanPosition);
}



void CMainDlg::RedrawBookList() {
	IShellItem *pShellItemDir;
	HRESULT hr = ::SHCreateItemInKnownFolder(FOLDERID_Documents, 0, nullptr, IID_PPV_ARGS(&pShellItemDir));

	if (SUCCEEDED(hr)) {
		hr = SetCurrentLocationList(pShellItemDir, false);
		if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == hr)
		{
		}
	}


	SetPageCount();
}



HRESULT CMainDlg::SetCurrentLocationList(IShellItem* shellFolder, bool recursive)
{
	HRESULT hr = S_OK;


	m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, m_renderTarget->GetSize().width, m_renderTarget->GetSize().height));
	hr = m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);

	if (SUCCEEDED(hr))
	{
		hr = RemoveAllItems();
	}


	std::vector<ComPtr<IShellItem> > shellItems;


	wchar_t * buffer;
	hr = shellFolder->GetDisplayName(SIGDN_FILESYSPATH, &buffer);
	std::wstring pathDocument(buffer);
	pathDocument.append(m_strRootDir);
	::SHCreateItemFromParsingName(pathDocument.c_str(), nullptr, IID_PPV_ARGS(&shellFolder));
	::CoTaskMemFree(buffer);

	if (!shellFolder)
		return hr;

	ShellItemsLoader::EnumerateFolderItems(shellFolder, FileTypeFolder, recursive, shellItems);

	int i = 0;
	for (auto shellItem = shellItems.begin(); shellItem != shellItems.end(); shellItem++)
	{
		
		ThumbnailInfo info(*shellItem);
		info.fileType = FileTypeFolder;
		ComPtr<IThumbnail> imageThumbnailControl;
		if (SUCCEEDED(SharedObject<ImageThumbnail>::Create(info, &imageThumbnailControl)))
		{
			imageThumbnailControl->SetRenderingParameters(m_renderingParametersList);
			imageThumbnailControl->SetDefaultBitmap(m_defaultThumbnailBitmap, m_defaultBitmapSizeMap, sizeof(m_defaultBitmapSizeMap));
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


	//std::vector<ThumbnailCell> thumbnailCells;
	//std::vector<ThumbnailCell> prevThumbnailCells;

	// Capture currently visible thumbnails
	//hr = GetVisibleThumbnailCells(prevThumbnailCells);
	//if (SUCCEEDED(hr))
	//{
	//	m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, m_renderTarget->GetSize().width, m_renderTarget->GetSize().height));
	//}

	if (SUCCEEDED(hr))
	{
		// Initialize thumbnail layout manager with new cells
		CreateThumbnailCells(false);
		hr = m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);
	}

	if (SUCCEEDED(hr))
	{
		// Get updated thumbnail cells
		//hr = GetVisibleThumbnailCells(thumbnailCells);
	}


	if (SUCCEEDED(hr))
	{
		// Setup the FlyIn animation
		//m_animationController->Setup(previousThumbnails, currentThumbnails, m_renderTarget->GetSize());
		//m_updatingFolder = true;
		::InvalidateRect(m_hWnd, nullptr, FALSE);
		
	}


	return hr;
}


HRESULT CMainDlg::GetVisibleThumbnailCells(std::vector<ThumbnailCell> &cells)
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

HRESULT CMainDlg::RemoveAllItems()
{
	m_AsyncLoaderHelper->ClearItems();
	m_thumbnailControls.clear();

	return S_OK;
}




void CMainDlg::DrawAnimatedThumbnailCells()
{
	std::vector<AnimatedThumbnailCell> thumbnails;
	m_animationController->GetAnimatedThumbnailCells(thumbnails);

	int i = 0;
	for (auto thumbnail = thumbnails.begin(); thumbnail != thumbnails.end(); ++thumbnail)
	{
		D2D1_POINT_2F center;

		if (SUCCEEDED(m_animationController->GetAnimationPosition(thumbnail->cell.control, &center)))
		{
			float offsetX = Direct2DUtility::GetRectWidth(thumbnail->cell.position) / 2;
			float offsetY = Direct2DUtility::GetRectHeight(thumbnail->cell.position) / 2;

			D2D1_RECT_F position = { center.x - offsetX, center.y - offsetY, center.x + offsetX, center.y + offsetY };

			thumbnail->cell.control->SetThumbnailRect(position);
			thumbnail->cell.control->SetIsFullImage(false);
			thumbnail->cell.control->Draw();
		}
	}
}

float CMainDlg::GetThumbnailSize()
{
	float thumbnailSize = 0;

	if (m_thumbnailLayoutManager)
	{
		m_thumbnailLayoutManager->GetThumbnailSize(&thumbnailSize);
	}

	return thumbnailSize;
}


void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);

	// page button
	for (int i = 0; i < m_pageIconRect.size(); ++i) {

		if (Direct2DUtility::HitTest(m_pageIconRect[i], mousePosition) == Direct2DUtility::HIT_IN) {
			m_currentPage = i + 1;
			m_thumbnailLayoutManager->SetCurrentPage(m_currentPage);
			m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);
			::InvalidateRect(m_hWnd, nullptr, FALSE);
			break;
		}
	}


	SetCapture();
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMainDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	::ReleaseCapture();
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);
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
					if (m_bDeleteMode) {
						if (m_thumbnailControls[currentIndex]->IsSelectDeleteItem() == DELETE_MODE_SEL) {
							m_thumbnailControls[currentIndex]->SetSelectDeleteItem(DELETE_MODE_UNSEL);
						}
						else {
							m_thumbnailControls[currentIndex]->SetSelectDeleteItem(DELETE_MODE_SEL);
						}

						::InvalidateRect(m_hWnd, nullptr, FALSE);
					}
					else {
						D2D1_RECT_F positionRectLeft;
						positionRectLeft.left = rect.left;
						positionRectLeft.top = rect.bottom - 80;
						positionRectLeft.right = positionRectLeft.left + (rect.right - rect.left) * 0.5;
						positionRectLeft.bottom = rect.bottom;

						D2D1_RECT_F positionRectRight;
						positionRectRight.left = positionRectLeft.right;
						positionRectRight.top = rect.bottom - 80;
						positionRectRight.right = rect.right;
						positionRectRight.bottom = rect.bottom;

						ThumbnailInfo info;
						m_thumbnailControls[currentIndex]->GetThumbnailInfo(&info);

						D2D1_RECT_F clipRect;
						m_thumbnailControls[currentIndex]->GetClipRect(&clipRect);
						bool bLandscape = (clipRect.right > clipRect.bottom) ? true : false;

						if (Direct2DUtility::HitTest(positionRectLeft, mousePosition) == Direct2DUtility::HIT_IN) {
							static_cast<CAbleClassDlg*>(GetParent())->OpenEditor(info.m_fileName, bLandscape);
						}

						if (Direct2DUtility::HitTest(positionRectRight, mousePosition) == Direct2DUtility::HIT_IN) {
							static_cast<CAbleClassDlg*>(GetParent())->OpenBook(info.m_fileName, bLandscape);
						}
					}
					break;
				}
			}
		}
	}

	m_bTouch = FALSE;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);
	bool redraw = false;
	int currentIndex;
	int endIndex;

	m_mouseOverPage = 0;
	// page button
	for (int i = 0; i < m_pageIconRect.size(); ++i) {

		if (Direct2DUtility::HitTest(m_pageIconRect[i], mousePosition) == Direct2DUtility::HIT_IN) {
			m_mouseOverPage = i + 1;
			::InvalidateRect(m_hWnd, nullptr, FALSE);
			break;
		}
	}

	int nCurrentPageStartIndex;
	int nCurrentPageEndIndex;
	HRESULT hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);
	if (SUCCEEDED(hr))
	{
		for (; currentIndex < endIndex; currentIndex++)
		{
			D2D1_RECT_F positionRect;
			hr = m_thumbnailControls[currentIndex]->GetThumbnailRect(&positionRect);

			ThumbnailSelectionState selectionState = SelectionStateNone;
			hr = m_thumbnailControls[currentIndex]->GetSelectionState(&selectionState);

			if (Direct2DUtility::HitTest(positionRect, mousePosition) == Direct2DUtility::HIT_IN)
			{
				m_hoverOnRect = positionRect;

				D2D1_RECT_F positionRectLeft;
				positionRectLeft.left = positionRect.left;
				positionRectLeft.top = positionRect.bottom - 80;
				positionRectLeft.right = positionRectLeft.left + (positionRect.right - positionRect.left) * 0.5;
				positionRectLeft.bottom = positionRect.bottom;

				if (selectionState == SelectionStateHoverOnLeft || selectionState == SelectionStateHoverOnRight) {

					if (Direct2DUtility::HitTest(positionRectLeft, mousePosition) == Direct2DUtility::HIT_IN) {
						selectionState = static_cast<ThumbnailSelectionState>(SelectionStateHoverOnLeft);
					}
					else {
						selectionState = static_cast<ThumbnailSelectionState>(SelectionStateHoverOnRight);
					}
					m_thumbnailControls[currentIndex]->SetSelectionState(selectionState);
					redraw = true;
					
				}
				else {
					if (Direct2DUtility::HitTest(positionRectLeft, mousePosition) == Direct2DUtility::HIT_IN) {
						selectionState = static_cast<ThumbnailSelectionState>(SelectionStateHoverOnLeft);
					}
					else {
						selectionState = static_cast<ThumbnailSelectionState>(SelectionStateHoverOnRight);
					}
					m_thumbnailControls[currentIndex]->SetSelectionState(selectionState);
					m_thumbnailControls[currentIndex]->CreateHoverAnimation();
					redraw = true;
					m_bIsHoverAnimation = true;
				}
			}
			else {
				m_thumbnailControls[currentIndex]->SetSelectionState(SelectionStateNone);
				redraw = true;
			}
		}
		
	}

	if (redraw)
	{
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	else {
		//m_bIsHover = false;
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CMainDlg::DrawGif() {

	//if (m_scale > 1.1) {
	//	m_scale = 1.f;
	//}
	//else {
	//	m_scale = m_scale + 0.001;
	//}

	//m_renderTarget->SetTransform(D2D1::Matrix3x2F::Scale(m_scale, m_scale));
	//m_renderTarget->DrawBitmap(m_bgItem1);
	//m_renderTarget->DrawBitmap(m_bgItem2, D2D1::RectF(0, 0, m_bgItem2->GetSize().width, m_bgItem2->GetSize().height), m_scale - .1f);
	//m_renderTarget->DrawBitmap(m_bgItem3, D2D1::RectF(1200, 690, 1200 + m_bgItem3->GetSize().width, 690 + m_bgItem3->GetSize().height), m_scale - .1f);
	//m_renderTarget->SetTransform(D2D1::Matrix3x2F::Scale(1, 1));


	{
		ComPtr<IWICBitmapDecoder> pWICBitmapDecoder;
		ComPtr<IWICBitmapFrameDecode> pWICBitmapFrame;
		ComPtr<IWICFormatConverter> pFormatConverter;
		ComPtr<ID2D1Bitmap> pBitmap;
		HRESULT hResult = S_OK;

		ComPtr<IWICImagingFactory> pWICImagingFactory;

		hResult = Direct2DUtility::GetWICFactory(&pWICImagingFactory);

		hResult = pWICImagingFactory->CreateDecoderFromFilename(
			g_pSkin->GetSource(L"main_menu/main_background.gif").c_str()
			, NULL
			, GENERIC_READ
			, WICDecodeMetadataCacheOnLoad
			, &pWICBitmapDecoder
		);
		if (FAILED(hResult)) {
			return;
		}

		UINT qFrameCount = 0;
		hResult = pWICBitmapDecoder->GetFrameCount(&qFrameCount);
		if (FAILED(hResult)) {
			return;
		}

		if (m_frameIndex >= qFrameCount) {
			m_frameIndex = 0;
		}

		hResult = pWICBitmapDecoder->GetFrame(m_frameIndex++, &pWICBitmapFrame);
		if (FAILED(hResult)) {
			return;
		}

		hResult = pWICImagingFactory->CreateFormatConverter(&pFormatConverter);
		if (FAILED(hResult)) {
			return;
		}


		hResult = pFormatConverter->Initialize(
			pWICBitmapFrame                   
			, GUID_WICPixelFormat32bppPBGRA
			, WICBitmapDitherTypeNone
			, NULL
			, 1.0f
			, WICBitmapPaletteTypeMedianCut
		);
		if (FAILED(hResult)) {
			return;
		}


		hResult = m_renderTarget->CreateBitmapFromWicBitmap(pFormatConverter, NULL, &pBitmap);
		if (FAILED(hResult)) {
			return;
		}


		{
			D2D1_SIZE_F tBitmapSize = pBitmap->GetSize();
			D2D_POINT_2F tLeftTop = D2D1::Point2F(0,0);

			D2D1_RECT_F oDrawRect = D2D1::RectF(
				tLeftTop.x                        // left
				, tLeftTop.y                        // top
				, m_renderTarget->GetSize().width
				, m_renderTarget->GetSize().height
			);

			m_renderTarget->DrawBitmap(pBitmap, oDrawRect);
		}
	}
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		m_frameIndex++;
		::InvalidateRect(m_hWnd, nullptr, FALSE);
		//if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
		//{
		//	m_renderTarget->BeginDraw();
		//	m_renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		//	D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();
		//	m_renderTarget->DrawBitmap(m_bgItem1);
		//}
		//m_renderTarget->EndDraw();
	
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CMainDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 100) {
		PreviousPage();
	} else {
		NextPage();
	}



	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}



void CMainDlg::PreviousPage() {

	m_beforePage = m_currentPage;

	if (m_currentPage > 1) {
		m_currentPage -= 1;
	}
	else {
		m_currentPage = 1;
	}


	m_thumbnailLayoutManager->SetCurrentPage(m_currentPage);
	m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);
	SetPageAnimation();
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CMainDlg::NextPage() {

	m_beforePage = m_currentPage;

	if (m_currentPage < m_pageCount) {
		m_currentPage += 1;
	}
	else {
		m_currentPage = m_pageCount;
	}



	m_thumbnailLayoutManager->SetCurrentPage(m_currentPage);
	m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);
	SetPageAnimation();
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}


void CMainDlg::SetPageCount() {
	m_thumbnailLayoutManager->GetPageCount(&m_pageCount);
	m_pageIconRect.clear();
}


HRESULT CMainDlg::SetPageAnimation() {
	HRESULT hr = S_OK;


	if (m_bIsPageAnimation) {
		m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);
		//m_transitionY->SetInitialVelocity(0.1);
	}

	m_varAniTransY = nullptr;

	int currentIndex;
	int endIndex;
	int nCurrentPageStartIndex;
	int nCurrentPageEndIndex;

	hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);


	D2D1_RECT_F rect;
	m_thumbnailControls[nCurrentPageStartIndex]->GetThumbnailRect(&rect);

	unsigned int nPagePerRow;
	m_thumbnailLayoutManager->GetPagePerRow(&nPagePerRow);
	float heightDistance = (rect.bottom - rect.top) * nPagePerRow;


	if (m_beforePage < m_currentPage) {
		m_fScrollDistance = heightDistance;
	}
	else if (m_beforePage > m_currentPage) {
		m_fScrollDistance = -heightDistance;
	}
	else {
		return hr;
	}

	m_bIsPageAnimation = true;


	hr = m_animationManager->CreateAnimationVariable(m_fScrollDistance, &m_varAniTransY);

	float TransformationAnimationDuration = 0.4f;

	ComPtr<IUIAnimationStoryboard> storyboard;
	//ComPtr<IUIAnimationTransition> transitionY;



	if (SUCCEEDED(hr))
	{
		// Initialize storyboard
		hr = m_animationManager->CreateStoryboard(&storyboard);
	}

	if (SUCCEEDED(hr))
	{
		if (m_transitionY) m_transitionY = nullptr;

		// Create rotation transition
		hr = m_transitionLibrary->CreateAccelerateDecelerateTransition(
			TransformationAnimationDuration,
			0,
			0.4,
			0.6,
			&m_transitionY);
	}

	if (SUCCEEDED(hr))
	{
		hr = storyboard->AddTransition(m_varAniTransY, m_transitionY);
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


LRESULT CMainDlg::OnTouch(WPARAM wParam, LPARAM lParam) {

	//BYTE digitizerStatus = (BYTE)GetSystemMetrics(SM_DIGITIZER);
	//if ((digitizerStatus & (0x80 + 0x40)) == 0) //Stack Ready + MultiTouch
	//{
	//	MessageBox(0, L"No touch support is currently availible", L"Error", MB_OK);
	//	return 1;
	//}

	//BYTE nInputs = (BYTE)GetSystemMetrics(SM_MAXIMUMTOUCHES);
	//wsprintf(szTitle, L"%s - %d touch inputs", szTitle, nInputs);
	

	BOOL bHandled = FALSE;
	UINT cInputs = LOWORD(wParam);

	m_bTouch = TRUE;

	if (cInputs < 1) {
		return S_FALSE;
	}



	PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
	if (pInputs) {
		if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT))) {
			for (UINT i = 0; i < cInputs; i++) {
				TOUCHINPUT ti = pInputs[i];
				POINT pt;
				pt.x = ti.x / 100;
				pt.y = ti.y / 100;
				ScreenToClient(&pt);

				D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);

				if (ti.dwFlags & TOUCHEVENTF_DOWN) {
				}
				else if (ti.dwFlags & TOUCHEVENTF_UP) {
				}
				else if (ti.dwFlags & TOUCHEVENTF_MOVE) {
				}
				else {
				}
			}
			bHandled = TRUE;
		}
		else {
		}
		delete[] pInputs;
	}
	else {
	}

	if (bHandled) {
		CloseTouchInputHandle((HTOUCHINPUT)lParam);
		return 0L;
	}
	else {
		return 0L;
	}
}


void CMainDlg::SetDeleteModeItem(bool bDeleteMode) {
	m_bDeleteMode = bDeleteMode;
	if (!m_bDeleteMode) {
		// unselect selected delete item
		for (auto thumbnail = m_thumbnailControls.begin(); thumbnail != m_thumbnailControls.end(); thumbnail++)
		{
			(*thumbnail)->SetSelectDeleteItem(DELETE_MODE_OFF);
		}
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	else {
		for (auto thumbnail = m_thumbnailControls.begin(); thumbnail != m_thumbnailControls.end(); thumbnail++)
		{
			(*thumbnail)->SetSelectDeleteItem(DELETE_MODE_UNSEL);
		}
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
}


void CMainDlg::SetDeleteBookMode() {
	if (m_bDeleteMode) {
		SetDeleteModeItem(false);
	}
	else {
		SetDeleteModeItem(true);
	}
	
}

void CMainDlg::DoDeleteBook() {
	if (m_bDeleteMode) {
		bool bIsExistDeletedItem = false;
		// delete book directory and files
		for (auto thumbnail = m_thumbnailControls.begin(); thumbnail != m_thumbnailControls.end(); thumbnail++)
		{
			if ((*thumbnail)->IsSelectDeleteItem() == DELETE_MODE_SEL) {
				bIsExistDeletedItem = true;

				ThumbnailInfo info;
				(*thumbnail)->GetThumbnailInfo(&info);
				DeleteBookFiles(info.GetFullPath());

			}
		}

		if (bIsExistDeletedItem) {
			RedrawBookList();
		}
	}

	static_cast<CAbleClassDlg*>(GetParent())->ShowBookDeleteDialog(false);
	SetDeleteBookMode();
}


int CMainDlg::DeleteBookFiles(const std::wstring &refcstrRootDirectory, bool bDeleteSubdirectories)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
												 // subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::wstring	strFilePath;                 // Filepath
	std::wstring	strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = refcstrRootDirectory + L"\\*.*";
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + L"\\" + FileInformation.cFileName;

				if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteBookFiles(strFilePath, bDeleteSubdirectories);
						if (iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if (::SetFileAttributes(strFilePath.c_str(),
						FILE_ATTRIBUTE_NORMAL) == FALSE)
						return ::GetLastError();

					// Delete file
					if (::DeleteFile(strFilePath.c_str()) == FALSE)
						return ::GetLastError();
				}
			}
		} while (::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if (dwError != ERROR_NO_MORE_FILES)
			return dwError;
		else
		{
			if (!bSubdirectory)
			{
				// Set directory attributes
				if (::SetFileAttributes(refcstrRootDirectory.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

				// Delete directory
				if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
					return ::GetLastError();
			}
		}
	}

	return 0;
}


LRESULT CMainDlg::OnPointerDown(WPARAM wParam, LPARAM lParam) {
	
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(&pt);


	BookHoverDown(pt);

	return 0L;
}

LRESULT CMainDlg::OnPointerUpdate(WPARAM wParam, LPARAM lParam) {
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(&pt);
	return 0L;
}

LRESULT CMainDlg::OnPointerUp(WPARAM wParam, LPARAM lParam) {
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(&pt);


	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(pt.x, pt.y);

	// page button
	for (int i = 0; i < m_pageIconRect.size(); ++i) {

		if (Direct2DUtility::HitTest(m_pageIconRect[i], mousePosition) == Direct2DUtility::HIT_IN) {
			m_currentPage = i + 1;
			m_thumbnailLayoutManager->SetCurrentPage(m_currentPage);
			m_thumbnailLayoutManager->UpdateVisibleBook(m_thumbnailControls);
			::InvalidateRect(m_hWnd, nullptr, FALSE);
			break;
		}
	}


	return 0L;
}

void CMainDlg::BookHoverDown(POINT point) {
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);
	bool redraw = false;
	int currentIndex;
	int endIndex;

	m_mouseOverPage = 0;
	// page button
	for (int i = 0; i < m_pageIconRect.size(); ++i) {

		if (Direct2DUtility::HitTest(m_pageIconRect[i], mousePosition) == Direct2DUtility::HIT_IN) {
			m_mouseOverPage = i + 1;
			::InvalidateRect(m_hWnd, nullptr, FALSE);
			break;
		}
	}

	int nCurrentPageStartIndex;
	int nCurrentPageEndIndex;
	HRESULT hr = m_thumbnailLayoutManager->GetVisibleThumbnailRange(&currentIndex, &endIndex, &nCurrentPageStartIndex, &nCurrentPageEndIndex);
	if (SUCCEEDED(hr))
	{
		for (; currentIndex < endIndex; currentIndex++)
		{
			D2D1_RECT_F positionRect;
			hr = m_thumbnailControls[currentIndex]->GetThumbnailRect(&positionRect);

			ThumbnailSelectionState selectionState = SelectionStateNone;
			hr = m_thumbnailControls[currentIndex]->GetSelectionState(&selectionState);


			if (Direct2DUtility::HitTest(positionRect, mousePosition) == Direct2DUtility::HIT_IN)
			{

				if (m_bDeleteMode) {
					if (m_thumbnailControls[currentIndex]->IsSelectDeleteItem() == DELETE_MODE_SEL) {
						m_thumbnailControls[currentIndex]->SetSelectDeleteItem(DELETE_MODE_UNSEL);
					}
					else {
						m_thumbnailControls[currentIndex]->SetSelectDeleteItem(DELETE_MODE_SEL);
					}

					::InvalidateRect(m_hWnd, nullptr, FALSE);
				}
				else {
					m_hoverOnRect = positionRect;

					D2D1_RECT_F positionRectLeft;
					positionRectLeft.left = positionRect.left;
					positionRectLeft.top = positionRect.bottom - 80;
					positionRectLeft.right = positionRectLeft.left + (positionRect.right - positionRect.left) * 0.5;
					positionRectLeft.bottom = positionRect.bottom;

					D2D1_RECT_F positionRectRight;
					positionRectRight.left = positionRectLeft.right;
					positionRectRight.top = positionRect.bottom - 80;
					positionRectRight.right = positionRect.right;
					positionRectRight.bottom = positionRect.bottom;

					if (selectionState == SelectionStateHoverOnLeft || selectionState == SelectionStateHoverOnRight) {

						redraw = true;
						ThumbnailInfo info;
						m_thumbnailControls[currentIndex]->GetThumbnailInfo(&info);

						D2D1_RECT_F clipRect;
						m_thumbnailControls[currentIndex]->GetClipRect(&clipRect);
						bool bLandscape = (clipRect.right > clipRect.bottom) ? true : false;

						if (Direct2DUtility::HitTest(positionRectLeft, mousePosition) == Direct2DUtility::HIT_IN) {
							static_cast<CAbleClassDlg*>(GetParent())->OpenEditor(info.m_fileName, bLandscape);
						}

						if (Direct2DUtility::HitTest(positionRectRight, mousePosition) == Direct2DUtility::HIT_IN) {
							static_cast<CAbleClassDlg*>(GetParent())->OpenBook(info.m_fileName, bLandscape);
						}


					}
					else {
						if (Direct2DUtility::HitTest(positionRectLeft, mousePosition) == Direct2DUtility::HIT_IN) {
							selectionState = static_cast<ThumbnailSelectionState>(SelectionStateHoverOnLeft);
						}
						else {
							selectionState = static_cast<ThumbnailSelectionState>(SelectionStateHoverOnRight);
						}
						m_thumbnailControls[currentIndex]->SetSelectionState(selectionState);
						m_thumbnailControls[currentIndex]->CreateHoverAnimation();
						redraw = true;
						m_bIsHoverAnimation = true;
					}
				}

			}
			else {
				m_thumbnailControls[currentIndex]->SetSelectionState(SelectionStateNone);
				redraw = true;
			}
		}

	}

	if (redraw)
	{
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	else {
		//m_bIsHover = false;
	}
}