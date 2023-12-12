/******************************************************************************

  Copyright (C), 1999-2017, Tenda Tech Co., Ltd.

 ******************************************************************************
  File Name     : thread.h
  Version       : Initial Draft
  Author        : zengfanfan
  Created       : 2017-10-10
  Last Modified :
  Description   : thread wrapper
  Function List :
  History       :

******************************************************************************/
#ifndef LIB_DBAPI_IPC_THREAD_H
#define LIB_DBAPI_IPC_THREAD_H

#include <pthread.h>

typedef void (*db_thread_body_t)(void *args);

/*
 * db_start_thread - �����������߳�
 * @body: �߳��庯��
 * @args: ԭ������ @body ����
 *
 * ע��args��ò�Ҫ��ָ��ֲ�������ָ��,
 * ��Ϊ�� @body �з���ʱ�þֲ����������ѱ��ͷ�
 *
 * return: �߳�id, ʧ�ܷ��ظ���
 */
pthread_t db_start_thread(db_thread_body_t body, void *args);

/*
 * db_set_thread_name - ���õ�ǰ�̵߳�����
 * @fmt: just like printf
 * @...: just like printf
 *
 * return: 0-fail, 1-ok
 */
int db_set_thread_name(const char *fmt, ...);

char *db_get_thread_name(void);

#endif // LIB_DBAPI_IPC_THREAD_H