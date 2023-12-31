/*
 * Common interface to the 802.11 Station Control Block (scb) structure
 *
 * Copyright 2021 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: wlc_scb.c 798155 2021-04-26 17:02:16Z $
 */

/**
 * @file
 * @brief
 * SCB is a per-station data structure that is stored in the wl driver. SCB container provides a
 * mechanism through which different wl driver modules can each allocate and maintain private space
 * in the scb used for their own purposes. The scb subsystem (wlc_scb.c) does not need to know
 * anything about the different modules that may have allocated space in scb. It can also be used
 * by per-port code immediately after wlc_attach() has been done (but before wlc_up()).
 *
 * - "container" refers to the entire space within scb that can be allocated opaquely to other
 *   modules.
 * - "cubby" refers to the per-module private space in the container.
 */

/* XXX: Define wlc_cfg.h to be the first header file included as some builds
 * get their feature flags thru this file.
 */
#include <wlc_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <d11.h>
#include <wlc_rate.h>
#include <wlc_pub.h>
#include <wlc_bsscfg.h>
#include <wlc.h>
#include <wlc_dbg.h>
#include <wlc_scb.h>
#include <wlc_scb_ratesel.h>
#include <wlc_macfltr.h>
#include <wlc_cubby.h>
#include <wlc_dump.h>
#include <wlc_stf.h>
#ifdef WLCFP
#include <wlc_cfp.h>
#endif
#include <wlc_ampdu.h>
#include <wlc_tx.h>
#include <phy_chanmgr_api.h>
#include <wlc_assoc.h>
#include <wlc_event.h>
#include <wlc_event_utils.h>
#ifdef WLTDLS
#include <wlc_tdls.h>
#endif
#ifdef WLTAF
#include <wlc_taf.h>
#endif
#include <d11_cfg.h>
#ifdef DONGLEBUILD
#include <wl_export.h>
#endif
#include <wlc_stamon.h>
#if defined(BCM_PKTFWD)
#include <wl_pktfwd.h>
#endif /* BCM_PKTFWD */
#include <wlc_fifo.h>
#ifdef CONFIG_TENDA_PRIVATE_KM
#ifdef VLAN_VID_MASK
#undef VLAN_VID_MASK
#endif
#include <net/km_common.h>
#include <wl_linux.h>
#endif
#ifdef CONFIG_TENDA_PRIVATE_WLAN
#include "td_debug.h"
#include <wlc_iocv.h>
#include <wlc_wnm.h>
#include <wlc_lq.h>
#endif
#ifdef WL_MU_TX
#include <wlc_mutx.h>
#endif

#if defined(AI_MESH_SUPPORT) && defined(CONFIG_TENDA_PRIVATE_WLAN)
#include "ai_mesh.h"
#endif


#define SCB_MAGIC 0x0505a5a5u

typedef struct scb_lookup {
    uint8    incarnation;        /**< Current incarnation index */
    uint16    incarn_mismatch;    /**< incarnation mimsatch counter */
    struct    scb* scb_ptr;        /**< Stored SCB ptr */
} scb_lookup_t;

/** structure for storing public and private global scb module state */
struct scb_module {
    wlc_info_t    *wlc;            /**< global wlc info handle */
    uint16        nscb;            /**< total number of allocated scbs */
    uint16        scbtotsize;        /**< total scb size including scb_info */
    uint16        scbpubsize;        /**< struct scb size */
    wlc_cubby_info_t *cubby_info;
    int        cfgh;            /**< scb bsscfg cubby handle */
    bcm_notif_h    scb_state_notif_hdl;    /**< scb state notifier handle. */
    void        *user_flowid_allocator; /**< user scb flowid allocator */
    void        *int_flowid_allocator;  /**< internal scb flowid allocator */
    scb_lookup_t    *scb_lkp;        /**< FLOWID - SCB Lookup table */
    uint16        *amt_lookup;        /**< AMT - SCB flowid Lookup table */
#ifdef SCB_MEMDBG
    uint32        scballoced;        /**< how many scb calls to 'wlc_scb_allocmem' */
    uint32        scbfreed;        /**< how many scb calls to 'wlc_scb_freemem' */
    uint32        freelistcount;        /**< how many scb's are in free_list */
#endif /* SCB_MEMDBG */
};

/** station control block - one per remote MAC address */
struct scb_info {
    struct scb_info *hashnext;    /**< pointer to next scb under same hash entry */
    struct scb_info    *next;        /**< pointer to next allocated scb */
    uint8 *secbase;            /* secondary cubby allocation base pointer */
#ifdef SCB_MEMDBG
    struct scb_info *hashnext_copy;
    struct scb_info *next_copy;
    uint32 magic;
#endif
};

#define SCB_FLOWID_LKP(scbstate, flowid)    ((((scbstate)->scb_lkp)[flowid]).scb_ptr)

#define SCB_FLOWID_INCARN_LKP(scbstate, flowid)    (((scbstate)->scb_lkp[flowid]).incarnation)
#define SCB_FLOWID_INCARN_MISMATCH(scbstate, flowid) \
    (((scbstate)->scb_lkp[flowid]).incarn_mismatch)
#define SCB_FLOWID_AMT_LKP(scbstate, amt_id)    ((scbstate)->amt_lookup[amt_id])

#define SCB_AMT_LKP(scbstate, amt_id)    \
    ({\
        uint16 lcl_flowid = SCB_FLOWID_AMT_LKP((scbstate), amt_id); \
        ((SCB_FLOWID_VALID(lcl_flowid)) ? SCB_FLOWID_LKP((scbstate), lcl_flowid) : NULL); \
    })

#define AMT_IDX_VALID(wlc, idx) \
    (((idx) >= 0) && ((idx) < AMT_SIZE((wlc)->pub->corerev)))

#define ASSERT_AMT_IDX(wlc, idx)    ASSERT(AMT_IDX_VALID((wlc), (idx)))

static void wlc_scb_hash_add(scb_module_t *scbstate, wlc_bsscfg_t *cfg, enum wlc_bandunit bandunit,
    struct scb *scb);
static void wlc_scb_hash_del(scb_module_t *scbstate, wlc_bsscfg_t *cfg,
    struct scb *scbd);
static void wlc_scb_list_add(scb_module_t *scbstate, wlc_bsscfg_t *cfg,
    struct scb *scb);
static void wlc_scb_list_del(scb_module_t *scbstate, wlc_bsscfg_t *cfg,
    struct scb *scbd);

static struct scb *wlc_scbvictim(wlc_info_t *wlc);

static int wlc_scbinit(wlc_info_t *wlc, wlc_bsscfg_t *cfg, enum wlc_bandunit bandunit,
    struct scb *scb);
static void wlc_scbdeinit(wlc_info_t *wlc, struct scb *scbd);

static struct scb_info *wlc_scb_allocmem(scb_module_t *scbstate);
static void wlc_scb_freemem(scb_module_t *scbstate, struct scb_info *scbinfo);

static void wlc_scb_init_rates(wlc_info_t *wlc, wlc_bsscfg_t *cfg, enum wlc_bandunit bandunit,
    struct scb *scb);

static int wlc_scb_flowid_attach(wlc_info_t* wlc, scb_module_t *scbstate);
static void wlc_scb_flowid_detach(wlc_info_t* wlc, scb_module_t *scbstate);
static int wlc_scb_flowid_init(wlc_info_t* wlc, scb_module_t *scbstate, struct scb* scb);
static void wlc_scb_flowid_deinit(wlc_info_t* wlc, scb_module_t *scbstate, struct scb* scb);
static int wlc_scb_flowid_dump(void * ctx, struct bcmstrbuf *b);
#ifdef BCMPCIEDEV
static void wlc_scb_flow_ring_delink(wlc_info_t* wlc, struct scb* scb);
#endif /* BCMPCIEDEV */

#ifdef WLSCB_HISTO
static void wlc_scb_histo_mem_free(wlc_info_t *wlc, struct scb *scb);
#endif /* WLSCB_HISTO */
#if defined(BCMDBG)
static int wlc_scb_dump(wlc_info_t *wlc, struct bcmstrbuf *b);
/** SCB Flags Names Initialization */
static const bcm_bit_desc_t scb_flags[] =
{
    {SCB_NONERP, "NonERP"},
    {SCB_LONGSLOT, "LgSlot"},
    {SCB_SHORTPREAMBLE, "ShPre"},
    {SCB_8021XHDR, "1X"},
    {SCB_WPA_SUP, "WPASup"},
    {SCB_DEAUTH, "DeA"},
    {SCB_WMECAP, "WME"},
    {SCB_BRCM, "BRCM"},
    {SCB_WDS_LINKUP, "WDSLinkUP"},
    {SCB_LEGACY_AES, "LegacyAES"},
    {SCB_MYAP, "MyAP"},
    {SCB_PENDING_PROBE, "PendingProbe"},
    {SCB_AMSDUCAP, "AMSDUCAP"},
    {SCB_HTCAP, "HT"},
    {SCB_RECV_PM, "RECV_PM"},
    {SCB_AMPDUCAP, "AMPDUCAP"},
    {SCB_IS40, "40MHz"},
    {SCB_NONGF, "NONGFCAP"},
    {SCB_APSDCAP, "APSDCAP"},
    {SCB_PENDING_PSPOLL, "PendingPSPoll"},
    {SCB_RIFSCAP, "RIFSCAP"},
    {SCB_HT40INTOLERANT, "40INTOL"},
    {SCB_WMEPS, "WMEPSOK"},
    {SCB_COEX_MGMT, "OBSSCoex"},
    {SCB_IBSS_PEER, "IBSS Peer"},
    {SCB_STBCCAP, "STBC"},
    {SCB_DTPCCAP, "DTPC"},
    {0, NULL}
};
static const bcm_bit_desc_t scb_flags2[] =
{
    {SCB2_SGI20_CAP, "SGI20"},
    {SCB2_SGI40_CAP, "SGI40"},
    {SCB2_RX_LARGE_AGG, "LGAGG"},
#ifdef BCMWAPI_WAI
    {SCB2_WAIHDR, "WAI"},
#endif /* BCMWAPI_WAI */
    {SCB2_LDPCCAP, "LDPC"},
    {SCB2_VHTCAP, "VHT"},
    {SCB2_AMSDU_IN_AMPDU_CAP, "AGG^2"},
    {SCB2_P2P, "P2P"},
    {SCB2_DWDS_ACTIVE, "DWDS_ACTIVE"},
    {SCB2_HECAP, "HE"},
    {0, NULL}
};
static const bcm_bit_desc_t scb_flags3[] =
{
    {SCB3_A4_DATA, "A4_DATA"},
    {SCB3_A4_NULLDATA, "A4_NULLDATA"},
    {SCB3_A4_8021X, "A4_8021X"},
    {SCB3_DWDS_CAP, "DWDS_CAP"},
    {SCB3_1024QAM_CAP, "1024QAM_CAP"},
    {SCB3_VHTMU, "VHTMU"},
    {SCB3_HEMMU, "HEMMU"},
    {SCB3_DLOFDMA, "DLOFDMA"},
    {SCB3_ULOFDMA, "ULOFDMA"},
    {SCB3_ULMMU, "ULMMU"},
    {SCB3_MAP_CAP, "MAP_CAP"},
    {0, NULL}
};
static const bcm_bit_desc_t scb_states[] =
{
    {AUTHENTICATED, "AUTH"},
    {ASSOCIATED, "ASSOC"},
    {PENDING_AUTH, "AUTH_PEND"},
    {PENDING_ASSOC, "ASSOC_PEND"},
    {AUTHORIZED, "AUTH_8021X"},
    {0, NULL}
};
#endif

/* Each scb has the layout:
 * +-----------------+ <<== offset 0
 * | struct scb      |
 * +-----------------+ <<== scbpubsize
 * | struct scb_info |
 * +-----------------+ <<== scbtotsize
 * | cubbies         |
 * +-----------------+
 */
#define SCBINFO(_scbstate, _scb) \
    (_scb ? (struct scb_info *)((uint8 *)(_scb) + (_scbstate)->scbpubsize) : NULL)
#define SCBPUB(_scbstate, _scbinfo) \
    (_scbinfo ? (struct scb *)((uint8 *)(_scbinfo) - (_scbstate)->scbpubsize) : NULL)

#ifdef SCB_MEMDBG

#define SCBSANITYCHECK(_scbstate, _scb) \
    if (((_scb) != NULL) &&    \
        ((SCBINFO(_scbstate, _scb)->magic != SCB_MAGIC) || \
         (SCBINFO(_scbstate, _scb)->hashnext != SCBINFO(_scbstate, _scb)->hashnext_copy) || \
         (SCBINFO(_scbstate, _scb)->next != SCBINFO(_scbstate, _scb)->next_copy))) { \
        WL_ERROR(("scbinfo corrupted: magic: 0x%x hn: %p hnc: %p n: %p nc: %p\n", \
                  SCBINFO(_scbstate, _scb)->magic, \
              OSL_OBFUSCATE_BUF(SCBINFO(_scbstate, _scb)->hashnext), \
                  OSL_OBFUSCATE_BUF(SCBINFO(_scbstate, _scb)->hashnext_copy), \
                  OSL_OBFUSCATE_BUF(SCBINFO(_scbstate, _scb)->next), \
              OSL_OBFUSCATE_BUF(SCBINFO(_scbstate, _scb)->next_copy))); \
        ASSERT(0); \
    }

#define SCBBSSCFGSANITYCHECK(_scb, _bsscfg)    \
    if ((_scb != NULL) &&    \
        (SCB_BSSCFG(_scb) != _bsscfg)) { \
        WL_ERROR(("scb->bsscfg (%p) corrupted, should be (%p)\n", \
            OSL_OBFUSCATE_BUF(SCB_BSSCFG(_scb)), \
            OSL_OBFUSCATE_BUF(_bsscfg))); \
        ASSERT(0); \
    }

void
scb_sanitycheck(scb_module_t *scbstate, scb_t* scb)
{
    SCBSANITYCHECK(scbstate, scb);
}

#else

#define SCBSANITYCHECK(_scbstate, _scb)        do {} while (0)
#define SCBBSSCFGSANITYCHECK(_scb, _bsscfg)    do {} while (0)
#endif /* SCB_MEMDBG */

/** bsscfg cubby */
typedef struct scb_bsscfg_cubby {
    struct scb    **scbhash[MAXBANDS];    /**< scb hash table */
    uint32        nscbhash;        /**< scb hash table size */
    struct scb    *scb;            /**< station control block link list */
} scb_bsscfg_cubby_t;

#define SCB_BSSCFG_CUBBY(ss, cfg) ((scb_bsscfg_cubby_t *)BSSCFG_CUBBY(cfg, (ss)->cfgh))

static int wlc_scb_bsscfg_init(void *context, wlc_bsscfg_t *cfg);
static void wlc_scb_bsscfg_deinit(void *context, wlc_bsscfg_t *cfg);
#if defined(BCMDBG)
static void wlc_scb_bsscfg_dump(void *context, wlc_bsscfg_t *cfg, struct bcmstrbuf *b);
#else
#define wlc_scb_bsscfg_dump NULL
#endif

static void wlc_scb_bsscfg_scbclear(struct wlc_info *wlc, wlc_bsscfg_t *cfg, bool perm);

/* # scb hash buckets */
#if defined(DONGLEBUILD)
#define SCB_HASH_N    (uint8)((wlc->pub->tunables->maxscb + 7) >> 3)
#define    SCBHASHINDEX(hash, ea)    (((ea)[3] ^ (ea)[4] ^ (ea)[5]) % (hash))
#else /* !DONGLEBUILD */
#define SCB_HASH_N    256
#define    SCBHASHINDEX(hash, ea) ((ea)[3] ^ (ea)[4] ^ (ea)[5])
#endif /* DONGLEBUILD */
#define SCB_HASH_SZ    (sizeof(struct scb *) * MAXBANDS * SCB_HASH_N)

