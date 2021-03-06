
// AbleClassDlg.h: 헤더 파일
//

#pragma once
//#include "ComTouchDriver.h"
#include "CWhiteBoardDlg.h"
#include "CBoardMenuDlg.h"
#include "CToolDlg.h"
#include "CBookMarkDlg.h"
#include "CInkDlg.h"
#include "CMainDlg.h"
#include "CMainMenuDlg.h"
#include "CMainUpDlg.h"
#include "CBookDeleteDlg.h"
#include "CEditorDlg.h"
#include "CEditorToolDlg.h"
#include "CCreadPipe.h"

#include <d3d11.h>
#include <dcomp.h>


#define PROC_REC_START (WM_USER + 1000)
#define PROC_REC_STOP (WM_USER + 1001)

struct ProcessWindowsInfo
{
	DWORD ProcessID;
	std::vector<HWND> Windows;

	ProcessWindowsInfo(DWORD const AProcessID)
		: ProcessID(AProcessID)
	{
	}
};


// CAbleClassDlg 대화 상자
class CAbleClassDlg : public CDialogEx
{
// 생성입니다.
public:
	CAbleClassDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CAbleClassDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABLECLASS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	enum {
		VIEW_MAIN = 1,
		VIEW_WHITEBOARD = 2,
		VIEW_EDIT = 3,
	};

	UINT m_viewMode;
	void SetViewMode(UINT viewmode);
	void SetViewChange();

	/************************************************************************
	// main view
	************************************************************************/
	CMainDlg m_mainDlg;
	/************************************************************************
	// main view end
	************************************************************************/



	/************************************************************************
	// image viewer old version
	************************************************************************/
	//CComTouchDriver* g_ctDriver;	// Encapsulates all Touch/Mouse Event handling
	/************************************************************************
	// image viewer old version end
	************************************************************************/

	/************************************************************************
	// implement
	************************************************************************/
	VOID SetTabletInputServiceProperties();
	BOOL m_bTrackMouse;


	/****************************************************************************
	/ 화면 View UI 설정
	// Dialog init
	// layerd window control
	****************************************************************************/
	CComPtr<ID3D11Device> _d3d11DeviceLayer;
	CComPtr<ID3D11DeviceContext> _d3d11DeviceContextLayer;
	CComPtr<IDCompositionDevice> m_pDeviceLayer;
	CComPtr<IDCompositionTarget> m_pHwndRenderTargetLayer;
	CComPtr<IDCompositionVisual> m_pRootVisualLayer;
	CComPtr<IDCompositionVisual> m_pVisualMainMenu;
	CComPtr<IUnknown> m_pMainMenuSurface;
	CComPtr<IDCompositionVisual> m_pVisualBookMark;
	CComPtr<IUnknown> m_pBookMarkSurface;


	////////////////////////////////////////////////////////////////////////
	// 툴 윈도우 visual  
	CComPtr<IDCompositionVisual> m_pVisualToolMenu;
	CComPtr<IUnknown> m_pToolMenuSurface;
	CComPtr<IDCompositionTranslateTransform> m_transTool;
	CComPtr<IDCompositionTransform> m_transToolGroup;
	////////////////////////////////////////////////////////////////////////

	CComPtr<IDCompositionTranslateTransform> m_pControlTranslateTransform;
	CComPtr<IDCompositionTransform> m_pTransformGroup;
	CComPtr<IDCompositionRotateTransform> m_pRotateTransform;
	HRESULT InitLayer();
	HRESULT CreateD3D11Device();
	HRESULT CreateDCompositionDevice();
	HRESULT CreateDCompositionRenderTarget();
	HRESULT CreateDCompositionVisualTree();
	HRESULT CreateTransforms();
	HRESULT DoTrans(UINT target, UINT show);
	HRESULT DoTransTool();
	HRESULT SetTranslation(float beginPositionX, float endPositionX, float beginPositionY, float endPositionY, float beginTime, float endTime, IDCompositionTranslateTransform *translateTransform);
	HRESULT CreateTranslateAnimation(float beginValue, float endValue, float beginTime, float endTime, IDCompositionAnimation **linearAnimation);
	RECT m_rtContext;

	

	CButtonDlg		m_btnLogo;
	CWhiteBoardDlg	m_board;
	CMainUpDlg		m_mainUpDlg;
	CMainMenuDlg	m_mainMenuDlg;
	CBoardMenuDlg	m_boardMenuDlg;
	CToolDlg		m_toolDlg;
	CBookMarkDlg	m_bookMarkDlg;
	CInkDlg			m_inkDlg;
	CBookDeleteDlg	m_bookDeleteDlg;
	CEditorDlg		m_editorDlg;
	CEditorToolDlg	m_editorToolDlg;


	BOOL m_toolMenuVisible;			// 툴창 위치 상태
	BOOL m_boardMenuVisible;

