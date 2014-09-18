// MyUnZipDlg.cpp : implementation file
//

#include "stdafx.h"
#include "UpPcShare.h"
#include "MyUnZipDlg.h"

#include "Lzw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern ULONGLONG m_CmdInfo[5];

/////////////////////////////////////////////////////////////////////////////
// CMyUnZipDlg dialog


CMyUnZipDlg::CMyUnZipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyUnZipDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyUnZipDlg)
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	pMainFileData = NULL;
	dMainFileLen = 0;

	pBinData = NULL;
	nBinDataLen = 0;

	dKeyDataLen = 0;
	pKeyDataBuf = new BYTE[PS_CUT_FILE_LEN * 2];
	memset(pKeyDataBuf, 0, PS_CUT_FILE_LEN * 2);

	memset(m_UrlPath, 0, sizeof(m_UrlPath));
	memset(sUserId, 0, sizeof(sUserId));
}

CMyUnZipDlg::~CMyUnZipDlg()
{
	if(pBinData != NULL)
	{
		delete [] pBinData;
	}

	if(pMainFileData != NULL)
	{
		delete [] pMainFileData;
	}

	if(pKeyDataBuf != NULL)
	{
		delete [] pKeyDataBuf;
	}
}

void CMyUnZipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyUnZipDlg)
	DDX_Control(pDX, IDC_PROGRESS_FILE, m_FilePos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyUnZipDlg, CDialog)
	//{{AFX_MSG_MAP(CMyUnZipDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_WORK_COMPLETE, OnWorkComplete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyUnZipDlg message handlers

void CMyUnZipDlg::OnOK() 
{

}

void CMyUnZipDlg::OnCancel() 
{
	CDialog::OnCancel();
}


BOOL CMyUnZipDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_FilePos.ShowWindow(SW_SHOW);
	m_FilePos.CenterWindow();
	SetWindowText("���ڽ�ѹ�ļ�������");

	GetMac();

	if(m_CmdInfo[1] == PS_START_NOKEY)
	{
		PostMessage(WM_WORK_COMPLETE, (WPARAM) TRUE, 0);
	}	
	else
	{
		_beginthread(GetUrlFileThread, 0, this);
	}
	return TRUE;  
}


