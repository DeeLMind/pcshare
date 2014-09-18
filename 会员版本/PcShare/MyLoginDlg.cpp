// MyLoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PcShare.h"
#include "MyLoginDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyLoginDlg dialog

#ifndef _VIP_
DWORD nConnectCount = 0;
#endif

CMyLoginDlg::CMyLoginDlg(CWnd* pParent, BOOL IsDlFile)
	: CDialog(CMyLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyLoginDlg)
	m_User = _T("");
	m_Pass = _T("");
	//}}AFX_DATA_INIT

	m_IsExit = FALSE;
	hThread = NULL;
	hMainIe = NULL;
	hMainFile = NULL;
	m_IsDlFile = IsDlFile;

	pSystemFileData = new char[1024 * 1024];
	memset(pSystemFileData, 0, 1024 * 1024);
}

CMyLoginDlg::~CMyLoginDlg()
{
	delete [] pSystemFileData;
}

void CMyLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyLoginDlg)
	DDX_Control(pDX, IDC_STATIC_TITLE, m_Title);
	DDX_Text(pDX, IDC_EDIT_USER, m_User);
	DDV_MaxChars(pDX, m_User, 63);
	DDX_Text(pDX, IDC_EDIT_PASS, m_Pass);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CMyLoginDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LOGONEND, OnLogonEnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyLoginDlg message handlers


void CMyLoginDlg::OnOK() 
{
	UpdateData();

#ifdef _VIP_
	if(m_User.IsEmpty() || m_Pass.IsEmpty())
	{
		return;
	}

	WritePrivateProfileString(PS_SHEZHI, _T("�û�����"), m_User, GetIniFileName());
#endif

	//�ƶ�λ��
	GetWindowRect(&m_Rect);
	MoveWindow(0, 0, 0, 0);
	CenterWindow();

	GetParent()->PostMessage(WM_SETMYCURSOR, (WPARAM) TRUE, 0);

	//�����߳����ļ�
	hThread = (HANDLE) _beginthread(PsLoginThread, 0, this);
}

void CMyLoginDlg::OnCancel() 
{
	if(hThread == NULL)
	{
		CDialog::OnCancel();
		return;
	}

	if(MessageBox(_T("ȷ��Ҫ��ֹ���������˳������������棿"), _T("��ʾ"), MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
	{
		return;
	}

	m_IsExit = TRUE;
	InternetCloseHandle(hMainFile);
	InternetCloseHandle(hMainIe);
	if(hThread != NULL)
	{
		WaitForSingleObject(hThread, 1000);
		CloseHandle(hThread);
	}
	
	GetParent()->PostMessage(WM_SETMYCURSOR, (WPARAM) FALSE, 0);
	CDialog::OnCancel();
}

BOOL CMyLoginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

#ifdef _VIP_
	TCHAR sUserName[256] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("�û�����"), _T(""), sUserName, 255, GetIniFileName());
	m_User = sUserName;
	UpdateData(FALSE);
	SetWindowText(_T("PcShare��ͨ��Ա��¼"));
	if(!m_User.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_PASS)->SetFocus();
		return FALSE;
	}

#else
	m_User = "pcshare";
	m_Pass = "pcshare";
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT_PASS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(FALSE);
	SetWindowText(_T("�������ɱ����ƶˡ�����"));
	PostMessage(WM_COMMAND, (WPARAM) IDOK, 0);
	if(nConnectCount > 3)
	{
		CDialog::OnCancel();
	}

#endif
	return TRUE;  
}

void CMyLoginDlg::PsLoginThread(LPVOID lPvoid)
{
	CMyLoginDlg* pThis = (CMyLoginDlg*) lPvoid;
	pThis->PsLogin();
}

