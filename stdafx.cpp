
// stdafx.cpp : source file that includes just the standard includes
// Media.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef DEBUG
	#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif


// ������ ����
CNetworkAnalyzerCtr* ctr;

CMediaHandler* handler;
// ������Ʈ ����
CProject* project;
CPredictionDlg* dlgPrediction;
CCollectionDlg* dlgCollection;


CString present_xml_input;
int general_msg_id_increment = 0;

// 
// prediction â���� ���̴� ������
// �ٸ����� Ȱ������ ������
// ���, ���� �
// 
int preiction_output_no_poles = 19;
bool prediction_output_flag[MAX_NO_POLES];
CString prediction_output_name[MAX_NO_POLES];
CString prediction_output_property[MAX_NO_POLES];
double prediction_output_result[MAX_NO_POLES];

int prediction_input_count;
double prediction_input_waveform_master[5][2048];
double prediction_input_waveform[10][5][2048];
double prediction_input_waveform_refl[5][2048];
double prediction_input_waveform_tran[5][2048];
char prediction_input_label[10][128];

double PCFreq = 0.0;
__int64 CounterStart = 0;

int _invisible_flag = 0;
// �׿ܿ��� ���� �� ��


void CopyCStringToChar(CString data, char* dest)
{
	wchar_t* wchar_str;     //ù��° �ܰ�(CString to wchar_t*)�� ���� ����
	int      char_str_len;  //char* �� ������ ���̸� ���� ����

	//1. CString to wchar_t* conversion
	wchar_str = data.GetBuffer(data.GetLength());

	//2. wchar_t* to char* conversion
	//char* ���� ���ѱ��̸� ����
	char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
	//char_str = new char[char_str_len];  //�޸� �Ҵ�
	//wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, dest, char_str_len, 0,0);  
	 


	/*


	wchar_t* wchar_str;     //ù��° �ܰ�(CString to wchar_t*)�� ���� ����
	char*    char_str;      //char* ���� ����
	int      char_str_len;  //char* �� ������ ���̸� ���� ����

	//1. CString to wchar_t* conversion
	wchar_str = weight_file_loc.GetBuffer(weight_file_loc.GetLength());

	//2. wchar_t* to char* conversion
	//char* ���� ���ѱ��̸� ����
	char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
	char_str = new char[char_str_len];  //�޸� �Ҵ�
	//wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, char_str, char_str_len, 0,0);  


	delete char_str;
	*/

}


BOOL _CreateDirectory( LPCTSTR lpszPath )
{
	TCHAR szPathBuffer[MAX_PATH];

	size_t len = _tcslen( lpszPath );

	for ( size_t i = 0 ; i < len ; i++ )
	{
		szPathBuffer[i] = *( lpszPath + i );
		if ( szPathBuffer[i] == _T('\\') || szPathBuffer[i] == _T('/') )
		{
			szPathBuffer[i + 1] = NULL;
			if ( ! PathFileExists( szPathBuffer ) )
			{
				if ( ! ::CreateDirectory( szPathBuffer, NULL ) )
				{
					if ( GetLastError() != ERROR_ALREADY_EXISTS )
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}


void StartCounter()
{
	LARGE_INTEGER li;
	if(!QueryPerformanceFrequency(&li))
	{
		return;
	}

	PCFreq = double(li.QuadPart)/1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart-CounterStart)/PCFreq;
}