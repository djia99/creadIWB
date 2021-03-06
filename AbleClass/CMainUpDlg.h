#pragma once

#include "CButtonDlg.h"

class CMainUpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainUpDlg)

public:
	CMainUpDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMainUpDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_UP };
#endif


	enum {
		BTN_BOOK_LIST = 1,
		BTN_VIDEO_LIST,
		BTN_LOGO,
		BTN_EXIT,
	};



	// skin
	Gdiplus::Image* m_pBack;

	CButtonDlg m_btnBookList;
	CButtonDlg m_btnVideoList;
	//CButtonDlg m_btnLogo;
	CButtonDlg m_btnExit;



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	LRESULT OnCreadButton(WPARAM wParam, LPARAM lParam);
};
