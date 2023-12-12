/******************************************************************************
          ��Ȩ���� (C), 2015-2018, �����м����ڴ�Ƽ����޹�˾
 ******************************************************************************
  �� �� ��   : monitor_extern.h
  �� �� ��   : ����
  ��    ��   : zzh
  ��������   : 2015��12��4��
  ����޸�   :
  ��������   : 

  ��������   : monitor���̶���ӿں��������Լ���Ϣ����

  �޸���ʷ   :
  1.��    ��   : 2015��12��4��
    ��    ��   : zzh
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef MONITOR_EXTERN_H
#define MONITOR_EXTERN_H

#include <unistd.h>
#include "common_extern.h"
#include "ugw_socket.h"

#define MONITOR_CMDLINE_LEN 256
#define MONITOR_PROGRAM_NAME_LEN 64
#define MONITOR_PATH_LENGTH 64

typedef enum MONITOR_HANDLE_MSG_TYPE
{
    MONITOR_HANDLE_TIMER = 0,
    MONITOR_HANDLE_NETWORK_SUCCESS = 1,
    MONITOR_HANDLE_PROGRAM_START,
    MONITOR_HANDLE_PROGRAM_STOP,
    MONITOR_HANDLE_PROGRAM_RESTART,
    MONITOR_HANDLE_PROGRAM_SYSTEM,    //ִ������system����
    MONITOR_GET_PROGRAM_PID_BY_NAME,
    MONITOR_SHOW_PROGRAM_INFO,
    MONITOR_CHECK_PROGRAM_INFO,
    MONITOR_HANDLE_SUCCESS,   //11
    MONITOR_HANDLE_FAILURE,   //12
    MONITOR_HANDLE_RELEASE_RELEASETIONSHIP,
    MONITOR_HANDLE_BUTT
}MONITOR_HANDLE_MSG_TYPE_ENUM;

typedef struct MONITOR_MSG_INFO
{
    int type;           //��Ϣ����
    char cmdline[MONITOR_CMDLINE_LEN];
    char program_name[MONITOR_PROGRAM_NAME_LEN];
    char path[MONITOR_PATH_LENGTH];
    pid_t program_pid;
    int proctype;       //��������
    int pid_exist;      //�����ֶΣ�Ŀǰ��Ϊ���̲�ѯ�ķ���ֵ��0�����ڣ�1����      
}MONITOR_MSG_INFO_STRU;

/*****************************************************************************
 �� �� ��  : monitor_decode_msg
 ��������  : ie��ʽ���ݽ��뺯��
 �������  : UGW_IN UGW_MSG_STRU *RcvMsg                
 �������  : UGW_OUT MONITOR_MSG_INFO_STRU *MonitorMsg  
 �� �� ֵ  : �ɹ�: UGW_OK
             ʧ��: UGW_ERR
 
 �޸���ʷ      :
  1.��    ��   : 2016��3��31��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM monitor_decode_msg(UGW_IN UGW_MSG_STRU *RcvMsg, UGW_OUT MONITOR_MSG_INFO_STRU *MonitorMsg);

/*****************************************************************************
 �� �� ��  : monitor_encode_msg
 ��������  : monitor������װie��ʽ���ݺ���
 �������  : UGW_IN MONITOR_MSG_INFO_STRU *SendMsg  
 �������  : UGW_OUT UGW_MSG_STRU **RespMsg   
 �� �� ֵ  : �ɹ�: UGW_OK
             ʧ��: UGW_ERR
             
 �޸���ʷ      :
  1.��    ��   : 2016��3��31��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM monitor_encode_msg(UGW_OUT UGW_MSG_STRU **RespMsg, UGW_IN MONITOR_MSG_INFO_STRU *SendMsg);

/*****************************************************************************
 �� �� ��  : monitor_proc_send_msg
 ��������  : ��ָ�����׽��ַ�������
 �������  : int sock           
             UGW_MSG_STRU *Msg  
 �������  : ��
 �� �� ֵ  : �ɹ�: UGW_OK
             ʧ��: UGW_ERR
 
 �޸���ʷ      :
  1.��    ��   : 2016��3��31��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM monitor_proc_send_msg(int sock, UGW_MSG_STRU *Msg);

//����cmdlineָ���Ľ��̣�����һ�����̹ҵ�monitor���Զ������ý���
int monitor_mstart(char *cmdline);

//����cmdlineָ���Ľ��̣������Ľ��̹ҵ�������������
int monitor_start(char *cmdline);
//�����������̣�������̴��ھ�kill����Ӧ�Ľ��̣����
//���̲����ھ�ֱ�������ý���
int monitor_restart(char *cmdline);
//ֹͣ����
int monitor_stop(char *program_name);
//���øýӿ��൱�ڵ�����system����
int monitor_system(char *cmdline);
//ͨ����������ȡ��Ӧ���̵�pid
int monitor_program_get_pid(char *program_name);
//��ӡ���м�ؽ�����Ϣ
int monitor_program_show();
/*�����ʼ��ok����ƽ̨����ģ�鳣פ����*/
int monitor_system_network_ok();

/*��ѯĳһ�����Ƿ���ڣ�0�����ڣ�1����*/
int monitor_check_exist(const char *program_name);

//����cmdlineָ���Ľ��̣�����һ�����̹ҵ��ͻ�֪ͨ����ý��̵�ģ�飬�������������ý���
//����multiwan��������wan�������
int monitor_start_extern(char *cmdline);

int monitor_release_relationship(const char *program_name);
#endif
