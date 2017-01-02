// PredictionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Media.h"
#include "PredictionDlg.h"
#include "afxdialogex.h"


// CPredictionDlg dialog

IMPLEMENT_DYNAMIC(CPredictionDlg, CDialogEx)

CPredictionDlg::CPredictionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPredictionDlg::IDD, pParent)
{

	int i;
	for(i=0;i<1000;i++)
	{
		labels[i] = new char[100];
	}

	_is_phase = 0;
}

CPredictionDlg::~CPredictionDlg()
{
	int i;
	for(i=0;i<1000;i++)
	{
		delete labels[i];
	}
	delete[] labels;
}

void CPredictionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHART, m_chartView);
}


BEGIN_MESSAGE_MAP(CPredictionDlg, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CPredictionDlg::OnBnClickedButton1)
	ON_STN_CLICKED(IDC_CHART, &CPredictionDlg::OnStnClickedChart)
	ON_WM_DESTROY()
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON2, &CPredictionDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CPredictionDlg message handlers


void CPredictionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==108)
	{
		if(project==NULL) return;
		CString dstr;
		CString pp;
		if(project!=NULL)
		{
			preiction_output_no_poles = project->GetTotalPoles();
		}


		int name_idx = 0;
		for(name_idx=1;name_idx<=project->GetTotalPoles();name_idx++)
		{
			prediction_output_name[name_idx - 1] = project->GetPoleNameAt(name_idx);
			prediction_output_property[name_idx - 1] = project->GetPolePropertyAt(name_idx);
		}



		int i;
		for(i=0;i<preiction_output_no_poles;i++)
		{
			CString dstr;
			dstr.Format(L"%d",i+1,i+1);
			SetDlgItemText(3000+i,dstr);

			if(!prediction_output_flag[i])
			{
				//dstr.Format(L"%.1lf",(prediction_output_result[i]-50)*100.0);
				dstr.Format(L"%.1lf",(prediction_output_result[i]));
			}
			else
			{
				//dstr.Format(L">%.1lf<",(prediction_output_result[i]-50)*100.0);
				dstr.Format(L">>%.1lf<<",(prediction_output_result[i]));
			}
			SetDlgItemText(3200+i,dstr);
		}
		for(i=0;i<project->GetTotalPoles();i++)
		{
			SetDlgItemText(3100+i,prediction_output_name[i]);
		}

		update_chart();
	}
	CDialogEx::OnTimer(nIDEvent);
}




void CPredictionDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	this->ShowWindow(SW_HIDE);
}


BOOL CPredictionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

#ifndef USE_VISA
	AfxMessageBox(L"계측기로부터 자료를 받아 오지 않습니다(define USE_VISA 확인 필요)");
