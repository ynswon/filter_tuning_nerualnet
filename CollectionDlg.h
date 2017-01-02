#pragma once
#include "afxwin.h"


// CCollectionDlg dialog

class CCollectionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCollectionDlg)

public:
	CCollectionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCollectionDlg();

// Dialog Data
	enum { IDD = IDD_COLLECTION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRTSave();
	afx_msg void OnBnClickedButtonFTSave();
	virtual BOOL OnInitDialog();
	CComboBox m_comboFTPole;
	CComboBox m_comboFTDegree;
	CComboBox m_comboRTPole;
	CComboBox m_comboRTDegree;
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnCbnSelchangeComboRtDegree();
	afx_msg void OnBnClickedButtonFtMasterLoad();
	afx_msg void OnBnClickedButtonRtMasterLoad();
};
