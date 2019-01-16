#pragma once

#define MESSAGE_THREAD_UPDATE (WM_USER + 1001)

struct ImageData {
	unsigned char *pixels;
	long  width;
	long height;
};

enum FILE_FORMAT { DOC, DOCX, XLS, XLSX, PPT, PPTX, PDF, JPG, HWP, ETC };
struct ST_FILEDATA {
	std::wstring strFileName;
	std::wstring strFileType;
	std::wstring strFilePath;
	std::wstring strFileDisplayName;

	int result;
	FILE_FORMAT format;
	static std::wstring GetFilename(const std::wstring& path) {
		return path.substr(path.find_last_of('\\') + 1);
	}
	static std::wstring GetExtension(const std::wstring& path) {
		return path.substr(path.find_last_of('.') + 1);
	}
	static std::wstring GetDisplayName(const std::wstring& path) {
		return path.substr(0, path.find_last_of('.'));
	}


	static FILE_FORMAT GetFormat(const std::wstring& type) {
		FILE_FORMAT file_format;
		std::wstring strLowerType = type;
		std::transform(strLowerType.begin(), strLowerType.end(), strLowerType.begin(), tolower);

		if (strLowerType.compare(L"doc") == 0 || strLowerType.compare(L"docx") == 0) {
			file_format = DOC;
		}
		else	if (type.compare(L"xls") == 0 || strLowerType.compare(L"xlsx") == 0) {
			file_format = XLS;
		}
		else	if (strLowerType.compare(L"ppt") == 0 || strLowerType.compare(L"pptx") == 0) {
			file_format = PPT;
		}
		else	if (strLowerType.compare(L"pdf") == 0) {
			file_format = PDF;
		}
		else	if (strLowerType.compare(L"jpg") == 0) {
			file_format = JPG;
		}
		else	if (strLowerType.compare(L"jpeg") == 0) {
			file_format = JPG;
		}
		else	if (strLowerType.compare(L"hwp") == 0) {
			file_format = HWP;
		}
		else	if (strLowerType.compare(L"rtf") == 0) {
			file_format = HWP;
		}
		else {
			file_format = ETC;
		}


		return file_format;
	}

};


class CriticalSectionLocker
{
	CRITICAL_SECTION* m_criticalSection;
public:
	CriticalSectionLocker(CRITICAL_SECTION* cs)
	{
		m_criticalSection = cs;
		if (nullptr != m_criticalSection)
		{
			EnterCriticalSection(m_criticalSection);
		}
	}
	~CriticalSectionLocker()
	{
		if (nullptr != m_criticalSection)
		{
			LeaveCriticalSection(m_criticalSection);
			m_criticalSection = nullptr;
		}
	}
};


[uuid("46365483-8197-4f76-2944-46DBA5FBFB79")]
__interface IDocumentConvert : public IUnknown
{
public:
	void	__stdcall SetUiHandle(CWnd* ui);
	HRESULT __stdcall Start();
	HRESULT __stdcall Stop();
	HRESULT __stdcall IsWorkingThreadActive(bool* active);
	HRESULT __stdcall GetCriticalSection(CRITICAL_SECTION** cs);
	HRESULT __stdcall Shutdown();


	void __stdcall SetBookDir(std::wstring bookDir);
	void __stdcall SetSourceFile(ST_FILEDATA sourceFile);

	HRESULT __stdcall Make_Directory();
};


class CDocumentConvert : public IDocumentConvert, public IInitializable
{
public:
	CDocumentConvert();
	virtual ~CDocumentConvert() {
		DeleteCriticalSection(&m_cs);
	}

	bool QueryInterfaceHelper(const IID &iid, void **object)
	{
		return CastHelper<IDocumentConvert>::CastTo(iid, this, object) || CastHelper<IInitializable>::CastTo(iid, this, object);
	}
	HRESULT __stdcall Initialize();
	HRESULT __stdcall Shutdown();

	CRITICAL_SECTION		m_cs;
	CWnd*					m_pUiHandle;
	HANDLE					m_thdHandle;
	unsigned long			m_thdId;
	bool					m_bWorking;


	void	__stdcall SetUiHandle(CWnd* ui);
	HRESULT __stdcall Start();
	HRESULT __stdcall Stop();
	HRESULT __stdcall Process();
	HRESULT __stdcall PstoImage(int nLandscapeType);
	HRESULT __stdcall PdfToImage();
	HRESULT __stdcall PPtPsToPdf();
	HRESULT __stdcall PPtPdfToImage();


	static unsigned long WINAPI ConvertThd(void* pCon);
	HRESULT __stdcall GetCriticalSection(CRITICAL_SECTION** cs);
	HRESULT __stdcall IsWorkingThreadActive(bool* active);

	void __stdcall SetBookDir(std::wstring bookDir);
	void __stdcall SetSourceFile(ST_FILEDATA sourceFile);


	

	// 기본 폴더 생성
	HRESULT __stdcall Make_Directory();
	static void tmkdir(const wchar_t *full_path);
	void GetProgramPath();					// 프로그램 실행 path
	void ResizeImages();
	int ResizeImageFile(std::wstring source, std::wstring thumbnail, int width);

protected:

	std::wstring m_strExecutePath;		// 프로그램 실행 path
	std::wstring m_strPrinter;			// printer 드라이버 명
	std::wstring m_strPostScriptFile;	// 프린터용 파일
	std::wstring m_strPostScriptFilePpt;// PPT전용 프린터용 파일
	std::wstring m_strBookDir;			// 교재 폴더 명
	std::wstring m_strMediaDir;			// 교재 폴더 \ media
	std::wstring m_strEtcDir;			// 교재 폴더 \ etc
	std::wstring m_strXxhdpiDir;		// 교재 폴더 \ xxhdpi
	std::wstring m_strXhdpiDir;			// 교재 폴더 \ xhdpi

	ST_FILEDATA m_sourceFile;

};

