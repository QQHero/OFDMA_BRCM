/*
 * Common interface to the 802.11 AP Power Save state per scb
 *
 * Copyright 2022 Broadcom
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
 * $Id: wlc_apps.c 810466 2022-04-07 12:44:30Z $
 */

/**
 * @file
 * @brief
 * Twiki: [WlDriverPowerSave]
 */

/* Define wlc_cfg.h to be the first header file included as some builds
 * get their feature flags thru this file.
 */
#include <wlc_cfg.h>

#ifndef AP
#error "AP must be defined to include this module"
#endif  /* AP */

#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <bcmendian.h>
#include <802.11.h>
#include <wpa.h>
#include <wlioctl.h>
#include <sbhndpio.h>
#include <sbhnddma.h>
#include <hnddma.h>
#include <d11.h>
#include <wlc_rate.h>
#include <wlc_pub.h>
#include <wlc_keymgmt.h>
#include <wlc_channel.h>
#include <wlc_bsscfg.h>
#include <wlc.h>
#include <wlc_ap.h>
#include <wlc_apps.h>
#include <wlc_scb.h>
#include <wlc_phy_hal.h>
#include <bcmwpa.h>
#include <wlc_bmac.h>
#ifdef PROP_TXSTATUS
#include <wlc_wlfc.h>
#endif
#ifdef WLTDLS
#include <wlc_tdls.h>
#endif
#include <wl_export.h>
#include <wlc_ampdu.h>
#include <wlc_pcb.h>
#ifdef WLTOEHW
#include <wlc_tso.h>
#endif /* WLTOEHW */
#include <wlc_ie_mgmt.h>
#include <wlc_ie_mgmt_ft.h>
#ifdef WLWNM
#include <wlc_wnm.h>
#endif
#include "wlc_txc.h"
#include <wlc_tx.h>
#include <wlc_mbss.h>
#include <wlc_txmod.h>
#include <wlc_pspretend.h>
#include <wlc_qoscfg.h>
#ifdef WL_BEAMFORMING
#include <wlc_txbf.h>
#endif /* WL_BEAMFORMING */
#ifdef BCMFRWDPOOLREORG
#include <hnd_poolreorg.h>
#endif /* BCMFRWDPOOLREORG */
#ifdef WLCFP
#include <wlc_cfp.h>
#endif
#include <wlc_he.h>

#if defined(HNDPQP)
#ifndef WL_USE_SUPR_PSQ
#error "WL_USE_SUPR_PSQ needs to be defined with HNDPQP"
#endif

#include <hnd_pqp.h>
#endif /* HNDPQP */

#ifdef WLTAF
#include <wlc_taf.h>
#endif
#ifdef PKTQ_LOG
#include <wlc_perf_utils.h>
#endif

#include <wlc_event_utils.h>
#include <wlc_pmq.h>
#include <wlc_twt.h>
#include <wlc_dump.h>

#ifndef WL_PS_STATS
#define WL_PS_STATS
#endif /*WL_PS_STATS*/
#ifdef WL_PS_STATS
#include <wlc_perf_utils.h>
#endif /* WL_PS_STATS */
#include <wlc_nar.h>

#ifdef PKTQ_LOG
#include <wlc_perf_utils.h>
#endif
#ifdef BCMPCIEDEV
#include <wlc_sqs.h>
#include <wlc_amsdu.h>
#endif /* BCMPCIEDEV */

// Forward declarations
static void wlc_apps_ps_timedout(wlc_info_t *wlc, struct scb *scb);
static bool wlc_apps_ps_send_one(wlc_info_t *wlc, struct scb *scb, uint prec_bmp, bool apsd,
    taf_scheduler_public_t *taf);
static bool wlc_apps_ps_send(wlc_info_t *wlc, struct scb *scb, uint prec_bmp, bool apsd,
    taf_scheduler_public_t *taf, uint count);
static bool wlc_apps_ps_enq_resp(wlc_info_t *wlc, struct scb *scb, void *pkt, int prec,
    taf_scheduler_public_t *taf);
static void wlc_apps_transmit_packet(void *ctx, struct scb *scb, void *pkt, uint prec);
static int wlc_apps_apsd_delv_count(wlc_info_t *wlc, struct scb *scb);
static int wlc_apps_apsd_ndelv_count(wlc_info_t *wlc, struct scb *scb);
static void wlc_apps_apsd_send(wlc_info_t *wlc, struct scb *scb);
static void wlc_apps_txq_to_psq(wlc_info_t *wlc, struct scb *scb);
static void wlc_apps_move_cpktq_to_psq(wlc_info_t *wlc, struct cpktq *cpktq, struct scb* scb);
static uint16 wlc_apps_scb_pktq_tot(wlc_info_t *wlc, struct scb *scb);
static INLINE void wlc_apps_tim_pvb(wlc_info_t *wlc, wlc_bsscfg_t *cfg, uint8 *offset,
    int16 *length);

#ifdef PROP_TXSTATUS
static void wlc_apps_move_to_psq(wlc_info_t *wlc, struct pktq *txq, struct scb* scb);
static void wlc_apps_nar_txq_to_psq(wlc_info_t *wlc, struct scb *scb);
static void wlc_apps_ampdu_txq_to_psq(wlc_info_t *wlc, struct scb *scb);
#endif /* PROP_TXSTATUS */

static void wlc_apps_apsd_eosp_send(wlc_info_t *wlc, struct scb *scb);
static int wlc_apps_bss_init(void *ctx, wlc_bsscfg_t *cfg);
static void wlc_apps_bss_deinit(void *ctx, wlc_bsscfg_t *cfg);
static void wlc_apps_bss_wd_ps_check(void *handle);
#if defined(BCMDBG)
static void wlc_apps_dump(void *ctx, struct bcmstrbuf *b);
static void wlc_apps_bss_dump(void *ctx, wlc_bsscfg_t *cfg, struct bcmstrbuf *b);
static void wlc_apps_scb_psinfo_dump(void *context, struct scb *scb, struct bcmstrbuf *b);
static void wlc_apps_scb_psinfo_complete_dump(void *context, struct scb *scb, struct bcmstrbuf *b);
#else
/* Use NULL to pass as reference on init */
#define wlc_apps_bss_dump NULL
#define wlc_apps_scb_psinfo_dump NULL
#endif
static void wlc_apps_omi_waitpmq_war(wlc_info_t *wlc);

#ifdef WLTWT
static void wlc_apps_twt_enter_ready(wlc_info_t *wlc, scb_t *scb);
#else
#define wlc_apps_twt_enter_ready(a, b) do {} while (0)
#endif /* WLTWT */

static void wlc_apps_bcmc_ps_off_start(wlc_info_t *wlc, struct scb *bcmc_scb);
static void wlc_apps_bcmc_ps_off_done(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
static void wlc_apps_bcmc_suppress_done(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);

/* IE mgmt */
static uint wlc_apps_calc_tim_ie_len(void *ctx, wlc_iem_calc_data_t *data);
static int wlc_apps_write_tim_ie(void *ctx, wlc_iem_build_data_t *data);
static void wlc_apps_scb_state_upd_cb(void *ctx, scb_state_upd_data_t *notif_data);
static void wlc_apps_bss_updn(void *ctx, bsscfg_up_down_event_data_t *evt);
static int wlc_apps_send_psp_response_cb(wlc_info_t *wlc, wlc_bsscfg_t *cfg, void *pkt, void *data);

#ifdef WL_PS_STATS
/* ps stats */
static int wlc_apps_stats_dump(void *ctx, struct bcmstrbuf *b);
static int wlc_apps_stats_dump_clr(void *ctx);
static void wlc_apps_ps_stats_datablock_start(wlc_info_t *wlc);
static void wlc_apps_ps_stats_datablock_stop(wlc_info_t *wlc);
void wlc_apps_upd_pstime(struct scb *scb);
#else
#define wlc_apps_ps_stats_datablock_start(wlc);
#define wlc_apps_ps_stats_datablock_stop(wlc);
#define wlc_apps_upd_pstime(scb);
#endif /* WL_PS_STATS */

#ifdef WLTAF
static void wlc_apps_taf_tag_release(wlc_info_t *wlc, taf_scheduler_public_t *taf, void *p,
    scb_t *scb);
#endif /* WLTAF */

#ifdef HNDPQP
static int wlc_apps_pqp_pgi_cb(pqp_cb_t* cb);
static int wlc_apps_pqp_pgo_cb(pqp_cb_t* cb);
static bool wlc_apps_pqp_ps_send(wlc_info_t *wlc, struct scb *scb, uint prec_bmp, bool apsd,
    taf_scheduler_public_t *taf);
static uint32 wlc_apps_scb_pqp_ps_flush_prec(wlc_info_t *wlc, struct scb *scb,
    struct pktq *pq, int prec);
static void wlc_apps_scb_pqp_map_pkts(wlc_info_t *wlc, struct scb *scb,
    map_pkts_cb_fn cb, void *ctx);
static void wlc_apps_scb_pqp_pktq_filter(wlc_info_t *wlc, struct scb *scb, void *ctx);

/* Asynchronous PQP PGI flags for PS transition */
#define PQP_PGI_PS_OFF        0x0001
#define PQP_PGI_PS_TWT_OFF    0x0002
#define PQP_PGI_PS_BCMC_OFF    0x0004
#define PQP_PGI_PS_TAF_REL    0x0008
#define PQP_PGI_PS_SEND        0x0010

#define PQP_PGI_PS_TRANS_OFF \
    (PQP_PGI_PS_OFF | PQP_PGI_PS_TWT_OFF | PQP_PGI_PS_BCMC_OFF | PQP_PGI_PS_TAF_REL)

#define PQP_PGI_PS_TRANS(psinfo)        (psinfo)->pqp_ps_trans
#define PQP_PGI_PS_PRECMAP(psinfo)        (psinfo)->pqp_ps_precbmp
#define PQP_PGI_PS_EXTRAFLAG(psinfo)        (psinfo)->pqp_ps_extraflag

#define PQP_PGI_PS_SEND_ISSET(psinfo)        ((psinfo)->pqp_ps_trans & PQP_PGI_PS_SEND)
#define PQP_PGI_PS_TRANS_OFF_ISSET(psinfo)    ((psinfo)->pqp_ps_trans & PQP_PGI_PS_TRANS_OFF)

#define PQP_PGI_PS_TRANS_PEND_SET(psinfo, trans, precbmp) \
({ \
    (psinfo)->pqp_ps_trans |= (trans); \
    (psinfo)->pqp_ps_precbmp |= (precbmp); \
    (psinfo)->pqp_ps_extraflag = 0; \
})
#define PQP_PGI_PS_SEND_PEND_SET(psinfo, precbmp, flag) \
({ \
    ASSERT((psinfo)->pqp_ps_trans == 0); \
    (psinfo)->pqp_ps_trans = (PQP_PGI_PS_SEND); \
    (psinfo)->pqp_ps_precbmp = (precbmp); \
    (psinfo)->pqp_ps_extraflag = (flag); \
})
#define PQP_PGI_PS_TRANS_PEND_CLR(psinfo) \
({ \
    (psinfo)->pqp_ps_trans = 0; \
    (psinfo)->pqp_ps_precbmp = 0; \
    (psinfo)->pqp_ps_extraflag = 0; \
})

#define APPS_PKTQ_MLEN(pq, prec_bmp)        pktq_pqp_mlen(pq, prec_bmp)
#define APPS_PKTQ_PREC_N_PKTS(pq, prec)        pktq_pqp_pkt_cnt(pq, prec)
#define APPS_SPKTQ_N_PKTS(spktq)        pqp_pkt_cnt(spktq.q)
#define APPS_PS_SEND(wlc, scb, prec_bmp, apsd, taf) \
    wlc_apps_pqp_ps_send(wlc, scb, prec_bmp, apsd, taf)
#else /* !HNDPQP */
#define APPS_PKTQ_MLEN(pq, prec_bmp)        pktq_mlen(pq, prec_bmp)
#define APPS_PKTQ_PREC_N_PKTS(pq, prec)        pktqprec_n_pkts(pq, prec)
#define APPS_SPKTQ_N_PKTS(spktq)        spktq_n_pkts(spktq)
#define APPS_PS_SEND(wlc, scb, prec_bmp, apsd, taf) \
    wlc_apps_ps_send(wlc, scb, prec_bmp, apsd, taf, 1)
#endif /* !HNDPQP */

/* Flags for psp_flags */
#define PS_PSP_ONRESP            0x01    /* a pspoll req is under handling (SWWLAN-42801) */

/* PS transition status flags of apps_scb_psinfo */
#define SCB_PS_TRANS_OFF_PEND        0x01    /* Pend PS off until txfifo draining is done */
#define SCB_PS_TRANS_OFF_BLOCKED    0x02    /* Block attempts to switch PS off */
#define SCB_PS_TRANS_OFF_IN_PROG    0x04    /* PS off transition is already in progress */

/* PS transition status flags of apps_bss_psinfo */
#define BSS_PS_ON_BY_TWT        0x01    /* Force PS of bcmc scb by TWT */

#define PSQ_PKTQ_LEN_DEFAULT        512    /* Max 512 packets */

/* flags of apps_scb_psinfo */
#define SCB_PS_FIRST_SUPPR_HANDLED    0x01
#define SCB_PS_FLUSH_IN_PROGR        0x02

/* Threshold of triggers before resetting usp */
#define APSD_TRIG_THR            5

#define WLC_APPS_RELEASE_ALL        0xFFFFFFF
#define WLC_APPS_RELEASE_EXP        4    /* Release from CQ after 1 << 4 = 16 packets */

struct apps_scb_psinfo {
    int        suppressed_pkts;    /* Count of suppressed pkts in the
                         * scb's "psq". Used as a check to
                         * mark psq as requiring
                         * normalization (or not).
                         */
    struct pktq    psq;        /** PS defer (multi priority) packet queue */
#ifdef WL_USE_SUPR_PSQ
    struct spktq supr_psq[WLC_PREC_COUNT]; /* Use to store PS Supressed pkts */
#endif
#ifdef WLTAF
    uint8          taf_prio_active;
#endif
    bool        psp_pending;    /* whether uncompleted PS-POLL response is pending */
    uint8        psp_flags;    /* various ps mode bit flags defined below */
    uint8        flags;        /* a.o. have we handled first supr'd frames ? */
    bool        apsd_usp;    /* APSD Unscheduled Service Period in progress */
    int        apsd_cnt;    /* Frames left in this service period */
    int        apsd_trig_cnt;    /* apsd trigger received, but not processed */
    mbool        tx_block;    /* tx has been blocked */
    bool        ext_qos_null;    /* Send extra QoS NULL frame to indicate EOSP
                     * if last frame sent in SP was MMPDU
                     */
    uint8        ps_trans_status; /* PS transition status flag */
/* PROP_TXSTATUS starts */
    bool        apsd_hpkt_timer_on;
    bool        apsd_tx_pending;
    struct wl_timer    *apsd_hpkt_timer;
    struct scb    *scb;
    wlc_info_t    *wlc;
    int        apsd_v2r_in_transit;
/* PROP_TXSTATUS ends */
    uint16        tbtt;        /**< count of tbtt intervals since last ageing event */
    uint16        listen;        /**< minimum # bcn's to buffer PS traffic */
    uint32        ps_discard;    /* cnt of PS pkts which were dropped */
    uint32        supr_drop;    /* cnt of Supr pkts dropped */
    uint32        ps_queued;    /* cnt of PS pkts which were queued */
    uint32        last_in_ps_tsf;
    uint32        last_in_pvb_tsf;
    bool        in_pvb;        /* This STA already set in partial virtual bitmap */
    bool        change_scb_state_to_auth; /* After disassoc, scb goes into reset state
                           * and switch back to auth state, in case scb
                           * is in power save and AP holding disassoc
                           * frame for this, let the disassoc frame
                           * go out with NULL data packet from STA and
                           * wlc_apps module to change the state to auth
                           * state.
                           */
    uint8        ps_requester;    /* source of ps requests */
    bool        twt_active;    /**< TWT is active on SCB (twt_enter was called) */
    bool        twt_wait4drain_enter;
    bool        twt_wait4drain_exit;
    bool        twt_wait4drain_norm;    /* Drain txfifo for supr_q normalization */
    bool        pspoll_pkt_waiting;
#ifdef HNDPQP
    uint16        pqp_ps_trans; /* Asynchronous PQP PGI flags for PS transition */
    uint16        pqp_ps_precbmp; /* Asynchronous PQP PGI prec bitmap */
    uint32        pqp_ps_extraflag; /* additional flags on SDU */
#endif /* HNDPQP */
};

#define SCB_PSPOLL_PKT_WAITING(psinfo)        (psinfo)->pspoll_pkt_waiting

typedef struct apps_scb_psinfo apps_scb_psinfo_t;

typedef struct apps_bss_info
{
    uint8        pvb[251];        /* full partial virtual bitmap */
    uint16        aid_lo;            /* lowest aid with traffic pending */
    uint16        aid_hi;            /* highest aid with traffic pending */

    uint32        ps_nodes;        /* num of STAs in PS-mode */
    uint8        ps_trans_status;    /* PS transition status flag */
    uint32        bcmc_pkts_seen;        /* Packets thru BC/MC SCB queue WLCNT */
    uint32        bcmc_discards;        /* Packets discarded due to full queue WLCNT */
} apps_bss_info_t;

struct apps_wlc_psinfo
{
    wlc_info_t    *wlc;            /* backpointer to WLC */
    int        cfgh;            /* bsscfg cubby handle */
    osl_t        *osh;            /* pointer to os handle */
    int        scb_handle;        /* scb cubby handle to retrieve data from scb */
    uint32        ps_nodes_all;        /* Count of nodes in PS across all BBSes */
    uint        ps_deferred;        /* cnt of all PS pkts buffered on unicast scbs */
    uint32        ps_discard;        /* cnt of all PS pkts which were dropped */
    uint32        supr_drop;        /* cnt of Supr pkts dropped */
    uint32        ps_aged;        /* cnt of all aged PS pkts */
};

/* AC bitmap to precedence bitmap mapping (constructed in wlc_attach) */
static uint wlc_acbitmap2precbitmap[16] = { 0 };

/* Map AC bitmap to precedence bitmap */
#define WLC_ACBITMAP_TO_PRECBITMAP(ab)    wlc_acbitmap2precbitmap[(ab) & 0xf]

/* AC bitmap to tid bitmap mapping (constructed in wlc_attach) */
static uint8 wlc_acbitmap2tidbitmap[16] = { 0 };

/* Map AC bitmap to tid bitmap */
#define WLC_ACBITMAP_TO_TIDBITMAP(ab)    wlc_acbitmap2tidbitmap[(ab) & 0xf]

#define SCB_PSINFO_LOC(psinfo, scb) ((apps_scb_psinfo_t **)SCB_CUBBY(scb, (psinfo)->scb_handle))
#define SCB_PSINFO(psinfo, scb) (*SCB_PSINFO_LOC(psinfo, scb))

/* apps info accessor */
#define APPS_BSSCFG_CUBBY_LOC(psinfo, cfg) ((apps_bss_info_t **)BSSCFG_CUBBY((cfg), (psinfo)->cfgh))
#define APPS_BSSCFG_CUBBY(psinfo, cfg) (*(APPS_BSSCFG_CUBBY_LOC(psinfo, cfg)))

#define BSS_PS_NODES(psinfo, bsscfg) ((APPS_BSSCFG_CUBBY(psinfo, bsscfg))->ps_nodes)

static int wlc_apps_scb_psinfo_init(void *context, struct scb *scb);
static void wlc_apps_scb_psinfo_deinit(void *context, struct scb *scb);
static uint wlc_apps_scb_psinfo_secsz(void *context, struct scb *scb);
static uint wlc_apps_txpktcnt(void *context);
static void wlc_apps_ps_flush_tid(void *context, struct scb *scb, uint8 tid);
#ifdef BCMPCIEDEV
static INLINE void _wlc_apps_scb_psq_resp(wlc_info_t *wlc, struct scb *scb,
    struct apps_scb_psinfo *scb_psinfo);
static void wlc_apps_scb_apsd_dec_v2r_in_transit(wlc_info_t *wlc, struct scb *scb,
    void *pkt, int prec);
#endif /* BCMPCIEDEV */
static void wlc_apps_apsd_hpkt_tmout(void *arg);
static void wlc_apps_apsd_complete(wlc_info_t *wlc, void *pkt, uint txs);
static void wlc_apps_psp_resp_complete(wlc_info_t *wlc, void *pkt, uint txs);
#ifdef WL_USE_SUPR_PSQ
/* Version operation on ps suppress queue */
static void wlc_apps_dequeue_scb_supr_psq(wlc_info_t *wlc, struct scb *scb);
static bool wlc_apps_enqueue_scb_supr_psq(struct apps_scb_psinfo *scb_psinfo, void* pkt, int prec);
#endif /* WL_USE_SUPR_PSQ */

#define  DOT11_MNG_TIM_NON_PVB_LEN  (DOT11_MNG_TIM_FIXED_LEN + TLV_HDR_LEN)

#ifdef BCMPCIEDEV
static int wlc_apps_sqs_vpkt_count(wlc_info_t *wlc, struct scb *scb);
static int wlc_apps_apsd_ndelv_vpkt_count(wlc_info_t *wlc, struct scb *scb);
static int wlc_apps_apsd_delv_vpkt_count(wlc_info_t *wlc, struct scb *scb);
/* Packet Queue watermarks used for fetching packets from host */
#define WLC_APSD_DELV_CNT_HIGH_WATERMARK    4
#define WLC_APSD_DELV_CNT_LOW_WATERMARK        2
#endif /* BCMPCIEDEV */

static txmod_fns_t BCMATTACHDATA(apps_txmod_fns) = {
    wlc_apps_transmit_packet,
    wlc_apps_txpktcnt,
    wlc_apps_ps_flush_tid,
    NULL,
    NULL
};

int
BCMATTACHFN(wlc_apps_attach)(wlc_info_t *wlc)
{
    scb_cubby_params_t cubby_params;
    apps_wlc_psinfo_t *wlc_psinfo;
    bsscfg_cubby_params_t bsscfg_cubby_params;
    int i;

    if (!(wlc_psinfo = MALLOCZ(wlc->osh, sizeof(apps_wlc_psinfo_t)))) {
        WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
                  wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
        wlc->psinfo = NULL;
        return -1;
    }

    /* Save backpointer to WLC */
    wlc_psinfo->wlc = wlc;

    /* reserve cubby space in the bsscfg container for per-bsscfg private data */
    bzero(&bsscfg_cubby_params, sizeof(bsscfg_cubby_params));
    bsscfg_cubby_params.context = wlc_psinfo;
    bsscfg_cubby_params.fn_deinit = wlc_apps_bss_deinit;
    bsscfg_cubby_params.fn_init = wlc_apps_bss_init;
    bsscfg_cubby_params.fn_dump = wlc_apps_bss_dump;

    if ((wlc_psinfo->cfgh = wlc_bsscfg_cubby_reserve_ext(wlc, sizeof(apps_bss_info_t *),
                                                          &bsscfg_cubby_params)) < 0) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_cubby_reserve() failed\n",
                  wlc->pub->unit, __FUNCTION__));
        return -1;
    }

    /* bsscfg up/down callback */
    if (wlc_bsscfg_updown_register(wlc, wlc_apps_bss_updn, wlc_psinfo) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_bsscfg_updown_register() failed\n",
            wlc->pub->unit, __FUNCTION__));
        return -1;
    }

    /* calculate the total ps pkts required */
    wlc->pub->psq_pkts_total = (wlc->pub->tunables->maxscb * PSQ_PKTQ_LEN_DEFAULT);

    /* reserve cubby in the scb container for per-scb private data */
    bzero(&cubby_params, sizeof(cubby_params));

    cubby_params.context = wlc;
    cubby_params.fn_init = wlc_apps_scb_psinfo_init;
    cubby_params.fn_deinit = wlc_apps_scb_psinfo_deinit;
    cubby_params.fn_secsz = wlc_apps_scb_psinfo_secsz;
    cubby_params.fn_dump = wlc_apps_scb_psinfo_dump;

    wlc_psinfo->scb_handle = wlc_scb_cubby_reserve_ext(wlc,
                                                       sizeof(apps_scb_psinfo_t *),
                                                       &cubby_params);

    if (wlc_psinfo->scb_handle < 0) {
        WL_ERROR(("wl%d: %s: wlc_scb_cubby_reserve() failed\n",
                  wlc->pub->unit, __FUNCTION__));
        wlc_apps_detach(wlc);
        return -2;
    }

    /* construct mapping from AC bitmap to precedence bitmap */
    for (i = 0; i < 16; i++) {
        wlc_acbitmap2precbitmap[i] = 0;
        if (AC_BITMAP_TST(i, AC_BE))
            wlc_acbitmap2precbitmap[i] |= WLC_PREC_BMP_AC_BE;
        if (AC_BITMAP_TST(i, AC_BK))
            wlc_acbitmap2precbitmap[i] |= WLC_PREC_BMP_AC_BK;
        if (AC_BITMAP_TST(i, AC_VI))
            wlc_acbitmap2precbitmap[i] |= WLC_PREC_BMP_AC_VI;
        if (AC_BITMAP_TST(i, AC_VO))
            wlc_acbitmap2precbitmap[i] |= WLC_PREC_BMP_AC_VO;
    }

    /* construct mapping from AC bitmap to tid bitmap */
    for (i = 0; i < 16; i++) {
        wlc_acbitmap2tidbitmap[i] = 0;
        if (AC_BITMAP_TST(i, AC_BE))
            wlc_acbitmap2tidbitmap[i] |= TID_BMP_AC_BE;
        if (AC_BITMAP_TST(i, AC_BK))
            wlc_acbitmap2tidbitmap[i] |= TID_BMP_AC_BK;
        if (AC_BITMAP_TST(i, AC_VI))
            wlc_acbitmap2tidbitmap[i] |= TID_BMP_AC_VI;
        if (AC_BITMAP_TST(i, AC_VO))
            wlc_acbitmap2tidbitmap[i] |= TID_BMP_AC_VO;
    }

    /* register module */
    if (wlc_module_register(wlc->pub, NULL, "apps", wlc, NULL, wlc_apps_bss_wd_ps_check, NULL,
        NULL)) {
        WL_ERROR(("wl%d: %s wlc_module_register() failed\n",
                  wlc->pub->unit, __FUNCTION__));
        return -4;
    }

    /* register packet class callback */
    if (wlc_pcb_fn_set(wlc->pcb, 1, WLF2_PCB2_APSD, wlc_apps_apsd_complete) != BCME_OK) {
        WL_ERROR(("wl%d: %s wlc_pcb_fn_set() failed\n", wlc->pub->unit, __FUNCTION__));
        return -5;
    }
    if (wlc_pcb_fn_set(wlc->pcb, 1, WLF2_PCB2_PSP_RSP, wlc_apps_psp_resp_complete) != BCME_OK) {
        WL_ERROR(("wl%d: %s wlc_pcb_fn_set() failed\n", wlc->pub->unit, __FUNCTION__));
        return -6;
    }

    /* register IE mgmt callbacks */
    /* bcn */
    if (wlc_iem_add_build_fn(wlc->iemi, FC_BEACON, DOT11_MNG_TIM_ID,
            wlc_apps_calc_tim_ie_len, wlc_apps_write_tim_ie, wlc) != BCME_OK) {
        WL_ERROR(("wl%d: %s: wlc_iem_add_build_fn failed, tim in bcn\n",
                  wlc->pub->unit, __FUNCTION__));
        return -7;
    }

#if defined(PSPRETEND) && !defined(PSPRETEND_DISABLED)
    if ((wlc->pps_info = wlc_pspretend_attach(wlc)) == NULL) {
        WL_ERROR(("wl%d: %s: wlc_pspretend_attach failed\n",
                  wlc->pub->unit, __FUNCTION__));
        return -8;
    }
    wlc->pub->_pspretend = TRUE;
#endif /* PSPRETEND  && !PSPRETEND_DISABLED */
    wlc_txmod_fn_register(wlc->txmodi, TXMOD_APPS, wlc, apps_txmod_fns);

    /* Add client callback to the scb state notification list */
    if (wlc_scb_state_upd_register(wlc, wlc_apps_scb_state_upd_cb, wlc) != BCME_OK) {
        WL_ERROR(("wl%d: %s: unable to register callback %p\n",
                  wlc->pub->unit, __FUNCTION__,
            OSL_OBFUSCATE_BUF(wlc_apps_scb_state_upd_cb)));
        return -9;
    }

#ifdef WL_PS_STATS
    /* Register dump ps stats */
    wlc_dump_add_fns(wlc->pub, "ps_stats", wlc_apps_stats_dump, wlc_apps_stats_dump_clr, wlc);
#endif /* WL_PS_STATS */

#if defined(BCMDBG)
    wlc_dump_register(wlc->pub, "apps", (dump_fn_t)wlc_apps_dump, (void*)wlc);
#endif

    wlc_psinfo->osh = wlc->osh;
    wlc->psinfo = wlc_psinfo;

#if defined(HNDPQP)
    /* Configure PQP callbacks */
    pqp_config(wlc->osh, hwa_dev, wlc_apps_pqp_pgo_cb, wlc_apps_pqp_pgi_cb);
#endif

    return 0;
}

void
BCMATTACHFN(wlc_apps_detach)(wlc_info_t *wlc)
{
    apps_wlc_psinfo_t *wlc_psinfo;

    ASSERT(wlc);

#ifdef PSPRETEND
    wlc_pspretend_detach(wlc->pps_info);
#endif /* PSPRETEND */

    wlc_psinfo = wlc->psinfo;

    if (!wlc_psinfo)
        return;

    /* All PS packets shall have been flushed */
    ASSERT(wlc_psinfo->ps_deferred == 0);

    wlc_scb_state_upd_unregister(wlc, wlc_apps_scb_state_upd_cb, wlc);
    (void)wlc_bsscfg_updown_unregister(wlc, wlc_apps_bss_updn, wlc_psinfo);

    wlc_module_unregister(wlc->pub, "apps", wlc);

    MFREE(wlc->osh, wlc_psinfo, sizeof(apps_wlc_psinfo_t));
    wlc->psinfo = NULL;
}

#if defined(BCMDBG)
/* Verify that all ps packets have been flushed.
 * This is called after bsscfg down notify callbacks were processed.
 * So wlc_apps_bss_updn has been called for each bsscfg and all packets should have been flushed.
 */
void
wlc_apps_wlc_down(wlc_info_t *wlc)
{
    apps_wlc_psinfo_t *wlc_psinfo;

    ASSERT(wlc);

    wlc_psinfo = wlc->psinfo;

    if (!wlc_psinfo)
        return;

    /* All PS packets have been flushed */
    ASSERT(wlc_psinfo->ps_deferred == 0);
}
#endif /* BCMDBG */

/* bsscfg cubby */
static int
wlc_apps_bss_init(void *ctx, wlc_bsscfg_t *cfg)
{
    apps_wlc_psinfo_t *wlc_psinfo = (apps_wlc_psinfo_t *)ctx;
    wlc_info_t *wlc = cfg->wlc;
    apps_bss_info_t **papps_bss = APPS_BSSCFG_CUBBY_LOC(wlc_psinfo, cfg);
    apps_bss_info_t *apps_bss = NULL;
    UNUSED_PARAMETER(wlc);

    /* Allocate only for AP || TDLS bsscfg */
    if (BSSCFG_HAS_PSINFO(cfg)) {
        if (!(apps_bss = (apps_bss_info_t *)MALLOCZ(wlc_psinfo->osh,
            sizeof(apps_bss_info_t)))) {
            WL_ERROR(("wl%d: %s out of memory, malloced %d bytes\n",
                wlc->pub->unit, __FUNCTION__, MALLOCED(wlc_psinfo->osh)));
            return BCME_NOMEM;
        }
    }
    *papps_bss = apps_bss;

    return BCME_OK;
}

static void
wlc_apps_bss_deinit(void *ctx, wlc_bsscfg_t *cfg)
{
    apps_wlc_psinfo_t *wlc_psinfo = (apps_wlc_psinfo_t *)ctx;
    apps_bss_info_t **papps_bss = APPS_BSSCFG_CUBBY_LOC(wlc_psinfo, cfg);
    apps_bss_info_t *apps_bss = *papps_bss;

    if (apps_bss != NULL) {
        MFREE(wlc_psinfo->osh, apps_bss, sizeof(apps_bss_info_t));
        *papps_bss = NULL;
    }

    return;
}

/* Return the count of all the packets being held by APPS TxModule */
static uint
wlc_apps_txpktcnt(void *context)
{
    wlc_info_t *wlc = (wlc_info_t *)context;
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;

    return (wlc_psinfo->ps_deferred);
}

/* Return the count of all the packets being held in scb psq */
uint
wlc_apps_scb_txpktcnt(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo)
        return 0;

    return (pktq_n_pkts_tot(&scb_psinfo->psq));
}

static int
wlc_apps_scb_psinfo_init(void *context, struct scb *scb)
{
    wlc_info_t *wlc = (wlc_info_t *)context;
    apps_scb_psinfo_t **cubby_info;
    apps_scb_psinfo_t *scb_psinfo;
    wlc_bsscfg_t *bsscfg;

    cubby_info = SCB_PSINFO_LOC(wlc->psinfo, scb);
    ASSERT(*cubby_info == NULL);

    bsscfg = SCB_BSSCFG(scb);
    ASSERT(bsscfg != NULL);

    if (BSSCFG_HAS_PSINFO(bsscfg)) {
        *cubby_info = wlc_scb_sec_cubby_alloc(wlc, scb, sizeof(apps_scb_psinfo_t));
        if (*cubby_info == NULL)
            return BCME_NOMEM;

        scb_psinfo = *cubby_info;

        if (!(scb_psinfo->apsd_hpkt_timer = wl_init_timer(wlc->wl,
            wlc_apps_apsd_hpkt_tmout, *cubby_info, "appsapsdhkpt"))) {
            WL_ERROR(("wl: apsd_hpkt_timer failed\n"));
            return 1;
        }
        scb_psinfo->wlc = wlc;
        scb_psinfo->scb = scb;
        /* PS state init */
#ifdef WL_USE_SUPR_PSQ
        /* Init the max psq size, but limit the psq length at enqueue */
        pktq_init(&scb_psinfo->psq, WLC_PREC_COUNT, PKTQ_LEN_MAX);
        {
            int prec;
            /* Init supress ps spktq */
            for (prec = 0; prec < WLC_PREC_COUNT; prec++)
                spktq_init(&scb_psinfo->supr_psq[prec], PKTQ_LEN_MAX);
        }
#else
        pktq_init(&scb_psinfo->psq, WLC_PREC_COUNT, PKTQ_LEN_MAX);
#endif
    }
    return 0;
}