#if defined(BCMDBG)
extern struct ether_addr wl_msg_macfltr[];
#endif /* BCMDBG */

static int
wlc_scb_bsscfg_init(void *context, wlc_bsscfg_t *cfg)
{
    scb_module_t *scbstate = (scb_module_t *)context;
    wlc_info_t *wlc = scbstate->wlc;
    scb_bsscfg_cubby_t *scb_cfg = SCB_BSSCFG_CUBBY(scbstate, cfg);
    struct scb **scbhash;
    uint32 i;

    ASSERT(scb_cfg->scbhash[0] == NULL);

    scbhash = MALLOCZ(wlc->osh, SCB_HASH_SZ);
    if (scbhash == NULL) {
        WL_ERROR((WLC_BSS_MALLOC_ERR, WLCWLUNIT(wlc), WLC_BSSCFG_IDX(cfg), __FUNCTION__,
            (int)SCB_HASH_SZ, MALLOCED(wlc->osh)));
        return BCME_NOMEM;
    }

    scb_cfg->nscbhash = SCB_HASH_N;    /* # scb hash buckets */
    for (i = 0; i < MAXBANDS; i++) {
        scb_cfg->scbhash[i] = scbhash + (i * SCB_HASH_N);
    }

    return BCME_OK;
}
#ifdef CONFIG_TENDA_PRIVATE_WLAN
#ifdef CONFIG_TENDA_WLAN_REPEATER
#define SIG_LINKUP      SIGUSR1    /* Send linkup notification to wserver */
#define SIG_LINKDOWN    SIGUSR2    /* Send linkdown notification to wserver */
extern int kill_proc_info(int sig, struct siginfo *info, pid_t pid);
bool wl_is_client_mode(wlc_info_t *wlc)
{    
    int err = 0;    
    int apsta = 0, wet = 0;    
    err = wlc_iovar_getint(wlc, "apsta", &apsta);
    if (err) {        
        WL_ERROR(("wl get apsta error %d\n", err));
        return FALSE;    
    }    
    if (apsta) {        
        return TRUE;    
    }    
    err = wlc_ioctl(wlc, WLC_GET_WET, &wet, sizeof(wet), NULL);
    if (err) {        
        WL_ERROR(("wl get wet error %d\n", err));
        return FALSE;    
    }    
    if (wet) {        
        return TRUE;    
    }    
    
    return FALSE;
}

static int kill_proc_to_wserver(wlc_info_t *wlc, int sig)
{    
    int err = 0;    
    int wserver_pid = 0;    
    
    err = wlc_iovar_getint(wlc, "wserver_pid", &wserver_pid);
    if (err) {        
        WL_ERROR(("wl get wserver pid error %d\n", err));
        return BCME_ERROR;    
    }    
    
    if (wserver_pid > 0) {        
        kill_proc_info(sig, SEND_SIG_PRIV, wserver_pid);
    } else {        WL_ERROR(("wserver pid error: %d\n", wserver_pid));
        return BCME_ERROR;    
    }    

    return BCME_OK;
    }
#endif

static void td_sta_online(wlc_info_t *wlc, struct scb *scb, uint8 add_state)
{
    wlc_bsscfg_t *bsscfg = SCB_BSSCFG(scb);
    bool wpa_enable = FALSE;
    bool sta_online = FALSE;
#ifdef CONFIG_TENDA_WLAN_NODE_ROAMING
    wlc_assoc_t *as = scb->bsscfg->assoc;
    uint type = as->type;
#endif /* CONFIG_TENDA_WLAN_NODE_ROAMING */

    /* internal virtual scb, not real station */
    if (SCB_INTERNAL(scb)) {
        return ;
    }

    wpa_enable = (0 == bsscfg->WPA_auth)? FALSE : TRUE;

    if (wpa_enable) {
        if (!SCB_AUTHORIZED(scb) && (add_state & AUTHORIZED)) {
            sta_online = TRUE;
        }
    }else {
        if (!SCB_ASSOCIATED(scb) && (add_state & ASSOCIATED)) {
            sta_online = TRUE;
        }
    }

    if (sta_online) {
#ifdef CONFIG_TENDA_PRIVATE_KM
        if (km_wireless_client_online) {
            km_wireless_client_online(scb->ea.octet, bsscfg->wlc->pub->unit, bsscfg->wlcif->wlif->dev, bsscfg->SSID, 
bsscfg->SSID_len);
        }
#endif

    scb->last_time = jiffies;

#ifdef TD_ISP_WIFIEVENT
        wifi_ev_sta_join_handler(scb->ea.octet, scb->bsscfg->wlcif->wlif->dev->name,
                                wlc_lq_rssi_get(wlc, scb->bsscfg, scb), WLCWLUNIT(wlc),
                                SCB_RRM(scb), wlc_wnm_get_scbcap(wlc, scb));
#endif

#ifdef CONFIG_TENDA_PRIVATE_KM
        if (km_eventscenter_wifiev_sta_join_handler) {
            km_eventscenter_wifiev_sta_join_handler(scb->ea.octet, scb->bsscfg->wlcif->wlif->dev->name, 
                wlc_lq_rssi_get(wlc, scb->bsscfg, scb), WLCWLUNIT(wlc), SCB_RRM(scb), wlc_wnm_get_scbcap(wlc, scb));
        }
#endif

#ifdef CONFIG_TENDA_WLAN_REPEATER
        if (wl_is_client_mode(wlc) && (0 == WLC_BSSCFG_IDX(bsscfg))
#ifdef CONFIG_TENDA_WLAN_NODE_ROAMING
            && (type != AS_ROAM)
#endif
        ) {
            if (kill_proc_to_wserver(wlc, SIG_LINKUP)) {
                WL_ERROR(("kill_proc_to_wserver failed!\n"));
            }
        }
#endif

        TDP_INFO("wl%d.%d STA++(now:%d) %pM", bsscfg->wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), 
            bsscfg->wlc->scbstate->nscb, &scb->ea);

#if defined(CONFIG_TENDA_PRIVATE_WLAN) && defined(CONFIG_TENDA_WLAN_AUTOHIDE_SSID)        /* Hidden SSID */
        if ((wlc_bss_assocscb_getcnt(wlc, bsscfg) + 1 >= bsscfg->maxassoc) && (SCB_AUTO_HIDE_ON == bsscfg->td_auto_hide)) {
            if (false == bsscfg->closednet) {
                bsscfg->closednet = true;
                bsscfg->td_auto_hide = SCB_AUTO_HIDE;
            } else if (true == bsscfg->closednet) {
                bsscfg->td_auto_hide = SCB_USER_HIDE;
            }
        }
#endif /* CONFIG_TENDA_PRIVATE_WLAN and CONFIG_TENDA_WLAN_AUTOHIDE_SSIDIDE_SSID */
    }
}

void td_sta_offline(wlc_info_t *wlc, struct scb *scb, uint8 rm_state)
{
    wlc_bsscfg_t *bsscfg = SCB_BSSCFG(scb);
    bool wpa_enable = FALSE;
    bool sta_offline = FALSE;
#ifdef CONFIG_TENDA_WLAN_NODE_ROAMING
    wlc_assoc_t *as = scb->bsscfg->assoc;
    uint type = as->type;
#endif /* CONFIG_TENDA_WLAN_NODE_ROAMING */

    /* internal virtual scb, not real station */
    if (SCB_INTERNAL(scb)) {
        return ;
    }


    wpa_enable = (0 == bsscfg->WPA_auth)? FALSE : TRUE;

    if (wpa_enable) {
        if (SCB_AUTHORIZED(scb) && (rm_state & AUTHORIZED)) {
            sta_offline = TRUE;
        }
    }else {
        if (SCB_ASSOCIATED(scb) && (rm_state & ASSOCIATED)) {
            sta_offline = TRUE;
        }
    }


    if (sta_offline) {
#ifdef CONFIG_TENDA_PRIVATE_KM
        if (km_wireless_client_offline) {
            km_wireless_client_offline(scb->ea.octet, scb->bsscfg->wlcif->wlif->dev);
        }

        char ifname[IFNAMSIZ] = {0};
        snprintf(ifname, IFNAMSIZ, "wlan%d", wlc->pub->unit);

        if (km_eventscenter_wifiev_sta_status_handler) {
            km_eventscenter_wifiev_sta_status_handler(scb->ea.octet, ifname, KM_WIRELESS_STA_OFFLIN);
        }
#endif

        scb->last_time = jiffies;

#ifdef CONFIG_TENDA_PRIVATE_KM
        if (km_eventscenter_wifiev_sta_leave_handler) {
            km_eventscenter_wifiev_sta_leave_handler(scb->ea.octet);
        }
#endif


#ifdef CONFIG_TENDA_WLAN_REPEATER
        if (wl_is_client_mode(wlc) && (0 == WLC_BSSCFG_IDX(bsscfg))
#ifdef CONFIG_TENDA_WLAN_NODE_ROAMING
            && (type != AS_ROAM)
#endif
        ) {
            if (kill_proc_to_wserver(wlc, SIG_LINKDOWN)) {
                WL_ERROR(("kill_proc_to_wserver failed!\n"));
            }
        }
#endif

        /* STA will be delete soon, so num-1 */
        TDP_INFO("wl%d.%d STA--(now:%d) %pM, online %d senconds", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), 
            wlc->scbstate->nscb - 1, &scb->ea, wlc->pub->now - scb->assoctime);

#if defined(CONFIG_TENDA_PRIVATE_WLAN) && defined(CONFIG_TENDA_WLAN_AUTOHIDE_SSID)        /* Display SSID */
        if ((wlc_bss_assocscb_getcnt(wlc, bsscfg) <= (bsscfg->maxassoc)) && (SCB_AUTO_HIDE == bsscfg->td_auto_hide)) {
            bsscfg->closednet = false;
            bsscfg->td_auto_hide = SCB_AUTO_HIDE_ON;
        }
#endif /* CONFIG_TENDA_PRIVATE_WLAN and CONFIG_TENDA_WLAN_AUTOHIDE_SSIDIDE_SSID */
    }
}
#endif

static void
wlc_scb_bsscfg_deinit(void *context, wlc_bsscfg_t *cfg)
{
    scb_module_t *scbstate = (scb_module_t *)context;
    wlc_info_t *wlc = scbstate->wlc;
    scb_bsscfg_cubby_t *scb_cfg = SCB_BSSCFG_CUBBY(scbstate, cfg);
    uint32 i;

    /* clear all scbs */
    wlc_scb_bsscfg_scbclear(wlc, cfg, TRUE);

    scb_cfg->nscbhash = 0;

    ASSERT(scb_cfg->scbhash[0] != NULL);

    /* N.B.: the hash is contiguously allocated across multiple bands */
    MFREE(wlc->osh, scb_cfg->scbhash[0], SCB_HASH_SZ);
    scb_cfg->scbhash[0] = NULL;

    scb_cfg->nscbhash = 0;
    for (i = 0; i < MAXBANDS; i++) {
        scb_cfg->scbhash[i] = NULL;
    }
}

void
wlc_scb_bss_state_upd(void *ctx, bsscfg_state_upd_data_t *evt_data)
{
    scb_module_t *scbstate = (scb_module_t *)ctx;
    wlc_info_t *wlc = scbstate->wlc;
    wlc_bsscfg_t *cfg = evt_data->cfg;

    /* clear all old non-permanent scbs for IBSS only if assoc recreate is not enabled */
    /* and WLC_BSSCFG_PRESERVE cfg flag is not set */
    if (!evt_data->old_up && cfg->up) {
        if (BSSCFG_IBSS(cfg) &&
            !(ASSOC_RECREATE_ENAB(wlc->pub) && (cfg->flags & WLC_BSSCFG_PRESERVE))) {
            wlc_scb_bsscfg_scbclear(wlc, cfg, FALSE);
        }
    }
    /* clear all non-permanent scbs when disabling the bsscfg */
    else if (evt_data->old_enable && !cfg->enable) {
        wlc_scb_bsscfg_scbclear(wlc, cfg, FALSE);
    }
}

/* # scb cubby registry entries */
#define SCB_CUBBY_REG_N  (wlc->pub->tunables->maxscbcubbies)
#define SCB_CUBBY_REG_SZ (sizeof(scb_module_t))

scb_module_t *
BCMATTACHFN(wlc_scb_attach)(wlc_info_t *wlc)
{
    scb_module_t *scbstate;

    if ((scbstate = MALLOCZ(wlc->osh, SCB_CUBBY_REG_SZ)) == NULL) {
        WL_ERROR((WLC_MALLOC_ERR, WLCWLUNIT(wlc), __FUNCTION__, (int)SCB_CUBBY_REG_SZ,
            MALLOCED(wlc->osh)));
        goto fail;
    }
    scbstate->wlc = wlc;

    if ((scbstate->cubby_info = wlc_cubby_attach(wlc->osh, wlc->pub->unit,
                                                 SCB_CUBBY_REG_N)) == NULL) {
        WL_ERROR(("wl%d: %s: wlc_cubby_attach failed\n",
            wlc->pub->unit, __FUNCTION__));
        goto fail;
    }

    /* reserve cubby in the bsscfg container for per-bsscfg private data */
    if ((scbstate->cfgh = wlc_bsscfg_cubby_reserve(wlc, sizeof(scb_bsscfg_cubby_t),
            wlc_scb_bsscfg_init, wlc_scb_bsscfg_deinit, wlc_scb_bsscfg_dump,
            (void *)scbstate)) < 0) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_cubby_reserve failed\n",
            wlc->pub->unit, __FUNCTION__));
        goto fail;
    }

    scbstate->scbpubsize = (uint16)sizeof(struct scb);
    scbstate->scbtotsize = scbstate->scbpubsize;
    scbstate->scbtotsize += (uint16)sizeof(struct scb_info);

    /* Flowid allocator and lookup table */
    if (wlc_scb_flowid_attach(wlc, scbstate) != BCME_OK) {
        WL_ERROR(("wl%d: %s: SCB flowid attach failed.\n",
            wlc->pub->unit, __FUNCTION__));
        goto fail;
    }

#if defined(BCMDBG)
    wlc_dump_register(wlc->pub, "scb", (dump_fn_t)wlc_scb_dump, (void *)wlc);
#endif

    wlc_dump_add_fns(wlc->pub, "scb_flows", wlc_scb_flowid_dump, NULL, (void *)wlc);

    /* create notification list for scb state change. */
    if (bcm_notif_create_list(wlc->notif, &scbstate->scb_state_notif_hdl) != BCME_OK) {
        WL_ERROR(("wl%d: %s: scb bcm_notif_create_list() failed\n",
            wlc->pub->unit, __FUNCTION__));
        goto fail;
    }

    return scbstate;

fail:
    MODULE_DETACH(scbstate, wlc_scb_detach);
    return NULL;
} /* wlc_scb_attach */

void
BCMATTACHFN(wlc_scb_detach)(scb_module_t *scbstate)
{
    wlc_info_t *wlc;

    if (!scbstate)
        return;

    wlc = scbstate->wlc;

    if (scbstate->scb_state_notif_hdl != NULL)
        bcm_notif_delete_list(&scbstate->scb_state_notif_hdl);

    (void)wlc_bsscfg_state_upd_unregister(wlc, wlc_scb_bss_state_upd, scbstate);

    wlc_cubby_detach(scbstate->cubby_info);

    /* SCB flowid detach */
    wlc_scb_flowid_detach(wlc, scbstate);

    MFREE(wlc->osh, scbstate, SCB_CUBBY_REG_SZ);
}

/* Methods for iterating along a list of scb */

/**
 * Direct access to the next
 * @param scbstate   This parameter was added to reduce scb size by 2 pointers
 */
static struct scb *
wlc_scb_next(scb_module_t *scbstate, struct scb *scb)
{
    if (scb) {
        struct scb_info *scbinfo = SCBINFO(scbstate, scb);
        SCBSANITYCHECK(scbstate, scb);
        return SCBPUB(scbstate, scbinfo->next);
    }
    return NULL;
}

