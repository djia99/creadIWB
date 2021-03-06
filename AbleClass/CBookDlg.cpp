// CBookDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "AbleClass.h"
#include "CBookDlg.h"
#include "afxdialogex.h"


// CBookDlg 대화 상자

IMPLEMENT_DYNAMIC(CBookDlg, CDialogEx)

CBookDlg::CBookDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BOOK_DLG, pParent),
	m_strRootDir(L"\\PoDo\\"),
	m_strBookDir(L""),
	m_bConverting(FALSE),
	m_pConvertThread(nullptr),
	m_bIsNewItem(FALSE),
	m_nProgressMax(1000)
{

}

CBookDlg::~CBookDlg()
{
	::DeleteObject(m_hBitmap);
}

void CBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_PATH, m_editFilePath);
	DDX_Control(pDX, IDC_TITLE, m_editTitle);
	DDX_Control(pDX, IDC_CONTENT, m_editContent);
	DDX_Control(pDX, IDC_BTN_FILE_OPEN, m_btnFileOpen);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_STATUS_PROGRESS, m_progress);
	DDX_Control(pDX, ID_CANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CBookDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FILE_OPEN, &CBookDlg::OnBnClickedBtnFileOpen)
	ON_WM_CREATE()
	//ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_START, &CBookDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(ID_CANCEL, &CBookDlg::OnBnClickedCancel)
	ON_MESSAGE(MESSAGE_THREAD_UPDATE, &CBookDlg::OnThreadUpdate)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


int CBookDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitDialogSkin();

	return 0;
}


void CBookDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}


void CBookDlg::OnClose()
{
	CDialogEx::OnClose();
}


void CBookDlg::InitDialogSkin() {

	SetWindowLong(m_hWnd, GWL_EXSTYLE,
		GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, 255, LWA_ALPHA);

	HINSTANCE	hInstResource = NULL;

	m_pBack = Gdiplus::Image::FromFile(g_pSkin->GetSource(L"main_menu/book_bg.bmp").c_str());
	if (m_pBack)
	{
		Gdiplus::Bitmap* pBitmap = static_cast<Gdiplus::Bitmap*>(m_pBack->Clone());
		Gdiplus::Status status = pBitmap->GetHBITMAP(Gdiplus::Color(0, 0, 0), &m_hBitmap);
	}


	m_btnStart.LoadStdImage(g_pSkin->GetSource(L"main_menu/book_btn_start_n.png").c_str());
	m_btnStart.LoadAltImage(g_pSkin->GetSource(L"main_menu/book_btn_start_o.png").c_str());
	m_btnStart.EnableToggle(TRUE);
	m_btnStart.SetToolTipText(_T("시작"));

	m_btnCancel.LoadStdImage(g_pSkin->GetSource(L"main_menu/book_btn_cancel_n.png").c_str());
	m_btnCancel.LoadAltImage(g_pSkin->GetSource(L"main_menu/book_btn_cancel_o.png").c_str());
	m_btnCancel.EnableToggle(TRUE);
	m_btnCancel.SetToolTipText(_T("취소"));

}

BOOL CBookDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	IShellItem *pShellItemDir;
	HRESULT hr = ::SHCreateItemInKnownFolder(FOLDERID_Documents, 0, nullptr, IID_PPV_ARGS(&pShellItemDir));
	wchar_t * buffer;
	hr = pShellItemDir->GetDisplayName(SIGDN_FILESYSPATH, &buffer);
	m_strBookDir = buffer;
	m_strBookDir.append(m_strRootDir);
	m_progress.SetRange(0, m_nProgressMax);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CBookDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(rect);

	//CMyMemDC pDevC(pDC, CRect(0, 0, rect.right, rect.bottom));
	CMyMemDC pDevC(pDC, CRect(0, 0, 4, 4));

	if (m_hBitmap)
	{
		BITMAP bit;
		GetObject(m_hBitmap, sizeof(BITMAP), &bit);
		HBITMAP oldBitmap = (HBITMAP)pDevC->SelectObject(m_hBitmap);
		pDC->StretchBlt(0, 0, rect.Width(), rect.Height(), pDevC, 0, 0, bit.bmWidth, bit.bmHeight, SRCCOPY);
		return TRUE;
	}
	else {
		return CDialogEx::OnEraseBkgnd(pDC);
	}

}