LRESULT CMyLoginDlg::OnLogonEnd(WPARAM wParam, LPARAM lParam)
{
	if(wParam == 0)
	{
		GetParent()->PostMessage(WM_SETMYCURSOR, (WPARAM) FALSE, 0);
		CDialog::OnOK();
		return TRUE;
	}

	hThread = NULL;
	TCHAR m_Text[256] = {0};
	switch(wParam)
	{
		case 1 : lstrcpy(m_Text, _T("�ʺ��Ѿ����ڣ�")); break;
		case 2 : lstrcpy(m_Text, _T("���ɴ������ޣ�ÿ��ֻ������3�Σ�ÿ��ֻ������30�Σ�ע���ʺŰ�ȫ����Ҫ�����˹����ʺţ�")); break;
		case 3 : lstrcpy(m_Text, _T("�û������������")); break;
		case 4 : lstrcpy(m_Text, _T("���ո����ļ������쳣��")); break;
		case 5 : lstrcpy(m_Text, _T("ͨ�Ŵ���")); break;
		case 6 : lstrcpy(m_Text, _T("�ʺű����ᣡ")); break;
		case 7 : lstrcpy(m_Text, _T("�ʺ��Ѿ����ڣ�")); break;
		case 8 : lstrcpy(m_Text, _T("У�����ݴ���")); break;
		default: lstrcpy(m_Text, _T("δ֪����")); break;
	}

	m_Pass = "";
	UpdateData(FALSE);

	MessageBox(m_Text, _T("��¼ʧ��"), MB_OKCANCEL|MB_ICONWARNING);
	GetParent()->PostMessage(WM_SETMYCURSOR, (WPARAM) FALSE, 0);
	MoveWindow(&m_Rect);
	return TRUE;
}

void CMyLoginDlg::PsLogin()
{
	BYTE m_RandStr[128] = {0};
	BYTE m_IntStr[128] = {0};
	srand((unsigned) time(NULL));
	for(int i = 0; i < 12; i++)
	{
		m_IntStr[i] = rand();
	}
	BcdToAsc(m_IntStr, m_RandStr, 12);

	TCHAR m_VerifyStr[256] = {0};
#ifdef UNICODE
		MultiByteToWideChar(CP_ACP, 0,	(char*) m_RandStr, lstrlenA((char*) m_RandStr) + 1, m_VerifyStr, 256);
#else
		lstrcpy(m_VerifyStr, (char*) m_RandStr);
#endif

	TCHAR m_sPortMain[100] = {0};
	if(!m_IsDlFile)
	{
		lstrcpy(m_sPortMain, _T("0"));
	}
	else
	{
		lstrcpy(m_sPortMain, _T("1"));
	}

	//����url
	TCHAR m_Url[512] = {0};

#ifdef _VIP_
	wsprintf(m_Url, _T("http://%s/pcsharevip/ip/20100310PsGetFile.asp?user=%s&pass=%s&port=%s&ver=%s"),	
			m_MainUrl, m_User, m_Pass, m_sPortMain, m_VerifyStr);
#else
	wsprintf(m_Url, _T("http://%s/pcsharefree/ip/20100410PsGetFile.asp?user=%s&pass=%s&port=%s&ver=%s"),	
			m_MainUrl, m_User, m_Pass, m_sPortMain, m_VerifyStr);
#endif

	BYTE* pData = m_RandStr + 24;
	pData[0] = 'z';
	pData[1] = 'm';
	pData[2] = 'r';

	//��ʼ��HTTP����
	HINTERNET hMainIe = InternetOpen(_T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; .NET CLR 1.1.4322)"),
						INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hMainIe == NULL) 
	{
		PostMessage(WM_LOGONEND, 5, 0);
		return;
	}

	pData[3] = 'd';
	

	//������
	HINTERNET hMainFile = InternetOpenUrl(
			hMainIe, m_Url, NULL, 0,
			INTERNET_FLAG_PRAGMA_NOCACHE|
			INTERNET_FLAG_RELOAD|
			INTERNET_FLAG_DONT_CACHE|
			INTERNET_FLAG_HYPERLINK|
			INTERNET_FLAG_NO_CACHE_WRITE|
			INTERNET_FLAG_NO_COOKIES,
			NULL);
	if(hMainFile == NULL)
	{
		InternetCloseHandle(hMainIe);
		PostMessage(WM_LOGONEND, 5, 0);
		return;
	}

	pData[4] = 'k';

	//�鿴������
	TCHAR sCode[256] = {0};
	DWORD nSize = 250;
	DWORD nIndex = 0;
	if(!HttpQueryInfo(hMainFile, HTTP_QUERY_STATUS_CODE, sCode, &nSize, &nIndex) || StrToInt(sCode) != 200)
	{
		InternetCloseHandle(hMainFile);
		InternetCloseHandle(hMainIe);
		PostMessage(WM_LOGONEND, 5, 0);
		return;
	}

	pData[5] = 'o';

	//���շ�����
	nSize = 250;
	nIndex = 0;
	TCHAR m_Response[256] = _T("Ps-Response");
	if(!HttpQueryInfo(hMainFile, HTTP_QUERY_CUSTOM , m_Response, &nSize, &nIndex))
	{
		InternetCloseHandle(hMainFile);
		InternetCloseHandle(hMainIe);
		PostMessage(WM_LOGONEND, 5, 0);
		return;
	}

	//�鿴������
	if(StrToInt(m_Response) != 0)
	{
		InternetCloseHandle(hMainFile);
		InternetCloseHandle(hMainIe);
		PostMessage(WM_LOGONEND, StrToInt(m_Response), 0);
		return;
	}

	//������֤
	if(!m_IsDlFile)
	{
		InternetCloseHandle(hMainFile);
		InternetCloseHandle(hMainIe);
		PostMessage(WM_LOGONEND, 0, 0);
		return;
	}

	//����У����
	nSize = 250;
	nIndex = 0;
	TCHAR m_RemoteVerifyCode[256] = _T("Ps-VerifyCode");
	if(!HttpQueryInfo(hMainFile, HTTP_QUERY_CUSTOM , m_RemoteVerifyCode, &nSize, &nIndex))
	{
		InternetCloseHandle(hMainFile);
		InternetCloseHandle(hMainIe);
		PostMessage(WM_LOGONEND, 5, 0);
		return;
	}

	//�Ƚ�У����
	pData[6] = 'f';
	pData[7] = 'k';
	pData[8] = 't';

	BYTE m_TmpVerifyData[24] = {0};
	GetMd5Verify(m_RandStr, lstrlenA((char*) m_RandStr), m_TmpVerifyData);

	BYTE m_aLocalVerifyCode[256] = {0};
	TCHAR m_LocalVerifyCode[256] = {0};
	BcdToAsc((BYTE*) m_TmpVerifyData, m_aLocalVerifyCode, 16);
