
// BoxBlurDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CBoxBlurDlg dialog
class CBoxBlurDlg : public CDialogEx
{
// Construction
public:
	CBoxBlurDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BOXBLUR_DIALOG };


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpen();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BYTE* pBuf;
	BYTE* pImg;

protected:
	void	_ShowImage();
	void	_DoBoxBlur();
	UINT	nh;
	UINT	nw;
	int m_PixelFormat;
	int m_nSizePerPixel;
	int m_nStride;

	UINT	nLevel;
public:
	afx_msg void OnBnClickedButton1();
};