LRESULT CMyUnZipDlg::OnWorkComplete(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 0)
	{
		MessageBox("����У���������ϵ���ߣ�", "����");
		return FALSE;
	}

	//��ѹ���������
	DWORD nReadLen = 0;
	char m_ExeFileName[MAX_PATH] = {0};
	GetModuleFileName(NULL, m_ExeFileName, 200);
	HANDLE hFile = CreateFile(m_ExeFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox("ϵͳ����", "����");
		return FALSE;
	}
	DWORD nFileLen = GetFileSize(hFile, NULL);
	BYTE* pFileData = new BYTE[nFileLen];
	ReadFile(hFile, pFileData, nFileLen, &nReadLen, NULL);
	CloseHandle(hFile);

	//���Ҵ洢�ļ���־
	BYTE* pData = NULL;
	for(DWORD i = nFileLen - sizeof(ULONGLONG); i > sizeof(ULONGLONG); i--)
	{
		if(*(ULONGLONG*) &pFileData[i] == PS_VER_ULONGLONG)
		{
			pData = &pFileData[i + sizeof(ULONGLONG)];
			break;
		}
	}
	if(pData == NULL)
	{
		delete [] pFileData;
		MessageBox("����У���������ϵ���ߣ�", "����");
		return FALSE;
	}

	//����̨�ļ�����
	DWORD dZipMainFileLen = *((DWORD*) pData);

	//����̨�ļ�
	BYTE* pZipMainFileData = new BYTE[dZipMainFileLen + PS_CUT_FILE_LEN + 65535];
	memset(pZipMainFileData, 0, dZipMainFileLen + PS_CUT_FILE_LEN + 65535);
	
	if(m_CmdInfo[1] == PS_START_NOKEY)
	{
		memcpy(pZipMainFileData, pData + sizeof(DWORD), dZipMainFileLen);
	}
	else
	{
		//����̨�ļ�ǰ�������
		memcpy(pZipMainFileData, pKeyDataBuf, PS_CUT_FILE_LEN);

		//����̨�ļ���������
		memcpy(pZipMainFileData + PS_CUT_FILE_LEN, pData + sizeof(DWORD), dZipMainFileLen);
	}

	//��ѹ�������ļ�
	FCLzw lzw;
	lzw.PcUnZip(pZipMainFileData, &pMainFileData, &dMainFileLen);
	delete [] pZipMainFileData;

	//�޸��������ļ���ʶ
	ULONGLONG* pFindFlag = NULL;
	for(i = 0; i < dMainFileLen; i++)
	{
		if(*((ULONGLONG*) &pMainFileData[i]) == PS_STARTUP_FLAG)
		{
			pFindFlag = (ULONGLONG*) &pMainFileData[i];
			break;
		}
	}
	if(pFindFlag == NULL)
	{
		delete [] pFileData;
		return FALSE;
	}

	//�󶨻���
	pFindFlag[1] = nVolumeSerialNumber;

	//��ѹ�����ļ�����
	lzw.PcUnZip(pData + sizeof(DWORD) + dZipMainFileLen, &pBinData, &nBinDataLen);
	delete [] pFileData;
	
	BYTE* pCurPoint = pBinData;

	//�ļ�����
	DWORD nFileCount = *((DWORD*) pCurPoint);
	pCurPoint += sizeof(DWORD);

	//���Ŀ¼
	char m_OldPath[256] = {0};
	memcpy(m_OldPath, pCurPoint, 255);
	pCurPoint += 255;

	//�ļ�ͷ����
	LPSAVEFILEINFO pFileInfo = (LPSAVEFILEINFO) pCurPoint;
	pCurPoint += nFileCount * sizeof(SAVEFILEINFO);

	//�ָ�����
	SetWindowText("��װ�ļ�");

	//����·��
	CXTBrowseDialog dlg;
	dlg.SetTitle(_T("ѡ��װĿ¼"));
	if(dlg.DoModal() != IDOK)
	{
		PostMessage(WM_COMMAND, IDCANCEL, 0);
		return FALSE;
	}

	//��������̨�ļ�
	char m_MainFilePath[256] = {0};
	wsprintf(m_MainFilePath, "%s\\%s\\%s.exe", dlg.GetSelPath(), sUserId, sUserId);
	MakeFilePath(m_MainFilePath);
	WriteMyFile(m_MainFilePath, pMainFileData, dMainFileLen);

	//���������ļ�
	char m_FilePath[256] = {0};
	for(i = 0; i < nFileCount; i ++)
	{
		char* pFind = strstr(pFileInfo[i].m_FilePath, m_OldPath);
		if(pFind == NULL)
		{
			pFind = pFileInfo[i].m_FilePath;
		}
		else
		{
			pFind += strlen(m_OldPath);
		}
		wsprintf(m_FilePath, "%s\\%s\\%s", dlg.GetSelPath(), sUserId, pFind);
		MakeFilePath(m_FilePath);
		WriteMyFile(m_FilePath, pCurPoint, pFileInfo[i].m_FileLen);
		pCurPoint += pFileInfo[i].m_FileLen;
	}

	//����������̨
	ShellExecute(NULL, NULL, m_MainFilePath, NULL, NULL, SW_SHOW);

	//�Լ��˳�
	PostMessage(WM_COMMAND, IDCANCEL, 0);
	return TRUE;
}

