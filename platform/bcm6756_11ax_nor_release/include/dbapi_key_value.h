/******************************************************************************

  Copyright (C), 1999-2017, Tenda Tech Co., Ltd.

 ******************************************************************************
  File Name     : column.h
  Version       : 1.0
  Author        : zengfanfan
  Created       : 2017/6/3
  Description   : declaration of attributes and operations of sqlite3 column
  History       :

******************************************************************************/
#ifndef LIB_DBAPI_KEYVALUE_H
#define LIB_DBAPI_KEYVALUE_H

#include "dbapi_table.h"

#define KV_DB_NAME "kv"
#define KV_TABLE_NAME "kv"
#define KV_FIXED_DB_NAME DB_PATH"/kv.fixed" /* ʹ�þ���·���Ա��ⱻ��λ����ɾ�� */
#define KV_TMP_DB_NAME "/var/kv.db"

#define DB_MAX_KEY_LEN  500
#define DB_MAX_VAL_LEN  1500

/*
 * �������ṩ���ⲿ�Ľӿ�
 */

/*
    lock_value/unlock_value:
    lock_fixed_value/unlock_fixed_value:
    lock_tmp_value/unlock_tmp_value:
    ���� set ʱ, ʹ�� lock/unlock (���ݿ�����) �ܼӿ��ٶ�

    *tmp* ����ʱ����(����ϵͳ����ʧ)
    *fixed* ��Ĭ������(��λ����Ȼ����)
*/
#define lock_value() db_lock_value(get_kv_table())
#define unlock_value() db_unlock_value(get_kv_table())
#define lock_tmp_value() db_lock_value(get_tmp_kv_table())
#define unlock_tmp_value() db_unlock_value(get_tmp_kv_table())
#define lock_fixed_value() db_lock_value(get_fixed_kv_table())
#define unlock_fixed_value() db_unlock_value(get_fixed_kv_table())

/*
    set_value - ��������ֵ
    @key: ���ü�
    @value: ����ֵ
    @args: printf-like, @key �ĸ�ʽ������

    *tmp* ����ʱ����(����ϵͳ����ʧ)
    *fixed* ��Ĭ������(��λ����Ȼ����)

    return: 1=�ɹ�, 0=ʧ��
*/
#define set_value(key, value, args...) db_set_value(get_kv_table(), value, key,  ##args)
#define set_tmp_value(key, value, args...) db_set_value(get_tmp_kv_table(), value, key,  ##args)
#define set_fixed_value(key, value, args...) db_set_value(get_fixed_kv_table(), value, key,  ##args)

/*
    set_int_value - ��������ֵ(����)
    @key: ���ü�
    @value: ����ֵ
    @args: printf-like, @key �ĸ�ʽ������

    *tmp* ����ʱ����(����ϵͳ����ʧ)
    *fixed* ��Ĭ������(��λ����Ȼ����)

    return: 1=�ɹ�, 0=ʧ��
*/
#define set_int_value(key, value, args...) db_set_int_value(get_kv_table(), (int)(value), (char *)(key),  ##args)
#define set_tmp_int_value(key, value, args...) db_set_int_value(get_tmp_kv_table(), value, key,  ##args)
#define set_fixed_int_value(key, value, args...) db_set_int_value(get_fixed_kv_table(), value, key,  ##args)

/*
    get_value - ��ȡ����ֵ
    @key: ���ü�
    @value: ����ֵ
    @args: printf-like, @key �ĸ�ʽ������

    ���� @key ��������, ���ҵ�������ֵд�� @value ��

    *tmp* ����ʱ����(����ϵͳ����ʧ)
    *fixed* ��Ĭ������(��λ����Ȼ����)

    return: 1=�ɹ�, 0=ʧ��
*/
#define get_value(key, value, value_size, args...) db_get_value(get_kv_table(), value, value_size, key, ##args)
#define get_tmp_value(key, value, value_size, args...) db_get_value(get_tmp_kv_table(), value, value_size, key, ##args)
#define get_fixed_value(key, value, value_size, args...) db_get_value(get_fixed_kv_table(), value, value_size, key, ##args)

/*
    get_value - ��ȡ����ֵ
    @key: ���ü�
    @value: ����ֵ
    @args: printf-like, @key �ĸ�ʽ������

    ���� @key ��������, ���ҵ�������ֵд�� @value ��

    *tmp* ����ʱ����(����ϵͳ����ʧ)
    *fixed* ��Ĭ������(��λ����Ȼ����)

    return: 1=�ɹ�, 0=ʧ��
*/
#define get_int_value(key, args...) db_get_int_value(get_kv_table(), key, ##args)
#define get_tmp_int_value(key, args...) db_get_int_value(get_tmp_kv_table(), key, ##args)
#define get_fixed_int_value(key, args...) db_get_int_value(get_fixed_kv_table(), key, ##args)

/*
    del_value - ɾ������ֵ
    @key: ���ü�
    @args: printf-like, @key �ĸ�ʽ������

    *tmp* ����ʱ����(����ϵͳ����ʧ)
    *fixed* ��Ĭ������(��λ����Ȼ����)

    return: 1=�ɹ�, 0=ʧ��
*/
#define del_value(key, args...) db_del_value(get_kv_table(), key, ##args)
#define del_tmp_value(key, args...) db_del_value(get_tmp_kv_table(), key, ##args)
#define del_fixed_value(key, args...) db_del_value(get_fixed_kv_table(), key, ##args)


/*>>> [begin: internal declare] ���º���, �ⲿ��Ҫֱ�ӵ��� */
db_table_t *get_kv_table(void);
db_table_t *get_fixed_kv_table(void);
db_table_t *get_tmp_kv_table(void);
int db_lock_value(db_table_t *table);
int db_unlock_value(db_table_t *table);
int db_del_value(db_table_t *table, char *key, ...);
int db_set_value(db_table_t *table, char *key, char *value, ...);
int db_get_value(db_table_t *table, char *value, int value_size, char *key, ...);
int db_set_int_value(db_table_t *table, int value, char *key, ...);
int db_get_int_value(db_table_t *table, char *key, ...);
/*<<< [end: internal declare] ���Ϻ���, �ⲿ��Ҫֱ�ӵ��� */
#endif // LIB_DBAPI_KEYVALUE_H