// CEditorDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "AbleClassDlg.h"
#include "CEditorDlg.h"
#include "afxdialogex.h"
#include "CEditorInsertDlg.h"
#include "CDocumentConvert.h"

#include "ImageThumbnailControl.h"
#include "FlyerAnimation.h"

// CEditorDlg 대화 상자
CComPtr<IMFPMediaPlayer>	CEditorDlg::m_mediaPlayer = nullptr;
CComPtr<IMFPMediaPlayerCallback> CEditorDlg::m_mediaPlayerCallBack = nullptr;

IMPLEMENT_DYNAMIC(CEditorDlg, CDialogEx)

CEditorDlg::CEditorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDITOR_DLG, pParent),
	m_operationType(ImageOperationTypeNone),
	m_currentInsertType(EDITOR_NONE),
	//m_mediaPlayer(nullptr),
	//m_mediaPlayerCallBack(nullptr),
	BackgroundColor(0x000000),
	m_strRootDir(L"\\PoDo"),
	m_strDocumentName(L"\\book2"),
	m_nCurrentPage(0),
	m_currentPanPoint(D2D1::Point2F(0, 0)),
	m_bIsDrag(false),
	m_mouseDownPoint(D2D1::Point2F(0, 0)),
	m_mousePrevPoint(D2D1::Point2F(0, 0)),
	m_bInsertItemDrawing(FALSE),
	m_bChanged(FALSE),
	m_bLandscape(false)
{

}

CEditorDlg::~CEditorDlg()
{
}

void CEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEditorDlg, CDialogEx)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYUP()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



int CEditorDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

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

	}

	if (m_editorSetDlg.Create(IDD_EDITOR_SET, this)) {
	}

	return 0;
}




void CEditorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	DiscardDeviceResources();
}




BOOL CEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rt;
	GetClientRect(&rt);
	LONG nCenter = static_cast<LONG>((rt.right - rt.left) * 0.5);



	m_videoClip.MoveWindow(static_cast<int>(nCenter - 15), rt.bottom - 800, 500, 400);
	m_videoClip.ShowWindow(SW_HIDE);
	m_videoClip.UpdateWindow();


	m_editorSetDlg.MoveWindow(20, 20, 310, 107);
	m_editorSetDlg.ShowWindow(SW_HIDE);
	m_editorSetDlg.UpdateWindow();
	

	return TRUE;
}

void CEditorDlg::LoadBook(std::wstring strDocumentName) {

	m_nCurrentPage = 0;
	m_strDocumentName = strDocumentName;

	IShellItem *pShellItemDir;
	HRESULT hr = ::SHCreateItemInKnownFolder(FOLDERID_Documents, 0, nullptr, IID_PPV_ARGS(&pShellItemDir));
	if (SUCCEEDED(hr)) {
		hr = SetCurrentLocationList(pShellItemDir, false);
		if (HRESULT_FROM_WIN32(ERROR_CANCELLED) == hr)
		{
		}
	}

	m_AsyncLoaderHelper->StartFullImageLoad(AsyncLoaderSpace::XxhdpiSize, m_nCurrentPage);

	D2D1_SIZE_F renderTargetSize = m_renderTarget->GetSize();
	m_thumbnailLayoutManager->SetBoundaryRect(D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height));
	m_thumbnailLayoutManager->SetSlideShowMode(true);
	m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);
	SetFullRect(m_nCurrentPage);

	LoadDbData();

}

void CEditorDlg::LoadDbData() {
	std::vector<CDataStructBook> vBook;
	((CAbleClassDlg*)GetParent())->GetBookData(vBook);

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
				editorItem->SetDataId((*bookData).nid);
				editorItem->SetType(static_cast<eItemType>((*bookData).book_type));
				editorItem->SetRect(m_renderTarget, rect);
				editorItem->SetColor((*bookData).book_color_bg, (*bookData).book_color_border);
				editorItem->SetBitmap(m_renderTarget);
				editorItem->SetFile((*bookData).book_path);

				m_lastId = (*bookData).nid;
				TRACE(L"\n>>>> last id (%d) <<<<\n", m_lastId);

				int bookPage = (*bookData).book_page;
				if (bookPage > -1 && bookPage < m_thumbnailControls.size()) {
					m_thumbnailControls[bookPage]->PushImageOperation(m_currentOperation);
				}


			}
			m_currentOperation = nullptr;
		}
	}
}


