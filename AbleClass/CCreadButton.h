#ifndef CREADBUTTON_H
#define CREADBUTTON_H


#include "MyMemDC.h"

class CGdiPlusBitmapResource;

/////////////////////////////////////////////////////////////////////////////
// CCreadButton window

class CCreadButton : public CButton
{
public:

	CCreadButton();
	virtual ~CCreadButton();

	// image types
	enum {
		STD_TYPE = 0,
		ALT_TYPE,
		DIS_TYPE
	};



	void SetSelect(BOOL bSelect) {
		m_bIsSelected = bSelect;
		m_nCurType = (m_bIsSelected) ? ALT_TYPE : STD_TYPE;
		if (::IsWindow(m_hWnd))
			Invalidate();
	}

	// sets the image type
	void SetImage(int type);

	BOOL LoadAltImage(UINT id, LPCTSTR pType);
	BOOL LoadAltImage(LPCWSTR strFile);
	BOOL LoadStdImage(UINT id, LPCTSTR pType);
	BOOL LoadStdImage(LPCWSTR strFile);

	// if false, disables the press state and uses grayscale image if it exists
	void EnableButton(BOOL bEnable = TRUE) { m_bIsDisabled = !bEnable; }

	// in toggle mode each press toggles between std and alt images
	void EnableToggle(BOOL bEnable = TRUE);

	// return the enable/disable state
	BOOL IsDisabled(void) { return (m_bIsDisabled == TRUE); }

	void SetBkGnd(CDC* pDC);

	void SetToolTipText(CString spText, BOOL bActivate = TRUE);
	void SetToolTipText(UINT nId, BOOL bActivate = TRUE);
	//void SetHorizontal(bool ImagesAreLaidOutHorizontally = FALSE);
	void DeleteToolTip();

	void GetImageSize(UINT& width, UINT& height) {
		width = m_imageWidth;
		height = m_imageHeight;
	}

	void SetColor(COLORREF clr, BOOL b) {
		m_color = clr;
		m_bColorButtonEnable = b;
	}
	void SetColorButton(BOOL b) {
		m_bColorButton = b;
	}

protected:

	void PaintBk(CDC* pDC);
	void PaintBtn(CDC* pDC);

	BOOL	m_bHaveAltImage;
	BOOL	m_bHaveBitmaps;

	BOOL	m_bIsDisabled;
	BOOL	m_bIsToggle;
	BOOL	m_bIsHovering;
	BOOL	m_bIsTracking;
	BOOL	m_bIsSelected;

	int		m_nCurType;

	CGdiPlusBitmapResource* m_pAltImage;
	CGdiPlusBitmapResource* m_pStdImage;

	CString			m_tooltext;
	CToolTipCtrl*	m_pToolTip;

	void	InitToolTip();

	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//{{AFX_MSG(CCreadButton)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	UINT m_imageWidth;
	UINT m_imageHeight;


	COLORREF m_color;
	BOOL	m_bColorButton;
	BOOL	m_bColorButtonEnable;

	CRect	m_wndRect;		// size

	CDC		m_dcBk;			// button background

	CDC		m_dcStd;		// standard button
	CDC		m_dcStdP;		// standard button pressed
	CDC		m_dcStdH;		// standard button hot

	CDC		m_dcAlt;		// alternate button
	CDC		m_dcAltP;		// alternate button pressed
	CDC		m_dcAltH;		// alternate button hot

	CDC		m_dcGS;			// grayscale button (does not have a hot or pressed state)

	CDC*	m_pCurBtn;		// current pointer to one of the above

public:

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#endif CREADBUTTON_H