static struct wlc_bsscfg *
wlc_scb_next_bss(scb_module_t *scbstate, int idx)
{
    wlc_info_t *wlc = scbstate->wlc;
    wlc_bsscfg_t *next_bss = NULL;

    /* get next bss walking over hole */
    while (idx < WLC_MAXBSSCFG) {
        next_bss = WLC_BSSCFG(wlc, idx);
        if (next_bss != NULL)
            break;
        idx++;
    }
    return next_bss;
}

/** Initialize an iterator keeping memory of the next scb as it moves along the list */
void
wlc_scb_iterinit(scb_module_t *scbstate, struct scb_iter *scbiter, wlc_bsscfg_t *bsscfg)
{
    scb_bsscfg_cubby_t *scb_cfg;
    ASSERT(scbiter != NULL);

    if (bsscfg == NULL) {
        /* walk scbs of all bss */
        scbiter->all = TRUE;
        scbiter->next_bss = wlc_scb_next_bss(scbstate, 0);
        if (scbiter->next_bss == NULL) {
            /* init next scb pointer also to null */
            scbiter->next = NULL;
            return;
        }
    } else {
        /* walk scbs of specified bss */
        scbiter->all = FALSE;
        scbiter->next_bss = bsscfg;
    }

    ASSERT(scbiter->next_bss != NULL);
    scb_cfg = SCB_BSSCFG_CUBBY(scbstate, scbiter->next_bss);
    SCBSANITYCHECK(scbstate, scb_cfg->scb);
    SCBBSSCFGSANITYCHECK(scb_cfg->scb, scbiter->next_bss);

    /* Prefetch next scb, so caller can free an scb before going on to the next */
    scbiter->next = scb_cfg->scb;
}

/** move the iterator */
struct scb *
wlc_scb_iternext(scb_module_t *scbstate, struct scb_iter *scbiter)
{
    scb_bsscfg_cubby_t *scb_cfg;
    struct scb *scb;

    ASSERT(scbiter != NULL);

    while (scbiter->next_bss) {

        /* get the next scb in the current bsscfg */
        if ((scb = scbiter->next) != NULL) {
            /* get next scb of bss */
            scbiter->next = wlc_scb_next(scbstate, scb);
            SCBBSSCFGSANITYCHECK(scb, scbiter->next_bss);
            return scb;
        }

        /* get the next bsscfg if we have run out of scbs in the current bsscfg */
        if (scbiter->all) {
            scbiter->next_bss =
                    wlc_scb_next_bss(scbstate, WLC_BSSCFG_IDX(scbiter->next_bss) + 1);
            if (scbiter->next_bss != NULL) {
                scb_cfg = SCB_BSSCFG_CUBBY(scbstate, scbiter->next_bss);
                scbiter->next = scb_cfg->scb;
            }
        } else {
            scbiter->next_bss = NULL;
        }
    }

    /* done with all bsscfgs and scbs */
    scbiter->next = NULL;

    return NULL;
}

#ifdef BCMDBG
/* undefine the BCMDBG helper macros so they will not interfere with the function definitions */
#undef wlc_scb_cubby_reserve
#undef wlc_scb_cubby_reserve_ext
#endif

/**
 * Reduced parameter version of wlc_scb_cubby_reserve_ext().
 *
 * Return value: negative values are errors, non-negative are cubby offsets
 */
#ifdef BCMDBG
int
BCMATTACHFN(wlc_scb_cubby_reserve)(wlc_info_t *wlc, uint size, scb_cubby_init_t fn_init,
    scb_cubby_deinit_t fn_deinit, scb_cubby_dump_t fn_dump, void *context, const char *func)
#else
int
BCMATTACHFN(wlc_scb_cubby_reserve)(wlc_info_t *wlc, uint size, scb_cubby_init_t fn_init,
    scb_cubby_deinit_t fn_deinit, scb_cubby_dump_t fn_dump, void *context)
#endif /* BCMDBG */
{
    scb_cubby_params_t params;
    int ret;

    bzero(&params, sizeof(params));

    params.fn_init = fn_init;
    params.fn_deinit = fn_deinit;
    params.fn_dump = fn_dump;
    params.context = context;

#ifdef BCMDBG
    ret = wlc_scb_cubby_reserve_ext(wlc, size, &params, func);
#else
    ret = wlc_scb_cubby_reserve_ext(wlc, size, &params);
#endif
    return ret;
}

/**
 * Multiple modules have the need of reserving some private data storage related to a specific
 * communication partner. During ATTACH time, this function is called multiple times, typically one
 * time per module that requires this storage. This function does not allocate memory, but
 * calculates values to be used for a future memory allocation by wlc_scb_allocmem() instead.
 *
 * Return value: negative values are errors, non-negative are cubby offsets.
 */
#ifdef BCMDBG
int
BCMATTACHFN(wlc_scb_cubby_reserve_ext)(wlc_info_t *wlc, uint size, scb_cubby_params_t *params,
    const char *func)
#else
int
BCMATTACHFN(wlc_scb_cubby_reserve_ext)(wlc_info_t *wlc, uint size, scb_cubby_params_t *params)
#endif /* BCMDBG */
{
    scb_module_t *scbstate = wlc->scbstate;
    wlc_cubby_fn_t fn;
    int offset;

    ASSERT(scbstate->nscb == 0);

    bzero(&fn, sizeof(fn));
    fn.fn_init = (cubby_init_fn_t)params->fn_init;
    fn.fn_deinit = (cubby_deinit_fn_t)params->fn_deinit;
    fn.fn_secsz = (cubby_secsz_fn_t)params->fn_secsz;
    fn.fn_dump = (cubby_dump_fn_t)params->fn_dump;
#ifdef BCMDBG
    fn.name = func;
#endif

    offset = wlc_cubby_reserve(scbstate->cubby_info, size, &fn, params->context);

    if (offset < 0) {
        WL_ERROR(("wl%d: %s: wlc_cubby_reserve failed with err %d\n",
                  wlc->pub->unit, __FUNCTION__, offset));
        return offset;
    }

    return (int)scbstate->scbtotsize + offset;
} /* wlc_scb_cubby_reserve_ext */

struct wlcband *
wlc_scbband(wlc_info_t *wlc, struct scb *scb)
{
    return wlc->bandstate[scb->bandunit];
}

static void
wlc_scb_reset(scb_module_t *scbstate, struct scb_info *scbinfo)
{
    struct scb *scb = SCBPUB(scbstate, scbinfo);
    uint len;

    len = scbstate->scbtotsize + wlc_cubby_totsize(scbstate->cubby_info);
    bzero(scb, len);

#ifdef SCB_MEMDBG
    scbinfo->magic = SCB_MAGIC;
#endif
}

/**
 * After all the modules indicated how much cubby space they need in the scb, the actual scb can be
 * allocated. This happens one time fairly late within the attach phase, but also when e.g.
 * communication with a new remote party is started.
 */
static struct scb_info *
wlc_scb_allocmem(scb_module_t *scbstate)
{
    wlc_info_t *wlc = scbstate->wlc;
    struct scb_info *scbinfo;
    struct scb *scb;
    uint len;

    /* Make sure free_mem never gets below minimum threshold due to scb_allocs */
    if (OSL_MEM_AVAIL() <= (uint)wlc->pub->tunables->min_scballoc_mem) {
        WL_ERROR(("wl%d: %s: low memory. %d bytes left.\n",
                  wlc->pub->unit, __FUNCTION__, OSL_MEM_AVAIL()));
        return NULL;
    }

    len = scbstate->scbtotsize + wlc_cubby_totsize(scbstate->cubby_info);
    /* Per the above comment, can be freed dynamically */
    scb = MALLOCZ_NOPERSIST(wlc->osh, len);
    if (scb == NULL) {
        WL_ERROR((WLC_MALLOC_ERR, WLCWLUNIT(wlc), __FUNCTION__,
                  len, MALLOCED(wlc->osh)));
        return NULL;
    }

    scbinfo = SCBINFO(scbstate, scb);

#ifdef SCB_MEMDBG
    scbstate->scballoced++;
#endif /* SCB_MEMDBG */

    return scbinfo;
}

#define _wlc_internalscb_free(wlc, scb) wlc_scbfree(wlc, scb)

/**
 * Internal scbs don't participate in scb hash and lookup i.e. you can't find internal scbs by
 * ethernet address. There are two internal scbs types: hwrs and bcmc.
 */
static struct scb *
_wlc_internalscb_alloc(wlc_info_t *wlc, wlc_bsscfg_t *cfg,
    const struct ether_addr *ea, struct wlcband *band, uint32 flags2)
{
    struct scb_info *scbinfo = NULL;
    scb_module_t *scbstate = wlc->scbstate;
    int bcmerror = 0;
    struct scb *scb;
#ifdef MEM_ALLOC_STATS
    memuse_info_t mu;
    uint32 freemem_before, bytes_allocated;

    hnd_get_heapuse(&mu);
    freemem_before = mu.arena_free;
#endif /* MEM_ALLOC_STATS */

    if ((scbinfo = wlc_scb_allocmem(scbstate)) == NULL) {
        WL_ERROR(("wl%d: %s: Couldn't alloc internal scb\n",
                  wlc->pub->unit, __FUNCTION__));
        return NULL;
    }
    wlc_scb_reset(scbstate, scbinfo);

    scb = SCBPUB(scbstate, scbinfo);
    scb->bsscfg = cfg;
    scb->if_stats = cfg->wlcif->_cnt;
    scb->ea = *ea;

    /* used by hwrs and bcmc scbs */
    scb->flags2 = flags2;

    /* SCB flowid init */
    if (wlc_scb_flowid_init(wlc, scbstate, scb) != BCME_OK) {
        WL_ERROR(("wl%d: %s SCB flowid init failed\n",
            wlc->pub->unit, __FUNCTION__));
        return NULL;
    }

    bcmerror = wlc_scbinit(wlc, cfg, band->bandunit, scb);
    if (bcmerror) {
        WL_ERROR(("wl%d: %s failed with err %d\n",
            wlc->pub->unit, __FUNCTION__, bcmerror));
        _wlc_internalscb_free(wlc, scb);
        return NULL;
    }

    wlc_scb_init_rates(wlc, cfg, band->bandunit, scb);

#ifdef MEM_ALLOC_STATS
    hnd_get_heapuse(&mu);
    bytes_allocated = freemem_before - mu.arena_free;
    scb->mem_bytes = bytes_allocated;
    if (bytes_allocated > mu.max_scb_alloc) {
        mu.max_scb_alloc = bytes_allocated;
    }
    mu.total_scb_alloc += bytes_allocated;
    hnd_update_mem_alloc_stats(&mu);
#endif /* MEM_ALLOC_STATS */

    return scb;
} /* _wlc_internalscb_alloc */

/** allocs broadcast/multicast internal scb */
struct scb *
wlc_bcmcscb_alloc(wlc_info_t *wlc, wlc_bsscfg_t *cfg, struct wlcband *band)
{
    return _wlc_internalscb_alloc(wlc, cfg, &ether_bcast, band, SCB2_BCMC);
}

/** hardware rates scb contains the rates that the local hardware supports for the requested band */
struct scb *
wlc_hwrsscb_alloc(wlc_info_t *wlc, struct wlcband *band)
{
    const struct ether_addr ether_local = {{2, 0, 0, 0, 0, 0}};
    wlc_bsscfg_t *cfg = wlc->primary_bsscfg;

    /* TODO: pass NULL as cfg as hwrs scb doesn't belong to
     * any bsscfg.
     */
    return _wlc_internalscb_alloc(wlc, cfg, &ether_local, band, SCB2_HWRS);
}

#ifdef WL_PROXDETECT
/** proximity detection scb contains fixed rate */
struct scb *
wlc_proxdscb_alloc(wlc_info_t *wlc, struct wlcband *band)
{
    const struct ether_addr ether_local = {{2, 2, 0, 0, 0, 0}};
    wlc_bsscfg_t *cfg = wlc->primary_bsscfg;
    /* TODO: pass NULL as cfg as PROXD scb doesn't belong to
     * any bsscfg.
     */
    return _wlc_internalscb_alloc(wlc, cfg, &ether_local, band, SCB2_PROXD);
}
#endif /* WL_PROXDETECT */

/**
 * a 'user' scb as opposed to an 'internal' scb. Does not only allocate, but also initializes the
 * new scb.
 */
static struct scb *
wlc_userscb_alloc(wlc_info_t *wlc, wlc_bsscfg_t *cfg,
    const struct ether_addr *ea, struct wlcband *band)
{
    scb_module_t *scbstate = wlc->scbstate;
    struct scb_info *scbinfo = NULL;
    struct scb *oldscb;
    wlc_bsscfg_t *oldcfg;
    int bcmerror;
    struct scb *scb;
#ifdef MEM_ALLOC_STATS
    memuse_info_t mu;
    uint32 freemem_before, bytes_allocated;

    hnd_get_heapuse(&mu);
    freemem_before = mu.arena_free;
#endif /* MEM_ALLOC_STATS */

    /* Make sure we live within our budget, and kick someone out if needed. */
    if (scbstate->nscb >= wlc->pub->tunables->maxscb ||
        /* age scb in low memory situation as well */
        (OSL_MEM_AVAIL() <= (uint)wlc->pub->tunables->min_scballoc_mem)) {
        /* free the oldest entry */
        if (!(oldscb = wlc_scbvictim(wlc))) {
            WL_ERROR(("wl%d: %s: no SCBs available to reclaim\n",
                      wlc->pub->unit, __FUNCTION__));
            return NULL;
        }
        oldcfg = SCB_BSSCFG(oldscb);
        if (oldcfg && BSSCFG_AP(oldcfg) && SCB_AUTHENTICATED(oldscb)) {
            WL_ASSOC(("wl%d.%d: %s: send deauth to "MACF" with reason %d\n",
                wlc->pub->unit, WLC_BSSCFG_IDX(oldcfg), __FUNCTION__,
                ETHER_TO_MACF(oldscb->ea), DOT11_RC_INACTIVITY));
            (void)wlc_senddeauth(wlc, oldcfg, oldscb, &oldscb->ea, &oldcfg->BSSID,
                &oldcfg->cur_etheraddr, DOT11_RC_INACTIVITY, TRUE);
            wlc_deauth_complete(wlc, oldcfg, WLC_E_STATUS_SUCCESS, &oldscb->ea,
                DOT11_RC_INACTIVITY, 0);
        }
        if (!wlc_scbfree(wlc, oldscb)) {
            WL_ERROR(("wl%d: %s: Couldn't free a victimized scb\n",
                      wlc->pub->unit, __FUNCTION__));
            return NULL;
        }
    }
    ASSERT(scbstate->nscb <= wlc->pub->tunables->maxscb);

    if ((scbinfo = wlc_scb_allocmem(scbstate)) == NULL) {
        WL_ERROR(("wl%d: %s: Couldn't alloc user scb\n",
                  wlc->pub->unit, __FUNCTION__));
        return NULL;
    }

    wlc_scb_reset(scbstate, scbinfo);

    scbstate->nscb++;

    scb = SCBPUB(scbstate, scbinfo);
    scb->bsscfg = cfg;
    scb->if_stats = cfg->wlcif->_cnt;
    scb->ea = *ea;

    /* SCB flowid init */
    if (wlc_scb_flowid_init(wlc, scbstate, scb) != BCME_OK) {
        WL_ERROR(("wl%d: %s SCB flowid init failed\n",
            wlc->pub->unit, __FUNCTION__));
        return NULL;
    }

    bcmerror = wlc_scbinit(wlc, cfg, band->bandunit, scb);
    if (bcmerror) {
        WL_ERROR(("wl%d: %s failed with err %d\n", wlc->pub->unit, __FUNCTION__, bcmerror));
        wlc_scbfree(wlc, scb);
        return NULL;
    }

    /* add it to the link list */
    wlc_scb_list_add(scbstate, cfg, scb);

    /* install it in the cache */
    wlc_scb_hash_add(scbstate, cfg, band->bandunit, scb);

    wlc_scb_init_rates(wlc, cfg, band->bandunit, scb);

#ifdef MEM_ALLOC_STATS
    hnd_get_heapuse(&mu);
    bytes_allocated = freemem_before - mu.arena_free;
    scb->mem_bytes = bytes_allocated;
    if (bytes_allocated > mu.max_scb_alloc) {
        mu.max_scb_alloc = bytes_allocated;
    }
    mu.total_scb_alloc += bytes_allocated;
    hnd_update_mem_alloc_stats(&mu);
#endif /* MEM_ALLOC_STATS */

#ifdef WL_PKTDROP_STATS
    /*
     * if pktdrop counters feature is enabled, ALL scbs
     * automatically get a brief counter
     */
    if (wlc->pktdrop_stats_feature_enabled) {
        scb_pktdrop_brief_counters_enable(wlc, scb);
    } else {
        scb->pktdrop_brief_counters = NULL;
    }
#endif /* WL_PKTDROP_STATS */

    return scb;
} /* wlc_userscb_alloc */