void CEditorDlg::DeleteDbData()
{
	((CAbleClassDlg*)GetParent())->DeleteBookData(m_vDeleteItem);
}


HRESULT CEditorDlg::CreateDeviceIndependentResources()
{
	HRESULT hr = Direct2DUtility::GetD2DFactory(&m_d2dFactory);

	if (SUCCEEDED(hr))
	{
		hr = Direct2DUtility::GetDWriteFactory(&m_dWriteFactory);
	}

	if (SUCCEEDED(hr))
	{
		// Stroke Style with custom Dash Style
		hr = m_d2dFactory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_SOLID,
				0.0f),
			NULL,
			0,
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
	//}


	return hr;
}

//
//  This method creates resources which are bound to a particular Direct2D render target
//
HRESULT CEditorDlg::CreateDeviceResources()
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
HRESULT CEditorDlg::DiscardDeviceResources()
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

	if (m_mediaPlayer)
	{
		m_mediaPlayer->Shutdown();
		m_mediaPlayer = nullptr;
	}

	//m_drawingObjects;
	//ComPtr<ID2D1SolidColorBrush>	m_solidBrushList;
	//ComPtr<ID2D1SolidColorBrush>	m_paintBrush;
	//ComPtr<ID2D1SolidColorBrush>	m_bgBrush;
	//int								BackgroundColor;
	//ComPtr<ID2D1StrokeStyle>		strokeStyleCustomOffsetZero;

	//// Direct2D rendering parameters
	//RenderingParameters						m_renderingParametersList;
	//ComPtr<IDWriteTextFormat>				m_textFormatList;
	//ComPtr<IDWriteTextLayout>				m_textLayoutList;

	return S_OK;
}




void CEditorDlg::OnSize(UINT nType, int cx, int cy)
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
		m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);
		SetFullRect(m_nCurrentPage);
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}

}


//TODO : OnPaint
void CEditorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	HRESULT hr = CreateDeviceResources();
	OnPaintItem();

	// Paint the video window.
	if (m_mediaPlayer)
	{
		m_mediaPlayer->UpdateVideo();
	}
	
}





void CEditorDlg::OnPaintItem() {

	HRESULT hr = S_OK;
	if (!(m_renderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{


		m_renderTarget->BeginDraw();
		m_renderTarget->Clear(D2D1::ColorF(BackgroundColor));
		D2D1_MATRIX_3X2_F scale = D2D1::Matrix3x2F::Scale(1, 1, GetCenter());
		D2D1_MATRIX_3X2_F translation = D2D1::Matrix3x2F::Translation(m_currentPanPoint.x, m_currentPanPoint.y);
		m_renderTarget->SetTransform(scale * translation);

		if (m_nCurrentPage < static_cast<UINT>(m_thumbnailControls.size())) {
			m_thumbnailControls[m_nCurrentPage]->DrawDynamicLoad(false);

			if (m_operationType == ImageOperationTypeInsert) {
				OnPaintItemDrawInsertBox();
			}
		}
	}

	hr = m_renderTarget->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		DiscardDeviceResources();
	}


}




void CEditorDlg::OnPaintItemDrawInsertBox() {
	if (m_bInsertItemDrawing) {
		D2D1_RECT_F imageRect;
		m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);

		D2D1_COLOR_F savedColor = m_paintBrush->GetColor();
		float savedOpacity = m_paintBrush->GetOpacity();
		float lineWidth = 1.0f;
		m_paintBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Gold));
		m_paintBrush->SetOpacity(1);
		m_renderTarget->DrawRectangle(m_itemBoxDrawing, m_paintBrush, lineWidth, strokeStyleCustomOffsetZero);
		m_paintBrush->SetColor(savedColor);
		m_paintBrush->SetOpacity(savedOpacity);
	}

}

void CEditorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{



	SetCapture();
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);








	m_mouseDownPoint = mousePosition;
	m_mousePrevPoint = mousePosition;
	D2D1_POINT_2F AbsolutionPos = GetAbsolutePosition(mousePosition);

	if (m_operationType == ImageOperationTypeNone) {
		m_bIsDrag = true;
	}
	else if (m_operationType == ImageOperationTypeInsert) {
		m_bInsertItemDrawing = TRUE;
		m_itemBox.left = m_itemBox.right = AbsolutionPos.x;
		m_itemBox.top = m_itemBox.bottom = AbsolutionPos.y;
		mousePosition = RemoveRenderingTransformations(mousePosition);
		m_itemBoxDrawing.left = m_itemBoxDrawing.right = mousePosition.x;
		m_itemBoxDrawing.top = m_itemBoxDrawing.bottom = mousePosition.y;
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}


	


	::InvalidateRect(m_hWnd, nullptr, FALSE);

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	HRESULT hr = S_OK;
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);
	::ReleaseCapture();

	m_bIsDrag = false;



	if (m_operationType == ImageOperationTypeNone) {
		if (EditorItemHitTest(GetAbsolutePosition(mousePosition), TRUE) == Direct2DUtility::HIT_NON) {
			EditorSetVisible(SW_HIDE);
			m_thumbnailControls[m_nCurrentPage]->EditorOperationSelect(false);
			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
	}
	else if (m_operationType == ImageOperationTypeInsert) {
		if (m_bInsertItemDrawing) {
			if (Direct2DUtility::GetRectWidth(m_itemBox) > 0 && Direct2DUtility::GetRectHeight(m_itemBox) > 0)
			{
				D2D1_POINT_2F AbsolutionPos = GetAbsolutePosition(mousePosition);
				m_itemBox.right = AbsolutionPos.x;
				if (m_currentInsertType == EDITOR_BLANK) {
					m_itemBox.bottom = AbsolutionPos.y;
				}
				else {
					m_itemBox.bottom = m_itemBox.top + (m_itemBox.right - m_itemBox.left);
				}
				m_itemBox = Direct2DUtility::FixRect(m_itemBox);


				if (m_currentInsertType == EDITOR_VIDEO) {
					NewItemVideo(m_itemBox);
				}
				else if (m_currentInsertType == EDITOR_AUDIO) {
					NewItemAudio(m_itemBox);
				}
				else if (m_currentInsertType == EDITOR_BLANK) {
					NewItemBlank(m_itemBox);
				}
				
				

				m_bInsertItemDrawing = FALSE;
				::InvalidateRect(m_hWnd, nullptr, FALSE);

			}
			else {
				m_bInsertItemDrawing = FALSE;
				::InvalidateRect(m_hWnd, nullptr, FALSE);
			}
		}

		m_operationType = ImageOperationTypeNone;
		m_currentInsertType = EDITOR_NONE;
		EditorSetVisible(SW_HIDE);
		((CAbleClassDlg*)GetParent())->SetEditorToolMenuButttonSelect(FALSE);
	}




	CDialogEx::OnLButtonUp(nFlags, point);
}


void CEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	D2D1_POINT_2F mousePosition = Direct2DUtility::GetMousePositionForCurrentDpi(point.x, point.y);


	//HDC hdc = ::GetDC(m_hWnd);
	//COLORREF clrRef[4] = { 0, };

	//int x = static_cast<int>(mousePosition.x - 2);
	//int y = static_cast<int>(mousePosition.y - 2);
	//clrRef[0] = GetPixel(hdc, x, y);

	//TRACE(L"\n>>>>> rect(%d,%d) color rgb(%d,%d,%d) <<<<<<\n", x, y, GetRValue(clrRef[0]), GetGValue(clrRef[0]), GetBValue(clrRef[0]));


	if (m_operationType == ImageOperationTypeNone && m_bIsDrag) {
		// Update pan based on delta
		D2D1_POINT_2F delta;
		delta.x = (mousePosition.x - m_mousePrevPoint.x);
		delta.y = (mousePosition.y - m_mousePrevPoint.y);

		if (!(delta.x == 0 && delta.y == 0))
		{
			m_mousePrevPoint = mousePosition;

			PanMove(delta.y);
			::InvalidateRect(m_hWnd, nullptr, FALSE);
		}
	}
	else if (m_operationType == ImageOperationTypeInsert && m_bInsertItemDrawing) {
		m_itemBox.right = mousePosition.x;

		if (m_currentInsertType == EDITOR_BLANK) {
			m_itemBox.bottom = mousePosition.y;
		}
		else {
			m_itemBox.bottom = m_itemBox.top + (m_itemBox.right - m_itemBox.left) /*mousePosition.y*/;
		}


		mousePosition = RemoveRenderingTransformations(mousePosition);
		D2D1_RECT_F imageRect;
		m_thumbnailControls[m_nCurrentPage]->GetTransformedRect(GetCenter(), &imageRect);

		if (m_currentInsertType == EDITOR_BLANK) {
			m_itemBoxDrawing.right = std::max<FLOAT>(imageRect.left, std::min<FLOAT>(imageRect.right, mousePosition.x));
			m_itemBoxDrawing.bottom = std::max<FLOAT>(imageRect.top, std::min<FLOAT>(imageRect.bottom, mousePosition.y));
		}
		else {
			m_itemBoxDrawing.right = std::max<FLOAT>(imageRect.left, std::min<FLOAT>(imageRect.right, mousePosition.x));
			m_itemBoxDrawing.bottom = m_itemBoxDrawing.top + (m_itemBoxDrawing.right - m_itemBoxDrawing.left);
		}

		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}


	CDialogEx::OnMouseMove(nFlags, point);
}





void CEditorDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CEditorDlg::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}

//
// Navigate to the previous image
//
bool CEditorDlg::PreviousPage()
{
	bool isPrevImage = false;
	if (m_nCurrentPage > 0) {
		m_nCurrentPage--;
		m_AsyncLoaderHelper->StartFullImageLoad(AsyncLoaderSpace::XxhdpiSize, m_nCurrentPage);
		m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);

		SetFullRect(m_nCurrentPage);
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}

	EditorSetVisible(SW_HIDE);

	return isPrevImage;
}

//
// Navigate to the next image
//
bool CEditorDlg::NextPage()
{
	bool isNextImage = false;
	if (m_nCurrentPage < m_thumbnailControls.size() - 1) {
		m_nCurrentPage++;
		m_AsyncLoaderHelper->StartFullImageLoad(AsyncLoaderSpace::XxhdpiSize, m_nCurrentPage);
		m_thumbnailLayoutManager->SetVisibleIndex(m_thumbnailControls, m_nCurrentPage);

		SetFullRect(m_nCurrentPage);
		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}

	EditorSetVisible(SW_HIDE);
	return isNextImage;
}

void CEditorDlg::VideoInsert()
{
	m_operationType = ImageOperationTypeInsert;
	m_currentInsertType = EDITOR_VIDEO;
	EditorSetVisible(SW_SHOW);
	return;
}

void CEditorDlg::AudioInsert()
{
	m_operationType = ImageOperationTypeInsert;
	m_currentInsertType = EDITOR_AUDIO;
	EditorSetVisible(SW_SHOW);
	return;
}

void CEditorDlg::PictureInsert()
{
	m_operationType = ImageOperationTypeInsert;
	m_currentInsertType = EDITOR_PICTURE;
	EditorSetVisible(SW_SHOW);
	return;
}

void CEditorDlg::MemoInsert()
{
	m_operationType = ImageOperationTypeInsert;
	m_currentInsertType = EDITOR_MEMO;
	EditorSetVisible(SW_SHOW);
	return;
}

void CEditorDlg::BlankInsert()
{
	m_operationType = ImageOperationTypeInsert;
	m_currentInsertType = EDITOR_BLANK;
	EditorSetVisible(SW_SHOW);
	return;
}




BOOL CEditorDlg::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

BOOL CEditorDlg::PreTranslateMessage(MSG* pMsg)
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


BOOL CEditorDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 100) {
		ScrollUp();
	}
	else {
		ScrollDown();
	}
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CEditorDlg::ScrollUp() {
	PanMove(60);
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}

void CEditorDlg::ScrollDown() {
	PanMove(-60);
	::InvalidateRect(m_hWnd, nullptr, FALSE);
}


