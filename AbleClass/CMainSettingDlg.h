#pragma once

#include "CCreadButton.h"
#include "CButtonDlg.h"

// CMainSettingDlg 대화 상자

class CMainSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainSettingDlg)

public:
	CMainSettingDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMainSettingDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_SETTING_DLG };
#endif



	void SetDlgItem();
	void Save();
	void UpdatePenStyleDlgItem();
	void SetPenStyleDlgItem(PENTIP_STYLE ps);
	BOOL IsFullSizeUpdate() {
		return m_bFullSizeUpdate;
	}


	BOOL CheckCert();


protected:
	BOOL m_bFullSizeUpdate;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
public:

	HBRUSH m_backColor;
	HBITMAP m_hBitmap;
	Gdiplus::Image* m_pBack;


	CButton m_chkFullSize;

	CCreadButton m_chkPenStyle1;
	CCreadButton m_chkPenStyle2;
	CCreadButton m_chkPenStyle3;
	CButton m_chkPenStyle1Clone;
	CButton m_chkPenStyle2Clone;
	CButton m_chkPenStyle3Clone;
	CButton m_chkBoardPressure;
	CButton m_chkBoardReversal;
	CCreadButton m_btnColor[6];
	CButton m_btnLicense;
	CCreadButton m_btnOk;
	CCreadButton m_btnCancel;
	CStatic m_staticVersion;
	CEdit m_editRecPath;

	stPenColor m_color[6];
	CMyMFCColorButton m_colorPicker;
	int		m_currentColorDlgItemNumber;
	CArray<COLORREF, COLORREF> m_colorPalette;
	std::wstring strAutoColorText;
	std::wstring strOtherText;
	std::wstring strDocColorsText;
	CList<COLORREF, COLORREF> m_lstDocColors;
	void	InitColorPalette();
	void	LoadColorPalette();

	afx_msg void OnBnClickedChkFullsize();
	afx_msg void OnBnClickedChkPressure();
	afx_msg void OnBnClickedChkPenStyle1();
	afx_msg void OnBnClickedChkPenStyle2();
	afx_msg void OnBnClickedChkPenStyle3();
	afx_msg void OnBnClickedChkPenStyle1Clone();
	afx_msg void OnBnClickedChkPenStyle2Clone();
	afx_msg void OnBnClickedChkPenStyle3Clone();
	afx_msg void OnBnClickedColor1();
	afx_msg void OnBnClickedColor2();
	afx_msg void OnBnClickedColor3();
	afx_msg void OnBnClickedColor4();
	afx_msg void OnBnClickedColor5();
	afx_msg void OnBnClickedColor6();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	LRESULT OnColorUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnLicense();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);


	afx_msg void OnBnClickedBtnSavePath();
};