#ifdef UNICODE
		MultiByteToWideChar(CP_ACP, 0,	(char*) m_aLocalVerifyCode, 
			lstrlenA((char*) m_aLocalVerifyCode) + 1, m_LocalVerifyCode, 256);
#else
		lstrcpy(m_LocalVerifyCode, (char*) m_aLocalVerifyCode);
#endif
	
	for(i = 0; i < 32; i++)
	{
		if(m_RemoteVerifyCode[i] != m_LocalVerifyCode[i])
		{
			//У�����ݴ���
			InternetCloseHandle(hMainFile);
			InternetCloseHandle(hMainIe);
			PostMessage(WM_LOGONEND, 8, 0);
			return;
		}
	}

	//�����ļ�����
	nSize = 250;
	nIndex = 0;
	TCHAR m_sLength[256] = _T("Ps-Length");
	if(!HttpQueryInfo(hMainFile, HTTP_QUERY_CUSTOM , m_sLength, &nSize, &nIndex))
	{
		InternetCloseHandle(hMainFile);
		InternetCloseHandle(hMainIe);
		PostMessage(WM_LOGONEND, 5, 0);
		return;
	}

	//��������
	DWORD nReadLen = StrToInt(m_sLength);
	char* pFileBuf = pSystemFileData;
	while(nReadLen > 0)
	{
		DWORD nRead = 0;
		if(!InternetReadFile(hMainFile, pFileBuf, nReadLen, &nRead))
		{
			InternetCloseHandle(hMainFile);
			InternetCloseHandle(hMainIe);
			PostMessage(WM_LOGONEND, 5, 0);
			return;
		}
		if(nRead == 0)
		{
			break;
		}
		if(m_IsExit)
		{
			InternetCloseHandle(hMainFile);
			InternetCloseHandle(hMainIe);
			return;
		}
		pFileBuf += nRead;
		nReadLen -= nRead;
	}
	InternetCloseHandle(hMainFile);
	InternetCloseHandle(hMainIe);

	//��������
	LPSYSFILEINFO pInfo = (LPSYSFILEINFO) pSystemFileData;
	if(nReadLen != 0 || pInfo->nVerifyFlag != _FLAG_SYSTEM_FILE_)
	{
		PostMessage(WM_LOGONEND, 4, 0);
	}
	else
	{
#ifdef _VIP_
		TCHAR m_Url[512] = {0};
		wsprintf(m_Url, _T("http://%s/pcsharevip/ip/20100310.htm"), m_MainUrl);
		TCHAR m_IpStr[256] = {0};
		BOOL bRet = GetLoginInfo(m_Url, m_IpStr);
		if(bRet)
		{
			WritePrivateProfileString(PS_SHEZHI, _T("�ϴθ���ʱ��"), m_IpStr, GetIniFileName());
		}
#else
		nConnectCount ++;
#endif
		PostMessage(WM_LOGONEND, 0, 0);
	}
}