// 스크롤 y 축
void CEditorDlg::PanMove(float deltaY) {
	m_currentPanPoint.y += deltaY;

	if (m_currentPanPoint.y > 0) m_currentPanPoint.y = 0;

	D2D1_RECT_F imageRect;
	m_thumbnailControls[m_nCurrentPage]->GetDrawingRect(&imageRect);
	D2D1_SIZE_F renderSize = m_renderTarget->GetSize();

	float transedImageHeight = (imageRect.bottom - imageRect.top);
	float fSlideDistanceV;

	if (transedImageHeight > renderSize.height) {
		fSlideDistanceV = (transedImageHeight - renderSize.height);
	}
	else {
		fSlideDistanceV = (renderSize.height - transedImageHeight);
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
}

void CEditorDlg::CreateThumbnailCells(bool resetPanPosition)
{
	//m_thumbnailLayoutManager->SetTextBoxHeight(m_textLayout->GetMaxHeight());
	m_thumbnailLayoutManager->CreateLayout(static_cast<int>(m_thumbnailControls.size()), resetPanPosition);
}




HRESULT CEditorDlg::SetCurrentLocationList(IShellItem* shellFolder, bool recursive)
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
	std::wstring pathDocument(buffer);
	pathDocument.append(m_strRootDir);
	pathDocument.append(L"\\");
	pathDocument.append(m_strDocumentName);

	::SHCreateItemFromParsingName(pathDocument.c_str(), nullptr, IID_PPV_ARGS(&shellFolder));
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
			imageThumbnailControl->SetClipRect(clipRect);
			imageThumbnailControl->SetRenderingParameters(m_renderingParametersList);
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



	return hr;
}


HRESULT CEditorDlg::GetVisibleThumbnailCells(std::vector<ThumbnailCell> &cells)
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

HRESULT CEditorDlg::RemoveAllItems()
{
	m_AsyncLoaderHelper->ClearItems();
	m_thumbnailControls.clear();

	return S_OK;
}




void CEditorDlg::DrawAnimatedThumbnailCells()
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

float CEditorDlg::GetThumbnailSize()
{
	float thumbnailSize = 0;

	if (m_thumbnailLayoutManager)
	{
		m_thumbnailLayoutManager->GetThumbnailSize(&thumbnailSize);
	}

	return thumbnailSize;
}


D2D1_POINT_2F CEditorDlg::GetAbsolutePosition(D2D1_POINT_2F mousePosition)
{
	// First transform back the point (disregarding current translation and scale)
	mousePosition = RemoveRenderingTransformations(mousePosition);

	// Translate to an absolute point within the image current drawing rect
	D2D1_POINT_2F absPoint;
	m_thumbnailControls[m_nCurrentPage]->TranslateToAbsolutePoint(mousePosition, &absPoint);

	D2D1_RECT_F rect;
	m_thumbnailControls[m_nCurrentPage]->GetDrawingRect(&rect);

	// Scale to actual point relative to the original bitmap
	float scale;
	m_thumbnailControls[m_nCurrentPage]->GetScale(&scale);


	// 이미지 영역 안에서만 그리기 해제
	//return AdjustToClipRect(
	//	D2D1::Point2F(
	//		scale * (absPoint.x - rect.left),
	//		scale * (absPoint.y - rect.top)));
	return D2D1::Point2F(scale * (absPoint.x - rect.left), scale * (absPoint.y - rect.top));
}

D2D1_POINT_2F CEditorDlg::AdjustToClipRect(D2D1_POINT_2F absPoint)
{
	D2D1_RECT_F clipRect;
	m_thumbnailControls[m_nCurrentPage]->GetClipRect(&clipRect);

	return D2D1::Point2F(
		std::max<FLOAT>(clipRect.left, std::min<FLOAT>(clipRect.right, absPoint.x)),
		std::max<FLOAT>(clipRect.top, std::min<FLOAT>(clipRect.bottom, absPoint.y)));
}

D2D1_POINT_2F CEditorDlg::RemoveRenderingTransformations(D2D1_POINT_2F mousePosition)
{
	D2D1_POINT_2F panPoint = m_currentPanPoint;
	mousePosition = D2D1::Matrix3x2F::Translation(-panPoint.x, -panPoint.y).TransformPoint(mousePosition);
	mousePosition = D2D1::Matrix3x2F::Scale(1, 1, GetCenter()).TransformPoint(mousePosition);

	return mousePosition;
}



int CEditorDlg::IsImageHit(D2D1_POINT_2F mousePosition)
{
	// First transform back the point (disregarding current translation and scale)
	mousePosition = RemoveRenderingTransformations(mousePosition);
	int isHit = Direct2DUtility::HIT_NON;

	if (m_nCurrentPage < static_cast<UINT>(m_thumbnailControls.size())) {
		m_thumbnailControls[m_nCurrentPage]->ContainsPoint(mousePosition, &isHit);
	}

	return isHit;
}


