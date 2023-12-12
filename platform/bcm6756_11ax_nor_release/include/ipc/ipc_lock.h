#ifndef LIB_DBAPI_IPC_LOCK_H
#define LIB_DBAPI_IPC_LOCK_H

/******************************************************************************

  Copyright (C), 1999-2017, Tenda Tech Co., Ltd.

 ******************************************************************************
  File Name     : lock.h
  Version       : Initial Draft
  Author        : zengfanfan
  Created       : 2017-7-31
  Last Modified :
  Description   : lock mechanism between processes
  Function List :
  History       :

******************************************************************************/

/*
 * db_ipc_lock - ����
 * @name: Ҫ����������Ψһ����
 * @...: like printf
 *
 * ������ѱ�ռ��, ����������, ֱ������ռ�����ͷ�
 * ע�����Ϊ����������
 *
 * return: 1-ok, 0-fail
 */
int db_ipc_lock(char *name, ...);

/*
 * db_ipc_lock_nowait - ���� (������)
 * @name: Ҫ����������Ψһ����
 * @...: like printf
 *
 * ������ѱ�ռ��, ֱ�ӷ���ʧ��(0), ��������
 *
 * return: 1-ok, 0-fail
 */
int db_ipc_lock_nowait(char *name, ...);

/*
 * db_ipc_lock_timeout - ���� (�����ó�ʱ)
 * @seconds: ��ʱʱ��/��
 * @name: Ҫ����������Ψһ����
 * @...: like printf
 *
 * ������ѱ�ռ��, ����������, ֱ������ռ�����ͷ�, ��ʱ
 *
 * return: 1-ok, 0-fail
 */
int db_ipc_lock_timeout(int seconds, char *name, ...);

/*
 * db_ipc_lock - ����
 * @name: Ҫ����������Ψһ����
 * @...: like printf
 *
 * return: 1-ok, 0-fail
 */
int db_ipc_unlock(char *name, ...);

int db_fd_rlock(int fd);
int db_fd_wlock(int fd);
int db_fd_unlock(int fd);

#endif // LIB_DBAPI_IPC_LOCK_H

