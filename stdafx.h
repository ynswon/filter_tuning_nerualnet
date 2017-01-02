// 
// http://zacurr.tistory.com/217
//


// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#define ANDY_YES	0
#define ANDY_NO	1

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars






#define WM_DIALOG_SHOW	WM_USER + 101	//다이얼로그 감추기/보이기
#define WM_APP_EXIT		WM_USER + 102	//다이얼로그 종료
#define WM_TRAYICON		WM_USER + 103	//트레이 아이콘 메시지


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#define MAX_NO_POLES 100
#define INT int
//#include "External Training Lib/neuralNetwork.h"
//#include "External Training Lib/neuralNetworkTrainer.h"
#include "Feedforward_Neural_Network.h"
#include "Rprop.h"
#include "Rprop_Network.h"
#include "Fetch Lib/NetworkAnalyzerCtr.h"
#include "Tiny Xml Lib/tinyxml2.h"
#include "MediaHandler.h"
#include "Fetch Lib/CProject.h"
#include "PredictionDlg.h"
#include "CollectionDlg.h"
#include "NamedPipe.h"
#include "SocketComm.h"


 

extern CNetworkAnalyzerCtr* ctr; 
extern CMediaHandler* handler;
extern CProject* project;
extern CPredictionDlg* dlgPrediction;
extern CCollectionDlg* dlgCollection;

extern int preiction_output_no_poles;
extern bool prediction_output_flag[MAX_NO_POLES];
extern CString prediction_output_name[MAX_NO_POLES];
extern CString prediction_output_property[MAX_NO_POLES];
extern double prediction_output_result[MAX_NO_POLES];

extern int prediction_input_count;
extern double prediction_input_waveform_master[5][2048];
extern double prediction_input_waveform[10][5][2048];
extern double prediction_input_waveform_refl[5][2048];
extern double prediction_input_waveform_tran[5][2048];
extern char prediction_input_label[10][128];
extern BOOL _CreateDirectory( LPCTSTR lpszPath );

void CopyCStringToChar(CString data, char* dest);

extern void StartCounter();
extern double GetCounter();
extern double PCFreq;
extern __int64 CounterStart;



extern CString present_xml_input;
extern int general_msg_id_increment;

extern int _invisible_flag;