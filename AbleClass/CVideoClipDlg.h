#pragma once

#include "CButtonDlg.h"

// media player
#include <mfplay.h>
#include <mferror.h>
#include <Shlwapi.h>


class CVideoClipDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVideoClipDlg)

public:
	CVideoClipDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CVideoClipDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIDEO_CLIP };
#endif


	//enum {
	//	BTN_INK_COLOR1 = 1,
	//	BTN_INK_COLOR2,
	//	BTN_INK_COLOR3,
	//	BTN_INK_COLOR4,
	//	BTN_INK_COLOR5,
	//	BTN_INK_COLOR6,
	//	BTN_GONE
	//};


	// skin
	//Gdiplus::Image* m_pBack;
	//Gdiplus::Image* m_pBackGone;

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
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
