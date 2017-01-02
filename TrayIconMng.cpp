// TrayIconMng.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Media.h"
#include "TrayIconMng.h"


// CTrayIconMng

CTrayIconMng::CTrayIconMng()
: m_bAdded(false)
, m_bHide(false)
{
}

CTrayIconMng::~CTrayIconMng()
{
}


// �˾��޴� ����
void CTrayIconMng::MakePopupMenu(HWND hWnd, int x, int y)
{	
	//�˾� �޴��� �����ϰ� �޴� ����
	HMENU hMenu = CreatePopupMenu();
	if(m_bHide)		//���̾�αװ� ������ ���¶�� 
        AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("���̾�α� ���̱�"));
    else			//���̾�αװ� ������ ���¶��
        AppendMenu(hMenu, MF_STRING, WM_DIALOG_SHOW, _T("���̾�α� ���߱�"));
	
	AppendMenu(hMenu, MF_STRING, WM_APP_EXIT, _T("����"));
	
	SetForegroundWindow(hWnd);//������ �˾��޴� ���� Ŭ���� �� �˾� �ݱ�
	//�˾� �޴� ����
    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hWnd, NULL);
}


// �˾��޴��� �̺�Ʈ �߻��� ó�� �Լ�
void CTrayIconMng::ProcTrayMsg(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu = NULL;
    POINT pos;

    if(lParam == WM_LBUTTONDOWN)
    {
        GetCursorPos(&pos);
		MakePopupMenu(hWnd, pos.x, pos.y); //�˾� �޴� ���� �� ���
    }
}

// Ʈ���� ������ ����
BOOL CTrayIconMng::AddTrayIcon(HWND hWnd)
{
	if(m_bAdded)		//�̹� Ʈ���� �������� �ִٸ� ����
		return FALSE;

	NOTIFYICONDATA nid;	//�������� �����Ͽ� ����
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    sprintf((char*)nid.szTip, (char*)_T("TrayTest"));
    nid.uID = 0;
    nid.hIcon = theApp.LoadIcon(IDR_MAINFRAME);

    if(Shell_NotifyIcon(NIM_ADD, &nid)==0) //Ʈ���� ������ ǥ��
        return FALSE;
	m_bAdded = true;	
    return TRUE;
}

// Ʈ���� ������ ����
BOOL CTrayIconMng::DelTrayIcon(HWND hWnd)
{
	NOTIFYICONDATA nid;

    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uFlags = NULL;
    if(Shell_NotifyIcon(NIM_DELETE, &nid)==0) //Ʈ���� ������ ����
        return FALSE;
    return TRUE;

}
