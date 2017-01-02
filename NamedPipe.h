#pragma once
#include <queue>
#define BUFSIZE_IN_SIZE 4096
class CNamedPipe
{
public:
	CNamedPipe(CString pipe_in_name, CString pipie_out_name);
	~CNamedPipe(void);
	static UINT ThreadCommunicationOutboundInfo(LPVOID _mothod);  
	static UINT ThreadCommunicationOutboundPing(LPVOID _mothod);  
	static UINT ThreadCommunicationOutbound(LPVOID _mothod);  
	static UINT ThreadCommunicationInbound(LPVOID _mothod);  
	CWinThread *p1;
	CWinThread *p2;
	CWinThread *p3;
	CWinThread *p4;

	void start();
	void stop();
public:
	HANDLE hPipeIn;
	HANDLE hPipeOut;
	int exit_signal;
	CString pipe_out_name;
	CString pipe_in_name;
public:
	int period_info;
	int period_ping;
public:
	std::queue <CString> qMsgInbound;
	std::queue <CString> qMsgOutbound;
	CRITICAL_SECTION cs_queue_inbound;
	CRITICAL_SECTION cs_queue_outbound;
public:
	void PushQueueOutbound(CString data);
	int EmptyQueueInbound();	
	int EmptyQueueOutbound();
	CString GetQueueInboundDataAndPop();
	CString GetQueueOutboundDataAndPop();

	void SetOutboundPipeValid();
	void UnsetOutboundPipeValid();
	bool IsPipeValid();
	int increment;
private:
	int _is_outbound_pipe_valid;
};
extern CNamedPipe* namedPipe;