static void
wlc_apps_scb_psinfo_deinit(void *context, struct scb *remove)
{
    wlc_info_t *wlc = (wlc_info_t *)context;
    apps_scb_psinfo_t **cubby_info = SCB_PSINFO_LOC(wlc->psinfo, remove);
    apps_scb_psinfo_t *scb_psinfo = *cubby_info;
    struct pktq *psq;
#ifdef WL_USE_SUPR_PSQ
    int prec;
    struct spktq* suppr_psq;
#endif /* WL_USE_SUPR_PSQ */

    if (scb_psinfo) {
#ifdef WLTAF
        int prio;
        for (prio = 0; scb_psinfo->taf_prio_active && prio < NUMPRIO; prio++) {
            if (scb_psinfo->taf_prio_active & (1 << prio)) {
                /* tell TAF we are not using this apps context anymore */
                wlc_taf_link_state(wlc->taf_handle, remove, prio, TAF_PSQ,
                    TAF_LINKSTATE_REMOVE);
                scb_psinfo->taf_prio_active &= ~(1 << prio);
            }
        }
        wlc_taf_scb_state_update(wlc->taf_handle, remove, TAF_PSQ, NULL,
            TAF_SCBSTATE_POWER_SAVE);
#endif
        psq = &scb_psinfo->psq;

        wl_del_timer(wlc->wl, scb_psinfo->apsd_hpkt_timer);
        scb_psinfo->apsd_hpkt_timer_on = FALSE;
        wl_free_timer(wlc->wl, scb_psinfo->apsd_hpkt_timer);

        if (BSSCFG_AP(SCB_BSSCFG(remove)) || (BSS_TDLS_ENAB(wlc, SCB_BSSCFG(remove)) &&
                SCB_PS(remove))) {
#ifdef WL_USE_SUPR_PSQ
            if (SCB_PSINFO(wlc->psinfo, remove)->suppressed_pkts > 0) {
                wlc_apps_dequeue_scb_supr_psq(wlc, remove);
            }
#endif /* WL_USE_SUPR_PSQ */
            if (SCB_PS(remove) && !SCB_ISMULTI(remove)) {
    /* dump_flag_qqdx */
#ifdef dump_stack_ps_qqdx_print
    printk(KERN_ALERT"----------[fyl] wlc_apps_scb_psinfo_deinit (%u)wlc_apps_scb_ps_off----------",OSL_SYSUPTIME());
#endif /*dump_stack_ps_qqdx_print*/
                wlc_apps_scb_ps_off(wlc, remove, TRUE);
            } else if (!pktq_empty(psq))
                wlc_apps_ps_flush(wlc, remove);
        }

#ifdef WL_USE_SUPR_PSQ
        /* Deinit supress ps spktq */
        for (prec = 0; prec < WLC_PREC_COUNT; prec++) {
            suppr_psq = &scb_psinfo->supr_psq[prec];
#ifdef HNDPQP
            ASSERT(!pqp_owns(&suppr_psq->q));
#endif /* HNDPQP */
            spktq_deinit(suppr_psq);
        }
#endif /* WL_USE_SUPR_PSQ */

        /* Now clear the pktq_log allocated buffer */
#ifdef PKTQ_LOG
        wlc_pktq_stats_free(wlc, psq);
#endif
#ifdef HNDPQP
        ASSERT(!pktq_mpqp_owns(psq, WLC_PREC_BMP_ALL));
#endif
        pktq_deinit(psq);

        wlc_scb_sec_cubby_free(wlc, remove, scb_psinfo);
        *cubby_info = NULL;
    }
}

static uint
wlc_apps_scb_psinfo_secsz(void *context, struct scb *scb)
{
    wlc_bsscfg_t *cfg;
    BCM_REFERENCE(context);

    cfg = SCB_BSSCFG(scb);
    ASSERT(cfg != NULL);

    if (BSSCFG_HAS_PSINFO(cfg)) {
        return sizeof(apps_scb_psinfo_t);
    }

    return 0;
}

void
wlc_apps_dbg_dump(wlc_info_t *wlc)
{
    struct scb *scb;
    struct scb_iter scbiter;
    struct apps_scb_psinfo *scb_psinfo;

    WL_PRINT(("WLC discards:%d, ps_deferred:%d\n",
        wlc->psinfo->ps_discard, wlc->psinfo->ps_deferred));

    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
        if (scb_psinfo == NULL)
            continue;

        WL_PRINT(("scb at %p for [%02x:%02x:%02x:%02x:%02x:%02x]\n",
            OSL_OBFUSCATE_BUF(scb),
            scb->ea.octet[0], scb->ea.octet[1], scb->ea.octet[2],
            scb->ea.octet[3], scb->ea.octet[4], scb->ea.octet[5]));
        WL_PRINT(("  (psq_items,state)=(%d,%s), psq_bucket: %d items\n",
            scb_psinfo->psq.n_pkts_tot,
            ((scb->PS == FALSE) ? " OPEN" : "CLOSE"),
            scb_psinfo->psq.n_pkts_tot));
    }

    return;
}

void
wlc_apps_apsd_usp_end(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

#ifdef WLTDLS
    if (BSS_TDLS_ENAB(wlc, scb->bsscfg)) {
        if (wlc_tdls_in_pti_interval(wlc->tdls, scb))
            return;
        wlc_tdls_apsd_usp_end(wlc->tdls, scb);
    }
#endif /* WLTDLS */

    scb_psinfo->apsd_usp = FALSE;

#ifdef WLTDLS
    if (BSS_TDLS_ENAB(wlc, scb->bsscfg) &&
        (wlc_apps_apsd_delv_count(wlc, scb) > 0)) {
        /* send PTI again */
        wlc_tdls_send_pti(wlc->tdls, scb);
    }
#endif /* WLTDLS */

}

#if defined(BCMDBG)
/* Limited dump routine for APPS SCB info */
static void
wlc_apps_scb_psinfo_dump(void *context, struct scb *scb, struct bcmstrbuf *b)
{
    wlc_info_t *wlc = (wlc_info_t *)context;
    struct apps_scb_psinfo *scb_psinfo;
    wlc_bsscfg_t *bsscfg;
    struct pktq *pktq;
    uint32 cur_time = 0;

    if (scb == NULL)
        return;

    bsscfg = scb->bsscfg;
    if (bsscfg == NULL)
        return;

    if (!BSSCFG_AP(bsscfg))
        return;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL)
        return;

    bcm_bprintf(b, "\tPS %s TWTPS %s listen %d",
        SCB_PS(scb) ? "on" : "off",
        SCB_TWTPS(scb) ? "on" : "off",
        scb_psinfo->listen);

    if (SCB_PS(scb) || SCB_TWTPS(scb)) {
#ifdef PSPRETEND
        if (SCB_PS_PRETEND(scb)) {
            bcm_bprintf(b, " PPS");
        }
#endif /* PSPRETEND */

        if (wlc->clk) {
            uint32 ps_duration;
            cur_time = wlc_lifetime_now(wlc);
            ps_duration = ((cur_time - scb_psinfo->last_in_ps_tsf) +
                500) / 1000;
            bcm_bprintf(b, " inPS %dms", ps_duration);
        }

        bcm_bprintf(b, " in_pvb %d", scb_psinfo->in_pvb);
        if (wlc->clk && scb_psinfo->in_pvb) {
            uint32 ps_in_pvb_duration;

            ps_in_pvb_duration = ((cur_time - scb_psinfo->last_in_pvb_tsf) +
                500) / 1000;
            bcm_bprintf(b, "(%dms)", ps_in_pvb_duration);
        }

        bcm_bprintf(b, "\n\tps_trans_status 0x%x ps_requester 0x%x",
            scb_psinfo->ps_trans_status, scb_psinfo->ps_requester);

#if defined(WL_PS_SCB_TXFIFO_BLK)
        bcm_bprintf(b, " ps_txfifo_blk %d", SCB_PS_TXFIFO_BLK(scb));
#endif /* WL_PS_SCB_TXFIFO_BLK */

        bcm_bprintf(b, "\n");

        bcm_bprintf(b, "\ttx_block %d ext_qos_null %d psp_flags %d psp_pending %d"
            " pspoll_pkt_waiting %d \n",
            scb_psinfo->tx_block, scb_psinfo->ext_qos_null,
            scb_psinfo->psp_flags, scb_psinfo->psp_pending,
            scb_psinfo->pspoll_pkt_waiting);
    } else {
        bcm_bprintf(b, "\n");
    }

    /* Print queue information */
    pktq = &scb_psinfo->psq;
    if (pktq != NULL) {
        uint16 psq_npkts = pktq_n_pkts_tot(pktq);
        bcm_bprintf(b, "\tPSQ len %d max %d avail %d ps_queued %d"
            " discards %d suppressed %d\n",
            psq_npkts, pktq_max(pktq), pktq_avail(pktq),
            scb_psinfo->ps_queued, scb_psinfo->ps_discard,
            scb_psinfo->suppressed_pkts);

        if (psq_npkts || scb_psinfo->suppressed_pkts) {
            int prec;
            for (prec = WLC_PREC_COUNT - 1; prec >= 0; prec--) {
                bcm_bprintf(b, "\tprec[%d]: psq:%d", prec,
                    APPS_PKTQ_PREC_N_PKTS(&scb_psinfo->psq, prec));
#ifdef WL_USE_SUPR_PSQ
                bcm_bprintf(b, " supr_psq:%d",
                    APPS_SPKTQ_N_PKTS(&scb_psinfo->supr_psq[prec]));
#endif
                bcm_bprintf(b, " \n");
            }
        }
    }
}

static void
wlc_apps_scb_psinfo_complete_dump(void *context, struct scb *scb, struct bcmstrbuf *b)
{
    wlc_info_t *wlc = (wlc_info_t *)context;
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL)
        return;

    wlc_apps_scb_psinfo_dump(wlc, scb, b);

    if (SCB_PS(scb) || SCB_TWTPS(scb)) {
        bcm_bprintf(b, "\tflags 0x%x tbtt %d supr_drop %d cssta %d\n",
            scb_psinfo->flags, scb_psinfo->tbtt, scb_psinfo->supr_drop,
            scb_psinfo->change_scb_state_to_auth);

#ifdef WLTAF
        bcm_bprintf(b, "\tTAF prio_active 0x%x\n", scb_psinfo->taf_prio_active);
#endif /* WLTAF */

        bcm_bprintf(b, "\tAPSD: usp %d cnt %d trig_cnt %d hpkt_timer %d "
            "tx_pending %d v2r_intransit %d\n",
            scb_psinfo->apsd_usp,
            scb_psinfo->apsd_cnt,
            scb_psinfo->apsd_trig_cnt,
            scb_psinfo->apsd_hpkt_timer_on,
            scb_psinfo->apsd_tx_pending,
            scb_psinfo->apsd_v2r_in_transit);

#ifdef WLTWT
        bcm_bprintf(b, "\tTWT: active %d dr_enter %d dr_exit %d dr_norm %d\n",
            scb_psinfo->twt_active,
            scb_psinfo->twt_wait4drain_enter,
            scb_psinfo->twt_wait4drain_exit,
            scb_psinfo->twt_wait4drain_norm);
#endif /* WLTWT */
#ifdef HNDPQP
        bcm_bprintf(b, "\tPQP: trans 0x%x precbmp 0x%x extra 0x%x\n",
            scb_psinfo->pqp_ps_trans,
            scb_psinfo->pqp_ps_precbmp,
            scb_psinfo->pqp_ps_extraflag);
#endif /* HNDPQP */
    }
}

static void
wlc_apps_bss_dump(void *ctx, wlc_bsscfg_t *cfg, struct bcmstrbuf *b)
{
    apps_wlc_psinfo_t *wlc_psinfo = (apps_wlc_psinfo_t *)ctx;
    apps_bss_info_t *bss_psinfo;
    uint8 offset;
    int16 length;

    ASSERT(cfg != NULL);

    bss_psinfo = APPS_BSSCFG_CUBBY(wlc_psinfo, cfg);
    if (bss_psinfo == NULL) {
        return;
    }

    wlc_apps_tim_pvb(cfg->wlc, cfg, &offset, &length);

    bcm_bprintf(b, "\toffset %u length %d", offset, length);
    bcm_bprhex(b, " pvb ", TRUE, &bss_psinfo->pvb[offset], length);

    bcm_bprintf(b, "\tps nodes %d ps_trans_status %d, bcmc %d(discard %d)\n",
        bss_psinfo->ps_nodes, bss_psinfo->ps_trans_status, bss_psinfo->bcmc_pkts_seen,
        bss_psinfo->bcmc_discards);
}

#define MAX_NODES_COMPLETE_DUMP 5
void
wlc_apps_dump(void *ctx, struct bcmstrbuf *b)
{
    int i;
    wlc_info_t *wlc = (wlc_info_t *)ctx;
    apps_wlc_psinfo_t *psinfo = wlc->psinfo;
    struct scb *scb;
    struct scb_iter scbiter;
    wlc_bsscfg_t *bsscfg;

    /* Print wlc psinfo */
    bcm_bprintf(b, "PS nodes: %d, deferred %d, discarded %d\n"
        "suppressed drop %d, timed out %d, block_datafifo 0x%x\n",
        psinfo->ps_nodes_all, psinfo->ps_deferred, psinfo->ps_discard,
        psinfo->supr_drop, psinfo->ps_aged, wlc->block_datafifo);

#if defined(WL_PS_SCB_TXFIFO_BLK)
    bcm_bprintf(b, "per-scb_blk %d cnt %d\n",
        wlc->ps_scb_txfifo_blk, wlc->ps_txfifo_blk_scb_cnt);
#endif /* WL_PS_SCB_TXFIFO_BLK */

    /* Look through all BSSCFGs/SCBs and display relevant PS info */
    i = 0;  /* i=BSSs */
    FOREACH_UP_AP(wlc, i, bsscfg) {
        bcm_bprintf(b, "\nBSS["MACF"]:\n", ETHER_TO_MACF(bsscfg->BSSID));
        wlc_apps_bss_dump(psinfo, bsscfg, b);
        FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
            if (SCB_PS(scb) || SCB_TWTPS(scb)) {
                bcm_bprintf(b, "\n\tSCB["MACF"]:\n",
                    ETHER_TO_MACF(scb->ea));
                if (psinfo->ps_nodes_all <= MAX_NODES_COMPLETE_DUMP) {
                    wlc_apps_scb_psinfo_complete_dump(wlc, scb, b);
                } else {
                    wlc_apps_scb_psinfo_dump(wlc, scb, b);
                }
            }
        }
    }
}
#endif

#ifdef BCMDBG
#define APPS_SCB_DUMP_LEN (1 * 1024)

void
wlc_apps_print_scb_info(wlc_info_t *wlc, struct scb *scb)
{
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    struct apps_scb_psinfo *scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    char    *buf = NULL;
    struct  bcmstrbuf bstr;
    uint16  c = 0;

    if (!scb_psinfo) {
        return;
    }

    buf = MALLOCZ(wlc->osh, APPS_SCB_DUMP_LEN);

    if (!buf) {
        return;
    }

    bcm_binit(&bstr, buf, APPS_SCB_DUMP_LEN);

    wlc_apps_scb_psinfo_dump(wlc, scb, &bstr);
    while (buf[c] && (c < APPS_SCB_DUMP_LEN)) {
        printf("%c", buf[c++]);
    }

    MFREE(wlc->osh, buf, APPS_SCB_DUMP_LEN);
}
#endif /* BCMDBG */

static INLINE void
wlc_apps_scb_ps_off_ps_pgi_done(wlc_info_t *wlc, struct scb *scb,
    struct apps_scb_psinfo *scb_psinfo)
{
#ifdef WLCFP
    if (CFP_ENAB(wlc->pub) == TRUE) {
        /* CFP path doesn't handle out of order packets.
         * There might be some OOO packets queued from legacy
         * patch when STA was in PS. So drain those packets
         * first before actually resume CFP.
         */
        if (SCB_AMPDU(scb)) {
            wlc_ampdu_txeval_alltid(wlc, scb, TRUE);
        }
    }
#endif /* WLCFP */
}

/* This routine deals with all PS transitions from ON->OFF */
void
wlc_apps_scb_ps_off(wlc_info_t *wlc, struct scb *scb, bool discard)
{
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    struct apps_scb_psinfo *scb_psinfo;
    apps_bss_info_t *bss_info;
    wlc_bsscfg_t *bsscfg;
    struct ether_addr ea;

    /* sanity */
    ASSERT(scb);

    bsscfg = SCB_BSSCFG(scb);
    ASSERT(bsscfg != NULL);

    bss_info = APPS_BSSCFG_CUBBY(wlc_psinfo, bsscfg);
    ASSERT(bss_info);

    /* process ON -> OFF PS transition */
    WL_PS_EX(scb, ("AID %d\n", SCB_AID(scb)));

    /* update PS state info */
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    if (scb_psinfo == NULL) {
        return;
    }

#ifdef dump_stack_qqdx_print
    int dump_rand_flag = OSL_RAND() % 100000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl] wlc_apps_scb_ps_off dump_stack start----------");
        dump_stack();
        printk(KERN_ALERT"----------[fyl] wlc_apps_scb_ps_off dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
#if defined(BCMDBG_PPS_qq) && defined(PSPRETEND)
    if (SCB_PS_PRETEND(scb)) {
        wlc_pspretend_scb_time_upd(wlc->pps_info, scb);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl] wlc_pspretend_scb_time_upd(wlc->pps_info, scb); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl] wlc_pspretend_scb_time_upd(wlc->pps_info, scb); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
    }
#endif /* PSPRETEND */

    /* if already in this process but came here due to pkt callback then
     * just return.
     */
    if ((scb_psinfo->ps_trans_status & SCB_PS_TRANS_OFF_IN_PROG))
        return;
    scb_psinfo->ps_trans_status |= SCB_PS_TRANS_OFF_IN_PROG;

#if defined(WL_PS_SCB_TXFIFO_BLK)
    if (scb->ps_txfifo_blk) {
        /* This should not happen. The statemachines have not completed yet but something
         * is pulling the SCB out of PS, During SCB cleanup, we can get here. Make sure
         * to update counters correctly.
         */
        ASSERT(wlc->ps_txfifo_blk_scb_cnt);
        scb->ps_txfifo_blk = FALSE;
        wlc->ps_txfifo_blk_scb_cnt--;
        /* remove PMQ entry for this STA */
        wlc_pmq_process_switch(wlc, scb, PMQ_REMOVE);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl] wlc_pmq_process_switch(wlc, scb, PMQ_REMOVE); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl] wlc_pmq_process_switch(wlc, scb, PMQ_REMOVE); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
    }
#endif /* WL_PS_SCB_TXFIFO_BLK */

    ASSERT(bss_info->ps_nodes);
    bss_info->ps_nodes--;

    ASSERT(wlc_psinfo->ps_nodes_all);
    wlc_psinfo->ps_nodes_all--;
    ASSERT(scb->PS);
    scb->PS = FALSE;
    wlc_apps_upd_pstime(scb);
    scb_psinfo->last_in_ps_tsf = 0;
    scb_psinfo->ps_trans_status &= ~SCB_PS_TRANS_OFF_PEND;

#ifdef PSPRETEND
    if (wlc->pps_info != NULL) {
        wlc_pspretend_scb_ps_off(wlc->pps_info, scb);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl] wlc_pspretend_scb_ps_off(wlc->pps_info, scb); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl] wlc_pspretend_scb_ps_off(wlc->pps_info, scb); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
    }
#endif /* PSPRETEND */

#if defined(BCMPCIEDEV)
    /* Reset all pending fetch and rollback flow fetch ptrs */
    wlc_scb_flow_ps_update(wlc->wl, SCB_FLOWID(scb), FALSE);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  wlc_scb_flow_ps_update(wlc->wl, SCB_FLOWID(scb), FALSE); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  wlc_scb_flow_ps_update(wlc->wl, SCB_FLOWID(scb), FALSE); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
#endif

    /* XXX wlc_wlfc_scb_ps_off did a reset of AMPDU seq with a BAR.
     * Done only for FD mode. Required .??
     */

    /* Reset Packet waiting status */
    SCB_PSPOLL_PKT_WAITING(scb_psinfo) = FALSE;

    /* Unconfigure the APPS from the txpath */
    wlc_txmod_unconfig(wlc->txmodi, scb, TXMOD_APPS);

    /* If this is last STA to leave PS mode,
     * trigger BCMC FIFO drain and
     * set BCMC traffic to go through regular fifo
     */
    if ((bss_info != NULL) && (bss_info->ps_nodes == 0) && BSSCFG_HAS_BCMC_SCB(bsscfg) &&
        !(bss_info->ps_trans_status & BSS_PS_ON_BY_TWT)) {

        wlc_apps_bcmc_ps_off_start(wlc, WLC_BCMCSCB_GET(wlc, bsscfg));
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  wlc_apps_bcmc_ps_off_start(wlc, WLC_BCMCSCB_GET(wlc, bsscfg)); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  wlc_apps_bcmc_ps_off_start(wlc, WLC_BCMCSCB_GET(wlc, bsscfg)); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
    }

    scb_psinfo->psp_flags &= ~PS_PSP_ONRESP;
    scb_psinfo->flags &= ~SCB_PS_FIRST_SUPPR_HANDLED;
    wlc_apps_apsd_usp_end(wlc, scb);
    scb_psinfo->apsd_cnt = 0;

    /* save ea before calling wlc_apps_ps_flush */
    ea = scb->ea;

    /* clear the PVB entry since we are leaving PM mode */
    if (scb_psinfo->in_pvb) {
        wlc_apps_pvb_update(wlc, scb);
    }

#ifdef WL_BEAMFORMING
    if (TXBF_ENAB(wlc->pub)) {
        /* Notify txbf module of the scb's PS change */
        wlc_txbf_scb_ps_notify(wlc->txbf, scb, FALSE);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  wlc_txbf_scb_ps_notify(wlc->txbf, scb, FALSE); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  wlc_txbf_scb_ps_notify(wlc->txbf, scb, FALSE); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
    }
#endif /* WL_BEAMFORMING */

#ifdef WL_USE_SUPR_PSQ
    /* Make sure to prepend suppressed pkts to PSQ before PS off to
     * avoid that pkts get stuck in suppress PSQ forever.
     */
    if (scb_psinfo->suppressed_pkts > 0) {
        wlc_apps_dequeue_scb_supr_psq(wlc, scb);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  wlc_apps_dequeue_scb_supr_psq(wlc, scb); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  wlc_apps_dequeue_scb_supr_psq(wlc, scb); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
    }
#endif /* WL_USE_SUPR_PSQ */

    /* Note: We do not clear up any pending PS-POLL pkts
     * which may be enq'd with the IGNOREPMQ bit set. The
     * relevant STA should stay awake until it rx's these
     * response pkts
     */

    if (discard == FALSE) {
#ifdef HNDPQP
        /* Page in all Host resident packets into dongle */
        wlc_apps_scb_pqp_pgi(wlc, scb, WLC_PREC_BMP_ALL, PKTQ_PKTS_ALL, PQP_PGI_PS_OFF);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  wlc_apps_scb_pqp_pgi(wlc, scb, WLC_PREC_BMP_ALL, PKTQ_PKTS_ALL, PQP_PGI_PS_OFF); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  wlc_apps_scb_pqp_pgi(wlc, scb, WLC_PREC_BMP_ALL, PKTQ_PKTS_ALL, PQP_PGI_PS_OFF); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
#endif /* HNDPQP */

#ifdef WLTAF
        /* If taf is not enabled then we need to release the data in the old way */
        if (!wlc_taf_psq_in_use(wlc->taf_handle, NULL) || SCB_MARKED_DEL(scb))
#endif /* WLTAF */
        {
            /* Move psq entries to Common Q for immediate tx */
#ifdef dump_stack_qqdx_print
            printk(KERN_ALERT"###########OSL_SYSUPTIME()(%u)",OSL_SYSUPTIME());
#endif /*dump_stack_qqdx_print*/
            wlc_apps_ps_send(wlc, scb, WLC_PREC_BMP_ALL, FALSE, NULL,
                WLC_APPS_RELEASE_ALL);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  wlc_apps_ps_send(wlc, scb, WLC_PREC_BMP_ALL, FALSE, NULL, dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"###########OSL_SYSUPTIME()(%u)",OSL_SYSUPTIME());
        printk(KERN_ALERT"----------[fyl]  wlc_apps_ps_send(wlc, scb, WLC_PREC_BMP_ALL, FALSE, NULL, dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
        }
    } else { /* free any pending frames */
        wlc_apps_ps_flush(wlc, scb);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  wlc_apps_ps_flush(wlc, scb); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"###########OSL_SYSUPTIME()(%u)",OSL_SYSUPTIME());
        printk(KERN_ALERT"----------[fyl]  wlc_apps_ps_flush(wlc, scb); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/

        /* callbacks in wlc_apps_ps_flush are not allowed to free scb */
        if (!ETHER_ISMULTI(&ea) && (wlc_scbfind(wlc, bsscfg, &ea) == NULL)) {
            WL_ERROR(("wl%d.%d: %s exiting, scb for "MACF" was freed\n",
                wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
                __FUNCTION__, ETHER_TO_MACF(ea)));
            ASSERT(0);
            return;
        }
    }

#ifdef WLTAF
    /* Update TAF state */
    wlc_taf_scb_state_update(wlc->taf_handle, scb, TAF_NO_SOURCE, TAF_PARAM(scb->PS),
        TAF_SCBSTATE_POWER_SAVE);
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  TAF_SCBSTATE_POWER_SAVE); dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  TAF_SCBSTATE_POWER_SAVE); dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
#endif /* WLTAF */

#ifdef PROP_TXSTATUS
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  PROP_TXSTATUS dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  PROP_TXSTATUS dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
    if (PROP_TXSTATUS_ENAB(wlc->pub)) {
        wlc_check_txq_fc(wlc, SCB_WLCIFP(scb)->qi);
        if (AMPDU_ENAB(wlc->pub)) {
            wlc_check_ampdu_fc(wlc->ampdu_tx, scb);
        }
    }
#endif /* PROP_TXSTATUS */

    if (scb_psinfo->change_scb_state_to_auth) {
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  scb_psinfo->change_scb_state_to_auth) dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  scb_psinfo->change_scb_state_to_auth) dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
        wlc_scb_resetstate(wlc, scb);
        wlc_scb_setstatebit(wlc, scb, AUTHENTICATED);

        wlc_bss_mac_event(wlc, SCB_BSSCFG(scb), WLC_E_DISASSOC_IND, &scb->ea,
            WLC_E_STATUS_SUCCESS, DOT11_RC_BUSY, 0, NULL, 0);
        scb_psinfo->change_scb_state_to_auth = FALSE;
    }

    scb_psinfo->ps_trans_status &= ~SCB_PS_TRANS_OFF_IN_PROG;

#ifdef HNDPQP
    /* If there are still pkts in host, it means PQP is out of resource.
     * PQP will set flags for current PS transition and
     * resume the remain process when resource is available.
     */
#ifdef dump_stack_qqdx_print
    //int dump_rand_flag = OSL_RAND() % 10000;
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  HNDPQP dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"----------[fyl]  HNDPQP dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
    if (PQP_PGI_PS_TRANS_OFF_ISSET(scb_psinfo)) {
        return;
    }
#endif /* HNDPQP */

    //int dump_rand_flag = OSL_RAND() % 10000;
#ifdef dump_stack_qqdx_print
    printk(KERN_ALERT"###########OSL_SYSUPTIME()(%u)",OSL_SYSUPTIME());
#endif /*dump_stack_qqdx_print*/
    wlc_apps_scb_ps_off_ps_pgi_done(wlc, scb, scb_psinfo);
#ifdef dump_stack_qqdx_print
    if (dump_rand_flag>=0) {
        printk(KERN_ALERT"----------[fyl]  end dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"###########OSL_SYSUPTIME()(%u)",OSL_SYSUPTIME());
        printk(KERN_ALERT"----------[fyl]  end dump_stack stop----------");
    }
#endif /*dump_stack_qqdx_print*/
}

static void
wlc_apps_bcmc_scb_ps_on(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    struct scb *bcmc_scb;
    /*  Use the bsscfg pointer of this scb to help us locate the
     *  correct bcmc_scb so that we can turn on PS
     */
    bcmc_scb = WLC_BCMCSCB_GET(wlc, bsscfg);
    ASSERT(bcmc_scb->bsscfg == bsscfg);

    if (SCB_PS(bcmc_scb)) {
        WL_PS0(("wl%d.%d: %s [bcmc_scb] Already in PS!\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
    }

    /* Clear transition off pending. */
    bsscfg->flags &= ~WLC_BSSCFG_PS_OFF_TRANS;

    WL_PS0(("wl%d.%d: %s BCMC PS on, in flight %d\n", wlc->pub->unit,
        WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(bcmc_scb)));
    bcmc_scb->PS = TRUE;

#ifdef WL_PS_STATS
    /* skip the counter increment if PS is already on */
    if (bcmc_scb->ps_starttime == 0) {
        bcmc_scb->ps_on_cnt++;
        bcmc_scb->ps_starttime = OSL_SYSUPTIME();
    /* dump_flag_qqdx */
#ifdef dump_stack_qqdx_print
        printk(KERN_ALERT"----------[fyl] wlc_apps_bcmc_scb_ps_on(wlc_info_t *wlc, scb_t *scb) dump_stack start----------");
        dump_stack();
        printk(KERN_ALERT"----------[fyl] wlc_apps_bcmc_scb_ps_on(wlc_info_t *wlc, scb_t *scb) dump_stack stop----------");
#endif /*dump_stack_qqdx_print*/
    }
#endif /* WL_PS_STATS */
    if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(bcmc_scb)) {
        wlc_pmq_process_switch(wlc, bcmc_scb, PMQ_ADD);
        bcmc_scb->ps_txfifo_blk = TRUE;
        wlc->ps_scb_txfifo_blk = TRUE;
        /* In case of !AUXPMQ no need to do full datablock. It will happen upon PM
         * processing for SCB (non BCMC) and BCMC can rely on that.
         */
        /* Add APPS to the txpath for this SCB */
        wlc_txmod_config(wlc->txmodi, bcmc_scb, TXMOD_APPS);
        /* Move all CQ packets to APPS PSQ */
        wlc_apps_txq_to_psq(wlc, bcmc_scb);
    }
}

/* This deals with all PS transitions from OFF->ON */
void
wlc_apps_scb_ps_on(wlc_info_t *wlc, struct scb *scb)
{
    /* dump_flag_qqdx */
#ifdef dump_stack_qqdx_print
        printk(KERN_ALERT"----[fyl] wlc_apps_scb_ps_on dump_stack start----(%u)",OSL_SYSUPTIME());
        dump_stack();
        printk(KERN_ALERT"----[fyl] wlc_apps_scb_ps_on dump_stack stop---(%u)",OSL_SYSUPTIME());
#endif /*dump_stack_qqdx_print*/
    struct apps_scb_psinfo *scb_psinfo;
    apps_wlc_psinfo_t *wlc_psinfo;
    apps_bss_info_t *bss_info;
    wlc_bsscfg_t *bsscfg;

    ASSERT(scb);

    bsscfg = SCB_BSSCFG(scb);
    ASSERT(bsscfg != NULL);

    if (BSSCFG_STA(bsscfg) && !BSS_TDLS_ENAB(wlc, bsscfg) && !BSSCFG_IBSS(bsscfg)) {
        WL_PS_EX(scb, ("AID %d: BSSCFG_STA(bsscfg)=%s, "
            "BSS_TDLS_ENAB(wlc,bsscfg)=%s\n\n", SCB_AID(scb),
            BSSCFG_STA(bsscfg) ? "TRUE" : "FALSE",
            BSS_TDLS_ENAB(wlc, bsscfg) ? "TRUE" : "FALSE"));
        return;
    }

    /* process OFF -> ON PS transition */
    wlc_psinfo = wlc->psinfo;
    bss_info = APPS_BSSCFG_CUBBY(wlc_psinfo, bsscfg);
    ASSERT(bss_info);

    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);

    WL_PS_EX(scb, ("AID %d in flight %d (%d)\n", SCB_AID(scb),
        SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb), TXPKTPENDTOT(wlc)));

    scb_psinfo->flags &= ~SCB_PS_FIRST_SUPPR_HANDLED;

    /* update PS state info */
    bss_info->ps_nodes++;
    wlc_psinfo->ps_nodes_all++;
    ASSERT(!scb->PS);
    scb->PS = TRUE;
#ifdef WL_PS_STATS
    if (scb->ps_starttime == 0) {
        scb->ps_on_cnt++;
        scb->ps_starttime = OSL_SYSUPTIME();
    }
#endif /* WL_PS_STATS */
    if (wlc->clk) {
        scb_psinfo->last_in_ps_tsf = wlc_lifetime_now(wlc);
    }

    scb_psinfo->tbtt = 0;

#ifdef WLTAF
    /* Update TAF state */
    wlc_taf_scb_state_update(wlc->taf_handle, scb, TAF_NO_SOURCE, TAF_PARAM(scb->PS),
        TAF_SCBSTATE_POWER_SAVE);
#endif /* WLTAF */

#if defined(BCMPCIEDEV)
    /* Reset all pending fetch and rollback flow fetch ptrs */
    wlc_scb_flow_ps_update(wlc->wl, SCB_FLOWID(scb), TRUE);
#endif
    /* If this is first STA to enter PS mode, set BCMC traffic to go through BCMC Fifo. */
    if (BSSCFG_HAS_BCMC_SCB(bsscfg)) {
        if (bss_info->ps_nodes == 1) {
            wlc_apps_bcmc_scb_ps_on(wlc, bsscfg);
        }
        ASSERT(SCB_PS(WLC_BCMCSCB_GET(wlc, bsscfg)));
    }

    /* Add the APPS to the txpath for this SCB */
    wlc_txmod_config(wlc->txmodi, scb, TXMOD_APPS);

    /* ps enQ any pkts on the txq, narq, ampduq */
    wlc_apps_txq_to_psq(wlc, scb);

    /* XXX TODO : Rollback packets to flow ring rather than
     * sending them into PSq during PS transition for PQP
     */
#ifdef PROP_TXSTATUS
    /* This causes problems for PSPRETEND */
    wlc_apps_ampdu_txq_to_psq(wlc, scb);
    wlc_apps_nar_txq_to_psq(wlc, scb);
#endif /* PROP_TXSTATUS */

    /* If there is anything in the data fifo then allow it to drain */
#if defined(WL_PS_SCB_TXFIFO_BLK)
    if (AUXPMQ_ENAB(wlc->pub)) {
        if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)) {
            wlc_pmq_process_switch(wlc, scb, PMQ_ADD);
            scb->ps_txfifo_blk = TRUE;
            wlc->ps_scb_txfifo_blk = TRUE;
            wlc->ps_txfifo_blk_scb_cnt++;
        }
    } else
#endif /* ! WL_PS_SCB_TXFIFO_BLK */
    if (TXPKTPENDTOT(wlc) > 0) {
        wlc_pmq_process_switch(wlc, scb, PMQ_ADD);
        wlc_block_datafifo(wlc, DATA_BLOCK_PS, DATA_BLOCK_PS);
        wlc->ps_scb_txfifo_blk = TRUE;
        wlc_apps_ps_stats_datablock_start(wlc);
    }

