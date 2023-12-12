/******************************************************************************
          ��Ȩ���� (C), 2015-2018, �����м����ڴ�Ƽ����޹�˾
 ******************************************************************************
  �� �� ��   : cfm_extern.h
  �� �� ��   : ����
  ��    ��   : l0018898
  ��������   : 2015��10��28��
  ����޸�   :
  ��������   : 

  ��������   : UGWϵͳ֧��ƽ̨cfmģ����⣨service/product��������Ψһͷ�ļ�

  �޸���ʷ   :
  1.��    ��   : 2015��10��28��
    ��    ��   : l0018898
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef CFM_EXTERN_H
#define CFM_EXTERN_H

#include "ugw_socket.h"

//�������롢�����׼
#define IN 
#define OUT 

//����mib�У���������ֵ����󳤶�
#define MAX_MIB_NAME_LENGTH         512
#define MAX_MIB_VALUE_LENGTH        2308
#define MAX_MIB_LINE_LENGTH         (MAX_MIB_NAME_LENGTH + MAX_MIB_VALUE_LENGTH + 2) // 2byte: '='��'\n'
#define NVRAM_DEFAULT_FLAG          "default_nvram"
#define FASTNAT_SWITCH_FILE         "/proc/net/fn_switch"

typedef enum MSG_CMD
{
    MSG_CMD_TIMER_REQ = 0,              //��ʱ�¼���Ϣ֪ͨ
    MSG_CMD_TIMER_RES,
    MSG_CMD_SET_REQ,                    //������ֵ����
    MSG_CMD_SET_RES,                    //������ֵ��Ӧ
    MSG_CMD_GET_REQ,                    //��ȡ��������
    MSG_CMD_GET_RES,                    //��ȡ������Ӧ
    MSG_CMD_REG_REQ,                    //ע����Ϣ֪ͨ����
    MSG_CMD_REG_RES,                    //ע����Ϣ֪ͨ��Ӧ
    MSG_CMD_POSTMSG_REQ,                //������Ϣ����
    MSG_CMD_POSTMSG_RES,                //������Ϣ��Ӧ
    MSG_CMD_PUSHMSG_REQ,                //�ƽ���Ϣ����
    MSG_CMD_PUSHMSG_RES,                //�ƽ���Ϣ��Ӧ
    MSG_CMD_COMMIT_REQ,                 //������Ϣ����
    MSG_CMD_COMMIT_RES,                 //������Ϣ��Ӧ
    MSG_CMD_SHOW_REQ,                   //��ʾMIB����
    MSG_CMD_SHOW_RES,                   //��ʾMIB����Ӧ��
    MSG_CMD_RESTORE_REQ,                //�ָ�����MIB
    MSG_CMD_RESTORE_RES,                //�ָ�����MIB ��Ӧ
    MSG_CMD_UNSET_REQ,                  //������Ϣ��Ӧ(�Ƿ�дFLASH)
    MSG_CMD_UNSET_RES,                  //���������ֵ����
    MSG_CMD_COMMIT_NONE_RES,            //���������ֵ��Ӧ
    MSG_CMD_APMIB_SET_REQ,
    MSG_CMD_APMIB_SET_RES,
    MSG_CMD_APMIB_GET_REQ,
    MSG_CMD_APMIB_GET_RES,
    MSG_CMD_APMIB_SHOW_REQ,
    MSG_CMD_APMIB_SHOW_RES,
    MSG_CMD_APMIB_COMMIT_REQ,
    MSG_CMD_APMIB_COMMIT_RES,
    MSG_CMD_APMIB_DEFAULT_REQ,
    MSG_CMD_APMIB_DEFAULT_RES,
    MSG_CMD_COMMIT_SYNC_REQ,               //ͬ������mib��������
    MSG_CMD_COMMIT_SYNC_RES,
    MSG_CMD_APMIB_UNSET_REQ,
    MSG_CMD_APMIB_UNSET_RES,
    MSG_CMD_APMIB_LOADFILE_REQ,         // apmib���ļ�������������
    MSG_CMD_APMIB_LOADFILE_RES,         // apmib���ļ��������»�Ӧ
}MSG_CMD_ENUM;

typedef enum CFMD_FLASH_WRITE_TYPE
{
    CFMD_FLASH_WRITE_ASYNC = 0,   //�첽дflash
    CFMD_FLASH_WRITE_SYNC,        //ͬ��дflash
    CFMD_FLASH_WRITE_BUTT
}CFMD_FLASH_WRITE_TYPE_ENUM;

typedef struct CFMD_MSG_INFO
{
    int  type;
    char name[MAX_MIB_NAME_LENGTH];
    char value[MAX_MIB_VALUE_LENGTH];          
}CFMD_MSG_INFO_STRU;

/*****************************************************************************
 �� �� ��  : cfms_encode_msg
 ��������  : ��װie��Ԫ��ʽ����
 �������  : UGW_IN CFMD_MSG_INFO_STRU *SendMsg  
 �������  : UGW_OUT UGW_MSG_STRU **RespMsg
 �� �� ֵ  : �ɹ�: UGW_OK
             ʧ��: UGW_ERR
 
 �޸���ʷ      :
  1.��    ��   : 2016��4��1��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM cfms_encode_msg(UGW_OUT UGW_MSG_STRU **RespMsg, UGW_IN CFMD_MSG_INFO_STRU *SendMsg);

/*****************************************************************************
 �� �� ��  : cfms_decode_msg
 ��������  : ����cfms ie��Ԫ��ʽ����
 �������  : UGW_IN UGW_MSG_STRU *RcvMsg          
 �������  : UGW_OUT CFMD_MSG_INFO_STRU *CfmdMsg
 �� �� ֵ  : �ɹ�: UGW_OK
             ʧ��: UGW_ERR
 
 �޸���ʷ      :
  1.��    ��   : 2016��4��1��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM cfms_decode_msg(UGW_IN UGW_MSG_STRU *RcvMsg, UGW_OUT CFMD_MSG_INFO_STRU *CfmdMsg);

/*****************************************************************************
 �� �� ��  : cfms_proc_send_msg
 ��������  : cfms����ȷ�����ݰ�
 �������  : int sock           
             UGW_MSG_STRU *Msg  
 �������  : ��
 �� �� ֵ  : �ɹ�: UGW_OK
             ʧ��: UGW_ERR
 
 �޸���ʷ      :
  1.��    ��   : 2016��4��1��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM cfms_proc_send_msg(int sock, UGW_MSG_STRU *Msg);

/*************************************************
Function:     GetValue
Description:  ��mib���У���ȡ��ֵ
Input:
  name, ������Ҫ��ȡ�ַ���������

Output:
  value, �ṩ�����ַ����Ĵ�ŵ�ַ

Return:
1�� �ɹ�
0�� ʧ�� 
Others:
*************************************************/
int GetValue(IN const char *name, OUT char *value);

