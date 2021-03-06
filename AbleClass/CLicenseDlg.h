#pragma once

#include "CCert.h"


class CLicenseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLicenseDlg)

public:
	CLicenseDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLicenseDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LICENSE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


public:
	CEdit m_editId;
	CEdit m_editLicenseKey;

	CCert m_cert;


	afx_msg void OnBnClickedOk();
};

