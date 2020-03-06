// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

//#define WINVER		0x0501
//#define _WIN32_IE		0x0600
//#define _RICHEDIT_VER	0x0300

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atlctrls.h>

#define helios_module "helios"

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#import ".\ImportTlb\MSADDNDR.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
#import ".\ImportTlb\MSO.DLL" rename("DocumentProperties", "MSO_DocumentProperties") rename("RGB", "MSO_RGB") named_guids rename_namespace("MSOffice") 
using namespace MSOffice;

#import ".\ImportTlb\VBE6EXT.OLB" auto_search rename_namespace("MSVBE")
using namespace MSVBE;

#import ".\ImportTlb\EXCEL.exe" rename("RGB", "Excel_RGB") rename("CopyFile", "Excel_CopyFile") rename("ReplaceText", "Excel_ReplaceText") rename("DialogBox", "Excel_DialogBox") \
								exclude("IFont", "IPicture", "Adjustments", "CalloutFormat", "ColorFormat", "LineFormat", "ShapeNode", "ShapeNodes", "PictureFormat", "ShadowFormat", \
								"TextEffectFormat", "ThreeDFormat", "FillFormat", "TextFrame2", "DiagramNode", "DiagramNodes", "DiagramNodeChildren") named_guids rename_namespace("MSExcel")
using namespace MSExcel;