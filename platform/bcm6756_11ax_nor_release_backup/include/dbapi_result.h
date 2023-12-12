/******************************************************************************

  Copyright (C), 1999-2017, Tenda Tech Co., Ltd.

 ******************************************************************************
  File Name     : result.h
  Version       : 1.0
  Author        : zengfanfan
  Created       : 2017/6/3
  Last Modified :
  Description   : declaration of attributes and operations of sqlite3 result
  Function List :
  History       :

******************************************************************************/
#ifndef LIB_DBAPI_RESULT_H
#define LIB_DBAPI_RESULT_H

#include "list.h"
#include "dbapi_row.h"
#include "dbapi_tool.h"

typedef struct db_result
{
    struct list_head rows;
    int row_num;
    char *col_names[DB_MAX_COL_NUM];
    int col_num;
    int inited;

    /*
     *  free - �ͷŽ����ռ�õ��ڴ�
     *  @self: ���
     *
     *  �ѹ������� self->rows ��������ж��ͷŵ�
     */
    void (*free)(struct db_result *self);
} db_result_t;

#define db_foreach_result(row, result) \
    db_row_t *DB_LINE_VAR(db_next); \
    list_for_each_entry_safe(row, DB_LINE_VAR(db_next), &(result)->rows, link)
#define db_foreach_result_reverse(row, result) \
    db_row_t *DB_LINE_VAR(db_next); \
    list_for_each_entry_safe_reverse(row, DB_LINE_VAR(db_next), &(result)->rows, link)
#define db_first_result(result) list_first_entry(&(result)->rows, db_row_t, link)

int db_result_init(db_result_t *self);
db_row_t *db_strcmp(db_result_t *result, int type, char *str);

#endif // LIB_DBAPI_RESULT_H