//
// GdipButton.cpp : Version 1.0 - see article at CodeProject.com
//
// Author:  Darren Sessions
//          
//
// Description:
//     GdipButton is a CButton derived control that uses GDI+ 
//     to support alternate image formats
//
// History
//     Version 1.0 - 2008 June 10
//     - Initial public release
//
// License:
//     This software is released under the Code Project Open License (CPOL),
//     which may be found here:  http://www.codeproject.com/info/eula.aspx
//     You are free to use this software in any way you like, except that you 
//     may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CCreadButton.h"

#include "CGdiPlusBitmap.h"
//#include "MemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCreadButton

CCreadButton::CCreadButton() :
	m_imageWidth(0),
	m_imageHeight(0),
	m_pStdImage(NULL),
	m_pAltImage(NULL),
	m_bIsSelected(FALSE),
	m_bColorButton(FALSE),
	m_bColorButtonEnable(TRUE),
	m_color(RGB(255,255,255))
{


	m_bHaveBitmaps = FALSE;
	m_bHaveAltImage = FALSE;

	m_pCurBtn = NULL;

	m_bIsDisabled = FALSE;
	m_bIsToggle = FALSE;

	m_bIsHovering = FALSE;
	m_bIsTracking = FALSE;

	m_nCurType = STD_TYPE;

	m_pToolTip = NULL;
	m_wndRect = CRect(0, 0, 0, 0);
}

CCreadButton::~CCreadButton()
{
	if (m_pStdImage) {
		delete m_pStdImage;
	}
	if (m_pAltImage) {
		delete m_pAltImage;
	}

	if (m_pToolTip)	delete m_pToolTip;
}


BEGIN_MESSAGE_MAP(CCreadButton, CButton)
	//{{AFX_MSG_MAP(CCreadButton)
	ON_WM_DRAWITEM()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()



//=============================================================================
//
// LoadStdImage()
//
// Purpose:     The LoadStdImage() Loads the image for the button.  This 
//				function must be called at a minimum or the button wont do 
//				anything.
//
// Parameters:  
//		[IN]	id
//				resource id, one of the resources already imported with the 
//				resource editor, usually begins with IDR_  
//
//		[IN]	pType
//				pointer to string describing the resource type
//				
// Returns:     BOOL
//				Non zero if successful, otherwise zero
//
//=============================================================================
BOOL CCreadButton::LoadStdImage(UINT id, LPCTSTR pType)
{
	if (m_pStdImage) {
		delete m_pStdImage;
		m_pStdImage = NULL;
	}
	m_pStdImage = new CGdiPlusBitmapResource;
	BOOL bRet = m_pStdImage->Load(id, pType);


	if (bRet) {
		m_imageWidth = m_pStdImage->m_pBitmap->GetWidth();
		m_imageHeight = m_pStdImage->m_pBitmap->GetHeight();
	}
	return bRet;
}


BOOL CCreadButton::LoadStdImage(LPCWSTR strFile)
{
	if (m_pStdImage) {
		delete m_pStdImage;
		m_pStdImage = NULL;
	}
	m_pStdImage = new CGdiPlusBitmapResource;

	BOOL bRet = m_pStdImage->LoadFile(strFile);
	if (bRet) {
		m_imageWidth = m_pStdImage->m_pBitmap->GetWidth();
		m_imageHeight = m_pStdImage->m_pBitmap->GetHeight();
	}

	return bRet;
}

//=============================================================================
//
// LoadAltImage()
//
// Purpose:     The LoadAltImage() Loads the altername image for the button.  
//				This function call is optional
// Parameters:  
//		[IN]	id
//				resource id, one of the resources already imported with the 
//				resource editor, usually begins with IDR_  
//
//		[IN]	pType
//				pointer to string describing the resource type
//				
// Returns:     BOOL
//				Non zero if successful, otherwise zero
//
//=============================================================================
BOOL CCreadButton::LoadAltImage(UINT id, LPCTSTR pType)
{
	if (m_pAltImage) {
		delete m_pAltImage;
		m_pAltImage = NULL;
	}
	m_bHaveAltImage = TRUE;
	m_pAltImage = new CGdiPlusBitmapResource;
	return (m_pAltImage->Load(id, pType));
}
BOOL CCreadButton::LoadAltImage(LPCWSTR strFile)
{
	if (m_pAltImage) {
		delete m_pAltImage;
		m_pAltImage = NULL;
	}

	m_bHaveAltImage = TRUE;
	m_pAltImage = new CGdiPlusBitmapResource;
	return (m_pAltImage->LoadFile(strFile));
}