#ifdef WL_BEAMFORMING
    if (TXBF_ENAB(wlc->pub)) {
        /* Notify txbf module of the scb's PS change */
        wlc_txbf_scb_ps_notify(wlc->txbf, scb, TRUE);
    }
#endif /* WL_BEAMFORMING */

#ifdef HNDPQP
    /* PQP page out on SCB PS ON */
    wlc_apps_scb_pqp_pgo(wlc, scb, WLC_PREC_BMP_ALL);
#endif

    /* determine the PVB entry as we are entering PM mode */
    wlc_apps_pvb_update(wlc, scb);
}

/* Mark the source of PS on/off request so that one doesn't turn PS off while
 * another source still needs PS on.
 */
void
wlc_apps_ps_requester(wlc_info_t *wlc, struct scb *scb, uint8 on_rqstr, uint8 off_rqstr)
{
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    struct apps_scb_psinfo *scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);

    if ((scb_psinfo && ((scb_psinfo->ps_requester & on_rqstr) == 0) && (on_rqstr)) ||
        (scb_psinfo->ps_requester & off_rqstr)) {
        WL_PS_EX(scb, ("AID %d ON 0x%x OFF 0x%x   PS 0x%x\n",
            SCB_AID(scb), on_rqstr, off_rqstr,
            scb_psinfo->ps_requester));
    }

    if (wlc->clk && on_rqstr) {
        scb_psinfo->last_in_ps_tsf = wlc_lifetime_now(wlc);
    }
    scb_psinfo->ps_requester |= on_rqstr;
    scb_psinfo->ps_requester &= ~off_rqstr;
}

/* Workaround to prevent scb to remain in PS, waiting on an OMI PMQ or OMI HTC.
 * Both need to be received to exit PS, adding a timeout to guard against stuck condition.
 */
static void
wlc_apps_omi_waitpmq_war(wlc_info_t *wlc)
{
    struct scb_iter scbiter;
    struct scb *scb;
    struct apps_scb_psinfo *scb_psinfo;
    uint32 ps_duration, cur_time;

    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
        if (!scb_psinfo) {
            continue;
        }
        cur_time = wlc_lifetime_now(wlc);
        ps_duration = ((cur_time - scb_psinfo->last_in_ps_tsf) +
                500) / 1000;
        if (SCB_PS(scb) && (ps_duration >= 1000)) {
            if (scb_psinfo->ps_requester & PS_SWITCH_OMI) {
                WL_ERROR(("wl%d.%d "MACF": %s: WAR No OMI PMQ entry\n",
                    wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)),
                    ETHER_TO_MACF(scb->ea), __FUNCTION__));
                wlc_apps_ps_requester(wlc, scb, 0, PS_SWITCH_OMI);
                wlc_he_omi_pmq_reset(wlc, scb);
                wlc_apps_process_ps_switch(wlc, scb, PS_SWITCH_OFF);
            }
        }
    }
}

/* "normalize" a packet queue - move packets tagged with WLF3_SUPR flag
 * to the front and retain the order in case other packets were inserted
 * in the queue before.
 */
#ifdef WL_USE_SUPR_PSQ
/* Version operation on ps suppress queue */
static void
wlc_apps_dequeue_scb_supr_psq(wlc_info_t *wlc, struct scb *scb)
{
    int prec;
    struct apps_scb_psinfo *scb_psinfo;
    uint16 suppr_npkts;
    struct pktq *psq;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    ASSERT(scb_psinfo);
#ifdef HNDPQP
    BCM_REFERENCE(scb_psinfo);
    BCM_REFERENCE(prec);
    BCM_REFERENCE(suppr_npkts);
    BCM_REFERENCE(psq);
    /* PQP handles normalizing(joining) PS Q and suppress PS Q */
    wlc_apps_scb_suprq_pqp_normalize(wlc, scb);
#else /* !HNDPQP */

    psq = &scb_psinfo->psq;
    for (prec = (WLC_PREC_COUNT-1); prec >= 0; prec--) {
        suppr_npkts = spktq_n_pkts(&scb_psinfo->supr_psq[prec]);
        if (suppr_npkts == 0) {
            continue;
        }

        /* There should always be enough room to combine the queues. */
        ASSERT(suppr_npkts <= pktq_avail(psq));
        ASSERT(suppr_npkts <= pktqprec_avail_pkts(psq, prec));

        WL_PS_EX(scb, ("(Norm) prec %d suplen %d psqlen %d\n",
            prec, spktq_n_pkts(&scb_psinfo->supr_psq[prec]),
            pktq_n_pkts_tot(psq)));

        pktq_prepend(psq, prec, &scb_psinfo->supr_psq[prec]);
    }

    scb_psinfo->suppressed_pkts = 0;
#endif /* !HNDPQP */
}
#else /* ! WL_USE_SUPR_PSQ */
/* Version operation on txq */

static bool
is_pkt_wlf3_supr(void *pkt) /* callback to pktq_promote */
{
    if (WLPKTTAG(pkt)->flags3 & WLF3_SUPR) {
        WLPKTTAG(pkt)->flags3 &= ~WLF3_SUPR;
        return TRUE;
    }
    return FALSE;
}

/** @param pktq   Multi-priority packet queue */
static void
wlc_pktq_supr_norm(wlc_info_t *wlc, struct pktq *pktq)
{
    BCM_REFERENCE(wlc);

    if (pktq_n_pkts_tot(pktq) == 0)
        return;

    pktq_promote(pktq, is_pkt_wlf3_supr);
}
#endif /* ! WL_USE_SUPR_PSQ */

/* "normalize" the SCB's PS queue - move packets tagged with WLF3_SUPR flag
 * to the front and retain the order in case other packets were inserted
 * in the queue before.
 */
void
wlc_apps_scb_psq_norm(wlc_info_t *wlc, struct scb *scb)
{
    apps_wlc_psinfo_t *wlc_psinfo;
    struct apps_scb_psinfo *scb_psinfo;

    wlc_psinfo = wlc->psinfo;
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);

    /* Only normalize if necessary */
    if (scb_psinfo->suppressed_pkts) {
        WL_PS_EX(scb, ("Normalizing PSQ for STA\n"));
#ifdef WL_USE_SUPR_PSQ
        wlc_apps_dequeue_scb_supr_psq(wlc, scb);
#else
        wlc_pktq_supr_norm(wlc, &scb_psinfo->psq);
        scb_psinfo->suppressed_pkts = 0;
#endif
        if (!SCB_ISMULTI(scb)) {
            /* In the case of traffic oversubscription, the low priority
             * packets may stay long in TxFIFO and be suppreesed by MAC
             * when the STA goes to sleep.  Resetting scb ampdu alive may
             * avoid unnecessary ampdu session reset.
             */
            wlc_ampdu_scb_reset_alive(wlc, scb);
        }
    }
}

/* Some parts of APPS use statemachines where states are tracked to continue processing after
 * an SCB has no longer outstanding packets (on lo/hw queue). When the queue is empty this function
 * should be called to continue the different statemachines awaiting the drain complete.
 */
static void
wlc_apps_scb_drained(wlc_info_t *wlc, scb_t *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    bool discard;

    ASSERT(SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb) == 0);
    ASSERT(!SCB_INTERNAL(scb));
    ASSERT(!SCB_ISMULTI(scb));

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL) {
        return;
    }

    /* Process TWT related delayed state switches */
    if (SCB_TWTPS(scb)) {
        if (scb_psinfo->twt_wait4drain_norm) {
            wlc_apps_scb_psq_norm(wlc, scb);
            scb_psinfo->twt_wait4drain_norm = FALSE;
        }
        if (scb_psinfo->twt_wait4drain_exit) {
            wlc_apps_twt_sp_release_ps(wlc, scb);
        } else {
            wlc_twt_ps_suppress_done(wlc->twti, scb);
        }
    }

    /* Process (normal) PS related delayed state switches */
    if (SCB_PS(scb)) {
        if (scb->ps_txfifo_blk) {
            scb->ps_txfifo_blk = FALSE;
            wlc->ps_txfifo_blk_scb_cnt--;

            /* Notify bmac to remove PMQ entry for this STA */
            wlc_pmq_process_switch(wlc, scb, PMQ_REMOVE);
        }

        wlc_apps_scb_psq_norm(wlc, scb);

        if (scb_psinfo->twt_wait4drain_enter) {
            wlc_apps_twt_enter_ready(wlc, scb);
            scb_psinfo->twt_wait4drain_enter = FALSE;

        } else if ((scb_psinfo->ps_trans_status & SCB_PS_TRANS_OFF_PEND)) {
            /* we may get here as result of SCB deletion, so avoid
             * re-enqueueing frames in that case by discarding them
             */
            discard = SCB_DEL_IN_PROGRESS(scb)? TRUE : FALSE;
#ifdef PSPRETEND
            if (SCB_PS_PRETEND_BLOCKED(scb)) {
                WL_ERROR(("wl%d.%d: %s: SCB_PS_PRETEND_BLOCKED, expected to see "
                    "PMQ PPS entry\n", wlc->pub->unit,
                    WLC_BSSCFG_IDX(SCB_BSSCFG(scb)), __FUNCTION__));
            }
#endif /* PSPRETEND */
            WL_PS_EX(scb, ("Allowing PS Off for STA\n"));
    /* dump_flag_qqdx */
#ifdef dump_stack_ps_qqdx_print
    printk(KERN_ALERT"----------[fyl] wlc_apps_scb_drained (%u)wlc_apps_scb_ps_off----------",OSL_SYSUPTIME());
#endif /*dump_stack_ps_qqdx_print*/
            wlc_apps_scb_ps_off(wlc, scb, discard);
        }

    }

#ifdef PSPRETEND
    if (SCB_CLEANUP_IN_PROGRESS(scb) || SCB_DEL_IN_PROGRESS(scb) ||
        SCB_MARKED_DEL(scb)) {
        WL_PS_EX(scb, ("scb del in progress or marked for del\n"));
    } else if (SCB_PS_PRETEND_PROBING(scb)) {
        wlc_pspretend_probe_sched(wlc->pps_info, scb);
    }
#endif /* PSPRETEND */
}

/* Process any pending PS states */
void
wlc_apps_process_pend_ps(wlc_info_t *wlc)
{
    struct scb_iter scbiter;
    struct scb *scb;
    int idx;
    wlc_bsscfg_t *cfg;

    ASSERT(wlc->ps_scb_txfifo_blk);

#if defined(WL_PS_SCB_TXFIFO_BLK)
    /* When the system gets here the TXPKTKENDTOT count is 0, meaning there is no data on
     * lo txq. It should also mean that ps_txfifo_blk_scb_cnt is 0. If that is not the case
     * then something is wrong. This may happen due to for example a flush, as in that case
     * not all packets result in wlc_apps_trigger_on_complete. Print the information in that
     * case and try to recover.
     */
    if (AUXPMQ_ENAB(wlc->pub)) {
        if (!wlc->ps_txfifo_blk_scb_cnt) {
            goto skip_scb_iterate;
        }
        WL_PS0(("wl%d: %s wlc->ps_txfifo_blk_scb_cnt=%d\n", wlc->pub->unit, __FUNCTION__,
            wlc->ps_txfifo_blk_scb_cnt));
    }
    /* One or more of the SCBs have ps_txfifo_blk set to TRUE. These SCBs need to be
     * handled for queue empty/drained.
     */
#endif /* WL_PS_SCB_TXFIFO_BLK */
    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        if (SCB_PS(scb) &&
#if defined(WL_PS_SCB_TXFIFO_BLK)
            (!AUXPMQ_ENAB(wlc->pub) || scb->ps_txfifo_blk) &&
#endif /* WL_PS_SCB_TXFIFO_BLK */
            !SCB_ISMULTI(scb)) {
    /* dump_flag_qqdx */
#ifdef dump_stack_ps_qqdx_print
    printk(KERN_ALERT"----------[fyl] wlc_apps_process_pend_ps (%u)wlc_apps_scb_drained----------",OSL_SYSUPTIME());
#endif /*dump_stack_ps_qqdx_print*/

            wlc_apps_scb_drained(wlc, scb);
        }
    }

#if defined(WL_PS_SCB_TXFIFO_BLK)
    ASSERT(wlc->ps_txfifo_blk_scb_cnt == 0);

skip_scb_iterate:
#endif /* WL_PS_SCB_TXFIFO_BLK */

    /* Notify bmac to clear the PMQ */
    wlc_pmq_process_switch(wlc, NULL, PMQ_CLEAR_ALL);

    FOREACH_UP_AP(wlc, idx, cfg) {
        scb = WLC_BCMCSCB_GET(wlc, cfg);
        if (scb) {
            if (scb->ps_txfifo_blk) {
                WL_PS_EX(scb, ("wlc_apps_bcmc_suppress_done \n"));
                wlc_apps_bcmc_suppress_done(wlc, cfg);
            }
            if (cfg->flags & WLC_BSSCFG_PS_OFF_TRANS) {
                wlc_apps_bcmc_ps_off_done(wlc, cfg);
            }
        }
    }

    wlc_block_datafifo(wlc, DATA_BLOCK_PS, 0);
    wlc->ps_scb_txfifo_blk = FALSE;
    wlc_apps_ps_stats_datablock_stop(wlc);

    /* If any suppressed BCMC packets at the head of txq,
     * they need to be sent to hw fifo right now.
     */
    if (wlc->active_queue != NULL && WLC_TXQ_OCCUPIED(wlc)) {
        wlc_send_q(wlc, wlc->active_queue);
    }
}

/* wlc_apps_ps_flush()
 * Free any pending PS packets for this STA
 *
 * Called when APPS is handling a driver down transision, when an SCB is deleted,
 * when wlc_apps_scb_ps_off() is called with the discard param
 *    - from wlc_scb_disassoc_cleanup()
 *    - when a STA re-associates
 *    - from a deauth completion
 */
void
wlc_apps_ps_flush(wlc_info_t *wlc, struct scb *scb)
{
    uint8 tid;
#if defined(BCMDBG) || defined(BCMDBG_ASSERT)
    struct apps_scb_psinfo *scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
#endif /* BCMDBG || BCMDBG_ASSERT */

#ifdef WLTAF
    /* Defer taf schedule to prevent packet enqueue during PSQ flush */
    wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */

    for (tid = 0; tid < NUMPRIO; tid++) {
        wlc_apps_ps_flush_tid(wlc, scb, tid);
    }

#if defined(BCMDBG) || defined(BCMDBG_ASSERT)
    BCM_REFERENCE(scb_psinfo);
    ASSERT(scb_psinfo == NULL || pktq_empty(&scb_psinfo->psq));
#endif /* BCMDBG || BCMDBG_ASSERT */

#ifdef WLTAF
    wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */
}

/* When a flowring is deleted or scb is removed, the packets related to that flowring or scb have
 * to be flushed.
 */
static void
wlc_apps_ps_flush_tid(void *context, struct scb *scb, uint8 tid)
{
    wlc_info_t *wlc = (wlc_info_t *)context;
    int prec;
    uint32 n_pkts_flushed = 0;
    struct apps_scb_psinfo *scb_psinfo;
    apps_wlc_psinfo_t *wlc_psinfo;
    struct pktq *psq;

    ASSERT(scb);
    ASSERT(wlc);

    wlc_psinfo = wlc->psinfo;
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    if (scb_psinfo == NULL)
        return;

    if (scb_psinfo->flags & SCB_PS_FLUSH_IN_PROGR) {
        WL_PS_EX(scb, ("flush SCB in progress\n"));
        return;
    }

    scb_psinfo->flags |= SCB_PS_FLUSH_IN_PROGR;
    prec = WLC_PRIO_TO_PREC(tid);
    psq = &scb_psinfo->psq;

#ifdef WL_USE_SUPR_PSQ
    /* Checking PS suppress queue and moving ps suppressed frames accordingly */
    if (scb_psinfo->suppressed_pkts > 0) {
        wlc_apps_dequeue_scb_supr_psq(wlc, scb);
    }
#endif /* WL_USE_SUPR_PSQ */

#ifdef HNDPQP
    /* Page in all Host resident packets into dongle */
    n_pkts_flushed += wlc_apps_scb_pqp_ps_flush_prec(wlc, scb, psq, prec);
#else  /* !HNDPQP */
    if (!pktqprec_empty(psq, prec)) {
        n_pkts_flushed += wlc_txq_pktq_pflush(wlc, psq, prec);
    }
#endif /* !HNDPQP */

    prec = WLC_PRIO_TO_HI_PREC(tid);

#ifdef HNDPQP
    /* Page in all Host resident packets into dongle */
    n_pkts_flushed += wlc_apps_scb_pqp_ps_flush_prec(wlc, scb, psq, prec);
#else  /* !HNDPQP */
    if (!pktqprec_empty(psq, prec)) {
        n_pkts_flushed += wlc_txq_pktq_pflush(wlc, psq, prec);
    }
#endif /* !HNDPQP */

    if (n_pkts_flushed > 0) {
        WL_PS_EX(scb, ("flushing %d packets for AID %d tid %d\n",
            n_pkts_flushed,    SCB_AID(scb), tid));

        if (!SCB_ISMULTI(scb)) {
            if (n_pkts_flushed > wlc_psinfo->ps_deferred) {
                ASSERT(0);
            }
            wlc_psinfo->ps_deferred -= n_pkts_flushed;
        }
    }

#ifdef WLTAF
    scb_psinfo->taf_prio_active &= ~(1 << tid);

    /* tell TAF we are not using this apps context anymore */
    wlc_taf_link_state(wlc->taf_handle, scb, tid, TAF_PSQ, TAF_LINKSTATE_REMOVE);

    if (scb_psinfo->taf_prio_active == 0) {
        wlc_taf_scb_state_update(wlc->taf_handle, scb, TAF_PSQ, NULL,
            TAF_SCBSTATE_SOURCE_DISABLE);
    }
#endif

    scb_psinfo->flags &= ~SCB_PS_FLUSH_IN_PROGR;

    /* If there is a valid aid (the bcmc scb wont have one) then ensure the PVB is updated. */
    if (scb->aid) {
        wlc_apps_pvb_update(wlc, scb);
    }
}

#ifdef PROP_TXSTATUS
void
wlc_apps_ps_flush_mchan(wlc_info_t *wlc, struct scb *scb)
{
    uint32 freed_count;
    struct apps_scb_psinfo *scb_psinfo;
    apps_wlc_psinfo_t *wlc_psinfo;

    ASSERT(scb);
    ASSERT(wlc);

    wlc_psinfo = wlc->psinfo;
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    if (scb_psinfo == NULL)
        return;

    freed_count = wlc_wlfc_flush_queue(wlc, &scb_psinfo->psq);

    if (!SCB_ISMULTI(scb)) {
        ASSERT(freed_count <= wlc_psinfo->ps_deferred);
        wlc_psinfo->ps_deferred -= freed_count;
    }

    /* If there is a valid aid (the bcmc scb wont have one) then ensure
     * the PVB is cleared.
     */
    if (scb->aid && scb_psinfo->in_pvb)
        wlc_apps_pvb_update(wlc, scb);
}
#endif /* defined(PROP_TXSTATUS) */

#ifdef WL_USE_SUPR_PSQ
static bool
wlc_apps_enqueue_scb_supr_psq(struct apps_scb_psinfo *scb_psinfo, void* pkt, int prec)
{
    void *p;
    struct spktq *supr_psq = scb_psinfo->supr_psq;

    ASSERT(!spktq_full(&supr_psq[prec]));
    if (spktq_full(&supr_psq[prec])) {
        return FALSE;
    }

    /* Enqueue */
    p = spktq_enq(&supr_psq[prec], pkt);

    if (p == NULL)
        WL_ERROR(("%s: null ptr2", __FUNCTION__));
    ASSERT(p != NULL);

    /* Bump suppressed count, indicating need for psq normalization */
    scb_psinfo->suppressed_pkts++;

    return TRUE;
}
#endif /* WL_USE_SUPR_PSQ */

/* Return TRUE if packet has been enqueued on a ps queue, FALSE otherwise */
#define WLC_PS_APSD_HPKT_TIME 12 /* in ms */

bool
wlc_apps_psq(wlc_info_t *wlc, void *pkt, int prec)
{
    apps_wlc_psinfo_t *wlc_psinfo;
    struct apps_scb_psinfo *scb_psinfo;
    struct scb *scb;
    uint psq_len;
    uint psq_len_diff;

    scb = WLPKTTAGSCBGET(pkt);

    ASSERT(SCB_PS(scb) || SCB_TWTPS(scb) || wlc_twt_scb_active(wlc->twti, scb) ||
        SCB_ISMULTI(scb));
    ASSERT(wlc);

    /* Do not enq packets for disassociated STA, so return FALSE if so, unless scb is WDS or
     * MULTI.
     */
    if (!SCB_ASSOCIATED(scb) && !BSSCFG_IBSS(SCB_BSSCFG(scb)) && !SCB_WDS(scb) &&
        !SCB_ISMULTI(scb)) {
        return FALSE;
    }

    wlc_psinfo = wlc->psinfo;
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);

#ifdef BCMPCIEDEV
    if (!SCB_ISMULTI(scb)) {
        wlc_apps_scb_apsd_dec_v2r_in_transit(wlc, scb, pkt, prec);
    }
#endif /* BCMPCIEDEV */

    if (scb_psinfo->flags & SCB_PS_FLUSH_IN_PROGR) {
        WL_PS_EX(scb, ("flush in progress\n"));
        return FALSE;
    }

#if defined(PROP_TXSTATUS)
    if (PROP_TXSTATUS_ENAB(wlc->pub) && !SCB_ISMULTI(scb)) {
        /* If a packet in response to PSPoll, or in response to APSD trigger is
         * wanted/requested then skip the suppress check and put the packet on the
         * on the PSQ. This used to be handled with WLFC_PKTFLAG_PKT_REQUESTED but that
         * no longer is set.
         */
        if (SCB_PSPOLL_PKT_WAITING(scb_psinfo) ||
            (scb_psinfo->apsd_hpkt_timer_on && scb_psinfo->apsd_cnt &&
            AC_BITMAP_TST(scb->apsd.ac_delv, WME_PRIO2AC(PKTPRIO(pkt))))) {
            goto skip_suppress_check;
        }
        /* If the host sets a flag marking the packet as "in response to
           credit request for pspoll" then only the fimrware enqueues it.
           Otherwise wlc drops it by sending a wlc_suppress.
           */
        if ((WL_TXSTATUS_GET_FLAGS(WLPKTTAG(pkt)->wl_hdr_information) &
                WLFC_PKTFLAG_PKTFROMHOST) &&
                HOST_PROPTXSTATUS_ACTIVATED(wlc) &&
                (!(WL_TXSTATUS_GET_FLAGS(WLPKTTAG(pkt)->wl_hdr_information) &
                WLFC_PKTFLAG_PKT_REQUESTED))) {

            WLFC_DBGMESG(("R[0x%.8x]\n", (WLPKTTAG(pkt)->wl_hdr_information)));
            return FALSE;
        }
    }
skip_suppress_check:
#endif /* PROP_TXSTATUS */

#ifdef WL_USE_SUPR_PSQ
    psq_len = pktq_n_pkts_tot(&scb_psinfo->psq) + scb_psinfo->suppressed_pkts;
#else
    psq_len = pktq_n_pkts_tot(&scb_psinfo->psq);
#endif

#ifdef DEBUG_PS_PACKETS
    WL_PS_EX(scb, ("enq %p to PSQ, prec = 0x%x, scb_psinfo->apsd_usp = %s\n",
        pkt, prec, scb_psinfo->apsd_usp ? "TRUE" : "FALSE"));
#endif /* DEBUG_PS_PACKETS */

#ifdef WLCFP
    if (PKTISCFP(pkt)) {
        /* Packet enqueued by CFP: Release by legacy path */
        /* Reset CFP flags now */
        PKTCLRCFPFLOWID(pkt, SCB_FLOWID_INVALID);
    }
#endif /* WLCFP */

#ifdef WL_USE_SUPR_PSQ
    if (WLPKTTAG(pkt)->flags3 & WLF3_SUPR) {
        /* Removing WLF3_SUPR flag since it doesn't require normalization.
         * supr_psq will be prepended to psq.
         */
        WLPKTTAG(pkt)->flags3 &= ~WLF3_SUPR;

        if (!wlc_apps_enqueue_scb_supr_psq(scb_psinfo, pkt, prec)) {
            WL_PS_EX(scb, ("supr_psq full\n"));
            wlc_psinfo->supr_drop++;
            scb_psinfo->supr_drop++;
            return FALSE;
        }

#ifdef WL_PS_STATS
        scb->suprps_cnt++;
#endif /* WL_PS_STATS */
    } else if (!wlc_prec_enq(wlc, &scb_psinfo->psq, pkt, prec)) {
        WL_PS_EX(scb, ("wlc_prec_enq() failed\n"));
        return FALSE;
    }
#else /* ! WL_USE_SUPR_PSQ */
    if (!wlc_prec_enq(wlc, &scb_psinfo->psq, pkt, prec)) {
        WL_PS_EX(scb, ("wlc_prec_enq() failed\n"));
        return FALSE;
    } else {
        /* Else pkt was added to psq. If it's a suppressed pkt, mark the
         * psq as dirty, thus requiring normalization.
         */
        if (WLPKTTAG(pkt)->flags3 & WLF3_SUPR) {
            /* Bump suppressed count, indicating need for psq normalization */
            scb_psinfo->suppressed_pkts++;
#ifdef WL_PS_STATS
            scb->suprps_cnt++;
#endif /* WL_PS_STATS */
        }
    }
#endif /* ! WL_USE_SUPR_PSQ */

#ifdef WLTAF
    if (!SCB_ISMULTI(scb)) {
        if (!(scb_psinfo->taf_prio_active & (1 << TAF_PREC(prec)))) {
            if (scb_psinfo->taf_prio_active == 0) {
                wlc_taf_scb_state_update(wlc->taf_handle, scb, TAF_PSQ, NULL,
                    TAF_SCBSTATE_SOURCE_ENABLE);
            }
            /* tell TAF we started using this context */
            wlc_taf_link_state(wlc->taf_handle, scb, TAF_PREC(prec), TAF_PSQ,
                TAF_LINKSTATE_ACTIVE);
            scb_psinfo->taf_prio_active |= (1 << TAF_PREC(prec));
        }
        /* (re-)enque into psq, possibly due to suppress, adjust TAF scores. */
        wlc_taf_txpkt_status(wlc->taf_handle, scb, TAF_PREC(prec), pkt,
            TAF_TXPKT_STATUS_PS_QUEUED);
    }
#endif /* WLTAF */

    /* increment total count of PS pkts enqueued in WL driver */
#ifdef WL_USE_SUPR_PSQ
    psq_len_diff = pktq_n_pkts_tot(&scb_psinfo->psq) + scb_psinfo->suppressed_pkts - psq_len;
#else
    psq_len_diff = pktq_n_pkts_tot(&scb_psinfo->psq) - psq_len;
#endif /* ! WL_USE_SUPR_PSQ */
    ASSERT(psq_len_diff < 2);
    if (psq_len_diff == 1) {
        if (!SCB_ISMULTI(scb)) {
            wlc_psinfo->ps_deferred++;
        }
    } else if (psq_len_diff == 0) {
        WL_PS_EX(scb, ("wlc_prec_enq() dropped pkt.\n"));
    }

#ifdef WLTDLS
    if (BSS_TDLS_ENAB(wlc, scb->bsscfg)) {
        if (!scb_psinfo->apsd_usp)
            wlc_tdls_send_pti(wlc->tdls, scb);
        else if (wlc_tdls_in_pti_interval(wlc->tdls, scb)) {
            scb_psinfo->apsd_cnt = wlc_apps_apsd_delv_count(wlc, scb);
            if (scb_psinfo->apsd_cnt)
                wlc_apps_apsd_send(wlc, scb);
            return TRUE;
        }
    }
#endif /* WLTDLS */

    /* Check if the PVB entry needs to be set */
    if (!SCB_ISMULTI(scb)) {
        if (!scb_psinfo->in_pvb) {
            wlc_apps_pvb_update(wlc, scb);
        }
#ifdef BCMPCIEDEV
        _wlc_apps_scb_psq_resp(wlc, scb, scb_psinfo);
#endif /* BCMPCIEDEV */
    }
    return (TRUE);
}

/*
 * Move a PS-buffered packet to the txq and send the txq.
 * Returns TRUE if a packet was available to dequeue and send.
 * extra_flags are added to packet flags (for SDU, only to last MPDU)
 * This function should not be called directly, but the function wlc_apps_ps_send
 * should be used instead. wlc_apps_ps_send will trigger the CQ release.
 */
static bool
wlc_apps_ps_send_one(wlc_info_t *wlc, struct scb *scb, uint prec_bmp, bool apsd,
    taf_scheduler_public_t *taf)
{
    void *pkt, *next_pkt;
    struct apps_scb_psinfo *scb_psinfo;
    apps_wlc_psinfo_t *wlc_psinfo;
    int prec;
    struct pktq *psq;        /**< multi-priority packet queue */
    bool apsd_end = FALSE;
    uint8 *data;
    struct dot11_header *h;
    uint tsoHdrSize;
    uint seq_num, next_seq_num;
    bool control;

    ASSERT(wlc);
    ASSERT(scb);

    if (SCB_DEL_IN_PROGRESS(scb)) {
        /* free any pending frames */
        WL_PS_EX(scb, ("AID %d, prec %x scb is marked for deletion, "
            " freeing all pending PS packets.\n",
            SCB_AID(scb), prec_bmp));
        wlc_apps_ps_flush(wlc, scb);
        return FALSE;
    }

    wlc_psinfo = wlc->psinfo;
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);
    psq = &scb_psinfo->psq;

    if (SCB_MARKED_DEL(scb)) {
        while ((pkt = pktq_mdeq(psq, prec_bmp, &prec)) != NULL) {
            if ((WLPKTTAG(pkt)->flags & WLF_CTLMGMT) == 0) {
                WL_PS_EX(scb, ("AID %d, prec %x dropped data pkt %p\n",
                    SCB_AID(scb), prec_bmp, pkt));
                apsd_end = TRUE;
                PKTFREE(wlc->osh, pkt, TRUE);
                /* Decrement the global ps pkt cnt */
                if (!SCB_ISMULTI(scb)) {
                    wlc_psinfo->ps_deferred--;
                }
            } else {
                break;
            }
        }
    } else {
        pkt = pktq_mdeq(psq, prec_bmp, &prec);
    }

    /* Dequeue the packet with highest precedence out of a given set of precedences */
    if (!pkt) {
        WL_TRACE(("wl%d.%d: %s AID %d, prec %x no traffic\n", wlc->pub->unit,
            WLC_BSSCFG_IDX(scb->bsscfg), __FUNCTION__, SCB_AID(scb), prec_bmp));
        return FALSE;        /* no traffic to send */
    }

    /* Decrement the global ps pkt cnt */
    if (!SCB_ISMULTI(scb)) {
        wlc_psinfo->ps_deferred--;
    }

#ifdef DEBUG_PS_PACKETS
    WL_PS_EX(scb, ("AID %d, prec %x dequed pkt %p\n", SCB_AID(scb), prec_bmp, pkt));
#endif /* DEBUG_PS_PACKETS */
    /*
     * If it's the first MPDU in a series of suppressed MPDUs that make up an SDU,
     * enqueue all of them together before calling wlc_send_q.
     */
    if (!AMPDU_AQM_ENAB(wlc->pub)) {
        goto skip_fragmented_combine;
    }
    if (!(WLPKTTAG(pkt)->flags & WLF_TXHDR)) {
        goto skip_fragmented_combine;
    }

    data = PKTDATA(wlc->osh, pkt);
#ifdef WLTOEHW
    tsoHdrSize = WLC_TSO_HDR_LEN(wlc, (d11ac_tso_t*)data);
#else
    tsoHdrSize = 0;
#endif
    h = (struct dot11_header *)(data + tsoHdrSize + D11_TXH_LEN_EX(wlc));
    control = FC_TYPE(ltoh16(h->fc)) == FC_TYPE_CTL;

    /* Control frames does not have seq field; directly queue them. */
    if (control) {
        goto skip_fragmented_combine;
    }
    seq_num = ltoh16(h->seq) >> SEQNUM_SHIFT;

    while ((next_pkt = pktqprec_peek(psq, prec)) != NULL) {
        /* Stop if not suppressed frame */
        if (!(WLPKTTAG(next_pkt)->flags & WLF_TXHDR)) {
            break;
        }

        data = PKTDATA(wlc->osh, next_pkt);
        /* Stop if different sequence number */
#ifdef WLTOEHW
        tsoHdrSize = WLC_TSO_HDR_LEN(wlc, (d11ac_tso_t*)data);
#endif
        h = (struct dot11_header *)(data + tsoHdrSize + D11_TXH_LEN_EX(wlc));
        control = FC_TYPE(ltoh16(h->fc)) == FC_TYPE_CTL;

        /* stop if different ft; control frames do not have sequence control. */
        if (control) {
            break;
        }

        next_seq_num = ltoh16(h->seq) >> SEQNUM_SHIFT;
        if (next_seq_num != seq_num) {
            break;
        }

        /* Enqueue the packet at higher precedence level */
        if (!wlc_apps_ps_enq_resp(wlc, scb, pkt, WLC_PRIO_TO_HI_PREC(PKTPRIO(pkt)), taf)) {
            apsd_end = TRUE;
        }

        /* Dequeue the peeked packet */
        pkt = pktq_pdeq(psq, prec);
        ASSERT(pkt == next_pkt);

        /* Decrement the global ps pkt cnt */
        if (!SCB_ISMULTI(scb)) {
            wlc_psinfo->ps_deferred--;
        }
    }
skip_fragmented_combine:

    /* Set additional flags on SDU or on final MPDU */
    WLPKTTAG(pkt)->flags |= (apsd ? WLF_APSD : 0);

    /* Enqueue the packet at higher precedence level */
    if (!wlc_apps_ps_enq_resp(wlc, scb, pkt, WLC_PRIO_TO_HI_PREC(PKTPRIO(pkt)), taf)) {
        apsd_end = TRUE;
    }

    if (apsd && apsd_end) {
        wlc_apps_apsd_usp_end(wlc, scb);
    }

#ifdef PROP_TXSTATUS
    if (apsd) {
        scb_psinfo->apsd_tx_pending = TRUE;
    }
#endif

    return TRUE;
}

static bool
wlc_apps_ps_send(wlc_info_t *wlc, struct scb *scb, uint prec_bmp, bool apsd,
    taf_scheduler_public_t *taf, uint count)
{
    bool ret_value;
    uint release_exp;
    uint release_counter;

    /* If this is possibly APSD response then start release after first packet */
    release_exp = apsd ? 0 : WLC_APPS_RELEASE_EXP;
    release_counter = (1 << release_exp);

    ret_value = TRUE;
    while (count) {
        if (wlc_apps_ps_send_one(wlc, scb, prec_bmp, apsd, taf)) {
            count--;
            release_counter--;
        } else {
            ret_value = FALSE;
            count = 0;
            release_counter = 0;
        }
        if (!release_counter) {
            /* Send to hardware */
            wlc_send_q(wlc, SCB_WLCIFP(scb)->qi);
            release_exp++;
            release_counter = (1 << release_exp);
        }
    }

    return ret_value;
}