/* secondary cubby total size. return 0 to skip secondary cubby init. */
static uint
wlc_scb_sec_sz(void *ctx, void *obj)
{
    scb_module_t *scbstate = (scb_module_t *)ctx;
    struct scb *scb = (struct scb *)obj;

#ifdef WL_PROXDETECT
    /* proxd scbs don't need any secondary cubbies so skip them */
    if (SCB_PROXD(scb)) {
        return 0;
    }
#endif

    return wlc_cubby_sec_totsize(scbstate->cubby_info, scb);
}

/** saves the base address of the secondary (cubby) container */
static void
wlc_scb_sec_set(void *ctx, void *obj, void *base)
{
    scb_module_t *scbstate = (scb_module_t *)ctx;
    struct scb *scb = (struct scb *)obj;
    struct scb_info *scbinfo = SCBINFO(scbstate, scb);

    scbinfo->secbase = base;
}

/** returns the base address of the secondary (cubby) container */
static void *
wlc_scb_sec_get(void *ctx, void *obj)
{
    scb_module_t *scbstate = (scb_module_t *)ctx;
    struct scb *scb = (struct scb *)obj;
    struct scb_info *scbinfo = SCBINFO(scbstate, scb);

    return scbinfo->secbase;
}

static int
wlc_scbinit(wlc_info_t *wlc, wlc_bsscfg_t *cfg, enum wlc_bandunit bandunit, struct scb *scb)
{
    scb_module_t *scbstate = wlc->scbstate;
    uint i;

    BCM_REFERENCE(cfg);
    ASSERT(scb != NULL);

    scb->used = wlc->pub->now;
    scb->bandunit = bandunit;

    for (i = 0; i < NUMPRIO; i++)
        scb->seqctl[i] = 0xFFFF;
    scb->seqctl_nonqos = 0xFFFF;

#if defined(WLATM_PERC)
    scb->staperc = 0;
    scb->sched_staperc = 0;
#endif /* WLATM_PERC */

#ifdef WLCNTSCB
    bzero((char*)&scb->scb_stats, sizeof(scb->scb_stats));
#endif
#if defined(BCMDBG)
    /*  Check if msg macfilter is enabled. If yes, restore mac filter */
    for (i = 0; i < MSG_MACFLTR_MAX; i++) {
        if (eacmp(&wl_msg_macfltr[i], &scb->ea) == 0) {
            scb->msg_macfltr = 1;
            break;
        }
    }
    if (scb->msg_macfltr)
        WL_ERROR((" %s add msg_macfltr "MACF"\n", __FUNCTION__, ETHER_TO_MACF(scb->ea)));
#endif /* BCMDBG */

    return wlc_cubby_init(scbstate->cubby_info, scb, wlc_scb_sec_sz, wlc_scb_sec_set, scbstate);
}

static void
wlc_scb_freemem(scb_module_t *scbstate, struct scb_info *scbinfo)
{
    wlc_info_t *wlc = scbstate->wlc;
    struct scb *scb = SCBPUB(scbstate, scbinfo);
    uint len;

    BCM_REFERENCE(wlc);

    if (scbinfo == NULL)
        return;

#ifdef SCB_MEMDBG
    scbinfo->magic = ~SCB_MAGIC;
#endif
    len = scbstate->scbtotsize + wlc_cubby_totsize(scbstate->cubby_info);
    MFREE(wlc->osh, scb, len);

#ifdef SCB_MEMDBG
    scbstate->scbfreed++;
#endif /* SCB_MEMDBG */
}

#ifdef WLSCB_HISTO
static void
wlc_scb_histo_mem_free(wlc_info_t *wlc, struct scb *scb)
{
    if (scb->histo) {
        MFREE(wlc->osh, scb->histo, sizeof(wl_rate_histo_maps1_t));
    }
}
#endif /* WLSCB_HISTO */

void
wlc_scb_dh_params_free(wlc_info_t *wlc, struct scb *scb)
{
    if (scb->dh_params) {
        MFREE(wlc->osh, scb->dh_params, sizeof(dh_params_ie_t));
        scb->dh_params = NULL;
    }
}

void
wlc_scb_rsn_ie_free(wlc_info_t *wlc, struct scb *scb)
{
    if (scb->rsn_ie) {
        MFREE(wlc->osh, scb->rsn_ie, scb->rsn_ie->len + TLV_HDR_LEN);
        scb->rsn_ie = NULL;
    }
}

void
wlc_scb_rsnxe_info_free(wlc_info_t *wlc, struct scb *scb)
{
    rsnxe_ie_info_t *rsnxe_info = RSNXE_INFO_GET(scb);
    if (rsnxe_info) {
        if (rsnxe_info->rsnxe) {
            MFREE(wlc->osh, rsnxe_info->rsnxe, rsnxe_info->rsnxe_len);
            rsnxe_info->rsnxe = NULL;
        }
        MFREE(wlc->osh, rsnxe_info, sizeof(rsnxe_ie_info_t));
        RSNXE_INFO_GET(scb) = NULL;
    }
}

bool
wlc_scbfree(wlc_info_t *wlc, struct scb *scbd)
{
    scb_module_t *scbstate = wlc->scbstate;
    struct scb_info *remove = SCBINFO(scbstate, scbd);
#ifdef MEM_ALLOC_STATS
    memuse_info_t mu;
    uint32 membytes = scbd->mem_bytes;
#endif /* MEM_ALLOC_STATS */

    if (scbd->permanent)
        return FALSE;

    /* Return if SCB is already being deleted else mark it */
    if (SCB_DEL_IN_PROGRESS(scbd))
        return FALSE;

    scbd->mark |= SCB_DEL_IN_PROG;

#ifdef WLTAF
    /* Defer taf schedule to prevent packet enqueue during scb packet cleanup */
    wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */

    /* cancel potential dangling sendcomplete callbacks */
    wlc_deauth_sendcomplete_clean(wlc, scbd);

    wlc_tx_fifo_scb_flush(wlc, scbd);
    scbd->if_stats = NULL;

#ifdef WLSCB_HISTO
    wlc_scb_histo_mem_free(wlc, scbd);
#endif /* WLSCB_HISTO */

#ifdef WL_PKTDROP_STATS
    scb_pktdrop_brief_counters_disable(wlc, scbd);
    scb_pktdrop_full_counters_disable(wlc, scbd);
#endif /* WL_PKTDROP_STATS */

    wlc_scb_dh_params_free(wlc, scbd);
    wlc_scb_rsn_ie_free(wlc, scbd);
    wlc_scb_rsnxe_info_free(wlc, scbd);

#ifdef BCMPCIEDEV
    /* Remove all flowid reference from bus layer
     * Indirect way to block all downstream calls to WL layers [SQS cubby access]
     */
    wlc_scb_flow_ring_delink(wlc, scbd);
#endif /* BCMPCIEDEV_ENABLED */

    wlc_scbdeinit(wlc, scbd);

    wlc_scb_resetstate(wlc, scbd);

    /* Deinit the SCB flowid, global lookup */
    wlc_scb_flowid_deinit(wlc, scbstate, scbd);

#ifdef WLTAF
    /* Resume taf schedule */
    wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */

    if (SCB_INTERNAL(scbd)) {
        goto free;
    }

    if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scbd) || SCB_PKTS_TOT_INFLT_CQCNT_VAL(scbd)) {
        WL_ERROR(("%s: packets pending for "MACF" in Tx FIFO %d, CMNQ %d\n", __FUNCTION__,
            ETHER_TO_MACF(scbd->ea), SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scbd),
            SCB_PKTS_TOT_INFLT_CQCNT_VAL(scbd)));
    }

#ifdef BCMDBG
    wlc_validate_empty_fifo_list(wlc);
#endif /* BCMDBG */

    /* delete it from the hash */
    wlc_scb_hash_del(scbstate, SCB_BSSCFG(scbd), scbd);

    /* delete it from the link list */
    wlc_scb_list_del(scbstate, SCB_BSSCFG(scbd), scbd);

    /* update total allocated scb number */
    scbstate->nscb--;

free:
    /* free scb memory */
    wlc_scb_freemem(scbstate, remove);

#ifdef MEM_ALLOC_STATS
    hnd_get_heapuse(&mu);
    mu.total_scb_alloc -= membytes;
    hnd_update_mem_alloc_stats(&mu);
#endif /* MEM_ALLOC_STATS */

    return TRUE;
}

static void
wlc_scbdeinit(wlc_info_t *wlc, struct scb *scbd)
{
    scb_module_t *scbstate = wlc->scbstate;

    wlc_cubby_deinit(scbstate->cubby_info, scbd, wlc_scb_sec_sz, wlc_scb_sec_get, scbstate);
}

static void
wlc_scb_list_add(scb_module_t *scbstate, wlc_bsscfg_t *bsscfg, struct scb *scb)
{
    struct scb_info *scbinfo = SCBINFO(scbstate, scb);
    scb_bsscfg_cubby_t *scb_cfg;

    ASSERT(bsscfg != NULL);

    scb_cfg = SCB_BSSCFG_CUBBY(scbstate, bsscfg);

    SCBSANITYCHECK(scbstate, scb_cfg->scb);

    /* update scb link list */
    scbinfo->next = SCBINFO(scbstate, scb_cfg->scb);
#ifdef SCB_MEMDBG
    scbinfo->next_copy = scbinfo->next;
#endif
    scb_cfg->scb = scb;
}

static void
wlc_scb_list_del(scb_module_t *scbstate, wlc_bsscfg_t *bsscfg, struct scb *scbd)
{
    scb_bsscfg_cubby_t *scb_cfg;
    struct scb_info *scbinfo;
    struct scb_info *remove = SCBINFO(scbstate, scbd);

    ASSERT(bsscfg != NULL);

    /* delete it from the link list */

    scb_cfg = SCB_BSSCFG_CUBBY(scbstate, bsscfg);
    scbinfo = SCBINFO(scbstate, scb_cfg->scb);
    if (scbinfo == remove) {
        scb_cfg->scb = wlc_scb_next(scbstate, scbd);
        return;
    }

    while (scbinfo) {
        SCBSANITYCHECK(scbstate, SCBPUB(scbstate, scbinfo));
        if (scbinfo->next == remove) {
            scbinfo->next = remove->next;
#ifdef SCB_MEMDBG
            scbinfo->next_copy = scbinfo->next;
#endif
            break;
        }
        scbinfo = scbinfo->next;
    }

    if (scbinfo == NULL) {
        WL_ERROR(("wl%d.%d: Unable to find scbinfo, scb = %p in the linked list",
            WLCWLUNIT(bsscfg->wlc), WLC_BSSCFG_IDX(bsscfg), OSL_OBFUSCATE_BUF(scbd)));
    }
}

/** free all scbs of a bsscfg */
static void
wlc_scb_bsscfg_scbclear(struct wlc_info *wlc, wlc_bsscfg_t *bsscfg, bool perm)
{
    struct scb_iter scbiter;
    struct scb *scb;

    if (wlc->scbstate == NULL)
        return;

    FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
        if (scb->permanent) {
            if (!perm)
                continue;
            scb->permanent = FALSE;
        }
        wlc_scbfree(wlc, scb);
    }
}

static struct scb *
wlc_scbvictim(wlc_info_t *wlc)
{
    uint oldest;
    struct scb *scb;
    struct scb *oldscb;
    uint now, age;
    struct scb_iter scbiter;
    wlc_bsscfg_t *bsscfg = NULL;

#ifdef AP
    /* search for an unauthenticated scb */
    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        if (!scb->permanent && (scb->state == 0))
            return scb;
    }
#endif /* AP */

    /* free the oldest scb */
    now = wlc->pub->now;
    oldest = 0;
    oldscb = NULL;
    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        bsscfg = SCB_BSSCFG(scb);
        ASSERT(bsscfg != NULL);
        if (BSSCFG_STA(bsscfg) && bsscfg->BSS && SCB_ASSOCIATED(scb))
            continue;
        if (SCB_MARKED_DEL(scb)) {
            /* prioritize scb that is already marked for deletion */
            oldscb = scb;
            goto victim_found;
        }
        if (!scb->permanent && ((age = (now - scb->used)) >= oldest)) {
            oldest = age;
            oldscb = scb;
        }
    }
    /* handle extreme case(s): all are permanent ... or there are no scb's at all */
    if (oldscb == NULL)
        return NULL;

#ifdef AP
    bsscfg = SCB_BSSCFG(oldscb);

    if (BSSCFG_AP(bsscfg)) {
        /* if the oldest authenticated SCB has only been idle a short time then
         * it is not a candidate to reclaim
         */
        if (oldest < SCB_SHORT_TIMEOUT)
            return NULL;
    }
#endif /* AP */
victim_found:
    WL_ASSOC(("wl%d.%d: %s: reclaim scb "MACF", idle %d sec\n",  wlc->pub->unit,
        WLC_BSSCFG_IDX(scb->bsscfg), __FUNCTION__, ETHER_TO_MACF(oldscb->ea), oldest));

    return oldscb;
} /* wlc_scbvictim */

/* Only notify registered clients of the following states' change. */
static uint8 scb_state_change_notif_mask = AUTHENTICATED | ASSOCIATED | AUTHORIZED;

#if defined BCMDBG
static char *
wlc_scb_state_ascii(uint8 state, char *str)
{
    sprintf(str, "[ %s%s%s%s%s]",
        (state & AUTHENTICATED)?"AUTHENTICATED ":"",
        (state & ASSOCIATED)?"ASSOCIATED ":"",
        (state & PENDING_AUTH)?"PENDING_AUTH ":"",
        (state & PENDING_ASSOC)?"PENDING_ASSOC ":"",
        (state & AUTHORIZED)?"AUTHORIZED ":"");
    return (str);
}
#else
#define wlc_scb_state_ascii(aa, bb) ""
#endif