// 편집용 이미지 사이즈 조정
// 가로 전체 사이즈 or 1/2 사이즈 or 전체 사이즈 or 1/6 사이즈
void CEditorDlg::SetFullRect(int nCurrentPage) {
	// pan 초기화
	m_currentPanPoint.y = 0;


	if (nCurrentPage < static_cast<UINT>(m_thumbnailControls.size())) {
		D2D1_RECT_F DrawingRect;
		m_thumbnailControls[nCurrentPage]->GetDrawingRect(&DrawingRect);
		D2D1_RECT_F FullRect;
		FullRect = DrawingRect;
		FullRect.left = m_renderTarget->GetSize().width / 6;
		FullRect.right = m_renderTarget->GetSize().width - FullRect.left;
		FullRect.top = 1;
		float fFullRectHeight = ((FullRect.right - FullRect.left) * (DrawingRect.bottom - DrawingRect.top)) / (DrawingRect.right - DrawingRect.left);
		FullRect.bottom = fFullRectHeight;
		m_thumbnailControls[nCurrentPage]->SetDrawingRect(FullRect);
	}
}



HRESULT CEditorDlg::MediaPlay() {
	HRESULT hr = S_OK;
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
				FALSE,						// Start playback automatically?
				MFP_OPTION_NONE,			// Flags
				m_mediaPlayerCallBack,		// Callback pointer
				m_videoClip.GetSafeHwnd(),		// Video window
				&m_mediaPlayer
			);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_mediaPlayer->CreateMediaItemFromURL(L"C://temp/test.mp4", FALSE, 0, NULL);
			if (SUCCEEDED(hr)) {
				//hr = m_mediaPlayer->Play();
			}
		
		}
	}
	else {
		MFP_MEDIAPLAYER_STATE state = MFP_MEDIAPLAYER_STATE_EMPTY;
		HRESULT hr = m_mediaPlayer->GetState(&state);

		if (SUCCEEDED(hr))
		{
			if (state == MFP_MEDIAPLAYER_STATE_PAUSED || state == MFP_MEDIAPLAYER_STATE_STOPPED)
			{
				hr = m_mediaPlayer->Play();
			}
			else if (state == MFP_MEDIAPLAYER_STATE_PLAYING)
			{
				hr = m_mediaPlayer->Stop();
			}
		}
	}


	return hr;
}

void CEditorDlg::MediaStop() {
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

void CEditorDlg::MediaPlayerCallback::OnMediaPlayerEvent(_In_ MFP_EVENT_HEADER * pEventHeader)
{
	if (FAILED(pEventHeader->hrEvent))
	{
		return;
	}

	switch (pEventHeader->eEventType)
	{
	case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
		CEditorDlg::OnMediaItemCreated(MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader));
		break;

	case MFP_EVENT_TYPE_MEDIAITEM_SET:
		break;
	}
}



void CEditorDlg::OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent)
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

// TODO : Editor Insert video
void CEditorDlg::NewItemVideo(const D2D1_RECT_F& rect) {
	CEditorInsertDlg dlg;
	dlg.SetType(EDITOR_VIDEO);

	if (dlg.DoModal() == IDOK) {
		
		if (!dlg.m_strFile.empty()) {
			std::wstring strMediaFile;
			std::wstring strFileName = dlg.m_strFile.substr(dlg.m_strFile.find_last_of('\\') + 1);
			CopyMediaFile(dlg.m_strFile, strFileName, strMediaFile);

			HRESULT hr = SharedObject<CEditorItemOperation>::Create(&m_currentOperation);
			ComPtr<IEditorItemOperation> editorItem;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&editorItem)))
			{
				editorItem->SetType(EDITOR_VIDEO);
				editorItem->SetDataId(++m_lastId);
				editorItem->SetRect(m_renderTarget, rect);
				editorItem->SetBitmap(m_renderTarget);
				m_thumbnailControls[m_nCurrentPage]->PushImageOperation(m_currentOperation);


				CDataStructBook book;
				//book.nid = m_lastId;
				book.nid = editorItem->GetDataId();
				book.book_page = m_nCurrentPage;
				book.book_type = EDITOR_VIDEO;
				book.book_left = rect.left;
				book.book_top = rect.top;
				book.book_right = rect.right;
				book.book_bottom = rect.bottom;
				book.book_path = strFileName;
				//DbInsertItem(&book);
				m_addItem.push_back(book);
				m_bChanged = TRUE;

			}
			m_currentOperation = nullptr;
		}
	}


}