#endif
	// UPDATE UI
	SetTimer(108,50, NULL);

	// TODO:  Add extra initialization here
	int start_y = 50, start_x = 0;
	int button_width = 55;
	int button_gap = 2;
	

	for(int i=0;i<MAX_NO_POLES;i++)
	{
		CString dstr;
		dstr.Format(L"%d",i+1);

		button_index[i] = new CButton;
		button_index[i]->Create(NULL, 
			BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 
			CRect(
			10 + i * (button_width+button_gap), start_y, 
			10 + i * (button_width+button_gap) + button_width, start_y + 40), this,
			3000 + i 
			);


		button_name[i] = new CButton;
		int delta = 0;
		if(i%2==1) delta = 15;

		button_name[i]->Create(NULL, 
			BS_PUSHBUTTON | WS_VISIBLE
			| WS_CHILD, CRect(
			10 + i * (button_width+button_gap), start_y + 50 + delta, 
			10 + i * (button_width+button_gap) + button_width, start_y + 90 + delta), this, 
			3100 + i 
			);



		button_data[i] = new CButton;
		button_data[i]->Create(NULL, 
			BS_PUSHBUTTON | WS_VISIBLE 
			| WS_CHILD, CRect(
			10 + i * (button_width+button_gap), start_y + 120, 
			10 + i * (button_width+button_gap) + button_width, start_y + 160 ), this, 
			3200 + i 
			);
		int disp_pole ;
		if(project!=NULL) 
		{
			disp_pole = 20;;	
		}
		else
		{

			disp_pole = 20;
		}
		if(disp_pole <= i)
		{
			button_index[i]->ShowWindow(SW_HIDE);
			button_name[i]->ShowWindow(SW_HIDE);
			button_data[i]->ShowWindow(SW_HIDE);
		}


		CFont m_font;
		m_font.CreateFont(13, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS, L"돋움");
		button_data[i]->SetFont(&m_font, TRUE);

	}
	prediction_output_result[10] = 123;

	/*
	prediction_output_title[name_idx++]=L"RX1";
	prediction_output_title[name_idx++]=L"C1-2";
	prediction_output_title[name_idx++]="RX2";
	prediction_output_title[name_idx++]=L"C2-3";
	prediction_output_title[name_idx++]=L"RX3";
	prediction_output_title[name_idx++]=L"C3-4";
	prediction_output_title[name_idx++]=L"RX4";
	prediction_output_title[name_idx++]=L"C1-4";
	prediction_output_title[name_idx++]=L"C1-3";
	prediction_output_title[name_idx++]=L"C4-5";
	prediction_output_title[name_idx++]=L"RX5";
	prediction_output_title[name_idx++]=L"C5-6";
	prediction_output_title[name_idx++]=L"RX6";
	prediction_output_title[name_idx++]=L"C6-7";
	prediction_output_title[name_idx++]=L"RX7";
	prediction_output_title[name_idx++]=L"C7-8";
	prediction_output_title[name_idx++]=L"RX8";
	prediction_output_title[name_idx++]=L"C5-8";
	prediction_output_title[name_idx++]=L"C5-7";
	*/
	


	if(_invisible_flag)
	{
		ShowWindow(SW_HIDE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPredictionDlg::update_chart()
{
	if(ctr==NULL) return;



	int total_points = prediction_input_count;
	if(total_points<=0) return; 
	int i;
	double display_data[300][310];
	double my_master[5][310];
	bool label_new_teached = false;

	label_new_teached = true; 
	std::list<int>::iterator itor;

	int display_idx = 0;
	memset(my_master,0 ,sizeof(my_master));
	for(int k=0;k<3;k++)
	{
		for(i=0;i<total_points;i++)
		{

			display_data[display_idx][i] = 1;
			my_master[IDX2_MAG][i] = prediction_input_waveform_master[IDX2_MAG][i];
			my_master[IDX2_PHASE][i] = abs(prediction_input_waveform_master[IDX2_PHASE][i]);

			sprintf(labels[i], prediction_input_label[k]);
		}

		display_idx++;
	} 

	// Create an XYChart object of size 600 x 300 pixels, with a light blue (EEEEFF) background,
	// black border, 1 pxiel 3D border effect and rounded corners
	XYChart *c = new XYChart(800, 400, 0xeeeeff, 0x000000, 1);
	c->setRoundedFrame();

	// Set the plotarea at (55, 58) and of size 520 x 195 pixels, with white background. Turn on
	// both horizontal and vertical grid lines with light grey color (0xcccccc)
	c->setPlotArea(55, 58, 700, 315, 0xffffff, -1, -1, 0xcccccc, 0xcccccc);

	// Add a legend box at (50, 30) (top of the chart) with horizontal layout. Use 9pt Arial Bold
	// font. Set the background and border color to Transparent.
	c->addLegend(50, 30, false, "arialbd.ttf", 9)->setBackground(Chart::Transparent);

	// Add a title box to the chart using 15pt Times Bold Italic font, on a light blue (CCCCFF)
	// background with glass effect. white (0xffffff) on a dark red (0x800000) background, with a 1
	// pixel 3D border.
	c->addTitle("Filter Builder", "timesbi.ttf", 15)->setBackground(0xccccff,
		0x000000, Chart::glassEffect());

	// Add a title to the y axis
	c->yAxis()->setTitle("Output");
	if(_is_phase)
	{
		c->yAxis()->setDateScale(0,360);

	}
	else
	{
		c->yAxis()->setDateScale(0,-35);
	}
	// Set the 
	c->xAxis()->setLabels(StringArray(labels, total_points));

	// Display 1 out of 3 labels on the x-axis.
	c->xAxis()->setLabelStep(3);

	// Add a title to the x axis
	c->xAxis()->setTitle("Filter Response");

	// Add a line layer to the chart
	LineLayer *layer = c->addLineLayer();

	// Set the default line width to 2 pixels
	layer->setLineWidth(3);

	// Add the three zdata sets to the line layer. For demo purpose, we use a dash line color for the
	// last line

	display_idx = 0;
	for(int k=0;k<3;k++)
	{
		for(i=0;i<total_points;i++)
		{
			if(_is_phase)
			{
				display_data[display_idx][i] = prediction_input_waveform[k][IDX2_PHASE][i] + 180; 
			}
			else
			{ 
				display_data[display_idx][i] = prediction_input_waveform[k][IDX2_MAG][i]; 
			}
//			data_master[i] = 5;
			sprintf(labels[i],"%.2f",6);
		}
		char line_title[64];
		// sprintf(line_title,"S%d%d",(mode/16)+1,(mode%16)+1);
		sprintf(line_title, prediction_input_label[k]);
		int display_color;
		switch(display_idx)
		{
		case 0: display_color = 0x0000ff; break;
		case 1: display_color = 0x00ff00; break;
		case 2: display_color = 0xff0000; break;
		case 3: display_color = 0xcccccc; break;
		case 4: display_color = 0xff00ff; break;
		case 5: display_color = 0xffff00; break;
		case 6: display_color = 0x00ffff; break;
		}
		layer->addDataSet(DoubleArray(display_data[display_idx], total_points), display_color, line_title);

		display_idx++;
	}

	if(_is_phase)
	{
		layer->addDataSet(DoubleArray(my_master[IDX2_PHASE], total_points), 0x000000,
			"Master");
	}
	else
	{
		layer->addDataSet(DoubleArray(my_master[IDX2_MAG], total_points), 0x000000,
			"Master");
	}
	// Output the chart
	c->makeChart();


	m_chartView.setChart(c);

	if(label_new_teached)
	{
		for(i=0;i<total_points;i++)
		{
			//delete labels[i];
		}
	}
	delete c;

}


void CPredictionDlg::OnStnClickedChart()
{
	// TODO: Add your control notification handler code here
}


BOOL CPredictionDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialogEx::DestroyWindow();
}


void CPredictionDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	int i;
	for(int i=0;i<MAX_NO_POLES;i++)
	{
		delete button_index[i];

		delete button_name[i];

		delete button_data[i];

		button_index[i] = NULL;


	}
}
BOOL CPredictionDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(wParam >= 3000 && wParam < 3000+MAX_NO_POLES)
	{
		int target_pole = wParam - 3000;
		target_pole = target_pole + 1;
		if(project->GetSWName()==L"RT")
		{

			project->SetRTTargetPole(target_pole);
			handler->LoadMasterWaveForm(target_pole);
			// handler->PredictRT(target_pole, NULL);
			CString dstr;
			dstr.Format(L"TW의 선택된 푤이 %d번 Pole로 바뀌었습니다",target_pole);

		}
	}

	return CDialog::OnCommand(wParam, lParam);
} 


void CPredictionDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default

/*	if(nIDCtl >= 3200 && nIDCtl < 3200 + project->GetTotalPoles()  )
	{
		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct->hDC);	//Get the Button DC to CDC

		rect = lpDrawItemStruct->rcItem	;	//Store the Button rect to local rect
		dc.Draw3dRect(&rect, RGB(200,200,200), RGB(20,20,20));
		dc.FillSolidRect(&rect, RGB(100,150,100));


		//Show the Effect of Click Event
		UINT state = lpDrawItemStruct->itemState;
		if((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);
		}
		else
		{
			dc.DrawEdge(&rect, EDGE_RAISED, BF_RECT);
		}

		//Draw Color Text
		dc.SetBkColor(RGB(100,150,100));		//Setting the Text Background Color
		dc.SetTextColor(RGB(255,100,0));		//Setting the Text Color

		TCHAR buffer[MAX_PATH];
		ZeroMemory(buffer, MAX_PATH);
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH);

		dc.DrawText(buffer, &rect, DT_CENTER|DT_VCENTER |DT_SINGLELINE);
		dc.Detach();							//Detach the Button DC

	}

	*/
	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CPredictionDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	_is_phase = 1 - _is_phase;
}
