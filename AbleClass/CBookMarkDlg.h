#pragma once
#include "CButtonDlg.h"

// CBookMarkDlg 대화 상자

class CBookMarkDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBookMarkDlg)

public:
	CBookMarkDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CBookMarkDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BOOKMARK };
#endif

	std::wstring m_strPage;
	void SetPage(std::wstring page) {
		m_strPage = page;
		if (m_pBack)
		{
			UpdateLayeredWindowPng(m_hWnd, m_hWnd, m_pBack, m_alpha);
		}
	}


	// skin
	Gdiplus::Image* m_pBack;
	BOOL m_bIsBoardMenuVisible;
	BYTE m_alpha;

	Gdiplus::Font* m_pFont;
	Gdiplus::SolidBrush* m_pBrush;

	CButtonDlg m_btnArrow;
	void SetBoardMenuVisible(BOOL b);
	void UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, BYTE alpha);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
