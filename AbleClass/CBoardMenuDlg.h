#pragma once

#include "CCreadButton.h"




class CBoardMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CBoardMenuDlg)

public:
	CBoardMenuDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CBoardMenuDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BOARD_MENU };
#endif

	BOOL m_bTrackMouse;

	// skin
	void InitDialogSkin();
	HBITMAP m_hBitmap;
	Gdiplus::Image* m_pBack;


	// button
	CCreadButton m_btnAll;
	CCreadButton m_btnNext;
	CCreadButton m_btnBefore;
	CCreadButton m_btnExpand;
	CCreadButton m_btnReduce;
	CCreadButton m_btnPen;
	CCreadButton m_btnBoard;
	CCreadButton m_btnClipping;
	CCreadButton m_btnRec;
	CCreadButton m_btnHidden;
	CCreadButton m_btnExit;


	// board menu button style change
	void SetButtonStyle(UINT id, BOOL selected) {

		switch (id) {
		case IDC_BTN_CLIPPING:
			m_btnClipping.SetSelect(selected);
			if (selected) {
				m_btnExpand.SetSelect(FALSE);
				m_btnPen.SetSelect(FALSE);
				m_btnBoard.SetSelect(FALSE);
			}
			break;
		case IDC_BTN_EXPAND:
			m_btnExpand.SetSelect(selected);
			if (selected) {
				m_btnClipping.SetSelect(FALSE);
				m_btnPen.SetSelect(FALSE);
				m_btnBoard.SetSelect(FALSE);
			}
			break;
		case IDC_BTN_PEN:
			m_btnPen.SetSelect(selected);
			if (selected) {
				m_btnExpand.SetSelect(FALSE);
				m_btnClipping.SetSelect(FALSE);
			}
			break;
		case IDC_BTN_BOARD:
			m_btnBoard.SetSelect(selected);
			//if (selected) {
			//	m_btnPen.SetSelect(TRUE);
			//}
			//m_btnExpand.SetSelect(FALSE);
			//m_btnClipping.SetSelect(FALSE);
			break;
		case IDC_BTN_ALL:
		case IDC_BTN_EXIT:
			m_btnExpand.SetSelect(selected);
			m_btnPen.SetSelect(selected);
			m_btnBoard.SetSelect(selected);
			m_btnClipping.SetSelect(selected);
			break;
		case IDC_BTN_HIDDEN:
			m_btnHidden.SetSelect(selected);
			break;
		case IDC_BTN_REC:
			m_btnRec.SetSelect(selected);
			break;
		default:
			break;
		}
	}


	// layout
	int m_nButtonWidth;
	void Resize();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnBefore();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnNext();
	afx_msg void OnBnClickedBtnExpand();
	afx_msg void OnBnClickedBtnClipping();
	afx_msg void OnBnClickedBtnPen();
	afx_msg void OnBnClickedBtnReduce();
	afx_msg void OnBnClickedBtnAll();
	afx_msg void OnBnClickedBtnBoard();
	afx_msg void OnBnClickedBtnHidden();
	afx_msg void OnBnClickedBtnRecord();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
