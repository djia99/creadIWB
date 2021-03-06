#pragma once


#include "Image.h"
#include "Animation.h"


// Image list
#include "ThumbnailControl.h"
#include "ThumbnailLayoutInterface.h"
#include "ThumbnailLayoutManager.h"
#include "AsyncLoaderHelper.h"

// media player
#include <mfplay.h>
#include <mferror.h>
#include <Shlwapi.h>

#include "CVideoClipDlg.h"
#include "CEditorSetDlg.h"



class CEditorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditorDlg)

public:
	CEditorDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CEditorDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITOR_DLG };
#endif


	// Factories
	ComPtr<ID2D1Factory>			m_d2dFactory;
	ComPtr<IDWriteFactory>			m_dWriteFactory;
	ComPtr<ID2D1HwndRenderTarget>	m_renderTarget;

	// Direct2D rendering resources
	RenderingParameters				m_drawingObjects;
	ComPtr<ID2D1SolidColorBrush>	m_solidBrushList;
	ComPtr<ID2D1SolidColorBrush>	m_paintBrush;
	ComPtr<ID2D1SolidColorBrush>	m_bgBrush;
	int								BackgroundColor;
	ComPtr<ID2D1StrokeStyle>		strokeStyleCustomOffsetZero;

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
	D2D1_POINT_2F							m_currentPanPoint;				// 중심 축 위치

	// Direct2D resources
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	HRESULT DiscardDeviceResources();


	// paint
	void	OnPaintItem();						// edit mode
	void	OnPaintItemDrawInsertBox();

	// animation setup
	ComPtr<IUIAnimationManager>				m_animationManager;
	ComPtr<IUIAnimationTransitionLibrary>	m_transitionLibrary;


	// Mouse information
	D2D1_POINT_2F	GetAbsolutePosition(D2D1_POINT_2F mousePosition);
	D2D1_POINT_2F	AdjustToClipRect(D2D1_POINT_2F absPoint);
	D2D1_POINT_2F	RemoveRenderingTransformations(D2D1_POINT_2F mousePosition);
	inline D2D1_POINT_2F GetCenter()
	{
		return D2D1::Point2F(
			m_renderTarget->GetSize().width / 2,
			m_renderTarget->GetSize().height / 2);
	}

	void	LoadBook(std::wstring strDocumentName);
	void	DeleteDbData();
	void	LoadDbData();
	HRESULT RemoveAllItems();
	void	CreateThumbnailCells(bool resetPanPosition);
	HRESULT SetCurrentLocationList(IShellItem* shellFolder, bool recursive);
	HRESULT GetVisibleThumbnailCells(std::vector<ThumbnailCell> &cells);
	void	DrawAnimatedThumbnailCells();
	float	GetThumbnailSize();




	// 편집용 이미지 사이즈 조정
	// 가로 전체 사이즈 or 1/2 사이즈 or 전체 사이즈
	void	SetFullRect(int nCurrentPage);


	// 문서 파일 정보
	std::wstring m_strRootDir;
	std::wstring m_strDocumentName;


	// page 처리 관련
	int	 m_nCurrentPage;	// 현재 보여지는 페이지
	bool PreviousPage();
	bool NextPage();
	void VideoInsert();
	void AudioInsert();
	void PictureInsert();
	void MemoInsert();
	void BlankInsert();
	int IsImageHit(D2D1_POINT_2F mousePoint);



	// pan drag
	bool						m_bIsDrag;						// 현재 드래그 상태
	D2D1_POINT_2F				m_mouseDownPoint;
	D2D1_POINT_2F				m_mousePrevPoint;
	void						ScrollUp();
	void						ScrollDown();
	void						PanMove(float deltaY);			// scroll Y 축



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
	HRESULT						MediaPlay();
	void						MediaStop();
	CVideoClipDlg				m_videoClip;
	static CComPtr<IMFPMediaPlayer>	m_mediaPlayer;
	static CComPtr<IMFPMediaPlayerCallback> m_mediaPlayerCallBack;
	static void STDMETHODCALLTYPE OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT *pEvent);




	BOOL						m_bChanged;						// 편집 내용 존재 확인
	BOOL						IsChanged() { return m_bChanged; }
	void						DoSave();
	void						DoClear();
	std::vector<UINT>			m_vDeleteItem;					// 아이템 삭제 관련
	ImageOperationType			m_operationType;				// 삽입 모드 
	eItemType					m_currentInsertType;			// 삽입 item 타입

	// 객체 삽입
	ComPtr<IImageOperation>		m_currentOperation;
	BOOL						m_bInsertItemDrawing;
	D2D1_RECT_F					m_itemBox;
	D2D1_RECT_F					m_itemBoxDrawing;
	void NewItemBlank(const D2D1_RECT_F& rect);
	void NewItemVideo(const D2D1_RECT_F& rect);
	void NewItemAudio(const D2D1_RECT_F& rect);
	void DbInsertItem(CDataStructBook* book);			// DB
	void CopyMediaFile(std::wstring strFilePath, std::wstring strFileName, std::wstring& strMediaFile);
	void DeleteSelectedItem();


	// landscape
	void	SetLandscape(bool bLandscape) { m_bLandscape = bLandscape; }
	bool	m_bLandscape;

	// Add item
	std::vector<CDataStructBook>	m_addItem;
	UINT							m_lastId;

	// editor setting dialog
	CEditorSetDlg m_editorSetDlg;
	void EditorSetVisible(UINT visible, BOOL deleteMode = FALSE);				// setting dialog show

	
	void DoClose();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
