#include "stdafx.h"
#include "CDocumentConvert.h"
#include "CDocumentAccessor.h"
#include "CMsWordAccessor.h"
#include "CMsPptAccessor.h"
#include "CHwpAccessor.h"

#include <experimental/filesystem>
#include ".\jpeg\jpeglib.h"
#include ".\jpeg\cdjpeg.h"



CDocumentConvert::CDocumentConvert() :
	m_pUiHandle(NULL),
	m_thdHandle(nullptr),
	m_bWorking(false),
	m_strPrinter(L"Document Printer"),
	m_strPostScriptFile(L""),
	m_strPostScriptFilePpt(L""),
	m_strBookDir(L""),
	m_strMediaDir(L""),
	m_strEtcDir(L""),
	m_strXxhdpiDir(L""),
	m_strXhdpiDir(L"")
{
	memset(&m_cs, 0, sizeof(m_cs));
	InitializeCriticalSection(&m_cs);
}


HRESULT CDocumentConvert::Initialize() {
	GetProgramPath();
	return S_OK;
}

HRESULT CDocumentConvert::Shutdown()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT CDocumentConvert::Start() {
	if (nullptr != m_thdHandle)
	{
		return S_OK;
	}

	m_bWorking = true;

	m_thdHandle = CreateThread(
		nullptr,
		0,
		ConvertThd,
		reinterpret_cast<void*>(this),
		0,
		&m_thdId);
	if (nullptr == m_thdHandle)
	{
		return E_FAIL;
	}

	if (!SetThreadPriority(m_thdHandle, THREAD_PRIORITY_IDLE))
	{
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CDocumentConvert::Stop() {
	CriticalSectionLocker csl(&m_cs);
	m_bWorking = false;
	return S_OK;
}


unsigned long WINAPI CDocumentConvert::ConvertThd(void* con)
{
	CDocumentConvert* processCls = reinterpret_cast<CDocumentConvert*>(con);
	HRESULT hr = S_OK;



	// process 
	processCls->Process();
	return hr;
}



HRESULT CDocumentConvert::GetCriticalSection(CRITICAL_SECTION** criticalSection)
{
	*criticalSection = &m_cs;
	return S_OK;
}

HRESULT CDocumentConvert::IsWorkingThreadActive(bool* active)
{
	if (nullptr == m_thdHandle)
	{
		*active = false;
		return S_OK;
	}

	unsigned long exitCode = 0;
	if (!GetExitCodeThread(m_thdHandle, &exitCode))
	{
		return E_FAIL;
	}

	if (STILL_ACTIVE == exitCode)
	{
		*active = true;
		return S_OK;
	}

	*active = false;
	return S_OK;
}



HRESULT CDocumentConvert::Process() {
	HRESULT hr = S_OK;


	Make_Directory();

	if (m_pUiHandle) {
		m_pUiHandle->PostMessage(MESSAGE_THREAD_UPDATE, WM_UPDATEUISTATE, 50);
	}

	if (m_bWorking && (SUCCEEDED(hr)))
	{
		{
			CriticalSectionLocker csl(&m_cs);


			if (m_sourceFile.format == DOC || m_sourceFile.format == DOCX) {
				CDocumentAccessor* pDocumentAccessor = new CMsWordAccessor;
				if (NULL == pDocumentAccessor)
					return S_FALSE;

				int nLandscapeType = 0;
				bool bResult = false;
				if (true == pDocumentAccessor->Initialize())
				{
					if (true == pDocumentAccessor->Open(m_sourceFile.strFilePath.c_str()))
					{
						nLandscapeType = pDocumentAccessor->Print(m_strPrinter.c_str(), m_strPostScriptFile.c_str());
						//TRACE(_T("\n>>>>>>>>> print (%d) <<<<<<<\n"), nLandscapeType);

						if (m_pUiHandle) {
							m_pUiHandle->PostMessage(MESSAGE_THREAD_UPDATE, WM_UPDATEUISTATE, 300);
						}


						if (nLandscapeType>-1)
							bResult = true;


					}
				}
				if (false == bResult)
				{
					return S_FALSE;
				}
				if (pDocumentAccessor) {
					pDocumentAccessor->Close();
					delete pDocumentAccessor;
				}
				PstoImage(nLandscapeType);
			}
			else if (m_sourceFile.format == PPT || m_sourceFile.format == PPTX) {
				CDocumentAccessor* pDocumentAccessor = new CMsPptAccessor;
				if (NULL == pDocumentAccessor)
					return S_FALSE;

				int nLandscapeType = 0;
				bool bResult = false;
				if (true == pDocumentAccessor->Initialize())
				{
					if (true == pDocumentAccessor->Open(m_sourceFile.strFilePath.c_str()))
					{
						nLandscapeType = pDocumentAccessor->Print(m_strPrinter.c_str(), m_strPostScriptFilePpt.c_str());
						if (nLandscapeType>-1)
							bResult = true;


					}
				}
				if (false == bResult)
				{
					return S_FALSE;
				}
				if (pDocumentAccessor) {
					pDocumentAccessor->Close();
					delete pDocumentAccessor;
				}
				
				//PstoImage(nLandscapeType);

				PPtPsToPdf();
				PPtPdfToImage();
			}
			else if (m_sourceFile.format == PDF) {
				PdfToImage();
			}
			else if (m_sourceFile.format == HWP) {
				CDocumentAccessor* pDocumentAccessor = new CHwpAccessor;
				if (NULL == pDocumentAccessor)
					return S_FALSE;

				int nLandscapeType = 0;
				bool bResult = false;
				if (true == pDocumentAccessor->Initialize())
				{
					if (true == pDocumentAccessor->Open(m_sourceFile.strFilePath.c_str()))
					{
						nLandscapeType = pDocumentAccessor->Print(m_strPrinter.c_str(), m_strPostScriptFile.c_str());

						if (nLandscapeType>-1)
							bResult = true;
					}
				}
				if (false == bResult)
				{
					return S_FALSE;
				}
				if (pDocumentAccessor) {
					pDocumentAccessor->Close();
					delete pDocumentAccessor;
				}
				PstoImage(nLandscapeType);
			}


			if (m_pUiHandle) {
				m_pUiHandle->PostMessage(MESSAGE_THREAD_UPDATE, WM_UPDATEUISTATE, 600);
			}

			ResizeImages();


		}

	}

	
	if (m_pUiHandle) {
		m_pUiHandle->PostMessage(MESSAGE_THREAD_UPDATE, WM_UPDATEUISTATE, 1000);
		m_pUiHandle->PostMessage(MESSAGE_THREAD_UPDATE, WM_QUIT, 0);
	}


	return hr;
}



HRESULT CDocumentConvert::PPtPsToPdf() {
	HRESULT hr = S_OK;



	std::wstring strCmd = L" -sDEVICE=pdfwrite -sOutputFile=\"";
	strCmd.append(m_strPostScriptFile);
	strCmd.append(L"\" -dNOPAUSE -dEPSCrop -f \"");
	strCmd.append(m_strPostScriptFilePpt);
	strCmd.append(L"\" -c quit");

	std::wstring strExcutePath = m_strExecutePath;
	strExcutePath.append(L"gs\\bin");

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = _T("gswin32c.exe");
	ShExecInfo.lpParameters = strCmd.c_str();
	ShExecInfo.lpDirectory = strExcutePath.c_str();
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	::DeleteFileW(m_strPostScriptFilePpt.c_str());

	return hr;
}



HRESULT CDocumentConvert::PstoImage(int nLandscapeType) {
	HRESULT hr = S_OK;



	std::wstring strDestFile = m_strXxhdpiDir;
	strDestFile.append(L"%03d.jpg");

	std::wstring strCmd = L" -sDEVICE=jpeg";
	strCmd.append(L" -dJPEGQ=100 -r400 -dNOPAUSE -dBATCH -dSAFER -sOutputFile=\"");
	strCmd.append(strDestFile);

	// 가로 출력
	if (nLandscapeType == 2) 
	{
		strCmd.append(L" -dAutoRotatePages=/All -c \"<</Orientation 3>> setpagedevice\" \"");
	}
	else {
		strCmd.append(L"\" -dAutoRotatePages=/All \"");
	}
	strCmd.append(m_strPostScriptFile);
	strCmd.append(L"\"");
	

	
	//TRACE(_T(">> ok plan(%d) (%s) <<\n"), nLandscapeType, strCmd.c_str());
	std::wstring strExcutePath = m_strExecutePath;
	strExcutePath.append(L"gs\\bin");

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = _T("gswin32c.exe");
	ShExecInfo.lpParameters = strCmd.c_str();
	ShExecInfo.lpDirectory = strExcutePath.c_str();
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	::DeleteFileW(m_strPostScriptFile.c_str());

	return hr;
}

HRESULT CDocumentConvert::PdfToImage() {
	HRESULT hr = S_OK;

	std::wstring strDestFile = m_strXxhdpiDir;
	strDestFile.append(L"%03d.jpg");


	// -sDEVICE=jpeg -dJPEGQ=100 -r400 -dNOPAUSE -dBATCH -dSAFER -sOutputFile="output%03d.jpg" -dAutoRotatePages=/All "1.pdf"


	std::wstring strCmd = L" -sDEVICE=jpeg";
	strCmd.append(L" -dJPEGQ=100 -r400 -dNOPAUSE -dBATCH -dSAFER -sOutputFile=\"");
	strCmd.append(strDestFile);
	strCmd.append(L"\" -dAutoRotatePages=/All \"");
	strCmd.append(m_sourceFile.strFilePath);
	strCmd.append(L"\"");
	//std::wstring strCmd = L" -q -c \"(";
	//strCmd.append(m_sourceFile.strFilePath);
	//strCmd.append(L") (r) file runpdfbegin pdfpagecount =\" -f ");
	//strCmd.append(m_sourceFile.strFilePath);
	//strCmd.append(L"\"");

	std::wstring strExcutePath = m_strExecutePath;
	strExcutePath.append(L"gs\\bin");

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = _T("gswin32c.exe");
	ShExecInfo.lpParameters = strCmd.c_str();
	ShExecInfo.lpDirectory = strExcutePath.c_str();
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	return hr;
}


HRESULT CDocumentConvert::PPtPdfToImage() {
	HRESULT hr = S_OK;

	std::wstring strDestFile = m_strXxhdpiDir;
	strDestFile.append(L"%03d.jpg");


	// -sDEVICE=jpeg -dJPEGQ=100 -r400 -dNOPAUSE -dBATCH -dSAFER -sOutputFile="output%03d.jpg" -dAutoRotatePages=/All "1.pdf"


	std::wstring strCmd = L" -sDEVICE=jpeg";
	strCmd.append(L" -dJPEGQ=100 -r400 -dNOPAUSE -dBATCH -dSAFER -sOutputFile=\"");
	strCmd.append(strDestFile);
	strCmd.append(L"\" -dAutoRotatePages=/All \"");
	strCmd.append(m_strPostScriptFile);
	strCmd.append(L"\"");

	//TRACE(L"\n>>>>> PptPdfToImage <<<<<\n");

	//std::wstring strCmd = L" -q -c \"(";
	//strCmd.append(m_sourceFile.strFilePath);
	//strCmd.append(L") (r) file runpdfbegin pdfpagecount =\" -f ");
	//strCmd.append(m_sourceFile.strFilePath);
	//strCmd.append(L"\"");

	std::wstring strExcutePath = m_strExecutePath;
	strExcutePath.append(L"gs\\bin");

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = _T("gswin32c.exe");
	ShExecInfo.lpParameters = strCmd.c_str();
	ShExecInfo.lpDirectory = strExcutePath.c_str();
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	::DeleteFileW(m_strPostScriptFile.c_str());
	return hr;
}



void CDocumentConvert::SetBookDir(std::wstring bookDir) {
	m_strBookDir = bookDir;
	m_strBookDir.append(L"\\");
	m_strMediaDir = m_strBookDir;
	m_strEtcDir = m_strBookDir;
	m_strXxhdpiDir = m_strBookDir;
	m_strXhdpiDir = m_strBookDir;
	m_strMediaDir.append(L"media\\");
	m_strEtcDir.append(L"etc\\");
	m_strXxhdpiDir.append(L"xxhdpi\\");
	m_strXhdpiDir.append(L"xhdpi\\");
	m_strPostScriptFile = m_strXxhdpiDir;
	m_strPostScriptFile.append(L"tmp.ps");
	m_strPostScriptFilePpt = m_strXxhdpiDir;
	m_strPostScriptFilePpt.append(L"tmp.ps.raw");

}


void CDocumentConvert::SetSourceFile(ST_FILEDATA sourceFile) {
	m_sourceFile = sourceFile;
}


void CDocumentConvert::SetUiHandle(CWnd* ui) {
	m_pUiHandle = ui;
}



HRESULT CDocumentConvert::Make_Directory() {
	tmkdir(m_strMediaDir.c_str());
	tmkdir(m_strEtcDir.c_str());
	tmkdir(m_strXxhdpiDir.c_str());
	tmkdir(m_strXhdpiDir.c_str());

	return S_OK;
}


void CDocumentConvert::tmkdir(const wchar_t *full_path)
{
	wchar_t temp[MAX_PATH], *sp;
	lstrcpy(temp, full_path);
	sp = temp;

	while ((sp = _tcschr(sp, _T('\\')))) {
		if (sp > temp && *(sp - 1) != ':') {
			*sp = _T('\0');
			WIN32_FIND_DATA wfd;
			memset(&wfd, 0, sizeof(wfd));
			HANDLE hFirstFile = FindFirstFile(temp, (LPWIN32_FIND_DATA)&wfd);
			if (hFirstFile == INVALID_HANDLE_VALUE) {
				_tmkdir(temp);
			}
			else {
				FindClose(hFirstFile);
			}
			*sp = _T('\\');
		}
		sp++;
	}

}



void CDocumentConvert::GetProgramPath()
{
	TCHAR szFullpath[MAX_PATH] = { 0 }, szDrive[256] = { 0 }, szDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szFullpath, MAX_PATH);
	_tsplitpath(szFullpath, szDrive, szDir, NULL, NULL);
	m_strExecutePath = szDrive;
	m_strExecutePath.append(szDir);

#if _DEBUG
	m_strExecutePath = L"..\\nsis\\";
#endif
}



void CDocumentConvert::ResizeImages() {
	
	std::experimental::filesystem::path xxhdpiDir(m_strXxhdpiDir);
	std::experimental::filesystem::path xhdpiDir(m_strXhdpiDir);

	bool bCover = true;

	for (auto & p : std::experimental::filesystem::directory_iterator(xxhdpiDir)) {
		if (!std::experimental::filesystem::is_directory(p.status()))
		{
			std::wstring dest = m_strXhdpiDir;
			dest.append(p.path().filename());
			ResizeImageFile(p.path(), dest, 1240);

			if (bCover) {
				bCover = false;
				dest = m_strEtcDir;
				dest.append(L"cover.jpg");
				ResizeImageFile(p.path(), dest, 512);
			}
		}
	}


	for (auto & xp : std::experimental::filesystem::directory_iterator(xhdpiDir)) {
		if (!std::experimental::filesystem::is_directory(xp.status()))
		{
			std::wstring dest = m_strBookDir;
			dest.append(xp.path().filename());
			ResizeImageFile(xp.path(), dest, 256);
		}
	}
}



int CDocumentConvert::ResizeImageFile(std::wstring source, std::wstring thumbnail, int width)
{
	struct jpeg_decompress_struct cinfo_decompress;
	FILE* infile;
	int row_stride;
	jpeg_error_mgr jerr;
	
	if (_wfopen_s(&infile, source.c_str(), L"rb") != 0) {
		return -1;
	}

	cinfo_decompress.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo_decompress);
	jpeg_stdio_src(&cinfo_decompress, infile);


	int ret = jpeg_read_header(&cinfo_decompress, TRUE);
	float scale = cinfo_decompress.image_width / width;

	cinfo_decompress.scale_num = 1;
	cinfo_decompress.scale_denom = std::min<int>(8, static_cast<int>(scale));

	if (ret != JPEG_HEADER_OK) return -1;
	jpeg_start_decompress(&cinfo_decompress);
	row_stride = cinfo_decompress.output_width * cinfo_decompress.output_components;
	int buffer_height = 1;
	JSAMPARRAY buffer = (JSAMPARRAY)malloc(sizeof(JSAMPROW) * buffer_height);
	buffer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * row_stride);

	ImageData* imageData;
	imageData = new ImageData;
	imageData->width = cinfo_decompress.output_width;
	imageData->height = cinfo_decompress.output_height;

	imageData->pixels = new unsigned char[cinfo_decompress.output_width * cinfo_decompress.output_height * cinfo_decompress.output_components];
	long counter = 0;

	while (cinfo_decompress.output_scanline < cinfo_decompress.output_height) {
		jpeg_read_scanlines(&cinfo_decompress, buffer, 1);
		memcpy(imageData->pixels + counter, buffer[0], row_stride);
		counter += row_stride;

	}

	jpeg_finish_decompress(&cinfo_decompress);
	jpeg_destroy_decompress(&cinfo_decompress);

	fclose(infile);

	//write  
	unsigned char* image_buffer;
	int image_height = cinfo_decompress.output_height;
	int image_width = cinfo_decompress.output_width;
	FILE * outfile;
	JSAMPROW row_pointer[1];
	int row_stride_dst;
	struct jpeg_compress_struct cinfo_compress;
	cinfo_compress.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo_compress);

	if (_wfopen_s(&outfile, thumbnail.c_str(), L"wb") != 0) {
		return -1;

	}

	jpeg_stdio_dest(&cinfo_compress, outfile);

	cinfo_compress.image_width = image_width;
	cinfo_compress.image_height = image_height;
	cinfo_compress.input_components = 3;
	cinfo_compress.in_color_space = JCS_RGB;

	int quality = 100;
	jpeg_set_defaults(&cinfo_compress);
	jpeg_set_quality(&cinfo_compress, quality, TRUE);
	jpeg_start_compress(&cinfo_compress, TRUE);

	row_stride_dst = image_width * 3;

	image_buffer = new unsigned char[row_stride_dst * cinfo_compress.image_height];
	memcpy(image_buffer, imageData->pixels, row_stride_dst * cinfo_compress.image_height);

	while (cinfo_compress.next_scanline < cinfo_compress.image_height) {
		row_pointer[0] = &image_buffer[cinfo_compress.next_scanline * row_stride_dst];
		jpeg_write_scanlines(&cinfo_compress, row_pointer, 1);

	}

	jpeg_finish_compress(&cinfo_compress);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo_compress);

	if (imageData) {
		delete imageData;
		imageData = NULL;

	}

	if (image_buffer)
		delete[] image_buffer;

	return 0;
}