static bool
wlc_apps_ps_enq_resp(wlc_info_t *wlc, struct scb *scb, void *pkt, int prec,
    taf_scheduler_public_t *taf)
{
    wlc_txq_info_t *qi = SCB_WLCIFP(scb)->qi;

#ifdef WLTAF
    if (taf) {
        wlc_apps_taf_tag_release(wlc, taf, pkt, scb);
    }
#endif /* WLTAF */

    /* register WLF2_PCB2_PSP_RSP for pkt */
    WLF2_PCB2_REG(pkt, WLF2_PCB2_PSP_RSP);

    /* Ensure the pkt marker (used for ageing) is cleared */
    WLPKTTAG(pkt)->flags &= ~WLF_PSMARK;

#ifdef DEBUG_PS_PACKETS
    WL_PS_EX(scb, ("%p supr %d apsd %d\n",
        OSL_OBFUSCATE_BUF(pkt), (WLPKTTAG(pkt)->flags & WLF_TXHDR) ? 1 : 0,
        (WLPKTTAG(pkt)->flags & WLF_APSD) ? 1 : 0));
#endif /* DEBUG_PS_PACKETS */

    /* Enqueue in order of precedence */
    if (!cpktq_prec_enq(wlc, &qi->cpktq, pkt, prec, FALSE)) {
        WL_ERROR(("wl%d.%d: %s txq full, frame discarded\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)), __FUNCTION__));
        PKTFREE(wlc->osh, pkt, TRUE);
        return FALSE;
    }

    return TRUE;
}

void
wlc_apps_set_listen_prd(wlc_info_t *wlc, struct scb *scb, uint16 listen)
{
    struct apps_scb_psinfo *scb_psinfo;
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo != NULL)
        scb_psinfo->listen = listen;
}

uint16
wlc_apps_get_listen_prd(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo != NULL)
        return scb_psinfo->listen;
    return 0;
}

static bool
wlc_apps_psq_ageing_needed(wlc_info_t *wlc, struct scb *scb)
{
    wlc_bss_info_t *current_bss = scb->bsscfg->current_bss;
    uint16 interval;
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    /* Using scb->listen + 1 sec for ageing to avoid packet drop.
     * In WMM-PS:Test Case 4.10(M.V) which is legacy mixed with wmmps.
     * buffered frame will be dropped because ageing occurs.
     */
    interval = scb_psinfo->listen + (1000/current_bss->beacon_period);

#ifdef WLWNM_AP
    if (WLWNM_ENAB(wlc->pub)) {
        uint32 wnm_scbcap = wlc_wnm_get_scbcap(wlc, scb);
        int sleep_interval = wlc_wnm_scb_sm_interval(wlc, scb);

        if (SCB_WNM_SLEEP(wnm_scbcap) && sleep_interval) {
            interval = MAX((current_bss->dtim_period * sleep_interval), interval);
        }
    }
#endif /* WLWNM_AP */

    return (scb_psinfo->tbtt >= interval);
}

/* Reclaim as many PS pkts as possible
 *    Reclaim from all STAs with pending traffic.
 */
void
wlc_apps_psq_ageing(wlc_info_t *wlc)
{
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    apps_scb_psinfo_t *scb_psinfo;
    scb_iter_t scbiter;
    scb_t *scb;

    if (wlc_psinfo->ps_nodes_all == 0) {
        return; /* No one in PS */
    }

    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        if (!scb->permanent && SCB_PS(scb) && wlc_apps_psq_ageing_needed(wlc, scb)) {
            scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
            scb_psinfo->tbtt = 0;
            /* Initiate an ageing event per listen interval */
            if (!pktq_empty(&scb_psinfo->psq)) {
                wlc_apps_ps_timedout(wlc, scb);
            }
        }
    }
}

/**
 * Context structure used by wlc_apps_ps_timeout_filter() while filtering a ps pktq
 */
struct wlc_apps_ps_timeout_filter_info {
    uint              count;    /**< total num packets deleted */
};

/**
 * Pktq filter function to age-out pkts on an SCB psq.
 */
static pktq_filter_result_t
wlc_apps_ps_timeout_filter(void* ctx, void* pkt)
{
    struct wlc_apps_ps_timeout_filter_info *info;
    pktq_filter_result_t ret;

    info = (struct wlc_apps_ps_timeout_filter_info *)ctx;

    /* If not marked just move on */
    if ((WLPKTTAG(pkt)->flags & WLF_PSMARK) == 0) {
        WLPKTTAG(pkt)->flags |= WLF_PSMARK;
        ret = PKT_FILTER_NOACTION;
    } else {
        info->count++;
        ret = PKT_FILTER_DELETE;
    }

    return ret;
}

/* check if we should age pkts or not */
static void
wlc_apps_ps_timedout(wlc_info_t *wlc, struct scb *scb)
{
    struct ether_addr ea;
    struct apps_scb_psinfo *scb_psinfo;
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    struct pktq *psq;        /**< multi-priority packet queue */
    wlc_bsscfg_t *bsscfg;
    struct wlc_apps_ps_timeout_filter_info info;

    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);

    psq = &scb_psinfo->psq;
    ASSERT(!pktq_empty(psq));

    /* save ea and bsscfg before call pkt flush */
    ea = scb->ea;
    bsscfg = SCB_BSSCFG(scb);

    /* init the state for the pktq filter */
    info.count = 0;
    BCM_REFERENCE(psq);

#ifdef WLTAF
    /* Defer taf schedule to prevent packet enqueue during PSQ flush */
    wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */
    scb_psinfo->flags |= SCB_PS_FLUSH_IN_PROGR;

#ifdef HNDPQP
    wlc_apps_scb_pqp_pktq_filter(wlc, scb, (void *)&info);
#else  /* !HNDPQP */
    /* Age out all pkts that have been through one previous listen interval */
    wlc_txq_pktq_filter(wlc, psq, wlc_apps_ps_timeout_filter, &info);
#endif /* !HNDPQP */

    scb_psinfo->flags &= ~SCB_PS_FLUSH_IN_PROGR;

    if (info.count) {
#ifdef BCMDBG
        WL_ERROR(("wl%d.%d: "MACF" %s: %d packets timed out\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), ETHERP_TO_MACF(&ea),
            __FUNCTION__, info.count));
        wlc_apps_print_scb_info(wlc, scb);
#else
        WL_PS_EX(scb, ("timing out %d packet for AID %d, %d remain\n",
            info.count, SCB_AID(scb), pktq_n_pkts_tot(psq)));
#endif /* BCMDBG */
    }

    /* Decrement the global ps pkt cnt */
    if (!SCB_ISMULTI(scb)) {
        ASSERT(wlc_psinfo->ps_deferred >= info.count);
        wlc_psinfo->ps_deferred -= info.count;
    }
    wlc_psinfo->ps_aged += info.count;

#ifdef WLTAF
    wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */

    /* callback may have freed scb, exit early if so */
    if (wlc_scbfind(wlc, bsscfg, &ea) == NULL) {
        WL_PS(("wl%d.%d: %s exiting, scb for "MACF" was freed after last packet"
            " timeout\n",
            wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, ETHERP_TO_MACF(&ea)));
        return;
    }

    /* update the beacon PVB, but only if the SCB was not deleted */
    wlc_apps_pvb_update(wlc, scb);
}

/* Provides how many packets can be 'released' to the PS queue.
 *   - PSQ avail len
 */
uint32
wlc_apps_release_count(wlc_info_t *wlc, struct scb *scb, int prec)
{
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    struct apps_scb_psinfo *scb_psinfo;
    struct pktq *q;        /**< multi-priority packet queue */
    uint32 psq_len, psq_avail;

    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    q = &scb_psinfo->psq;

    psq_len = pktq_n_pkts_tot(q);
    if (psq_len <= PSQ_PKTQ_LEN_DEFAULT) {
        psq_avail = PSQ_PKTQ_LEN_DEFAULT - psq_len;
        /* PS queue avail */
        return MIN(pktq_avail(q), psq_avail);
    }

    return 0;
}

/* wlc_apps_enqueue_pkt()
 *
 * Try to PS enq a pkt, return false if we could not
 *
 * wlc_apps_enqueue_pkt() Called from:
 *    wlc_apps_ps_enq() TxMod
 *    wlc_apps_move_to_psq()
 *        wlc_apps_ampdu_txq_to_psq() --| from wlc_apps_scb_ps_on()
 *        wlc_apps_txq_to_psq() --------| from wlc_apps_scb_ps_on()
 * Implements TDLS PRI response handling to bypass PSQ buffering
 */
static void
wlc_apps_enqueue_pkt(void *ctx, struct scb *scb, void *pkt, uint prec)
{
    wlc_info_t *wlc = (wlc_info_t *)ctx;
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    struct apps_scb_psinfo *scb_psinfo;

    ASSERT(!PKTISCHAINED(pkt));

#ifdef WLTDLS
    /* for TDLS PTI resp, don't enq to PSQ, send right away */
    if (BSS_TDLS_ENAB(wlc, SCB_BSSCFG(scb)) && SCB_PS(scb) &&
        (WLPKTTAG(pkt)->flags & WLF_PSDONTQ)) {
        WL_PS_EX(scb, ("skip enq to PSQ\n"));
        SCB_TX_NEXT(TXMOD_APPS, scb, pkt, prec);
        return;
    }
#endif /* WLTDLS */

    ASSERT(scb);
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);

    if (!wlc_apps_psq(wlc, pkt, prec)) {
        wlc_psinfo->ps_discard++;
        scb_psinfo->ps_discard++;
#ifdef PROP_TXSTATUS
        if (PROP_TXSTATUS_ENAB(wlc->pub)) {
            /* wlc decided to discard the packet, host should hold onto it,
             * this is a "suppress by wl" instead of D11
             */
#ifdef DEBUG_PS_PACKETS
            WL_PS_EX(scb, ("ps pkt %p suppressed AID %d\n", pkt, SCB_AID(scb)));
#endif /* DEBUG_PS_PACKETS */
            wlc_suppress_sync_fsm(wlc, scb, pkt, TRUE);
            wlc_process_wlhdr_txstatus(wlc, WLFC_CTL_PKTFLAG_WLSUPPRESS, pkt, FALSE);
        } else
#endif /* PROP_TXSTATUS */
        {
#ifdef DEBUG_PS_PACKETS
            WL_PS_EX(scb, ("ps pkt %p discarded AID %d\n", pkt, SCB_AID(scb)));
#endif /* DEBUG_PS_PACKETS */
        }
#ifdef WLTAF
        wlc_taf_txpkt_status(wlc->taf_handle, scb, TAF_PREC(prec), pkt,
            TAF_TXPKT_STATUS_SUPPRESSED_FREE);
#endif
        PKTFREE(wlc->osh, pkt, TRUE);
    } else {
        scb_psinfo->ps_queued++;
    }
}

/* PS TxModule enqueue function */
static void
wlc_apps_transmit_packet(void *ctx, struct scb *scb, void *pkt, uint prec)
{
    wlc_info_t *wlc = (wlc_info_t *)ctx;

    wlc_apps_enqueue_pkt(wlc, scb, pkt, prec);

#ifdef HNDPQP
    /* Page out normal PS queue */
    wlc_apps_scb_psq_prec_pqp_pgo(wlc, scb, prec);
#endif /* HNDPQP */
}

/* Try to ps enq the pkts on the txq */
static void
wlc_apps_txq_to_psq(wlc_info_t *wlc, struct scb *scb)
{
    wlc_apps_move_cpktq_to_psq(wlc, &(SCB_WLCIFP(scb)->qi->cpktq), scb);
}

#ifdef PROP_TXSTATUS
/* Try to ps enq the pkts on narq */
static void
wlc_apps_nar_txq_to_psq(wlc_info_t *wlc, struct scb *scb)
{
    struct pktq *txq = wlc_nar_txq(wlc->nar_handle, scb);
    if (txq) {
        wlc_apps_move_to_psq(wlc, txq, scb);
    }
}

/* This causes problems for PSPRETEND */
/* ps enq pkts on ampduq */
static void
wlc_apps_ampdu_txq_to_psq(wlc_info_t *wlc, struct scb *scb)
{
    if (AMPDU_ENAB(wlc->pub)) {
        struct pktq *txq = wlc_ampdu_txq(wlc->ampdu_tx, scb);
        if (txq) wlc_apps_move_to_psq(wlc, txq, scb);
    }
}

/** @param pktq   Multi-priority packet queue */
static void
wlc_apps_move_to_psq(wlc_info_t *wlc, struct pktq *pktq, struct scb* scb)
{
    void *head_pkt = NULL, *pkt;
    int prec;

#ifdef WLTDLS
    bool q_empty = TRUE;
    apps_wlc_psinfo_t *wlc_psinfo;
    struct apps_scb_psinfo *scb_psinfo;
#endif

    ASSERT(BSSCFG_AP(SCB_BSSCFG(scb)) || BSS_TDLS_BUFFER_STA(SCB_BSSCFG(scb)) ||
        BSSCFG_IBSS(SCB_BSSCFG(scb)));

    PKTQ_PREC_ITER(pktq, prec) {
        head_pkt = NULL;
        /* PS enq all the pkts we can */
        while (pktqprec_peek(pktq, prec) != head_pkt) {
            pkt = pktq_pdeq(pktq, prec);
            if (pkt == NULL) {
                /* txq could be emptied in wlc_apps_enqueue_pkt() */
                WL_ERROR(("WARNING: wl%d: %s NULL pkt\n", wlc->pub->unit,
                    __FUNCTION__));
                break;
            }
            if (scb != WLPKTTAGSCBGET(pkt)) {
                if (!head_pkt)
                    head_pkt = pkt;
                pktq_penq(pktq, prec, pkt);
                continue;
            }
            /* Enqueueing at the same prec may create a remote
             * possibility of suppressed pkts being reordered.
             * Needs to be investigated...
             */
            wlc_apps_enqueue_pkt(wlc, scb, pkt, prec);

#if defined(WLTDLS)
            if (TDLS_ENAB(wlc->pub) && wlc_tdls_isup(wlc->tdls))
                q_empty = FALSE;
#endif /* defined(WLTDLS) */
        }
    }

#if defined(WLTDLS)
    if (TDLS_ENAB(wlc->pub)) {
        ASSERT(wlc);
        wlc_psinfo = wlc->psinfo;

        ASSERT(scb);
        ASSERT(scb->bsscfg);
        scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
        ASSERT(scb_psinfo);
        if (!q_empty && !scb_psinfo->apsd_usp)
            wlc_tdls_send_pti(wlc->tdls, scb);
    }
#endif /* defined(WLTDLS) */
}
#endif /* PROP_TXSTATUS */

static void
wlc_apps_move_cpktq_to_psq(wlc_info_t *wlc, struct cpktq *cpktq, struct scb* scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    void *head_pkt = NULL, *pkt;
    int prec;
    struct pktq *txq = &cpktq->cq;        /**< multi-priority packet queue */
    struct pktq *psq;
#ifdef WLTDLS
    bool q_empty = TRUE;
#endif
    wlc_bsscfg_t *bsscfg;

    ASSERT(wlc);
    ASSERT(scb);

    bsscfg = SCB_BSSCFG(scb);
    ASSERT(bsscfg);

    ASSERT(BSSCFG_AP(bsscfg) || BSS_TDLS_BUFFER_STA(bsscfg) ||
        BSSCFG_IBSS(bsscfg));

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);
    psq = &scb_psinfo->psq;

    BCM_REFERENCE(bsscfg);
    BCM_REFERENCE(scb_psinfo);
    BCM_REFERENCE(psq);

#ifdef HNDPQP
    /* Previous PGI PS transition is not finish.
     * Reset the status and page-out the remain packets in pktq.
     */
    if (PQP_PGI_PS_TRANS(scb_psinfo)) {
        wlc_apps_scb_pqp_reset_ps_trans(wlc, scb);
    }
#endif /* HNDPQP */

    PKTQ_PREC_ITER(txq, prec) {
        head_pkt = NULL;
        /* PS enq all the pkts we can */
        while (pktqprec_peek(txq, prec) != head_pkt) {
            pkt = cpktq_pdeq(cpktq, prec);
            if (pkt == NULL) {
                /* txq could be emptied in wlc_apps_enqueue_pkt() */
                WL_ERROR(("WARNING: wl%d: %s NULL pkt\n", wlc->pub->unit,
                    __FUNCTION__));
                break;
            }
            if (scb != WLPKTTAGSCBGET(pkt)) {
                if (!head_pkt)
                    head_pkt = pkt;
                cpktq_penq(wlc, cpktq, prec, pkt, FALSE);
                continue;
            }
            /* Enqueueing at the same prec may create a remote
             * possibility of suppressed pkts being reordered.
             * Needs to be investigated...
             */
            wlc_apps_enqueue_pkt(wlc, scb, pkt, prec);

#if defined(WLTDLS)
            if (TDLS_ENAB(wlc->pub) && wlc_tdls_isup(wlc->tdls))
                q_empty = FALSE;
#endif /* defined(WLTDLS) */
        }

#ifdef HNDPQP
        /* After ps enq the packets from common txq.
         * If there are packets in this pktq and PQP owns this pktq.
         * To make sure the order of the packets,
         * Use prepend to page out normal PS queue.
         */
        if (pktqprec_n_pkts(psq, prec) && pqp_owns(&psq->q[prec])) {
            pktq_prec_pqp_pgo(psq, prec, PQP_PREPEND, scb_psinfo);
        }
#endif /* HNDPQP */
    }

#if defined(WLTDLS)
    if (TDLS_ENAB(wlc->pub)) {
        if (!q_empty && !scb_psinfo->apsd_usp)
            wlc_tdls_send_pti(wlc->tdls, scb);
    }
#endif /* defined(WLTDLS) */
}

#ifdef BCMPCIEDEV
static INLINE void
_wlc_apps_scb_psq_resp(wlc_info_t *wlc, struct scb *scb, struct apps_scb_psinfo *scb_psinfo)
{
    ASSERT(scb_psinfo);

    if (scb_psinfo->twt_active) {
        return;
    }

    /* Check if a previous PS POLL is waiting for a packet */
    if (SCB_PSPOLL_PKT_WAITING(scb_psinfo)) {
        /* Be sure to clear PS_PSP_ONRESP before calling wlc_apps_send_psp_response */
        scb_psinfo->psp_flags &= ~PS_PSP_ONRESP;
        wlc_apps_send_psp_response(wlc, scb, 0);
    }

    /* Check if waiting for packet from WMM PS trigger */
    if (scb_psinfo->apsd_hpkt_timer_on) {
#ifdef WLAMSDU_TX
        uint16 max_sf_frames = wlc_amsdu_scb_max_sframes(wlc->ami, scb);
#else
        uint16 max_sf_frames = 1;
#endif
        wl_del_timer(wlc->wl, scb_psinfo->apsd_hpkt_timer);

        if (!scb_psinfo->apsd_tx_pending && scb_psinfo->apsd_usp &&
            scb_psinfo->apsd_cnt) {

            /* Respond to WMM PS trigger */
            wlc_apps_apsd_send(wlc, scb);

            if (scb_psinfo->apsd_cnt > 1 &&
                wlc_apps_apsd_delv_count(wlc, scb) <=
                (WLC_APSD_DELV_CNT_LOW_WATERMARK * max_sf_frames)) {
                ac_bitmap_t ac_to_request;
                ac_to_request = scb->apsd.ac_delv & AC_BITMAP_ALL;

                /* Request packets from host flow ring */
                scb_psinfo->apsd_v2r_in_transit +=
                    wlc_sqs_psmode_pull_packets(wlc, scb,
                    WLC_ACBITMAP_TO_TIDBITMAP(ac_to_request),
                    (WLC_APSD_DELV_CNT_HIGH_WATERMARK * max_sf_frames));

                wl_add_timer(wlc->wl, scb_psinfo->apsd_hpkt_timer,
                    WLC_PS_APSD_HPKT_TIME, FALSE);
            } else
                scb_psinfo->apsd_hpkt_timer_on = FALSE;
        } else
            scb_psinfo->apsd_hpkt_timer_on = FALSE;

    }
}

static void
wlc_apps_scb_apsd_dec_v2r_in_transit(wlc_info_t *wlc, struct scb *scb, void *pkt, int prec)
{
    struct apps_scb_psinfo *scb_psinfo;
    int v2r_pkt_cnt;
    uint32 precbitmap;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    if (scb_psinfo->apsd_hpkt_timer_on && (scb_psinfo->apsd_v2r_in_transit > 0)) {
        /* Check bitmap and pkttag flag to make sure this packet is from pciedev layer.
         * And then decrease the counter of apsd_v2r_in_transit.
         */
        precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(scb->apsd.ac_delv);
        if (isset(&precbitmap, prec) && !(WLPKTTAG(pkt)->flags & WLF_TXHDR)) {
            if (WLPKTTAG_AMSDU(pkt)) {
                v2r_pkt_cnt = wlc_amsdu_msdu_cnt(wlc->osh, pkt);
            } else {
                v2r_pkt_cnt = 1;
            }
            scb_psinfo->apsd_v2r_in_transit -= v2r_pkt_cnt;
            ASSERT(scb_psinfo->apsd_v2r_in_transit >= 0);
        }
    }
}
#endif /* BCMPCIEDEV */

/* Set/clear PVB entry according to current state of power save queues */
void
wlc_apps_pvb_update(wlc_info_t *wlc, struct scb *scb)
{
    uint16 aid;
    struct apps_scb_psinfo *scb_psinfo;
    int ps_count;
    int pktq_total, pktq_ndelv_count;
    apps_bss_info_t *bss_info;

    ASSERT(wlc);
    ASSERT(scb);
    ASSERT(scb->bsscfg);
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL) {
        return;
    }

    if (scb_psinfo->flags & SCB_PS_FLUSH_IN_PROGR) {
        WL_PS_EX(scb, ("flush SCB in progress\n"));
        return;
    }

    bss_info = APPS_BSSCFG_CUBBY(wlc->psinfo, scb->bsscfg);
    if (bss_info == NULL) {
        return;
    }

    ps_count = 0;
    if ((SCB_PS(scb) || scb_psinfo->twt_active) && !SCB_DEL_IN_PROGRESS(scb)) {
        /* get available packet count for the given flow */
        pktq_total = wlc_apps_scb_pktq_tot(wlc, scb);
        pktq_ndelv_count = wlc_apps_apsd_ndelv_count(wlc, scb);

#ifdef BCMPCIEDEV
        /* Virtual packets */
        pktq_total += wlc_apps_sqs_vpkt_count(wlc, scb);
        /* If there are no packets in psq and virtual packets is zero.
         * Check apsd_v2r_in_transit for any packets which are not in psq.
         */
        if (pktq_total == 0)
            pktq_total = scb_psinfo->apsd_v2r_in_transit;
        pktq_ndelv_count += wlc_apps_apsd_ndelv_vpkt_count(wlc, scb);
#endif
        /* WMM/APSD 3.6.1.4: if no ACs are delivery-enabled (legacy), or all ACs are
         * delivery-enabled (special case), the PVB should indicate if any packet is
         * buffered.  Otherwise, the PVB should indicate if any packets are buffered
         * for non-delivery-enabled ACs only.
         */
        ps_count = ((scb->apsd.ac_delv == AC_BITMAP_NONE ||
            scb->apsd.ac_delv == AC_BITMAP_ALL) ?
            pktq_total : pktq_ndelv_count);

        /* When PSPretend is in probing mode, it sends probes (null data) to STA where
         * APPS is bypassed (PS_DONTQ). However it does expect that TIM bit is set. This
         * is normally true, since a frame got suppressed (which triggered PSPretend), but
         * this frame can get flushed, so the count is upped here when PSPretend is
         * probing to make sure TIM is set for this destination.
         */
        if (SCB_PS_PRETEND_PROBING(scb)) {
            ps_count++;
        }
    }

    aid = SCB_AID(scb);
    ASSERT(aid != 0);
    ASSERT((aid / NBBY) < ARRAYSIZE(bss_info->pvb));

    if (ps_count > 0) {
        if (scb_psinfo->in_pvb)
            return;
        WL_PS_EX(scb, ("setting AID %d scb:%p ps_count %d psqlen %d\n",
            aid, scb, ps_count, pktq_total));
        /* set the bit in the pvb */
        setbit(bss_info->pvb, aid);

        /* reset the aid range */
        if ((aid < bss_info->aid_lo) || !bss_info->aid_lo) {
            bss_info->aid_lo = aid;
        }
        if (aid > bss_info->aid_hi) {
            bss_info->aid_hi = aid;
        }

        scb_psinfo->in_pvb = TRUE;
        if (wlc->clk) {
            scb_psinfo->last_in_pvb_tsf = wlc_lifetime_now(wlc);
        }
    } else {
        if (!scb_psinfo->in_pvb) {
            return;
        }

        WL_PS_EX(scb, ("clearing AID %d scb:%p\n", aid, scb));
        /* clear the bit in the pvb */
        clrbit(bss_info->pvb, aid);

        /* reset the aid range */
        if (aid == bss_info->aid_hi) {
            /* find the next lowest aid value with PS pkts pending */
            for (aid = aid - 1; aid; aid--) {
                if (isset(bss_info->pvb, aid)) {
                    bss_info->aid_hi = aid;
                    break;
                }
            }
            /* no STAs with pending traffic ? */
            if (aid == 0) {
                bss_info->aid_hi = bss_info->aid_lo = 0;
            }
        } else if (aid == bss_info->aid_lo) {
            uint16 max_aid = wlc_ap_aid_max(wlc->ap);
            /* find the next highest aid value with PS pkts pending */
            for (aid = aid + 1; aid < max_aid; aid++) {
                if (isset(bss_info->pvb, aid)) {
                    bss_info->aid_lo = aid;
                    break;
                }
            }
            ASSERT(aid != max_aid);
        }

        scb_psinfo->in_pvb = FALSE;
        scb_psinfo->last_in_pvb_tsf = 0;
    }

    /* Update the PVB in the bcn template */
    WL_APSTA_BCN(("wl%d: %s -> wlc_bss_update_beacon\n", wlc->pub->unit, __FUNCTION__));

    wlc_bss_update_beacon(wlc, scb->bsscfg, TRUE);

}

/* Increment the TBTT count for all PS SCBs */
void
wlc_apps_tbtt_update(wlc_info_t *wlc)
{
    int idx;
    wlc_bsscfg_t *cfg;
    scb_t *scb;
    scb_iter_t scbiter;
    apps_scb_psinfo_t *scb_psinfo;

    /* If touching all the PS scbs multiple times is too inefficient
     * then we can restore the old code and have all scbs updated in one pass.
     */

    FOREACH_UP_AP(wlc, idx, cfg) {
        FOREACH_BSS_SCB(wlc->scbstate, &scbiter, cfg, scb) {
            if (!scb->permanent && SCB_PS(scb)) {
                scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
                /* do not wrap around */
                if (scb_psinfo->tbtt < 0xFFFF) {
                    scb_psinfo->tbtt++;
                }
            }
        }
    }
}

/* return the count of PS buffered pkts for an SCB */
int
wlc_apps_psq_len(wlc_info_t *wlc, struct scb *scb)
{
    int pktq_total;
    struct apps_scb_psinfo *scb_psinfo;

    if (!wlc || !scb) {
        return 0;
    }

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo) {
        return 0;
    }

    pktq_total = pktq_n_pkts_tot(&scb_psinfo->psq);
#ifdef WL_USE_SUPR_PSQ
    pktq_total += scb_psinfo->suppressed_pkts;
#endif /* WL_USE_SUPR_PSQ */

    return pktq_total;
}

/* return the count of PS buffered pkts for an SCB which are delivery-enabled ACs. */
int
wlc_apps_psq_delv_count(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint32 precbitmap;
    int delv_count;

    if (scb->apsd.ac_delv == AC_BITMAP_NONE)
        return 0;

    precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(scb->apsd.ac_delv);

    /* PS buffered pkts are on the scb_psinfo->psq */
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    delv_count = APPS_PKTQ_MLEN(&scb_psinfo->psq, precbitmap);

    return delv_count;
}

/* return the count of PS buffered pkts for an SCB which are non-delivery-enabled ACs. */
int
wlc_apps_psq_ndelv_count(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    ac_bitmap_t ac_non_delv;
    uint32 precbitmap;
    int count;

    if (scb->apsd.ac_delv == AC_BITMAP_ALL)
        return 0;

    ac_non_delv = ~scb->apsd.ac_delv & AC_BITMAP_ALL;
    precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(ac_non_delv);

    /* PS buffered pkts are on the scb_psinfo->psq */
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    count = APPS_PKTQ_MLEN(&scb_psinfo->psq, precbitmap);

    return count;
}

/* called from bmac when a PS state switch is detected from the transmitter.
 * On PS ON switch, directly call wlc_apps_scb_ps_on();
 *  On PS OFF, check if there are tx packets pending. If so, make a PS OFF reservation
 *  and wait for the drain. Otherwise, switch to PS OFF.
 *  Sends a message to the bmac pmq manager to signal that we detected this switch.
 *  PMQ manager will delete entries when switch states are in sync and the queue is drained.
 *  return 1 if a switch occured. This allows the caller to invalidate
 *  the header cache.
 */
void BCMFASTPATH
wlc_apps_process_ps_switch(wlc_info_t *wlc, struct scb *scb, uint8 ps_on)
{
    struct apps_scb_psinfo *scb_psinfo;
    bool ps_switch_da_sat = (ps_on & PS_SWITCH_DISASSOC_DEAUTH) ? TRUE : FALSE;

    /* only process ps transitions for associated sta's, IBSS bsscfg and WDS peers */
    if (!(SCB_ASSOCIATED(scb) || SCB_IS_IBSS_PEER(scb) || SCB_WDS(scb)) || ps_switch_da_sat) {

        if (!wlc->ps_scb_txfifo_blk) {
            wlc_pmq_process_switch(wlc, scb, PMQ_CLEAR_ALL);
        } else {
            /* Client's ps state should not be updated for disassoc/deauth frames.
             * ps state change will be handled in scb cleanup path from
             * deauth/disassoc frame processing after confirming that frame is
             * coming from legitimate client.
             */
            if (!ps_switch_da_sat) {
                wlc_pmq_process_switch(wlc, scb, PMQ_REMOVE);
            }
        }
        return;
    }

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (scb_psinfo == NULL)
        return;

    if (ps_on) {
        if ((ps_on & PS_SWITCH_PMQ_SUPPR_PKT)) {
            WL_PS_EX(scb, ("(0x%02x) Req by suppr pkt!\n", ps_on));
            scb_psinfo->ps_trans_status |= SCB_PS_TRANS_OFF_BLOCKED;
        } else if ((ps_on & (PS_SWITCH_PMQ_ENTRY | PS_SWITCH_OMI))) {
            if (!SCB_PS(scb)) {
                WL_PS_EX(scb, ("(0x%02x) Actual PMQ entry processing\n", ps_on));
            }
            /* This PS ON request is from actual PMQ entry addition. */
            scb_psinfo->ps_trans_status &= ~SCB_PS_TRANS_OFF_BLOCKED;
        }
        if (!SCB_PS(scb)) {
#ifdef GAME_SPEEDUP
           WLCNTSCBINCR(scb->scb_stats.ps_cnt);
#endif
#ifdef PSPRETEND
            /* reset pretend status */
            scb->ps_pretend &= ~PS_PRETEND_ON;
            if (PSPRETEND_ENAB(wlc->pub) &&
                (ps_on & PS_SWITCH_PMQ_PSPRETEND) && !SCB_ISMULTI(scb)) {
                wlc_pspretend_on(wlc->pps_info, scb, PS_PRETEND_ACTIVE_PMQ);
            }
            else
#endif /* PSPRETEND */
            {
                wlc_apps_scb_ps_on(wlc, scb);
            }

            WL_PS_EX(scb, ("(0x%02x) - PS on (%d), pretend mode %s\n", ps_on,
                SCB_PS(scb), (scb->ps_pretend & PS_PRETEND_ACTIVE) ? "on":"off"));
        }
#ifdef PSPRETEND
        else if (SCB_PS_PRETEND(scb) && (ps_on & PS_SWITCH_PMQ_PSPRETEND)) {
            WL_PS_EX(scb, ("(0x%02x) PS pretend was already active now "
                "with new PMQ PPS entry\n", ps_on));
            scb->ps_pretend |= PS_PRETEND_ACTIVE_PMQ;
        }
#endif /* PSPRETEND */
        else {
#ifdef PSPRETEND
            if (SCB_PS_PRETEND(scb) &&
                (ps_on & (PS_SWITCH_PMQ_ENTRY | PS_SWITCH_OMI))) {
                if (wlc->pps_info != NULL) {
                    wlc_pspretend_scb_ps_off(wlc->pps_info, scb);
                }
            }
#endif /* PSPRETEND */
            /* STA is already in PS, clear PS OFF pending bit only */
            scb_psinfo->ps_trans_status &= ~SCB_PS_TRANS_OFF_PEND;
        }
    } else if ((scb_psinfo->ps_trans_status & SCB_PS_TRANS_OFF_BLOCKED)) {
        WL_PS_EX(scb, ("PS off attempt is blocked by WAITPMQ\n"));
    } else if (scb_psinfo->ps_requester) {
        WL_PS_EX(scb, ("PS off attempt is blocked by another "
            "source. 0x%x\n", scb_psinfo->ps_requester));
    } else if (SCB_PS(scb)) {
        /* Do we need to prevent ON -> OFF transitions while data fifo is blocked.
         * We need to finish flushing HW and reque'ing before we can allow the STA
         * to come out of PS.
         */
        if ((wlc->block_datafifo & DATA_BLOCK_PS) ||
#if defined(WL_PS_SCB_TXFIFO_BLK)
            SCB_PS_TXFIFO_BLK(scb) ||
#endif /* WL_PS_SCB_TXFIFO_BLK */
            FALSE) {
            WL_PS_EX(scb, ("SCB Data Block (%d/%d/%d/%d/%d)%s\n",
                SCB_PS(scb), SCB_TWTPS(scb), wlc->ps_txfifo_blk_scb_cnt,
                SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb), TXPKTPENDTOT(wlc),
                SCB_PS_PRETEND(scb) ? " (ps pretend active)" : ""));
            scb_psinfo->ps_trans_status |= SCB_PS_TRANS_OFF_PEND;
        }
#ifdef PSPRETEND
        else if (SCB_PS_PRETEND_BLOCKED(scb) &&
            !(scb_psinfo->ps_trans_status & SCB_PS_TRANS_OFF_PEND)) {
            /* Prevent ON -> OFF transitions if we were expecting to have
             * seen a PMQ entry for ps pretend and we have not had it yet.
             * This is to ensure that when that entry does come later, it
             * does not cause us to enter ps pretend mode when that condition
             * should have been cleared
             */
            WL_PS_EX(scb, ("ps pretend pending off waiting for "
                "the PPS ON PMQ entry\n"));
            scb_psinfo->ps_trans_status |= SCB_PS_TRANS_OFF_PEND;
        }
#endif /* PSPRETEND */
        else {
#ifdef PSPRETEND
            if (SCB_PS_PRETEND_BLOCKED(scb)) {
                WL_PS_EX(scb, ("ps pretend pending off waiting "
                    "for the PPS ON PMQ, but received PPS OFF PMQ more than "
                    "once. Consider PPS ON PMQ as lost\n"));
            }
#endif /* PSPRETEND */
            /* Once TWT is active, ignore late PMQ off events. PM is forced to PS,
             * and it should stay there. TWT will take control of PM.
             */
            if (!scb_psinfo->twt_active) {
    /* dump_flag_qqdx */
#ifdef dump_stack_ps_qqdx_print
    printk(KERN_ALERT"----------[fyl] wlc_apps_process_ps_switch (%u)wlc_apps_scb_ps_off----------",OSL_SYSUPTIME());
#endif /*dump_stack_ps_qqdx_print*/
                wlc_apps_scb_ps_off(wlc, scb, FALSE);
            } else {
                WL_TWT(("wl%d.%d: %s: Block PS OFF %02x "MACF" (%d/%d/%d/%d/%d)\n",
                    wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)),
                    __FUNCTION__, ps_on, ETHER_TO_MACF(scb->ea),
                    scb_psinfo->twt_active, SCB_PS(scb),
                    SCB_TWTPS(scb), scb_psinfo->twt_wait4drain_enter,
                    SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)));
            }
        }
    }

    /* When there is no more data to suppress remove all PMQ entries */
    if (!wlc->ps_scb_txfifo_blk) {
        wlc_pmq_process_switch(wlc, scb, PMQ_CLEAR_ALL);
    }

    return;

}

