/********************************************************************
	created:	2005/09/29
	filename: 	c:\XDemoShow\XDemoShow\xGdiPlus.h
	file ext:	h
	author:		jianwen chen
	
	purpose:	Manage all GDIPlus funtions in	the projects
*********************************************************************/
#pragma once

//////////////////////////////////////////////////////////////////////////
// Add GDI+ support to MFC or WTL application.
// Ensure that GdiPlus header files work properly with MFC DEBUG_NEW and 
// STL header files.
// Q317799: PRB: Microsoft Foundation Classes DEBUG_NEW Does Not Work with GDI+
//
#define iterator _iterator

#ifdef _DEBUG

static int nGdiPlusObjects = 0;
namespace Gdiplus
{
	namespace DllExports
	{
		#include <GdiplusMem.h>
	};
	
	#ifndef _GDIPLUSBASE_H
	#define _GDIPLUSBASE_H
	class GdiplusBase
	{
	public:
		void (operator delete)(void* in_pVoid)
		{
			nGdiPlusObjects--;
			DllExports::GdipFree(in_pVoid);
		}
		
		void* (operator new)(size_t in_size)
		{
			nGdiPlusObjects++;
			return DllExports::GdipAlloc(in_size);
		}
		
		void (operator delete[])(void* in_pVoid)
		{
			nGdiPlusObjects--;
			DllExports::GdipFree(in_pVoid);
		}
		
		void* (operator new[])(size_t in_size)
		{
			nGdiPlusObjects++;
			return DllExports::GdipAlloc(in_size);
		}
		
		void * (operator new)(size_t nSize, LPCSTR /*lpszFileName*/, int /*nLine*/)
		{
			nGdiPlusObjects++;
			return DllExports::GdipAlloc(nSize);
		}
		
		void operator delete(void* p, LPCSTR /*lpszFileName*/, int /*nLine*/)
		{
			nGdiPlusObjects--;
			DllExports::GdipFree(p);
		}
		
	};
	#endif // #ifndef _GDIPLUSBASE_H
}
#endif // #ifdef _DEBUG

// GDI+ needed
typedef unsigned long ULONG_PTR;
typedef unsigned long *PULONG_PTR;
#include <GdiPlus.h>

#include <float.h>
#include <GdiPlusTypes.h>


#ifdef _MFC_VER
#include <afx.h>
#endif
#undef iterator

using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")

//////////////////////////////////////////////////////////////////////////
// Used in CWinApp object
class CXGdiPlusApp
{
public:
	CXGdiPlusApp():m_ulGdiPlusToken(0)
	{
		GdiplusStartupInput gdiPlusInput;
		// Startup GDI+ system
		if (Ok != GdiplusStartup(&m_ulGdiPlusToken, &gdiPlusInput, NULL))
		{
			ASSERT(FALSE);
			TRACE ("xCoreBase:: GDIPlusStartUp Failed!!\n");
		}
	}
	~CXGdiPlusApp()
	{
		// Close GDI+ system
		GdiplusShutdown(m_ulGdiPlusToken);

#ifdef _DEBUG
	#ifdef _MFC_VER
			if (nGdiPlusObjects > 0)
			{
				afxDump << _T(">>> GdiPlus Memory Leaks: ") 
						<< nGdiPlusObjects 
						<< _T(" objects! <<<\n");
			}
			else if (nGdiPlusObjects < 0)
			{
				afxDump << _T(">>> GdiPlus Multiple Deletes: ") 
						<< -nGdiPlusObjects << 
						_T(" objects! <<<\n");
			}
	#endif
	#ifdef _ATL_VER
			if (nGdiPlusObjects > 0)
				ATLTRACE2(_T(">>> GdiPlus Memory Leaks: %d objects! <<<\n"), nGdiPlusObjects);
			else if (nGdiPlusObjects < 0)
				ATLTRACE2(_T(">>> GdiPlus Multiple Deletes: %d objects! <<<\n"), -nGdiPlusObjects);
	#endif
#endif
	}
protected:
	ULONG m_ulGdiPlusToken;
};
