// http://blog.daum.net/aswip/3797090 참고
#include "StdAfx.h"
#include "NamedPipe.h"

CNamedPipe* namedPipe;

CNamedPipe::CNamedPipe(CString pipe_in_name, CString pipe_out_name)
{
	exit_signal = 0;

	this->pipe_in_name = pipe_in_name;
	this->pipe_out_name = pipe_out_name;

	InitializeCriticalSection(&cs_queue_inbound);
	InitializeCriticalSection(&cs_queue_outbound);

#ifdef DEBUG
	period_info = 1000;
	period_ping = 5000;
#else
	period_info = 100;
	period_ping = 5000;
#endif

	/*
	LPTSTR data = _T("Hello");
	DWORD bytesWritten = 0;
	string str="Hello";
	//WriteFile(hPipe, str.data(), _tcslen(data) * sizeof(TCHAR), &bytesWritten, NULL);
	//WriteFile(hPipe, str.data(), str.length(), &bytesWritten, NULL);
	ReadFile(hPipe,buf,_tcslen(data) * sizeof(TCHAR),&bytesWritten,NULL);

	cout<<buf<<endl;
	 */

	increment = 0;
	_is_outbound_pipe_valid = 0;
} 
void CNamedPipe::start()
{ 
	p1 = AfxBeginThread(ThreadCommunicationOutbound, this);
	p2 = AfxBeginThread(ThreadCommunicationInbound, this);
	p3 = AfxBeginThread(ThreadCommunicationOutboundPing, this);
	p4 = AfxBeginThread(ThreadCommunicationOutboundInfo, this);
}
void CNamedPipe::stop()
{
	exit_signal = 1;

}
UINT CNamedPipe::ThreadCommunicationOutboundInfo(LPVOID _mothod)                                       
{ 
	::Sleep(100);
	while(handler==NULL || namedPipe==NULL)
	{
		::Sleep(10);
	}
	HANDLE hPipe;
	int inc = 0;
	char* output_info;
	output_info = new char[1024*1024];
	while(true)
	{
		if(namedPipe->exit_signal) break;
		//StartCounter();
		handler->GenerateInfoXML(output_info,1024*1024);
		namedPipe->PushQueueOutbound(CString(output_info));


		//double tt = GetCounter();
		//tt = tt;
		::Sleep(namedPipe->period_info);
	}
	delete output_info;
	return 0;
}
UINT CNamedPipe::ThreadCommunicationOutboundPing(LPVOID _mothod)                                       
{ 
	HANDLE hPipe; 
	while(true)
	{
		if(namedPipe->exit_signal) break;
		CString data;
		data.Format(L"<ping_from_server><message>%d</message><message_id>%d_%d</message_id></ping_from_server>",
			namedPipe->increment,
			++general_msg_id_increment, 
			time(NULL));
		namedPipe->PushQueueOutbound(data);
		::Sleep(namedPipe->period_ping);
	}
	return 0;
}
UINT CNamedPipe::ThreadCommunicationOutbound(LPVOID _mothod)                                       
{ 
	HANDLE hPipe;
	while(true)
	{

		hPipe = namedPipe->hPipeOut = ::CreateNamedPipe(_T("\\\\.\\pipe\\" + namedPipe->pipe_out_name),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUFSIZE_IN_SIZE,
			BUFSIZE_IN_SIZE,
			0,
			NULL);

		ConnectNamedPipe(hPipe, NULL);
		namedPipe->SetOutboundPipeValid();
		int i = 0;
		while (1)
		{ 
			if(namedPipe->exit_signal) break;


			if(namedPipe->EmptyQueueOutbound()) 
			{
				::Sleep(1);
				continue;;
			}
			CString mymsg = namedPipe->GetQueueOutboundDataAndPop();
			CString ahahah = mymsg  +  L"///eof///";
			LPTSTR lpvMessage = ahahah.GetBuffer(0);

			DWORD bytesWritten;
			int retval;
			retval = WriteFile(
				hPipe,
				lpvMessage, 
				(lstrlen(lpvMessage)+1)*sizeof(TCHAR),
				&bytesWritten, NULL); 
			if(retval==0)
			{
				OutputDebugString(L"CNamedPipe::ThreadCommunicationOutbound\r\n");
				OutputDebugString(L"WriteFile에서 실패\r\n");
				break;
			}
		}
		namedPipe->UnsetOutboundPipeValid();

		EnterCriticalSection(&namedPipe->cs_queue_outbound);
		namedPipe->qMsgOutbound = std::queue<CString>(); 
		LeaveCriticalSection(&namedPipe->cs_queue_outbound);
		CloseHandle(hPipe);
	}
	return 0;
}
UINT CNamedPipe::ThreadCommunicationInbound(LPVOID _mothod)                                       
{ 
	HANDLE hPipe;
	while (1)
	{
		hPipe = namedPipe->hPipeIn = ::CreateNamedPipe(_T("\\\\.\\pipe\\" + namedPipe->pipe_in_name),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE |
			PIPE_READMODE_MESSAGE |
			PIPE_WAIT, // || blocking
			PIPE_UNLIMITED_INSTANCES,
			BUFSIZE_IN_SIZE,
			BUFSIZE_IN_SIZE,
			0,
			NULL);
		ConnectNamedPipe(hPipe, NULL);
		
// 		TCHAR chRequest[BUFSIZE]; 
		// 		TCHAR chReply[BUFSIZE]; 
// 		TCHAR* chRequest = new TCHAR[BUFSIZE_IN_SIZE / sizeof(TCHAR) + 1]; 
// 		TCHAR* chReply = new TCHAR[BUFSIZE_IN_SIZE / sizeof(TCHAR) + 1]; 
		TCHAR* chRequest = new TCHAR[BUFSIZE_IN_SIZE  + 1]; 
		TCHAR* chReply = new TCHAR[BUFSIZE_IN_SIZE  + 1]; 
		DWORD cbBytesRead, cbReplyBytes, cbWritten; 
		BOOL fSuccess;   
		CString totalBuffer = L"";
		int indexer = 0;
		while(true)
		{

			cbBytesRead = 0;

			fSuccess = ReadFile( 
				hPipe,        // handle to pipe 
				chRequest,    // buffer to receive data 
				BUFSIZE_IN_SIZE, // size of buffer 
				&cbBytesRead, // number of bytes read 
				NULL);        // not overlapped I/O 

			if (! fSuccess && cbBytesRead!=BUFSIZE_IN_SIZE) 
			{
				OutputDebugString(L"CNamedPipe::ThreadCommunicationInbound\r\n");
				OutputDebugString(L"Read File Failed\r\n");
				break; 
			}
			if(cbBytesRead%2==1)
			{
				cbBytesRead = cbBytesRead;
			}
			if(cbBytesRead == 0)
			{
				OutputDebugString(L"cbBytesRead = 0 ERROR");

				break;
			}
			::Sleep(1);
			chRequest[cbBytesRead / sizeof(TCHAR)] = 0;
			if(indexer==8)
			{
				indexer = indexer;
			}
			totalBuffer += CString(chRequest);
			while(true)
			{
				int pos;
				pos = totalBuffer.Find(L"///eof///");
				if(pos>=0)
				{
					indexer = 0;
					CString left;
					int length = totalBuffer.GetLength();
					if(length>10000)
					{
						length = length;
					}
					left = totalBuffer.Left(pos);
					totalBuffer = totalBuffer.Right(length - (pos + 9));
					totalBuffer = totalBuffer;
					if(left.Find(L"배고프다")>=0)
					{
						handler->IncreasePingCount();
					}

					EnterCriticalSection(&namedPipe->cs_queue_inbound);
					namedPipe->qMsgInbound.push(left);
					LeaveCriticalSection(&namedPipe->cs_queue_inbound);
				}
				else
				{
					break;
				}
			}
			if(namedPipe->exit_signal) break;
		}
		delete chRequest;
		delete chReply;
		CloseHandle(hPipe);
	}
	return 0;
}



