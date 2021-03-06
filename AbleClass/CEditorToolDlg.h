#pragma once

#include "CCreadButton.h"
#include "CButtonDlg.h"

class CEditorToolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditorToolDlg)

public:
	CEditorToolDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CEditorToolDlg();


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITOR_TOOL };
#endif


	enum {
		BTN_NONE = 0,
		BTN_PREVIOUS = 1,
		BTN_NEXT,
		BTN_VIDEO,
		BTN_AUDIO,
		BTN_PICTURE,
		BTN_MEMO,
		BTN_BLANK,
		BTN_SAVE,
		BTN_EXIT
	};


	// skin
	Gdiplus::Image* m_pBack;

	CButtonDlg m_btnPrevious;
	CButtonDlg m_btnNext;
	CButtonDlg m_btnVideo;
	CButtonDlg m_btnPicture;
	CButtonDlg m_btnAudio;
	CButtonDlg m_btnMemo;
	CButtonDlg m_btnBlank;
	CButtonDlg m_btnSave;
	CButtonDlg m_btnExit;


	// layout
	int nButtonSize;
	int nLeftOffset;


	void DoButtonLayoutChange(UINT id, BOOL selected) {
		switch (id) {
		case BTN_VIDEO:
			m_btnVideo.SetSelect(TRUE);
			m_btnAudio.SetSelect(FALSE);
			m_btnPicture.SetSelect(FALSE);
			m_btnMemo.SetSelect(FALSE);
			m_btnBlank.SetSelect(FALSE);
			break;
		case BTN_AUDIO:
			m_btnVideo.SetSelect(FALSE);
			m_btnAudio.SetSelect(TRUE);
			m_btnPicture.SetSelect(FALSE);
			m_btnMemo.SetSelect(FALSE);
			m_btnBlank.SetSelect(FALSE);
			break;
		case BTN_PICTURE:
			m_btnVideo.SetSelect(FALSE);
			m_btnAudio.SetSelect(FALSE);
			m_btnPicture.SetSelect(TRUE);
			m_btnMemo.SetSelect(FALSE);
			m_btnBlank.SetSelect(FALSE);
			break;
		case BTN_MEMO:
			m_btnVideo.SetSelect(FALSE);
			m_btnAudio.SetSelect(FALSE);
			m_btnPicture.SetSelect(FALSE);
			m_btnMemo.SetSelect(TRUE);
			m_btnBlank.SetSelect(FALSE);
			break;
		case BTN_BLANK:
			m_btnVideo.SetSelect(FALSE);
			m_btnAudio.SetSelect(FALSE);
			m_btnPicture.SetSelect(FALSE);
			m_btnMemo.SetSelect(FALSE);
			m_btnBlank.SetSelect(TRUE);
			break;
		default:
			m_btnVideo.SetSelect(FALSE);
			m_btnAudio.SetSelect(FALSE);
			m_btnPicture.SetSelect(FALSE);
			m_btnMemo.SetSelect(FALSE);
			m_btnBlank.SetSelect(FALSE);
			break;
		}
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	public:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		virtual BOOL OnInitDialog();
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		void OnWindowPosChanging(WINDOWPOS* lpwndpos);
		LRESULT OnCreadButton(WPARAM wParam, LPARAM lParam);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