//=============================================================================
//
//	The framework calls this member function when a child control is about to 
//	be drawn.  All the bitmaps are created here on the first call. Every thing
//	is done with a memory DC except the background, which get's it's information 
//	from the parent. The background is needed for transparent portions of PNG 
//	images. An always on top app (such as Task Manager) that is in the way can 
//	cause it to get an incorrect background.  To avoid this, the parent should 
//	call the SetBkGnd function with a memory DC when it creates the background.
//				
//=============================================================================
HBRUSH CCreadButton::CtlColor(CDC* pScreenDC, UINT nCtlColor)
{
	if (!m_bHaveBitmaps)
	{
		if (!m_pStdImage)
		{
			return NULL; // Load the standard image with LoadStdImage()
		}

		CBitmap bmp, *pOldBitmap;

		CRect rect;
		GetClientRect(rect);



		Gdiplus::RectF stdRect; stdRect.X = 0, stdRect.Y = 0; stdRect.Width = rect.Width(); stdRect.Height = rect.Height();

		float ImageWidth = (float)m_pStdImage->m_pBitmap->GetWidth();
		float ImageHeight = (float)m_pStdImage->m_pBitmap->GetHeight();


		// do everything with mem dc
		CMyMemDC pDC(pScreenDC, rect);

		Gdiplus::Graphics graphics(pDC->m_hDC);

		// background
		if (m_dcBk.m_hDC == NULL)
		{

			CRect rect1;
			CClientDC clDC(GetParent());
			GetWindowRect(rect1);
			GetParent()->ScreenToClient(rect1);

			m_dcBk.CreateCompatibleDC(&clDC);
			bmp.CreateCompatibleBitmap(&clDC, rect.Width(), rect.Height());
			pOldBitmap = m_dcBk.SelectObject(&bmp);
			m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), &clDC, rect1.left, rect1.top, SRCCOPY);
			bmp.DeleteObject();
		}

		// standard image
		if (m_dcStd.m_hDC == NULL)
		{
			PaintBk(pDC);

			//graphics.DrawImage(*m_pStdImage, 0, 0);
			graphics.DrawImage(*m_pStdImage, stdRect, 0, 0, ImageWidth, ImageHeight, Gdiplus::UnitPixel);

			m_dcStd.CreateCompatibleDC(pDC);
			bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
			pOldBitmap = m_dcStd.SelectObject(&bmp);
			m_dcStd.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
			bmp.DeleteObject();

			// standard image pressed
			if (m_dcStdP.m_hDC == NULL)
			{
				PaintBk(pDC);

				graphics.DrawImage(*m_pStdImage, stdRect, 1, 1, ImageWidth, ImageHeight, Gdiplus::UnitPixel);

				m_dcStdP.CreateCompatibleDC(pDC);
				bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
				pOldBitmap = m_dcStdP.SelectObject(&bmp);
				m_dcStdP.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}

			// standard image hot
			if (m_dcStdH.m_hDC == NULL)
			{
				PaintBk(pDC);

				Gdiplus::ColorMatrix HotMat = { 1.05f, 0.00f, 0.00f, 0.00f, 0.00f,
					0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
					0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
					0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
					0.05f, 0.05f, 0.05f, 0.00f, 1.00f };

				Gdiplus::ImageAttributes ia;
				ia.SetColorMatrix(&HotMat);

				float width = (float)m_pStdImage->m_pBitmap->GetWidth();
				float height = (float)m_pStdImage->m_pBitmap->GetHeight();
				Gdiplus::RectF grect; grect.X = 0, grect.Y = 0; grect.Width = rect.Width(); grect.Height = rect.Height();
				graphics.DrawImage(*m_pStdImage, grect, 0, 0, ImageWidth, ImageHeight, Gdiplus::UnitPixel, &ia);

				//graphics.DrawImage(*m_pStdImage, stdRect, 0, 0, ImageWidth, ImageHeight, Gdiplus::UnitPixel, &ia);

				m_dcStdH.CreateCompatibleDC(pDC);
				bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
				pOldBitmap = m_dcStdH.SelectObject(&bmp);
				m_dcStdH.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}

			// grayscale image
			if (m_dcGS.m_hDC == NULL)
			{
				PaintBk(pDC);

				Gdiplus::ColorMatrix GrayMat = { 0.30f, 0.30f, 0.30f, 0.00f, 0.00f,
					0.59f, 0.59f, 0.59f, 0.00f, 0.00f,
					0.11f, 0.11f, 0.11f, 0.00f, 0.00f,
					0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
					0.00f, 0.00f, 0.00f, 0.00f, 1.00f };

				Gdiplus::ImageAttributes ia;
				ia.SetColorMatrix(&GrayMat);

				float width = (float)m_pStdImage->m_pBitmap->GetWidth();
				float height = (float)m_pStdImage->m_pBitmap->GetHeight();
				Gdiplus::RectF grect; grect.X = 0, grect.Y = 0; grect.Width = width; grect.Height = height;
				graphics.DrawImage(*m_pStdImage, grect, 0, 0, ImageWidth, ImageHeight, Gdiplus::UnitPixel, &ia);

				//graphics.DrawImage(*m_pStdImage, stdRect, 0, 0, ImageWidth, ImageHeight, Gdiplus::UnitPixel, &ia);

				m_dcGS.CreateCompatibleDC(pDC);
				bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
				pOldBitmap = m_dcGS.SelectObject(&bmp);
				m_dcGS.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}
		}

		// alternate image
		if ((m_dcAlt.m_hDC == NULL) && m_bHaveAltImage)
		{
			PaintBk(pDC);

			//graphics.DrawImage(*m_pAltImage, 0, 0);
			graphics.DrawImage(*m_pAltImage, stdRect, 0, 0, ImageWidth, ImageHeight, Gdiplus::UnitPixel);

			m_dcAlt.CreateCompatibleDC(pDC);
			bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
			pOldBitmap = m_dcAlt.SelectObject(&bmp);
			m_dcAlt.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
			bmp.DeleteObject();

			// alternate image pressed
			if ((m_dcAltP.m_hDC == NULL) && m_bHaveAltImage)
			{
				PaintBk(pDC);

				//graphics.DrawImage(*m_pAltImage, 1, 1);
				graphics.DrawImage(*m_pAltImage, stdRect, 1, 1, ImageWidth, ImageHeight, Gdiplus::UnitPixel);

				m_dcAltP.CreateCompatibleDC(pDC);
				bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
				pOldBitmap = m_dcAltP.SelectObject(&bmp);
				m_dcAltP.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}

			// alternate image hot
			if (m_dcAltH.m_hDC == NULL)
			{
				PaintBk(pDC);

				Gdiplus::ColorMatrix HotMat = { 1.05f, 0.00f, 0.00f, 0.00f, 0.00f,
					0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
					0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
					0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
					0.05f, 0.05f, 0.05f, 0.00f, 1.00f };

				Gdiplus::ImageAttributes ia;
				ia.SetColorMatrix(&HotMat);

				float width = (float)m_pStdImage->m_pBitmap->GetWidth();
				float height = (float)m_pStdImage->m_pBitmap->GetHeight();
				Gdiplus::RectF grect; grect.X = 0, grect.Y = 0; grect.Width = width; grect.Height = height;
				graphics.DrawImage(*m_pAltImage, grect, 0, 0, ImageWidth, ImageHeight, Gdiplus::UnitPixel, &ia);

				//graphics.DrawImage(*m_pAltImage, stdRect, 0, 0, ImageWidth, ImageHeight, Gdiplus::UnitPixel, &ia);

				m_dcAltH.CreateCompatibleDC(pDC);
				bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
				pOldBitmap = m_dcAltH.SelectObject(&bmp);
				m_dcAltH.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
				bmp.DeleteObject();
			}
		}

		if (m_pCurBtn == NULL)
		{
			m_pCurBtn = &m_dcStd;
		}

		m_bHaveBitmaps = TRUE;
	}

	return NULL;
}

