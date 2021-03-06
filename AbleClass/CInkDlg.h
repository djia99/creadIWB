#pragma once


#include "CButtonDlg.h"





class CInkDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInkDlg)

public:
	CInkDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CInkDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INK };
#endif

	enum {
		BTN_INK_COLOR1 = 1,
		BTN_INK_COLOR2,
		BTN_INK_COLOR3,
		BTN_INK_COLOR4,
		BTN_INK_COLOR5,
		BTN_INK_COLOR6,
		BTN_GONE
	};


	// skin
	Gdiplus::Image* m_pBack;
	Gdiplus::Image* m_pBackGone;

	CButtonDlg m_btnInkColor[6];
	CButtonDlg m_btnGone;


	stPenColor	m_color[6];
	void		GetColorInfoFromDatabase();


	BOOL m_bIsGone;
	void LayoutChange();

	void DoButtonLayoutChange(UINT id, BOOL selected) {
		switch (id) {
		case BTN_INK_COLOR1:
			m_btnInkColor[0].SetSelect(selected);
			m_btnInkColor[1].SetSelect(!selected);
			m_btnInkColor[2].SetSelect(!selected);
			m_btnInkColor[3].SetSelect(!selected);
			m_btnInkColor[4].SetSelect(!selected);
			m_btnInkColor[5].SetSelect(!selected);
			break;
		case BTN_INK_COLOR2:
			m_btnInkColor[0].SetSelect(!selected);
			m_btnInkColor[1].SetSelect(selected);
			m_btnInkColor[2].SetSelect(!selected);
			m_btnInkColor[3].SetSelect(!selected);
			m_btnInkColor[4].SetSelect(!selected);
			m_btnInkColor[5].SetSelect(!selected);
			break;
		case BTN_INK_COLOR3:
			m_btnInkColor[0].SetSelect(!selected);
			m_btnInkColor[1].SetSelect(!selected);
			m_btnInkColor[2].SetSelect(selected);
			m_btnInkColor[3].SetSelect(!selected);
			m_btnInkColor[4].SetSelect(!selected);
			m_btnInkColor[5].SetSelect(!selected);
			break;
		case BTN_INK_COLOR4:
			m_btnInkColor[0].SetSelect(!selected);
			m_btnInkColor[1].SetSelect(!selected);
			m_btnInkColor[2].SetSelect(!selected);
			m_btnInkColor[3].SetSelect(selected);
			m_btnInkColor[4].SetSelect(!selected);
			m_btnInkColor[5].SetSelect(!selected);
			break;
		case BTN_INK_COLOR5:
			m_btnInkColor[0].SetSelect(!selected);
			m_btnInkColor[1].SetSelect(!selected);
			m_btnInkColor[2].SetSelect(!selected);
			m_btnInkColor[3].SetSelect(!selected);
			m_btnInkColor[4].SetSelect(selected);
			m_btnInkColor[5].SetSelect(!selected);
			break;
		case BTN_INK_COLOR6:
			m_btnInkColor[0].SetSelect(!selected);
			m_btnInkColor[1].SetSelect(!selected);
			m_btnInkColor[2].SetSelect(!selected);
			m_btnInkColor[3].SetSelect(!selected);
			m_btnInkColor[4].SetSelect(!selected);
			m_btnInkColor[5].SetSelect(selected);
			break;
		default:
			break;
		}
	}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	LRESULT OnCreadButton(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
