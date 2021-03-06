#pragma once


#include "CDocumentConvert.h"
#include "CCreadButton.h"


class CBookDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBookDlg)

public:
	CBookDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CBookDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BOOK_DLG };
#endif


	// skin
	void InitDialogSkin();
	HBITMAP m_hBitmap;
	Gdiplus::Image* m_pBack;


	CEdit			m_editFilePath;
	CEdit			m_editTitle;
	CEdit			m_editContent;
	CButton			m_btnFileOpen;
	CCreadButton	m_btnStart;
	CCreadButton	m_btnCancel;
	CProgressCtrl	m_progress;

	// progress
	int				m_nProgressPos;
	int				m_nProgressMax;
	int				m_nProgressPosDetail; // 상태바 초단위 세분화

	std::wstring	m_strRootDir;
	std::wstring	m_strBookDir;			// 교재 기본 폴더
	ST_FILEDATA		m_convertFile;


	// convert start
	// convert thread
	BOOL						m_bConverting;
	ComPtr<IDocumentConvert>	m_pConvertThread;
	CRITICAL_SECTION*			m_criticalSection;
	void						ConvertStart();
	void						ConvertStop();
	void						ConvertThreadAction(bool on);
	void						UpdateDialog(bool bConverting);

	BOOL						m_bIsNewItem;
	BOOL						IsExistNewItem() { return m_bIsNewItem; }




protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedBtnFileOpen();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedCancel();
	afx_msg LRESULT OnThreadUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

};
