// MySocksModule.cpp: implementation of the CMySocksModule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PcSocksServer.h"
#include "MySocksModule.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MYSOCKSINFO m_gSocksInfo = {0};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMySocksModule::CMySocksModule()
{
	m_MainSocket = NULL;
}

CMySocksModule::~CMySocksModule()
{

}

SOCKET CMySocksModule::GetConnectSocket(LPCTSTR pServerAddr, UINT nServerPort)
{
	struct sockaddr_in	m_ServerAddr = {0}; 
	int			m_AddrLen = sizeof(struct sockaddr_in);
	SOCKET		m_Socket = 0;

	char m_sServerIp[MAX_PATH] = {0};
	lstrcpy(m_sServerIp, pServerAddr);
	DWORD dIp = inet_addr(m_sServerIp);
	if(dIp == INADDR_NONE)
	{
		PHOSTENT hostinfo = gethostbyname(m_sServerIp);
		if(hostinfo == NULL)
		{
			return NULL;
		}
		struct sockaddr_in dest = {0};
		CopyMemory(&(dest.sin_addr), hostinfo->h_addr_list[0], hostinfo->h_length);
		lstrcpyA(m_sServerIp, inet_ntoa(dest.sin_addr));
	}

	//��ʼ��������Ϣ
	m_ServerAddr.sin_addr.S_un.S_addr = inet_addr(m_sServerIp);
	m_ServerAddr.sin_port = htons(nServerPort); 
	m_ServerAddr.sin_family = AF_INET;

	//���������׽���
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_Socket == INVALID_SOCKET)
	{
		return NULL;
	}
	
	//���ӿͻ������
	if(connect(m_Socket, (sockaddr*) &m_ServerAddr, m_AddrLen) == SOCKET_ERROR) 
	{ 
		closesocket(m_Socket); 
		return NULL;
	}

	BOOL nodelayval = TRUE;
	if (setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &nodelayval, sizeof(BOOL)))
	{
		closesocket(m_Socket); 
		return NULL;
	}

	const int one = 1;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char *)&one, sizeof(one)))
    {
		closesocket(m_Socket); 
		return NULL;
    }

	//���ӳɹ�
	return m_Socket;
}



