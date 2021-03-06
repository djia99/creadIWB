// CLicenseDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CLicenseDlg.h"
#include "afxdialogex.h"

// CLicenseDlg 대화 상자

IMPLEMENT_DYNAMIC(CLicenseDlg, CDialogEx)

CLicenseDlg::CLicenseDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LICENSE_DLG, pParent)
{

}

CLicenseDlg::~CLicenseDlg()
{
}

void CLicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MB_ID, m_editId);
	DDX_Control(pDX, IDC_MB_LICENSE_KEY, m_editLicenseKey);
}


BEGIN_MESSAGE_MAP(CLicenseDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLicenseDlg::OnBnClickedOk)
END_MESSAGE_MAP()




void CLicenseDlg::OnBnClickedOk()
{
	CString textId;
	CString textLicenseKey;
	m_editId.GetWindowText(textId);
	m_editLicenseKey.GetWindowText(textLicenseKey);

	std::string sId = CT2A(textId);
	std::string sLicenseKey = CT2A(textLicenseKey);


	if (m_cert.RegisterCert(sId, sLicenseKey) == TRUE) {
		AfxMessageBox(L"제품 등록이 성공적으로 완료되었습니다.");
	}
	else {
		AfxMessageBox(L"제품 등록에 실패하였습니다. \n아이디, 라이센스키 입력을 확인하시거나 온라인 상태를 확인하신 후 잠시후 다시 시도해 주시길 바랍니다.");
	}




	CDialogEx::OnOK();
}