HBRUSH CBookDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);


	if (pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATUS_MESSAGE)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_1)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_2)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_3)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_4)->m_hWnd ||
		pWnd->GetSafeHwnd() == GetDlgItem(IDC_STATIC_5)->m_hWnd
		) {
		pDC->SetBkMode(TRANSPARENT); 
		return (HBRUSH)::GetStockObject(NULL_BRUSH); 
	}


	return hbr;
}



void CBookDlg::OnBnClickedBtnFileOpen()
{
	const DWORD fileNameMaxLength = 1000 + 1;
	const DWORD bufferSize = 1000 + 1;
	TCHAR* filenamesBuffer = new TCHAR[bufferSize];
	filenamesBuffer[0] = NULL;
	filenamesBuffer[bufferSize - 1] = NULL;

	CFileDialog dlg(TRUE, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES,
		//_T("문서파일 (*.doc,*.docx,*.xls,*.xlsx,*.ppt,*.pptx,*.pdf)|*.doc;*.docx;*.xls;*.xlsx;*.ppt;*.pptx;*.pdf;|이미지파일(*.png,*.jpg,*.bmp)|*.png;*.jpg;*.jpeg;*.bmp|All Files (*.*)|*.*||"), this);
		_T("문서파일 (*.doc,*.docx,*.ppt,*.pptx,*.pdf,*.hwp)|*.doc;*.docx;*.ppt;*.pptx;*.pdf;*.hwp;|이미지파일(*.png,*.jpg,*.bmp)|*.png;*.jpg;*.jpeg;*.bmp|All Files (*.*)|*.*||"), NULL);
	dlg.m_ofn.lpstrFile = filenamesBuffer;
	dlg.m_ofn.nMaxFile = bufferSize;
	int key = 0;
	memset(&m_convertFile, 0, sizeof(m_convertFile));

	if (dlg.DoModal() == IDOK)
	{
		POSITION fileNamesPosition = dlg.GetStartPosition();
		m_convertFile.strFilePath = dlg.GetNextPathName(fileNamesPosition);
		if (!m_convertFile.strFilePath.empty()) {
			m_convertFile.strFileName = ST_FILEDATA::GetFilename(m_convertFile.strFilePath);
			m_convertFile.strFileType = ST_FILEDATA::GetExtension(m_convertFile.strFileName);
			m_convertFile.strFileDisplayName = ST_FILEDATA::GetDisplayName(m_convertFile.strFileName);
			m_convertFile.result = 0;
			m_convertFile.format = ST_FILEDATA::GetFormat(m_convertFile.strFileType);
			m_editFilePath.SetWindowTextW(m_convertFile.strFilePath.c_str());
			m_editTitle.SetWindowTextW(m_convertFile.strFileDisplayName.c_str());
			
		}
	}
	delete[] filenamesBuffer;
}


void CBookDlg::OnBnClickedBtnStart()
{


	if (m_bConverting) {
		ConvertStop();
	}
	else {
		ConvertStart();
	}
}



void CBookDlg::ConvertStart() {
	m_bConverting = TRUE;
	UpdateDialog(true);
	if (m_pConvertThread != nullptr) {
		ConvertThreadAction(false);
	}
	ConvertThreadAction(true);
}

void CBookDlg::ConvertStop() {
	m_bConverting = FALSE;
	UpdateDialog(false);
	ConvertThreadAction(false);
}


void CBookDlg::ConvertThreadAction(bool on) {

	if (on == true) {
		HRESULT hr = SharedObject<CDocumentConvert>::Create(&m_pConvertThread);
		if (SUCCEEDED(hr))
		{
			hr = m_pConvertThread->GetCriticalSection(&m_criticalSection);

			m_strBookDir.append(m_convertFile.strFileDisplayName);
			m_pConvertThread->SetBookDir(m_strBookDir);
			m_pConvertThread->SetSourceFile(m_convertFile);
			m_pConvertThread->SetUiHandle(this);
			m_pConvertThread->Start();
		}
	}
	else {
		bool active = true;
		do {
			if (m_pConvertThread == nullptr) {
				break;
			}

			HRESULT hr = m_pConvertThread->IsWorkingThreadActive(&active);
			if (FAILED(hr))
			{
				active = false;
			}
			else
			{
				if (active)
				{
					m_pConvertThread->Stop();
					::Sleep(100);
				}
			}
		} while (active);

		m_pConvertThread = nullptr;
	}

}


