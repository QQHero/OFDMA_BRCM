/******************************************************************************

  Copyright (C), 1999-2017, Tenda Tech Co., Ltd.

 ******************************************************************************
  File Name     : database.h
  Version       : 1.0
  Author        : zengfanfan
  Created       : 2017/6/3
  Description   : declaration of attributes and operations of sqlite3 database
  History       :

******************************************************************************/
#ifndef LIB_DBAPI_DATABASE_H
#define LIB_DBAPI_DATABASE_H

#include "dbapi_result.h"

#define DB_NAME_LEN 64

#define DB_PATH  "/cfg/database"

#define MEM_DB_NAME     "/var/mem.db"
#define DEF_DB_NAME     "default"

#define DB_MAX_CMD_LEN (DB_MAX_STR_LEN * 4)
#define DB_EXEC_TIMEOUT  20// seconds

#define DB_DELAY_SYNC_OFF     0
#define DB_DELAY_SYNC_ON      1

typedef struct database
{
    char *name;// ���ݿ���
    sqlite3 *db; // sqlite3 ���ݿ����Ӷ���ָ��
    char inited: 1; // ��(1)��(0)�ѳ�ʼ��
    char need_rollback: 1; // ��(1)��(0)��Ҫ�ع�
    int lock_cnt; // ��(>0)��(0)�Ѽ���

    /*
     *  execute - ִ��ָ��SQL���
     *  @self: ���ݿ�
     *  @cmd: Ҫִ�е����
     *  @result: ����ִ�н��, ��NULL�򲻱�����
     *
     *  resultʹ�����ע���ͷ��ڴ�: result.free(&result)
     *
     *  return: SQLITE_OK(0) if ok, else return SQLITE_XXX error code
     */
    int (*execute)(struct database *self, char *cmd, db_result_t *result);

    /*
     *  lock - ����
     *  @self: ���ݿ�
     *
     *  ���뻥����, �����ѱ�ռ��, �����ȴ�, ֱ��ռ���������ͷ���
     *  �������ע��Ҫ�ͷ���( self->unlock )
     *
     *  ���lock�����Ǳ�֤�������ִ�й����в��ᱻ�������޸�,
     *  ��: ִ�е������ݿ�����ǲ���Ҫlock��
     */
    void (*lock)(struct database *self);

    /*
     *  unlock - ����
     *  @self: ���ݿ�
     *
     *  �ͷŻ�����
     *  �� self->lock ���ʹ��
     */
    void (*unlock)(struct database *self);

    /*
     *  rollback - �ع�����
     *  @self: ���ݿ�
     *
     *  �������� (lock �� unlock ֮��) ����, �ع���������
     */
    void (*rollback)(struct database *self);

    /*
     *  close - �ر����ݿ�����
     *  @self: ���ݿ�
     */
    void (*close)(struct database *self);

    /*
     *  reopen - ��/���´����ݿ�����
     *  @self: ���ݿ�
     *
     *  return: 1-ok, 0-fail
     */
    int (*reopen)(struct database *self);
} database_t;

/*
 *  init_database - ��ʼ�����ݿ�
 *  @self: ���ݿ�
 *  @name: ���ݿ�����
 *
 *  ���� db_table_init ��ʹ��, �ⲿ��Ӧ�õ��øú���
 *
 *  returns: 1-ok, 0-fail
 */
int init_database(database_t *self, char *name);

/*
 *  db_name_to_path - �����ݿ���ת�����ݿ��ļ�·��
 *  @name: ���ݿ�����
 *  @path: �������ݿ��ļ�·��
 *  @pathlen: @path����󳤶�
 *
 */
void db_name_to_path(char *name, char *path, unsigned pathlen);

int db_execute_sql(database_t *self, char *cmd, void *func, db_result_t *res);

#endif // LIB_DBAPI_DATABASE_H