/* wlc_apps_pspoll_resp_prepare()
 * Do some final pkt touch up before DMA ring for PS delivered pkts.
 * Also track pspoll response state (pkt callback and more_data signaled)
 */
void
wlc_apps_pspoll_resp_prepare(wlc_info_t *wlc, scb_t *scb, void *pkt, struct dot11_header *h,
    bool last_frag)
{
    ASSERT(scb);
    ASSERT(SCB_PS(scb));

    /*
     * FC_MOREDATA is set for every response packet being sent while STA is in PS.
     * This forces STA to send just one more PS-Poll.  If by that time we actually
     * have more data, it'll be sent, else a Null data frame without FC_MOREDATA will
     * be sent.  This technique often improves TCP/IP performance.  The last NULL Data
     * frame is sent with the WLF_PSDONTQ flag.
     */

    h->fc |= htol16(FC_MOREDATA);

    /* Register pkt callback for PS-Poll response */
    if (last_frag && !SCB_ISMULTI(scb)) {
        WLF2_PCB2_REG(pkt, WLF2_PCB2_PSP_RSP);
    }
}

/* Fix PDU that is being sent as a PS-Poll response or APSD delivery frame. */
void
wlc_apps_ps_prep_mpdu(wlc_info_t *wlc, void *pkt)
{
    bool last_frag;
    struct dot11_header *h;
    uint16    macCtlLow, frameid;
    struct scb *scb;
    wlc_bsscfg_t *bsscfg;
    wlc_key_info_t key_info;
    wlc_txh_info_t txh_info;

    scb = WLPKTTAGSCBGET(pkt);

    wlc_get_txh_info(wlc, pkt, &txh_info);
    h = txh_info.d11HdrPtr;

    WL_PS_EX(scb, ("pkt %p flags 0x%x flags2 %x fc 0x%x\n",
        OSL_OBFUSCATE_BUF(pkt), WLPKTTAG(pkt)->flags, WLPKTTAG(pkt)->flags2, h->fc));

    /*
     * Set the IGNOREPMQ bit.
     *
     * PS bcast/mcast pkts have following differences from ucast:
     *    1. use the BCMC fifo
     *    2. FC_MOREDATA is set by ucode (except for the kludge)
     *    3. Don't set IGNOREPMQ bit as ucode ignores PMQ when draining
     *       during DTIM, and looks at PMQ when draining through
     *       MHF2_TXBCMC_NOW
     */
    if (ETHER_ISMULTI(txh_info.TxFrameRA)) {
        ASSERT(!SCB_WDS(scb));

        /* Kludge required from wlc_dofrag */
        bsscfg = SCB_BSSCFG(scb);

        ASSERT(!SCB_A4_DATA(scb));

        /* Be sure to update sequence number of frame id, if this was a suppressed pkt */
        if (WLPKTTAG(pkt)->flags & WLF_TXHDR) {
            frameid = wlc_compute_frameid(wlc, txh_info.TxFrameID,
                D11_TXFID_GET_FIFO(wlc, txh_info.TxFrameID));
            txh_info.TxFrameID = htol16(frameid);
            wlc_set_txh_frameid(wlc, pkt, frameid);
        }

        /* APSTA: MUST USE BSS AUTH DUE TO SINGLE BCMC SCB; IS THIS OK? */
        wlc_keymgmt_get_bss_tx_key(wlc->keymgmt, bsscfg, FALSE, &key_info);

        if (!bcmwpa_is_wpa_auth(bsscfg->WPA_auth) || key_info.algo != CRYPTO_ALGO_AES_CCM)
            h->fc |= htol16(FC_MOREDATA);
    }
    else if (!SCB_ISMULTI(scb)) {
        /* There is a hack to send uni-cast P2P_PROBE_RESP frames using bsscfg's
        * mcast scb because of no uni-cast scb is available for bsscfg, we need to exclude
        * such hacked packtes from uni-cast processing.
        */
        last_frag = (ltoh16(h->fc) & FC_MOREFRAG) == 0;
        /* Set IGNOREPMQ bit (otherwise, it may be suppressed again) */
        macCtlLow = ltoh16(txh_info.MacTxControlLow);
        if (D11REV_GE(wlc->pub->corerev, 40)) {
            d11txhdr_t* txh = txh_info.hdrPtr;
            macCtlLow |= D11AC_TXC_IPMQ;
            *D11_TXH_GET_MACLOW_PTR(wlc, txh) = htol16(macCtlLow);
#ifdef PSPRETEND
            if (PSPRETEND_ENAB(wlc->pub)) {
                uint16 macCtlHigh = ltoh16(txh_info.MacTxControlHigh);
                macCtlHigh &= ~D11AC_TXC_PPS;
                *D11_TXH_GET_MACHIGH_PTR(wlc, txh) = htol16(macCtlHigh);
            }
#endif /* PSPRETEND */
        } else {
            d11txh_pre40_t* nonVHTHdr = &(txh_info.hdrPtr->pre40);
            macCtlLow |= TXC_IGNOREPMQ;
            nonVHTHdr->MacTxControlLow = htol16(macCtlLow);
        }

        /*
         * Set FC_MOREDATA and EOSP bit and register callback.  WLF_APSD is set
         * for all APSD delivery frames.  WLF_PSDONTQ is set only for the final
         * Null frame of a series of PS-Poll responses.
         */
        if (WLPKTTAG(pkt)->flags & WLF_APSD)
            wlc_apps_apsd_prepare(wlc, scb, pkt, h, last_frag);
        else if (!(WLPKTTAG(pkt)->flags & WLF_PSDONTQ))
            wlc_apps_pspoll_resp_prepare(wlc, scb, pkt, h, last_frag);
    }
}

/* Packet callback fn for WLF2_PCB2_PSP_RSP
 *
 */
static void
wlc_apps_psp_resp_complete(wlc_info_t *wlc, void *pkt, uint txs)
{
    struct scb *scb;
    struct apps_scb_psinfo *scb_psinfo;
#ifdef WLTAF
    bool taf_in_use = wlc_taf_in_use(wlc->taf_handle);
#endif

    BCM_REFERENCE(txs);

    /* Is this scb still around */
    if ((scb = WLPKTTAGSCBGET(pkt)) == NULL)
        return;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL)
        return;

#ifdef WLTAF
    if (taf_in_use && (wlc_apps_apsd_ndelv_count(wlc, scb) > 0)) {
        /* If psq is empty, release new packets from ampdu or nar */
        if ((wlc_apps_psq_ndelv_count(wlc, scb) == 0) &&
            !wlc_taf_scheduler_blocked(wlc->taf_handle)) {
            /* Trigger a new TAF schedule cycle */
            wlc_taf_schedule(wlc->taf_handle, PKTPRIO(pkt), scb, FALSE);
        }
    }
#endif

    /* clear multiple ps-poll frame protection */
    scb_psinfo->psp_flags &= ~PS_PSP_ONRESP;

    /* Check if the PVB entry needs to be cleared */
    if (scb_psinfo->in_pvb) {
        wlc_apps_pvb_update(wlc, scb);
    }
}

static int
wlc_apps_send_psp_response_cb(wlc_info_t *wlc, wlc_bsscfg_t *cfg, void *pkt, void *data)
{
    BCM_REFERENCE(wlc);
    BCM_REFERENCE(cfg);
    BCM_REFERENCE(data);

    /* register packet callback */
    WLF2_PCB2_REG(pkt, WLF2_PCB2_PSP_RSP);
    return BCME_OK;
}

/* wlc_apps_send_psp_response()
 *
 * This function is used in rx path when we get a PSPoll.
 * Also used for proptxstatus when a tx pkt is queued to the driver and
 * SCB_PROPTXTSTATUS_PKTWAITING() was set (pspoll happend, but no pkts local).
 */
void
wlc_apps_send_psp_response(wlc_info_t *wlc, struct scb *scb, uint16 fc)
{
    struct apps_scb_psinfo *scb_psinfo;
    ac_bitmap_t ac_non_delv;
    uint32 precbitmap;
    int pktq_total;
    int err;

    ASSERT(scb);
    ASSERT(wlc);

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    /* Ignore trigger frames received during tx block period */
    if (scb_psinfo->tx_block != 0) {
        WL_PS_EX(scb, ("tx blocked; ignoring PS poll\n"));

        return;
    }

    if (scb_psinfo->twt_active) {
        return;
    }

    /* get available packet count for the given flow */
    pktq_total = wlc_apps_scb_pktq_tot(wlc, scb);

    /* enable multiple ps-poll frame check */
    if (scb_psinfo->psp_flags & PS_PSP_ONRESP) {
        WL_PS_EX(scb, ("previous ps-poll frame under handling. Drop new ps-poll frame\n"));

        return;
    }
    scb_psinfo->psp_flags |= PS_PSP_ONRESP;

#ifdef BCMPCIEDEV
    /* Check for real packets and transient packets ; If not request from Host */
    if ((pktq_total == 0) && (wlc_sqs_v2r_pkts_tot(scb) == 0) &&
        wlc_sqs_vpkts_tot(scb)) {
        /* Request packet from host flow ring */
        wlc_sqs_psmode_pull_packets(wlc, scb,
            WLC_ACBITMAP_TO_TIDBITMAP(AC_BITMAP_ALL), 1);

        SCB_PSPOLL_PKT_WAITING(scb_psinfo) = TRUE;

        WL_PS_EX(scb, ("Await host packet (%d/%d/%d) (0x%x)\n",
            pktq_total, scb_psinfo->suppressed_pkts, pktq_n_pkts_tot(&scb_psinfo->psq),
            scb->apsd.ac_delv));

        return;
    }

    SCB_PSPOLL_PKT_WAITING(scb_psinfo) = FALSE;
#endif /* BCMPCIEDEV */

    WL_PS_EX(scb, ("\n"));

    /*
     * Send a null data frame if there are no PS buffered
     * frames on APSD non-delivery-enabled ACs (WMM/APSD 3.6.1.6).
     */
    if (pktq_total == 0 || ((scb->apsd.ac_delv != AC_BITMAP_ALL) &&
        (scb->apsd.ac_delv != AC_BITMAP_NONE) &&
        (wlc_apps_apsd_ndelv_count(wlc, scb) == 0) &&
#ifdef BCMPCIEDEV
        (wlc_apps_apsd_ndelv_vpkt_count(wlc, scb) == 0) &&
#endif
        TRUE)) {

        goto reply_with_nulldata;
    }

    /* Check whether there are any legacy frames before sending
     * any delivery enabled frames
     */
    if ((scb->apsd.ac_delv != AC_BITMAP_ALL) && (wlc_apps_apsd_delv_count(wlc, scb) > 0)) {
        ac_non_delv = ~scb->apsd.ac_delv & AC_BITMAP_ALL;
        precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(ac_non_delv);
    } else {
        /* If all ACs are delivery enabled then limit the release to 1 packet */
        if ((scb->apsd.ac_delv == AC_BITMAP_ALL) && (scb_psinfo->apsd_cnt == 0)) {
            scb_psinfo->apsd_cnt = 1;
        }
        ac_non_delv = AC_BITMAP_ALL;
        precbitmap = WLC_PREC_BMP_ALL;
    }

    if (!APPS_PS_SEND(wlc, scb, precbitmap, FALSE, NULL)) {
        err = BCME_ERROR;
#ifdef WLCFP
        if (CFP_ENAB(wlc->pub) && SCB_AMPDU(scb)) {
            if (wlc_cfp_ampdu_ps_send(wlc, scb,
                WLC_ACBITMAP_TO_TIDBITMAP(ac_non_delv), 0)) {
                err = BCME_OK;
            }
        }
#endif /* WLCFP */
        if (err) {
            WL_ERROR(("wl%d: %s PS-Poll without data to respond "
                "(%d/%d/%d) (0x%x)\n", wlc->pub->unit, __FUNCTION__,
                pktq_total, scb_psinfo->suppressed_pkts,
                pktq_n_pkts_tot(&scb_psinfo->psq), scb->apsd.ac_delv));
            goto reply_with_nulldata;
        }
    }
    return;

reply_with_nulldata:
    /* Ensure pkt is not queued on psq */
    if (wlc_sendnulldata(wlc, scb->bsscfg, &scb->ea, 0, WLF_PSDONTQ, PRIO_8021D_BE,
        wlc_apps_send_psp_response_cb, NULL) == FALSE) {
        WL_ERROR(("wl%d: %s PS-Poll null data response failed\n",
            wlc->pub->unit, __FUNCTION__));
        scb_psinfo->psp_flags &= ~PS_PSP_ONRESP;
    } else {
        WL_PS_EX(scb, ("Reply with NULL frame (%d/%d/%d) (0x%x)\n",
            pktq_total, scb_psinfo->suppressed_pkts, pktq_n_pkts_tot(&scb_psinfo->psq),
            scb->apsd.ac_delv));
    }
}

/* get PVB info */
static INLINE void
wlc_apps_tim_pvb(wlc_info_t *wlc, wlc_bsscfg_t *cfg, uint8 *offset, int16 *length)
{
    apps_bss_info_t *bss_psinfo;
    uint8 n1 = 0, n2;

    bss_psinfo = APPS_BSSCFG_CUBBY(wlc->psinfo, cfg);

#ifdef WL_MBSSID
    if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype) && BSSCFG_IS_MAIN_AP(cfg)) {
        wlc_bsscfg_t *bsscfg;
        apps_bss_info_t *bsscfg_psinfo;
        int i;
        uint16 aid_lo, aid_hi;
        uint16 max_bss_count;

        max_bss_count = wlc_ap_get_maxbss_count(wlc->ap);

        aid_lo = (uint8)bss_psinfo->aid_lo;
        aid_hi = (uint8)bss_psinfo->aid_hi;
        FOREACH_UP_AP(wlc, i, bsscfg) {
            if (bsscfg != cfg) {
                bsscfg_psinfo = APPS_BSSCFG_CUBBY(wlc->psinfo, bsscfg);
                if (!aid_lo || ((bsscfg_psinfo->aid_lo < aid_lo) &&
                        bsscfg_psinfo->aid_lo)) {
                    aid_lo = bsscfg_psinfo->aid_lo;
                }
                if (bsscfg_psinfo->aid_hi > aid_hi) {
                    aid_hi = bsscfg_psinfo->aid_hi;
                }
            }
        }

        if (aid_lo) {
            n1 = (uint8)((aid_lo - max_bss_count)/8);
        }
        /* n1 must be highest even number */
        n1 &= ~1;
        n2 = (uint8)(aid_hi/8);
        /* offset is set to zero support, to support (non mbssid support)stations */
        if (!wlc_ap_get_block_mbssid(wlc)) {
            n1 = 0;
        }
    } else
#endif /* WL_MBSSID */
    {
        n1 = (uint8)(bss_psinfo->aid_lo/8);
        /* n1 must be highest even number */
        n1 &= ~1;
        n2 = (uint8)(bss_psinfo->aid_hi/8);
    }

    *offset = n1;
    *length = n2 - n1 + 1;

    ASSERT(*offset <= 127);
    ASSERT(*length >= 1 && *length <= sizeof(bss_psinfo->pvb));
}

/* calculate TIM IE length */
static uint
wlc_apps_tim_len(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
    uint8 offset;
    int16 length;
#ifdef WL_MBSSID
    uint16 max_bss_count;
    max_bss_count = wlc_ap_get_maxbss_count(wlc->ap);
#endif /* WL_MBSSID */

    ASSERT(cfg != NULL);
    ASSERT(BSSCFG_AP(cfg));

    wlc_apps_tim_pvb(wlc, cfg, &offset, &length);

#ifdef WL_MBSSID
    if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype)) {
        return TLV_HDR_LEN + DOT11_MNG_TIM_FIXED_LEN + length + (max_bss_count/8);
    } else {
        return TLV_HDR_LEN + DOT11_MNG_TIM_FIXED_LEN + length;
    }
#else
    return TLV_HDR_LEN + DOT11_MNG_TIM_FIXED_LEN + length;
#endif /* WL_MBSSID */
}

/* Fill in the TIM element for the specified bsscfg */
static int
wlc_apps_tim_create(wlc_info_t *wlc, wlc_bsscfg_t *cfg, uint8 *buf, uint len)
{
    apps_bss_info_t *bss_psinfo;
    uint8 offset;
    int16 length;
    wlc_bss_info_t *current_bss;
#ifdef WL_MBSSID
    uint16 max_bss_cnt;
    uint16 bcmc_bytes;
#endif /* WL_MBSSID */
    ASSERT(cfg != NULL);
    ASSERT(BSSCFG_AP(cfg));
    ASSERT(buf != NULL);

#ifdef WL_MBSSID
    max_bss_cnt = wlc_ap_get_maxbss_count(wlc->ap);
    bcmc_bytes = max_bss_cnt >= 8 ? max_bss_cnt/8 : 1;
#endif /* WL_MBSSID */
    /* perform length check to make sure tim buffer is big enough */
    if (wlc_apps_tim_len(wlc, cfg) > len)
        return BCME_BUFTOOSHORT;

    current_bss = cfg->current_bss;

    wlc_apps_tim_pvb(wlc, cfg, &offset, &length);

    buf[0] = DOT11_MNG_TIM_ID;
    /* set the length of the TIM */
#ifdef WL_MBSSID
    if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype)) {
        buf[1] = (uint8)(DOT11_MNG_TIM_FIXED_LEN + length + bcmc_bytes);
    } else {
        buf[1] = (uint8)(DOT11_MNG_TIM_FIXED_LEN + length);
    }
#else
    buf[1] = (uint8)(DOT11_MNG_TIM_FIXED_LEN + length);
#endif /* WL_MBSSID */
    buf[2] = (uint8)(current_bss->dtim_period - 1);
    buf[3] = (uint8)current_bss->dtim_period;
    /* set the offset field of the TIM */
    buf[4] = offset;
    /* copy the PVB into the TIM */
    bss_psinfo = APPS_BSSCFG_CUBBY(wlc->psinfo, cfg);
#ifndef WL_MBSSID
    bcopy(&bss_psinfo->pvb[offset], &buf[DOT11_MNG_TIM_NON_PVB_LEN], length);
#else
    if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype)) {
        /* copy bc/mc bits corresponding to all mbss */
        bcopy(&bss_psinfo->pvb[0], &buf[DOT11_MNG_TIM_NON_PVB_LEN], bcmc_bytes);
        /* copy the sta's traffic bits of primary bss */
        bcopy(&bss_psinfo->pvb[bcmc_bytes + offset],
            &buf[DOT11_MNG_TIM_NON_PVB_LEN + bcmc_bytes], length);
    } else {
        bcopy(&bss_psinfo->pvb[offset],    &buf[DOT11_MNG_TIM_NON_PVB_LEN], length);
    }
    if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype) && BSSCFG_IS_MAIN_AP(cfg)) {
        wlc_bsscfg_t *bsscfg;
        apps_bss_info_t *bsscfg_psinfo;
        int i, j;

        FOREACH_UP_AP(wlc, i, bsscfg) {
            if (bsscfg != cfg) {
                bsscfg_psinfo = APPS_BSSCFG_CUBBY(wlc->psinfo, bsscfg);
                for (j = 0; j < length; j++) {
                    buf[DOT11_MNG_TIM_NON_PVB_LEN + j + bcmc_bytes]
                        |= bsscfg_psinfo->pvb[bcmc_bytes + offset + j];
                }
            }
        }
    }
#endif /* WL_MBSSID */
    return BCME_OK;
}

/* wlc_apps_scb_supr_enq()
 *
 * Re-enqueue a suppressed frame.
 *
 * This fn is similar to wlc_apps_suppr_frame_enq(), except:
 *   - handles only unicast SCB traffic
 *   - SCB is Associated and in PS
 *   - Handles the suppression of PSPoll response. wlc_apps_suppr_frame does not handle
 *     these because they would not be suppressed due to PMQ suppression since PSP response
 *     are queued while STA is PS, so ignore PMQ is set.
 *
 * Called from:
 *  wlc.c:wlc_pkt_abs_supr_enq()
 *    <- wlc_dotxstatus()
 *    <- wlc_ampdu_dotxstatus_aqm_complete()
 *    <- wlc_ampdu_dotxstatus_complete() (non-AQM)
 *
 *  wlc_ampdu_suppr_pktretry()
 *   Which is called rom wlc_ampdu_dotxstatus_aqm_complete(),
 *   similar to TX_STATUS_SUPR_PPS (Pretend PS) case in wlc_dotxstatus()
 */
bool
wlc_apps_scb_supr_enq(wlc_info_t *wlc, struct scb *scb, void *pkt)
{
    struct apps_scb_psinfo *scb_psinfo;
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;

    ASSERT(scb != NULL);
    ASSERT(SCB_PS(scb));
    ASSERT(!SCB_ISMULTI(scb));
    ASSERT((SCB_ASSOCIATED(scb)) || (SCB_WDS(scb) != NULL));
    ASSERT(pkt != NULL);

    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);

    if (WLF2_PCB2(pkt) == WLF2_PCB2_PSP_RSP) {
        /* This packet was the ps-poll response. Clear multiple ps-poll frame protection. */
        scb_psinfo->psp_flags &= ~PS_PSP_ONRESP;
    }

    /* unregister pkt callback */
    WLF2_PCB2_UNREG(pkt);

    /* PR56242: tag the pkt so that we can identify them later and move them
     * to the front when tx fifo drain/flush finishes.
     */
    WLPKTTAG(pkt)->flags3 |= WLF3_SUPR;

    /* Mark as retrieved from HW FIFO */
    WLPKTTAG(pkt)->flags |= WLF_FIFOPKT;

    WL_PS_EX(scb, ("SUPPRESSED packet %p PS:%d \n", pkt, SCB_PS(scb)));

    /* If enqueue to psq successfully, return FALSE so that PDU is not freed */
    /* Enqueue at higher precedence as these are suppressed packets */
    if (wlc_apps_psq(wlc, pkt, WLC_PRIO_TO_HI_PREC(PKTPRIO(pkt)))) {
        WLPKTTAG(pkt)->flags &= ~WLF_APSD;
        return FALSE;
    }

    /* XXX The error message and accounting here is not really correct---f wlc_apps_psq() fails,
     * it is not always because a pkt was dropped.  In PropTxStatus, pkts are not enqueued if
     * the are host pkts, but they are suppressed back to host.  The drop accounting should only
     * be for pktq len limit, psq_pkts_hi high water mark, or SCB no longer associated.
     */
    WL_ERROR(("wl%d.%d: %s ps suppr pkt discarded\n", wlc->pub->unit,
        WLC_BSSCFG_IDX(SCB_BSSCFG(scb)), __FUNCTION__));
    wlc_psinfo->ps_discard++;
    scb_psinfo->ps_discard++;
#ifdef WLTAF
    wlc_taf_txpkt_status(wlc->taf_handle, scb, PKTPRIO(pkt), pkt,
        TAF_TXPKT_STATUS_SUPPRESSED_FREE);
#endif

    return TRUE;
}

/* wlc_apps_suppr_frame_enq()
 *
 * Enqueue a suppressed PDU to psq after fixing up the PDU
 *
 * Called from
 * wlc_dotxstatus():
 *   -> supr_status == TX_STATUS_SUPR_PMQ (PS)
 *   -> supr_status == TX_STATUS_SUPR_PPS (Pretend PS)
 *
 * wlc_ampdu_dotxstatus_aqm_complete()
 *   -> supr_status == TX_STATUS_SUPR_PMQ (PS)
 *
 * wlc_ampdu_dotxstatus_complete() (non-AQM)
 *   -> supr_status == TX_STATUS_SUPR_PMQ (PS)
 *
 * wlc_ampdu_suppr_pktretry()
 *   Which is called from wlc_ampdu_dotxstatus_aqm_complete(),
 *   similar to TX_STATUS_SUPR_PPS (Pretend PS) case in wlc_dotxstatus()
 */
bool
wlc_apps_suppr_frame_enq(wlc_info_t *wlc, void *pkt, tx_status_t *txs, bool last_frag)
{
    uint16 frag = 0;
    uint16 txcnt;
    uint16 seq_num = 0;
    struct scb *scb = WLPKTTAGSCBGET(pkt);
    struct apps_scb_psinfo *scb_psinfo;
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    struct dot11_header *h;
    uint16 txc_hwseq;
    wlc_txh_info_t txh_info;
    bool control;
    bool scb_put_in_ps = FALSE;

    BCM_REFERENCE(scb_put_in_ps);

    ASSERT(scb != NULL);

    BCM_REFERENCE(scb_put_in_ps);

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL)
        return TRUE;

    if ((WLPKTTAG(pkt)->flags & WLF_PSDONTQ) ||
        (!SCB_ISMULTI(scb) && !SCB_ASSOCIATED(scb)) ||
        (SCB_DEL_IN_PROGRESS(scb)) ||
#ifdef MFP
        /* Skip enqueue and SCB state transition for a suppressed SA Query frame. */
        (WLF2_PCB1(pkt) & WLF2_PCB1_MFP_SA) ||
#endif /* MFP */
#ifdef PROP_TXSTATUS
        /* in proptxstatus, the host will resend these suppressed packets */
        (PROP_TXSTATUS_ENAB(wlc->pub) && HOST_PROPTXSTATUS_ACTIVATED(wlc) &&
        (WL_TXSTATUS_GET_FLAGS(WLPKTTAG(pkt)->wl_hdr_information) &
            WLFC_PKTFLAG_PKTFROMHOST) && !SCB_ISMULTI(scb)) ||
#endif
        /* Drop the non-8021X packets when key exchange is in progress */
        ((scb->dropblock_dur != 0) && !(WLPKTTAG(pkt)->flags & WLF_8021X))) {

#ifdef WLTAF
        wlc_taf_txpkt_status(wlc->taf_handle, scb, PKTPRIO(pkt), pkt,
            TAF_TXPKT_STATUS_SUPPRESSED_FREE);
#endif
        return TRUE;
    }

    WL_PS_EX(scb, ("SUPPRESSED packet %p - PS:%d\n", OSL_OBFUSCATE_BUF(pkt), SCB_PS(scb)));

    if (!(scb_psinfo->flags & SCB_PS_FIRST_SUPPR_HANDLED)) {
        /* Is this the first suppressed frame, and either is partial
         * MSDU or has been retried at least once, driver needs to
         * preserve the retry count and sequence number in the PDU so that
         * next time it is transmitted, the receiver can put it in order
         * or discard based on txcnt. For partial MSDU, reused sequence
         * number will allow reassembly
         */
        wlc_get_txh_info(wlc, pkt, &txh_info);

        h = txh_info.d11HdrPtr;
        control = FC_TYPE(ltoh16(h->fc)) == FC_TYPE_CTL;

        if (!control) {
            seq_num = ltoh16(h->seq);
            frag = seq_num & FRAGNUM_MASK;
        }

        if (D11REV_GE(wlc->pub->corerev, 40)) {
            /* TxStatus in txheader is not needed in chips with MAC agg. */
            /* txcnt = txs->status.frag_tx_cnt << TX_STATUS_FRM_RTX_SHIFT; */
            txcnt = 0;
        }
        else {
            txcnt = txs->status.raw_bits & (TX_STATUS_FRM_RTX_MASK);
        }

        if ((frag || txcnt) && !control) {
            /* If the seq num was hw generated then get it from the
             * status pkt otherwise get it from the original pkt
             */
            if (D11REV_GE(wlc->pub->corerev, 40)) {
                txc_hwseq = txh_info.MacTxControlLow & htol16(D11AC_TXC_ASEQ);
            } else {
                txc_hwseq = txh_info.MacTxControlLow & htol16(TXC_HWSEQ);
            }

            if (txc_hwseq)
                seq_num = txs->sequence;
            else
                seq_num = seq_num >> SEQNUM_SHIFT;

            h->seq = htol16((seq_num << SEQNUM_SHIFT) | (frag & FRAGNUM_MASK));

            /* Clear hwseq flag in maccontrol low */
            /* set the retry counts */
            if (D11REV_GE(wlc->pub->corerev, 40)) {
                d11txhdr_t* txh = txh_info.hdrPtr;
                *D11_TXH_GET_MACLOW_PTR(wlc, txh) &=  ~htol16(D11AC_TXC_ASEQ);
                if (D11REV_LT(wlc->pub->corerev, 128)) {
                    txh->rev40.PktInfo.TxStatus = htol16(txcnt);
                }
            } else {
                d11txh_pre40_t* nonVHTHdr = &(txh_info.hdrPtr->pre40);
                nonVHTHdr->MacTxControlLow &= ~htol16(TXC_HWSEQ);
                nonVHTHdr->TxStatus = htol16(txcnt);
            }

            WL_PS_EX(scb, ("Partial MSDU PDU %p - frag:%d seq_num:%d txcnt: %d\n",
                OSL_OBFUSCATE_BUF(pkt), frag, seq_num, txcnt));
        }

        /* This ensures that all the MPDUs of the same SDU get
         * same seq_num. This is a case when first fragment was retried
         */
        if (last_frag || !(frag || txcnt))
            scb_psinfo->flags |= SCB_PS_FIRST_SUPPR_HANDLED;
    }

    /* Tag the pkt so that we can identify them later and move them
     * to the front when tx fifo drain/flush finishes.
     */
    WLPKTTAG(pkt)->flags3 |= WLF3_SUPR;

    /* Mark as retrieved from HW FIFO */
    WLPKTTAG(pkt)->flags |= WLF_FIFOPKT;

    if (wlc->lifetime_txfifo) {
        /* Clear packet expiration for PS path if TXFIFO lifetime mode enabled,
         * to avoid unintended packet drop.
         */
        WLPKTTAG(pkt)->flags &= ~WLF_EXPTIME;
    }

    /* If in PS mode, enqueue the suppressed PDU to PSQ for ucast SCB otherwise txq */
    if (!SCB_ISMULTI(scb)) {

        if (!SCB_PS(scb)) {
            /* Due to races in what indications are processed first, we either get
             * a PMQ indication that a SCB has entered PS mode, or we get a PMQ
             * suppressed packet. This is the patch where a PMQ suppressed packet is
             * the first indication that a SCB is in PS mode. Signal the PS switch
             * with the flag that the indication was a suppress packet.
             */
            WL_PS_EX(scb, ("PMQ entry interrupt delayed!\n"));
            wlc_apps_process_ps_switch(wlc, scb, PS_SWITCH_PMQ_SUPPR_PKT);
            ASSERT(SCB_PS(scb));
            scb_put_in_ps = TRUE;
        }

        /* If enqueue to psq successfully, return FALSE so that PDU is not freed */
        /* Enqueue at higher precedence as these are suppressed packets */

        if (wlc_apps_psq(wlc, pkt, WLC_PRIO_TO_HI_PREC(PKTPRIO(pkt)))) {
#if defined(WL_PS_SCB_TXFIFO_BLK)
            if (AUXPMQ_ENAB(wlc->pub) && scb_put_in_ps &&
                !SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)) {
                /* The last and only packet, make sure normalization
                 * is triggered.
                 */
                scb->ps_txfifo_blk = TRUE;
                wlc->ps_txfifo_blk_scb_cnt++;
            }
#endif /* WL_PS_SCB_TXFIFO_BLK */
            return FALSE;
        }

        WL_PS_EX(scb, ("ps suppr pkt discarded\n"));
        wlc_psinfo->ps_discard++;
        scb_psinfo->ps_discard++;
#ifdef WLTAF
        wlc_taf_txpkt_status(wlc->taf_handle, scb, PKTPRIO(pkt), pkt,
            TAF_TXPKT_STATUS_SUPPRESSED_FREE);
#endif
        return TRUE;
    }

    /* Make sure BSSCFG has transitioned to PS on for BCMC scb */
    if (!SCB_PS(scb)) {
        wlc_apps_bcmc_scb_ps_on(wlc, scb->bsscfg);
        ASSERT(SCB_PS(scb));
        scb_put_in_ps = TRUE;
    }
    if (wlc_apps_psq(wlc, pkt, WLC_PRIO_TO_HI_PREC(PKTPRIO(pkt)))) {
#if defined(WL_PS_SCB_TXFIFO_BLK)
        if (AUXPMQ_ENAB(wlc->pub) && scb_put_in_ps &&
            !SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)) {
            /* The last and only packet, make sure normalization is triggered */
            scb->ps_txfifo_blk = TRUE;
            wlc->ps_scb_txfifo_blk = TRUE;
        }
#endif /* WL_PS_SCB_TXFIFO_BLK */
        /* Frame enqueued, caller doesn't free */
        return FALSE;
    }

    /* error exit, return TRUE to have caller free packet */
    return TRUE;
}

#ifdef WLTWT

/* Put bcmc in (or out) PS as a result of TWT active link for scb */
static void
wlc_apps_bcmc_force_ps_by_twt(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, bool on)
{
    apps_bss_info_t *bss_info = APPS_BSSCFG_CUBBY(wlc->psinfo, bsscfg);
    struct scb *bcmc_scb;

    bcmc_scb = WLC_BCMCSCB_GET(wlc, bsscfg);
    ASSERT(bcmc_scb->bsscfg == bsscfg);

    if (on) {
        WL_PS0(("wl%d.%d: %s - ON\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
            __FUNCTION__));
        if (!SCB_PS(bcmc_scb)) {
            wlc_apps_bcmc_scb_ps_on(wlc, bsscfg);
        }
        bss_info->ps_trans_status |= BSS_PS_ON_BY_TWT;
    } else if (bss_info->ps_trans_status & BSS_PS_ON_BY_TWT) {
        WL_PS0(("wl%d.%d: %s - OFF\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
            __FUNCTION__));
        bss_info->ps_trans_status &= ~BSS_PS_ON_BY_TWT;

        /* Set PS flag on bcmc_scb to avoid ASSERTs */
        bcmc_scb->PS = TRUE;
        if (bss_info->ps_nodes == 0) {
            wlc_apps_bcmc_ps_off_start(wlc, bcmc_scb);
        }
    }
}

