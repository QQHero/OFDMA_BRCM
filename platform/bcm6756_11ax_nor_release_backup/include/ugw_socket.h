/******************************************************************************
          ��Ȩ���� (C), 2015-2018, �����м����ڴ�Ƽ����޹�˾
 ******************************************************************************
  �� �� ��   : ugw_socket.h
  �� �� ��   : ����
  ��    ��   : zzh
  ��������   : 2015��12��14��
  ����޸�   :
  ��������   : 

  ��������   : ugwƽ̨ͨ�Ż��ƹ���ͷ�ļ�����

  �޸���ʷ   :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef UGW_SOCKET_H
#define UGW_SOCKET_H
#include <sys/un.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "common_extern.h"

#define MAX_EPOLL_EVENTS            10
#define MAX_EPOLL_LISTEN_NUM        32

#define TIMER_SOCKET_PATH           "/var/tm_socket"
#define CFMD_SOCKET_PATH            "/var/cfm_socket"
#define NETCTRL_SOCKET_PATH         "/var/nt_socket"
#define MULTIWAN_SOCKET_PATH        "/var/mw_socket"
#define PMONITOR_SOCKET_PATH        "/var/pm_socket"
#define PLOG_SOCKET_PATH            "/var/plog_socket"
#define NET_CK_SOCKET_PATH          "/var/network_ck_socket"

#define UGW_MSG_HEADER  \
        int Length;     /*��Ϣ��ĳ���*/ 

#define UGW_MSG_HEADER_LEN              sizeof(UGW_MSG_STRU)

typedef int (*ugw_socket_msg_handler)(int sock);

typedef struct UGW_MSG
{
    UGW_MSG_HEADER
    char Body[0];
}UGW_MSG_STRU;

typedef struct UGW_SOCKET_FD
{
    int sock;
    ugw_socket_msg_handler fn;        /*�׽����հ�������*/     
}UGW_SOCKET_FD_STRU;

/*****************************************************************************
 �� �� ��  : ugw_proc_msg_register
 ��������  : ����ע���׽�����Ϣ������
 �������  : UGW_SOCKET_FD_STRU *pFdSocket        
             int ProcessId                            
             ugw_socket_msg_handler pMsgHandle  �ص�����
 �������  : ��
 �� �� ֵ  : 0�ɹ�,1ʧ��
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM ugw_proc_msg_register(UGW_SOCKET_FD_STRU *pFdSocket, 
                                           const char *path, 
                                           ugw_socket_msg_handler pMsgHandle);

/*****************************************************************************
 �� �� ��  : ugw_proc_msg_unregister
 ��������  : ע����Ϣ������
 �������  : UGW_SOCKET_FD_STRU *pFdSocket  
             int ProcessId                        
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_VOID ugw_proc_msg_unregister(UGW_SOCKET_FD_STRU *pFdSocket, const char *path);

/*****************************************************************************
 �� �� ��  : ugw_proc_send_msg
 ��������  : ��Ϣ���ͺ�������
 �������  : UGW_MSG_STRU *pMsgValue  
 �������  : ��
 �� �� ֵ  : ʧ��:-1
             �ɹ�:ʵ�ʷ��͵��ֽ���
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ugw_proc_send_msg(UGW_MSG_STRU *pMsgValue, char *path);

/*****************************************************************************
 �� �� ��  : ugw_proc_recv_msg
 ��������  : �����׽������ݽӿں���
 �������  : int sock    
             UGW_MSG_STRU *pMsgValue  
 �������  : ��
 �� �� ֵ  : ʧ��:-1
             �ɹ�:ʵ�ʽ��յ��ֽ���
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ugw_proc_recv_msg(int sock, UGW_MSG_STRU **pMsgValue);

/*****************************************************************************
 �� �� ��  : ugw_connect_server
 ��������  : �ͻ����������ӷ�����
 �������  : int ProcessId  
 �������  : ��
 �� �� ֵ  : ʧ��-1���ɹ������׽���
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ugw_connect_server(const char *path);

/*****************************************************************************
 �� �� ��  : ugw_socket_shut_down
 ��������  : �ر��׽���������
 �������  : int fd  
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_VOID ugw_socket_shut_down(int fd);

/*****************************************************************************
 �� �� ��  : ugw_set_socket_timeout
 ��������  : �����׽��ַ��ͺͽ��ճ�ʱʱ��
 �������  : int sock             
             struct timeval * tv  
 �������  : ��
 �� �� ֵ  : 0�ɹ���1ʧ��
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM ugw_set_socket_timeout(int sock, struct timeval * tv);

/*****************************************************************************
 �� �� ��  : ugw_accept_client
 ��������  : �����µĿͻ�������
 �������  : int server_fd                 
             struct sockaddr_un *out_addr  
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��14��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ugw_accept_client(int server_fd, struct sockaddr_un *out_addr);

/*****************************************************************************
 �� �� ��  : ugw_socket_run_epoll
 ��������  : �׽��ּ����ͻ��˺��շ�������
 �������  : UGW_SOCKET_FD_STRU *fd_server  
             int efd                
             struct epoll_event *events
             int i
 �������  : ��
 �� �� ֵ  : ��
 
 �޸���ʷ      :
  1.��    ��   : 2017��3��20��
    ��    ��   : luolang
    �޸�����   : �޸ĺ���

*****************************************************************************/
UGW_VOID ugw_socket_run_epoll(UGW_SOCKET_FD_STRU *fd_server, 
                        int efd, struct epoll_event *events, int i);

/*****************************************************************************
 �� �� ��  : ugw_register_poll
 ��������  : ����epoll_ctl����ļ�������
 �������  : int efd  int fd   
 �������  : ��
 �� �� ֵ  : 0��ʾע��ɹ���-1��ʾʧ��
 �޸���ʷ      :
  1.��    ��   : 2017��5��9��
    ��    ��   : luomj
    �޸�����   : �����ɺ���

*****************************************************************************/
int ugw_register_poll(int efd,int fd);

/*****************************************************************************
 �� �� ��  : ugw_fetch_epoll_events
 ��������  : ����epoll_wait����fd
 �������  : UGW_SOCKET_FD_STRU *fd_server  int efd  int timeout
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ      :
  1.��    ��   : 2017��5��9��
    ��    ��   : luomj
    �޸�����   : �����ɺ���

*****************************************************************************/

UGW_VOID ugw_fetch_epoll_events(UGW_SOCKET_FD_STRU *fd_server,int efd,int timeout);

#endif