/** "|" operation. */
void
wlc_scb_setstatebit(wlc_info_t *wlc, struct scb *scb, uint8 state)
{
    scb_state_upd_data_t data;
    uint8 oldstate;
#if defined BCMDBG
    char state_str[75];
#endif

    ASSERT(scb != NULL);
    WL_ASSOC(("wl%d.%d: %s: "MACF": set bits 0x%02x %s\n",
        wlc->pub->unit, WLC_BSSCFG_IDX(scb->bsscfg), __FUNCTION__, ETHER_TO_MACF(scb->ea),
        state, wlc_scb_state_ascii(state, state_str)));

    oldstate = scb->state;

    if (state & AUTHENTICATED)
    {
        scb->state &= ~PENDING_AUTH;
    }
    if (state & ASSOCIATED)
    {
        ASSERT((scb->state | state) & AUTHENTICATED);
        scb->state &= ~PENDING_ASSOC;
#if defined(BCMHWA) && defined(HWA_RXFILL_BUILD)
        scb->rx_auth_tsf = scb->rx_auth_tsf_reference = 0;
#endif
    }

#ifdef CONFIG_TENDA_PRIVATE_WLAN
    td_sta_online(wlc, scb, state);
#endif
    scb->state |= state;
    WL_ASSOC(("wl%d.%d: %s: "MACF": oldstate = 0x%02x, scb->state = 0x%02x %s\n",
        wlc->pub->unit, WLC_BSSCFG_IDX(scb->bsscfg), __FUNCTION__, ETHER_TO_MACF(scb->ea),
        oldstate, scb->state, wlc_scb_state_ascii(scb->state, state_str)));

    if ((BSSCFG_AP(SCB_BSSCFG(scb)) && (state & scb_state_change_notif_mask) != 0) ||
        (oldstate & scb_state_change_notif_mask) != (state & scb_state_change_notif_mask)) {
        data.scb = scb;
        data.oldstate = oldstate;
        bcm_notif_signal(wlc->scbstate->scb_state_notif_hdl, &data);
    }
}

/** "& ~" operation */
void
wlc_scb_clearstatebit(wlc_info_t *wlc, struct scb *scb, uint8 state)
{
    scb_state_upd_data_t data;
    uint8 oldstate;
#if defined BCMDBG
    char state_str[75];
#endif

    ASSERT(scb != NULL);
    ASSERT(!SCB_PS(scb) || !(state & ASSOCIATED) || !scb->sent_deauth);
    WL_ASSOC(("wl%d.%d: %s: "MACF": clear bits 0x%02x %s\n",
        wlc->pub->unit, WLC_BSSCFG_IDX(scb->bsscfg), __FUNCTION__, ETHER_TO_MACF(scb->ea),
        state, wlc_scb_state_ascii(state, state_str)));

    oldstate = scb->state;

#ifdef CONFIG_TENDA_PRIVATE_WLAN
    td_sta_offline(wlc, scb, state);
#endif

#if defined(AI_MESH_SUPPORT) && defined(CONFIG_TENDA_PRIVATE_WLAN)
        ai_mesh_handle_node(scb);
#endif

    scb->state &= ~state;
    WL_ASSOC(("wl%d.%d: %s: "MACF": oldstate = 0x%02x, scb->state = 0x%02x %s\n",
        wlc->pub->unit, WLC_BSSCFG_IDX(scb->bsscfg), __FUNCTION__, ETHER_TO_MACF(scb->ea),
        oldstate, scb->state, wlc_scb_state_ascii(scb->state, state_str)));

    if ((oldstate & scb_state_change_notif_mask) != (scb->state & scb_state_change_notif_mask))
    {
        data.scb = scb;
        data.oldstate = oldstate;
#ifdef WLTAF
        /* Defer taf schedule to prevent packet enqueue during
         * state notification callback
         */
        wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */

        bcm_notif_signal(wlc->scbstate->scb_state_notif_hdl, &data);
#ifdef WLTAF
        /* Resume taf schedule */
        wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */
    }
}

/** reset all state. */
void
wlc_scb_resetstate(wlc_info_t *wlc, struct scb *scb)
{
    WL_NONE(("reset state\n"));

    wlc_scb_clearstatebit(wlc, scb, ~0);
}

/** set/change rateset and init/reset ratesel */
static void
wlc_scb_init_rates(wlc_info_t *wlc, wlc_bsscfg_t *cfg, enum wlc_bandunit bandunit, struct scb *scb)
{
    wlcband_t *band = wlc->bandstate[bandunit];
    wlc_rateset_t *rs;

    /* use current, target, or per-band default rateset? */
    if (wlc->pub->up &&
        wlc_valid_chanspec_db(wlc->cmi, cfg->target_bss->chanspec))
        if (cfg->associated)
            rs = &cfg->current_bss->rateset;
        else
            rs = &cfg->target_bss->rateset;
    else
        rs = &band->defrateset;

    /*
     * Initialize the per-scb rateset:
     * - if we are AP, start with only the basic subset of the
     *    network rates.  It will be updated when receive the next
     *    probe request or association request.
     * - if we are IBSS and gmode, special case:
     *    start with B-only subset of network rates and probe for ofdm rates
     * - else start with the network rates.
     *    It will be updated on join attempts.
     */
    if (BSS_P2P_ENAB(wlc, cfg)) {
        wlc_rateset_filter(rs /* src */, &scb->rateset /* dst */,
                           FALSE, WLC_RATES_OFDM, RATE_MASK,
                           wlc_get_mcsallow(wlc, cfg));
    }
    else if (BSSCFG_AP(cfg)) {
        /* XXX Does not match with the comment above. Remove the
         * HT rates and possibly OFDM rates if not needed. If there
         * is a valid reason to add the HT rates, then check if we have to
         * add VHT rates as well.
         */
        uint8 mcsallow = BSS_N_ENAB(wlc, cfg) ? WLC_MCS_ALLOW_HT : 0;
        wlc_rateset_filter(rs /* src */, &scb->rateset /* dst */,
                           TRUE, WLC_RATES_CCK_OFDM, RATE_MASK,
                           mcsallow);
    }
    else if (BSSCFG_IBSS(cfg) && band->gmode) {
        wlc_rateset_filter(rs /* src */, &scb->rateset /* dst */,
                FALSE, WLC_RATES_CCK, RATE_MASK, 0);
        /* if resulting set is empty, then take all network rates instead */
        if (scb->rateset.count == 0) {
            wlc_rateset_filter(rs /* src */, &scb->rateset /* dst */,
                    FALSE, WLC_RATES_CCK_OFDM, RATE_MASK, 0);
        }
    }
    else {
        wlc_rateset_filter(rs /* src */, &scb->rateset /* dst */,
                FALSE, WLC_RATES_CCK_OFDM, RATE_MASK, 0);
    }

    if (!SCB_INTERNAL(scb)) {
        wlc_scb_ratesel_init(wlc, scb);
#ifdef STA
        /* send ofdm rate probe */
        if (BSSCFG_STA(cfg) && BSSCFG_IBSS(cfg) && band->gmode &&
            wlc->pub->up)
            wlc_rateprobe(wlc, cfg, &scb->ea, WLC_RATEPROBE_RSPEC);
#endif /* STA */
    }
} /* wlc_scb_init_rates */

static void
wlc_scb_bsscfg_reinit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    uint prev_count;
    const wlc_rateset_t *rs;
    wlcband_t *band;
    struct scb *scb;
    struct scb_iter scbiter;
    bool cck_only;
    bool reinit_forced;

    WL_INFORM(("wl%d: %s: bandunit 0x%x phy_type 0x%x gmode 0x%x\n", wlc->pub->unit,
        __FUNCTION__, wlc->band->bandunit, wlc->band->phytype, wlc->band->gmode));

    /* sanitize any existing scb rates against the current hardware rates */
    FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
        /* XXX SCB : should the following be done only if scb->bandunit matches
         * wlc->band->bandunit?
         */
        prev_count = scb->rateset.count;
        /* Keep only CCK if gmode == GMODE_LEGACY_B */
        band = wlc_scbband(wlc, scb);
        if (BAND_2G(band->bandtype) && (band->gmode == GMODE_LEGACY_B)) {
            rs = &cck_rates;
            cck_only = TRUE;
        } else {
            rs = &band->hw_rateset;
            cck_only = FALSE;
        }
        if (!wlc_rate_hwrs_filter_sort_validate(&scb->rateset /* [in+out] */, rs /* [in] */,
            FALSE, wlc->stf->op_txstreams)) {
            /* continue with default rateset.
             * since scb rateset does not carry basic rate indication,
             * clear basic rate bit.
             */
            WL_RATE(("wl%d: %s: invalid rateset in scb 0x%p bandunit 0x%x "
                "phy_type 0x%x gmode 0x%x\n", wlc->pub->unit, __FUNCTION__,
                OSL_OBFUSCATE_BUF(scb), band->bandunit,
                band->phytype, band->gmode));
#ifdef BCMDBG
            wlc_rateset_show(wlc, &scb->rateset, &scb->ea);
#endif

            wlc_rateset_default(wlc, &scb->rateset, &band->hw_rateset,
                                band->phytype, band->bandtype, cck_only, RATE_MASK,
                                wlc_get_mcsallow(wlc, scb->bsscfg),
                                CHSPEC_WLC_BW(scb->bsscfg->current_bss->chanspec),
                                wlc->stf->op_txstreams);
            reinit_forced = TRUE;
        }
        else
            reinit_forced = FALSE;

        /* if the count of rates is different, then the rate state
         * needs to be reinitialized
         */
        if (reinit_forced || (scb->rateset.count != prev_count))
            wlc_scb_ratesel_init(wlc, scb);

        WL_RATE(("wl%d: %s: bandunit 0x%x, phy_type 0x%x gmode 0x%x. final rateset is\n",
            wlc->pub->unit, __FUNCTION__,
            band->bandunit, band->phytype, band->gmode));
#ifdef BCMDBG
        wlc_rateset_show(wlc, &scb->rateset, &scb->ea);
#endif
    }
} /* wlc_scb_bsscfg_reinit */

void
wlc_scb_reinit(wlc_info_t *wlc)
{
    int32 idx;
    wlc_bsscfg_t *bsscfg;

    FOREACH_BSS(wlc, idx, bsscfg) {
        wlc_scb_bsscfg_reinit(wlc, bsscfg);
    }
}

/**
 * Get scb from band
 *
 * @param[in] bandunit   BAND_2G_INDEX,BAND_5G_INDEX, ...
 */
struct scb * BCMFASTPATH
wlc_scbfindband(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, const struct ether_addr *ea,
             enum wlc_bandunit bandunit)
{
    scb_module_t *scbstate = wlc->scbstate;
    int indx;
    struct scb_info *scbinfo;
    scb_bsscfg_cubby_t *scb_cfg;

    ASSERT(bsscfg != NULL);
    ASSERT(is_valid_bandunit(wlc, bandunit));

    /* All callers of wlc_scbfind() should first be checking to see
     * if the SCB they're looking for is a BC/MC address.  Because we're
     * using per bsscfg BCMC SCBs, we can't "find" BCMC SCBs without
     * knowing which bsscfg.
     */
    ASSERT(ea && !ETHER_ISMULTI(ea));

    /* search for the scb which corresponds to the remote station ea */
    scb_cfg = SCB_BSSCFG_CUBBY(scbstate, bsscfg);
    if (scb_cfg && scb_cfg->scbhash[bandunit]) {
        indx = SCBHASHINDEX(scb_cfg->nscbhash, ea->octet);
        scbinfo =
            SCBINFO(scbstate, scb_cfg->scbhash[bandunit][indx]);
        for (; scbinfo; scbinfo = scbinfo->hashnext) {
            SCBSANITYCHECK(scbstate, SCBPUB(scbstate, scbinfo));
            if (memcmp((const char*)ea,
                (const char*)&(SCBPUB(scbstate, scbinfo)->ea),
                sizeof(*ea)) == 0)
                break;
        }

        return SCBPUB(scbstate, scbinfo);
    }
    return (NULL);
}

/** Find station control block corresponding to the remote id */
struct scb * BCMFASTPATH
wlc_scbfind(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, const struct ether_addr *ea)
{
    struct scb *scb = NULL;

    scb = wlc_scbfindband(wlc, bsscfg, ea, wlc->band->bandunit);

#if defined(WLMCHAN)
/* current band could be different, so search again for all scb's */
    if MCHAN_ACTIVE(wlc->pub) {
        enum wlc_bandunit bandunit;
        FOREACH_WLC_BAND(wlc, bandunit) {
            if (scb != NULL) {
                break;
            }
            scb = wlc_scbfindband(wlc, bsscfg, ea, bandunit);
        }
    }
#endif /* WLMCHAN */
    return scb;
}

/**
 * Lookup station control block corresponding to the remote id.
 * If not found, create a new entry.
 */
static INLINE struct scb *
_wlc_scblookup(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, const struct ether_addr *ea,
               enum wlc_bandunit bandunit)
{
    struct scb *scb;
    struct wlcband *band;
#if defined(BCMDBG) || defined(WLMSG_ASSOC)
    char sa[ETHER_ADDR_STR_LEN];
#endif

    /* Don't allocate/find a BC/MC SCB this way. */
    ASSERT(!ETHER_ISMULTI(ea));
    if (ETHER_ISMULTI(ea))
        return NULL;

    /* apply mac filter */
    switch (wlc_macfltr_addr_match(wlc->macfltr, bsscfg, ea)) {
    case WLC_MACFLTR_ADDR_DENY:
        WL_ASSOC(("wl%d.%d mac restrict: Denying %s\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
                  bcm_ether_ntoa(ea, sa)));
        wlc_bss_mac_event(wlc, bsscfg, WLC_E_PRUNE, ea, 0,
                WLC_E_PRUNE_MAC_DENY, 0, 0, 0);
        return NULL;
    case WLC_MACFLTR_ADDR_NOT_ALLOW:
        WL_ASSOC(("wl%d.%d mac restrict: Not allowing %s\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
                  bcm_ether_ntoa(ea, sa)));
        wlc_bss_mac_event(wlc, bsscfg, WLC_E_PRUNE, ea, 0,
                WLC_E_PRUNE_MAC_NA, 0, 0, 0);
        return NULL;
#ifdef BCMDBG
    case WLC_MACFLTR_ADDR_ALLOW:
        WL_ASSOC(("wl%d.%d mac restrict: Allowing %s\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
                  bcm_ether_ntoa(ea, sa)));
        break;
    case WLC_MACFLTR_ADDR_NOT_DENY:
        WL_ASSOC(("wl%d.%d mac restrict: Not denying %s\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
                  bcm_ether_ntoa(ea, sa)));
        break;
    case WLC_MACFLTR_DISABLED:
        WL_NONE(("wl%d.%d no mac restrict: lookup %s\n",
                 wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
                 bcm_ether_ntoa(ea, sa)));
        break;
#endif /* BCMDBG */
    }

    if ((scb = wlc_scbfindband(wlc, bsscfg, ea, bandunit)))
        return (scb);

    /* no scb match, allocate one for the desired bandunit */
    band = wlc->bandstate[bandunit];

    return wlc_userscb_alloc(wlc, bsscfg, ea, band);
} /* _wlc_scblookup */

struct scb *
wlc_scblookup(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, const struct ether_addr *ea)
{
    return (_wlc_scblookup(wlc, bsscfg, ea, wlc->band->bandunit));
}

struct scb *
wlc_scblookupband(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, const struct ether_addr *ea,
                  enum wlc_bandunit bandunit)
{
    ASSERT(is_valid_bandunit(wlc, bandunit));

    return (_wlc_scblookup(wlc, bsscfg, ea, bandunit));
}

void
wlc_scb_switch_band(wlc_info_t *wlc, struct scb *scb, int new_bandunit,
    wlc_bsscfg_t *bsscfg)
{
    scb_module_t *scbstate = wlc->scbstate;

    /* first, del scb from hash table in old band */
    wlc_scb_hash_del(scbstate, bsscfg, scb);
    /* next add scb to hash table in new band */
    wlc_scb_hash_add(scbstate, bsscfg, new_bandunit, scb);
    /* XXX JQL: we should move/add at least rateset/ratesel init
     * code here as band change definitely needs it.
     */
    return;
}

/**
 * Move the scb's band info.
 * Parameter description:
 *
 * wlc - global wlc_info structure
 * bsscfg - the bsscfg that is about to move to a new chanspec
 * chanspec - the new chanspec the bsscfg is moving to
 *
 */