bool
wlc_apps_twt_sp_enter_ps(wlc_info_t *wlc, scb_t *scb)
{
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL) {
        return FALSE;
    }
    if (SCB_TWTPS(scb)) {
        return TRUE;
    }
    /* There is a race condition possible which is tough to handle.The SCB may
     * haven been taken out of TWT but still packets arrive with TWT suppress.
     * They cant be enqueued using regular PS, as there may not come PM event from
     * remote for long time. We cant put SCB in PS_TWT, as we are already supposed to
     * have exited. The best would be to use pspretend and get that module solve the
     * exit of SP. But hat is hard. For now are just going to drop the packet. May
     * need some fixing later. Lets not drop it but re-enque, gives possible out of
     * order, but less problematic. May cause AMPDU assert?
     */
    if (!(wlc_twt_scb_active(wlc->twti, scb))) {
        return FALSE;
    }
    WL_PS_EX(scb, ("Activating PS_TWT AID %d\n", SCB_AID(scb)));

    scb->PS_TWT = TRUE;
#ifdef WL_PS_STATS
    if (scb->ps_starttime == 0) {
        scb->ps_on_cnt++;
        scb->ps_starttime = OSL_SYSUPTIME();
    /* dump_flag_qqdx */
#ifdef dump_stack_qqdx_print
        printk(KERN_ALERT"----------[fyl] wlc_apps_twt_sp_enter_ps dump_stack start----------");
        dump_stack();
        printk(KERN_ALERT"----------[fyl] wlc_apps_twt_sp_enter_ps dump_stack stop----------");
#endif /*dump_stack_qqdx_print*/
    }
#endif /* WL_PS_STATS */
    /* NOTE: set flags to SCB_PS_FIRST_SUPPR_HANDLED then this function does not have to
     * use txstatus towards wlc_apps_suppr_frame_enq. So if suppport for
     * first_suppr_handled is needed then txstatus has to be passed on this fuction !!
     */
    scb_psinfo->flags |= SCB_PS_FIRST_SUPPR_HANDLED;
    /* Add the APPS to the txpath for this SCB */
    wlc_txmod_config(wlc->txmodi, scb, TXMOD_APPS);

#ifdef WLTAF
    /* Update TAF state */
    wlc_taf_scb_state_update(wlc->taf_handle, scb, TAF_NO_SOURCE, NULL,
        TAF_SCBSTATE_TWT_SP_EXIT);
#endif /* WLTAF */

#if defined(BCMPCIEDEV)
    /* Reset all pending fetch and rollback flow fetch ptrs */
    wlc_scb_flow_ps_update(wlc->wl, SCB_FLOWID(scb), TRUE);
#endif
    /* ps enQ any pkts on the txq, narq, ampduq */
    wlc_apps_txq_to_psq(wlc, scb);

    /* XXX TODO : Rollback packets to flow ring rather than
     * sending them into PSq during PS transition for PQP
     */
#ifdef PROP_TXSTATUS
    /* This causes problems for PSPRETEND */
    wlc_apps_ampdu_txq_to_psq(wlc, scb);
    wlc_apps_nar_txq_to_psq(wlc, scb);
#endif /* PROP_TXSTATUS */
    if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)) {
        scb_psinfo->twt_wait4drain_norm = TRUE;
    }

    /* validate the PVB entry since we are outside the SP, during the SP we want to
     * keep the TIM as it was while entering for if beacon gets out during that SP, it is
     * unpredictable what state of data will be at end of SP, but seems more logical to keep
     * the state of begin of SP, while at end of SP we need revalidate and see if we can
     * possibly clear the TIM.
     */
    wlc_apps_pvb_update(wlc, scb);

#ifdef HNDPQP
    /* PQP page out of suppress PSq and PSq */
    wlc_apps_scb_pqp_pgo(wlc, scb, WLC_PREC_BMP_ALL);
#endif
    return TRUE;
}

bool
wlc_apps_suppr_twt_frame_enq(wlc_info_t *wlc, void *pkt)
{
    scb_t *scb = WLPKTTAGSCBGET(pkt);
    struct apps_scb_psinfo *scb_psinfo;
    bool ret_value;
    bool current_ps_state;

    ASSERT(scb != NULL);
    ASSERT(!SCB_ISMULTI(scb));

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL) {
        return TRUE;
    }

    /* Cleare PSDONTQ in case of TWT */
    WLPKTTAG(pkt)->flags &= ~WLF_PSDONTQ;

    if (!wlc_apps_twt_sp_enter_ps(wlc, scb)) {
        /* On exit TWT the packet arrives here. The SCB should be in PS, use normal
         * wlc_apps_suppr_frame_enq to handle the packet.
         */
        if (SCB_PS(scb)) {
            ret_value = wlc_apps_suppr_frame_enq(wlc, pkt, NULL, TRUE);
        } else {
            ret_value = TRUE;
        }
        return ret_value;
    }

    /* Simulate PS to be able to call suppr_frame_enq */
    current_ps_state = scb->PS;
    scb->PS = TRUE;
    ret_value = wlc_apps_suppr_frame_enq(wlc, pkt, NULL, TRUE);
    ASSERT(scb->PS);
    scb->PS = current_ps_state;

    /* If enqueue to supr psq successfully. Driver should do psq normalization.
     * But below case will not call wlc_apps_scb_psq_norm() to do it.
     * 0. There is only one suppression packet.
     * 1. wlc_ampdu_dotxstatus_aqm_complete() decrease SCB_TOT_PKTS_INFLT_FIFOCNT_VAL to 0.
     * 2. wlc_apps_suppr_twt_frame_enq()
     * 3. wlc_apps_twt_sp_enter_ps(), SCB_TOT_PKTS_INFLT_FIFOCNT_VAL is 0.
     *    So twt_wait4drain_norm will not be set to TRUE.
     * 4. wlc_apps_trigger_on_complete(), will not call wlc_apps_scb_psq_norm()
     *    because twt_wait4drain_norm is FALSE.
     * When this happened, driver left this packet in supr psq without enqueue to psq.
     * This packet will not be sent via psq.
     * It will trigger ampdu watchdog to cleanup the ampdu ini for this tid.
     * Set twt_wait4drain_norm to TRUE if driver enqueue to supr psq or psq successfully.
     */
    if (!ret_value) {
        scb_psinfo->twt_wait4drain_norm = TRUE;
    }

    return ret_value;
}

void
wlc_apps_twt_sp_release_ps(wlc_info_t *wlc, scb_t *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
#ifdef dump_stack_qqdx_print
        printk(KERN_ALERT"----------[fyl]  wlc_apps_twt_sp_release_ps dump_stack start----------");
        //dump_stack();
        printk(KERN_ALERT"###########OSL_SYSUPTIME()(%u)",OSL_SYSUPTIME());
        printk(KERN_ALERT"----------[fyl]  wlc_apps_twt_sp_release_ps dump_stack stop----------");
#endif /*dump_stack_qqdx_print*/

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL)
        return;

    if (!SCB_TWTPS(scb)) {
        return;
    }

    if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)) {
        /* We cant release the PS yet, we are still waiting for more suppress */
        WL_PS_EX(scb, ("PS_TWT active AID %d, delaying release, "
            "fifocnt %d\n",
            SCB_AID(scb), SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)));
        scb_psinfo->twt_wait4drain_exit = TRUE;
        return;
    }

    WL_PS_EX(scb, ("PS_TWT active, releasing AID %d\n", SCB_AID(scb)));
    scb->PS_TWT = FALSE;
    wlc_apps_upd_pstime(scb);
    scb_psinfo->twt_wait4drain_exit = FALSE;

#if defined(BCMPCIEDEV)
    /* Reset all pending fetch and rollback flow fetch ptrs */
    wlc_scb_flow_ps_update(wlc->wl, SCB_FLOWID(scb), FALSE);
#endif
    /* XXX wlc_wlfc_scb_ps_off did a reset of AMPDU seq with a BAR.
     * Done only for FD mode. Required .??
     * XXX With prop tx gone, TWT need to handle packet release through TAF
     */

    /* Unconfigure the APPS from the txpath */
    wlc_txmod_unconfig(wlc->txmodi, scb, TXMOD_APPS);
#ifdef HNDPQP
    /* Page in all Host resident packets into dongle */
    wlc_apps_scb_pqp_pgi(wlc, scb, WLC_PREC_BMP_ALL, PKTQ_PKTS_ALL, PQP_PGI_PS_TWT_OFF);
#endif /* HNDPQP */

    /* release all packets from PSQ to Common Q */
#ifdef WLTAF
    /* Update TAF state */
    wlc_taf_scb_state_update(wlc->taf_handle, scb, TAF_NO_SOURCE, NULL,
        TAF_SCBSTATE_TWT_SP_ENTER);

    /* If taf is not enabled then we need to release the data in the old way */
    if (!wlc_taf_psq_in_use(wlc->taf_handle, NULL))
#endif /* WLTAF */
    {
        wlc_apps_ps_send(wlc, scb, WLC_PREC_BMP_ALL, FALSE, NULL, WLC_APPS_RELEASE_ALL);
    }
}

/*
 * This function is to be called when fifo (LO/HW) is empty and twt was to be entered. The SCB will
 * get PS state clearead and switched to TWT_PS mode. PS mode should be active, as it is expected
 * that data for this SCB is blocked to drain (LO/HW) queue allowing it to switch it over to TWT
 */
static void
wlc_apps_twt_enter_ready(wlc_info_t *wlc, scb_t *scb)
{
    apps_wlc_psinfo_t *wlc_psinfo = wlc->psinfo;
    struct apps_scb_psinfo *scb_psinfo;
    apps_bss_info_t *bss_info;

    WL_TWT(("%s Enter AID %d\n", __FUNCTION__, SCB_AID(scb)));

    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);

    bss_info = APPS_BSSCFG_CUBBY(wlc_psinfo, SCB_BSSCFG(scb));
    ASSERT(bss_info);

    /* SCB_PS_TRANS_OFF_BLOCKED should be cleared as for entering TWT we may have used
     * PS_SWITCH_PMQ_SUPPR_PKT which caused this flag to be set.
     */
    scb_psinfo->ps_trans_status &= ~SCB_PS_TRANS_OFF_BLOCKED;
    if (scb->PS) {
        wlc_apps_upd_pstime(scb);
    }
    ASSERT(scb->PS);
    scb->PS = FALSE;
    scb->PS_TWT = TRUE;
#ifdef WL_PS_STATS
    if (scb->ps_starttime == 0) {
        scb->ps_on_cnt++;
        scb->ps_starttime = OSL_SYSUPTIME();
    /* dump_flag_qqdx */
#ifdef dump_stack_qqdx_print
        printk(KERN_ALERT"----------[fyl] wlc_apps_twt_enter_ready(wlc_info_t *wlc, scb_t *scb) dump_stack start----------");
        dump_stack();
        printk(KERN_ALERT"----------[fyl] wlc_apps_twt_enter_ready(wlc_info_t *wlc, scb_t *scb) dump_stack stop----------");
#endif /*dump_stack_qqdx_print*/
    }
#endif /* WL_PS_STATS */
    ASSERT(bss_info->ps_nodes);
    bss_info->ps_nodes--;
    ASSERT(wlc_psinfo->ps_nodes_all);
    wlc_psinfo->ps_nodes_all--;
    /* Check if the PVB entry needs to be cleared */
    if (scb_psinfo->in_pvb) {
        wlc_apps_pvb_update(wlc, scb);
    }

    wlc_twt_apps_ready_for_twt(wlc->twti, scb);
}

/*
 * This function is to be called when scb puts first SP in TWT mode. APPS will take the necessary
 * steps to get the queues flushed (if needed). Once LO/HW queues are empty PS mode is cleared and
 * TWT_PS becomes active for this SCB. When this happens a callback to TWT will be made to
 * notify TWT that SCB is prepared and ready to be used for TWT.
 */
void
wlc_apps_twt_enter(wlc_info_t *wlc, scb_t *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct scb_iter scbiter;
    scb_t *scb_tst;
    uint total;

#if defined(WL_PS_SCB_TXFIFO_BLK)
    WL_TWT(("%s Enter AID %d current PS %d (%d/%d)\n", __FUNCTION__, SCB_AID(scb), SCB_PS(scb),
        SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb), SCB_PS_TXFIFO_BLK(scb)));
#else /* ! WL_PS_SCB_TXFIFO_BLK */
    WL_TWT(("%s Enter AID %d current PS %d (%d)\n", __FUNCTION__, SCB_AID(scb), SCB_PS(scb),
        SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)));
#endif /* ! WL_PS_SCB_TXFIFO_BLK */

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL)
        return;

    ASSERT(!scb_psinfo->twt_active);
    scb_psinfo->twt_active = TRUE;

    /* Clear twt_wait4drain_exit as we may have set this and it will cause (delayed)
     * exiting TWT even though we already got (re-)entered here.
     */
    scb_psinfo->twt_wait4drain_exit = FALSE;

    /* Force enter of PS when it is not active */
    if (SCB_PS(scb)) {
        WL_TWT(("%s SCB already in PS\n", __FUNCTION__));
#ifdef PSPRETEND
        /* Make sure pspretend wont control PS states anymore, TWT takes over */
        if (wlc->pps_info != NULL) {
            wlc_pspretend_scb_ps_off(wlc->pps_info, scb);
        }
#endif /* PSPRETEND */
    } else {
        WL_TWT(("%s Forcing SCB in PS\n", __FUNCTION__));
        wlc_apps_process_ps_switch(wlc, scb, PS_SWITCH_PMQ_SUPPR_PKT);
        /* Remove the wait for PMQ block */
        scb_psinfo->ps_trans_status &= ~SCB_PS_TRANS_OFF_BLOCKED;
    }

#if defined(WL_PS_SCB_TXFIFO_BLK)
    if (SCB_PS_TXFIFO_BLK(scb)) {
#else /* ! WL_PS_SCB_TXFIFO_BLK */
    if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb) && (wlc->block_datafifo & DATA_BLOCK_PS)) {
#endif /* ! WL_PS_SCB_TXFIFO_BLK */
        /* Set flag to identify the wait for drainage complete */
        scb_psinfo->twt_wait4drain_enter = TRUE;
    } else {
        /* APPS is ready, queus are empty, link can be put in TWT mode */
        wlc_apps_twt_enter_ready(wlc, scb);
    }

    /* Check if we should put BCMC for BSSCFG in PS. If this is first SCB to enter TWT and
     * then keep BCMC in PS.
     */
    total = 0;
    FOREACH_BSS_SCB(wlc->scbstate, &scbiter, SCB_BSSCFG(scb), scb_tst) {
        if (SCB_PSINFO(wlc->psinfo, scb_tst)->twt_active) {
            total++;
        }
    }
    if (total == 1) {
        wlc_apps_bcmc_force_ps_by_twt(wlc, SCB_BSSCFG(scb), TRUE);
    }
}

void
wlc_apps_twt_exit(wlc_info_t *wlc, scb_t *scb, bool enter_pm)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct scb_iter scbiter;
    scb_t *scb_tst;
    uint total;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL) {
        return;
    }

    WL_TWT(("%s Enter AID %d new PM %d (%d/%d/%d/%d/%d)\n", __FUNCTION__, SCB_AID(scb),
        enter_pm, scb_psinfo->twt_active, SCB_PS(scb), SCB_TWTPS(scb),
        scb_psinfo->twt_wait4drain_enter, SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)));

    if (!scb_psinfo->twt_active) {
        return;
    }
    scb_psinfo->twt_active = FALSE;

    /* Clear twt_wait4drain_enter as we may have set this and it will cause (delayed)
     * entering TWT even though we already got exited here.
     */
    scb_psinfo->twt_wait4drain_enter = FALSE;

    /* Force enter of PS when it is not active, even if PM mode indicates PM off then PM
     * has to be initiated first if there are packets in flight. This is needed so the TWT
     * flags get removed from the packets. Also if current mode is PS_TWT then switch to
     * regular PM mode.
     */
    if (SCB_TWTPS(scb) || (enter_pm) || (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb))) {
        WL_TWT(("%s Forcing SCB in PS\n", __FUNCTION__));
        wlc_apps_process_ps_switch(wlc, scb, PS_SWITCH_PMQ_SUPPR_PKT);
        /* Remove the wait for PMQ block, as the packet is 'simulated' */
        scb_psinfo->ps_trans_status &= ~SCB_PS_TRANS_OFF_BLOCKED;
        scb->PS_TWT = FALSE;
        wlc_apps_upd_pstime(scb);
    }

    /* Check if we should take BCMC for BSSCFG out of PS. If this is last SCB to exit TWT for
     * this bsscfg then switch back to normal BCMC PS mode.
     */
    total = 0;
    FOREACH_BSS_SCB(wlc->scbstate, &scbiter, SCB_BSSCFG(scb), scb_tst) {
        if (SCB_PSINFO(wlc->psinfo, scb_tst)->twt_active) {
            total++;
        }
    }
    if (total == 0) {
        wlc_apps_bcmc_force_ps_by_twt(wlc, SCB_BSSCFG(scb), FALSE);
    }

    /* If PM off then start releasing packets. Use PM routines to leave the PM mode. */
    if (!enter_pm) {
        wlc_apps_process_ps_switch(wlc, scb, PS_SWITCH_OFF);
    }
}

#endif /* WLTWT */

/*
 * This function is to be called once the txstatus processing has completed, and the SCB
 * counters have been updated. Do note that SCB can be null. In that case iteration over all
 * SCBs should be performed. This trigger is to be called from wlc_txfifo_complete, which
 * normally gets called to (among other things) handle/finalize txdata block. In this function
 * datablock on a per scb is to be handled. Currently in use for TWT & PS.
 */
void
wlc_apps_trigger_on_complete(wlc_info_t *wlc, scb_t *scb)
{
    struct apps_scb_psinfo *scb_psinfo;

    /* TO BE FIXED, take quick approach for testing now */
    if ((!scb) || (SCB_INTERNAL(scb))) {
        return;
    }

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (scb_psinfo == NULL) {
        return;
    }

#ifdef HNDPQP
    /* Page out PS/Suppress queue at the end of Tx status processing when STA in PS.
     * If PGI_PS_TRANS is in progress, don't page out the PS queue.
     */
    if ((SCB_PS(scb) || SCB_TWTPS(scb)) && (PQP_PGI_PS_TRANS(scb_psinfo) == 0)) {
        wlc_apps_scb_pqp_pgo(wlc, scb, WLC_PREC_BMP_ALL);
    }
#endif

    /* When this was the last packet outstanding in LO/HW queue then..... */
    if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb) == 0) {
#if defined(WL_PS_SCB_TXFIFO_BLK)
        if (AUXPMQ_ENAB(wlc->pub)) {
            if ((SCB_PS(scb) || SCB_TWTPS(scb)) && !SCB_ISMULTI(scb)) {
    /* dump_flag_qqdx */
#ifdef dump_stack_ps_qqdx_print
    printk(KERN_ALERT"----------[fyl] wlc_apps_trigger_on_complete (%u)wlc_apps_scb_drained----------",OSL_SYSUPTIME());
#endif /*dump_stack_ps_qqdx_print*/
                wlc_apps_scb_drained(wlc, scb);
            }
        } else
#endif /* WL_PS_SCB_TXFIFO_BLK */
        {
        if (SCB_TWTPS(scb)) {
            if (scb_psinfo->twt_wait4drain_norm) {
                wlc_apps_scb_psq_norm(wlc, scb);
                scb_psinfo->twt_wait4drain_norm = FALSE;
            }
            if (scb_psinfo->twt_wait4drain_exit) {
                wlc_apps_twt_sp_release_ps(wlc, scb);
            } else {
                wlc_twt_ps_suppress_done(wlc->twti, scb);
            }
        }
        }

    }
}

/*
 * WLF_PSDONTQ notes
 *    wlc_pkt_abs_supr_enq() drops PSDONTQ pkts on suppress with comment
 *        "toss driver generated NULL frame"
 *    wlc_dotxstatus(): PSPRETEND avoids working on PSDONTQ frames with the comment
 *        "the flag for WLF_PSDONTQ is checked because this is used for probe packets."
 *        Also in same fn pspretend avoids psdontq frame for wlc_apps_process_pspretend_status()
 *    wlc_queue_80211_frag(): added PSDONTQ flag to non-bufferable mgmt in r490247
 *        Auth, (Re)Assoc, Probe, Bcn, ATIM
 *    wlc_queue_80211_frag(): USE the PSDONTQ flag to send to TxQ instead of scb->psq
 *    wlc_sendctl(): USE the PSDONTQ flag to send to TxQ instead of scb->psq
 *    wlc_ap_sta_probe(): SETS for PSPretend NULL DATA, clear on AP sta probe.
 *    wlc_ap_do_pspretend_probe(): SETS for PSPretend NULL DATA
 *    wlc_apps_enqueue_pkt(): TDLS checks and passes to next TxMod "for TDLS PTI resp,
 *        send right away" odd that this fn is also called for txq_to_psq() for PS on transition,
 *        so don't think TxMod should be happening
 *    wlc_apps_ps_prep_mpdu(): checks PSDONTQ to identify final NULL DATA in pspoll
 *        chain termination
 *    wlc_apps_send_psp_response(): SETTING PSDONTQ in final NULL DATA for pspoll chain
 *    wlc_apps_suppr_frame_enq(): handling PMQ suppressed pkts, sends PSDONTQ pkts to TxQ instead
 *        of psq, just like wlc_queue_80211_frag() would have
 *    wlc_apps_apsd_eosp_send(): SETTING PSDONTQ in final NULL DATA for APSD service period
 *    wlc_apps_apsd_prepare(): ??? TDLS checks PSDONTQ in it's logic to clear eosp
 *    wlc_p2p_send_prbresp(): SETS PSDONTQ for Probe Resp
 *    wlc_probresp_send_probe_resp(): SETS PSDONTQ for Probe Resp
 *    wlc_tdls_send_pti_resp(): SETS PSDONTQ for action frame
 *    wlc_tx_fifo_sync_complete(): (OLD! Non-NEW_TXQ!) would send any frames recovered during sync
 *        to the psq for a SCB_PS() sta if not PSDONTQ. Looks like it was soft PMQ processing
 *        at the end of sync. Seems like this would have thrown off tx_intransit counts?
 *    wlc_ap_wds_probe():  SETS for PSPretend NULL DATA, clear on AP sta probe.
 */

/*
 * APSD Host Packet Timeout (hpkt_tmout)
 * In order to keep a U-APSD Service Period active in a Prop_TxStatus configuration,
 * a timer is used to indicate that a packet may be arriving soon from the host.
 *
 * Normally an apsd service period would end as soon as there were no more pkts queued
 * for a destination. But since there may be a lag from request to delivery of a pkt
 * from the host, the hpkt_tmout timer is set when a host pkt request is made.
 *
 * The pkt completion routine wlc_apps_apsd_complete() will normally send the next packet,
 * or end the service period if no more pkts. Instead of ending the serivice period,
 * if "apsd_hpkt_timer_on" is true, nothing is done in wlc_apps_apsd_complete(), and instead
 * this routine will end the service period if the timer expires.
 */
static void
wlc_apps_apsd_hpkt_tmout(void *arg)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct scb *scb;
    wlc_info_t *wlc;

    scb_psinfo = (struct apps_scb_psinfo *)arg;
    ASSERT(scb_psinfo);

    scb = scb_psinfo->scb;
    wlc = scb_psinfo->wlc;

    ASSERT(scb);
    ASSERT(wlc);

    /* send the eosp if still valid (entry to p2p abs makes apsd_usp false)
    * and no pkt in transit/waiting on pkt complete
    */

    if (scb_psinfo->apsd_usp == TRUE && !scb_psinfo->apsd_tx_pending &&
        (scb_psinfo->apsd_cnt > 0 || scb_psinfo->ext_qos_null)) {
        wlc_apps_apsd_send(wlc, scb);
    }
    scb_psinfo->apsd_hpkt_timer_on = FALSE;
}

static void
wlc_apps_apsd_send(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint prec_bmp;
#ifdef BCMPCIEDEV
    ac_bitmap_t ac_to_request = scb->apsd.ac_delv & AC_BITMAP_ALL;
#endif

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);
    ASSERT(scb_psinfo->apsd_cnt > 0 || scb_psinfo->ext_qos_null);

    /*
     * If there are no buffered frames, send a QoS Null on the highest delivery-enabled AC
     * (which AC to use is not specified by WMM/APSD).
     */
    if (scb_psinfo->ext_qos_null ||
        ((wlc_apps_apsd_delv_count(wlc, scb) == 0) &&
#ifdef BCMPCIEDEV
        (wlc_apps_apsd_delv_vpkt_count(wlc, scb) == 0) &&
#endif
        TRUE)) {
#ifdef WLTDLS
        if (BSS_TDLS_ENAB(wlc, scb->bsscfg) &&
            wlc_tdls_in_pti_interval(wlc->tdls, scb)) {
            return;
        }
#endif /* WLTDLS */
        wlc_apps_apsd_eosp_send(wlc, scb);
        return;
    }

    prec_bmp = WLC_ACBITMAP_TO_PRECBITMAP(scb->apsd.ac_delv);

#ifdef BCMPCIEDEV
    /* Continuous pkt flow till last packet is is needed for Wi-Fi P2P 6.1.12/6.1.13.
     * by fetching pkts from host one after another
     * and wait till either timer expires or new packet is received
     */
#ifdef WLAMSDU_TX
    uint16 max_sf_frames = wlc_amsdu_scb_max_sframes(wlc->ami, scb);
#else
    uint16 max_sf_frames = 1;
#endif

    if (!scb_psinfo->apsd_hpkt_timer_on &&
        scb_psinfo->apsd_cnt > 1 &&
        (wlc_apps_apsd_delv_count(wlc, scb) <=
        (WLC_APSD_DELV_CNT_LOW_WATERMARK * max_sf_frames))) {

        /* fetch packets from host flow ring */
        scb_psinfo->apsd_v2r_in_transit +=
            wlc_sqs_psmode_pull_packets(wlc, scb,
            WLC_ACBITMAP_TO_TIDBITMAP(ac_to_request),
            (WLC_APSD_DELV_CNT_HIGH_WATERMARK * max_sf_frames));

        wl_add_timer(wlc->wl, scb_psinfo->apsd_hpkt_timer,
            WLC_PS_APSD_HPKT_TIME, FALSE);
        scb_psinfo->apsd_hpkt_timer_on = TRUE;

        if (wlc_apps_apsd_delv_count(wlc, scb) == 0)
            return;
    }
#endif /* BCMPCIEDEV */
    /*
     * Send a delivery frame.  When the frame goes out, the wlc_apps_apsd_complete()
     * callback will attempt to send the next delivery frame.
     */
    if (!APPS_PS_SEND(wlc, scb, prec_bmp, TRUE, NULL)) {
#ifdef WLCFP
        if (CFP_ENAB(wlc->pub) && SCB_AMPDU(scb) && !wlc_cfp_ampdu_ps_send(wlc, scb,
            WLC_ACBITMAP_TO_TIDBITMAP(scb->apsd.ac_delv), WLF_APSD))
#endif /* WLCFP */
            wlc_apps_apsd_usp_end(wlc, scb);
    }

}

#ifdef WLTDLS
void
wlc_apps_apsd_tdls_send(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    if (BSS_TDLS_ENAB(wlc, scb->bsscfg)) {
        if (!scb_psinfo->apsd_usp)
            return;

        scb_psinfo->apsd_cnt = wlc_apps_apsd_delv_count(wlc, scb);

        if (scb_psinfo->apsd_cnt)
            wlc_apps_apsd_send(wlc, scb);
        else
            wlc_apps_apsd_eosp_send(wlc, scb);
    }
    return;
}
#endif /* WLTDLS */

static const uint8 apsd_delv_acbmp2maxprio[] = {
    PRIO_8021D_BE, PRIO_8021D_BE, PRIO_8021D_BK, PRIO_8021D_BK,
    PRIO_8021D_VI, PRIO_8021D_VI, PRIO_8021D_VI, PRIO_8021D_VI,
    PRIO_8021D_NC, PRIO_8021D_NC, PRIO_8021D_NC, PRIO_8021D_NC,
    PRIO_8021D_NC, PRIO_8021D_NC, PRIO_8021D_NC, PRIO_8021D_NC
};

/* Send frames in a USP, called in response to receiving a trigger frame */
void
wlc_apps_apsd_trigger(wlc_info_t *wlc, struct scb *scb, int ac)
{
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    /* Ignore trigger frames received during tx block period */
    if (scb_psinfo->tx_block != 0) {
        WL_PS_EX(scb, ("tx blocked; ignoring trigger\n"));
        return;
    }

    /* Ignore trigger frames received during an existing USP */
    if (scb_psinfo->apsd_usp) {
        WL_PS_EX(scb, ("already in USP; ignoring trigger\n"));

        /* Reset usp if the num of triggers exceeds the threshold */
        if (++scb_psinfo->apsd_trig_cnt > APSD_TRIG_THR) {
            scb_psinfo->apsd_trig_cnt = 0;
            wlc->apsd_usp_reset++;
            wlc_apps_apsd_usp_end(wlc, scb);
        }
        return;
    }

    WL_PS_EX(scb, ("ac %d buffered %d delv %d\n", ac,
        APPS_PKTQ_PREC_N_PKTS(&scb_psinfo->psq, ac), wlc_apps_apsd_delv_count(wlc, scb)));

    scb_psinfo->apsd_usp = TRUE;

    /* initialize the delivery count for this SP */
    scb_psinfo->apsd_cnt = scb->apsd.maxsplen;

    /*
     * Send the first delivery frame.  Subsequent delivery frames will be sent by the
     * completion callback of each previous frame.  This is not very efficient, but if
     * we were to queue a bunch of frames to different FIFOs, there would be no
     * guarantee that the MAC would send the EOSP last.
     */

    wlc_apps_apsd_send(wlc, scb);
}

static void
wlc_apps_apsd_eosp_send(wlc_info_t *wlc, struct scb *scb)
{
    int prio = (int)apsd_delv_acbmp2maxprio[scb->apsd.ac_delv & 0xf];
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    WL_PS_EX(scb, ("sending QoS Null prio=%d\n", prio));

    scb_psinfo->ext_qos_null = FALSE;
    scb_psinfo->apsd_cnt = 0;

    if (wlc_sendnulldata(wlc, scb->bsscfg, &scb->ea, 0,
        (WLF_PSDONTQ | WLF_APSD), prio, NULL, NULL) == FALSE) {
        WL_ERROR(("wl%d: %s could not send QoS Null\n",
            wlc->pub->unit, __FUNCTION__));
        wlc_apps_apsd_usp_end(wlc, scb);
    }

    /* just reset the apsd_uspflag, don't update the apsd_endtime to allow TDLS PTI */
    /* to send immediately for the first packet */
    if (BSS_TDLS_ENAB(wlc, scb->bsscfg))
        wlc_apps_apsd_usp_end(wlc, scb);
}

/* Make decision if we need to count MMPDU in SP */
static bool
wlc_apps_apsd_count_mmpdu_in_sp(wlc_info_t *wlc, struct scb *scb, void *pkt)
{
    BCM_REFERENCE(wlc);
    BCM_REFERENCE(scb);
    BCM_REFERENCE(pkt);

    return TRUE;
}

/* wlc_apps_apsd_prepare()
 *
 * Keeps track of APSD info as pkts are prepared for TX. Updates the apsd_cnt (number of pkts
 * sent during a Service Period) and marks the EOSP bit on the pkt or calls for
 * a null data frame to do the same (ext_qos_null) if the end of the SP has been reached.
 */
void
wlc_apps_apsd_prepare(wlc_info_t *wlc, struct scb *scb, void *pkt,
    struct dot11_header *h, bool last_frag)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint16 *pqos;
    bool qos;
    bool more = FALSE;
    bool eosp = FALSE;

    /* The packet must have 802.11 header */
    ASSERT(WLPKTTAG(pkt)->flags & WLF_TXHDR);

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    /* Set MoreData if there are still buffered delivery frames */
    if ((wlc_apps_apsd_delv_count(wlc, scb) > 0) ||
#ifdef BCMPCIEDEV
        (wlc_apps_apsd_delv_vpkt_count(wlc, scb) > 0) ||
#endif
        FALSE) {

        more = TRUE;
    }

    if (more) {
        h->fc |= htol16(FC_MOREDATA);
    } else {
        h->fc &= ~htol16(FC_MOREDATA);
    }

    qos = ((ltoh16(h->fc) & FC_KIND_MASK) == FC_QOS_DATA) ||
          ((ltoh16(h->fc) & FC_KIND_MASK) == FC_QOS_NULL);

    /* SP countdown */
    if (last_frag &&
        (qos || wlc_apps_apsd_count_mmpdu_in_sp(wlc, scb, pkt))) {
        /* Indicate EOSP when this is the last MSDU in the psq */
        /* JQL: should we keep going in case there are on-the-fly
         * MSDUs and let the completion callback to check if there is
         * any other buffered MSDUs then and indicate the EOSP using
         * an extra QoS NULL frame?
         */
        if (!more)
            scb_psinfo->apsd_cnt = 1;
        /* Decrement count of packets left in service period */
        if (scb_psinfo->apsd_cnt != WLC_APSD_USP_UNB)
            scb_psinfo->apsd_cnt--;
    }

    /* SP termination */
    if (qos) {
        pqos = (uint16 *)((uint8 *)h +
            (SCB_A4_DATA(scb) ? DOT11_A4_HDR_LEN : DOT11_A3_HDR_LEN));
        ASSERT(ISALIGNED(pqos, sizeof(*pqos)));

        /* Set EOSP if this is the last frame in the Service Period */
#ifdef WLTDLS
        /* Trigger frames are delivered in PTI interval, because
         * the the PTI response frame triggers the delivery of buffered
         * frames before PTI response is processed by TDLS module.
         * QOS null frames have WLF_PSDONTQ, why should they not terminate
         * an SP?
         */
        if (BSS_TDLS_ENAB(wlc, scb->bsscfg) &&
            (wlc_tdls_in_pti_interval(wlc->tdls, scb) ||
            (SCB_PS(scb) && (WLPKTTAG(pkt)->flags & WLF_PSDONTQ) &&
            (scb_psinfo->apsd_cnt != 0)    &&
            ((ltoh16(h->fc) & FC_KIND_MASK) != FC_QOS_NULL)))) {
            eosp = FALSE;
        }
        else
#endif
        eosp = scb_psinfo->apsd_cnt == 0 && last_frag;
        if (eosp)
            *pqos |= htol16(QOS_EOSP_MASK);
        else
            *pqos &= ~htol16(QOS_EOSP_MASK);
    }
    /* Send an extra QoS Null to terminate the USP in case
     * the MSDU doesn't have a EOSP field i.e. MMPDU.
     */
    else if (scb_psinfo->apsd_cnt == 0)
        scb_psinfo->ext_qos_null = TRUE;

    /* Register callback to end service period after this frame goes out */
    if (last_frag) {
        WLF2_PCB2_REG(pkt, WLF2_PCB2_APSD);
    }

    WL_PS_EX(scb, ("pkt %p qos %d more %d eosp %d cnt %d lastfrag %d\n",
        OSL_OBFUSCATE_BUF(pkt), qos, more, eosp,
        scb_psinfo->apsd_cnt, last_frag));
}