CNamedPipe::~CNamedPipe(void)
{
	CloseHandle(hPipeIn);
	CloseHandle(hPipeOut);


	DeleteCriticalSection(&cs_queue_inbound);
	DeleteCriticalSection(&cs_queue_outbound);
}

int CNamedPipe::EmptyQueueInbound()
{
	bool retv;
	EnterCriticalSection(&cs_queue_inbound);
	retv = qMsgInbound.empty();
	LeaveCriticalSection(&cs_queue_inbound);
	return retv;
}
int CNamedPipe::EmptyQueueOutbound()
{
	bool retv;
	EnterCriticalSection(&cs_queue_outbound);
	retv = qMsgOutbound.empty();
	LeaveCriticalSection(&cs_queue_outbound);
	return retv;
}

CString CNamedPipe::GetQueueInboundDataAndPop()
{
	CString retv = L"";
	EnterCriticalSection(&cs_queue_inbound);
	if(!qMsgInbound.empty())
	{
		retv = qMsgInbound.front();
		qMsgInbound.pop();
	}
	LeaveCriticalSection(&cs_queue_inbound);
	return retv;
}

CString CNamedPipe::GetQueueOutboundDataAndPop()
{
	CString retv = L"";
	EnterCriticalSection(&cs_queue_outbound);
	if(!qMsgOutbound.empty())
	{
		retv = qMsgOutbound.front();
		qMsgOutbound.pop();
	}
	LeaveCriticalSection(&cs_queue_outbound);
	return retv;
}

void CNamedPipe::PushQueueOutbound( CString data )
{
	if(IsPipeValid())
	{
		EnterCriticalSection(&cs_queue_outbound);
		qMsgOutbound.push(data);
		LeaveCriticalSection(&cs_queue_outbound);
	}
	else
	{
		OutputDebugString(L"CNamedPipe::PushQueueOutbound\r\n");
		OutputDebugString(L"파이프 통신이 연결되어 있지 않기 때문에 Message를 Queue에 넣지 않습니다.\r\n");
	}
}




void CNamedPipe::SetOutboundPipeValid()
{
	_is_outbound_pipe_valid = 1;
}
void CNamedPipe::UnsetOutboundPipeValid()
{
	_is_outbound_pipe_valid = 0;
}

bool CNamedPipe::IsPipeValid()
{
	return _is_outbound_pipe_valid;
}
