#ifndef __NTDD_MAIN_H__
#define __NTDD_MAIN_H__

#include "common_extern.h"


// �豸���͸��¼��
#define NTDD_DEV_TYPE_UPDATE_INTERVAL       30

// �豸������
#define NTDD_DEV_NUM_MAX                    1000

// ��ϣ���С
#define NTDD_DEV_HASH_SIZE                  64

// ���Ϸ�����̫���˿�
#define NTDD_INVALID_ETHER_PORT             0xE0

// lldpd����unix������·��
#define NTDD_LLDPD_UNIX_SOCKET_PATH         "/var/run/lldpd.sock"

// ssdpd����unix������·��
#define NTDD_SSDPD_UNIX_SOCKET_PATH         "/var/run/ssdpd.sock"

// �豸�����¼�
#define NTDD_DEV_OFFLINE_EVENT              "net_topo.dev_offline.from_kernel"

// �豸�����¼�
#define NTDD_DEV_ONLINE_EVENT                "net_topo.dev_online.from_kernel"

// ���������ڶ˿��¼�
#define NTDD_SERVER_PORT_EVENT              "net_topo.server_port.from_kernel"


#define NTDD_SET_BIT(num, bit)              ((num) |= (1 << (bit)))
#define NTDD_CLR_BIT(num, bit)              ((num) &= ~(1 << (bit)))
#define NTDD_IS_SET_BIT(num, bit)           (((num) >> (bit)) & 0x1)

#define NTDD_DEV_TYPE_LEN                   64
#define NTDD_DEV_SN_LEN                     64
#define NTDD_DEV_PROJECT_ID_LEN             64


enum {
    NTDD_MSG_NONE = 0,
    NTDD_MSG_ONLINE,
    NTDD_MSG_OFFLINE,
};


#define NTDD_PRINTF(fmt, arg...)  do { \
        printf("** NTDD ** %s %d: " fmt, __FUNCTION__, __LINE__, ##arg); \
    } while (0)


enum ntdd_ident_proto {
    NTDD_PROTO_ARP = 0,
    NTDD_PROTO_LLDP,
    NTDD_PROTO_SSDP,
};

struct ntdd_lldpd_head {
    int command;
    int length;
    char data[0];
};

struct ntdd_lldpd_dev_info {
    char mac[UGW_MAC_STR_LEN];
    char type[NTDD_DEV_TYPE_LEN];
    char ip[UGW_IPADDR_STR_LEN];
    char sn[NTDD_DEV_SN_LEN];
    char project_id[NTDD_DEV_PROJECT_ID_LEN];
    struct list_head list;
};

struct ntdd_ssdpd_head {
    int command;
    int length;
    char data[0];
};

struct ntdd_ssdpd_dev_info {
    char ip[UGW_IPADDR_STR_LEN];
    char type[NTDD_DEV_TYPE_LEN];
    struct list_head list;
};

struct ntdd_dev_info {
    char mac[UGW_MAC_STR_LEN];                      /* MAC��ַ */
    char ip[UGW_IPADDR_STR_LEN];                    /* IP��ַ */
    uint8_t from_port;                              /* ����: �ĸ��˿ڷ��ֵ��豸, ����: 0xFF */
    uint32_t proto_mask;                            /* �豸֧������Э�� */
    char type[NTDD_DEV_TYPE_LEN];                   /* �豸���� */
    char sn[NTDD_DEV_SN_LEN];                       /* ���к� */
    char project_id[NTDD_DEV_PROJECT_ID_LEN];       /* ��ĿID */
    bool is_neighbor;                               /* 0: �����ھ�, 1:���ھ� */
    bool is_active;                                 /* 0: ����, 1:���� */
};

struct ntdd_dev_node {
    struct list_head list;
    struct ntdd_dev_info info;
};

struct ntdd_buf_list {
    struct list_head list;
    struct ntdd_dev_info info;
    bool is_del;      /*��¼�ڵ��Ƿ�Ϊ��Ҫɾ���Ľڵ�*/
};


#endif
