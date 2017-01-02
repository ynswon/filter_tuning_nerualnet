	
// MediaDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "TrayIconMng.h"

// CMediaDlg dialog
class CMediaDlg : public CDialogEx
{
// Construction
public:
	CMediaDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MEDIA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	int gPass;
	int gFail;


	template< class T > bool XMLTest( const char* testString, T expected, T found, bool echo=true );

	tinyxml2::XMLError test_xml1();
	afx_msg void OnBnClickedButtonXmlInfo();
	afx_msg void OnBnClickedButtonXmlInfoProjectList();
	afx_msg void OnBnClickedButtonXmlInputDataSet();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButtonLearningTest();
	afx_msg void OnBnClickedButtonFTLearn();
	afx_msg void OnBnClickedButtonFTTest();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton2(); 


	afx_msg void OnBnClickedButtonShowPrediction();
	afx_msg void OnBnClickedButtonShowCollection();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButtonGoStart();
	afx_msg void OnBnClickedButtonCycle();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton19();
	CTreeCtrl m_treeInfo;
	afx_msg void OnBnClickedButton20();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void SetRadioStatus(UINT ID);
protected:
	HTREEITEM  hRoot;
	HTREEITEM  hChild[10];
	HTREEITEM  hChild2[100];
	HTREEITEM hChildProjectName;
	HTREEITEM hChildFilterName;
	HTREEITEM hChildSwName;

	HTREEITEM hChildFetchCount;
	HTREEITEM hChildMachStatus;
	HTREEITEM hChildPingCount;
	HTREEITEM hChildPoleOfInterest;
	HTREEITEM hChildMsgCountFromServerCount;
	HTREEITEM hChildErrorRate0;
	HTREEITEM hChildErrorRate1;
	HTREEITEM hChildErrorRate2;
	HTREEITEM hChildErrorRate3;
	HTREEITEM hChildErrorRate4;
	HTREEITEM hChildErrorRate5;
	HTREEITEM hChildTotalPoints;
	HTREEITEM hChildWaveRefl;
	HTREEITEM hChildWaveTran;


public:
	int m_RadioLang;
	int m_RadioDS;
	afx_msg void OnBnClickedButtonShowPrediction2();
	afx_msg void OnBnClickedRadioCpp();
	afx_msg void OnBnClickedButtonFt();
	afx_msg void OnBnClickedButtonFilterFt(); 
	afx_msg void OnBnClickedButtonTwLearn();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnBnClickedButton22();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton6();
	void ChangeLegacyLearningData(CString project_name, CString filter_name, CString sw_name);
	int CMediaDlg::GetFindCharCount(CString parm_string, char parm_find_char) ;
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton191();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton24();
	afx_msg void OnBnClickedButton25();
	afx_msg void OnBnClickedButton26();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton2711();

public:
	// 트레이 아이콘 관리용 객체
	CTrayIconMng m_myTray;
	// 트레이 아이콘 보이기/숨기기 여부
	long OnTrayIcon(WPARAM wParam, LPARAM lParam);
	void OnAppExit(void);
	void OnDialogShow(void);
	afx_msg void OnBnClickedRadioRscript();
	afx_msg void OnBnClickedButton28();
	afx_msg void OnBnClickedButton29();
	afx_msg void OnBnClickedButton30();
	afx_msg void OnBnClickedButton31();
	afx_msg void OnBnClickedButton32();
	afx_msg void OnBnClickedButton33();
	afx_msg void OnBnClickedButton34();
};