void CBookDlg::UpdateDialog(bool bConverting) {
	if (bConverting) {
		m_btnStart.SetSelect(TRUE);
		m_btnStart.SetWindowTextW(L"중지 (&S)");
		m_editFilePath.EnableWindow(FALSE);
		m_editTitle.EnableWindow(FALSE);
		m_editContent.EnableWindow(FALSE);
		m_btnFileOpen.EnableWindow(FALSE);

	}
	else {
		m_btnStart.SetSelect(FALSE);
		m_btnStart.SetWindowTextW(L"시작 (&S)");
		m_editFilePath.EnableWindow(TRUE);
		m_editTitle.EnableWindow(TRUE);
		m_editContent.EnableWindow(TRUE);
		m_btnFileOpen.EnableWindow(TRUE);
	}
}



void CBookDlg::OnBnClickedCancel()
{
	if (m_pConvertThread != nullptr) {
		if (AfxMessageBox(_T("교재를 생성 중 입니다. 취소하시겠습니까?"), MB_OKCANCEL) == IDOK) {
			ConvertStop();
			CDialogEx::OnCancel();
		}
	}
	else {
		CDialogEx::OnCancel();
	}

	return;
}


LRESULT CBookDlg::OnThreadUpdate(WPARAM wParam, LPARAM lParam) {
	CString strItemText = _T("");
	CString strData = _T("");
	int nIndex = 0;



	//if ((lParam == 0) || (lParam == 1)) {
		//if (lParam == 0) SetTimer(1, 1000, NULL); // 프로그래스바 상세 출력 타이머 설정
		//m_nProgressPosDetail = 0;

		//T_ITER itr = m_mapFile.find((int)wParam);

		//if (itr != m_mapFile.end()) {
		//	strData.Format(_T("변환 파일 : %s"), itr->second.strFileName.c_str());

		//	for (nIndex = 0; nIndex<m_list.GetItemCount(); nIndex++) {
		//		if (itr == m_mapFile.find(m_list.GetItemData(nIndex))) {
		//			if (lParam == 0) m_list.SetItemText(nIndex, 3, _T("변환중"));
		//			else if (lParam == 1) {

		//				CString strOutputFile;
		//				if (m_fileFormat == JPG) {
		//					strOutputFile.Format(_T("%s\\%s_001.%s"), m_strOutputDir.c_str(), itr->second.strSaveFileName.c_str(), itr->second.strSaveFileFormat.c_str());
		//				}
		//				else {
		//					strOutputFile.Format(_T("%s\\%s.%s"), m_strOutputDir.c_str(), itr->second.strSaveFileName.c_str(), itr->second.strSaveFileFormat.c_str());
		//				}

		//				WIN32_FIND_DATA filedata;
		//				HANDLE hfile = FindFirstFile(strOutputFile, &filedata);

		//				if (hfile == INVALID_HANDLE_VALUE) {
		//					//strItemText.Format(_T("[실패]%s.%s"), itr->second.strSaveFileName.c_str(), itr->second.strSaveFileFormat.c_str());
		//					//strItemText.Format(_T("[실패]"), itr->second.strSaveFileName.c_str(), itr->second.strSaveFileFormat.c_str());
		//					strItemText = _T("[실패]파일에러");
		//					m_list.SetItemText(nIndex, 3, strItemText);
		//				}
		//				else {
		//					m_nProgressPos++;
		//					strItemText.Format(_T("[완료]%s.%s"), itr->second.strSaveFileName.c_str(), itr->second.strSaveFileFormat.c_str());
		//					m_list.SetItemText(nIndex, 3, strItemText);
		//				}

		//				FindClose(hfile);

		//			}
		//		}
		//	}


		//	if (m_nProgressPos>m_nProgressMax) m_nProgressPos = 0;
		//	m_progress.SetPos(m_nProgressPos * 10);

		//}
		//m_status.SetWindowTextW(strData);
		//m_btnOk.SetWindowTextW(_T("변환중..\n[중지 Click]"));
	//}

	if (wParam == WM_UPDATEUISTATE) {
		m_progress.SetPos(lParam);
	}
	else if (wParam == WM_QUIT) {
		m_bIsNewItem = TRUE;
		ConvertStop();
		if (AfxMessageBox(L"교재 생성이 완료 되었습니다.") == IDOK) {
			CDialogEx::OnCancel();
		}
	}

	return 0L;
}

void CBookDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	SendMessage(WM_NCLBUTTONDOWN, HTCAPTION, 0);
	CDialogEx::OnMouseMove(nFlags, point);
}
