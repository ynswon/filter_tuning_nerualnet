#pragma once

// CTrayIconMng ��� ����Դϴ�.

class CTrayIconMng : public CObject
{
public:
	CTrayIconMng();
	virtual ~CTrayIconMng();
	// Ʈ���� �������� �����Ǿ����� ����
	bool m_bAdded;
	// ���̾�αװ� ������ �������� ����
	bool m_bHide;
	// �˾��޴� ����
	void MakePopupMenu(HWND hWnd, int x, int y);
	// �˾��޴��� �̺�Ʈ �߻��� ó�� �Լ�
	void ProcTrayMsg(HWND hWnd, WPARAM wParam, LPARAM lParam);
	// Ʈ���� ������ ����
	BOOL AddTrayIcon(HWND hWnd);
	// Ʈ���� ������ ����
	BOOL DelTrayIcon(HWND hWnd);
};


