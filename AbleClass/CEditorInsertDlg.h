#pragma once


// CEditorInsertDlg 대화 상자

class CEditorInsertDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditorInsertDlg)

public:
	CEditorInsertDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CEditorInsertDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITOR_INSERT_DLG };
#endif

	CEdit			m_editFilePath;
	std::wstring	m_strFile;
	eItemType		m_type;
	void SetType(eItemType type) {
		m_type = type;
	}


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnFileOpen();
};