SOCKET CMySocksModule::GetTcpListenSocket(WORD wPort)
{
	SOCKET		Sck = NULL;	
	sockaddr_in addr = {0};
	int optval = 1000000;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(wPort);

	Sck = socket(AF_INET, SOCK_STREAM, 0);
	if(Sck==INVALID_SOCKET)
	{
		return FALSE;
	}

	if(bind(Sck, (sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR)
	{
		closesocket(Sck);
		return FALSE;
	}

	if(setsockopt(Sck, SOL_SOCKET, SO_SNDTIMEO, (char*) &optval, sizeof(optval)) == SOCKET_ERROR)
	{
		closesocket(Sck);
		return FALSE;
	}

	if(setsockopt(Sck, SOL_SOCKET, SO_RCVTIMEO, (char*) &optval, sizeof(optval))	== SOCKET_ERROR)
	{
		closesocket(Sck);
		return FALSE;
	}

	if(listen(Sck, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(Sck);
		return FALSE;
	}
	return Sck;
}

void CMySocksModule::DoTcpSendRecvThread(LPVOID lPvoid)
{
	TCPSENDRECVINFO m_Info = {0};
	memcpy(&m_Info, lPvoid, sizeof(TCPSENDRECVINFO));
	delete (LPTCPSENDRECVINFO) lPvoid;

	struct fd_set sset = {0};
	struct timeval waited = {0};
	waited.tv_sec = 30000000;
	waited.tv_usec= 0;
	int ret = 0;
	BYTE* m_Buf = new BYTE[262144];
	while(WaitForSingleObject(m_gSocksInfo.hExitEvent, 0) == WAIT_TIMEOUT)
	{
		//�ȴ�����������
		FD_ZERO(&sset);
		FD_SET(m_Info.sRecv, &sset);
		if(select(m_Info.sRecv + 1, &sset, NULL, NULL, &waited) <= 0)
		{
			break;
		}

		//���շ���������
		ret = recv(m_Info.sRecv, (char*) m_Buf, 262144, 0);
		if(ret <= 0)
		{
			break;
		}

		send(m_Info.sSend, (char*) m_Buf, ret, 0);
	}
	delete [] m_Buf;
	closesocket(m_Info.sRecv);
	closesocket(m_Info.sSend);
}

void CMySocksModule::SSH_ProxyMainSocksThread(LPVOID lPvoid)
{
	DWORD	m_ProxyTypes = 0;
	char	m_ServerAddr[MAX_PATH] = {0};
	WORD	m_ServerPort = {0};
	BOOL	m_IsUdp = FALSE;
	SOCKET	m_ClientSocket = (SOCKET) lPvoid;
	SOCKET	m_LocalSocket = NULL;

	//Э�̰�ͷ
	char* m_DataBuf = new char[65535];
	int ret = recv(m_ClientSocket, m_DataBuf, 2, 0);
	if(ret != 2)
	{
		delete [] m_DataBuf;
		closesocket(m_ClientSocket);
		return;
	}
	
	if(m_DataBuf[0] == 0x04 && m_DataBuf[1] == 0x01)
	{
		//SOCKS4
		m_ProxyTypes = PS_PROXY_SOCKS4;
		ret = recv(m_ClientSocket, m_DataBuf + 2, 255, 0);
		if(ret <= 0)
		{
			delete [] m_DataBuf;
			closesocket(m_ClientSocket);
			return;
		}

		if(m_DataBuf[4] == 0 && m_DataBuf[5] == 0 && m_DataBuf[6] == 0 && m_DataBuf[7] != 0)
		{
			//����
			char* pFind = &m_DataBuf[7];
			int nLen = lstrlenA(pFind);
			pFind += nLen + 1;
			lstrcpyA(m_ServerAddr, pFind); 
		}
		else
		{
			//IP
			struct in_addr addr = {0}; 
			CopyMemory(&addr.s_addr, m_DataBuf + 4, 4); 
			lstrcpyA(m_ServerAddr, inet_ntoa(addr)); 
		}
		m_ServerPort = ntohs(*(unsigned short*)(m_DataBuf + 2)); 
	}
	else if(m_DataBuf[0] == 0x05)
	{
		//SOCKS5
		m_ProxyTypes = PS_PROXY_SOCKS5;

		//���ܷ�������,���˵�
		ret = recv(m_ClientSocket, m_DataBuf, m_DataBuf[1], 0);
		if(ret != m_DataBuf[1])
		{
			delete [] m_DataBuf;
			closesocket(m_ClientSocket);
			return;
		}

		if(lstrlenA(m_gSocksInfo.m_User) != 0 || lstrlenA(m_gSocksInfo.m_Pass))
		{
			//���ͷ��ذ�(��Ҫ�����֤)
			m_DataBuf[0] = 0x05;
			m_DataBuf[1] = 0x02;
			send(m_ClientSocket, m_DataBuf, 2, 0);

			//��ʶ
			ret = recv(m_ClientSocket, m_DataBuf, 1, 0);
			if(ret != 1 || m_DataBuf[0] != 0x01)
			{
				delete [] m_DataBuf;
				closesocket(m_ClientSocket);
				return;
			}

			//�û�������
			char ch = 0;
			ret = recv(m_ClientSocket, &ch, 1, 0);
			if(ret != 1)
			{
				delete [] m_DataBuf;
				closesocket(m_ClientSocket);
				return;
			}

			//�û���
			ret = recv(m_ClientSocket, m_DataBuf, ch, 0);
			if(ret != ch)
			{
				delete [] m_DataBuf;
				closesocket(m_ClientSocket);
				return;
			}
			m_DataBuf[ch] = 0x00;
			if(StrCmp(m_gSocksInfo.m_User, m_DataBuf) != 0)
			{
				m_DataBuf[0] = 0x01;
				m_DataBuf[1] = 0x03;
				send(m_ClientSocket, m_DataBuf, 2, 0);
				closesocket(m_ClientSocket);
				delete [] m_DataBuf;
				return;
			}
				
			//���볤��
			ch = 0;
			ret = recv(m_ClientSocket, &ch, 1, 0);
			if(ret != 1)
			{
				delete [] m_DataBuf;
				closesocket(m_ClientSocket);
				return;
			}

			//����
			ret = recv(m_ClientSocket, m_DataBuf, ch, 0);
			if(ret != ch)
			{
				delete [] m_DataBuf;
				closesocket(m_ClientSocket);
				return;
			}
			m_DataBuf[ch] = 0x00;
			if(StrCmp(m_gSocksInfo.m_Pass, m_DataBuf) != 0)
			{
				m_DataBuf[0] = 0x01;
				m_DataBuf[1] = 0x03;
				send(m_ClientSocket, m_DataBuf, 2, 0);
				closesocket(m_ClientSocket);
				delete [] m_DataBuf;
				return;
			}
			m_DataBuf[0] = 0x01;
			m_DataBuf[1] = 0x00;
			send(m_ClientSocket, m_DataBuf, 2, 0);
		}
		else
		{
			//���ͷ��ذ�(����Ҫ������֤)
			m_DataBuf[0] = 0x05;
			m_DataBuf[1] = 0;
			send(m_ClientSocket, m_DataBuf, 2, 0);
		}

		//������Э�̰�
		ret = recv(m_ClientSocket, m_DataBuf, 4, 0);
		if(ret != 4)
		{
			delete [] m_DataBuf;
			closesocket(m_ClientSocket);
			return;
		}

		//ȡ��������ַ��Ϣ
		SOCKETRECVINFO m_RecvInfo = {0};
		memcpy(&m_RecvInfo, m_DataBuf, sizeof(SOCKETRECVINFO));
		if(m_RecvInfo.m_CMD == 0x01 || m_RecvInfo.m_CMD == 0x03)
		{
			//TCP����
			if(m_RecvInfo.m_ATYP == 0x01)
			{
				//IP��ַ
				ret = recv(m_ClientSocket, m_DataBuf, 6, 0);
				if(ret != 6)
				{
					delete [] m_DataBuf;
					closesocket(m_ClientSocket);
					return;
				}

				struct in_addr addr = {0}; 
				memcpy((char* )&addr.s_addr, m_DataBuf, 4); 
				strcpy(m_ServerAddr, inet_ntoa(addr)); 
				m_ServerPort = ntohs(*(unsigned short*)(m_DataBuf + 4)); 
			}
			else if(m_RecvInfo.m_ATYP == 0x03)
			{
				//����
				char len = 0;
				ret = recv(m_ClientSocket, &len, 1, 0);
				if(ret != 1)
				{
					delete [] m_DataBuf;
					closesocket(m_ClientSocket);
					return;
				}
				ret = recv(m_ClientSocket, m_DataBuf, len + 2, 0);
				if(ret != len + 2)
				{
					delete [] m_DataBuf;
					closesocket(m_ClientSocket);
					return;
				}
				memcpy(m_ServerAddr, m_DataBuf, len); 
				m_ServerAddr[len] = 0; 
				m_ServerPort = ntohs(*(unsigned short*)(m_DataBuf + len)); 
			}
			else
			{
				//����Э��,��֧��
				delete [] m_DataBuf;
				closesocket(m_ClientSocket);
				return;
			}

			if(m_RecvInfo.m_CMD == 0x03)
			{
				m_IsUdp = TRUE;
			}
		}
		else
		{
			//δ֪����
			delete [] m_DataBuf;
			closesocket(m_ClientSocket);
			return;
		}
	}
	else
	{
		//δ֪����
		delete [] m_DataBuf;
		closesocket(m_ClientSocket);
		return;
	}

	if(!m_IsUdp)
	{
		sockaddr_in m_addr = {0};
		m_addr.sin_family = AF_INET;
		int	addrlen = sizeof(sockaddr_in);

		//����Ŀ�Ļ���
		m_LocalSocket = GetConnectSocket(m_ServerAddr, m_ServerPort);
		if(m_LocalSocket != NULL)
		{
			getsockname(m_LocalSocket, (sockaddr*) &m_addr, &addrlen);
		}
		
		if(m_ProxyTypes == PS_PROXY_SOCKS5)
		{
			//���SOCKS5���ذ�
			m_DataBuf[0] = 0x05;
			if(m_LocalSocket== NULL)
			{
				m_DataBuf[1] = 0x03;
			}
			else
			{
				m_DataBuf[1] = 0x00;
			}
			m_DataBuf[2] = 0x00;
			m_DataBuf[3] = 0x01;
			memcpy(m_DataBuf + 4, &m_addr.sin_addr, sizeof(in_addr));
			*((WORD*) (m_DataBuf + 4 + sizeof(in_addr))) = m_addr.sin_port;
			send(m_ClientSocket, m_DataBuf, 10, 0);
		}
		else if(m_ProxyTypes == PS_PROXY_SOCKS4)
		{
			//���SOCKS4���ذ�
			ZeroMemory(m_DataBuf, sizeof(m_DataBuf));
			m_DataBuf[0] = 0x00;
			if(m_LocalSocket== NULL)
			{
				m_DataBuf[1] = 91;
			}
			else
			{
				m_DataBuf[1] = 90;
			}
			*((WORD*) (m_DataBuf + 2)) = m_addr.sin_port;
			CopyMemory(m_DataBuf + 4, &m_addr.sin_addr, sizeof(in_addr));
			send(m_ClientSocket, m_DataBuf, 8, 0);
		}

		//���Э�̻���
		delete [] m_DataBuf;
		if(m_LocalSocket == NULL)
		{
			closesocket(m_ClientSocket);
			return;
		}

		//��������
		LPTCPSENDRECVINFO pInfo = new TCPSENDRECVINFO;
		pInfo->sSend = m_LocalSocket;
		pInfo->sRecv = m_ClientSocket;
		_beginthread(DoTcpSendRecvThread, 0, (LPVOID) pInfo);

		pInfo = new TCPSENDRECVINFO;
		pInfo->sSend = m_ClientSocket;
		pInfo->sRecv = m_LocalSocket;
		_beginthread(DoTcpSendRecvThread, 0, (LPVOID) pInfo);
	}
	else
	{
		//�ͻ�������IP��ַ�Ͷ˿�
		sockaddr_in m_ClientAddr = {0};
		int	addrlen = sizeof(sockaddr_in);
		m_ClientAddr.sin_family = AF_INET;
		getpeername(m_ClientSocket, (sockaddr*) &m_ClientAddr, &addrlen);
		m_ClientAddr.sin_port = htons(m_ServerPort);

		//��ȡ������UDP�����׽���
		WORD m_UdpPort = 0;
		SOCKET m_LocalSocket = GetUdpListenSocket(&m_UdpPort);

		//��ȡ����IP��ַ�Ͷ˿�
		sockaddr_in m_LocalAddr = {0};
		m_LocalAddr.sin_family = AF_INET;
		getsockname(m_ClientSocket, (sockaddr*) &m_LocalAddr, &addrlen);
					
		//���SOCKS5���ذ�
		m_DataBuf[0] = 0x05;
		if(m_LocalSocket== NULL)
		{
			m_DataBuf[1] = 0x03;
		}
		else
		{
			m_DataBuf[1] = 0x00;
		}
		m_DataBuf[2] = 0x00;
		m_DataBuf[3] = 0x01;
		memcpy(m_DataBuf + 4, &m_LocalAddr.sin_addr, 4);
		*((WORD*) (m_DataBuf + 4 + sizeof(in_addr))) = htons(m_UdpPort);
		send(m_ClientSocket, m_DataBuf, 10, 0);
		delete [] m_DataBuf;

		if(m_LocalSocket == NULL)
		{		
			closesocket(m_ClientSocket);
			return;
		}

		LPUDPSENDRECVINFO pInfo = new UDPSENDRECVINFO;
		memcpy(&pInfo->m_ClientAddr, &m_ClientAddr, sizeof(sockaddr_in));
		pInfo->m_LocalSocket = m_LocalSocket;
		_beginthread(DoUdpSendRecvThread, 0, (LPVOID) pInfo);

		//��ʼUDPת��
		while(1)
		{
			char ch = 0;
			int ret = recv(m_ClientSocket, &ch, 1, 0);
			if(ret <= 0)
			{
				break;
			}
		}
		closesocket(m_LocalSocket);
		closesocket(m_ClientSocket);
	}
}

void CMySocksModule::DoUdpSendRecvThread(LPVOID lPvoid)
{
	UDPSENDRECVINFO m_Info = {0};
	memcpy(&m_Info, lPvoid, sizeof(UDPSENDRECVINFO));
	delete (LPUDPSENDRECVINFO) lPvoid;

	char* m_RecvBuf = new char[PS_PROXY_BUFLEN + 1];
	while(WaitForSingleObject(m_gSocksInfo.hExitEvent, 0) == WAIT_TIMEOUT)
	{
		sockaddr_in m_recvaddr = {0};
		m_recvaddr.sin_family = AF_INET;
		int	addrlen = sizeof(sockaddr_in);
		int ret = recvfrom(m_Info.m_LocalSocket, m_RecvBuf, PS_PROXY_BUFLEN, 0, (sockaddr*) &m_recvaddr, &addrlen);
		if(ret <= 0)
		{
			break;
		}

		if(memcmp(&m_recvaddr.sin_addr, &m_Info.m_ClientAddr.sin_addr, sizeof(in_addr)) == 0)
		{
			char* pCurPoint = m_RecvBuf;

			//���Կͻ���
			pCurPoint += 3;

			sockaddr_in m_sendaddr = {0};
			m_sendaddr.sin_family = AF_INET;
			int	addrlen = sizeof(sockaddr_in);
			
			//TCP����
			if(*pCurPoint == 0x01)
			{
				pCurPoint ++;

				//IP��ַ
				memcpy((char*) &m_sendaddr.sin_addr, pCurPoint, 4);
				pCurPoint += 4;

				//�˿�
				m_sendaddr.sin_port = *((WORD*) pCurPoint);
				pCurPoint += 2;
			}
			else if(*pCurPoint == 0x03)
			{
				pCurPoint ++;

				//��������
				char len = *pCurPoint;
				pCurPoint ++;

				//����
				char m_ServerAddr[256] = {0};
				memcpy(m_ServerAddr, pCurPoint, len);
				pCurPoint += len;

				PHOSTENT hostinfo = gethostbyname(m_ServerAddr);
				if(hostinfo == NULL)
				{
					continue;
				}
				memcpy(&m_sendaddr.sin_addr, hostinfo->h_addr_list[0], hostinfo->h_length);
				
				//�˿�
				m_sendaddr.sin_port = *((WORD*) pCurPoint);
				pCurPoint += 2;
			}
			else
			{
				//��֧�ֵ�Э��
				continue;
			}

			//���͵�Զ�˷�����
			int ret1 = sendto(m_Info.m_LocalSocket, pCurPoint, ret - (pCurPoint - m_RecvBuf), 
				0, (sockaddr*) &m_sendaddr, sizeof(sockaddr_in));

			if(ret1 <= 0)
			{
				TRACE("\nret1 = %d\n", ::WSAGetLastError());
			}

			TRACE("\n�ͻ��˷�������\n");
			for(int i = 0; i < ret; i++)
			{
				TRACE("%02x,", (BYTE) m_RecvBuf[i]);
			}
		}
		else
		{
			char* m_SendBuf = new char[ret + 10];
			
			char* pCurPoint = m_SendBuf;
			pCurPoint += 3;
			
			*pCurPoint = 0x01;
			pCurPoint ++;

			//��ַ
			memcpy(pCurPoint, &m_recvaddr.sin_addr, 4);
			*pCurPoint += 4;

			//�˿�
			*((WORD*) pCurPoint) = m_recvaddr.sin_port;
			pCurPoint += 2;

			//ʵ������
			memcpy(pCurPoint, m_RecvBuf, ret);

			//���͵��ͻ���
			sendto(m_Info.m_LocalSocket, m_SendBuf, ret + 10, 
				0, (sockaddr*) &m_Info.m_ClientAddr, sizeof(sockaddr_in));
			delete [] m_SendBuf;

			TRACE("\n��������������\n");
			for(int i = 0; i < ret; i++)
			{
				TRACE("%02x,", (BYTE) m_RecvBuf[i]);
			}
		}
	}
	delete [] m_RecvBuf;
	closesocket(m_Info.m_LocalSocket);
}

SOCKET CMySocksModule::GetUdpListenSocket(WORD* wPort)
{
	//�����׽���
	SOCKET Sck = socket(AF_INET, SOCK_DGRAM, 0);
	if(Sck==INVALID_SOCKET)
	{
		return NULL;
	}

	//���׽���
	for(int i = 1025; i < 65534; i++)
	{
		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(i);

		if(bind(Sck, (sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR)
		{
			closesocket(Sck);
			continue;
		}
		else
		{
			*wPort = i;
			return Sck;
		}
	}

	closesocket(Sck);
	return NULL;
}

void CMySocksModule::SSH_ProxyListenThreadSocks(LPVOID lPvoid)
{
	SOCKET sMainSocket = (SOCKET) lPvoid;
	SOCKET sAccept = NULL;
	while(1)
	{
		//�ȴ��ͻ�����
		if((sAccept = accept(sMainSocket, 0, 0)) == INVALID_SOCKET) 
		{
			break;
		}
		_beginthread(SSH_ProxyMainSocksThread, 0, (LPVOID) sAccept);
	}
}

BOOL CMySocksModule::StartWork(LPMYSOCKSINFO pSocketInfo)
{
	CopyMemory(&m_gSocksInfo, pSocketInfo, sizeof(MYSOCKSINFO));
	
	//�����˿�
	m_MainSocket = GetTcpListenSocket(m_gSocksInfo.m_Port);
	if(m_MainSocket == NULL)
	{
		return FALSE;
	}

	//��ֹ�¼�
	m_gSocksInfo.hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	//���������߳�
	_beginthread(SSH_ProxyListenThreadSocks, 0, (LPVOID) m_MainSocket);

	return TRUE;
}

void CMySocksModule::StopWork()
{
	if(m_MainSocket != NULL)
	{
		closesocket(m_MainSocket);
		m_MainSocket = NULL;
	}

	if(m_gSocksInfo.hExitEvent != NULL)
	{
		CloseHandle(m_gSocksInfo.hExitEvent);
		m_gSocksInfo.hExitEvent = NULL;
	}
}
