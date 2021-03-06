#pragma once

#include "CButtonDlg.h"

class CMainMenuDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainMenuDlg)

public:
	CMainMenuDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMainMenuDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_MENU };
#endif

	enum {
		BTN_ADD = 1,
		BTN_DEL,
		BTN_SORT,
		BTN_SETTING
	};



	// skin
	Gdiplus::Image* m_pBack;

	CButtonDlg m_btnAdd;
	CButtonDlg m_btnDel;
	CButtonDlg m_btnSort;
	CButtonDlg m_btnSetting;



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
