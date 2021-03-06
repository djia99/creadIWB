#pragma once

#include "CButtonDlg.h"
// CBookDeleteDlg 대화 상자

class CBookDeleteDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBookDeleteDlg)

public:
	CBookDeleteDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CBookDeleteDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BOOK_DELETE };
#endif

	enum {
		BTN_DELETE = 1,
		BTN_CLOSE
	};

	std::wstring m_strMessage;
	void SetMessage(std::wstring message) {
		m_strMessage = message;
		if (m_pBack)
		{
			UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack, m_alpha);
		}
	}


	// skin
	Gdiplus::Image* m_pBack;
	BOOL m_bIsBoardMenuVisible;
	long m_alpha;

	Gdiplus::Font* m_pFont;
	Gdiplus::SolidBrush* m_pBrush;

	CButtonDlg m_btnDelete;
	CButtonDlg m_btnClose;

	void UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, BYTE alpha);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	LRESULT OnCreadButton(WPARAM wParam, LPARAM lParam);
};