//=============================================================================
// paint the background
//=============================================================================
void CCreadButton::PaintBk(CDC *pDC)
{
	CRect rect;
	GetClientRect(rect);
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_dcBk, 0, 0, SRCCOPY);
}

//=============================================================================
// paint the bitmap currently pointed to with m_pCurBtn
//=============================================================================
void CCreadButton::PaintBtn(CDC *pDC)
{
	CRect rect;
	GetClientRect(rect);

	if (m_pCurBtn != NULL) {
		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), m_pCurBtn, 0, 0, SRCCOPY);
	}
	else {
	
	}
}

//=============================================================================
// enables the toggle mode
// returns if it doesn't have the alternate image
//=============================================================================
void CCreadButton::EnableToggle(BOOL bEnable)
{
	if (!m_bHaveAltImage) return;

	m_bIsToggle = bEnable;

	// this actually makes it start in the std state since toggle is called before paint
	if (bEnable)	m_pCurBtn = &m_dcAlt;
	else		m_pCurBtn = &m_dcStd;

}

//=============================================================================
// sets the image type and disabled state then repaints
//=============================================================================
void CCreadButton::SetImage(int type)
{
	m_nCurType = type;

	(type == DIS_TYPE) ? m_bIsDisabled = TRUE : m_bIsDisabled = FALSE;

	Invalidate();
}

