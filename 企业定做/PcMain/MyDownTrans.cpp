// MyDownTrans.cpp: implementation of the CMyDownTrans class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyDownTrans.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMyDownTrans::CMyDownTrans()
{
	
}

CMyDownTrans::~CMyDownTrans()
{
	
}

void CMyDownTrans::StartWork(HWND hWnd)
{
	//���ӵ�������
	if(!Create(WM_CONNECT_DL_FILE, hWnd))
	{
		return;
	}

	while(1)
	{
		//�����ļ�������Ϣ
		INTERFILEINFO m_FileInfo = {0};
		LARGE_INTEGER nTransLen = {0};
		if(!RecvData(&m_FileInfo, sizeof(INTERFILEINFO)))
		{
			break;
		}

		//�򿪴����͵��ļ�
		m_FileInfo.m_Response = 0;
		HANDLE hFile = m_gFunc.CreateFile(m_FileInfo.m_RemoteFile, GENERIC_READ, 
			FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			m_FileInfo.m_Response = DL_FILE_LOSE;
		}
		
		//ȷ�������ļ��ĳ���
		if(m_FileInfo.m_Response == 0)
		{
			m_gFunc.GetFileSizeEx(hFile, &nTransLen);
			m_FileInfo.m_FileLen.QuadPart = nTransLen.QuadPart;
			if(m_FileInfo.m_FilePoint.QuadPart > nTransLen.QuadPart)
			{
				m_gFunc.CloseHandle(hFile);
				m_FileInfo.m_Response = DL_FILE_LEN_ERR;
			}
		}

		//ȷ�����͵��ļ�����
		if(m_FileInfo.m_Response == 0 && m_FileInfo.m_FilePoint.QuadPart > 0)
		{
			//�ƶ����ϵ�λ��
			m_gFunc.SetFilePointerEx(hFile, m_FileInfo.m_FilePoint, NULL, FILE_BEGIN);
			nTransLen.QuadPart = m_FileInfo.m_FileLen.QuadPart - m_FileInfo.m_FilePoint.QuadPart;
		}

		//���ͷ�������
		if(!SendData(&m_FileInfo, sizeof(INTERFILEINFO)))
		{
			m_gFunc.CloseHandle(hFile);
			break;
		}

		//�鿴�Ƿ���Ҫ��������
		if(m_FileInfo.m_Response != 0)
		{
			continue;
		}

		//�鿴�Ƿ���Ҫ����
		if(nTransLen.QuadPart == 0)
		{
			m_gFunc.CloseHandle(hFile);
			continue;
		}

		DWORD len = 0;
		DWORD nTmpLen = 1024 * 1024;
		DWORD nSendLen = 0;
		BYTE* pFileBuf = new BYTE[nTmpLen];
		while(nTransLen.QuadPart > 0)
		{
			len = DWORD (nTransLen.QuadPart > nTmpLen ? nTmpLen : nTransLen.QuadPart);
			m_gFunc.ReadFile(hFile, pFileBuf, len, &nSendLen, NULL);
			if(!SendData(pFileBuf, len))
			{
				break;
			}
			nTransLen.QuadPart -= len;
		}
		delete [] pFileBuf;
		m_gFunc.CloseHandle(hFile);
	}
}