void
wlc_scb_update_band_for_cfg(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, chanspec_t chanspec)
{
    struct scb_iter scbiter;
    struct scb *scb, *stale_scb;
    enum wlc_bandunit bandunit;
    bool reinit = FALSE;

    FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
        if (SCB_ASSOCIATED(scb)) {
            bandunit = CHSPEC_BANDUNIT(chanspec);
            if (scb->bandunit != (uint)bandunit) {
                /* We're about to move our scb to the new band.
                 * Check to make sure there isn't an scb entry for us there.
                 * If there is one for us, delete it first.
                 */
                if ((stale_scb = wlc_scbfindband(wlc, bsscfg,
                                      &bsscfg->BSSID, bandunit)) &&
                    (stale_scb->permanent == FALSE)) {
                    WL_ASSOC(("wl%d.%d: %s: found stale scb %p on %s band, "
                              "remove it\n",
                              wlc->pub->unit, bsscfg->_idx, __FUNCTION__,
                              OSL_OBFUSCATE_BUF(stale_scb),
                              wlc_bandunit_name(bandunit)));
                    /* mark the scb for removal */
                    stale_scb->mark |= SCB_MARK_TO_REM;
                }
                /* Now perform the move of our scb to the new band */
                wlc_scb_switch_band(wlc, scb, bandunit, bsscfg);
                reinit = TRUE;
            }
        }
    }
    /* remove stale scb's marked for removal */
    FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
        if (scb->mark & SCB_MARK_TO_REM) {
            WL_ASSOC(("remove stale scb %p\n", OSL_OBFUSCATE_BUF(scb)));
            scb->mark &= ~SCB_MARK_TO_REM;
            wlc_scbfree(wlc, scb);
        }
    }

    if (reinit) {
        wlc_scb_reinit(wlc);
    }
} /* wlc_scb_update_band_for_cfg */

struct scb *
wlc_scbibssfindband(wlc_info_t *wlc, const struct ether_addr *ea, enum wlc_bandunit bandunit,
                    wlc_bsscfg_t **bsscfg)
{
    int idx;
    wlc_bsscfg_t *cfg;
    struct scb *scb = NULL;

    ASSERT(is_valid_bandunit(wlc, bandunit));

    FOREACH_IBSS(wlc, idx, cfg) {
        /* Find the bsscfg and scb matching specified peer mac */
        scb = wlc_scbfindband(wlc, cfg, ea, bandunit);
        if (scb != NULL) {
            *bsscfg = cfg;
            break;
        }
    }

    return scb;
}

struct scb *
wlc_scbapfind(wlc_info_t *wlc, const struct ether_addr *ea)
{
    int idx;
    wlc_bsscfg_t *cfg;
    struct scb *scb = NULL;

    FOREACH_UP_AP(wlc, idx, cfg) {
        /* Find the bsscfg and scb matching specified peer mac */
        scb = wlc_scbfind(wlc, cfg, ea);
        if (scb != NULL) {
            break;
        }
    }

    return scb;
}

struct scb * BCMFASTPATH
wlc_scbbssfindband(wlc_info_t *wlc, const struct ether_addr *hwaddr,
                   const struct ether_addr *ea, enum wlc_bandunit bandunit, wlc_bsscfg_t **bsscfg)
{
    wlc_bsscfg_t *cfg;
    struct scb *scb = NULL;
    int idx;

    ASSERT(is_valid_bandunit(wlc, bandunit));
    *bsscfg = NULL;
    FOREACH_BSS(wlc, idx, cfg) {
        if (!eacmp(hwaddr->octet, cfg->cur_etheraddr.octet)) {
            scb = wlc_scbfindband(wlc, cfg, ea, bandunit);
            if (scb != NULL) {
                *bsscfg = cfg;
                break;
            }
        }
    }

    return scb;
}

struct scb *
wlc_scbfind_from_wlcif(wlc_info_t *wlc, struct wlc_if *wlcif, uint8 *addr)
{
    struct scb *scb = NULL;
    wlc_bsscfg_t *bsscfg;
    char *bss_addr;

    if (wlcif && (wlcif->type == WLC_IFTYPE_WDS)) {
        return (wlcif->u.scb);
    }

    bsscfg = wlc_bsscfg_find_by_wlcif(wlc, wlcif);
    ASSERT(bsscfg != NULL);

    if (BSSCFG_STA(bsscfg) && bsscfg->BSS) {
#ifdef WLTDLS
        if (TDLS_ENAB(wlc->pub)) {
            /* When a TDLS is active and running we will have an SCB specified
             * to the peer MAC address. In that case we should find the directed
             * SCB rather than going with the scb of the AP
             */
            if (!ETHER_ISMULTI(addr) && !ETHER_ISNULLADDR(addr))
                scb = _wlc_tdls_scbfind_all(wlc, (struct ether_addr *)addr);
        }
#endif
        if (scb == NULL) {
            bss_addr = (char *)&bsscfg->BSSID;
            /* We may have zeroed bssid, get previos one */
            if (ETHER_ISNULLADDR(bss_addr))
                bss_addr = (char *)&bsscfg->prev_BSSID;
            scb = wlc_scbfind(wlc, bsscfg, (struct ether_addr *)bss_addr);
        }
    } else if (!ETHER_ISMULTI(addr)) {
        scb = wlc_scbfind(wlc, bsscfg, (struct ether_addr *)addr);
    } else {
        scb = bsscfg->bcmc_scb;
    }

    return scb;
} /* wlc_scbfind_from_wlcif */

static void
wlc_scb_hash_add(scb_module_t *scbstate, wlc_bsscfg_t *bsscfg, enum wlc_bandunit bandunit,
                 struct scb *scb)
{
    scb_bsscfg_cubby_t *scb_cfg;
    int indx;
    struct scb_info *scbinfo;

    ASSERT(bsscfg != NULL);

    scb->bandunit = bandunit;

    scb_cfg = SCB_BSSCFG_CUBBY(scbstate, bsscfg);
    indx = SCBHASHINDEX(scb_cfg->nscbhash, scb->ea.octet);
    scbinfo = SCBINFO(scbstate, scb_cfg->scbhash[bandunit][indx]);

    SCBINFO(scbstate, scb)->hashnext = scbinfo;
#ifdef SCB_MEMDBG
    SCBINFO(scbstate, scb)->hashnext_copy = SCBINFO(scbstate, scb)->hashnext;
#endif

    scb_cfg->scbhash[bandunit][indx] = scb;
}

static void
wlc_scb_hash_del(scb_module_t *scbstate, wlc_bsscfg_t *bsscfg, struct scb *scbd)
{
    scb_bsscfg_cubby_t *scb_cfg;
    int indx;
    struct scb_info *scbinfo;
    struct scb_info *remove = SCBINFO(scbstate, scbd);
    enum wlc_bandunit bandunit = scbd->bandunit;

    ASSERT(bsscfg != NULL);

    scb_cfg = SCB_BSSCFG_CUBBY(scbstate, bsscfg);
    indx = SCBHASHINDEX(scb_cfg->nscbhash, scbd->ea.octet);

    /* delete it from the hash */
    scbinfo = SCBINFO(scbstate, scb_cfg->scbhash[bandunit][indx]);
    /* If SCB is not in the hash table, return */
    if (scbinfo == NULL) {
        goto done;
    }

    SCBSANITYCHECK(scbstate, SCBPUB(scbstate, scbinfo));
    /* special case for the first */
    if (scbinfo == remove) {
        if (scbinfo->hashnext)
            SCBSANITYCHECK(scbstate, SCBPUB(scbstate, scbinfo->hashnext));
        scb_cfg->scbhash[bandunit][indx] =
                SCBPUB(scbstate, scbinfo->hashnext);
    } else {
        for (; scbinfo; scbinfo = scbinfo->hashnext) {
            SCBSANITYCHECK(scbstate, SCBPUB(scbstate, scbinfo->hashnext));
            if (scbinfo->hashnext == remove) {
                scbinfo->hashnext = remove->hashnext;
#ifdef SCB_MEMDBG
                scbinfo->hashnext_copy = scbinfo->hashnext;
#endif
                break;
            }
        }
    done:
        if (scbinfo == NULL) {
            WL_ERROR(("wl%d.%d: Unable to find scbinfo, scb = %p in the hash table",
                WLCWLUNIT(bsscfg->wlc), WLC_BSSCFG_IDX(bsscfg),
                OSL_OBFUSCATE_BUF(scbd)));
        }
    }
} /* wlc_scb_hash_del */

void
wlc_scb_sortrates(wlc_info_t *wlc, struct scb *scb)
{
    wlcband_t *band = wlc_scbband(wlc, scb);

    wlc_rate_hwrs_filter_sort_validate(&scb->rateset /* [in+out] */,
        &band->hw_rateset /* [in] */, FALSE,
        wlc->stf->op_txstreams);
}

void
BCMINITFN(wlc_scblist_validaterates)(wlc_info_t *wlc)
{
    struct scb *scb;
    struct scb_iter scbiter;

    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        wlc_scb_sortrates(wlc, scb);
        if (scb->rateset.count == 0)
            wlc_scbfree(wlc, scb);
    }
}

struct scb *
wlc_scbfind_dualband(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
    const struct ether_addr *addr)
{
    struct scb *scb;
    enum wlc_bandunit bandunit;

    scb = wlc_scbfind(wlc, bsscfg, addr);
    if (scb) {
        return scb;
    }

    if (IS_SINGLEBAND(wlc))
        return NULL;

    FOREACH_WLC_BAND(wlc, bandunit) {
        scb = wlc_scbfindband(wlc, bsscfg, addr, bandunit);
        if (scb != NULL) {
            break;
        }
    }

    return scb;
}

#if defined(BCMDBG)
void
wlc_scb_dump_scb(wlc_info_t *wlc, wlc_bsscfg_t *cfg, struct scb *scb, struct bcmstrbuf *b, int idx)
{
    int i;
    char flagstr[64];
    char flagstr2[64];
    char flagstr3[64];
    char statestr[64];
#ifdef AP
    char ssidbuf[SSID_FMT_BUF_LEN] = "";
#endif /* AP */
    scb_module_t *scbstate = wlc->scbstate;

    bcm_format_flags(scb_flags, scb->flags, flagstr, 64);
    bcm_format_flags(scb_flags2, scb->flags2, flagstr2, 64);
    bcm_format_flags(scb_flags3, scb->flags3, flagstr3, 64);
    bcm_format_flags(scb_states, scb->state, statestr, 64);

    if (SCB_INTERNAL(scb))
        bcm_bprintf(b, "  I");
    else
        bcm_bprintf(b, "%3d", idx);
    bcm_bprintf(b, "%s "MACF"\n", (scb->permanent? "*":" "),
        ETHER_TO_MACF(scb->ea));

    bcm_bprintf(b, "     State:0x%02x (%s) Used:%d(%d)\n",
        scb->state, statestr, scb->used,
        (int)(scb->used - wlc->pub->now));
    bcm_bprintf(b, "     Band:%s", wlc_bandunit_name(scb->bandunit));
    bcm_bprintf(b, "\n");
    bcm_bprintf(b, "     Flags:0x%x", scb->flags);
    if (flagstr[0] != '\0')
        bcm_bprintf(b, " (%s)", flagstr);
    bcm_bprintf(b, "\n");
    bcm_bprintf(b, "     Flags2:0x%x", scb->flags2);
    if (flagstr2[0] != '\0')
        bcm_bprintf(b, " (%s)", flagstr2);
    bcm_bprintf(b, "\n");
    bcm_bprintf(b, "     Flags3:0x%x", scb->flags3);
    if (flagstr3[0] != '\0')
        bcm_bprintf(b, " (%s)", flagstr3);
    bcm_bprintf(b, "\n");
    bcm_bprintf(b, "\n");
#ifdef WL_HAPD_WDS
    if (SCB_LEGACY_WDS(scb)) {
        bcm_bprintf(b, "     link_event %d", scb->wds->link_event);
        bcm_bprintf(b, "\n");
    }
#endif /* WL_HAPD_WDS */
    if (cfg != NULL)
        bcm_bprintf(b, "     Cfg:%d(%p)", WLC_BSSCFG_IDX(cfg), OSL_OBFUSCATE_BUF(cfg));

    bcm_bprintf(b, "\n");

    wlc_dump_rateset("     rates", &scb->rateset, b);
    bcm_bprintf(b, "\n");

    bcm_bprintf(b, "     Prop HT rates support:%d\n",
                SCB_HT_PROP_RATES_CAP(scb));

#ifdef AP
    if (cfg != NULL && BSSCFG_AP(cfg)) {
        bcm_bprintf(b, "     AID:0x%x PS:%d WDS:%d(%p)",
                    scb->aid, scb->PS, (scb->wds ? 1 : 0),
                    OSL_OBFUSCATE_BUF(scb->wds));
        wlc_format_ssid(ssidbuf, cfg->SSID, cfg->SSID_len);
        bcm_bprintf(b, " BSS %d \"%s\"\n",
                    WLC_BSSCFG_IDX(cfg), ssidbuf);
    }
#endif
#ifdef STA
    if (cfg != NULL && BSSCFG_STA(cfg)) {
        bcm_bprintf(b, "     MAXSP:%u DEFL:0x%x TRIG:0x%x DELV:0x%x\n",
                    scb->apsd.maxsplen, scb->apsd.ac_defl,
                    scb->apsd.ac_trig, scb->apsd.ac_delv);
    }
#endif
    bcm_bprintf(b,  "     WPA_auth 0x%x wsec 0x%x\n", scb->WPA_auth, scb->wsec);

#if defined(STA) && defined(DBG_BCN_LOSS)
    bcm_bprintf(b,    "      last_rx:%d last_rx_rssi:%d last_bcn_rssi: "
                "%d last_tx: %d\n",
                scb->dbg_bcn.last_rx, scb->dbg_bcn.last_rx_rssi, scb->dbg_bcn.last_bcn_rssi,
                scb->dbg_bcn.last_tx);
#endif

#if defined(WL_PS_SCB_TXFIFO_BLK)
    bcm_bprintf(b, "ps_txfifo_blk:%d pkts_inflt_fifocnt_tot:%d\n",
            SCB_PS_TXFIFO_BLK(scb), SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb));
#endif /* WL_PS_SCB_TXFIFO_BLK */

    for (i = 0; i < NUMPRIO; i++) {
        if (SCB_PKTS_INFLT_FIFOCNT_VAL(scb, i)) {
            bcm_bprintf(b, "pkts_inflt_fifocnt: prio-%d =>%d\n", i,
                SCB_PKTS_INFLT_FIFOCNT_VAL(scb, i));
        }
    }

    for (i = 0; i < (NUMPRIO * 2); i++) {
        if (SCB_PKTS_INFLT_CQCNT_VAL(scb, i)) {
            bcm_bprintf(b, "pkts_inflt_cqcnt: prec-%d =>%d\n", i,
                SCB_PKTS_INFLT_CQCNT_VAL(scb, i));
        }
    }

    bcm_bprintf(b, "scb base size: %u\n", (uint)sizeof(struct scb));
    bcm_bprintf(b, "-------- scb cubbies --------\n");

    wlc_cubby_dump(scbstate->cubby_info, scb, wlc_scb_sec_sz, scbstate, b);

    /* display scb state change callbacks */
    bcm_bprintf(b, "-------- state change notify list --------\n");
    bcm_notif_dump_list(scbstate->scb_state_notif_hdl, b);

#ifdef MEM_ALLOC_STATS
    /* display memory usage information */
    bcm_bprintf(b, "SCB Memory Allocated: %d bytes\n", scb->mem_bytes);
#endif /* MEM_ALLOC_STATS */
} /* wlc_scb_dump_scb */

static void
wlc_scb_bsscfg_dump(void *context, wlc_bsscfg_t *cfg, struct bcmstrbuf *b)
{
    scb_module_t *scbstate = (scb_module_t *)context;
    wlc_info_t *wlc = scbstate->wlc;
    int k;
    struct scb *scb;
    struct scb_iter scbiter;

    bcm_bprintf(b, "idx  ether_addr\n");
    k = 0;
    FOREACH_BSS_SCB(scbstate, &scbiter, cfg, scb) {
        wlc_scb_dump_scb(wlc, cfg, scb, b, k);
        k++;
    }

    return;
}