//=============================================================================
// set the control to owner draw
//=============================================================================
void CCreadButton::PreSubclassWindow()
{
	// Set control to owner draw
	ModifyStyle(0, BS_OWNERDRAW, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
}

//=============================================================================
// disable double click 
//=============================================================================
BOOL CCreadButton::PreTranslateMessage(MSG* pMsg)
{

	if (m_pToolTip != NULL)
	{
		if (::IsWindow(m_pToolTip->m_hWnd))
		{
			m_pToolTip->RelayEvent(pMsg);
		}
	}


	//if (pMsg->message == WM_KEYDOWN)
	//{
	//	const BOOL repeat = pMsg->lParam & (1 << 30);
	//	if (!repeat)
	//		GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
	//}

	return CButton::PreTranslateMessage(pMsg);
}


//=============================================================================
// overide the erase function
//=============================================================================
BOOL CCreadButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}



//=============================================================================
// Paint the button depending on the state of the mouse
//=============================================================================
void CCreadButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	if (m_bColorButton) {
		CRect rect;
		GetClientRect(rect);

		CBrush brush(m_color);
		CPen penBlack;
		if (m_bColorButtonEnable) penBlack.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		else penBlack.CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
		CPen* pOldPen = pDC->SelectObject(&penBlack);
		CBrush* pOldBrush = pDC->SelectObject(&brush);

		pDC->Rectangle(rect);

		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		
	}

	// handle disabled state
	if (m_bIsDisabled)
	{
		m_pCurBtn = &m_dcGS;
		PaintBtn(pDC);
		return;
	}

	//BOOL bIsPressed = (lpDIS->itemState & ODS_SELECTED);
	BOOL bIsPressed = m_bIsSelected;

	// handle toggle button
	if (m_bIsToggle && bIsPressed)
	{
		//(m_nCurType == STD_TYPE) ? m_nCurType = ALT_TYPE : m_nCurType = STD_TYPE;
	}

	if (bIsPressed)
	{
		if (m_nCurType == STD_TYPE)
			m_pCurBtn = &m_dcStdP;
		else
			m_pCurBtn = &m_dcAltP;
	}
	else if (m_bIsHovering)
	{

		if (m_nCurType == STD_TYPE)
			m_pCurBtn = &m_dcStdH;
		else
			m_pCurBtn = &m_dcAltH;
	}
	else
	{
		if (m_nCurType == STD_TYPE)
			m_pCurBtn = &m_dcStd;
		else
			m_pCurBtn = &m_dcAlt;
	}
	//TRACE(_T("\n>>>>>>>> cread button (%d,%d,%d,%d) <<<<<<<<\n"), rect.left, rect.top, rect.right, rect.bottom);
	//TRACE(_T("\n>>>>>>>> cread button) <<<<<<<<\n"));

	// paint the button
	PaintBtn(pDC);
}

