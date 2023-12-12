/******************************************************************************
          ��Ȩ���� (C), 2015-2018, �����м����ڴ�Ƽ����޹�˾
 ******************************************************************************
  �� �� ��   : nvram_extern.h
  �� �� ��   : ����
  ��    ��   : zzh
  ��������   : 2016��3��26��
  ����޸�   :
  ��������   : 

  ��������   : nvram��������ӿ�ͷ�ļ�

  �޸���ʷ   :
  1.��    ��   : 2016��3��26��
    ��    ��   : zzh
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef NVRAM_EXTERN_H
#define NVRAM_EXTERN_H

char *bcm_nvram_get(const char *name);
int bcm_nvram_match(char *name, char *match);
int bcm_nvram_getall(char *buf, int count);
int bcm_nvram_set(const char *name, const char *value);
int bcm_nvram_unset(const char *name);
int bcm_nvram_commit(void);

#define bcm_nvram_safe_get(name)  (bcm_nvram_get(name) ? : "")
#endif

