// CollectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Media.h"
#include "CollectionDlg.h"
#include "afxdialogex.h"


// CCollectionDlg dialog

IMPLEMENT_DYNAMIC(CCollectionDlg, CDialogEx)

CCollectionDlg::CCollectionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCollectionDlg::IDD, pParent)
{

}

CCollectionDlg::~CCollectionDlg()
{
}

void CCollectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FT_POLE, m_comboFTPole);
	DDX_Control(pDX, IDC_COMBO_FT_DEGREE, m_comboFTDegree);
	DDX_Control(pDX, IDC_COMBO_RT_POLE, m_comboRTPole);
	DDX_Control(pDX, IDC_COMBO_RT_DEGREE, m_comboRTDegree);
}


BEGIN_MESSAGE_MAP(CCollectionDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_RT_SAVE, &CCollectionDlg::OnBnClickedButtonRTSave)
	ON_BN_CLICKED(IDC_BUTTON_FT_SAVE, &CCollectionDlg::OnBnClickedButtonFTSave)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CCollectionDlg::OnBnClickedButtonNext)
	ON_CBN_SELCHANGE(IDC_COMBO_RT_DEGREE, &CCollectionDlg::OnCbnSelchangeComboRtDegree)
	ON_BN_CLICKED(IDC_BUTTON_FT_MASTER_LOAD, &CCollectionDlg::OnBnClickedButtonFtMasterLoad)
	ON_BN_CLICKED(IDC_BUTTON_RT_MASTER_LOAD, &CCollectionDlg::OnBnClickedButtonRtMasterLoad)
END_MESSAGE_MAP()


// CCollectionDlg message handlers


void CCollectionDlg::OnBnClickedButtonRTSave()
{
	// TODO: Add your control notification handler code here
	/*
	m_comboFTPole
	m_comboFTDegree
	*/
	CString rt_pole_str  = L"";
	CString rt_degree_str = L"";
	int idx;
	idx = m_comboRTPole.GetCurSel();
	if(idx>=0) m_comboRTPole.GetLBText(idx,rt_pole_str);
	idx = m_comboRTDegree.GetCurSel();
	if(idx>=0) m_comboRTDegree.GetLBText(idx,rt_degree_str);

	int pole_idx = _ttoi(rt_pole_str);
	int pole_degree = 0;

	if(rt_degree_str == "Master")			
	{
		pole_degree = 0;
		handler->SetCollectMasterDataSignalAtThisTime(pole_idx);
				 

	}
	else if(rt_degree_str.Left(1)==L"+")	
	{
		pole_degree = _ttoi(rt_degree_str.Mid(2));
		handler->SetCollectSampleDataSignalAtThisTime(pole_idx,pole_degree,1);

	}
	else									
	{
		pole_degree = -1 * _ttoi(rt_degree_str.Mid(2));
		handler->SetCollectSampleDataSignalAtThisTime(pole_idx,pole_degree,1);
	}

//	handler->DoMeasurementRT(pole_idx,pole_degree); 

	
}


void CCollectionDlg::OnBnClickedButtonFTSave()
{
	// TODO: Add your control notification handler code here
	/*
	m_comboFTPole
	m_comboFTDegree
	*/
	CString ft_pole_str  = L"";
	CString ft_degree_str = L"";
	int idx;
	idx = m_comboFTPole.GetCurSel();
	if(idx>=0) m_comboFTPole.GetLBText(idx,ft_pole_str);
	idx = m_comboFTDegree.GetCurSel();
	if(idx>=0) m_comboFTDegree.GetLBText(idx,ft_degree_str);

	int pole_idx = _ttoi(ft_pole_str);
	int pole_degree = 0;

	if(ft_degree_str == "Master")			
	{
		pole_degree = 0;

		handler->SetCollectMasterDataSignalAtThisTime(pole_idx);
	}
	else if(ft_degree_str.Left(1)==L"+")	
	{
		pole_degree = _ttoi(ft_degree_str.Mid(2));

		handler->SetCollectSampleDataSignalAtThisTime(pole_idx,pole_degree,1);
	}
	else									
	{
		pole_degree = -1 * _ttoi(ft_degree_str.Mid(2));

		handler->SetCollectSampleDataSignalAtThisTime(pole_idx,pole_degree,1);
	}

	// handler->DoMeasurementFT(pole_idx,pole_degree); 
}

BOOL CCollectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	int i;
	for(i=0;i<project->GetTotalPoles();i++)
	{
		CString pole_name;
		pole_name.Format(L"%d",i+1);
		m_comboFTPole.InsertString(i,pole_name);
		m_comboRTPole.InsertString(i,pole_name);
	}

	int index = 0;

	m_comboFTDegree.InsertString(index, L"Master");
	m_comboRTDegree.InsertString(index, L"Master");
	index++;
	for(i=1;i<=10;i++)
	{
		CString pole_name;
		pole_name.Format(L"+ %d",i);
		m_comboFTDegree.InsertString(index, pole_name);	
		m_comboRTDegree.InsertString(index, pole_name);
		index++;
	}
	for(i=1;i<=10;i++)
	{
		CString pole_name;
		pole_name.Format(L"- %d",i);
		m_comboFTDegree.InsertString(index, pole_name);
		m_comboRTDegree.InsertString(index, pole_name);
		index++;
	}
	m_comboRTPole.SetCurSel(0);
	m_comboRTDegree.SetCurSel(0);
	m_comboFTPole.SetCurSel(0);
	m_comboFTDegree.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CCollectionDlg::OnBnClickedButtonNext()
{
	// TODO: Add your control notification handler code here
	int idx = m_comboFTDegree.GetCurSel();
	if(idx == 20)
	{
		m_comboFTDegree.SetCurSel(0);
		int pole_idx = m_comboFTPole.GetCurSel();
		m_comboFTPole.SetCurSel( pole_idx +1  );
	}
	else
	{

		m_comboFTDegree.SetCurSel(idx+1);
	}
}


void CCollectionDlg::OnCbnSelchangeComboRtDegree()
{
	// TODO: Add your control notification handler code here
}


void CCollectionDlg::OnBnClickedButtonFtMasterLoad()
{
	// TODO: Add your control notification handler code here
	CString ft_pole_str  = L"";
	CString ft_degree_str = L"";
	int idx;
	idx = m_comboFTPole.GetCurSel();
	if(idx>=0) m_comboFTPole.GetLBText(idx,ft_pole_str);
	idx = m_comboFTDegree.GetCurSel();
	if(idx>=0) m_comboFTDegree.GetLBText(idx,ft_degree_str);

	int pole_idx = _ttoi(ft_pole_str);
	int pole_degree = 0;

	handler->LoadMasterWaveForm(pole_idx);
}


void CCollectionDlg::OnBnClickedButtonRtMasterLoad()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	CString rt_pole_str  = L"";
	CString rt_degree_str = L"";
	int idx;
	idx = m_comboRTPole.GetCurSel();
	if(idx>=0) m_comboRTPole.GetLBText(idx,rt_pole_str);
	idx = m_comboRTDegree.GetCurSel();
	if(idx>=0) m_comboRTDegree.GetLBText(idx,rt_degree_str);

	int pole_idx = _ttoi(rt_pole_str);
	int pole_degree = 0;

	handler->LoadMasterWaveForm(pole_idx);
}
