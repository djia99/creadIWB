// CEditorInsertDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CEditorInsertDlg.h"
#include "afxdialogex.h"


// CEditorInsertDlg 대화 상자

IMPLEMENT_DYNAMIC(CEditorInsertDlg, CDialogEx)

CEditorInsertDlg::CEditorInsertDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDITOR_INSERT_DLG, pParent), 
	m_strFile(L"")
{

}

CEditorInsertDlg::~CEditorInsertDlg()
{
}

void CEditorInsertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_PATH, m_editFilePath);
}


BEGIN_MESSAGE_MAP(CEditorInsertDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FILE_OPEN, &CEditorInsertDlg::OnBnClickedBtnFileOpen)
END_MESSAGE_MAP()


// CEditorInsertDlg 메시지 처리기
void CEditorInsertDlg::OnBnClickedBtnFileOpen()
{
	const DWORD fileNameMaxLength = MAX_PATH + 1;
	const DWORD bufferSize = MAX_PATH + 1;
	TCHAR* filenamesBuffer = new TCHAR[bufferSize];
	filenamesBuffer[0] = NULL;
	filenamesBuffer[bufferSize - 1] = NULL;

	std::wstring fileFilter;

	if (m_type == EDITOR_VIDEO) {
		fileFilter = L"비디오 파일(*.mp4; *.wmv;) | *.mp4; *.wmv;| All Files(*.*) | *.* || ";
	}
	else if (m_type == EDITOR_AUDIO) {
		fileFilter = L"오디오 파일(*.mp3; *.wma;) | *.mp3; *.wma; | All Files(*.*) | *.* || ";
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES,
		fileFilter.c_str(), this);
	dlg.m_ofn.lpstrFile = filenamesBuffer;
	dlg.m_ofn.nMaxFile = bufferSize;
	int key = 0;

	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		m_strFile = dlg.GetNextPathName(fileNamesPosition);
		if (!m_strFile.empty()) {
			m_editFilePath.SetWindowTextW(m_strFile.c_str());
		}
	}
	delete[] filenamesBuffer;
}
