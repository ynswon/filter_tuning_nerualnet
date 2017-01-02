#pragma once

#include "resource.h"

#include "chartviewer.h"

// CPredictionDlg dialog

class CPredictionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPredictionDlg)

public:
	CPredictionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPredictionDlg();

// Dialog Data
	enum { IDD = IDD_PREDICTION_DIALOG };


	CChartViewer m_chartView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	char* labels[1000];
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
	
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	void update_chart();

private:

	CButton* button_index[MAX_NO_POLES];
	CButton* button_name[MAX_NO_POLES];
	CButton* button_data[MAX_NO_POLES];

public:
	afx_msg void OnStnClickedChart();
	virtual BOOL DestroyWindow();
	afx_msg void OnDestroy();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedButton2();
	int _is_phase;
};