static int
wlc_scb_dump(wlc_info_t *wlc, struct bcmstrbuf *b)
{
    int32 idx;
    wlc_bsscfg_t *bsscfg;
    scb_module_t *scbstate = wlc->scbstate;

#ifdef SCB_MEMDBG
    bcm_bprintf(b, "# of scbs: %u, scballoced[%u] scbfreed[%u] freelistcount[%u]\n",
        scbstate->nscb, scbstate->scballoced, scbstate->scbfreed,
        scbstate->freelistcount);
#else
    bcm_bprintf(b, "# of scbs: %u\n", scbstate->nscb);
#endif /* SCB_MEMDBG */

    FOREACH_BSS(wlc, idx, bsscfg) {
        wlc_scb_bsscfg_dump(scbstate, bsscfg, b);
    }

    return 0;
}
#endif

int
wlc_scb_state_upd_register(wlc_info_t *wlc, scb_state_upd_cb_t fn, void *arg)
{
    bcm_notif_h hdl = wlc->scbstate->scb_state_notif_hdl;

    return bcm_notif_add_interest(hdl, (bcm_notif_client_callback)fn, arg);
}

int
wlc_scb_state_upd_unregister(wlc_info_t *wlc, scb_state_upd_cb_t fn, void *arg)
{
    bcm_notif_h hdl = wlc->scbstate->scb_state_notif_hdl;

    return bcm_notif_remove_interest(hdl, (bcm_notif_client_callback)fn, arg);
}

void
wlc_scb_cleanup_unused(wlc_info_t *wlc)
{
    struct scb *scb;
    struct scb_iter scbiter;

    wlc_bsscfg_t *bsscfg = NULL;

    if (!wlc->cleanup_unused_scbs)
        return;
    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        bsscfg = SCB_BSSCFG(scb);
        if (!BSSCFG_STA(bsscfg) || (bsscfg->BSS && SCB_ASSOCIATED(scb)) ||
            (bsscfg->wlc != wlc) || BSSCFG_IS_TDLS(bsscfg))
        {
#ifdef CONFIG_TENDA_PRIVATE_KM
            if (km_eventscenter_wifiev_sta_update_rssi_handler) {
                km_eventscenter_wifiev_sta_update_rssi_handler(scb->ea.octet, wlc_lq_rssi_get(wlc, scb->bsscfg, scb));
            }
#endif
            continue;
        }
        /* we release the SCB if it was idle for more then 10 seconds */
        /* we could start a join process with scb that was idle for almost 10 seocnds */
        /* and in a rare case the watchdog for 10 seconds expiration would come */
        /* before the join complete and before we update "scb->used". to prevent this */
        /* problem we do not release SCB that is in the middle of join */
        if (!scb->permanent &&
            ((wlc->pub->now - scb->used) >= 10) && !SCB_DURING_JOIN(scb))
            {
                wlc_scbfree(wlc, scb);
            }
    }
}

void
wlc_scbfind_delete(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, struct ether_addr *ea)
{
    uint i;
#if defined(BCMDBG) || defined(WLMSG_ASSOC)
    char eabuf[ETHER_ADDR_STR_LEN];
#endif /* BCMDBG || WLMSG_ASSOC */
    struct scb *scb;

    FOREACH_WLC_BAND(wlc, i) {
        scb = wlc_scbfindband(wlc, bsscfg, ea, i);
        if (scb) {
            WL_ASSOC(("wl%d: %s: scb for the STA-%s"
                " already exists\n", wlc->pub->unit, __FUNCTION__,
                bcm_ether_ntoa(ea, eabuf)));
            wlc_scbfree(wlc, scb);
        }
    }
}

#if defined(STA) && defined(DBG_BCN_LOSS)
int
wlc_get_bcn_dbg_info(wlc_bsscfg_t *cfg, struct ether_addr *addr,
    struct wlc_scb_dbg_bcn *dbg_bcn)
{
    wlc_info_t *wlc = cfg->wlc;

    if (cfg->BSS) {
        struct scb *scb = wlc_scbfindband(wlc, cfg, addr,
            CHSPEC_BANDUNIT(cfg->current_bss->chanspec));
        if (scb) {
            memcpy(dbg_bcn, &(scb->dbg_bcn), sizeof(struct wlc_scb_dbg_bcn));
            return BCME_OK;
        }
    }
    return BCME_ERROR;
}
#endif /* defined(STA) && defined(DBG_BCN_LOSS) */

/**
 * These function allocates/frees a secondary cubby in the secondary cubby area.
 */
void *
wlc_scb_sec_cubby_alloc(wlc_info_t *wlc, struct scb *scb, uint secsz)
{
    scb_module_t *scbstate = wlc->scbstate;

    return wlc_cubby_sec_alloc(scbstate->cubby_info, scb, secsz);
}

void
wlc_scb_sec_cubby_free(wlc_info_t *wlc, struct scb *scb, void *secptr)
{
    scb_module_t *scbstate = wlc->scbstate;

    wlc_cubby_sec_free(scbstate->cubby_info, scb, secptr);
}

#if defined(BCM_PKTFWD_FLCTL)

/* Get SCB queue lengths of a station. */
void
wlc_scb_get_link_credits(wlc_info_t *wlc, wlc_if_t *wlcif, uint8 *addr,
    uint16 flowid, int32 *credits)
{
    struct scb *scb;

    if (flowid != ID16_INVALID) {
        scb = wlc_scb_flowid_lookup(wlc, flowid);
    } else {
        scb = wlc_scbfind_from_wlcif(wlc, wlcif, addr);
    }

    ASSERT(scb != NULL);
    /* TODO: Check for station capability (AMPDU, NAR, etc) and update credits accordingly */
    wlc_ampdu_get_link_credits(wlc, scb, credits);
}

/**
 * Link SCB to the wfd->pktc layer (via the PT table).
 * In NIC mode, wfd->pktc is effectively the "bus" layer.
 *
 * This function is invoked, when the Linux bridge creates a new entry into
 * the PT table via PKTC_TBL_UPDATE.
 */
int
wlc_scb_link_update(wlc_info_t *wlc, wlc_if_t *wlcif, uint8 *addr,
    uint16 *flowid)
{
    struct scb *scb;

    if (ETHER_ISMULTI(addr) || ETHER_ISNULLADDR(addr)) {
        WL_INFORM(("SCB linkup multi or NULL addr:"MACF"\n", ETHERP_TO_MACF(addr)));
        return BCME_ERROR;
    }

    scb = wlc_scbfind_from_wlcif(wlc, wlcif, addr);

    if ((scb == NULL) || !(SCB_ASSOCIATED(scb) || SCB_LEGACY_WDS(scb))) {
        WL_INFORM(("SCB linkup cannot find scb for MAC:"MACF"\n", ETHERP_TO_MACF(addr)));
        return BCME_ERROR;
    }

    /* Return SCB flowid */
    wlc_scb_host_ring_link(wlc, 0, 0, scb, flowid);

    if (!SCB_FLOWID_VALID(*flowid)) {
        *flowid = ID16_INVALID;
    }

    return BCME_OK;
} /* wlc_scb_link_update() */

#endif /* BCM_PKTFWD_FLCTL */

void
wlc_scb_pause_traffic(wlc_info_t* wlc, struct scb *scb, uint8 ac)
{
    int prio;
    uint32 pkt_inflt = 0;

    ASSERT(ac < NBBY);
    if (scb->ac_suspend & (1 << ac)) {
        return;
    }
    scb->ac_suspend |= 1 << ac;

    for (prio = 0; prio < NUMPRIO; prio++) {
        if (prio2ac[prio] == ac) {
            pkt_inflt += SCB_PKTS_INFLT_FIFOCNT_VAL(scb, prio);
#ifdef WLTAF
            wlc_taf_sched_state(wlc->taf_handle, scb, prio, 0,
                TAF_NO_SOURCE, TAF_SCHED_STATE_DL_SUSPEND);
#endif /* WLTAF */
        }
    }

    WL_INFORM(("SCB "MACF" AC %d closing link. Packets pending: %d\n",
        ETHER_TO_MACF(scb->ea), ac, pkt_inflt));

#ifdef WLCFP
    if (CFP_ENAB(wlc->pub) == TRUE) {
        wlc_cfp_tcb_upd_pause_state(wlc, scb, TRUE);
    }
#endif /* WLCFP */
}

void
wlc_scb_resume_traffic(wlc_info_t* wlc, struct scb *scb, uint8 ac)
{
#ifdef WLTAF
    int prio;
#endif /* WLTAF */

    if (!(scb->ac_suspend & (1 << ac))) {
        return;
    }

    if ((scb->ac_suspend & 0xF) == 0xF) {
      if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)) {
            return;
        } else {
#ifdef WL_MU_TX
            wlc_mutx_scb_check_txdrain_complete(wlc, scb);
            WL_MUTX(("SCB "MACF" AC_ALL opening link\n",
                ETHER_TO_MACF(scb->ea)));
#endif
            scb->ac_suspend &= ~(0xF);
#ifdef WLTAF
        for (prio = 0; prio < NUMPRIO; prio++) {
            wlc_taf_sched_state(wlc->taf_handle, scb, prio, 0,
                TAF_NO_SOURCE, TAF_SCHED_STATE_DL_RESUME);
        }
#endif /* WLTAF */
#ifdef WLCFP
        if (CFP_ENAB(wlc->pub) == TRUE) {
            wlc_cfp_tcb_upd_pause_state(wlc, scb, FALSE);
        }
#endif /* WLCFP */

        }
    } else if (wlc_fifo_scb_inflt_ac(scb, ac) == 0) {
        WL_INFORM(("SCB "MACF" AC %d opening link\n",
            ETHER_TO_MACF(scb->ea), ac));
        scb->ac_suspend &= ~(1 << ac);

#ifdef WLTAF
        for (prio = 0; prio < NUMPRIO; prio++) {
            if (prio2ac[prio] == ac) {
                wlc_taf_sched_state(wlc->taf_handle, scb, prio, 0,
                    TAF_NO_SOURCE, TAF_SCHED_STATE_DL_RESUME);
            }
        }
#endif /* WLTAF */
#ifdef WLCFP
        if (CFP_ENAB(wlc->pub) == TRUE) {
            wlc_cfp_tcb_upd_pause_state(wlc, scb, FALSE);
        }
#endif /* WLCFP */
    }
}

static int
BCMATTACHFN(wlc_scb_flowid_attach)(wlc_info_t* wlc, scb_module_t *scbstate)
{
    uint16 idx;

    ASSERT(scbstate);

    /* Construct a 16bit flowid allocator */

    /* user_flowid_allocator is used to allocate flow ids for unicast traffic
     * with flowids in the inclusive range [ 1 .. MAXSCB ].
     *
     * int_flowid_allocator is used to allocate  flow ids for the "internal"
     * SCBs (BCM per BSS, HWRS and OLPC), and will have value in the inclusive
     * range [ MAXSCB+1 .. SCB_MAX_FLOWS - 1 ],
     */
    scbstate->user_flowid_allocator =
        id16_map_init(wlc->osh, SCB_USER_FLOWID_TOTAL, SCB_USER_FLOWID_STARTID);
    if (scbstate->user_flowid_allocator == NULL) {
        WL_ERROR(("wl%d: %s: user scb flowid allocator init failure\n",
            wlc->pub->unit, __FUNCTION__));
        return BCME_NOMEM;
    }

    /* Internal SCB flowid alloctor */
    scbstate->int_flowid_allocator =
        id16_map_init(wlc->osh, SCB_INT_FLOWID_TOTAL, SCB_INT_FLOWID_STARTID);
    if (scbstate->int_flowid_allocator == NULL) {
        WL_ERROR(("wl%d: %s: internal scb flowid allocator init failure\n",
            wlc->pub->unit, __FUNCTION__));
        return BCME_NOMEM;
    }

    /* Allocate a lookup table for flowid to scb ptr mapping */
    scbstate->scb_lkp = MALLOCZ(wlc->osh,
        ((sizeof(scb_lookup_t)) * (SCB_MAX_FLOWS)));

    if (scbstate->scb_lkp == NULL) {
        WL_ERROR(("wl%d: %s: Failed to allocate %d bytes\n",
            wlc->pub->unit, __FUNCTION__,
            (int)((sizeof(scb_lookup_t)) * (SCB_MAX_FLOWS))));
        return BCME_NOMEM;
    }

    /* Allocate a lookup table for AMT idx to SCB flow ID lookup */
    scbstate->amt_lookup = MALLOCZ(wlc->osh,
        (sizeof(uint16) * AMT_SIZE(wlc->pub->corerev)));
    if (scbstate->amt_lookup == NULL) {
        WL_ERROR(("wl%d: %s: AMT lookup table init failed  \n",
            wlc->pub->unit, __FUNCTION__));
        return BCME_NOMEM;
    }

    /* Initialize the table with SCB_FLOWID_INVALID */
    for (idx = 0; idx < AMT_SIZE(wlc->pub->corerev); idx++) {
        scbstate->amt_lookup[idx] = SCB_FLOWID_INVALID;
    }

    return BCME_OK;
}

static void
BCMATTACHFN(wlc_scb_flowid_detach)(wlc_info_t* wlc, scb_module_t *scbstate)
{
    /* Remove AMT Lookup table */
    if (scbstate->amt_lookup) {
        MFREE(wlc->osh, scbstate->amt_lookup,
            (sizeof(uint16) * AMT_SIZE(wlc->pub->corerev)));
    }

    /* Remove SCB Flowid Lookup table */
    if (scbstate->scb_lkp) {
        MFREE(wlc->osh, scbstate->scb_lkp,
            ((sizeof(scb_lookup_t)) * (SCB_MAX_FLOWS)));
    }

    /* Remove Internal scb flowid allocator */
    if (scbstate->int_flowid_allocator) {
        id16_map_fini(wlc->osh, scbstate->int_flowid_allocator);
        scbstate->int_flowid_allocator  = NULL;
    }

    /* Remove User scb flowid allocator */
    if (scbstate->user_flowid_allocator) {
        id16_map_fini(wlc->osh, scbstate->user_flowid_allocator);
        scbstate->user_flowid_allocator = NULL;
    }
}

static int
wlc_scb_flowid_init(wlc_info_t* wlc, scb_module_t *scbstate, struct scb* scb)
{
    uint16 flowid;
    uint8 incarn;

    /* Allocate a SCB flowid */
    if (SCB_INTERNAL(scb)) {
        ASSERT(scbstate->int_flowid_allocator != NULL);
        flowid = id16_map_alloc(scbstate->int_flowid_allocator);
    } else {
        ASSERT(scbstate->user_flowid_allocator != NULL);
        flowid = id16_map_alloc(scbstate->user_flowid_allocator);
    }

    if (flowid == ID16_INVALID) {
        WL_ERROR(("wl%d: %s Failed to allocate flowid for scb %p ea "MACF"\n",
            wlc->pub->unit, __FUNCTION__, scb, ETHER_TO_MACF(scb->ea)));
        return BCME_ERROR;
    }

    /* Check that the flowid is not in use */
    ASSERT(SCB_FLOWID_LKP(scbstate, flowid) == NULL);

    /* Increment 2 bit incarnation id for this flow */
    incarn = (SCB_FLOWID_INCARN_LKP(scbstate, flowid) + 1) &
        __SCB_UCODE_STS_AMT_INCARN_MASK;

    /* Register the SCB flowid */
    SCB_FLOWID_GLOBAL(scb) = SCB_FLOWID_GLOBAL_SET(WLC_UNIT(wlc), incarn, flowid);

    /* Fillup the SCB lookup table */
    SCB_FLOWID_LKP(scbstate, flowid) = scb;

    /* Update incarnation id for this flow */
    SCB_FLOWID_INCARN_LKP(scbstate, flowid) = incarn;

#ifdef BCMPCIEDEV_ENABLED
    /* Host ringid valid for only pcie donge builds for now */
    /* Initialize host ringids */
    {
        uint8 idx;
        for (idx = 0; idx < NUMPRIO; idx++) {
            SCB_HOST_RINGID(scb, idx) = SCB_HOST_RINGID_INVALID;
        }
    }
#endif

    WL_INFORM(("wl%d %s :  scb %p ea "MACF", id %x  Global id %x \n",
        wlc->pub->unit, __FUNCTION__, scb, ETHER_TO_MACF(scb->ea),
        flowid, SCB_FLOWID_GLOBAL(scb)));

    return BCME_OK;
}

