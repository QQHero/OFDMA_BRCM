#include <802.11.h>


#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ratelimit.h>

#include <wlc_ie_mgmt.h>
#include <wlc_ie_mgmt_vs.h>

#include <wlc_ie_mgmt_ft.h>
#include <bcmendian.h>
#include <wlc_scb.h>
#include "p_aes.h"
#ifdef TD_PRIVATE_IE
#include "td_ie_manage.h"
#include "td_ie_helper.h"
#include "td_wlan_proc.h"
#endif

#define tie_println(fmt, args...) \
printk("td_ie, %s(%d). "fmt"\n", __func__, __LINE__, ##args)

#define tie_println_limited(fmt, args...) \
printk_ratelimited("td_ie, %s(%d). "fmt"\n", __func__, __LINE__, ##args)

#define PROC_NAME "td_ie"
bool proc_enable = false;

typedef struct td_ie_reg {
    unsigned char port;
    unsigned char *data;
    unsigned char len;
    unsigned char frame_types;
    ven_vap_t *vap;
    te_ie_callback_t rx_callback;
    struct list_head ie_list;
    struct hlist_node port_list;
} td_ie_reg_t;

/* ���ڴ���û��·���ָ��oui������Ϣ�����ڸ���Ϣ��Ҫ������/�ں�ģ��ʹ�ã��ʷ��ڴ˴����� */
LIST_HEAD(g_td_oui_rule_list);
EXPORT_SYMBOL(g_td_oui_rule_list);

static DEFINE_RWLOCK(s_lock);
static LIST_HEAD(s_ie_list);
static struct hlist_head s_ports[TD_MAX_KERNEL_IE];
//bcm������д��˽��ie֮ǰ�ѵ���ע�ắ�����㳤�ȣ����Իص�td_add_elements_cbʱ���迼�ǳ��ȣ��˺��ɴ˶���
#define MAX_UINT_LEN (unsigned int)(~0)

int (*td_add_elements_cb)(unsigned char *buff, size_t size, struct net_device *dev, int frame_type) = NULL;
void (*td_save_element_cb)(unsigned char *content, unsigned char len, int rssi, 
    const unsigned char *sa, int frame_type, int band) = NULL;
//��ע��д��ieʱ������ie���ȵĻص�
unsigned int (*td_calc_element_cb)(struct net_device *dev) = NULL;

EXPORT_SYMBOL(td_add_elements_cb);
EXPORT_SYMBOL(td_save_element_cb);
EXPORT_SYMBOL(td_calc_element_cb);

void td_update_dev_beacon(struct net_device *dev)
{
    ven_vap_t *vap = NULL;

    if (!dev) {
        return;
    }

    vap = ven_dev_to_vap(dev);
    if (vap) {
        td_update_beacon(vap);
    }

    return;
}
EXPORT_SYMBOL(td_update_dev_beacon);

static inline td_ie_reg_t* td_search_ie_reg(ven_vap_t *vap, unsigned char port)
{
    td_ie_reg_t *ie_reg;
    struct hlist_node *n;

    hlist_for_each_entry_safe(ie_reg, n, &(s_ports[port]), port_list) {
        if (ie_reg->vap == vap) {
            return ie_reg;
        }
    }

    return NULL;
}


/*****************************************************************************
 * �� �� ��  : td_search_ie_and_cb
 * �� �� ��  : bcz
 * ��������  : 
 * ��������  : �յ�ie��ִ�лص�
 * �������  : 
 * �������  : 
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 
*****************************************************************************/
static void td_search_ie_and_cb(ven_vap_t *vap, 
                                        wlc_info_t *wlc,
                                        td_element_t *ele,
                                        const unsigned char *sa,
                                        td_pe_frame_type_e frame_type)
{
    td_ie_reg_t *ie_reg;
    struct hlist_node *n;
    te_ie_callback_t rx_callback;
    unsigned long flags;

    read_lock_irqsave(&s_lock, flags);

    hlist_for_each_entry_safe(ie_reg, n, &(s_ports[ele->port]), port_list) {
        if (!(ie_reg->frame_types & (1 << frame_type))) {
            continue;
        }

        if (vap) {      //ָ��vap�ص�
            if (ie_reg->vap == vap) {
                rx_callback = ie_reg->rx_callback;
                read_unlock_irqrestore(&s_lock, flags);
                if (rx_callback) {
                    rx_callback(ele->data, ele->len - TD_OUI_LEN -1, 
                            vap, sa, frame_type);
                }
                return;
            }
        } else {    //radio����ص�
            if (ie_reg->vap->wlc == wlc) {
                rx_callback = ie_reg->rx_callback;
                if (!rx_callback) {
                    continue;
                }
                read_unlock_irqrestore(&s_lock, flags);
                rx_callback(ele->data, ele->len - TD_OUI_LEN -1, 
                            ie_reg->vap, sa, frame_type);
                read_lock_irqsave(&s_lock, flags);
                
            }
        }
    }

    read_unlock_irqrestore(&s_lock, flags);

    return ;
}

/*****************************************************************************
 �� �� ��  : td_lookup_oui_rule
 ��������  : ����oui����
 �������  : td_element_t *ele  
             int band           
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2021��7��17��
    ��    ��   : tys
    �޸�����   : �����ɺ���

*****************************************************************************/
int td_lookup_oui_rule(td_element_t *ele, int band)
{
    td_ie_oui_node_t *node = NULL, *n = NULL;

    if (!list_empty(&g_td_oui_rule_list)) {
        list_for_each_entry_safe(node, n, &g_td_oui_rule_list, list_node) {
            if (!memcmp(node->oui, ele->oui, TD_OUI_LEN) && 
                (node->band & (1 << band))) {
                return 1;
            }
        }
    }

    return 0;
}

/* receive an IE
 *
 * sa: source address of this frame
 */
void td_recv_ie(td_element_t *ele, 
                ven_vap_t *vap, 
                wlc_info_t *wlc,
                const unsigned char *sa, 
                td_pe_frame_type_e frame_type,
                int rssi) {

    int oui_exist_flag = 0;
#ifdef CONFIG_PRIVATE_IE_ENCRYPT
    char buf[U8_MAX] = {0};
    unsigned int Decrypt_len = 0;
#endif
    if (unlikely(!ele || !sa)) {
        return;
    }

    /* �˴����TD_OUI�жϣ���Ϊ���ܴ���������̽�������ӹ� */
    if (ele->port >= PIE_USER_PORT_START && is_td_ie(ele)) {
#ifdef CONFIG_PRIVATE_IE_ENCRYPT
        /*���ݶ�����ע���֡��ͳһ�����ܴ���*/
        *buf = ele->port;
        /* OUI,portδ�����ܴ�����port֮����ֶο�ʼ���� */
        IE_AesCbcDecrypt((char *)(&(ele->data)), buf+1, ele->len - TD_OUI_LEN - 1, TD_PRIVATE_IE_KEY, &Decrypt_len);
        /* IE used in user space */
        pie_save_element((unsigned char*)buf, Decrypt_len + 1, rssi, sa, frame_type, wlc->pub->unit);
#else
        pie_save_element((unsigned char*)&ele->port, ele->len - TD_OUI_LEN, rssi, sa, frame_type, wlc->pub->unit);
#endif
    } else if (ele->port < TD_MAX_KERNEL_IE && is_td_ie(ele)) {
        /* IE used in kernel space ,�����ӿ��ݲ����ӽ��ܴ�������������Ҫ����ʱ���ٴ��ڴ��ع�*/
        td_search_ie_and_cb(vap, wlc, ele, sa, frame_type);
    }

    oui_exist_flag = td_lookup_oui_rule(ele, wlc->pub->unit);
    if (oui_exist_flag) {
        /* ����ele->len + 2������Ҫ��tag num��length������Ϣһ����δ��� */
        pie_save_element((unsigned char*)&ele->element_id, ele->len + 2, rssi, sa, frame_type, wlc->pub->unit);
    }

    return;
}

/*****************************************************************************
 * �� �� ��  : td_get_frame_type
 * �� �� ��  : bcz
 * ��������  : 
 * ��������  : ͨ��ӳ���ϵ��get֡����
 * �������  : 
 * �������  : 
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 
*****************************************************************************/
td_pe_frame_type_e td_get_frame_type(wlc_iem_ft_t ft)
{

    switch(ft) {
        case FC_BEACON:
        case WLC_IEM_FC_AP_BCN:
            return TD_PE_FRAME_TYPE_BEACON;
        case FC_PROBE_REQ:
            return TD_PE_FRAME_TYPE_PROBE_REQ;
        case FC_PROBE_RESP:
            return TD_PE_FRAME_TYPE_PROBE_RES;
        case FC_ASSOC_REQ:
            return TD_PE_FRAME_TYPE_ASSOC_REQ;
        case FC_ASSOC_RESP:
            return TD_PE_FRAME_TYPE_ASSOC_RES;
        case FC_AUTH:
            return TD_PE_FRAME_TYPE_AUTH;
        default:
            return TD_PE_FRAME_TYPE_UNKNOW;
    }

}

/*****************************************************************************
 * �� �� ��  : td_parse_vs_ie_register
 * �� �� ��  : bcz
 * ��������  : 
 * ��������  : ����ie�ص�ע��
 * �������  : 
 * �������  : 
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 
*****************************************************************************/
static int td_parse_vs_ie_register(void *ctx, wlc_iem_parse_data_t *data)
{
    wlc_info_t *wlc = (wlc_info_t *)ctx;
    wlc_bsscfg_t *cfg = NULL;
    unsigned char *samac = NULL;
    td_pe_frame_type_e frame_type;
    int rssi = 0;

    if (!data || !data->ie) {
        return BCME_OK;
    }

    cfg = data->cfg;
    rssi = data->pparm->wrxh->rssi;
    if(!is_td_ie((td_element_t *)(data->ie))) {
        return BCME_OK;
    }
    
    frame_type = td_get_frame_type(data->ft);

    switch (frame_type) {
        case TD_PE_FRAME_TYPE_BEACON:
            samac = ((struct dot11_management_header *)(data->pparm->hdr))->sa.octet;
            break;
        case TD_PE_FRAME_TYPE_PROBE_REQ:
            samac = ((struct dot11_management_header *)(data->pparm->hdr))->sa.octet;
            break;
        case TD_PE_FRAME_TYPE_PROBE_RES:
            samac = ((struct dot11_management_header *)(data->pparm->hdr))->sa.octet;
            break;
        case TD_PE_FRAME_TYPE_ASSOC_REQ:
            samac = data->pparm->ft->assocreq.scb->ea.octet;
            break;
        case TD_PE_FRAME_TYPE_ASSOC_RES:
            samac = data->pparm->ft->assocresp.scb->ea.octet;
            break;
        case TD_PE_FRAME_TYPE_AUTH:
            samac = data->pparm->ft->auth.scb->ea.octet;
            break;
        default:
            return BCME_OK;
    }

    td_recv_ie((td_element_t *)(data->ie), (ven_vap_t *)cfg, wlc, samac, frame_type, rssi);

    return BCME_OK;
}

/*****************************************************************************
 * �� �� ��  : td_add_ie_register
 * �� �� ��  : bcz
 * ��������  : 
 * ��������  : ע�����ie�ֶ�
 * �������  : 
 * �������  : 
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 
*****************************************************************************/
int td_add_ie_register(void *ctx, wlc_iem_build_data_t *data)
{

    unsigned long flags;
    td_ie_reg_t *ie_reg;
    td_ie_reg_t *n;
    struct net_device *dev;
    unsigned char *pos = NULL;
    ven_vap_t *vap = NULL;
    td_pe_frame_type_e frame_type = TD_PE_FRAME_TYPE_UNKNOW;

    if (unlikely(!data || !data->buf || !data->cfg)) {
        return BCME_OK;
    }

    pos = data->buf;
    vap = data->cfg;
    frame_type = td_get_frame_type(data->ft);
    if (TD_PE_FRAME_TYPE_UNKNOW == frame_type) {
        return BCME_OK;
    }

    read_lock_irqsave(&s_lock, flags);
    list_for_each_entry_safe(ie_reg, n, &s_ie_list, ie_list) {
        if (ie_reg->vap != vap) {
            continue;
        }

        if (!(ie_reg->frame_types & (1 << frame_type))) {
            continue;
        }

        if (unlikely(!ie_reg->data)) {
            tie_println_limited("ERROR: NULL pointer, port %d", ie_reg->port);
            continue;
        }

        memcpy(pos, ie_reg->data, ie_reg->len);
        pos += ie_reg->len;
    }
    read_unlock_irqrestore(&s_lock, flags);

    dev = ven_vap_to_dev(vap);
    if (dev) {
        pos += pie_add_elements(pos, MAX_UINT_LEN, dev, frame_type);
    }

    return BCME_OK;
    
}

/*****************************************************************************
 * �� �� ��  : td_calc_ie_register
 * �� �� ��  : bcz
 * ��������  : 
 * ��������  : ע�����ieʱ���ȼ���Ļص�
 * �������  : 
 * �������  : 
 * �� �� ֵ  : 
 * ���ù�ϵ  : 
 * ��    ��  : 
*****************************************************************************/
uint td_calc_ie_register(void *ctx, wlc_iem_calc_data_t *data)
{

    unsigned long flags;
    td_ie_reg_t *ie_reg;
    td_ie_reg_t *n;
    struct net_device *dev;
    uint len = 0;
    ven_vap_t *vap = NULL;
    td_pe_frame_type_e frame_type = TD_PE_FRAME_TYPE_UNKNOW;

    if (unlikely(!data || !data->cfg)) {
        return 0;
    }

    vap = data->cfg;
    frame_type = td_get_frame_type(data->ft);
    if (TD_PE_FRAME_TYPE_UNKNOW == frame_type) {
        return 0;
    }

    read_lock_irqsave(&s_lock, flags);
    list_for_each_entry_safe(ie_reg, n, &s_ie_list, ie_list) {
        if (ie_reg->vap != vap) {
            continue;
        }

        if (!(ie_reg->frame_types & (1 << frame_type))) {
            continue;
        }

        if (unlikely(!ie_reg->data)) {
            tie_println_limited("ERROR: NULL pointer, port %d", ie_reg->port);
            continue;
        }
        len += ie_reg->len;
    }
    read_unlock_irqrestore(&s_lock, flags);

    dev = ven_vap_to_dev(vap);
    if (dev) {
        len += pie_calc_element(dev);
    }

    return len;
    
}


static inline void td_ie_put_data(td_element_t *ele, 
                            unsigned char port,
                            unsigned char *data, 
                            unsigned char data_len)
{
    ele->element_id = TD_IE_VENDOR;
    ele->len = data_len + TD_OUI_LEN + 1;
    memcpy(ele->oui, TD_PRIVATE_OUI, TD_OUI_LEN);
    ele->port = port;
    memcpy(ele->data, data, data_len);

    return;
}

static int _td_update_ie(td_ie_reg_t *ie_reg, unsigned char* data, unsigned char len)
{
    if (ie_reg->data) {
        kfree(ie_reg->data);
        ie_reg->data = NULL;
    }

    ie_reg->data = kzalloc(len + sizeof(td_element_t), GFP_ATOMIC);
    if (!ie_reg->data) {
        tie_println("ERROR: no memory");
        return -ENOMEM;
    }

    td_ie_put_data((td_element_t*)ie_reg->data, ie_reg->port, data, len);
    ie_reg->len = len + sizeof(td_element_t);

    if (ie_reg->frame_types & (1 << TD_PE_FRAME_TYPE_BEACON)) {
        if (ie_reg->vap) {
            td_update_beacon(ie_reg->vap);
        }
    }

    return 0;
}

int td_update_ie(ven_vap_t *vap, unsigned char port, 
                unsigned char* data, unsigned char len)
{
    int ret = 0;
    unsigned long flags = 0;
    td_ie_reg_t *ie_reg = NULL;

    if (!vap || !data || !len) {
        tie_println("ERROR: invalid argument");
        return -EINVAL;
    }

    if (port > TD_MAX_KERNEL_IE) {
        tie_println("ERROR: invalid port %d", port);
        return -EINVAL;
    }

    if (len > U8_MAX - sizeof(td_element_t)) {
        tie_println("ERROR: element too long %d", len);
        return -EINVAL;
    }

    write_lock_irqsave(&s_lock, flags);
    ie_reg = td_search_ie_reg(vap, port);
    if (!ie_reg) {
        tie_println("ERROR: port%d is not registerd", port);
        write_unlock_irqrestore(&s_lock, flags);
        return -EPERM;
    }

    ret = _td_update_ie(ie_reg, data, len);
    write_unlock_irqrestore(&s_lock, flags);
    return ret;
}

int td_clean_vap_ie(ven_vap_t *vap)
{
    int i;
    td_ie_reg_t *ie_reg;
    struct hlist_node *n;
    unsigned long flags = 0;

    if (!vap) {
        tie_println("ERROR: invalid argument");
        return -EINVAL;
    }

    write_lock_irqsave(&s_lock, flags);

    for (i = 0; i < TD_MAX_KERNEL_IE; i++) {
        hlist_for_each_entry_safe(ie_reg, n, &(s_ports[i]), port_list) {
            if (ie_reg->vap == vap) {
                hlist_del(&ie_reg->port_list);
                list_del(&ie_reg->ie_list);

                if (ie_reg->data) {
                    kfree(ie_reg->data);
                }

                kfree(ie_reg);
            }
        }
    }

    write_unlock_irqrestore(&s_lock, flags);

    return 0;
}


int td_unregister_ie(ven_vap_t *vap, unsigned char port)
{
    bool undate_beacon = false;
    unsigned long flags = 0;
    td_ie_reg_t *ie_reg = NULL;

    if (!vap || port > TD_MAX_KERNEL_IE) {
        tie_println("ERROR: invalid argument");
        return -EINVAL;
    }

    write_lock_irqsave(&s_lock, flags);

    ie_reg = td_search_ie_reg(vap, port);
    if (!ie_reg) {
        tie_println("ERROR: port%d is not registerd", port);
        write_unlock_irqrestore(&s_lock, flags);
        return -EPERM;
    }

    if (ie_reg->frame_types & (1 << TD_PE_FRAME_TYPE_BEACON)) {
        undate_beacon = true;
    }

    hlist_del(&ie_reg->port_list);
    list_del(&ie_reg->ie_list);

    if (ie_reg->data) {
        kfree(ie_reg->data);
    }
    kfree(ie_reg);

    write_unlock_irqrestore(&s_lock, flags);

    if (undate_beacon) {\
        printk("[%s][%d]@@@undate_beacon fron here!!",__func__,__LINE__);
        td_update_beacon(vap);
    }

    return 0;
}

int td_register_ie(ven_vap_t *vap, unsigned char port, 
                    unsigned char frame_types, unsigned char* data, 
                    unsigned char len, te_ie_callback_t rx_callback)
{
    int ret = 0;
    unsigned long flags = 0;
    td_ie_reg_t *ie_reg = NULL;

    if (!vap || !data || !len || !rx_callback || !frame_types) {
        tie_println("ERROR: invalid argument");
        return -EINVAL;
    }

    if (port > TD_MAX_KERNEL_IE) {
        tie_println("ERROR: invalid port %d", port);
        return -EINVAL;
    }

    if (len > U8_MAX - sizeof(td_element_t)) {
        tie_println("ERROR: element too long %d", len);
        return -EINVAL;
    }

    write_lock_irqsave(&s_lock, flags);

    ie_reg = td_search_ie_reg(vap, port);
    if (ie_reg) {
        tie_println("WARNING: port %d is already registered", port);
        ret = _td_update_ie(ie_reg, data, len);
        write_unlock_irqrestore(&s_lock, flags);
        return ret;
    }

    ie_reg = kzalloc(sizeof(td_ie_reg_t), GFP_ATOMIC);
    if (!ie_reg) {
        tie_println("ERROR: no memory");
        write_unlock_irqrestore(&s_lock, flags);
        return -ENOMEM;
    }

    ie_reg->data = kzalloc(len + sizeof(td_element_t), GFP_ATOMIC);
    if (!ie_reg->data) {
        tie_println("ERROR: no memory");
        kfree(ie_reg);
        write_unlock_irqrestore(&s_lock, flags);
        return -ENOMEM;
    }

    td_ie_put_data((td_element_t*)ie_reg->data, port, data, len);
    ie_reg->len = len + sizeof(td_element_t);
    ie_reg->frame_types = frame_types;
    ie_reg->port = port;
    ie_reg->vap = vap;
    ie_reg->rx_callback = rx_callback;

    list_add(&ie_reg->ie_list, &s_ie_list);
    hlist_add_head(&ie_reg->port_list, &s_ports[port]);
    write_unlock_irqrestore(&s_lock, flags);

    if (frame_types & (1 << TD_PE_FRAME_TYPE_BEACON)) {
        td_update_beacon(vap);
    }

    return 0;
}

static int td_ie_show(struct seq_file *s, void *data)
{
    unsigned long flags;
    td_ie_reg_t *ie_reg;
    td_ie_reg_t *n;
    struct net_device *dev;
    unsigned char *pos;

    seq_printf(s, "registerd tenda IE informations:\n");
    read_lock_irqsave(&s_lock, flags);
    list_for_each_entry_safe(ie_reg, n, &s_ie_list, ie_list) {
        seq_printf(s, "----------\n");
        seq_printf(s, "port: %d\n", ie_reg->port);
        dev = ven_vap_to_dev(ie_reg->vap);
        seq_printf(s, "interface: %s\n", dev ? dev->name : "(NULL)");
        seq_printf(s, "frame_types:%s%s%s%s%s\n", 
            ie_reg->frame_types & (1 << TD_PE_FRAME_TYPE_BEACON) ? "Beacon " : "",
            ie_reg->frame_types & (1 << TD_PE_FRAME_TYPE_PROBE_REQ) ? "ProbeRequest " : "",
            ie_reg->frame_types & (1 << TD_PE_FRAME_TYPE_PROBE_RES) ? "ProbeResponse " : "",
            ie_reg->frame_types & (1 << TD_PE_FRAME_TYPE_ASSOC_REQ) ? "AssocRequest " : "",
            ie_reg->frame_types & (1 << TD_PE_FRAME_TYPE_ASSOC_RES) ? "AssocResponse " : "");
        seq_printf(s, "data:\n");

        if (!ie_reg->data || ie_reg->len < sizeof(td_element_t)) {
            seq_printf(s, "(NULL)\n");
            continue;
        }

        for (pos = ie_reg->data + sizeof(td_element_t); pos < ie_reg->data + ie_reg->len; pos++) {
            seq_printf(s, "%02x", *pos);
        }
        seq_printf(s, "\n");
    }
    read_unlock_irqrestore(&s_lock, flags);

    seq_printf(s, "Note: read '/proc/private_ie' to get private IEs for applications\n");

    return 0;
}

TD_DECLARE_READ_PROC_FOPS(td_ie_show);


void td_ie_init(wlc_info_t *wlc)
{
    uint16 arqfstbmp = FT2BMP(FC_BEACON) | FT2BMP(FC_PROBE_RESP) | FT2BMP(FC_PROBE_REQ)
                     | FT2BMP(WLC_IEM_FC_SCAN_BCN) | FT2BMP(WLC_IEM_FC_SCAN_PRBRSP) 
                     | FT2BMP(WLC_IEM_FC_AP_BCN);
    //��ֹ2.4G��5G�ظ�ע��proc
    if(false == proc_enable) {
        TD_CREATE_PROC_READ_ENTRY(PROC_NAME, td_ie_show, NULL);
        proc_enable = true;
    }

    //ע���հ������ص�
    wlc_iem_vs_add_parse_fn_mft(wlc->iemi, arqfstbmp, WLC_IEM_VS_IE_TD_PRIVATE_PRIO, td_parse_vs_ie_register, wlc);
    //ע�ᷢ�������ص�
    wlc_iem_vs_add_build_fn_mft(wlc->iemi, arqfstbmp, WLC_IEM_VS_IE_TD_PRIVATE_PRIO, td_calc_ie_register, td_add_ie_register, wlc);

    return;
}

void td_ie_exit(void)
{
    unsigned long flags;
    td_ie_reg_t *ie_reg;
    td_ie_reg_t *n;

    if(true == proc_enable) {
        TD_REMOVE_PROC(PROC_NAME);
        proc_enable = false;
    }

    write_lock_irqsave(&s_lock, flags);
    list_for_each_entry_safe(ie_reg, n, &s_ie_list, ie_list) {
        hlist_del(&ie_reg->port_list);
        list_del(&ie_reg->ie_list);

        if (ie_reg->data) {
            kfree(ie_reg->data);
        }
        kfree(ie_reg);  
    }
    write_unlock_irqrestore(&s_lock, flags);
    
    return;
}
