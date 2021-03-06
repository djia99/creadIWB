#pragma once


// Image list
#include "Animation.h"
#include "ThumbnailControl.h"
#include "ThumbnailLayoutInterface.h"
#include "ThumbnailLayoutManager.h"
#include "AsyncLoaderHelper.h"


class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMainDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DLG };
#endif


	// Factories
	ComPtr<ID2D1Factory>			m_d2dFactory;
	ComPtr<IDWriteFactory>			m_dWriteFactory;
	ComPtr<ID2D1HwndRenderTarget>	m_renderTarget;
	ComPtr<ID2D1SolidColorBrush>	m_solidBrushList;
	ComPtr<ID2D1LinearGradientBrush> m_pGradientAppearBrush;
	ComPtr<ID2D1LinearGradientBrush> m_pGradientDisappearBrush;
	D2D1_RECT_F						m_bitmapRect;
	// Direct2D resources
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	HRESULT DiscardDeviceResources();

	// Direct2D rendering parameters
	RenderingParameters						m_renderingParametersList;
	ComPtr<AsyncLoaderSpace::IAsyncLoaderHelper> m_AsyncLoaderHelper;
	// Thumnails layout manager
	ComPtr<IThumbnailLayoutManager>			m_thumbnailLayoutManager;
	// Thumbnails
	std::vector<ComPtr<IThumbnail>>			m_thumbnailControls;
	// Unique animation controller
	ComPtr<IMediaPaneAnimation>				m_animationController;
	ComPtr<IDWriteTextFormat>				m_textFormatList;
	ComPtr<IDWriteTextLayout>				m_textLayoutList;

	// book 교재 배경 이미지 맵
	// 이미지 맵 사이즈
	// 0 : 배경
	// 1 : 에디트버튼 normal, 2 : 에디트 버튼 hover
	// 3 : 실행 버튼 normal, 4 : 실행 버튼 hover
	// 5 : check 이미지
	ComPtr<ID2D1Bitmap>						m_defaultThumbnailBitmap;
	D2D1_RECT_F								m_defaultBitmapSizeMap[6];



	// scroll or paging
	ComPtr<IUIAnimationManager>				m_animationManager;
	ComPtr<IUIAnimationTransitionLibrary>	m_transitionLibrary;
	ComPtr<IUIAnimationVariable>			m_varAniTransY;
	ComPtr<IUIAnimationTransition>			m_transitionY;
	bool									m_bIsPageAnimation;
	unsigned int							m_pageCount;
	unsigned int							m_currentPage;
	unsigned int							m_beforePage;
	unsigned int							m_mouseOverPage;
	float									m_fScroll;
	float									m_fScrollDistance;
	float									m_previousPosition;				// 이전 출력 항목(rect)의 top 위치값
	float									m_nextPosition;					// 다음 출력 항목(rect)의 top 위치값
	ComPtr<ID2D1SolidColorBrush>			m_brushPageIcon;
	ComPtr<ID2D1SolidColorBrush>			m_brushPageIconBorder;
	std::vector<D2D1_RECT_F>				m_pageIconRect;
	HRESULT SetPageAnimation();
	void SetPageCount();
	void OnPaintPageIcon();
	void PreviousPage();
	void NextPage();


	// 교재 삭제
	bool	m_bDeleteMode;
	void	SetDeleteBookMode();
	void	SetDeleteModeItem(bool bDeleteMode);
	void	DoDeleteBook();						// 선택한 교재 삭제
	// 교재 폴더 삭제
	int		DeleteBookFiles(const std::wstring &refcstrRootDirectory, bool bDeleteSubdirectories = true);
	


	// 교재 선택
	void BookHoverDown(POINT point);

	// gradient 처리
	CComPtr<ID2D1Layer> m_pLayer;
	//hr = m_renderTarget->CreateLayer(NULL, &pLayer);



	AnimationType m_currentAnimation;
	D2D1_SIZE_F m_lastAnimationSize;

	void OnPaintListAnimation();
	void OnPaintList();
	HRESULT RemoveAllItems();
	void CreateThumbnailCells(bool resetPanPosition);
	HRESULT SetCurrentLocationList(IShellItem* shellFolder, bool recursive);
	HRESULT GetVisibleThumbnailCells(std::vector<ThumbnailCell> &cells);
	void DrawAnimatedThumbnailCells();
	float GetThumbnailSize();
	void RedrawBookList();

	// 문서 파일 정보
	std::wstring m_strRootDir;


	// item move hover action
	D2D1_RECT_F m_hoverOnRect;
	bool		m_bIsHoverAnimation;


	// 배경 애니메이션
	ComPtr<ID2D1Bitmap>				m_backgroundBitmap;
	ComPtr<ID2D1Bitmap>				m_bgItem1;
	ComPtr<ID2D1Bitmap>				m_bgItem2;
	ComPtr<ID2D1Bitmap>				m_bgItem3;
	float m_scale;
	int m_frameIndex;
	void DrawGif();


	// is touch down
	BOOL m_bTouch;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnTouch(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPointerDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPointerUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPointerUp(WPARAM wParam, LPARAM lParam);
};