static void
wlc_scb_flowid_deinit(wlc_info_t* wlc, scb_module_t *scbstate, struct scb* scb)
{
    uint16 flowid;

    ASSERT(scb);

    /* SCB flowid */
    flowid = SCB_FLOWID(scb);
    ASSERT_SCB_FLOWID(flowid);

    /* Delink SCB flowid from the AMT lookup table */
    wlc_scb_amt_delink(wlc, AMT_FLOWID_INVALID(wlc->pub->corerev), flowid);

#if defined(BCM_PKTFWD)
    /* Delete the scb->ea reference from PKTFWD layer */
    wl_pktfwd_lut_del((uint8 *)(&scb->ea), NULL);
#endif /* BCM_PKTFWD */

    /* Reset the stored flowid */
    SCB_FLOWID_GLOBAL(scb) = SCB_FLOWID_INVALID;

    /* Remove the SCB lookup */
    SCB_FLOWID_LKP(scbstate, flowid) = NULL; /* release from scb cfp list */

    /* Release the flowid */
    if (SCB_INTERNAL(scb)) {
        id16_map_free(scbstate->int_flowid_allocator, flowid);
    } else {
        id16_map_free(scbstate->user_flowid_allocator, flowid);
    }

    WL_INFORM(("wl%d %s :  scb %p ea "MACF", id %d \n",
        wlc->pub->unit, __FUNCTION__,
        scb, ETHER_TO_MACF(scb->ea), flowid));
}

static int
wlc_scb_flowid_dump(void * ctx, struct bcmstrbuf *b)
{
    uint16 radio_i, flowid_i;
    wlc_info_t* wlc;
    struct scb* scb;

    for (radio_i = 0; radio_i < WLC_UNIT_MAX; radio_i++) {
        wlc = WLC_G(radio_i);

        if (wlc == NULL)
            continue;

        for (flowid_i = 0; flowid_i < SCB_FLOWID_INVALID; flowid_i++) {
            scb = SCB_FLOWID_LKP(wlc->scbstate, flowid_i);

            if (scb == NULL)
                continue;

            bcm_bprintf(b, "Radio id %d Flowid %d SCB %p <"MACF"> Internal %d \n",
                radio_i, flowid_i, scb, ETHER_TO_MACF(scb->ea),
                SCB_INTERNAL(scb));
        }
    }
    return 0;
}

/**
 * Link AMT A2[Transmitter] index with SCB flow ID
 *
 * Loop through available SCB flows to do address comparison.
 * Bind SCB and AMT flow ids if address is found.
 */
int
wlc_scb_amt_link(wlc_info_t *wlc, int amt_idx,
    const struct ether_addr *amt_addr)
{
    uint16        flowid;    /* Flowid iterator */
    struct scb    *scb;        /* SCB */
    bool found;
    int ret = BCME_ERROR;

    ASSERT_AMT_IDX(wlc, amt_idx);

    /* Initialize */
    found = FALSE;
    BCM_REFERENCE(found);

    /* Skip for stamon reserved amt entry */
    if (STAMON_ENAB(wlc->pub) && wlc_stamon_is_slot_reserved(wlc, amt_idx)) {
        return ret;
    }

    WL_INFORM(("wl%d : %s  amtid %d wlc bandunit %d \n",
        wlc->pub->unit, __FUNCTION__,  amt_idx, wlc->band->bandunit));

    for (flowid = 0; flowid < SCB_MAX_FLOWS; flowid++) {
        scb = SCB_FLOWID_LKP(wlc->scbstate, flowid);

        if (scb == NULL)
            continue;

        /* Internal SCBs seem to end up with BC/MC address
         * Dont use them for AMT linkups
         */
        if (SCB_INTERNAL(scb))
            continue;

        if ((eacmp((const char*)amt_addr, (const char*)&scb->ea) == 0) &&
            (wlc->band->bandunit == scb->bandunit))
        {
            /* Check for unique Transmitter Address.
             *
             * Assumption is every AMT TA would map to unique scb.
             * If there are multiple scbs poitning to same TA,
             * whole flow lookup based on AMT fails.
             *
             */
            ASSERT(found == FALSE);
            found = TRUE;

            ASSERT(SCB_FLOWID_AMT_LKP(wlc->scbstate, amt_idx) == SCB_FLOWID_INVALID);
            SCB_FLOWID_AMT_LKP(wlc->scbstate, amt_idx) = flowid;
            ret = SCB_FLOWID_INCARN_LKP(wlc->scbstate, flowid);
        }
    }

    return ret;
}

/**
 * De-Link CFP-AMT
 *
 * Reset AMT lookup table entry for given AMT index
 * For a given cfp flowid loop through amt lookup table and delink
 */
void
wlc_scb_amt_delink(wlc_info_t *wlc, int amt_idx, uint16 flowid)
{
    uint16 cur_id;

    WL_INFORM(("wl%d: %s : Delink AMT idx %d Flowid %d\n",
        wlc->pub->unit, __FUNCTION__, amt_idx, flowid));

    if (amt_idx == AMT_FLOWID_INVALID(wlc->pub->corerev)) {
        int amt_size;
        /* Triggered from a CFP cubby deinit */
        /* Search for the given cfp_flowid and delink */
        ASSERT_SCB_FLOWID(flowid);

        amt_size = AMT_SIZE(wlc->pub->corerev);
        for (amt_idx = 0; amt_idx < amt_size; amt_idx++) {
            cur_id = SCB_FLOWID_AMT_LKP(wlc->scbstate, amt_idx);
            if (cur_id == flowid) {
                SCB_FLOWID_AMT_LKP(wlc->scbstate, amt_idx) = SCB_FLOWID_INVALID;
            }
        }
    } else {
        /* Triggered from AMT delink */
        ASSERT_AMT_IDX(wlc, amt_idx);
        ASSERT(flowid == SCB_FLOWID_INVALID);
        SCB_FLOWID_AMT_LKP(wlc->scbstate, amt_idx) = SCB_FLOWID_INVALID;
    }
}

/** Return Linked SCB flowid for given amt idx */
uint16
wlc_scb_amt_linkid_get(wlc_info_t *wlc, int amt_idx)
{
    if (AMT_IDX_VALID(wlc, amt_idx))
        return SCB_FLOWID_AMT_LKP(wlc->scbstate, amt_idx);
    else
        return SCB_FLOWID_INVALID;
}

int
wlc_scb_amt_incarnation_id_get(wlc_info_t *wlc, int amt_idx)
{
    uint16 flowid;

    if (!AMT_IDX_VALID(wlc, amt_idx))
        return 0;

    flowid = SCB_FLOWID_AMT_LKP(wlc->scbstate, amt_idx);

    if (!SCB_FLOWID_VALID(flowid))
        return 0;

    return SCB_FLOWID_INCARN_LKP(wlc->scbstate, flowid);
}

/* Lookup scb for a given amt index */
struct scb* BCMFASTPATH
wlc_scb_amt_lookup(wlc_info_t *wlc, uint16 amt_idx)
{
    struct scb* scb;
    uint16 flowid;
    uint8 frame_incarn;

    /* Check for amt idx validity */
    if (amt_idx & SCB_UCODE_STS_AMT_INVALID_MASK) {
        WL_INFORM(("wl%d %s : invalid amt : idx %x \n",
            wlc->pub->unit, __FUNCTION__, amt_idx));
        return NULL;
    }

    frame_incarn = (amt_idx & SCB_UCODE_STS_AMT_INCARN_MASK) >>
        SCB_UCODE_STS_AMT_INCARN_SHIFT;

    amt_idx = (amt_idx & SCB_UCODE_STS_AMT_IDX_MASK);

    /* AMT to SCB Flowid lookup */
    flowid = SCB_FLOWID_AMT_LKP(wlc->scbstate, amt_idx);

    /* Do incarnation checks only for valid flowids */
    if (!SCB_FLOWID_VALID(flowid)) {
        return NULL;
    }

    if (frame_incarn != SCB_FLOWID_INCARN_LKP(wlc->scbstate, flowid)) {
        SCB_FLOWID_INCARN_MISMATCH(wlc->scbstate, flowid)++;
        WL_INFORM(("wl%d Incarnation mismatch ::: amd itd %d frame incarn %d "
            "flowid %d scb incarn %d \n",
            wlc->pub->unit, amt_idx, frame_incarn, flowid,
            SCB_FLOWID_INCARN_LKP(wlc->scbstate, flowid)));
        return NULL;
    }

    /* Finally get to SCB lookup. */
    scb = SCB_FLOWID_LKP(wlc->scbstate, flowid);

    /* Better be a valid scb */
    ASSERT(scb);

    return scb;
}

/* Link host ringid with SCB flowid
 * Store host ringid inside SCB.
 * Pass SCB flowid into host layer
 */
void
wlc_scb_host_ring_link(wlc_info_t *wlc, uint16 ringid, uint8 tid,
        struct scb *scb,  uint16* flowid)
{

    ASSERT(scb);

#ifdef BCMPCIEDEV_ENABLED
    /* Host ringid valid for only pcie donge builds for now */
    /* Store the host ringid inside SCB */
    SCB_HOST_RINGID(scb, tid) = ringid;
#endif /* BCMPCIEDEV_ENABLED */

#if defined(WLSQS) && defined(WLTAF)
    /* Activate SQS source for the given tid */
    wlc_taf_link_state(wlc->taf_handle, scb, tid, TAF_SQSHOST, TAF_LINKSTATE_ACTIVE);
#endif

    /* Return SCB flowid */
    *flowid = SCB_FLOWID(scb);

    WL_INFORM(("wl%d: %s hostringid %d tid %d scb flowid %d\n",
        wlc->pub->unit, __FUNCTION__, ringid, tid, *flowid));
}

#ifdef BCMPCIEDEV
/* Delink SCB flow and BUS flow ring IDs */
static void
wlc_scb_flow_ring_delink(wlc_info_t* wlc, struct scb* scb)
{
    uint8 tid = 0;
    /* Remove all cross reference from BUS layer */
    wl_scb_flow_ring_delink(wlc->wl, SCB_FLOWID(scb));

    /* Remove all cross reference from SCB layer */
    for (tid = 0; tid < NUMPRIO; tid++) {
        wlc_scb_flow_ring_tid_delink(wlc, scb, tid);
    }
}

/* Delink an SCB from the host ringid */
int
wlc_scb_flow_ring_tid_delink(wlc_info_t *wlc, struct scb *scb, uint8 tid)
{
    ASSERT(scb);

    /* Host ringid valid for only pcie donge builds for now */
    if (SCB_HOST_RINGID_VALID(SCB_HOST_RINGID(scb, tid))) {
        SCB_HOST_RINGID(scb, tid) = SCB_HOST_RINGID_INVALID;
#if defined(WLSQS) && defined(WLTAF)
        /* Remove SQS source from TAF scheduler for the given flow */
        wlc_taf_link_state(wlc->taf_handle, scb, tid, TAF_SQSHOST,
            TAF_LINKSTATE_REMOVE);
#endif /* WLSQS && WLTAF */
        return BCME_OK;
    }
    return BCME_ERROR;
}

#endif /* BCMPCIEDEV */
/** Lookup scb for a given flowid */
struct scb* BCMFASTPATH
wlc_scb_flowid_lookup(wlc_info_t *wlc, uint16 flowid)
{
    struct scb* scb;

    /* Currently expect only the current radio local flowid */
    SCB_FLOWID_LOCAL_ASSERT(flowid);

    if (!SCB_FLOWID_VALID(flowid))
        return NULL;

    /* Per radio Lookup */
    scb = SCB_FLOWID_LKP(wlc->scbstate, flowid);

    return scb;
}

/* Lookup scb for a given global flowid */
struct scb* BCMFASTPATH
#ifdef BCMDBG
wlc_scb_flowid_global_lookup(uint16 flowid_g, void* call_site)
#else
wlc_scb_flowid_global_lookup(uint16 flowid_g)
#endif /* BCMDBG */
{
    struct scb* scb = NULL;
    wlc_info_t* wlc;
    uint16 flowid_local = SCB_FLOWID_LOCAL(flowid_g);

    if (!SCB_FLOWID_VALID(flowid_local))
        return NULL;

    /* Audit the radio id for wlc lookup */
    WLC_AUDIT_G(SCB_FLOWID_RADIO(flowid_g));

    wlc = WLC_G(SCB_FLOWID_RADIO(flowid_g));

    if (SCB_FLOWID_INCARN(flowid_g) != SCB_FLOWID_INCARN_LKP(wlc->scbstate, flowid_local)) {
#ifdef BCMDBG
        WL_ERROR(("wl%d: %s incarnation mismatch for flowid 0x%x, incarnation should be"
            " 0x%x. Called from 0x%p\n", wlc->pub->unit, __FUNCTION__,
            flowid_g, SCB_FLOWID_INCARN_LKP(wlc->scbstate, flowid_local),
            call_site));
#else
        WL_ERROR(("wl%d: %s incarnation mismatch for flowid 0x%x, incarnation should be"
            " 0x%x\n", wlc->pub->unit, __FUNCTION__,
            flowid_g, SCB_FLOWID_INCARN_LKP(wlc->scbstate, flowid_local)));
#endif /* BCMDBG */

        return NULL;
    }

    /* Per radio Lookup */
    scb = SCB_FLOWID_LKP(wlc->scbstate, flowid_local);

    return scb;
}

/* DO a SCB flowid lookup for a given mac address */
uint16
wlc_scb_flowid_addr_lookup(wlc_info_t *wlc, struct ether_addr *ea)
{
    uint16          idx;            /* Flowid iterator */
    struct scb      *scb;

    for (idx = 0; idx < SCB_MAX_FLOWS; idx++) {
        /* SCB flowid lookup */
        scb = wlc_scb_flowid_lookup(wlc, idx);

        if (scb == NULL)
            continue;

        if (!memcmp(&ea->octet, &scb->ea.octet, ETHER_ADDR_LEN)) {
            return idx;
        }
    }
    return SCB_FLOWID_INVALID;
}

#ifdef WLC_MEDIAN_RATE_REPORT
/*
 * Return median rate from an array of ratespec rates
 * only first NRATELOG entries are compared
 */
ratespec_t
wlc_scb_get_median_rate(ratespec_t *arr, int arrsz)
{
    int refrate;
    int i, j, leqcnt;
    ratespec_t rates[NRATELOG];
    int sorted[NRATELOG];

    if (arrsz > NRATELOG)
        arrsz = NRATELOG;

    /* sort rates, convert ratespec to numerical rates */
    for (i = 0; i < arrsz; i++) {
        rates[i] = RSPEC2KBPS(arr[i]);
        sorted[i] = 0;
        if (rates[i] == -1) {
            WL_NONE(("valid rates %d < %d\n", i, arrsz));
            return 0;
        }
    }

    for (i = 0; i < arrsz; i++) {
        refrate = rates[i];
        leqcnt = 0;
        for (j = 0; j < arrsz; j++) {
            if (rates[j] < refrate)
                leqcnt++;
        }

        for (; leqcnt < arrsz; leqcnt++) {
            if (!sorted[leqcnt]) {
                sorted[leqcnt] = arr[i];
                break;
            }
        }
    }

    return sorted[arrsz/2];
}
#endif /* WLC_MEDIAN_RATE_REPORT */
