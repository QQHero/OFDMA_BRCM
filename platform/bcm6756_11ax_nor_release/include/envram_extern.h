/******************************************************************************
          ��Ȩ���� (C), 2015-2018, �����м����ڴ�Ƽ����޹�˾
 ******************************************************************************
  �� �� ��   : envram_extern.h
  �� �� ��   : ����
  ��    ��   : zzh
  ��������   : 2016��3��26��
  ����޸�   :
  ��������   : 

  ��������   : envram��������ӿڶ���ͷ�ļ�

  �޸���ʷ   :
  1.��    ��   : 2016��3��26��
    ��    ��   : zzh
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef ENVRAM_EXTERN_H
#define ENVRAM_EXTERN_H

int envram_submit(void *unused);
int envram_get(int argc, char* argv[]);
int envram_set_value(char* name, char* value);
int envram_set(int argc, char* argv[]);
int envram_commit(int argc, char* argv[]);
int envram_show(void *unused);

#endif
