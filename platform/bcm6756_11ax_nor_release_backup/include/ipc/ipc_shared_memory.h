#ifndef LIB_DBAPI_IPC_SHARED_MEMORY_H
#define LIB_DBAPI_IPC_SHARED_MEMORY_H

/******************************************************************************

  Copyright (C), 1999-2017, Tenda Tech Co., Ltd.

 ******************************************************************************
  File Name     : shared_memory.h
  Version       : Initial Draft
  Author        : zengfanfan
  Created       : 2017-9-29
  Last Modified :
  Description   : shared memory between processes
  Function List :
  History       :

******************************************************************************/

/*
 * db_get_shared_memory - ��ȡ�����ڴ�
 * @name: Ҫ�����Ĺ����ڴ��Ψһ����
 * @size: �ڴ�Ĵ�С
 *
 * ����������򴴽�
 *
 * return: �ɹ�����ָ��, ʧ�ܷ���NULL
 */
void *db_get_shared_memory(char *name, unsigned size);

#endif // LIB_DBAPI_IPC_SHARED_MEMORY_H