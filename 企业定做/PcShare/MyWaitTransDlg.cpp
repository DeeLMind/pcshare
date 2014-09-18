// MyWaitTransDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PcShare.h"
#include "MyWaitTransDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyWaitTransDlg dialog


CMyWaitTransDlg::CMyWaitTransDlg(CWnd* pParent)
	: CDialog(CMyWaitTransDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMyWaitTransDlg)
	//}}AFX_DATA_INIT
	m_TransData = new BYTE[TRAN_DATA_LEN + 65535];
	m_dTransLen = 0;
	m_Command = 0;
	hThread = NULL;
	pTran = pParent;
}

CMyWaitTransDlg::~CMyWaitTransDlg()
{
	delete [] m_TransData;
}

void CMyWaitTransDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyWaitTransDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMyWaitTransDlg, CDialog)
	//{{AFX_MSG_MAP(CMyWaitTransDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRANS_COMPLETE, OnTransComplete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyWaitTransDlg message handlers

void CMyWaitTransDlg::OnOK() 
{

}

void CMyWaitTransDlg::OnCancel() 
{
	if(MessageBox(_T("ȷ��Ҫ��ֹ���������˳������������棿"), _T("��ʾ"), MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
	{
		return;
	}

	CWaitCursor m_Cur;
	if(hThread != NULL)
	{
		WaitForSingleObject(hThread, 1000);
	}
	m_Cur.Restore();
	GetParent()->PostMessage(WM_SETMYCURSOR, (WPARAM) FALSE, 0);
	CDialog::OnCancel();
}

void CMyWaitTransDlg::ShowResponse(UINT m_Response)
{
	TCHAR m_Text[4096] = {0};
	switch(m_Response)
	{
		case INVALID_COMMAND			:	lstrcpy(m_Text, _T("��Ч����")); break;
		case CAN_NOT_OPEN_FILE			:	lstrcpy(m_Text, _T("�޷����ļ�")); break;
		case EXEC_FILE_FAIL				:	lstrcpy(m_Text, _T("Զ�����г���ʧ��")); break;
		case RENAME_FAILE				:	lstrcpy(m_Text, _T("����ʧ��")); break;
		case CREATE_DIR_FAILE			:	lstrcpy(m_Text, _T("����Ŀ¼ʧ��")); break;
		case COMMAND_PARAM_ERR			:	lstrcpy(m_Text, _T("��������")); break;
		case OPEN_REG_KEY_ERR			:	lstrcpy(m_Text, _T("�޷���ָ����ֵ")); break;
		case CAN_NOT_CREATE_TMP_FILE	: 	lstrcpy(m_Text, _T("�޷�������ʱ�ļ�")); break;
		case GET_PROCESS_LIST_ERR		:	lstrcpy(m_Text, _T("ȡ�����б�ʧ��")); break;
		case KILL_PROCESS_ERR			:	lstrcpy(m_Text, _T("��ֹ����ʧ��")); break;
		case ENUM_SERVICES_FAIL			:	lstrcpy(m_Text, _T("ȡ�����б�ʧ��")); break;
		case CONTROL_SERVICES_FAIL		:	lstrcpy(m_Text, _T("���Ʒ���ʧ��")); break;
		case DL_FILE_LOSE				:	lstrcpy(m_Text, _T("���ص��ļ�������")); break;
		case UP_FILE_FAIL				:	lstrcpy(m_Text, _T("Զ�̻����ļ��Ѿ�����,�޷������ļ�")); break;
		case REG_DELETE_KEY_ERR			:	lstrcpy(m_Text, _T("ɾ���Ӽ�ʧ��")); break;
		case REG_RENAME_VALUE_ERR		:	lstrcpy(m_Text, _T("������ֵ��ʧ��")); break;
		case REG_CREATE_VALUE_ERR		:	lstrcpy(m_Text, _T("�����¼�ֵʧ��")); break;
		case REG_EDIT_VALUE_ERR			:	lstrcpy(m_Text, _T("���ļ�ֵʧ��")); break;
		default							:	return;
	}
	MessageBox(m_Text, PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
}

LRESULT CMyWaitTransDlg::OnTransComplete(WPARAM wParam,LPARAM lParam)
{
	if(wParam == 0)
	{
		//ͨ��ʧ��
		GetParent()->PostMessage(WM_SETMYCURSOR, (WPARAM) FALSE, 0);
		MessageBox(_T("��ͻ��������Ѿ��жϣ�"), PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
		CDialog::OnCancel();
		return FALSE;
	}

	if(m_Command != 0)
	{
		ShowResponse(m_Command);
	}

	GetParent()->PostMessage(WM_SETMYCURSOR, (WPARAM) FALSE, 0);
	CDialog::OnOK();
	return TRUE;
}

BOOL CMyWaitTransDlg::ReadBag()
{
	if(m_TransData != NULL)
	{
		delete [] m_TransData;
		m_TransData = NULL;
	}
	CMyBaseWnd * pTranCwnd = (CMyBaseWnd*) pTran;
	return pTranCwnd->RecvDataWithCmd(&m_TransData, &m_dTransLen, &m_Command);
}

BOOL CMyWaitTransDlg::SendBag()
{
	CMyBaseWnd * pTranCwnd = (CMyBaseWnd*) pTran;
	return pTranCwnd->SendDataWithCmd(m_TransData, m_dTransLen, m_Command);
}

void CMyWaitTransDlg::TransDataThread(LPVOID lPvoid)
{
	CMyWaitTransDlg* pThis = (CMyWaitTransDlg*) lPvoid;
	pThis->TransData();
}

void CMyWaitTransDlg::TransData()
{
	//���ݴ���
	if(!SendBag() || !ReadBag())
	{
		//���ݴ���ʧ��
		PostMessage(WM_TRANS_COMPLETE, (WPARAM) FALSE, 0);
		return ;
	}
	PostMessage(WM_TRANS_COMPLETE, (WPARAM) TRUE, 0);
}

BOOL CMyWaitTransDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetParent()->BringWindowToTop();
	hThread = (HANDLE) _beginthread(TransDataThread, 0, this);
	MoveWindow(0, 0, 0, 0);
	CenterWindow();
	GetParent()->PostMessage(WM_SETMYCURSOR, (WPARAM) TRUE, 0);
	return TRUE;  
}


