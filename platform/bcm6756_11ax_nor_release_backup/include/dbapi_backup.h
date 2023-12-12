#ifndef LIB_DBAPI_BACKUP_H
#define LIB_DBAPI_BACKUP_H

#include <pthread.h>
#include "dbapi_database.h"

#define DB_DUMP_START       "###> product database [ %s ] start >###"
#define DB_DUMP_END         "###< product database [ %s ] end <###"

typedef sqlite_uint64 uint64;         /* 8-byte unsigned integer */

typedef struct db_backup
{
    struct list_head link;
    struct list_head sync;
    database_t main;   // �����ݿ�
    database_t backup; // �������ݿ�
    database_t mem;    // �ڴ����ݿ�
    unsigned char inited: 1;
    unsigned char lock_cnt;
    unsigned short sync_cnt;
    pthread_mutex_t mutex;
    uint64 lock_start;
    unsigned char delay_sync;//��ʱͬ���ڴ����ݿ⵽�����ݿ��ʱ��,0��ʾ������������ʱ����

    /*
     *  modify - ִ��ָ��SQL�޸����
     *  @self: ���ݿⱸ�ݶ���
     *  @cmd: Ҫִ�е����
     *  @result: ����ִ�н��, ��NULL�򲻱�����
     *
     *  resultʹ�����ע���ͷ��ڴ�: result.free(&result)
     *
     *  return: SQLITE_OK(0) if ok, else return SQLITE_XXX error code
     */
    int (*modify)(struct db_backup *self, char *cmd, db_result_t *result);

    /*
     *  query - ִ��ָ��SQL��ѯ���
     *  @self: ���ݿⱸ�ݶ���
     *  @cmd: Ҫִ�е����
     *  @result: ����ִ�н��, ��NULL�򲻱�����
     *
     *  resultʹ�����ע���ͷ��ڴ�: result.free(&result)
     *
     *  return: SQLITE_OK(0) if ok, else return SQLITE_XXX error code
     */
    int (*query)(struct db_backup *self, char *cmd, db_result_t *result);

    /*
     *  rollback - �ع�
     *  @self: ���ݿⱸ�ݶ���
     *
     *  �ع����ݿ�����
     */
    void (*rollback)(struct db_backup *self);

    /*
     *  recover - ���Դ��쳣�лָ�
     *  @self: ���ݿⱸ�ݶ���
     *  @main_err: �����ݿ������
     *  @backup_err: �������ݿ������
     *  @mem_err: �ڴ����ݿ������
     */
    void (*recover)(struct db_backup *self, int main_err, int backup_err, int mem_err);

    /*
     *  lock - ����
     *  @self: ���ݿⱸ�ݶ���
     *
     *  ���뻥����, �����ѱ�ռ��, �����ȴ�, ֱ��ռ���������ͷ���
     *  �������ע��Ҫ�ͷ���( self->unlock )
     *
     *  ���lock�����Ǳ�֤�������ִ�й����в��ᱻ�������޸�,
     *  ��: ִ�е������ݿ�����ǲ���Ҫlock��
     */
    void (*lock)(struct db_backup *self);

    /*
     *  unlock - ����
     *  @self: ���ݿⱸ�ݶ���
     *
     *  �ͷŻ�����
     *  �� self->lock ���ʹ��
     */
    void (*unlock)(struct db_backup *self);

    /*
     *  delay_sync_func - ִ�����ݿⱸ��
     *  @self: ���ݿⱸ�ݶ���
     *  @path_backup_db �������ݿ��·��
     *
     */
    int (*delay_sync_func)(struct db_backup *self, char* path_backup_db);

} db_backup_t;

/*
 *  db_backup_save - �������ݿ⵽�ļ�
 *  @name: ���ݿ�����
 *  @filename: �ļ���
 *
 *  �����ݿ�����dump��ָ���ļ���
 */
void db_backup_save(char *db_name, char *filename);

/*
 *  db_backup_load - �ָ����ݿ�
 *  @name: ���ݿ�����
 *  @data: ���ݿ�����(SQL��伯)
 *
 *  ��ָ�����ݿ�ִ�� @data ��伯
 */
void db_backup_load(char *db_name, char *data);

db_backup_t *get_backup_obj(char *name);
int db_get_col_name_list(db_backup_t *self, char *tbl_name, db_result_t *result);

#endif // LIB_DBAPI_BACKUP_H