// TODO : Editor Insert video
void CEditorDlg::NewItemAudio(const D2D1_RECT_F& rect) {
	CEditorInsertDlg dlg;
	dlg.SetType(EDITOR_AUDIO);
	if (dlg.DoModal() == IDOK) {
		if (!dlg.m_strFile.empty()) {
			std::wstring strMediaFile;
			std::wstring strFileName = dlg.m_strFile.substr(dlg.m_strFile.find_last_of('\\') + 1);
			CopyMediaFile(dlg.m_strFile, strFileName, strMediaFile);

			HRESULT hr = SharedObject<CEditorItemOperation>::Create(&m_currentOperation);
			ComPtr<IEditorItemOperation> editorItem;
			if (SUCCEEDED(m_currentOperation.QueryInterface(&editorItem)))
			{
				editorItem->SetType(EDITOR_AUDIO);
				editorItem->SetDataId(++m_lastId);
				editorItem->SetRect(m_renderTarget, rect);
				editorItem->SetBitmap(m_renderTarget);
				m_thumbnailControls[m_nCurrentPage]->PushImageOperation(m_currentOperation);


				CDataStructBook book;
				book.nid = editorItem->GetDataId();
				book.book_page = m_nCurrentPage;
				book.book_type = EDITOR_AUDIO;	// sound
				book.book_left = rect.left;
				book.book_top = rect.top;
				book.book_right = rect.right;
				book.book_bottom = rect.bottom;
				book.book_path = strFileName;

				//DbInsertItem(&book);
				m_addItem.push_back(book);
				m_bChanged = TRUE;


			}
			m_currentOperation = nullptr;
		}
	}


}


// TODO : Editor Insert blank
void CEditorDlg::NewItemBlank(const D2D1_RECT_F& rect) {

	HRESULT hr = SharedObject<CEditorItemOperation>::Create(&m_currentOperation);
	ComPtr<IEditorItemOperation> editorItem;
	if (SUCCEEDED(m_currentOperation.QueryInterface(&editorItem)))
	{
		HDC hdc = ::GetDC(m_hWnd);
		COLORREF clrRef[4] = { 0, };

		int x = static_cast<int>(m_itemBoxDrawing.left-2);
		int y = static_cast<int>(m_itemBoxDrawing.top-2);
		clrRef[0] = GetPixel(hdc, x, y);

		//TRACE(L"\n>>>>> rect(%d,%d) color rgb(%d,%d,%d) <<<<<<\n", x, y, GetRValue(clrRef[0]), GetGValue(clrRef[0]), GetBValue(clrRef[0]));

		x = static_cast<int>(m_itemBoxDrawing.right - 4);
		y = static_cast<int>(m_itemBoxDrawing.top + 4);
		clrRef[1] = GetPixel(hdc, x, y);
		x = static_cast<int>(m_itemBoxDrawing.left + 4);
		y = static_cast<int>(m_itemBoxDrawing.bottom - 4);
		clrRef[2] = GetPixel(hdc, x, y);
		x = static_cast<int>(m_itemBoxDrawing.right - 4);
		y = static_cast<int>(m_itemBoxDrawing.bottom - 4);
		clrRef[3] = GetPixel(hdc, x, y);

		
		::ReleaseDC(m_hWnd, hdc);
		


		editorItem->SetType(EDITOR_BLANK);
		editorItem->SetDataId(++m_lastId);
		editorItem->SetRect(m_renderTarget, rect);
		editorItem->SetBitmap(m_renderTarget);
		editorItem->SetColor(clrRef[0], clrRef[1]);
		m_thumbnailControls[m_nCurrentPage]->PushImageOperation(m_currentOperation);

		CDataStructBook book;
		book.nid = editorItem->GetDataId();
		book.book_page = m_nCurrentPage;
		book.book_type = EDITOR_BLANK;		// blank
		book.book_left = rect.left;
		book.book_top = rect.top;
		book.book_right = rect.right;
		book.book_bottom = rect.bottom;
		book.book_color_bg = clrRef[0];
		book.book_path = L"";

		//DbInsertItem(&book);
		m_addItem.push_back(book);
		m_bChanged = TRUE;

		::InvalidateRect(m_hWnd, nullptr, FALSE);
	}
	m_currentOperation = nullptr;
}


