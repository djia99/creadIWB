#pragma once


// CButtonDlg 대화 상자
#define MSG_CREAD_BUTTON (WM_USER + 1000)
#define WM_COLOR_UPDATE (WM_USER + 1001)

class CMyMFCColorButton : public CMFCColorButton {
public:
	CMyMFCColorButton() {};
	virtual ~CMyMFCColorButton() {};

	CWnd* m_pParent;
	void SetParent(CWnd* pParent) {
		m_pParent = pParent;
	}

	COLORREF m_color;
	COLORREF GetUpdateColor() { return m_color; }

	virtual void UpdateColor(COLORREF color) {
		m_color = color;
		m_pParent->SendMessage(WM_COLOR_UPDATE, NULL, NULL);
	}
};


class CButtonDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CButtonDlg)

public:
	CButtonDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CButtonDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BTN_DLG };
#endif

private:
	std::wstring m_pathNormal;
	std::wstring m_pathHover;
	std::wstring m_pathSelect;

	BOOL m_bSelected;


	int m_width;
	int m_height;

	HWND m_pParent;
	UINT m_nButtonId;
	BOOL	m_bIsHovering;
	BOOL	m_bIsTracking;

	Gdiplus::Image* m_imgNormal;
	Gdiplus::Image* m_imgHover;
	Gdiplus::Image* m_imgSelect;

	COLORREF m_clr;
	BOOL	m_bInk;
	BOOL	m_bInkUse;
	BOOL	m_bPen;


	CString			m_tooltext;
	CToolTipCtrl*	m_pToolTip;

public:
	void SetPen(BOOL b);
	void SetImage(UINT buttonId, HWND pParent, std::wstring pathNormal, std::wstring pathOver, std::wstring pathSelect, int width=0, int height=0, COLORREF clr=0, BOOL bInk=FALSE);
	void SetColor(COLORREF clr, BOOL bInkUse);
	void SetInk(BOOL b) { m_bInk; }
	void SetSelect(BOOL bSelect);
	void			InitToolTip();
	void SetToolTipText(CString spText, BOOL bActivate = TRUE);
	void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
	//void SetHorizontal(bool ImagesAreLaidOutHorizontally = FALSE);
	void DeleteToolTip();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
