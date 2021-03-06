#pragma once


// D2D
#include <d2d1.h>
#include <d2d1helper.h>	
#include <comdef.h>
#include <WinCodec.h>

#include "Image.h"
#include "Animation.h"
#include "PointAnimation.h"

// Image list
#include "ThumbnailControl.h"
#include "ThumbnailLayoutInterface.h"
#include "ThumbnailLayoutManager.h"
#include "AsyncLoaderHelper.h"

#include "CVideoClipDlg.h"
#include "CBoardSetDlg.h"


#define STYLUS_ON


#define PENTYPE_PEN			0
#define PENTYPE_HPEN		1


class CBoardPen {
public:
	CBoardPen(int pentyle) :
		m_tipStyle(PS_NORMAL),
		m_size(0),
		m_color(0),
		m_pressure(true),
		m_penType(pentyle)
	{
		if (pentyle == PENTYPE_PEN) {
			sizeReference[0] = 1;
			sizeReference[1] = 2;
			sizeReference[2] = 3;
			sizeReference[3] = 4;
			sizeReference[4] = 5;
			sizeReference[5] = 6;
		}
		else {
			sizeReference[0] = 5;
			sizeReference[1] = 10;
			sizeReference[2] = 16;
			sizeReference[3] = 22;
			sizeReference[4] = 28;
			sizeReference[5] = 36;
		}

	}
	~CBoardPen() {

	}

private:
	PENTIP_STYLE			m_tipStyle;
	int						m_size;
	int						m_color;
	bool					m_pressure;
	int						m_penType;
	int						sizeReference[6];

public:
	//index
	PENTIP_STYLE			GetPentipStyle() { return m_tipStyle; }
	int						GetSizeIndex() { return m_size; }
	int						GetColorIndex() { return m_color; }
	bool					GetPressure() { return m_pressure; }
	int						GetAlphaIndex() { return m_penType; }

	// value
	float					GetAlphaValue() { return (m_penType == PENTYPE_PEN) ? 1.0f : 0.5f; }
	int						GetSizeValue() { return sizeReference[m_size]; }

	void					SetPressure(bool b) { m_pressure = b; }
	void					SetPentipStyle(PENTIP_STYLE ps) { m_tipStyle = ps; }
	void					SetSize(int size) { m_size = size; }
	void					SetColor(int color) { m_color = color; }

	void					SetSizeUp() { if (m_size < 5) m_size++; }
	void					SetSizeDown() { if (m_size > 0) m_size--; }



};



class CWhiteBoardDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWhiteBoardDlg)

public:
	CWhiteBoardDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CWhiteBoardDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WHITEBOARD };
#endif

	// setting dlg
	// editor setting dialog
	CBoardSetDlg m_setDlg;
	void SetDlgVisible(UINT visible, std::wstring strMessage = L"");				// setting dialog show




	// test code
	ULONG m_autoIncrese;

	// Factories
	ComPtr<ID2D1Factory>			m_d2dFactory;
	ComPtr<IDWriteFactory>			m_dWriteFactory;
	ComPtr<ID2D1HwndRenderTarget>	m_renderTarget;
	ComPtr<ID2D1Bitmap>				m_renderTargetBitmap;


	// Direct2D rendering resources
	//RenderingParameters				m_drawingObjects;
	ComPtr<ID2D1SolidColorBrush>	m_solidBrushList;
	ComPtr<ID2D1SolidColorBrush>	m_paintBrush;
	ComPtr<ID2D1SolidColorBrush>	m_bgBrush;
	ComPtr<ID2D1SolidColorBrush>	m_textBrush;
	ComPtr<ID2D1StrokeStyle>		strokeStyleCustomOffsetZero;
	int								BackgroundColor;

	// Direct2D resources
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	HRESULT DiscardDeviceResources();


	HRESULT				RegisterTouchInput();
	HRESULT				UnregisterTouchInput();