/* End the USP when the EOSP has gone out
 * Pkt callback fn
 */
static void
wlc_apps_apsd_complete(wlc_info_t *wlc, void *pkt, uint txs)
{
    struct scb *scb;
    struct apps_scb_psinfo *scb_psinfo;

    /* Is this scb still around */
    if ((scb = WLPKTTAGSCBGET(pkt)) == NULL) {
        WL_ERROR(("%s(): scb = %p, WLPKTTAGSCBGET(pkt) = %p\n",
            __FUNCTION__, OSL_OBFUSCATE_BUF(scb),
            OSL_OBFUSCATE_BUF(WLPKTTAGSCBGET(pkt))));
        return;
    }

#ifdef BCMDBG
    /* What to do if not ack'd?  Don't want to hang in USP forever... */
    if (txs & TX_STATUS_ACK_RCV)
        WL_PS_EX(scb, ("delivery frame %p sent\n", OSL_OBFUSCATE_BUF(pkt)));
    else
        WL_PS_EX(scb, ("delivery frame %p sent (no ACK)\n", OSL_OBFUSCATE_BUF(pkt)));
#else

    BCM_REFERENCE(txs);

#endif

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    /* Check if the PVB entry needs to be cleared */
    if (scb_psinfo->in_pvb) {
        wlc_apps_pvb_update(wlc, scb);
    }

#ifdef PROP_TXSTATUS
    scb_psinfo->apsd_tx_pending = FALSE;
#endif
    /* apsd trigger is completed */
    scb_psinfo->apsd_trig_cnt = 0;

    /* If APSD is used on all ACs then we can now clear the PSPoll response block. This frame
     * may have been result of pspoll receive, if not then that is also fine to clear the
     * PSP_ONRESP.
     */
    if (scb->apsd.ac_delv == AC_BITMAP_ALL) {
        scb_psinfo->psp_flags &= ~PS_PSP_ONRESP;
    }

#ifdef WLTAF
    if (wlc_taf_in_use(wlc->taf_handle) && (wlc_apps_apsd_delv_count(wlc, scb) > 0)) {
        /* If psq is empty, release new packets from ampdu or nar */
        if ((wlc_apps_psq_delv_count(wlc, scb) == 0) &&
            !wlc_taf_scheduler_blocked(wlc->taf_handle)) {
            /* Trigger a new TAF schedule cycle */
            wlc_taf_schedule(wlc->taf_handle, PKTPRIO(pkt), scb, FALSE);
        }
    }
#endif

    /* Send more frames until the End Of Service Period */
    if (scb_psinfo->apsd_cnt > 0 || scb_psinfo->ext_qos_null) {
        if (scb_psinfo->tx_block != 0) {
            WL_PS_EX(scb, ("tx blocked, cnt %u\n",
                scb_psinfo->apsd_cnt));
            return;
        }
#ifdef PROP_TXSTATUS
        if (!scb_psinfo->apsd_hpkt_timer_on)
#endif
            wlc_apps_apsd_send(wlc, scb);
        return;
    }

    wlc_apps_apsd_usp_end(wlc, scb);
}

void
wlc_apps_scb_tx_block(wlc_info_t *wlc, struct scb *scb, uint reason, bool block)
{
    struct apps_scb_psinfo *scb_psinfo;

    ASSERT(scb != NULL);

    WL_PS_EX(scb, ("block %d reason %d\n", block, reason));

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    if (block) {
        mboolset(scb_psinfo->tx_block, reason);
        /* terminate the APSD USP */
        scb_psinfo->apsd_usp = FALSE;
        scb_psinfo->apsd_cnt = 0;
#ifdef PROP_TXSTATUS
        scb_psinfo->apsd_tx_pending = FALSE;
#endif
    } else {
        mboolclr(scb_psinfo->tx_block, reason);
    }
}

int
wlc_apps_scb_apsd_cnt(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;

    ASSERT(scb != NULL);

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    WL_PS_EX(scb, ("apsd_cnt = %d\n",  scb_psinfo->apsd_cnt));

    return scb_psinfo->apsd_cnt;
}
#ifdef BCMPCIEDEV
/* Return the total v_pkts and v2r_pkts for the given scb */
static int
wlc_apps_sqs_vpkt_count(wlc_info_t *wlc, struct scb *scb)
{
    int tot_count;

    ASSERT(scb);

    tot_count = wlc_sqs_vpkts_tot(scb);    /* Virtual packets */
    tot_count += wlc_sqs_v2r_pkts_tot(scb);    /* Transient packets */

    return tot_count;
}
/*
 * Return the number of virtual packets  pending on delivery-enabled ACs.
 * Include both v_pkts and v2r_pkts
 */
static int
wlc_apps_apsd_delv_vpkt_count(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    int delv_count;

    if (scb->apsd.ac_delv == AC_BITMAP_NONE)
        return 0;

    /* PS buffered pkts are on the scb_psinfo->psq */
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    /* Virtual packets in host */
    delv_count = wlc_sqs_vpkts_multi_ac(scb,
        WLC_ACBITMAP_TO_TIDBITMAP(scb->apsd.ac_delv));

    /* Transient packets */
    delv_count += wlc_sqs_v2r_pkts_multi_ac(scb,
        WLC_ACBITMAP_TO_TIDBITMAP(scb->apsd.ac_delv));

    /* Real packets in dongle but not in psq */
    delv_count += scb_psinfo->apsd_v2r_in_transit;

    return delv_count;
}
/*
 * Return the number of virtual packets pending on non-delivery-enabled ACs.
 * Include both v_pkts and v2r_pkts
 */
static int
wlc_apps_apsd_ndelv_vpkt_count(wlc_info_t *wlc, struct scb *scb)
{
    ac_bitmap_t ac_non_delv;
    int ndelv_count;

    if (scb->apsd.ac_delv == AC_BITMAP_ALL)
        return 0;

    ac_non_delv = ~scb->apsd.ac_delv & AC_BITMAP_ALL;

    /* Virtual packets in host */
    ndelv_count = wlc_sqs_vpkts_multi_ac(scb,
        WLC_ACBITMAP_TO_TIDBITMAP(ac_non_delv));

    /* Transient packets */
    ndelv_count += wlc_sqs_v2r_pkts_multi_ac(scb,
        WLC_ACBITMAP_TO_TIDBITMAP(ac_non_delv));

    return ndelv_count;
}
#endif /* BCMPCIEDEV */

/*
 * Return the number of frames pending on delivery-enabled ACs.
 */
static int
wlc_apps_apsd_delv_count(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint32 precbitmap;
    int delv_count;

    if (scb->apsd.ac_delv == AC_BITMAP_NONE)
        return 0;

    precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(scb->apsd.ac_delv);

    /* PS buffered pkts are on the scb_psinfo->psq */
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    delv_count = APPS_PKTQ_MLEN(&scb_psinfo->psq, precbitmap);

    /* PS buffered pkts also need to account the packets in AMPDUq and NARq */
    delv_count += wlc_ampdu_scb_pktq_mlen(wlc->ampdu_tx, scb,
        WLC_ACBITMAP_TO_TIDBITMAP(scb->apsd.ac_delv));
    delv_count += wlc_nar_scb_pktq_mlen(wlc->nar_handle, scb, precbitmap);

    return delv_count;
}

/*
 * Return the number of frames pending on non-delivery-enabled ACs.
 */
static int
wlc_apps_apsd_ndelv_count(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    ac_bitmap_t ac_non_delv;
    uint32 precbitmap;
    int count;

    if (scb->apsd.ac_delv == AC_BITMAP_ALL)
        return 0;

    ac_non_delv = ~scb->apsd.ac_delv & AC_BITMAP_ALL;
    precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(ac_non_delv);

    /* PS buffered pkts are on the scb_psinfo->psq */
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    count = APPS_PKTQ_MLEN(&scb_psinfo->psq, precbitmap);

    /* PS buffered pkts also need to account the packets in AMPDUq and NARq */
    count += wlc_ampdu_scb_pktq_mlen(wlc->ampdu_tx, scb,
        WLC_ACBITMAP_TO_TIDBITMAP(ac_non_delv));
    count += wlc_nar_scb_pktq_mlen(wlc->nar_handle, scb, precbitmap);

    return count;
}

uint8
wlc_apps_apsd_ac_available(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint32 precbitmap;
    uint8 ac_bitmap = 0;
    struct pktq* q;            /**< multi-priority packet queue */

    /* PS buffered pkts are on the scb_psinfo->psq */
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    q = &scb_psinfo->psq;

    precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(1 << AC_BK);
    if (APPS_PKTQ_MLEN(q, precbitmap))
        ac_bitmap |= TDLS_PU_BUFFER_STATUS_AC_BK;

    precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(1 << AC_BE);
    if (APPS_PKTQ_MLEN(q, precbitmap))
        ac_bitmap |= TDLS_PU_BUFFER_STATUS_AC_BE;

    precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(1 << AC_VO);
    if (APPS_PKTQ_MLEN(q, precbitmap))
        ac_bitmap |= TDLS_PU_BUFFER_STATUS_AC_VO;

    precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(1 << AC_VI);
    if (APPS_PKTQ_MLEN(q, precbitmap))
        ac_bitmap |= TDLS_PU_BUFFER_STATUS_AC_VI;

    return ac_bitmap;
}

/* periodically check whether BC/MC queue needs to be flushed */
static void
wlc_apps_bss_wd_ps_check(void *handle)
{
    wlc_info_t *wlc = (wlc_info_t *)handle;
    struct scb *bcmc_scb;
    wlc_bsscfg_t *bsscfg;
    apps_bss_info_t *bss_info;
    uint i;

    if (wlc->excursion_active) {
        return;
    }

    if (wlc->clk) {
        wlc_apps_omi_waitpmq_war(wlc);
    }

    FOREACH_UP_AP(wlc, i, bsscfg) {
        bcmc_scb = WLC_BCMCSCB_GET(wlc, bsscfg);
        if (!bcmc_scb) {
            continue;
        }
        bss_info = APPS_BSSCFG_CUBBY(wlc->psinfo, bsscfg);
        ASSERT(bss_info);
        if ((SCB_PS(bcmc_scb) == TRUE) && (TXPKTPENDGET(wlc, TX_BCMC_FIFO) == 0) &&
            (!(bss_info->ps_trans_status & BSS_PS_ON_BY_TWT))) {
            if (MBSS_ENAB(wlc->pub)) {
                if (bsscfg->bcmc_fid_shm != INVALIDFID) {
                    WL_ERROR(("wl%d.%d: %s cfg(%p) bcmc_fid = 0x%x"
                        " bcmc_fid_shm = 0x%x, resetting bcmc_fids"
                        " tot pend %d mc_pkts %d\n",
                        wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
                        __FUNCTION__, bsscfg, bsscfg->bcmc_fid,
                        bsscfg->bcmc_fid_shm, TXPKTPENDTOT(wlc),
                        wlc_mbss_get_bcmc_pkts_sent(wlc, bsscfg)));
                }
                /* Reset the BCMC FIDs, no more pending bcmc packets */
                wlc_mbss_bcmc_reset(wlc, bsscfg);
            } else {
                BCMCFID(wlc, INVALIDFID);
            }

            if (bcmc_scb->ps_txfifo_blk) {
                WL_PS_EX(bcmc_scb, ("wlc_apps_bcmc_suppress_done \n"));
                wlc_apps_bcmc_suppress_done(wlc, bsscfg);
            }
            if (bsscfg->flags & WLC_BSSCFG_PS_OFF_TRANS) {
                wlc_apps_bcmc_ps_off_done(wlc, bsscfg);
            }
        }
    }
}

/*
 * Last STA has gone out of PS. If there are packets on the BCMC queue for this SCB then set txmod
 * to move packets to APPS (psq) and wait for this to clear. Otherwhise clear here.
 */
static void
wlc_apps_bcmc_ps_off_start(wlc_info_t *wlc, struct scb *bcmc_scb)
{
    wlc_bsscfg_t *bsscfg;

    bsscfg = bcmc_scb->bsscfg;
    ASSERT(bsscfg != NULL);

    WL_PS0(("wl%d.%d: %s BCMC PS off start, in flight %d\n", wlc->pub->unit,
        WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(bcmc_scb)));

    if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(bcmc_scb) == 0) {
        /* No pkts in BCMC fifo */
        if (bcmc_scb->ps_txfifo_blk) {
            WL_PS_EX(bcmc_scb, ("wlc_apps_bcmc_suppress_done \n"));
            wlc_apps_bcmc_suppress_done(wlc, bsscfg);
        }
        wlc_apps_bcmc_ps_off_done(wlc, bsscfg);
    } else { /* Mark in transition */
        ASSERT(bcmc_scb->PS); /* Should only have BCMC pkts if in PS */
        WL_PS_EX(bcmc_scb, ("START PS-OFF. last fid 0x%x. shm fid 0x%x\n",
            bsscfg->bcmc_fid, bsscfg->bcmc_fid_shm));
        wlc_txmod_config(wlc->txmodi, bcmc_scb, TXMOD_APPS);
        bsscfg->flags |= WLC_BSSCFG_PS_OFF_TRANS;
    }
}

void
wlc_apps_dotxstatus_bcmc(wlc_info_t *wlc, wlc_bsscfg_t *cfg, tx_status_t *txs)
{
    struct scb *bcmc_scb;

#ifdef MBSS
    if (MBSS_ENAB(wlc->pub) && txs) {
        wlc_mbss_dotxstatus_mcmx(wlc, cfg, txs);
    }
#endif
    bcmc_scb = WLC_BCMCSCB_GET(wlc, cfg);
    ASSERT(bcmc_scb);

    if (!SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(bcmc_scb)) {
        if (bcmc_scb->ps_txfifo_blk) {
            WL_PS_EX(bcmc_scb, ("wlc_apps_bcmc_suppress_done \n"));
            wlc_apps_bcmc_suppress_done(wlc, cfg);
        }
        if (cfg->flags & WLC_BSSCFG_PS_OFF_TRANS) {
            wlc_apps_bcmc_ps_off_done(wlc, cfg);
        }
    }
}

static void
wlc_apps_release_bcmc(wlc_info_t *wlc, scb_t *bcmc_scb)
{
    /* Remove the PMQ entry for the BSSCFG from AuxPMQ */
    wlc_pmq_process_switch(wlc, bcmc_scb, PMQ_REMOVE);

    wlc_apps_scb_psq_norm(wlc, bcmc_scb);

#ifdef HNDPQP
    /* Page in all Host resident packets into dongle */
    wlc_apps_scb_pqp_pgi(wlc, bcmc_scb, WLC_PREC_BMP_ALL, PKTQ_PKTS_ALL, PQP_PGI_PS_BCMC_OFF);
#endif  /* HNDPQP */

    /* Forward any packets in MC-PSQ according to new state */
    wlc_apps_ps_send(wlc, bcmc_scb, WLC_PREC_BMP_ALL, FALSE, NULL, WLC_APPS_RELEASE_ALL);

    /* Remove APPS to the txpath for this SCB */
    wlc_txmod_unconfig(wlc->txmodi, bcmc_scb, TXMOD_APPS);
}

/*
 * When BCMC suppression is complete this function is to be called. BCMC suppression starts upon
 * first STA entering PS, where BCMC packets are still on BE. In that case the
 * bcmc_scb->ps_txfifo_blk will be set. Once suppression has completed all BCMC packets can be
 * released. First the txq needs normalisation. After releaseing the packets the txmod can be
 * updated so BCMC can flow directly into CQ again.
 */
void
wlc_apps_bcmc_suppress_done(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    scb_t *bcmc_scb;
    struct apps_scb_psinfo *scb_psinfo;

    bcmc_scb = WLC_BCMCSCB_GET(wlc, bsscfg);
    ASSERT(bcmc_scb);

    ASSERT(bcmc_scb->ps_txfifo_blk);
    bcmc_scb->ps_txfifo_blk = FALSE;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, bcmc_scb);
    ASSERT(scb_psinfo);
    BCM_REFERENCE(scb_psinfo);
    WL_PS0(("wl%d.%d: %s BCMC PS on done (%d/%d)\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
        __FUNCTION__, scb_psinfo->suppressed_pkts, pktq_n_pkts_tot(&scb_psinfo->psq)));
#ifdef MBSS
    if (MBSS_ENAB(wlc->pub)) {
        wlc_mbss_increment_ps_trans_cnt(wlc, bsscfg);
    }
#endif
    wlc_apps_release_bcmc(wlc, bcmc_scb);
}

/*
 * Last STA for a BSS exitted PS; BSS has no pkts in BC/MC fifo.
 * Check whether other stations have entered PS since and update
 * state accordingly.
 */
static void
wlc_apps_bcmc_ps_off_done(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
    struct scb *bcmc_scb;

    WL_PS0(("wl%d.%d: %s BCMC PS off done\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
        __FUNCTION__));
    bsscfg->flags &= ~WLC_BSSCFG_PS_OFF_TRANS; /* Clear transition flag */

    if (!(bcmc_scb = WLC_BCMCSCB_GET(wlc, bsscfg))) {
        return;
    }

    ASSERT(SCB_PS(bcmc_scb));

    ASSERT(BSS_PS_NODES(wlc->psinfo, bsscfg) == 0);

    /* Completed transition: Clear PS delivery mode */
    bcmc_scb->PS = FALSE;
    wlc_apps_upd_pstime(bcmc_scb);
#ifdef MBSS
    if (MBSS_ENAB(wlc->pub)) {
        wlc_mbss_increment_ps_trans_cnt(wlc, bsscfg);
    }
#endif

    wlc_apps_release_bcmc(wlc, bcmc_scb);
}

/*
 * Return the bitmap of ACs with buffered traffic.
 */
uint8
wlc_apps_apsd_ac_buffer_status(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint32 precbitmap;
    uint8 ac_bitmap = 0;
    struct pktq *q;            /**< multi-priority packet queue */
    int i;

    /* PS buffered pkts are on the scb_psinfo->psq */
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    q = &scb_psinfo->psq;

    for (i = 0; i < AC_COUNT; i++) {
        precbitmap = WLC_ACBITMAP_TO_PRECBITMAP(1 << i);

        if (APPS_PKTQ_MLEN(q, precbitmap))
            AC_BITMAP_SET(ac_bitmap, i);
    }

    return ac_bitmap;
}

/* TIM */
static uint
wlc_apps_calc_tim_ie_len(void *ctx, wlc_iem_calc_data_t *data)
{
    wlc_info_t *wlc = (wlc_info_t *)ctx;
    wlc_bsscfg_t *cfg = data->cfg;

    if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype) && !BSSCFG_IS_MAIN_AP(cfg)) {
        return 0;
    }

    if (BSSCFG_AP(cfg))
        return wlc_apps_tim_len(wlc, cfg);

    return 0;
}

static int
wlc_apps_write_tim_ie(void *ctx, wlc_iem_build_data_t *data)
{
    wlc_info_t *wlc = (wlc_info_t *)ctx;
    wlc_bsscfg_t *cfg = data->cfg;

    if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype) && !BSSCFG_IS_MAIN_AP(cfg)) {
        return BCME_ERROR;
    }
    if (BSSCFG_AP(cfg)) {
        wlc_apps_tim_create(wlc, cfg, data->buf, data->buf_len);

        data->cbparm->ft->bcn.tim_ie = data->buf;

#ifdef MBSS
        if (MBSS_ENAB(wlc->pub)) {
            wlc_mbss_set_bcn_tim_ie(wlc, cfg, data->buf);
        }
#endif /* MBSS */
    }

    return BCME_OK;
}

static void
wlc_apps_scb_state_upd_cb(void *ctx, scb_state_upd_data_t *notif_data)
{
    wlc_info_t *wlc = (wlc_info_t *)ctx;
    struct scb *scb;
    uint8 oldstate;

    ASSERT(notif_data != NULL);

    scb = notif_data->scb;
    ASSERT(scb != NULL);
    oldstate = notif_data->oldstate;

    if (BSSCFG_AP(scb->bsscfg) && (oldstate & ASSOCIATED) && !SCB_ASSOCIATED(scb)) {
        if (SCB_PS(scb) && !SCB_ISMULTI(scb)) {
            WL_PS_EX(scb, ("SCB disassociated, take it out of PS\n"));
    /* dump_flag_qqdx */
#ifdef dump_stack_ps_qqdx_print
    printk(KERN_ALERT"----------[fyl] wlc_apps_scb_state_upd_cb (%u)wlc_apps_scb_ps_off----------",OSL_SYSUPTIME());
#endif /*dump_stack_ps_qqdx_print*/
            wlc_apps_scb_ps_off(wlc, scb, TRUE);
        }
    }
}

static void
wlc_apps_bss_updn(void *ctx, bsscfg_up_down_event_data_t *evt)
{
    wlc_bsscfg_t *bsscfg = evt->bsscfg;
    wlc_info_t *wlc = bsscfg->wlc;
    struct scb *scb;
    struct scb_iter scbiter;
    struct apps_scb_psinfo *scb_psinfo;

    if (!evt->up) {
        FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
            scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

            if (scb_psinfo == NULL)
                continue;

    /* dump_flag_qqdx */
#ifdef dump_stack_ps_qqdx_print
    printk(KERN_ALERT"----------[fyl] wlc_apps_bss_updn (%u)wlc_apps_scb_ps_off----------",OSL_SYSUPTIME());
#endif /*dump_stack_ps_qqdx_print*/
            if (SCB_PS(scb) && !SCB_ISMULTI(scb))
                wlc_apps_scb_ps_off(wlc, scb, TRUE);
            else if (!pktq_empty(&scb_psinfo->psq))
                wlc_apps_ps_flush(wlc, scb);
        }

        if (BSSCFG_HAS_BCMC_SCB(bsscfg)) {
            wlc_apps_ps_flush(wlc, WLC_BCMCSCB_GET(wlc, bsscfg));
            if (MBSS_ENAB(wlc->pub)) {
                wlc_mbss_bcmc_reset(wlc, bsscfg);
            }
        }
    }
}

void
wlc_apps_ps_trans_upd(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

#if defined(WL_PS_SCB_TXFIFO_BLK)
    ASSERT(SCB_PS_TXFIFO_BLK(scb) ||
        ((wlc->block_datafifo & DATA_BLOCK_PS) && !AUXPMQ_ENAB(wlc->pub)));
#else /* ! WL_PS_SCB_TXFIFO_BLK */
    ASSERT(wlc->block_datafifo & DATA_BLOCK_PS);
#endif /* ! WL_PS_SCB_TXFIFO_BLK */

    /* We have received an ack in pretend state and are free to exit */
    WL_PS_EX(scb, ("received successful txstatus in threshold "
        "ps pretend active state\n"));
    ASSERT(scb->PS);
    scb_psinfo->ps_trans_status |= SCB_PS_TRANS_OFF_PEND;
}

/** @return   Multi-priority packet queue */
struct pktq *
wlc_apps_get_psq(wlc_info_t * wlc, struct scb * scb)
{
    if ((wlc == NULL) ||
        (scb == NULL) ||
        (SCB_PSINFO(wlc->psinfo, scb) == NULL)) {
        return NULL;
    }

    return &SCB_PSINFO(wlc->psinfo, scb)->psq;
}

void
wlc_apps_map_pkts(wlc_info_t *wlc, struct scb *scb, map_pkts_cb_fn cb, void *ctx)
{
    struct pktq *pktq;        /**< multi-priority packet queue */

#ifdef HNDPQP
    BCM_REFERENCE(pktq);
    /* Page in all Host resident packets into dongle */
    wlc_apps_scb_pqp_map_pkts(wlc, scb, cb, ctx);
#else /* !HNDPQP */

#ifdef WL_USE_SUPR_PSQ
    {
        int prec;
        struct apps_scb_psinfo *scb_psinfo;

        scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

        /* The supr_psq should not be prepended to psq in wlc_apps_map_pkts using
         * wlc_apps_dequeue_scb_supr_psq, as that call can lead to packets being freed
         * and may result in packet callback (which in turn can release new packets).
         */
        if ((scb_psinfo != NULL) &&
            (scb_psinfo->suppressed_pkts > 0)) {
            for (prec = (WLC_PREC_COUNT-1); prec >= 0; prec--) {
                wlc_scb_supp_q_map_pkts(wlc, &scb_psinfo->supr_psq[prec], cb, ctx);
            }
        }
    }
#endif /* WL_USE_SUPR_PSQ */

    if ((pktq = wlc_apps_get_psq(wlc, scb)))
        wlc_scb_psq_map_pkts(wlc, pktq, cb, ctx);
#endif /* !HNDPQP */
}

void
wlc_apps_set_change_scb_state(wlc_info_t *wlc, struct scb *scb, bool reset)
{
    apps_wlc_psinfo_t *wlc_psinfo;
    struct apps_scb_psinfo *scb_psinfo;

    wlc_psinfo = wlc->psinfo;
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    scb_psinfo->change_scb_state_to_auth = reset;
}

#ifdef WLTAF
uint16 BCMFASTPATH
wlc_apps_get_taf_scb_pktq_tot(wlc_info_t *wlc, scb_t* scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    int total;

    if (!wlc || !scb) {
        return 0;
    }
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo) {
        return 0;
    }

    total = pktq_n_pkts_tot(&scb_psinfo->psq);
#ifdef WL_USE_SUPR_PSQ
    total += scb_psinfo->suppressed_pkts;
#endif /* WL_USE_SUPR_PSQ */

    return total;
}

void * BCMFASTPATH
wlc_apps_get_taf_scb_info(void *appsh, struct scb* scb)
{
    apps_wlc_psinfo_t *wlc_psinfo = (apps_wlc_psinfo_t *)appsh;

    return (wlc_psinfo && scb) ? (void*)SCB_PSINFO(wlc_psinfo, scb) : NULL;
}

void * BCMFASTPATH
wlc_apps_get_taf_scb_tid_info(void *scb_h, int tid)
{
    return TAF_PARAM(tid);
}

uint16 BCMFASTPATH
wlc_apps_get_taf_scb_tid_pktlen(void *appsh, void *scbh, void *tidh, uint32 ts)
{
    int prio = (int)(size_t)tidh;
    struct apps_scb_psinfo * scb_psinfo = (struct apps_scb_psinfo *)scbh;
    int prec, hiprec;

    if (!scb_psinfo) {
        return 0;
    }

    prec = WLC_PRIO_TO_PREC(prio);
    hiprec = WLC_PRIO_TO_HI_PREC(prio);
#ifdef WL_USE_SUPR_PSQ
    return APPS_PKTQ_PREC_N_PKTS(&scb_psinfo->psq, prec) +
        APPS_PKTQ_PREC_N_PKTS(&scb_psinfo->psq, hiprec) +
        APPS_SPKTQ_N_PKTS(&scb_psinfo->supr_psq[prec]) +
        APPS_SPKTQ_N_PKTS(&scb_psinfo->supr_psq[hiprec]);
#else
    return pktqprec_n_pkts(&scb_psinfo->psq, prec) +
        pktqprec_n_pkts(&scb_psinfo->psq, hiprec);
#endif
}

static void
wlc_apps_taf_tag_release(wlc_info_t *wlc, taf_scheduler_public_t *taf, void *p, scb_t *scb)
{
    uint32 taf_pkt_tag;
    uint32 pktbytes = pkttotlen(wlc->osh, p);
    uint32 taf_pkt_time_units = TAF_PKTBYTES_TO_UNITS((uint16)pktbytes,
        taf->ias.pkt_rate, taf->ias.byte_rate);

    BCM_REFERENCE(scb);
    ASSERT(!taf->ias.is_ps_mode);

    if (taf_pkt_time_units == 0) {
        taf_pkt_time_units = 1;
    }

    taf->ias.actual.released_bytes += (uint16)pktbytes;

    if ((WLF2_PCB1(p) & WLF2_PCB1_NAR) == WLF2_PCB1_NAR ||
        (WLF2_PCB4(p) & WLF2_PCB4_AMPDU) == WLF2_PCB4_AMPDU) {
        taf_pkt_tag = TAF_UNITS_TO_PKTTAG(taf_pkt_time_units);
        taf->ias.actual.released_units += TAF_PKTTAG_TO_UNITS(taf_pkt_tag);
    } else if (WLPKTTAG(p)->flags & WLF_CTLMGMT) {
        taf_pkt_tag = TAF_PKTTAG_PS;
    } else {
        WL_TAFF(wlc, "%s: "MACF" No NAR/AMPDU callback pktflags:0x%x flags2:0x%x\n",
            __FUNCTION__, ETHER_TO_MACF(scb->ea), WLPKTTAG(p)->flags,
            WLPKTTAG(p)->flags2);
        taf_pkt_tag = TAF_PKTTAG_PS;
    }

    taf->ias.actual.release++;
    TAF_SET_TAG_IDX(WLPKTTAG(p), taf->ias.index);
    TAF_SET_TAG_UNITS(WLPKTTAG(p), taf_pkt_tag);
    TAF_SET_TAG(WLPKTTAG(p));
}

bool
wlc_apps_taf_release(void* appsh, void* scbh, void* tidh, bool force, taf_scheduler_public_t* taf)
{
    apps_wlc_psinfo_t *wlc_psinfo = (apps_wlc_psinfo_t *)appsh;
    apps_scb_psinfo_t *scb_psinfo = (apps_scb_psinfo_t *)scbh;
    int prio = (int)(size_t)tidh;
    bool taf_released = FALSE;
    wlc_info_t *wlc;
    scb_t *scb;
    uint prec_bmp;

    if (!scb_psinfo) {
        WL_ERROR(("%s: no cubby!\n", __FUNCTION__));
        taf->complete = TAF_REL_COMPLETE_ERR;
        return FALSE;
    }
    if (taf->how != TAF_RELEASE_LIKE_IAS) {
        ASSERT(0);
        taf->complete = TAF_REL_COMPLETE_ERR;
        return FALSE;
    }

    /* TAF can try to release from SCB even though it has been put into PS, the release should
     * be ignored. Packets would get lost.
     */
    if (taf->ias.is_ps_mode) {
        taf->complete = TAF_REL_COMPLETE_PS;
        taf->ias.was_emptied = TRUE;
        return FALSE;
    }

    scb = scb_psinfo->scb;
    wlc = wlc_psinfo->wlc;

    prec_bmp = NBITVAL(WLC_PRIO_TO_HI_PREC(prio)) |    NBITVAL(WLC_PRIO_TO_PREC(prio));

#ifdef HNDPQP
    /* If the psq is still owned by PQP,  page in all Host resident packets into dongle
     * before release the packets from psq.
     */
    wlc_apps_scb_pqp_pgi(wlc, scb, prec_bmp, PKTQ_PKTS_ALL, PQP_PGI_PS_TAF_REL);
#endif /* HNDPQP */

    /* XXX: We have to do full release for now. Partial release is not working
     * yet, as it can result in suppress while in release, and that is not yet
     * handled properly.
     */
    wlc_apps_ps_send(wlc, scb, prec_bmp, FALSE, taf, WLC_APPS_RELEASE_ALL);
    taf_released = TRUE;

    taf->ias.was_emptied = TRUE;
    taf->complete = TAF_REL_COMPLETE_EMPTIED;

    return taf_released;
}
#endif /* WLTAF */

#ifdef WL_PS_STATS
static void
wlc_apps_ps_stats_datablock_start(wlc_info_t *wlc)
{
    /* only update time when previous DATA_BLOCK_PS is off */
    if (wlc->datablock_starttime == 0) {
        int pktpend_cnts = TXPKTPENDTOT(wlc);

        wlc->datablock_cnt++;
        wlc->datablock_starttime = OSL_SYSUPTIME_US();

        if ((wlc->pktpend_min == 0) || (wlc->pktpend_min > pktpend_cnts)) {
            wlc->pktpend_min = pktpend_cnts;
        }

        if (wlc->pktpend_max < pktpend_cnts) {
            wlc->pktpend_max = pktpend_cnts;
        }

        wlc->pktpend_tot += pktpend_cnts;
    }
}

static void
wlc_apps_ps_stats_datablock_stop(wlc_info_t *wlc)
{
    if (wlc->datablock_starttime != 0) {
        uint32 datablock_delta;
        uint32 datablock_now = OSL_SYSUPTIME_US();

        datablock_delta = datablock_now - wlc->datablock_starttime;
        if (datablock_delta > wlc->datablock_maxtime) {
            wlc->datablock_maxtime = datablock_delta;
        }

        if ((wlc->datablock_mintime == 0) ||
                (wlc->datablock_mintime > datablock_delta)) {
            wlc->datablock_mintime = datablock_delta;
        }

        wlc->datablock_tottime += datablock_delta;
        wlc->datablock_starttime = 0;
    }
}

void
wlc_apps_upd_pstime(struct scb *scb)
{
    uint32 ps_delta;
    uint32 ps_now = OSL_SYSUPTIME();
    /* in case ps_starttime was cleared */
    if (scb->ps_starttime == 0) {
        scb->ps_on_cnt = 0;
    } else {
        ps_delta = ps_now - scb->ps_starttime;
        if (ps_delta > scb->ps_maxtime)
            scb->ps_maxtime = ps_delta;

        if ((scb->ps_mintime == 0) || (scb->ps_mintime > ps_delta))
            scb->ps_mintime = ps_delta;

        /* Update STA PS duration histogram */
        PS_UPD_HISTOGRAM(ps_delta, scb);

        /* Update total duraion */
        scb->ps_tottime += ps_delta;
        scb->ps_starttime = 0;
    }
}