void CEditorDlg::CopyMediaFile(std::wstring strFilePath, std::wstring strFileName, std::wstring& strMediaFile) {

	ThumbnailInfo info;
	m_thumbnailControls[m_nCurrentPage]->GetThumbnailInfo(&info);
	strMediaFile = info.GetMediaPath();
	strMediaFile.append(L"\\");
	CDocumentConvert::tmkdir(strMediaFile.c_str());
	strMediaFile.append(strFileName);

	::CopyFile(strFilePath.c_str(), strMediaFile.c_str(), FALSE);
}

void CEditorDlg::DbInsertItem(CDataStructBook* book) {
	((CAbleClassDlg*)GetParent())->GetBookDB()->sqlite_insert_book(book);
}


int CEditorDlg::EditorItemHitTest(D2D1_POINT_2F mousePoint, BOOL bClick) {
	int isHit = Direct2DUtility::HIT_NON;

	ComPtr<IImageOperation> hitOperation;
	if (m_thumbnailControls[m_nCurrentPage]->EditorItemHistTest(mousePoint, bClick, &hitOperation) == S_OK) {

		SetCursor(LoadCursor(NULL, IDC_HAND));
		if (bClick) {
			if (hitOperation) {
				ComPtr<IEditorItemOperation> editorOperation;
				if (SUCCEEDED(hitOperation.QueryInterface(&editorOperation)))
				{
					isHit = Direct2DUtility::HIT_IN;
					editorOperation->SetSelected(TRUE);
					EditorSetVisible(SW_SHOW, TRUE);

					UINT selectItemId = editorOperation->GetDataId();

					m_bChanged = TRUE;
					m_vDeleteItem.push_back(selectItemId);						// 삭제 항목 추가 (close 시 일괄 삭제)

					// 현재 편집 세션에서 추가한 항목을 검색하여 제거
					std::remove_if(m_addItem.begin(), m_addItem.end(),
						[selectItemId](const CDataStructBook & m) -> bool { return m.nid == selectItemId; });

					//m_thumbnailControls[m_nCurrentPage]->EraserEditorOperation(hitOperation);		// draw 삭제
					::InvalidateRect(m_hWnd, nullptr, FALSE);
				}
			}
		}
	}

	return isHit;
}




void CEditorDlg::EditorSetVisible(UINT visible, BOOL deleteMode) {

	if (deleteMode) {
		m_editorSetDlg.SetDeleteMode(TRUE);
	}
	else {
		m_editorSetDlg.SetDeleteMode(FALSE);
	}

	m_editorSetDlg.ShowWindow(visible);
	m_editorSetDlg.UpdateWindow();
}


void CEditorDlg::DeleteSelectedItem() {
	std::vector<ComPtr<IImageOperation>> imgOperations;
	m_thumbnailControls[m_nCurrentPage]->GetImageOperationList(imgOperations);

	bool bDeleted = false;
	long sizeOperations = static_cast<long>(imgOperations.size());

	// 가장 최근에 drawing 항목 먼저 출력
	for (long i = sizeOperations - 1; i > -1; --i) {
		ComPtr<IEditorItemOperation> item;
		if (SUCCEEDED(imgOperations[i]->QueryInterface(&item))) {
			if (item->GetSelected() == TRUE) {
				bDeleted = true;
				m_bChanged = TRUE;
				m_vDeleteItem.push_back(item->GetDataId());						// 삭제 항목 추가 (close 시 일괄 삭제)
				m_thumbnailControls[m_nCurrentPage]->EraserEditorOperation(imgOperations[i]);		// draw 삭제
				
			}
		}
	}

	if (bDeleted)
		::InvalidateRect(m_hWnd, nullptr, FALSE);
}


void CEditorDlg::DoClose() {
	if (IsChanged() || m_addItem.size() > 0) {
		if (AfxMessageBox(L"변경 사항을 저장하시겠습니까?", MB_YESNO) == IDYES) {
			DoSave();
		}
	}

	DoClear();
}

void CEditorDlg::DoSave() {

	// [DB] 항목 삭제
	DeleteDbData();

	// [DB] 항목 추가
	for (int i = 0; i < m_addItem.size(); i++) {
		DbInsertItem(&m_addItem[i]);
	}

	DoClear();

}
void CEditorDlg::DoClear() {
	m_bChanged = FALSE;
	m_addItem.clear();
	m_vDeleteItem.clear();

}