	void ResizeLayout(int cx, int cy);
	/****************************************************************************
	// 화면 View UI 설정 끝
	****************************************************************************/

	// db
	CSqliteEx* m_pBookDB;
	// db
	std::vector<CDataStructBook> m_bookData;





public:
	/****************************************************************************
	// 환경 설정
	****************************************************************************/
	void DoMainSetting();


	void GetBookData(std::vector<CDataStructBook>& outBook) {
		outBook = m_bookData;
	}
	void DeleteBookData(std::vector<UINT>& vDeleteItem);
	void DbSelectBookData();

	CSqliteEx* GetBookDB() {
		return m_pBookDB;
	}
	void		LoadBookDB(std::wstring dbName) {
		if (m_pBookDB) {
			m_pBookDB->CloseDatabase();
			delete m_pBookDB;
			m_pBookDB = NULL;
		}
		m_pBookDB = new CSqliteEx;
		m_pBookDB->Init(dbName);
		DbSelectBookData();
	}
	void		CloseBookDB() {
		if (m_pBookDB) {
			m_pBookDB->CloseDatabase();
			delete m_pBookDB;
			m_pBookDB = NULL;
		}
	}





	/****************************************************************************
	// window handle
	****************************************************************************/
	void SetBoardNextImage() {
		m_board.NextPage();
	}
	void SetBoardPrevImage() {
		m_board.PreviousPage();
	}
	void SetBoardZoom() {
		if (m_board.GetPageMode() == PAGE_ITEM) {
			//SetBoardMenuButtonStyle(IDC_BTN_EXPAND, TRUE);
			m_board.DoZoom();
		}
	}
	void SetBoardReduce() {
		if (m_board.GetPageMode() == PAGE_ITEM) {
			m_board.DoReduce();
		}
	}
	void SetBoardClipping() {
		if (m_board.GetPageMode() == PAGE_ITEM) {
			//SetBoardMenuButtonStyle(IDC_BTN_CLIPPING, TRUE);
			m_board.DoClipping();
		}
	}



	void SetBoardPen() {
		if (m_board.GetPageMode() == PAGE_ITEM) {

			if (m_board.GetItemMode() == ITEM_EDIT) {
				SetBoardMenuButtonStyle(IDC_BTN_PEN, FALSE);
				m_board.DoView();
			}
			else {
				SetBoardMenuButtonStyle(IDC_BTN_PEN, TRUE);
				m_board.DoPen();
			}
			SetWhiteBoardViewChange();
		}
		else if (m_board.GetPageMode() == PAGE_EMPTYBOARD) {
			if (m_board.GetItemMode() == ITEM_EDIT) {
				SetBoardMenuButtonStyle(IDC_BTN_PEN, FALSE);
				m_board.DoView();
			}
			else {
				SetBoardMenuButtonStyle(IDC_BTN_PEN, TRUE);
				m_board.DoPen();
			}
			SetWhiteBoardViewChange();
		}
	}
	void SetBoardEraser() {
		m_toolDlg.DoButtonLayoutChange(3, TRUE);
		m_board.DoEraser();
	}
	void SetBoardClear() {
		m_board.DoClear();
	}
	void SetBoardUndo() {
		m_board.DoUndo();
	}
	void SetBoardNon() {
		if (m_board.GetPageMode() == PAGE_ITEM) {
			SetBoardMenuButtonStyle(IDC_BTN_PEN, FALSE);
			m_board.DoView();
			SetWhiteBoardViewChange();
		}
		else if (m_board.GetPageMode() == PAGE_EMPTYBOARD) {
			SetBoardMenuButtonStyle(IDC_BTN_PEN, FALSE);
			m_board.DoView();
			SetWhiteBoardViewChange();
		}
	}
	void SetListMode() {
		SetBoardMenuButtonStyle(IDC_BTN_ALL, FALSE);
		m_board.DoListMode();
		m_board.DoView();
		SetWhiteBoardViewChange();
	}

	void SetEmptyBoard() {
		m_board.DoEmptyBoardMode();
		if (m_board.GetPageMode() == PAGE_ITEM) {
			SetBoardMenuButtonStyle(IDC_BTN_BOARD, FALSE);
		}
		else if (m_board.GetPageMode() == PAGE_EMPTYBOARD) {
			SetBoardMenuButtonStyle(IDC_BTN_BOARD, TRUE);
		}
		SetWhiteBoardViewChange();
	}
	void SetBoardBlank() {
		if (m_board.GetPageMode() == PAGE_ITEM) {
			if (m_board.GetEditorBlankVisible()) {
				SetBoardMenuButtonStyle(IDC_BTN_HIDDEN, FALSE);
			}
			else {
				SetBoardMenuButtonStyle(IDC_BTN_HIDDEN, TRUE);
			}
			m_board.DoBlank();
		}
	}
	void SetBoardRecord() {
		if (IsRecording()) {
			SetBoardMenuButtonStyle(IDC_BTN_REC, FALSE);
		}
		else {
			
		}
		DoRecord();
	}