#ifdef STYLUS_ON
	// realtimestylus
	bool				m_bUseDyniamcRenderer;
	IRealTimeStylus*	m_pRealTimeStylus;
	IDynamicRenderer*	m_pDynamicRenderer;
	void SetDynamicRendererMode(BOOL isOn) {
		if (m_pDynamicRenderer) {
			if (m_bUseDyniamcRenderer && IsItemEditMode() && GetOperationType() == ImageOperationTypePen) {
				m_pDynamicRenderer->put_Enabled(isOn);
			}
			else
			{
				m_pDynamicRenderer->put_Enabled(FALSE);
			}

		}
	}
	BOOL				EnableDynamicDrawing(D2D1_POINT_2F mousePoint) {
		if (IsOperationTypePen() && IsImageHit(mousePoint) == Direct2DUtility::HIT_IN) {
			SetDynamicRendererMode(TRUE);
			return TRUE;
		}
		else
		{
			SetDynamicRendererMode(FALSE);
			return FALSE;
		}
	}
	IDynamicRenderer*	CreateDynamicRenderer(IRealTimeStylus* pRealTimeStylus);
	bool EnableRealTimeStylus(IRealTimeStylus* pRealTimeStylus, IDynamicRenderer* pDynamicRenderer);
	IStylusSyncPlugin*	m_pSyncEventHandlerRTS;       // EventHandler object
	//IStrokeBuilder*		m_pStrokeBuilder;