/*************************************************
Function:     SetValue
Description:  ������ֵ��mib����
Input:
  name, ������Ҫ������ַ�������
  value, ������Ҫ����ֵ

Output:
  ��

Return:
1�� �ɹ�
0�� ʧ�� 
Others:
*************************************************/
int SetValue(IN const char *name, IN char *value);


/*************************************************
Function:     UnSetValue
Description:  ɾ��mib�е�ĳ����ֵ
Input:
  name, ������Ҫɾ�����ַ�������

Output:
  ��

Return:
1�� �ɹ�
0�� ʧ�� 
Others:
*************************************************/
int UnSetValue(IN char *name);



/*************************************************
Function:     ShowValue
Description:  ��ӡ���е�mib����ֵ��ָ���ļ�
Input:
  file, ��ֵ�����ָ�����ļ�����

Output:
  ��

Return:
1�� �ɹ�
0�� ʧ�� 
Others:
*************************************************/
int ShowValue(char *file);

/*************************************************
Function:     RestoreValue
Description:  ������е�ǰ��ز������ָ�������Ĭ������
Input:
 ��
Output:
 �� 

Return:
1�� �ɹ�
0�� ʧ�� 
Others:
*************************************************/
int RestoreValue();


/*************************************************
Function:     CommitCfm
Description:  ֪ͨCFM���������Flash
Input:
 �� 
Output:
 �� 

Return:
1�� �ɹ�
0�� ʧ�� 
Others:
*************************************************/
int CommitCfm();

/*****************************************************************************
 �� �� ��  : CommitSyncCfm
 ��������  : ����ͬ��дflash�ĺ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
             1�� �ɹ�
             0�� ʧ�� 
 
 �޸���ʷ      :
  1.��    ��   : 2017��7��31��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int CommitSyncCfm();


/*************************************************
Function:     PostMsg
Description:  ������Ϣ����ָ������
Input:
  AppName, ��Ϣ����
  MsgType, ��Ϣ����
Output:
 �� 

Return:
1�� �ɹ�
0�� ʧ�� 
Others:
  AppNameΪ"*"����ϵͳ֪ͨ���е�ע�����Ϣ���͵�����
*************************************************/
int PostMsg(IN char  *AppName,  IN char *MsgType);

/*************************************************
Function:     RegMsgHandle
Description:  ע��һ�����������ڽ�����CFMģ�鷢�͵����������Ϣ
Input:
  AppName, ��Ϣ����
  AppId,    Ӧ��id
  handle,  �յ���Ϣ������ص�����
Output:
 �� 

Return:
1�� �ɹ�
0�� ʧ�� 
Others:
*************************************************/
typedef int (MSGHANDLE)(char *AppName,char *MsgType);
int RegMsgHandle(IN char *AppName, IN char *MsgType, IN MSGHANDLE *handle);

/*****************************************************************************
 �� �� ��  : RestoreNvram
 ��������  : �ָ�nvramĬ������
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��8��30��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
void RestoreNvram();

/*****************************************************************************
 �� �� ��  : UploadValue
 ��������  : ���������ļ���mib
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��8��30��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int UploadValue();

int ApmibShow(void);
int ApmibCommitCfm(void);
int ApmibGetValue(IN const char *name, OUT char *value);
int ApmibSetValue(IN const char *name, IN char *value);
int ApmibUnSetValue(IN const char *name);
int ApmibDefault(void);
int ApmibLoadFile(IN const char *file);
/*****************************************************************************
 �� �� ��  : cfms_set_default_cfg
 ��������  : ���ڱ���ָ�����������Ҫͬ����mibֵ�е�����
 �������  : char *key    
             char *value  
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2017��9��5��
    ��    ��   : xujun
    �޸�����   : �����ɺ���

*****************************************************************************/
int cfms_set_default_cfg(char *key, char *value);
#endif

