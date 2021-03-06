#pragma once


// CToolDlg 대화 상자

#include "CCreadButton.h"
#include "CButtonDlg.h"


class CToolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CToolDlg)

public:
	CToolDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CToolDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOL_MENU };
#endif

	enum {
		BTN_PEN = 1,
		BTN_HPEN,
		BTN_ERASER,
		BTN_WASTEBASKET,
		BTN_RETURN,
		BTN_COLOR1,
		BTN_COLOR2,
		BTN_COLOR3,
		BTN_COLOR4,
		BTN_COLOR5,
		BTN_COLOR6,
		BTN_PENSIZE1,
		BTN_PENSIZE2,
		BTN_PENSIZE3,
		BTN_PENSIZE4,
		BTN_PENSIZE5,
		BTN_PENSIZE6,
		BTN_PENSIZE_UP,
		BTN_PENSIZE_DOWN,
		BTN_PENSIZE_TEXT1,
		BTN_PENSIZE_TEXT2,
		BTN_PENSIZE_TEXT3,
		BTN_PENSIZE_TEXT4,
		BTN_PENSIZE_TEXT5,
		BTN_PENSIZE_TEXT6,
		BTN_CLOSE
	};


	// skin
	Gdiplus::Image* m_pBack;
	Gdiplus::Image* m_pBackInk;

	CButtonDlg m_btnPen;
	CButtonDlg m_btnHPen;
	CButtonDlg m_btnEraser;
	CButtonDlg m_btnWastebasket;
	CButtonDlg m_btnReturn;
	CButtonDlg m_btnColor[6];
	//CButtonDlg m_btnPenSize[6];
	CButtonDlg m_btnPenSizeUp;
	CButtonDlg m_btnPenSizeDown;
	CButtonDlg m_btnPenSizeText1;
	CButtonDlg m_btnPenSizeText2;
	CButtonDlg m_btnPenSizeText3;
	CButtonDlg m_btnPenSizeText4;
	CButtonDlg m_btnPenSizeText5;
	CButtonDlg m_btnPenSizeText6;
	CButtonDlg m_btnClose;


	stPenColor	m_color[6];
	void		GetColorInfoFromDatabase();

	BOOL		m_bClick;
	BOOL		m_bDrag;
	BOOL		IsDrag() { return m_bDrag; }

	BOOL		m_bPen;
	BOOL		IsPen() { return m_bPen; }

	// 잉크창 모드 <-> 기본 모드 전환
	BOOL m_bInkMode;
	void InkModeLayoutChange();

	// layout
	//int nButtonSize;
	//int nColorSize;
	int nLeftOffset;

	void ChangePenSizeText();
	void DoButtonLayoutChange(UINT id, BOOL selected) {

		switch (id) {
		case BTN_PEN:
			m_bPen = TRUE;
			m_btnPen.SetSelect(TRUE);
			m_btnHPen.SetSelect(FALSE);
			m_btnEraser.SetSelect(FALSE);
			break;
		case BTN_HPEN:
			m_bPen = FALSE;
			m_btnPen.SetSelect(FALSE);
			m_btnHPen.SetSelect(TRUE);
			m_btnEraser.SetSelect(FALSE);
			break;
		case BTN_ERASER:
			m_btnPen.SetSelect(FALSE);
			m_btnHPen.SetSelect(FALSE);
			m_btnEraser.SetSelect(selected);
			break;
		case BTN_COLOR1:
			m_btnColor[0].SetSelect(selected);
			m_btnColor[1].SetSelect(!selected);
			m_btnColor[2].SetSelect(!selected);
			m_btnColor[3].SetSelect(!selected);
			m_btnColor[4].SetSelect(!selected);
			m_btnColor[5].SetSelect(!selected);

			if (m_bPen) {
				m_btnPen.SetColor(m_color[0].clr, TRUE);
				m_btnPen.SetSelect(TRUE);
			}
			else {
				m_btnHPen.SetColor(m_color[0].clr, TRUE);
				m_btnHPen.SetSelect(TRUE);
			}
			m_btnClose.SetColor(m_color[0].clr, TRUE);
			m_btnClose.SetSelect(IsPen());

			break;
		case BTN_COLOR2:
			m_btnColor[0].SetSelect(!selected);
			m_btnColor[1].SetSelect(selected);
			m_btnColor[2].SetSelect(!selected);
			m_btnColor[3].SetSelect(!selected);
			m_btnColor[4].SetSelect(!selected);
			m_btnColor[5].SetSelect(!selected);
			if (m_bPen) {
				m_btnPen.SetColor(m_color[1].clr, TRUE);
				m_btnPen.SetSelect(TRUE);
			}
			else {
				m_btnHPen.SetColor(m_color[1].clr, TRUE);
				m_btnHPen.SetSelect(TRUE);
			}
			m_btnClose.SetColor(m_color[1].clr, TRUE);
			m_btnClose.SetSelect(IsPen());
			break;
		case BTN_COLOR3:
			m_btnColor[0].SetSelect(!selected);
			m_btnColor[1].SetSelect(!selected);
			m_btnColor[2].SetSelect(selected);
			m_btnColor[3].SetSelect(!selected);
			m_btnColor[4].SetSelect(!selected);
			m_btnColor[5].SetSelect(!selected);
			if (m_bPen) {
				m_btnPen.SetColor(m_color[2].clr, TRUE);
				m_btnPen.SetSelect(TRUE);
			}
			else {
				m_btnHPen.SetColor(m_color[2].clr, TRUE);
				m_btnHPen.SetSelect(TRUE);
			}
			m_btnClose.SetColor(m_color[2].clr, TRUE);
			m_btnClose.SetSelect(IsPen());
			break;
		case BTN_COLOR4:
			m_btnColor[0].SetSelect(!selected);
			m_btnColor[1].SetSelect(!selected);
			m_btnColor[2].SetSelect(!selected);
			m_btnColor[3].SetSelect(selected);
			m_btnColor[4].SetSelect(!selected);
			m_btnColor[5].SetSelect(!selected);
			if (m_bPen) {
				m_btnPen.SetColor(m_color[3].clr, TRUE);
				m_btnPen.SetSelect(TRUE);
			}
			else {
				m_btnHPen.SetColor(m_color[3].clr, TRUE);
				m_btnHPen.SetSelect(TRUE);
			}
			m_btnClose.SetColor(m_color[3].clr, TRUE);
			m_btnClose.SetSelect(IsPen());

			break;
		case BTN_COLOR5:
			m_btnColor[0].SetSelect(!selected);
			m_btnColor[1].SetSelect(!selected);
			m_btnColor[2].SetSelect(!selected);
			m_btnColor[3].SetSelect(!selected);
			m_btnColor[4].SetSelect(selected);
			m_btnColor[5].SetSelect(!selected);
			if (m_bPen) {
				m_btnPen.SetColor(m_color[4].clr, TRUE);
				m_btnPen.SetSelect(TRUE);
			}
			else {
				m_btnHPen.SetColor(m_color[4].clr, TRUE);
				m_btnHPen.SetSelect(TRUE);
			}
			m_btnClose.SetColor(m_color[4].clr, TRUE);
			m_btnClose.SetSelect(IsPen());

			break;
		case BTN_COLOR6:
			m_btnColor[0].SetSelect(!selected);
			m_btnColor[1].SetSelect(!selected);
			m_btnColor[2].SetSelect(!selected);
			m_btnColor[3].SetSelect(!selected);
			m_btnColor[4].SetSelect(!selected);
			m_btnColor[5].SetSelect(selected);
			if (m_bPen) {
				m_btnPen.SetColor(m_color[5].clr, TRUE);
				m_btnPen.SetSelect(TRUE);
			}
			else {
				m_btnHPen.SetColor(m_color[5].clr, TRUE);
				m_btnHPen.SetSelect(TRUE);
			}
			m_btnClose.SetColor(m_color[5].clr, TRUE);
			m_btnClose.SetSelect(IsPen());
			break;
		case BTN_PENSIZE_UP:
		case BTN_PENSIZE_DOWN:
			ChangePenSizeText();
			break;
		//case BTN_PENSIZE1:
		//	m_btnPenSize[0].SetSelect(selected);
		//	m_btnPenSize[1].SetSelect(!selected);
		//	m_btnPenSize[2].SetSelect(!selected);
		//	m_btnPenSize[3].SetSelect(!selected);
		//	m_btnPenSize[4].SetSelect(!selected);
		//	m_btnPenSize[5].SetSelect(!selected);
		//	break;
		//case BTN_PENSIZE2:
		//	m_btnPenSize[0].SetSelect(!selected);
		//	m_btnPenSize[1].SetSelect(selected);
		//	m_btnPenSize[2].SetSelect(!selected);
		//	m_btnPenSize[3].SetSelect(!selected);
		//	m_btnPenSize[4].SetSelect(!selected);
		//	m_btnPenSize[5].SetSelect(!selected);
		//	break;
		//case BTN_PENSIZE3:
		//	m_btnPenSize[0].SetSelect(!selected);
		//	m_btnPenSize[1].SetSelect(!selected);
		//	m_btnPenSize[2].SetSelect(selected);
		//	m_btnPenSize[3].SetSelect(!selected);
		//	m_btnPenSize[4].SetSelect(!selected);
		//	m_btnPenSize[5].SetSelect(!selected);
		//	break;
		//case BTN_PENSIZE4:
		//	m_btnPenSize[0].SetSelect(!selected);
		//	m_btnPenSize[1].SetSelect(!selected);
		//	m_btnPenSize[2].SetSelect(!selected);
		//	m_btnPenSize[3].SetSelect(selected);
		//	m_btnPenSize[4].SetSelect(!selected);
		//	m_btnPenSize[5].SetSelect(!selected);
		//	break;
		//case BTN_PENSIZE5:
		//	m_btnPenSize[0].SetSelect(!selected);
		//	m_btnPenSize[1].SetSelect(!selected);
		//	m_btnPenSize[2].SetSelect(!selected);
		//	m_btnPenSize[3].SetSelect(!selected);
		//	m_btnPenSize[4].SetSelect(selected);
		//	m_btnPenSize[5].SetSelect(!selected);
		//	break;
		//case BTN_PENSIZE6:
		//	m_btnPenSize[0].SetSelect(!selected);
		//	m_btnPenSize[1].SetSelect(!selected);
		//	m_btnPenSize[2].SetSelect(!selected);
		//	m_btnPenSize[3].SetSelect(!selected);
		//	m_btnPenSize[4].SetSelect(!selected);
		//	m_btnPenSize[5].SetSelect(selected);
		//	break;
		default:
			break;
		}
	}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	LRESULT OnCreadButton(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