//=============================================================================
LRESULT CCreadButton::OnMouseHover(WPARAM wparam, LPARAM lparam)
//=============================================================================
{
	m_bIsHovering = TRUE;
	Invalidate();
	DeleteToolTip();

	// Create a new Tooltip with new Button Size and Location
	SetToolTipText(m_tooltext);

	if (m_pToolTip != NULL)
	{

		if (::IsWindow(m_pToolTip->m_hWnd))
		{
			m_pToolTip->SetDelayTime(200);
			CRect rt;
			GetClientRect(rt);
			m_pToolTip->SetToolRect(this, 1, rt);

			//Display ToolTip
			m_pToolTip->Update();
			
		}
	}

	return 0;
}


//=============================================================================
LRESULT CCreadButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
//=============================================================================
{
	m_bIsTracking = FALSE;
	m_bIsHovering = FALSE;
	Invalidate();
	return 0;
}

//=============================================================================
void CCreadButton::OnMouseMove(UINT nFlags, CPoint point)
//=============================================================================
{
	if (!m_bIsTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bIsTracking = _TrackMouseEvent(&tme);
	}

	CButton::OnMouseMove(nFlags, point);
}

//=============================================================================
//	
//	Call this member function with a memory DC from the code that paints 
//	the parents background.  Passing the screen DC defeats the purpose of 
//  using this function.
//
//=============================================================================
void CCreadButton::SetBkGnd(CDC* pDC)
{
	CRect rect, rectS;
	CBitmap bmp, *pOldBitmap;

	GetClientRect(rect);
	GetWindowRect(rectS);
	GetParent()->ScreenToClient(rectS);

	m_dcBk.DeleteDC();

	m_dcBk.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	pOldBitmap = m_dcBk.SelectObject(&bmp);
	m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rectS.left, rectS.top, SRCCOPY);
	bmp.DeleteObject();
}


//=============================================================================
// Set the tooltip with a string resource
//=============================================================================
void CCreadButton::SetToolTipText(UINT nId, BOOL bActivate)
{
	// load string resource
	m_tooltext.LoadString(nId);

	// If string resource is not empty
	if (m_tooltext.IsEmpty() == FALSE)
	{
		SetToolTipText(m_tooltext, bActivate);
	}

}

//=============================================================================
// Set the tooltip with a CString
//=============================================================================
void CCreadButton::SetToolTipText(CString spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText.IsEmpty()) return;


	// Initialize ToolTip
	InitToolTip();
	m_tooltext = spText;

	// If there is no tooltip defined then add it
	if (m_pToolTip->GetToolCount() == 0)
	{
		CRect rectBtn;
		rectBtn.left = 100;
		rectBtn.top = 100;
		rectBtn.right = 200;
		rectBtn.bottom = 200;

		//GetClientRect(rectBtn);
		m_pToolTip->AddTool(this, m_tooltext, rectBtn, 1);
	}

	// Set text for tooltip
	m_pToolTip->UpdateTipText(m_tooltext, this, 1);
	m_pToolTip->SetDelayTime(200);
	m_pToolTip->Activate(bActivate);
}

//=============================================================================
void CCreadButton::InitToolTip()
//=============================================================================
{
	if (m_pToolTip == NULL)
	{
		m_pToolTip = new CToolTipCtrl;
		// Create ToolTip control
		m_pToolTip->Create(this);
		m_pToolTip->Activate(TRUE);
	}
}

//=============================================================================
void CCreadButton::DeleteToolTip()
//=============================================================================
{
	// Destroy Tooltip incase the size of the button has changed.
	if (m_pToolTip != NULL)
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
}





void CCreadButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	//m_imageWidth = cx;
	//m_imageHeight = cy;
}


int CCreadButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//TRACE(L"\n>> >OnCreate  (%d) (%d) <<<\n", lpCreateStruct->cx, lpCreateStruct->cy);

	//// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	//m_imageWidth = lpCreateStruct->cx;
	//m_imageHeight = lpCreateStruct->cy;

	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	


	return 0;
}
