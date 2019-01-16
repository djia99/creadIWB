#pragma once


#define SKIN_VER_122	001002002

class CCreadSkin
{
public:
	CCreadSkin();
	~CCreadSkin();


	std::wstring m_dir;
	void SetDir(std::wstring dir);

	std::wstring GetSource(std::wstring strFile);


	// 화면 해상도
	void	GetScreenSize();
	float	GetScreenRatio() { return m_ratio; }
	int		m_screenWidth;
	int		m_screenHeight;
	float	m_ratio;
	


	void Init();



	// major version : 000
	// minor version : 000
	// sub version : 000
	// version = [major][minor][sub]
	DWORD m_version;
	std::wstring GetVersion() { return L"Software Version : 0.9.5"; }

	// main menu
	CRect m_mainMenu;
	void SetMainMenu(const CRect& rect) { m_mainMenu = rect; }

	CRect m_mainMenuBtn;
	void SetMainMenuBtn(const CRect& rect) { m_mainMenuBtn = rect; }

	CRect m_mainThumbnailSize;
	void SetMainThumbnail(const CRect& rect) { m_mainThumbnailSize = rect; }


	// main menu up
	ULONG m_mainUpAlign;
	int m_mainUpHeight;
	CRect m_mainUpBookList;
	void SetMainUpBookList(const CRect& rect) { m_mainUpBookList = rect; }
	CRect m_mainUpVideoList;
	void SetMainUpVideoList(const CRect& rect) { m_mainUpVideoList = rect; }
	CRect m_mainUpLogo;
	void SetMainUpLogo(const CRect& rect) { m_mainUpLogo = rect; }
	ULONG m_logoAlign;
	CRect m_mainUpExit;
	void SetMainUpExit(const CRect& rect) { m_mainUpExit = rect; }


	CRect m_boardMenu;
	void SetBoardMenu(const CRect& rect) { m_boardMenu = rect; }

	CRect m_bookMark;
	void SetBookMark(const CRect& rect) { m_bookMark = rect; }

	CRect m_bookMarkArrow;
	void SetBookMarkArrow(const CRect& rect) { m_bookMarkArrow = rect; }


	// dpi
	float GetDpiRatio();

	// font size
	int m_fontSize;
	int GetFontSize() { return m_fontSize; /*14*/}

	// tool
	CRect m_toolRect;
	void SetToolRect(const CRect& rect) { m_toolRect = rect; }
	CRect m_toolBtn;
	void SetToolBtn(const CRect& rect) { m_toolBtn = rect; }
	CRect m_toolBtnColor;
	void SetToolBtnColor(const CRect& rect) { m_toolBtnColor = rect; }
	CRect m_toolBtnClose;
	void SetToolBtnClose(const CRect& rect) { m_toolBtnClose = rect; }
	int m_toolBtnPadding;
	int m_toolBtnOffsetY;

	std::wstring m_imgToolMenuBg;
	std::wstring GetImgToolMenuBg() { return GetSource(m_imgToolMenuBg); }


	// ink
	CRect m_inkRect;
	void SetInkRect(const CRect& rect) { m_inkRect = rect; }
	CRect m_inkRectSmall;
	void SetInkRectSmall(const CRect& rect) { m_inkRectSmall = rect; }
	CRect m_inkColor;
	void SetInkColor(const CRect& rect) { m_inkColor = rect; }
	CRect m_inkGone;
	void SetInkGone(const CRect& rect) { m_inkGone = rect; }


	void UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, BYTE alpha = 255);
	void UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, int width, int height);
	void UpdateLayeredWindowPng(HWND pParent, HWND hWnd, Gdiplus::Image *pImage, COLORREF clr, BOOL bInk, BOOL bPen, BYTE alpha = 255);

};