BOOL CMyUnZipDlg::GetUrlFile()
{
	/*http://www.supperpc.com/psv/*/
	m_UrlPath[0] = (char) 0x68;m_UrlPath[1] = (char) 0x74;m_UrlPath[2] = (char) 0x74;m_UrlPath[3] = (char) 0x70;m_UrlPath[4] = (char) 0x3a;m_UrlPath[5] = (char) 0x2f;m_UrlPath[6] = (char) 0x2f;m_UrlPath[7] = (char) 0x77;m_UrlPath[8] = (char) 0x77;m_UrlPath[9] = (char) 0x77;m_UrlPath[10] = (char) 0x2e;m_UrlPath[11] = (char) 0x73;m_UrlPath[12] = (char) 0x75;m_UrlPath[13] = (char) 0x70;m_UrlPath[14] = (char) 0x70;m_UrlPath[15] = (char) 0x65;m_UrlPath[16] = (char) 0x72;m_UrlPath[17] = (char) 0x70;m_UrlPath[18] = (char) 0x63;m_UrlPath[19] = (char) 0x2e;m_UrlPath[20] = (char) 0x63;m_UrlPath[21] = (char) 0x6f;m_UrlPath[22] = (char) 0x6d;m_UrlPath[23] = (char) 0x2f;m_UrlPath[24] = (char) 0x70;m_UrlPath[25] = (char) 0x73;m_UrlPath[26] = (char) 0x76;m_UrlPath[27] = (char) 0x2f;m_UrlPath[28] = 0x00;
	wsprintf(m_UrlPath + strlen(m_UrlPath), "%s.rar", sUserId);

	//��ʼ��HTTP����
	HINTERNET hIe = InternetOpen("", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(!hIe) 
	{
		return FALSE;
	}

	//����HTTP����,��������
	HINTERNET hFp = InternetOpenUrl(hIe, m_UrlPath, NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD, 0);
	if(!hFp)
	{
		InternetCloseHandle(hIe);
		return FALSE; 
	}

	//�鿴������
	char sCode[256] = {0};
	DWORD nSize = 250;
	DWORD nIndex = 0;
	if(!HttpQueryInfo(hFp, HTTP_QUERY_STATUS_CODE, sCode, &nSize, &nIndex) || lstrcmp(sCode, "200"))
	{
		InternetCloseHandle(hFp);
		InternetCloseHandle(hIe);
		return FALSE;
	}

	//�鿴������
	memset(sCode, 0, sizeof(sCode));
	nSize = 250;
	nIndex = 0;
	if(!HttpQueryInfo(hFp, HTTP_QUERY_CONTENT_LENGTH, sCode, &nSize, &nIndex))
	{
		InternetCloseHandle(hFp);
		InternetCloseHandle(hIe);
		return FALSE;
	}

	dKeyDataLen = atoi(sCode);
	m_FilePos.SetRange32(0, atoi(sCode));

	//�鿴�ļ������Ƿ�Ϸ�
	if(atoi(sCode) != PS_CUT_FILE_LEN)
	{
		InternetCloseHandle(hFp);
		InternetCloseHandle(hIe);
		return FALSE;
	}

	DWORD nFilePos = 0;
	BOOL bIsSuccess = FALSE;
	BYTE*  pDatabuf = pKeyDataBuf;
	DWORD dwNumberOfBytesReaded = 0;
	while(1)
	{
		if(!InternetReadFile(hFp, pDatabuf, 64, &dwNumberOfBytesReaded))
		{
			break;
		}
		if(nFilePos >= PS_CUT_FILE_LEN || dwNumberOfBytesReaded == 0)
		{
			bIsSuccess = TRUE; 
			break;
		}
		else
		{
			nFilePos += dwNumberOfBytesReaded;
			pDatabuf += dwNumberOfBytesReaded;
			m_FilePos.SetPos(nFilePos);
		}
	}

	InternetCloseHandle(hFp);
	InternetCloseHandle(hIe);
	return bIsSuccess;
}

void CMyUnZipDlg::GetUrlFileThread(LPVOID lPvoid)
{
	CMyUnZipDlg* pThis = (CMyUnZipDlg*) lPvoid;
	pThis->PostMessage(WM_WORK_COMPLETE, (WPARAM) pThis->GetUrlFile(), 0);
}

BOOL CMyUnZipDlg::GetMac()
{
	nVolumeSerialNumber = GetMySerialNumber();
	return TRUE;
}

void CMyUnZipDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
