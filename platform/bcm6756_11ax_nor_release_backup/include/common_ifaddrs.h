/*     $Id: ifaddrs.h,v 1.1.1.1 2003/01/16 15:41:11 root Exp $    */
/*    from USAGI: ifaddrs.h,v 1.1 2001/01/26 07:11:48 yoshfuji Exp    */

/*
 * Copyright (c) 1995, 1999
 *    Berkeley Software Design, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY Berkeley Software Design, Inc. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Berkeley Software Design, Inc. BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef COMMON_IFADDRS_H
#define COMMON_IFADDRS_H
#include "common_extern.h"

#ifndef invalidstr
#define invalidstr(s) (s == 0 || s[0] == 0)
#endif

/*****************************************************************************
 �� �� ��  : ifaddrs_get_interface_addr
 ��������  : ͨ���ӿ�����ȡ��Ӧ��IP��ַ
 �������  : char *IfName  
 �������  : ��
 �� �� ֵ  : �ɹ����ض�Ӧ�������ֽ����IP��ַ��0ʧ��
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��1��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long ifaddrs_get_interface_addr(char *IfName);

/*****************************************************************************
 �� �� ��  : ifaddrs_ethaddr_aton
 ��������  : ����̫����ַ�ַ�����ʽת��Ϊ�����������ֽ����ʽ
 �������  : const char *mac_str  string in xx:xx:xx:xx:xx:xx notation   
 �������  : unsigned char *macaddr  binary data
 �� �� ֵ  : TRUE if conversion was successful and FALSE otherwise
 
 �޸���ʷ      :
  1.��    ��   : 2016��1��8��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ifaddrs_ethaddr_aton(const char *mac_str, unsigned char *macaddr);

/*****************************************************************************
 �� �� ��  : ifaddrs_ethaddr_ntoa
 ��������  : ����̫����ַ�����Ƹ�ʽת�����ַ�����ʽ
 �������  : const unsigned char *macaddr  binary data
 �������  : char *mac_str string in xx:xx:xx:xx:xx:xx notation
 �� �� ֵ  : mac_str
 
 �޸���ʷ      :
  1.��    ��   : 2016��1��8��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
char* ifaddrs_ethaddr_ntoa(const unsigned char *macaddr, char *mac_str);

/*
    Function:ifaddrs_numbers_and_dot_ip_valid
    Description:
        �жϵ��ʮ����ip��Ч��
    Input:
        ip��ַ
    Output:
        none
    Return:
        0:ip��ַ��Ч��1��ip��ַ��Ч
    Author:kdz
    Date:2014.8.20
*/
int ifaddrs_numbers_and_dot_ip_valid(const char *ip);

/*****************************************************************************
 �� �� ��  : ifaddrs_get_ifip
 ��������  : ��ȡ�ӿ�ip��ַ
 �������  : const char *ifname �ӿ���  
 �������  : char *if_addr �ӿ�ip
 �� �� ֵ  : -1ʧ�ܣ�0�ɹ�
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��25��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ifaddrs_get_ifip(const char *ifname, char *if_addr);

/*****************************************************************************
 �� �� ��  : ifaddrs_get_if_netmask
 ��������  : ��ȡ�ӿ���������
 �������  : const char *ifname  �ӿ���
 �������  : char *if_net  ����
 �� �� ֵ  : -1ʧ�ܣ�0�ɹ�
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��25��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ifaddrs_get_if_netmask(const char *ifname, char *if_net);

/*****************************************************************************
 �� �� ��  : ifaddrs_get_if_netmac
 ��������  : ͨ���ӿ�����ȡmac��ַ
 �������  : char *ifname  �ӿ���
 �������  : unsigned char *if_mac  ��Ӧ�Ľӿ�mac��ַ
 �� �� ֵ  : -1ʧ�ܣ�0�ɹ�
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��25��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ifaddrs_get_if_netmac(char *ifname, unsigned char *if_mac);

/*****************************************************************************
 �� �� ��  : ifaddrs_ipmask_to_numeric
 ��������  : ����������ת��Ϊ���ֱ�ʾ
 �������  : const char *mask  
 �������  : ��
 �� �� ֵ  : ��Ӧ��������ĺ�׺ֵ
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��25��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ifaddrs_ipmask_to_numeric(const char *mask);

/*****************************************************************************
 �� �� ��  : ifaddrs_get_network_addr_seg
 ��������  : ��ȡip��ַ�ĵ�ַ�� (����:192.168.10.1-->���:192.168.10.0)
 �������  : const char *ip     
             const char *mask   
 �������  : char *networkaddr ���ε�ַ
 �� �� ֵ  :  -1ʧ�ܣ�0�ɹ�
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��25��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ifaddrs_get_network_addr_seg(const char *ip,const char *mask,char *networkaddr);

/*****************************************************************************
 �� �� ��  : ifaddrs_mac_increase
 ��������  : ��Ӧλ��mac��ַ��1 //����:00:11:22:33:44:55 
 �������  : unsigned char *mac  
             int c   ��ʾmac��ַ��Ӧ��λ    
             ��c = 5ʱ,ִ�иú�����mac��ַ��Ϊ00:11:22:33:44:56       
 �������  : ��
 �� �� ֵ  : -1ʧ�ܣ�0�ɹ�
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��25��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ifaddrs_mac_increase(unsigned char *mac, int c);

/******************************************************************************
Function:     ifaddrs_ip_increase
Description:  ���λ�ip����1
Input:
inIpAddr ��һ��ip��ַ
ipNetMask  ��Ӧ����������
outIpAddr ���ip��ַ
mode   mode=0,���μ�1��mode=1�iip��1��mode=2��ip�����ζ���1

Output:
   ��
Return:
��0 ����Ҫ�޸�
0  �޸ĳɹ�
Others:
*******************************************************************************/
int ifaddrs_ip_increase(char *inIpAddr, char *ipNetMask, char *outIpAddr, int mode);

/*****************************************************************************
 �� �� ��  : ifaddrs_get_subnet
 ��������  : ͨ��ip�����������ȡ�������� 
             ����(192.168.10.1,255.255.255.0-->192.168.10.0/24)
 �������  : const char *ip    
             const char *mask  
 �������  : char *net  
 �� �� ֵ  : 0�ɹ���-1ʧ��
 
 �޸���ʷ      :
  1.��    ��   : 2015��11��26��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
int ifaddrs_get_subnet(const char *ip, const char *mask, char *net);


/*****************************************************************************
 �� �� ��  : ifaddrs_get_interface_mtu
 ��������  : ��ȡ��Ӧ�ӿڵ�mtuֵ
 �������  : const char *ifname       
             unsigned int *mtu_value  
 �������  : ��
 �� �� ֵ  : ʧ��:UGW_ERR
             �ɹ�:UGW_OK
 
 �޸���ʷ      :
  1.��    ��   : 2015��12��24��
    ��    ��   : zzh
    �޸�����   : �����ɺ���

*****************************************************************************/
UGW_RETURN_CODE_ENUM ifaddrs_get_interface_mtu(const char *ifname, unsigned int *mtu_value);

/*****************************************************************************
 �� �� ��  : ifaddrs_get_interface_downup
 ��������  : ��ȡbss down up״̬��Ϣ
 �������  : mib_name�����߽ӿ�
 �������  : 
 �� �� ֵ  : Up: UGW_OK  down: UGW_ERR
 �޸���ʷ      :
  1.��    ��   : 2021��6��28��
    ��    ��   : huangyongjie
    �޸�����   : �����ɺ���
*****************************************************************************/
int ifaddrs_get_interface_downup(const char *osifname);

#endif