#endif


	// paint
	void	OnPaintItem();						// edit mode
	void	OnPaintList();						// list mode
	void	OnPaintItemDrawZoomBox();			// zoom box
	void	OnPaintItemDrawClippingBox();		// crop box
	void	OnPaintEmptyBoard();				// empty whiteboard paint
	void	OnPaintTextMessage();				// text message draw
	HRESULT OnPaintModeChangeAnimation();
	HRESULT OnPaintSlideAnimation();
	HRESULT OnPaintZoomAnimation();
	HRESULT OnPaintEmptyBoardAnimation();


	// animation setup
	HRESULT ReleaseAnimation();
	HRESULT AbandonAnimation();
	HRESULT SetModeChangeAnimation();		// 리스트 모드에서 에디트 모드로 전환될 때 image 줌인
	HRESULT SetZoomAnimation();				// zoom in zoom out animation
	HRESULT SetEmptyBoardAnimation();		// empty board in animation
	ComPtr<IUIAnimationManager>				m_animationManager;
	ComPtr<IUIAnimationTransitionLibrary>	m_transitionLibrary;
	ComPtr<IUIAnimationVariable>			m_varAniTransX;
	ComPtr<IUIAnimationVariable>			m_varAniTransY;
	ComPtr<IUIAnimationVariable>			m_varAniZoom;
	ComPtr<IUIAnimationVariable>			m_varAniAlpha;
	float									m_beforeZoom;
	float									m_currentZoom;
	float									m_emptyBoardZoom;
	float									m_beforeTransX;
	float									m_currentTransX;
	float									m_beforeTransY;
	float									m_currentTransY;
	float									m_beforeAlpha;
	float									m_currentAlpha;
	bool									m_bIsModeChangeAnimation;		// 모드 변경시 애니메이션
	bool									m_bIsSlideAnimation;			// page 이동 슬라이드 애니메이션
	bool									m_bIsListAnimation;				// 리스트 모드 애니메이션
	bool									m_bIsZoomAnimation;				// 줌 애니메이션
	bool									m_bIsEmptyBoardAnimation;		// 칠판 모드 애니메이션
	D2D1_POINT_2F							m_currentPanPoint;				// 중심 축 위치
	D2D1_POINT_2F							m_emptyBoardPanPoint;			// 빈칠판 중심 축 위치
	float									m_zoomMin;						// 줌 최소 값
	float									m_zoomMax;						// 줌 최대 값
	float									m_zoomStep;						// 줌 단계 값
	bool									m_bIsBackgroundLoading;			// 백그라운드 이미지 로딩
	// mode change animation setup
	ComPtr<IUIAnimationVariable>			m_varRectLeft;
	ComPtr<IUIAnimationVariable>			m_varRectTop;
	ComPtr<IUIAnimationVariable>			m_varRectRight;
	ComPtr<IUIAnimationVariable>			m_varRectBottom;
	AnimationType							m_currentAnimation;
	D2D1_SIZE_F								m_lastAnimationSize;


	// Direct2D rendering parameters
	RenderingParameters						m_renderingParametersList;
	ComPtr<IDWriteTextFormat>				m_textFormatList;
	ComPtr<IDWriteTextLayout>				m_textLayoutList;
	ComPtr<AsyncLoaderSpace::IAsyncLoaderHelper> m_AsyncLoaderHelper;
	// Thumnails layout manager
	ComPtr<IThumbnailLayoutManager>			m_thumbnailLayoutManager;
	// Thumbnails
	std::vector<ComPtr<IThumbnail>>			m_thumbnailControls;
	// Unique animation controller
	ComPtr<IMediaPaneAnimation>				m_animationController;
	ComPtr<ID2D1Bitmap>						m_splashBitmap;						// splash 
	ComPtr<IThumbnail>						GetCurrentThumbnailPtr();
	bool									m_bFirstListLoading;				// 제일 처음 리스트 항목 호출 시 


	/****************************************************************************
	// 비어있는 이미지 화이트보드 칠판
	****************************************************************************/
	int										m_nEmptyBoardPage;
	std::vector<ComPtr<IThumbnail>>			m_emptyBoard;
	void AddEmptyBoardItem();
	/****************************************************************************
	// 비어있는 이미지 화이트보드 칠판 end
	****************************************************************************/




	void	CloseBook();
	void	LoadBook(std::wstring strDocumentName);
	HRESULT RemoveAllItems();
	void	CreateThumbnailCells(bool resetPanPosition);
	HRESULT SetCurrentLocationList(IShellItem* shellFolder, bool recursive);
	HRESULT SetupAnimationList(AnimationType animationType, D2D1_SIZE_F size);
	HRESULT GetVisibleThumbnailCells(std::vector<ThumbnailCell> &cells);
	void	DrawAnimatedThumbnailCells();
	float	GetThumbnailSize();

	// landscape
	void	SetLandscape(bool bLandscape) { m_bLandscape = bLandscape; }
	bool	m_bLandscape;

	// 문서 파일 정보
	std::wstring			m_MyDocument;
	std::wstring			m_strRootDir;
	std::wstring			m_strDocumentName;




	// Drawing operations
	bool					m_bIsDrawing;					// 현재 그리기 상태
	bool					IsDrawing() { return m_bIsDrawing; }
	bool					m_bIsEraser;					// 현재 지우개 상태
	//int						m_penColorIndex;
	stPenColor				m_penColorPalette[6];
	int						NextPenColor();

	ImageOperationType		m_operationTypeEmptyBoard;		//빈칠판 모드에서 설정 값
	ImageOperationType		m_operationType;	
	ImageOperationType		m_prevOperationType;
	ImageOperationType		GetOperationType();
	BOOL					m_bReversal;					// 반전모드
	void					DoReversal();


	// pen
	UINT					m_penType;
	CBoardPen*				m_Pen;
	CBoardPen*				m_HPen;
	CBoardPen*				m_PenEmptyBoard;
	CBoardPen*				m_HPenEmptyBoard;
	UINT					GetPenType() { return m_penType; }
	void					SetPenType(int index);
	void					SetPenToggle() { m_penType ^= 1; }
	CBoardPen*				GetPen() { return (GetPageMode() == PAGE_EMPTYBOARD ? (m_penType==PENTYPE_PEN? m_PenEmptyBoard : m_HPenEmptyBoard) : (m_penType == PENTYPE_PEN ? m_Pen : m_HPen)); }
	
	ComPtr<IImageOperation> m_currentOperation;
	UINT					m_penId;
	void					SetOperationType(ImageOperationType type);
	bool					IsOperationTypePen(); // 현재 그리기 모드 인지 확인 
	long					GetPenColor() { return m_penColorPalette[GetPen()->GetColorIndex()].clr; /*RGB(m_penColorPalette[m_penColorIndex].r * 255, m_penColorPalette[m_penColorIndex].g * 255, m_penColorPalette[m_penColorIndex].b * 255);*/ }
	void					GetColorInfoFromDatabase();
	void					SetPenColor(int index);
	void					SetPenSize(int index);
	void					SetPenSizeUp();
	void					SetPenSizeDown();
	//float					GetPenSize();
	float					GetPenSizeDynamicRender();
	//void					SetPenAlpha(int index);




	// Mouse information
	D2D1_POINT_2F	GetAbsolutePosition(D2D1_POINT_2F mousePosition);
	D2D1_POINT_2F	AdjustToClipRect(D2D1_POINT_2F absPoint);
	D2D1_POINT_2F	RemoveRenderingTransformations(D2D1_POINT_2F mousePosition);
	void			UpdateMouseCursor(D2D1_POINT_2F mousePosition);
	inline D2D1_POINT_2F GetCenter()
	{
		return D2D1::Point2F(
			m_renderTarget->GetSize().width / 2,
			m_renderTarget->GetSize().height / 2);
	}

	// page 처리 관련
	int	 m_nCurrentPage;	// 현재 보여지는 페이지
	int  m_nAnimatePage;	// 현재 애니메이션 중 페이지
	bool PreviousPage();
	bool NextPage();
	void PrintPageNumber();	// bookmark 에 페이지 번호/ 전체 페이지 표시
	int GetCurrentPage() {
		return (GetPageMode() == PAGE_EMPTYBOARD) ?  m_nEmptyBoardPage: m_nCurrentPage;
	}
	int GetLastPage() {
		return (GetPageMode() == PAGE_EMPTYBOARD) ? m_emptyBoard.size() : m_thumbnailControls.size();
	}



	// 화면에 text 메시지 animation 출력
	void ShowMessageAnimation(std::wstring strMessage);
	ComPtr<IDWriteTextFormat>				m_tfMessage;
	ComPtr<IDWriteTextLayout>				m_tlMessage;

	// pan drag
	enum PAGE_MOVE {
		PAGE_NON = 0,
		PAGE_PREV,
		PAGE_NEXT
	};
	PAGE_MOVE					m_pageChanging;
	bool						m_bIsDrag;						// 현재 드래그 상태
	D2D1_POINT_2F				m_mouseDownPoint;
	D2D1_POINT_2F				m_mousePrevPoint;


	// zoom action
	float			m_zoomDuration;
	bool			m_bIsZoomming;
	D2D1_RECT_F		m_zoomBox;
	D2D1_RECT_F		m_zoomBoxDrawing;
	int				IsImageHit(D2D1_POINT_2F mousePoint);
	void			ZoomIn();
	void			ZoomOut();
	void			ResetBookData();						// 교재 시작시 관련 값 초기화
	void			ResetZoomData();
	float			GetZoomming(FLOAT width, FLOAT height);	// 영역 확대 사이즈 계산
	bool			CheckOutOfBoundary();					// 이미지가 화면에서 보이지 않을 경우
	void			DoRecord() {
		//ShowMessageAnimation(_T("1111111111111111111"));
	}


	class MediaPlayerCallback : public IMFPMediaPlayerCallback
	{
	private:
		long m_cRef; // Reference count

	public:
		MediaPlayerCallback() : m_cRef(1)
		{
		}

		STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
		{
			HRESULT hr = S_OK;
			*ppv = NULL;

			if (riid == __uuidof(IMFPMediaPlayerCallback))
			{
				*ppv = static_cast<IMFPMediaPlayerCallback*>(this);
			}

			else if (riid == __uuidof(IUnknown))
			{
				*ppv = static_cast<IUnknown*>(this);
			}

			else
			{
				hr = E_NOINTERFACE;
			}

			if (SUCCEEDED(hr))
			{
				AddRef();
			}

			return hr;
		}

		STDMETHODIMP_(ULONG) AddRef()
		{
			return InterlockedIncrement(&m_cRef);
		}

		STDMETHODIMP_(ULONG) Release()
		{
			ULONG count = InterlockedDecrement(&m_cRef);

			if (count == 0)
			{
				delete this;
			}

			return count;
		}

		// IMFPMediaPlayerCallback methods
		void STDMETHODCALLTYPE OnMediaPlayerEvent(_In_ MFP_EVENT_HEADER *pEventHeader);
	};


	// editorItem hittest
	int							EditorItemHitTest(D2D1_POINT_2F mousePoint, BOOL bClick);
	// media player
	HRESULT						MediaPlay(std::wstring file);
	void						MediaStop();
	CVideoClipDlg				m_videoClip;
	void						VideoClipDialog(UINT visible);
	static CComPtr<IMFPMediaPlayer>	m_mediaPlayer;
	static CComPtr<IMFPMediaPlayerCallback> m_mediaPlayerCallBack;
	static void STDMETHODCALLTYPE OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent);
	static void STDMETHODCALLTYPE OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT * pSet);


	// clipping action
	bool			m_bIsClipping;
	D2D1_RECT_F		m_clippingBox;
	D2D1_RECT_F		m_clippingBoxDrawing;


	// database loading
	void LoadDbData();


	// 액션 처리
	void DoItemMode(int page);					// 아이템 모드
	void DoListMode();							// 리스트 모드
	void DoPen();								// 판서 모드
	void DoView();								// 뷰 모드
	void DoMove();								// 이동 모드
	void DoClipping();							// 스크린 캡쳐 후 칠판 모드 전환 
	void DoZoom();								// 줌 모드
	void DoReduce();							// 전체 모드 (축소모드)
	void DoEmptyBoardMode();					// 칠판 모드
	void DoEraser();							// 판서 지우개
	void DoClear();								// 전체 삭제
	void DoUndo();								// 실행 취소
	void DoBlank();								// 빈칸삽입


	// 빈칸 삽입 상태
	bool m_bEditorBlankVisibled;
	bool GetEditorBlankVisible() { return m_bEditorBlankVisibled; }

	// 페이지 모드 : 리스트, 아이템, 빈 화이트보드
	UINT m_pageMode;
	UINT GetPageMode() { return m_pageMode; }
	void DoModeChange(UINT nMode, unsigned int imageIndex);

	// 아이템 모드 : 판서 모드, 이동 모드
	UINT m_itemMode;
	UINT m_itemModeEmptyBoard;
	UINT GetItemMode() { return (GetPageMode() == PAGE_EMPTYBOARD) ? m_itemModeEmptyBoard : m_itemMode; }
	bool IsItemEditMode() { return ((GetPageMode() == PAGE_EMPTYBOARD) ? (m_itemModeEmptyBoard == ITEM_EDIT) : (m_itemMode == ITEM_EDIT)); }
	void SetItemMode(UINT mode) { (GetPageMode() == PAGE_EMPTYBOARD) ? m_itemModeEmptyBoard = mode : m_itemMode = mode; }


	/****************************************************************************
	// mouse input, touch, pointer 입력 통합처리
	****************************************************************************/
	BOOL InputProcessDown(D2D1_POINT_2F mousePosition, LONG pressure=1);
	void InputProcessMove(D2D1_POINT_2F mousePosition, LONG pressure = 1);
	void InputProcessUp(D2D1_POINT_2F mousePosition, LONG pressure = 1);
	/****************************************************************************
	// mouse input, touch, pointer 입력 통합처리 end
	****************************************************************************/



	// test
	void DoSpline();

	//screen rect
	D2D1_RECT_F GetScreenRect(D2D1_RECT_F rect);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//virtual BOOL OnTouchInput(CPoint pt, int nInputNumber, int nInputsCount, PTOUCHINPUT pInput);
	afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPointerDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPointerUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPointerUp(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	LRESULT OnBeginDraw(WPARAM wParam, LPARAM lParam);
	LRESULT OnVideoClipClick(WPARAM wParam, LPARAM lParam);
};