static int
wlc_apps_stats_dump(void *ctx, struct bcmstrbuf *b)
{
    int i, j, k;
    wlc_info_t *wlc = ctx;
    int32 ps_cnt;
    int32 datablock_cnts;
    struct apps_scb_psinfo *scb_psinfo;
    struct scb *scb;
    struct scb_iter scbiter;
    wlc_bsscfg_t *bsscfg;

    if ((wlc->block_datafifo) && (wlc->datablock_cnt > 0))
        datablock_cnts = wlc->datablock_cnt - 1;
    else
        datablock_cnts = wlc->datablock_cnt;

    if (datablock_cnts > 0) {
        bcm_bprintf(b, "data_block_ps %d cnt %d dur avg:%u (min:%u max:%u) usec, "
            "pktpend avg:%u (min:%u max:%u), ",
            wlc->block_datafifo, wlc->datablock_cnt,
            wlc->datablock_tottime/(uint32)datablock_cnts,
            wlc->datablock_mintime, wlc->datablock_maxtime,
            wlc->pktpend_tot/(uint32)datablock_cnts,
            wlc->pktpend_min, wlc->pktpend_max);
    } else {
        bcm_bprintf(b, "data_block_ps 0 cnt 0 avg:0 (min:0 max:0) usec, "
            "pktpend avg:0 (min:0 max:0), ");
    }
#if defined(WL_PS_SCB_TXFIFO_BLK)
    bcm_bprintf(b, "scb_blk %d cnt %d ",
        wlc->ps_scb_txfifo_blk, wlc->ps_txfifo_blk_scb_cnt);
#endif /* WL_PS_SCB_TXFIFO_BLK */
    bcm_bprintf(b, "\n");

    /* Look through all SCBs and display relevant PS info */
    i = j = k = 0;  /* i=BSSs, j=BSS.SCBs, k=total scbs */
    bcm_bprintf(b, "SCB: hist bins: [0..200ms 200ms..500ms 500ms..2sec 2sec..5sec 5sec..]\n");
    FOREACH_BSS(wlc, i, bsscfg) {
        j = 0;
        FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
            if ((scb->PS || scb->PS_TWT) && (scb->ps_on_cnt > 0))
                ps_cnt = scb->ps_on_cnt - 1;
            else
                ps_cnt = scb->ps_on_cnt;

            if (ps_cnt > 0) {
                bcm_bprintf(b, " %2d.%3d "MACF"%s ps %d ps_on %d dur avg:%u "
                    "(min:%u max:%u) msec, hist:[%u %u %u %u %u], supr %u",
                    i, j, ETHER_TO_MACF(scb->ea), (scb->permanent ? "*":" "),
                    (scb->PS || scb->PS_TWT), scb->ps_on_cnt,
                    scb->ps_tottime/(uint32)ps_cnt,
                    scb->ps_mintime, scb->ps_maxtime,
                    scb->ps_dur0_200, scb->ps_dur200_500, scb->ps_dur500_2,
                    scb->ps_dur2_5, scb->ps_dur5, scb->suprps_cnt);
            } else {
                bcm_bprintf(b, " %2d.%3d "MACF"%s ps %d ps_on 0 dur avg:0 "
                    "(min:0 max:0) msec, hist:[0 0 0 0 0], supr %u",
                    i, j, ETHER_TO_MACF(scb->ea), (scb->permanent ? "*":" "),
                    (scb->PS || scb->PS_TWT), scb->suprps_cnt);
            }

            if (scb->PS) {
                scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
                ps_cnt = pktq_n_pkts_tot(&scb_psinfo->psq);

                bcm_bprintf(b, " psqlen %u\n", ps_cnt);
            } else {
                bcm_bprintf(b, "\n");
            }
            j++;
            k++;
        }
    }

    if (k == 0) {
        bcm_bprintf(b, " %s\n", "No SCBs");
    }

    bcm_bprintf(b, "BSSCFG: \n");
    FOREACH_UP_AP(wlc, i, bsscfg) {
        scb = WLC_BCMCSCB_GET(wlc, bsscfg);
        if (scb->PS && (scb->ps_on_cnt > 0))
            ps_cnt = scb->ps_on_cnt - 1;
        else
            ps_cnt = scb->ps_on_cnt;

        if (ps_cnt > 0) {
            bcm_bprintf(b, " "MACF" ps %d ps_on %d dur avg:%u (min:%u max:%u) msec, "
                "bcmc_supr %u",
                ETHER_TO_MACF(bsscfg->BSSID), scb->PS, scb->ps_on_cnt,
                scb->ps_tottime/(uint32)ps_cnt,
                scb->ps_mintime, scb->ps_maxtime, scb->suprps_cnt);
        } else {
            bcm_bprintf(b, " "MACF" ps %d ps_on 0 dur avg:0 (min:0 max:0) msec, "
                "bcmc_supr %u",
                ETHER_TO_MACF(bsscfg->BSSID), scb->PS, scb->suprps_cnt);
        }
        bcm_bprintf(b, "\n");
    }
    return BCME_OK;
}

static int
wlc_apps_stats_dump_clr(void *ctx)
{
    wlc_info_t *wlc = ctx;
    struct scb *scb;
    struct scb_iter scbiter;
    wlc_bsscfg_t *bsscfg;
    int i;

    FOREACHSCB(wlc->scbstate, &scbiter, scb) {
        scb->ps_on_cnt = 0;
        scb->ps_mintime = 0;
        scb->ps_maxtime = 0;
        scb->ps_tottime = 0;
        scb->ps_starttime = 0;
        scb->suprps_cnt = 0;
        scb->ps_dur0_200 = 0;
        scb->ps_dur200_500 = 0;
        scb->ps_dur500_2 = 0;
        scb->ps_dur2_5 = 0;
        scb->ps_dur5 = 0;
    }

    FOREACH_UP_AP(wlc, i, bsscfg) {
        scb = WLC_BCMCSCB_GET(wlc, bsscfg);
        scb->ps_on_cnt = 0;
        scb->ps_mintime = 0;
        scb->ps_maxtime = 0;
        scb->ps_tottime = 0;
        scb->ps_starttime = 0;
        scb->suprps_cnt = 0;
        scb->ps_dur0_200 = 0;
        scb->ps_dur200_500 = 0;
        scb->ps_dur500_2 = 0;
        scb->ps_dur2_5 = 0;
        scb->ps_dur5 = 0;
    }
    wlc->datablock_starttime = 0;
    wlc->datablock_cnt = 0;
    wlc->datablock_mintime = 0;
    wlc->datablock_maxtime = 0;
    wlc->datablock_tottime = 0;
    wlc->pktpend_min = 0;
    wlc->pktpend_max = 0;
    wlc->pktpend_tot = 0;

    return BCME_OK;
}
#endif /* WL_PS_STATS */

#ifdef WLCFP
#if defined(AP) && defined(BCMPCIEDEV)
void
wlc_apps_scb_psq_resp(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    _wlc_apps_scb_psq_resp(wlc, scb, scb_psinfo);
}

void
wlc_apps_scb_apsd_tx_pending(wlc_info_t *wlc, struct scb *scb, uint32 extra_flags)
{
    struct apps_wlc_psinfo *wlc_psinfo;
    struct apps_scb_psinfo *scb_psinfo;

    wlc_psinfo = wlc->psinfo;
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);

    if (extra_flags & WLF_APSD)
        scb_psinfo->apsd_tx_pending = TRUE;
}
#endif /* AP && PCIEDEV */

void
wlc_cfp_apps_ps_send(wlc_info_t *wlc, struct scb *scb, void *pkt, uint8 prio)
{
    wlc_txq_info_t *qi = SCB_WLCIFP(scb)->qi;

#ifdef BCMPCIEDEV
    wlc_apps_scb_apsd_dec_v2r_in_transit(wlc, scb, pkt, WLC_PRIO_TO_PREC(prio));
#endif /* BCMPCIEDEV */

    /* register WLF2_PCB2_PSP_RSP for pkt */
    WLF2_PCB2_REG(pkt, WLF2_PCB2_PSP_RSP);

    /* Ensure the pkt marker (used for ageing) is cleared */
    WLPKTTAG(pkt)->flags &= ~WLF_PSMARK;

    WL_PS_EX(scb, ("ps_enq_resp %p supr %d apsd %d\n",
           OSL_OBFUSCATE_BUF(pkt),
           (WLPKTTAG(pkt)->flags & WLF_TXHDR) ? 1 : 0,
           (WLPKTTAG(pkt)->flags & WLF_APSD) ? 1 : 0));

    /* Enqueue the PS-Poll response at higher precedence level */
    if (!cpktq_prec_enq(wlc, &qi->cpktq, pkt, WLC_PRIO_TO_HI_PREC(prio), FALSE)) {
        WL_ERROR(("wl%d.%d: %s: txq full, frame discarded\n",
                  wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)), __FUNCTION__));
        PKTFREE(wlc->osh, pkt, TRUE);
        wlc_apps_apsd_usp_end(wlc, scb);
        return;
    }

    /* Send to hardware (latency for first APSD-delivered frame is especially important) */
    wlc_send_q(wlc, qi);
}
#endif /* WLCFP */

/* Return avaiable real packets in the system for the given flow */
static uint16
wlc_apps_scb_pktq_tot(wlc_info_t *wlc, struct scb *scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    int pktq_total;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    ASSERT(scb_psinfo);

    /* get the count of pkts buffered for the scb */
    pktq_total = pktq_n_pkts_tot(&scb_psinfo->psq);

#ifdef WL_USE_SUPR_PSQ
    pktq_total += scb_psinfo->suppressed_pkts;
#endif

    /* PS buffered pkts also need to account the packets in AMPDUq and NARq */
    pktq_total += wlc_ampdu_scb_txpktcnt(wlc->ampdu_tx, scb);
    pktq_total += wlc_nar_scb_txpktcnt(wlc->nar_handle, scb);

    return pktq_total;
}

#if defined(HNDPQP)
/* PQP integration for APPS module. */

/* Check if a given scb->psq:prec is owned by PQP */
bool
wlc_apps_scb_pqp_owns(wlc_info_t* wlc, struct scb* scb, uint16 prec_bmp)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct pktq*    pq;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo)
        return FALSE;

    pq = &scb_psinfo->psq;

    ASSERT(pq);

    return (pktq_mpqp_owns(pq, prec_bmp));
}
/* PQP Page out callback called on every successfull page out completion */
static int
wlc_apps_pqp_pgo_cb(pqp_cb_t* cb)
{
    void* pkt;
    pkt = (void*)cb->pqp_pkt;

    /* Free the dongle Packet now */
    ASSERT(pkt);

#if defined(PQP_USE_MGMT_LOCAL)
    /* Do not free mgmt local buffer */
    if (PKTISMGMTTXPKT(cb->osh, pkt))
        return 0;
#endif

    /* PQP specific PKTFREE where all packet callbacks are skipped */
    PQP_PKTFREE(cb->osh, pkt);

    return 0;
}

/* Calcaulte the pktq prec */
static INLINE uint16
wlc_apps_pqp_psq_ptr2idx(struct pktq *psq, void *pktq_ptr)
{
    uint32 pktq_idx = (uint32)
        (((uintptr)pktq_ptr - (uintptr)psq) / sizeof(pktq_prec_t));
    ASSERT(pktq_idx < WLC_PREC_COUNT);
    return pktq_idx;
}

/* PQP Page IN callback called on page in of requested packets */
static int
wlc_apps_pqp_pgi_cb(pqp_cb_t* cb)
{
    wlc_info_t *wlc;
    struct scb *scb;
    struct apps_scb_psinfo *scb_psinfo;

    struct pktq *pq;
    pqp_pktq_t  *pqp_pktq;
    bool apsd;
    int prec;
    uint prec_bmp;

    scb_psinfo = (struct apps_scb_psinfo *)cb->ctx;
    ASSERT(scb_psinfo);
    pqp_pktq = cb->pqp_pktq;

    wlc = scb_psinfo->wlc;
    scb = scb_psinfo->scb;
    pq = &scb_psinfo->psq;

    if (PQP_PGI_PS_TRANS(scb_psinfo) == 0) {
        return 0;
    }

    apsd = PQP_PGI_PS_EXTRAFLAG(scb_psinfo) ? TRUE : FALSE;
    prec = wlc_apps_pqp_psq_ptr2idx(pq, pqp_pktq);
    prec_bmp = (1 << prec);

    WL_PS_EX(scb, ("SCB flowid: %d  prec %d ps_trans 0x%x\n",
        SCB_FLOWID(scb), prec, PQP_PGI_PS_TRANS(scb_psinfo)));

    /* pq->hi_prec will be changed in pktq_mdeq(),
     * If last pktq_pqp_pgi() didn't page-in all pkts in pktq and
     * then called wlc_apps_ps_send(). pq->hi_prec will be set to 0.
     * hi_prec should be re-assigned before pktq_mdeq().
     * Set hi_prec start from prec.
     */
    if (pq->hi_prec < prec)
        pq->hi_prec = prec;

    if (PQP_PGI_PS_SEND_ISSET(scb_psinfo)) {
        PQP_PGI_PS_TRANS_PEND_CLR(scb_psinfo);

        wlc_apps_ps_send(wlc, scb, prec_bmp, apsd, NULL, 1);

        return 0;
    } else if (PQP_PGI_PS_TRANS_OFF_ISSET(scb_psinfo)) {
#ifdef WLTAF
        /* If taf is not enabled then we need to release the data in the old way */
        if (((PQP_PGI_PS_TRANS(scb_psinfo)) & PQP_PGI_PS_BCMC_OFF) ||
            (!wlc_taf_psq_in_use(wlc->taf_handle, NULL)))
#endif /* WLTAF */
        {
            wlc_apps_ps_send(wlc, scb, prec_bmp, apsd, NULL, WLC_APPS_RELEASE_ALL);
        }
    }

    if (!pktq_mpqp_owns(pq, PQP_PGI_PS_PRECMAP(scb_psinfo))) {
        WL_PS_EX(scb, ("pgi_ps_trans 0x%x finished\n",
            PQP_PGI_PS_TRANS(scb_psinfo)));

        if ((PQP_PGI_PS_TRANS(scb_psinfo)) & PQP_PGI_PS_OFF) {
            wlc_apps_scb_ps_off_ps_pgi_done(wlc, scb, scb_psinfo);
        }

        PQP_PGI_PS_TRANS_PEND_CLR(scb_psinfo);
    }

    return 0;
}

void
wlc_apps_scb_pqp_pgi(wlc_info_t* wlc, struct scb* scb, uint16 prec_bmp, int fill_pkts,
    uint16 ps_trans)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct pktq*    pq;
    int prec;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (!scb_psinfo)
        return;

    pq = &scb_psinfo->psq;
    if (!pq)
        return;

    if (!pktq_mpqp_owns(pq, prec_bmp)) {
        return;
    }

    WL_PS_EX(scb, ("SCB flowid: %d  prec bmp 0x%x ps_trans 0x%x\n",
        SCB_FLOWID(scb), prec_bmp, ps_trans));

    PKTQ_PREC_ITER(pq, prec) {
        if ((prec_bmp & (1 << prec)) == 0)
            continue;

        /* PQP Page in all the packets for a given prec */
        /* If PGI resource is out, it will resume when resource is avaiable.
         * Set cont_pkts to 1 to explicitly invoke QCB to
         * free more resource for next resource resume callback.
         */
        pktq_pqp_pgi(pq, prec, 1, fill_pkts);
    }

    /* Expected to page in the full queue. Check the status */
    if (pktq_mpqp_owns(pq, prec_bmp)) {
        WL_PS_EX(scb, ("PQP PGI waiting for resources : SCB flow: %d "
            "prec 0x%x ::: <Called from 0x%p>\n",
            SCB_FLOWID(scb), prec_bmp, CALL_SITE));

        /* If PQP PGI wait for resource then set flag to resume the PGI transaction. */
        PQP_PGI_PS_TRANS_PEND_SET(scb_psinfo, ps_trans, prec_bmp);
    }

    return;
}

static bool
wlc_apps_pqp_ps_send(wlc_info_t *wlc, struct scb *scb, uint prec_bmp, bool apsd,
    taf_scheduler_public_t *taf)
{
    struct apps_scb_psinfo *scb_psinfo;
    apps_wlc_psinfo_t *wlc_psinfo;
    struct pktq *psq;        /**< multi-priority packet queue */

    if (SCB_DEL_IN_PROGRESS(scb)) {
        /* free any pending frames */
        WL_PS_EX(scb, ("AID %d, prec %x scb is marked for deletion, "
            " freeing all pending PS packets.\n",
            SCB_AID(scb), prec_bmp));
        wlc_apps_ps_flush(wlc, scb);
        return FALSE;
    }

    wlc_psinfo = wlc->psinfo;
    scb_psinfo = SCB_PSINFO(wlc_psinfo, scb);
    ASSERT(scb_psinfo);
    psq = &scb_psinfo->psq;

    /* Attempt PQP PGI of 1 pdu from highest prec before dequeuing.
     * TODO: The fragmented packets must be sent together.
     */
    if (!pktq_mpqp_pgi(psq, prec_bmp)) {
        /* If PQP PGI wait for resource then set flag to resume the PGI transaction. */
        PQP_PGI_PS_SEND_PEND_SET(scb_psinfo, prec_bmp, apsd);
        return TRUE;
    }

    return wlc_apps_ps_send(wlc, scb, prec_bmp, apsd, taf, 1);
}

#define PQP_PGI_PS_POLLLOOP    1000

/* Page In regular PS Q for a given prec and flush pkts */
static uint32
wlc_apps_scb_pqp_ps_flush_prec(wlc_info_t *wlc, struct scb *scb, struct pktq *pq, int prec)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint32 n_pkts_flushed = 0;
    uint32 loop_count = PQP_PGI_PS_POLLLOOP;
    bool retry;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (!scb_psinfo)
        return 0;

    /* Reset previous PGI flags for PS transition */
    PQP_PGI_PS_TRANS_PEND_CLR(scb_psinfo);

    if (pktq_pqp_pkt_cnt(pq, prec) == 0) {
        return 0;
    }

    WL_PS_EX(scb, ("SCB flowid: %d prec 0x%x n_pkts %d\n",
        SCB_FLOWID(scb), prec, pktq_pqp_pkt_cnt(pq, prec)));

    /* Set flag to enable special handling for page in */
    pqp_pgi_spl_set(TRUE);

pgi_retry:
    retry = FALSE;

    /* PQP Page in all the packets for this prec */
    pktq_pqp_pgi(pq, prec, PKTQ_PKTS_ALL, PKTQ_PKTS_ALL);

    if (pktq_pqp_owns(pq, prec)) {
        if (pktqprec_n_pkts(pq, prec) == 0) {
            WL_ERROR(("wl%d.%d: %s SCB flowid: %d PGI 0 pkts\n",
                wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)),
                __FUNCTION__, SCB_FLOWID(scb)));
            OSL_SYS_HALT();
            return 0;
        }
        retry = TRUE;
    }

    if (!pktqprec_empty(pq, prec)) {
        n_pkts_flushed += wlc_txq_pktq_pflush(wlc, pq, prec);
    }

    /* Retry PGI if there are pkts in host. */
    if (retry) {
        loop_count--;
        ASSERT(loop_count);
        goto pgi_retry;
    }

    /* Clear flag to disable special handling */
    pqp_pgi_spl_set(FALSE);

    return n_pkts_flushed;
}

#ifdef WL_USE_SUPR_PSQ
/* Page In Suppress PS Q to execute map_pkts_cb_fn */
static void
wlc_apps_scb_pqp_supp_q_map_pkts(wlc_info_t *wlc, struct apps_scb_psinfo *scb_psinfo,
    map_pkts_cb_fn cb, void *ctx)
{
    struct scb *scb;
    struct spktq *spktq;
    struct spktq map_psq;
    struct pktq_prec *q_A, *q_B; /**< single precedence packet queue */
    void *pkt;
    uint32 loop_count;
    int prec;
    bool retry;

    scb = scb_psinfo->scb;

    spktq_init(&map_psq, PKTQ_LEN_MAX);

    WL_PS_EX(scb, ("SCB flowid: %d map_pkts for suppr psq cb 0x%p ctx 0x%p total %d\n",
        SCB_FLOWID(scb), cb, ctx, scb_psinfo->suppressed_pkts));

    for (prec = (WLC_PREC_COUNT-1); prec >= 0; prec--) {
        spktq = &scb_psinfo->supr_psq[prec];
        if (pqp_pkt_cnt(&spktq->q) == 0)
            continue;

        loop_count = PQP_PGI_PS_POLLLOOP;
pgi_retry:
        retry = FALSE;

        /* PQP Page in all the packets for this spktq */
        spktq_pqp_pgi(spktq, PKTQ_PKTS_ALL, PKTQ_PKTS_ALL);

        if (pqp_owns(&spktq->q)) {
            if (spktq_n_pkts(spktq) == 0) {
                WL_ERROR(("wl%d.%d: %s SCB flowid: %d PGI 0 pkts\n",
                    wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)),
                    __FUNCTION__, SCB_FLOWID(scb)));
                OSL_SYS_HALT();
                return;
            }
            retry = TRUE;
        }

        /* Do map_pkts_cb_fn and enqueu pkts to tempary spktq for later page-out */
        while ((pkt = spktq_deq(spktq)) != NULL) {
            cb(ctx, pkt);
            spktq_enq(&map_psq, pkt);
        }

        /* Page out pkts and release DBM resource */
        if (spktq_n_pkts(&map_psq)) {
            spktq_pqp_pgo(&map_psq, PQP_APPEND, scb_psinfo);
        }

        /* Retry PGI if there are pkts in host. */
        if (retry) {
            loop_count--;
            ASSERT(loop_count);
            goto pgi_retry;
        }

        /* If  PGI operation finished, append Map PSq to Suppress PSq */
        /* All Pkts in Suppress PSq should be moved to Map PSq */
        ASSERT(spktq_n_pkts(spktq) == 0);

        /* Suppress PS queue */
        q_A = &spktq->q;

        /* Mapp PS Q */
        q_B = &map_psq.q;

        /* Append the queues q_A  =  q_A -> [link] q_B */
        pqp_join(q_A, q_B, PQP_APPEND, scb_psinfo);
    }
}
#endif /* WL_USE_SUPR_PSQ */

/* Page In regular PS Q to execute map_pkts_cb_fn */
static void
wlc_apps_scb_pqp_psq_map_pkts(wlc_info_t *wlc, struct apps_scb_psinfo *scb_psinfo,
    map_pkts_cb_fn cb, void *ctx)
{
    struct scb *scb;
    struct pktq *psq;
    struct spktq map_psq;
    struct pktq_prec *q_A, *q_B; /**< single precedence packet queue */
    uint16 n_pkts;
    void *pkt;
    uint32 loop_count;
    int prec;
    bool retry;

    scb = scb_psinfo->scb;
    psq = &scb_psinfo->psq;
    ASSERT(psq);

    spktq_init(&map_psq, PKTQ_LEN_MAX);

    WL_PS_EX(scb, ("SCB flowid: %d map_pkts for psq cb 0x%p ctx 0x%p total %d\n",
        SCB_FLOWID(scb), cb, ctx, pktq_n_pkts_tot(psq)));

    PKTQ_PREC_ITER(psq, prec) {
        if (pktq_pqp_pkt_cnt(psq, prec) == 0)
            continue;

        loop_count = PQP_PGI_PS_POLLLOOP;
pgi_retry:
        retry = FALSE;

        /* PQP Page in all the packets for this prec */
        pktq_pqp_pgi(psq, prec, PKTQ_PKTS_ALL, PKTQ_PKTS_ALL);

        if (pktq_pqp_owns(psq, prec)) {
            if (pktqprec_n_pkts(psq, prec) == 0) {
                WL_ERROR(("wl%d.%d: %s SCB flowid: %d PGI 0 pkts\n",
                    wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)),
                    __FUNCTION__, SCB_FLOWID(scb)));
                OSL_SYS_HALT();
                return;
            }
            retry = TRUE;
        }

        /* Do map_pkts_cb_fn and enqueu pkts to tempary spktq for later page-out */
        while ((pkt = pktq_pdeq(psq, prec)) != NULL) {
            cb(ctx, pkt);
            spktq_enq(&map_psq, pkt);
        }

        /* Page out pkts and release DBM resource */
        if (spktq_n_pkts(&map_psq)) {
            spktq_pqp_pgo(&map_psq, PQP_APPEND, scb_psinfo);
        }

        /* Retry PGI if there are pkts in host. */
        if (retry) {
            loop_count--;
            ASSERT(loop_count);
            goto pgi_retry;
        }

        /* If  PGI operation finished, append Map PSq to regular PSq */
        /* All Pkts in regular PSq should be moved to Map PSq */
        ASSERT(pktqprec_n_pkts(psq, prec) == 0);

        /* PS queue */
        q_A = &(psq->q[prec]);

        /* Mapp PS Q */
        q_B = &map_psq.q;
        n_pkts = pqp_pkt_cnt(q_B);

        /* Append the queues q_A  =  q_A -> [link] q_B */
        pqp_join(q_A, q_B, PQP_APPEND, scb_psinfo);

        if (n_pkts) {
            if (prec > psq->hi_prec)
                psq->hi_prec = (uint8)prec;
            psq->n_pkts_tot += n_pkts;
        }
    }
}

/* Page In PS Q to execute map_pkts_cb_fn */
static void
wlc_apps_scb_pqp_map_pkts(wlc_info_t *wlc, struct scb *scb, map_pkts_cb_fn cb, void *ctx)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint psq_len;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (!scb_psinfo)
        return;

    psq_len = pktq_n_pkts_tot(&scb_psinfo->psq);
#ifdef WL_USE_SUPR_PSQ
    psq_len += scb_psinfo->suppressed_pkts;
#endif
    if (psq_len == 0) {
        return;
    }

    /* Reset previous PGI flags for PS transition */
    PQP_PGI_PS_TRANS_PEND_CLR(scb_psinfo);

    /* Set flag to enable special handling for page in */
    pqp_pgi_spl_set(TRUE);

#ifdef WL_USE_SUPR_PSQ
    /* Observe that there are packets in the supr_psq during ampdu_cleanup_tid_ini()
     * before wlc_apps_scb_suprq_pqp_normalize(). So do map_pkts_cb_fn for supr_psq.
     */
    if (scb_psinfo->suppressed_pkts) {
        wlc_apps_scb_pqp_supp_q_map_pkts(wlc, scb_psinfo, cb, ctx);
    }
#endif

    if (pktq_n_pkts_tot(&scb_psinfo->psq)) {
        wlc_apps_scb_pqp_psq_map_pkts(wlc, scb_psinfo, cb, ctx);
    }

    /* Clear flag to disable special handling */
    pqp_pgi_spl_set(FALSE);
}

/* Page In regular PS Q to age out all pkts that have been through one previous listen interval */
static void
wlc_apps_scb_pqp_pktq_filter(wlc_info_t *wlc, struct scb *scb, void *ctx)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct pktq *psq;
    struct spktq temp_psq;
    struct pktq_prec *q_A, *q_B; /**< single precedence packet queue */
    uint16 n_pkts;
    void *pkt;
    int prec;
    bool retry;
    uint32 loop_count;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);
    if (!scb_psinfo)
        return;

    psq = &scb_psinfo->psq;
    ASSERT(psq);

    spktq_init(&temp_psq, PKTQ_LEN_MAX);

    /* Reset previous PGI flags for PS transition */
    PQP_PGI_PS_TRANS_PEND_CLR(scb_psinfo);

    if (pktq_n_pkts_tot(psq) == 0) {
        return;
    }

    WL_PS_EX(scb, ("SCB flowid: %d total %d\n",
        SCB_FLOWID(scb), pktq_n_pkts_tot(psq)));

    /* Set flag to enalbe special handling for page in */
    pqp_pgi_spl_set(TRUE);

    PKTQ_PREC_ITER(psq, prec) {
        loop_count = PQP_PGI_PS_POLLLOOP;
pgi_retry:
        retry = FALSE;

        /* PQP Page in all the packets for this prec */
        pktq_pqp_pgi(psq, prec, PKTQ_PKTS_ALL, PKTQ_PKTS_ALL);

        if (pktq_pqp_owns(psq, prec)) {
            if (pktqprec_n_pkts(psq, prec) == 0) {
                WL_ERROR(("wl%d.%d: %s SCB flowid: %d PGI 0 pkts\n",
                    wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)),
                    __FUNCTION__, SCB_FLOWID(scb)));
                OSL_SYS_HALT();
                return;
            }
            retry = TRUE;
        }

        /* Age out all pkts that have been through one previous listen interval */
        if (pktqprec_n_pkts(psq, prec)) {
            wlc_txq_pktq_pfilter(wlc, psq, prec, wlc_apps_ps_timeout_filter, ctx);
        }

        if (pktqprec_n_pkts(psq, prec)) {
            /* enqueu pkts to temporary spktq for later page-out */
            while ((pkt = pktq_pdeq(psq, prec)) != NULL) {
                spktq_enq(&temp_psq, pkt);
            }

            /* Page Out the single prio queue */
            spktq_pqp_pgo(&temp_psq, PQP_APPEND, scb_psinfo);
        }

        /* Retry PGI if there are pkts in host. */
        if (retry) {
            loop_count--;
            ASSERT(loop_count);
            goto pgi_retry;
        }

        /* If  PGI operation finished, append Temporary PSq back to regular PSq */
        /* All Pkts in regular PSq should be moved to temporary PSq */
        ASSERT(pktqprec_n_pkts(psq, prec) == 0);

        /* PS queue */
        q_A = &(psq->q[prec]);

        /* Temporary PS Q */
        q_B = &temp_psq.q;
        n_pkts = pqp_pkt_cnt(q_B);

        /* Append the queues q_A  =  q_A -> [link] q_B */
        pqp_join(q_A, q_B, PQP_APPEND, scb_psinfo);

        if (n_pkts) {
            if (prec > psq->hi_prec)
                psq->hi_prec = (uint8)prec;
            psq->n_pkts_tot += n_pkts;
        }
    }

    /* Clear flag to disable special handling */
    pqp_pgi_spl_set(FALSE);
}

/* Page Out regular PS Q and suppress PS Q for given SCB */
void
wlc_apps_scb_pqp_pgo(wlc_info_t* wlc, struct scb* scb, uint16 prec_bmp)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct pktq*    psq;
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo)
        return;

    psq = &scb_psinfo->psq;

    ASSERT(psq);

#ifdef WL_USE_SUPR_PSQ
    /* Page out Suppress PS queue */
    wlc_apps_scb_suprq_pqp_pgo(wlc, scb, prec_bmp);
#endif /* WL_USE_SUPR_PSQ */

    /* Page out normal PS queue if there are pkts in queue */
    if (pktq_mlen(psq, prec_bmp)) {
        WL_PS_EX(scb, ("SCB flowid: %d  prec bmp 0x%x\n",
            SCB_FLOWID(scb), prec_bmp));
        pktq_pqp_pgo(psq, prec_bmp, PQP_APPEND, scb_psinfo);
    }
}

/* Page out normal PS queue of a given prec for a given SCB */
void
wlc_apps_scb_psq_prec_pqp_pgo(wlc_info_t* wlc, struct scb* scb, int prec)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct pktq*    psq;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo)
        return;

    psq = &scb_psinfo->psq;
    ASSERT(psq);

    if (pktqprec_n_pkts(psq, prec)) {
        WL_PS_EX(scb, ("SCB flowid: %d  prec 0x%x\n", SCB_FLOWID(scb), prec));

        /* Page out normal PS queue */
        pktq_prec_pqp_pgo(psq, prec, PQP_APPEND, scb_psinfo);
    }
}

/* Previous PGI PS transition is not finish. Use prepend to page-out PS queue */
void
wlc_apps_scb_pqp_reset_ps_trans(wlc_info_t* wlc, struct scb* scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct pktq*    psq;
    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo)
        return;

    psq = &scb_psinfo->psq;

    ASSERT(psq);

    WL_ERROR(("wl%d.%d: %s SCB flowid: %d previous PS trans 0x%x is not finish %p\n",
        wlc->pub->unit, WLC_BSSCFG_IDX(SCB_BSSCFG(scb)), __FUNCTION__,
        SCB_FLOWID(scb), PQP_PGI_PS_TRANS(scb_psinfo), CALL_SITE));

    /* Reset previous PGI flags for PS transition */
    PQP_PGI_PS_TRANS_PEND_CLR(scb_psinfo);

#ifdef WL_USE_SUPR_PSQ
    /* Page out Suppress PS queue */
    wlc_apps_scb_suprq_pqp_pgo(wlc, scb, WLC_PREC_BMP_ALL);
#endif /* WL_USE_SUPR_PSQ */

    /* Page out normal PS queue if there are pkts in queue */
    if (pktq_n_pkts_tot(psq))
        pktq_pqp_pgo(psq, WLC_PREC_BMP_ALL, PQP_PREPEND, scb_psinfo);
}

#ifdef WL_USE_SUPR_PSQ
/* Page out suppress PS Q for a given SCB */
void
wlc_apps_scb_suprq_pqp_pgo(wlc_info_t* wlc, struct scb* scb, uint16 prec_bmp)
{
    struct apps_scb_psinfo *scb_psinfo;
    uint16 prec;
    struct spktq* suppr_psq;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo)
        return;

    /* Return if there are no suppressed packets */
    if (scb_psinfo->suppressed_pkts == 0)
        return;

    WL_PS_EX(scb, ("SCB flowid: %d  prec bmp 0x%x\n",
        SCB_FLOWID(scb), prec_bmp));

    /* Loop through all available prec suppress PSq */
    for (prec = 0; prec < WLC_PREC_COUNT; prec++) {
        if ((prec_bmp & (1 << prec)) == 0)
            continue;

        /* Suppress PS Q */
        suppr_psq = &scb_psinfo->supr_psq[prec];

        /* Page out suppress PS Queue */
        spktq_pqp_pgo(suppr_psq, PQP_APPEND, scb_psinfo);
    }
}

/* Join a suppress PSq and regular PSq as part of PSq normalization */
void
wlc_apps_scb_suprq_pqp_normalize(wlc_info_t* wlc, struct scb* scb)
{
    struct apps_scb_psinfo *scb_psinfo;
    struct pktq*    pq;
    struct pktq_prec *q_A, *q_B;            /**< single precedence packet queue */
    int prec, hi_prec = 0;
    struct spktq* suppr_psq;

    scb_psinfo = SCB_PSINFO(wlc->psinfo, scb);

    if (!scb_psinfo)
        return;

    /* Previous PGI PS transition is not finish.
     * Reset the status and page-out the remain packets in pktq.
     */
    if (PQP_PGI_PS_TRANS(scb_psinfo)) {
        wlc_apps_scb_pqp_reset_ps_trans(wlc, scb);
    }

    /* Return if there are no suppressed packets */
    if (scb_psinfo->suppressed_pkts == 0)
        return;

    pq = &scb_psinfo->psq;
    ASSERT(pq);

    WL_PS_EX(scb, ("SCB flowid: %d \n", SCB_FLOWID(scb)));

    /* Walk through each prec */
    for (prec = (WLC_PREC_COUNT-1); prec >= 0; prec--) {
        /* PS queue */
        q_A = &(pq->q[prec]);

        /* Suppress PS Q */
        suppr_psq = &scb_psinfo->supr_psq[prec];
        q_B = &suppr_psq->q;

        /* Prepend the queues q_A  =  q_B -> [link] q_A */
        pqp_join(q_A, q_B, PQP_PREPEND, scb_psinfo);

        if ((pqp_pkt_cnt(q_A)) && (prec > hi_prec)) {
            hi_prec = prec;
        }
    }

    /* Reset PSq and suppress PSq stats after the join */
    pq->n_pkts_tot += scb_psinfo->suppressed_pkts;
    pq->hi_prec = (uint8)hi_prec;
    scb_psinfo->suppressed_pkts = 0;
}
#endif /* WL_USE_SUPR_PSQ */
#endif /* HNDPQP */