	void SetBoardClose() {
		if (m_board.GetPageMode() == PAGE_EMPTYBOARD) {
			SetEmptyBoard();
		}
		else {
			if (AfxMessageBox(L"판서 페이지를 종료 하시겠습니까?", MB_OKCANCEL) == IDOK) {
				SetBoardMenuButtonStyle(IDC_BTN_EXIT, FALSE);
				//SetButtonStyle(IDC_BTN_EXIT, FALSE);
				CloseBook();
			}
		}
	}


	// book mark
	void SetBookMarkPage(std::wstring strPage) {
		m_bookMarkDlg.SetPage(strPage);
	}


	// ink tool
	void SetPenColor(int index) {
		m_inkDlg.DoButtonLayoutChange(1 + index, TRUE);
		m_toolDlg.DoButtonLayoutChange(6 + index, TRUE);
		m_board.SetPenColor(index);
		if (m_board.GetOperationType() == ImageOperationTypeEraser) {
			SetPenType(m_board.GetPenType());

		}
	}
	void SetPenSize(int index) {
		//m_toolDlg.DoButtonLayoutChange(12 + index, TRUE);
		m_toolDlg.ChangePenSizeText();
		m_board.SetPenSize(index);
	}
	void SetPenSizeUp() {
		m_board.SetPenSizeUp();
	}
	void SetPenSizeDown() {
		m_board.SetPenSizeDown();
	}
	int GetPenSizeIndex() {
		return m_board.GetPen()->GetSizeIndex();
	}

	void SetPenType(int index) {
		m_board.SetPenType(index);		// 먼저 저장
		m_toolDlg.DoButtonLayoutChange(1 + index, TRUE);	// 펜 종류 선택

		// 펜 종류에 따라 색상, 선굵기 변동 적용
		//SetPenColor(m_board.GetPen()->GetColorIndex());
		//SetPenSize(m_board.GetPen()->GetSizeIndex());
		m_toolDlg.DoButtonLayoutChange(6 + m_board.GetPen()->GetColorIndex(), TRUE);
		m_toolDlg.ChangePenSizeText();

	}
	//void SetEraser() {
		//m_toolDlg.DoButtonLayoutChange(3, TRUE);
		//m_board.SetPenAlpha(index);
	//}


	// editor mode
	void SetEditorPreviousPage() {
		m_editorDlg.PreviousPage();
	}
	void SetEditorNextPage() {
		m_editorDlg.NextPage();
	}
	void SetEditorVideoInsert() {
		m_editorDlg.VideoInsert();
	}
	void SetEditorAudioInsert() {
		m_editorDlg.AudioInsert();
	}
	void SetEditorPictureInsert() {
		m_editorDlg.PictureInsert();
	}
	void SetEditorMemoInsert() {
		m_editorDlg.MemoInsert();
	}
	void SetEditorBlankInsert() {
		m_editorDlg.BlankInsert();
	}
	void SetEditorToolMenuButttonSelect(BOOL select) {
		m_editorToolDlg.DoButtonLayoutChange(NULL, select);
	}


	// 교재 선택 판서 모드 시작
	void OpenBook(std::wstring strBook, bool bLandscape);
	// 새로운 교재 추가
	void AddBook();
	// 교재 삭제
	void SetDeleteBookMode();
	void ShowBookDeleteDialog(int cmd);
	void DoDeleteBook();
	// 교재 종료 메인 모드 시작
	void CloseBook();
	// 교재 리스트 갱신
	void RedrawBookList();
	// 보드 모드 변경
	void SetWhiteBoardViewChange();
	// 보드 메뉴 버튼 스타일 변경
	void SetBoardMenuButtonStyle(UINT id, BOOL bSelect);
	// 항상 보드판에 포커스
	void ResetFocus() {
		m_board.SetFocus();
	}

	// 교재 편집 모드
	void OpenEditor(std::wstring strBook, bool bLandscape);
	void CloseEditorBook();
	void SaveEditorBook();


	/****************************************************************************
	// about record
	****************************************************************************/
	BOOL m_bIsRecording;
	BOOL IsRecording() { return m_bIsRecording; }
	void DoRecord();
	ProcessWindowsInfo* m_AbleRecInfo;
	//CCreadPipe m_pipeLine;
	HWND	m_hwndRec;
	void	RunRecHandle();
	void	StartRecordSendMessage();
	void	StopRecordSendMessage();
	//std::wstring GetNow();

	void GetProgramPath();
	std::wstring m_strExecutePath;

	// 설정 변경
	void ChangeFullSize();


	// 구현입니다.
protected:
	HICON m_hIcon;




	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	//virtual BOOL OnTouchInput(CPoint pt, int nInputNumber, int nInputsCount, PTOUCHINPUT pInput);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// user message
	LRESULT OnLayoutMessage(WPARAM wParam, LPARAM lParam);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnExitSizeMove();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
