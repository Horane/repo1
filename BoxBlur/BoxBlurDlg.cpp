
// BoxBlurDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BoxBlur.h"
#include "BoxBlurDlg.h"
#include "afxdialogex.h"
#include "XGdiPlus.h"
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBoxBlurDlg dialog


CXGdiPlusApp m_gdiPlusInstance;

CBoxBlurDlg::CBoxBlurDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBoxBlurDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pBuf = NULL;
	pImg = NULL;
	m_PixelFormat = PixelFormatUndefined;
	nw = 0;
	nh = 0;
	nLevel = 0;
	m_nSizePerPixel = 0;
	m_nStride = 0;
}


BEGIN_MESSAGE_MAP(CBoxBlurDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_OPEN, &CBoxBlurDlg::OnBnClickedOpen)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON1, &CBoxBlurDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CBoxBlurDlg message handlers

BOOL CBoxBlurDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CScrollBar* p = (CScrollBar*)GetDlgItem(IDC_SCROLLBAR2);
	p->SetScrollRange(0, 100);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBoxBlurDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBoxBlurDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CBoxBlurDlg::OnBnClickedOpen()
{
	// TODO: Add your control notification handler code here
	CFileDialog cfd(TRUE);
	CString strFilename;
	if(IDOK == cfd.DoModal())
	{
		strFilename = cfd.GetPathName();
		Gdiplus::Bitmap bmp(strFilename);
		nh = bmp.GetHeight();
		nw = bmp.GetWidth();
		Gdiplus::Rect rcBmp(0, 0, 0, 0);
		rcBmp.Width = nw;
		rcBmp.Height = nh;
		BitmapData bd;
		bmp.LockBits(&rcBmp, ImageLockModeRead|ImageLockModeWrite, bmp.GetPixelFormat(), &bd);
		if(bd.Scan0 != NULL)
		{
			m_nStride = bd.Stride;
			m_nSizePerPixel = (((bd.PixelFormat >> 8) & 0x000000ff) / 8);
			if(pBuf)
			{
				delete [] pBuf;
				pBuf = NULL;
			}
			pBuf = new BYTE[bd.Height * bd.Stride];
			ZeroMemory(pBuf, bd.Height * bd.Stride);
			if (pImg)
			{
				delete[] pImg;
				pImg = NULL;
			}
			pImg = new BYTE[bd.Height * bd.Stride];
			ZeroMemory(pImg, bd.Height * bd.Stride);
			memcpy_s(pBuf, bd.Height * bd.Stride, bd.Scan0, bd.Height * bd.Stride);
			memcpy_s(pImg, bd.Height * bd.Stride, bd.Scan0, bd.Height * bd.Stride);
			m_PixelFormat = bd.PixelFormat;
			_ShowImage();
		}

	}
}

void CBoxBlurDlg::OnDestroy()
{
	if(pBuf != NULL)
	{
		delete [] pBuf;
		pBuf = NULL;
	}
	if (pImg)
	{
		delete[] pImg;
		pImg = NULL;
	}
}

void CBoxBlurDlg::_ShowImage()
{
	if (pImg == NULL)
	{
		return;
	}
	CClientDC dc(this);
	CRect rc;
	GetClientRect(rc);
	rc.bottom -= 30;
	RectF frc;
	frc.X = rc.left;
	frc.Y = rc.top;
	frc.Width = rc.Width();
	frc.Height = rc.Height();
	Graphics g(dc.GetSafeHdc());
	//Bitmap bmp(nw, nh, nw*m_nSizePerPixel, m_PixelFormat, pImg);
	Bitmap bmp(nw, nh, m_nStride, m_PixelFormat, pImg);

	g.DrawImage(&bmp, 0, 0, 0, 0, nw, nh, UnitPixel);
}

void CBoxBlurDlg::_DoBoxBlur()
{
	for(int nLine = 0; nLine < nh; ++nLine)
	{
		BYTE* pLine = pBuf + nLine * m_nStride;
		BYTE* pImgLine = pImg + nLine * m_nStride;
		for (int nColumn = 0; nColumn < nw; ++nColumn)
		{
			int nStart = nColumn - nLevel > 0 ? nColumn - nLevel : 0;
			DWORD dwPixel = 0;
			BYTE* pPixel = (BYTE*)&dwPixel;
			memcpy(pPixel, pLine + nColumn*m_nSizePerPixel, m_nSizePerPixel);
			//TRACE(_T("dwPixel = %d\r\n"), dwPixel);
			for (int n = nStart; n < (nw < nColumn + nLevel ? nw : nColumn + nLevel); ++n)
			{
				DWORD dwValue = 0;
				BYTE* pValue = (BYTE*)&dwValue;
				memcpy(pValue, pLine + n*m_nSizePerPixel, m_nSizePerPixel);
				dwPixel = dwPixel >> 1 + dwValue >> 1;
				//TRACE(_T("%d\r\n"), dwPixel);
			}
			memcpy(pImgLine + nColumn * m_nSizePerPixel, pPixel, m_nSizePerPixel);
		}
	}
	
}

BOOL CBoxBlurDlg::PreTranslateMessage(MSG* pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

void CBoxBlurDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	nLevel = nPos;
	pScrollBar->SetScrollPos(nPos);
	CString str;
	str.Format(_T("%d %d"), nPos, pScrollBar->GetScrollPos());
	SetDlgItemText(IDC_SLI, str);
	if (pBuf)
	{
		_DoBoxBlur();
		_ShowImage();
	}
}


void CBoxBlurDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
}
