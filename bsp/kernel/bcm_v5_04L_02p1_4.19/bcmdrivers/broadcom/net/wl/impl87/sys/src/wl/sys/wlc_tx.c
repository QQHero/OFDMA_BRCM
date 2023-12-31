/*
 * wlc_tx.c
 *
 * Common transmit datapath components
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
 * $Id: wlc_tx.c 809891 2022-03-25 15:51:33Z $
 *
 */

#include <wlc_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <bcmendian.h>
#include <wlc_types.h>
#include <siutils.h>
#include <sbhnddma.h>
#include <hnddma.h>
#include <bcmwifi_channels.h>
#include <802.1d.h>
#include <802.11.h>
#include <vlan.h>
#include <d11.h>
#include <wlioctl.h>
#include <wlc_rate.h>
#include <wlc.h>
#include <wlc_pub.h>
#include <wlc_hw.h>
#include <wlc_hw_priv.h>
#include <wlc_bsscfg.h>
#include <wlc_scb.h>
#include <wlc_ampdu.h>
#include <wlc_scb_ratesel.h>
#include <wlc_key.h>
#include <wlc_stf.h>
#include <wlc_ht.h>
#include <wlc_prot.h>
#include <wlc_prot_g.h>
#define _inc_wlc_prot_n_preamble_	/* include static INLINE uint8 wlc_prot_n_preamble() */
#include <wlc_prot_n.h>
#include <wlc_apps.h>
#include <wlc_bmac.h>
#if defined(WL_PROT_OBSS) && !defined(WL_PROT_OBSS_DISABLED)
#include <wlc_prot_obss.h>
#endif
#include <wlc_cac.h>
#include <wlc_vht.h>
#include <wlc_he.h>
#include <wlc_txbf.h>
#include <wlc_txc.h>
#include <wlc_keymgmt.h>
#include <wlc_led.h>
#include <wlc_ap.h>
#include <bcmwpa.h>
#include <wlc_btcx.h>
#ifdef BCMLTECOEX
#include <wlc_ltecx.h>
#endif
#include <wlc_p2p.h>
#include <wlc_bsscfg.h>
#include <wl_export.h>
#ifdef WLTOEHW
#include <wlc_tso.h>
#endif /* WLTOEHW */
#ifdef WL_LPC
#include <wlc_scb_powersel.h>
#endif /* WL_LPC */
#ifdef WLTDLS
#include <wlc_tdls.h>
#endif /* WLTDLS */
#ifdef WL_RELMCAST
#include "wlc_relmcast.h"
#endif /* WL_RELMCAST */
#if defined(STA)
#include <eapol.h>
#endif /* STA */
#include <eap.h>
#ifdef WLMCNX
#include <wlc_mcnx.h>
#endif /* WLMCNX */
#ifdef WLMCHAN
#include <wlc_mchan.h>
#endif /* WLMCHAN */
#ifdef WLAMSDU_TX
#include <wlc_amsdu.h>
#endif /* WLAMSDU_TX */
#ifdef L2_FILTER
#include <wlc_l2_filter.h>
#endif /* L2_FILTER */
#ifdef WET
#include <wlc_wet.h>
#endif /* WET */
#ifdef WET_TUNNEL
#include <wlc_wet_tunnel.h>
#endif /* WET_TUNNEL */
#ifdef WMF
#include <wlc_wmf.h>
#endif /* WMF */
#ifdef PSTA
#include <wlc_psta.h>
#endif /* PSTA */
#include <ethernet.h>
#include <802.3.h>
#ifdef STA
#include <wlc_wpa.h>
#include <wlc_pm.h>
#endif /* STA */
#ifdef PROP_TXSTATUS
#include <wlc_wlfc.h>
#endif /* PROP_TXSTATUS */
#include <wlfc_proto.h>
#if defined(BCMWAPI_WPI) || defined(BCMWAPI_WAI)
#include <wlc_wapi.h>
#include <sms4.h>
#endif /* BCMWAPI_WPI || BCMWAPI_WAI */
#ifdef WLTEST
#include <bcmnvram.h>
#include <bcmotp.h>
#endif /* WLTEST */
#ifdef WLWNM
#include <wlc_wnm.h>
#endif
#ifdef WL_PROXDETECT
#include <wlc_pdsvc.h>
#endif
#include <wlc_txtime.h>
#include <wlc_airtime.h>
#include <wlc_11u.h>
#include <wlc_tx.h>
#include <wlc_mbss.h>
#ifdef WL11K
#include <wlc_rrm.h>
#endif
#ifdef WL_MODESW
#include <wlc_modesw.h>
#endif
#ifdef WL_PWRSTATS
#include <wlc_pwrstats.h>
#endif
#if defined(WL_LINKSTAT)
#include <wlc_linkstats.h>
#endif
#include <wlc_bsscfg_psq.h>
#include <wlc_txmod.h>
#include <wlc_pktc.h>
#include <wlc_misc.h>
#include <event_trace.h>
#include <wlc_pspretend.h>
#include <wlc_assoc.h>
#include <wlc_rspec.h>
#include <wlc_event_utils.h>
#include <wlc_txs.h>

#if defined(WL_OBSS_DYNBW) && !defined(WL_OBSS_DYNBW_DISABLED)
#include <wlc_obss_dynbw.h>
#endif /* defined(WL_OBSS_DYNBW) && !defined(WL_OBSS_DYNBW_DISABLED) */
#include <wlc_qoscfg.h>
#include <wlc_pm.h>
#include <wlc_scan_utils.h>
#include <wlc_perf_utils.h>
#include <wlc_dbg.h>
#include <wlc_macdbg.h>
#include <wlc_dump.h>
#if defined(SAVERESTORE)
#include <saverestore.h>
#endif /* SAVERESTORE */
#include <phy_api.h>
#include <phy_lpc_api.h>
#include <phy_tpc_api.h>
#include <phy_tssical_api.h>
#include <phy_utils_api.h>
#include <wlc_log.h>

#ifdef WDS
#include <wlc_wds.h>
#endif /* WDS */

#include <wlc_ampdu_cmn.h>
#include <wlc_ampdu_rx.h>
#include <wlc_pcb.h>

#ifdef WL_MU_TX
#include <wlc_mutx.h>
#endif
#ifdef WLCFP
#include <wlc_cfp.h>
#endif

#ifdef HEALTH_CHECK
#include <hnd_hchk.h>
#endif

#include <wlc_ratelinkmem.h>
#include <wlc_musched.h>

#ifdef WLC_DTPC
#include <wlc_dtpc.h>
#endif /* WLC_DTPC */

#if defined(WLTEST) || defined(WLPKTENG) || defined(TESTBED_AP_11AX)
#include <wlc_test.h>
#endif

#ifdef WLTAF
#include <wlc_taf.h>
#endif

#include <wlc_fifo.h>
#include <wlc_txcfg.h>

#include <wlc_iocv.h>
#include <wlc_twt.h>
#include <wlc_ulmu.h>
#if defined(BCM_BLOG) && defined(BCM_WLAN_PER_CLIENT_FLOW_LEARNING)
#include <wl_blog.h>
#endif

#ifdef BCMDBG_TXSTALL
#include <wlc_lq.h>
#include <wlc_dfs.h>
typedef struct {
	uint16 txallfrm;
	uint16 txbcn;
	uint16 txrts;
	uint16 rxcts;
	uint16 rsptmout;
	uint16 rxstrt;
	uint16 glitch;
	uint32 txop;
	uint16 txtrig;
} mac_dbg_t;

/* offset of cntmember by sizeof(uint32) from the first cnt variable. */
#define NUM_MCST_IN_MAC_DBG_T 7
static const uint8 mac_dbg_t_to_macstat_t[NUM_MCST_IN_MAC_DBG_T] = {
	MCSTOFF_TXFRAME,
	MCSTOFF_TXBCNFRM,
	MCSTOFF_TXRTSFRM,
	MCSTOFF_RXCTSUCAST,
	MCSTOFF_RXRSPTMOUT,
	MCSTOFF_RXSTRT,
	MCSTOFF_RXCRSGLITCH
};

typedef struct {
	uint32 time;
	uint16 duration;
	uint   datablock;
	uint8  fifo_bitmap[TX_FIFO_BITMAP_SIZE_MAX];
#ifdef WLCHANIM
	uint8  rxcca;
	uint8  txavail;
#endif /* WLCHANIM */
} txstall_data_t;

#define DATABLOCK_TIMEOUT_MSEC 400
#endif /* BCMDBG_TXSTALL */

static int wlc_wme_wmmac_check_fixup(wlc_info_t *wlc, struct scb *scb, void *sdu);

#if defined(BCMDBG)
static int wlc_datapath_dump(wlc_info_t *wlc, struct bcmstrbuf *b);
static void wlc_datapath_dump_bss_summary(wlc_info_t *wlc, wlc_txq_info_t *qi, wlc_bsscfg_t * cfg,
	struct bcmstrbuf *b);
static int wlc_datapath_dump_lowtxq_summary(txq_t *txq, struct bcmstrbuf *b);
static void wlc_datapath_dump_q_summary(wlc_info_t *wlc, wlc_txq_info_t *qi, struct bcmstrbuf *b);
static int wlc_pktq_dump(wlc_info_t *wlc, struct pktq *q,
	wlc_bsscfg_t *bsscfg, struct scb * scb, const char * prefix, struct bcmstrbuf *b);
#endif

static uint16 wlc_frameid_set_fifo(wlc_info_t *wlc, uint16 frameid, uint fifo);
static uint16* wlc_get_txh_frameid_ptr(wlc_info_t *wlc, void *p);

#define MUTX_TYPE_AC_ALL  (MU_AC_MASK << (MU_AC_MASK_SIZE * AC_VO)) | \
			  (MU_AC_MASK << (MU_AC_MASK_SIZE * AC_VI)) | \
			  (MU_AC_MASK << (MU_AC_MASK_SIZE * AC_BE)) | \
			  (MU_AC_MASK << (MU_AC_MASK_SIZE * AC_BK));

/* status during txfifo sync */
#define TXQ_PKT_DEL	0x01
#define HEAD_PKT_FLUSHED 0xFF

#ifdef WLCNT
/** TxQ per-FIFO backup Sw Queue Statistics */
typedef struct txq_swq_stats {
	uint32 pkts;
	uint32 time;
	uint32 q_stops;
	uint32 q_service;
	uint32 hw_stops;
	uint32 hw_service;
} txq_swq_stats_t;
#endif

/** TxQ per-FIFO backup SwQ Queue Flow Control */
typedef struct txq_swq_flowctl {
	int flags;		/* queue flow control state */
	int buffered;	/* current buffered estimate, in usec */
} txq_swq_flowctl_t;

/** TxQ Sw Queue backing each d11 FIFO */
typedef struct txq_swq {
	dll_t node;     /* retain as first elem, used in dll */
	txq_swq_flowctl_t flowctl;
	struct spktq spktq; /**< single priority queue */
#ifdef WLCNT
	txq_swq_stats_t stats;
#endif
#ifdef HWA_PKTPGR_BUILD
	hwa_pp_hdbm_pktq_t hpktq;
#endif
} txq_swq_t;

struct txq {
	txq_t *next;
	dll_t active;   /* list of non-empty txq_swq */
	dll_t empty;    /* list of empty txq_swq */
	txq_swq_t *swq_table; /**< table of Sw Queues backing NFIFO_EXT_MAX FIFOs */
	uint nfifo;
};

#define TXQ_ASSERT(_txq_, _fifo_idx_) \
({ \
	ASSERT((_txq_) != (txq_t*)NULL); \
	ASSERT((_txq_)->swq_table != (txq_swq_t*)NULL); \
	ASSERT((_fifo_idx_) < (_txq_)->nfifo); \
})

#define TXQ_FIFO_IDX(_txq_, _txq_swq_) \
({ \
	int _fifo_idx_ = (int)((_txq_swq_) - ((_txq_)->swq_table)); \
	ASSERT(_fifo_idx_ < (_txq_)->nfifo); \
	_fifo_idx_; \
})

#define TXQ_SWQ(_txq_, _fifo_idx_) \
({ \
	ASSERT((_txq_) != (txq_t *)NULL); \
	ASSERT((_fifo_idx_) < (_txq_)->nfifo); \
	((txq_t *)(_txq_))->swq_table + (_fifo_idx_); \
})

/*
#define TXQ_SWQ_STATS(_txq_, _fifo_idx_) \
({ \
	&(TXQ_SWQ((_txq_), (_fifo_idx_))->stats); \
})
*/

struct txq_info {
	wlc_info_t *wlc;	/* Handle to wlc cntext */
	wlc_pub_t *pub;		/* Handle to public context */
	osl_t *osh;		/* OSL handle */
	struct spktq *delq;	/* delete queue (single prio) holding pkts-to-delete temporarily */
	txq_t *txq_list;	/* List of tx queues */
	int bsscfg_handle;	/* bsscfg cubby handler */
	uint16 mutx_ac_mask;	/* Bit fields for per AC MUTX */
#ifdef BCMDBG_TXSTALL
	mac_dbg_t mdbg;
#define TXSTALL_HISTORY_SZ 10
	txstall_data_t stall_history[TXSTALL_HISTORY_SZ];
	uint stall_history_idx;
	uint stall_cnt;
	uint stall_time;
#ifdef WLCHANIM
	uint8  rxcca_stallcnt;
	uint8  txavail_stallcnt;
#endif /* WLCHANIM */
#endif /* BCMDBG_TXSTALL */
	void	*p_last_scb[4];		/* pointer of the last scb sent to txfifo */
	uint16	order_seq[4];		/* PSM descriptor schedule order seq */
};

#if defined(WL_PRQ_RAND_SEQ)
struct tx_prs_cfg {
	uint8 prq_rand_seq; /* randomize probe seq feat flag */
	uint16 prq_rand_seqId; /* randomize probe sequence ID */
};
typedef struct tx_prs_cfg tx_prs_cfg_t;

#define TX_PRS_CFG(t, b) ((tx_prs_cfg_t*)BSSCFG_CUBBY(b, (t)->bsscfg_handle))

/* Macros related to randomization of probe request seq */
#define PRQ_RAND_SEQ_ENABLE 0x01 /* IOVAR to enable/disable RAND_SEQ */

/* Enable/Disable random seqId from RANDMAC or PFN module */
#define PRQ_RAND_SEQID      0x02

#define PRQ_RAND_SEQ_APPLY  (PRQ_RAND_SEQ_ENABLE | PRQ_RAND_SEQID)
#endif /* WL_PRQ_RAND_SEQ */

/** txq_swq_flowctl.flags: FLow Control Flags of Sw Queue backing a FIFO's */
/** Flow control at top of low TxQ for queue buffered time */
#define TXQ_STOPPED     0x01

/** Mask of any HW (DMA) flow control flags */
#define TXQ_HW_MASK     0x06

/** HW (DMA) flow control for packet count limit */
#define TXQ_HW_STOPPED  0x02

/** HW (DMA) flow control for outside reason */
#define TXQ_HW_HOLD     0x04

#define IS_EXCURSION_QUEUE(q, txqi) ((txq_t *)(q) == (txqi)->wlc->excursion_queue->low_txq)

#if defined(BCMDBG) || (defined(WLTEST) && !defined(WLTEST_DISABLED))
static const uint32 txbw2rspecbw[] = {
	WL_RSPEC_BW_20MHZ, /* WL_TXBW20L   */
	WL_RSPEC_BW_20MHZ, /* WL_TXBW20U   */
	WL_RSPEC_BW_40MHZ, /* WL_TXBW40    */
	WL_RSPEC_BW_40MHZ, /* WL_TXBW40DUP */
	WL_RSPEC_BW_20MHZ, /* WL_TXBW20LL */
	WL_RSPEC_BW_20MHZ, /* WL_TXBW20LU */
	WL_RSPEC_BW_20MHZ, /* WL_TXBW20UL */
	WL_RSPEC_BW_20MHZ, /* WL_TXBW20UU */
	WL_RSPEC_BW_40MHZ, /* WL_TXBW40L */
	WL_RSPEC_BW_40MHZ, /* WL_TXBW40U */
	WL_RSPEC_BW_80MHZ /* WL_TXBW80 */
};

static const uint16 txbw2phyctl0bw[] = {
	PHY_TXC1_BW_20MHZ,
	PHY_TXC1_BW_20MHZ_UP,
	PHY_TXC1_BW_40MHZ,
	PHY_TXC1_BW_40MHZ_DUP,
};

static const uint16 txbw2acphyctl0bw[] = {
	D11AC_PHY_TXC_BW_20MHZ,
	D11AC_PHY_TXC_BW_20MHZ,
	D11AC_PHY_TXC_BW_40MHZ,
	D11AC_PHY_TXC_BW_40MHZ,
	D11AC_PHY_TXC_BW_20MHZ,
	D11AC_PHY_TXC_BW_20MHZ,
	D11AC_PHY_TXC_BW_20MHZ,
	D11AC_PHY_TXC_BW_20MHZ,
	D11AC_PHY_TXC_BW_40MHZ,
	D11AC_PHY_TXC_BW_40MHZ,
	D11AC_PHY_TXC_BW_80MHZ
};

static const uint16 txbw2acphyctl1bw[] = {
	(WL_CHANSPEC_CTL_SB_LOWER >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_UPPER >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_LOWER >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_LOWER >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_LL >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_LU >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_UL >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_UU >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_LOWER >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_UPPER >> WL_CHANSPEC_CTL_SB_SHIFT),
	(WL_CHANSPEC_CTL_SB_LOWER >> WL_CHANSPEC_CTL_SB_SHIFT)
};

/* Override BW bit in phyctl */
#define WLC_PHYCTLBW_OVERRIDE(pctl, m, pctlo) \
		if (~pctlo & 0xffff)  \
			pctl = (pctl & ~(m)) | (pctlo & (m))

#else /*  defined(BCMDBG) || (defined(WLTEST) && !defined(WLTEST_DISABLED)) */

#define WLC_PHYCTLBW_OVERRIDE(pctl, m, pctlo)

#endif /* defined(BCMDBG) || (defined(WLTEST) && !defined(WLTEST_DISABLED))) */

enum {
	IOV_PRQ_RAND_SEQ = 1,
	IOV_MUTX_AC = 2,
	IOV_DATABLOCK_TIMEOUT = 3,
	IOV_FORCE_DATABLOCK = 4,
	IOV_DATABLOCK_TIMEOUT_PSMWD = 5,
	IOV_DATABLOCK_TIMEOUT_FATAL = 6,
	IOV_FORCE_PKTPEND_SYNC = 7,
	IOV_DATABLOCK_TIMEOUT_STATS = 8,
	IOV_DATABLOCK_TIMEOUT_TXAVAIL_THR = 9,
	IOV_DATABLOCK_TIMEOUT_RXCCA_THR = 10,
	IOV_NEWKEY_PKTDROP_TIMEOUT = 11
};

static const bcm_iovar_t txq_iovars[] = {
	{"mutx_ac", IOV_MUTX_AC, 0, 0, IOVT_BUFFER, sizeof(wl_mutx_ac_mg_t)},
#if defined(BCMDBG) && defined(WL_PRQ_RAND_SEQ)
	{"prq_rand_seq", IOV_PRQ_RAND_SEQ,
	(0), 0, IOVT_BOOL, 0
	},
#endif /* BCMDBG && WL_PRQ_RAND_SEQ */
#ifdef BCMDBG_TXSTALL
	{"datablock_timeout", IOV_DATABLOCK_TIMEOUT, 0, 0, IOVT_UINT32, 0},
	{"force_datablock", IOV_FORCE_DATABLOCK, 0, 0, IOVT_UINT32, 0},
	{"block_timeout_psmwd", IOV_DATABLOCK_TIMEOUT_PSMWD, 0, 0, IOVT_UINT32, 0},
	{"block_timeout_fatal", IOV_DATABLOCK_TIMEOUT_FATAL, 0, 0, IOVT_UINT32, 0},
	{"block_timeout_stats", IOV_DATABLOCK_TIMEOUT_STATS, 0, 0, IOVT_UINT32, 0},
	{"block_timeout_txavail_thr", IOV_DATABLOCK_TIMEOUT_TXAVAIL_THR, 0, 0, IOVT_UINT32, 0},
	{"block_timeout_rxcca_thr", IOV_DATABLOCK_TIMEOUT_RXCCA_THR, 0, 0, IOVT_UINT32, 0},
#endif /* BCMDBG_TXSTALL */
#ifdef WL_TXPKTPEND_SYNC
	{"force_pktpend_sync", IOV_FORCE_PKTPEND_SYNC, 0, 0, IOVT_UINT32, 0},
#endif /* WL_TXPKTPEND_SYNC */
	{"newkey_pktdrop_timeout", IOV_NEWKEY_PKTDROP_TIMEOUT, 0, 0, IOVT_UINT32, 0},
	{NULL, 0, 0, 0, 0, 0}
};

static int
wlc_txq_doiovar(void *context, uint32 actionid,
	void *params, uint p_len, void *arg, uint alen, uint vsize, struct wlc_if *wlcif);

static void txq_init(txq_t *txq, wlc_txq_info_t *qi, uint nfifo);
static void BCMFASTPATH wlc_pull_q(wlc_info_t *wlc);

static txq_t* wlc_low_txq_alloc(txq_info_t *txqi, wlc_txq_info_t *qi, uint nfifo);

static INLINE int  __txq_swq_inc(txq_swq_t *txq_swq, uint fifo, int pkt_cnt);
static INLINE int  __txq_swq_dec(txq_swq_t *txq_swq, uint fifo, int pkt_cnt);
static INLINE int  __txq_swq_space(wlc_info_t *wlc, txq_swq_t *txq_swq);
static INLINE int  __txq_swq_buffered_time(txq_swq_t *txq_swq);

static INLINE int  __txq_swq_buffered_inc(txq_swq_t *txq_swq, uint fifo, int pkt_cnt);
static INLINE int  __txq_swq_buffered_dec(txq_swq_t *txq_swq, uint fifo, int pkt_cnt);
static INLINE void __txq_swq_hw_stop_set(txq_swq_t *txq_swq);
static INLINE void __txq_swq_hw_stop_clr(txq_swq_t *txq_swq);
static INLINE int  __txq_swq_hw_hold(txq_swq_t *txq_swq);
static INLINE void __txq_swq_hw_hold_set(txq_swq_t *txq_swq);
static INLINE void __txq_swq_hw_hold_clr(txq_swq_t *txq_swq);
static INLINE int  __txq_swq_hw_stopped(txq_swq_t *txq_swq);

static void wlc_txq_watchdog(void *ctx);
static int wlc_txq_down(void *ctx);
#if defined(BCMDBG)
static int wlc_txq_module_dump(void *ctx, struct bcmstrbuf *b);
static int wlc_txq_dump(txq_info_t *txqi, txq_t *txq, struct bcmstrbuf *b);
#endif

static const char *fifo_names[] = { "AC_BK", "AC_BE", "AC_VI", "AC_VO", "BCMC", "ATIM" };

const char *wlc_fifo_names(int fifo);

const char *
BCMRAMFN(wlc_fifo_names)(int fifo)
{
	return fifo_names[fifo];
}

static int wlc_txfast(wlc_info_t *wlc, struct scb *scb, void *sdu, uint pktlen, wlc_key_t *key);
static void wlc_update_txpktfail_stats(wlc_info_t *wlc, uint pkt_len, uint8 prio);
static void wlc_dofrag(wlc_info_t *wlc, void *p, uint frag, uint nfrags,
	uint next_payload_len, struct scb *scb, bool is8021x,
	uint fifo, wlc_key_t *key, const wlc_key_info_t *key_info,
	uint8 prio, uint frag_length, ratespec_t rspec);
static struct dot11_header *wlc_80211hdr(wlc_info_t *wlc, void *p,
	struct scb *scb, bool more_frag, bool allow_htc, const wlc_key_info_t *key_info,
	uint8 prio, uint16 *pushlen);
static void* wlc_allocfrag(osl_t *osh, void *sdu, uint offset, uint headroom, uint frag_length,
	uint tailroom);
#if defined(BCMPCIEDEV) && (defined(BCMFRAGPOOL) || (defined(BCMHWA) && \
	defined(HWA_TXPOST_BUILD)))
static void *wlc_lfrag_get(osl_t *osh);
static void * wlc_allocfrag_txfrag(osl_t *osh, void *sdu, uint offset,
	uint frag_length, bool lastfrag);
#endif
static INLINE uint wlc_frag(wlc_info_t *wlc, struct scb *scb, enum wme_ac ac, uint plen,
	uint *flen);
static INLINE uint16 wlc_d11_hdrs_rts_cts(struct scb *scb /* [in] */,
	wlc_bsscfg_t *bsscfg /* [in] */, ratespec_t rspec, bool use_rts, bool use_cts,
	ratespec_t *rts_rspec /* [in/out] */, uint8 *rts_preamble_type /* [out] */);
static uint wlc_calc_frame_len(wlc_info_t *wlc, ratespec_t rate, uint8 preamble_type, uint dur);
static uint16* wlc_get_txh_mactxcontrollow_ptr(wlc_info_t *wlc, void *p);

static void wlc_detach_queue(wlc_info_t *wlc, uint8 active_queue_flush_flag);
static void wlc_attach_queue(wlc_info_t *wlc, wlc_txq_info_t *qi);
static void wlc_txq_freed_pkt_cnt(wlc_info_t *wlc, void *pkt, uint16 *pkt_cnt);
static void wlc_low_txq_account(wlc_info_t *wlc, txq_t *low_txq, uint fifo,
	uint8 flag, uint16 *pkt_cnt, bool partial_reclaim);
static void wlc_low_txq_sync(wlc_info_t *wlc, wlc_txq_info_t *qi, uint fifo, uint8 flag);
static bool wlc_tx_pktpend_check(wlc_info_t *wlc, uint fifo);
static void wlc_tx_fifo_epoch_save(wlc_info_t *wlc, wlc_txq_info_t *qi, uint fifo_idx);

#ifdef PROP_TXSTATUS
static void *wlc_proptxstatus_process_pkt(wlc_info_t *wlc, void *pkt, uint16 *pkt_cnt);
#endif /* PROP_TXSTATUS */

#ifdef STA
static void wlc_pm_tx_upd(wlc_info_t *wlc, struct scb *scb, void *pkt, bool ps0ok, uint fifo);
#endif /* STA */

static uint16 wlc_compute_ampdu_mpdu_dur(wlc_info_t *wlc, enum wlc_bandunit bandunit,
	ratespec_t rate);

static int wlc_txq_scb_init(void *ctx, struct scb *scb);
static void wlc_txq_scb_deinit(void *context, struct scb *scb);

static void wlc_txq_enq(void *ctx, struct scb *scb, void *sdu, uint prec);
static uint wlc_txq_txpktcnt(void *ctx);

static uint16 wlc_acphy_txctl0_calc(wlc_info_t *wlc, ratespec_t rspec, uint8 preamble);
static uint16 wlc_acphy_txctl1_calc(wlc_info_t *wlc, ratespec_t rspec,
	int8 lpc_offset, uint8 txpwr, bool is_mu);
static uint16 wlc_acphy_txctl2_calc(wlc_info_t *wlc, ratespec_t rspec, uint8 txbf_uidx);
static INLINE void txq_lifetime(wlc_info_t *wlc, wlc_pkttag_t *pkttag, d11txhdr_t *txh);

static bool BCMFASTPATH
wlc_tx_preassigned_seqnum(wlc_info_t *wlc, wlc_pkttag_t *pkttag, uint16* seq);

#if defined(HWA_PKTPGR_BUILD)
static void wlc_hwa_tx_fifo_shadow_account(wlc_info_t *wlc, txq_t *txq, uint fifo, uint8 flag);
static void wlc_hwa_tx_fifo_sync_hpktq(wlc_info_t *wlc, txq_t *txq, uint fifo, uint8 flag,
	bool is_active_q);
static void wlc_hwa_hpktq_flush(wlc_info_t *wlc, txq_swq_t *txq_swq, uint32 fifo, uint16 *pktcnt);
static void wlc_hwa_hpktq_map_pkts(wlc_info_t *wlc, txq_swq_t *txq_swq, uint32 fifo,
	map_pkts_cb_fn cb, void *ctx);
#define hpktq_n_pkts(txq_swq)	((txq_swq)->hpktq.mpdu_count)
#else
#define hpktq_n_pkts(txq_swq)	(0)
#endif /* HWA_PKTPGR_BUILD */

/* txmod */
static const txmod_fns_t BCMATTACHDATA(txq_txmod_fns) = {
	wlc_txq_enq,
	wlc_txq_txpktcnt,
	NULL,		/* common queue flush is taken care of in wlc_tx_fifo_scb_flush */
	NULL,
	NULL
};

/* enqueue the packet to delete queue */
static void
wlc_txq_delq_enq(void *ctx, void *pkt)
{
	txq_info_t *txqi = ctx;

	spktq_enq(txqi->delq, pkt);
}

/* flush the delete queue/free all the packet */
static void
wlc_txq_delq_flush(void *ctx)
{
	txq_info_t *txqi = ctx;

	spktq_flush(txqi->osh, txqi->delq, TRUE);
}

/** pktq filter returns PKT_FILTER_DELETE if pkt idx matches with bsscfg idx value */
static pktq_filter_result_t
wlc_ifpkt_chk_cb(void *ctx, void *pkt)
{
	int idx = (int)(uintptr)ctx;

	return (WLPKTTAGBSSCFGGET(pkt) == idx) ? PKT_FILTER_DELETE : PKT_FILTER_NOACTION;
}

/**
 * delete packets that belong to a particular bsscfg in a packet queue.
 *
 * @param pq   Multi-priority packet queue
 */
void
wlc_txq_pktq_cfg_filter(wlc_info_t *wlc, struct pktq *pq, wlc_bsscfg_t *cfg)
{
	txq_info_t *txqi = wlc->txqi;

	pktq_filter(pq, wlc_ifpkt_chk_cb, (void *)(uintptr)WLC_BSSCFG_IDX(cfg),
	            wlc_txq_delq_enq, txqi, wlc_txq_delq_flush, txqi);
}

/**
 * pktq filter function to delete pkts associated with an SCB
 */
static pktq_filter_result_t
wlc_pktq_scb_free_filter(void* ctx, void* pkt)
{
	struct scb *scb = (struct scb *)ctx;

	return ((WLPKTTAGSCBGET(pkt) == scb) ?
			PKT_FILTER_DELETE: PKT_FILTER_NOACTION);
}

/**
 * Frees all pkts associated with the given scb on a pktq
 *
 * @param pq   Multi-priority packet queue
 */
void
wlc_txq_pktq_scb_filter(wlc_info_t *wlc, struct pktq *pq, struct scb *scb)
{
	txq_info_t *txqi = wlc->txqi;

	pktq_filter(pq, wlc_pktq_scb_free_filter, scb,
	            wlc_txq_delq_enq, txqi, wlc_txq_delq_flush, txqi);
}

/**
 * generic pktq filter - packets passing the pktq filter will be placed
 * in the delete queue first and freed later once all packets are processed.
 *
 * @param pq   Multi-priority packet queue
 */
void
wlc_txq_pktq_filter(wlc_info_t *wlc, struct pktq *pq, pktq_filter_t fltr, void *ctx)
{
	txq_info_t *txqi = wlc->txqi;

	pktq_filter(pq, fltr, ctx,
	            wlc_txq_delq_enq, txqi, wlc_txq_delq_flush, txqi);
}

/** @param pq   Multi-priority packet queue */
void
wlc_txq_pktq_pfilter(wlc_info_t *wlc, struct pktq *pq, int prec, pktq_filter_t fltr, void *ctx)
{
	txq_info_t *txqi = wlc->txqi;

	pktq_pfilter(pq, prec, fltr, ctx,
	             wlc_txq_delq_enq, txqi, wlc_txq_delq_flush, txqi);
}

/**
 * Safe pktq flush
 *
 * @param pq   Multi-priority packet queue
 */
void
wlc_txq_pktq_flush(wlc_info_t *wlc, struct pktq *pq)
{
	/* This function must not be used for the common queue */
	ASSERT(IS_COMMON_QUEUE(pq) == FALSE);

#ifdef WLTAF
	/* Defer taf schedule to prevent packet enqueue during flush */
	wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */

	pktq_flush(wlc->osh, pq, TRUE);

#ifdef WLTAF
	wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */
}

uint16
wlc_txq_pktq_pavail(struct scb *scb, uint prec)
{
	struct pktq *common_q = WLC_GET_CQ(SCB_WLCIFP(scb)->qi);
	return MIN(pktq_avail(common_q), pktqprec_avail_pkts(common_q, prec));
}

/** @param pq   Multi-priority packet queue */
uint32
wlc_txq_pktq_pflush(wlc_info_t *wlc, struct pktq *pq, int prec)
{
	uint32 n_pkts_flushed = 0;

	/* This function must not be used for the common queue */
	ASSERT(IS_COMMON_QUEUE(pq) == FALSE);

#ifdef WLTAF
	/* Defer taf schedule to prevent packet enqueue during flush */
	wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */

	n_pkts_flushed = pktqprec_n_pkts(pq, prec);
	pktq_pflush(wlc->osh, pq, prec, TRUE);

#ifdef WLTAF
	wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */

	return n_pkts_flushed;
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
bool BCMFASTPATH
wlc_low_txq_empty(txq_t *txq)
{
	ASSERT(txq);
	return (dll_empty(&txq->active));
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE int
__txq_swq_buffered_time(txq_swq_t *txq_swq)
{
	return txq_swq->flowctl.buffered;
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
int
wlc_txq_buffered_time(txq_t *txq, uint fifo_idx)
{
	return __txq_swq_buffered_time(TXQ_SWQ(txq, fifo_idx));
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE int
__txq_swq_space(wlc_info_t *wlc, txq_swq_t *txq_swq)
{
	return (wlc->pub->txmaxpkts - txq_swq->flowctl.buffered);
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
int BCMFASTPATH
txq_space(wlc_info_t *wlc, txq_t *txq, uint fifo_idx)
{
	return __txq_swq_space(wlc, TXQ_SWQ(txq, fifo_idx));
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE int BCMFASTPATH
__txq_swq_inc(txq_swq_t *txq_swq, uint fifo_idx, int pkt_cnt)
{
	int rem;

	rem = (txq_swq->flowctl.buffered += pkt_cnt);

	if (rem < 0) {
		WL_ERROR(("%s rem: %d fifo:%d\n", __FUNCTION__, rem, fifo_idx));
	}

#ifndef WL_TXPKTPEND_SYNC
	ASSERT(rem >= 0);
#endif /* WL_TXPKTPEND_SYNC */

	return rem;
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE int BCMFASTPATH
__txq_swq_dec(txq_swq_t *txq_swq, uint fifo_idx, int pkt_cnt)
{
	int rem;

	rem = (txq_swq->flowctl.buffered -= pkt_cnt);

	if (rem < 0) {
		WL_ERROR(("%s rem: %d fifo:%d pkt_cnt:%d \n",
			__FUNCTION__, rem, fifo_idx, pkt_cnt));
	}
#ifndef WL_TXPKTPEND_SYNC
	ASSERT(rem >= 0);
#endif /* WL_TXPKTPEND_SYNC */

	return rem;
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE int
__txq_swq_buffered_inc(txq_swq_t *txq_swq, uint fifo_idx, int pkt_cnt)
{
	int buffered_time = __txq_swq_inc(txq_swq, fifo_idx, pkt_cnt);

	return buffered_time;
}

int
wlc_low_txq_buffered_inc(wlc_txq_info_t *qi, uint fifo_idx, int pkt_cnt)
{
	txq_t *txq = qi->low_txq;
	txq_swq_t *txq_swq;		/**< a non-priority queue backing one d11 FIFO */

	TXQ_ASSERT(txq, fifo_idx);
	txq_swq = TXQ_SWQ(txq, fifo_idx);

	return __txq_swq_buffered_inc(txq_swq, fifo_idx, pkt_cnt);
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE int
__txq_swq_buffered_dec(txq_swq_t *txq_swq, uint fifo_idx, int pkt_cnt)
{
	int remaining_time = __txq_swq_dec(txq_swq, fifo_idx, pkt_cnt);

	return remaining_time;
}

int
wlc_low_txq_buffered_dec(wlc_txq_info_t *qi, uint fifo_idx, int pkt_cnt)
{
	txq_swq_t *txq_swq;		/**< a non-priority queue backing one d11 FIFO */
	txq_t *txq = qi->low_txq;

	TXQ_ASSERT(txq, fifo_idx);
	txq_swq = TXQ_SWQ(txq, fifo_idx);

	return __txq_swq_buffered_dec(txq_swq, fifo_idx, pkt_cnt);
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE void BCMFASTPATH
__txq_swq_hw_stop_set(txq_swq_t *txq_swq)
{
	txq_swq->flowctl.flags |= TXQ_HW_STOPPED;
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE void BCMFASTPATH
__txq_swq_hw_stop_clr(txq_swq_t *txq_swq)
{
	txq_swq->flowctl.flags &= ~TXQ_HW_STOPPED;
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE int BCMFASTPATH
__txq_swq_hw_hold(txq_swq_t *txq_swq)
{
	return ((txq_swq->flowctl.flags & TXQ_HW_HOLD) != 0);
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE void
__txq_swq_hw_hold_set(txq_swq_t *txq_swq)
{
	txq_swq->flowctl.flags |= TXQ_HW_HOLD;
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
void BCMFASTPATH
txq_hw_hold_set(txq_t *txq, uint fifo_idx)
{
	__txq_swq_hw_hold_set(TXQ_SWQ(txq, fifo_idx));
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE void
__txq_swq_hw_hold_clr(txq_swq_t *txq_swq)
{
	txq_swq->flowctl.flags &= ~TXQ_HW_HOLD;
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
void BCMFASTPATH
txq_hw_hold_clr(txq_t *txq, uint fifo_idx)
{
	__txq_swq_hw_hold_clr(TXQ_SWQ(txq, fifo_idx));
}

/** @param txq_swq        A non-priority queue backing one d11 FIFO */
static INLINE int
__txq_swq_hw_stopped(txq_swq_t *txq_swq)
{
	return ((txq_swq->flowctl.flags & TXQ_HW_MASK) != 0);
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
int BCMFASTPATH
txq_hw_stopped(txq_t *txq, uint fifo_idx)
{
	return __txq_swq_hw_stopped(TXQ_SWQ(txq, fifo_idx));
}

#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
#define TXQ_DMA_NPKT       32 /* Initial burst of packets to the DMA, each one committed */
#define TXQ_DMA_NPKT_SKP   32 /* Commit every (TXQ_DMA_NPKT_SKP +1)afterwards */
#else
#define TXQ_DMA_NPKT       8 /* Initial burst of packets to the DMA, each one committed */
#define TXQ_DMA_NPKT_SKP   8 /* Commit every (TXQ_DMA_NPKT_SKP +1)afterwards */
#endif

/**
 * Release packets only for the active fifos (which has pending packets)
 *
 * @param txq    E.g. the low queue, consisting of a queue for every d11 fifo
 */
void BCMFASTPATH
txq_hw_fill_active(txq_info_t *txqi, txq_t *txq)
{
	dll_t *item_p, *next_p;
	int fifo_idx;

	for (item_p = dll_head_p(&txq->active); !dll_end(&txq->active, item_p); item_p = next_p) {
		next_p = dll_next_p(item_p);
		/* BCM_CONTAINER_OF(item_p, txq_swq_t, node) */
		fifo_idx = TXQ_FIFO_IDX(txq, (txq_swq_t*)item_p);
		if (fifo_idx == ULMU_TRIG_FIFO) {
			txq_hw_fill_utxd(txqi, txq);
		} else {
			txq_hw_fill(txqi, txq, fifo_idx);
		}
	}
}

/**
 * Drains MPDUs from the caller supplied queue and feeds them to the d11 core
 * using DMA.
 *
 * @param txq    E.g. the low queue, consisting of a queue for every d11 fifo
 */
void BCMFASTPATH
txq_hw_fill(txq_info_t *txqi, txq_t *txq, uint fifo_idx)
{
	void *p;
	struct spktq *spktq; /**< single priority packet queue */
	osl_t *osh;
	wlc_info_t *wlc = txqi->wlc;
	wlc_pub_t *pub = txqi->pub;
	bool commit = FALSE;
	bool delayed_commit = TRUE;
	uint npkt = 0;
	uint iter = 0;
#if defined(BCMLTECOEX) || defined(STA)
	uint16 fc;
#endif /* BCMLTECOEX || STA */
#ifdef BCM_DMA_CT
	bool firstentry = TRUE;
	uint prev_fifo;
	bool bulk_commit = FALSE;
#endif /* BCM_DMA_CT */
	bool sw_ampdu;
	wlc_bsscfg_t *bsscfg;
	txq_swq_t *txq_swq;			/**< a non-priority queue backing one d11 FIFO */
#ifdef WLCNT
	txq_swq_stats_t *stats;
#endif
	uint corerev = pub->corerev;
#if defined(BCMHWA) && defined(HWA_PKTPGR_BUILD)
	wlc_hwa_txpkt_t *hwa_txpkt = wlc->hwa_txpkt;
	uint16 txdescs, aqmdescs;
#endif
	hnddma_t *di;
	uint fifo;

	sw_ampdu = (AMPDU_ENAB(pub) && AMPDU_HOST_ENAB(pub));
	if (sw_ampdu) {
		/* sw_ampdu has its own method of determining when to commit */
		delayed_commit = FALSE;
	}
	TXQ_ASSERT(txq, fifo_idx);
	txq_swq = TXQ_SWQ(txq, fifo_idx);

	spktq = &txq_swq->spktq;
	osh = txqi->osh;

	BCM_REFERENCE(bsscfg);

	di = WLC_HW_DI(wlc, fifo_idx);
#ifdef BCM_DMA_CT
	fifo = prev_fifo = WLC_HW_NFIFO_INUSE(wlc);
	if (BCM_DMA_CT_ENAB(wlc)) {
		/* in CutThru dma do not update dma for every pkt. */
		commit = FALSE;
		/* Do not use delayed_commit, use bulk_commit instead for CTDMA
		 * Instead of performing dma commit for evey 8 mpdus, perform txdma
		 * and aqm dma_commit for all the mpdus together.
		 */
		delayed_commit = FALSE;
	}
#endif /* BCM_DMA_CT */

	while ((p = spktq_deq(spktq))) {
		uint ndesc;
		struct scb *scb = WLPKTTAGSCBGET(p);
		d11txhdr_t *txh;
		struct dot11_header *d11h = NULL;
		uint hdrSize;
		uint16 TxFrameID;
		uint16 MacTxControlLow;
		uint8 *pkt_data = PKTDATA(osh, p);
		uint tsoHdrSize = 0;
#ifdef WLC_MACDBG_FRAMEID_TRACE
		uint8 *tsoHdrPtr;
		uint8 epoch = 0;
#endif
#if defined(WLPKTDLYSTAT) || defined(WL11K)
		uint32 cached_enqtime = 0;
#endif /* defined (WLPKTDLYSTAT) || defined(WL11K) */

		wlc_pkttag_t *pkttag;

		BCM_REFERENCE(d11h);

		if (D11REV_GE(corerev, 129) && WLPKTTAG(p)->flags & WLF_UTXD) {
			/* should never happen */
			ASSERT(0);
			continue;
		}

		/* calc the number of descriptors needed to queue this frame */

		/* for unfragmented packets, count the number of packet buffer
		 * segments, each being a contiguous virtual address range.
		 */

		bsscfg = wlc->bsscfg[WLPKTTAGBSSCFGGET(p)];
		ASSERT(bsscfg != NULL);
		if (bsscfg == NULL) {
			/* bsscfg not assigned in packet!!!  This should not happen,
			 * but if it does, this will prevent a crash.
			 * Allocated buffer but not consumed, need to manually free packet.
			 * This is a malformed packet.
			 */
			WL_ERROR(("wl%d: %s: unable to find bsscfg from packet\n",
				wlc->pub->unit, __FUNCTION__));
			WLCNTINCR(wlc->pub->_cnt->txfail);
			TXPKTPENDDEC(wlc, fifo_idx, 1);
			__txq_swq_buffered_dec(txq_swq, fifo_idx, 1);
			if (scb != NULL) {
				SCB_PKTS_INFLT_FIFOCNT_DECR(wlc, scb, PKTPRIO(p));
			}
			PKTFREE(wlc->osh, p, TRUE);
			continue;
		}

		ndesc = pktsegcnt(osh, p);

		if (D11REV_LT(corerev, 40)) {
			hdrSize = sizeof(d11txh_pre40_t);
			txh = (d11txhdr_t *)pkt_data;
			d11h = (struct dot11_header*)((uint8*)txh + hdrSize +
				D11_PHY_HDR_LEN);
		} else {
#ifdef WLTOEHW
			tsoHdrSize = WLC_TSO_HDR_LEN(wlc, (d11ac_tso_t*)pkt_data);
#ifdef WLC_MACDBG_FRAMEID_TRACE
			tsoHdrPtr = (void*)((tsoHdrSize != 0) ? pkt_data : NULL);
			epoch = (tsoHdrPtr[2] & TOE_F2_EPOCH) ? 1 : 0;
#endif /* WLC_MACDBG_FRAMEID_TRACE */
#endif /* WLTOEHW */
			if (D11REV_GE(corerev, 128)) {
				hdrSize = D11_TXH_LEN_EX(wlc);
			} else {
				hdrSize = sizeof(d11actxh_t);
			}

			txh = (d11txhdr_t *)(pkt_data + tsoHdrSize);
			d11h = (struct dot11_header*)((uint8*)txh + hdrSize);
		}

		TxFrameID = ltoh16(*D11_TXH_GET_FRAMEID_PTR(wlc, txh));
		MacTxControlLow = *D11_TXH_GET_MACLOW_PTR(wlc, txh);

		fifo = D11_TXFID_GET_FIFO(wlc, TxFrameID);

		/* The number of active fifos should not change, validate and handle
		 * exception.
		 */
		if (fifo >= WLC_HW_NFIFO_INUSE(wlc)) {
			WL_ERROR(("%s: fifo %u is inactive (%u)\n", __FUNCTION__,
				fifo, WLC_HW_NFIFO_INUSE(wlc)));
			ASSERT(0);
			/* Select a valid TX FIFO */
			wlc_scb_get_txfifo(wlc, scb, p, &fifo);
		}

		di = WLC_HW_DI(wlc, fifo);
		txq_swq = TXQ_SWQ(txq, fifo);

#ifdef BCM_DMA_CT
		if (BCM_DMA_CT_ENAB(wlc)) {
			if (prev_fifo == WLC_HW_NFIFO_INUSE(wlc)) {
				/* Should be first iteration of the loop */
				/* Set start of bulk dma tx */
				DMA_BULK_DESCR_TX_START(WLC_HW_DI(wlc, fifo));
				DMA_BULK_DESCR_TX_START(WLC_HW_AQM_DI(wlc, fifo));
#if defined(WLPKTDLYSTAT) || defined(WL11K)
				/* init cached enqtime */
				cached_enqtime = WLC_GET_CURR_TIME(wlc);
#endif /* defined (WLPKTDLYSTAT) || defined(WL11K) */
			}

			/* take care of fifo change */
			if ((fifo != prev_fifo) &&
				(prev_fifo != WLC_HW_NFIFO_INUSE(wlc))) {
#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
				(void)wlc_bmac_hwa_txfifo_commit(wlc, prev_fifo,
					__FUNCTION__);
#else
				DMA_BULK_DESCR_TX_COMMIT(WLC_HW_DI(wlc, prev_fifo));
				DMA_BULK_DESCR_TX_COMMIT(WLC_HW_AQM_DI(wlc, prev_fifo));
#endif /* BCMHWA */
				bulk_commit = FALSE;
				//start bulk_dma for new fifo
				DMA_BULK_DESCR_TX_START(WLC_HW_DI(wlc, fifo));
				DMA_BULK_DESCR_TX_START(WLC_HW_AQM_DI(wlc, fifo));
#if defined(WLPKTDLYSTAT) || defined(WL11K)
				/* init cached enqtime */
				cached_enqtime = WLC_GET_CURR_TIME(wlc);
#endif /* defined (WLPKTDLYSTAT) || defined(WL11K) */
			}
		}
#endif /* BCM_DMA_CT */
		if (__txq_swq_hw_stopped(txq_swq)) {
			/* return the packet to the queue */
			spktq_enq_head(spktq, p);
			break;
		}
#ifdef WLCNT
		stats = &txq_swq->stats;
		WLCNTINCR(stats->hw_service);
#endif

#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
		/* Check 3b pktchain ring is full */
		BCM_REFERENCE(di);
		BCM_REFERENCE(ndesc);

#if defined(HWA_PKTPGR_BUILD)
		/* Check if TX/AQM descs available? */
		if (hwa_txpkt->fifo == fifo) {
			aqmdescs = hwa_txpkt->mpdu_count;
			txdescs = hwa_txpkt->txdescs;
		} else {
			aqmdescs = 0;
			txdescs = 0;
		}
		/* Assume "p" needs 1 AQM and HWA_TXFIFO_TXDESC_MAX_PER_MPDU */
		aqmdescs += 1;
		txdescs += HWA_TXFIFO_TXDESC_MAX_PER_MPDU;
		if (hwa_txfifo_descs_isfull(hwa_dev, WLC_HW_MAP_TXFIFO(wlc, fifo),
			txdescs, aqmdescs)) {
			/* mark this hw fifo as stopped */
			__txq_swq_hw_stop_set(txq_swq);
			WLCNTINCR(stats->hw_stops);

			/* return the packet to the queue */
			spktq_enq_head(spktq, p);
			break;
		}
#endif /* HWA_PKTPGR_BUILD && FALSE */

		if (wlc->hwa_txpkt->isfull) {
			WL_TRACE(("wl%d %s: NO TXFIFO pktchain room available, fifo=%d \n",
				pub->unit, __FUNCTION__, fifo));
			if (hwa_txfifo_pktchain_ring_isfull(hwa_dev)) {
				/* Don't mark this hw fifo as stopped because
				 * we stop by 3b pktchain ring full not fifo
				 * specific.
				 */
				WLCNTINCR(stats->hw_stops);

				/* return the packet to the queue */
				spktq_enq_head(spktq, p);
				break;
			}
			wlc_bmac_hwa_txfifo_ring_full(wlc, FALSE);
		}
#else
		/* check for sufficient dma resources */
		if ((uint)TXAVAIL(wlc, fifo) <= ndesc) {
			/* this pkt will not fit on the dma ring */

			/* mark this hw fifo as stopped */
			__txq_swq_hw_stop_set(txq_swq);
			WLCNTINCR(stats->hw_stops);

			/* return the packet to the queue */
			spktq_enq_head(spktq, p);
			break;
		}
#endif /* BCMHWA */

		pkttag = WLPKTTAG(p);

		/* We are done with WLF_FIFOPKT regardless */
		pkttag->flags &= ~WLF_FIFOPKT;

		/* We are done with WLF_TXCMISS regardless */
		pkttag->flags &= ~WLF_TXCMISS;

		/* Apply misc fixups for state and powersave */
#ifdef STA
		fc = ltoh16(d11h->fc);
		if (BSSCFG_STA(bsscfg) && !(pkttag->flags3 & WLF3_NO_PMCHANGE) &&
			!(FC_TYPE(fc) == FC_TYPE_CTL &&
				FC_SUBTYPE_ANY_PSPOLL(FC_SUBTYPE(fc))) &&
			!(FC_TYPE(fc) == FC_TYPE_DATA &&
				FC_SUBTYPE_ANY_NULL(FC_SUBTYPE(fc)) &&
			(bsscfg->pm->PM == PM_MAX ? !FC_SUBTYPE_ANY_QOS(fc) : TRUE))) {

			if (bsscfg->pm->PMenabled == TRUE) {
				wlc_pm_tx_upd(wlc, scb, p, TRUE, fifo);
			}
			if (bsscfg->pm->adv_ps_poll == TRUE) {
				wlc_adv_pspoll_upd(wlc, scb, p, FALSE, fifo);
			}
		}
#endif /* STA */
		PKTDBG_TRACE(osh, p, PKTLIST_TXFIFO);

#if defined(WLPKTDLYSTAT) || defined(WL11K)
		/* Save the packet enqueue time (for latency calculations) */
		if (pkttag->shared.enqtime == 0) {
			if (!cached_enqtime) {
				pkttag->shared.enqtime = WLC_GET_CURR_TIME(wlc);
			} else {
				pkttag->shared.enqtime = cached_enqtime;
			}
		}
#endif
		txq_lifetime(wlc, pkttag, txh);

		if (D11REV_GE(corerev, 128)) {
			uint16 schedid;
			if (wlc_twt_scb_get_schedid(wlc->twti, scb, &schedid)) {
				/* Identify the packet to be for TWT link */
				*D11_REV128_GET_MACHIGH_PTR(txh) |= htol16(D11REV128_TXC_TWT);
				/* Disable PS pretend for TWT data frames */
				*D11_REV128_GET_MACHIGH_PTR(txh) &= ~htol16(D11AC_TXC_PPS);
				txh->rev128.TWTSPStart = htol16(schedid);
			} else {
				*D11_REV128_GET_MACHIGH_PTR(txh) &= ~htol16(D11REV128_TXC_TWT);
			}
			if (txqi->p_last_scb[WME_PRIO2AC(PKTPRIO(p))] != scb) {
				txqi->order_seq[WME_PRIO2AC(PKTPRIO(p))]++;
				txqi->p_last_scb[WME_PRIO2AC(PKTPRIO(p))] = scb;
			}
			txh->rev128.OrderSeq = htol16(txqi->order_seq[WME_PRIO2AC(PKTPRIO(p))]);
		}

#ifdef BCMLTECOEX
		/* activate coex signal if 2G tx-active co-ex
		 * enabled and we're about to send packets
		 */
		if BCMLTECOEX_ENAB(pub) {
			fc = ltoh16(d11h->fc);
			if (wlc->hw->btc->bt_shm_addr && BAND_2G(wlc->band->bandtype) &&
					FC_TYPE(fc) == FC_TYPE_DATA &&
					!FC_SUBTYPE_ANY_NULL(FC_SUBTYPE(fc))) {
				uint16 ltecx_flags;
				ltecx_flags = wlc_bmac_read_shm(wlc->hw, M_LTECX_FLAGS(wlc->hw));
				if ((ltecx_flags & (1 << C_LTECX_FLAGS_TXIND)) == 0) {
					ltecx_flags |= (1 << C_LTECX_FLAGS_TXIND);
					wlc_bmac_write_shm(wlc->hw, M_LTECX_FLAGS(wlc->hw),
						ltecx_flags);
				}
			}
		}
#endif /* BCMLTECOEX */

		/* When a Broadcast/Multicast frame is being committed to the BCMC
		 * fifo via DMA (NOT PIO), update ucode or BSS info as appropriate.
		 */
		if (fifo == TX_BCMC_FIFO) {
#if defined(MBSS)
			/* For MBSS mode, keep track of the
			 * last bcmc FID in the bsscfg info.
			 * A snapshot of the FIDs for each BSS
			 * will be committed to shared memory at DTIM.
			 */
			if (MBSS_ENAB(pub)) {
				ASSERT(bsscfg != NULL);
				bsscfg->bcmc_fid = TxFrameID;
				wlc_mbss_txq_update_bcmc_counters(wlc, bsscfg, p);
			} else
#endif /* MBSS */
			{
				/*
				 * Commit BCMC sequence number in
				 * the SHM frame ID location
				*/
				wlc_bmac_write_shm(wlc->hw, M_BCMC_FID(wlc), TxFrameID);
			}
		}

		if (D11REV_LT(corerev, 128)) {
#ifdef BCM_DMA_CT
			/* XXX:CRWLDOT11M-2187, 2432 WAR, avoid AQM hang when PM !=0.
			 * A 10us delay is needed for the 1st entry of posting
			 * if the override is already ON for WAR this issue and
			 * not suffer much penalty.
			 */
			if (BCM_DMA_CT_ENAB(wlc) && firstentry &&
				wlc->primary_bsscfg->pm->PM != PM_OFF &&
				(D11REV_IS(corerev, 65))) {
				if (!(wlc->hw->maccontrol & MCTL_WAKE) &&
				    !(wlc->hw->wake_override & WLC_WAKE_OVERRIDE_CLKCTL)) {
					wlc_ucode_wake_override_set(wlc->hw,
					WLC_WAKE_OVERRIDE_CLKCTL);
				} else {
					OSL_DELAY(10);
				}
				firstentry = FALSE;
			}
#endif /* BCM_DMA_CT */
		} /* skip revid >= 128 */

		/* Toggle commit bit on last AMPDU when using software/host aggregation */
		if (sw_ampdu) {
			uint16 mpdu_type = (((ltoh16(MacTxControlLow)) &
				TXC_AMPDU_MASK) >> TXC_AMPDU_SHIFT);
			commit = ((mpdu_type == TXC_AMPDU_LAST) || (mpdu_type == TXC_AMPDU_NONE));
		}

#if defined(BCMDBG) || defined(BCMDBG_ASSERT)
		if (D11REV_GE(corerev, 40) &&
			D11REV_LT(corerev, 128)) {
			/* DON'T PUT ANYTHING THERE */
			/* ucode is going to report the value
			 * plus modification back to the host
			 * in txstatus.
			 */
			ASSERT(txh->rev40.PktInfo.TxStatus == 0);
		}
#endif
#ifdef WLC_MACDBG_FRAMEID_TRACE
		wlc_macdbg_frameid_trace_pkt(wlc->macdbg, p, txh,
			(uint8)fifo_idx, htol16(TxFrameID), epoch, scb);
#endif
#if defined(CFP_DEBUG_LEVEL) && ((CFP_DEBUG_LEVEL & CFP_TX_TRACE_ENABLE))
		/* Dump packet headers */
		wlc_print_hdrs(wlc, "BMAC TXFAST hdr", (uint8*)d11h,
			(uint8*)PKTDATA(osh, p), NULL, PKTLEN(osh, p));
#endif /* CFP_DEBUG */

		if (wlc_bmac_dma_txfast(wlc, fifo, p, commit) < 0) {
			/* the dma did not have enough room to take the pkt */

			/* mark this hw fifo as stopped */
			__txq_swq_hw_stop_set(txq_swq);
			WLCNTINCR(stats->hw_stops);

			/* return the packet to the queue */
			spktq_enq_head(spktq, p);
			break;
		}
#ifdef BCM_DMA_CT
		if (BCM_DMA_CT_ENAB(wlc)) {
			prev_fifo = fifo;
			bulk_commit = TRUE;
			PKTCNTR_INC(wlc, PKTCNTR_MPDU_ENQ_TXFIFO, 1);
		}
#endif /* BCM_DMA_CT */

		if (delayed_commit) {
			npkt++;
			iter++;
			/*
			 * Commit the first TXQ_DMA_NPKT packets
			 * and then every (TXQ_DMA_NPKT_SKP + 1)-th packet
			 */
			/* Why do we commit in 9 MPDU x 7 commit + 1 MPDU/commit = 64 MPDU
			 * Why don't we commit in 8 MPDU x 8 commit = 64 MPDU ?
			 * Change to 8 MPDU x 8 commit
			 */
			if ((npkt >= TXQ_DMA_NPKT) && (iter >= TXQ_DMA_NPKT_SKP)) {
				iter = 0;
#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
				(void)wlc_bmac_hwa_txfifo_commit(wlc, fifo, __FUNCTION__);
#else
				dma_txcommit(di);
#endif /* BCMHWA */
			}
		}

		PKTDBG_TRACE(osh, p, PKTLIST_DMAQ);

		/* WES: Check if this is still needed. HW folks should be in on this. */
		/* XXX: WAR for in case pkt does not wake up chip, read maccontrol reg
		 * tracked by PR94097
		 */
		if (BCM4350_CHIP(pub->sih->chip) ||
			BCM4352_CHIP_ID == CHIPID(pub->sih->chip) ||
			BCM43602_CHIP(pub->sih->chip) ||
			BCM4360_CHIP_ID == CHIPID(pub->sih->chip))  {
			ASSERT(bsscfg != NULL);
			if (bsscfg->pm->PM)
			(void)R_REG(osh, D11_MACCONTROL(wlc));
		}
	}  /* while ((p = spktq_deq(spktq))) */

	/*
	 * Do the commit work for packets posted to the DMA.
	 * This call updates the DMA registers for the additional descriptors
	 * posted to the ring.
	 */
	if (iter > 0) {
		ASSERT(delayed_commit);
		/*
		 * Software AMPDU takes care of the commit in the code above,
		 * delay commit for everything else.
		 */
#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
		(void)wlc_bmac_hwa_txfifo_commit(wlc, fifo, __FUNCTION__);
#else
		dma_txcommit(di);
#endif /* BCMHWA && HWA_TXFIFO_BUILD */
	}
#if defined(BCM_DMA_CT)
	if (BCM_DMA_CT_ENAB(wlc) && bulk_commit) {
		ASSERT(fifo < WLC_HW_NFIFO_INUSE(wlc));
#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
		(void)wlc_bmac_hwa_txfifo_commit(wlc, fifo, __FUNCTION__);
#else
		DMA_BULK_DESCR_TX_COMMIT(WLC_HW_DI(wlc, fifo));
		DMA_BULK_DESCR_TX_COMMIT(WLC_HW_AQM_DI(wlc, fifo));
#endif /* BCMHWA && HWA_TXFIFO_BUILD */
	}

#if defined(BULK_DESCR_FLUSH)
	/* catch any failure cases where bulk tx was started but for some reason
	 * there was no successfull dma tx
	 */
	if (BCM_DMA_CT_ENAB(wlc) && (fifo < WLC_HW_NFIFO_INUSE(wlc)) && (!bulk_commit) &&
		DMA_BULK_DESCR_TX_IS_ACTIVE(WLC_HW_DI(wlc, fifo))) {
		DMA_BULK_DESCR_TX_CANCEL(WLC_HW_DI(wlc, fifo));
		DMA_BULK_DESCR_TX_CANCEL(WLC_HW_AQM_DI(wlc, fifo));
	}
#endif /* BULK_DESCR_FLUSH */

#endif /* BCM_DMA_CT */

	/* Remove node from active list to empty if there is no pending packet */
	if (spktq_n_pkts(spktq) == 0) {
		txq_swq = TXQ_SWQ(txq, fifo_idx);
		dll_delete(&txq_swq->node);
		dll_append(&txq->empty, &txq_swq->node);
	}
}

/**
 * Drains UTXDs from the caller supplied queue and feeds them to the d11 core
 * using DMA.
 *
 * @param txq    E.g. the low queue, consisting of a queue for every d11 fifo
 */
void BCMFASTPATH
txq_hw_fill_utxd(txq_info_t *txqi, txq_t *txq)
{
	void *p;
	struct spktq *spktq; /**< single priority packet queue */
	osl_t *osh;
	wlc_info_t *wlc = txqi->wlc;
	wlc_pub_t *pub = txqi->pub;
	bool commit = TRUE;
	txq_swq_t *txq_swq;			/**< a non-priority queue backing one d11 FIFO */
#ifdef WLCNT
	txq_swq_stats_t *stats;
#endif
	uint fifo = ULMU_TRIG_FIFO;
	hnddma_t *di = WLC_HW_DI(wlc, fifo);

	TXQ_ASSERT(txq, fifo);
	txq_swq = TXQ_SWQ(txq, fifo);

	spktq = &txq_swq->spktq;
	osh = txqi->osh;

	BCM_REFERENCE(osh);
	BCM_REFERENCE(pub);
	BCM_REFERENCE(di);

	while ((p = spktq_deq(spktq))) {
		uint ndesc;

		/* calc the number of descriptors needed to queue this frame */

		ndesc = pktsegcnt(osh, p);

		if (__txq_swq_hw_stopped(txq_swq)) {
			/* return the packet to the queue */
			spktq_enq_head(spktq, p);
			break;
		}
#ifdef WLCNT
		stats = &txq_swq->stats;
		WLCNTINCR(stats->hw_service);
#endif

#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
		/* Check 3b pktchain ring is full */
		BCM_REFERENCE(ndesc);
		if (wlc->hwa_txpkt->isfull) {
			WL_TRACE(("wl%d %s: NO TXFIFO pktchain room available, fifo=%d \n",
				wlc->pub->unit, __FUNCTION__, fifo));
			if (hwa_txfifo_pktchain_ring_isfull(hwa_dev)) {
				/* Don't mark this hw fifo as stopped because
				 * we stop by 3b pktchain ring full not fifo
				 * specific.
				 */
				WLCNTINCR(stats->hw_stops);

				/* return the packet to the queue */
				spktq_enq_head(spktq, p);
				break;
			}
			wlc_bmac_hwa_txfifo_ring_full(wlc, FALSE);
		}
#else
		/* check for sufficient dma resources */
		if ((uint)TXAVAIL(wlc, fifo) <= ndesc) {
			/* this pkt will not fit on the dma ring */

			/* mark this hw fifo as stopped */
			__txq_swq_hw_stop_set(txq_swq);
			WLCNTINCR(stats->hw_stops);

			/* return the packet to the queue */
			spktq_enq_head(spktq, p);
			break;
		}
#endif /* BCMHWA */

		commit = spktq_peek(spktq) ? FALSE : TRUE;

		if (wlc_bmac_dma_txfast(wlc, fifo, p, commit) < 0) {
			/* the dma did not have enough room to take the pkt */

			/* mark this hw fifo as stopped */
			__txq_swq_hw_stop_set(txq_swq);
			WLCNTINCR(stats->hw_stops);

			/* return the packet to the queue */
			spktq_enq_head(spktq, p);

			commit = FALSE;
			break;
		}

		PKTDBG_TRACE(osh, p, PKTLIST_DMAQ);
	}  /* while ((p = spktq_deq(spktq))) */

	if (!commit) {
#if defined(BCMHWA) && defined(HWA_TXFIFO_BUILD)
		wlc_bmac_hwa_txfifo_commit(wlc, fifo, __FUNCTION__);
#else
		dma_txcommit(di);
#endif /* BCMHWA && HWA_TXFIFO_BUILD */
	}

	/* Remove node from active list to empty if there is no pending packet */
	if (spktq_n_pkts(spktq) == 0) {
		txq_swq = TXQ_SWQ(txq, fifo);
		dll_delete(&txq_swq->node);
		dll_append(&txq->empty, &txq_swq->node);
	}
}

#ifdef BCMDBG_TXSTALL
static int
wlc_txstall_dump(void *ctx, struct bcmstrbuf *b)
{
	wlc_info_t *wlc = ctx;
	txq_info_t *txqi = wlc->txqi;
	int i, fifo;
	txstall_data_t *stall;
	uint avg_stall_time = 0;
	uint cnt = 0;

	if (txqi->stall_cnt > 0) {
		avg_stall_time = txqi->stall_time / txqi->stall_cnt;
	}
	bcm_bprintf(b, "wl%d %d TX stalls. Total stalled time: %d msec (AVG: %d)\n",
		wlc->pub->unit, txqi->stall_cnt, txqi->stall_time, avg_stall_time);
#ifdef WLCHANIM
	bcm_bprintf(b, "TX stalls: rxcca_stallcnt=%d txavail_stallcnt=%d\n",
		txqi->rxcca_stallcnt, txqi->txavail_stallcnt);
#endif

	for (i = 0; i < TXSTALL_HISTORY_SZ; i++) {
		stall = &txqi->stall_history[i];
		if (stall->time > 0) {
			cnt++;
			bcm_bprintf(b, "%d sec ago: stalled for %d msec; block bitmap=0x%x FIFO's:",
				wlc->pub->now - stall->time, stall->duration,
				stall->datablock);
			for (fifo = 0; fifo < WLC_HW_NFIFO_INUSE(wlc); fifo++) {
				if (isset(stall->fifo_bitmap, fifo)) {
					bcm_bprintf(b, "%d ", fifo);
				}
			}
#ifdef WLCHANIM
			bcm_bprintf(b, "rxcca/rx_thr %d/%d txavail/txavail_thr %d/%d",
				stall->rxcca, wlc->block_timeout_rxcca_thr, stall->txavail,
				wlc->block_timeout_txavail_thr);
#endif
			bcm_bprintf(b, "\n");
		}
	}
	if (cnt == 0) {
		bcm_bprintf(b, "< empty history >\n");
	}
	return BCME_OK;
}
static int
wlc_txstall_dump_clr(void *ctx)
{
	wlc_info_t *wlc = ctx;
	txq_info_t *txqi = wlc->txqi;
	bzero(txqi->stall_history, sizeof(txqi->stall_history));
	txqi->stall_history_idx = 0;
	txqi->stall_cnt = 0;
	txqi->stall_time = 0;
#ifdef WLCHANIM
	txqi->rxcca_stallcnt = 0;
	txqi->txavail_stallcnt = 0;
#endif /* WLCHANIM */
	return BCME_OK;
}

static void
wlc_txq_stall_dump_rev128(wlc_info_t *wlc)
{
	uint16 rdy0, rdy1, rdy2, rdy3, rdy4;
	osl_t *osh = wlc->osh;

	BCM_REFERENCE(rdy0);
	BCM_REFERENCE(rdy1);
	BCM_REFERENCE(rdy2);
	BCM_REFERENCE(rdy3);
	BCM_REFERENCE(rdy4);

	rdy0 = R_REG(osh, D11_AQMF_READY0(wlc));
	rdy1 = R_REG(osh, D11_AQMF_READY1(wlc));
	rdy2 = R_REG(osh, D11_AQMF_READY2(wlc));
	rdy3 = R_REG(osh, D11_AQMF_READY3(wlc));
	rdy4 = R_REG(osh, D11_AQMF_READY4(wlc));
	WL_ERROR(("wl%d: fifordy 0-15:%04x 16-31:%04x 32-47:%04x 48-63:%04x 64-69:%04x\n",
		wlc->pub->unit, rdy0, rdy1, rdy2, rdy3, rdy4));
	WL_ERROR(("wl%d: SuspReq2/1/0:0x%08x/0x%08x/0x%08x FlushReq:0x%08x/0x%08x/0x%08x\n",
		wlc->pub->unit, R_REG(osh, D11_SUSPREQ2(wlc)),
		R_REG(osh, D11_SUSPREQ1(wlc)),
		R_REG(osh, D11_SUSPREQ(wlc)),
		R_REG(osh, D11_FLUSHREQ2(wlc)),
		R_REG(osh, D11_FLUSHREQ1(wlc)),
		R_REG(osh, D11_FLUSHREQ(wlc))));
	WL_ERROR(("wl%d: SuspStatus2/1/0:0x%08x/0x%08x/0x%08x FlushStatus:0x%08x/0x%08x/0x%08x\n",
		wlc->pub->unit, R_REG(osh, D11_CHNSUSP_STATUS2(wlc)),
		R_REG(osh, D11_CHNSUSP_STATUS1(wlc)),
		R_REG(osh, D11_CHNSUSP_STATUS(wlc)),
		R_REG(osh, D11_CHNFLUSH_STATUS2(wlc)),
		R_REG(osh, D11_CHNFLUSH_STATUS1(wlc)),
		R_REG(osh, D11_CHNFLUSH_STATUS(wlc))));

}
static void
wlc_txq_stall_dump_fifos(wlc_info_t *wlc, txstall_data_t *stall)
{
	txq_t *lowtxq = wlc->active_queue->low_txq;
	txq_swq_t *txq_swq;
	hnddma_t *di;
	int npkts;
	uint i;
	BCM_REFERENCE(txq_swq);

	WL_ERROR(("wl%d: ap=%d datablock=0x%x TXPKTPENDTOT=%d\n",
		wlc->pub->unit,	AP_ENAB(wlc->pub), wlc->block_datafifo, TXPKTPENDTOT(wlc)));
	WL_ERROR(("wl%d: pktpend: BK:%d BE:%d VI:%d VO:%d BCMC:%d\n",
		wlc->pub->unit,	TXPKTPENDGET(wlc, TX_AC_BK_FIFO),
		TXPKTPENDGET(wlc, TX_AC_BE_FIFO), TXPKTPENDGET(wlc, TX_AC_VI_FIFO),
		TXPKTPENDGET(wlc, TX_AC_VO_FIFO), TXPKTPENDGET(wlc, TX_BCMC_FIFO)));

	WL_ERROR(("FIFO PKTPEND DMATXP buffered\n"));
	for (i = 0; i < WLC_HW_NFIFO_INUSE(wlc); i++) {
		if (TXPKTPENDGET(wlc, i) > 0) {
			txq_swq = &lowtxq->swq_table[i];
			WL_ERROR(("%3d: %6d %6d %8d\n", i, TXPKTPENDGET(wlc, i),
				dma_txp(WLC_HW_DI(wlc, i)), txq_swq->flowctl.buffered));
		}
		di = WLC_HW_DI(wlc, i);
		npkts = dma_txactive(di);

		if (wlc->txs_fifo_cnts[i]) {
			WL_PRINT(("TXStatus[%3d]: %8d\n", i, wlc->txs_fifo_cnts[i]));
			wlc->txs_fifo_cnts[i] = 0;
		}
		if (npkts == 0) continue;

		setbit(stall->fifo_bitmap, i);

		WL_PRINT(("wl%d: TX FIFO[%d] TXPEND = %d TX-DMA%d =>\n",
			wlc->pub->unit, i, npkts, i));
		dma_dumptx(di, NULL, FALSE);

		WL_PRINT(("AQM FIFO[%d] =>\n", i));
		di = WLC_HW_AQM_DI(wlc, i);
		dma_dumptx(di, NULL, FALSE);
	}

}

static void
wlc_txq_stall_dump_macstats(wlc_info_t *wlc, bool reset_stats, txstall_data_t *stall)
{
	mac_dbg_t now_mdbg;
	mac_dbg_t *mdbg;
	uint16 *x16;
	uint i;
	osl_t *osh = wlc->osh;

	mdbg = &wlc->txqi->mdbg;

	memset(&now_mdbg, 0, sizeof(now_mdbg));
	x16 = (uint16 *)&now_mdbg;

	for (i = 0; i < NUM_MCST_IN_MAC_DBG_T; i++) {
		x16[i] = wlc_read_shm(wlc, MACSTAT_ADDR(wlc, mac_dbg_t_to_macstat_t[i]));
	}
	now_mdbg.txop = wlc_bmac_cca_read_counter(wlc->hw,
		M_CCA_TXOP_L_OFFSET(wlc), M_CCA_TXOP_H_OFFSET(wlc));

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		now_mdbg.txtrig = wlc_read_shm(wlc, M_TXTRIG_CNT(wlc));

		wlc_txq_stall_dump_rev128(wlc);
	}

	if (reset_stats && wlc->block_timeout > 0) {
		/* Don't do this if in test/debug mode when block_timeout < 0. */
		memcpy(mdbg, &now_mdbg, sizeof(now_mdbg));
		bzero(wlc->txs_fifo_cnts, sizeof(wlc->txs_fifo_cnts));
	}

	WL_ERROR(("wl%d: txall %d txbcn %d txrts %d rxcts %d rsptmout %d rxstrt %d glitch %d "
		"txtrig %d txop %d\n",
		wlc->pub->unit, now_mdbg.txallfrm - mdbg->txallfrm, now_mdbg.txbcn - mdbg->txbcn,
		now_mdbg.txrts - mdbg->txrts, now_mdbg.rxcts - mdbg->rxcts,
		now_mdbg.rsptmout - mdbg->rsptmout, now_mdbg.rxstrt - mdbg->rxstrt,
		now_mdbg.glitch - mdbg->glitch, now_mdbg.txtrig - mdbg->txtrig,
		now_mdbg.txop - mdbg->txop));

	/* Copy macstat_t counters */
	memcpy(mdbg, &now_mdbg, sizeof(now_mdbg));

	WL_ERROR(("wl%d: cwcur0-3 %x %x %x %x bslots cur/0-3 %d %d %d %d %d ifs_boff %d\n",
		wlc->pub->unit,	wlc_read_shm(wlc, 0x240 + 3*2), wlc_read_shm(wlc, 0x260 + 3*2),
		wlc_read_shm(wlc, 0x280 + 3*2), wlc_read_shm(wlc, 0x2a0 + 3*2),
		wlc_read_shm(wlc, 0x16f*2), wlc_read_shm(wlc, 0x240 + 5*2),
		wlc_read_shm(wlc, 0x260 + 5*2),	wlc_read_shm(wlc, 0x280 + 5*2),
		wlc_read_shm(wlc, 0x2a0 + 5*2),	R_REG(osh, D11_IFS_BOFF_CTR(wlc))));

#define NAV_CNTR_L (volatile uint16 *)(D11_NAV_STAT(wlc) + 1)
#define NAV_CNTR_H (volatile uint16 *)(D11_NAV_STAT(wlc) + 2)

	WL_ERROR(("wl%d: ifsstat 0x%x nav_stat 0x%x nav_cntr=0x%08x_%08x\n",
		wlc->pub->unit, R_REG(osh, D11_IFS_STAT(wlc)), R_REG(osh, D11_NAV_STAT(wlc)),
		R_REG(osh, NAV_CNTR_H), R_REG(osh, NAV_CNTR_L)));

	for (i = 0; i < 2; i++) {
		WL_PRINT(("again %d ifsstat 0x%x nav_stat 0x%x\n",
			i + 1,	R_REG(osh, D11_IFS_STAT(wlc)), R_REG(osh, D11_NAV_STAT(wlc))));
	}

	WL_PRINT(("wl%d: phydebug :\n", wlc->pub->unit));
	for (i = 0; i < 32; i += 4) {
		uint32 val32[4];

		BCM_REFERENCE(val32);

		val32[0] = R_REG(osh, D11_PHY_DEBUG(wlc));
		val32[1] = R_REG(osh, D11_PHY_DEBUG(wlc));
		val32[2] = R_REG(osh, D11_PHY_DEBUG(wlc));
		val32[3] = R_REG(osh, D11_PHY_DEBUG(wlc));
		WL_ERROR(("0x%-8x 0x%-8x 0x%-8x 0x%-8x\n",
			val32[0], val32[1], val32[2], val32[3]));
	}
	WL_ERROR(("wl%d: pktprocdebug :\n", wlc->pub->unit));
	for (i = 0; i < 8; i += 4) {
		uint16 val16[4];

		BCM_REFERENCE(val16);

		W_REG(osh, D11_PHY_REG_ADDR(wlc), (uint16)0x1f0); /* pktprocdbg */
		val16[0] = R_REG(osh, D11_PHY_REG_DATA(wlc));
		val16[1] = R_REG(osh, D11_PHY_REG_DATA(wlc));
		val16[2] = R_REG(osh, D11_PHY_REG_DATA(wlc));
		val16[3] = R_REG(osh, D11_PHY_REG_DATA(wlc));
		WL_ERROR(("0x%-4x 0x%-4x 0x%-4x 0x%-4x\n",
			val16[0], val16[1], val16[2], val16[3]));
	}
}

static void
wlc_txq_stall_dump_ccastats(wlc_info_t *wlc)
{
	cca_msrmnt_query cca_query;
	cca_stats_n_flags *cca_results = NULL;
	uint cca_results_len = 1024;
	int err;
	osl_t *osh = wlc->osh;

	cca_query.time_req = 0;
	cca_query.msrmnt_query = 1;
	cca_query.report_opt = 2;

	cca_results = MALLOCZ(osh, cca_results_len);
	if (! cca_results) {
		WL_ERROR(("%s: could not alloc mem for cca debug\n", __FUNCTION__));
		return;
	} else {
		if ((err = wlc_iovar_op(wlc, "ccastats", &cca_query, sizeof(cca_query),
				cca_results, cca_results_len, IOV_GET, NULL)) < 0) {
			WL_ERROR(("failed to get ccastats err=%d\n", err));
			goto ccadone1;
		} else {
			WL_PRINT(("wl%d: CCASTATS msmtdone=%d\n%s\n",
				wlc->pub->unit, cca_results->msrmnt_done, cca_results->buf));
		}
	}

	/* new request */
	cca_query.time_req = 900; /* 900 milliseconds - complete prior to next 1sec watchdog */
	cca_query.msrmnt_query = 0;

	if ((err = wlc_iovar_op(wlc, "ccastats", &cca_query, sizeof(cca_query), cca_results,
		cca_results_len, IOV_GET, NULL)) < 0) {
		WL_ERROR(("failed to query ccastats err=%d\n", err));
	}
ccadone1:
	MFREE(osh, cca_results, cca_results_len);
}

static void
wlc_txq_stall_watchdog(wlc_info_t *wlc)
{
	bool stall_detect = FALSE, first_pass = FALSE;
	int dt = 0;
	txq_info_t *txqi = wlc->txqi;
	txstall_data_t *stall;
	bool macdump_ready = FALSE;
#ifdef WLCHANIM
	int valid;
	uint8 stall_rxcca = 0;
	uint8 stall_txavail = 0;
#endif /* WLCHANIM */
	uint32 now = OSL_SYSUPTIME();

	if (!wlc->pub->up) {
		return;
	}

	/* If block_time is set (indicating block_datafifo set)
	 * and block_timeout is set (indicating stall detection enabled)
	 */
	if (wlc->block_time && wlc->block_timeout && wlc->block_datafifo) {
		if (now > wlc->block_time) {
			dt = now - wlc->block_time;
		}
		if (dt > wlc->block_timeout) {
			stall_detect = TRUE;
		}
	}
	/* During CAC, all transmissions are suspended leading to
	 * block_datafifo set for the duration of the CAC.
	 */
	if (wlc_dfs_cac_in_progress(wlc->dfs)) {
		wlc->block_time = 0;
		return;
	}

	if (! stall_detect && wlc->block_timeout >= 0) {
		/* If not stalled and block_timeout is configured.
		 * negative value of block_timeout is used as a self-test
		 * so that the stats can be printed regardless of an
		 * actual stall
		 */
		return;
	}

	WL_ERROR(("wl%d TX Stall: block_datafifo set for %d msec\n",
		wlc->pub->unit, dt));

	/* first_pass is used for calculating 1-second deltas between
	 * stats counters; to reinitialize mac stats deltas to 0 when stall
	 * is detected the first time to avoid garbage deltas being printed.
	 */

	first_pass = dt <= (wlc->block_timeout + 1000);

	if (first_pass) {
		txqi->stall_cnt++;
		txqi->stall_time += wlc->block_timeout;
		/* update history */
		txqi->stall_history_idx = (txqi->stall_history_idx + 1) % TXSTALL_HISTORY_SZ;

		stall = &txqi->stall_history[txqi->stall_history_idx];
		bzero(stall, sizeof(*stall));
		stall->time = wlc->pub->now - (wlc->block_timeout / 1000); /* seconds */
		stall->duration = wlc->block_timeout;
		stall->datablock = wlc->block_datafifo;
	} else {
		txqi->stall_time += 1000; /* watchdog interval 1 sec */
		stall = &txqi->stall_history[txqi->stall_history_idx];
		stall->duration = dt;
		stall->datablock |= wlc->block_datafifo;
	}

#ifdef WLCHANIM
	if (WLC_CHANIM_ENAB(wlc->pub) && wlc->block_chanstats) {
		wl_chanim_stats_t *param;

		param = (wl_chanim_stats_t *) wlc->block_chanstats;
		param->count  = WL_CHANIM_COUNT_ONE;
		param->buflen = sizeof(wl_chanim_stats_t) + 64;
		valid = wlc_iovar_op(wlc, "chanim_stats", param, param->buflen,
			wlc->block_chanstats, param->buflen, IOV_GET, NULL);
		if (valid >= 0) {
			stall_rxcca = wlc->block_chanstats->stats[0].ccastats[CCASTATS_OBSS] +
				wlc->block_chanstats->stats[0].ccastats[CCASTATS_INBSS] +
				wlc->block_chanstats->stats[0].ccastats[CCASTATS_NOCTG] +
				wlc->block_chanstats->stats[0].ccastats[CCASTATS_NOPKT];

			stall_txavail = wlc->block_chanstats->stats[0].ccastats[CCASTATS_TXOP] +
				wlc->block_chanstats->stats[0].ccastats[CCASTATS_TXDUR];
			stall->rxcca = stall_rxcca;
			stall->txavail =  stall_txavail;
			WL_TRACE(("CCASTATs stall_rxcca=%d stall_txavail=%d\n",
				stall_rxcca, stall_txavail));

			if (stall_rxcca >= wlc->block_timeout_rxcca_thr) {
				WL_ERROR(("TX Stall: High RX traffic obss=%d"
					" inbss=%d noctg=%d nopkt=%d\n",
					wlc->block_chanstats->stats[0].ccastats[CCASTATS_OBSS],
					wlc->block_chanstats->stats[0].ccastats[CCASTATS_INBSS],
					wlc->block_chanstats->stats[0].ccastats[CCASTATS_NOCTG],
					wlc->block_chanstats->stats[0].ccastats[CCASTATS_NOPKT]));
				txqi->rxcca_stallcnt++;
				wlc->block_time = 0;
				return;
			} else if (stall_txavail <= wlc->block_timeout_txavail_thr) {
				WL_ERROR(("TX Stall: Low txop=%d txdur=%d\n",
					wlc->block_chanstats->stats[0].ccastats[CCASTATS_TXOP],
					wlc->block_chanstats->stats[0].ccastats[CCASTATS_TXDUR]));
				txqi->txavail_stallcnt++;
				wlc->block_time = 0;
				return;
			}
		} else {
			WL_ERROR(("failed to query chanim_stats valid=%d\n", valid));
		}
	}
#endif /* WLCHANIM */

	wlc_txq_stall_dump_fifos(wlc, stall);

	wlc_txq_stall_dump_macstats(wlc, first_pass, stall);

	wlc_txq_stall_dump_ccastats(wlc);

	macdump_ready = dt >= (wlc->block_timeout_stats * (wlc->block_timeout + 1));
	if (macdump_ready) {
		if (wlc->block_timeout_psmwd && !wlc->block_timeout_once) {
			/* Dump mac logs once */
			WL_PRINT(("<<< TX STUCK DEBUG PSMWD>>>\n"));
			wl_sched_macdbg_dump(wlc->wl);
			wlc->block_time = 0;
			wlc->block_timeout_once++;
		}
		if (wlc->block_timeout_fatal) {
			WL_ERROR(("<<< TX STUCK DEBUG FATAL>>>\n"));
			wlc_fatal_error(wlc);
			wlc->block_time = 0;
		}
		wlc->block_time = 0;
		wl_reset_dump_signature(wlc->wl);
	}
}
#endif /* BCMDBG_TXSTALL */

#ifdef WL_TXPKTPEND_SYNC
static void
wlc_pktpend_sync_flush(wlc_info_t *wlc)
{
	int i, nfifo;
	wlc_hw_info_t *wlc_hw = wlc->hw;
	txq_t *txq = wlc->active_queue->low_txq;
	txq_swq_t *txq_swq;
	struct scb *scb;
	struct scb_iter scbiter;

	nfifo = wlc_hw->pub->nfifo_inuse;
	WL_ERROR(("wl%d %s nfifo %d\n", wlc->pub->unit, __FUNCTION__, nfifo));

	/* make sure we do not open datafifo before resetting the counters */
	wlc->pktpend_sync |= TXPKTPEND_SYNC_PEND;

	/* Flush radio txfifos and free txpktpend packets */
	wlc_sync_txfifo_all(wlc, wlc->active_queue, FLUSHFIFO);

	for (i = 0; i < nfifo; i++) {
		if (wlc->core->txpktpend[i] != 0) {
			WL_ERROR(("FiFo:%d txpend %d\n", i, wlc->core->txpktpend[i]));
			wlc->core->txpktpend[i] = 0;
		}
		txq_swq = TXQ_SWQ(txq, i);
		if (wlc_txq_buffered_time(txq, i)) {
			WL_ERROR(("FiFo:%d buffered %d flag 0x%x\n", i,
				wlc_txq_buffered_time(txq, i), txq_swq->flowctl.flags));
			__txq_swq_buffered_dec(txq_swq, i, wlc_txq_buffered_time(txq, i));
		}
	}
	if (wlc->core->txpktpendtot != 0) {
		WL_ERROR(("txpendtot %d\n", wlc->core->txpktpendtot));
		wlc->core->txpktpendtot = 0;
	}

	/* Resetting scb inflt fifocnts */
	FOREACHSCB(wlc->scbstate, &scbiter, scb) {
		if (scb) {
			for (i = 0; i < NUMPRIO; i++) {
				if (scb->pkts_inflt_fifocnt[i] != 0) {
					WL_ERROR(("scb "MACF" %d cnt %d\n",
						ETHER_TO_MACF(scb->ea), i,
						scb->pkts_inflt_fifocnt[i]));
					wlc->pktpend_sync_drop += scb->pkts_inflt_fifocnt[i];
					scb->pkts_inflt_fifocnt[i] = 0;
				}
			}
		}
	}

	/* Open data fifo */
	wlc->pktpend_sync = 0;
	wlc->pktpend_sync_cnt++;
	WL_ERROR(("wl%d %s done, sync_cnt %d sync_drop=%d\n", wlc->pub->unit, __FUNCTION__,
		wlc->pktpend_sync_cnt, wlc->pktpend_sync_drop));
}
#endif /* WL_TXPKTPEND_SYNC */

#ifdef BCMDBG
void
wlc_validate_empty_fifo_list(wlc_info_t *wlc)
{
	wlc_txq_info_t *qi = wlc->active_queue;
	txq_t *txq = qi->low_txq;
	txq_swq_t *txq_swq;			/**< a non-priority queue backing one d11 FIFO */
	uint fifo_idx;
	dll_t *item_p, *tmp_p;

	/* Check if empty fifo has any pending pkts -- deletion-safe */
	for (item_p = dll_head_p(&txq->empty), tmp_p = item_p->next_p;
			!dll_end(&txq->empty, item_p);
			item_p = tmp_p, tmp_p = item_p->next_p) {
		txq_swq = (txq_swq_t*)item_p; /* BCM_CONTAINER_OF */

		fifo_idx = TXQ_FIFO_IDX(txq, txq_swq);
		if (spktq_n_pkts(&txq_swq->spktq)) {
			WL_ERROR(("wl%d:%s: pend[%d]=%d buf=%d npkts=%d EMPTY\n",
					wlc->pub->unit, __FUNCTION__,
					fifo_idx, TXPKTPENDGET(wlc, fifo_idx),
					__txq_swq_buffered_time(txq_swq),
					spktq_n_pkts(&txq_swq->spktq)));
			WL_ERROR(("detach pending=%d attach pending=%d\n",
					wlc->txfifo_detach_pending,
					wlc->txfifo_attach_pending));
			WL_ERROR(("active=%p [pri=%p excur=%p detach-trans=%p]\n",
					wlc->active_queue, wlc->primary_queue,
					wlc->excursion_queue,
					wlc->txfifo_detach_transition_queue));

#ifndef DONGLEBUILD
			dump_stack();
#else
			ASSERT(0);
#endif /* DONGLEBUILD */

			/* Move swq_node from empty to active list */
			dll_delete(&txq_swq->node);
			dll_append(&txq->active, &txq_swq->node);
		}
	}
}
#endif /* BCMDBG */

static void
wlc_txq_watchdog(void *ctx)
{
	txq_info_t *txqi = (txq_info_t*)ctx;
	wlc_info_t *wlc = txqi->wlc;

#ifdef WL_TXPKTPEND_SYNC
	if (wlc->pktpend_sync) {
		wlc_pktpend_sync_flush(wlc);
	}
#endif /* WL_TXPKTPEND_SYNC */

	/* block_datafifo is not allowed to open during wl reset/down,
	 * so add a timer to recover this situation.
	 */
	wlc_tx_open_datafifo(wlc);
#ifdef BCMDBG
	wlc_validate_empty_fifo_list(wlc);
#endif /* BCMDBG */
#ifdef BCMDBG_TXSTALL
	wlc_txq_stall_watchdog(wlc);
#endif /* BCMDBG_TXSTALL */
}

static int
wlc_txq_down(void *ctx)
{
	txq_info_t *txqi = (txq_info_t*)ctx;
	txq_t *txq;	/**< E.g. the low queue, consisting of a queue for every d11 fifo */

	for (txq = txqi->txq_list; txq != NULL; txq = txq->next) {
		wlc_low_txq_flush(txqi, txq);
	}

	wlc_txq_delq_flush(txqi);

	return 0;
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
static void
txq_init(txq_t *txq, wlc_txq_info_t *qi, uint nfifo)
{
	uint fifo;
	txq_swq_t *txq_swq;		/**< a non-priority queue backing one d11 FIFO */

	txq->next = NULL;

	dll_init(&txq->active);
	dll_init(&txq->empty);

	txq->nfifo = nfifo;

	for (fifo = 0; fifo < nfifo; fifo++) {
		txq_swq = TXQ_SWQ(txq, fifo);
		dll_init(&txq_swq->node); /* init and place swq into txq empty list */
		spktq_init(&txq_swq->spktq, PKTQ_LEN_MAX);
		dll_append(&txq->empty, &txq_swq->node);
	}
}

/** @return E.g. the low queue, consisting of a queue for every d11 fifo */
static txq_t*
wlc_low_txq_alloc(txq_info_t *txqi, wlc_txq_info_t *qi, uint nfifo)
{
	txq_t *txq;	/**< E.g. the low queue, consisting of a queue for every d11 fifo */

	/* The fifos managed by the low txq can never exceed the count of hw fifos */
	ASSERT(nfifo <= WLC_HW_NFIFO_TOTAL(txqi->wlc));

	/* Allocate private state struct */
	if ((txq = MALLOCZ(txqi->osh, sizeof(txq_t))) == NULL) {
		WL_ERROR(("wl%d: %s: MALLOC failed, malloced %d bytes\n",
		          txqi->pub->unit, __FUNCTION__, MALLOCED(txqi->osh)));
		return NULL;
	}

	/* Allocate software queue */
	if ((txq->swq_table = MALLOCZ(txqi->osh, (sizeof(txq_swq_t) * nfifo))) == NULL) {
		MFREE(txqi->osh, txq, sizeof(txq_t));
		WL_ERROR(("wl%d: %s: MALLOC software queue failed, malloced %d bytes\n",
		          txqi->pub->unit, __FUNCTION__, MALLOCED(txqi->osh)));
		return NULL;
	}

	txq_init(txq, qi, nfifo);

	/* add new queue to the list */
	txq->next = txqi->txq_list;
	txqi->txq_list = txq;

	return txq;
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
void
wlc_low_txq_free(txq_info_t *txqi, txq_t* txq)
{
	txq_t *p;		/**< E.g. the low queue, consisting of a queue for every d11 fifo */

	if (txq == NULL) {
		return;
	}

	wlc_low_txq_flush(txqi, txq);

	/* remove the queue from the linked list */
	p = txqi->txq_list;
	if (p == txq)
		txqi->txq_list = txq->next;
	else {
		while (p != NULL && p->next != txq)
			p = p->next;

		if (p != NULL) {
			p->next = txq->next;
		} else {
			/* assert that we found txq before getting to the end of the list */
			WL_ERROR(("%s: did not find txq %p\n",
				__FUNCTION__, OSL_OBFUSCATE_BUF(txq)));
			ASSERT(p != NULL);
		}
	}
	txq->next = NULL;

	/* since entire txq_t is being deleted, no active/empty management */
	dll_init(&txq->active); /* avoid latent access */

	MFREE(txqi->osh, txq->swq_table, (sizeof(*txq->swq_table) * txq->nfifo));
	MFREE(txqi->osh, txq, sizeof(*txq));
}

/** @param spktq  Single priority packet queue */
static void
wlc_spktq_map_pkts(wlc_info_t *wlc, struct spktq *spktq, map_pkts_cb_fn cb, void *ctx)
{
	void *head_pkt, *pkt;

	head_pkt = NULL;
	while (spktq_peek(spktq) != head_pkt) {
		pkt = spktq_deq(spktq);
		ASSERT(pkt);
		cb(ctx, pkt);
		if (!head_pkt)
			head_pkt = pkt;
		spktq_enq(spktq, pkt);
	}
}

void
wlc_low_txq_map_pkts(wlc_info_t *wlc, wlc_txq_info_t *qi, map_pkts_cb_fn cb, void *ctx)
{
	txq_t *txq;		/**< e.g. the low queue, consisting of a queue for every d11 fifo */
	txq_swq_t *txq_swq;	/**< a non-priority queue backing one d11 FIFO */
	struct spktq *spktq;	/**< single priority packet queue */
	dll_t *item_p, *next_p;

	txq = qi->low_txq;

	for (item_p = dll_head_p(&txq->active); !dll_end(&txq->active, item_p); item_p = next_p) {
		next_p = dll_next_p(item_p);
		txq_swq = (txq_swq_t*)item_p;
		spktq = &txq_swq->spktq;

#if defined(BCMHWA)
		HWA_PKTPGR_EXPR({
			if (hpktq_n_pkts(txq_swq) > 0) {
				wlc_hwa_hpktq_map_pkts(wlc, txq_swq, TXQ_FIFO_IDX(txq, txq_swq),
					cb, ctx);
			}
		});
#endif /* BCMHWA */

		wlc_spktq_map_pkts(wlc, spktq, cb, ctx);

		/* Remove node from active list to empty if there is no pending packet */
		if ((spktq_n_pkts(spktq) == 0) && (hpktq_n_pkts(txq_swq) == 0)) {
			dll_delete(&txq_swq->node);
			dll_append(&txq->empty, &txq_swq->node);
		}
	}
}

/** @param q   Multi-priority packet queue */
static void
wlc_pktq_map_pkts(wlc_info_t *wlc, struct pktq *q, map_pkts_cb_fn cb, void *ctx)
{
	int prec;
	void *head_pkt, *pkt;

	PKTQ_PREC_ITER(q, prec) {
		head_pkt = NULL;
		while (pktqprec_peek(q, prec) != head_pkt) {
			pkt = pktq_pdeq(q, prec);
			if (pkt == NULL) {
				break;
			}
			cb(ctx, pkt);
			if (!head_pkt)
				head_pkt = pkt;
			pktq_penq(q, prec, pkt);
		}
	}
}

void
wlc_txq_map_pkts(wlc_info_t *wlc, wlc_txq_info_t *qi, map_pkts_cb_fn cb, void *ctx)
{

	struct pktq *common_q;			/**< Multi-priority packet queue */
	common_q =  WLC_GET_CQ(qi);

	wlc_pktq_map_pkts(wlc, common_q, cb, ctx);
}

#ifdef AP
/** @param q   Multi-priority packet queue */
void
wlc_scb_psq_map_pkts(wlc_info_t *wlc, struct pktq *q, map_pkts_cb_fn cb, void *ctx)
{
	wlc_pktq_map_pkts(wlc, q, cb, ctx);
}
#ifdef WL_USE_SUPR_PSQ
void
wlc_scb_supp_q_map_pkts(wlc_info_t *wlc, struct spktq *spktq, map_pkts_cb_fn cb, void *ctx)
{
	wlc_spktq_map_pkts(wlc, spktq, cb, ctx);
}
#endif /* WL_USE_SUPR_PSQ */
#endif /* AP */

/** @param q   Multi-priority packet queue */
void
wlc_bsscfg_psq_map_pkts(wlc_info_t *wlc, struct pktq *q, map_pkts_cb_fn cb, void *ctx)
{
	wlc_pktq_map_pkts(wlc, q, cb, ctx);
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
void
wlc_low_txq_flush(txq_info_t *txqi, txq_t* txq)
{
	uint fifo_idx;
	uint16 pktcnt;
	txq_swq_t *txq_swq;		/**< a non-priority queue backing one d11 FIFO */
	wlc_info_t *wlc = txqi->wlc;
	dll_t *item_p, *next_p;
	struct spktq *spktq;		/**< single priority packet queue */
	txq_swq_flowctl_t *txq_swq_flowctl;
	void *p;
	BCM_REFERENCE(wlc);

#ifdef WLTAF
	/* Defer taf schedule to prevent pkt nqueue during flush */
	wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */

	for (item_p = dll_head_p(&txq->active); !dll_end(&txq->active, item_p);
		item_p = next_p) {
		next_p = dll_next_p(item_p);
		txq_swq = (txq_swq_t*)item_p;
		fifo_idx = TXQ_FIFO_IDX(txq, txq_swq);
		spktq = &txq_swq->spktq;
		pktcnt = 0;
		txq_swq_flowctl = &txq_swq->flowctl;

#if defined(BCMHWA)
		HWA_PKTPGR_EXPR({
			if (hpktq_n_pkts(txq_swq) > 0) {
				wlc_hwa_hpktq_flush(wlc, txq_swq, fifo_idx, &pktcnt);
			}
		});
#endif /* BCMHWA */

		/* flush any pkts in the software queues */
		/* no need for a mutex protection! */
		/* start with the head of the list */
		while ((p = spktq_deq(spktq)) != NULL) {
			/* delete this packet */
			wlc_txq_free_pkt(wlc, p, &pktcnt);
		}

		ASSERT(pktcnt);

		if (wlc->active_queue->low_txq == txq)
			TXPKTPENDDEC(wlc, fifo_idx, pktcnt);
		else {
			WL_INFORM(("%s: bypass TXPKTENDDEC for inactive low_txq"
				" fifo %d pkts %d\n",
				__FUNCTION__, fifo_idx, pktcnt));
		}

		/* Decrease low txq buffered count accordingly */
		__txq_swq_buffered_dec(txq_swq, fifo_idx, pktcnt);

		WL_INFORM(("%s: flush fifo %d pkts %d\n", __FUNCTION__, fifo_idx, pktcnt));

		if (txq_swq_flowctl->buffered != 0) {
			WL_ERROR(("wl%d: %s: txq 0x%p primary 0x%p excursion 0x%p"
				" txq_swq flowctl[%u].buffered=%d\n",
				wlc->pub->unit, __FUNCTION__,
				txq, wlc->primary_queue ? wlc->primary_queue->low_txq :
				(void *)wlc->primary_queue,
				wlc->excursion_queue->low_txq,
				fifo_idx, txq_swq_flowctl->buffered));
		}
		/* Remove node from active list to empty if there is no pending packet */
		if ((spktq_n_pkts(spktq) == 0) && (hpktq_n_pkts(txq_swq) == 0)) {
			dll_delete(&txq_swq->node);
			dll_append(&txq->empty, &txq_swq->node);
		}
	}

#ifdef WLTAF
	wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */
}

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
void BCMFASTPATH
wlc_txq_complete(txq_info_t *txqi, txq_t *txq, uint fifo_idx, int complete_pkts)
{
	txq_swq_t *txq_swq;			/**< a non-priority queue backing one d11 FIFO */
	BCM_REFERENCE(txqi);

	txq_swq = TXQ_SWQ(txq, fifo_idx);
	__txq_swq_dec(txq_swq, fifo_idx, complete_pkts);

	/* XXX:TXQ
	 * WES: Consider making the a hi/low water mark on the dma ring just like the sw.
	 * On a large ring, it could save some work.
	 * On a small ring, we may want to keep every last bit full
	 * at the cost of often hitting the stop condition
	 */
	/* open up the hw fifo as soon as any pkts free */
	__txq_swq_hw_stop_clr(txq_swq);
}

#if defined(BCMDBG)
/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
static int
wlc_txq_dump(txq_info_t *txqi, txq_t *txq, struct bcmstrbuf *b)
{
	uint fifo_idx;
	txq_swq_t *txq_swq;			/**< a non-priority queue backing one d11 FIFO */
	wlc_info_t *wlc = txqi->wlc;
	const char *name;

	if (txq == NULL || txq->swq_table == NULL) {
		ASSERT(0);
		return BCME_OK;
	}

	if (wlc->active_queue->low_txq == txq) {
		name = "Active Q";
	} else if (wlc->primary_queue != NULL && wlc->primary_queue->low_txq == txq) {
		name = "Primary Q";
	} else if (wlc->excursion_queue->low_txq == txq) {
		name = "Excursion Q";
	} else {
		name = "";
	}

	bcm_bprintf(b, "txq %p: %s\n", OSL_OBFUSCATE_BUF(txq), name);

	for (fifo_idx = 0, txq_swq = txq->swq_table; fifo_idx < txq->nfifo; fifo_idx++, txq_swq++) {
		txq_swq_flowctl_t *f = &txq_swq->flowctl;
		struct pktq_prec  *q = &txq_swq->spktq.q; /**< single precedence packet queue */
#ifdef WLCNT
		txq_swq_stats_t   *s = &txq_swq->stats;
#endif

		bcm_bprintf(b, "fifo %u(p%u):\n", fifo_idx, WLC_HW_MAP_TXFIFO(wlc, fifo_idx));

		bcm_bprintf(b, "qpkts %u flags 0x%x buffered %d\n",
		            (uint)q->n_pkts, f->flags, f->buffered);

#ifdef WLCNT
		bcm_bprintf(b,
			"Total: pkts %u time %u Q stop %u service %u HW stop %u service %u\n",
			s->pkts, s->time, s->q_stops, s->q_service, s->hw_stops, s->hw_service);
#endif
	}

	return BCME_OK;
}

static int
wlc_txq_module_dump(void *ctx, struct bcmstrbuf *b)
{
	txq_info_t *txqi = (txq_info_t *)ctx;
	txq_t *txq;		/**< e.g. the low queue, consisting of a queue for every d11 fifo */

	txq = txqi->txq_list;

	while (txq != NULL) {
		wlc_txq_dump(txqi, txq, b);
		txq = txq->next;
		if (txq != NULL) {
			bcm_bprintf(b, "\n");
		}
	}
	return BCME_OK;
}

#endif

static uint BCMFASTPATH
wlc_scb_tx_ac(struct scb *scb, void *pkt)
{
	uint8 prio;
	enum wme_ac ac;

	ASSERT(scb != NULL);

	/* Default to BE unless WME is enabled */
	ac = AC_BE;

	if (SCB_WME(scb) && SCB_QOS(scb)) {
		prio = (uint8)PKTPRIO(pkt);
		ASSERT(prio <= MAXPRIO);
		ac = WME_PRIO2AC(prio);
	}
	return ac;
}

/**
 * Append a list of packets to Low txq
 * Handle low q accounting for the list of packets
 * @param txq   E.g. the low queue, consisting of a queue for every d11 fifo
 * @param list  Single priority packet queue
 */
void BCMFASTPATH
wlc_low_txq_enq_list(wlc_info_t *wlc, txq_t *txq, uint fifo_idx, struct spktq *list, int pkt_cnt)
{
	txq_swq_t *txq_swq = TXQ_SWQ(txq, fifo_idx); /**< a non-prio queue backing one d11 FIFO */
#ifdef WLCNT
	txq_swq_stats_t *txq_swq_stats = &txq_swq->stats;
#endif

	ASSERT(fifo_idx < WLC_HW_NFIFO_INUSE(wlc));
	ASSERT(pkt_cnt);

	/* Counters update */
	WLCNTADD(txq_swq_stats->pkts, spktq_n_pkts(list));
	WLCNTADD(txq_swq_stats->time, pkt_cnt);
	WLCNTINCR(txq_swq_stats->q_service);

	/* track pending packet count per fifo */
	TXPKTPENDINC(wlc, fifo_idx, (int16)pkt_cnt);

	/* track added time per fifo */
	__txq_swq_inc(txq_swq, fifo_idx, pkt_cnt);

	/* Move swq_node from empty list */
	dll_delete(&txq_swq->node);

	/* append the provided packets to the (lower) txq */
	spktq_enq_list(&txq_swq->spktq, list);

	/* Move swq_node to active list */
	dll_append(&txq->active, &txq_swq->node);

	WL_TRACE(("wl:%s: fifo:%d added cnt %d pktpend  %d  low txq size %d \n", __FUNCTION__,
		fifo_idx, pkt_cnt, TXPKTPENDGET(wlc, fifo_idx),
		spktq_n_pkts(&txq_swq->spktq)));
}

/** @param txq    the low queue, containing of a queue for every d11 fifo */
void BCMFASTPATH
wlc_low_txq_enq(wlc_info_t *wlc, txq_t *txq, uint fifo_idx, void *pkt)
{
	txq_swq_t *txq_swq = TXQ_SWQ(txq, fifo_idx); /**< a non-prio queue backing one d11 FIFO */
#ifdef WLCNT
	txq_swq_stats_t *txq_swq_stats = &txq_swq->stats;
#endif

	/* Move swq_node from empty list */
	dll_delete(&txq_swq->node);

	/* enqueue the packet to the specified fifo queue */
	spktq_enq(&txq_swq->spktq, pkt);

	/* Move swq_node to active list */
	dll_append(&txq->active, &txq_swq->node);

	/* total accumulation counters update */
	WLCNTADD(txq_swq_stats->pkts, 1);
	WLCNTADD(txq_swq_stats->time, 1);

	/* track pending packet count per fifo */
	TXPKTPENDINC(wlc, fifo_idx, 1);
	WL_TRACE(("wl:%s: pktpend inc 1 to %d\n", __FUNCTION__,
	          TXPKTPENDGET(wlc, fifo_idx)));

	/* track added time per fifo */
	__txq_swq_inc(txq_swq, fifo_idx, 1);
}

/**
 * The d11 core has a set of transmit FIFOs, consisting of a small number of SU FIFOs, and a large
 * number of MU FIFOs. The set of transmit FIFOs may not be large enough to contain all traffic
 * classes for all remote nodes. That means that when traffic changes (for instance, MU traffic
 * starts for a certain user), that one of the d11 MU FIFOs has to be dynamically assigned to this
 * specific user/QoS.
 *
 * This function will allocate a MU FIFO if no MU FIFO was yet assigned to the caller-supplied
 * user.
 */
void BCMFASTPATH
wlc_scb_get_txfifo(wlc_info_t *wlc, struct scb *scb, void *pkt, uint *pfifo)
{
	wlc_bsscfg_t *cfg;
	int prio;

	BCM_REFERENCE(cfg);

	ASSERT(pkt != NULL);

	cfg = SCB_BSSCFG(scb);
	ASSERT(cfg != NULL);

	if (D11REV_GE(wlc->pub->corerev, 129) && WLPKTTAG(pkt)->flags & WLF_UTXD) {
		*pfifo = ULMU_TRIG_FIFO;
		return;
	}

	if ((BSSCFG_AP(cfg) || BSSCFG_IBSS(cfg)) && SCB_ISMULTI(scb) && SCB_PS(scb)) {
		/*
		 * On the AP, bcast/mcast always goes through the BCMC fifo if BCMC SCB is in
		 * PS Mode. BCMC SCB is in PS Mode if at least one ucast STA is in PS.
		 */
		*pfifo = TX_BCMC_FIFO;
	} else {
		/* WME: send frames based on priority setting  Default prio has to be 0
		 * for TKIP MIC calculation when sending packet on a non-WME link, as the
		 * receiving end uses prio of 0 for MIC calculation when QoS header is absent
		 * from the packet.
		 */
		prio = 0;
		if (SCB_QOS(scb) || (WLPKTTAG(pkt)->flags & WLF_CTLMGMT)) {
			prio = (uint8)PKTPRIO(pkt);
			ASSERT(prio <= MAXPRIO);
		} else {
			PKTSETPRIO(pkt, prio);
		}
		*pfifo = wlc_fifo_index_get(wlc->fifo, scb, prio2ac[prio]);
	}

	if (WLPKTTAG(pkt)->flags & WLF_TXHDR) {
		/* Update the TX FIFO index in the TXH */
		uint16 *frameid = wlc_get_txh_frameid_ptr(wlc, pkt);
		*frameid = htol16(wlc_frameid_set_fifo(wlc, ltoh16(*frameid), *pfifo));
	}

	return;
}

#ifdef WL_BSSCFG_TX_SUPR
/* XXX:TXQ
 * WES:BSS absence suppression can change into
 * stopping the flow from SCB queues that
 * belong to BSSCFG that are BSS_TX_SUP(). No need to support
 * wlc_bsscfg_tx_psq_enq() since the pkts are already held on a queue.
 */
static bool
wlc_bsscfg_tx_supr(wlc_info_t *wlc, scb_t *scb, void *pkt, int prec, bool *suppress_to_host)
{
	wlc_bsscfg_t *cfg;
	wlc_pkttag_t *pkttag;

	cfg = SCB_BSSCFG(scb);
	ASSERT(cfg != NULL);
	pkttag = WLPKTTAG(pkt);
	if (BSS_TX_SUPR(cfg) && !(pkttag->flags & WLF_PSDONTQ)) {
		ASSERT(!(wlc->block_datafifo & DATA_BLOCK_TX_SUPR));
#ifdef PROP_TXSTATUS
		if (PROP_TXSTATUS_ENAB(wlc->pub)) {
			if (*suppress_to_host) {
				if (wlc_suppress_sync_fsm(wlc, scb, pkt, FALSE)) {
					wlc_process_wlhdr_txstatus(wlc, WLFC_CTL_PKTFLAG_WLSUPPRESS,
						pkt, FALSE);
					PKTFREE(wlc->osh, pkt, TRUE);
				} else {
					*suppress_to_host = FALSE;
				}
			}
			if (*suppress_to_host == FALSE) {
				if (wlc_bsscfg_tx_psq_enq(wlc->psqi, cfg, pkt, prec)) {
					*suppress_to_host = TRUE;
					wlc_suppress_sync_fsm(wlc, scb, pkt, TRUE);
					/* release this packet and move on to the next element in q,
					 * host will resend this later.
					 */
					wlc_process_wlhdr_txstatus(wlc, WLFC_CTL_PKTFLAG_WLSUPPRESS,
						pkt, FALSE);
					TX_PKTDROP_COUNT(wlc, scb, TX_PKTDROP_RSN_PS_ENQ_FAILED);
					PKTFREE(wlc->osh, pkt, TRUE);
				}
			}
		} else
#endif /* PROP_TXSTATUS */
		if (wlc_bsscfg_tx_psq_enq(wlc->psqi, cfg, pkt, prec)) {
			WL_ERROR(("%s: FAILED TO ENQUEUE PKT\n", __FUNCTION__));
			TX_PKTDROP_COUNT(wlc, scb, TX_PKTDROP_RSN_PS_ENQ_FAILED);
			PKTFREE(wlc->osh, pkt, TRUE);
		}
		return TRUE;
	}
#ifdef BCMDBG
	if (wlc_bsscfg_tx_pktcnt(wlc->psqi, cfg) > 0) {
		WL_ERROR(("%s: %u SUPR PKTS LEAKED IN PSQ\n", __FUNCTION__,
			wlc_bsscfg_tx_pktcnt(wlc->psqi, cfg)));
	}
#endif
	return FALSE;
}
#endif /* WL_BSSCFG_TX_SUPR */

/**
 * 'Supply/feed function' of the lower transmit queue. Pulls one or more packets
 * from the common/wlc transmit queue and puts them on the matching
 * output_q. After this function returns, the complete common q has been processed.
 *
 */
static void BCMFASTPATH
wlc_pull_q(wlc_info_t *wlc)
{
	void *pkt;
	void *pkts[DOT11_MAXNUMFRAGS];
	wlc_txq_info_t *qi;
	uint16 prec_map;				/**< precedence map */
	scb_t *scb;
	struct pktq *common_q;				/** Multi-priority packet queue */
	struct spktq output_q;
	struct spktq blocked_q;
	int pkt_cnt;
	void *blocked;
	int prec, prev_prec;
	uint fifo, prev_fifo;
	int max_pkt_cnt = 0;
	int remaining_bcmc_pkts;
	int err, i;
	uint count;
	wlc_pkttag_t *pkttag;
	wlc_txh_info_t txh_info;
	uint16 frameid;
	bool epoch;
	int prio;
#ifdef WL_BSSCFG_TX_SUPR
	bool suppress_to_host = FALSE;
#endif /* WL_BSSCFG_TX_SUPR */
	uint32 txfifo_expiry = 0;
	uint corerev = wlc->pub->corerev;

	/* only do work for the active queue */
	qi = wlc->active_queue;
	ASSERT(qi);
	common_q = WLC_GET_CQ(qi);

	prec_map = WLC_PREC_BMP_ALL;
	prev_prec = WLC_PREC_BMP_ALL;

	spktq_init(&output_q, PKTQ_LEN_MAX);
	spktq_init(&blocked_q, PKTQ_LEN_MAX);

	prev_fifo = 0;
	pkt_cnt = 0;
	while (prec_map && (pkt = cpktq_mdeq(&qi->cpktq, prec_map, &prec))) {
#ifdef BCMPCIEDEV
		/* if PCIE D3 suspend is active then drop packet now */
		if (BCMPCIEDEV_ENAB() && BCMLFRAG_ENAB() &&
			!wlc->cmn->hostmem_access_enabled && PKTISTXFRAG(wlc->osh, pkt)) {
			PKTFREE(wlc->osh, pkt, TRUE);
			continue;
		}
#endif /* BCMPCIEDEV */

		scb = WLPKTTAGSCBGET(pkt);
		ASSERT(scb != NULL);

		ASSERT(!SCB_DEL_IN_PROGRESS(scb)); /* SCB should not be marked for deletion */

		pkttag = WLPKTTAG(pkt);
		if (SCB_MARKED_DEL(scb) && !(pkttag->flags & WLF_CTLMGMT)) {
			PKTFREE(wlc->osh, pkt, TRUE);
			continue;
		}

		if ((scb->dropblock_dur > 0) &&
			((pkttag->flags & WLF_8021X) == 0)) {
			/* drop the non 802.1X packets when rekeying is in progress */
			WLCNTINCR(wlc->pub->_cnt->txfail);
			WLCNTSCBADD(scb->scb_stats.tx_failures, 1);
			PKTFREE(wlc->osh, pkt, TRUE);
			continue;
		}

#ifdef WL_BSSCFG_TX_SUPR
		if (wlc_bsscfg_tx_supr(wlc, scb, pkt, prec, &suppress_to_host)) {
			continue;
		}
#endif /* WL_BSSCFG_TX_SUPR */

		/* If there are packets on blocked list and prec changes then put back the blocked
		 * list and update prec_map so they wont be pulled again.
		 */
		if (prec != prev_prec) {
			if (blocked_q.q.n_pkts) {
				while ((blocked = spktq_deq(&blocked_q))) {
					ASSERT(prev_prec < WLC_PREC_COUNT);
					if (prev_prec < WLC_PREC_COUNT) {
						cpktq_penq(wlc, &qi->cpktq, prev_prec, blocked,
							TRUE);
					}
				}
				if (prev_prec != WLC_PREC_BMP_ALL) {
					prec_map &= ~(1 << prev_prec);
				}
			}
			prev_prec = prec;
		}

#if defined(WL_PS_SCB_TXFIFO_BLK)
		/* Skip PS blocked SCBs unless packet is PSDONTQ */
		if (SCB_PS_TXFIFO_BLK(scb) && !(pkttag->flags & WLF_PSDONTQ)) {
			spktq_enq_head(&blocked_q, pkt);
			continue;
		}
#endif /* WL_PS_SCB_TXFIFO_BLK */

		/* Something is blocking data packets, block_datafifo can come in middle of
		 * processing common queues, this is why this check has to come so late and in
		 * middle of processing the common queue.
		 */
		if (wlc->block_datafifo & ~DATA_BLOCK_JOIN) {
			WL_INFORM(("wl%d: %s: block_datafifo 0x%x\n",
				wlc->pub->unit, __FUNCTION__, wlc->block_datafifo));
			spktq_enq_head(&blocked_q, pkt);
			break;
		}

		/* At this point all checks for dropping/moving out the packet should be done
		 * start initializing fifo and max pkt count if this is the first packet
		 */
		wlc_scb_get_txfifo(wlc, scb, pkt, &fifo);
		if ((pkt_cnt) && (fifo != prev_fifo)) {
			/* New fifo to process. The packest which are on output_q should now
			 * be moved to low_txq
			 */
			wlc_low_txq_enq_list(wlc, qi->low_txq, prev_fifo, &output_q, pkt_cnt);
			pkt_cnt = 0;
		}
		if (pkt_cnt == 0) {
			prev_fifo = fifo;
			/* find out how much room in this fifo of txq */
			max_pkt_cnt = __txq_swq_space(wlc, TXQ_SWQ(qi->low_txq, fifo));
		}

		/* Drop BCMC packets here, when too much outstanding */
		if (SCB_ISMULTI(scb)) {
			/* Limit is FID size (9 bit) but outstanding depends on FIFO in use. If
			 * fifo is TX_BCMC_FIFO then limit is FIFO room left, otherwise uses the
			 * pkt in flight for the SCB.
			 */
			remaining_bcmc_pkts = D11_TXFID_GET_MAX(wlc) - pkt_cnt;
			if (fifo == TX_BCMC_FIFO) {
				 remaining_bcmc_pkts -= (TXPKTPENDGET(wlc, TX_BCMC_FIFO) + 1);
			} else {
				 remaining_bcmc_pkts -= (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb) + 1);
			}
			if (remaining_bcmc_pkts <= 0) {
				PKTFREE(wlc->osh, pkt, TRUE);
				continue;
			}
		}

		/* If there is no more room on low txq then put the the packet on the blocked
		 * queue to skip it.
		 */
		if (pkt_cnt >= max_pkt_cnt) {
			spktq_enq_head(&blocked_q, pkt);
			continue;
		}
		/* after get_txfifo check if scb is paused */
		if ((scb->ac_suspend & (1 << WME_PRIO2AC(PKTPRIO(pkt)))) &&
			!SCB_MARKED_DEL(scb)) {
			spktq_enq_head(&blocked_q, pkt);
			continue;
		}

		/* Send AMPDU using wlc_sendampdu (calls wlc_txfifo() also),
		 * SDU using wlc_prep_sdu and PDU using wlc_prep_pdu followed by
		 * wlc_txfifo() for each of the fragments
		 */
		if (WLPKTFLAG_AMPDU(pkttag)) {
			ASSERT(prec < WLC_PREC_COUNT);
			if (prec < WLC_PREC_COUNT) {
				/* coverity[tainted_data] */
				err = wlc_sendampdu(wlc->ampdu_tx, qi, &pkt, prec, &output_q,
					&pkt_cnt, fifo);
			} else {
				err = BCME_BUSY;
			}
		} else {
			pkts[0] = pkt;
			if (pkttag->flags & (WLF_CTLMGMT | WLF_TXHDR)) {
				count = 1;
				err = wlc_prep_pdu(wlc, scb, pkt, fifo);
			} else {
				err = wlc_prep_sdu(wlc, scb, pkts, &count, fifo);
			}
			/* transmit if no error */
			if (err) {
				/* When SW encryption is active (large number of STAs) then packet
				 * can get replaced with other packet.
				 */
				pkt = pkts[0];
				goto prep_error_processing;
			}
			if (AMPDU_MAC_ENAB(wlc->pub) && !AMPDU_AQM_ENAB(wlc->pub)) {
				wlc_ampdu_change_epoch(wlc->ampdu_tx, fifo, AMU_EPOCH_CHG_MPDU);
			}

			/* Set lifetime prior to low txq */
			if (wlc->lifetime_txfifo && D11REV_GE(corerev, 128)) {
				txfifo_expiry = wlc_lifetime_now(wlc) + wlc->lifetime_txfifo;
			}

			for (i = 0; i < count; i++) {
				pkt = pkts[i];
				wlc_get_txh_info(wlc, pkt, &txh_info);
				frameid = ltoh16(txh_info.TxFrameID);

				if (AMPDU_AQM_ENAB(wlc->pub)) {
					if (i == 0) {
						epoch = wlc_ampdu_chgnsav_epoch(wlc->ampdu_tx,
							fifo, AMU_EPOCH_CHG_MPDU, scb,
							(uint8)PKTPRIO(pkt), FALSE, &txh_info);
						wlc_txh_set_epoch(wlc, txh_info.tsoHdrPtr, epoch);
					}
					if (D11REV_GE(corerev, 128)) {
						struct dot11_header *h;
						uint16 fc_type;
						h = (struct dot11_header *)(txh_info.d11HdrPtr);
						fc_type = FC_TYPE(ltoh16(h->fc));
						wlc_txh_set_ft(wlc, txh_info.tsoHdrPtr, fc_type);
						/* non-AMPDU does not allow dynamic fragment */
						wlc_txh_set_frag_allow(wlc, txh_info.tsoHdrPtr,
							FALSE);
					}
				}

				if (D11_TXFID_IS_RATE_PROBE(corerev, frameid)) {
					wlc_scb_ratesel_probe_ready(wlc->wrsi, scb, frameid,
						FALSE, 0, WME_PRIO2AC(PKTPRIO(pkt)));
				}

				pkttag = WLPKTTAG(pkt);

				/* Set lifetime prior to low txq */
				if (txfifo_expiry) {
					if (pkttag->flags & WLF_EXPTIME) {
						pkttag->u.exptime = MIN(txfifo_expiry,
							pkttag->u.exptime);
					} else {
						pkttag->flags |= WLF_EXPTIME;
						pkttag->u.exptime = txfifo_expiry;
					}
				}

				/* add this pkt to the output queue */
				spktq_enq(&output_q, pkt);
				pkt_cnt += 1;
				SCB_PKTS_INFLT_FIFOCNT_INCR(scb, PKTPRIO(pkt));
			}
		}
prep_error_processing:
		if (err == BCME_BUSY) {
			spktq_enq_head(&blocked_q, pkt);
		}
	}
	/* push back scb blocked packets of previous prec */
	while ((blocked = spktq_deq(&blocked_q))) {
		ASSERT(prev_prec < WLC_PREC_COUNT);
		if (prev_prec < WLC_PREC_COUNT) {
			cpktq_penq(wlc, &qi->cpktq, prev_prec, blocked, TRUE);
		}
	}
	spktq_deinit(&blocked_q);

	if (pkt_cnt) {
		wlc_low_txq_enq_list(wlc, qi->low_txq, prev_fifo, &output_q, pkt_cnt);
	}
	spktq_deinit(&output_q);

	/* Check if flow control needs to be turned off after sending the packet */
	if (!WME_ENAB(wlc->pub) || (wlc->pub->wlfeatureflag & WL_SWFL_FLOWCONTROL)) {
		if (wlc_txflowcontrol_prio_isset(wlc, qi, ALLPRIO) &&
				(pktq_n_pkts_tot(common_q) < wlc->pub->tunables->datahiwat / 2)) {
			wlc_txflowcontrol(wlc, qi, OFF, ALLPRIO);
		}
	} else if (wlc->pub->_priofc) {
		for (prio = MAXPRIO; prio >= 0; prio--) {
			if (wlc_txflowcontrol_prio_isset(wlc, qi, prio) &&
					(pktqprec_n_pkts(common_q, wlc_prio2prec_map[prio]) <
					 wlc->pub->tunables->datahiwat/2)) {
				wlc_txflowcontrol(wlc, qi, OFF, prio);
			}
		}
	}
}

#if defined(WL_PRQ_RAND_SEQ) && !defined(WL_PRQ_RAND_SEQ_DISABLED)
#if defined(BCMDBG)
static void
wlc_tx_prs_bsscfg_dump(void *ctx, wlc_bsscfg_t *cfg, struct bcmstrbuf *b)
{
	txq_info_t *txqi = (txq_info_t *)ctx;
	tx_prs_cfg_t *prs_cfg = TX_PRS_CFG(txqi, cfg);

	bcm_bprintf(b, "\tprq_rand_seq: %02x\n", prs_cfg->prq_rand_seq);
	bcm_bprintf(b, "\tprq_rand_seqId: 0x%x\n", prs_cfg->prq_rand_seqId);
}
#else
#define wlc_tx_prs_bsscfg_dump NULL
#endif

static int
wlc_tx_prs_bsscfg_init(void *ctx, wlc_bsscfg_t *cfg)
{
	txq_info_t *txqi = (txq_info_t *)ctx;
	tx_prs_cfg_t *prs_cfg = TX_PRS_CFG(txqi, cfg);

	/* Initialize randomize probe request seq variables */
	prs_cfg->prq_rand_seq = PRQ_RAND_SEQ_ENABLE;
	prs_cfg->prq_rand_seqId = 0;
	return BCME_OK;
}
#endif /* WL_PRQ_RAND_SEQ && WL_PRQ_RAND_SEQ_DISABLED */

/* module entries */
txq_info_t *
BCMATTACHFN(wlc_txq_attach)(wlc_info_t *wlc)
{
	txq_info_t *txqi;
	int err;
	const bcm_iovar_t *iovars = NULL;
	wlc_iov_disp_fn_t iovar_fn = NULL;

	/* Allocate private states struct. */
	if ((txqi = MALLOCZ(wlc->osh, sizeof(txq_info_t))) == NULL) {
		WL_ERROR(("wl%d: %s: MALLOC failed, malloced %d bytes\n",
		          wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->pub->osh)));
		goto fail;
	}

	txqi->wlc = wlc;
	txqi->pub = wlc->pub;
	txqi->osh = wlc->osh;

	if ((txqi->delq = MALLOC(wlc->osh, sizeof(*(txqi->delq)))) == NULL) {
		WL_ERROR(("wl%d: %s: MALLOC failed, malloced %d bytes\n",
		          wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->pub->osh)));
		goto fail;
	}
	spktq_init(txqi->delq, PKTQ_LEN_MAX);

	iovars = txq_iovars;
	iovar_fn = wlc_txq_doiovar;

	/* Register module entries. */
	err = wlc_module_register(wlc->pub,
	                          iovars,
	                          "txq", txqi,
	                          iovar_fn,
	                          wlc_txq_watchdog, /* watchdog fn */
	                          NULL, /* up fn */
	                          wlc_txq_down);

	if (err != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_module_register() failed\n",
		          txqi->pub->unit, __FUNCTION__));
		goto fail;
	}

#if defined(BCMDBG)
	wlc_dump_register(txqi->pub, "txq", wlc_txq_module_dump, (void *)txqi);
	wlc_dump_register(wlc->pub, "datapath", (dump_fn_t)wlc_datapath_dump, (void *)wlc);
#endif
#if defined(BCMDBG_TXSTALL)
	wlc_dump_add_fns(wlc->pub, "txstall", wlc_txstall_dump, wlc_txstall_dump_clr, wlc);
	if ((wlc->block_chanstats = MALLOCZ(wlc->osh, sizeof(wl_chanim_stats_t) + 64)) == NULL) {
		WL_ERROR(("wl%d: %s: MALLOC block_chanstats failed, malloced %d bytes\n",
		          wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->pub->osh)));
		goto fail;
	}

	wlc->block_timeout = 5000;
	wlc->block_timeout_psmwd = 1;
	wlc->block_timeout_stats = 2;
	wlc->block_timeout_txavail_thr = 10;
	wlc->block_timeout_rxcca_thr = 85;
#endif /* BCMDBG_TXSTALL */

#if defined(WL_PRQ_RAND_SEQ) && !defined(WL_PRQ_RAND_SEQ_DISABLED)
	/* Handler to handle bsscfg cubby */
	txqi->bsscfg_handle = wlc_bsscfg_cubby_reserve(wlc, sizeof(tx_prs_cfg_t),
		wlc_tx_prs_bsscfg_init, NULL, wlc_tx_prs_bsscfg_dump, (void *)txqi);
	if (txqi->bsscfg_handle < 0) {
		WL_ERROR(("wl%d: %s: wlc_bsscfg_cubby_reserve failed with err %d\n",
			txqi->pub->unit, __FUNCTION__, txqi->bsscfg_handle));
		goto fail;
	}
	txqi->pub->_prq_rand_seq = TRUE;
#endif /* WL_PRQ_RAND_SEQ && WL_PRQ_RAND_SEQ_DISABLED */

	/* Get notified when an SCB is freed */
	err = wlc_scb_cubby_reserve(wlc, 0, wlc_txq_scb_init, wlc_txq_scb_deinit, NULL, wlc);
	if (err < 0) {
		WL_ERROR(("wl%d: %s: wlc_scb_cubby_reserve failed with err %d\n",
		          txqi->pub->unit, __FUNCTION__, err));
		goto fail;
	}
	/* Register the 1st txmod */
	wlc_txmod_fn_register(wlc->txmodi, TXMOD_TRANSMIT, wlc, txq_txmod_fns);

#ifndef DONGLEBUILD
	/* Setup wlc_d11hdrs routine */
	if (D11REV_GE(wlc->pub->corerev, 128)) {
		wlc->wlc_d11hdrs_fn = wlc_d11hdrs_rev128;
	}
	else if (D11REV_GE(wlc->pub->corerev, 40)) {
		wlc->wlc_d11hdrs_fn = wlc_d11hdrs_rev40;
	}
	else {
		wlc->wlc_d11hdrs_fn = wlc_d11hdrs_pre40;
	}
#endif /* DONGLEBUILD */

	txqi->mutx_ac_mask = MUTX_TYPE_AC_ALL;
	return txqi;
fail:
	MODULE_DETACH(txqi, wlc_txq_detach);
	return NULL;
}

void
BCMATTACHFN(wlc_txq_detach)(txq_info_t *txqi)
{
	osl_t *osh;
	txq_t *txq, *next;	/**< e.g. the low queue, consisting of a queue for every d11 fifo */

	if (txqi == NULL)
		return;

	osh = txqi->osh;

	for (txq = txqi->txq_list; txq != NULL; txq = next) {
		next = txq->next;
		wlc_low_txq_free(txqi, txq);

		/* delete these asserts */
		ASSERT(dll_empty(&txq->active));
		ASSERT(dll_empty(&txq->empty));
	}

	wlc_module_unregister(txqi->pub, "txq", txqi);

	if (txqi->delq != NULL) {
		spktq_deinit(txqi->delq);
		MFREE(osh, txqi->delq, sizeof(*(txqi->delq)));
	}

#if defined(BCMDBG_TXSTALL)
	MFREE(osh, txqi->wlc->block_chanstats, sizeof(wl_chanim_stats_t) + 64);
#endif /* BCMDBG_TXSTALL */
	MFREE(osh, txqi, sizeof(txq_info_t));
}

static int
wlc_txq_doiovar(void *context, uint32 actionid,
	void *params, uint p_len, void *arg, uint len, uint vsize, struct wlc_if *wlcif)
{
	txq_info_t *txqi = (txq_info_t*)context;
	int32 int_val = 0;
	int32 *ret_int_ptr;
	int err = BCME_OK;

	BCM_REFERENCE(len);
	BCM_REFERENCE(txqi);
	BCM_REFERENCE(ret_int_ptr);

	/* convenience int parameter for sets */
	if (p_len >= (int)sizeof(int_val))
		bcopy(params, &int_val, sizeof(int_val));

	/* convenience int ptr for 4-byte gets (requires int aligned arg) */
	ret_int_ptr = (int32 *)arg;

	switch (actionid) {
#ifdef BCMDBG
#if defined(WL_PRQ_RAND_SEQ)
	case IOV_GVAL(IOV_PRQ_RAND_SEQ): {
		wlc_bsscfg_t *bsscfg = wlc_bsscfg_find_by_wlcif(txqi->wlc, wlcif);
		tx_prs_cfg_t *tx_prs_cfg;

		if (!bsscfg || !(PRQ_RAND_SEQ_ENAB(txqi->wlc->pub))) {
			err = BCME_ERROR;
			break;
		}
		tx_prs_cfg = (tx_prs_cfg_t*)TX_PRS_CFG(txqi, bsscfg);
		*ret_int_ptr = (uint32)(tx_prs_cfg->prq_rand_seq & PRQ_RAND_SEQ_ENABLE);
		break;
	}

	case IOV_SVAL(IOV_PRQ_RAND_SEQ): {
		wlc_bsscfg_t *bsscfg = wlc_bsscfg_find_by_wlcif(txqi->wlc, wlcif);
		tx_prs_cfg_t *tx_prs_cfg;
		if (!bsscfg || !(PRQ_RAND_SEQ_ENAB(txqi->wlc->pub))) {
			err = BCME_ERROR;
			break;
		}

		tx_prs_cfg = (tx_prs_cfg_t*)TX_PRS_CFG(txqi, bsscfg);
		if (int_val != 0) {
			tx_prs_cfg->prq_rand_seq |= PRQ_RAND_SEQ_ENABLE;
		} else {
			tx_prs_cfg->prq_rand_seq &= ~PRQ_RAND_SEQ_ENABLE;
		}
		break;
	}
#endif /* WL_PRQ_RAND_SEQ */
#endif /* BCMDBG */
	case IOV_SVAL(IOV_MUTX_AC): {
		wl_mutx_ac_mg_t *mutx_ac_mask_in = (wl_mutx_ac_mg_t *)params;
		if ((mutx_ac_mask_in->ac < AC_COUNT) && (mutx_ac_mask_in->mask <= MU_AC_MASK)) {
			uint16 bit_shift, old_mask;
			bit_shift = MU_AC_MASK_SIZE * mutx_ac_mask_in->ac;
			old_mask = MU_AC_MASK & (txqi->mutx_ac_mask >> bit_shift);
			if (old_mask == mutx_ac_mask_in->mask)
				break;
			txqi->mutx_ac_mask &= ~(MU_AC_MASK << bit_shift);
			txqi->mutx_ac_mask |= mutx_ac_mask_in->mask << bit_shift;
			/* If cache enabled, invalidate it. */
			if (WLC_TXC_ENAB(txqi->wlc)) {
				wlc_txc_inv_all(txqi->wlc->txc);
			}
			/* When MU is disabled, all MU FIFOs assigned to SCBs for given MU/AC
			 * combination should be reset to SU FIFO. When MU is enabled, MU admission
			 * control logic followed by traffic for any AC will allocate MU FIFO.
			 */
			wlc_fifo_mutx_ac_release(txqi->wlc->fifo, mutx_ac_mask_in);

#ifdef WLTAF
			{
				int tid;
				struct wlc_taf_info * taf_handle = txqi->wlc->taf_handle;

				for (tid = 0; tid < NUMPRIO; tid++) {
					if (mutx_ac_mask_in->ac != WME_PRIO2AC(tid)) {
						continue;
					}
					wlc_taf_sched_state(taf_handle, NULL, tid,
						mutx_ac_mask_in->mask & 1 << (WL_MU_DLMMU - 1),
						TAF_NO_SOURCE, TAF_SCHED_STATE_MU_DL_HEMIMO);
					wlc_taf_sched_state(taf_handle, NULL, tid,
						mutx_ac_mask_in->mask & 1 << (WL_MU_DLOFDMA - 1),
						TAF_NO_SOURCE, TAF_SCHED_STATE_MU_DL_OFDMA);
				}
			}
#endif /* WLTAF */
		} else {
			err = BCME_BADARG;
		}
		break;

	}
	case IOV_GVAL(IOV_MUTX_AC): {
		wl_mutx_ac_mg_t *mutx_ac_mask_in = (wl_mutx_ac_mg_t *)params;
		wl_mutx_ac_mg_t *mutx_ac_mask_ret = (wl_mutx_ac_mg_t *)arg;
		if (mutx_ac_mask_in->ac < AC_COUNT) {
			uint16 ret_mask;
			mutx_ac_mask_ret->ac = mutx_ac_mask_in->ac;
			ret_mask = txqi->mutx_ac_mask >> (MU_AC_MASK_SIZE * mutx_ac_mask_in->ac);
			mutx_ac_mask_ret->mask = MU_AC_MASK & ret_mask;
		} else {
			err = BCME_BADARG;
		}
		break;
	}
#ifdef BCMDBG_TXSTALL
	case IOV_SVAL(IOV_DATABLOCK_TIMEOUT): {
		wlc_info_t * wlc = txqi->wlc;
		if (int_val >= DATABLOCK_TIMEOUT_MSEC || int_val == 0) {
			wlc->block_timeout = int_val;
		} else {
			err = BCME_RANGE;
		}
		break;
	}
	case IOV_GVAL(IOV_DATABLOCK_TIMEOUT): {
		wlc_info_t * wlc = txqi->wlc;
		*ret_int_ptr = wlc->block_timeout;
		break;
	}
	case IOV_SVAL(IOV_FORCE_DATABLOCK): {
		wlc_info_t * wlc = txqi->wlc;
		wlc->block_datafifo = int_val & 0xffff;
		break;
	}
	case IOV_GVAL(IOV_FORCE_DATABLOCK): {
		wlc_info_t * wlc = txqi->wlc;
		*ret_int_ptr = wlc->block_datafifo;
		break;
	}
	case IOV_SVAL(IOV_DATABLOCK_TIMEOUT_PSMWD): {
		wlc_info_t * wlc = txqi->wlc;
		wlc->block_timeout_psmwd = int_val;
		break;
	}
	case IOV_GVAL(IOV_DATABLOCK_TIMEOUT_PSMWD): {
		wlc_info_t * wlc = txqi->wlc;
		*ret_int_ptr = wlc->block_timeout_psmwd;
		break;
	}
	case IOV_SVAL(IOV_DATABLOCK_TIMEOUT_FATAL): {
		wlc_info_t * wlc = txqi->wlc;
		wlc->block_timeout_fatal = int_val;
		break;
	}
	case IOV_GVAL(IOV_DATABLOCK_TIMEOUT_FATAL): {
		wlc_info_t * wlc = txqi->wlc;
		*ret_int_ptr = wlc->block_timeout_fatal;
		break;
	}
	case IOV_SVAL(IOV_DATABLOCK_TIMEOUT_STATS): {
		wlc_info_t * wlc = txqi->wlc;
		wlc->block_timeout_stats = int_val;
		break;
	}
	case IOV_GVAL(IOV_DATABLOCK_TIMEOUT_STATS): {
		wlc_info_t * wlc = txqi->wlc;
		*ret_int_ptr = wlc->block_timeout_stats;
		break;
	}
	case IOV_SVAL(IOV_DATABLOCK_TIMEOUT_TXAVAIL_THR): {
		wlc_info_t * wlc = txqi->wlc;
		wlc->block_timeout_txavail_thr = int_val;
		break;
	}
	case IOV_GVAL(IOV_DATABLOCK_TIMEOUT_TXAVAIL_THR): {
		wlc_info_t * wlc = txqi->wlc;
		*ret_int_ptr = wlc->block_timeout_txavail_thr;
		break;
	}
	case IOV_SVAL(IOV_DATABLOCK_TIMEOUT_RXCCA_THR): {
		wlc_info_t * wlc = txqi->wlc;
		wlc->block_timeout_rxcca_thr = int_val;
		break;
	}
	case IOV_GVAL(IOV_DATABLOCK_TIMEOUT_RXCCA_THR): {
		wlc_info_t * wlc = txqi->wlc;
		*ret_int_ptr = wlc->block_timeout_rxcca_thr;
		break;
	}
#endif /* BCMDBG_TXSTALL */
#ifdef WL_TXPKTPEND_SYNC
	case IOV_SVAL(IOV_FORCE_PKTPEND_SYNC): {
		wlc_info_t * wlc = txqi->wlc;
		if (int_val == 1) {
			wlc->pktpend_sync |= TXPKTPEND_SYNC_EN;
		} else {
			err = BCME_BADARG;
		}
		break;
	}
	case IOV_GVAL(IOV_FORCE_PKTPEND_SYNC): {
		wlc_info_t * wlc = txqi->wlc;
		*ret_int_ptr = wlc->pktpend_sync;
		break;
	}
#endif /* WL_TXPKTPEND_SYNC */

	case IOV_SVAL(IOV_NEWKEY_PKTDROP_TIMEOUT): {
		txqi->wlc->pub->newkey_pktdrop_timeout = int_val;
		break;
	}
	case IOV_GVAL(IOV_NEWKEY_PKTDROP_TIMEOUT): {
		*ret_int_ptr = txqi->wlc->pub->newkey_pktdrop_timeout;
		break;
	}

	default:
		err = BCME_UNSUPPORTED;
		break;
	}

	return err;
}

static int
wlc_txq_scb_init(void *ctx, struct scb *scb)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;

	/* Init the basic feature tx path to regular tx function */
	wlc_txmod_config(wlc->txmodi, scb, TXMOD_TRANSMIT);
	return BCME_OK;
}

static void
wlc_txq_scb_deinit(void *ctx, struct scb *scb)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;

#ifdef WLTAF
	/* Defer taf schedule to prevent packet enqueue during flush */
	wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */
	/* Flush packets in queues and FIFOs. */
	wlc_tx_fifo_scb_flush(wlc, scb);
#ifdef WLTAF
	wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */
}

void
wlc_block_datafifo(wlc_info_t *wlc, uint32 mask, uint32 val)
{
	uint16 block = wlc->block_datafifo;
	uint16 new_block;

	/* the mask should specify some flag bits */
	ASSERT((mask & 0xFFFF) != 0);
	/* the value should specify only valid flag bits */
	ASSERT((val & 0xFFFF0000) == 0);
	/* value should only have bits in the mask */
	ASSERT((val & ~mask) == 0);

	new_block = (block & ~mask) | val;

	/* just return if no change */
	if (block == new_block) {
		return;
	}
	wlc->block_datafifo = new_block;
#ifdef BCMDBG_TXSTALL
	if (!wlc->block_datafifo) {
		/* Re-arm dump mac flag */
		wlc->block_timeout_once = 0;
		/* reset block time */
		wlc->block_time = 0;
	}
#endif /* BCMDBG_TXSTALL */
#ifdef WLCFP
	/* Update CFP states, when ever data fifo state changes
	 * All registered SCB CFP cubbies are updated
	 */
	wlc_cfp_state_update(wlc->cfp);
#endif /* WLCFP */
#ifdef WLTAF
	wlc_taf_sched_state(wlc->taf_handle, NULL, ALLPRIO, wlc->block_datafifo,
		TAF_NO_SOURCE, TAF_SCHED_STATE_DATA_BLOCK_FIFO);
#endif
} /* wlc_block_datafifo */

/* This function will check if the packet has to be fragmeted or not
 * and based on this for TKIp, pktdfetch will be done
 */
bool
wlc_is_packet_fragmented(wlc_info_t *wlc, struct scb *scb, void *lb)
{
	enum wme_ac ac;
	uint16 thresh;
	osl_t *osh;
	int btc_mode;
	uint pkt_length, bt_thresh = 0;
	osh = wlc->osh;

	ac = wlc_scb_tx_ac(scb, lb);
	thresh = wlc->fragthresh[ac];

	btc_mode = wlc_btc_mode_get(wlc);
	if (IS_BTCX_FULLTDM(btc_mode))
		bt_thresh = wlc_btc_frag_threshold(wlc, scb);
	if (bt_thresh)
		thresh = thresh > bt_thresh ? bt_thresh : thresh;

	pkt_length = pkttotlen(osh, (void *)lb);
	pkt_length -= ETHER_HDR_LEN;
	if (pkt_length < (uint)(thresh - (DOT11_A4_HDR_LEN + DOT11_QOS_LEN +
			DOT11_FCS_LEN + ETHER_ADDR_LEN + TKIP_MIC_SIZE))) {
		return FALSE;
	}

	return TRUE;
}

/**
 * Attempts to fill lower transmit queue with packets from wlc/common queue.
 * @param wlc
 * @param qi
 * - for each priority, forwards packets from wlc/common q to lower tx q
 * - attempts to transfer packets from lower tx q to d11/dma q
 */
void BCMFASTPATH
wlc_send_q(wlc_info_t *wlc, wlc_txq_info_t *qi)
{
	/* Don't send the packets while flushing DMA and low tx queues */
	if (!wlc->pub->up || wlc->hw->reinit) {
		return;
	}
	if (wlc->fifoflush_scb) {
		/* do not fill queues during fifo flush */
		return;
	}
	/* Detaching queue is still pending, don't queue packets to FIFO */
	if (wlc->txfifo_detach_pending) {
		return;
	}
	if (qi != wlc->active_queue) {
		return;
	}

	if (wlc->in_send_q) {
		return;
	}
	wlc->in_send_q = TRUE;

#ifdef WLTAF
	/* In slow path low_txq enqueue, defer taf schedule to prevent low_txq enqueue reentry */
	wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */
	wlc_pull_q(wlc);
#ifdef WLTAF
	/* slow path low_txq enqueue done. resume taf schedule */
	wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */

	txq_hw_fill_active(wlc->txqi, qi->low_txq);

	wlc->in_send_q = FALSE;
} /* wlc_send_q */

void
wlc_send_active_q(wlc_info_t *wlc)
{
	if (WLC_TXQ_OCCUPIED(wlc)) {
		wlc_send_q(wlc, wlc->active_queue);
	}
}

static int
wlc_wme_wmmac_check_fixup(wlc_info_t *wlc, struct scb *scb, void *sdu)
{
	int bcmerror;
	uint fifo_prio;
	uint fifo_ret;
	uint8 prio = (uint8)PKTPRIO(sdu);

	fifo_ret = fifo_prio = prio2fifo[prio];
	bcmerror = wlc_wme_downgrade_fifo(wlc, &fifo_ret, scb);
	if (bcmerror == BCME_OK) {
		/* Change packet priority if fifo changed */
		if (fifo_prio != fifo_ret) {
			prio = fifo2prio[fifo_ret];
			PKTSETPRIO(sdu, prio);
		}
	}

	return bcmerror;
}

/*
 * Enqueues a packet on txq, then sends as many packets as possible.
 * Packets are enqueued to a maximum depth of WLC_DATAHIWAT*2.
 * tx flow control is turned on at WLC_DATAHIWAT, and off at WLC_DATAHIWAT/2.
 *
 * NOTE: it's important that the txq be able to accept at least
 * one packet after tx flow control is turned on, as this ensures
 * NDIS does not drop a packet.
 *
 * Returns TRUE if packet discarded and freed because txq was full, FALSE otherwise.
 */
bool BCMFASTPATH
wlc_sendpkt(wlc_info_t *wlc, void *sdu, struct wlc_if *wlcif)
{
	struct scb *scb = NULL;
	osl_t *osh;
	struct ether_header *eh;
	struct ether_addr *dst;
	wlc_bsscfg_t *bsscfg;
	struct ether_addr *wds = NULL;
	bool discarded = FALSE;
#ifdef WLTDLS
	struct scb *tdls_scb = NULL;
#endif
	void *pkt, *n;
	int8 bsscfgidx = -1;
	uint32 lifetime = 0;
	char eabuf[ETHER_ADDR_STR_LEN];
#ifdef WL_RELMCAST
	uint8 flag = FALSE;
#endif
	wlc_bss_info_t *current_bss;
	uint prec;
#if defined(PKTC) || defined(PKTC_TX_DONGLE)
	uint next_fid;
#endif
	enum wlc_bandunit bandunit;
	wlc_key_info_t scb_key_info;
	wlc_key_info_t bss_key_info;
	wlc_tx_pktdrop_reason_t toss_reason; /* must be set if tossing */
	BCM_REFERENCE(eabuf);

	WL_TRACE(("wl%d: wlc_sendpkt\n", wlc->pub->unit));

	osh = wlc->osh;

	/* sanity */
	ASSERT(sdu != NULL);
	ASSERT(PKTLEN(osh, sdu) >= ETHER_HDR_LEN);

	if (PKTLEN(osh, sdu) < ETHER_HDR_LEN)
	{
		toss_reason = TX_PKTDROP_RSN_LEN_TOO_SHORT;
		goto toss_silently;
	}

	/* figure out the bsscfg for this packet */
	bsscfg = wlc_bsscfg_find_by_wlcif(wlc, wlcif);
	ASSERT(bsscfg != NULL);
	if (bsscfg == NULL) {
		toss_reason = TX_PKTDROP_RSN_BSSCFG_DOWN;
		goto toss_silently;
	}

	if (wlcif == NULL) {
		if (bsscfg->wlcif == NULL) {
			toss_reason = TX_PKTDROP_RSN_WLCIF_REMOVED;
			goto toss_silently;
		}
		wlcif = bsscfg->wlcif;
	}

	ASSERT(WLPKTTAG(sdu) != NULL);

	eh = (struct ether_header*) PKTDATA(osh, sdu);

#ifdef ENABLE_CORECAPTURE
	if (ntoh16(eh->ether_type) ==  ETHER_TYPE_802_1X) {
		char dst[ETHER_ADDR_STR_LEN] = {0};
		char src[ETHER_ADDR_STR_LEN] = {0};
		WIFICC_LOGDEBUG(("wl%d: %s: 802.1x dst %s src %s\n",
			wlc->pub->unit, __FUNCTION__,
			bcm_ether_ntoa((struct ether_addr*)eh->ether_dhost, dst),
			bcm_ether_ntoa((struct ether_addr*)eh->ether_shost, src)));
	}
#endif /* ENABLE_CORECAPTURE */

	WL_APSTA_TX(("wl%d: wlc_sendpkt() pkt %p, len %u, wlcif %p type %d\n",
	             wlc->pub->unit, OSL_OBFUSCATE_BUF(sdu), pkttotlen(osh, sdu),
			OSL_OBFUSCATE_BUF(wlcif), wlcif->type));

	/* QoS map set */
	if (WL11U_ENAB(wlc)) {
		wlc_11u_set_pkt_prio(wlc->m11u, bsscfg, sdu);
	}

#ifdef WET
	/* Apply WET only on the STA interface */
	if (WET_ENAB(wlc) && BSSCFG_STA(bsscfg)) {
		if (((scb = wlc_scbfind(wlc, bsscfg, &bsscfg->BSSID)) != NULL) &&
			!SCB_DWDS(scb) &&
			wlc_wet_send_proc(wlc->weth, bsscfg, sdu, &sdu) < 0) {
				toss_reason = TX_PKTDROP_RSN_WET;
				goto toss;
		}
	}
#endif /* WET */

	if (wlcif->type == WLC_IFTYPE_WDS) {
		scb = wlcif->u.scb;
		wds = &scb->ea;
	}

#ifdef WL_HAPD_WDS
#ifndef DONGLEBUILD
	if ((wlcif->type == WLC_IFTYPE_BSS) && BSSCFG_AP(bsscfg) &&
		(eh->ether_type == hton16(ETHER_TYPE_802_1X))) {
		if (!(ETHER_ISMULTI(eh->ether_dhost))) {
			struct scb *wds_scb = NULL;
			wds_scb = wlc_scbfind(wlc, bsscfg, (struct ether_addr*)eh->ether_dhost);
			if (wds_scb && SCB_LEGACY_WDS(wds_scb)) {
				WL_APSTA_TX(("wl%d: %s: setting wds scb \n",
					wlc->pub->unit, __FUNCTION__));
				wds = &wds_scb->ea;
				wlcif = wds_scb->wds;
				scb = wds_scb;
			}
		}
	}
#endif /* DONGLEBUILD */
#endif /* WL_HAPD_WDS */

	/* discard if we're not up or not yet part of a BSS */
	if (!wlc->pub->up ||
	    (!wds &&
	     (!bsscfg->up ||
	      (BSSCFG_STA(bsscfg) && ETHER_ISNULLADDR(&bsscfg->BSSID))))) {
		WL_INFORM(("wl%d: %s: bsscfg %d is not permitted to transmit. "
		           "wlc up:%d bsscfg up:%d BSSID %s\n", wlc->pub->unit, __FUNCTION__,
		           WLC_BSSCFG_IDX(bsscfg), wlc->pub->up, bsscfg->up,
		           bcm_ether_ntoa(&bsscfg->BSSID, eabuf)));
		WLCNTINCR(wlc->pub->_cnt->txnoassoc);
		toss_reason = TX_PKTDROP_RSN_BSSCFG_DOWN;
		goto toss_silently;
	}

#ifdef L2_FILTER
	if (L2_FILTER_ENAB(wlc->pub)) {
		if (wlc_l2_filter_send_data_frame(wlc, bsscfg, sdu) == 0) {
			toss_reason = TX_PKTDROP_RSN_L2_FILTER;
			goto toss;
		}
	}
#endif /* L2_FILTER */

#ifdef WL_MCAST_FILTER_NOSTA
	if (ETHER_ISMULTI(eh->ether_dhost) && bsscfg->mcast_filter_nosta && !wds &&
			(wlc_bss_assocscb_getcnt(wlc, bsscfg) == 0)) {
		toss_reason = TX_PKTDROP_RSN_MULTICAST_NO_SCB;
		goto toss;
	}
#endif

#ifdef WMF
	/* Do the WMF processing for multicast packets */
#if (defined(BCM_AWL) || defined(BCM_WLAN_RXMC_RNR_OFFLOAD)) && \
	defined(BCM_WLAN_PER_CLIENT_FLOW_LEARNING)
	if (!PKTISRX(sdu)) {
#endif /* BCM_AWL||BCM_WLAN_RXMC_RNR_OFFLOAD && BCM_WLAN_PER_CLIENT_FLOW_LEARNING */
	if (!wds && WMF_ENAB(bsscfg) &&
	    (ETHER_ISMULTI(eh->ether_dhost) ||
	     (bsscfg->wmf_ucast_igmp && is_igmp(eh)))) {
		/* We only need to process packets coming from the stack/ds */
		switch (wlc_wmf_packets_handle(wlc->wmfi, bsscfg, NULL, sdu, 0)) {
		case WMF_TAKEN:
			/* The packet is taken by WMF return */
			return (FALSE);
		case WMF_DROP:
			/* Packet DROP decision by WMF. Toss it */
			toss_reason = TX_PKTDROP_RSN_WMF_DROP;
			goto toss;
		default:
			/* Continue the transmit path */
			break;
		}
	}
#if (defined(BCM_AWL) || defined(BCM_WLAN_RXMC_RNR_OFFLOAD)) && \
	defined(BCM_WLAN_PER_CLIENT_FLOW_LEARNING)
	}
#endif
#endif /* WMF */

#ifdef MAC_SPOOF
	/* in MAC Clone/Spoof mode, change our MAC address to be that of the original
	 * sender of the packet.  This will allow full layer2 bridging for that client.
	 * Note:  This is to be used in STA mode, and is not to be used with WET.
	 */
	if (wlc->mac_spoof &&
	    bcmp(&eh->ether_shost, &wlc->pub->cur_etheraddr, ETHER_ADDR_LEN)) {
		if (WET_ENAB(wlc))
			WL_ERROR(("Configuration Error,"
				"MAC spoofing not supported in WET mode"));
		else {
			bcopy(&eh->ether_shost, &wlc->pub->cur_etheraddr, ETHER_ADDR_LEN);
			bcopy(&eh->ether_shost, &bsscfg->cur_etheraddr, ETHER_ADDR_LEN);
			WL_INFORM(("%s:  Setting WL device MAC address to %s",
				__FUNCTION__,
				bcm_ether_ntoa(&bsscfg->cur_etheraddr, eabuf)));
			wlc_set_mac(bsscfg);
		}
	}
#endif /* MAC_SPOOF */

#ifdef WLWNM_AP
	/* Do the WNM processing */
	/* if packet was handled by DMS/FMS already, bypass it */
	if (BSSCFG_AP(bsscfg) && WLWNM_ENAB(wlc->pub) && WLPKTTAGSCBGET(sdu) == NULL) {
		/* try to process wnm related functions before sending packet */
		int ret = wlc_wnm_packets_handle(bsscfg, sdu, 1);
		switch (ret) {
		case WNM_TAKEN:
			/* The packet is taken by WNM return */
			return FALSE;
		case WNM_DROP:
			/* The packet drop decision by WNM free and return */
			toss_reason = TX_PKTDROP_RSN_WNM_DROP;
			goto toss;
		default:
			/* Continue the forwarding path */
			break;
		}
	}
#endif /* WLWNM_AP */

#ifdef WET_TUNNEL
	if (BSSCFG_AP(bsscfg) && WET_TUNNEL_ENAB(wlc->pub)) {
		wlc_wet_tunnel_send_proc(wlc->wetth, sdu);
	}
#endif /* WET_TUNNEL */

#ifdef PSTA
	if (PSTA_ENAB(wlc->pub) && BSSCFG_STA(bsscfg)) {
		/* If a connection for the STAs or the wired clients
		 * doesn't exist create it. If connection already
		 * exists send the frames on it.
		 */
		if (wlc_psta_send_proc(wlc->psta, &sdu, &bsscfg) != BCME_OK) {
			WL_INFORM(("wl%d: tossing frame\n", wlc->pub->unit));
			toss_reason = TX_PKTDROP_RSN_PSTA_DROP;
			goto toss;
		}
	}
#endif /* PSTA */

	if (wds)
		dst = wds;
	else if (BSSCFG_AP(bsscfg)) {
#ifdef WLWNM_AP
		/* Do the WNM processing */
		if (WLWNM_ENAB(wlc->pub) &&
		    wlc_wnm_dms_amsdu_on(wlc, bsscfg) &&
		    WLPKTTAGSCBGET(sdu) != NULL) {
			dst = &(WLPKTTAGSCBGET(sdu)->ea);
		} else
#endif /* WLWNM_AP */
		dst = (struct ether_addr*)eh->ether_dhost;
	} else {
		dst = bsscfg->BSS ? &bsscfg->BSSID : (struct ether_addr*)eh->ether_dhost;
#ifdef WL_RELMCAST
		/* increment rmc tx frame counter only when RMC is enabled */
		if (RMC_ENAB(wlc->pub) && ETHER_ISMULTI(dst))
			flag = TRUE;
#endif
#ifdef WLTDLS
		if (TDLS_ENAB(wlc->pub) && wlc_tdls_isup(wlc->tdls)) {
			/* if BSSID equal to DA, don't create lookaside_t structure
			 * JIRA:SWWLAN-78213
			 */
			if (memcmp(&bsscfg->BSSID, (void *)eh->ether_dhost, ETHER_ADDR_LEN)) {
				tdls_scb = wlc_tdls_query(wlc->tdls, bsscfg,
					sdu, (struct ether_addr*)eh->ether_dhost);
				if (tdls_scb) {
					dst = (struct ether_addr*)eh->ether_dhost;
					bsscfg = tdls_scb->bsscfg;
					wlcif = bsscfg->wlcif;
					WL_TMP(("wl%d:%s(): to dst %s, use TDLS, bsscfg=0x%p\n",
						wlc->pub->unit, __FUNCTION__,
						bcm_ether_ntoa(dst, eabuf),
						OSL_OBFUSCATE_BUF(bsscfg)));
					ASSERT(bsscfg != NULL);
				}
			}
		}
#endif /* WLTDLS */

	}

	current_bss = bsscfg->current_bss;

	bandunit = CHSPEC_BANDUNIT(current_bss->chanspec);

#ifdef WL_IAPP
	/* check IAPP L2 update frame */
	if (IAPP_ENAB(wlc->pub) && !wds && BSSCFG_AP(bsscfg) && ETHER_ISMULTI(dst)) {

		if ((ntoh16(eh->ether_type) == ETHER_TYPE_IAPP_L2_UPDATE) &&
			((WLPKTTAG(sdu)->flags & WLF_HOST_PKT) ||
#ifdef PROP_TXSTATUS
			(WL_TXSTATUS_GET_FLAGS(WLPKTTAG(sdu)->wl_hdr_information) &
			WLFC_PKTFLAG_PKTFROMHOST) ||
#endif /* PROP_TXSTATUS */
			0)) {
			struct ether_addr *src;

			src = (struct ether_addr*)eh->ether_shost;

			/* cleanup the scb */
			if ((scb = wlc_scbfindband(wlc, bsscfg, src, bandunit)) != NULL) {
				WL_ASSOC(("wl%d.%d: %s: IAPP L2 disassoc station "MACF"\n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__,
					ETHERP_TO_MACF(src)));
				wlc_bss_mac_event(wlc, bsscfg, WLC_E_DISASSOC_IND, &scb->ea,
					WLC_E_STATUS_SUCCESS, DOT11_RC_DISASSOC_LEAVING, 0, 0, 0);
#ifdef WLTDLS
				if (tdls_scb == scb) {
					tdls_scb = NULL;
				}
#endif /* WLTDLS */
				wlc_scbfree(wlc, scb);
				scb = NULL;
			}
		}
	}
#endif /* WL_IAPP */

	/* Done with WLF_HOST_PKT flag; clear it now. This flag should not be
	 * used beyond this point as it is overloaded on WLF_FIFOPKT. It is
	 * set when pkt leaves the per port layer indicating it is coming from
	 * host or bridge.
	 */
	WLPKTTAG(sdu)->flags &= ~WLF_HOST_PKT;

	/* toss if station is not associated to the correct bsscfg. Make sure to use
	 * the band while looking up as we could be scanning on different band
	 */
	/* Class 3 (BSS) frame */
	if (!wds && bsscfg->BSS && !ETHER_ISMULTI(dst)) {
		if ((scb = wlc_scbfindband(wlc, bsscfg, dst, bandunit)) == NULL) {
			WL_INFORM(("wl%d: %s: invalid class 3 frame to "
				"non-associated station %s\n", wlc->pub->unit, __FUNCTION__,
				bcm_ether_ntoa(dst, eabuf)));
#if defined(BCM_BLOG_UNKNOWN_UCAST)
			/* Notify blog of dropping unknown UCAST packets in acceleration */
			wl_handle_blog_emit_drop(wlc->wl, bsscfg->wlcif->wlif, sdu, NULL);
#endif /* BCM_BLOG_UNKNOWN_UCAST */

			WLCNTINCR(wlc->pub->_cnt->txnoassoc);
			toss_reason = TX_PKTDROP_RSN_CLASS3_BSS;
			goto toss;
		}
	}
	/* Class 1 (IBSS/TDLS) or 4 (WDS) frame */
	else {
#if defined(WLTDLS)
		if ((TDLS_ENAB(wlc->pub)) && (tdls_scb != NULL))
			scb = tdls_scb;
		else
#endif /* defined(WLTDLS) */

		if (ETHER_ISMULTI(dst)) {
			scb = WLC_BCMCSCB_GET(wlc, bsscfg);
			if (scb == NULL) {
				WL_ERROR(("wl%d: %s: invalid multicast frame\n",
				          wlc->pub->unit, __FUNCTION__));
				WLCNTINCR(wlc->pub->_cnt->txnoassoc);
				toss_reason = TX_PKTDROP_RSN_INV_MCAST_FRAME;
				goto toss;
			}
		} else if (scb == NULL &&
		         (scb = wlc_scblookupband(wlc, bsscfg, dst, bandunit)) == NULL) {
			WL_ERROR(("wl%d: %s: out of scbs\n", wlc->pub->unit, __FUNCTION__));
			WLCNTINCR(wlc->pub->_cnt->txnobuf);
			/* Increment interface stats */
			if (wlcif) {
				WLCNTINCR(wlcif->_cnt->txnobuf);
			}
			toss_reason = TX_PKTDROP_RSN_MAX_SCBS;
			goto toss;
		}
	}

	if (SCB_DEL_IN_PROGRESS(scb) || SCB_MARKED_DEL(scb)) {
		toss_reason = TX_PKTDROP_RSN_NO_SCB;
		goto toss;
	}

	/* per-port code must keep track of WDS cookies */
	ASSERT(!wds || SCB_WDS(scb));

	if (SCB_LEGACY_WDS(scb)) {
		/* Discard frame if wds link is down */
		if (!(scb->flags & SCB_WDS_LINKUP)) {
			WLCNTINCR(wlc->pub->_cnt->txnoassoc);
			toss_reason = TX_PKTDROP_RSN_INV_CLASS3_NO_ASSOC;
#if defined(BCM_BLOG_UNKNOWN_UCAST)
			/* Notify blog of dropping unknown UCAST packets in acceleration */
			wl_handle_blog_emit_drop(wlc->wl, scb->wds->wlif, sdu, NULL);
#endif /* BCM_BLOG_UNKNOWN_UCAST */
			goto toss;
		}

		/* Discard 802_1X frame if it belongs to another WDS interface
		 * Frame might be received here due to bridge flooding.
		 */
		if (eh->ether_type == hton16(ETHER_TYPE_802_1X)) {
			struct scb *scb_da;

			scb_da = wlc_scbapfind(wlc, (struct ether_addr *)(&eh->ether_dhost));

			if (scb_da && (scb != scb_da)) {
				toss_reason = TX_PKTDROP_RSN_BRIGE_FLOODING;
				goto toss;
			}
		}
	}

#ifdef DWDS
	/* For MultiAP, Maintaining a list of clients attached to the DWDS STA.
	 * Using this mac address table to avoid handling BCMC packets
	 * that get looped back from rootAP.
	 */
	if (MAP_ENAB(bsscfg) && BSSCFG_STA(bsscfg) && bsscfg->dwds_loopback_filter &&
		SCB_DWDS(scb) && ETHER_ISMULTI(eh->ether_dhost)) {
		uint8 *sa = eh->ether_shost;
		if (wlc_dwds_findsa(wlc, bsscfg, sa) == NULL) {
			wlc_dwds_addsa(wlc, bsscfg, sa);
		}
	}
#endif /* DWDS */

#if defined(WLTDLS)
	if (TDLS_ENAB(wlc->pub))
		ASSERT(!tdls_scb || BSSCFG_IS_TDLS(bsscfg));
#endif /* defined(WLTDLS) */

#ifdef PROP_TXSTATUS
	if (PROP_TXSTATUS_ENAB(wlc->pub)) {
#ifdef WLTDLS
		bool tdls_action = FALSE;
		if (eh->ether_type == hton16(ETHER_TYPE_89_0D))
			tdls_action = TRUE;
#endif
#ifdef WLMCHAN
		if (MCHAN_ACTIVE(wlc->pub)) {
			/* Free up packets to non active queue */
			if (wlc->primary_queue) {
				wlc_bsscfg_t *other_cfg = wlc_mchan_get_other_cfg_frm_q(wlc,
					wlc->primary_queue);
				if ((other_cfg == scb->bsscfg) && !SCB_ISMULTI(scb)) {
					FOREACH_CHAINED_PKT(sdu, n) {
						/* Pretend packet never came at all as it is
						 * being suppressed. Ignoring for TX HC
						 * accounting purposes as packets are
						 * not queued
						 */
						PKTCLRCHAINED(osh, sdu);
						if (WLFC_CONTROL_SIGNALS_TO_HOST_ENAB(wlc->pub))
							wlc_suppress_sync_fsm(wlc, scb, sdu, TRUE);
						wlc_process_wlhdr_txstatus(wlc,
							WLFC_CTL_PKTFLAG_WLSUPPRESS, sdu, FALSE);
						PKTFREE(wlc->pub->osh, sdu, TRUE);
					}
					return FALSE;
				}
			}
		}
#endif /* WLMCHAN */
		if (!SCB_ISMULTI(scb) && WLFC_CONTROL_SIGNALS_TO_HOST_ENAB(wlc->pub) &&
#ifdef WLTDLS
			!tdls_action &&
#endif
		1) {
			void *head = NULL, *tail = NULL;
			void *pktc_head = sdu;
			uint32 pktc_head_flags = WLPKTTAG(sdu)->flags;

			FOREACH_CHAINED_PKT(sdu, n) {
				if (wlc_suppress_sync_fsm(wlc, scb, sdu, FALSE)) {
					/* Pretend packet never came at all as it
					 * is being suppressed. Ignoring for TX HC accounting
					 * purposes as packets are
					 * not queued
					 */
					PKTCLRCHAINED(osh, sdu);
					/* wlc_suppress? */
					wlc_process_wlhdr_txstatus(wlc, WLFC_CTL_PKTFLAG_WLSUPPRESS,
						sdu, FALSE);
					PKTFREE(wlc->pub->osh, sdu, TRUE);
				} else {
					PKTCENQTAIL(head, tail, sdu);
				}
			}

			/* return if all packets are suppressed */
			if (head == NULL)
				return FALSE;
			sdu = head;

			/* XXX: head packet in the chain was suppressed. So, need to copy
				saved flags from the dropped head pkt to new head pkt.
			*/
			if (pktc_head != sdu)
				WLPKTTAG(sdu)->flags = pktc_head_flags;
		}
	}
#endif /* PROP_TXSTATUS */

#if defined(BCM_BLOG) && defined(BCM_WLAN_PER_CLIENT_FLOW_LEARNING)
	if (wl_handle_blog_emit(wlc->wl, bsscfg->wlcif->wlif, sdu, NULL) == BCME_ERROR)  {
		toss_reason = TX_PKTDROP_RSN_BLOG_DROP;
		goto toss;
	}
#endif

	/* allocs headroom, converts to 802.3 frame */
	pkt = wlc_hdr_proc(wlc, sdu, scb);

	/* Header conversion failed */
	if (pkt == NULL) {
		toss_reason = TX_PKTDROP_RSN_HDR_CONV_FAILED;
		goto toss;
	}

	/*
	 * Header conversion is done and packet is in 802.3 format
	 * 802.3 format:
	 * -------------------------------------------
	 * |                   |   DA   |   SA   | L |
	 * -------------------------------------------
	 *                          6        6     2
	 */

	eh = (struct ether_header *)PKTDATA(osh, pkt);
	if (ntoh16(eh->ether_type) > DOT11_MAX_MPDU_BODY_LEN) {
		toss_reason = TX_PKTDROP_RSN_HDR_CONV_FAILED;
		goto toss;
	}

	sdu = pkt;

#ifdef BCM_CEVENT
	if (CEVENT_STATE(wlc->pub) && (WLPKTTAG(pkt)->flags & WLF_8021X) != 0) {
		wlc_send_cevent(wlc, bsscfg, SCB_EA(scb), 0, 0, 0, NULL, 0,
				CEVENT_D2C_MT_EAP_TX,
				CEVENT_D2C_FLAG_QUEUED | CEVENT_FRAME_DIR_TX);
	}
#endif /* BCM_CEVENT */

	/* early discard of non-8021x frames if keys are not plumbed */
	(void)wlc_keymgmt_get_tx_key(wlc->keymgmt, scb, bsscfg, &scb_key_info);
	(void)wlc_keymgmt_get_bss_tx_key(wlc->keymgmt, bsscfg, FALSE, &bss_key_info);
	if ((WSEC_ENABLED(bsscfg->wsec) && !WSEC_SES_OW_ENABLED(bsscfg->wsec)) &&
	    !(WLPKTTAG(pkt)->flags & WLF_8021X) &&
#ifdef BCMWAPI_WAI
	    !(WLPKTTAG(pkt)->flags & WLF_WAI) &&
#endif
	    (scb_key_info.algo == CRYPTO_ALGO_OFF) &&
	    (bss_key_info.algo == CRYPTO_ALGO_OFF)) {
		/* Silently toss multicast frames when no STA associates to AP */
		if (SCB_ISMULTI(scb) && BSSCFG_AP(bsscfg) &&
			wlc_bss_assocscb_getcnt(wlc, bsscfg) == 0) {
			toss_reason = TX_PKTDROP_RSN_MULTICAST_NO_SCB;
			goto toss_silently;
		}

		WL_INFORM(("wl%d: %s: tossing unencryptable frame, flags=0x%x\n",
			wlc->pub->unit, __FUNCTION__, WLPKTTAG(pkt)->flags));
		toss_reason = TX_PKTDROP_RSN_CRYPTO_ALGO_OFF;
		goto toss;
	}

	bsscfgidx = WLC_BSSCFG_IDX(bsscfg);
	ASSERT(BSSCFGIDX_ISVALID(bsscfgidx));
	WLPKTTAGBSSCFGSET(sdu, bsscfgidx);
	WLPKTTAGSCBSET(sdu, scb);

	/* wlc_wme_wmmac_check_fixup is called by wlc_ampdu_agg()
	 * Now we do it here, the top level fn wlc_sendpkt().
	 * This way we're done for good no matter how many forks
	 * (AMPDU, AMPDU+PKTC, non-AMPDU, etc.).
	 * Becasue ap mode didn't implement the TSPEC now,
	 * so ap mode skip WMMAC downgrade check.
	 */
	if (SCB_WME(scb) && BSSCFG_STA(bsscfg)) {
		if (wlc_wme_wmmac_check_fixup(wlc, scb, sdu) != BCME_OK) {
			/* let the main prep code drop the frame and account it properly */
			WL_CAC(("%s: Pkt dropped. WMMAC downgrade check failed.\n", __FUNCTION__));
			toss_reason = TX_PKTDROP_RSN_WMMAC_FAIL;
			goto toss;
		}
	}

#ifdef	WLCAC
	if (CAC_ENAB(wlc->pub)) {

		if (!wlc_cac_is_traffic_admitted(wlc->cac, WME_PRIO2AC(PKTPRIO(sdu)), scb)) {
			WL_CAC(("%s: Pkt dropped. Admission not granted for ac %d pktprio %d\n",
				__FUNCTION__, WME_PRIO2AC(PKTPRIO(sdu)), PKTPRIO(sdu)));
			toss_reason = TX_PKTDROP_RSN_DROP_CAC_PKT;
			goto toss;
		}

		if (BSSCFG_AP(bsscfg) && !SCB_ISMULTI(scb)) {
			wlc_cac_reset_inactivity_interval(wlc->cac, WME_PRIO2AC(PKTPRIO(sdu)), scb);
		}
	}
#endif /* WLCAC */

	/* Set packet lifetime if configured */
	if (!(WLPKTTAG(sdu)->flags & WLF_EXPTIME) &&
	    (lifetime = wlc->lifetime[(SCB_WME(scb) ? WME_PRIO2AC(PKTPRIO(sdu)) : AC_BE)]))
		wlc_lifetime_set(wlc, sdu, lifetime);

	WLPKTTAG(sdu)->flags |= WLF_DATA;
#ifdef STA
	/* Restart the Dynamic Fast Return To Sleep sleep return timer if needed */
	wlc_update_sleep_ret(bsscfg, FALSE, TRUE, 0, pkttotlen(wlc->osh, sdu));
#ifdef WL11K
	if (WL11K_ENAB(wlc->pub)) {
		wlc_rrm_upd_data_activity_ts(wlc->rrm_info);
	}
#endif /* WL11K */
#endif /* STA */

	prec = WLC_PRIO_TO_PREC(PKTPRIO(sdu));

#if defined(PKTC) || defined(PKTC_TX_DONGLE)
	next_fid = SCB_TXMOD_NEXT_FID(scb, TXMOD_START);

	if ((next_fid == TXMOD_AMPDU) || ((next_fid == TXMOD_NAR) && PKTC_TX_ENAB(wlc->pub))) {
		WL_TRACE(("%s: ampdu mod for sdu %p chained %d\n", __FUNCTION__,
			OSL_OBFUSCATE_BUF(sdu), PKTISCHAINED(sdu)));
		SCB_TX_NEXT(TXMOD_START, scb, sdu, prec);
	} else {
		/* Modules other than ampdu and NAR are not aware of chaining */
		FOREACH_CHAINED_PKT(sdu, n) {
			PKTCLRCHAINED(osh, sdu);
			if (n != NULL) {
				wlc_pktc_sdu_prep(wlc, scb, sdu, n);
			}

			SCB_TX_NEXT(TXMOD_START, scb, sdu, prec);
		} /* FOREACH_CHAINED_PKT(sdu, n) */
	} /* (next_fid == TXMOD_AMPDU) */
#else /* PKTC */
	SCB_TX_NEXT(TXMOD_START, scb, sdu, prec);
#endif /* PKTC */

#ifdef WL_RELMCAST
	if (RMC_ENAB(wlc->pub) && flag) {
		wlc_rmc_tx_frame_inc(wlc);
	}
#endif
	if (WLC_TXQ_OCCUPIED(wlc)) {
		wlc_send_q(wlc, wlcif->qi);
	}

#ifdef WL_EXCESS_PMWAKE
	wlc->excess_pmwake->ca_txrxpkts++;
#endif /* WL_EXCESS_PMWAKE */

	return (FALSE);

toss:
#ifndef WL_PKTDROP_STATS
	if (WL_INFORM_ON()) {
		wlc_log_unexpected_tx_frame_log_8023hdr(wlc, eh, toss_reason);
	}
#endif

toss_silently:
	FOREACH_CHAINED_PKT(sdu, n) {
		PKTCLRCHAINED(osh, sdu);

		/* Increment wme stats */
		if (WME_ENAB(wlc->pub)) {
			WLCNTINCR(wlc->pub->_wme_cnt->tx_failed[WME_PRIO2AC(PKTPRIO(sdu))].packets);
			WLCNTADD(wlc->pub->_wme_cnt->tx_failed[WME_PRIO2AC(PKTPRIO(sdu))].bytes,
			         pkttotlen(osh, sdu));
		}

#ifdef PROP_TXSTATUS
		if (PROP_TXSTATUS_ENAB(wlc->pub)) {
			wlc_process_wlhdr_txstatus(wlc, WLFC_CTL_PKTFLAG_TOSSED_BYWLC, sdu, FALSE);
		}
#endif /* PROP_TXSTATUS */

		WL_APSTA_TX(("wl%d: %s: tossing pkt %p\n",
			wlc->pub->unit, __FUNCTION__, OSL_OBFUSCATE_BUF(sdu)));
		discarded = TRUE;
		TX_PKTDROP_COUNT(wlc, scb, toss_reason);
		PKTFREE(osh, sdu, TRUE);
	}
	return (discarded);
} /* wlc_sendpkt */

#ifdef AP
static bool
wlc_is_eapfail_msg(wlc_info_t *wlc, void *sdu, bool *is_m3)
{
	struct dot11_llc_snap_header *lsh;
	eap_header_t *eap_hdr = NULL;
	osl_t *osh = wlc->osh;
	uchar *pbody = NULL;
	uint total_len = pkttotlen(osh, sdu);
	uint len1;
	bool retval = FALSE;
	void *next = NULL;
	uint16 ether_type;
	eapol_wpa_key_header_t *eapol_key_hdr;
	uint16 keyinfo_m3;
	*is_m3 = FALSE;

	/* validate if it is EAP FAIL pkt */
	if (total_len <
		((uint)ETHER_HDR_LEN +
		DOT11_LLC_SNAP_HDR_LEN +
		EAPOL_HDR_LEN +
		EAP_HEADER_LEN)) {
		goto exit;
	}

	total_len -= ETHER_HDR_LEN;
	len1 = PKTLEN(osh, sdu);
	pbody = (uchar *)(PKTDATA(osh, sdu));

	len1 -= ETHER_HDR_LEN;
	pbody += ETHER_HDR_LEN;

	/* check and verify LLCSNAP header */
	if (len1 >= ((uint)DOT11_LLC_SNAP_HDR_LEN)) {
		lsh = (struct dot11_llc_snap_header *)(pbody);
	} else {
		goto exit;
	}

	len1 -= DOT11_LLC_SNAP_HDR_LEN;
	pbody += DOT11_LLC_SNAP_HDR_LEN;
	ether_type = ntoh16(lsh->type);

#ifdef MULTIAP
	/* For MultiAP we have started sending VLAN Tagged EAPOL. So, if the ethertype is VLAN,
	 * get the ether type after VLAN TAG
	 */
	if (ether_type == ETHER_TYPE_8021Q) {
		uint16 *type = (uint16*)(pbody + (VLAN_TAG_LEN - ETHER_TYPE_LEN));
		ether_type = ntoh16(*type);
		len1 -= VLAN_TAG_LEN;
		pbody += VLAN_TAG_LEN;
	}
#endif /* MULTIAP */

	/* Check if it is EAPOL or not. If the ethertype is VLAN, check is the next ethertype
	 * is EAPOL or not
	 */
	if (!(lsh->dsap == 0xaa &&
		lsh->ssap == 0xaa &&
		lsh->ctl == 0x03 &&
		lsh->oui[0] == 0 &&
		lsh->oui[1] == 0 &&
		lsh->oui[2] == 0 &&
		(ether_type == ETHER_TYPE_802_1X))) {
		goto exit;
	}

	/* Move to the next buffer if we have reached the end of the current buffer */
	if (len1 == 0) {
		next  = PKTNEXT(osh, sdu);
		if (next == NULL)
			return FALSE;
		pbody = PKTDATA(osh, next);
		len1 = PKTLEN(osh, next);
	}

	/* check and verify the EAPOL version header */
	if (len1 < (EAPOL_HDR_LEN + EAP_HEADER_LEN)) {
		goto exit;
	}

	pbody += EAPOL_HDR_LEN;
	eap_hdr = (eap_header_t*)pbody;

	if (eap_hdr->code == EAP_FAILURE) {
		retval = TRUE;
	}

	/* Key message-3 has PAIRWISE, ACK and MIC bit set */
	eapol_key_hdr = (eapol_wpa_key_header_t *)pbody;
	keyinfo_m3 = WPA_KEY_PAIRWISE | WPA_KEY_ACK | WPA_KEY_MIC;
	*is_m3 = (ntoh16(eapol_key_hdr->key_info) & keyinfo_m3) == keyinfo_m3;

exit:
	return retval;
}

/* PCB for EAP failure. We send Deauth frame to Enrollee. This is done to ensure that all deauth
 * frame GO after EAP-Failure
 */
static void
wlc_send_deauth_on_txcmplt(wlc_info_t *wlc, uint txstatus, void *arg)
{
	struct scb *scb;
	wlc_bsscfg_t *bsscfg;
	wlc_deauth_send_cbargs_t *cbarg = arg;

	ASSERT(cbarg);

	if (!(txstatus & TX_STATUS_ACK_RCV)) {
		WL_ERROR(("wl%d: %s: EAP Failure TX failed %d\n",
			wlc->pub->unit, __FUNCTION__, cbarg->_idx));
		goto exit;
	}

	/* Is this scb still around */
	bsscfg = wlc_bsscfg_find_by_ID(wlc, cbarg->_idx);
	if (bsscfg == NULL) {
		WL_ERROR(("wl%d: %s: unable to find bsscfg by ID %d\n",
			wlc->pub->unit, __FUNCTION__, cbarg->_idx));
		goto exit;
	}

	if ((scb = wlc_scbfind(wlc, bsscfg, &cbarg->ea)) == NULL) {
		goto exit;
	}

	if (SCB_IS_UNUSABLE(scb)) {
		goto exit;
	}

	/* Send deauth packet following EAP failure */
	WL_ASSOC(("wl%d.%d: %s: send deauth to "MACF" with reason %d\n", wlc->pub->unit,
		WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, ETHER_TO_MACF(cbarg->ea),
		DOT11_RC_8021X_AUTH_FAIL));
	wlc_send_deauth(wlc, bsscfg, scb, &cbarg->ea, &bsscfg->BSSID,
		&bsscfg->cur_etheraddr,  DOT11_RC_8021X_AUTH_FAIL);

exit:
	MFREE(wlc->osh, cbarg, sizeof(*cbarg));
}

/* Schedule de-auth tx as pkt callback on EAP fail tx status */
static void
wlc_sched_deauth_on_txcmplt(wlc_info_t *wlc, struct scb *scb, void *sdu)
{
	wlc_deauth_send_cbargs_t *args;
	struct ether_header *eh;

	/* save params used during pcb */
	if (sdu != NULL) {
		args = MALLOCZ(wlc->osh, sizeof(*args));
		if (args == NULL) {
			WL_ERROR(("%s: failed to allocate %d bytes\n",
				__FUNCTION__, (uint)sizeof(*args)));
			return;
		}

		eh = (struct ether_header*) PKTDATA(wlc->osh, sdu);
		memcpy(&args->ea, &eh->ether_dhost, sizeof(struct ether_addr));
		args->_idx = (int8)scb->bsscfg->ID;
		args->pkt = sdu;

		if (wlc_pcb_fn_register(wlc->pcb, wlc_send_deauth_on_txcmplt, (void *)args, sdu)) {
			WL_ERROR(("wl%d: %s: out of pkt callbacks\n",
				wlc->pub->unit, __FUNCTION__));
			MFREE(wlc->osh, args, sizeof(*args));
		}
	}
}
#endif /* AP */

/**
 * Common transmit packet routine, called when one or more SDUs in a queue are to be forwarded to
 * the d11 hardware. Called by e.g. wlc_send_q() and _wlc_sendampdu_aqm().
 *
 * Parameters:
 *     scb   : remote party to send the packets to
 *     *pkts : an array of SDUs (so no MPDUs)
 *
 * Return 0 when a packet is accepted and should not be referenced again by the caller -- wlc will
 * free it.
 * Return an error code when we're flow-controlled and the caller should try again later..
 *
 * - determines encryption key to use
 */
int BCMFASTPATH
wlc_prep_sdu(wlc_info_t *wlc, scb_t *scb, void *pkts[DOT11_MAXNUMFRAGS], uint *npkts, uint fifo)
{
	uint i, j, nfrags, frag_length = 0, next_payload_len;
	void *sdu;
	uint offset;
	osl_t *osh;
	uint headroom = wlc->txhroff, want_tailroom = 0;
	uint pkt_length;
	uint8 prio = 0;
	struct ether_header *eh;
	bool is_8021x;
#ifdef BCMWAPI_WAI
	uint32 is_wai;
#endif /* BCMWAPI_WAI */
	bool fast_path;
	wlc_bsscfg_t *bsscfg;
	wlc_pkttag_t *pkttag;
	bool is_tkip = FALSE;
	bool key_prep_sdu = FALSE;

#ifdef STA
	bool is_4way_m4 = FALSE;
#endif /* STA */
	wlc_key_t *key = NULL;
	wlc_key_info_t key_info;
	int skip_enc;
	uint8 hdrLength = ETHER_HDR_LEN;
	bool broadcast = FALSE;
	wlc_tx_pktdrop_reason_t toss_reason; /* must be set if tossing */

	/* Make sure that we are passed in an SDU */
	sdu = pkts[0];
	ASSERT(sdu != NULL);
	pkttag = WLPKTTAG(sdu);
	ASSERT(pkttag != NULL);

	ASSERT((pkttag->flags & (WLF_CTLMGMT | WLF_TXHDR)) == 0);

	osh = wlc->osh;

	ASSERT(scb != NULL);

	/* Something is blocking data packets */
	if (wlc->block_datafifo) {
		WL_INFORM(("wl%d: %s: block_datafifo 0x%x\n",
			wlc->pub->unit, __FUNCTION__, wlc->block_datafifo));
		return BCME_BUSY;
	}

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
	if (PKTISTXFRAG(wlc->osh, sdu) &&
		!PKTHASHEAPBUF(wlc->osh, sdu) &&
		!D11BUF_POOL_AVAIL()) {
		WL_INFORM(("wl%d: %s: d11 lfrag pool empty\n",
		           wlc->pub->unit, __FUNCTION__));
		return BCME_BUSY;
	}
#endif
	is_8021x = (WLPKTTAG(sdu)->flags & WLF_8021X);
	if (is_8021x) {
		WL_ASSOC_LT(("tx:802.1x for "MACF"\n", ETHER_TO_MACF(scb->ea)));
	}
#ifdef BCMWAPI_WAI
	is_wai = (WLPKTTAG(sdu)->flags & WLF_WAI);
#endif /* BCMWAPI_WAI */

	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);

	WL_APSTA_TX(("wl%d.%d: wlc_prep_sdu: pkt %p dst "MACF"\n", wlc->pub->unit,
		WLC_BSSCFG_IDX(bsscfg), OSL_OBFUSCATE_BUF(sdu),
		ETHER_TO_MACF(scb->ea)));

	/* check enough headroom has been reserved */
	/* uncomment after fixing vx port
	 * ASSERT((uint)PKTHEADROOM(osh, sdu) < D11_TXH_LEN_EX(wlc));
	 */
	eh = (struct ether_header*) PKTDATA(osh, sdu);

	if (!wlc->pub->up) {
		WL_INFORM(("wl%d: wlc_prep_sdu: wl is not up\n", wlc->pub->unit));
		WLCNTINCR(wlc->pub->_cnt->txnoassoc);
		toss_reason = TX_PKTDROP_RSN_WL_DOWN;
		goto toss;
	}

	/* toss if station is not associated to the correct bsscfg */
	if (bsscfg->BSS && !SCB_LEGACY_WDS(scb) && !SCB_ISMULTI(scb)) {
		/* Class 3 (BSS) frame */
		if (!SCB_ASSOCIATED(scb)) {
			WL_INFORM(("wl%d.%d: wlc_prep_sdu: invalid class 3 frame to "
				"non-associated station "MACF"\n", wlc->pub->unit,
				WLC_BSSCFG_IDX(bsscfg), ETHER_TO_MACF(scb->ea)));
			WLCNTINCR(wlc->pub->_cnt->txnoassoc);
			toss_reason = TX_PKTDROP_RSN_INV_CLASS3_NO_ASSOC;
			goto toss;
		}
	}

	/* Toss the frame if scb's band does not match our current band
	 * this is possible if the STA has roamed while the packet was on txq
	 */
	if ((scb->bandunit != wlc->band->bandunit) &&
		!BSS_TDLS_ENAB(wlc, SCB_BSSCFG(scb))) {
		/* different band */
		WL_INFORM(("wl%d.%d: frame destined to "MACF" sent on incorrect band %d\n",
		           wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), ETHER_TO_MACF(scb->ea),
		           scb->bandunit));
		WLCNTINCR(wlc->pub->_cnt->txnoassoc);
		toss_reason = TX_PKTDROP_RSN_INV_BAND;
		goto toss;
	}

	if (SCB_QOS(scb) || SCB_WME(scb)) {
		/* prio could be different now */
		prio = (uint8)PKTPRIO(sdu);
	}

	/* XXX:
	 * WES: Note, the runt check is wrong. Min length is
	 * ETHER_HDR_LEN (DA/SA/TypeLen) plus 3 bytes LLC
	 * If LLC is AA-AA-03, then 8 bytes req.
	 * SDU runt check should be moved up the stack to wlc_hdr_proc
	 */

	/* toss runt frames */
	pkt_length = pkttotlen(osh, sdu);
	if (pkt_length < ETHER_HDR_LEN + DOT11_LLC_SNAP_HDR_LEN) {
		WLCNTINCR(wlc->pub->_cnt->txrunt);
		WLCIFCNTINCR(scb, txrunt);
		WLCNTSCBINCR(scb->scb_stats.tx_failures);
#ifdef WL11K
		wlc_rrm_stat_qos_counter(wlc, scb, prio, OFFSETOF(rrm_stat_group_qos_t, txfail));
#endif
		toss_reason = TX_PKTDROP_RSN_RUNT_FRAME;
		goto toss;
	}

	broadcast = ETHER_ISBCAST(eh->ether_dhost);
	if (BSSCFG_AP(bsscfg) && !SCB_LEGACY_WDS(scb)) {
		/* Check if this is a packet indicating a STA roam and remove assoc state if so.
		 * Only check if we are getting the send on a non-wds link so that we do not
		 * process our own roam/assoc-indication packets
		 */
		if (ETHER_ISMULTI(eh->ether_dhost))
			if (wlc_roam_check(wlc->ap, bsscfg, eh, pkt_length)) {
				toss_reason = TX_PKTDROP_RSN_MCAST_PKT_ROAM;
				goto toss;
			}
	}

	/* toss if station not yet authorized to receive non-802.1X frames */
	if (bsscfg->eap_restrict && !SCB_ISMULTI(scb) && !SCB_AUTHORIZED(scb)) {
		if (!is_8021x) {
			WL_INFORM(("wl%d.%d: non-802.1X frame to unauthorized station "MACF
				" (%s)\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
				ETHER_TO_MACF(scb->ea),
				SCB_LEGACY_WDS(scb) ? "WDS" : "STA"));
			toss_reason = TX_PKTDROP_RSN_STA_NOTAUTH;
			goto toss;
		}
	}

#ifdef BCMWAPI_WAI
	/* toss if station not yet authorized to receive non-WAI frames */
	if (WAPI_WAI_RESTRICT(wlc, bsscfg) && !SCB_ISMULTI(scb) && !SCB_AUTHORIZED(scb)) {
		if (!is_wai) {
			WL_ERROR(("wl%d.%d: non-WAI frame len %d to unauthorized station "MACF"\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), pkt_length,
				ETHER_TO_MACF(scb->ea)));
			toss_reason = TX_PKTDROP_RSN_NWAI_FRAME_NOTAUTH;
			goto toss;
		}
	}
#endif /* BCMWAPI_WAI */

	WL_PRUSR("tx", (uchar*)eh, ETHER_HDR_LEN);

	skip_enc = 0;
	if (WSEC_ENABLED(bsscfg->wsec) && !skip_enc) {
		if (is_8021x) {
			/*
			* use per scb WPA_auth to handle 802.1x frames differently
			* in WPA/802.1x mixed mode when having a pairwise key:
			*  - 802.1x frames are unencrypted in plain 802.1x mode
			*  - 802.1x frames are encrypted in WPA mode
			*/
			uint32 WPA_auth = SCB_LEGACY_WDS(scb) ? bsscfg->WPA_auth : scb->WPA_auth;

			key = wlc_keymgmt_get_tx_key(wlc->keymgmt, scb, bsscfg, &key_info);
			if ((WPA_auth != WPA_AUTH_DISABLED) &&
			    (key_info.algo != CRYPTO_ALGO_OFF)) {
				WL_WSEC(("wl%d.%d: wlc_prep_sdu: encrypting 802.1X frame using "
					"per-path key\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			} else {
				/* Do not encrypt 802.1X packets with group keys */
				WL_WSEC(("wl%d.%d: wlc_prep_sdu: not encrypting 802.1x frame\n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			}
		}
#ifdef BCMWAPI_WAI
		else if (is_wai) {
			/* Do not encrypt WAI packets with group keys */
			WL_WSEC(("wl%d.%d: wlc_prep_sdu: not encrypting WAI frame\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		}
#endif /* BCMWAPI_WAI */
		else {
			/* Use a paired key or primary group key if present, toss otherwise */
			key = wlc_keymgmt_get_tx_key(wlc->keymgmt, scb, bsscfg, &key_info);
			if (key_info.algo != CRYPTO_ALGO_OFF) {
				WL_WSEC(("wl%d.%d: wlc_prep_sdu: using pairwise key\n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			} else if (((bsscfg->WPA_auth == WPA_AUTH_DISABLED) ||
				(bsscfg->WPA_auth == WPA_AUTH_NONE) ||
				ETHER_ISMULTI(eh->ether_dhost)) &&
				(key = wlc_keymgmt_get_bss_tx_key(wlc->keymgmt,
				bsscfg, FALSE, &key_info)) != NULL &&
				key_info.algo != CRYPTO_ALGO_OFF) {
				WL_WSEC(("wl%d.%d: wlc_prep_sdu: using default tx key\n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
#ifdef AP
			} else if (SCB_ISMULTI(scb) && BSSCFG_AP(bsscfg) &&
				wlc_bss_assocscb_getcnt(wlc, bsscfg) == 0) {
				toss_reason = TX_PKTDROP_RSN_MULTICAST_NO_SCB;
				goto toss_silently;
#endif
			} else {
				WL_WSEC(("wl%d.%d: wlc_prep_sdu: tossing unencryptable frame\n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
				toss_reason = TX_PKTDROP_RSN_UNENCRYPT_FRAME2;
				goto toss;
			}
#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
			/* XXX, saw a case after wl reinit that the key is not WLC_KEY_IN_HW
			 * and there could be stale packets in txq which are still in split format
			 * there is no way to do SW encription here. TOSS the packets.
			 */
			if (WLC_KEY_SW_ONLY(&key_info) && PKTISTXFRAG(osh, sdu) &&
				(PKTFRAGTOTLEN(osh, sdu) > 0) && !PKTHASHEAPBUF(osh, sdu)) {
				WL_WSEC(("wl%d.%d: wlc_prep_sdu: Tossing stale split packet\n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
				toss_reason = TX_PKTDROP_RSN_UNENCRYPT_FRAME2;
				goto toss;
			}
#endif /* BCM_DHDHDR && DONGLEBUILD */
		}
	} else {	/* Do not encrypt packet */
		WL_WSEC(("wl%d.%d: wlc_prep_sdu: not encrypting frame, encryption disabled\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		WL_WSEC(("wl%d.%d: wlc_prep_sdu: wsec 0x%x \n", wlc->pub->unit,
			WLC_BSSCFG_IDX(bsscfg), bsscfg->wsec));
	}

	/*
	* If we get here without setting a key, it means it passed the tests above,
	* and we intended this packet to be sent.
	* Ensure we have a valid (potentially null, with ALGO_OFF) key and key_info
	* for all those cases
	*/
	if (key == NULL) {
		key = wlc_keymgmt_get_bss_key(wlc->keymgmt, bsscfg, WLC_KEY_ID_INVALID, &key_info);
	}

#if !defined(WLWSEC) || defined(WLWSEC_DISABLED)
	if (key == NULL) {
		memset(&key_info, 0, sizeof(wlc_key_info_t));
	}
#else
	ASSERT(key != NULL);
#endif
#ifdef STA
	if (is_8021x && wlc_keymgmt_b4m4_enabled(wlc->keymgmt, bsscfg)) {
		if (wlc_is_4way_msg(wlc, sdu, ETHER_HDR_LEN, PMSG4)) {
			WL_WSEC(("wl%d:%s(): Tx 4-way M4 pkt...\n", wlc->pub->unit, __FUNCTION__));
			is_4way_m4 = TRUE;
		}
	}
#endif /* STA */

#ifdef AP
	if (is_8021x && BSSCFG_AP(bsscfg)) {
		bool is_m3;
		if (wlc_is_eapfail_msg(wlc, sdu, &is_m3)) {
			/* It may so happen that if peer is in VSDB mode , EAP fail and
			 * subsequent de-auth pkt could be put to SCB PS queue.. Upon
			 * peer exiting PS mode, they may go in wrong order, resulting in
			 * group formation failure
			 * With de-auth as pkt call cb to EAP fail pkt .. order is enforced
			 */
			wlc_sched_deauth_on_txcmplt(wlc, scb, sdu);
		}
		WL_ASSOC_LT(("wl%d:%s prep:802.1x is_m3 %d\n",
			wlc->pub->unit, __FUNCTION__, is_m3));
		if (is_m3) {
			scb->dropblock_dur = wlc->pub->newkey_pktdrop_timeout; /* Max key timeout */
			/* Need to drain all packets for this SCB before sending out M3 message */
			if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb) > 0) {
				return BCME_BUSY;
			}
		}
	}
#endif /* AP */

	if (key_info.algo != CRYPTO_ALGO_OFF) {
		/* TKIP MIC space reservation */
		if (key_info.algo == CRYPTO_ALGO_TKIP) {
			is_tkip = TRUE;
			pkt_length += TKIP_MIC_SIZE;
		}
	}

	/* calculate how many frags in device memory needed
	 *  ETHER_HDR - (CKIP_LLC_MIC) - LLC/SNAP-ETHER_TYPE - payload - TKIP_MIC
	 */
	if (!WLPKTFLAG_AMSDU(pkttag) && !WLPKTFLAG_AMPDU(pkttag) && !is_8021x) {
		nfrags = wlc_frag(wlc, scb, WME_PRIO2AC(prio), pkt_length, &frag_length);
	} else {
		frag_length = pkt_length - hdrLength;
		nfrags = 1;
	}

	fast_path = (nfrags == 1);

	/* prepare sdu (non ckip, which already added the header mic) */
	if (key_prep_sdu != TRUE) {
		(void)wlc_key_prep_tx_msdu(key, sdu, ((nfrags > 1) ? frag_length : 0), prio);
	}

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
	/* TKIP + Fragmentation requires the whole packet to be fetched
	 * so that we can do SW MIC computation before the fragmentation
	 * if fragthresh is changed on the fly, there could be stale packets
	 * in low txq which are still in split format
	 * there is no way to do MIC computation or fragmentation now.
	 * TOSS the packets for now instead of sending corrupted packet on air.
	 * XXX TODO: Flush the packets in the system as part of "wl fragthresh iovar"
	 */
	if ((is_tkip) && ((!(WLC_KEY_MIC_IN_HW(&key_info))) || (nfrags > 1))) {
		if (PKTISTXFRAG(osh, sdu) && (PKTFRAGTOTLEN(osh, sdu) > 0) &&
			!PKTHASHEAPBUF(osh, sdu)) {
			WL_WSEC(("wlc_prep_sdu: Stale buffers prepped with old fragthresh \n"));
			toss_reason = TX_PKTDROP_RSN_UNENCRYPT_FRAME2;
			goto toss;
		}
	}
#endif /* BCM_DHDHDR && DONGLEBUILD */

	if (key_info.algo != CRYPTO_ALGO_OFF) {
		if (WLC_KEY_SW_ONLY(&key_info) || WLC_KEY_IS_LINUX_CRYPTO(&key_info)) {
			headroom += key_info.iv_len;
			want_tailroom = key_info.icv_len;
			fast_path = FALSE;		/* no fast path for SW encryption */
		}
	}

	if (fast_path && is_tkip) {
		/* In TKIP, MIC field is part of last fragment.
		 *   use slow_path for pkt with >=3 buffers(header + data)
		 *   since wlc_dofrag_tkip only works for pkt with <=2 buffers
		 * WPA FIXME: algo checking is done inconsistently
		 */
		if (!WLC_KEY_MIC_IN_HW(&key_info)) {
			void *p;
			p = PKTNEXT(osh, sdu);
			WL_WSEC(("wl%d.%d: wlc_prep_sdu: checking "
				"space for TKIP tailroom\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			if (p != NULL) {
				if (PKTNEXT(osh, p))
					fast_path = FALSE;
				else if ((uint)PKTTAILROOM(osh, p) <
					(want_tailroom + TKIP_MIC_SIZE))
					fast_path = FALSE;
			} else if ((uint)PKTTAILROOM(osh, sdu) <
				(want_tailroom + TKIP_MIC_SIZE))
				fast_path = FALSE;

			/* Cloned TKIP packets to go via the slow path so that
			 * each interface gets a private copy of data to add MIC
			 */
			if (PKTSHARED(sdu) || (p && PKTSHARED(p)))
				fast_path = FALSE;
		}
	}

	if (fast_path) {
		/* fast path for non fragmentation/non copy of pkts
		 * header buffer has been allocated in wlc_sendpkt()
		 * don't change ether_header at the front
		 */
		/* tx header cache hit */
		if (WLC_TXC_ENAB(wlc)) {
			wlc_txc_info_t *txc = wlc->txc;

			if (TXC_CACHE_ENAB(txc) &&
			    /* wlc_prep_sdu_fast may set WLF_TXCMISS... */
			    (pkttag->flags & WLF_TXCMISS) == 0 &&
			    (pkttag->flags & WLF_BYPASS_TXC) == 0) {
				if (wlc_txc_hit(txc, scb, sdu,
					((is_tkip && WLC_KEY_MIC_IN_HW(&key_info)) ?
					(pkt_length - TKIP_MIC_SIZE) : pkt_length),
					fifo, prio)) {
					int err;

					WLCNTINCR(wlc->pub->_cnt->txchit);
					err = wlc_txfast(wlc, scb, sdu, pkt_length, key);
					if (err != BCME_OK)
						return err;

					*npkts = 1;
					goto done;
				}
				pkttag->flags |= WLF_TXCMISS;
				WLCNTINCR(wlc->pub->_cnt->txcmiss);
			}
		}

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
		/* It's time to swap to use D11_BUFFER to construct mpdu */
		if (PKTISTXFRAG(osh, sdu) &&
			!PKTHASHEAPBUF(wlc->osh, sdu) &&
			(PKTSWAPD11BUF(osh, sdu) != BCME_OK))
			return BCME_BUSY;

#if defined(HWA_PKTPGR_BUILD)
		HWA_PKT_DUMP_EXPR({
			if (PKTISTXFRAG(osh, sdu) && !PKTHASHEAPBUF(wlc->osh, sdu)) {
				hwa_txpost_dump_pkt(sdu, NULL, "wlc_prep_sdu", 0, TRUE);
			}
		});
#endif
#endif /* BCM_DHDHDR && DONGLEBUILD */
		pkts[0] = sdu;
	} else {
		/*
		 * Prealloc all fragment buffers for this frame.
		 * If any of the allocs fail, free them all and bail.
		 * p->data points to ETHER_HEADER, original ether_type is passed separately
		 */
		offset = ETHER_HDR_LEN;

		for (i = 0; i < nfrags; i++) {
			/*
			 * wlc_frag chose frag_length such that tkip mic is part of last fragment.
			 * Reserve tailroom for TKIP MIC for the last fragment only.
			 */
			if (i + 1 == nfrags) {
				WL_WSEC(("wl%d.%d: wlc_prep_sdu: checking "
					"space for TKIP tailroom\n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
				want_tailroom += TKIP_MIC_SIZE;
			}

			/* before fragmentation make sure the frame contents are valid */
			PKTCTFMAP(osh, sdu);

			/* fragmentation:
			 * realloc new buffer for each frag, copy over data,
			 * append original ether_header at the front
			 */
#if defined(BCMPCIEDEV) && (defined(BCMFRAGPOOL) || (defined(BCMHWA) && \
	defined(HWA_TXPOST_BUILD)))
			if (BCMPCIEDEV_ENAB() && BCMLFRAG_ENAB() && PKTISTXFRAG(osh, sdu) &&
			    !PKTHASHEAPBUF(osh, sdu) && (PKTFRAGTOTLEN(osh, sdu) > 0)) {
				pkts[i] = wlc_allocfrag_txfrag(osh, sdu, offset, frag_length,
					(i == (nfrags-1)));
				wl_inform_additional_buffers(wlc->wl, nfrags);
#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
			} else if (PKTISTXFRAG(osh, sdu) &&
				PKTISBUFALLOC(osh, sdu) &&
				(nfrags == 1)) {
				pkts[0] = sdu;
				goto resume;
#endif /* BCM_DHDHDR && DONGLEBUILD */
			} else
#endif /* BCMPCIEDEV && (BCMFRAGPOOL || HWA_TXPOST_BUILD) */
				pkts[i] = wlc_allocfrag(osh, sdu, offset, headroom, frag_length,
					want_tailroom); /* alloc and copy */

			if (pkts[i] == NULL) {
				for (j = 0; j < i; j++) {
					PKTFREE(osh, pkts[j], TRUE);
				}
				pkts[0] = sdu;    /* restore pointer to sdu */
				WL_ERROR(("wl%d.%d: %s: allocfrag failed\n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg),
					__FUNCTION__));
				WLCNTINCR(wlc->pub->_cnt->txnobuf);
				WLCIFCNTINCR(scb, txnobuf);
				WLCNTSCBINCR(scb->scb_stats.tx_failures);
#ifdef WL11K
				wlc_rrm_stat_qos_counter(wlc, scb, prio,
					OFFSETOF(rrm_stat_group_qos_t, txfail));
#endif
#if defined(BCMPCIEDEV) && defined(BCMLFRAG) && defined(PROP_TXSTATUS)
				if (BCMPCIEDEV_ENAB() && BCMLFRAG_ENAB() &&
					(PROP_TXSTATUS_ENAB(wlc->pub)))
					if (!wlc_suppress_recent_for_fragmentation(wlc,
						sdu, nfrags)) {
						/* If we could not find anything else to suppress,
						 * this must be only one lets suppress this instead
						 * of dropping it.
						 */
						wlc_process_wlhdr_txstatus(wlc,
							WLFC_CTL_PKTFLAG_WLSUPPRESS, sdu, FALSE);
						/* free up pkt */
						toss_reason = TX_PKTDROP_RSN_SUPR_PKT;
						goto toss;
					}
#endif /* BCMPCIEDEV && BCMLFRAG && PROP_TXSTATUS */

				return BCME_BUSY;
			} else {
#ifdef BCMWAPI_WPI
				if ((key_info.algo == CRYPTO_ALGO_SMS4) &&
				    WLC_KEY_SW_ONLY(&key_info)) {
					int cur_len = PKTLEN(osh, pkts[i]);
					PKTSETLEN(osh, pkts[i],
						cur_len + SMS4_WPI_CBC_MAC_LEN);
				}
#endif /* BCMWAPI_WPI */

				/* Copy the ETH in sdu to each pkts[i] fragment */
				PKTPUSH(osh, pkts[i], hdrLength);
				bcopy((char*)eh, (char*)PKTDATA(osh, pkts[i]), hdrLength);

				/* Slow path, when BCM_DHDHDR is enabled,
				 * each pkts[] has ether header 14B in dongle and host data addr
				 * points to llc snap 8B in DHDHDR for first pkts[].
				 * The others host data addr point to data partion in different
				 * offest.
				 */

				/* Transfer SDU's pkttag info to the last fragment */
				if (i == (nfrags - 1)) {
					wlc_pkttag_info_move(wlc, sdu, pkts[i]);

#ifdef BCMPCIEDEV
					/* Reset original sdu's metadata, so that PKTFREE of
					 * original sdu does not send tx status prematurely
					 */
					if (BCMPCIEDEV_ENAB() && BCMLFRAG_ENAB() &&
					    PKTISTXFRAG(osh, sdu) && (PKTFRAGTOTLEN(osh, sdu) > 0))
						PKTRESETHASMETADATA(osh, sdu);
#endif
				} else {
					wlc_pkttag_info_frag_cpy(wlc, sdu, pkts[i]);
#ifdef BCMPCIEDEV
					/* non-last frags should not have metadata */
					if (BCMPCIEDEV_ENAB() && BCMLFRAG_ENAB() &&
					    PKTISTXFRAG(osh, sdu) && (PKTFRAGTOTLEN(osh, sdu) > 0))
						PKTRESETHASMETADATA(osh, pkts[i]);
#endif
				}
			}

			/* set fragment priority */
			PKTSETPRIO(pkts[i], PKTPRIO(sdu));

			/* leading frag buf must be aligned 0-mod-2 */
			ASSERT(ISALIGNED(PKTDATA(osh, pkts[i]), 2));
			offset += frag_length;
		}
	}

#if defined(BCMPCIEDEV) && defined(BCM_DHDHDR) && defined(DONGLEBUILD) && \
	(defined(BCMFRAGPOOL) || (defined(BCMHWA) && defined(HWA_TXPOST_BUILD)))
resume:
#endif /* BCMPCIEDEV && BCMFRAGPOOL && BCM_DHDHDR && DONGLEBUILD */

	/* build and transmit each fragment */
	for (i = 0; i < nfrags; i++) {
		if (i < nfrags - 1) {
			next_payload_len = pkttotlen(osh, pkts[i + 1]);
			ASSERT(next_payload_len >= hdrLength);
			next_payload_len -= hdrLength;
		} else {
			next_payload_len = 0;
		}
		WLPKTTAGBSSCFGSET(pkts[i], WLC_BSSCFG_IDX(scb->bsscfg));
		WLPKTTAGSCBSET(pkts[i], scb);

		/* ether header is on front of each frag to provide the dhost and shost
		 *  ether_type may not be original, should not be used
		 */
		wlc_dofrag(wlc, pkts[i], i, nfrags, next_payload_len, scb,
			is_8021x, fifo, key, &key_info, prio, frag_length, 0);

	}

	/* Free the original SDU if not shared */
	if (pkts[nfrags - 1] != sdu) {
		PKTFREE(osh, sdu, TRUE);
		sdu = pkts[nfrags - 1];
		BCM_REFERENCE(sdu);
	}

	*npkts = nfrags;

done:
#ifdef STA
	if (is_4way_m4)
		wlc_keymgmt_notify(wlc->keymgmt, WLC_KEYMGMT_NOTIF_M4_TX, bsscfg, scb, key, sdu);
#endif /* STA */

	WLCNTSCBINCR(scb->scb_stats.tx_pkts);
#ifdef WL11K
	wlc_rrm_stat_bw_counter(wlc, scb, TRUE);
	wlc_rrm_stat_qos_counter(wlc, scb, prio, OFFSETOF(rrm_stat_group_qos_t, txframe));
#endif
	WLCNTSCBADD(scb->scb_stats.tx_ucast_bytes, pkt_length);
	/*
	 * 1) In this function we do not process 802.11 control or
	 * management frames so we can safely increase the counters
	 * for 802.3 packets.
	 * 2) Despite the fact that tx_ucast is updated, this can still be a
	 * multicast or broadcast packet as tx_ucast != txucast.
	 * So add the argument whether this packet was a broadcast packet or
	 * not, as the flag in the wlc will be true for BOTH multi and broadcast
	 * packets
	 */
	wlc_update_txpktsuccess_stats(wlc, scb, broadcast, pkt_length, prio, 1);
	return 0;
toss:
#ifndef WL_PKTDROP_STATS
	if (WL_INFORM_ON()) {
		wlc_log_unexpected_tx_frame_log_8023hdr(wlc, eh, toss_reason);
	}
#endif

#ifdef AP
toss_silently:
#endif
	*npkts = 0;
	wlc_update_txpktfail_stats(wlc, pkttotlen(osh, sdu), prio);
#ifdef WLTAF
	wlc_taf_txpkt_status(wlc->taf_handle, scb, prio, sdu, TAF_TXPKT_STATUS_PKTFREE_DROP);
#endif
	TX_PKTDROP_COUNT(wlc, scb, toss_reason);
	PKTFREE(osh, sdu, TRUE);
	return BCME_ERROR;
} /* wlc_prep_sdu */

/**
 * Obtain preassigned sequence number from the pkttag in case
 * PCIEDEV, NAR or AMPDU has already assigned one.
 */
static bool BCMFASTPATH
wlc_tx_preassigned_seqnum(wlc_info_t *wlc, wlc_pkttag_t *pkttag, uint16* seq)
{
	if (WLPKTFLAG_AMPDU(pkttag) ||
#ifdef PROP_TXSTATUS
		GET_DRV_HAS_ASSIGNED_SEQ(pkttag->seq) ||
#endif /* PROP_TXSTATUS */
		GET_WL_HAS_ASSIGNED_SEQ(pkttag->seq) ||
		FALSE) {
		*seq = WL_SEQ_GET_NUM(pkttag->seq);
		return TRUE;
	}
	return FALSE;
}

/**
 * Despite its name, wlc_txfast() does not transmit anything.
 * Driver fast path to send sdu using cached tx d11-phy-mac header (before
 * llc/snap).
 * Doesn't support wsec and eap_restrict for now.
 */
static int BCMFASTPATH
wlc_txfast(wlc_info_t *wlc, struct scb *scb, void *sdu, uint pktlen, wlc_key_t *key)
{
	osl_t *osh = wlc->osh;
	struct ether_header *eh;
	d11txhdr_t *txh;
	uint16 seq = 0, frameid;
	struct dot11_header *h;
	struct ether_addr da, sa;
	uint fifo = 0;
	wlc_pkttag_t *pkttag;
	wlc_bsscfg_t *bsscfg;
	uint flags = 0;
	uint16 txc_hwseq, TxFrameID_off;
	uint d11TxdLen;
	bool is_amsdu;
	wlc_tx_rtc_t rtc;

	ASSERT(scb != NULL);

	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);

	ASSERT(ISALIGNED(PKTDATA(osh, sdu), 2));

	pkttag = WLPKTTAG(sdu);
	is_amsdu = (pkttag->flags & WLF_AMSDU);

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
	/* It's time to swap to use D11_BUFFER to construct mpdu */
	if (PKTISTXFRAG(wlc->osh, sdu) &&
		!PKTHASHEAPBUF(wlc->osh, sdu) &&
		(PKTSWAPD11BUF(wlc->osh, sdu) != BCME_OK))
		return BCME_BUSY;

#if defined(HWA_PKTPGR_BUILD)
	HWA_PKT_DUMP_EXPR({
		if (PKTISTXFRAG(wlc->osh, sdu) && !PKTHASHEAPBUF(wlc->osh, sdu)) {
			hwa_txpost_dump_pkt(sdu, NULL, "wlc_txfast", 0, TRUE);
		}
	});
#endif

	/* Subframe header allready present in host */
	if (is_amsdu)
		PKTPULL(osh, sdu, ETHER_HDR_LEN);
#endif /* BCM_DHDHDR && DONGLEBUILD */

	/* headroom has been allocated, may have llc/snap header already */
	/* uncomment after fixing osl_vx port
	 * ASSERT(PKTHEADROOM(osh, sdu) >= (TXOFF - DOT11_LLC_SNAP_HDR_LEN - ETHER_HDR_LEN));
	 */
	if (is_amsdu == 0) {
		/* AMSDU frames need to retain subframe header inserted */
		/* eh is going to be overwritten, save DA and SA for later fixup */
		eh = (struct ether_header*) PKTDATA(osh, sdu);
		eacopy((char*)eh->ether_dhost, &da);
		eacopy((char*)eh->ether_shost, &sa);

		/* strip off ether header, copy cached tx header onto the front of the frame */
		PKTPULL(osh, sdu, ETHER_HDR_LEN);
	} else {
		eacopy(&scb->ea, &da);
		eacopy(&bsscfg->cur_etheraddr, &sa);
	}

	ASSERT(WLC_TXC_ENAB(wlc));

	/* Fillup RTC elements used inside wlc_txc_cp */
	rtc.wlc = wlc;
	rtc.scb = scb;
	wlc_he_htc_rtc_init(&rtc);
	rtc.sti = wlc_scb_txc_info(wlc, scb);

	txh = wlc_txc_cp(&rtc, sdu, &flags);

	d11TxdLen = D11_TXH_LEN_EX(wlc);

	/* txc_hit doesn't check DA,SA, fixup is needed for different configs */
	h = (struct dot11_header*) ((uint8 *)txh + d11TxdLen);

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		TxFrameID_off = OFFSETOF(d11txh_rev128_t, FrameID);
		txc_hwseq = (ltoh16(txh->rev128.MacControl_0)) &
			D11AC_TXC_ASEQ;
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		TxFrameID_off = OFFSETOF(d11actxh_t, PktInfo.TxFrameID);
		txc_hwseq = (ltoh16(txh->rev40.PktInfo.MacTxControlLow)) &
			D11AC_TXC_ASEQ;
	} else {
		TxFrameID_off = OFFSETOF(d11txh_pre40_t, TxFrameID);
		txc_hwseq = (ltoh16(txh->pre40.MacTxControlLow)) & TXC_HWSEQ;
	}

	if (BSSCFG_IBSS(bsscfg)) {
		/* IBSS, no need to fixup BSSID */
	} else if (SCB_A4_DATA(scb)) {
		/* wds: fixup a3 with DA, a4 with SA */
		ASSERT((ltoh16(h->fc) & (FC_TODS | FC_FROMDS)) == (FC_TODS | FC_FROMDS));
		eacopy((char*)&da, (char*)&h->a3);
		eacopy((char*)&sa, (char*)&h->a4);
	} else if (BSSCFG_STA(bsscfg)) {
		/* normal STA to AP, fixup a3 with DA */
		eacopy((char*)&da, (char*)&h->a3);
	} else {
		/* normal AP to normal STA, fixup a3 with SA */
		eacopy((char*)&sa, (char*)&h->a3);
	}

#ifdef WLAMSDU_TX
	/* fixup qos control field to indicate it is an AMSDU frame */
	if (AMSDU_TX_ENAB(wlc->pub) &&	AMSDU_TX_AC_ENAB(wlc->ami, PKTPRIO(sdu)) &&
			SCB_QOS(scb)) {
		uint16 *qos;
		qos = (uint16 *)((uint8 *)h + ((SCB_WDS(scb) || SCB_DWDS(scb)) ? DOT11_A4_HDR_LEN :
		                                              DOT11_A3_HDR_LEN));
		/* set or clear the A-MSDU bit */
		if (WLPKTFLAG_AMSDU(pkttag))
			*qos |= htol16(QOS_AMSDU_MASK);
		else
			*qos &= ~htol16(QOS_AMSDU_MASK);
	}
#endif

	/* fixup counter in TxFrameID */
	frameid = *(uint16 *)((uint8 *)txh + TxFrameID_off);
	fifo = D11_TXFID_GET_FIFO(wlc, ltoh16(frameid));
	frameid = wlc_compute_frameid(wlc, frameid, fifo);
	*(uint16 *)((uint8 *)txh + TxFrameID_off) = htol16(frameid);

	/* fix up the seqnum in the hdr */
	if (!wlc_tx_preassigned_seqnum(wlc, pkttag, &seq) && !txc_hwseq) {
		seq = SCB_SEQNUM(scb, PKTPRIO(sdu));
		SCB_SEQNUM(scb, PKTPRIO(sdu))++;
	}

	h->seq = htol16(seq << SEQNUM_SHIFT);

#ifdef PROP_TXSTATUS
	if (PROP_TXSTATUS_ENAB(wlc->pub) && WLFC_GET_REUSESEQ(wlfc_query_mode(wlc->wlfc))) {
		WL_SEQ_SET_NUM(pkttag->seq, seq);
		SET_WL_HAS_ASSIGNED_SEQ(pkttag->seq);
	}
#endif /* PROP_TXSTATUS */

	/* TDLS U-APSD Buffer STA: save Seq and TID for PIT */
#ifdef WLTDLS
	if (BSS_TDLS_ENAB(wlc, SCB_BSSCFG(scb)) && wlc_tdls_buffer_sta_enable(wlc->tdls)) {
		uint16 fc;
		uint16 type;
		uint8 tid = 0;
		bool a4;

		fc = ltoh16(h->fc);
		type = FC_TYPE(fc);
		a4 = ((fc & (FC_TODS | FC_FROMDS)) == (FC_TODS | FC_FROMDS));
		if ((type == FC_TYPE_DATA) && FC_SUBTYPE_ANY_QOS(FC_SUBTYPE(fc))) {
			uint16 qc;
			int offset = a4 ? DOT11_A4_HDR_LEN : DOT11_A3_HDR_LEN;

			qc = (*(uint16 *)((uchar *)h + offset));
			tid = (QOS_TID(qc) & 0x0f);
		}
		wlc_tdls_update_tid_seq(wlc->tdls, scb, tid, seq << SEQNUM_SHIFT);
	}
#endif /* WLTDLS */

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		uint tsoHdrSize = 0;

		txh->rev128.SeqCtl = h->seq;

		/* Get the packet length after adding d11 header. */
		pktlen = pkttotlen(osh, sdu);
		/* Update frame len (fc to fcs) */
#ifdef WLTOEHW
		tsoHdrSize = (wlc->toe_bypass ?
			0 : wlc_tso_hdr_length((d11ac_tso_t*)PKTDATA(wlc->osh, sdu)));
#endif
		txh->rev128.FrmLen =
			htol16((uint16)(pktlen +  DOT11_FCS_LEN - d11TxdLen - tsoHdrSize));
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		uint tsoHdrSize = 0;

		txh->rev40.PktInfo.Seq = h->seq;

		/* Get the packet length after adding d11 header. */
		pktlen = pkttotlen(osh, sdu);
		/* Update frame len (fc to fcs) */
#ifdef WLTOEHW
		tsoHdrSize = (wlc->toe_bypass ?
			0 : wlc_tso_hdr_length((d11ac_tso_t*)PKTDATA(wlc->osh, sdu)));
#endif
		txh->rev40.PktInfo.FrameLen =
			htol16((uint16)(pktlen +  DOT11_FCS_LEN - d11TxdLen - tsoHdrSize));
	}

	if (CAC_ENAB(wlc->pub) && fifo <= TX_AC_VO_FIFO) {
		/* update cac used time with cached value */
		if (wlc_cac_update_used_time(wlc->cac, wme_fifo2ac[fifo], -1, scb))
			WL_ERROR(("wl%d: ac %d: txop exceeded allocated TS time\n",
			          wlc->pub->unit, wme_fifo2ac[fifo]));
	}

	/* update packet tag with the saved flags */
	pkttag->flags |= flags;

	wlc_txc_prep_sdu(wlc->txc, scb, key, sdu);

	if ((BSSCFG_AP(bsscfg) || BSS_TDLS_BUFFER_STA(bsscfg)) &&
		SCB_PS(scb) && (fifo != TX_BCMC_FIFO)) {
		if (AC_BITMAP_TST(scb->apsd.ac_delv, prio2ac[PKTPRIO(sdu)]))
			wlc_apps_apsd_prepare(wlc, scb, sdu, h, TRUE);
		else
			wlc_apps_pspoll_resp_prepare(wlc, scb, sdu, h, TRUE);
	}

	/* In case of RATELINKMEM, don't update rspec history, instead do so on TXS */
	if (!RATELINKMEM_ENAB(wlc->pub)) {
		/* (bsscfg specific): Add one more entry of the current rate to keep an accurate
		 * histogram. If the rate changed, we wouldn't be in the fastpath
		*/
		uint previdx = (bsscfg->txrspecidx - 1) % NTXRATE;
		 /* rspec */
		bsscfg->txrspec[bsscfg->txrspecidx][0] = bsscfg->txrspec[previdx][0];
		 /* nfrags */
		bsscfg->txrspec[bsscfg->txrspecidx][1] = bsscfg->txrspec[previdx][1];
		bsscfg->txrspecidx = (bsscfg->txrspecidx + 1) % NTXRATE;
	}

#if defined(PROP_TXSTATUS)
#ifdef WLCNTSCB
	pkttag->rspec = scb->scb_stats.tx_rate;
#endif
#endif /* PROP_TXSTATUS */

	return BCME_OK;
} /* wlc_txfast */

/**
 * Common transmit packet routine, called when an sdu in a queue is to be
 * forwarded to he d11 hardware.
 * Called by e.g. wlc_send_q(),_wlc_sendampdu_aqm() and wlc_ampdu_output_aqm()
 *
 * Parameters:
 *     scb   : remote party to send the packets to
 *     **key : when key is not NULL, we already obtained the key & keyinfo
 *             we can bypass calling the expensive routines: wlc_txc_hit(),
 *             wlc_keymgmt_get_tx_key(), and wlc_keymgmt_get_bss_tx_key().
 *             The key shall be cleared in wlc_ampdu_output_aqm() when TID is
 *             changed. So this routine can reevaluate the wlc_tx_hit() call.
 *
 * Return 0 when a packet is accepted and should not be referenced again
 * by the caller -- wlc will free it.
 * When an error code is returned, wlc_prep_sdu ahall be called.
 */
int BCMFASTPATH
wlc_prep_sdu_fast(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, struct scb *scb,
	void *sdu, uint fifo, wlc_key_t **key)
{
	uint pktlen;
	uint8 prio;
	wlc_txc_info_t *txc;
	wlc_key_info_t key_info;

	ASSERT(SCB_AMPDU(scb));
	ASSERT(!SCB_ISMULTI(scb));

	if (!WLC_TXC_ENAB(wlc)) {
		return BCME_UNSUPPORTED;
	}

	prio = SCB_QOS(scb) ? (uint8)PKTPRIO(sdu) : 0;
	ASSERT(prio <= MAXPRIO);

	/* 1x or fragmented frames will force slow prep */
	pktlen = pkttotlen(wlc->osh, sdu);
	if ((WLPKTTAG(sdu)->flags &
	    (WLF_8021X
#ifdef BCMWAPI_WAI
	     | WLF_WAI
#endif
	     | 0)) || (((WLPKTTAG(sdu)->flags & WLF_AMSDU) == 0) &&
	      ((uint16)pktlen >= (wlc->fragthresh[WME_PRIO2AC(prio)] -
	      (uint16)(DOT11_A4_HDR_LEN + DOT11_QOS_LEN + DOT11_FCS_LEN + ETHER_ADDR_LEN))))) {
		return BCME_UNSUPPORTED;
	}

	/* TX header cache hit */
	txc = wlc->txc;

	if (TXC_CACHE_ENAB(txc) && (WLPKTTAG(sdu)->flags & WLF_BYPASS_TXC) == 0) {
		ASSERT((WLPKTTAG(sdu)->flags & WLF_TXCMISS) == 0);

		if (wlc_txc_hit(txc, scb, sdu, pktlen, fifo, prio)) {
			int err;

			if (*key == NULL) {
				*key = wlc_keymgmt_get_tx_key(wlc->keymgmt, scb, bsscfg,
					&key_info);
				if (key_info.algo == CRYPTO_ALGO_OFF) {
					/* Don't use bss keys with txc with wpa/rsn */
					if ((bsscfg->WPA_auth != WPA_AUTH_DISABLED) &&
					    (bsscfg->WPA_auth != WPA_AUTH_NONE)) {
					    return BCME_UNSUPPORTED;
					}
					*key = wlc_keymgmt_get_bss_tx_key(wlc->keymgmt,
						bsscfg, FALSE, &key_info);
				}

				if (key_info.algo != CRYPTO_ALGO_OFF &&
					WLC_KEY_SW_ONLY(&key_info)) {
					return BCME_UNSUPPORTED;
				}
			}

			WLCNTINCR(wlc->pub->_cnt->txchit);
			err = wlc_txfast(wlc, scb, sdu, pktlen, *key);
			if (err != BCME_OK)
				return err;
			goto done;
		}
		WLPKTTAG(sdu)->flags |= WLF_TXCMISS;
		WLCNTINCR(wlc->pub->_cnt->txcmiss);
	}

	return BCME_UNSUPPORTED;

done:
	WLCNTSCBINCR(scb->scb_stats.tx_pkts);
#ifdef WL11K
	wlc_rrm_stat_bw_counter(wlc, scb, TRUE);
	wlc_rrm_stat_qos_counter(wlc, scb, prio, OFFSETOF(rrm_stat_group_qos_t, txframe));
#endif
	WLCNTSCBADD(scb->scb_stats.tx_ucast_bytes, pktlen);
	wlc_update_txpktsuccess_stats(wlc, scb, 0, pktlen, prio, 1);
	return BCME_OK;
} /* wlc_prep_sdu_fast */

void BCMFASTPATH
wlc_txfifo_complete(wlc_info_t *wlc, scb_t *scb, uint fifo, uint16 txpktpend)
{
	txq_t *txq;				/**< consisting of a queue for every d11 fifo */

	/* Select the correct txq context for completions.
	 * txq will be the queue being detached if a detach is in progress.
	 */
	txq = (wlc->txfifo_detach_transition_queue != NULL) ?
		wlc->txfifo_detach_transition_queue->low_txq : wlc->active_queue->low_txq;

	WL_TRACE(("%s, pktpend dec %d current %d\n", __FUNCTION__, txpktpend,
		TXPKTPENDGET(wlc, fifo)));

	TXPKTPENDDEC(wlc, fifo, txpktpend);

	wlc_txq_complete(wlc->txqi, txq, fifo, txpktpend);

#ifndef WL_TXPKTPEND_SYNC
	ASSERT(TXPKTPENDGET(wlc, fifo) >= 0);
#endif /* WL_TXPKTPEND_SYNC */

	/* For per SCB data block handling this is the moment to check. */
	wlc_apps_trigger_on_complete(wlc, scb);

	/* Try to open blocked datafifo flag */
	wlc_tx_open_datafifo(wlc);

} /* wlc_txfifo_complete */

#ifndef DONGLEBUILD
int
wlc_txfifo_freed_pkt_cnt_noaqm(wlc_info_t *wlc, uint fifo_idx)
{
	void *pkt;
	uint16 pktcnt = 0;

	while ((pkt = dma_getnexttxp(WLC_HW_DI(wlc, fifo_idx), HNDDMA_RANGE_ALL))) {
		wlc_txq_freed_pkt_cnt(wlc, pkt, &pktcnt);
		PKTFREE(wlc->osh, pkt, TRUE);
	}

	return pktcnt;
}
#endif /* DONGLEBUILD */

void BCMFASTPATH
wlc_tx_open_datafifo(wlc_info_t *wlc)
{
	uint16 block_datafifo = wlc->block_datafifo;
	wlc_txq_info_t *main_qi;

#ifdef WL_TXPKTPEND_SYNC
	/* Don't open datafifo if pktpend sync is in progress */
	if (wlc->pktpend_sync & TXPKTPEND_SYNC_PEND) {
		return;
	}
#endif /* WL_TXPKTPEND_SYNC */

	/* Don't open datafifo if active_queue is not set to primary_queue */
	main_qi = wlc->primary_queue ? wlc->primary_queue : wlc->def_primary_queue;

	/* Don't do the piggy back call in critical session */
	if (!wlc->pub->up || wlc->hw->reinit || wlc->txfifo_detach_pending ||
		(wlc->active_queue != main_qi)) {
#ifdef BCMDBG_TXSTALL
		if (wlc->block_datafifo && wlc->block_time && wlc->block_timeout &&
			(OSL_SYSUPTIME() - wlc->block_time) > wlc->block_timeout) {
			WL_ERROR(("wl%d STALL: up=%d reinit=%d detach=%d datablock=0x%x "
				"pktpend=%d dt=%d\n",
				wlc->pub->unit, wlc->pub->up, wlc->hw->reinit,
				wlc->txfifo_detach_pending, wlc->block_datafifo, TXPKTPENDTOT(wlc),
				wlc->block_time ? OSL_SYSUPTIME() - wlc->block_time : 0));
		}
#endif /* BCMDBG_TXSTALL */
		return;
	}

	/* Do not open datafifo if any SCB is in the process of freeing MU FIFOs */
	if (wlc_fifo_free_in_progress_count(wlc->fifo) != 0) {
		return;
	}

	/* Use local copy of block_datafifo, since during processing the data_block, a new block
	 * can be set, and it shouldnt be cleared right away in that case.
	 */
	if (TXPKTPENDTOT(wlc) == 0) {
#ifdef WL_BSSCFG_TX_SUPR
		if (block_datafifo & DATA_BLOCK_TX_SUPR) {
			wlc_bsscfg_tx_check(wlc->psqi);
		}
#endif /* WL_BSSCFG_TX_SUPR */

		if (wlc->ps_scb_txfifo_blk) {
			wlc_apps_process_pend_ps(wlc);
		}

#ifdef WL_MU_TX
		if (block_datafifo & DATA_BLOCK_MUTX) {
			wlc_mutx_txfifo_complete(wlc);
		}
#endif /* WL_MU_TX */

		if (block_datafifo & DATA_BLOCK_TXCHAIN) {
#ifdef WL_STF_ARBITRATOR
			if (WLC_STF_ARB_ENAB(wlc->pub)) {
				wlc_stf_arb_handle_txfifo_complete(wlc);
			} else
#endif /* WL_STF_ARBITRATOR */
			if (wlc->stf->txchain_pending) {
				wlc_stf_txchain_set_complete(wlc);
			}
			/* remove the fifo block now */
			wlc_block_datafifo(wlc, DATA_BLOCK_TXCHAIN, 0);

#ifdef WL_MODESW
			if (wlc->stf->pending_opstreams) {
				wlc_stf_op_txrxstreams_complete(wlc);
				wlc_block_datafifo(wlc, DATA_BLOCK_TXCHAIN, 0);
				WL_MODE_SWITCH(("wl%d: %s: UNBLOCKED DATAFIFO \n", WLCWLUNIT(wlc),
					__FUNCTION__));
			}

			if (WLC_MODESW_ENAB(wlc->pub)) {
				wlc_modesw_bsscfg_complete_downgrade(wlc->modesw);
			}
#endif /* WL_MODESW */
		}
		if (block_datafifo & DATA_BLOCK_SPATIAL) {
			wlc_stf_spatialpolicy_set_complete(wlc);
			wlc_block_datafifo(wlc, DATA_BLOCK_SPATIAL, 0);
		}
	}

	if (D11REV_LT(wlc->pub->corerev, 128)) {
#ifdef BCM_DMA_CT
		if (BCM_DMA_CT_ENAB(wlc) && wlc->primary_bsscfg->pm->PM != PM_OFF) {
			if ((D11REV_IS(wlc->pub->corerev, 65)) &&
				(pktq_empty(WLC_GET_CQ(wlc->primary_bsscfg->wlcif->qi)) &&
				(wlc->hw->wake_override & WLC_WAKE_OVERRIDE_CLKCTL))) {
				wlc_ucode_wake_override_clear(wlc->hw, WLC_WAKE_OVERRIDE_CLKCTL);
			}
		}
#endif /* BCM_DMA_CT */

		/*
		 * PR 113378: Checking for the PM wake override bit before calling the override
		 * PR 107865: DRQ output should be latched before being written to DDQ.
		 * If awake, try to sleep.
		 * XXX: 1) need to check whether the pkts in other queue's are empty.
		 * XXX: 2) currently only for primary STA interface
		 */
		if (((D11REV_GE(wlc->pub->corerev, 128))) &&
			(pktq_empty(WLC_GET_CQ(wlc->primary_bsscfg->wlcif->qi)) &&
			(TXPKTPENDTOT(wlc) == 0) &&
			(wlc->user_wake_req & WLC_USER_WAKE_REQ_TX))) {
			mboolclr(wlc->user_wake_req, WLC_USER_WAKE_REQ_TX);
			wlc_set_wake_ctrl(wlc);
		}
	} /* skip revid >= 128 */

#ifdef BCMDBG_TXSTALL
	if (block_datafifo && wlc->block_datafifo && wlc->block_time == 0) {
		/* still blocked */
		wlc->block_time = OSL_SYSUPTIME();
	}
	if (wlc->block_datafifo == 0) {
		/* reset block time */
		wlc->block_time = 0;
	}
#endif /* BCMDBG_TXSTALL */

#ifdef STA
	wlc_assoc_check_drain_complete(wlc);
#endif /* STA */
} /* wlc_tx_open_datafifo */

/**
 * Called by wlc_prep_pdu(). Create the d11 hardware txhdr for an MPDU packet that does not (yet)
 * have a txhdr. The packet begins with a tx_params structure used to supply some parameters to
 * wlc_d11hdrs().
 */
void
wlc_pdu_txhdr(wlc_info_t *wlc, void *p, struct scb *scb, uint fifo)
{
	wlc_pdu_tx_params_t tx_params;
	wlc_key_info_t key_info;
	struct dot11_header *h;

	/* pull off the saved tx params */
	memcpy(&tx_params, PKTDATA(wlc->osh, p), sizeof(tx_params));
	PKTPULL(wlc->osh, p, sizeof(tx_params));

	h = (struct dot11_header*)PKTDATA(wlc->osh, p);
	if (ltoh16(h->fc) != FC_TYPE_DATA) {
		WLCNTINCR(wlc->pub->_cnt->txctl);
	}

	/* add headers */
	wlc_key_get_info(tx_params.key, &key_info);
	wlc_d11hdrs(wlc, p, scb, tx_params.flags, 0, 1, fifo, 0,
		tx_params.key, &key_info, tx_params.rspec_override);
}

/**
 * Prepares PDU for transmission. Called by e.g. wlc_send_q() and _wlc_sendampdu_aqm().
 * Returns a (BCM) error code when e.g. something is blocking the transmission of the PDU.
 * Adds a D11 header to the PDU if not yet present.
 */
int
wlc_prep_pdu(wlc_info_t *wlc, struct scb *scb, void *pdu, uint fifo)
{
	wlc_bsscfg_t *cfg;
	wlc_pkttag_t *pkttag;

	WL_TRACE(("wl%d: %s()\n", wlc->pub->unit, __FUNCTION__));

	pkttag = WLPKTTAG(pdu);
	ASSERT(pkttag != NULL);

	/* Make sure that it's a PDU */
	ASSERT(pkttag->flags & (WLF_CTLMGMT | WLF_TXHDR));

	ASSERT(scb != NULL);

	/* Drop the frame if it's not on the current band */
	/* Note: We drop it instead of returning as this can result in head-of-line
	 * blocking for Probe request frames during the scan
	 */
	if (scb->bandunit != CHSPEC_BANDUNIT(phy_utils_get_chanspec(WLC_PI(wlc)))) {
		PKTFREE(wlc->osh, pdu, TRUE);
		WLCNTINCR(wlc->pub->_cnt->txchanrej);
		return BCME_BADBAND;
	}

	/* Something is blocking data packets */
	if (wlc->block_datafifo & ~DATA_BLOCK_JOIN) {
		if (wlc->block_datafifo & ~(DATA_BLOCK_QUIET | DATA_BLOCK_JOIN | DATA_BLOCK_PS)) {
			WL_ERROR(("wl%d: %s: block_datafifo 0x%x\n",
				wlc->pub->unit, __FUNCTION__, wlc->block_datafifo));
		}
		return BCME_BUSY;
	}

	if ((scb->dropblock_dur > 0) && !(pkttag->flags & WLF_8021X)) {
		/* drop the non 802.1X packets when rekeying is in progress */
		WLCNTINCR(wlc->pub->_cnt->txfail);
		WLCNTSCBADD(scb->scb_stats.tx_failures, 1);
		PKTFREE(wlc->osh, pdu, TRUE);
		return BCME_NOTREADY;
	}

	/* add the txhdr if not present */
	if ((pkttag->flags & WLF_TXHDR) == 0) {
		wlc_pdu_txhdr(wlc, pdu, scb, fifo);
	}

	/*
	 * If the STA is in PS mode, this must be a PS Poll response or APSD delivery frame;
	 * fix the MPDU accordingly.
	 */
	cfg = SCB_BSSCFG(scb);
	BCM_REFERENCE(cfg);
	ASSERT(cfg != NULL);
	if (((BSSCFG_AP(cfg) || BSS_TDLS_ENAB(wlc, cfg)) && SCB_PS(scb)) ||
		(SCB_ISMULTI(scb) && ((pkttag->flags & WLF_CTLMGMT) == 0))) {
		/* TxFrameID can be updated for multi-cast packets */
		wlc_apps_ps_prep_mpdu(wlc, pdu);
	}

	ASSERT(fifo == D11_TXFID_GET_FIFO(wlc, wlc_get_txh_frameid(wlc, pdu)));

	return 0;
} /* wlc_prep_pdu */

static void
wlc_update_txpktfail_stats(wlc_info_t *wlc, uint32 pkt_len, uint8 prio)
{
	if (WME_ENAB(wlc->pub)) {
		WLCNTINCR(wlc->pub->_wme_cnt->tx_failed[WME_PRIO2AC(prio)].packets);
		WLCNTADD(wlc->pub->_wme_cnt->tx_failed[WME_PRIO2AC(prio)].bytes, pkt_len);
	}
}

void BCMFASTPATH
wlc_update_txpktsuccess_stats(wlc_info_t *wlc, struct scb *scb, bool broadcast, uint32 pkt_len,
                              uint8 prio, uint16 npkts)
{
	/* update stat counters */
	WLCNTADD(wlc->pub->_cnt->txframe, npkts);
	WLCNTADD(wlc->pub->_cnt->txbyte, pkt_len);
	WLPWRSAVETXFADD(wlc, npkts);
#ifdef WLLED
	wlc_led_start_activity_timer(wlc->ledh);
#endif

	/* update interface stat counters */
	WLCNTADD(SCB_WLCIFP(scb)->_cnt->txframe, npkts);
	WLCNTADD(SCB_WLCIFP(scb)->_cnt->txbyte, pkt_len);

	if (WME_ENAB(wlc->pub)) {
		WLCNTADD(wlc->pub->_wme_cnt->tx[WME_PRIO2AC(prio)].packets, npkts);
		WLCNTADD(wlc->pub->_wme_cnt->tx[WME_PRIO2AC(prio)].bytes, pkt_len);

	}
#ifdef WL_BSS_STATS
	if (broadcast) {
		WLCIFCNTADD(scb, txbcastpkts, npkts);
		WLCIFCNTADD(scb, txbcastbytes, pkt_len);
	} else if (SCB_BCMC(scb)) {
		WLCIFCNTADD(scb, txmcastpkts, npkts);
		WLCIFCNTADD(scb, txmcastbytes, pkt_len);
	}
	else {
		WLCIFCNTADD(scb, txucastpkts, npkts);
		WLCIFCNTADD(scb, txucastbytes, pkt_len);
	}
#endif /* WL_BSS_STATS */
}

/**
 * Convert 802.3 MAC header to 802.11 MAC header (data only)
 * and add WEP IV information if enabled.
 */
static struct dot11_header * BCMFASTPATH
wlc_80211hdr(wlc_info_t *wlc, void *p, struct scb *scb, bool more_frag, bool allow_htc,
	const wlc_key_info_t *key_info, uint8 prio, uint16 *pushlen)
{
	struct ether_header *eh;
	struct dot11_header *h;
	struct ether_addr tmpaddr;
	uint16 offset;
	struct ether_addr *ra;
	wlc_pkttag_t *pkttag;
	bool a4;
	osl_t *osh = wlc->osh;
	wlc_bsscfg_t *bsscfg;
	uint16 fc = 0;
	bool is_amsdu;
	bool htc_enab = FALSE;
	uint32 htc_code;

	ASSERT(scb != NULL);

	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);

	osh = wlc->osh;
	BCM_REFERENCE(osh);
	BCM_REFERENCE(is_amsdu);

	pkttag = WLPKTTAG(p);
	eh = (struct ether_header *) PKTDATA(osh, p);
	/* Only doing A3 frame if 802.3 frame's destination address
	 * matches this SCB's address.
	 */
	if (pkttag->flags & WLF_8021X)
		a4 = SCB_DWDS(scb) ?
		(bcmp(&eh->ether_dhost, &scb->ea, ETHER_ADDR_LEN) != 0) : (SCB_A4_8021X(scb));
	else
		a4 = (SCB_A4_DATA(scb) != 0);
	ra = (a4 ? &scb->ea : NULL);

	/* convert 802.3 header to 802.11 header */
	/* Make room for 802.11 header, add additional bytes if WEP enabled for IV */
	is_amsdu = (pkttag->flags & WLF_AMSDU);

	/* For AMSDU frames, ethernet header on top need to be retained
	 * For non -AMSDU ether header should be replaced with dot 11 header
	 */
#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
	offset = DOT11_A3_HDR_LEN - ETHER_HDR_LEN;
#else
	offset = (is_amsdu) ? DOT11_A3_HDR_LEN : (DOT11_A3_HDR_LEN - ETHER_HDR_LEN);
#endif

	if (a4)
		offset += ETHER_ADDR_LEN;

	htc_code = 0;
	if (SCB_QOS(scb)) {
		offset += DOT11_QOS_LEN;
		htc_enab = allow_htc && wlc_he_htc_tx(wlc, scb, p, &htc_code);
		if (htc_enab == TRUE) {
			offset += DOT11_HTC_LEN;
		}
	}

	ASSERT(key_info != NULL);
	if (!WLC_KEY_IS_LINUX_CRYPTO(key_info))
		offset += key_info->iv_len;

	h = (struct dot11_header *) PKTPUSH(osh, p, offset);
	bzero((char*)h, offset);

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
	*pushlen = offset + ETHER_HDR_LEN;
#else
	*pushlen = (is_amsdu) ? offset : (offset + ETHER_HDR_LEN);
#endif

	if (a4) {
		ASSERT(ra != NULL);
		/* WDS: a1 = RA, a2 = TA, a3 = DA, a4 = SA, ToDS = 0, FromDS = 1 */
		bcopy((char*)ra, (char*)&h->a1, ETHER_ADDR_LEN);
		bcopy((char*)&bsscfg->cur_etheraddr, (char*)&h->a2, ETHER_ADDR_LEN);
		/* eh->ether_dhost and h->a3 may overlap */
#ifdef WLWSEC
		if (key_info->algo != CRYPTO_ALGO_OFF || SCB_QOS(scb)) {
			/* In WEP case, &h->a3 + 4 = &eh->ether_dhost due to IV offset, thus
			 * need to bcopy
			 */
			bcopy((char*)&eh->ether_dhost, (char*)&tmpaddr, ETHER_ADDR_LEN);
			bcopy((char*)&tmpaddr, (char*)&h->a3, ETHER_ADDR_LEN);
		}
#endif
		/* eh->ether_shost and h->a4 may overlap */
		bcopy((char*)&eh->ether_shost, (char*)&tmpaddr, ETHER_ADDR_LEN);
		bcopy((char*)&tmpaddr, (char*)&h->a4, ETHER_ADDR_LEN);
		fc |= FC_TODS | FC_FROMDS;

		/* A-MSDU: use BSSID for A3 and A4, only need to fix up A3 */
		if (WLPKTFLAG_AMSDU(pkttag))
			bcopy((char*)&bsscfg->BSSID, (char*)&h->a3, ETHER_ADDR_LEN);
	} else if (BSSCFG_AP(bsscfg)) {
		ASSERT(ra == NULL);
		/* AP: a1 = DA, a2 = BSSID, a3 = SA, ToDS = 0, FromDS = 1 */
		bcopy(eh->ether_dhost, h->a1.octet, ETHER_ADDR_LEN);
		bcopy(bsscfg->BSSID.octet, h->a2.octet, ETHER_ADDR_LEN);
		/* eh->ether_shost and h->a3 may overlap */
#ifdef WLWSEC
		if (key_info->algo != CRYPTO_ALGO_OFF || SCB_QOS(scb)) {
			/* In WEP case, &h->a3 + 4 = &eh->ether_shost due to IV offset,
			 * thus need to bcopy
			 */
			bcopy((char*)&eh->ether_shost, (char*)&tmpaddr, ETHER_ADDR_LEN);
			bcopy((char*)&tmpaddr, (char*)&h->a3, ETHER_ADDR_LEN);
		}
#endif
		fc |= FC_FROMDS;
	} else {
		ASSERT(ra == NULL);
		if (bsscfg->BSS) {
			/* BSS STA: a1 = BSSID, a2 = SA, a3 = DA, ToDS = 1, FromDS = 0 */
			bcopy((char*)&bsscfg->BSSID, (char*)&h->a1, ETHER_ADDR_LEN);
			bcopy((char*)&eh->ether_dhost, (char*)&tmpaddr, ETHER_ADDR_LEN);
			bcopy((char*)&eh->ether_shost, (char*)&h->a2, ETHER_ADDR_LEN);
			bcopy((char*)&tmpaddr, (char*)&h->a3, ETHER_ADDR_LEN);
			fc |= FC_TODS;
		} else {
			/* IBSS/TDLS STA: a1 = DA, a2 = SA, a3 = BSSID, ToDS = 0, FromDS = 0 */
			bcopy((char*)&eh->ether_dhost, (char*)&h->a1, ETHER_ADDR_LEN);
			bcopy((char*)&eh->ether_shost, (char*)&h->a2, ETHER_ADDR_LEN);
			bcopy((char*)&bsscfg->BSSID, (char*)&h->a3, ETHER_ADDR_LEN);
		}
	}

	/* SCB_QOS: Fill QoS Control Field */
	if (SCB_QOS(scb)) {
		uint16 qos, *pqos;
		wlc_wme_t *wme = bsscfg->wme;

		/* Set fragment priority */
		qos = (prio << QOS_PRIO_SHIFT) & QOS_PRIO_MASK;

		/* Set the ack policy; AMPDU overrides wme_noack */
		if (WLPKTFLAG_AMPDU(pkttag))
			qos |= (QOS_ACK_NORMAL_ACK << QOS_ACK_SHIFT) & QOS_ACK_MASK;
		else if (wme->wme_noack == QOS_ACK_NO_ACK) {
			WLPKTTAG(p)->flags |= WLF_WME_NOACK;
			qos |= (QOS_ACK_NO_ACK << QOS_ACK_SHIFT) & QOS_ACK_MASK;
		} else {
			qos |= (wme->wme_noack << QOS_ACK_SHIFT) & QOS_ACK_MASK;
		}

		/* Set the A-MSDU bit for AMSDU packet */
		if (WLPKTFLAG_AMSDU(pkttag))
			qos |= (1 << QOS_AMSDU_SHIFT) & QOS_AMSDU_MASK;

		pqos = (uint16 *)((uchar *)h + (a4 ? DOT11_A4_HDR_LEN : DOT11_A3_HDR_LEN));
		ASSERT(ISALIGNED(pqos, sizeof(*pqos)));

		*pqos = htol16(qos);

		/* Set subtype to QoS Data */
		fc |= (FC_SUBTYPE_QOS_DATA << FC_SUBTYPE_SHIFT);

		/* Insert 11ax HTC+ field if needed */
		if (htc_enab) {
			uint32 *phtc;

			phtc = (uint32 *)((uint8 *)pqos + DOT11_QOS_LEN);
			*phtc = htol32(htc_code);
			fc |= FC_ORDER;
		}
	}

	fc |= (FC_TYPE_DATA << FC_TYPE_SHIFT);

	/* Set MoreFrag, WEP, and Order fc fields */
	if (more_frag)
		fc |= FC_MOREFRAG;
#ifdef WLWSEC
	if (key_info->algo != CRYPTO_ALGO_OFF)
		fc |= FC_WEP;
#endif
	h->fc = htol16(fc);

	return h;
} /* wlc_80211hdr */

/** alloc fragment buf and fill with data from msdu input packet */
static void *
wlc_allocfrag(osl_t *osh, void *sdu, uint offset, uint headroom, uint frag_length, uint tailroom)
{
	void *p1;
	uint plen;
	uint totlen = pkttotlen(osh, sdu);

	/* In TKIP, (offset >= pkttotlen(sdu)) is possible due to MIC. */
	if (offset >= totlen) {
		plen = 0; /* all sdu has been consumed */
	} else {
		plen = MIN(frag_length, totlen - offset);
	}
	/* one-copy: alloc fragment buffers and fill with data from msdu input pkt */

	/* XXX: this must be a copy until we fix up our PKTXXX macros to handle
	 * non contiguous pkts.
	 */

	/* alloc a new pktbuf */
	if ((p1 = PKTGET(osh, headroom + tailroom + plen, TRUE)) == NULL)
		return (NULL);
	PKTPULL(osh, p1, headroom);

	/* copy frags worth of data at offset into pktbuf */
	pktcopy(osh, sdu, offset, plen, (uchar*)PKTDATA(osh, p1));
	PKTSETLEN(osh, p1, plen);

	/* Currently our PKTXXX macros only handle contiguous pkts. */
	ASSERT(!PKTNEXT(osh, p1));

	return (p1);
}

#if defined(BCMPCIEDEV) && (defined(BCMFRAGPOOL) || (defined(BCMHWA) && \
	defined(HWA_TXPOST_BUILD)))
static void *
wlc_lfrag_get(osl_t *osh)
{
#if (defined(BCMHWA) && defined(HWA_TXPOST_BUILD))
	void *txbuf;
	NO_HWA_PKTPGR_EXPR(void *lfrag);

	txbuf = hwa_txpost_txbuffer_get(hwa_dev);

#if defined(HWA_PKTPGR_BUILD)
	return txbuf;
#else
	if (txbuf == NULL)
		return NULL;

	lfrag = HWAPKT2LFRAG((char *)txbuf);
	if (PKTBINDD11BUF(osh, lfrag) != BCME_OK) {
		hwa_txpost_txbuffer_free(hwa_dev, txbuf);
		return NULL;
	}
	return lfrag;
#endif /* HWA_PKTPGR_BUILD */
#else
	return pktpool_lfrag_get(SHARED_FRAG_POOL, D11_LFRAG_BUF_POOL);
#endif /* BCMHWA && HWA_TXPOST_BUILD */
}

/** called in the context of wlc_prep_sdu() */
static void *
wlc_allocfrag_txfrag(osl_t *osh, void *sdu, uint offset, uint frag_length,
	bool lastfrag)
{
	void *p1;
	uint plen;

	/* Len remaining to fill in the fragments */
	uint lenToFill = pkttotlen(osh, sdu) - offset;

	/* Currently TKIP is not handled */
	plen = MIN(frag_length, lenToFill);

	/* Cut out ETHER_HDR_LEN as frag_data does not account for that */
	offset -= ETHER_HDR_LEN;

	/* Need 202 bytes of headroom for TXOFF, 22 bytes for amsdu path */
	/* TXOFF + amsdu headroom */
	if ((p1 = wlc_lfrag_get(osh)) == NULL)
		return (NULL);

	PKTPULL(osh, p1, PKTFRAGSZ);
	PKTSETLEN(osh, p1, 0);

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
	/* The sdu packet it has ETH 14B in DNG and 8B LSH in BCM_DHDHDR.
	 * The first fragment pkts[0], it must have 8B LSH in BCM_DHDHDR as well
	 * we don't generate it in DNG, so pkt[0] has same data address as sdu.
	 * So don't PKTPUSH DOT11_LLC_SNAP_HDR_LEN in DNG and don't subtract
	 * DOT11_LLC_SNAP_HDR_LEN from plen.
	 * So for BCM_DHDHDR we get new txlfrag with D11_BUFFER and setup HOST
	 * data_address len .etc for each fragment pkt.  Now in this function each pkts[]'s
	 * length is 0, the caller will push ETH 14B and copy it from sdu to each pkts[].
	 */
#else
	/* If first fragment, copy LLC/SNAP header
	 * Host fragment length in this case, becomes plen - DOT11_LLC_SNAP hdr len
	 */
	if (WLPKTTAG(sdu)->flags & WLF_NON8023) {
		/* If first fragment, copy LLC/SNAP header
		* Host fragment length in this case, becomes plen - DOT11_LLC_SNAP hdr len
		*/
		if (offset == 0) {
			PKTPUSH(osh, p1, DOT11_LLC_SNAP_HDR_LEN);
			bcopy((uint8*) (PKTDATA(osh, sdu) + ETHER_HDR_LEN),
				PKTDATA(osh, p1), DOT11_LLC_SNAP_HDR_LEN);
			plen -= DOT11_LLC_SNAP_HDR_LEN;
		} else {
			/* For fragments other than the first fragment, deduct DOT11_LLC_SNAP len,
			* as that len should not be offset from Host data low address
			*/
			offset -= DOT11_LLC_SNAP_HDR_LEN;
		}
	}
#endif /* BCM_DHDHDR && DONGLEBUILD */

	/* Set calculated address offsets for Host data */
	PKTSETFRAGDATA_HI(osh, p1, LB_FRAG1, PKTFRAGDATA_HI(osh, sdu, LB_FRAG1));
	PKTSETFRAGDATA_LO(osh, p1, LB_FRAG1, PKTFRAGDATA_LO(osh, sdu, LB_FRAG1) + offset);
	PKTSETFRAGTOTNUM(osh, p1, 1);
	PKTSETFRAGLEN(osh, p1, LB_FRAG1, plen);
	PKTSETFRAGTOTLEN(osh, p1, plen);
	PKTSETIFINDEX(osh, p1, PKTIFINDEX(osh, sdu));

#if defined(PROP_TXSTATUS)
	/* If incoming sdu is from host, set appropriate flags for new frags too */
	if (WL_TXSTATUS_GET_FLAGS(WLPKTTAG(sdu)->wl_hdr_information) &
		WLFC_PKTFLAG_PKTFROMHOST) {

		WL_TXSTATUS_SET_FLAGS(WLPKTTAG(p1)->wl_hdr_information,
			WLFC_PKTFLAG_PKTFROMHOST);
	}
#endif /* PROP_TXSTATUS */

	/* Only last fragment should have metadata
	 * and valid PKTID. Reset metadata and set invalid PKTID
	 * for other fragments
	 */
	if (lastfrag) {
		PKTSETFRAGPKTID(osh, p1, PKTFRAGPKTID(osh, sdu));
		PKTSETFRAGFLOWRINGID(osh, p1, PKTFRAGFLOWRINGID(osh, sdu));
#if defined(PROP_TXSTATUS)
		PKTFRAGSETRINGINDEX(osh, p1, PKTFRAGRINGINDEX(osh, sdu));
#endif /* PROP_TXSTATUS */
		PKTSETHASMETADATA(osh, p1);
	} else {
		PKTRESETHASMETADATA(osh, p1);
		PKTSETFRAGPKTID(osh, p1, 0xdeadbeaf);
	}

	return (p1);
} /* wlc_allocfrag_txfrag */
#endif	/* BCMPCIEDEV */

/** converts a single caller provided fragment (in 'p') to 802.11 format */
static void BCMFASTPATH
wlc_dofrag(wlc_info_t *wlc, void *p, uint frag, uint nfrags,
	uint next_payload_len, struct scb *scb, bool is8021x, uint fifo,
	wlc_key_t *key, const wlc_key_info_t *key_info,
	uint8 prio, uint frag_length, ratespec_t rspec)
{
	struct dot11_header *h = NULL;
	uint next_frag_len;
	uint16 frameid, txc_hdr_len = 0;
	uint16 txh_off = 0;
	wlc_bsscfg_t *cfg;
	uint16 d11hdr_len = 0;
	ASSERT(scb != NULL);

	cfg = SCB_BSSCFG(scb);
	ASSERT(cfg != NULL);
	BCM_REFERENCE(frag_length);

	WL_APSTA_TX(("wl%d.%d: wlc_dofrag: send %p to "MACF
		", cfg %p, fifo %d, frag %d, nfrags %d\n",
		wlc->pub->unit, WLC_BSSCFG_IDX(cfg), OSL_OBFUSCATE_BUF(p),
		ETHER_TO_MACF(scb->ea), OSL_OBFUSCATE_BUF(cfg), fifo, frag, nfrags));

	/*
	 * 802.3 (header length = 22):
	 *                     (LLC includes ether_type in last 2 bytes):
	 * ----------------------------------------------------------------------------------------
	 * |                                      |   DA   |   SA   | L | LLC/SNAP | T |  Data... |
	 * ----------------------------------------------------------------------------------------
	 *                                             6        6     2       6      2
	 *
	 * NON-WDS
	 *
	 * Conversion to 802.11 (header length = 32):
	 * ----------------------------------------------------------------------------------------
	 * |              | FC | D |   A1   |   A2   |   A3   | S | QoS | LLC/SNAP | T |  Data... |
	 * ----------------------------------------------------------------------------------------
	 *                   2   2      6        6        6     2    2        6      2
	 *
	 * Conversion to 802.11 (WEP, QoS):
	 * ----------------------------------------------------------------------------------------
	 * |         | FC | D |   A1   |   A2   |   A3   | S | QoS | IV | LLC/SNAP | T |  Data... |
	 * ----------------------------------------------------------------------------------------
	 *             2   2      6        6        6     2    2     4        6      2
	 *
	 * WDS
	 *
	 * Conversion to 802.11 (header length = 38):
	 * ----------------------------------------------------------------------------------------
	 * |     | FC | D |   A1   |   A2   |   A3   | S |   A4   | QoS | LLC/SNAP | T |  Data... |
	 * ----------------------------------------------------------------------------------------
	 *         2   2      6        6        6     2      6      2         6      2
	 *
	 * Conversion to 802.11 (WEP, QoS):
	 * ----------------------------------------------------------------------------------------
	 * | FC | D |   A1   |   A2   |   A3   | S |   A4   |  QoS | IV | LLC/SNAP | T |  Data... |
	 * ----------------------------------------------------------------------------------------
	 *    2   2      6        6        6     2      6       2    4        6      2
	 *
	 */

	WL_WSEC(("wl%d.%d: %s: tx sdu, len %d(with 802.3 hdr)\n",
		wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__, pkttotlen(wlc->osh, p)));

	ASSERT(key != NULL && key_info != NULL);

	{
		/*
		 * Convert 802.3 MAC header to 802.11 MAC header (data only)
		 * and add WEP IV information if enabled.
		 *  requires valid DA and SA, does not care ether_type
		 */
		/* cannot do HTC on low rate packets; 8021X is sent on basic rate */
		bool forbid_htc = (RSPEC_ACTIVE(rspec) && RSPEC_ISLEGACY(rspec)) || is8021x;
		h = wlc_80211hdr(wlc, p, scb, (bool)(frag != (nfrags - 1)), !forbid_htc,
				key_info, prio, &d11hdr_len);
	}

	WL_WSEC(("wl%d.%d: %s: tx sdu, len %d(w 80211hdr and iv, no d11hdr, icv and fcs)\n",
		wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__, pkttotlen(wlc->osh, p)));

	/* determine total MPDU length of next frag */
	next_frag_len = next_payload_len;
	if (next_frag_len > 0) {	/* there is a following frag */
		next_frag_len += DOT11_A3_HDR_LEN + DOT11_FCS_LEN;
		/* A4 header */
		if (SCB_A4_DATA(scb))
			next_frag_len += ETHER_ADDR_LEN;
		/* SCB_QOS: account for QoS Control Field */
		if (SCB_QOS(scb)) {
			next_frag_len += DOT11_QOS_LEN;
		}

		if (!WLC_KEY_IS_LINUX_CRYPTO(key_info))
			next_frag_len += key_info->iv_len + key_info->icv_len;
	}

	/* add d11 headers */
	frameid = wlc_d11hdrs(wlc, p, scb,
		(WLC_PROT_CFG_SHORTPREAMBLE(wlc->prot, cfg) &&
		(scb->flags & SCB_SHORTPREAMBLE) != 0),
		frag, nfrags, fifo, next_frag_len, key, key_info, rspec);
	BCM_REFERENCE(frameid);

#ifdef WLTOEHW
	if (D11REV_GE(wlc->pub->corerev, 40) && (!wlc->toe_bypass)) {
		txh_off = wlc_tso_hdr_length((d11ac_tso_t*)PKTDATA(wlc->osh, p));
	}
#endif /* WLTOEHW */
	txc_hdr_len = d11hdr_len + (txh_off + D11_TXH_LEN_EX(wlc));

#ifdef STA
	/*
	 * TKIP countermeasures: register pkt callback for last frag of
	 * MIC failure notification
	 *
	 * XXX - This only checks for the next 802.1x frame; how can we make
	 * sure this is the MIC failure notification?
	 */
	if (BSSCFG_STA(cfg) && next_frag_len == 0 && is8021x) {
		bool tkip_cm;
		tkip_cm = wlc_keymgmt_tkip_cm_enabled(wlc->keymgmt, cfg);
		if (tkip_cm) {
			WL_WSEC(("wl%d.%d: %s: TKIP countermeasures: sending MIC failure"
				" report...\n", WLCWLUNIT(wlc),
				WLC_BSSCFG_IDX(cfg), __FUNCTION__));
			if (frameid == 0xffff) {
				WL_ERROR(("wl%d.%d: %s: could not register MIC failure packet"
					" callback\n", WLCWLUNIT(wlc), WLC_BSSCFG_IDX(cfg),
					__FUNCTION__));
			} else {
				/* register packet callback */
				WLF2_PCB1_REG(p, WLF2_PCB1_TKIP_CM);
			}
		}
	}
#endif /* STA */

	if (BSSCFG_AP(cfg) ||
		BSS_TDLS_BUFFER_STA(cfg)) {
		/* Processing to set MoreData when needed */
		if (fifo != TX_BCMC_FIFO) {
			/* Check if packet is being sent to STA in PS mode */
			if (SCB_PS(scb)) {
				bool last_frag;

				last_frag = (frag == nfrags - 1);
				/* Make preparations for APSD delivery frame or PS-Poll response */
				if (AC_BITMAP_TST(scb->apsd.ac_delv, prio2ac[prio]))
					wlc_apps_apsd_prepare(wlc, scb, p, h, last_frag);
				else
					wlc_apps_pspoll_resp_prepare(wlc, scb, p, h, last_frag);
				BCM_REFERENCE(last_frag);
			}
		} else {
			/* Broadcast/multicast. The uCode clears the MoreData field of the last bcmc
			 * pkt per dtim period. Suppress setting MoreData if we have to support
			 * legacy AES. This should probably also be conditional on at least one
			 * legacy STA associated.
			 */
			/*
			* Note: We currently use global WPA_auth for WDS and per scb WPA_auth
			* for others, but fortunately there is no bcmc frames over WDS link
			* therefore we don't need to worry about WDS and it is safe to use per
			* scb WPA_auth only here!
			*/
			/* Also look at wlc_apps_ps_prep_mpdu if following condition ever changes */
			wlc_key_info_t bss_key_info;
			(void)wlc_keymgmt_get_bss_tx_key(wlc->keymgmt, cfg, FALSE, &bss_key_info);
			if (!bcmwpa_is_wpa_auth(cfg->WPA_auth) ||
				(bss_key_info.algo != CRYPTO_ALGO_AES_CCM)) {
				h->fc |= htol16(FC_MOREDATA);
			}
		}
	}

	/* dont cache 8021x frames in case of DWDS */
	if (is8021x && SCB_A4_DATA(scb) && !SCB_A4_8021X(scb))
		return;

	/* install new header into tx header cache: starting from the bytes after DA-SA-L  */
	if (WLC_TXC_ENAB(wlc)) {
		wlc_txc_info_t *txc = wlc->txc;

		if (TXC_CACHE_ENAB(txc) && (nfrags == 1) &&
		    !(WLPKTTAG(p)->flags & WLF_BYPASS_TXC) &&
		    TRUE) {
			wlc_txc_add(txc, scb, p, txc_hdr_len, fifo, prio, txh_off, d11hdr_len);
		}
	}
} /* wlc_dofrag */

/**
 * allocate headroom buffer if necessary and convert ether frame to 8023 frame
 * !! WARNING: HND WL driver only supports data frame of type ethernet, 802.1x or 802.3
 */
void * BCMFASTPATH
wlc_hdr_proc(wlc_info_t *wlc, void *sdu, struct scb *scb)
{
	osl_t *osh;
	struct ether_header *eh;
	void *pkt;
	int prio;
	uint16 ether_type;
	uint headroom = wlc->txhroff;

	osh = wlc->osh;

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
	/* When BCM_DHDHDR enabled dongle uses D3_BUFFER by default,
	* it doesn't have extra headroom to construct 802.3 or 802.11 header.
	* The DHD host driver will prepare the 802.3 header for dongle.
	*/
	if (PKTISTXFRAG(osh, sdu))
		headroom = 0;
#endif

	/* allocate enough room once for all cases */
	prio = PKTPRIO(sdu);

	if ((uint)PKTHEADROOM(osh, sdu) < headroom || PKTSHARED(sdu)) {
		pkt = PKTGET(osh, wlc->txhroff, TRUE);

		if (pkt == NULL) {
			WL_ERROR(("wl%d: %s, PKTGET headroom %d failed\n",
				wlc->pub->unit, __FUNCTION__, wlc->txhroff));
			WLCNTINCR(wlc->pub->_cnt->txnobuf);
			/* increment interface stats */
			WLCIFCNTINCR(scb, txnobuf);
			WLCNTSCBINCR(scb->scb_stats.tx_failures);
#ifdef WL11K
			wlc_rrm_stat_qos_counter(wlc, scb, prio,
				OFFSETOF(rrm_stat_group_qos_t, txfail));
#endif
			return NULL;
		}

		PKTPULL(osh, pkt, wlc->txhroff);

		wlc_pkttag_info_move(wlc, sdu, pkt);

		/* Transfer priority */
		PKTSETPRIO(pkt, prio);

		/* move ether_hdr from data buffer to header buffer */
		eh = (struct ether_header*) PKTDATA(osh, sdu);
		PKTPULL(osh, sdu, ETHER_HDR_LEN);
		PKTPUSH(osh, pkt, ETHER_HDR_LEN);
		bcopy((char*)eh, (char*)PKTDATA(osh, pkt), ETHER_HDR_LEN);

		/* chain original sdu onto newly allocated header */
		PKTSETNEXT(osh, pkt, sdu);
		sdu = pkt;
	}

	/*
	 * Optionally add an 802.1Q VLAN tag to convey non-zero priority for
	 * non-WMM associations.
	 */
	eh = (struct ether_header *)PKTDATA(osh, sdu);
	ether_type = ntoh16(eh->ether_type);

	if (prio && !SCB_QOS(scb)) {
		if (headroom != 0 && (wlc->vlan_mode != OFF) &&
			(ntoh16(eh->ether_type) != ETHER_TYPE_8021Q)) {
			struct ethervlan_header *vh;
			struct ether_header da_sa;

			bcopy(eh, &da_sa, VLAN_TAG_OFFSET);
			vh = (struct ethervlan_header *)PKTPUSH(osh, sdu, VLAN_TAG_LEN);
			bcopy(&da_sa, vh, VLAN_TAG_OFFSET);

			vh->vlan_type = hton16(ETHER_TYPE_8021Q);
			vh->vlan_tag = hton16(prio << VLAN_PRI_SHIFT);	/* Priority-only Tag */
			eh = (struct ether_header *)PKTDATA(osh, sdu);
		}
	}

#ifdef MULTIAP
	/* In MultiAP R2, if we are sending EAPOL message to a profile 2 device and that device
	 * activated traffic separation, then the EAPOL packet should be tagged
	 */
	if ((wlc->vlan_mode == OFF) && (ether_type == ETHER_TYPE_802_1X)) {
		wlc_bsscfg_t *bsscfg;

		bsscfg = SCB_BSSCFG(scb);

		if (MAP_ENAB(bsscfg) && SCB_MAP_P2_CAP(scb) && (bsscfg->map_prim_vlan_id > 0) &&
			((bsscfg->map_attr & MAP_EXT_ATTR_BACKHAUL_BSS) ||
			(bsscfg->map_attr & MAP_EXT_ATTR_BACKHAUL_STA))) {

			struct dot11_llc_snap_header *lsh =
				(struct dot11_llc_snap_header*)(PKTDATA(osh, sdu) + ETHER_HDR_LEN);
			uint8 *src;
			uint16 vlan_offset = VLAN_TAG_OFFSET;

			ASSERT((uint)PKTHEADROOM(osh, sdu) > (headroom + VLAN_TAG_LEN));

			src = (uint8*)PKTDATA(osh, sdu);
			/* If the packet already has llc snap header.
			 * |   DA   |   SA   | L | LLC/SNAP | T |
			 */
			if (rfc894chk(wlc, lsh)) {
				struct dot3_mac_llc_snapvlan_header *llc_vh;

				vlan_offset += DOT11_LLC_SNAP_HDR_LEN;
				llc_vh = (struct dot3_mac_llc_snapvlan_header*)
					(PKTPUSH(osh, sdu, VLAN_TAG_LEN));
				bcopy(src, llc_vh, vlan_offset);
				llc_vh->vlan_type = hton16(ETHER_TYPE_8021Q);
				llc_vh->vlan_tag = hton16(bsscfg->map_prim_vlan_id);
			} else {
				struct ethervlan_header *vh;

				vh = (struct ethervlan_header *)PKTPUSH(osh, sdu, VLAN_TAG_LEN);
				bcopy(src, vh, vlan_offset);

				vh->vlan_type = hton16(ETHER_TYPE_8021Q);
				vh->vlan_tag = hton16(bsscfg->map_prim_vlan_id);
			}
			eh = (struct ether_header *)PKTDATA(osh, sdu);
		}
	}
#endif /* MULTIAP */

	/*
	 * Original Ethernet (header length = 14):
	 * ----------------------------------------------------------------------------------------
	 * |                                                     |   DA   |   SA   | T |  Data... |
	 * ----------------------------------------------------------------------------------------
	 *                                                            6        6     2
	 *
	 * Conversion to 802.3 (header length = 22):
	 *                     (LLC includes ether_type in last 2 bytes):
	 * ----------------------------------------------------------------------------------------
	 * |                                      |   DA   |   SA   | L | LLC/SNAP | T |  Data... |
	 * ----------------------------------------------------------------------------------------
	 *                                             6        6     2       6      2
	 */
#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
		/* When BCM_DHDHDR enabled, 8 byte LLC SNAP is always added in DDR
		 * Adjust host address and length to include LLC header.
		 */

		/* Don't adjust host data addr if sdu was pktfetched, because llc snap is
		 * already part of pktfetched data.
		 */
		if (PKTFRAGTOTNUM(wlc->osh, sdu)) {
			PKTSETFRAGDATA_LO(wlc->osh, sdu, LB_FRAG1,
				PKTFRAGDATA_LO(wlc->osh, sdu, LB_FRAG1) -
				DOT11_LLC_SNAP_HDR_LEN);
			PKTSETFRAGLEN(wlc->osh, sdu, LB_FRAG1,
				PKTFRAGLEN(wlc->osh, sdu, LB_FRAG1) +
				DOT11_LLC_SNAP_HDR_LEN);
			PKTSETFRAGTOTLEN(wlc->osh, sdu,
				PKTFRAGTOTLEN(wlc->osh, sdu) + DOT11_LLC_SNAP_HDR_LEN);
		}
#endif /* BCM_DHDHDR && DONGLEBUILD */

	if (ether_type > ETHER_MAX_DATA) {
		if (ether_type == ETHER_TYPE_802_1X) {
			if (SCAN_IN_PROGRESS(wlc->scan) && !AS_IN_PROGRESS(wlc)) {
				wlc_scan_abort(wlc->scan, WLC_E_STATUS_ABORT);
			}
			WLPKTTAG(sdu)->flags |= WLF_8021X;
			WLPKTTAG(sdu)->flags |= WLF_BYPASS_TXC;
			WLPKTTAG(sdu)->flags3 |= WLF3_BYPASS_AMPDU;
			WL_ASSOC(("wl%d: %s "MACF" 8021X p %p\n",
				wlc->pub->unit, __FUNCTION__, ETHER_TO_MACF(scb->ea), sdu));
			/*
			 * send 8021x packets at higher priority than best efforts.
			 * use _VI, as _VO prioroty creates "txop exceeded" error
			 */
			prio = PRIO_8021D_VI;
			PKTSETPRIO(sdu, prio);
		}
#ifdef BCMWAPI_WAI
		else if (ether_type == ETHER_TYPE_WAI) {
			WLPKTTAG(sdu)->flags |= WLF_WAI;
			WLPKTTAG(sdu)->flags |= WLF_BYPASS_TXC;
		}
#endif /* BCMWAPI_WAI */

		/* save original type in pkt tag */
		WLPKTTAG(sdu)->flags |= WLF_NON8023;

#if defined(BCM_DHDHDR) && defined(DONGLEBUILD)
		/* When BCM_DHDHDR enabled, dongle just need to adjust the host data addr to
		 * include llc snap 8B in host.
		 */
		if (headroom == 0) {
			uint16 plen;
			plen = (uint16)pkttotlen(osh, sdu) - ETHER_HDR_LEN;
			eh->ether_type = hton16(plen);
		} else
#endif /* BCM_DHDHDR && DONGLEBUILD */
		{
			wlc_ether_8023hdr(wlc, osh, eh, sdu);
		}
	}

	return sdu;
} /* wlc_hdr_proc */

/** Determines the number of fragments and the fragment length in case packet length exceeds
 *  fragmentation threshold.
 *  According to the standard (10.4), the length of each fragment shall be an equal number of
 *  octets for all fragments except the last, which may be smaller.
 *  If security encapsulation is active for the MPDU, then the MPDU shall be expanded by the
 *  encapsulation overhead and this may result is a fragment larger than fragmentation threshold.
 */
static INLINE uint
wlc_frag(wlc_info_t *wlc, struct scb *scb, enum wme_ac ac, uint plen, uint *flen)
{
	uint payload, thresh, nfrags, bt_thresh = 0;
	int btc_mode;
	uint8 hdrLength = ETHER_HDR_LEN;

	plen -= hdrLength;

	ASSERT(ac < AC_COUNT);

	thresh = wlc->fragthresh[ac];

	btc_mode = wlc_btc_mode_get(wlc);

	if (IS_BTCX_FULLTDM(btc_mode))
		bt_thresh = wlc_btc_frag_threshold(wlc, scb);

	if (bt_thresh)
		thresh = thresh > bt_thresh ? bt_thresh : thresh;

	/* optimize for non-fragmented case */
	if (plen < (thresh - (DOT11_A4_HDR_LEN + DOT11_QOS_LEN +
		DOT11_FCS_LEN + ETHER_ADDR_LEN))) {
		*flen = plen;
		return (1);
	}

	/* account for 802.11 MAC header */
	thresh -= DOT11_A3_HDR_LEN + DOT11_FCS_LEN;

	/* account for A4 */
	if (SCB_A4_DATA(scb))
		thresh -= ETHER_ADDR_LEN;

	/* SCB_QOS: account for QoS Control Field */
	if (SCB_QOS(scb))
		thresh -= DOT11_QOS_LEN;

	/*
	 * Spec says broadcast and multicast frames are not fragmented.
	 * LLC/SNAP considered part of packet payload.
	 * Fragment length must be even per 9.4 .
	 */
	if ((plen > thresh) && !SCB_ISMULTI(scb)) {
		payload = thresh & ~1;
		nfrags = (plen + payload - 1) / payload;
		*flen = ((plen / nfrags) + 1) & ~1;
	} else {
		*flen = plen;
		nfrags = 1;
	}

	ASSERT(nfrags <= DOT11_MAXNUMFRAGS);

	return (nfrags);
} /* wlc_frag */

/**
 * Add d11txh_t, cck_phy_hdr_t for pre-11ac mac & phy.
 *
 * 'p' data must start with 802.11 MAC header
 * 'p' must allow enough bytes of local headers to be "pushed" onto the packet
 *
 * headroom == D11_PHY_HDR_LEN + D11_TXH_LEN (D11_TXH_LEN is now 104 bytes)
 *
 */
uint16
wlc_d11hdrs_pre40(wlc_info_t *wlc, void *p, struct scb *scb, uint txparams_flags, uint frag,
	uint nfrags, uint fifo, uint next_frag_len, wlc_key_t *key, const wlc_key_info_t *key_info,
	ratespec_t rspec_override)
{
	struct dot11_header *h;
	d11txh_pre40_t *txh;
	uint8 *plcp, plcp_fallback[D11_PHY_HDR_LEN];
	osl_t *osh;
	int len, phylen, rts_phylen;
	uint16 fc, type, frameid, mch, phyctl, xfts, mainrates, rate_flag;
	uint16 seq = 0, mcl = 0, status = 0;
	bool use_rts = FALSE;
	bool use_cts = FALSE, rspec_history = FALSE;
	bool use_rifs = FALSE;
	bool short_preamble;
	uint8 preamble_type = WLC_LONG_PREAMBLE, fbr_preamble_type = WLC_LONG_PREAMBLE;
	uint8 rts_preamble_type = WLC_LONG_PREAMBLE, rts_fbr_preamble_type = WLC_LONG_PREAMBLE;
	uint8 *rts_plcp, rts_plcp_fallback[D11_PHY_HDR_LEN];
	struct dot11_rts_frame *rts = NULL;
	ratespec_t rts_rspec = 0, rts_rspec_fallback = 0;
	bool qos, a4;
	enum wme_ac ac;
	wlc_pkttag_t *pkttag;
	ratespec_t rspec, rspec_fallback;
	ratesel_txparams_t ratesel_rates;
#if defined(BCMDBG) || (defined(WLTEST) && !defined(WLTEST_DISABLED))
	uint16 phyctl_bwo = -1;
#endif /* (BCMDBG) || (WLTEST) */
#if WL_HT_TXBW_OVERRIDE_ENAB
	int8 txbw_override_idx;
#endif /* WL_HT_TXBW_OVERRIDE_ENAB */
	bool use_mimops_rts = FALSE;
	uint phyctl1_stf = 0;
	uint16 durid = 0;
	wlc_bsscfg_t *bsscfg;
	bool g_prot;
	int8 n_prot;
	wlc_wme_t *wme;
#ifdef WL_LPC
	uint8 lpc_offset = 0;
#endif
	uint8 sgi_tx;
	wlc_ht_info_t *hti = wlc->hti;
	uint16 rtsthresh;
	d11txhdr_t *hdrptr;

	ASSERT(scb != NULL);
	ASSERT(fifo < WLC_HW_NFIFO_INUSE(wlc));

	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);

	rtsthresh = wlc->RTSThresh;
	short_preamble = (txparams_flags & WLC_TX_PARAMS_SHORTPRE) != 0;

	/* init the g_prot to bsscfg protections only when band is 2.4G */
	if (BAND_2G(wlc->band->bandtype))
		g_prot = WLC_PROT_G_CFG_G(wlc->prot_g, bsscfg);
	else
		g_prot = FALSE;

	n_prot = WLC_PROT_N_CFG_N(wlc->prot_n, bsscfg);

	wme = bsscfg->wme;

	osh = wlc->osh;

	/* locate 802.11 MAC header */
	h = (struct dot11_header*) PKTDATA(osh, p);
	pkttag = WLPKTTAG(p);
	fc = ltoh16(h->fc);
	type = FC_TYPE(fc);
	qos = (type == FC_TYPE_DATA && FC_SUBTYPE_ANY_QOS(FC_SUBTYPE(fc)));
	a4 = (fc & (FC_TODS | FC_FROMDS)) == (FC_TODS | FC_FROMDS);

	/* compute length of frame in bytes for use in PLCP computations
	 * phylen =  packet length + ICV_LEN + FCS_LEN
	 */
	len = pkttotlen(osh, p);
	phylen = len + DOT11_FCS_LEN;

	/* Add room in phylen for the additional bytes for icv, if required.
	 * this is true for both h/w and s/w encryption. The latter only
	 * modifies the pkt length
	 */
	if (key_info != NULL) {
		if (WLC_KEY_IS_MGMT_GROUP(key_info))
			phylen += WLC_KEY_MMIC_IE_LEN(key_info);
		else
			phylen += key_info->icv_len;

		/* external crypto adds iv to the pkt, include it in phylen */
		if (WLC_KEY_IS_LINUX_CRYPTO(key_info))
			phylen += key_info->iv_len;

		/* Only the last fragment will have tkip mic, complete tkip mic size can be
		 * added to phylen.
		 */
		if (WLC_KEY_FRAG_HAS_TKIP_MIC(p, key_info, frag, nfrags))
			phylen += TKIP_MIC_SIZE;
	}

	WL_NONE(("wl%d: %s: len %d, phylen %d\n", WLCWLUNIT(wlc), __FUNCTION__, len, phylen));

	/* add PLCP */
	plcp = PKTPUSH(osh, p, D11_PHY_HDR_LEN);

	/* add Broadcom tx descriptor header */
	txh = (d11txh_pre40_t*)PKTPUSH(osh, p, D11_TXH_LEN);
	bzero((char*)txh, D11_TXH_LEN);

	/* use preassigned or software seqnum */
	if (!wlc_tx_preassigned_seqnum(wlc, pkttag, &seq)) {
		if (SCB_QOS(scb) &&
			((fc & FC_KIND_MASK) == FC_QOS_DATA) && !ETHER_ISMULTI(&h->a1)) {
			seq = SCB_SEQNUM(scb, PKTPRIO(p));
			/* Increment the sequence number only after the last fragment */
			if (frag == (nfrags - 1))
				SCB_SEQNUM(scb, PKTPRIO(p))++;
		} else if (type != FC_TYPE_CTL) {
			/* use h/w seqnum */
			mcl |= TXC_HWSEQ;
		}
	}

	if (type != FC_TYPE_CTL) {
		seq = (seq << SEQNUM_SHIFT) | (frag & FRAGNUM_MASK);
		h->seq = htol16(seq);
	}

#ifdef PROP_TXSTATUS
	if (PROP_TXSTATUS_ENAB(wlc->pub) &&
		WLFC_GET_REUSESEQ(wlfc_query_mode(wlc->wlfc))) {
		WL_SEQ_SET_NUM(pkttag->seq, ltoh16(h->seq) >> SEQNUM_SHIFT);
		SET_WL_HAS_ASSIGNED_SEQ(pkttag->seq);
	}
#endif /* PROP_TXSTATUS */
	/* setup frameid */
	frameid = wlc_compute_frameid(wlc,
		fifo == TX_BCMC_FIFO ? txh->TxFrameID : 0,
		fifo);

	/* set the ignpmq bit for all pkts tx'd in PS mode and for beacons and for anything
	 * going out from a STA interface.
	 */
	if (SCB_PS(scb) || ((fc & FC_KIND_MASK) == FC_BEACON) || BSSCFG_STA(bsscfg))
		mcl |= TXC_IGNOREPMQ;

	ac = WME_PRIO2AC(PKTPRIO(p));
	/* (1) RATE: determine and validate primary rate and fallback rates */

	if (RSPEC_ACTIVE(rspec_override)) {
		rspec = rspec_fallback = rspec_override;
		rspec_history = TRUE;
	}
	/* XXX: set both regular and fb rate for 802.1x as lowest basic
	 * Setting only the fb rate as basic fails for 4321
	 */
	else if ((type == FC_TYPE_MNG) ||
		(type == FC_TYPE_CTL) ||
		(pkttag->flags & WLF_8021X) ||
#ifdef BCMWAPI_WAI
		(pkttag->flags & WLF_WAI) ||
#endif
		(pkttag->flags3 & WLF3_DATA_WOWL_PKT))
		rspec = rspec_fallback = scb->rateset.rates[0] & RATE_MASK;
	else if (RSPEC_ACTIVE(wlc->band->mrspec_override) && ETHER_ISMULTI(&h->a1))
		rspec = rspec_fallback = wlc->band->mrspec_override;
	else if (RSPEC_ACTIVE(wlc->band->rspec_override) && !ETHER_ISMULTI(&h->a1)) {
		rspec = rspec_fallback = wlc->band->rspec_override;
		rspec_history = TRUE;
	} else if (ETHER_ISMULTI(&h->a1) || SCB_INTERNAL(scb))
		rspec = rspec_fallback = scb->rateset.rates[0] & RATE_MASK;
	else {
		/* run rate algorithm for data frame only, a cookie will be deposited in frameid */
		ratesel_rates.num = 2; /* only need up to 2 for non-11ac */
		ratesel_rates.ac = ac;
		wlc_scb_ratesel_gettxrate(wlc->wrsi, scb, &frameid, &ratesel_rates, &rate_flag);
		rspec = ratesel_rates.rspec[0];
		rspec_fallback = ratesel_rates.rspec[1];

		if (((scb->flags & SCB_BRCM) == 0) &&
		    (((fc & FC_KIND_MASK) == FC_NULL_DATA) ||
		     ((fc & FC_KIND_MASK) == FC_QOS_NULL))) {
			/* Use RTS/CTS rate for NULL frame */
			rspec = wlc_rspec_to_rts_rspec(bsscfg, rspec, FALSE);
			rspec_fallback = scb->rateset.rates[0] & RATE_MASK;
		} else {
			pkttag->flags |= WLF_RATE_AUTO;

			/* The rate histo is updated only for packets on auto rate. */
			/* perform rate history after txbw has been selected */
			if (frag == 0)
				rspec_history = TRUE;
		}
		if (rate_flag & RATESEL_VRATE_PROBE)
			WLPKTTAG(p)->flags |= WLF_VRATE_PROBE;

#ifdef WL_LPC
		if (LPC_ENAB(wlc)) {
			/* Query the power offset to be used from LPC */
			lpc_offset = wlc_scb_lpc_getcurrpwr(wlc->wlpci, scb, ac);
		} else {
			/* No Link Power Control. Transmit at nominal power. */
		}
#endif
	}

	/*
	 * At this point the rspec may not include a valid txbw. Apply HT mimo use rules.
	 *
	 * XXX: The stf mode also needs to be selected. It is unknown at this date (12/10/05)
	 * whether or not mimo auto rate will include mode selection. For now forced rates can
	 * include an stf mode, if none is selected the default is SISO.
	 */
	phyctl1_stf = wlc->stf->ss_opmode;

	if (N_ENAB(wlc->pub)) {
		uint32 mimo_txbw;
		uint8 mimo_preamble_type;

		bool stbc_tx_forced = WLC_IS_STBC_TX_FORCED(wlc);
		bool stbc_ht_scb_auto = WLC_STF_SS_STBC_HT_AUTO(wlc, scb);

		/* apply siso/cdd to single stream mcs's or ofdm if rspec is auto selected */
		if (((RSPEC_ISHT(rspec) && IS_SINGLE_STREAM(rspec & WL_RSPEC_HT_MCS_MASK)) ||
		     RSPEC_ISOFDM(rspec)) &&
		    !(rspec & WL_RSPEC_OVERRIDE_MODE)) {
			rspec &= ~(WL_RSPEC_TXEXP_MASK | WL_RSPEC_STBC);

			/* For SISO MCS use STBC if possible */
			if (RSPEC_ISHT(rspec) && (stbc_tx_forced || stbc_ht_scb_auto)) {
				ASSERT(WLC_STBC_CAP_PHY(wlc));
				rspec |= WL_RSPEC_STBC;
			} else if (phyctl1_stf == PHY_TXC1_MODE_CDD) {
				rspec |= (1 << WL_RSPEC_TXEXP_SHIFT);
			}
		}

		if (((RSPEC_ISHT(rspec_fallback) &&
		      IS_SINGLE_STREAM(rspec_fallback & WL_RSPEC_HT_MCS_MASK)) ||
		     RSPEC_ISOFDM(rspec_fallback)) &&
		    !(rspec_fallback & WL_RSPEC_OVERRIDE_MODE)) {
			rspec_fallback &= ~(WL_RSPEC_TXEXP_MASK | WL_RSPEC_STBC);

			/* For SISO MCS use STBC if possible */
			if (RSPEC_ISHT(rspec_fallback) && (stbc_tx_forced || stbc_ht_scb_auto)) {
				ASSERT(WLC_STBC_CAP_PHY(wlc));
				rspec_fallback |= WL_RSPEC_STBC;
			} else if (phyctl1_stf == PHY_TXC1_MODE_CDD) {
				rspec_fallback |= (1 << WL_RSPEC_TXEXP_SHIFT);
			}
		}

		/* Is the phy configured to use 40MHZ frames? If so then pick the desired txbw */
		if (CHSPEC_IS40(wlc->chanspec)) {
			/* default txbw is 20in40 SB */
			mimo_txbw = WL_RSPEC_BW_20MHZ;

			if (RSPEC_BW(rspec) != WL_RSPEC_BW_UNSPECIFIED) {
				/* If the ratespec override has a bw greater than
				 * 20MHz, specify th txbw value here.
				 * Otherwise, the default setup above is already 20MHz.
				 */
				if (!RSPEC_IS20MHZ(rspec)) {
					/* rspec bw > 20MHz is not allowed for CCK/DSSS
					 * so we can only have OFDM or HT here
					 * mcs 32 and legacy OFDM must be 40b/w DUP, other HT
					 * is just plain 40MHz
					 */
					mimo_txbw = RSPEC_BW(rspec);
				}
			} else if (RSPEC_ISHT(rspec)) {
				if ((scb->flags & SCB_IS40) &&
#ifdef WLMCHAN
					 /* if mchan enabled and bsscfg is AP, then must
					  * check the bsscfg chanspec to make sure our AP
					  * is operating on 40MHz channel.
					  */
					 (!MCHAN_ENAB(wlc->pub) || !BSSCFG_AP(bsscfg) ||
					  CHSPEC_IS40(bsscfg->current_bss->chanspec)) &&
#endif /* WLMCHAN */
					 TRUE) {
					mimo_txbw = WL_RSPEC_BW_40MHZ;
				}
			}
#if WL_HT_TXBW_OVERRIDE_ENAB
			WL_HT_TXBW_OVERRIDE_IDX(hti, rspec, txbw_override_idx);

			if (txbw_override_idx >= 0) {
				mimo_txbw = txbw2rspecbw[txbw_override_idx];
				phyctl_bwo = txbw2phyctl0bw[txbw_override_idx];
			}
#endif /* WL_HT_TXBW_OVERRIDE_ENAB */
			/* mcs 32 must be 40b/w DUP */
			if (RSPEC_ISHT(rspec) && ((rspec & WL_RSPEC_HT_MCS_MASK) == 32))
				mimo_txbw = WL_RSPEC_BW_40MHZ;
		} else  {
			/* mcs32 is 40 b/w only.
			 * This is possible for probe packets on a STA during SCAN
			 */
			if ((rspec & WL_RSPEC_HT_MCS_MASK) == 32) {
				WL_INFORM(("wl%d: wlc_d11hdrs mcs 32 invalid in 20MHz mode, using"
					"mcs 0 instead\n", wlc->pub->unit));

				rspec = HT_RSPEC(0);	/* mcs 0 */
			}

			/* Fix fallback as well */
			if ((rspec_fallback & WL_RSPEC_HT_MCS_MASK) == 32)
				rspec_fallback = HT_RSPEC(0);	/* mcs 0 */

			mimo_txbw = WL_RSPEC_BW_20MHZ;
		}

		rspec &= ~WL_RSPEC_BW_MASK;
		rspec |= mimo_txbw;

		rspec_fallback &= ~WL_RSPEC_BW_MASK;
		if (RSPEC_ISHT(rspec_fallback))
			rspec_fallback |= mimo_txbw;
		else
			rspec_fallback |= WL_RSPEC_BW_20MHZ;

		if (!RSPEC_ACTIVE(wlc->band->rspec_override)) {
			sgi_tx = WLC_HT_GET_SGI_TX(hti);
			if (sgi_tx == OFF) {
				rspec &= ~WL_RSPEC_SGI;
				rspec_fallback &= ~WL_RSPEC_SGI;
			}
			if (RSPEC_ISHT(rspec) && (sgi_tx == ON))
				rspec |= WL_RSPEC_SGI;

			if (RSPEC_ISHT(rspec_fallback) && (sgi_tx == ON))
				rspec_fallback |= WL_RSPEC_SGI;

			ASSERT(!(rspec & WL_RSPEC_LDPC));
			ASSERT(!(rspec_fallback & WL_RSPEC_LDPC));
			rspec &= ~WL_RSPEC_LDPC;
			rspec_fallback &= ~WL_RSPEC_LDPC;
			if (wlc->stf->ldpc_tx == ON ||
			    (SCB_HT_LDPC_CAP(scb) && wlc->stf->ldpc_tx == AUTO)) {
				if (RSPEC_ISHT(rspec))
					rspec |= WL_RSPEC_LDPC;

				if (RSPEC_ISHT(rspec_fallback) ||
				    !IS_PROPRIETARY_11N_MCS(rspec_fallback & WL_RSPEC_HT_MCS_MASK))
					rspec_fallback |= WL_RSPEC_LDPC;
			}
		}

		mimo_preamble_type = wlc_prot_n_preamble(wlc, scb);
		if (RSPEC_ISHT(rspec)) {
			preamble_type = mimo_preamble_type;
			if (n_prot == WLC_N_PROTECTION_20IN40 &&
			    RSPEC_IS40MHZ(rspec))
				use_cts = TRUE;

			/* if the mcs is multi stream check if it needs an rts */
			if (!IS_SINGLE_STREAM(rspec & WL_RSPEC_HT_MCS_MASK)) {
				if (WLC_STF_GET_SCB_MIMOPS_RTS_ENAB(wlc->stf, scb)) {
					use_rts = use_mimops_rts = TRUE;
				}
			}

			/* if SGI is selected, then forced mm for single stream */
			if ((rspec & WL_RSPEC_SGI) &&
			    IS_SINGLE_STREAM(rspec & WL_RSPEC_HT_MCS_MASK)) {
				preamble_type = WLC_MM_PREAMBLE;
			}
		}

		if (RSPEC_ISHT(rspec_fallback)) {
			fbr_preamble_type = mimo_preamble_type;

			/* if SGI is selected, then forced mm for single stream */
			if ((rspec_fallback & WL_RSPEC_SGI) &&
			    IS_SINGLE_STREAM(rspec_fallback & WL_RSPEC_HT_MCS_MASK))
				fbr_preamble_type = WLC_MM_PREAMBLE;
		}
	} else {
		/* Set ctrlchbw as 20Mhz */
		ASSERT(!RSPEC_ISHT(rspec));
		ASSERT(!RSPEC_ISHT(rspec_fallback));
		rspec &= ~WL_RSPEC_BW_MASK;
		rspec_fallback &= ~WL_RSPEC_BW_MASK;
		rspec |= WL_RSPEC_BW_20MHZ;
		rspec_fallback |= WL_RSPEC_BW_20MHZ;

		/* for nphy, stf of ofdm frames must follow policies */
		if (WLCISNPHY(wlc->band) && RSPEC_ISOFDM(rspec)) {
			rspec &= ~WL_RSPEC_TXEXP_MASK;
			if (phyctl1_stf == PHY_TXC1_MODE_CDD) {
				rspec |= (1 << WL_RSPEC_TXEXP_SHIFT);
			}
		}
		if (WLCISNPHY(wlc->band) && RSPEC_ISOFDM(rspec_fallback)) {
			rspec_fallback &= ~WL_RSPEC_TXEXP_MASK;
			if (phyctl1_stf == PHY_TXC1_MODE_CDD) {
				rspec_fallback |= (1 << WL_RSPEC_TXEXP_SHIFT);
			}
		}
	}

	/* mimo bw field MUST now be valid in the rspec (it affects duration calculations) */

	/* (2) PROTECTION, may change rspec */
	if (((type == FC_TYPE_DATA) || (type == FC_TYPE_MNG)) &&
	    ((phylen > rtsthresh) || (pkttag->flags & WLF_USERTS)) &&
	    !ETHER_ISMULTI(&h->a1))
		use_rts = TRUE;

	if ((wlc->band->gmode && g_prot && RSPEC_ISOFDM(rspec)) ||
	    (N_ENAB(wlc->pub) && RSPEC_ISHT(rspec) && n_prot)) {
		if (nfrags > 1) {
			/* For a frag burst use the lower modulation rates for the entire frag burst
			 * instead of protection mechanisms.
			 * As per spec, if protection mechanism is being used, a fragment sequence
			 * may only employ ERP-OFDM modulation for the final fragment and control
			 * response. (802.11g Sec 9.10) For ease we send the *whole* sequence at the
			 * lower modulation instead of using a higher modulation for the last frag.
			 */

			/* downgrade the rate to CCK or OFDM */
			if (g_prot) {
				/* Use 11 Mbps as the rate and fallback. We should make sure that if
				 * we are downgrading an OFDM rate to CCK, we should pick a more
				 * robust rate.  6 and 9 Mbps are not usually selected by rate
				 * selection, but even if the OFDM rate we are downgrading is 6 or 9
				 * Mbps, 11 Mbps is more robust.
				 */
				rspec = rspec_fallback = CCK_RSPEC(WLC_RATE_FRAG_G_PROTECT);
			} else {
				/* Use 24 Mbps as the rate and fallback for what would have been
				 * a MIMO rate. 24 Mbps is the highest phy mandatory rate for OFDM.
				 */
				rspec = rspec_fallback = OFDM_RSPEC(WLC_RATE_FRAG_N_PROTECT);
			}
			pkttag->flags &= ~WLF_RATE_AUTO;
		} else {
			/* Use protection mechanisms on unfragmented frames */
			/* If this is a 11g protection, then use CTS-to-self */
			if (wlc->band->gmode && g_prot && !RSPEC_ISCCK(rspec))
				use_cts = TRUE;
		}
	}

	/* calculate minimum rate */
	ASSERT(RSPEC2KBPS(rspec_fallback) <= RSPEC2KBPS(rspec));
	ASSERT(VALID_RATE_DBG(wlc, rspec));
	ASSERT(VALID_RATE_DBG(wlc, rspec_fallback));

	/* fix the preamble type for non-MCS rspec/rspec-fallback
	 * If SCB is short preamble capable and and shortpreamble is enabled
	 * and rate is NOT 1M or Override is NOT set to LONG preamble then use SHORT preamble
	 * else use LONG preamble
	 * OFDM should bypass below preamble set, but old chips are OK since they ignore that bit
	 */
	if (RSPEC_ISCCK(rspec)) {
		if (short_preamble &&
		    !((RSPEC2RATE(rspec) == WLC_RATE_1M) ||
		      (scb->bsscfg->PLCPHdr_override == WLC_PLCP_LONG)))
			preamble_type = WLC_SHORT_PREAMBLE;
		else
			preamble_type = WLC_LONG_PREAMBLE;
	}

	if (RSPEC_ISCCK(rspec_fallback)) {
		if (short_preamble &&
		    !((RSPEC2RATE(rspec_fallback) == WLC_RATE_1M) ||
		      (scb->bsscfg->PLCPHdr_override == WLC_PLCP_LONG)))
			fbr_preamble_type = WLC_SHORT_PREAMBLE;
		else
			fbr_preamble_type = WLC_LONG_PREAMBLE;
	}

	ASSERT(!RSPEC_ISHT(rspec) || WLC_IS_MIMO_PREAMBLE(preamble_type));
	ASSERT(!RSPEC_ISHT(rspec_fallback) || WLC_IS_MIMO_PREAMBLE(fbr_preamble_type));

	WLCNTSCB_COND_SET(((type == FC_TYPE_DATA) &&
		((FC_SUBTYPE(fc) != FC_SUBTYPE_NULL) &&
			(FC_SUBTYPE(fc) != FC_SUBTYPE_QOS_NULL))),
			scb->scb_stats.tx_rate, rspec);

#ifdef WLC_MEDIAN_RATE_REPORT
	if (!ETHER_ISMULTI(&h->a1)) {
		WLCNTSCB_COND_SET(((type == FC_TYPE_DATA) &&
			((FC_SUBTYPE(fc) != FC_SUBTYPE_NULL) &&
				(FC_SUBTYPE(fc) != FC_SUBTYPE_QOS_NULL))),
				scb->scb_rate_stats.tx_rate_log[WLCNT_SCB_TX_RATE_INDEX(scb)],
				rspec);
		WLCNTSCB_COND_INCR_LIMIT(((type == FC_TYPE_DATA) &&
			((FC_SUBTYPE(fc) != FC_SUBTYPE_NULL) &&
				(FC_SUBTYPE(fc) != FC_SUBTYPE_QOS_NULL))),
				scb->scb_rate_stats.tx_rate_index, (NRATELOG-1));
	}
#endif /* WLC_MEDIAN_RATE_REPORT */

	/* record rate history after the txbw is valid */
	if (rspec_history) {
		/* update per bsscfg tx rate */
		bsscfg->txrspec[bsscfg->txrspecidx][0] = rspec;
		bsscfg->txrspec[bsscfg->txrspecidx][1] = (uint8) nfrags;
		bsscfg->txrspecidx = (bsscfg->txrspecidx+1) % NTXRATE;

		WLCNTSCBSET(scb->scb_stats.tx_rate_fallback, rspec_fallback);
	}
	WLPKTTAG(p)->rspec = rspec;

	/* RIFS(testing only): based on frameburst, non-CCK frames only */
	if (SCB_HT_CAP(scb) &&
		(fifo < TX_BCMC_FIFO) &&
		WLC_HT_GET_FRAMEBURST(hti) &&
		WLC_HT_GET_FRAMEBURST_PER_AC(hti, ac) &&
		WLC_HT_GET_RIFS(hti) &&
		n_prot != WLC_N_PROTECTION_MIXEDMODE &&
		!RSPEC_ISCCK(rspec) &&
		!ETHER_ISMULTI(&h->a1) &&
		((fc & FC_KIND_MASK) == FC_QOS_DATA)) {
		uint16 qos_field, *pqos;

		WLPKTTAG(p)->flags |= WLF_RIFS;
		mcl |= (TXC_FRAMEBURST | TXC_USERIFS);
		use_rifs = TRUE;

		/* RIFS implies QoS frame with no-ack policy, hack the QoS field */
		pqos = (uint16 *)((uchar *)h + (a4 ?
			DOT11_A4_HDR_LEN : DOT11_A3_HDR_LEN));
		qos_field = ltoh16(*pqos) & ~QOS_ACK_MASK;
		qos_field |= (QOS_ACK_NO_ACK << QOS_ACK_SHIFT) & QOS_ACK_MASK;
		*pqos = htol16(qos_field);
	}

	/* (3) PLCP: determine PLCP header and MAC duration, fill d11txh_t */
	wlc_compute_plcp(wlc, bsscfg, rspec, phylen, fc, plcp);
	wlc_compute_plcp(wlc, bsscfg, rspec_fallback, phylen, fc, plcp_fallback);
	bcopy(plcp_fallback, (char*)&txh->FragPLCPFallback, sizeof(txh->FragPLCPFallback));

	/* Length field now put in CCK FBR CRC field for AES */
	if (RSPEC_ISCCK(rspec_fallback)) {
		txh->FragPLCPFallback[4] = phylen & 0xff;
		txh->FragPLCPFallback[5] = (phylen & 0xff00) >> 8;
	}

	/* mark pkt as aggregable if it is */
	if (WLPKTFLAG_AMPDU(pkttag) && RSPEC_ISHT(rspec)) {
		if (WLC_KEY_ALLOWS_AMPDU(key_info)) {
			WLPKTTAG(p)->flags |= WLF_MIMO;
			if (WLC_HT_GET_AMPDU_RTS(wlc->hti))
				use_rts = TRUE;
		}
	}

	/* MIMO-RATE: need validation ?? */
	mainrates = RSPEC_ISOFDM(rspec) ? D11A_PHY_HDR_GRATE((ofdm_phy_hdr_t *)plcp) : plcp[0];

	/* DUR field for main rate */
	if (((fc & FC_KIND_MASK) != FC_PS_POLL) && !ETHER_ISMULTI(&h->a1) && !use_rifs) {
		if (WLPKTFLAG_AMPDU(pkttag))
			durid = wlc_compute_ampdu_mpdu_dur(wlc,
				CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec), rspec);
		else
			durid = wlc_compute_frame_dur(wlc,
				CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
				rspec, preamble_type, next_frag_len);
		h->durid = htol16(durid);
	} else if (use_rifs) {
		/* NAV protect to end of next max packet size */
		durid = (uint16)wlc_calc_frame_time(wlc, rspec, preamble_type, DOT11_MAX_FRAG_LEN);
		durid += RIFS_11N_TIME;
		h->durid = htol16(durid);
	}

	/* DUR field for fallback rate */
	if (fc == FC_PS_POLL)
		txh->FragDurFallback = h->durid;
	else if (ETHER_ISMULTI(&h->a1) || use_rifs)
		txh->FragDurFallback = 0;
	else {
		if (WLPKTFLAG_AMPDU(pkttag))
			durid = wlc_compute_ampdu_mpdu_dur(wlc,
				CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec), rspec_fallback);
		else
			durid = wlc_compute_frame_dur(wlc,
				CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
				rspec_fallback, fbr_preamble_type, next_frag_len);
		txh->FragDurFallback = htol16(durid);
	}

	/* Timestamp */
	if ((pkttag->flags & WLF_EXPTIME)) {
		txh->TstampLow = htol16(pkttag->u.exptime & 0xffff);
		txh->TstampHigh = htol16((pkttag->u.exptime >> 16) & 0xffff);

		mcl |= TXC_LIFETIME;	/* Enable timestamp for the packet */
	}

	/* (4) MAC-HDR: MacTxControlLow */
	if (frag == 0)
		mcl |= TXC_STARTMSDU;

	if (!ETHER_ISMULTI(&h->a1) && !WLPKTFLAG_RIFS(pkttag) &&
	    !(wlc->_amsdu_noack && WLPKTFLAG_AMSDU(pkttag)) &&
	    !(!WLPKTFLAG_AMPDU(pkttag) && qos && wme->wme_noack))
		mcl |= TXC_IMMEDACK;

	if (type == FC_TYPE_DATA && (fifo < TX_BCMC_FIFO)) {
		uint rate = RSPEC2KBPS(rspec) / 500;

		/* don't allow bursting if rts is required around each mimo frame */
		if (WLC_HT_GET_FRAMEBURST(hti) && (rate > WLC_FRAMEBURST_MIN_RATE) &&
			WLC_HT_GET_FRAMEBURST_PER_AC(hti, ac) &&
			(!wlc->active_bidir) &&
			(wlc_ampdu_frameburst_override(wlc->ampdu_tx, scb) == FALSE) &&
			(!wme->edcf_txop[ac] || WLPKTFLAG_AMPDU(pkttag)) &&
			(use_mimops_rts == FALSE)) {

			mcl |= TXC_FRAMEBURST;
		}
	}

	if (BAND_5G(wlc->band->bandtype))
		mcl |= TXC_FREQBAND_5G; /* there is no need for a TXC_FREQBAND_6G */

	if (CHSPEC_IS40(phy_utils_get_chanspec(WLC_PI(wlc))))
		mcl |= TXC_BW_40;

	txh->MacTxControlLow = htol16(mcl);

	/* MacTxControlHigh */
	mch = 0;

	/* Set fallback rate preamble type */
	if ((fbr_preamble_type == WLC_SHORT_PREAMBLE) ||
	    (fbr_preamble_type == WLC_GF_PREAMBLE)) {
		ASSERT((fbr_preamble_type == WLC_GF_PREAMBLE) ||
		       (!RSPEC_ISHT(rspec_fallback)));
		mch |= TXC_PREAMBLE_DATA_FB_SHORT;
	}

	/* MacFrameControl */
	bcopy((char*)&h->fc, (char*)&txh->MacFrameControl, sizeof(uint16));

	txh->TxFesTimeNormal = htol16(0);

	txh->TxFesTimeFallback = htol16(0);

	/* TxFrameRA */
	bcopy((char*)&h->a1, (char*)&txh->TxFrameRA, ETHER_ADDR_LEN);

	/* TxFrameID */
	txh->TxFrameID = htol16(frameid);

#ifdef WLMCNX
	if (MCNX_ENAB(wlc->pub)) {
		uint16 bss = (uint16)wlc_mcnx_BSS_idx(wlc->mcnx, bsscfg);
		ASSERT(bss < M_P2P_BSS_MAX);
		txh->ABI_MimoAntSel |= htol16(bss << ABI_MAS_ADDR_BMP_IDX_SHIFT);
	}
#endif

	/* TxStatus, Note the case of recreating the first frag of a suppressed frame
	 * then we may need to reset the retry cnt's via the status reg
	 */
	txh->TxStatus = htol16(status);

	if (D11REV_GE(wlc->pub->corerev, 16)) {
		/* extra fields for ucode AMPDU aggregation, the new fields are added to
		 * the END of previous structure so that it's compatible in driver.
		 * In old rev ucode, these fields should be ignored
		 */
		txh->MaxNMpdus = htol16(0);
		txh->u1.MaxAggDur = htol16(0);
		txh->u2.MaxAggLen_FBR = htol16(0);
		txh->MinMBytes = htol16(0);
	}

	/* (5) RTS/CTS: determine RTS/CTS PLCP header and MAC duration, furnish d11txh_t */

	/* RTS PLCP header and RTS frame */
	if (use_rts || use_cts) {
		if (use_rts && use_cts)
			use_cts = FALSE;

#ifdef NOT_YET
		/* XXX Optimization: if use rts-cts or cts-to-self,
		 * mixedmode preamble may not be necessary if the dest
		 * support GF. Clear hdr_mcs_mixedmode and reset
		 * short_preamble, recalculating duration fields are
		 * required.
		 */
		if (SCB_ISGF_CAP(scb)) {
			hdr_mcs_mixedmode = FALSE;
			hdr_cckofdm_shortpreamble = TRUE;
		}
#endif /* NOT_YET */

		rts_rspec = wlc_rspec_to_rts_rspec(bsscfg, rspec, FALSE);
		rts_rspec_fallback = wlc_rspec_to_rts_rspec(bsscfg, rspec_fallback, FALSE);

		/* XXX
		 * OFDM should bypass below preamble set, but old chips are OK since they ignore
		 * that bit. Before 4331A0 PR79339 is fixed, restrict to CCK, which is actually
		 * correct
		 */
		if (!RSPEC_ISOFDM(rts_rspec) &&
		    !((RSPEC2RATE(rts_rspec) == WLC_RATE_1M) ||
		      (scb->bsscfg->PLCPHdr_override == WLC_PLCP_LONG))) {
			rts_preamble_type = WLC_SHORT_PREAMBLE;
			mch |= TXC_PREAMBLE_RTS_MAIN_SHORT;
		}

		if (!RSPEC_ISOFDM(rts_rspec_fallback) &&
		    !((RSPEC2RATE(rts_rspec_fallback) == WLC_RATE_1M) ||
		      (scb->bsscfg->PLCPHdr_override == WLC_PLCP_LONG))) {
			rts_fbr_preamble_type = WLC_SHORT_PREAMBLE;
			mch |= TXC_PREAMBLE_RTS_FB_SHORT;
		}

		/* RTS/CTS additions to MacTxControlLow */
		if (use_cts) {
			txh->MacTxControlLow |= htol16(TXC_SENDCTS);
		} else {
			txh->MacTxControlLow |= htol16(TXC_SENDRTS);
			txh->MacTxControlLow |= htol16(TXC_LONGFRAME);
		}

		/* RTS PLCP header */
		ASSERT(ISALIGNED(txh->RTSPhyHeader, sizeof(uint16)));
		rts_plcp = txh->RTSPhyHeader;
		if (use_cts)
			rts_phylen = DOT11_CTS_LEN + DOT11_FCS_LEN;
		else
			rts_phylen = DOT11_RTS_LEN + DOT11_FCS_LEN;

		/* dot11n headers */
		wlc_compute_plcp(wlc, bsscfg, rts_rspec, rts_phylen, fc, rts_plcp);

		/* fallback rate version of RTS PLCP header */
		wlc_compute_plcp(wlc, bsscfg, rts_rspec_fallback, rts_phylen,
			fc, rts_plcp_fallback);

		bcopy(rts_plcp_fallback, (char*)&txh->RTSPLCPFallback,
			sizeof(txh->RTSPLCPFallback));

		/* RTS frame fields... */
		rts = (struct dot11_rts_frame*)&txh->rts_frame;

		durid = wlc_compute_rtscts_dur(wlc, CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
			use_cts, rts_rspec, rspec, rts_preamble_type, preamble_type, phylen, FALSE);
		rts->durid = htol16(durid);

		/* fallback rate version of RTS DUR field */
		durid = wlc_compute_rtscts_dur(wlc, CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
			use_cts, rts_rspec_fallback, rspec_fallback, rts_fbr_preamble_type,
		        fbr_preamble_type, phylen, FALSE);
		txh->RTSDurFallback = htol16(durid);

		if (use_cts) {
			rts->fc = htol16(FC_CTS);
			bcopy((char*)&h->a2, (char*)&rts->ra, ETHER_ADDR_LEN);
		} else {
			rts->fc = htol16((uint16)FC_RTS);
			bcopy((char*)&h->a1, (char*)&rts->ra, ETHER_ADDR_LEN);
			bcopy((char*)&h->a2, (char*)&rts->ta, ETHER_ADDR_LEN);
		}

		/* mainrate
		 *    low 8 bits: main frag rate/mcs,
		 *    high 8 bits: rts/cts rate/mcs
		 */
		mainrates |= (RSPEC_ISOFDM(rts_rspec) ?
			D11A_PHY_HDR_GRATE((ofdm_phy_hdr_t *)rts_plcp) : rts_plcp[0]) << 8;
	} else {
		bzero((char*)txh->RTSPhyHeader, D11_PHY_HDR_LEN);
		bzero((char*)&txh->rts_frame, sizeof(struct dot11_rts_frame));
		bzero((char*)txh->RTSPLCPFallback, sizeof(txh->RTSPLCPFallback));
		txh->RTSDurFallback = 0;
	}

	/* add null delimiter count */
	if (WLPKTFLAG_AMPDU(pkttag) && RSPEC_ISHT(rspec)) {
		uint16 minbytes = 0;
		txh->RTSPLCPFallback[AMPDU_FBR_NULL_DELIM] =
			wlc_ampdu_null_delim_cnt(wlc->ampdu_tx, scb, rspec, phylen, &minbytes);
	}

	/* Now that RTS/RTS FB preamble types are updated, write the final value */
	txh->MacTxControlHigh = htol16(mch);

	/* MainRates (both the rts and frag plcp rates have been calculated now) */
	txh->MainRates = htol16(mainrates);

	/* XtraFrameTypes */
	xfts = WLC_RSPEC_ENC2FT(rspec_fallback);
	xfts |= (WLC_RSPEC_ENC2FT(rts_rspec) << XFTS_RTS_FT_SHIFT);
	xfts |= (WLC_RSPEC_ENC2FT(rts_rspec_fallback) << XFTS_FBRRTS_FT_SHIFT);
	xfts |= CHSPEC_CHANNEL(phy_utils_get_chanspec(WLC_PI(wlc))) << XFTS_CHANNEL_SHIFT;
	txh->XtraFrameTypes = htol16(xfts);

	/* PhyTxControlWord */
	phyctl = WLC_RSPEC_ENC2FT(rspec);
	if ((preamble_type == WLC_SHORT_PREAMBLE) ||
	    (preamble_type == WLC_GF_PREAMBLE)) {
		ASSERT((preamble_type == WLC_GF_PREAMBLE) || !RSPEC_ISHT(rspec));
		phyctl |= PHY_TXC_SHORT_HDR;
		WLCNTINCR(wlc->pub->_cnt->txprshort);
	}

	/* phytxant is properly bit shifted */
	phyctl |= wlc_stf_d11hdrs_phyctl_txant(wlc, rspec);
	if (WLCISNPHY(wlc->band) && (wlc->pub->sromrev >= 9)) {
		uint8 rate_offset;
		uint16 minbytes;
		rate_offset = wlc_stf_get_pwrperrate(wlc, rspec, 0);
		phyctl |= (rate_offset <<  PHY_TXC_PWR_SHIFT);
		rate_offset = wlc_stf_get_pwrperrate(wlc, rspec_fallback, 0);
		minbytes = ltoh16(txh->MinMBytes);
		minbytes |= (rate_offset << MINMBYTES_FBRATE_PWROFFSET_SHIFT);
		txh->MinMBytes = htol16(minbytes);
	}

#ifdef WL_LPC
	if ((pkttag->flags & WLF_RATE_AUTO) && LPC_ENAB(wlc)) {
			uint16 ratepwr_offset = 0;

			/* Note the per rate power offset for this rate */
			ratepwr_offset = wlc_phy_lpc_get_txcpwrval(WLC_PI(wlc), phyctl);

			/* Update the Power offset bits */
			/* lpc_offset = lpc_offset + ratepwr_offset;
			*  XXX Investigate how to combine offsets
			*/
			if (ratepwr_offset) {
				/* for the LPC enabled 11n chips ratepwr_offset should be 0 */
				ASSERT(FALSE);
			}
			wlc_phy_lpc_set_txcpwrval(WLC_PI(wlc), &phyctl, lpc_offset);
	}
#endif /* WL_LPC */
	txh->PhyTxControlWord = htol16(phyctl);

	/* PhyTxControlWord_1 */
	{
		uint16 phyctl1 = 0;

		phyctl1 = wlc_phytxctl1_calc(wlc, rspec, wlc->chanspec);
		WLC_PHYCTLBW_OVERRIDE(phyctl1, PHY_TXC1_BW_MASK, phyctl_bwo);
		txh->PhyTxControlWord_1 = htol16(phyctl1);

		phyctl1 = wlc_phytxctl1_calc(wlc, rspec_fallback, wlc->chanspec);
		if (RSPEC_ISHT(rspec_fallback)) {
			WLC_PHYCTLBW_OVERRIDE(phyctl1, PHY_TXC1_BW_MASK, phyctl_bwo);
		}
		txh->PhyTxControlWord_1_Fbr = htol16(phyctl1);

		if (use_rts || use_cts) {
			phyctl1 = wlc_phytxctl1_calc(wlc, rts_rspec, wlc->chanspec);
			txh->PhyTxControlWord_1_Rts = htol16(phyctl1);
			phyctl1 = wlc_phytxctl1_calc(wlc, rts_rspec_fallback, wlc->chanspec);
			txh->PhyTxControlWord_1_FbrRts = htol16(phyctl1);
		}

		/*
		 * For mcs frames, if mixedmode(overloaded with long preamble) is going to be set,
		 * fill in non-zero MModeLen and/or MModeFbrLen
		 *  it will be unnecessary if they are separated
		 */
		if (RSPEC_ISHT(rspec) && (preamble_type == WLC_MM_PREAMBLE)) {
			uint16 mmodelen = wlc_calc_lsig_len(wlc, rspec, phylen);
			txh->MModeLen = htol16(mmodelen);
		}

		if (RSPEC_ISHT(rspec_fallback) && (fbr_preamble_type == WLC_MM_PREAMBLE)) {
			uint16 mmodefbrlen = wlc_calc_lsig_len(wlc, rspec_fallback, phylen);
			txh->MModeFbrLen = htol16(mmodefbrlen);
		}
	}

	/* XXX Make sure that either rate is NOT MCS or if preamble type is mixedmode
	 * then length is not 0
	 */
	ASSERT(!RSPEC_ISHT(rspec) ||
	       ((preamble_type == WLC_MM_PREAMBLE) == (txh->MModeLen != 0)));
	ASSERT(!RSPEC_ISHT(rspec_fallback) ||
	       ((fbr_preamble_type == WLC_MM_PREAMBLE) == (txh->MModeFbrLen != 0)));

	if (SCB_WME(scb) && qos && wme->edcf_txop[ac]) {
		uint frag_dur, dur, dur_fallback;

		ASSERT(!ETHER_ISMULTI(&h->a1));

		/* WME: Update TXOP threshold */
		/*
		 * XXX should this calculation be done for null frames also?
		 * What else is wrong with these calculations?
		 */
		if ((!WLPKTFLAG_AMPDU(pkttag)) && (frag == 0)) {
			int16 delta;

			frag_dur = wlc_calc_frame_time(wlc, rspec, preamble_type, phylen);

			if (rts) {
				/* 1 RTS or CTS-to-self frame */
				dur = wlc_calc_cts_time(wlc, rts_rspec, rts_preamble_type);
				dur_fallback = wlc_calc_cts_time(wlc, rts_rspec_fallback,
				                                 rts_fbr_preamble_type);
				/* (SIFS + CTS) + SIFS + frame + SIFS + ACK */
				dur += ltoh16(rts->durid);
				dur_fallback += ltoh16(txh->RTSDurFallback);
			} else if (use_rifs) {
				dur = frag_dur;
				dur_fallback = 0;
			} else {
				/* frame + SIFS + ACK */
				dur = frag_dur;
				dur += wlc_compute_frame_dur(wlc,
					CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
					rspec, preamble_type, 0);

				dur_fallback = wlc_calc_frame_time(wlc, rspec_fallback,
				               fbr_preamble_type, phylen);
				dur_fallback += wlc_compute_frame_dur(wlc,
						CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
						rspec_fallback, fbr_preamble_type, 0);
			}
			/* NEED to set TxFesTimeNormal (hard) */
			txh->TxFesTimeNormal = htol16((uint16)dur);
			/* NEED to set fallback rate version of TxFesTimeNormal (hard) */
			txh->TxFesTimeFallback = htol16((uint16)dur_fallback);

			ASSERT(fifo < TX_FIFO_HE_MU_START); /* no MU FIFOs for pre-40 */

			/* update txop byte threshold (txop minus intraframe overhead) */
			delta = (int16)(wme->edcf_txop[ac] - (dur - frag_dur));
			if (delta >= 0) {
#ifdef WLAMSDU_TX
				if (!(AMSDU_TX_ENAB(wlc->pub) &&
				    WLPKTFLAG_AMSDU(pkttag) && (fifo == TX_AC_BE_FIFO)))
#endif
				{

					/* XXX radar 19423513 MACOS sends EAPOL pkt in
					* VO queueand since TXOP is applied for VO pkts
					* causing it to be fragmented and to avoid it this
					* check is added
					*/
					if (!(pkttag->flags & WLF_8021X)) {
						uint newfragthresh =
							wlc_calc_frame_len(wlc, rspec,
								preamble_type, (uint16)delta);
						/* range bound the fragthreshold */
						newfragthresh = MAX(newfragthresh,
							DOT11_MIN_FRAG_LEN);
						newfragthresh = MIN(newfragthresh,
							wlc->usr_fragthresh);
						/* update the fragthresh and do txc update */
						if (wlc->fragthresh[ac] != (uint16)newfragthresh)
						{
							wlc_fragthresh_set(wlc,
								ac, (uint16)newfragthresh);
						}
					}
				}
			} else
				WL_ERROR(("wl%d: %s txop invalid for rate %d\n",
				          wlc->pub->unit, wlc_fifo_names(fifo),
				          RSPEC2KBPS(rspec)));

			if (CAC_ENAB(wlc->pub) &&
				fifo <= TX_AC_VO_FIFO) {
				/* update cac used time */
				if (wlc_cac_update_used_time(wlc->cac, ac, dur, scb))
					WL_ERROR(("wl%d: ac %d: txop exceeded allocated TS time\n",
						wlc->pub->unit, ac));
			}

			/*
			 * FIXME: The MPDUs of the next transmitted MSDU after
			 * rate drops or RTS/CTS kicks in may exceed
			 * TXOP. Without tearing apart the transmit
			 * path---either push rate and RTS/CTS decisions much
			 * earlier (hard), allocate fragments just in time
			 * (harder), or support late refragmentation (even
			 * harder)---it's too difficult to fix this now.
			 */
			if (dur > wme->edcf_txop[ac])
				WL_INFORM(("wl%d: %s txop exceeded phylen %d/%d dur %d/%d\n",
					wlc->pub->unit, wlc_fifo_names(fifo), phylen,
					wlc->fragthresh[ac], dur, wme->edcf_txop[ac]));
		}
	} else if (SCB_WME(scb) && qos && CAC_ENAB(wlc->pub) && fifo <= TX_AC_VO_FIFO) {
		uint dur;
		if (rts) {
			/* 1 RTS or CTS-to-self frame */
			dur = wlc_calc_cts_time(wlc, rts_rspec, rts_preamble_type);
			/* (SIFS + CTS) + SIFS + frame + SIFS + ACK */
			dur += ltoh16(rts->durid);
		} else {
			/* frame + SIFS + ACK */
			dur = wlc_calc_frame_time(wlc, rspec, preamble_type, phylen);
			dur += wlc_compute_frame_dur(wlc,
				CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
				rspec, preamble_type, 0);
		}

		/* update cac used time */
		if (wlc_cac_update_used_time(wlc->cac, ac, dur, scb))
			WL_ERROR(("wl%d: ac %d: txop exceeded allocated TS time\n",
				wlc->pub->unit, ac));
	}

	/* With d11 hdrs on, mark the packet as TXHDR */
	WLPKTTAG(p)->flags |= WLF_TXHDR;

	/* TDLS U-APSD buffer STA: if peer is in PS, save the last MPDU's seq and tid for PTI */
#ifdef WLTDLS
	if (BSS_TDLS_ENAB(wlc, SCB_BSSCFG(scb)) &&
		SCB_PS(scb) &&
		wlc_tdls_buffer_sta_enable(wlc->tdls)) {
		uint8 tid = 0;
		if (qos) {
			uint16 qc;
			qc = (*(uint16 *)((uchar *)h + (a4 ?
				DOT11_A4_HDR_LEN : DOT11_A3_HDR_LEN)));
			tid = (QOS_TID(qc) & 0x0f);
		}
		wlc_tdls_update_tid_seq(wlc->tdls, scb, tid, seq);
	}
#endif

	if ((key_info) && (key_info->algo != CRYPTO_ALGO_OFF)) {
		hdrptr = (d11txhdr_t *)PKTDATA(wlc->osh, p);
		PKTPULL(wlc->osh, p, D11_TXH_LEN + D11_PHY_HDR_LEN);
		if (wlc_key_prep_tx_mpdu(key, p, hdrptr) != BCME_OK) {
			WL_WSEC(("wl%d: %s: wlc_key_prep_tx_mpdu error\n",
				wlc->pub->unit, __FUNCTION__));
		}
		PKTPUSH(wlc->osh, p, D11_TXH_LEN + D11_PHY_HDR_LEN);
	}

	return (frameid);
} /* wlc_d11hdrs_pre40 */

/** At this point the rspec may not include a valid txbw. Pick a transmit bandwidth. */
static INLINE ratespec_t
wlc_d11hdrs_rev40_determine_mimo_txbw(wlc_info_t *wlc, struct scb *scb,
	wlc_bsscfg_t *bsscfg, ratespec_t rspec)
{
	uint32 mimo_txbw;

	/* In rev128, we use global SCB for global rate entry and therefor cannot use
	 * per-SCB capabilities to determine support for 160/80p80 BW for rate overrides
	 */
	bool rspec_override = (D11REV_GE(wlc->pub->corerev, 128) &&
		RSPEC_ACTIVE(wlc->band->rspec_override)) ? TRUE : FALSE;

	if (RSPEC_BW(rspec) != WL_RSPEC_BW_UNSPECIFIED) {
		mimo_txbw = RSPEC_BW(rspec);

		/* If the ratespec override has a bw greater than
		 * the channel bandwidth, limit here.
		 */
		if (CHSPEC_IS20(wlc->chanspec)) {
			mimo_txbw = WL_RSPEC_BW_20MHZ;
		} else if (CHSPEC_IS40(wlc->chanspec)) {
			mimo_txbw = MIN(mimo_txbw, WL_RSPEC_BW_40MHZ);
		} else if (CHSPEC_IS80(wlc->chanspec)) {
			mimo_txbw = MIN(mimo_txbw, WL_RSPEC_BW_80MHZ);
		} else if (CHSPEC_IS160(wlc->chanspec) || CHSPEC_IS8080(wlc->chanspec)) {
			mimo_txbw = MIN(mimo_txbw, WL_RSPEC_BW_160MHZ);
		}
	}
	/* Is the phy configured to use > 20MHZ frames? If so then pick the
	 * desired txbw
	 */
	else if (CHSPEC_IS8080(wlc->chanspec) && (RSPEC_ISHE(rspec) || RSPEC_ISVHT(rspec)) &&
		(rspec_override || (scb->flags3 & SCB3_IS_80_80))) {
		mimo_txbw = WL_RSPEC_BW_160MHZ;
	} else if (CHSPEC_IS160(wlc->chanspec) && (RSPEC_ISHE(rspec) || RSPEC_ISVHT(rspec)) &&
		(rspec_override || (scb->flags3 & SCB3_IS_160))) {
		mimo_txbw = WL_RSPEC_BW_160MHZ;
	} else if (CHSPEC_BW_GE(wlc->chanspec, WL_CHANSPEC_BW_80) &&
		(RSPEC_ISHE(rspec) || RSPEC_ISVHT(rspec))) {
		mimo_txbw = WL_RSPEC_BW_80MHZ;
	} else if (CHSPEC_BW_GE(wlc->chanspec, WL_CHANSPEC_BW_40)) {
		/* default txbw is 20in40 */
		mimo_txbw = WL_RSPEC_BW_20MHZ;

		if (RSPEC_ISHT(rspec) || RSPEC_ISVHT(rspec) || RSPEC_ISHE(rspec)) {
			if (rspec_override || (scb->flags & SCB_IS40)) {
				mimo_txbw = WL_RSPEC_BW_40MHZ;
#ifdef WLMCHAN
				/* XXX 4360: why would scb->flags indicate is40
				 * if the sta is associated at a 20MHz? Do we
				 *	need different flags for capability (is40)
				 * from operational for the current state,
				 * which would be is20?  This same problem needs
				 * to be fixed
				 * for the 80MHz case.
				 */
				/* PR95044: if mchan enabled and bsscfg is AP,
				 * then must check the bsscfg chanspec to make
				 * sure our AP is operating on 40MHz channel.
				 */
				if (MCHAN_ENAB(wlc->pub) && BSSCFG_AP(bsscfg) &&
					CHSPEC_IS20(
					bsscfg->current_bss->chanspec)) {
					mimo_txbw = WL_RSPEC_BW_20MHZ;
				}
#endif /* WLMCHAN */
			}
		}

		if (RSPEC_ISHT(rspec) && ((rspec & WL_RSPEC_HT_MCS_MASK) == 32))
			mimo_txbw = WL_RSPEC_BW_40MHZ;
	} else	{
		mimo_txbw = WL_RSPEC_BW_20MHZ;
	}

	return mimo_txbw;
} /* wlc_d11ac_hdrs_determine_mimo_txbw */

/**
 * If the caller decided that an rts or cts frame needs to be transmitted, the transmit properties
 * of the rts/cts have to be determined and communicated to the transmit hardware. RTS/CTS rates are
 * always legacy rates (HR/DSSS, ERP, or OFDM).
 */
static INLINE uint16
wlc_d11_hdrs_rts_cts(struct scb *scb /* [in] */, wlc_bsscfg_t *bsscfg /* [in] */,
	ratespec_t rspec, bool use_rts, bool use_cts, ratespec_t *rts_rspec /* [out] */,
	uint8 *rts_preamble_type /* [out] */)
{
	uint16 rtsctscontrol;
	uint16 phy_rate;
	uint8 rts_rate;
	*rts_preamble_type = WLC_LONG_PREAMBLE;

#ifdef NOT_YET
	/* XXX Optimization: if use rts-cts or cts-to-self,
	 * mixedmode preamble may not be necessary if the dest
	 * support GF. Clear hdr_mcs_mixedmode and reset
	 * short_preamble, recalculating duration fields are
	 * required.
	 */
	if (SCB_ISGF_CAP(scb)) {
		hdr_mcs_mixedmode = FALSE;
		hdr_cckofdm_shortpreamble = TRUE;
	}
#endif /* NOT_YET */

	*rts_rspec = wlc_rspec_to_rts_rspec(bsscfg, rspec, FALSE);
	ASSERT(RSPEC_ISLEGACY(*rts_rspec));

	/* extract the MAC rate in 0.5Mbps units */
	rts_rate = (uint8)RSPEC2RATE(*rts_rspec);

	rtsctscontrol = RSPEC_ISCCK(*rts_rspec) ?
		htol16(D11AC_RTSCTS_FRM_TYPE_11B) :
		htol16(D11AC_RTSCTS_FRM_TYPE_11AG);

	/* XXX
	 * OFDM should bypass below preamble set, but old chips are OK
	 * since they ignore that bit. Before 4331A0 PR79339 is fixed,
	 * restrict to CCK, which is actually correct
	 */
	if (!RSPEC_ISOFDM(*rts_rspec) &&
		!((rts_rate == WLC_RATE_1M) ||
		(scb->bsscfg->PLCPHdr_override == WLC_PLCP_LONG))) {
		*rts_preamble_type = WLC_SHORT_PREAMBLE;
		rtsctscontrol |= htol16(D11AC_RTSCTS_SHORT_PREAMBLE);
	}

	/* set RTS/CTS flag */
	if (use_rts) {
		rtsctscontrol |= htol16(D11AC_RTSCTS_USE_RTS);
	} else if (use_cts) {
		rtsctscontrol |= htol16(D11AC_RTSCTS_USE_CTS);
	}

	/* RTS/CTS Rate index - Bits 3-0 of plcp byte0	*/
	phy_rate = rate_info[rts_rate] & RATE_INFO_M_RATE_MASK;
	rtsctscontrol |= htol16((phy_rate << D11AC_RTSCTS_USE_RATE_SHIFT));

	return rtsctscontrol;
} /* wlc_d11_hdrs_rts_cts */

/* XXX  FBW_BW_XXX number space is used to calculate FbwInfo field value.
 * See http://hwnbu-twiki.sj.broadcom.com/bin/view/Mwgroup/TxDescriptor
 * for FbwInfo definition.
 */
#define FBW_BW_20MHZ		4
#define FBW_BW_40MHZ		5
#define FBW_BW_80MHZ		6
#define FBW_BW_INVALID		(FBW_BW_20MHZ + 3)

static INLINE uint8 BCMFASTPATH
wlc_tx_dynbw_fbw(wlc_info_t *wlc, struct scb *scb, ratespec_t rspec)
{
	uint8 fbw = FBW_BW_INVALID;

	if (RSPEC_BW(rspec) == WL_RSPEC_BW_80MHZ)
		fbw = FBW_BW_40MHZ;
	else if (RSPEC_BW(rspec) == WL_RSPEC_BW_40MHZ) {
#ifdef WL11AC
		if (RSPEC_ISVHT(rspec)) {
			uint8 mcs, nss, prop_mcs = VHT_PROP_MCS_MAP_NONE;
			bool ldpc;
			uint16 mcsmap = 0;
			uint8 vht_ratemask = wlc_vht_get_scb_ratemask(wlc->vhti, scb);
			mcs = rspec & WL_RSPEC_VHT_MCS_MASK;
			nss = (rspec & WL_RSPEC_VHT_NSS_MASK) >> WL_RSPEC_VHT_NSS_SHIFT;
			ldpc = (rspec & WL_RSPEC_LDPC) ? TRUE : FALSE;
			mcs = VHT_MCS_MAP_GET_MCS_PER_SS(nss, vht_ratemask);
			if (vht_ratemask & WL_VHT_FEATURES_1024QAM)
				prop_mcs = VHT_PROP_MCS_MAP_10_11;
			mcsmap = wlc_get_valid_vht_mcsmap(mcs, prop_mcs, BW_20MHZ,
			                                  ldpc, nss, vht_ratemask);
			if (mcsmap & (1 << mcs))
				fbw = FBW_BW_20MHZ;
		} else
#endif /* WL11AC */
			fbw = FBW_BW_20MHZ;
	}

	return fbw;
}

typedef struct wlc_d11mac_params wlc_d11mac_parms_t;
struct wlc_d11mac_params {
	struct dot11_header *h;
	wlc_pkttag_t *pkttag;
	uint16 fc;
	uint16 type;
	bool qos;
	bool a4;
	int len;
	int phylen;
	uint8 IV_offset;
	uint frag;
	uint nfrags;
};

/**
 * Function to extract 80211 mac hdr information from the pkt.
 */
static void
wlc_extract_80211mac_hdr(wlc_info_t *wlc, void *p, wlc_d11mac_parms_t *parms,
	const wlc_key_info_t *key_info, uint frag, uint nfrags)
{
	uint8 *IV_offset = &(parms->IV_offset);
	uint16 *type = &(parms->type);
	int *phylen = &(parms->phylen);

	parms->frag = frag;
	parms->nfrags = nfrags;

	/* locate 802.11 MAC header */
	parms->h = (struct dot11_header*) PKTDATA(wlc->osh, p);
	parms->pkttag = WLPKTTAG(p);
	parms->fc = ltoh16((parms->h)->fc);
	*type = FC_TYPE(parms->fc);
	parms->qos = (*type == FC_TYPE_DATA &&
		FC_SUBTYPE_ANY_QOS(FC_SUBTYPE(parms->fc)));
	parms->a4 = (parms->fc & (FC_TODS | FC_FROMDS)) == (FC_TODS | FC_FROMDS);

	/* compute length of frame in bytes for use in PLCP computations */
	parms->len = pkttotlen(wlc->osh, p);
	*phylen = parms->len + DOT11_FCS_LEN;

	/* Add room in phylen for the additional bytes for icv, if required.
	 * this is true for both h/w and s/w encryption. The latter only
	 * modifies the pkt length
	 */

	if (key_info != NULL) {

		if (WLC_KEY_IS_MGMT_GROUP(key_info))
			*phylen += WLC_KEY_MMIC_IE_LEN(key_info);
		else
			*phylen += key_info->icv_len;

		/* external crypto adds iv to the pkt, include it in phylen */
		if (WLC_KEY_IS_LINUX_CRYPTO(key_info))
			*phylen += key_info->iv_len;

		if (WLC_KEY_FRAG_HAS_TKIP_MIC(p, key_info, frag, nfrags))
			*phylen += TKIP_MIC_SIZE;
	}

	/* IV Offset, i.e. start of the 802.11 header */

	*IV_offset = DOT11_A3_HDR_LEN;

	if (*type == FC_TYPE_DATA) {
		if (parms->a4)
			*IV_offset += ETHER_ADDR_LEN;

		if (parms->qos) {
			*IV_offset += DOT11_QOS_LEN;

			if (parms->fc & FC_ORDER) {
				*IV_offset += DOT11_HTC_LEN;
			}
		}
	} else if (*type == FC_TYPE_CTL) {
		/* Subtract one address and SeqNum */
		*IV_offset -= ETHER_ADDR_LEN + 2;
	}

	WL_NONE(("wl%d: %s: len %d, phylen %d\n", WLCWLUNIT(wlc), __FUNCTION__,
		parms->len, *phylen));
}

/**
 * Update the FIFO index in a frameID, leaving sequence number and rate probe bits intact.
 *
 * @param wlc		Handle to wlc context.
 * @param frameid	Frame ID.
 * @param fifo		New FIFO index.
 * @return		New Frame ID.
 */
static uint16
wlc_frameid_set_fifo(wlc_info_t *wlc, uint16 frameid, uint fifo)
{
	if (D11REV_GE(wlc->pub->corerev, 128)) {
		fifo = WLC_HW_MAP_TXFIFO(wlc, fifo);	/* Map to HW FIFO */
		frameid &= ~D11_REV128_TXFID_FIFO_MASK;
		frameid |= (fifo & D11_REV128_TXFID_FIFO_MASK);
	} else {
		frameid &= ~TXFID_FIFO_MASK;
		frameid |= (fifo & TXFID_FIFO_MASK);
	}

	ASSERT(D11_TXFID_GET_FIFO(wlc, frameid) == WLC_HW_UNMAP_TXFIFO(wlc, fifo));
	return frameid;
}

/**
 * Generate a new frame ID.
 *
 * The FrameID field in the TXH/TXS consists of a FIFO index and unique sequence number. For
 * D11 corerev <128 it also contains rate probe information. The frameid_le argument can be
 * used to specify a frameID from which the rate probe information is to be copied into the
 * new frameID, which is useful when updating the frameID of an existing frame.
 *
 * @param wlc		Handle to wlc context.
 * @param frameid_le	Frame ID from which rate probe bits are to be copied. @note: LITTLE ENDIAN.
 * @param fifo		TX FIFO index.
 * @return		Frame ID.
 */
uint16
wlc_compute_frameid(wlc_info_t *wlc, uint16 frameid_le, uint fifo)
{
	uint seq;

	if (fifo == TX_BCMC_FIFO) {
		seq = wlc->mc_fid_counter++;
	} else {
		seq = wlc->counter++;
	}
	if (D11REV_GE(wlc->pub->corerev, 128)) {
		ASSERT((fifo & ~D11_REV128_TXFID_FIFO_MASK) == 0);
		BCM_REFERENCE(frameid_le);

		fifo = WLC_HW_MAP_TXFIFO(wlc, fifo);	/* Map to HW FIFO */

		return (((seq << D11_REV128_TXFID_SEQ_SHIFT) & D11_REV128_TXFID_SEQ_MASK) |
		       (fifo & D11_REV128_TXFID_FIFO_MASK));
	} else {
		ASSERT((fifo & ~TXFID_FIFO_MASK) == 0);

		/* Copy rate probe information from frameid_le argument */
		return ((ltoh16(frameid_le) & ~(TXFID_SEQ_MASK | TXFID_FIFO_MASK)) |
		       ((seq << TXFID_SEQ_SHIFT) & TXFID_SEQ_MASK) |
		       (fifo & TXFID_FIFO_MASK));
	}
}

/*
 * Generate a new frame ID - RTC variant
 */
uint16
wlc_compute_frameid_rtc(wlc_tx_rtc_t *rtc, uint16 frameid_le)
{
	wlc_info_t *wlc = rtc->wlc;
	uint seq;

	if (rtc->fifo == TX_BCMC_FIFO) {
		seq = wlc->mc_fid_counter++;
	} else {
		seq = wlc->counter++;
	}

	if (rtc->is_ge128) {
		ASSERT((rtc->fifo & ~D11_REV128_TXFID_FIFO_MASK) == 0);
		BCM_REFERENCE(frameid_le);

		return (((seq << D11_REV128_TXFID_SEQ_SHIFT) & D11_REV128_TXFID_SEQ_MASK) |
		       (rtc->hw_fifo & D11_REV128_TXFID_FIFO_MASK));
	} else {
		ASSERT((rtc->fifo & ~TXFID_FIFO_MASK) == 0);

		/* Copy rate probe information from frameid_le argument */
		return ((ltoh16(frameid_le) & ~(TXFID_SEQ_MASK | TXFID_FIFO_MASK)) |
		       ((seq << TXFID_SEQ_SHIFT) & TXFID_SEQ_MASK) |
		       (rtc->fifo & TXFID_FIFO_MASK));
	}
}

static INLINE void
wlc_compute_initial_MacTxCtrl(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
	struct scb *scb, uint16 *mcl, uint16 *mch, wlc_d11mac_parms_t *parms,
	bool mutx_pkteng_on)
{
	wlc_pkttag_t *pkttag = parms->pkttag;

	/* MAC-HDR: MacTxControlLow */
	/* set the ignpmq bit for all pkts tx'd in PS mode and for beacons and for anything
	 * going out from a STA interface.
	 */
	if (SCB_PS(scb) || ((parms->fc & FC_KIND_MASK) == FC_BEACON) || BSSCFG_STA(bsscfg))
		*mcl |= D11AC_TXC_IPMQ;

	if (mutx_pkteng_on)
		*mcl |= D11AC_TXC_IPMQ;

	if (parms->frag == 0)
		*mcl |= D11AC_TXC_STMSDU;

	/* MacTxControlHigh */
	/* start from fix rate. clear it if auto */
	*mch = D11AC_TXC_FIX_RATE;

#ifdef PSPRETEND
	/* If D11AC_TXC_PPS is set, ucode is monitoring for failed TX status. In that case,
	 * it will add new PMQ entry so start the process of draining the TX fifo.
	 * Of course, PS pretend should be in enabled state and we guard against the "ignore PMQ"
	 * because this is used to force a packet to air.
	 */

	/* Do not allow IBSS bsscfg to use normal pspretend yet,
	 * as PMQ process is not supported for them now.
	 */
	if (SCB_PS_PRETEND_ENABLED(bsscfg, scb) && !BSSCFG_IBSS(bsscfg) &&
		!(*mcl & D11AC_TXC_IPMQ) && (parms->type == FC_TYPE_DATA)) {
		*mch |= D11AC_TXC_PPS;
	}
#endif /* PSPRETEND */

	/* Setting Packet Expiry */
	if (pkttag->flags & WLF_EXPTIME) {
		*mcl |= D11AC_TXC_AGING; /* Enable timestamp for the packet */
	}

	if (!ETHER_ISMULTI(&parms->h->a1) && !WLPKTFLAG_RIFS(pkttag) &&
		!(wlc->_amsdu_noack && WLPKTFLAG_AMSDU(pkttag)) &&
		!(!WLPKTFLAG_AMPDU(pkttag) && parms->qos && bsscfg->wme->wme_noack)) {
		*mcl |= D11AC_TXC_IACK;
	}
}

/**
 * Function to fech / compute and assign seq numbers to MAC hdr.
 * 1. Try to use preassigned / software seq numbers
 * 2. If SW generated seq is not available :
 *     - set MacTxControlLow to indicate usage of ucode generated seq
 */
static INLINE void
wlc_fill_80211mac_hdr_seqnum(wlc_info_t *wlc, void *p, struct scb *scb,
	uint16 *mcl, wlc_d11mac_parms_t *parms)
{
	wlc_pkttag_t *pkttag = parms->pkttag;
	uint16 seq = 0;

	/* use preassigned or software seqnum */
	if (!wlc_tx_preassigned_seqnum(wlc, pkttag, &seq)) {
		if (SCB_QOS(scb) && ((parms->fc & FC_KIND_MASK) == FC_QOS_DATA) &&
			!ETHER_ISMULTI(&parms->h->a1)) {
			seq = SCB_SEQNUM(scb, PKTPRIO(p));
			/* Increment the sequence number only after the last fragment */
			if (parms->frag == (parms->nfrags - 1))
				SCB_SEQNUM(scb, PKTPRIO(p))++;
		} else if (parms->type != FC_TYPE_CTL) {

#if defined(WL_PRQ_RAND_SEQ)
			/* Check if randomize probe req seq number is enabled
			 * if enabled fetch the random seq number for probe
			 * request message by calling wlc_getrand API.
			 * If not Ucode will fill the seq number.
			 */
			if ((parms->type == FC_TYPE_MNG) &&
				((parms->fc & FC_KIND_MASK) == FC_PROBE_REQ) &&
				(PRQ_RAND_SEQ_ENAB(wlc->pub))) {
				tx_prs_cfg_t *tx_prs_cfg = TX_PRS_CFG(wlc->txqi, SCB_BSSCFG(scb));
				if (tx_prs_cfg &&
					(tx_prs_cfg->prq_rand_seq == PRQ_RAND_SEQ_APPLY)) {
					seq = tx_prs_cfg->prq_rand_seqId++;
				} else {
					/* Feature is enabled during init time only but IOVAR
					 * is disabled during init. So UCODE needs to fill the seq
					 * number else seq number will all be set to 0.
					 */
					*mcl |= D11AC_TXC_ASEQ;
				}
			}
			else
#endif /* WL_PRQ_RAND_SEQ */
			{
				*mcl |= D11AC_TXC_ASEQ;
			}
		}
	}

	if (parms->type != FC_TYPE_CTL) {
		parms->h->seq = htol16((seq << SEQNUM_SHIFT) | (parms->frag & FRAGNUM_MASK));
	}

#ifdef PROP_TXSTATUS
	if (WLFC_GET_REUSESEQ(wlfc_query_mode(wlc->wlfc))) {
		WL_SEQ_SET_NUM(pkttag->seq, seq);
		SET_WL_HAS_ASSIGNED_SEQ(pkttag->seq);
	}
#endif /* PROP_TXSTATUS */

	WL_NONE(("wl%d: %s: seq %d/%d, Add ucode generated SEQ %d\n",
		WLCWLUNIT(wlc), __FUNCTION__, seq, parms->frag, (*mcl & D11AC_TXC_ASEQ)));
}

/**
 * Compute and fill "per packet info" field of the TX desc.
 * This field of size 22 bytes is also referred as the short TX desc (not cached).
 * Called for e.g. 43602/4366.
 */
static INLINE void
wlc_fill_txd_per_pkt_info(wlc_info_t *wlc, void *p, struct scb *scb,
	d11pktinfo_common_t *PktInfo, uint16 mcl, uint16 mch, wlc_d11mac_parms_t *parms,
	uint16 frameid, uint16 d11_txh_len, uint16 HEModeControl)
{
	BCM_REFERENCE(d11_txh_len);
	BCM_REFERENCE(HEModeControl);

	/* Chanspec - channel info for packet suppression */
	PktInfo->Chanspec = htol16(wlc->chanspec);

	/* IV Offset, i.e. start of the 802.11 header */
	PktInfo->IVOffset = parms->IV_offset;

	/* FrameLen */
	PktInfo->FrameLen = htol16((uint16)parms->phylen);

	/* Sequence number final write */
	PktInfo->Seq = parms->h->seq;

	/* Timestamp */
	if (parms->pkttag->flags & WLF_EXPTIME) {
		PktInfo->Tstamp = htol16((parms->pkttag->u.exptime >>
			D11AC_TSTAMP_SHIFT) & 0xffff);
	}

	/* TxFrameID (gettxrate may have updated it) */
	PktInfo->TxFrameID = htol16(frameid);

	/* MacTxControl High and Low */
	PktInfo->MacTxControlLow = htol16(mcl);
	PktInfo->MacTxControlHigh = htol16(mch);
} /* wlc_fill_txd_per_pkt_info */

/**
 * Function to fill the per Cache Info field of the txd. Called for d11rev < 80.
 */
static void
wlc_fill_txd_per_cache_info(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
	struct scb *scb, void *p, wlc_pkttag_t *pkttag, d11actxh_cache_t *d11_cache_info,
	bool mutx_pkteng_on)
{
#ifdef WLMCNX
	if (MCNX_ENAB(wlc->pub)) {
		d11txh_cache_common_t *cache_info;
		uint16 bss = (uint16)wlc_mcnx_BSS_idx(wlc->mcnx, bsscfg);
		ASSERT(bss < M_P2P_BSS_MAX);
		cache_info = &d11_cache_info->common;
		cache_info->BssIdEncAlg |= (bss << D11AC_BSSID_SHIFT);
	}
#endif /* WLMCNX */

	if (WLPKTFLAG_AMPDU(pkttag) && D11REV_GE(wlc->pub->corerev, 40)) {
#ifdef WL_MUPKTENG
		if (mutx_pkteng_on)
			wlc_ampdu_mupkteng_fill_percache_info(wlc->ampdu_tx, scb,
				(uint8)PKTPRIO(p), d11_cache_info);
		else
#endif /* WL_MUPKTENG */
		{
			BCM_REFERENCE(mutx_pkteng_on);
			/* Fill per cache info */
			wlc_ampdu_fill_percache_info(wlc->ampdu_tx, scb,
				(uint8)PKTPRIO(p), d11_cache_info);
		}
	}
}

/**
 * Add d11actxh_t for 11ac mac & phy
 *
 * 'p' data must start with 802.11 MAC header
 * 'p' must allow enough bytes of local headers to be "pushed" onto the packet
 *
 * headroom == D11AC_TXH_LEN (D11AC_TXH_LEN is now 124 bytes which include PHY PLCP header)
 */
uint16
wlc_d11hdrs_rev40(wlc_info_t *wlc, void *p, struct scb *scb, uint txparams_flags, uint frag,
	uint nfrags, uint fifo, uint next_frag_len, wlc_key_t *key, const wlc_key_info_t *key_info,
	ratespec_t rspec_override)
{
	d11actxh_t *txh;
	osl_t *osh;
	wlc_d11mac_parms_t parms;
	uint16 frameid, mch, phyctl, rate_flag;
	uint16 mcl = 0;
	bool use_rts = FALSE;
	bool use_cts = FALSE, rspec_history = FALSE;
	bool use_rifs = FALSE;
	bool short_preamble;
	uint8 preamble_type = WLC_LONG_PREAMBLE;
	uint8 rts_preamble_type;
	struct dot11_rts_frame *rts = NULL;
	ratespec_t rts_rspec = 0;
	enum wme_ac ac;
	uint txrate;
	ratespec_t rspec;
	ratesel_txparams_t ratesel_rates;
#if defined(BCMDBG) || (defined(WLTEST) && !defined(WLTEST_DISABLED))
	uint16 phyctl_bwo = -1;
	uint16 phyctl_sbwo = -1;
#endif /* defined(BCMDBG) || (defined(WLTEST) && !defined(WLTEST_DISABLED)) */
#if WL_HT_TXBW_OVERRIDE_ENAB
	int8 txbw_override_idx;
#endif
	wlc_ht_info_t *hti = wlc->hti;
	uint8 sgi_tx;
	bool use_mimops_rts = FALSE;
	int rspec_legacy = -1; /* is initialized to prevent compiler warning */
	wlc_bsscfg_t *bsscfg;
	bool g_prot;
	int8 n_prot;
	wlc_wme_t *wme;
	uint corerev = wlc->pub->corerev;
	d11actxh_rate_t *rate_blk;
	d11actxh_rate_t *rate_hdr;
	uint8 *plcp;
#ifdef WL_LPC
	uint8 lpc_offset = 0;
#endif
#if defined(WL_BEAMFORMING)
	uint8 bf_shm_index = BF_SHM_IDX_INV, bf_shmx_idx = BF_SHM_IDX_INV;
	bool bfen = FALSE, fbw_bfen = FALSE;
#endif /* WL_BEAMFORMING */
	uint8 fbw = FBW_BW_INVALID; /* fallback bw */
	txpwr204080_t txpwrs;
	int txpwr_bfsel;
	uint8 txpwr;
	int k;
	d11actxh_cache_t *cache_info;
	bool mutx_pkteng_on = FALSE;
	wl_tx_chains_t txbf_chains = 0;
	uint8	bfe_sts_cap = 0, txbf_uidx = 0;
	uint16 rtsthresh;
#if defined(WL_PROT_OBSS) && !defined(WL_PROT_OBSS_DISABLED)
	ratespec_t phybw = (CHSPEC_IS8080(wlc->chanspec) || CHSPEC_IS160(wlc->chanspec)) ?
		WL_RSPEC_BW_160MHZ :
		(CHSPEC_IS80(wlc->chanspec) ? WL_RSPEC_BW_80MHZ :
		(CHSPEC_IS40(wlc->chanspec) ? WL_RSPEC_BW_40MHZ : WL_RSPEC_BW_20MHZ));
#endif
	uint16 txh_off = 0;
	d11txhdr_t *hdrptr;

	BCM_REFERENCE(bfe_sts_cap);
	BCM_REFERENCE(txbf_chains);
	BCM_REFERENCE(mutx_pkteng_on);

	ASSERT(scb != NULL);
	ASSERT(fifo < WLC_HW_NFIFO_INUSE(wlc));

#ifdef WL_MUPKTENG
	mutx_pkteng_on = wlc_mutx_pkteng_on(wlc->mutx);
#endif
	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);

	rtsthresh = wlc->RTSThresh;
	short_preamble = (txparams_flags & WLC_TX_PARAMS_SHORTPRE) != 0;

	/* init the g_prot to bsscfg protections only when band is 2.4G */
	if (BAND_2G(wlc->band->bandtype))
		g_prot = WLC_PROT_G_CFG_G(wlc->prot_g, bsscfg);
	else
		g_prot = FALSE;
	n_prot = WLC_PROT_N_CFG_N(wlc->prot_n, bsscfg);

	wme = bsscfg->wme;

	osh = wlc->osh;

	/* Extract 802.11 mac hdr and compute frame length in bytes (for PLCP) */
	wlc_extract_80211mac_hdr(wlc, p, &parms, key_info, frag, nfrags);

	/* Add Broadcom tx descriptor header */
	txh = (d11actxh_t*)PKTPUSH(osh, p, D11AC_TXH_LEN);
	bzero((char*)txh, D11AC_TXH_LEN);

	/*
	 * Some fields of the MacTxCtrl are tightly coupled,
	 * and dependant on various other params, which are
	 * yet to be computed. Due to complex dependancies
	 * with a number of params, filling of MacTxCtrl cannot
	 * be completely decoupled and made independant.
	 *
	 * Compute and fill independant fields of the the MacTxCtrl here.
	 */
	wlc_compute_initial_MacTxCtrl(wlc, bsscfg, scb, &mcl, &mch,
		&parms, mutx_pkteng_on);

	/* Compute and assign sequence number to MAC HDR */
	wlc_fill_80211mac_hdr_seqnum(wlc, p, scb, &mcl, &parms);

	/* TDLS U-APSD buffer STA: if peer is in PS, save the last MPDU's seq and tid for PTI */
#ifdef WLTDLS
	if (BSS_TDLS_ENAB(wlc, SCB_BSSCFG(scb)) &&
		SCB_PS(scb) &&
		wlc_tdls_buffer_sta_enable(wlc->tdls)) {
		uint8 tid = 0;
		if (parms.qos) {
			uint16 qc;
			qc = (*(uint16 *)((uchar *)parms.h + (parms.a4 ?
				DOT11_A4_HDR_LEN : DOT11_A3_HDR_LEN)));
			tid = (QOS_TID(qc) & 0x0f);
		}
		wlc_tdls_update_tid_seq(wlc->tdls, scb, tid, ltoh16(parms.h->seq));
	}
#endif

	/* Compute frameid, also possibly change seq */
	frameid = wlc_compute_frameid(wlc, txh->PktInfo.TxFrameID, fifo);

	/* TxStatus, Note the case of recreating the first frag of a suppressed frame
	 * then we may need to reset the retry cnt's via the status reg
	 */
	txh->PktInfo.TxStatus = 0;

	ac = WME_PRIO2AC(PKTPRIO(p));

	/* (1) RATE: determine and validate primary rate and fallback rates */

	ratesel_rates.num = 1; /* default to 1 */

	if (RSPEC_ACTIVE(rspec_override)) {
		ratesel_rates.rspec[0] = rspec_override;
	} else if ((parms.type == FC_TYPE_MNG) ||
		(parms.type == FC_TYPE_CTL) ||
		((parms.pkttag)->flags & WLF_8021X) ||
#ifdef BCMWAPI_WAI
		((parms.pkttag)->flags & WLF_WAI) ||
#endif
		FALSE) {
		ratesel_rates.rspec[0] = scb->rateset.rates[0] & RATE_MASK;
	} else if (RSPEC_ACTIVE(wlc->band->mrspec_override) && ETHER_ISMULTI(&parms.h->a1)) {
		ratesel_rates.rspec[0] = wlc->band->mrspec_override;
	} else if (RSPEC_ACTIVE(wlc->band->rspec_override) && !ETHER_ISMULTI(&parms.h->a1)) {
		ratesel_rates.rspec[0] = wlc->band->rspec_override;
		rspec_history = TRUE;
	} else if (ETHER_ISMULTI(&parms.h->a1) || SCB_INTERNAL(scb)) {
		ratesel_rates.rspec[0] = scb->rateset.rates[0] & RATE_MASK;
	} else {
		/* run rate algorithm for data frame only, a cookie will be deposited in frameid */
		ratesel_rates.num = 4; /* enable multi fallback rate */
		ratesel_rates.ac = ac;
		wlc_scb_ratesel_gettxrate(wlc->wrsi, scb, &frameid, &ratesel_rates, &rate_flag);

#if defined(WL_LINKSTAT)
		if (LINKSTAT_ENAB(wlc->pub))
			wlc_cckofdm_tx_inc(bsscfg, parms.type,
				wf_rspec_to_rate(ratesel_rates.rspec[0])/500);
#endif /* WL_LINKSTAT */

		if (((scb->flags & SCB_BRCM) == 0) &&
		    (((parms.fc & FC_KIND_MASK) == FC_NULL_DATA) ||
		     ((parms.fc & FC_KIND_MASK) == FC_QOS_NULL))) {
			/* Use RTS/CTS rate for NULL frame */
			ratesel_rates.num = 2;
			ratesel_rates.rspec[0] =
				wlc_rspec_to_rts_rspec(bsscfg, ratesel_rates.rspec[0], FALSE);
			ratesel_rates.rspec[1] = scb->rateset.rates[0] & RATE_MASK;
		} else {
			(parms.pkttag)->flags |= WLF_RATE_AUTO;

			/* The rate histo is updated only for packets on auto rate. */
			/* perform rate history after txbw has been selected */
			if (frag == 0)
				rspec_history = TRUE;
		}

		mch &= ~D11AC_TXC_FIX_RATE;
		if (rate_flag & RATESEL_VRATE_PROBE)
			WLPKTTAG(p)->flags |= WLF_VRATE_PROBE;

#ifdef WL_LPC
		if (LPC_ENAB(wlc)) {
			/* Query the power offset to be used from LPC */
			lpc_offset = wlc_scb_lpc_getcurrpwr(wlc->wlpci, scb, ac);
		} else {
			/* No Link Power Control. Transmit at nominal power. */
		}
#endif
	}

#ifdef WL_RELMCAST
	if (RMC_ENAB(wlc->pub))
		wlc_rmc_process(wlc->rmc, parms.type, p, parms.h, &mcl, &ratesel_rates.rspec[0]);
#endif

#if defined(WL_MU_TX) && !defined(WL_MU_TX_DISABLED)
	bf_shmx_idx = wlc_txbf_get_mubfi_idx(wlc->txbf, scb);
	if (bf_shmx_idx != BF_SHM_IDX_INV) {
		/* link is capable of using mu sounding enabled in shmx bfi interface  */
		mch |= D11AC_TXC_BFIX;
		if (SCB_MU(scb) && WLPKTFLAG_AMPDU(parms.pkttag) &&
		/* Check if client has MU FIFO. If yes set MU bits */
		wlc_fifo_isMU(wlc->fifo, scb, ac)) {
#if defined(BCMDBG) || defined(WLTEST)
			wlc_check_fifo_type(wlc->fifo, scb, ac, WL_MU_DLMMU);
#endif

			ASSERT(nfrags == 1); /* should not frag AMPDUs, can't MU frags */

			/* link is capable of mutx and has been enabled to do mutx */
			mch |= D11AC_TXC_MU;
#ifdef WL_MUPKTENG
			if (mutx_pkteng_on) {
				mcl |=  D11AC_TXC_AMPDU;
				mch &= ~D11AC_TXC_SVHT;
			}
#endif
		}
	}
#endif /* WL_MU_TX && !WL_MU_TX_DISABLED */

	rate_blk = WLC_TXD_RATE_INFO_GET(txh, corerev);

	for (k = 0; k < ratesel_rates.num; k++) {
		/* init primary and fallback rate pointers */
		rspec = ratesel_rates.rspec[k];
		rate_hdr = &rate_blk[k];

		plcp = rate_hdr->plcp;
		rate_hdr->RtsCtsControl = 0;

		if (N_ENAB(wlc->pub)) {
			uint32 mimo_txbw;

			rspec_legacy = RSPEC_ISLEGACY(rspec);

			mimo_txbw = wlc_d11hdrs_rev40_determine_mimo_txbw(wlc, scb, bsscfg, rspec);
#if WL_HT_TXBW_OVERRIDE_ENAB
			if (CHSPEC_IS40(wlc->chanspec) || CHSPEC_IS80(wlc->chanspec)) {
				WL_HT_TXBW_OVERRIDE_IDX(hti, rspec, txbw_override_idx);

				if (txbw_override_idx >= 0) {
					mimo_txbw = txbw2rspecbw[txbw_override_idx];
					phyctl_bwo = txbw2acphyctl0bw[txbw_override_idx];
					phyctl_sbwo = txbw2acphyctl1bw[txbw_override_idx];
				}
			}
#endif /* WL_HT_TXBW_OVERRIDE_ENAB */

#ifdef WL_OBSS_DYNBW
			if (WLC_OBSS_DYNBW_ENAB(wlc->pub)) {
				wlc_obss_dynbw_tx_bw_override(wlc->obss_dynbw, bsscfg,
					&mimo_txbw);
			}
#endif /* WL_OBSS_DYNBW */

			rspec &= ~WL_RSPEC_BW_MASK;
			rspec |= mimo_txbw;
		} else /* N_ENAB */
		{
			/* Set ctrlchbw as 20Mhz */
			ASSERT(RSPEC_ISLEGACY(rspec));
			rspec &= ~WL_RSPEC_BW_MASK;
			rspec |= WL_RSPEC_BW_20MHZ;
		}

		if (N_ENAB(wlc->pub)) {
			uint8 mimo_preamble_type;

			sgi_tx = WLC_HT_GET_SGI_TX(hti);

			/* XXX 4360: consider having SGI always come from the rspec
			 * instead of calculating here
			 */
			if (!RSPEC_ACTIVE(wlc->band->rspec_override)) {
				bool _scb_stbc_on = FALSE;
				bool stbc_tx_forced = WLC_IS_STBC_TX_FORCED(wlc);
				bool stbc_ht_scb_auto = WLC_STF_SS_STBC_HT_AUTO(wlc, scb);
				bool stbc_vht_scb_auto = WLC_STF_SS_STBC_VHT_AUTO(wlc, scb);

				if (sgi_tx == OFF) {
					rspec &= ~WL_RSPEC_SGI;
				} else if (sgi_tx == ON) {
					if (RSPEC_ISHT(rspec) || RSPEC_ISVHT(rspec)) {
						rspec |= WL_RSPEC_SGI;
					}
				}

				/* XXX move this LDPC decision outside, i.e. turn off when power
				 * is a concern ?
				 */
				/* XXX 4360: consider having LDPC always come from the rspec
				 * instead of calculating here
				 */

				ASSERT(!(rspec & WL_RSPEC_LDPC));
				rspec &= ~WL_RSPEC_LDPC;
				rspec &= ~WL_RSPEC_STBC;

				/* LDPC */
				if (wlc->stf->ldpc_tx == ON ||
				    (((RSPEC_ISVHT(rspec) && SCB_VHT_LDPC_CAP(wlc->vhti, scb)) ||
				      (RSPEC_ISHT(rspec) && SCB_HT_LDPC_CAP(scb))) &&
				     wlc->stf->ldpc_tx == AUTO)) {
					if (!rspec_legacy)
#ifdef WL_PROXDETECT
						if (!(PROXD_ENAB(wlc->pub) &&
							wlc_proxd_frame(wlc, parms.pkttag)))
#endif
							rspec |= WL_RSPEC_LDPC;
				}

				/* STBC */
				if ((wlc_ratespec_nsts(rspec) == 1)) {
					if (stbc_tx_forced ||
					  ((RSPEC_ISHT(rspec) && stbc_ht_scb_auto) ||
					   (RSPEC_ISVHT(rspec) && stbc_vht_scb_auto))) {
						_scb_stbc_on = TRUE;
					}

					/* XXX: CRDOT11ACPHY-1826 : Disable STBC when mcs > 7
					 * if rspec is 160Mhz for 80p80phy.
					 */
					if (WLC_PHY_AS_80P80(wlc, wlc->chanspec) &&
						RSPEC_ISVHT(rspec) && RSPEC_IS160MHZ(rspec)) {
						uint mcs = (rspec & WL_RSPEC_VHT_MCS_MASK);
						if (mcs > 7)
							_scb_stbc_on = FALSE;
					}

					if (_scb_stbc_on && !rspec_legacy)
#ifdef WL_PROXDETECT
						if (!(PROXD_ENAB(wlc->pub) &&
							wlc_proxd_frame(wlc, parms.pkttag)))
#endif
							rspec |= WL_RSPEC_STBC;
				}
			}

			/* Check if it needs rts for VHT d-msps enabled STAs */
			if (RSPEC_ISVHT(rspec) &&
				(wlc_ratespec_nsts(rspec) > 1) &&
				WLC_STF_GET_SCB_MIMOPS_ENAB(wlc->stf, scb) &&
				WLC_STF_GET_SCB_MIMOPS_RTS_ENAB(wlc->stf, scb)) {
				use_rts = use_mimops_rts = TRUE;
			}
			/* Determine the HT frame format, HT_MM or HT_GF */
			if (RSPEC_ISHT(rspec)) {
				int nsts;

				mimo_preamble_type = wlc_prot_n_preamble(wlc, scb);
				if (RSPEC_ISHT(rspec)) {
					nsts = wlc_ratespec_nsts(rspec);
					preamble_type = mimo_preamble_type;
					/* XXX 4360: might need the same 20in40 check
					 * and protection for VHT case
					 */
					if (n_prot == WLC_N_PROTECTION_20IN40 &&
						RSPEC_IS40MHZ(rspec))
						use_cts = TRUE;

					/* if the mcs is multi stream check if it needs
					 * an rts
					 */
					if (nsts > 1) {
						if (WLC_STF_GET_SCB_MIMOPS_ENAB(wlc->stf, scb) &&
						    WLC_STF_GET_SCB_MIMOPS_RTS_ENAB(wlc->stf, scb))
							use_rts = use_mimops_rts = TRUE;
					}

					/* if SGI is selected, then forced mm for single
					 * stream
					 * (spec section 9.16, Short GI operation)
					 */
					if (RSPEC_ISSGI(rspec) && nsts == 1) {
						preamble_type = WLC_MM_PREAMBLE;
					}
				}
			} else {
				/* VHT always uses MM preamble */
				if (RSPEC_ISVHT(rspec)) {
					preamble_type = WLC_MM_PREAMBLE;
				}
			}
		}

		if (wlc->pub->_dynbw == TRUE) {
			fbw = wlc_tx_dynbw_fbw(wlc, scb, rspec);
		}

		/* - FBW_BW_20MHZ to make it 0-index based */
		rate_hdr->FbwInfo = (((fbw - FBW_BW_20MHZ) & FBW_BW_MASK) << FBW_BW_SHIFT);

		/* (2) PROTECTION, may change rspec */
		if (((parms.type == FC_TYPE_DATA) || (parms.type == FC_TYPE_MNG)) &&
			((parms.phylen > rtsthresh) || ((parms.pkttag)->flags & WLF_USERTS)) &&
			!ETHER_ISMULTI(&parms.h->a1))
			use_rts = TRUE;

		if ((wlc->band->gmode && g_prot && RSPEC_ISOFDM(rspec)) ||
			((RSPEC_ISHT(rspec) || RSPEC_ISVHT(rspec)) && n_prot)) {
			if (nfrags > 1) {
				/* For a frag burst use the lower modulation rates for the
				 * entire frag burst instead of protection mechanisms.
				 * As per spec, if protection mechanism is being used, a
				 * fragment sequence may only employ ERP-OFDM modulation for
				 * the final fragment and control response. (802.11g Sec 9.10)
				 * For ease we send the *whole* sequence at the
				 * lower modulation instead of using a higher modulation for the
				 * last frag.
				 */

				/* downgrade the rate to CCK or OFDM */
				if (g_prot) {
					/* Use 11 Mbps as the rate and fallback. We should make
					 * sure that if we are downgrading an OFDM rate to CCK,
					 * we should pick a more robust rate.  6 and 9 Mbps are not
					 * usually selected by rate selection, but even if the OFDM
					 * rate we are downgrading is 6 or 9 Mbps, 11 Mbps is more
					 * robust.
					 */
					rspec = CCK_RSPEC(WLC_RATE_FRAG_G_PROTECT);
				} else {
					/* Use 24 Mbps as the rate and fallback for what would have
					 * been a MIMO rate. 24 Mbps is the highest phy mandatory
					 * rate for OFDM.
					 */
					rspec = OFDM_RSPEC(WLC_RATE_FRAG_N_PROTECT);
				}
				parms.pkttag->flags &= ~WLF_RATE_AUTO;
			} else {
				/* Use protection mechanisms on unfragmented frames */
				/* If this is a 11g protection, then use CTS-to-self */
				if (wlc->band->gmode && g_prot && !RSPEC_ISCCK(rspec))
					use_cts = TRUE;
			}
		}

		/* calculate minimum rate */
		ASSERT(VALID_RATE_DBG(wlc, rspec));

		/* fix the preamble type for non-MCS rspec/rspec-fallback
		 * If SCB is short preamble capable and and shortpreamble is enabled
		 * and rate is NOT 1M or Override is NOT set to LONG preamble then use SHORT
		 * preamble else use LONG preamble
		 * OFDM should bypass below preamble set, but old chips are OK since they ignore
		 * that bit XXX before 4331A0 PR79339 is fixed, restrict to CCK, which is actually
		 * correct
		 */
		if (RSPEC_ISCCK(rspec)) {
			if (short_preamble &&
				!((RSPEC2RATE(rspec) == WLC_RATE_1M) ||
				(scb->bsscfg->PLCPHdr_override == WLC_PLCP_LONG)))
				preamble_type = WLC_SHORT_PREAMBLE;
			else
				preamble_type = WLC_LONG_PREAMBLE;
		}

		ASSERT(RSPEC_ISLEGACY(rspec) || WLC_IS_MIMO_PREAMBLE(preamble_type));

#if defined(WL_BEAMFORMING)
		if (TXBF_ENAB(wlc->pub)) {
			/* bfe_sts_cap = 3: foursteams, 2: three streams, 1: two streams */
			bfe_sts_cap = wlc_txbf_get_bfe_sts_cap(wlc->txbf, scb);
			if (bfe_sts_cap && (D11REV_LT(wlc->pub->corerev, 64) ||
				!wlc_txbf_bfrspexp_enable(wlc->txbf))) {
				/* Explicit TxBF: Number of txbf chains
				 * is min(#active txchains, #bfe sts + 1)
				 */
				txbf_chains = MIN((uint8)WLC_BITSCNT(wlc->stf->txchain),
						(bfe_sts_cap + 1));
			} else {
				/* bfe_sts_cap=0 indicates there is no Explicit TxBf link to this
				 * peer, and driver will probably use Implicit TxBF. Ignore the
				 * spatial_expension policy, and always use all currently enabled
				 * txcores
				 */
				txbf_chains = (uint8)WLC_BITSCNT(wlc->stf->txchain);
			}
		}
#endif /* WL_BEAMFORMING */
		/* get txpwr for bw204080 and txbf on/off */
		if (wlc_stf_get_204080_pwrs(wlc, rspec, &txpwrs, txbf_chains) != BCME_OK) {
			WL_ERROR(("wl%d:%s phyctl1 ppr returns error!\n",
				wlc->pub->unit, __FUNCTION__));
			ASSERT(0);
		}

		txpwr_bfsel = 0;
#if defined(WL_BEAMFORMING)
		if (TXBF_ENAB(wlc->pub) &&
			(wlc->allow_txbf) &&
			(preamble_type != WLC_GF_PREAMBLE) &&
			!SCB_ISMULTI(scb) &&
			(parms.type == FC_TYPE_DATA) &&
			!WLC_PHY_AS_80P80(wlc, wlc->chanspec)) {
			ratespec_t fbw_rspec;
			uint16 txpwr_mask, stbc_val;
			fbw_rspec = rspec;
			bfen = wlc_txbf_sel(wlc->txbf, rspec, scb, &bf_shm_index, &txpwrs);

			if (bfen) {
				/* BFM0: Provide alternative tx info if phyctl has bit 3
				 * (BFM) bit
				 * set but ucode has to tx with BFM cleared.
				 */
				if (D11AC2_PHY_SUPPORT(wlc)) {
					/* acphy2 mac/phy interface */
					txpwr_mask = D11AC2_BFM0_TXPWR_MASK;
					stbc_val = D11AC2_BFM0_STBC;
				} else {
					txpwr_mask = BFM0_TXPWR_MASK;
					stbc_val = BFM0_STBC;
				}
				rate_hdr->Bfm0 =
					((uint16)(txpwrs.pbw[((rspec & WL_RSPEC_BW_MASK) >>
					WL_RSPEC_BW_SHIFT) - BW_20MHZ][TXBF_OFF_IDX])) & txpwr_mask;
				rate_hdr->Bfm0 |=
					(uint16)(RSPEC_ISSTBC(rspec) ? stbc_val : 0);
			}

			txpwr_bfsel = bfen ? 1 : 0;

			if (fbw != FBW_BW_INVALID) {
				fbw_rspec = (fbw_rspec & ~WL_RSPEC_BW_MASK) |
				((fbw == FBW_BW_40MHZ) ?  WL_RSPEC_BW_40MHZ : WL_RSPEC_BW_20MHZ);
				if (!RSPEC_ISSTBC(fbw_rspec))
					fbw_bfen = wlc_txbf_sel(wlc->txbf, fbw_rspec, scb,
						&bf_shm_index, &txpwrs);
				else
					fbw_bfen = bfen;

				rate_hdr->FbwInfo |= (uint16)(fbw_bfen ? FBW_TXBF : 0);
			}

			if (RSPEC_ACTIVE(wlc->band->rspec_override)) {
				wlc_txbf_applied2ovr_upd(wlc->txbf, bfen);
			}
		}
#endif /* WL_BEAMFORMING */

		/* (3) PLCP: determine PLCP header and MAC duration, fill d11txh_t */
		wlc_compute_plcp(wlc, bsscfg, rspec, parms.phylen, parms.fc, plcp);

		/* RateInfo.TxRate */
		txrate = wf_rspec_to_rate(rspec);
		rate_hdr->TxRate = htol16(txrate/500);

		/* RIFS(testing only): based on frameburst, non-CCK frames only */
		if (SCB_HT_CAP(scb) && WLC_HT_GET_FRAMEBURST(hti) &&
			WLC_HT_GET_RIFS(hti) &&
			n_prot != WLC_N_PROTECTION_MIXEDMODE && !RSPEC_ISCCK(rspec) &&
			!ETHER_ISMULTI(&parms.h->a1) && ((parms.fc & FC_KIND_MASK) ==
			FC_QOS_DATA) && (fifo < TX_BCMC_FIFO) &&
			WLC_HT_GET_FRAMEBURST_PER_AC(hti, ac)) {
			uint16 qos_field, *pqos;

			WLPKTTAG(p)->flags |= WLF_RIFS;
			mcl |= (D11AC_TXC_MBURST | D11AC_TXC_URIFS);
			use_rifs = TRUE;

			/* RIFS implies QoS frame with no-ack policy, hack the QoS field */
			pqos = (uint16 *)((uchar *)parms.h + (parms.a4 ?
				DOT11_A4_HDR_LEN : DOT11_A3_HDR_LEN));
			qos_field = ltoh16(*pqos) & ~QOS_ACK_MASK;
			qos_field |= (QOS_ACK_NO_ACK << QOS_ACK_SHIFT) & QOS_ACK_MASK;
			*pqos = htol16(qos_field);
		}

		/* mark pkt as aggregable if it is */
		if ((k == 0) && WLPKTFLAG_AMPDU(parms.pkttag) && !RSPEC_ISLEGACY(rspec)) {
			if (WLC_KEY_ALLOWS_AMPDU(key_info)) {
				WLPKTTAG(p)->flags |= WLF_MIMO;
				if (WLC_HT_GET_AMPDU_RTS(wlc->hti))
					use_rts = TRUE;
			}
		}

		if (parms.type == FC_TYPE_DATA && (fifo < TX_BCMC_FIFO)) {
			/* dont allow bursting if rts is required around each mimo frame */
			if ((WLC_HT_GET_FRAMEBURST(hti) &&
				(txrate > WLC_FRAMEBURST_MIN_RATE) &&
				WLC_HT_GET_FRAMEBURST_PER_AC(hti, ac) &&
				(!wlc->active_bidir) &&
				(wlc_ampdu_frameburst_override(wlc->ampdu_tx, scb) == FALSE) &&
				(!wme->edcf_txop[ac] || WLPKTFLAG_AMPDU(parms.pkttag))) &&
				(use_mimops_rts == FALSE)) {

				mcl |= D11AC_TXC_MBURST;
			}
		}

		/* XXX 4360: VHT rates are always in an AMPDU, setup the TxD fields here
		 * By default use Single-VHT mpdu ampdu mode.
		 */
		if (RSPEC_ISVHT(rspec)) {
			mch |= D11AC_TXC_SVHT;
		}

#if defined(WL_PROT_OBSS) && !defined(WL_PROT_OBSS_DISABLED)
		/* If OBSS protection is enabled, set CTS/RTS accordingly. */
		if (WLC_PROT_OBSS_ENAB(wlc->pub)) {
			if (WLC_PROT_OBSS_PROTECTION(wlc->prot_obss) && !use_rts && !use_cts) {
				if (ETHER_ISMULTI(&parms.h->a1)) {
					/* Multicast and broadcast pkts need CTS protection */
					use_cts = TRUE;
				} else {
					/* Unicast pkts < 80 bw need RTS protection */
					if (RSPEC_BW(rspec) < phybw) {
						use_rts = TRUE;
					}
				}
			}
		}
#endif /* WL_PROT_OBSS && !WL_PROT_OBSS_DISABLED */

		/* (5) RTS/CTS: determine RTS/CTS PLCP header and MAC duration, furnish d11txh_t */
		rate_hdr->RtsCtsControl = wlc_d11_hdrs_rts_cts(scb, bsscfg, rspec, use_rts,
			use_cts, &rts_rspec, &rts_preamble_type);
		if (use_rts) {
			mcl |= D11AC_TXC_LFRM;
		}

#ifdef WL_BEAMFORMING
		if (TXBF_ENAB(wlc->pub)) {
			bool mutx_on = ((k == 0) && (mch & D11AC_TXC_MU));
			if (bfen || mutx_on) {
				if (bf_shm_index != BF_SHM_IDX_INV) {
					rate_hdr->RtsCtsControl |= htol16((bf_shm_index <<
						D11AC_RTSCTS_BF_IDX_SHIFT));
				}
				else if (mutx_on) {
					/* Allow MU txbf even if SU explicit txbf is not allowed.
					 * If still implicit SU txbf capable and if we end up
					 * doing it, we won't have its index in the header
					 * but it is okay as ucode correctly calculates it.
					 */
					rate_hdr->RtsCtsControl |= htol16((bf_shmx_idx <<
						D11AC_RTSCTS_BF_IDX_SHIFT));
					WL_TXBF(("wl:%d %s MU txbf capable with %s SU txbf\n",
						wlc->pub->unit, __FUNCTION__,
						(bfen ? "IMPLICIT" : "NO")));
				} else {
					rate_hdr->RtsCtsControl |= htol16(D11AC_RTSCTS_IMBF);
				}
				/* Move wlc_txbf_fix_rspec_plcp() here to remove RSPEC_STBC
				 * from rspec before wlc_acphy_txctl0_calc_ex().
				 * rate_hdr->PhyTxControlWord_0 will not set D11AC2_PHY_TXC_STBC
				 * if this rate enable txbf.
				 */
				wlc_txbf_fix_rspec_plcp(wlc->txbf, &rspec, plcp, txbf_chains);
			}
		}
#endif /* WL_BEAMFORMING */
		/* PhyTxControlWord_0 */
		phyctl = wlc_acphy_txctl0_calc(wlc, rspec, preamble_type);

		if (WLC_PHY_AS_80P80(wlc, wlc->chanspec) && !RSPEC_IS160MHZ(rspec) &&
			!RSPEC_ISOFDM(rspec)) {
			phyctl = wlc_stf_d11hdrs_phyctl_txcore_80p80phy(wlc, phyctl);
		}

		WLC_PHYCTLBW_OVERRIDE(phyctl, D11AC_PHY_TXC_BW_MASK, phyctl_bwo);
#ifdef WL_PROXDETECT
		if (PROXD_ENAB(wlc->pub) && wlc_proxd_frame(wlc, parms.pkttag)) {
			/* TOF measurement pkts use only primary antenna to tx */
			wlc_proxd_tx_conf(wlc, &phyctl, &mch, parms.pkttag);
		}
#endif
#ifdef WL_BEAMFORMING
		if (TXBF_ENAB(wlc->pub) && (bfen)) {
			phyctl |= (D11AC_PHY_TXC_BFM);
#if !defined(WLTXBF_DISABLED)
		} else {
			if (wlc_txbf_bfmspexp_enable(wlc->txbf) &&
			    (preamble_type != WLC_GF_PREAMBLE) &&
			    (!RSPEC_ISSTBC(rspec)) && !RSPEC_ISLEGACY(rspec) &&
			!WLC_PHY_AS_80P80(wlc, wlc->chanspec)) {
				uint8 nss = (uint8) wlc_ratespec_nss(rspec);
				uint8 ntx = (uint8) wlc_stf_txchain_get(wlc, rspec);

				if (nss == 3 && ntx == 4) {
					txbf_uidx = 96;
				} else if (nss == 2 && ntx == 4) {
					txbf_uidx = 97;
				} else if (nss == 2 && ntx == 3) {
					txbf_uidx = 98;
				}
				/* no need to set BFM (phytxctl.B3) as ucode will do it */
			}
#endif /* !defined(WLTXBF_DISABLED) */
		}
#endif /* WL_BEAMFORMING */
		rate_hdr->PhyTxControlWord_0 = htol16(phyctl);

		/* PhyTxControlWord_1 */
		txpwr = txpwrs.pbw[((rspec & WL_RSPEC_BW_MASK) >> WL_RSPEC_BW_SHIFT) -
			BW_20MHZ][txpwr_bfsel];
		if ((FC_TYPE(parms.fc) == FC_TYPE_MNG) &&
				(FC_SUBTYPE(parms.fc) == FC_SUBTYPE_PROBE_RESP)) {
			txpwr = wlc_get_bcnprs_txpwr_offset(wlc, txpwr);
		}
#ifdef WL_LPC
		/* Apply the power index */
		if ((parms.pkttag->flags & WLF_RATE_AUTO) && LPC_ENAB(wlc) && k == 0) {
			phyctl = wlc_acphy_txctl1_calc(wlc, rspec, lpc_offset, txpwr, FALSE);
			/* Preserve the PhyCtl for later (dotxstatus) */
			wlc_scb_lpc_store_pcw(wlc->wlpci, scb, ac, phyctl);
		} else
#endif
		{
			phyctl = wlc_acphy_txctl1_calc(wlc, rspec, 0, txpwr, FALSE);
		}

		/* for fallback bw */
		if (fbw != FBW_BW_INVALID) {
			txpwr = txpwrs.pbw[fbw - FBW_BW_20MHZ][TXBF_OFF_IDX];
			rate_hdr->FbwInfo |= ((uint16)txpwr << FBW_BFM0_TXPWR_SHIFT);
#ifdef WL_BEAMFORMING
			if (fbw_bfen) {
				txpwr = txpwrs.pbw[fbw - FBW_BW_20MHZ][TXBF_ON_IDX];
				rate_hdr->FbwInfo |= ((uint16)txpwr << FBW_BFM_TXPWR_SHIFT);
			}
#endif
			rate_hdr->FbwInfo = htol16(rate_hdr->FbwInfo);
		}

		WLC_PHYCTLBW_OVERRIDE(phyctl, D11AC_PHY_TXC_PRIM_SUBBAND_MASK, phyctl_sbwo);
#ifdef WL_PROXDETECT
		if (PROXD_ENAB(wlc->pub) && wlc_proxd_frame(wlc, parms.pkttag)) {
			/* TOF measurement pkts use initiator's subchannel to tx */
			wlc_proxd_tx_conf_subband(wlc, &phyctl, parms.pkttag);
		}
#endif
		rate_hdr->PhyTxControlWord_1 = htol16(phyctl);

		/* PhyTxControlWord_2 */
		phyctl = wlc_acphy_txctl2_calc(wlc, rspec, txbf_uidx);
		rate_hdr->PhyTxControlWord_2 = htol16(phyctl);

		/* Avoid changing TXOP threshold based on multicast packets */
		if ((k == 0) && SCB_WME(scb) && parms.qos && wme->edcf_txop[ac] &&
			!ETHER_ISMULTI(&parms.h->a1) && !(parms.pkttag->flags & WLF_8021X)) {
			uint frag_dur, dur;

			/* WME: Update TXOP threshold */
			/*
			 * XXX should this calculation be done for null frames also?
			 * What else is wrong with these calculations?
			 */
			if ((!WLPKTFLAG_AMPDU(parms.pkttag)) && (frag == 0)) {
				int16 delta;
				uint queue_type = (fifo < TX_FIFO_HE_MU_START) ? fifo :
					((fifo - TX_FIFO_HE_MU_START) % AC_COUNT);
				BCM_REFERENCE(queue_type);

				frag_dur = wlc_calc_frame_time(wlc, rspec, preamble_type,
					parms.phylen);

				if (rts) {
					/* 1 RTS or CTS-to-self frame */
					dur = wlc_calc_cts_time(wlc, rts_rspec, rts_preamble_type);
					/* (SIFS + CTS) + SIFS + frame + SIFS + ACK */
					dur += ltoh16(rts->durid);
				} else if (use_rifs) {
					dur = frag_dur;
				} else {
					/* frame + SIFS + ACK */
					dur = frag_dur;
					dur += wlc_compute_frame_dur(wlc,
						CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
						rspec, preamble_type, 0);
				}

				/* update txop byte threshold (txop minus intraframe overhead) */
				delta = (int16)(wme->edcf_txop[ac] - (dur - frag_dur));
				if (delta >= 0) {
#ifdef WLAMSDU_TX
					if (!(WLPKTFLAG_AMSDU(parms.pkttag) &&
						(fifo == TX_AC_BE_FIFO)))
#endif
					{
						/* XXX radar 19423513 MACOS sends EAPOL pkt in
						* VO queueand since TXOP is applied for VO pkts
						* causing it to be fragmented and to avoid it this
						* check is added
						*/
						if (!((parms.pkttag)->flags & WLF_8021X)) {
							uint newfragthresh =
								wlc_calc_frame_len(wlc, rspec,
								preamble_type, (uint16)delta);
							/* range bound the fragthreshold */
							newfragthresh = MAX(newfragthresh,
								DOT11_MIN_FRAG_LEN);
							newfragthresh = MIN(newfragthresh,
								wlc->usr_fragthresh);
							/* update the fragthr and do txc update */
							if (wlc->fragthresh[ac] !=
								(uint16)newfragthresh) {
									wlc_fragthresh_set(wlc,
									ac,
									(uint16)newfragthresh);
							}

						}
					}
				} else {
					WL_ERROR(("wl%d: %s txop invalid for rate %d\n",
						wlc->pub->unit, wlc_fifo_names(queue_type),
						RSPEC2KBPS(rspec)));
				}

				if (CAC_ENAB(wlc->pub) &&
					fifo <= TX_AC_VO_FIFO) {
					/* update cac used time */
					if (wlc_cac_update_used_time(wlc->cac, ac, dur, scb))
						WL_ERROR(("wl%d: ac %d: txop exceeded allocated TS"
							"time\n", wlc->pub->unit, ac));
				}

				/*
				 * FIXME: The MPDUs of the next transmitted MSDU after
				 * rate drops or RTS/CTS kicks in may exceed
				 * TXOP. Without tearing apart the transmit
				 * path---either push rate and RTS/CTS decisions much
				 * earlier (hard), allocate fragments just in time
				 * (harder), or support late refragmentation (even
				 * harder)---it's too difficult to fix this now.
				 */
				if (dur > wme->edcf_txop[ac])
					WL_INFORM(("wl%d: %s txop exceeded phylen "
						"%d/%d dur %d/%d\n",
						wlc->pub->unit, wlc_fifo_names(queue_type),
						parms.phylen, wlc->fragthresh[ac], dur,
						wme->edcf_txop[ac]));
			}
		} else if ((k == 0) && SCB_WME(scb) &&
			parms.qos && CAC_ENAB(wlc->pub) &&
			fifo <= TX_AC_VO_FIFO) {
			uint dur;
			if (rts) {
				/* 1 RTS or CTS-to-self frame */
				dur = wlc_calc_cts_time(wlc, rts_rspec, rts_preamble_type);
				/* (SIFS + CTS) + SIFS + frame + SIFS + ACK */
				dur += ltoh16(rts->durid);
			} else {
				/* frame + SIFS + ACK */
				dur = wlc_calc_frame_time(wlc, rspec, preamble_type,
					parms.phylen);
				dur += wlc_compute_frame_dur(wlc,
					CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
					rspec, preamble_type, 0);
			}
			/* update cac used time */
			if (wlc_cac_update_used_time(wlc->cac, ac, dur, scb))
				WL_ERROR(("wl%d: ac %d: txop exceeded allocated TS time\n",
					wlc->pub->unit, ac));
		}
		/* Store the final rspec back into the ratesel_rates */
		ratesel_rates.rspec[k] = rspec;
	} /* rate loop ends */

	/* Mark last rate */
	rate_blk[ratesel_rates.num-1].RtsCtsControl |= htol16(D11AC_RTSCTS_LAST_RATE);

	/* record rate history here (pick final primary rspec) */
	rspec = ratesel_rates.rspec[0];
	WLCNTSCB_COND_SET(((parms.type == FC_TYPE_DATA) &&
		((FC_SUBTYPE(parms.fc) != FC_SUBTYPE_NULL) &&
			(FC_SUBTYPE(parms.fc) != FC_SUBTYPE_QOS_NULL))),
			scb->scb_stats.tx_rate, rspec);

#ifdef WLC_MEDIAN_RATE_REPORT
	if (!ETHER_ISMULTI(&parms.h->a1)) {
		WLCNTSCB_COND_SET(((parms.type == FC_TYPE_DATA) &&
			((FC_SUBTYPE(parms.fc) != FC_SUBTYPE_NULL) &&
				(FC_SUBTYPE(parms.fc) != FC_SUBTYPE_QOS_NULL))),
				scb->scb_rate_stats.tx_rate_log[WLCNT_SCB_TX_RATE_INDEX(scb)],
				rspec);
		WLCNTSCB_COND_INCR_LIMIT(((parms.type == FC_TYPE_DATA) &&
			((FC_SUBTYPE(parms.fc) != FC_SUBTYPE_NULL) &&
				(FC_SUBTYPE(parms.fc) != FC_SUBTYPE_QOS_NULL))),
				scb->scb_rate_stats.tx_rate_index, (NRATELOG-1));
	}
#endif /* WLC_MEDIAN_RATE_REPORT */

	/* record rate history after the txbw is valid */
	if (rspec_history) {
		/* update per bsscfg tx rate */
		bsscfg->txrspec[bsscfg->txrspecidx][0] = rspec;
		bsscfg->txrspec[bsscfg->txrspecidx][1] = (uint8) nfrags;
		bsscfg->txrspecidx = (bsscfg->txrspecidx+1) % NTXRATE;

		WLCNTSCBSET(scb->scb_stats.tx_rate_fallback,
		            ratesel_rates.rspec[ratesel_rates.num - 1]);
	}
	WLPKTTAG(p)->rspec = rspec;

	/* Compute and fill "per packet info" / short TX Desc (24 bytes) */
	wlc_fill_txd_per_pkt_info(wlc, p, scb, &txh->PktInfo,
		mcl, mch, &parms, frameid, D11AC_TXH_LEN, 0);

	cache_info = WLC_TXD_CACHE_INFO_GET(txh, wlc->pub->corerev);

	/* Compute and fill "per cache info" for TX Desc (24 bytes) */
	wlc_fill_txd_per_cache_info(wlc, bsscfg, scb, p, parms.pkttag,
		cache_info, mutx_pkteng_on);

	hdrptr = (d11txhdr_t *)PKTDATA(wlc->osh, p);

#ifdef WLTOEHW
	/* add tso-oe header, if capable and toe module is not bypassed */
	if (wlc->toe_capable && !wlc->toe_bypass)
		wlc_toe_add_hdr(wlc, p, scb, key_info, nfrags, &txh_off);
#endif

	if ((key_info) && (key_info->algo != CRYPTO_ALGO_OFF)) {
		PKTPULL(wlc->osh, p, txh_off + D11AC_TXH_LEN);
		if (wlc_key_prep_tx_mpdu(key, p, hdrptr) != BCME_OK) {
			WL_WSEC(("wl%d: %s: wlc_key_prep_tx_mpdu error\n",
				wlc->pub->unit, __FUNCTION__));
		}
		PKTPUSH(wlc->osh, p, txh_off + D11AC_TXH_LEN);
	}

	/* With d11 hdrs on, mark the packet as TXHDR */
	WLPKTTAG(p)->flags |= WLF_TXHDR;

	return (frameid);
} /* wlc_d11hdrs_rev40 */

#ifdef WL_PROXDETECT
static INLINE uint16
wlc_proxd_d11_rev128_phy_fixed_txctl_calc_word_2(wlc_info_t *wlc, ratespec_t rspec)
{
	uint16 phytxant = TXCHAIN_DEF;
	uint16 mask = PHY_TXC_ANT_MASK;

	/* use single chain for FTM frames */
	if (WLCISNPHY(wlc->band) || WLCISACPHY(wlc->band) ||
		WLCISHECAPPHY(wlc->band)) {
		phytxant = wlc_proxd_core_get(wlc);
		if (WLCISHECAPPHY(wlc->band)) {
			/* antenna cfg is in upper nibble, coremask in lower nibble
			 * move antenna cfg to next byte
			 */
			phytxant |= (phytxant & D11_REV80_PHY_TXC_ANT_CONFIG_MASK) <<
				D11_REV80_PHY_TXC_ANT_CONFIG_SHIFT;
			mask = D11_REV80_PHY_TXC_ANT_CORE_MASK;
		} else if (WLCISACPHY(wlc->band))
			mask = D11AC_PHY_TXC_ANT_MASK;
		else
			mask = PHY_TXC_HTANT_MASK;
	}

	phytxant |= phytxant & mask;
	return phytxant;
}
#endif /* WL_PROXDETECT */

/**
 * Function to fill word 2 of TX Ctl Word :
 * - bits[0-8] Core Mask (Using only lower 4-bits for 4369a0)
 * - bits[9-15] Antennae Configuration (Using only lower 4-bits for 4369a0)
 */
static INLINE uint16
wlc_d11_rev128_phy_fixed_txctl_calc_word_2(wlc_info_t *wlc, ratespec_t rspec)
{
	return wlc_stf_d11hdrs_phyctl_txant(wlc, rspec);
}

/**
 * Function to fill word 1 (bytes 2-3) of TX Ctl Word :
 * 1. Bits [0-1] Packet BW
 * 2. Bits [2-4] Sub band location
 * 3. Bits [5-12] Partial Ofdma Sub band (not to be filled by SW)
 * 4. Bit [13] DynBW present
 * 5. Bit [14] DynBw Mode
 * 6. Bit [15] MU
 */
static INLINE uint16
wlc_d11_rev128_phy_fixed_txctl_calc_word_1(wlc_info_t *wlc, scb_t *scb,
	ratespec_t rspec, bool is_mu)
{
	uint16 bw, sb;
	uint16 phyctl = 0;
#ifdef WL_PROXDETECT
	chanspec_t chspec;
#endif
	BCM_REFERENCE(scb);

	/* bits [0:1] - 00/01/10/11 => 20/40/80/160 */
	switch (RSPEC_BW(rspec)) {
		case WL_RSPEC_BW_20MHZ:
			bw = D11_REV80_PHY_TXC_BW_20MHZ;
		break;
		case WL_RSPEC_BW_40MHZ:
			bw = D11_REV80_PHY_TXC_BW_40MHZ;
		break;
		case WL_RSPEC_BW_80MHZ:
			bw = D11_REV80_PHY_TXC_BW_80MHZ;
		break;
		case WL_RSPEC_BW_160MHZ:
			bw = D11_REV80_PHY_TXC_BW_160MHZ;
		break;
		default:
			ASSERT(0);
			bw = D11_REV80_PHY_TXC_BW_20MHZ;
		break;
	}
	phyctl |= bw;

	/**
	 * bits [2:4] Sub band location
	 * Primary Subband Location: b 2-4
	 * LLL ==> 000
	 * LLU ==> 001
	 * ...
	 * UUU ==> 111
	 */
#ifdef WL_PROXDETECT
	if (scb && SCB_PROXD(scb)) {
		chspec = (chanspec_t)scb->aid;
		sb = wlc_proxd_get_tx_subband(wlc, chspec);
	} else
#endif
	sb = ((wlc->chanspec & WL_CHANSPEC_CTL_SB_MASK) >> WL_CHANSPEC_CTL_SB_SHIFT);
	phyctl |= ((sb >> ((RSPEC_BW(rspec) >> WL_RSPEC_BW_SHIFT) - 1)) <<
	           D11_REV80_PHY_TXC_SB_SHIFT);

	/* Note : Leave DynBw present and DynBw mode fields as 0 */
	if (is_mu) {
		phyctl |= D11_REV80_PHY_TXC_MU;
	}

	return phyctl;
} /* wlc_d11_rev128_phy_fixed_txctl_calc_word_1 */

/**
 * Function to fill Byte 1 of TX Ctl Word :
 * 1. Bits [0-7] MCS +NSS
 * 2. Bit [8] STBC
 */
static INLINE uint8
wlc_d11_rev128_phy_fixed_txctl_calc_byte_1(wlc_info_t *wlc, ratespec_t rspec)
{
	uint nss = wlc_ratespec_nss(rspec);
	uint mcs;
	uint8 rate, phyctl_byte;

	ASSERT(D11REV_GE(wlc->pub->corerev, 128));

	/* mcs+nss: bits [0-7] of PhyCtlWord Byte 1  */
	if (RSPEC_ISHT(rspec)) {
		/* for 11n: B[0:5] for mcs[0:32] */
		if (WLPROPRIETARY_11N_RATES_ENAB(wlc->pub)) {
			mcs = rspec & D11_REV80_PHY_TXC_11N_MCS_MASK;
		} else {
			mcs = rspec & WL_RSPEC_HT_MCS_MASK;
		}
		phyctl_byte = (uint8)mcs;
	} else if (RSPEC_ISHE(rspec)) {
		/* for 11ax: B[0:3] for mcs[0-9], B[4:5] for n_ss[1-4]
		(0 = 1ss, 1 = 2ss, 2 = 3ss, 3 = 4ss)
		*/
		mcs = rspec & WL_RSPEC_HE_MCS_MASK;
		ASSERT(mcs <= WLC_MAX_HE_MCS);
		phyctl_byte = (uint8)mcs;
		ASSERT(nss <= 8);
		phyctl_byte |= ((nss-1) << D11_REV80_PHY_TXC_11AX_NSS_SHIFT);
	} else if (RSPEC_ISVHT(rspec)) {
		/* for 11ac: B[0:3] for mcs[0-9], B[4:5] for n_ss[1-4]
			(0 = 1ss, 1 = 2ss, 2 = 3ss, 3 = 4ss)
		*/
		mcs = rspec & WL_RSPEC_VHT_MCS_MASK;
		ASSERT(mcs <= WLC_MAX_VHT_MCS);
		phyctl_byte = (uint8)mcs;
		ASSERT(nss <= 8);
		phyctl_byte |= ((nss-1) << D11_REV80_PHY_TXC_11AC_NSS_SHIFT);
	} else {
		ASSERT(RSPEC_ISLEGACY(rspec));
		rate = (uint8)RSPEC2RATE(rspec);

		if (RSPEC_ISOFDM(rspec)) {
			/* phy control rate field must have same value as .11a plcp rate field */
			switch (rate) {
				case WLC_RATE_6M: phyctl_byte = 0xB; break;
				case WLC_RATE_9M: phyctl_byte = 0xF; break;
				case WLC_RATE_12M: phyctl_byte = 0xA; break;
				case WLC_RATE_18M: phyctl_byte = 0xE; break;
				case WLC_RATE_24M: phyctl_byte = 0x9; break;
				case WLC_RATE_36M: phyctl_byte = 0xD; break;
				case WLC_RATE_48M: phyctl_byte = 0x8; break;
				case WLC_RATE_54M: phyctl_byte = 0xC; break;
				default: phyctl_byte = 0x0; ASSERT(0); break;
			};
		} else {
			/* 11b phyrate control value defined in txCtrlWord_plcp_ver6.0 for rev128 */
			switch (rate) {
				case WLC_RATE_1M: phyctl_byte = 0x0A; break;
				case WLC_RATE_2M: phyctl_byte = 0x14; break;
				case WLC_RATE_5M5: phyctl_byte = 0x37; break;
				case WLC_RATE_11M: phyctl_byte = 0x6E; break;
				default: phyctl_byte = 0x0; ASSERT(0); break;
			};
		}
	}

	if (D11AC2_PHY_SUPPORT(wlc)) {
		if (WLPROPRIETARY_11N_RATES_ENAB(wlc->pub)) {
			if (RSPEC_ISHT(rspec) && (rspec & WL_RSPEC_HT_MCS_MASK)
				>= WLC_11N_FIRST_PROP_MCS) {
				phyctl_byte |= D11_REV80_PHY_TXC_11N_PROP_MCS;
			}
		}
	}

	/* STBC : Bit [8] of PhyCtlWord Byte 3 */
	if (D11AC2_PHY_SUPPORT(wlc)) {
		/* STBC */
		if (RSPEC_ISSTBC(rspec)) {
			/* set 8th bit */
			phyctl_byte |= D11_REV80_PHY_TXC_STBC;
		}
	}

	return phyctl_byte;
} /* wlc_d11_rev128_phy_fixed_txctl_calc_byte_1 */

/**
* Function to fill word 0 (bytes 0-1) of TX Ctl Word :
* 1. Bits [0-2] FrameType
* 2. Bits [3-4] HE Format
* 3. Bits [5] Reserved
* 4. Bit [6] Sounding
* 5. Bit [7] Preamble info
* 6. Bit [8-14] MCS+NSS
* 7. Bit [15] STBC
*/
static INLINE uint16
wlc_axphy_fixed_txctl_calc_word_0(wlc_info_t *wlc, ratespec_t rspec, uint8 preamble_type)
{
	uint16 phyctl = 0;

	/* ---- [2:0] Compute and fill PhyCtlWord Byte Pos (offset from length field) 0  ---- */
	/* Compute Frame Format */
	phyctl = WLC_RSPEC_ENC2FT(rspec);

	/* [4:3] HE Format field not to be populated by SW.
	 *
	 * HEControl field in the short TXD shall be used by the ucode,
	 * for determining the method of TX for the given packet.
	 *
	 * [5] Reserved
	 */
	/* [6] Not Sounding
	 * TODO: following fields need to be computed and filled:
	 * D11AX_PHY_TXC_NON_SOUNDING - bit 2 - is this sounding frame or not?
	 * hard-code to NOT
	 */
	phyctl |= (D11_REV80_PHY_TXC_NON_SOUNDING);

#ifdef WL11AX
	phyctl |= RSPEC_HE_FORMAT(wlc_get_heformat(wlc));
#endif /* WL11AX */

	/* [7] Preamble info */
	if ((preamble_type == WLC_SHORT_PREAMBLE) ||
	    (preamble_type == WLC_GF_PREAMBLE)) {
		ASSERT((preamble_type == WLC_GF_PREAMBLE) || !RSPEC_ISHT(rspec));
		phyctl |= D11_REV80_PHY_TXC_SHORT_PREAMBLE;
		WLCNTINCR(wlc->pub->_cnt->txprshort);
	}

	/* ---- [1] Compute and fill PhyCtlWord Byte Pos (offset from length field) 1  ---- */
	phyctl |= ((uint16)wlc_d11_rev128_phy_fixed_txctl_calc_byte_1(wlc, rspec) <<
					D11_REV80_PHY_TXC_MCS_NSS_SHIFT);

	return phyctl;
} /* wlc_axphy_fixed_txctl_calc_word_0 */

/**
 * CoreRev 128 (AX AP) TXH related functions
 */

/**
 * Function to compute and fill the link mem entry (Rev 128), called by RateLinkMem module.
 */
void
wlc_tx_fill_link_entry(wlc_info_t *wlc, scb_t *scb, d11linkmem_entry_t *link_entry)
{
	wlc_bsscfg_t *bsscfg;
	wlc_key_t *key = NULL;
	wlc_key_info_t key_info;
	int bss_idx = 0;
	uint8 bw, link_bw;

	ASSERT(scb != NULL);
	ASSERT(link_entry != NULL);
	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);
	BCM_REFERENCE(bsscfg);

	memset(link_entry, 0, sizeof(*link_entry));

#ifdef WL_SPP_AMSDU
	if (SCB_AMSDU(scb) && (scb->flags & SCB_AMSDU_SPP)) {
		link_entry->BFIConfig1 |= 1 << C_LTX_SPP_AMSDU_EN_NBIT;
	}
#endif /* WL_SPP_AMSDU */
#ifdef WLMCNX
	if (MCNX_ENAB(wlc->pub)) {
		bss_idx = wlc_mcnx_BSS_idx(wlc->mcnx, bsscfg);
		ASSERT((uint)bss_idx < M_P2P_BSS_MAX);
	}
#ifdef MBSS
	else
#endif /* MBSS */
#endif /* WLMCNX */
#ifdef MBSS
	if (MBSS_ENAB(wlc->pub)) {
		bss_idx = wlc_mbss_bss_idx(wlc, bsscfg);
		ASSERT(bss_idx < WLC_MAX_AP_BSS(wlc->pub->corerev));
		if (!BSSCFG_IS_PRIMARY(bsscfg)) {
			link_entry->BssIdx = 1 << D11_REV128_SECBSS_NBIT;
		}
	}
#endif /* MBSS */
	link_entry->BssIdx |= (bss_idx & D11_REV128_BSSIDX_MASK);
	if (BSSCFG_AP(bsscfg)) {
		link_entry->StaID_IsAP = D11_REV128_STAID_ISAP;
		if (scb != WLC_RLM_BSSCFG_LINK_SCB(wlc, bsscfg)) {
			link_entry->StaID_IsAP |= htol16(scb->aid & D11_REV128_STAID_MASK);
		}
	} else if (scb != WLC_RLM_BSSCFG_LINK_SCB(wlc, bsscfg)) {
		link_entry->StaID_IsAP = htol16(bsscfg->AID & D11_REV128_STAID_MASK);
	}

#ifdef WL11AX
	if (SCB_HE_CAP(scb)) {
		/* BFIConfig1 also gets updated */
		wlc_he_fill_link_entry(wlc->hei, bsscfg, scb, link_entry);
	} else
#endif /* WL11AX */
	/* Setting OMI link BW if not configured by HE */
	if (!SCB_INTERNAL(scb)) {
		bw = wlc_scb_ratesel_get_link_bw(wlc, scb);

		if (bw == BW_160MHZ) {
			link_bw = D11_REV128_BW_160MHZ;
		} else if (bw == BW_80MHZ) {
			link_bw = D11_REV128_BW_80MHZ;
		} else if (bw == BW_40MHZ) {
			link_bw = D11_REV128_BW_40MHZ;
		} else {
			link_bw = D11_REV128_BW_20MHZ;
		}
		setbits((uint8 *)&link_entry->OMI, sizeof(link_entry->OMI), C_LTX_OMI_BW_NBIT,
			NBITSZ(C_LTX_OMI_BW), link_bw);
	}

#if defined(WL_BEAMFORMING)
	if (TXBF_ENAB(wlc->pub)) {
		wlc_txbf_fill_link_entry(wlc->txbf, bsscfg, scb, link_entry);
	}
#endif /* WL_BEAMFORMING */
	/* TWT uses the BFIConfig1 field to specify TWT for a destination, call it now */
	if (TWT_ENAB(wlc->pub)) {
		wlc_twt_fill_link_entry(wlc->twti, scb, link_entry);
	}

	/* don't fill Buffer Status info, leave it to ucode */

#if defined(WLTEST) || defined(WLPKTENG) || defined(WL_MUPKTENG)
	if (AMPDU_ENAB(wlc->pub) && (RATELINKMEM_ENAB(wlc->pub)) &&
		(scb == WLC_RLM_SPECIAL_LINK_SCB(wlc))) {
		if (wlc->hw->pkteng_status == PKTENG_TX_BUSY) {
			wlc_ampdu_fill_link_entry_pkteng(wlc, link_entry);
#ifdef WL_MUPKTENG
		} else if (wlc_mutx_pkteng_on(wlc->mutx) == TRUE) {
			wlc_ampdu_mupkteng_fill_link_entry_info(wlc->ampdu_tx, scb,
				link_entry);
#endif /* WL_MUPKTENG */
		}
#if defined(WLTEST) || defined(WLPKTENG)
	} else if (AMPDU_ENAB(wlc->pub) && (RATELINKMEM_ENAB(wlc->pub)) &&
		wlc_test_pkteng_en(wlc->testi)) {
			wlc_ampdu_fill_link_entry_pkteng(wlc, link_entry);
#endif
	} else
#endif
	if (SCB_AMPDU(scb)) {
		/* Fill per cache info */
		wlc_ampdu_fill_link_entry_info(wlc->ampdu_tx, scb, link_entry);
	}

	/* no need to memset key_info, handled by wlc_keymgmt itself */
	if (WSEC_ENABLED(bsscfg->wsec)) {
		/* Use a paired key or primary group key if present */
		key = wlc_keymgmt_get_tx_key(wlc->keymgmt, scb, bsscfg, &key_info);
		if (key_info.algo != CRYPTO_ALGO_OFF) {
			WL_WSEC(("wl%d.%d: %s: using pairwise key\n",
				WLCWLUNIT(wlc), WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
		} else {
			key = wlc_keymgmt_get_bss_tx_key(wlc->keymgmt, bsscfg, FALSE, &key_info);
			if (key_info.algo != CRYPTO_ALGO_OFF) {
				WL_WSEC(("wl%d.%d: %s: using group key\n",
					WLCWLUNIT(wlc), WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
			} else {
				WL_INFORM(("wl%d.%d: %s: no key for encryption\n",
					WLCWLUNIT(wlc), WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
			}
		}
	} else {
		key = wlc_keymgmt_get_bss_key(wlc->keymgmt, bsscfg, WLC_KEY_ID_INVALID, &key_info);
		ASSERT(key != NULL);
	}
	wlc_key_fill_link_entry(key, scb, link_entry);

	if (HE_ENAB(wlc->pub)) {
		wlc_musched_fill_link_entry(wlc->musched, bsscfg, scb, link_entry);
	}
}

/**
 * Function to compute and fill PhyTxControl for rev128; reuse rev80 code.
 */
static INLINE void
wlc_tx_d11_rev128_phy_txctl_calc(wlc_info_t *wlc, scb_t *scb, wlc_bsscfg_t *bsscfg,
	d11ratemem_rev128_rate_t *rate, ratespec_t rspec, uint8 preamble_type,
	uint8 curpwr, txpwr204080_t txpwrs)
{
	uint16 phyctl_word;
	int i;

	phyctl_word = wlc_axphy_fixed_txctl_calc_word_0(wlc, rspec, preamble_type);
	rate->TxPhyCtl[0] = htol16(phyctl_word);

	phyctl_word = wlc_d11_rev128_phy_fixed_txctl_calc_word_1(wlc, scb, rspec, FALSE);
	rate->TxPhyCtl[1] = htol16(phyctl_word);

#ifdef WL_PROXDETECT
	if (SCB_PROXD(scb)) {
		phyctl_word = wlc_proxd_d11_rev128_phy_fixed_txctl_calc_word_2(wlc, rspec);
	}
	else
#endif /* WL_PROXDETECT */
	phyctl_word = wlc_d11_rev128_phy_fixed_txctl_calc_word_2(wlc, rspec);
	rate->TxPhyCtl[2] = htol16(phyctl_word);

	/* Configure txpwr for phyctl 6/7 */
	curpwr = SCALE_5_1_TO_5_2_FORMAT(curpwr);
	rate->TxPhyCtl[3] = rate->TxPhyCtl[4] = htol16((curpwr << PHYCTL_TXPWR_CORE1_SHIFT) |
		curpwr);

	/* Configure txpwr_bw and FbwPwr */
	for (i = 0; i < D11_REV128_RATEENTRY_NUM_BWS; i++) {
		rate->txpwr_bw[i] = SCALE_5_1_TO_5_2_FORMAT(txpwrs.pbw[i][0]);
		if (i < D11_REV128_RATEENTRY_FBWPWR_WORDS) {
			rate->FbwPwr[i] = htol16((SCALE_5_1_TO_5_2_FORMAT(txpwrs.pbw[i][1]) << 8) |
				SCALE_5_1_TO_5_2_FORMAT(txpwrs.pbw[i][0]));
		}
	}
} /* wlc_tx_d11_rev128_phy_txctl_calc */

/**
 * Internal function to compute and fill a single rate info block (rev128)
 */
static INLINE ratespec_t
wlc_tx_fill_rate_info_block_internal(wlc_info_t *wlc, scb_t *scb,
	d11ratemem_rev128_rate_t *rate_info_block, ratespec_t rspec,
	uint16 type, wlc_key_info_t *key_info, bool *use_rts, bool *use_cts, uint k)
{
	wlc_ht_info_t *hti = wlc->hti;
	wlc_bsscfg_t *bsscfg;
	uint8 *plcp = rate_info_block->plcp;
	uint8 sgi_tx;
	uint8 preamble_type = WLC_LONG_PREAMBLE;
	wl_tx_chains_t txbf_chains = 0;
	uint8 fbw = FBW_BW_INVALID; /* fallback bw */
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
	uint8 bf_shm_index = BF_SHM_IDX_INV;
#endif /* WL_BEAMFORMING */
	bool bfen = FALSE;
	ratespec_t rts_rspec = 0;
	uint8 rts_preamble_type;
	bool rspec_legacy;
	bool fbw_bfen = FALSE;
	bool g_prot = FALSE;
	int8 n_prot;
	txpwr204080_t txpwrs, txpwrs_sdm;
	int8 curbw;
	uint8 curpwr;
	uint16 fc;

	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);

	if (BAND_2G(wlc->band->bandtype)) {
		g_prot = WLC_PROT_G_CFG_G(wlc->prot_g, bsscfg);
	}
	n_prot = WLC_PROT_N_CFG_N(wlc->prot_n, bsscfg);

	rspec_legacy = RSPEC_ISLEGACY(rspec);

	if (N_ENAB(wlc->pub)) {
		uint32 mimo_txbw;

		mimo_txbw = wlc_d11hdrs_rev40_determine_mimo_txbw(wlc, scb,
			bsscfg, rspec);
#if WL_HT_TXBW_OVERRIDE_ENAB
		if (CHSPEC_IS40(wlc->chanspec) || CHSPEC_IS80(wlc->chanspec)) {
			int8 txbw_override_idx;
			WL_HT_TXBW_OVERRIDE_IDX(hti, rspec, txbw_override_idx);

			if (txbw_override_idx >= 0) {
				mimo_txbw = txbw2rspecbw[txbw_override_idx];
			}
		}
#endif /* WL_HT_TXBW_OVERRIDE_ENAB */
#ifdef WL_OBSS_DYNBW
		if (WLC_OBSS_DYNBW_ENAB(wlc->pub)) {
			wlc_obss_dynbw_tx_bw_override(wlc->obss_dynbw, bsscfg, &mimo_txbw);
		}
#endif /* WL_OBSS_DYNBW */

		if (!SCB_PROXD(scb)) {
			rspec &= ~WL_RSPEC_BW_MASK;
			rspec |= mimo_txbw;
		}
	} else {
		/* !N_ENAB */
		/* Set ctrlchbw as 20Mhz */
		ASSERT(rspec_legacy == TRUE);
		rspec &= ~WL_RSPEC_BW_MASK;
		rspec |= WL_RSPEC_BW_20MHZ;
	}
	/* bw has been decided, get bw */
	curbw = ((rspec & WL_RSPEC_BW_MASK) >> WL_RSPEC_BW_SHIFT) - 1;
	ASSERT(curbw >= WL_BW_20MHZ && curbw <= WL_BW_160MHZ);

	if (N_ENAB(wlc->pub)) {
		if (!RSPEC_ACTIVE(wlc->band->rspec_override)) {
			sgi_tx = WLC_HT_GET_SGI_TX(hti);
			switch (sgi_tx) {
			case OFF: /* 0 */
				rspec &= ~WL_RSPEC_GI_MASK;
				if (RSPEC_ISHE(rspec)) {
					/* choose 2x_LTF_GI_1_6 as long GI (for now) */
					rspec |= HE_GI_TO_RSPEC(WL_RSPEC_HE_2x_LTF_GI_1_6us);
				}
				break;
			case ON : /* 1 */
			case WL_HEGI_VAL(WL_RSPEC_HE_2x_LTF_GI_0_8us): /* 3 */
			case WL_HEGI_VAL(WL_RSPEC_HE_2x_LTF_GI_1_6us): /* 4 */
			case WL_HEGI_VAL(WL_RSPEC_HE_4x_LTF_GI_3_2us): /* 5 */
				rspec &= ~WL_RSPEC_GI_MASK;
				if (RSPEC_ISHT(rspec) || RSPEC_ISVHT(rspec)) {
					rspec |= WL_RSPEC_SGI;
				} else if (RSPEC_ISHE(rspec)) {
					uint8 hegi;
					if (sgi_tx == ON) {
						hegi = WL_RSPEC_HE_2x_LTF_GI_0_8us;
					} else {
						/* get hegi value bye subtract the delta */
						hegi = sgi_tx - WL_SGI_HEGI_DELTA;
					}

					rspec |= HE_GI_TO_RSPEC(hegi);
				}
				break;
			default: /* AUTO */
				if (RSPEC_ISHE(rspec) && (rspec & WL_RSPEC_SGI)) {
					rspec &= ~WL_RSPEC_GI_MASK;
					/* 43684 doesn't support 1x_LTF_GI_0_8us */
					rspec |= HE_GI_TO_RSPEC(WL_RSPEC_HE_2x_LTF_GI_0_8us);
				}
				break;
			}

			ASSERT(!(rspec & WL_RSPEC_LDPC));
			rspec &= ~WL_RSPEC_LDPC;
			rspec &= ~WL_RSPEC_STBC;

			/* LDPC */
			if (wlc->stf->ldpc_tx == ON ||
				(((RSPEC_ISHE(rspec) && SCB_HE_LDPC_CAP(wlc->hei, scb)) ||
				(RSPEC_ISVHT(rspec) && SCB_VHT_LDPC_CAP(wlc->vhti, scb)) ||
				(RSPEC_ISHT(rspec) && SCB_HT_LDPC_CAP(scb))) &&
				wlc->stf->ldpc_tx == AUTO) ||
#ifdef WL_PROXDETECT
				(PROXD_ENAB(wlc->pub) && SCB_PROXD(scb) && RSPEC_ISHE(rspec)) ||
#endif /* WL_PROXDETECT */
				FALSE) {
				if (!rspec_legacy) {
					rspec |= WL_RSPEC_LDPC;
#ifdef WL_PROXDETECT
					/* Disable LDPC for non-HE FTM frames */
					if (PROXD_ENAB(wlc->pub) && SCB_PROXD(scb) &&
						!RSPEC_ISHE(rspec)) {
						rspec &= ~WL_RSPEC_LDPC;
					}
#endif /* WL_PROXDETECT */
				}
			}

			/* STBC */
			if ((wlc_ratespec_nsts(rspec) == 1) && (!rspec_legacy)) {
				if (WLC_IS_STBC_TX_FORCED(wlc) ||
				  ((RSPEC_ISHT(rspec) && WLC_STF_SS_STBC_HT_AUTO(wlc, scb)) ||
				   (RSPEC_ISVHT(rspec) && WLC_STF_SS_STBC_VHT_AUTO(wlc, scb)) ||
				   (RSPEC_ISHE(rspec) && WLC_STF_SS_STBC_HE_AUTO(wlc, scb)))) {
#ifdef WL_PROXDETECT
					if (!(PROXD_ENAB(wlc->pub) && SCB_PROXD(scb)))
#endif /* WL_PROXDETECT */
						rspec |= WL_RSPEC_STBC;
				}
			}
		}
		if (RSPEC_ISHE(rspec) && ((rspec & WL_RSPEC_LDPC) == 0) && (curbw > WL_BW_20MHZ)) {
			/* 802.11ax requires LDPC on for BW > 20MHz */
			WL_ERROR(("HE fixup: force LDPC\n"));
			rspec |= WL_RSPEC_LDPC;
		}

		/* Determine the HT frame format, HT_MM or HT_GF */
		if (RSPEC_ISHT(rspec)) {
			int nsts = wlc_ratespec_nsts(rspec);
			preamble_type = wlc_prot_n_preamble(wlc, scb);
			/* XXX 4360: might need the same 20in40 check
			 * and protection for VHT case
			 */
			if (n_prot == WLC_N_PROTECTION_20IN40 && RSPEC_IS40MHZ(rspec)) {
				*use_cts = TRUE;
			}

			/* XXX 4360: might need the same mimops check
			 * for VHT case
			 */
			/* if the mcs is multi stream check if it needs
			 * an rts
			 */
			if (nsts > 1) {
				if (WLC_STF_GET_SCB_MIMOPS_ENAB(wlc->stf, scb) &&
				    WLC_STF_GET_SCB_MIMOPS_RTS_ENAB(wlc->stf, scb)) {
					*use_rts = TRUE;
				}
			}

			/* if SGI is selected, then forced mm for single
			 * stream
			 * (spec section 9.16, Short GI operation)
			 */
			if (RSPEC_ISSGI(rspec) && nsts == 1) {
				preamble_type = WLC_MM_PREAMBLE;
			}
		} else {
			/* VHT & HE always uses MM preamble */
			if (RSPEC_ISVHT(rspec) || RSPEC_ISHE(rspec)) {
				preamble_type = WLC_MM_PREAMBLE;
			}
		}
	}

	if (wlc->pub->_dynbw == TRUE) {
		fbw = wlc_tx_dynbw_fbw(wlc, scb, rspec);
	}

	/* - FBW_BW_20MHZ to make it 0-index based */
	rate_info_block->FbwCtl = (((fbw - FBW_BW_20MHZ) & FBW_BW_MASK) << FBW_BW_SHIFT);

	/* (2) PROTECTION, may change rspec */
#ifdef WL_PROXDETECT
	if (PROXD_ENAB(wlc->pub) && SCB_PROXD(scb)) {
		*use_rts = TRUE;
	}
#endif /* WL_PROXDETECT */

	if ((wlc->band->gmode && g_prot) && (RSPEC_ISOFDM(rspec) ||
		((RSPEC_ISHT(rspec) || RSPEC_ISVHT(rspec) || RSPEC_ISHE(rspec)) && n_prot))) {
		/* Use protection mechanisms on unfragmented frames */
		*use_cts = TRUE;
	}

	/* calculate minimum rate */
	ASSERT(VALID_RATE_DBG(wlc, rspec));

	/* fix the preamble type for non-MCS rspec/rspec-fallback
	 * If SCB is short preamble capable and and shortpreamble is enabled
	 * and rate is NOT 1M or Override is NOT set to LONG preamble then use SHORT
	 * preamble else use LONG preamble
	 * OFDM should bypass below preamble set, but old chips are OK since they ignore
	 * that bit XXX before 4331A0 PR79339 is fixed, restrict to CCK, which is actually
	 * correct
	 */
	if (RSPEC_ISCCK(rspec)) {
		if ((type == FC_TYPE_DATA) &&
			!((RSPEC2RATE(rspec) == WLC_RATE_1M) ||
			(bsscfg->PLCPHdr_override == WLC_PLCP_LONG))) {
			preamble_type = WLC_SHORT_PREAMBLE;
		} else {
			preamble_type = WLC_LONG_PREAMBLE;
		}
	}

	ASSERT(RSPEC_ISLEGACY(rspec) || WLC_IS_MIMO_PREAMBLE(preamble_type));

#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
	if (TXBF_ENAB(wlc->pub)) {
		/* bfe_sts_cap = 3: foursteams, 2: three streams, 1: two streams */
		uint8 bfe_sts_cap = wlc_txbf_get_bfe_sts_cap(wlc->txbf, scb);
		if (bfe_sts_cap && !wlc_txbf_bfrspexp_enable(wlc->txbf)) {
			/* Explicit TxBF: Number of txbf chains
			 * is min(#active txchains, #bfe sts + 1)
			 */
			txbf_chains = MIN((uint8)WLC_BITSCNT(wlc->stf->txchain),
					(bfe_sts_cap + 1));
		} else {
			/* bfe_sts_cap=0 indicates there is no Explicit TxBf link to this
			 * peer, and driver will probably use Implicit TxBF. Ignore the
			 * spatial_expension policy, and always use all currently enabled
			 * txcores
			 */
			txbf_chains = (uint8)WLC_BITSCNT(wlc->stf->txchain);
		}
	}
#endif /* WL_BEAMFORMING */
	/* get txpwr for bw204080 and txbf on/off */
	if (wlc_stf_get_204080_pwrs(wlc, rspec, &txpwrs, txbf_chains) != BCME_OK) {
		WL_ERROR(("wl%d:%s phyctl1 ppr returns error!\n",
			wlc->pub->unit, __FUNCTION__));
		ASSERT(0);
	}
	/* get txpwr without CDD */
	if (wlc_stf_get_204080_sdm_pwrs(wlc, rspec, &txpwrs_sdm) != BCME_OK) {
		WL_ERROR(("wl%d:%s phyctl1 ppr returns error!\n",
			wlc->pub->unit, __FUNCTION__));
		ASSERT(0);
	}

#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
	if (TXBF_ENAB(wlc->pub) &&
	        (!SCB_PROXD(scb)) &&
		(wlc->allow_txbf) &&
		(((preamble_type != WLC_GF_PREAMBLE) &&
		!SCB_ISMULTI(scb) &&
		(type == FC_TYPE_DATA) &&
		!WLC_PHY_AS_80P80(wlc, wlc->chanspec)) ||
		(scb == WLC_RLM_SPECIAL_RATE_SCB(wlc)))) {
		ratespec_t fbw_rspec;

		fbw_rspec = rspec;
		bfen = wlc_txbf_sel(wlc->txbf, rspec, scb, &bf_shm_index, &txpwrs);
		if (bfen && RSPEC_ISSTBC(rspec)) {
			/* BFM0STBC: Provide alternative tx/stbc info
			 * if ratectl has BFM bit set but ucode has to tx with BFM cleared.
			 */
			rate_info_block->RateCtl |= D11AX_RTCTL_BFM0STBC;
		}

		if (fbw != FBW_BW_INVALID) {
			fbw_rspec = (fbw_rspec & ~WL_RSPEC_BW_MASK) |
			((fbw == FBW_BW_40MHZ) ?  WL_RSPEC_BW_40MHZ : WL_RSPEC_BW_20MHZ);
			if (!RSPEC_ISSTBC(fbw_rspec))
				fbw_bfen = wlc_txbf_sel(wlc->txbf, fbw_rspec, scb,
					&bf_shm_index, &txpwrs);
			else
				fbw_bfen = bfen;

			rate_info_block->FbwCtl |= (uint16)(fbw_bfen ? FBW_TXBF : 0);
		}

		if (RSPEC_ACTIVE(wlc->band->rspec_override) &&
			(k == WLC_RLM_SPECIAL_RATE_RSPEC) &&
			(scb == WLC_RLM_SPECIAL_RATE_SCB(wlc))) {
			wlc_txbf_applied2ovr_upd(wlc->txbf, bfen);
		}
	}
#endif /* WL_BEAMFORMING */

	/* (3) PLCP: determine PLCP header and MAC duration, fill d11txh_t */
	fc = 0;
	if (SCB_A4_DATA(scb)) {
		fc = FC_FROMDS | FC_TODS;
	} else if (BSSCFG_INFRA_STA(bsscfg)) {
		fc = FC_TODS;
	} else if (BSSCFG_AP(bsscfg)) {
		fc = FC_FROMDS;
	}
	wlc_compute_plcp(wlc, bsscfg, rspec, 0, fc, plcp);

	if ((k == 0) && SCB_AMPDU(scb) && !RSPEC_ISLEGACY(rspec)) {
		if (WLC_KEY_ALLOWS_AMPDU(key_info)) {
			if (WLC_HT_GET_AMPDU_RTS(hti))
				*use_rts = TRUE;
		}
	}

#ifdef WL_PROT_OBSS
	/* If OBSS protection is enabled, set CTS/RTS accordingly.
	 * Do not set use_rts for SPECIAL_RATE_SCB.
	 */
	if (WLC_PROT_OBSS_ENAB(wlc->pub) && (scb != WLC_RLM_SPECIAL_RATE_SCB(wlc))) {
		if (WLC_PROT_OBSS_PROTECTION(wlc->prot_obss) && !(*use_rts) && !(*use_cts)) {
			ratespec_t phybw = (CHSPEC_IS8080(wlc->chanspec) ||
				CHSPEC_IS160(wlc->chanspec)) ? WL_RSPEC_BW_160MHZ :
				(CHSPEC_IS80(wlc->chanspec) ? WL_RSPEC_BW_80MHZ :
				(CHSPEC_IS40(wlc->chanspec) ? WL_RSPEC_BW_40MHZ :
				WL_RSPEC_BW_20MHZ));
			/* Unicast pkts < 80 bw need RTS protection */
			if (RSPEC_BW(rspec) < phybw) {
				*use_rts = TRUE;
			}
		}
	}
#endif /* WL_PROT_OBSS */

	/* BCMC packets will use special rate MRSPEC.
	 * These should never use rts or cts, as ucode will not invalidate the BCFID
	 * shared memory location when rts has been retried exhaustively.
	 * Probe responses are sent using hwrs_scb and will also make use of SPECIAL_RATE_SCB.
	 * Do not use rts for SPECIAL_RATE_SCB.
	 */
	ASSERT((*use_rts == FALSE) || (scb != WLC_RLM_SPECIAL_RATE_SCB(wlc)));

	/* (5) RTS/CTS: determine RTS/CTS PLCP header and MAC duration, furnish d11txh_t */
	rate_info_block->RateCtl |= wlc_d11_hdrs_rts_cts(scb, bsscfg, rspec, *use_rts,
		*use_cts, &rts_rspec, &rts_preamble_type);

#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
	if (TXBF_ENAB(wlc->pub) && (!SCB_PROXD(scb))) {
		uint8 txbf_tx;

		txbf_tx = wlc_txbf_get_txbf_tx(wlc->txbf);

		switch (txbf_tx) {
		case TXBF_OFF:
			rate_info_block->RateCtl &= ~htol16(D11AX_RTCTL_BFM);
			break;
		case TXBF_ON:
			if (!RSPEC_ISCCK(rspec) && !RSPEC_ISOFDM(rspec)) {
				if (scb == WLC_RLM_SPECIAL_RATE_SCB(wlc)) {
					if (k == WLC_RLM_SPECIAL_RATE_RSPEC) {
						rate_info_block->RateCtl |=
								htol16(D11AX_RTCTL_BFM);
					}
				} else if (bf_shm_index != BF_SHM_IDX_INV) {
					rate_info_block->RateCtl |= htol16(D11AX_RTCTL_BFM);
				}
			}
			/* fall through */
		case TXBF_AUTO:
			if (bfen) {
				rate_info_block->RateCtl |= htol16(D11AX_RTCTL_BFM);
				if (bf_shm_index == BF_SHM_IDX_INV &&
					(!RSPEC_ACTIVE(wlc->band->rspec_override))) {
					rate_info_block->RateCtl |= htol16(D11AC_RTSCTS_IMBF);
				}
				/* Move wlc_txbf_fix_rspec_plcp() here to remove WL_RSPEC_STBC
				 * from rspec before wlc_d11_rev128_phy_txctl_calc().
				 * rate_info_block->PhyTxControlWord_0 will not set
				 * D11AC2_PHY_TXC_STBC if this rate enable txbf.
				 */
				wlc_txbf_fix_rspec_plcp(wlc->txbf, &rspec, plcp, txbf_chains);
			}
		default:
			break;
		}
	}
#endif /* WL_BEAMFORMING */

	curpwr = txpwrs.pbw[curbw][bfen];

	/* (6) Compute and fill PHY TX CTL */
	if ((scb == WLC_RLM_SPECIAL_RATE_SCB(wlc)) && (k == WLC_RLM_SPECIAL_RATE_BASIC))
		curpwr = wlc_get_bcnprs_txpwr_offset(wlc, curpwr);
	wlc_tx_d11_rev128_phy_txctl_calc(wlc, scb, bsscfg, rate_info_block, rspec,
		preamble_type, curpwr, txpwrs);
	wlc_txs_dyntxc_update(wlc, scb, rspec, rate_info_block, txpwrs_sdm);
#ifdef WLC_DTPC
	if (DTPC_ENAB(wlc->pub) && DTPC_SUPPORTED(wlc->dtpc) &&
		(scb != WLC_RLM_SPECIAL_RATE_SCB(wlc)) && !SCB_MU(scb)) {
		wlc_dtpc_upd_pwroffset(wlc->dtpc, scb, rspec, rate_info_block);
	}
#endif /* WLC_DTPC */

	/* for fallback bw */
	if (fbw != FBW_BW_INVALID) {
		uint8 txpwr = txpwrs.pbw[fbw - FBW_BW_20MHZ][TXBF_OFF_IDX];
		rate_info_block->FbwCtl |= ((uint16)txpwr << FBW_BFM0_TXPWR_SHIFT);
		if (fbw_bfen) {
			txpwr = txpwrs.pbw[fbw - FBW_BW_20MHZ][TXBF_ON_IDX];
			rate_info_block->FbwCtl |= ((uint16)txpwr << FBW_BFM_TXPWR_SHIFT);
		}
	}
	rate_info_block->FbwCtl = htol16(rate_info_block->FbwCtl);

	return rspec;
} /* wlc_tx_fill_rate_info_block_internal */

/**
 * Internal function to compute and fill the rate entry (rev128) for per-SCB rates
 */
static INLINE void
wlc_tx_fill_rate_entry_internal(wlc_info_t *wlc, scb_t *scb,
	d11ratemem_rev128_entry_t *rate_entry, ratesel_txparams_t *ratesel_rates,
	wlc_rlm_rate_store_t *rstore)
{
	wlc_key_info_t key_info;
	int k;
	bool use_rts = FALSE;
	bool use_cts = FALSE;

	ASSERT(ratesel_rates);
	ASSERT(SCB_BSSCFG(scb));

	(void) wlc_keymgmt_get_tx_key(wlc->keymgmt, scb, SCB_BSSCFG(scb), &key_info);

	for (k = 0; k < ratesel_rates->num; k++) {
		d11ratemem_rev128_rate_t *rate_info_block = &rate_entry->rate_info_block[k];
		ratespec_t rspec = ratesel_rates->rspec[k];
		if (!wlc_valid_rate(wlc, rspec, wlc->band->bandtype, FALSE)) {
			WL_ERROR(("Invalid rate selected: %s, %x (index %d)\n",
				__FUNCTION__, rspec, k));
			rspec = OFDM_RSPEC(WLC_RATE_6M); /* fallback to something 'safe' */
		}
		/* Store the final rspec back into the rstore */
		rstore->rspec[k] = wlc_tx_fill_rate_info_block_internal(wlc, scb,
			rate_info_block, rspec, FC_TYPE_DATA, &key_info, &use_rts, &use_cts, k);
		rstore->use_rts |= (use_rts ? 1 << k : 0);
	} /* rate loop ends */

	/* Mark last rate */
	rate_entry->rate_info_block[ratesel_rates->num - 1].RateCtl |=
		htol16(D11AC_RTSCTS_LAST_RATE);
	WLCNTSCBSET(scb->scb_stats.tx_rate, rstore->rspec[0]);
	WLCNTSCBSET(scb->scb_stats.tx_rate_fallback, rstore->rspec[ratesel_rates->num - 1]);
#ifdef WLC_MEDIAN_RATE_REPORT
	WLCNTSCBSET(scb->scb_rate_stats.tx_rate_log[WLCNT_SCB_TX_RATE_INDEX(scb)],
		rstore->rspec[0]);
	WLCNTSCB_INCR_LIMIT(scb->scb_rate_stats.tx_rate_index, (NRATELOG-1));
#endif /* WLC_MEDIAN_RATE_REPORT */
	wlc_txs_dyntxc_cnt_update(wlc, scb, rstore->rspec[0]);
}

/**
 * Public function to compute and fill the rate mem entry (Rev 128) for an SCB, to be called
 * by RateLinkMem module.
 */
void
wlc_tx_fill_rate_entry(wlc_info_t *wlc, scb_t *scb, d11ratemem_rev128_entry_t *rate_entry,
	wlc_rlm_rate_store_t *rate_store, ratesel_txparams_t *ratesel)
{
	wlc_bsscfg_t *bsscfg;
	wlc_key_info_t key_info;

	ASSERT(scb != NULL);
	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);
	ASSERT(rate_entry != NULL);
	ASSERT(rate_store != NULL);

	memset(rate_entry, 0, sizeof(*rate_entry));
	memset(rate_store, 0, sizeof(*rate_store));

	if (scb == WLC_RLM_SPECIAL_RATE_SCB(wlc)) {
		/* this is the 'global' SCB, special handling */
		d11ratemem_rev128_rate_t *rate_info_block;
		ratespec_t rspec;
		int k;
		bool use_rts;
		bool use_cts;

		(void) wlc_keymgmt_get_tx_key(wlc->keymgmt, NULL /* scb */, bsscfg, &key_info);

		for (k = 0; k < D11_REV128_RATEENTRY_NUM_RATES; k++) {
			bool fixed_rate = FALSE;

			use_rts = FALSE;
			use_cts = FALSE;
			rate_info_block = &rate_entry->rate_info_block[k];

			switch (k) {
				case WLC_RLM_SPECIAL_RATE_RSPEC : {
					if (RSPEC_ACTIVE(wlc->band->rspec_override)) {
						rspec = wlc->band->rspec_override;
						fixed_rate = TRUE;
					} else if (HE_ENAB_BAND(wlc->pub, wlc->band->bandtype) &&
						BSSCFG_STA(bsscfg)) {
						/* only allow STAs to use LOWEST_RATE_HE_RSPEC */
						rspec = LOWEST_RATE_HE_RSPEC;
					} else {
						rspec = scb->rateset.rates[0] & RATE_MASK;
					}
					break;
				}
				case WLC_RLM_SPECIAL_RATE_MRSPEC : {
					if (RSPEC_ACTIVE(wlc->band->mrspec_override)) {
						rspec = wlc->band->mrspec_override;
						fixed_rate = TRUE;
					} else {
						rspec = wlc_lowest_basic_rspec(wlc,
							&bsscfg->current_bss->rateset);
					}
					break;
				}
				case WLC_RLM_SPECIAL_RATE_BASIC : {
					rspec = wlc_lowest_basic_rspec(wlc,
							&bsscfg->current_bss->rateset);
					break;
				}
				case WLC_RLM_SPECIAL_RATE_OFDM : {
					rspec = OFDM_RSPEC(WLC_RATE_6M);
					break;
				}
			}
			if (!wlc_valid_rate(wlc, rspec, wlc->band->bandtype, FALSE)) {
				WL_INFORM(("Invalid rate selected: %s, %x (index %d)\n",
					__FUNCTION__, rspec, k));
				rspec = OFDM_RSPEC(WLC_RATE_6M); /* fallback to something 'safe' */
			}

			if (CHSPEC_IS6G(wlc->chanspec) &&
				(((wlc->lpi_mode == AUTO && wlc->stf->psd_limit_indicator) ||
				(wlc->lpi_mode == ON)) && (!fixed_rate))) {
				rspec &= ~WL_RSPEC_BW_MASK;
				rspec |= CHSPECBW_TO_RSPECBW(CHSPEC_BW(wlc->chanspec));
			}

			rate_store->rspec[k] = wlc_tx_fill_rate_info_block_internal(wlc, scb,
				rate_info_block, rspec, FC_TYPE_MNG, &key_info,
				&use_rts, &use_cts, k);
			rate_store->use_rts |= (use_rts ? 1 << k : 0);
			/* Mark last rate for each rate */
			rate_info_block->RateCtl |= htol16(D11AC_RTSCTS_LAST_RATE);
		}
	} else {
		wlc_tx_fill_rate_entry_internal(wlc, scb, rate_entry, ratesel, rate_store);
	}
} /* wlc_tx_fill_rate_entry */

/**
 * Fill d11txh_rev128_t version of TX desc.
 */
static INLINE void
wlc_tx_fill_txd_rev128(wlc_info_t *wlc, struct scb *scb, d11txh_rev128_t* txh,
	uint16 mcl, uint16 mch, wlc_d11mac_parms_t *parms, uint16 frameid,
	uint16 link_idx, uint8 tid, uint16 rmem_idx, int rate_block, uint16 mch2)
{
	const d11linkmem_entry_t * lmem;
	wlc_bsscfg_t *bsscfg = SCB_BSSCFG(scb);

	/* Chanspec - channel info for packet suppression */
	txh->Chanspec = htol16(wlc->chanspec);

	/* IV Offset, i.e. start of the 802.11 header */
	txh->IVOffset_Lifetime_lo = parms->IV_offset;
	ASSERT(txh->IVOffset_Lifetime_lo < (1 << D11_REV128_LIFETIME_LO_SHIFT));

	/* FrameLen */
	txh->FrmLen = htol16((uint16)parms->phylen);

	/* Sequence number final write */
	if (parms->type != FC_TYPE_CTL) {
		txh->SeqCtl = parms->h->seq;
	} else {
		txh->SeqCtl = 0;
	}

	/* LinkMemIdx */
	txh->LinkMemIdxTID = htol16(link_idx & D11_REV128_LINKIDX_MASK);

	/* Add TID */
	txh->LinkMemIdxTID |= htol16(tid << D11_REV128_LINKTID_SHIFT);

	/* RateMemIdx */
	txh->RateMemIdxRateIdx = htol16(rmem_idx & D11_REV128_RATEIDX_MASK);

	/* select specific rate if requested */
	if (rate_block >= 0) {
		uint16 rate_idx = rate_block & 0xf;
		uint16 max_rateidx = D11_REV128_RATEENTRY_NUM_RATES;
		BCM_REFERENCE(max_rateidx);
		ASSERT(rate_idx < max_rateidx);
		txh->RateMemIdxRateIdx |= htol16(rate_idx << D11_REV128_RATE_SPECRATEIDX_SHIFT);

		mch |= D11AC_TXC_FIX_RATE;
	} else {
		mch &= ~D11AC_TXC_FIX_RATE;
	}

#if defined(WLTEST) || defined(WLPKTENG)
	if (wlc_test_pkteng_run(wlc->testi)) {
		mcl |= D11AC_TXC_AMPDU;
		mch &= ~D11AC_TXC_SVHT;
		if (wlc_test_pkteng_get_mode(wlc->testi) == WL_PKTENG_CMD_STRT_DL_CMD) {
			mch |= D11REV128_TXC_HEOFDMA;
			mch |= D11AC_TXC_MU;
		}
		// assign fifo number as the OrderSeq by default for pkteng.
		// pkteng mode has pkts sit in fifos and recycle them to tx many ppdus
		// so ordering fifos isn't important anyway.
		// for non-pkteng mode, this field will be written by txq_hw_fill().
		txh->OrderSeq = htol16(D11_TXFID_GET_FIFO(wlc, frameid));
	}
#endif

	/* MacControl words */
	txh->MacControl_0 = htol16(mcl);
	txh->MacControl_1 = htol16(mch);
	txh->MacControl_2 = htol16(mch2);

	/* TxFrameID (gettxrate may have updated it) */
	txh->FrameID = htol16(frameid);

	/* sanity check lmem validity, sw copy */
	if (link_idx != WLC_RLM_SPECIAL_LINK_IDX &&
		link_idx != WLC_RLM_BSSCFG_LINK_IDX(wlc, bsscfg)) {

		lmem = wlc_ratelinkmem_retrieve_cur_link_entry(wlc, link_idx);
		if ((!lmem) || (lmem &&
			!(lmem->BssColor_valid & (1 << D11_REV128_LMEMVLD_NBIT)))) {
			WL_ERROR(("wl%d uninitialized linkmem for STA "MACF" lmidx:%04x"
				" rmidx:%04x ridx:%04x staid:%04x \n", wlc->pub->unit,
				ETHER_TO_MACF(scb->ea), link_idx, rmem_idx,
				rate_block, lmem ? lmem->BssColor_valid : 0));
			ASSERT(0);
		}
	}
} /* wlc_tx_fill_txd_rev128 */

/**
 * Add d11txh_rev128_t for rev128 mac & phy
 *
 * 'p' data must start with 802.11 MAC header
 * 'p' must allow enough bytes of local headers to be "pushed" onto the packet
 *
 * headroom == D11_REV128_TXH_LEN
 *
 */
uint16
wlc_d11hdrs_rev128(wlc_info_t *wlc, void *p, struct scb *scb, uint txparams_flags, uint frag,
	uint nfrags, uint fifo, uint next_frag_len, wlc_key_t *key, const wlc_key_info_t *key_info,
	ratespec_t rspec_override)
{
	d11txh_rev128_t *txh;
	osl_t *osh;
	wlc_d11mac_parms_t parms;
	uint16 frameid, mch;
	uint16 mcl = 0, mch2 = 0;
	uint16 rate_idx, link_idx;
	enum wme_ac ac;
	ratespec_t rspec = 0;
	wlc_bsscfg_t *bsscfg;
	bool mutx_pkteng_on = FALSE, bsrt = FALSE;
	wlc_wme_t *wme;
	uint8 preamble_type = WLC_LONG_PREAMBLE;
	uint8 tid = 0;
	int special_rate_block = -1;
	wlc_ht_info_t *hti = wlc->hti;
	bool use_mimops_rts = FALSE;
	const wlc_rlm_rate_store_t *rstore = NULL;
	uint16 txh_off = 0;
	d11txhdr_t *hdrptr;
	uint8 mucidx;
#ifdef TESTBED_AP_11AX
	uint32 nav;
#endif /* TESTBED_AP_11AX */

	BCM_REFERENCE(bsrt);
	BCM_REFERENCE(mucidx);

	ASSERT(scb != NULL);
	ASSERT(fifo < WLC_HW_NFIFO_INUSE(wlc));
	ASSERT(!SCB_DEL_IN_PROGRESS(scb));

	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);

#ifdef WL_MUPKTENG
	mutx_pkteng_on = wlc_mutx_pkteng_on(wlc->mutx);
#endif

#if defined(WLTEST) || defined(WLPKTENG)
	//TODO: Merge with the original mutx_pkteng_on later
	mutx_pkteng_on |= wlc_test_pkteng_run(wlc->testi);
#endif

	osh = wlc->osh;
	wme = bsscfg->wme;
	ac = WME_PRIO2AC(PKTPRIO(p));

	/* Extract 802.11 mac hdr and compute frame length in bytes (for PLCP) */
	wlc_extract_80211mac_hdr(wlc, p, &parms, key_info, frag, nfrags);

	ASSERT(RATELINKMEM_ENAB(wlc->pub));

	/* set default rate_idx and retrieve link index */
	rate_idx = D11_RATE_LINK_MEM_IDX_INVALID;
	link_idx = wlc_ratelinkmem_get_scb_link_index(wlc, scb);

	if (SCB_PROXD(scb)) {
		rate_idx = WLC_RLM_FTM_RATE_IDX;
		special_rate_block = WLC_RLM_SPECIAL_RATE_RSPEC;
		bsrt = FALSE;
	} else if (RSPEC_ACTIVE(rspec_override)) {
		/* Check for possible overrides */
#if defined(WLTEST) || defined(WLPKTENG)
		if (wlc_test_pkteng_run_dlofdma(wlc->testi))
			rate_idx = wlc_ratelinkmem_get_scb_rate_index(wlc, scb);
		else
#endif
		{
			rate_idx = WLC_RLM_SPECIAL_RATE_IDX; /* use 'global' scb */
		}
		if (rspec_override == wlc->band->rspec_override) {
			special_rate_block = WLC_RLM_SPECIAL_RATE_RSPEC;
		} else if (rspec_override == wlc->band->mrspec_override) {
			special_rate_block = WLC_RLM_SPECIAL_RATE_MRSPEC;
		} else if (rspec_override == OFDM_RSPEC(WLC_RATE_6M)) {
			special_rate_block = WLC_RLM_SPECIAL_RATE_OFDM;
		} else if (rspec_override == wlc_lowest_basic_rspec(wlc,
			&bsscfg->current_bss->rateset)) {
			special_rate_block = WLC_RLM_SPECIAL_RATE_BASIC;
		} else if (SCB_INTERNAL(scb) || ETHER_ISMULTI(&scb->ea) ||
			ETHER_ISNULLADDR(&scb->ea)) {
			WL_TRACE(("wl%d: %s, override: %x for scb: "MACF", using mrspec\n",
				wlc->pub->unit, __FUNCTION__,
				rspec_override, ETHER_TO_MACF(scb->ea)));
			special_rate_block = WLC_RLM_SPECIAL_RATE_MRSPEC;
		} else if (RSPEC_ACTIVE(wlc->band->rspec_override)) {
			special_rate_block = WLC_RLM_SPECIAL_RATE_RSPEC;
		} else if ((rspec_override == LOWEST_RATE_HE_RSPEC) && SCB_HE_CAP(scb)) {
			special_rate_block = WLC_RLM_SPECIAL_RATE_RSPEC;
		} else {
			WL_INFORM(("wl%d: %s, override: %x for scb: "MACF", using basic spec\n",
				wlc->pub->unit, __FUNCTION__,
				rspec_override, ETHER_TO_MACF(scb->ea)));
			special_rate_block = WLC_RLM_SPECIAL_RATE_BASIC;
		}
	} else if ((parms.type == FC_TYPE_MNG) ||
		(parms.type == FC_TYPE_CTL) ||
		((parms.pkttag)->flags & WLF_8021X) ||
#ifdef BCMWAPI_WAI
		((parms.pkttag)->flags & WLF_WAI) ||
#endif
		FALSE) {
		rate_idx = WLC_RLM_SPECIAL_RATE_IDX; /* use 'global' scb */
		special_rate_block = WLC_RLM_SPECIAL_RATE_BASIC;
		bsrt = TRUE;
	} else if (ETHER_ISMULTI(&parms.h->a1) || SCB_INTERNAL(scb)) {
		rate_idx = WLC_RLM_SPECIAL_RATE_IDX; /* use 'global' scb */
		link_idx = WLC_RLM_BSSCFG_LINK_IDX(wlc, bsscfg);
		special_rate_block = WLC_RLM_SPECIAL_RATE_MRSPEC;
		bsrt = TRUE;
	} else if (RSPEC_ACTIVE(wlc->band->rspec_override) &&
		!ETHER_ISMULTI(&parms.h->a1)) {
		rate_idx = WLC_RLM_SPECIAL_RATE_IDX; /* use 'global' scb */
		special_rate_block = WLC_RLM_SPECIAL_RATE_RSPEC;
	} else if (SCB_INTERNAL(scb) || ETHER_ISMULTI(&scb->ea) ||
		ETHER_ISNULLADDR(&scb->ea)) {
		ASSERT(rate_idx == WLC_RLM_SPECIAL_RATE_IDX);
		special_rate_block = WLC_RLM_SPECIAL_RATE_MRSPEC;
		bsrt = TRUE;
	} else {
		/* auto rate */
		ASSERT(rate_idx == D11_RATE_LINK_MEM_IDX_INVALID);
		(parms.pkttag)->flags |= WLF_RATE_AUTO;
	}

	/* if not fixed rate, try to find matching SCB rate index */
	if (rate_idx == D11_RATE_LINK_MEM_IDX_INVALID) {
		rate_idx = wlc_ratelinkmem_get_scb_rate_index(wlc, scb);
		ASSERT(rate_idx == link_idx);
		if (rate_idx == D11_RATE_LINK_MEM_IDX_INVALID) {
			/* no valid index, default to basic rate */
			rate_idx = WLC_RLM_SPECIAL_RATE_IDX; /* use 'global' scb */
			special_rate_block = WLC_RLM_SPECIAL_RATE_BASIC;
			bsrt = TRUE;
		}
	}

	ASSERT(rate_idx != D11_RATE_LINK_MEM_IDX_INVALID);
	if (rate_idx != WLC_RLM_SPECIAL_RATE_IDX && rate_idx != WLC_RLM_FTM_RATE_IDX) {
#if defined(WLTEST) || defined(WLPKTENG)
		if (!wlc_test_pkteng_run_dlofdma(wlc->testi)) {
#endif
			/* auto rate */
			ratesel_txparams_t ratesel_rates;
			uint16 flag0 = 0, flag1; /* as passed in para but not use it at caller */
			bool updated;

			ratesel_rates.num = 4; /* enable multi fallback rate */
			ratesel_rates.ac = ac;
			updated = wlc_scb_ratesel_gettxrate(wlc->wrsi, scb, &flag0,
				&ratesel_rates, &flag1);
			if (updated) {
				/* if rate is newly computed,
				 * get rspec from updated ratesel_rates
				 */
				rspec = ratesel_rates.rspec[0];
			} else {
				/* else get from stored rate_entry[0] */
				rstore = wlc_ratelinkmem_retrieve_cur_rate_store(wlc,
					rate_idx, TRUE);
				if (rstore == NULL) {
					WL_ERROR(("wl%d: %s, no rstore for scb: %p ("MACF
						", idx: %d)\n",
						wlc->pub->unit, __FUNCTION__,
						scb, ETHER_TO_MACF(scb->ea), rate_idx));
					wlc_scb_ratesel_init(wlc, scb);
					updated = wlc_scb_ratesel_gettxrate(wlc->wrsi, scb, &flag0,
						&ratesel_rates, &flag1);
					ASSERT(updated);
					rspec = ratesel_rates.rspec[0];
				} else {
					rspec = WLC_RATELINKMEM_GET_RSPEC(rstore, 0);
				}
			}
#if defined(WLTEST) || defined(WLPKTENG)
		} else {
			rstore = wlc_ratelinkmem_retrieve_cur_rate_store(wlc, rate_idx, TRUE);
			ASSERT(rstore != NULL);
			rspec = WLC_RATELINKMEM_GET_RSPEC(rstore, 0);
		}
#endif

	} else if (SCB_PROXD(scb)) {
		/* compute rspec from static rate_entry/block */
		rstore = wlc_ratelinkmem_retrieve_cur_rate_store(wlc, rate_idx, TRUE);
		if (rstore == NULL) {
			/* apparently first time use, so force update */
			wlc_ratelinkmem_update_rate_entry(wlc, scb,
				NULL, 0);
			rstore = wlc_ratelinkmem_retrieve_cur_rate_store(wlc, rate_idx, TRUE);
			ASSERT(rstore != NULL);
		}
		rspec = WLC_RATELINKMEM_GET_RSPEC(rstore,
			(special_rate_block & (D11_REV128_RATE_SPECRATEIDX_MASK >>
			D11_REV128_RATE_SPECRATEIDX_SHIFT)));
	} else {
		/* compute rspec from static rate_entry/block */
		ASSERT(special_rate_block != -1); /* should be set up in non-auto case */
		rstore = wlc_ratelinkmem_retrieve_cur_rate_store(wlc, rate_idx, TRUE);
		if (rstore == NULL) {
			/* apparently first time use, so force update */
			wlc_ratelinkmem_update_rate_entry(wlc, WLC_RLM_SPECIAL_RATE_SCB(wlc),
				NULL, 0);
			rstore = wlc_ratelinkmem_retrieve_cur_rate_store(wlc, rate_idx, TRUE);
			ASSERT(rstore != NULL);
		}
		rspec = WLC_RATELINKMEM_GET_RSPEC(rstore,
				(special_rate_block & (D11_REV128_RATE_SPECRATEIDX_MASK >>
				D11_REV128_RATE_SPECRATEIDX_SHIFT)));
	}
	WL_TRACE(("rspec: 0x%x (rspec override: 0x%x) special_rate_block=0x%x\n", rspec,
		rspec_override, special_rate_block));

	WL_TRACE(("wl%d: %s, rateidx: %d (%d), original linkidx: %d for scb: "MACF"\n",
		wlc->pub->unit, __FUNCTION__, rate_idx, 0, link_idx, ETHER_TO_MACF(scb->ea)));
	if ((link_idx == D11_RATE_LINK_MEM_IDX_INVALID) ||
		(wlc_ratelinkmem_retrieve_cur_link_entry(wlc, link_idx) == NULL)) {

		scb_t *link_scb = (link_idx == WLC_RLM_BSSCFG_LINK_IDX(wlc, bsscfg)) ?
			WLC_RLM_BSSCFG_LINK_SCB(wlc, bsscfg) : scb;
		/* no entry yet, need to trigger update */
		wlc_ratelinkmem_update_link_entry(wlc, link_scb);
		link_idx = wlc_ratelinkmem_get_scb_link_index(wlc, link_scb);
		if (link_idx == D11_RATE_LINK_MEM_IDX_INVALID) {
			if (!SCB_MARKED_DEL(scb)) {
				/* if the scb has been marked for deletion, only a deauth frame
				 * should be sent and it is intended to make use of the fall back.
				 */
				WL_ERROR(("wl%d: %s, Unable to create Link entry %d for scb "
					""MACF"\n",
					wlc->pub->unit, __FUNCTION__, link_idx,
					ETHER_TO_MACF(scb->ea)));

			} else
			{
				WL_INFORM(("wl%d: %s, Unable to create Link entry %d for scb "
					""MACF"\n",
					wlc->pub->unit, __FUNCTION__, link_idx,
					ETHER_TO_MACF(scb->ea)));

			}

			/* fall back to 'global' scb */
			link_idx = WLC_RLM_SPECIAL_LINK_IDX;
			rate_idx = WLC_RLM_SPECIAL_RATE_IDX;
			special_rate_block = WLC_RLM_SPECIAL_RATE_BASIC;
		}
	}
	WL_TRACE(("wl%d: %s, using rateidx %d block %d, linkidx: %d for scb:"MACF"; ",
		wlc->pub->unit, __FUNCTION__, rate_idx, special_rate_block,
		link_idx, ETHER_TO_MACF(scb->ea)));
	ASSERT(link_idx != D11_RATE_LINK_MEM_IDX_INVALID);

	txh = (d11txh_rev128_t*)PKTPUSH(osh, p, D11_REV128_TXH_LEN);
	bzero((char*)txh, D11_REV128_TXH_LEN);

	/*
	 * Some fields of the MacTxCtrl are tightly coupled,
	 * and dependant on various other params, which are
	 * yet to be computed. Due to complex dependancies
	 * with a number of params, filling of MacTxCtrl cannot
	 * be completely decoupled and made independant.
	 *
	 * Compute and fill independant fields of the the MacTxCtrl here.
	 */
	wlc_compute_initial_MacTxCtrl(wlc, bsscfg, scb, &mcl, &mch,
		&parms, mutx_pkteng_on);

	/* Compute and assign sequence number to MAC HDR */
	wlc_fill_80211mac_hdr_seqnum(wlc, p, scb, &mcl, &parms);

	/* determine TID */
	if (parms.qos) {
		uint16 qc;
		qc = (*(uint16 *)((uchar *)parms.h + (parms.a4 ?
			DOT11_A4_HDR_LEN : DOT11_A3_HDR_LEN)));
		tid = (QOS_TID(qc) & 0x0f);
	}

	/* TDLS U-APSD buffer STA: if peer is in PS, save the last MPDU's seq and tid for PTI */
#ifdef WLTDLS
	if (BSS_TDLS_ENAB(wlc, bsscfg) && SCB_PS(scb) &&
		wlc_tdls_buffer_sta_enable(wlc->tdls)) {
		wlc_tdls_update_tid_seq(wlc->tdls, scb, tid, ltoh16(parms.h->seq));
	}
#endif

	/* Compute frameid, also possibly change seq */
	frameid = wlc_compute_frameid(wlc, txh->FrameID, fifo);

	/* Rev 128 has no TxStatus field, so skip txh->TxStatus = 0; */

#ifdef WL_MU_TX
	if (TXBF_ENAB(wlc->pub) && SCB_MU(scb) &&
		((mucidx = wlc_mutx_sta_mucidx_get(wlc->mutx, scb)) != BF_SHM_IDX_INV)) {
		/* link is capable of mu sounding enabled in shmx bfi interface */
		if (WLPKTFLAG_AMPDU(parms.pkttag) &&
			/* Check if client has MU FIFO. If yes set MU bits */
			wlc_fifo_isMU(wlc->fifo, scb, ac)) {
#if defined(BCMDBG) || defined(WLTEST)
			mu_type_t mu = WL_MU_DLMMU;
			if (wlc_twt_scb_active(wlc->twti, scb)) {
				mu = WL_MU_TWT;
			}
			wlc_check_fifo_type(wlc->fifo, scb, ac, mu);
#endif

			ASSERT(nfrags == 1); /* should not frag AMPDUs, can't MU frags */

			/* link is capable of mutx and has been enabled to do mutx */
			mch |= D11AC_TXC_MU;
			mch2 = mucidx << MCTL2_MCIDX_SHIFT;
#ifdef WL_MUPKTENG
			if (mutx_pkteng_on) {
				mcl |=  D11AC_TXC_AMPDU;
				mch &= ~D11AC_TXC_SVHT;
			}
#endif /* WL_MUPKTENG */
		}
	}
#endif /* WL_MU_TX */

	/* populate LFRM bit if using RTS XXX: Need to find a way to streamline the settings
	 * in wlc_d11_hdrs_rts_cts() as in revid<128
	 */
	if (rstore && rstore->use_rts) {
		mcl |= D11AC_TXC_LFRM;
	}

	/* TXH fixups */
	/* mark pkt as aggregable if it is */
	if (WLPKTFLAG_AMPDU(parms.pkttag) && !RSPEC_ISLEGACY(rspec) &&
		WLC_KEY_ALLOWS_AMPDU(key_info)) {
		WLPKTTAG(p)->flags |= WLF_MIMO;
	}
	if (N_ENAB(wlc->pub) && (RSPEC_ISHT(rspec) || RSPEC_ISVHT(rspec) || RSPEC_ISHE(rspec)) &&
		(wlc_ratespec_nsts(rspec) > 1) &&
		WLC_STF_GET_SCB_MIMOPS_ENAB(wlc->stf, scb) &&
		WLC_STF_GET_SCB_MIMOPS_RTS_ENAB(wlc->stf, scb))
		use_mimops_rts = TRUE;
	/* dont allow bursting if rts is required around each mimo frame */
	if ((parms.type == FC_TYPE_DATA) && (fifo != TX_BCMC_FIFO) && (fifo != TX_ATIM_FIFO) &&
		(WLC_HT_GET_FRAMEBURST(hti) &&
		WLC_HT_GET_FRAMEBURST_PER_AC(hti, ac) &&
		(wf_rspec_to_rate(rspec) > WLC_FRAMEBURST_MIN_RATE) &&
		(!wlc->active_bidir) &&
		(wlc_ampdu_frameburst_override(wlc->ampdu_tx, scb) == FALSE) &&
		(!wme->edcf_txop[ac] || WLPKTFLAG_AMPDU(parms.pkttag))) &&
		(use_mimops_rts == FALSE)) {

		mcl |= D11AC_TXC_MBURST;
	}
	if (RSPEC_ISVHT(rspec) || RSPEC_ISHE(rspec)) {
		mch |= D11AC_TXC_SVHT;
	}

	/* Don't update rate history, do it upon TXS instead */
	WLPKTTAG(p)->rspec = rspec;

	if (SCB_WME(scb) && parms.qos) {
		/* first determine preamble_type for TXOP calculations */
		if (N_ENAB(wlc->pub)) {
			/* Determine the HT frame format, HT_MM or HT_GF */
			if (RSPEC_ISHT(rspec)) {
				preamble_type = wlc_prot_n_preamble(wlc, scb);
				if (RSPEC_ISSGI(rspec) && (wlc_ratespec_nsts(rspec) == 1)) {
					preamble_type = WLC_MM_PREAMBLE;
				}
			} else if (RSPEC_ISVHT(rspec) || RSPEC_ISHE(rspec)) {
				/* VHT & HE always uses MM preamble */
				preamble_type = WLC_MM_PREAMBLE;
			}
		}
		if (RSPEC_ISCCK(rspec)) {
			bool short_preamble = (txparams_flags & WLC_TX_PARAMS_SHORTPRE) != 0;
			if (short_preamble &&
				!((RSPEC2RATE(rspec) == WLC_RATE_1M) ||
				(bsscfg->PLCPHdr_override == WLC_PLCP_LONG))) {
				preamble_type = WLC_SHORT_PREAMBLE;
			} else {
				preamble_type = WLC_LONG_PREAMBLE;
			}
		}

		/* Avoid changing TXOP threshold based on multicast packets */
		if (wme->edcf_txop[ac] &&
			!ETHER_ISMULTI(&parms.h->a1) && !(parms.pkttag->flags & WLF_8021X)) {
			/* WME: Update TXOP threshold */
			if ((!WLPKTFLAG_AMPDU(parms.pkttag)) && (frag == 0)) {
				uint dur;
				int16 delta;
				uint queue_type = (fifo < TX_FIFO_HE_MU_START) ? fifo :
					((fifo - TX_FIFO_HE_MU_START) % AC_COUNT);
				BCM_REFERENCE(queue_type);

				/* frame + SIFS + ACK */
				dur = wlc_compute_frame_dur(wlc,
					CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
					rspec, preamble_type, 0);

				/* update txop byte threshold (txop minus intraframe overhead) */
				delta = (int16)(wme->edcf_txop[ac] - dur);
				if (delta >= 0) {
#ifdef WLAMSDU_TX
					if (!(WLPKTFLAG_AMSDU(parms.pkttag) &&
						(fifo == TX_AC_BE_FIFO)))
#endif /* WLAMSDU_TX */
					{
						uint newfragthresh = wlc_calc_frame_len(wlc, rspec,
							preamble_type, (uint16)delta);
						/* range bound the fragthreshold */
						newfragthresh = MAX(newfragthresh,
							DOT11_MIN_FRAG_LEN);
						newfragthresh = MIN(newfragthresh,
							wlc->usr_fragthresh);
						/* update the fragthr and do txc update */
						if (wlc->fragthresh[ac] != (uint16)newfragthresh) {
							wlc_fragthresh_set(wlc, ac,
								(uint16)newfragthresh);
						}
					}
				} else {
					WL_ERROR(("wl%d: %s txop invalid for rate %d\n",
						wlc->pub->unit, fifo_names[queue_type],
						RSPEC2KBPS(rspec)));
				}

				dur += wlc_calc_frame_time(wlc, rspec, preamble_type, parms.phylen);

				if (CAC_ENAB(wlc->pub) && (fifo <= TX_AC_VO_FIFO)) {
					/* update cac used time */
					if (wlc_cac_update_used_time(wlc->cac, ac, dur, scb))
						WL_ERROR(("wl%d: ac %d: txop exceeded allocated TS"
							"time\n", wlc->pub->unit, ac));
				}

				/*
				 * FIXME: The MPDUs of the next transmitted MSDU after
				 * rate drops or RTS/CTS kicks in may exceed
				 * TXOP. Without tearing apart the transmit
				 * path---either push rate and RTS/CTS decisions much
				 * earlier (hard), allocate fragments just in time
				 * (harder), or support late refragmentation (even
				 * harder)---it's too difficult to fix this now.
				 */
				if (dur > wme->edcf_txop[ac])
					WL_INFORM(("wl%d: %s txop exceeded phylen "
						"%d/%d dur %d/%d\n",
						wlc->pub->unit, fifo_names[queue_type],
						parms.phylen, wlc->fragthresh[ac], dur,
						wme->edcf_txop[ac]));
			}
		} else if (CAC_ENAB(wlc->pub) && (fifo <= TX_AC_VO_FIFO)) {
			uint dur;
			/* frame + SIFS + ACK */
			dur = wlc_calc_frame_time(wlc, rspec, preamble_type, parms.phylen);
			dur += wlc_compute_frame_dur(wlc,
				CHSPEC_BANDTYPE(bsscfg->current_bss->chanspec),
				rspec, preamble_type, 0);
			/* update cac used time */
			if (wlc_cac_update_used_time(wlc->cac, ac, dur, scb))
				WL_ERROR(("wl%d: ac %d: txop exceeded allocated TS time\n",
					wlc->pub->unit, ac));
		}
	}

#ifdef WL11AX
	/* Configure HEModeControl */
	if (SCB_DLOFDMA_ADM(scb) && WLPKTFLAG_AMPDU(parms.pkttag) &&
#if defined(WLTEST) || defined(WLPKTENG)
		(wlc_test_pkteng_run_dlofdma(wlc->testi) ||
#else
		(TRUE &&
#endif
		/* Check if MU FIFO */
		wlc_fifo_isMU(wlc->fifo, scb, ac)) &&
		!wlc_fifo_is_ulofdma(wlc->fifo, scb, ac)) {
		int8 dl_schpos, dl_schid;
#if defined(BCMDBG) || defined(WLTEST)
		mu_type_t mu = SCB_HEMMU(scb) ? WL_MU_DLMMU:WL_MU_DLOFDMA;
		if (wlc_twt_scb_active(wlc->twti, scb)) {
			mu = WL_MU_TWT;
		}
		wlc_check_fifo_type(wlc->fifo, scb, ac, mu);
#endif
		if (wlc_scbmusched_get_dlsch(wlc->musched, scb, &dl_schid, &dl_schpos) != BCME_OK) {
			WL_ERROR(("wl%d: %s: fail to get scheduler info\n",
				wlc->pub->unit, __FUNCTION__));
			ASSERT(0);
		}
		ASSERT(nfrags == 1); /* should not frag AMPDUs, can't MU frags */
		if (!RSPEC_ISHE(rspec)) {
			WL_ERROR(("%d: %s HEOMU "MACF" 0x%x rspec:0x%x\n", wlc->pub->unit,
				__FUNCTION__, ETHER_TO_MACF(scb->ea), scb->flags3, rspec));
		}
		mch2 = ((dl_schpos << MCTL2_SCHPOS_SHIFT) & MCTL2_SCHPOS_MASK) |
			((dl_schid << MCTL2_SCHIDX_SHIFT) & MCTL2_SCHIDX_MASK);
		mch |= D11AC_TXC_MU;
		mch |= D11REV128_TXC_HEOFDMA;
		if (wlc_txbf_tbcap_check(wlc->txbf, scb, DLOFDMA) != BCME_OK) {
			WL_ERROR(("wl%d: %s Invalid BFI config\n",
				wlc->pub->unit, __FUNCTION__));
			ASSERT(0);
		}
	}
#endif /* WL11AX */

#ifdef WL_MU_TX
	if (SCB_HEMMU(scb) &&
		((mucidx = wlc_mutx_sta_mucidx_get(wlc->mutx, scb)) != BF_SHM_IDX_INV) &&
		WLPKTFLAG_AMPDU(parms.pkttag) &&
		/* Check if client has MU FIFO. If yes set MU bits */
		wlc_fifo_isMU(wlc->fifo, scb, ac)) {
		ASSERT(nfrags == 1); /* should not frag AMPDUs, can't MU frags */
		if (!RSPEC_ISHE(rspec)) {
			WL_ERROR(("%d: %s HEMMU "MACF" 0x%x rspec:0x%x\n", wlc->pub->unit,
				__FUNCTION__, ETHER_TO_MACF(scb->ea), scb->flags3, rspec));
		}
		mch |= D11REV128_TXC_HEMUMIMO;
		mch &= ~D11REV128_TXC_HEOFDMA;
		mch2 = mucidx << MCTL2_MCIDX_SHIFT;
		if (wlc_txbf_tbcap_check(wlc->txbf, scb, HEMMU) != BCME_OK) {
			WL_ERROR(("wl%d: %s Invalid BFI config\n",
				wlc->pub->unit, __FUNCTION__));
			ASSERT(0);
		}
	}
#endif /* WL_MU_TX */

#ifdef WL_PROXDETECT
	if (PROXD_ENAB(wlc->pub) && wlc_proxd_frame(wlc, parms.pkttag)) {
		WL_INFORM(("wlc_d11hdrs_rev128: proxd enab, rspec 0x%x, bottom packetid 0x%08x\n",
			rspec,
			parms.pkttag->shared.packetid));
		/* signal ucode to enable timestamping for this frame */
		mch |= D11AC_TXC_TOF;
	}
#endif /* WL_PROXDETECT */

#ifdef BCM_CSIMON
	/* If CSI Monitor is enabled for this STA, set the CSI bit for this QoS Null
	 * frame header as an indication to the ucode
	 */
	if (WLPKTTAG(p)->flags & WLF_CSI_NULL_PKT) {
		mch |= D11REV128_TXC_CSI;
		WL_INFORM(("wl%d: CSIMON: set mchflag 0x%x\n", wlc->pub->unit,
		              D11REV128_TXC_CSI));
	}
#endif /* BCM_CSIMON */

#ifdef TESTBED_AP_11AX
	if (((ltoh16(parms.h->fc) & FC_KIND_MASK) == FC_QOS_NULL) &&
	    ((nav = update_pkt_eng_get_nav_override(wlc)) != 0)) {
		parms.h->durid = htol16(nav);
		mch |= D11AC_TXC_FIXDUR;
	}
#endif /* TESTBED_AP_11AX */

	/* Compute and fill TXH fields */
	wlc_tx_fill_txd_rev128(wlc, scb, txh, mcl, mch, &parms, frameid, link_idx, tid, rate_idx,
		special_rate_block, mch2);

	wlc_macdbg_dtrace_log_txd(wlc->macdbg, scb, &rspec, txh);

	hdrptr = (d11txhdr_t *)PKTDATA(wlc->osh, p);

#ifdef WLTOEHW
	/* add tso-oe header, if capable and toe module is not bypassed */
	if (wlc->toe_capable && !wlc->toe_bypass) {
		wlc_toe_add_hdr(wlc, p, scb, key_info, nfrags, &txh_off);
#ifdef WLCFP
		if (CFP_ENAB(wlc->pub) == TRUE) {
			/* Update TSO header before taking a snapshot of txheader in TXC */
			uint8* tsoHdrPtr;
			uint16 fc_type;

			tsoHdrPtr = (uint8*)(PKTDATA(wlc->osh, p));
			fc_type = FC_TYPE(parms.fc);

			wlc_txh_set_ft(wlc, tsoHdrPtr, fc_type);
		}
#endif /* WLCFP */
	}
#endif /* WLTOEHW */

	if ((key_info) && (key_info->algo != CRYPTO_ALGO_OFF)) {
		PKTPULL(wlc->osh, p, txh_off + D11_REV128_TXH_LEN);
		if (wlc_key_prep_tx_mpdu(key, p, hdrptr) != BCME_OK) {
			WL_WSEC(("wl%d: %s: wlc_key_prep_tx_mpdu error\n",
				wlc->pub->unit, __FUNCTION__));
		}
		PKTPUSH(wlc->osh, p, txh_off + D11_REV128_TXH_LEN);
	}

	/* With d11 hdrs on, mark the packet as TXHDR */
	WLPKTTAG(p)->flags |= WLF_TXHDR;

	return (frameid);
} /* wlc_d11hdrs_rev128 */

/**
 * Add d11 tx headers
 *
 * 'pkt' data must start with 802.11 MAC header
 * 'pkt' must allow enough bytes of headers (headroom) to be "pushed" onto the packet
 *
 * Each core revid may require different headroom. See each wlc_d11hdrs_xxx() for details.
 */
#ifdef WLC_D11HDRS_DBG
uint16
wlc_d11hdrs(wlc_info_t *wlc, void *pkt, struct scb *scb, uint txparams_flags,
	uint frag, uint nfrags, uint fifo, uint next_frag_len, wlc_key_t *key,
	const wlc_key_info_t *key_info, ratespec_t rspec_override)
{
	uint16 retval;
	uint8 *txh;
#if defined(BCMDBG) || defined(WLMSG_PRHDRS)
	uint8 *h = PKTDATA(wlc->osh, pkt);
#endif

	retval = WLC_D11HDRS_FN(wlc, pkt, scb, txparams_flags,
		frag, nfrags, fifo, next_frag_len, key, key_info,
		rspec_override); // calls e.g. wlc_d11hdrs_rev40()

	txh = PKTDATA(wlc->osh, pkt);

	WL_PRHDRS(wlc, "txpkt hdr (MPDU)", h, txh, NULL, PKTLEN(wlc->osh, pkt));
	/** what we're seeing here: d11actxh_t (124 bytes), plcp, d11 header, remainder of MPDU */
	WL_PRPKT("txpkt body (MPDU)", txh, PKTLEN(wlc->osh, pkt));

	return retval;

} /* wlc_d11hdrs */
#endif /* WLC_D11HDRS_DBG */

/** The opposite of wlc_calc_frame_time */
static uint
wlc_calc_frame_len(wlc_info_t *wlc, ratespec_t ratespec, uint8 preamble_type, uint dur)
{
	uint nsyms, mac_len, Ndps;
	uint rate;

	WL_TRACE(("wl%d: %s: rspec 0x%x, preamble_type %d, dur %d\n",
	          wlc->pub->unit, __FUNCTION__, ratespec, preamble_type, dur));

	if (RSPEC_ISVHT(ratespec) || RSPEC_ISHE(ratespec)) {
		mac_len = (dur * wf_rspec_to_rate(ratespec)) / 8000;
	} else if (RSPEC_ISHT(ratespec)) {
		mac_len = wlc_ht_calc_frame_len(wlc->hti, ratespec, preamble_type, dur);
	} else if (RSPEC_ISOFDM(ratespec)) {
		rate = RSPEC2RATE(ratespec);
		dur -= APHY_PREAMBLE_TIME;
		dur -= APHY_SIGNAL_TIME;
		/* Ndbps = Mbps * 4 = rate(500Kbps) * 2 */
		Ndps = rate*2;
		nsyms = dur / APHY_SYMBOL_TIME;
		mac_len = ((nsyms * Ndps) - (APHY_SERVICE_NBITS + APHY_TAIL_NBITS)) / 8;
	} else {
		ASSERT(RSPEC_ISCCK(ratespec));
		rate = RSPEC2RATE(ratespec);
		if (preamble_type & WLC_SHORT_PREAMBLE)
			dur -= BPHY_PLCP_SHORT_TIME;
		else
			dur -= BPHY_PLCP_TIME;
		mac_len = dur * rate;
		/* divide out factor of 2 in rate (1/2 mbps) */
		mac_len = mac_len / 8 / 2;
	}
	return mac_len;
}

/**
 * Fetch the TxD (DMA hardware hdr), vhtHdr (rev 40+ ucode hdr),
 * and 'd11_hdr' (802.11 frame header), from the given Tx prepared pkt.
 * This function only works for d11 core rev >= 40 since it is extracting
 * rev 40+ specific header information.
 *
 * A Tx Prepared packet is one that has been prepared for the hw fifo and starts with the
 * hw/ucode header.
 *
 * @param wlc           wlc_info_t pointer
 * @param p             pointer to pkt from wich to extract interesting header pointers
 * @param ptxd          on return will be set to a pointer to the TxD hardware header,
 *                      also know as @ref d11ac_tso_t
 * @param ptxh          on return will be set to a pointer to the ucode TxH header @ref d11actxh_t
 * @param pd11hdr       on return will be set to a pointer to the 802.11 header bytes,
 *                      starting with Frame Control
 */
void BCMFASTPATH
wlc_txprep_pkt_get_hdrs(wlc_info_t* wlc, void* p,
	uint8** ptxd, d11txhdr_t** ptxh, struct dot11_header** pd11_hdr)
{
	uint8* txd;
	d11txhdr_t* txh;
	uint txd_len;
	uint txh_len;
	uint16 mcl;

	/* this fn is only for new VHT (11ac) capable ucode headers */
	ASSERT(D11REV_GE(wlc->pub->corerev, 40));

	BCM_REFERENCE(mcl);

	txd = PKTDATA(wlc->osh, p);
	txd_len = (txd[0] & TOE_F0_HDRSIZ_NORMAL) ?
	        TSO_HEADER_LENGTH : TSO_HEADER_PASSTHROUGH_LENGTH;

	txh = (d11txhdr_t *)(txd + txd_len);
	mcl = *D11_TXH_GET_MACLOW_PTR(wlc, txh);
	if (mcl & htol16(D11AC_TXC_HDR_FMT_SHORT)) {
		txh_len = D11_TXH_SHORT_LEN(wlc);
	} else {
		txh_len = D11_TXH_LEN_EX(wlc);
	}

	*ptxd = txd;
	*ptxh = txh;
	*pd11_hdr = (struct dot11_header*)((uint8*)txh + txh_len);
} /* wlc_txprep_pkt_get_hdrs */

/** have to extract SGI/STBC differently pending on frame type being 11n or 11vht */
bool BCMFASTPATH
wlc_txh_get_isSGI(wlc_info_t* wlc, const wlc_txh_info_t* txh_info)
{
	uint16 frametype;

	ASSERT(txh_info);

	/* should not call this function for RATELINKMEM cases, use rspec instead */
	ASSERT(!RATELINKMEM_ENAB(wlc->pub));
	ASSERT(D11REV_LT(wlc->pub->corerev, 128));

	frametype = ltoh16(txh_info->PhyTxControlWord0) & D11_PHY_TXC_FT_MASK(wlc->pub->corerev);

	if (frametype == FT_HT)
		return (PLCP3_ISSGI(txh_info->plcpPtr[3]));
	else if (frametype == FT_VHT)
		return (VHT_PLCP3_ISSGI(txh_info->plcpPtr[3]));
	/* in FT_HE there are multiple GI configs, not sure what to return here, so use default */

	return FALSE;
}

bool BCMFASTPATH
wlc_txh_get_isSTBC(wlc_info_t* wlc, const wlc_txh_info_t* txh_info)
{
	uint8 frametype;

	ASSERT(txh_info);

	/* should not call this function for RATELINKMEM cases, use rspec instead */
	ASSERT(!RATELINKMEM_ENAB(wlc->pub));
	ASSERT(D11REV_LT(wlc->pub->corerev, 128));

	ASSERT(txh_info->plcpPtr);

	frametype = ltoh16(txh_info->PhyTxControlWord0) & D11_PHY_TXC_FT_MASK(wlc->pub->corerev);

	if (frametype == FT_HT)
		return (PLCP3_ISSTBC(txh_info->plcpPtr[3]));
	else if (frametype == FT_VHT)
		return ((txh_info->plcpPtr[0] & VHT_SIGA1_STBC) != 0);
	else if (frametype == FT_HE)
		return ((txh_info->PhyTxControlWord0 &
			htol16(D11_REV80_PHY_TXC_STBC << D11_REV80_PHY_TXC_MCS_NSS_SHIFT)) != 0);
	return FALSE;
}

chanspec_t
wlc_txh_get_chanspec(wlc_info_t* wlc, wlc_txh_info_t* tx_info)
{
	if (D11REV_GE(wlc->pub->corerev, 128)) {
		return (ltoh16(tx_info->hdrPtr->rev128.Chanspec));
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		return ltoh16(tx_info->hdrPtr->rev40.PktInfo.Chanspec);
	} else {
		return (ltoh16(tx_info->hdrPtr->pre40.XtraFrameTypes) >> XFTS_CHANNEL_SHIFT);
	}
}

/* Get pointer to FrameID field in TXH. Note: field itself is in LE order */
static uint16* BCMFASTPATH
wlc_get_txh_frameid_ptr(wlc_info_t *wlc, void *p)
{
	uint8 *pkt_data = NULL;
	uint tsoHdrSize = 0;

	ASSERT(p);
	pkt_data = PKTDATA(wlc->osh, p);
	ASSERT(pkt_data != NULL);

#ifdef WLTOEHW
	if (D11REV_GE(wlc->pub->corerev, 40)) {
		tsoHdrSize = (wlc->toe_bypass ? 0 : wlc_tso_hdr_length((d11ac_tso_t*)pkt_data));
	}
#endif /* WLTOEHW */

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		d11txh_rev128_t *heRev128Hdr = (d11txh_rev128_t *)(pkt_data + tsoHdrSize);

		ASSERT(PKTLEN(wlc->osh, p) >= D11_REV128_TXH_LEN + tsoHdrSize);
		return &heRev128Hdr->FrameID;
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		d11actxh_t *vhtHdr = (d11actxh_t*)(pkt_data + tsoHdrSize);
		d11actxh_pkt_t *ppkt_info = &(vhtHdr->PktInfo);

		ASSERT(PKTLEN(wlc->osh, p) >= D11AC_TXH_SHORT_LEN + tsoHdrSize);
		return &ppkt_info->TxFrameID;
	} else {
		d11txh_pre40_t *nonVHTHdr = (d11txh_pre40_t *)pkt_data;
		return &nonVHTHdr->TxFrameID;
	}
}

void BCMFASTPATH
wlc_set_txh_frameid(wlc_info_t *wlc, void *p, uint16 frameid)
{
	uint16* frameid_ptr = wlc_get_txh_frameid_ptr(wlc, p);
	*frameid_ptr = htol16(frameid);
}

uint16 BCMFASTPATH
wlc_get_txh_frameid(wlc_info_t *wlc, void *p)
{
	uint16* frame_id_ptr = wlc_get_txh_frameid_ptr(wlc, p);
	return ltoh16(*frame_id_ptr);
}

/* Get pointer to MacTxControlHigh field in TXH. Note: field itself is in LE order */
static uint16* BCMFASTPATH
wlc_get_txh_mactxcontrolhigh_ptr(wlc_info_t *wlc, void *p)
{
	uint8 *pkt_data = NULL;
	uint tsoHdrSize = 0;

	ASSERT(p);
	pkt_data = PKTDATA(wlc->osh, p);
	ASSERT(pkt_data != NULL);

#ifdef WLTOEHW
	if (D11REV_GE(wlc->pub->corerev, 40)) {
		tsoHdrSize = (wlc->toe_bypass ? 0 : wlc_tso_hdr_length((d11ac_tso_t*)pkt_data));
	}
#endif /* WLTOEHW */

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		d11txh_rev128_t *heRev128Hdr = (d11txh_rev128_t *)(pkt_data + tsoHdrSize);
		return &heRev128Hdr->MacControl_1;
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		d11actxh_t *vhtHdr = (d11actxh_t*)(pkt_data + tsoHdrSize);
		d11actxh_pkt_t *ppkt_info = &(vhtHdr->PktInfo);
		return &ppkt_info->MacTxControlHigh;
	} else {
		d11txh_pre40_t *nonVHTHdr = (d11txh_pre40_t *)pkt_data;
		return &nonVHTHdr->MacTxControlHigh;
	}
}

void BCMFASTPATH
wlc_set_txh_mactxcontrolhigh(wlc_info_t *wlc, void *p, uint16 mactxcontrolhigh)
{
	uint16* mactxcontrolhigh_ptr = wlc_get_txh_mactxcontrolhigh_ptr(wlc, p);
	*mactxcontrolhigh_ptr = htol16(mactxcontrolhigh);
}

uint16 BCMFASTPATH
wlc_get_txh_mactxcontrolhigh(wlc_info_t *wlc, void *p)
{
	uint16* mactxcontrolhigh_ptr = wlc_get_txh_mactxcontrolhigh_ptr(wlc, p);
	return ltoh16(*mactxcontrolhigh_ptr);
}

/* Get pointer to ABI_MimoAntSel field in TXH. Note: field itself is in LE order */
static uint16* BCMFASTPATH
wlc_get_txh_abi_mimoantsel_ptr(wlc_info_t *wlc, void *p)
{
	ASSERT(p);

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		return NULL;
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		return NULL;
	} else {
		d11txh_pre40_t *nonVHTHdr = (d11txh_pre40_t *)PKTDATA(wlc->osh, p);
		return &nonVHTHdr->ABI_MimoAntSel;
	}
}

void BCMFASTPATH
wlc_set_txh_abi_mimoantsel(wlc_info_t *wlc, void *p, uint16 abi_mimoantsel)
{
	uint16* abi_mimoantsel_ptr = wlc_get_txh_abi_mimoantsel_ptr(wlc, p);
	if (abi_mimoantsel_ptr) {
		*abi_mimoantsel_ptr = htol16(abi_mimoantsel);
	}
}

uint16 BCMFASTPATH
wlc_get_txh_abi_mimoantsel(wlc_info_t *wlc, void *p)
{
	uint16* abi_mimoantsel_ptr = wlc_get_txh_abi_mimoantsel_ptr(wlc, p);
	return abi_mimoantsel_ptr ? ltoh16(*abi_mimoantsel_ptr) : 0;
}

static uint16* BCMFASTPATH
wlc_get_txh_mactxcontrollow_ptr(wlc_info_t *wlc, void *p)
{
	uint8 *pkt_data = NULL;
	uint tsoHdrSize = 0;

	ASSERT(p);
	pkt_data = PKTDATA(wlc->osh, p);
	ASSERT(pkt_data != NULL);

#ifdef WLTOEHW
	if (D11REV_GE(wlc->pub->corerev, 40)) {
		tsoHdrSize = (wlc->toe_bypass ? 0 : wlc_tso_hdr_length((d11ac_tso_t*)pkt_data));
	}
#endif /* WLTOEHW */

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		d11txh_rev128_t *heRev128Hdr = (d11txh_rev128_t *)(pkt_data + tsoHdrSize);
		return &heRev128Hdr->MacControl_0;
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		d11actxh_t *vhtHdr = (d11actxh_t*)(pkt_data + tsoHdrSize);
		d11actxh_pkt_t *ppkt_info = &(vhtHdr->PktInfo);
		return &ppkt_info->MacTxControlLow;
	} else {
		d11txh_pre40_t *nonVHTHdr = (d11txh_pre40_t *)PKTDATA(wlc->osh, p);
		return &nonVHTHdr->MacTxControlLow;
	}
}

/**
 * parse info from a tx header and return in a common format
 * -- needed due to differences between ac, ax and pre-ac tx hdrs
 */
void BCMFASTPATH
wlc_get_txh_info(wlc_info_t* wlc, void* p, wlc_txh_info_t* tx_info)
{
	uint8 *pkt_data = NULL;
	if (p == NULL || tx_info == NULL ||
		((pkt_data = PKTDATA(wlc->osh, p)) == NULL)) {
		WL_ERROR(("%s: null P or tx_info or pktdata.\n", __FUNCTION__));
		ASSERT(p != NULL);
		ASSERT(tx_info != NULL);
		ASSERT(pkt_data != NULL);
		return;
	}

	/* cannot get TXH info if no TXH is present */
	ASSERT(WLPKTTAG(p)->flags & WLF_TXHDR);

	if (D11REV_LT(wlc->pub->corerev, 40)) {
		d11txh_pre40_t* nonVHTHdr = (d11txh_pre40_t *)pkt_data;

		tx_info->tsoHdrPtr = NULL;
		tx_info->tsoHdrSize = 0;

		tx_info->TxFrameID = nonVHTHdr->TxFrameID;
		tx_info->MacTxControlLow = nonVHTHdr->MacTxControlLow;
		tx_info->MacTxControlHigh = nonVHTHdr->MacTxControlHigh;
		tx_info->hdrSize = D11_TXH_LEN;
		tx_info->hdrPtr = (d11txhdr_t *)nonVHTHdr;
		tx_info->d11HdrPtr = (void*)((uint8*)nonVHTHdr + D11_TXH_LEN + D11_PHY_HDR_LEN);
		tx_info->TxFrameRA = nonVHTHdr->TxFrameRA;
		tx_info->plcpPtr = (uint8 *)(nonVHTHdr + 1);
		tx_info->PhyTxControlWord0 = nonVHTHdr->PhyTxControlWord;
		tx_info->PhyTxControlWord1 = nonVHTHdr->PhyTxControlWord_1;
		tx_info->seq = ltoh16(((struct dot11_header*)tx_info->d11HdrPtr)->seq) >>
			SEQNUM_SHIFT;

		tx_info->d11FrameSize = (uint16)(pkttotlen(wlc->osh, p) -
			(D11_TXH_LEN + D11_PHY_HDR_LEN));
	} else if (D11REV_LT(wlc->pub->corerev, 128)) {
		uint8* pktHdr;
		uint tsoHdrSize = 0;
		d11actxh_t* vhtHdr;
		d11actxh_rate_t *rateInfo;
		d11actxh_pkt_t *ppkt_info;

		pktHdr = (uint8*)pkt_data;

#ifdef WLTOEHW
		tsoHdrSize = WLC_TSO_HDR_LEN(wlc, (d11ac_tso_t*)pktHdr);
		tx_info->tsoHdrPtr = (void*)((tsoHdrSize != 0) ? pktHdr : NULL);
		tx_info->tsoHdrSize = tsoHdrSize;
#else
		tx_info->tsoHdrPtr = NULL;
		tx_info->tsoHdrSize = 0;
#endif /* WLTOEHW */

		vhtHdr = (d11actxh_t*)(pktHdr + tsoHdrSize);

		ppkt_info = &(vhtHdr->PktInfo);

		tx_info->TxFrameID = ppkt_info->TxFrameID;
		tx_info->MacTxControlLow = ppkt_info->MacTxControlLow;
		tx_info->MacTxControlHigh = ppkt_info->MacTxControlHigh;

		rateInfo = WLC_TXD_RATE_INFO_GET(vhtHdr, wlc->pub->corerev);
		tx_info->hdrSize = D11AC_TXH_LEN;
		tx_info->hdrPtr = (d11txhdr_t *)vhtHdr;
		tx_info->d11HdrPtr = (void*)((uint8*)vhtHdr + tx_info->hdrSize);
		tx_info->d11FrameSize =
		        pkttotlen(wlc->osh, p) - (tsoHdrSize + tx_info->hdrSize);

		/* a1 holds RA when RA is used; otherwise this field can/should be ignored */
		tx_info->TxFrameRA = (uint8*)(((struct dot11_header *)
		                               (tx_info->d11HdrPtr))->a1.octet);

		tx_info->plcpPtr = (uint8*)(rateInfo[0].plcp);
		/* usu this will work, as long as primary rate is the only one concerned with */
		tx_info->PhyTxControlWord0 = rateInfo[0].PhyTxControlWord_0;
		tx_info->PhyTxControlWord1 = rateInfo[0].PhyTxControlWord_1;
		tx_info->FbwInfo = ltoh16(rateInfo[0].FbwInfo);

		tx_info->seq = ltoh16(ppkt_info->Seq) >> SEQNUM_SHIFT;
	} else { /* D11REV_GE(wlc->pub->corerev, 128) */
		uint8 *pktHdr;
		uint tsoHdrSize = 0;
		d11txh_rev128_t *txh;

		pktHdr = (uint8*)pkt_data;

#ifdef WLTOEHW
		tsoHdrSize = WLC_TSO_HDR_LEN(wlc, (d11ac_tso_t*)pktHdr);
		/* coverity[dead_error_line] */
		tx_info->tsoHdrPtr = (void*)((tsoHdrSize != 0) ? pktHdr : NULL);
		tx_info->tsoHdrSize = tsoHdrSize;
#else
		tx_info->tsoHdrPtr = NULL;
		tx_info->tsoHdrSize = 0;
#endif /* WLTOEHW */

		ASSERT((uint)PKTLEN(wlc->osh, p) >= D11_REV128_TXH_LEN + tsoHdrSize);

		txh = (d11txh_rev128_t*)(pktHdr + tsoHdrSize);

		tx_info->TxFrameID = txh->FrameID;
		tx_info->MacTxControlLow = txh->MacControl_0;
		tx_info->MacTxControlHigh = txh->MacControl_1;

		tx_info->hdrSize = D11_REV128_TXH_LEN;
		tx_info->hdrPtr = (d11txhdr_t *)txh;
		tx_info->d11HdrPtr = (void*)((uint8*)txh + tx_info->hdrSize);
		tx_info->d11FrameSize =
			pkttotlen(wlc->osh, p) - (tsoHdrSize + tx_info->hdrSize);

		/* a1 holds RA when RA is used; otherwise this field can/should be ignored */
		tx_info->TxFrameRA = (uint8*)(((struct dot11_header *)
								(tx_info->d11HdrPtr))->a1.octet);
		tx_info->plcpPtr = NULL;
		/* this will work as long as primary rate is the only one concerned with */
		tx_info->PhyTxControlWord0 = 0;
		tx_info->PhyTxControlWord1 = 0;
		tx_info->FbwInfo = 0;
		tx_info->seq = ltoh16(txh->SeqCtl) >> SEQNUM_SHIFT;
	}
} /* wlc_get_txh_info */

enum wlc_bandunit
wlc_txhinfo2bandunit(wlc_info_t* wlc, wlc_txh_info_t* txh_info)
{
	uint16 mcl;
	enum wlc_bandunit bandunit;

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		uint16 cs = txh_info->hdrPtr->rev128.Chanspec;
		bandunit = CHSPEC_BANDUNIT(cs);
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		d11actxh_t* vhtHdr = &(txh_info->hdrPtr->rev40);
		uint16 cs = (uint16)ltoh16(vhtHdr->PktInfo.Chanspec);
		bandunit = CHSPEC_BANDUNIT(cs);
	} else {
		ASSERT(txh_info);
		mcl = ltoh16(txh_info->MacTxControlLow);
		bandunit = ((mcl & TXC_FREQBAND_5G) == TXC_FREQBAND_5G) ?
		             BAND_5G_INDEX : BAND_2G_INDEX;
	}

	return bandunit;
}

bool
wlc_txh_has_rts(wlc_info_t* wlc, wlc_txh_info_t* txh_info)
{
	uint16 flag = 0;

	ASSERT(txh_info);
	if (D11REV_GE(wlc->pub->corerev, 128)) {
		uint16 ratememidxrateidx = ltoh16(txh_info->hdrPtr->rev128.RateMemIdxRateIdx);
		uint16 rmem_idx = ratememidxrateidx & D11_REV128_RATEIDX_MASK;

		/* function is used in wlc_dotxstatus() only, so set newrate to FALSE */
		const wlc_rlm_rate_store_t *rstore = wlc_ratelinkmem_retrieve_cur_rate_store(wlc,
			rmem_idx, FALSE);
		if (rstore) {
			bool fix_rate = txh_info->MacTxControlHigh & htol16(D11AC_TXC_FIX_RATE);
			int entry = fix_rate ? ((ratememidxrateidx &
				D11_REV128_RATE_SPECRATEIDX_MASK) >>
				D11_REV128_RATE_SPECRATEIDX_SHIFT) : 0;
			flag = (rstore->use_rts & (1 << entry));
		}
	} else if (D11REV_GE(wlc->pub->corerev, 64)) {
		flag = htol16(txh_info->hdrPtr->rev40.rev64.RateInfo[0].RtsCtsControl)
			& D11AC_RTSCTS_USE_RTS;
	} else if (D11REV_GE(wlc->pub->corerev, 40)) {
		flag = htol16(txh_info->hdrPtr->rev40.rev40.RateInfo[0].RtsCtsControl)
			& D11AC_RTSCTS_USE_RTS;
	} else {
		flag = htol16(txh_info->MacTxControlLow) & TXC_SENDRTS;
	}

	return (flag ? TRUE : FALSE);
}

/**
 * p2p normal/excursion queue mechanism related. Attaches a software queue to all hardware d11 FIFOs
 */
static void
wlc_attach_queue(wlc_info_t *wlc, wlc_txq_info_t *qi)
{
	if (wlc->active_queue == qi)
		return;

	/* When psm_watchdog_debug is set, wlc_bmac_tx_fifo_sync will deliberately skip the fifo
	 * sync. If we attach new queue, the counters between software queue and hardware d11 FIFOs
	 * may be inconsistent.
	 */
	if (wlc->psm_watchdog_debug)
		return;

	WL_MQ(("MQ: %s: qi %p\n", __FUNCTION__, OSL_OBFUSCATE_BUF(qi)));

	/* No need to process further (TX FIFO sync, etc.,) if WLC is down */
	if (!wlc->clk) {
		wlc->active_queue = qi;
		return;
	}

	/* set a flag indicating that wlc_tx_fifo_attach_complete() needs to be called */
	wlc->txfifo_attach_pending = TRUE;

	if (wlc->active_queue != NULL) {
		if (MCHAN_ACTIVE(wlc->pub) || (qi == wlc->excursion_queue))
			wlc_detach_queue(wlc, SYNCFIFO);
		else
			wlc_detach_queue(wlc, SUPPRESS_FLUSH_FIFO);
	}

	/* attach the new queue */
	wlc->active_queue = qi;

	/* If the detach is is pending at this point, wlc_tx_fifo_attach_complete() will
	 * be called when the detach completes in wlc_tx_fifo_sync_complete().
	 */

	/* If the detach is not pending (done or call was skipped just above), then see if
	 * wlc_tx_fifo_attach_complete() was called. If wlc_tx_fifo_attach_complete() was not
	 * called (wlc->txfifo_attach_pending), then call it now.
	 */
	if (!wlc->txfifo_detach_pending &&
	    wlc->txfifo_attach_pending) {
		wlc_tx_fifo_attach_complete(wlc);
	}
} /* wlc_attach_queue */

/**
 * p2p/normal/excursion queue mechanism related. Detaches the caller supplied software queue 'qi'
 * from all hardware d11 FIFOs.
 */
static void
wlc_detach_queue(wlc_info_t *wlc, uint8 active_queue_flush_flag)
{
	txq_swq_t *txq_swq;			/**< a non-priority queue backing one d11 FIFO */
	uint txpktpendtot = 0; /* packets that driver/firmware handed over to d11 DMA */
	uint i;

	WL_MQ(("MQ: %s: qi %p\n", __FUNCTION__, OSL_OBFUSCATE_BUF(wlc->active_queue)));

	/* no hardware to sync if we are down */
	if (!wlc->pub->up) {
		WL_MQ(("MQ: %s: !up, so bailing early. TXPEND = %d\n",
		__FUNCTION__, TXPKTPENDTOT(wlc)));
		ASSERT(TXPKTPENDTOT(wlc) == 0);
		return;
	}

	/* If there are pending packets on the fifo, then stop the fifo
	 * processing and re-enqueue packets
	 */
	for (i = 0; i < WLC_HW_NFIFO_INUSE(wlc); i++) {
		txpktpendtot += TXPKTPENDGET(wlc, i);
	}

	if ((txpktpendtot > 0) && (!wlc->txfifo_detach_pending)) {
		/* Do not allow any new packets to flow to the fifo from the new active_queue
		 * while we are synchronizing the fifo for the detached queue.
		 * The wlc_bmac_tx_fifo_sync() process started below will trigger tx status
		 * processing that could trigger new pkt enqueues to the fifo.
		 * The 'hold' call will flow control the fifo.
		 */
		txq_swq = wlc->active_queue->low_txq->swq_table;
		for (i = 0; i < WLC_HW_NFIFO_INUSE(wlc); i++, txq_swq++) {
			__txq_swq_hw_hold_set(txq_swq);
		}

		/* flush the fifos and process txstatus from packets that were sent before the flush
		 * wlc_tx_fifo_sync_complete() will be called when all transmitted
		 * packets' txstatus have been processed. The call may be done
		 * before wlc_bmac_tx_fifo_sync() returns, or after in a split driver.
		 */
		wlc_sync_txfifo_all(wlc, wlc->active_queue, active_queue_flush_flag);
	} else {
		if (TXPKTPENDTOT(wlc) == 0) {
			WL_MQ(("MQ: %s: skipping FIFO SYNC since TXPEND = 0\n", __FUNCTION__));
		} else {
			WL_MQ(("MQ: %s: skipping FIFO SYNC since detach already pending\n",
			__FUNCTION__));
		}

		WL_MQ(("MQ: %s: DMA pend 0:%d 1:%d 2:%d 3:%d 4:%d 5:%d \n", __FUNCTION__,
		       WLC_HW_DI(wlc, 0) ? dma_txpending(WLC_HW_DI(wlc, 0)) : 0,
		       WLC_HW_DI(wlc, 1) ? dma_txpending(WLC_HW_DI(wlc, 1)) : 0,
		       WLC_HW_DI(wlc, 2) ? dma_txpending(WLC_HW_DI(wlc, 2)) : 0,
		       WLC_HW_DI(wlc, 3) ? dma_txpending(WLC_HW_DI(wlc, 3)) : 0,
		       WLC_HW_DI(wlc, 4) ? dma_txpending(WLC_HW_DI(wlc, 4)) : 0,
		       WLC_HW_DI(wlc, 5) ? dma_txpending(WLC_HW_DI(wlc, 5)) : 0));
	}
} /* wlc_detach_queue */

/**
 * Start or continue an excursion from the currently active tx queue context. Switch to the
 * dedicated excursion queue. During excursions from the primary channel, transfers from the txq to
 * the DMA ring are suspended, and the FIFO processing in the d11 core by ucode is halted for all
 * but the VI AC FIFO (Voice and 802.11 control/management). During the excursion the VI FIFO may be
 * used for outgoing frames (e.g. probe requests). When the driver returns to the primary channel,
 * the FIFOs are re-enabled and the txq processing is resumed.
 */
void
wlc_excursion_start(wlc_info_t *wlc)
{
	if (wlc->excursion_active) {
		WL_MQ(("MQ: %s: already active, exiting\n", __FUNCTION__));
		return;
	}

	wlc->excursion_active = TRUE;

	if (wlc->excursion_queue == wlc->active_queue) {
		WL_MQ(("MQ: %s: same queue %p\n",
			__FUNCTION__, OSL_OBFUSCATE_BUF(wlc->excursion_queue)));
		return;
	}

	WL_MQ(("MQ: %s: %p\n", __FUNCTION__, OSL_OBFUSCATE_BUF(wlc->excursion_queue)));

	/* if we are not in an excursion, the active_queue should be the primary_queue */
	ASSERT(wlc->primary_queue == wlc->active_queue || wlc->primary_queue == NULL);

	wlc_attach_queue(wlc, wlc->excursion_queue);
}

/**
 * Terminate the excursion from the active tx queue context. Switch from the excursion queue back to
 * the current primary_queue.
 */
void
wlc_excursion_end(wlc_info_t *wlc)
{
	if (!wlc->excursion_active) {
		WL_MQ(("MQ: %s: not in active excursion, exiting\n", __FUNCTION__));
		return;
	}

	WL_MQ(("MQ: %s: %p\n", __FUNCTION__, OSL_OBFUSCATE_BUF(wlc->primary_queue)));

	wlc->excursion_active = FALSE;

	if (wlc->primary_queue) {
		wlc_attach_queue(wlc, wlc->primary_queue);
	}
}

/**
 * Use the given queue as the new primary queue wlc->primary_queue is updated, detaching the former
 * primary_queue from the fifos if necessary
 */
void
wlc_primary_queue_set(wlc_info_t *wlc, wlc_txq_info_t *new_primary_queue)
{
	WL_MQ(("MQ: %s: qi %p\n", __FUNCTION__, OSL_OBFUSCATE_BUF(new_primary_queue)));

#ifdef RTS_PER_ITF
	/* Update the RTS/CTS information of the per-interface stats
	 * of the current primary queue.
	 */
	wlc_statsupd(wlc);
#endif /* RTS_PER_ITF */

	wlc->primary_queue = new_primary_queue;

	/* if an excursion is active the active_queue should remain on the
	 * excursion_queue. At the end of the excursion, the new primary_queue
	 * will be made active.
	 */
	if (wlc->excursion_active)
		return;

	wlc_attach_queue(wlc, new_primary_queue? new_primary_queue : wlc->excursion_queue);

	/* When the primary queue is not NULL and not set to the def_primary_queue,
	 * check if there are still packets pending on the def_primary_queue low_txqs.
	 */
	if (new_primary_queue && (new_primary_queue != wlc->def_primary_queue)) {
		if (!wlc_low_txq_empty(wlc->def_primary_queue->low_txq)) {
			WL_ERROR(("%s: def_primary_queue->low txq was not empty!!\n",
				__FUNCTION__));
		}
		wlc_low_txq_flush(wlc->txqi, wlc->def_primary_queue->low_txq);
	}
}

static void
wlc_txq_freed_pkt_cnt(wlc_info_t *wlc, void *pkt, uint16 *pkt_cnt)
{
	ASSERT(pkt_cnt != NULL);

	/* For SW AMPDU, only the last MPDU in an AMPDU counts
	 * as txpkt_weight.
	 * Otherwise, 1 for each packet.
	 *
	 * SW AMPDU is only relavant for d11 rev < 40, non-AQM.
	 */
	if (WLPKTFLAG_AMPDU(WLPKTTAG(pkt)) &&
		AMPDU_HOST_ENAB(wlc->pub)) {
		uint16 *mactxcontrollow;
		uint16 mpdu_type;

		mactxcontrollow = wlc_get_txh_mactxcontrollow_ptr(wlc, pkt);
		mpdu_type = (((ltoh16(*mactxcontrollow)) & TXC_AMPDU_MASK) >> TXC_AMPDU_SHIFT);

		if (mpdu_type == TXC_AMPDU_NONE) {
			/* regular MPDUs just count as 1 */
			*pkt_cnt += 1;
		} else if (mpdu_type == TXC_AMPDU_LAST) {
			/* the last MPDU in an AMPDU has a count that equals the txpkt_weight */
			*pkt_cnt += wlc_ampdu_get_txpkt_weight(wlc->ampdu_tx);
		}
	}
	else {
		*pkt_cnt += 1;
	}
}

void
wlc_txq_free_pkt(wlc_info_t *wlc, void *pkt, uint16 *pkt_cnt)
{
	struct scb *scb;
	wlc_txq_freed_pkt_cnt(wlc, pkt, pkt_cnt);

	scb = WLPKTTAGSCBGET(pkt);
	if (scb != NULL) {
		if (D11REV_GE(wlc->pub->corerev, 129) && (WLPKTTAG(pkt)->flags & WLF_UTXD)) {
			SCB_UTXD_INFLT_DECR(scb);
		} else {
			SCB_PKTS_INFLT_FIFOCNT_DECR(wlc, scb, PKTPRIO(pkt));
		}
	}
#ifdef WLTAF
	wlc_taf_txpkt_status(wlc->taf_handle, scb, PKTPRIO(pkt), pkt,
		TAF_TXPKT_STATUS_PKTFREE_DROP);
#endif
	PKTFREE(wlc->osh, pkt, TRUE);
}

#ifdef PROP_TXSTATUS
/* Suppress host generated packet and flush internal once,
 * Returns TRUE if its host pkt else FALSE,
 * Also note packet will be freed in this function.
 */
static bool
wlc_suppress_flush_pkts(wlc_info_t *wlc, void *p, uint16 *pkt_cnt)
{
	struct scb *scb;
	bool ret = FALSE;
	scb = WLPKTTAGSCBGET(p);
	BCM_REFERENCE(scb);
	if (PROP_TXSTATUS_ENAB(wlc->pub) &&
		((WL_TXSTATUS_GET_FLAGS(WLPKTTAG(p)->wl_hdr_information) &
		WLFC_PKTFLAG_PKTFROMHOST))) {
		wlc_suppress_sync_fsm(wlc, scb, p, TRUE);
		wlc_process_wlhdr_txstatus(wlc, WLFC_CTL_PKTFLAG_WLSUPPRESS, p, FALSE);
		ret = TRUE;
	}
	wlc_txq_free_pkt(wlc, p, pkt_cnt);
	return ret;
}

/**
 * Additional processing for full dongle proptxstatus. Packets are freed unless
 * they are retried. NEW_TXQ specific function.
 */
static void *
wlc_proptxstatus_process_pkt(wlc_info_t *wlc, void *pkt, uint16 *pkt_cnt)
{
	if (PROP_TXSTATUS_ENAB(wlc->pub) && MCHAN_ACTIVE(wlc->pub)) {
		struct scb *scb_pkt = WLPKTTAGSCBGET(pkt);

		if (!(scb_pkt && SCB_ISMULTI(scb_pkt))) {
			wlc_suppress_sync_fsm(wlc, scb_pkt, pkt, TRUE);
			wlc_process_wlhdr_txstatus(wlc,
				WLFC_CTL_PKTFLAG_WLSUPPRESS, pkt, FALSE);
		}
		if (wlc_should_retry_suppressed_pkt(wlc, pkt, TX_STATUS_SUPR_PMQ) ||
			(scb_pkt && SCB_ISMULTI(scb_pkt))) {
			/* These are dongle generated pkts
			 * don't free and attempt requeue to interface queue
			 */
			return pkt;
		} else {
			wlc_txq_free_pkt(wlc, pkt, pkt_cnt);
			return NULL;
		}
	} else {
		return pkt;
	}
} /* wlc_proptxstatus_process_pkt */
#endif /* PROP_TXSTATUS */

#ifdef BCMDBG
/** WL_MQ() debugging output helper function */
static void
wlc_txpend_counts(wlc_info_t *wlc, const char* fn)
{
	int i;
	struct {
		uint txp;
		uint pend;
	} counts[NFIFO_EXT_MAX] = {{0, 0}};

	if (!WL_MQ_ON()) {
		return;
	}

	for  (i = 0; i < WLC_HW_NFIFO_INUSE(wlc); i++) {
		hnddma_t *di = WLC_HW_DI(wlc, i);

		if (di != NULL) {
			counts[i].txp = dma_txp(di);
			counts[i].pend = TXPKTPENDGET(wlc, i);
		}
	}

	// XXX: only legacy FIFOs are printed
	WL_MQ(("MQ: %s: TXPKTPEND/DMA "
	       "[0]:(%d,%d) [1]:(%d,%d) [2]:(%d,%d) [3]:(%d,%d) [4]:(%d,%d) [5]:(%d,%d)\n",
	       fn,
	       counts[0].pend, counts[0].txp,
	       counts[1].pend, counts[1].txp,
	       counts[2].pend, counts[2].txp,
	       counts[3].pend, counts[3].txp,
	       counts[4].pend, counts[4].txp,
	       counts[5].pend, counts[5].txp));
}
#else /* BCMDBG */
#define wlc_txpend_counts(w, fn)
#endif /* BCMDBG */

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
static void
wlc_low_txq_account(wlc_info_t *wlc, txq_t *txq, uint fifo,
	uint8 flag, uint16* pkt_cnt, bool partial_reclaim)
{
	void *pkt;
	struct spktq *spktq;		/**< single priority packet queue */
	txq_swq_t *txq_swq;		/**< a non-priority queue backing one d11 FIFO */
	struct spktq pkt_list;		/**< single priority packet queue */
	uint8 to_fifo = 0;
	uint8 flipEpoch = 0, tofifo_flipEpoch = 0;
	uint8 lastEpoch = HEAD_PKT_FLUSHED, tofifo_lastEpoch = HEAD_PKT_FLUSHED;
	wlc_txq_info_t *qi = wlc->txfifo_detach_transition_queue;

	if (flag == MOVEFIFO_FLUSHSCB) {
		/* Store to_fifo once as a speed optimization */
		to_fifo = wlc_fifo_index_peek(wlc->fifo, wlc->fifoflush_scb,
			wlc_fifo_get_ac(wlc->fifo, fifo));
	}

#if defined(BCMHWA)
	HWA_PKTPGR_EXPR({
		if (partial_reclaim) {
			if (wlc_ampdu_get_epoch(wlc->ampdu_tx, fifo)) {
				lastEpoch = TOE_F2_EPOCH;
			} else {
				lastEpoch = 0;
			}
			flipEpoch |= TXQ_PKT_DEL;
		}
	});
#endif /* BCMHWA */

	spktq_init(&pkt_list, PKTQ_LEN_MAX);

	txq_swq = TXQ_SWQ(txq, fifo);
	spktq = &txq_swq->spktq;
	while ((pkt = spktq_deq(spktq))) {
		/* SWWLAN-39516: Removing WLF3_TXQ_SHORT_LIFETIME optimization.
		 * Could be addressed by pkt lifetime?
		 */
		if (WLPKTTAG(pkt)->flags3 & WLF3_TXQ_SHORT_LIFETIME) {
			WL_INFORM(("MQ: %s: cancel TxQ short-lived pkt %p"
				" during chsw...\n",
				__FUNCTION__, OSL_OBFUSCATE_BUF(pkt)));
			flipEpoch |= TXQ_PKT_DEL;
			tofifo_flipEpoch |= TXQ_PKT_DEL;
			wlc_txq_free_pkt(wlc, pkt, pkt_cnt);
			continue;
		}

		/* Do not assert when psm_watchdog_debug is set,
		 * wlc_bmac_tx_fifo_sync will skip the fifo sync.
		 */
		if (!wlc->psm_watchdog_debug) {
			if (flag == FLUSHFIFO_FLUSHSCB) {
				ASSERT(WLPKTTAGSCBGET(pkt) != NULL);
				ASSERT(wlc->fifoflush_scb != NULL);
			}
		}

		/* For the following cases, pkt is freed here:
		 * 1. flag is FLUSHFIFO and all packets should be freed.
		 * 2. flag is FLUSHFIFO_FLUSHID and the flowring ID matches.
		 * 3. flag is FLUSHFIFO_FLUSHSCB and the scb pointer matches.
		 */
		if ((flag == FLUSHFIFO) ||
#ifdef PROP_TXSTATUS
			((flag == FLUSHFIFO_FLUSHID) &&
				(wlc->fifoflush_id == PKTFRAGFLOWRINGID(wlc->osh, pkt))) ||
#endif /* PROP_TXSTATUS */
			((flag == FLUSHFIFO_FLUSHSCB) &&
				(wlc->fifoflush_scb == WLPKTTAGSCBGET(pkt)))) {
			flipEpoch |= TXQ_PKT_DEL;
			wlc_txq_free_pkt(wlc, pkt, pkt_cnt);
			continue;
		}
#ifdef PROP_TXSTATUS
		else if (flag == SUPPRESS_FLUSH_FIFO) {
			if (wlc_suppress_flush_pkts(wlc, pkt, pkt_cnt)) {
				WL_MQ(("MQ: %s: packet %p suppressed\n",
					__FUNCTION__, OSL_OBFUSCATE_BUF(pkt)));
			}
			flipEpoch |= TXQ_PKT_DEL;
			continue;
		}
#endif /* PROP_TXSTATUS */
		else if (flag == MOVEFIFO_FLUSHSCB) {
			if (wlc->fifoflush_scb == WLPKTTAGSCBGET(pkt)) {
				uint16 *frameid = wlc_get_txh_frameid_ptr(wlc, pkt);
				wlc_txq_freed_pkt_cnt(wlc, pkt, pkt_cnt);

				if (D11REV_GE(wlc->pub->corerev, 129) &&
					WLPKTTAG(pkt)->flags & WLF_UTXD) {
					continue;
				}

				ASSERT(WLPKTTAG(pkt)->flags & WLF_TXHDR);
				/* Update the TX FIFO index in the TXH */
				*frameid = htol16(wlc_frameid_set_fifo(wlc, ltoh16(*frameid),
					to_fifo));

				/* Verify if the tofifo_lastEpoch needs to be updated */
				if (tofifo_lastEpoch == HEAD_PKT_FLUSHED) {
					void *tailpkt = TXQ_SWQ(txq, to_fifo)->spktq.q.tail;

					if (tailpkt) {
						wlc_txh_info_t txh_info;

						wlc_get_txh_info(wlc, tailpkt, &txh_info);
						/* Force epoch toggle for first moved packet */
						tofifo_lastEpoch =
							(wlc_txh_get_epoch(wlc, &txh_info) ?
							TOE_F2_EPOCH : 0);
						tofifo_flipEpoch |= TXQ_PKT_DEL;
					} else {
						if (wlc_ampdu_get_epoch(wlc->ampdu_tx, to_fifo)) {
							tofifo_lastEpoch = TOE_F2_EPOCH;
						} else {
							tofifo_lastEpoch = 0;
						}
						tofifo_flipEpoch |= TXQ_PKT_DEL;
					}
				}

				wlc_epoch_upd(wlc, pkt, &tofifo_flipEpoch, &tofifo_lastEpoch);
				tofifo_flipEpoch &= ~TXQ_PKT_DEL;

				wlc_low_txq_enq(wlc, txq, to_fifo, pkt);

				flipEpoch |= TXQ_PKT_DEL;
				continue;
			} else {
				tofifo_flipEpoch |= TXQ_PKT_DEL;
			}
		}

		/* For the following cases, pkt needs to be queued back:
		 * 1. flag is SYNCFIFO and packets are supposed to be queued back.
		 * 2. flag is FLUSHFIFO_FLUSHID but the flowring ID doesn't match.
		 * 3. flag is FLUSHFIFO_FLUSHSCB but the scb pointer doesn't match.
		 */
#ifdef PROP_TXSTATUS
		/* Go through the proptxstatus process before queuing back */
		if (wlc_proptxstatus_process_pkt(wlc, pkt, pkt_cnt) == NULL) {
			flipEpoch |= TXQ_PKT_DEL;
			tofifo_flipEpoch |= TXQ_PKT_DEL;
			continue;
		}
#endif /* PROP_TXSTATUS */

		/* not a host packet or pktfree not allowed
		 * enqueue it back
		 */
		wlc_epoch_upd(wlc, pkt, &flipEpoch, &lastEpoch);
		/* clear pkt delete condition */
		flipEpoch &= ~TXQ_PKT_DEL;
		spktq_enq(&pkt_list, pkt);
	}

	spktq_enq_head_list(spktq, &pkt_list);
	spktq_deinit(&pkt_list);

	if (AMPDU_AQM_ENAB(wlc->pub) && (flag == MOVEFIFO_FLUSHSCB) &&
	    (tofifo_lastEpoch != HEAD_PKT_FLUSHED)) {
		void *tailpkt = TXQ_SWQ(txq, to_fifo)->spktq.q.tail;

		/* Packets moved to new FIFO should also update ampdu
		 * aggregate determine params according to the moved packets.
		 */
		if (tailpkt) {
			wlc_txh_info_t txh_info;

			wlc_get_txh_info(wlc, tailpkt, &txh_info);

			/* After calling wlc_ampdu_chgnsave_epoch(), ampdu determine params updates,
			 * but epoch value in ampdu_tx might incorrect. wlc_tx_fifo_epoch_save()
			 * call to wlc_ampdu_set_epoch() can correct the epoch value.
			 */
			wlc_ampdu_chgnsav_epoch(wlc->ampdu_tx, to_fifo, AMU_EPOCH_CHG_MPDU,
				wlc->fifoflush_scb, (uint8)PKTPRIO(tailpkt), FALSE, &txh_info);
		}

		wlc_tx_fifo_epoch_save(wlc, qi, to_fifo);
	}

	/* Remove node from active list if there is no pending packet */
	if ((spktq_n_pkts(spktq) == 0) && (hpktq_n_pkts(txq_swq) == 0)) {
		dll_delete(&txq_swq->node);
		dll_append(&txq->empty, &txq_swq->node);
	}

#if defined(BCMHWA)
	HWA_PKTPGR_EXPR({
		if (AMPDU_AQM_ENAB(wlc->pub) && partial_reclaim) {
			wlc_tx_fifo_epoch_save(wlc, qi, fifo);
		}
	});
#endif /* BCMHWA */
}

/* Update and save epoch value of the last packet in the queue.
 * Epoch value shall be saved for the queue that is getting dettached,
 * Same shall be restored upon "re-attach" of the given queue in wlc_tx_fifo_attach_complete.
 */
static void
wlc_tx_fifo_epoch_save(wlc_info_t *wlc, wlc_txq_info_t *qi, uint fifo_idx)
{
	txq_t *txq;		/**< e.g. the low queue, consisting of a queue for every d11 fifo */
	wlc_txh_info_t txh_info;

	ASSERT(qi != NULL);
	txq = qi->low_txq;
	TXQ_ASSERT(txq, fifo_idx);

	if (fifo_idx < WLC_HW_NFIFO_INUSE(wlc)) {
		void *pkt = TXQ_SWQ(txq, fifo_idx)->spktq.q.tail;
		if (pkt) {
			if (D11REV_GE(wlc->pub->corerev, 129) && WLPKTTAG(pkt)->flags & WLF_UTXD)
				return;

			wlc_get_txh_info(wlc, pkt, &txh_info);
			qi->epoch[fifo_idx] = wlc_txh_get_epoch(wlc, &txh_info);
			/* restore the epoch bit setting for this queue fifo */
			if (qi == wlc->active_queue) {
				wlc_ampdu_set_epoch(wlc->ampdu_tx, fifo_idx, qi->epoch[fifo_idx]);
			}
		}
	}
}

/* After deletion of packet to flip epoch bit of next enque packets in low_txq if last save epoch
 * value equal to next enque packet epoch value otherwise don't flip epoch value of enqueue
 * packets.
 * flipEpoch 0th bit is used to keep track of last packet state i.e deleted or enq.
 * flipEpoch 1st bit is uesed to keep track of flip epoch state.
*/

void
wlc_epoch_upd(wlc_info_t *wlc, void *pkt, uint8 *flipEpoch, uint8 *lastEpoch)
{
	wlc_txh_info_t txh_info;
	uint8* tsoHdrPtr;

	if (D11REV_GE(wlc->pub->corerev, 129) && WLPKTTAG(pkt)->flags & WLF_UTXD)
		return;

	wlc_get_txh_info(wlc, pkt, &txh_info);

	tsoHdrPtr = txh_info.tsoHdrPtr;
	if (tsoHdrPtr == NULL)
		return;

	/* if last packet was deleted then storing the epoch flip state in 1st bit of flipEpoch */
	if ((*flipEpoch & TXQ_PKT_DEL) && (*lastEpoch != HEAD_PKT_FLUSHED)) {
		if ((*lastEpoch & TOE_F2_EPOCH) == (tsoHdrPtr[2] & TOE_F2_EPOCH))
			*flipEpoch |= TOE_F2_EPOCH;
		else
			*flipEpoch &= ~TOE_F2_EPOCH;
	}

	/* flipping epoch bit of packet if flip epoch state bit (1st bit) is set */
	if (*flipEpoch & TOE_F2_EPOCH)
		tsoHdrPtr[2] ^= TOE_F2_EPOCH;
	/* storing current epoch bit */
	*lastEpoch = (tsoHdrPtr[2] & TOE_F2_EPOCH);
}

#ifdef AP
/**
 * This function is used to 'reset' bcmc administration for all applicable bsscfgs to prevent bcmc
 * fifo lockup.
 * Should only be called when there is no more bcmc traffic pending due to flush.
 */
void
wlc_tx_fifo_sync_bcmc_reset(wlc_info_t *wlc)
{
	wlc_bsscfg_t *cfg;
	uint i;

	FOREACH_AP(wlc, i, cfg) {
		if (WLC_BCMCSCB_GET(wlc, cfg)) {
#ifdef MBSS
			if (MBSS_ENAB(wlc->pub)) {
				/* Let's reset the FIDs since we have completed flush */
				wlc_mbss_bcmc_reset(wlc, cfg);
			} else
#endif /* MBSS */
			{
				wlc_bmac_write_shm(wlc->hw, M_BCMC_FID(wlc), INVALIDFID);
			}
			wlc_apps_dotxstatus_bcmc(wlc, cfg, NULL);
		}
	}
}
#endif /* AP */

/**
 * called by BMAC, related to new ucode method of suspend & flushing d11 core tx
 * fifos.
 */
void
wlc_tx_fifo_sync_complete(wlc_info_t *wlc, void *fifo_bitmap, uint8 flag)
{
	struct spktq pkt_list; /**< single priority packet queue */
	void *pkt;
	txq_t *txq;		/**< e.g. the low queue, consisting of a queue for every d11 fifo */
	txq_swq_t *txq_swq;	/**< a non-priority queue backing one d11 FIFO */
	uint fifo;
	uint16 pkt_cnt;
	bool partial_reclaim;
	hnddma_t *di;

	ASSERT(fifo_bitmap != NULL);

	/* check for up first? If we down the driver, maybe we will
	 * get and ignore this call since we flush all txfifo pkts,
	 * and wlc->txfifo_detach_pending would be false.
	 */
	ASSERT(wlc->txfifo_detach_pending);
	ASSERT(wlc->txfifo_detach_transition_queue != NULL);

	WL_MQ(("MQ: %s: TXPENDTOT = %d\n", __FUNCTION__, TXPKTPENDTOT(wlc)));
	wlc_txpend_counts(wlc, __FUNCTION__);

	/* init a local pkt queue to shuffle pkts in this fn */
	spktq_init(&pkt_list, PKTQ_LEN_MAX);

	/* get the destination queue */
	txq = wlc->txfifo_detach_transition_queue->low_txq;

	/* pull all the packets that were queued to HW layer,
	 * and push them on the low software TxQ
	 */
	txq_swq = txq->swq_table;
	for (fifo = 0; fifo < WLC_HW_NFIFO_INUSE(wlc); fifo++, txq_swq++) {
		if (!isset(fifo_bitmap, fifo)) {
			/* Handle no packets in DMA but low Q */
			if (AMPDU_AQM_ENAB(wlc->pub)) {
				wlc_tx_fifo_epoch_save(wlc,
					wlc->txfifo_detach_transition_queue, fifo);
			}
			continue;
		}
		pkt_cnt = 0;
		partial_reclaim = FALSE;

		di = WLC_HW_DI(wlc, fifo);

		if (di == NULL)
			continue;

#if defined(BCMHWA)
		HWA_PKTPGR_EXPR({
			// Account for the collective packets in hdbm pktq at this stage.
			if (hpktq_n_pkts(txq_swq) > 0) {
				wlc_hwa_tx_fifo_sync_hpktq(wlc, txq, fifo, flag, TRUE);
				partial_reclaim = TRUE;
			}

			// Pagein packets in HWA OvfQ shadow to SW shadow.
			while (hwa_pktpgr_txfifo_shadow_reclaim(WL_HWA_DEVP(wlc),
				WLC_HW_MAP_TXFIFO(wlc, fifo), TRUE)) {
				// There are only MGMT packets in ULMU_TRIG_FIFO.
				// It will not occupy DDBM resources.
				// Don't need to do partial reclaim for this fifo.
				if (fifo != ULMU_TRIG_FIFO) {
					wlc_hwa_tx_fifo_shadow_account(wlc, txq, fifo, flag);
					partial_reclaim = TRUE;
				}
			}

			// Remove specific packets in PageOut Req Q to SW shadow.
			hwa_pktpgr_pageout_ring_shadow_reclaim(WL_HWA_DEVP(wlc),
				WLC_HW_MAP_TXFIFO(wlc, fifo));
		});
#endif /* BCMHWA */

		while (NULL != (pkt = wlc_bmac_dma_getnexttxp(wlc, fifo, HNDDMA_RANGE_ALL))) {
#ifdef WLC_MACDBG_FRAMEID_TRACE
			wlc_macdbg_frameid_trace_sync(wlc->macdbg, pkt);
#endif
			spktq_enq(&pkt_list, pkt);
		}
		/* The DMA should have been cleared of all packets by the
		 * wlc_bmac_dma_getnexttxp() loop above.
		 */
		ASSERT(dma_txactive(di) == 0);

		WL_MQ(("MQ: %s: fifo %d: collected %d pkts\n", __FUNCTION__,
			fifo, spktq_n_pkts(&pkt_list)));

		/* enqueue the collected pkts from DMA ring
		* at the head of the low software TxQ
		*/
		if (spktq_n_pkts(&pkt_list) > 0) {
			/* Move swq_node from empty list */
			dll_delete(&txq_swq->node);
			spktq_enq_head_list(&txq_swq->spktq, &pkt_list);
			/* Move swq_node to active list */
			dll_append(&txq->active, &txq_swq->node);
		}

		/*
		* Account for the collective packets in low txq at this stage.
		* Need to suppress or flush all packets currently in low txq
		* depending on the argument 'flag'
		*/
		wlc_low_txq_account(wlc, txq, fifo, flag, &pkt_cnt, partial_reclaim);

#if defined(BCMHWA) && defined(BCMDBG)
		HWA_PKTPGR_EXPR({
			/* If partial_reclaim is TRUE. Check the counter here.
			 * In case of FLUSHFIFO_FLUSHSCB,
			 * the the packet in flight for this fifo should be 0.
			 */
			if (partial_reclaim) {
				scb_t *scb = (scb_t*)wlc->fifoflush_scb;
				if ((flag == FLUSHFIFO_FLUSHSCB) && !SCB_INTERNAL(scb)) {
					enum wme_ac ac;
					for (ac = 0; ac < AC_COUNT; ac++) {
						if (FIFO_INDEX_GET_AC(scb, ac) == fifo) {
							break;
						}
					}
					if (ac != AC_COUNT) {
						ASSERT(wlc_fifo_scb_inflt_ac(scb, ac) == 0);
					}
				}
			}
		});
#endif /* BCMHWA && BCMDBG */

		if ((TXPKTPENDGET(wlc, fifo) > 0) && (pkt_cnt > 0)) {
			/* The pkt_cnt is the weighted pkt count for TXPKTPEND. Normally this
			 * is just a count of pkts, but for SW AMPDU, an entire AMPDU of pkts
			 * is a weighted value, txpkt_weight, typically 2.
			 */
			wlc_txfifo_complete(wlc, NULL, fifo, pkt_cnt);
		}

		spktq_deinit(&pkt_list);

#ifdef AP
		/**
		 * Rather than checking fids on a per-packet basis using wlc_mbss_dotxstatus_mcmx
		 * for each freed bcmc packet in wlc_low_txq_account, simply 'reset' the bcmc
		 * administration. (a speed optimization)
		 */
		if ((fifo == TX_BCMC_FIFO) &&
			(wlc->txfifo_detach_transition_queue != wlc->excursion_queue)) {
			wlc_tx_fifo_sync_bcmc_reset(wlc);
		}
#endif /* AP */

		/*
		 * Make a backup of epoch bit settings before completing queue switch.
		 * Packets on the dma ring have already been accounted for at this point and
		 * low_q packet state is finalized. Now find and save appropriate epoch bit
		 * setting which shall be restored when switching back to this queue.
		 *
		 * For multi-destination traffic case, each destination traffic packet stream
		 * would be differentiated from each other by an epoch flip which would have
		 * already been well handled at this point. We are only concerned about the
		 * final packet in order to determine the appropriate epoch state of the FIFO,
		 * for use from the next pkt on wards.
		 *
		 * Note : It would not be handling the selective TID
		 * flush cases (FLUSHFIFO_FLUSHID)
		 * TBD :
		 * 1. Epoch save and restore for NON -AQM cases (SWAMPDU and HWAMPDU)
		 * 2. Handle selective TID flushes (FLUSHFIFO_FLUSHID)
		*/
		if (AMPDU_AQM_ENAB(wlc->pub)) {
			wlc_tx_fifo_epoch_save(wlc, wlc->txfifo_detach_transition_queue, fifo);
		}

		/* clear any HW 'stopped' flags since the hw fifo is now empty */
		__txq_swq_hw_stop_clr(txq_swq);
	} /* end for */

	if (IS_EXCURSION_QUEUE(txq, wlc->txqi)) {
		/* Packets being sent during an excursion should only be valid
		 * for the duration of that excursion.  If any packets are still
		 * in the queue at the end of excursion should be flushed.
		 * Flush both the high and low txq.
		 */
		wlc_low_txq_flush(wlc->txqi, txq);
		cpktq_flush(wlc, &wlc->excursion_queue->cpktq);
	}

	wlc->txfifo_detach_transition_queue = NULL;
	wlc->txfifo_detach_pending = FALSE;
#ifdef WLCFP
	/* Update cfp state for all SCBs : txfifo_detach_pending */
	wlc_cfp_state_update(wlc->cfp);
#endif /* WLCFP */
} /* wlc_tx_fifo_sync_complete */

void
wlc_tx_fifo_attach_complete(wlc_info_t *wlc)
{
	int i;
	int new_count;
	txq_t *txq;		/**< e.g. the low queue, consisting of a queue for every d11 fifo */
	txq_swq_t *txq_swq;	/**< a non-priority queue backing one d11 FIFO */
#ifdef BCMDBG
	struct {
		uint pend;
		uint pkt;
		uint txp;
		uint buffered;
	} counts [NFIFO_LEGACY] = {{0, 0, 0, 0}};
	hnddma_t *di;
#endif /* BCMDBG */

	wlc->txfifo_attach_pending = FALSE;

	/* bail out if no new active_queue */
	if (wlc->active_queue == NULL) {
		return;
	}

	txq = wlc->active_queue->low_txq;

	/* for each HW fifo, set the new pending count when we are attaching a new queue */
	for  (i = 0, txq_swq = txq->swq_table; i < WLC_HW_NFIFO_INUSE(wlc); i++, txq_swq++) {
		TXPKTPENDCLR(wlc, i);

		/* The TXPKTPEND count is the number of pkts in the low queue, except for
		 * SW AMPDU. For SW AMPDUs, the TXPKTPEND count is a weighted value, txpkt_weight,
		 * typically 2, for an entire SW AMPDU. The buffered time holds the
		 * weighted TXPKTPEND count we want to use.
		 */
		new_count = __txq_swq_buffered_time(txq_swq);

		/* Initialize per FIFO TXPKTPEND count */
		TXPKTPENDINC(wlc, i, (uint16)new_count);

		/* Clear the hw fifo hold since we are done with the fifo_detach */
		__txq_swq_hw_hold_clr(txq_swq);

#if defined(HWA_PKTPGR_BUILD)
		wlc_hwa_tx_fifo_hpq_hw_fill(wlc, txq, i);
#endif

		if (new_count == 0 && __txq_swq_buffered_time(txq_swq) > 0) {
			WL_ERROR(("wl%d: wlc_tx_fifo_attach_complete: "
			          "Mismatch: fifo %d TXPKTPEND %d buffered %d\n",
			          wlc->pub->unit, i, new_count,
			          __txq_swq_buffered_time(txq_swq)));
			ASSERT(0);
		}

		/* queue must be in active list if packets queued */
		if (spktq_n_pkts(&txq_swq->spktq) > 0) {
			dll_delete(&txq_swq->node);
			dll_append(&txq->active, &txq_swq->node);
		} else {
			dll_delete(&txq_swq->node);
			dll_append(&txq->empty, &txq_swq->node);
		}

#ifdef BCMDBG
		if (i < NFIFO_LEGACY) {
			counts[i].pend = new_count;
			counts[i].pkt = spktq_n_pkts(&txq_swq->spktq);
			counts[i].buffered = __txq_swq_buffered_time(txq_swq);
			/* At the point we are attaching a new queue, the DMA should have been
			 * cleared of all packets
			 */
			if ((di = WLC_HW_DI(wlc, i)) != NULL) {
				if ((dma_txactive(di) != 0)) {
					hnddma_t *dbg_di;
					int n = 0;
					int txact_cnt[NFIFO_LEGACY] = {0};

					for (n = 0; n < NFIFO_LEGACY; n++) {
						if ((dbg_di = WLC_HW_DI(wlc, n)) != NULL) {
							txact_cnt[n] = dma_txactive(dbg_di);
						}
					}
					WL_ERROR(("wl%d: fifo %d error!! fifo 0-5 retain pkts "
						"%d %d %d %d %d %d\n", wlc->pub->unit, i,
						txact_cnt[0], txact_cnt[1], txact_cnt[2],
						txact_cnt[3], txact_cnt[4], txact_cnt[5]));

					/* Do not assert when psm_watchdog_debug is set,
					 * wlc_bmac_tx_fifo_sync will deliberately skip the
					 * fifo sync.
					 */
					if (!wlc->psm_watchdog_debug) {
						ASSERT(dma_txactive(di) == 0);
					}
				}

				counts[i].txp = dma_txp(di);
			}
		}
#endif /* BCMDBG */

		if (AMPDU_AQM_ENAB(wlc->pub)) {
			uint8 epoch;
			wlc_txh_info_t txh_info;
			void *pkt = txq_swq->spktq.q.tail;

			if (pkt && !(WLPKTTAG(pkt)->flags & WLF_UTXD)) {
				wlc_get_txh_info(wlc, pkt, &txh_info);
				epoch = wlc_txh_get_epoch(wlc, &txh_info);
				if (wlc->active_queue->epoch[i] != epoch) {
					wlc->active_queue->epoch[i] = epoch;
				}
			}

			/* restore the epoch bit setting for this queue fifo */
			wlc_ampdu_set_epoch(wlc->ampdu_tx, i, wlc->active_queue->epoch[i]);
		}
	}

	for  (i = 0; i < WLC_HW_NFIFO_INUSE(wlc); i++) {
		/* Process completions from all fifos.
		 * TXFIFO_COMPLETE is called with pktpend count as
		 * zero in order to ensure that FIFO blocks are reset properly
		 * as attach is now complete.
		 */
		wlc_txfifo_complete(wlc, NULL, i, 0);
	}

	WL_MQ(("MQ: %s: New TXPKTPEND/pkt/time/DMA "
	       "[0]:(%d,%d,%d,%d) [1]:(%d,%d,%d,%d) [2]:(%d,%d,%d,%d) "
	       "[3]:(%d,%d,%d,%d) [4]:(%d,%d,%d,%d) [5]:(%d,%d,%d,%d)\n",
	       __FUNCTION__,
	       counts[0].pend, counts[0].pkt, counts[0].buffered, counts[0].txp,
	       counts[1].pend, counts[1].pkt, counts[0].buffered, counts[1].txp,
	       counts[2].pend, counts[2].pkt, counts[0].buffered, counts[2].txp,
	       counts[3].pend, counts[3].pkt, counts[0].buffered, counts[3].txp,
	       counts[4].pend, counts[4].pkt, counts[0].buffered, counts[4].txp,
	       counts[5].pend, counts[5].pkt, counts[0].buffered, counts[5].txp));
} /* wlc_tx_fifo_attach_complete */

void
wlc_tx_fifo_scb_flush(wlc_info_t *wlc, struct scb *remove)
{
	uint8 fifo_bitmap[TX_FIFO_BITMAP_SIZE_MAX] = { 0 };
	wlc_txq_info_t *main_qi, *qi;
	wlc_fifo_sta_bitmap(wlc->fifo, remove, fifo_bitmap);
	if (SCB_BCMC(remove)) {
		fifo_bitmap[0] |= (1 << TX_BCMC_FIFO);
	}
	ASSERT(wlc->fifoflush_scb == NULL);
	wlc->fifoflush_scb = remove;
	if (SCB_BSSCFG(remove) && SCB_BSSCFG(remove)->wlcif) {
		main_qi = SCB_BSSCFG(remove)->wlcif->qi;
	} else {
		main_qi = wlc->active_queue;
	}

	wlc_sync_txfifos(wlc, main_qi, fifo_bitmap, FLUSHFIFO_FLUSHSCB);
	wlc_txq_pktq_scb_filter(wlc, WLC_GET_CQ(main_qi), remove);

	if ((SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(remove) != 0)) {
		/* If there are still packets pending then they are on a different txq */

		for (qi = wlc->tx_queues; qi != NULL; qi = qi->next) {
			if (qi == main_qi) {
				continue;
			}
			wlc_sync_txfifos(wlc, qi, fifo_bitmap, FLUSHFIFO_FLUSHSCB);
			wlc_txq_pktq_scb_filter(wlc, WLC_GET_CQ(qi), remove);
			if (SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(remove) == 0) {
				break;
			}
		}
	}
	wlc->fifoflush_scb = NULL;

#ifdef WL_TXPKTPEND_SYNC
	if ((SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(remove) != 0)) {
		/* It is not expected to have frames that belong to this scb */
		WL_ERROR(("wl%d %s "MACF" BCMC %d not flushed %u\n",
			wlc->pub->unit, __FUNCTION__,
			ETHER_TO_MACF(remove->ea), SCB_BCMC(remove),
			SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(remove)));

		/* Force to sync all FiFos */
		wlc_pktpend_sync_flush(wlc);
	}
#else
	/* Do not assert when psm_watchdog_debug is set,
	 * wlc_bmac_tx_fifo_sync will deliberately skip the fifo sync.
	 */
	if (!wlc->psm_watchdog_debug) {
		if ((SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(remove) != 0)) {
			WL_ERROR(("wl%d %s "MACF" not flushed %u\n",
				wlc->pub->unit, __FUNCTION__,
				ETHER_TO_MACF(remove->ea),
				SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(remove)));
#ifdef BCMDBG
			{
			uint32 npkts, fifo_idx;
			uint8 ac;
			for (ac = 0; ac < AC_COUNT; ac++) {
				npkts = wlc_fifo_scb_inflt_ac(remove, ac);
				if (npkts != 0) {
					fifo_idx = FIFO_INDEX_GET_AC(remove, ac);
					WL_ERROR(("ampdu_dbg: FIFO-%d pktinflt %d "
						"TXPEND %d =>\n", fifo_idx,
						npkts, TXPKTPENDGET(wlc, fifo_idx)));
#ifdef WLC_MACDBG_FRAMEID_TRACE
					wlc_macdbg_frameid_trace_dump(wlc->macdbg, fifo_idx);
#endif /* WLC_MACDBG_FRAMEID_TRACE */
				}
			}
			}
#endif /* BCMDBG */
		}

		ASSERT(SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(remove) == 0);
	}
#endif /* WL_TXPKTPEND_SYNC */
}

void
wlc_tx_fifo_scb_fifo_swap(wlc_info_t *wlc, struct scb *remove, void *fifo_bitmap)
{
	wlc_txq_info_t *qi = wlc->primary_queue;

	if (qi == NULL) {
		qi = wlc->active_queue;
	}
	ASSERT(wlc->fifoflush_scb == NULL);
	/* Flush packets in queues and FIFOs. */
	wlc->fifoflush_scb = remove;
	wlc_sync_txfifos(wlc, qi, fifo_bitmap, MOVEFIFO_FLUSHSCB);
	wlc->fifoflush_scb = NULL;
}

/**
 * Sync low TX queue.
 *
 * @param wlc           wlc_info_t pointer
 * @param qi		wlc_txq_info_t pointer
 * @param fifo		Fifo index
 * @param flag		Flag to pass to wlc_low_txq_account
 */
static inline void
wlc_low_txq_sync(wlc_info_t *wlc, wlc_txq_info_t *qi, uint fifo_idx, uint8 flag)
{
	txq_t *txq;		/**< e.g. the low queue, consisting of a queue for every d11 fifo */
	txq_swq_t *txq_swq;	/**< a non-priority queue backing one d11 FIFO */
	uint16 pkt_cnt = 0;
	bool partial_reclaim = FALSE;

	txq = qi->low_txq;
	TXQ_ASSERT(txq, fifo_idx);
	txq_swq = TXQ_SWQ(txq, fifo_idx);

	if ((spktq_n_pkts(&txq_swq->spktq) == 0) && (hpktq_n_pkts(txq_swq) == 0)) {
		return;
	}

	wlc->txfifo_detach_transition_queue = qi;
	wlc->txfifo_detach_pending = TRUE;

#ifdef WLTAF
	/* Defer taf schedule to prevent packet enqueue during lowtxq flush */
	wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */

#if defined(BCMHWA)
	HWA_PKTPGR_EXPR({
		// Account for the collective packets in hdbm pktq at this stage.
		if (hpktq_n_pkts(txq_swq) > 0) {
			wlc_hwa_tx_fifo_sync_hpktq(wlc, txq, fifo_idx, flag, FALSE);
			partial_reclaim = TRUE;
		}
	});
#endif /* BCMHWA */

	wlc_low_txq_account(wlc, txq, fifo_idx, flag, &pkt_cnt, partial_reclaim);
	__txq_swq_dec(txq_swq, fifo_idx, pkt_cnt);
	wlc->txfifo_detach_transition_queue = NULL;
	wlc->txfifo_detach_pending = FALSE;

#ifdef WLTAF
	wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */
}

/**
 * Check if the specified TX FIFO has packets pending.
 *
 * @param wlc           wlc_info_t pointer
 * @param fifo		Fifo index
 * @return		TRUE if packets are pending.
 */
static inline bool
wlc_tx_pktpend_check(wlc_info_t *wlc, uint fifo)
{
	ASSERT(fifo < WLC_HW_NFIFO_INUSE(wlc));

	return TXPKTPENDGET(wlc, fifo) != 0;
}

/**
 * Sync multiple FIFOs/low TXQs with bitmap.
 *
 * @param wlc           wlc_info_t pointer
 * @param qi		wlc_txq_info_t pointer
 * @param fifo_bitmap	Fifo bitmap
 * @param flag		Flag to pass to wlc_bmac_tx_fifo_sync
 */
void
wlc_sync_txfifos(wlc_info_t *wlc, wlc_txq_info_t *qi, void *fifo_bitmap, uint8 flag)
{
	txq_t *txq;		/**< e.g. the low queue, consisting of a queue for every d11 fifo */
	txq_swq_t *txq_swq;	/**< a non-priority queue backing one d11 FIFO */
	int i;

	/* protective call to avoid any recursion because of fifo sync */
	if (wlc->txfifo_detach_pending)
		return;

	ASSERT(qi != NULL);
	txq = qi->low_txq;

	if (qi != wlc->active_queue) {
		/* sync low txqs only, cannot touch the FIFO */
		for (i = 0; i < WLC_HW_NFIFO_INUSE(wlc); i++) {
			if (!isset(fifo_bitmap, i))
				continue;
			wlc_low_txq_sync(wlc, qi, i, flag);
		}
	} else {
		bool pending = FALSE;
		uint8 hold_bitmap[TX_FIFO_BITMAP_SIZE_MAX] = { 0 };
		uint8 bitmap[TX_FIFO_BITMAP_SIZE_MAX] = { 0 };

		ASSERT(WLC_HW_NFIFO_INUSE(wlc)/NBBY < sizeof(bitmap));

		/* Handle over fifo_bitmap to bitmap because
		 * wlc_bmac_tx_fifo_sync makes change of the bit map
		 */
		for (i = 0; i < WLC_HW_NFIFO_INUSE(wlc); i++) {
			if (!isset(fifo_bitmap, i))
				continue;

			txq_swq = TXQ_SWQ(txq, i);

			/* Save hw hold flag and set if no originally */
			if (__txq_swq_hw_hold(txq_swq))
				setbit(hold_bitmap, i);
			else
				__txq_swq_hw_hold_set(txq_swq);

			/* Set on bitmap if any packet pending */
			if (wlc_tx_pktpend_check(wlc, i)) {
				setbit(bitmap, i);
				pending = TRUE;
			}
		}

		if (pending) {
			wlc->txfifo_detach_transition_queue = qi;
			wlc->txfifo_detach_pending = TRUE;
#ifdef WLC_MACDBG_FRAMEID_TRACE
			wlc_macdbg_frameid_trace_sync_start(wlc->macdbg);
#endif
#ifdef WLCFP
			/* Update cfp state for all SCBs :txfifo_detach_pending */
			wlc_cfp_state_update(wlc->cfp);
#endif /* WLCFP */
			wlc_bmac_tx_fifo_sync(wlc->hw, bitmap, flag);
		}

		/* Restore to the original hold status  */
		for (i = 0; i < WLC_HW_NFIFO_INUSE(wlc); i++) {
			if (!isset(fifo_bitmap, i))
				continue;

			/* Restore hw hold flag if not set originally */
			if (!isset(hold_bitmap, i)) {
				__txq_swq_hw_hold_clr(TXQ_SWQ(txq, i));
#if defined(HWA_PKTPGR_BUILD)
				wlc_hwa_tx_fifo_hpq_hw_fill(wlc, txq, i);
#endif
			}
		}
	}

	/* Check the opportunity to open block_datafifo */
	wlc_tx_open_datafifo(wlc);
}

/**
 * Sync all active FIFO/low TXQ.
 *
 * @param wlc           wlc_info_t pointer
 * @param qi		wlc_txq_info_t pointer
 * @param fifo		Fifo index
 * @param flag		Flag to pass to wlc_bmac_tx_fifo_sync
 */
void
wlc_sync_txfifo_all(wlc_info_t *wlc, wlc_txq_info_t *qi, uint8 flag)
{
	uint8 fifo_bitmap[TX_FIFO_BITMAP_SIZE_MAX];

	memset(fifo_bitmap, 0xff, sizeof(fifo_bitmap));
	wlc_sync_txfifos(wlc, qi, fifo_bitmap, flag);
}

bool BCMFASTPATH
wlc_txh_get_isAMPDU(wlc_info_t* wlc, const wlc_txh_info_t* txh_info)
{
	bool isAMPDU = FALSE;
	uint16 mcl;
	ASSERT(wlc);
	ASSERT(txh_info);

	mcl = ltoh16(txh_info->MacTxControlLow);
	if (D11REV_GE(wlc->pub->corerev, 40)) {
		isAMPDU = ((mcl & D11AC_TXC_AMPDU) != 0);
	} else {
		isAMPDU = ((mcl & TXC_AMPDU_MASK) != TXC_AMPDU_NONE);
	}

	return isAMPDU;
}

/**
 * Set the frame type value in the given packet's TSO header section.
 * This function is only for AQM hardware (d11 core rev >= 128) headers.
 */
void BCMFASTPATH
wlc_txh_set_ft(wlc_info_t* wlc, uint8* pdata, uint16 fc_ft)
{
	uint8* tsoHdrPtr = pdata;

	ASSERT(D11REV_GE(wlc->pub->corerev, 128));
	ASSERT(pdata != NULL);

	tsoHdrPtr[1] &= ~TOE_F1_FT_MASK;
	tsoHdrPtr[1] |= ((GE128_FC_2_TXH_FT(fc_ft) << TOE_F1_FT_SHIFT) & TOE_F1_FT_MASK);
}

/**
 * Set the frame type value in the given packet's TSO header section.
 * This function is only for AQM hardware (d11 core rev >= 80) headers.
 */
void BCMFASTPATH
wlc_txh_set_frag_allow(wlc_info_t* wlc, uint8* pdata, bool frag_allow)
{
	uint8* tsoHdrPtr = pdata;

	ASSERT(D11REV_GE(wlc->pub->corerev, 128));
	ASSERT(pdata != NULL);

	if (frag_allow)
		tsoHdrPtr[1] |= TOE_F1_FRAG_ALLOW;
	else
		tsoHdrPtr[1] &= ~TOE_F1_FRAG_ALLOW;
}

/**
 * Set the epoch value in the given packet data.
 * This function is only for AQM hardware (d11 core rev >= 40) headers.
 *
 * @param wlc           wlc_info_t pointer
 * @param pdata	        pointer to the beginning of the packet data which
 *                      should be the TxD hardware header.
 * @param epoch         the epoch value (0/1) to set
 */
void BCMFASTPATH
wlc_txh_set_epoch(wlc_info_t* wlc, uint8* pdata, uint8 epoch)
{
	uint8* tsoHdrPtr = pdata;

	ASSERT(D11REV_GE(wlc->pub->corerev, 40));
	ASSERT(pdata != NULL);

	if (epoch) {
		tsoHdrPtr[2] |= TOE_F2_EPOCH;
	} else {
		tsoHdrPtr[2] &= ~TOE_F2_EPOCH;
	}

	/* JIRA:CRWLDOT11M-978:
	 * PR105383 WAR: add soft-epoch in tx-descriptor to work-around
	 * the problem of aggregating across epoch.
	 */
	if (D11REV_LT(wlc->pub->corerev, 42)) {
		uint tsoHdrSize;
		d11actxh_t* vhtHdr;

		tsoHdrSize = ((tsoHdrPtr[0] & TOE_F0_HDRSIZ_NORMAL) ?
		              TSO_HEADER_LENGTH : TSO_HEADER_PASSTHROUGH_LENGTH);

		vhtHdr = (d11actxh_t*)(tsoHdrPtr + tsoHdrSize);

		vhtHdr->PktInfo.TSOInfo = htol16(tsoHdrPtr[2] << 8);
	}
}

/**
 * Get the epoch value from the given packet data.
 * This function is only for AQM hardware (d11 core rev >= 40) headers.
 *
 * @param wlc           wlc_info_t pointer
 * @param pdata	        pointer to the beginning of the packet data which
 *                      should be the TxD hardware header.
 * @return              The fucntion returns the epoch value (0/1) from the frame
 */
uint8
wlc_txh_get_epoch(wlc_info_t* wlc, wlc_txh_info_t* txh_info)
{
	uint8* tsoHdrPtr = txh_info->tsoHdrPtr;

	ASSERT(D11REV_GE(wlc->pub->corerev, 40));
	ASSERT(txh_info->tsoHdrPtr != NULL);

	return ((tsoHdrPtr[2] & TOE_F2_EPOCH)? 1 : 0);
}

/**
* wlc_compute_ampdu_mpdu_dur()
 *
 * Calculate the 802.11 MAC header DUR field for MPDU in an A-AMPDU DUR for MPDU = 1 SIFS + 1 BA
 *
 * rate			MPDU rate in unit of 500kbps
 */
static uint16
wlc_compute_ampdu_mpdu_dur(wlc_info_t *wlc, enum wlc_bandunit bandunit, ratespec_t rate)
{
	uint16 dur = BANDUNIT_SIFS(bandunit);

	dur += (uint16)wlc_calc_ba_time(wlc->hti, rate, WLC_SHORT_PREAMBLE);

	return (dur);
}

#ifdef STA
static void
wlc_pm_tx_upd(wlc_info_t *wlc, struct scb *scb, void *pkt, bool ps0ok, uint fifo)
{
	wlc_bsscfg_t *cfg;
	wlc_pm_st_t *pm;

	WL_RTDC(wlc, "wlc_pm_tx_upd", 0, 0);

	ASSERT(scb != NULL);

	cfg = SCB_BSSCFG(scb);
	ASSERT(cfg != NULL);

	pm = cfg->pm;

	if (!pm) {
		return;
	}

	WL_NONE(("%s: wme=%d, qos=%d, pm_ena=%d, pm_pend=%d\n", __FUNCTION__,
		WME_ENAB(wlc->pub), SCB_QOS(scb), pm->PMenabled, pm->PMpending));

	if (!pm->PMenabled ||
#ifdef WLTDLS
	    (BSS_TDLS_ENAB(wlc, cfg) && !wlc_tdls_pm_enabled(wlc->tdls, cfg)) ||
#endif
	    (BSSCFG_IBSS(cfg) && !BSS_TDLS_ENAB(wlc, cfg)))
		return;

	if (BSSCFG_AP(cfg)) {
		WL_PS(("%s: PMEnabled on AP cfg %d\n", __FUNCTION__, WLC_BSSCFG_IDX(cfg)));
		ASSERT(0);
		return;
	}

	/* Turn off PM mode */
	/* Do not leave PS mode on a tx if the Receive Throttle
	 * feature is  enabled. Leaving PS mode during the OFF
	 * part of the receive throttle duty cycle
	 * for any reason will defeat the whole purpose
	 * of the OFF part of the duty cycle - heat reduction.
	 */
	if (!PM2_RCV_DUR_ENAB(cfg)) {
		/* Leave PS mode if in fast PM mode */
		if (pm->PM == PM_FAST &&
		    ps0ok &&
		    !pm->PMblocked) {
			/* XXX JQL: we need to wait until PM is indicated to the BSS
			 * (PMpending is FALSE) when turning on PM mode but in case
			 * transmitting another back to back frame which is a trigger to
			 * turn off the PM mode we use a boolean pm2_ps0_allowed to
			 * control if turning off PM mode is allowed when turning on PM mode
			 * is in progress...
			 */
			if (!pm->PMpending || pm->pm2_ps0_allowed) {
				WL_RTDC(wlc, "wlc_pm_tx_upd: exit PS", 0, 0);
				wlc_set_pmstate(cfg, FALSE);
#ifdef WL_PWRSTATS
				if (PWRSTATS_ENAB(wlc->pub)) {
					wlc_pwrstats_set_frts_data(wlc->pwrstats, TRUE);
				}
#endif /* WL_PWRSTATS */
				wlc_pm2_sleep_ret_timer_start(cfg, 0);
			} else if (pm->PMpending) {
				pm->pm2_ps0_allowed = TRUE;
			}
		}
	}

#ifdef WME
	/* Start an APSD USP */
	ASSERT(WLPKTTAG(pkt)->flags & WLF_TXHDR);

	/* If sending APSD trigger frame, stay awake until EOSP */
	if (WME_ENAB(wlc->pub) &&
	    SCB_QOS(scb) &&
	    /* APSD trigger is only meaningful in PS mode */
	    pm->PMenabled &&
	    /* APSD trigger must not be any PM transitional frame */
	    !pm->PMpending) {
		struct dot11_header *h;
		wlc_txh_info_t txh_info;
		uint16 kind;
		bool qos;
#ifdef WLTDLS
		uint16 qosctrl;
#endif

		wlc_get_txh_info(wlc, pkt, &txh_info);
		h = txh_info.d11HdrPtr;
		kind = (ltoh16(h->fc) & FC_KIND_MASK);
		qos = (kind  == FC_QOS_DATA || kind == FC_QOS_NULL);

#ifdef WLTDLS
		qosctrl = qos ? ltoh16(*(uint16 *)((uint8 *)h + DOT11_A3_HDR_LEN)) : 0;
#endif
		WL_NONE(("%s: qos=%d, fifo=%d, trig=%d, sta_usp=%d\n", __FUNCTION__,
			qos, fifo, AC_BITMAP_TST(scb->apsd.ac_trig,
			WME_PRIO2AC(PKTPRIO(pkt))), pm->apsd_sta_usp));

		if (qos &&
		    fifo != TX_BCMC_FIFO &&
		    AC_BITMAP_TST(scb->apsd.ac_trig, WME_PRIO2AC(PKTPRIO(pkt))) &&
#ifdef WLTDLS
			(!BSS_TDLS_ENAB(wlc, cfg) || kind != FC_QOS_NULL ||
			!QOS_EOSP(qosctrl)) &&
#endif
			TRUE) {
#ifdef WLTDLS
			if (BSS_TDLS_ENAB(wlc, cfg))
				wlc_tdls_apsd_upd(wlc->tdls, cfg);
#endif
			if (!pm->apsd_sta_usp) {
				WL_PS(("wl%d.%d: APSD wake\n",
				       wlc->pub->unit, WLC_BSSCFG_IDX(cfg)));
				wlc_set_apsd_stausp(cfg, TRUE);
#ifdef WLP2P
				/* APSD trigger frame is being sent, and re-trigger frame
				 * is expected when the next presence period starts unless
				 * U-APSD EOSP is received between now and then.
				 */
				if (BSS_P2P_ENAB(wlc, cfg))
					wlc_p2p_apsd_retrigger_upd(wlc->p2p, cfg, TRUE);
#endif
			}
			scb->flags |= SCB_SENT_APSD_TRIG;
		}
	}
#endif /* WME */
} /* wlc_pm_tx_upd */

/**
 * Given an ethernet packet, prepare the packet as if for transmission. The driver needs to program
 * the frame in the template and also provide d11txh equivalent information to it.
 */
void *
wlc_sdu_to_pdu(wlc_info_t *wlc, void *sdu, struct scb *scb, bool is_8021x, ratespec_t rspec)
{
	void *pkt;
	wlc_key_t *key = NULL;
	wlc_key_info_t key_info;
	uint8 prio = 0;
	uint frag_length = 0;
	uint pkt_length, nfrags;
	wlc_bsscfg_t *bsscfg;

	ASSERT(scb != NULL);

	bsscfg = SCB_BSSCFG(scb);
	ASSERT(bsscfg != NULL);
	memset(&key_info, 0, sizeof(wlc_key_info_t));

	/* wlc_hdr_proc --> Ether to 802.3 */
	pkt = wlc_hdr_proc(wlc, sdu, scb);

	if (pkt == NULL) {
		PKTFREE(wlc->osh, sdu, TRUE);
		return NULL;
	}

	ASSERT(sdu == pkt);

	prio = 0;
	if (SCB_QOS(scb)) {
		prio = (uint8)PKTPRIO(sdu);
		ASSERT(prio <= MAXPRIO);
	}

	if (WSEC_ENABLED(bsscfg->wsec)) {
		/* Use a paired key or primary group key if present */
		key = wlc_keymgmt_get_tx_key(wlc->keymgmt, scb, bsscfg, &key_info);
		if (key_info.algo != CRYPTO_ALGO_OFF) {
			WL_WSEC(("wl%d.%d: %s: using pairwise key\n",
				WLCWLUNIT(wlc), WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
		} else if (is_8021x && WSEC_SES_OW_ENABLED(bsscfg->wsec)) {
			WL_WSEC(("wl%d.%d: wlc_sdu_to_pdu: not encrypting 802.1x frame "
					"during OW\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		} else {
			key = wlc_keymgmt_get_bss_tx_key(wlc->keymgmt, bsscfg, FALSE, &key_info);
			if (key_info.algo != CRYPTO_ALGO_OFF) {
				WL_WSEC(("wl%d.%d: %s: using group key",
					WLCWLUNIT(wlc), WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
			} else {
				WL_ERROR(("wl%d.%d: %s: no key for encryption\n",
					WLCWLUNIT(wlc), WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
			}
		}
	} else {
		key = wlc_keymgmt_get_bss_key(wlc->keymgmt, bsscfg, WLC_KEY_ID_INVALID, &key_info);
		ASSERT(key != NULL);
	}

	pkt_length = pkttotlen(wlc->osh, pkt);

	/* TKIP MIC space reservation */
	if (key_info.algo == CRYPTO_ALGO_TKIP) {
		pkt_length += TKIP_MIC_SIZE;
		PKTSETLEN(wlc->osh, pkt, pkt_length);
	}

	nfrags = wlc_frag(wlc, scb, AC_BE, pkt_length, &frag_length);
	BCM_REFERENCE(nfrags);
	ASSERT(nfrags == 1);

	/* This header should not be installed to or taken from Header Cache */
	WLPKTTAG(pkt)->flags |= WLF_BYPASS_TXC;

	/* wlc_dofrag --> Get the d11hdr put on it with TKIP MIC at the tail for TKIP */
	wlc_dofrag(wlc, pkt, 0, 1, 0, scb, is_8021x, TX_AC_BE_FIFO, key,
		&key_info, prio, frag_length, rspec);

	return pkt;
} /* wlc_sdu_to_pdu */
#endif /* STA */

/** allocates e.g. initial and excursion transmit queues */
wlc_txq_info_t*
wlc_txq_alloc(wlc_info_t *wlc, osl_t *osh)
{
	wlc_txq_info_t *qi, *p;
	int i;
	qi = (wlc_txq_info_t*)MALLOCZ(osh, sizeof(wlc_txq_info_t));
	if (qi == NULL) {
		return NULL;
	}

	/* Set the overall queue packet limit to the max, just rely on per-prec limits */
	pktq_init(WLC_GET_CQ(qi), WLC_PREC_COUNT, PKTQ_LEN_MAX);

	/* Have enough room for control packets along with HI watermark */
	/* Also, add room to txq for total psq packets if all the SCBs leave PS mode */
	/* The watermark for flowcontrol to OS packets will remain the same */
	for (i = 0; i < WLC_PREC_COUNT; i++) {
		pktq_set_max_plen(WLC_GET_CQ(qi), i,
			(2 * wlc->pub->tunables->datahiwat) + PKTQ_LEN_DEFAULT +
			wlc->pub->psq_pkts_total);
	}

#if defined(WLCFP)
	if (CFP_ENAB(wlc->pub) == TRUE) {
		(WLC_GET_CQ(qi))->common_queue = TRUE;
	} else
#endif /* WLCFP */
	{
		(WLC_GET_CQ(qi))->common_queue = FALSE;
	}

	/* add this queue to the the global list */
	p = wlc->tx_queues;
	if (p == NULL) {
		wlc->tx_queues = qi;
	} else {
		while (p->next != NULL)
			p = p->next;
		p->next = qi;
	}

	WL_MQ(("MQ: %s: qi %p\n", __FUNCTION__, OSL_OBFUSCATE_BUF(qi)));

	/*
	 * Allocate the low txq to accompany this queue context
	 * Allocated for all physical queues in the device
	 */
	qi->low_txq = wlc_low_txq_alloc(wlc->txqi, qi, WLC_HW_NFIFO_TOTAL(wlc));
	if (qi->low_txq == NULL) {
		WL_ERROR(("wl%d: %s: wlc_low_txq_alloc failed\n", wlc->pub->unit, __FUNCTION__));
		wlc_txq_free(wlc, osh, qi);
		return NULL;
	}

	WL_MQ(("MQ: %s: qi %p\n", __FUNCTION__, OSL_OBFUSCATE_BUF(qi)));

	return qi;
} /* wlc_txq_alloc */

void
wlc_txq_free(wlc_info_t *wlc, osl_t *osh, wlc_txq_info_t *qi)
{
	wlc_txq_info_t *p;

	if (qi == NULL)
		return;

	WL_MQ(("MQ: %s: qi %p\n", __FUNCTION__, OSL_OBFUSCATE_BUF(qi)));

	/* remove the queue from the linked list */
	p = wlc->tx_queues;
	if (p == qi)
		wlc->tx_queues = p->next;
	else {
		while (p != NULL && p->next != qi)
			p = p->next;
		if (p == NULL)
			WL_ERROR(("%s: null ptr2", __FUNCTION__));

		/* assert that we found qi before getting to the end of the list */
		ASSERT(p != NULL);

		if (p != NULL) {
			p->next = p->next->next;
		}
	}

	/* Free the low_txq accompanying this txq context */
	wlc_low_txq_free(wlc->txqi, qi->low_txq);

#ifdef PKTQ_LOG
	wlc_pktq_stats_free(wlc, WLC_GET_CQ(qi));
#endif

	ASSERT(pktq_empty(WLC_GET_CQ(qi)));
	MFREE(osh, qi, sizeof(wlc_txq_info_t));
} /* wlc_txq_free */

static void
wlc_txflowcontrol_signal(wlc_info_t *wlc, wlc_txq_info_t *qi, bool on, int prio)
{
	wlc_if_t *wlcif;
	uint curr_qi_stopped = qi->stopped;

	for (wlcif = wlc->wlcif_list; wlcif != NULL; wlcif = wlcif->next) {
		if (curr_qi_stopped != qi->stopped) {
			/* This tells us that while performing wl_txflowcontrol(),
			 * the qi->stopped state changed.
			 * This can happen when turning wl flowcontrol OFF since in
			 * the process of draining packets from wl layer, flow control
			 * can get turned back on.
			 */
			WL_ERROR(("wl%d: qi(%p) stopped changed from 0x%x to 0x%x, exit %s\n",
			          wlc->pub->unit, OSL_OBFUSCATE_BUF(qi), curr_qi_stopped,
				qi->stopped, __FUNCTION__));
			break;
		}
		if (wlcif->qi == qi &&
		    wlcif->if_flags & WLC_IF_LINKED)
			wl_txflowcontrol(wlc->wl, wlcif->wlif, on, prio);
	}
}

void
wlc_txflowcontrol_reset(wlc_info_t *wlc)
{
	wlc_txq_info_t *qi;

	for (qi = wlc->tx_queues; qi != NULL; qi = qi->next) {
		wlc_txflowcontrol_reset_qi(wlc, qi);
	}
}

/** check for the particular priority flow control bit being set */
bool
wlc_txflowcontrol_prio_isset(wlc_info_t *wlc, wlc_txq_info_t *q, int prio)
{
	uint prio_mask;
	BCM_REFERENCE(wlc);

	if (prio == ALLPRIO) {
		prio_mask = TXQ_STOP_FOR_PRIOFC_MASK;
	} else {
		ASSERT(prio >= 0 && prio <= MAXPRIO);
		prio_mask = NBITVAL(prio);
	}

	return (q->stopped & prio_mask) == prio_mask;
}

/** check if a particular override is set for a queue */
bool
wlc_txflowcontrol_override_isset(wlc_info_t *wlc, wlc_txq_info_t *qi, uint override)
{
	BCM_REFERENCE(wlc);
	ASSERT(qi != NULL);
	/* override should have some bit on */
	ASSERT(override != 0);
	/* override should not have a prio bit on */
	ASSERT((override & TXQ_STOP_FOR_PRIOFC_MASK) == 0);

	return ((qi->stopped & override) != 0);
}

/** propagate the flow control to all interfaces using the given tx queue */
void
wlc_txflowcontrol(wlc_info_t *wlc, wlc_txq_info_t *qi, bool on, int prio)
{
	uint prio_bits;
	uint cur_bits;

	if (prio == ALLPRIO) {
		prio_bits = TXQ_STOP_FOR_PRIOFC_MASK;
	} else {
		ASSERT(prio >= 0 && prio <= MAXPRIO);
		prio_bits = NBITVAL(prio);
	}

	cur_bits = qi->stopped & prio_bits;

	/* Check for the case of no change and return early
	 * Otherwise update the bit and continue
	 */
	if (on) {
		if (cur_bits == prio_bits) {
			return;
		}
		mboolset(qi->stopped, prio_bits);
	} else {
		if (cur_bits == 0) {
			return;
		}
		mboolclr(qi->stopped, prio_bits);
	}

	/* If there is a flow control override we will not change the external
	 * flow control state.
	 */
	if (qi->stopped & ~TXQ_STOP_FOR_PRIOFC_MASK) {
		return;
	}

	wlc_txflowcontrol_signal(wlc, qi, on, prio);
} /* wlc_txflowcontrol */

/** called in e.g. association and AMPDU connection setup scenario's */
void
wlc_txflowcontrol_override(wlc_info_t *wlc, wlc_txq_info_t *qi, bool on, uint override)
{
	uint prev_override;

	ASSERT(override != 0);
	ASSERT((override & TXQ_STOP_FOR_PRIOFC_MASK) == 0);
	ASSERT(qi != NULL);

	prev_override = (qi->stopped & ~TXQ_STOP_FOR_PRIOFC_MASK);

	/* Update the flow control bits and do an early return if there is
	 * no change in the external flow control state.
	 */
	if (on) {
		mboolset(qi->stopped, override);
		/* if there was a previous override bit on, then setting this
		 * makes no difference.
		 */
		if (prev_override) {
			return;
		}

		wlc_txflowcontrol_signal(wlc, qi, ON, ALLPRIO);
	} else {
		mboolclr(qi->stopped, override);
		/* clearing an override bit will only make a difference for
		 * flow control if it was the only bit set. For any other
		 * override setting, just return
		 */
		if ((prev_override != 0) && (prev_override != override)) {
			return;
		}

		if (qi->stopped == 0) {
			wlc_txflowcontrol_signal(wlc, qi, OFF, ALLPRIO);
		} else {
			int prio;

			for (prio = MAXPRIO; prio >= 0; prio--) {
				if (!mboolisset(qi->stopped, NBITVAL(prio)))
					wlc_txflowcontrol_signal(wlc, qi, OFF, prio);
			}
		}
	}
} /* wlc_txflowcontrol_override */

void
wlc_txflowcontrol_reset_qi(wlc_info_t *wlc, wlc_txq_info_t *qi)
{
	ASSERT(qi != NULL);

	if (qi->stopped) {
		wlc_txflowcontrol_signal(wlc, qi, OFF, ALLPRIO);
		qi->stopped = 0;
	}
}

/** enqueue SDU on a remote party ('scb') specific queue */
int BCMFASTPATH
wlc_txq_enq_pkt(wlc_info_t *wlc, struct scb *scb, void *sdu, uint prec)
{
	wlc_txq_info_t *qi = SCB_WLCIFP(scb)->qi;

	ASSERT(!PKTISCHAINED(sdu));
#ifdef WLCFP
	ASSERT(!PKTISCFP(sdu));
#endif /* WLCFP */

#ifdef BCMDBG_POOL
	if (WLPKTTAG(sdu)->flags & WLF_PHDR) {
		void *pdata;

		pdata = PKTNEXT(wlc->pub.osh, sdu);
		ASSERT(pdata);
		ASSERT(WLPKTTAG(pdata)->flags & WLF_DATA);
		PKTPOOLSETSTATE(pdata, POOL_TXENQ);
	} else {
		PKTPOOLSETSTATE(sdu, POOL_TXENQ);
	}
#endif

	if (!cpktq_prec_enq(wlc, &qi->cpktq, sdu, prec, FALSE)) {
		if (!WME_ENAB(wlc->pub) || (wlc->pub->wlfeatureflag & WL_SWFL_FLOWCONTROL))
			WL_ERROR(("wl%d: %s: txq overflow\n", wlc->pub->unit, __FUNCTION__));
		else
			WL_INFORM(("wl%d: %s: txq overflow\n", wlc->pub->unit, __FUNCTION__));

		PKTDBG_TRACE(wlc->osh, sdu, PKTLIST_FAIL_PRECQ);
		WLCNTINCR(wlc->pub->_cnt->txnobuf);
		WLCIFCNTINCR(scb, txnobuf);
		WLCNTSCB_COND_INCR(scb, scb->scb_stats.tx_failures);
#ifdef WL11K
		wlc_rrm_stat_qos_counter(wlc, scb, PKTPRIO(sdu),
			OFFSETOF(rrm_stat_group_qos_t, txfail));
#endif
		PKTFREE(wlc->osh, sdu, TRUE);
		return BCME_TXFAIL;
	}
	return BCME_OK;
} /* wlc_txq_enq_pkt */

void BCMFASTPATH
wlc_txq_enq_flowcontrol_ampdu(wlc_info_t *wlc, struct scb *scb, int prio)
{
	wlc_txq_info_t *qi = SCB_WLCIFP(scb)->qi;
	struct pktq *common_q = WLC_GET_CQ(qi);
	int datahiwat;

	datahiwat = wlc->pub->tunables->ampdudatahiwat;

	ASSERT(pktq_max(common_q) >= datahiwat);
	/* Check if flow control needs to be turned on after enqueuing the packet
	 *   Don't turn on flow control if EDCF is enabled. Driver would make the decision on what
	 *   to drop instead of relying on stack to make the right decision
	 */
	if (!WME_ENAB(wlc->pub) || (wlc->pub->wlfeatureflag & WL_SWFL_FLOWCONTROL)) {
		if (pktq_n_pkts_tot(common_q) >= datahiwat) {
			wlc_txflowcontrol(wlc, qi, ON, ALLPRIO);
		}
	} else if (wlc->pub->_priofc) {
		if (pktqprec_n_pkts(common_q, wlc_prio2prec_map[prio]) >= datahiwat) {
			wlc_txflowcontrol(wlc, qi, ON, prio);
		}
	}
}

/** enqueue SDU on a remote party ('scb') specific queue */
static void BCMFASTPATH
wlc_txq_enq(void *ctx, struct scb *scb, void *sdu, uint prec)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	wlc_txq_info_t *qi = SCB_WLCIFP(scb)->qi;
	struct pktq *common_q = WLC_GET_CQ(qi);		/**< Multi-priority packet queue */
	int prio;
	int datahiwat;
#if defined(PKTC) || defined(PKTC_TX_DONGLE)
	void *nxt = NULL;

	FOREACH_CHAINED_PKT(sdu, nxt) {
		PKTCLRCHAINED(wlc->osh, sdu);
		if (nxt != NULL) {
			wlc_pktc_sdu_prep(wlc, scb, sdu, nxt);
		}
#else /* !(PKTC || PKTC_TX_DONGLE) */
	{
		ASSERT(!PKTISCHAINED(sdu));
#endif /* !(PKTC || PKTC_TX_DONGLE) */
		if (wlc_txq_enq_pkt(wlc, scb, sdu, prec) == BCME_OK) {
			/* Check if flow control needs to be turned on after enqueuing
			 * the packet Don't turn on flow control if EDCF is enabled.
			 * Driver would make the decision on what to drop instead of
			 * relying on stack to make the right decision
			 */
			datahiwat = (WLPKTTAG(sdu)->flags & WLF_AMPDU_MPDU)
				  ? wlc->pub->tunables->ampdudatahiwat
				  : wlc->pub->tunables->datahiwat;
			if (!WME_ENAB(wlc->pub) ||
					(wlc->pub->wlfeatureflag & WL_SWFL_FLOWCONTROL)) {
				if (pktq_n_pkts_tot(common_q) >= datahiwat) {
					wlc_txflowcontrol(wlc, qi, ON, ALLPRIO);
				}
			} else if (wlc->pub->_priofc) {
				prio = PKTPRIO(sdu);
				if (pktqprec_n_pkts(common_q,
					wlc_prio2prec_map[prio]) >= datahiwat) {
					wlc_txflowcontrol(wlc, qi, ON, prio);
				}
			}
		} else {
			/* sdu has been freed by wlc_txq_enq_pkt() */
			sdu = NULL;
		}
	}
} /* wlc_txq_enq */

/** returns number of packets on the *active* transmit queue */
static uint BCMFASTPATH
wlc_txq_txpktcnt(void *ctx)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	wlc_txq_info_t *qi = wlc->active_queue;

	return (uint)pktq_n_pkts_tot(WLC_GET_CQ(qi));
}

/**
 * Get the number of fifos configured for the specified txq
 * @param txq    E.g. the low queue, consisting of a queue for every d11 fifo
 */
uint
wlc_txq_nfifo(txq_t *txq)
{
	return (txq->nfifo);
}

#ifdef PKTQ_STATUS
/** pktq_status related. Get the value of the per scb txpkts in low TxQ */
int
wlc_get_lowtxq_pktcount(wlc_info_t *wlc, struct scb *scb, struct bcmstrbuf *b)
{
	wlc_txq_info_t *qi;
	txq_t *txq;	/**< E.g. the low queue, consisting of a queue for every d11 fifo */
	txq_swq_t *txq_swq; /**< a non-priority queue backing one d11 FIFO */
	uint8 i;
	struct pktq_prec *q = NULL;

	for (qi = wlc->tx_queues; qi != NULL; qi = qi->next) {
		txq = qi->low_txq;
		if (txq && txq->swq_table) {
			for (i = 0, txq_swq = txq->swq_table; i < txq->nfifo; i++, txq_swq++) {
				q = &txq_swq->spktq.q;
				if (q->head) {
					if (WLPKTTAGSCBGET(q->head) == scb) {
						bcm_bprintf(b, "\tLowTxQ[%d]:\t %d\n",
						            i, q->n_pkts);
					}
				}
			}
		}
	}
	return BCME_OK;
}

#ifdef BULK_PKTLIST
/** pktq_status related. Get the value of the per scb txpkts in Hw/Tx Fifos */
int
wlc_hwfifo_pktcount(wlc_info_t *wlc, struct scb *scb, struct bcmstrbuf *b)
{
	uint8 i;
	hnddma_t *di;
	void *pkt = NULL;
	uint16 pktCount;

	for (i = 0; i < WLC_HW_NFIFO_TOTAL(wlc); i++) {
		di = WLC_HW_DI(wlc, i);
		if (di) {
			pktCount = 0;
			while ((pkt = dma_get_nextpkt(di, pkt)))
			{
				if (WLPKTTAGSCBGET(pkt) == scb) {
					pktCount++;
				}
			}
			if (pktCount) {
				bcm_bprintf(b, "\tFifo[%d]\t%d\n", i, pktCount);
			}
		}
	}
	return BCME_OK;
}
#endif /* BULK_PKTLIST */
#endif /* PKTQ_STATUS */
/** Flow control related. Get the value of the txmaxpkts */
uint16
wlc_get_txmaxpkts(wlc_info_t *wlc)
{
	return wlc->pub->txmaxpkts;
}

/** Flow control related. Set the value of the txmaxpkts */
void
wlc_set_txmaxpkts(wlc_info_t *wlc, uint16 txmaxpkts)
{
	wlc->pub->txmaxpkts = txmaxpkts;
}

/** Flow control related. Reset txmaxpkts to their defaults */
void
wlc_set_default_txmaxpkts(wlc_info_t *wlc)
{
	uint16 txmaxpkts = MAXTXPKTS;

	if (AMPDU_AQM_ENAB(wlc->pub)) {
		txmaxpkts = MAXTXPKTS_AMPDUAQM_DFT;
	} else if (AMPDU_MAC_ENAB(wlc->pub)) {
		txmaxpkts = MAXTXPKTS_AMPDUMAC;
	}

	wlc_set_txmaxpkts(wlc, txmaxpkts);
}

/**
 * This function changes the phytxctl for beacon based on current beacon ratespec AND txant
 * setting as per this table:
 * ratespec     CCK		ant = wlc->stf->txant
 * OFDM		ant = 3
 */
void
wlc_beacon_phytxctl_txant_upd(wlc_info_t *wlc, ratespec_t bcn_rspec)
{
	uint16 phytxant = wlc->stf->phytxant;
	uint16 mask = PHY_TXC_ANT_MASK;
	uint shm_bcn_phy_ctlwd_txant;

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		shm_bcn_phy_ctlwd_txant = M_BCN_TXPCTL2(wlc);
	} else if (D11REV_GE(wlc->pub->corerev, 40))
		/* XXX 4360: 11AC need this to be updated, call wlc_acphy_txctl0_calc
		 * instead for AC chips...
		 */
		shm_bcn_phy_ctlwd_txant = M_BCN_TXPCTL0(wlc);
	else
		shm_bcn_phy_ctlwd_txant = M_BCN_PCTLWD(wlc);

	if (WLCISACPHY(wlc->band) || WLCISHECAPPHY(wlc->band)) {
		mask = PHY_TXC_HTANT_MASK;
		if (bcn_rspec == 0)
			bcn_rspec = wlc->bcn_rspec;
	}
	phytxant = wlc_stf_phytxchain_sel(wlc, bcn_rspec);

	if (WLC_PHY_AS_80P80(wlc, wlc->chanspec)) {
		phytxant = wlc_stf_d11hdrs_phyctl_txcore_80p80phy(wlc, phytxant);
	}

	if (D11REV_GE(wlc->pub->corerev, 128)) {
		phytxant = wlc_stf_d11hdrs_phyctl_txant(wlc, bcn_rspec);
		mask = D11_REV80_PHY_TXC_ANT_CORE_MASK;
	}

	WL_NONE(("wl%d: wlc_beacon_phytxctl_txant_upd: beacon txant 0x%04x mask 0x%04x\n",
		wlc->pub->unit, phytxant, mask));
	wlc_update_shm(wlc, shm_bcn_phy_ctlwd_txant, phytxant, mask);
}

/**
 *   This function updates beacon duration
 */
void
wlc_beacon_upddur(wlc_info_t *wlc, ratespec_t bcn_rspec, uint16 len)
{
	uint8 bcn_pre;
	uint16 txbcn_dur;

	if ((!wlc->pub->up) || D11REV_LT(wlc->pub->corerev, 40)) {
		return;
	}

	ASSERT(len);
	wlc->bcn_len = len;
	bcn_pre = (wlc_read_shm(wlc, M_BCN_TXPCTL0(wlc)) & D11AC_PHY_TXC_SHORT_PREAMBLE) ?
		WLC_SHORT_PREAMBLE : WLC_LONG_PREAMBLE;
	txbcn_dur = (uint16)wlc_calc_frame_time(wlc, bcn_rspec, bcn_pre, len);
	wlc_write_shm(wlc, M_TXBCN_DUR(wlc), txbcn_dur);
}

/**
 * This function doesn't change beacon body(plcp, mac hdr). It only updates the
 *   phyctl0 and phyctl1 with the exception of tx antenna,
 *   which is handled in wlc_stf_phy_txant_upd() and wlc_beacon_phytxctl_txant_upd()
 */
void
wlc_beacon_phytxctl(wlc_info_t *wlc, ratespec_t bcn_rspec, chanspec_t chanspec)
{
	uint16 phyctl;
	uint rate, bw_idx = BW20_IDX;
	wlcband_t *band;
	txpwr204080_t txpwrs;
	uint8 tot_offset = 0;
	if (D11REV_GE(wlc->pub->corerev, 128)) {
		phyctl = wlc_axphy_fixed_txctl_calc_word_0(wlc,
			bcn_rspec, WLC_LONG_PREAMBLE);
		wlc_write_shm(wlc, M_BCN_TXPCTL0(wlc), phyctl);

		wlc_write_shm(wlc, M_RSP_TXPCTL0(wlc), phyctl);

		if (CHSPEC_IS6G(wlc->chanspec) &&
			((wlc->lpi_mode == AUTO && wlc->stf->psd_limit_indicator) ||
			(wlc->lpi_mode == ON))) {
			bw_idx = (RSPEC_BW(bcn_rspec) >> WL_RSPEC_BW_SHIFT) - 1;
		}
		phyctl = wlc_d11_rev128_phy_fixed_txctl_calc_word_1(wlc, NULL, bcn_rspec, FALSE);
		wlc_write_shm(wlc, M_BCN_TXPCTL1(wlc), phyctl);

		wlc_write_shm(wlc, M_RSP_TXPCTL1(wlc), phyctl);

		phyctl = wlc_d11_rev128_phy_fixed_txctl_calc_word_2(wlc, bcn_rspec);
		wlc_write_shm(wlc, M_BCN_TXPCTL2(wlc), phyctl);

		if (wlc_stf_get_204080_pwrs(wlc, bcn_rspec, &txpwrs, 0) != BCME_OK) {
			WL_ERROR(("wl%d:%s phyctl1 ppr returns error!\n",
				wlc->pub->unit, __FUNCTION__));
			ASSERT(0);
		}
		tot_offset = wlc_get_bcnprs_txpwr_offset(wlc, txpwrs.pbw[bw_idx][TXBF_OFF_IDX]);
		phyctl = SCALE_5_1_TO_5_2_FORMAT(tot_offset);
		phyctl |= (phyctl << 8);
		wlc_write_shm(wlc, M_BCN_TXPCTL6(wlc), phyctl);
		return;
	}

	if (D11REV_GE(wlc->pub->corerev, 40)) {
		bcn_rspec &= ~WL_RSPEC_BW_MASK;
		bcn_rspec |= WL_RSPEC_BW_20MHZ;

		phyctl = wlc_acphy_txctl0_calc(wlc, bcn_rspec, WLC_LONG_PREAMBLE);

		if (WLC_PHY_AS_80P80(wlc, wlc->chanspec)) {
			phyctl = wlc_stf_d11hdrs_phyctl_txcore_80p80phy(wlc, phyctl);
		}

		wlc_write_shm(wlc, M_BCN_TXPCTL0(wlc), phyctl);
		if (wlc_stf_get_204080_pwrs(wlc, bcn_rspec, &txpwrs, 0) != BCME_OK) {
			WL_ERROR(("wl%d:%s phyctl1 ppr returns error!\n",
				wlc->pub->unit, __FUNCTION__));
			ASSERT(0);
		}
		tot_offset = wlc_get_bcnprs_txpwr_offset(wlc, txpwrs.pbw[BW20_IDX][TXBF_OFF_IDX]);
		phyctl = wlc_acphy_txctl1_calc(wlc, bcn_rspec, 0,
			tot_offset, FALSE);
		wlc_write_shm(wlc, M_BCN_TXPCTL1(wlc), phyctl);
		phyctl = wlc_acphy_txctl2_calc(wlc, bcn_rspec, 0);
		wlc_write_shm(wlc, M_BCN_TXPCTL2(wlc), phyctl);
		return;
	}

	phyctl = wlc_read_shm(wlc, M_BCN_PCTLWD(wlc));

	phyctl &= ~PHY_TXC_FT_MASK;
	phyctl |= WLC_RSPEC_ENC2FT(bcn_rspec);
	/* ??? If ever beacons use short headers or phy pwr override, add the proper bits here. */

	band = wlc->bandstate[CHSPEC_BANDUNIT(chanspec)];
	if (WLCISNPHY(band) && RSPEC_ISCCK(bcn_rspec)) {
		rate = RSPEC2RATE(bcn_rspec);
		phyctl |= ((rate * 5) << 10);
	}

	wlc_write_shm(wlc, M_BCN_PCTLWD(wlc), phyctl);

	if (D11REV_LT(wlc->pub->corerev, 40)) {
		uint16 phyctl1;

		bcn_rspec &= ~WL_RSPEC_BW_MASK;
		bcn_rspec |= WL_RSPEC_BW_20MHZ;

		phyctl1 = wlc_phytxctl1_calc(wlc, bcn_rspec, chanspec);
		wlc_write_shm(wlc, M_BCN_PCTL1WD(wlc), phyctl1);
	}
} /* wlc_beacon_phytxctl */

#ifdef BCMASSERT_SUPPORT
/** Validate the beacon phytxctl given current band */
static bool
wlc_valid_beacon_phytxctl(wlc_info_t *wlc)
{
	uint16 phyctl;
	uint shm_bcn_phy_ctlwd0;

	if (D11REV_GE(wlc->pub->corerev, 40))
		shm_bcn_phy_ctlwd0 = M_BCN_TXPCTL0(wlc);
	else
		shm_bcn_phy_ctlwd0 = M_BCN_PCTLWD(wlc);

	phyctl = wlc_read_shm(wlc, shm_bcn_phy_ctlwd0);

	return ((phyctl & D11_PHY_TXC_FT_MASK(wlc->pub->corerev)) ==
		WLC_RSPEC_ENC2FT(wlc->bcn_rspec));
}

/**
 * pass cfg pointer for future "per BSS bcn" validation - it is NULL
 * when the function is called from STA association context.
 */
void
wlc_validate_bcn_phytxctl(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
	int idx;
	wlc_bsscfg_t *bc;

	FOREACH_BSS(wlc, idx, bc) {
		if (HWBCN_ENAB(bc)) {
			ASSERT(wlc_valid_beacon_phytxctl(wlc));
			break;
		}
	}
}
#endif /* BCMASSERT_SUPPORT */

uint16
wlc_acphy_txctl0_calc(wlc_info_t *wlc, ratespec_t rspec, uint8 preamble_type)
{
	uint16 phyctl;
	uint16 bw;

	/* PhyTxControlWord_0 */
	phyctl = WLC_RSPEC_ENC2FT(rspec);
	if ((preamble_type == WLC_SHORT_PREAMBLE) ||
	    (preamble_type == WLC_GF_PREAMBLE)) {
		ASSERT((preamble_type == WLC_GF_PREAMBLE) || !RSPEC_ISHT(rspec));
		phyctl |= D11AC_PHY_TXC_SHORT_PREAMBLE;
		WLCNTINCR(wlc->pub->_cnt->txprshort);
	}

	if (D11AC2_PHY_SUPPORT(wlc)) {
		/* STBC */
		if (RSPEC_ISSTBC(rspec)) {
			/* set b5 */
			phyctl |= D11AC2_PHY_TXC_STBC;
		}
	}

	phyctl |= wlc_stf_d11hdrs_phyctl_txant(wlc, rspec);

	/* bit 14/15 - 00/01/10/11 => 20/40/80/160 */
	switch (RSPEC_BW(rspec)) {
		case WL_RSPEC_BW_20MHZ:
			bw = D11AC_PHY_TXC_BW_20MHZ;
		break;
		case WL_RSPEC_BW_40MHZ:
			bw = D11AC_PHY_TXC_BW_40MHZ;
		break;
		case WL_RSPEC_BW_80MHZ:
			bw = D11AC_PHY_TXC_BW_80MHZ;
		break;
		case WL_RSPEC_BW_160MHZ:
			bw = D11AC_PHY_TXC_BW_160MHZ;
		break;
		default:
			ASSERT(0);
			bw = D11AC_PHY_TXC_BW_20MHZ;
		break;
	}
	phyctl |= bw;

	/* XXX 4360 TODO: following fields need to be filled:
	 * D11AC_PHY_TXC_NON_SOUNDING - bit 2 - is this sounding frame or not?
	 * hard-code to NOT
	 */
	phyctl |= (D11AC_PHY_TXC_NON_SOUNDING);

	/* XXX 4360 TODO:
	 * Leave these as 0 for now
	 * D11AC_PHY_TXC_BFM - bit 3
	 * D11AC_PHY_TXC_NON_LAST_PSDU - bit 5
	 */
	/* XXX 4360: - beamforming and AMPDU support
		phyctl |= (D11AC_PHY_TXC_BFM);
		for QT -- no beamform support
	*/

	return phyctl;
} /* wlc_acphy_txctl0_calc */

#ifdef WL_LPC
#define PWR_SATURATE_POS 0x1F
#define PWR_SATURATE_NEG 0xE0
#endif

uint16
wlc_acphy_txctl1_calc(wlc_info_t *wlc, ratespec_t rspec, int8 lpc_offset,
	uint8 txpwr, bool is_mu)
{
	uint16 phyctl = 0;
	chanspec_t cspec = wlc->chanspec;
	uint16 sb = ((cspec & WL_CHANSPEC_CTL_SB_MASK) >> WL_CHANSPEC_CTL_SB_SHIFT);

	/* Primary Subband Location: b 16-18
		LLL ==> 000
		LLU ==> 001
		...
		UUU ==> 111
	*/
	phyctl = sb >> ((RSPEC_BW(rspec) >> WL_RSPEC_BW_SHIFT) - 1);

#ifdef WL_LPC
	/* Include the LPC offset also in the power offset */
	if (LPC_ENAB(wlc)) {
		int8 rate_offset = txpwr, tot_offset;

		/* Addition of two S4.1 words */
		rate_offset <<= 2;
		rate_offset >>= 2; /* Sign Extend */

		lpc_offset <<= 2;
		lpc_offset >>= 2; /* Sign Extend */

		tot_offset = rate_offset + lpc_offset;

		if (tot_offset < (int8)PWR_SATURATE_NEG)
			tot_offset = PWR_SATURATE_NEG;
		else if (tot_offset > PWR_SATURATE_POS)
			tot_offset = PWR_SATURATE_POS;

		txpwr = tot_offset;
	}
#endif /* WL_LPC */

	phyctl |= (txpwr << PHY_TXC1_HTTXPWR_OFFSET_SHIFT);

	if (D11REV_LT(wlc->pub->corerev, 64)) {
		if (WLPROPRIETARY_11N_RATES_ENAB(wlc->pub)) {
			if (RSPEC_ISHT(rspec) && (rspec & WL_RSPEC_HT_MCS_MASK)
				>= WLC_11N_FIRST_PROP_MCS) {
				phyctl |= D11AC_PHY_TXC_11N_PROP_MCS;
			}
		}
	} else if (is_mu) {
		/* MU frame for corerev >= 64 */
		phyctl |= D11AC2_PHY_TXC_MU;
	}

	return phyctl;
} /* wlc_acphy_txctl1_calc */

uint16
wlc_acphy_txctl2_calc(wlc_info_t *wlc, ratespec_t rspec, uint8 txbf_uidx)
{
	uint16 phyctl = 0;
	uint nss = wlc_ratespec_nss(rspec);
	uint mcs;
	uint8 rate;
	const wlc_rateset_t* cur_rates = NULL;
	uint16 rindex;

	/* mcs+nss: bit 32 through 37 */
	if (RSPEC_ISHT(rspec)) {
		/* for 11n: B[32:37] for mcs[0:32] */
		if (WLPROPRIETARY_11N_RATES_ENAB(wlc->pub)) {
			mcs = rspec & D11AC_PHY_TXC_11N_MCS_MASK;
		} else {
			mcs = rspec & WL_RSPEC_HT_MCS_MASK;
		}
		phyctl = (uint16)mcs;
	} else if (RSPEC_ISVHT(rspec)) {
		/* for 11ac: B[32:35] for mcs[0-9], B[36:37] for n_ss[1-4]
			(0 = 1ss, 1 = 2ss, 2 = 3ss, 3 = 4ss)
		*/
		mcs = rspec & WL_RSPEC_VHT_MCS_MASK;
		ASSERT(mcs <= WLC_MAX_VHT_MCS);
		phyctl = (uint16)mcs;
		ASSERT(nss <= 8);
		phyctl |= ((nss-1) << D11AC_PHY_TXC_11AC_NSS_SHIFT);
	} else if (RSPEC_ISHE(rspec)) {
		mcs = rspec & WL_RSPEC_HE_MCS_MASK;
		ASSERT(mcs <= WLC_MAX_HE_MCS);
		phyctl = (uint16)mcs;
		ASSERT(nss <= 8);
		phyctl |= ((nss-1) << D11_REV80_PHY_TXC_11AX_NSS_SHIFT);
	} else {
		ASSERT(RSPEC_ISLEGACY(rspec));
		rate = (uint8)RSPEC2RATE(rspec);

		if (RSPEC_ISOFDM(rspec)) {
			cur_rates = &ofdm_rates;
		} else {
			/* for 11b: B[32:33] represents phyrate
				(0 = 1mbps, 1 = 2mbps, 2 = 5.5mbps, 3 = 11mbps)
			*/
			cur_rates = &cck_rates;
		}
		for (rindex = 0; rindex < cur_rates->count; rindex++) {
			if ((cur_rates->rates[rindex] & RATE_MASK) == rate) {
				break;
			}
		}
		ASSERT(rindex < cur_rates->count);
		phyctl = rindex;
	}

	if (D11AC2_PHY_SUPPORT(wlc)) {
		if (WLPROPRIETARY_11N_RATES_ENAB(wlc->pub)) {
			if (RSPEC_ISHT(rspec) && (rspec & WL_RSPEC_HT_MCS_MASK)
				>= WLC_11N_FIRST_PROP_MCS) {
				phyctl |= D11AC2_PHY_TXC_11N_PROP_MCS;
			}
		}

		/* b41 - b47: TXBF user index */
		phyctl |= (txbf_uidx << D11AC2_PHY_TXC_TXBF_USER_IDX_SHIFT);
	} else {
		/* corerev < 64 */
		/* STBC */
		if (RSPEC_ISSTBC(rspec)) {
			/* set b38 */
			phyctl |= D11AC_PHY_TXC_STBC;
		}
		/* b39 - b 47 all 0 */
	}

	return phyctl;
} /* wlc_acphy_txctl2_calc */

static bool
wlc_phy_rspec_check(wlc_info_t *wlc, uint16 bw, ratespec_t rspec)
{
	if (RSPEC_ISHE(rspec)) {
		uint8 mcs;
		uint Nss;
		uint Nsts;

		mcs = rspec & WL_RSPEC_HE_MCS_MASK;
		Nss = (rspec & WL_RSPEC_HE_NSS_MASK) >> WL_RSPEC_HE_NSS_SHIFT;

		BCM_REFERENCE(mcs);
		ASSERT(mcs <= WLC_MAX_HE_MCS);

		/* HE STBC expansion always is Nsts = Nss*2, so STBC expansion == Nss */
		if (RSPEC_ISSTBC(rspec)) {
			Nsts = 2*Nss;
		} else {
			Nsts = Nss;
		}

		/* we only support up to 3x3 */
		BCM_REFERENCE(Nsts);
		ASSERT(Nsts + RSPEC_TXEXP(rspec) <= 3);
	} else if (RSPEC_ISVHT(rspec)) {
		uint8 mcs;
		uint Nss;
		uint Nsts;

		mcs = rspec & WL_RSPEC_VHT_MCS_MASK;
		Nss = (rspec & WL_RSPEC_VHT_NSS_MASK) >> WL_RSPEC_VHT_NSS_SHIFT;

		BCM_REFERENCE(mcs);
		ASSERT(mcs <= WLC_MAX_VHT_MCS);

		/* VHT STBC expansion always is Nsts = Nss*2, so STBC expansion == Nss */
		if (RSPEC_ISSTBC(rspec)) {
			Nsts = 2*Nss;
		} else {
			Nsts = Nss;
		}

		/* we only support up to 3x3 */
		BCM_REFERENCE(Nsts);
		ASSERT(Nsts + RSPEC_TXEXP(rspec) <= 3);
	} else if (RSPEC_ISHT(rspec)) {
		uint mcs = rspec & WL_RSPEC_HT_MCS_MASK;

		ASSERT(mcs <= 32);

		if (mcs == 32) {
			ASSERT(RSPEC_ISSTBC(rspec) == FALSE);
			ASSERT(RSPEC_TXEXP(rspec) == 0);
			ASSERT(bw == PHY_TXC1_BW_40MHZ_DUP);
		} else {
			uint Nss = 1 + (mcs / 8);
			uint Nsts;

			/* BRCM HT chips only support STBC expansion by 2*Nss */
			if (RSPEC_ISSTBC(rspec)) {
				Nsts = 2*Nss;
			} else {
				Nsts = Nss;
			}

			/* we only support up to 3x3 */
			BCM_REFERENCE(Nsts);
			ASSERT(Nsts + RSPEC_TXEXP(rspec) <= 3);
		}
	} else if (RSPEC_ISOFDM(rspec)) {
		ASSERT(RSPEC_ISSTBC(rspec) == FALSE);
	} else {
		ASSERT(RSPEC_ISCCK(rspec));

		ASSERT((bw == PHY_TXC1_BW_20MHZ) || (bw == PHY_TXC1_BW_20MHZ_UP));
		ASSERT(RSPEC_ISSTBC(rspec) == FALSE);
#if defined(WL_BEAMFORMING)
		if (TXBF_ENAB(wlc->pub) && PHYCORENUM(wlc->stf->op_txstreams) > 1) {
			ASSERT(RSPEC_ISTXBF(rspec) == FALSE);
		}
#endif
	}

	return TRUE;
} /* wlc_phy_rspec_check */

uint16
wlc_phytxctl1_calc(wlc_info_t *wlc, ratespec_t rspec, chanspec_t chanspec)
{
	uint16 phyctl1 = 0;
	uint16 bw;

	if (RSPEC_IS20MHZ(rspec)) {
		bw = PHY_TXC1_BW_20MHZ;
		if (CHSPEC_IS40(chanspec) && CHSPEC_SB_UPPER(phy_utils_get_chanspec(WLC_PI(wlc)))) {
			bw = PHY_TXC1_BW_20MHZ_UP;
		}
	} else  {
		ASSERT(RSPEC_IS40MHZ(rspec));

		bw = PHY_TXC1_BW_40MHZ;
		if ((RSPEC_ISHT(rspec) && ((rspec & WL_RSPEC_HT_MCS_MASK) == 32)) ||
		    RSPEC_ISOFDM(rspec)) {
			bw = PHY_TXC1_BW_40MHZ_DUP;
		}

	}
	wlc_phy_rspec_check(wlc, bw, rspec);

	/* Handle the other mcs capable phys (NPHY, LCN at this point) */
	if (RSPEC_ISHT(rspec)) {
		uint mcs = rspec & WL_RSPEC_HT_MCS_MASK;
		uint16 stf;

		/* Determine the PHY_TXC1_MODE value based on MCS and STBC or TX expansion.
		 * PHY_TXC1_MODE values cover 1 and 2 chains only and are:
		 * (Nss = 2, NTx = 2) 		=> SDM (Spatial Division Multiplexing)
		 * (Nss = 1, NTx = 2) 		=> CDD (Cyclic Delay Diversity)
		 * (Nss = 1, +1 STBC, NTx = 2) 	=> STBC (Space Time Block Coding)
		 * (Nss = 1, NTx = 1) 		=> SISO (Single In Single Out)
		 *
		 * MCS 0-7 || MCS == 32 	=> Nss = 1 (Number of spatial streams)
		 * MCS 8-15 			=> Nss = 2
		 */
		if ((!WLPROPRIETARY_11N_RATES_ENAB(wlc->pub) && mcs > 7 && mcs <= 15) ||
		     (WLPROPRIETARY_11N_RATES_ENAB(wlc->pub) && GET_11N_MCS_NSS(mcs) == 2)) {
			/* must be SDM, only 2x2 devices */
			stf = PHY_TXC1_MODE_SDM;
		} else if (RSPEC_TXEXP(rspec)) {
			/*
			 * Tx Expansion: number of tx chains (NTx) beyond the minimum required for
			 * the space-time-streams, to increase robustness. Expansion is non-STBC.
			 */
			stf = PHY_TXC1_MODE_CDD;
		} else if (RSPEC_ISSTBC(rspec)) {
			/* expansion is STBC */
			stf = PHY_TXC1_MODE_STBC;
		} else {
			/* no expansion */
			stf = PHY_TXC1_MODE_SISO;
		}

		phyctl1 = bw | (stf << PHY_TXC1_MODE_SHIFT);

		/* set the upper byte of phyctl1 */
		phyctl1 |= (mcs_table[mcs].tx_phy_ctl3 << 8);
	} else if (RSPEC_ISCCK(rspec)) {
		phyctl1 = (bw | PHY_TXC1_MODE_SISO);
	} else {	/* legacy OFDM/CCK */
		int16 phycfg;
		uint16 stf;
		uint rate = RSPEC2RATE(rspec);

		ASSERT(RSPEC_ISOFDM(rspec));

		/* get the phyctl byte from rate phycfg table */
		if ((phycfg = wlc_rate_legacy_phyctl(rate)) == -1) {
			WL_ERROR(("%s: wrong legacy OFDM/CCK rate\n", __FUNCTION__));
			ASSERT(0);
			phycfg = 0;
		}

		if (RSPEC_TXEXP(rspec)) {
			/* CDD expansion for OFDM */
			stf = PHY_TXC1_MODE_CDD;
		} else {
			stf = PHY_TXC1_MODE_SISO;
		}

		/* set the upper byte of phyctl1 */
		phyctl1 = (bw | (phycfg << 8) | (stf << PHY_TXC1_MODE_SHIFT));
	}
#ifdef BCMDBG
	/* phy clock must support 40Mhz if tx descriptor uses it */
	if ((phyctl1 & PHY_TXC1_BW_MASK) >= PHY_TXC1_BW_40MHZ) {
		ASSERT(CHSPEC_IS40(wlc->chanspec));
		ASSERT(wlc->chanspec == phy_utils_get_chanspec(WLC_PI(wlc)));
	}
#endif /* BCMDBG */
	return phyctl1;
} /* wlc_phytxctl1_calc */

#if defined(BCMDBG)

void
wlc_txq_dump_ampdu_txq(wlc_info_t *wlc, wlc_bsscfg_t * cfg, struct scb *scb, struct bcmstrbuf *b)
{
	struct pktq * pktq;			/**< Multi-priority packet queue */
	/* Print # of packets queued in tx ampdu queue */
	if (wlc->ampdu_tx && SCB_AMPDU(scb) &&
		(pktq = scb_ampdu_prec_pktq(wlc, scb)) &&
		(pktq_n_pkts_tot(pktq))) {
		bcm_bprintf(b, "\t\t\t\tSCB TX AMPDU Q len %d\n", pktq_n_pkts_tot(pktq));
		wlc_pktq_dump(wlc, pktq, NULL, scb, "\t\t\t\t\t", b);
	}
}

void
wlc_txq_dump_ampdu_rxq(wlc_info_t *wlc, wlc_bsscfg_t * cfg, struct scb *scb, struct bcmstrbuf *b)
{
	int idx, queued;

	/* Print # of packets queued in RX reorder queue */
	if (wlc->ampdu_rx && SCB_AMPDU(scb)) {
		for (idx = 0; idx < AMPDU_MAX_SCB_TID; idx++) {
			if ((queued = wlc_ampdu_rx_queued_pkts(wlc->ampdu_rx, scb, idx))) {
				wlc_ampdu_rx_dump_queued_pkts(wlc->ampdu_rx, scb, idx, b);
			}
		}
	}
}

#ifdef AP
void
wlc_txq_dump_ap_psq(wlc_info_t *wlc, wlc_bsscfg_t * cfg, struct scb *scb, struct bcmstrbuf *b)
{
	struct pktq * pktq;			/**< Multi-priority packet queue */
	int queued;

	/* Dump APPS PSQ */
	if (wlc->psinfo && BSSCFG_AP(cfg) &&
	    (pktq = wlc_apps_get_psq(wlc, scb)) &&
	    (queued = pktq_n_pkts_tot(pktq))) {
		bcm_bprintf(b, "\t\t\t\tSCB APPS Q len %d\n", queued);
		wlc_pktq_dump(wlc, pktq, NULL, NULL, "\t\t\t\t\t", b);
	}
}
#endif /* AP */

#ifdef WL_BSSCFG_TX_SUPR
void
wlc_txq_dump_supr_txq(wlc_info_t *wlc, wlc_bsscfg_t * cfg, struct scb *scb, struct bcmstrbuf *b)
{
	struct pktq * pktq;			/**< Multi-priority packet queue */
	int queued;

	/* Dump suppression Q */
	pktq = wlc_bsscfg_get_psq(wlc->psqi, cfg);
	if (pktq && (queued = pktq_n_pkts_tot(pktq))) {
		bcm_bprintf(b, "\t\t\t\t SUPPR Q len %d\n", queued);
		wlc_pktq_dump(wlc, pktq, NULL, scb, "\t\t\t\t\t", b);
	}
}
#endif /* WL_BSSCFG_TX_SUPR */

static int
wlc_datapath_dump(wlc_info_t *wlc, struct bcmstrbuf *b)
{
	wlc_txq_info_t *qi;

	if ((wlc == NULL) || (b == NULL))  {
		return BCME_BADARG;
	}

	/* Dump FIFO queued states */
	bcm_bprintf(b, "FIFO TXPKTPEND: BK:%d, BE:%d, VI:%d, VO:%d, BCMC:%d, ATIM:%d\n",
		TXPKTPENDGET(wlc, TX_AC_BK_FIFO), TXPKTPENDGET(wlc, TX_AC_BE_FIFO),
		TXPKTPENDGET(wlc, TX_AC_VI_FIFO), TXPKTPENDGET(wlc, TX_AC_VO_FIFO),
		TXPKTPENDGET(wlc, TX_BCMC_FIFO), TXPKTPENDGET(wlc, TX_ATIM_FIFO));

	/* Dump all txq states */
	for (qi = wlc->tx_queues; qi; qi = qi->next) {
		wlc_datapath_dump_q_summary(wlc, qi, b);
	}

	return BCME_OK;
}

static void
wlc_datapath_dump_q_summary(wlc_info_t *wlc, wlc_txq_info_t *qi, struct bcmstrbuf *b)
{
	uint idx = 0;
	wlc_bsscfg_t * cfg;

	bcm_bprintf(b, "tx_queue %p stopped = 0x%x %s%s\n",
	            OSL_OBFUSCATE_BUF(qi), qi->stopped,
	            (wlc->active_queue == qi ? "(Active Q)" : ""),
	            (wlc->primary_queue == qi ? "(Primry Q)" :
	             wlc->excursion_queue == qi ? "(Excursion Q)" : ""));

	wlc_datapath_dump_lowtxq_summary(qi->low_txq, b);

	/* serialized TxQ in qi context */
	bcm_bprintf(b, "\ttxq qlen:%u max:%u\n",
	            pktq_n_pkts_tot(WLC_GET_CQ(qi)), pktq_max(WLC_GET_CQ(qi)));

	/* Dump interfaces associated with this queue and pending packets per bsscfg */
	FOREACH_BSS(wlc, idx, cfg) {
		if ((cfg->wlcif != NULL) && (cfg->wlcif->qi == qi)) {
			wlc_datapath_dump_bss_summary(wlc, qi, cfg, b);
		}
	}
}

/* fifo state flags for low txq */
static const bcm_bit_desc_t txq_flag_desc[] = {
	{TXQ_STOPPED, "STOPPED"},
	{TXQ_HW_STOPPED, "HW_STOPPED"},
	{TXQ_HW_HOLD, "HW_HOLD"},
};

/** @param txq    E.g. the low queue, consisting of a queue for every d11 fifo */
static int
wlc_datapath_dump_lowtxq_summary(txq_t *txq, struct bcmstrbuf *b)
{
	txq_swq_t *txq_swq;			/**< a non-priority queue backing one d11 FIFO */
	char flagstr[64];
	int printed_lines = FALSE;
	uint i;

	bcm_bprintf(b, "\tlow_txq: %u fifos", txq->nfifo);

	for (i = 0, txq_swq = txq->swq_table; i < txq->nfifo; i++, txq_swq++) {
		txq_swq_flowctl_t *f = &txq_swq->flowctl;
		struct pktq_prec * q = &txq_swq->spktq.q;  /**< single precedence packet queue */

		/* skip if nothing interesting to print */
		if (q->n_pkts == 0 && f->flags == 0 && f->buffered == 0) {
			continue;
		}

		if (printed_lines == FALSE) {
			bcm_bprintf(b, "\n");
			printed_lines = TRUE;
		}

		bcm_format_flags(txq_flag_desc, f->flags, flagstr, sizeof(flagstr));

		bcm_bprintf(b, "\tlow_txq fifo %u: flags 0x%04x [%s] "
		            "buffered %5d qpkts %u\n",
		            i, f->flags, flagstr,
		            f->buffered, (uint)q->n_pkts);
	}

	if (printed_lines == FALSE) {
		bcm_bprintf(b, " (empty)\n");
	}

	return BCME_OK;
}

static void
wlc_datapath_dump_bss_summary(wlc_info_t *wlc, wlc_txq_info_t *qi, wlc_bsscfg_t * cfg,
	struct bcmstrbuf *b)
{
	char ifname[32];
	struct scb *scb;
	struct scb_iter scbiter;
	int scb_cnt = 0;
	struct pktq * common_q = WLC_GET_CQ(qi); /** serialized (multi-priority) TxQ */

	strncpy(ifname, wl_ifname(wlc->wl, cfg->wlcif->wlif),
	        sizeof(ifname));
	ifname[sizeof(ifname) - 1] = '\0';
	bcm_bprintf(b, "\tbsscfg %d (%s)\n", cfg->_idx, ifname);

	/* Dump packets queued for this bsscfg */
	wlc_pktq_dump(wlc, common_q, cfg, NULL, "\t\t", b);

	/* Dump packets queued for each scb */
	FOREACH_BSS_SCB(wlc->scbstate, &scbiter, cfg, scb) {
		int queued;
		struct pktq * tmp_pktq;
		BCM_REFERENCE(queued);
		BCM_REFERENCE(tmp_pktq);

		bcm_bprintf(b, "\t\t\tSCB:%3d%s "MACF" PS %d\n",
			scb_cnt, (scb->permanent? "*" : " "),
			ETHER_TO_MACF(scb->ea), SCB_PS(scb));
		wlc_pktq_dump(wlc, common_q, NULL, scb, "\t\t\t\t", b);
		wlc_txq_dump_ampdu_txq(wlc, cfg, scb, b);
		wlc_txq_dump_ampdu_rxq(wlc, cfg, scb, b);
#ifdef AP
		wlc_txq_dump_ap_psq(wlc, cfg, scb, b);
#endif

#ifdef WL_BSSCFG_TX_SUPR
		wlc_txq_dump_supr_txq(wlc, cfg, scb, b);
#endif
		scb_cnt++;
	}
}

/** @param q   Multi-priority packet queue */
static int
wlc_pktq_dump(wlc_info_t *wlc, struct pktq *q, wlc_bsscfg_t *bsscfg, struct scb * scb,
	const char * prefix, struct bcmstrbuf *b)
{
	uint prec;
	void * p;
	uint bsscfg_pkt_count, scb_pkt_count;

	/* Print the details of packets held in the queue */
	for (prec = 0; prec < q->num_prec; prec++) {

		/* Do not print empty queues */
		if (pktqprec_n_pkts(q, prec) == 0) {
			continue;
		}

		bsscfg_pkt_count = 0;
		scb_pkt_count = 0;

		for (p = pktqprec_peek(q, prec); (p != NULL); p = PKTLINK(p)) {
			if (bsscfg && (wlc->bsscfg[WLPKTTAGBSSCFGGET(p)] == bsscfg)) {
				bsscfg_pkt_count++;
			}

			if (scb && (WLPKTTAGSCBGET(p) == scb)) {
				scb_pkt_count++;
			}
		}

		/* print packets queued for all bsscfg and all SCB */
		if ((bsscfg == NULL) && (scb == NULL)) {
			bcm_bprintf(b, "%sprec %u qpkts %u, max_pkts:%u\n", prefix ? prefix : "",
				prec, pktqprec_n_pkts(q, prec), pktqprec_max_pkts(q, prec));
		} else if (bsscfg == NULL) {
			/* print packets queued for input scb */
			if (scb_pkt_count) {
				bcm_bprintf(b, "%sprec %u scb_pkts %u, \n", prefix ? prefix : "",
					prec, scb_pkt_count);
			}
		} else if (scb == NULL) {
			/* print packets queued for input bsscfg */
			if (bsscfg_pkt_count) {
				bcm_bprintf(b, "%sprec %u bss_pkts %u\n", prefix ? prefix : "",
					prec, bsscfg_pkt_count);
			}
		} else {
			/* print packets queued for both bsscfg and scb */
			bcm_bprintf(b, "%sprec %u qpkts %u, max_pkts:%u, bss_pkts %u,"
				" scb_pkts %u\n", prefix ? prefix : "",
				prec, pktqprec_n_pkts(q, prec), pktqprec_max_pkts(q, prec),
				bsscfg_pkt_count, scb_pkt_count);
		}
	}

	return BCME_OK;
}
#endif

#if defined(WL_PRQ_RAND_SEQ)
int
wlc_tx_prq_rand_seq_enable(wlc_info_t *wlc, wlc_bsscfg_t *cfg, bool enable)
{
	tx_prs_cfg_t *tx_prs_cfg = TX_PRS_CFG(wlc->txqi, cfg);

	if (!(PRQ_RAND_SEQ_ENAB(wlc->pub))) {
		WL_ERROR(("Feature is not supported!\n"));
		return BCME_UNSUPPORTED;
	}

	ASSERT(cfg);
	/*
	 * Update the randomize of probe_request sequence
	 */
	if (enable && tx_prs_cfg &&
		(tx_prs_cfg->prq_rand_seq & PRQ_RAND_SEQ_ENABLE)) {
		tx_prs_cfg->prq_rand_seq |= PRQ_RAND_SEQID;
		wlc_getrand(wlc, (uint8 *)&(tx_prs_cfg->prq_rand_seqId),
				sizeof(uint16));
		WL_INFORM(("Enable randomization of Probe Seq\n"));
	} else {
		/* Restore randomize probe seq */
		tx_prs_cfg->prq_rand_seq &= ~PRQ_RAND_SEQID;
		tx_prs_cfg->prq_rand_seqId = 0;
		WL_INFORM(("Disable randomization of Probe Seq\n"));
	}
	return BCME_OK;
}
#endif /* WL_PRQ_RAND_SEQ */

void * BCMFASTPATH
cpktq_deq(struct cpktq *cpktq, int *prec_out)
{
	void *p = pktq_deq(&cpktq->cq, prec_out);
	if (p) {
		SCB_PKTS_INFLT_CQCNT_DECR(WLPKTTAGSCBGET(p), *prec_out);
	}
	return	p;
}

void * BCMFASTPATH
cpktq_pdeq(struct cpktq *cpktq, int prec)
{
	void *p = pktq_pdeq(&cpktq->cq, prec);

	if (p) {
		SCB_PKTS_INFLT_CQCNT_DECR(WLPKTTAGSCBGET(p), prec);
	}
	return	p;
}

void * BCMFASTPATH
cpktq_deq_tail(struct cpktq *cpktq, int *prec_out)
{
	void *p = pktq_deq_tail(&cpktq->cq, prec_out);
	if (p) {
		SCB_PKTS_INFLT_CQCNT_DECR(WLPKTTAGSCBGET(p), *prec_out);
	}
	return p;
}

void * BCMFASTPATH
cpktq_pdeq_tail(struct cpktq *cpktq, int prec)
{
	void *p = pktq_pdeq_tail(&cpktq->cq, prec);
	if (p) {
		SCB_PKTS_INFLT_CQCNT_DECR(WLPKTTAGSCBGET(p), prec);
	}
	return p;
}

void * BCMFASTPATH
cpktq_mdeq(struct cpktq *cpktq, uint prec_bmp, int *prec_out)
{
	void *p = pktq_mdeq(&cpktq->cq, prec_bmp, prec_out);

	if (p) {
		SCB_PKTS_INFLT_CQCNT_DECR(WLPKTTAGSCBGET(p), *prec_out);
	}
	return p;
}

void * BCMFASTPATH
cpktq_penq(wlc_info_t *wlc, struct cpktq *cpktq, int prec, void *p, bool head)
{
	scb_t *scb = WLPKTTAGSCBGET(p);

	ASSERT(pktq_avail(&cpktq->cq) >= 1);
	if (cpktq->flush_in_progress) {
#ifdef WLTAF
		wlc_taf_txpkt_status(wlc->taf_handle, NULL, TAF_PREC(prec), p,
			TAF_TXPKT_STATUS_PKTFREE_DROP);
#endif
		if (scb) {
			TX_PKTDROP_COUNT(wlc, scb, TX_PKTDROP_RSN_COMMON_Q_FULL);
		}
		PKTFREE(wlc->osh, p, TRUE);
		return NULL;
	}
	if (head) {
		pktq_penq_head(&cpktq->cq, prec, p);
	} else {
		pktq_penq(&cpktq->cq, prec, p);
	}
	SCB_PKTS_INFLT_CQCNT_INCR(scb, prec);

	return p;
}

bool BCMFASTPATH
cpktq_prec_enq(wlc_info_t *wlc, struct cpktq *cpktq, void *p, int prec, bool head)
{
	bool rc;

	if (cpktq->flush_in_progress) {
		return FALSE;
	}

	/* Note: queue len not checked here, pkt will be evicted if queue is full */
	rc = wlc_prec_enq_head(wlc, &cpktq->cq, p, prec, head, NULL);

	if (rc) {
		SCB_PKTS_INFLT_CQCNT_INCR(WLPKTTAGSCBGET(p), prec);
	}
	return rc;
}

void
cpktq_flush(wlc_info_t *wlc, struct cpktq *cpktq)
{
	struct pktq *pq = &cpktq->cq;
	void *p;
	int prec;

	cpktq->flush_in_progress = TRUE;
#ifdef WLTAF
	/* Defer taf schedule to prevent pkt enqueue during flush */
	wlc_taf_inhibit(wlc, TRUE);
#endif /* TAF */
	PKTQ_PREC_ITER(pq, prec) {
		/* assuming single threaded operation,
		 * otherwise a mutex is needed.
		 */

		/* start with the head of the list */
		while ((p = pktq_pdeq(pq, prec)) != NULL) {
			/* delete this packet */
			SCB_PKTS_INFLT_CQCNT_DECR(WLPKTTAGSCBGET(p), prec);
#ifdef WLTAF
			wlc_taf_txpkt_status(wlc->taf_handle, NULL, TAF_PREC(prec), p,
				TAF_TXPKT_STATUS_PKTFREE_DROP);
#endif
			PKTFREE(wlc->osh, p, TRUE);
		}
	}
#ifdef WLTAF
	wlc_taf_inhibit(wlc, FALSE);
#endif /* TAF */
	cpktq->flush_in_progress = FALSE;
}

/** @param pq   Multi-priority packet queue */
void BCMFASTPATH
cpktq_dec(struct pktq *pq, void *p, int prec)
{
	if (IS_COMMON_QUEUE(pq)) {
		SCB_PKTS_INFLT_CQCNT_DECR(WLPKTTAGSCBGET(p), prec);
	}
}

/** @param pq   Multi-priority packet queue */
static void
_pktq_pfilter(struct pktq *pq, int prec, pktq_filter_t fltr, void* fltr_ctx,
              defer_free_pkt_fn_t defer, void *defer_ctx)
{
	struct pktq_prec wq;			/**< single precedence packet queue */
	struct pktq_prec *q;			/**< single precedence packet queue */
	void *p;

	/* protect shared resource */
	if (HND_PKTQ_MUTEX_ACQUIRE(&pq->mutex, OSL_EXT_TIME_FOREVER) != OSL_EXT_SUCCESS)
		return;

	/* move the prec queue aside to a work queue */
	q = &pq->q[prec];

	wq = *q;

	q->head = NULL;
	q->tail = NULL;
	q->n_pkts = 0;
	pq->n_pkts_tot -= wq.n_pkts;

	/* protect shared resource */
	if (HND_PKTQ_MUTEX_RELEASE(&pq->mutex) != OSL_EXT_SUCCESS)
		return;

	/* start with the head of the work queue */
	while ((p = wq.head) != NULL) {
		/* unlink the current packet from the list */
		wq.head = PKTLINK(p);
		PKTSETLINK(p, NULL);
		wq.n_pkts--;

		/* call the filter function on current packet */
		ASSERT(fltr != NULL);
		switch ((*fltr)(fltr_ctx, p)) {
		case PKT_FILTER_NOACTION:
			/* put this packet back */
			pktq_penq(pq, prec, p);
			break;

		case PKT_FILTER_DELETE:
			/* Update common queue accounting,
			 * NO-OP if queue is not a common queue
			 */
			cpktq_dec(pq, p, prec);

			/* delete this packet */
			ASSERT(defer != NULL);
			(*defer)(defer_ctx, p);
			break;

		case PKT_FILTER_REMOVE:
			/* pkt already removed from list */
			break;

		default:
			ASSERT(0);
			break;
		}
	}

	ASSERT(wq.n_pkts == 0);
}

/** @param pq   Multi-priority packet queue */
void
pktq_pfilter(struct pktq *pq, int prec, pktq_filter_t fltr, void* fltr_ctx,
	defer_free_pkt_fn_t defer, void *defer_ctx, flush_free_pkt_fn_t flush, void *flush_ctx)
{
	_pktq_pfilter(pq, prec, fltr, fltr_ctx, defer, defer_ctx);

	ASSERT(flush != NULL);
	(*flush)(flush_ctx);
}

/** @param pq   Multi-priority packet queue */
void
pktq_filter(struct pktq *pq, pktq_filter_t fltr, void* fltr_ctx,
	defer_free_pkt_fn_t defer, void *defer_ctx, flush_free_pkt_fn_t flush, void *flush_ctx)
{
	bool filter = FALSE;

	/* protect shared resource */
	if (HND_PKTQ_MUTEX_ACQUIRE(&pq->mutex, OSL_EXT_TIME_FOREVER) != OSL_EXT_SUCCESS)
		return;

	/* Optimize if pktq n_pkts = 0, just return.
	 * pktq len of 0 means pktq's prec q's are all empty.
	 */
	if (pq->n_pkts_tot > 0) {
		filter = TRUE;
	}

	/* protect shared resource */
	if (HND_PKTQ_MUTEX_RELEASE(&pq->mutex) != OSL_EXT_SUCCESS)
		return;

	if (filter) {
		int prec;

		PKTQ_PREC_ITER(pq, prec) {
			_pktq_pfilter(pq, prec, fltr, fltr_ctx, defer, defer_ctx);
		}

		ASSERT(flush != NULL);
		(*flush)(flush_ctx);
	}
}

/** @param spq  Single priority packet queue */
void
spktq_filter(struct spktq *spq, pktq_filter_t fltr, void* fltr_ctx,
	defer_free_pkt_fn_t defer, void *defer_ctx, flush_free_pkt_fn_t flush, void *flush_ctx,
	flip_epoch_t flip_epoch_callback)
{
	struct pktq_prec wq;				/**< single precedence packet queue */
	struct pktq_prec *q;				/**< single precedence packet queue */
	void *p;
	uint8 flipEpoch;
	uint8 lastEpoch;
	/* protect shared resource */
	if (HND_PKTQ_MUTEX_ACQUIRE(&spq->mutex, OSL_EXT_TIME_FOREVER) != OSL_EXT_SUCCESS)
		return;

	q = &spq->q;

	/* Optimize if pktq_prec n_pkts = 0, just return. */
	if (q->n_pkts == 0) {
		(void)HND_PKTQ_MUTEX_RELEASE(&spq->mutex);
		return;
	}

	wq = *q;

	q->head = NULL;
	q->tail = NULL;
	q->n_pkts = 0;

	/* protect shared resource */
	if (HND_PKTQ_MUTEX_RELEASE(&spq->mutex) != OSL_EXT_SUCCESS)
		return;
	flipEpoch = 0;
	lastEpoch = HEAD_PKT_FLUSHED;
	/* start with the head of the work queue */
	while ((p = wq.head) != NULL) {
		/* unlink the current packet from the list */
		wq.head = PKTLINK(p);
		PKTSETLINK(p, NULL);
		wq.n_pkts--;

		/* call the filter function on current packet */
		ASSERT(fltr != NULL);
		switch ((*fltr)(fltr_ctx, p)) {
		case PKT_FILTER_NOACTION:

			if (flip_epoch_callback != NULL) {
				(*flip_epoch_callback)(defer_ctx, p, &flipEpoch, &lastEpoch);
				/* clear pkt delete condition */
				flipEpoch &= ~TXQ_PKT_DEL;
			}
			/* put this packet back */
			spktq_enq(spq, p);
			break;

		case PKT_FILTER_DELETE:
			/* delete this packet */
			ASSERT(defer != NULL);
			(*defer)(defer_ctx, p);
			flipEpoch |= TXQ_PKT_DEL;
			break;

		case PKT_FILTER_REMOVE:
			/* pkt already removed from list */
			break;

		default:
			ASSERT(0);
			break;
		}
	}

	ASSERT(wq.n_pkts == 0);

	ASSERT(flush != NULL);
	(*flush)(flush_ctx);
}

#if defined(WLCFP) && defined(BCM_DHDHDR)
/* Given a pkt, adjust the host segment length */
static void
__wlc_tx_adjust_hostfrag(wlc_info_t *wlc, void* p, uint16 len)
{
	ASSERT(PKTISTXFRAG(wlc->osh, p));

	/* Adjust Host low address */
	PKTSETFRAGDATA_LO(wlc->osh, p, LB_FRAG1,
		PKTFRAGDATA_LO(wlc->osh, p, LB_FRAG1) - len);
	/* Adjust Frag Length */
	PKTSETFRAGLEN(wlc->osh, p, LB_FRAG1,
		PKTFRAGLEN(wlc->osh, p, LB_FRAG1) + len);
	PKTSETFRAGTOTLEN(wlc->osh, p,
		PKTFRAGTOTLEN(wlc->osh, p) + len);

}

void
wlc_tx_adjust_hostfrag(wlc_info_t *wlc, void* p, uint16 len)
{

	/* Adjust Host Length for subframes too */
	for (; p; p = PKTNEXT(wlc->osh, p)) {
		__wlc_tx_adjust_hostfrag(wlc, p, len);
	}
}
#endif /* WLCFP &&  BCM_DHDHDR */

bool
wlc_mutx_ac(txq_info_t *txqi, uint8 ac, uint8 bit_pos)
{
	return ((txqi->mutx_ac_mask & (bit_pos << (MU_AC_MASK_SIZE * ac))) != 0);
}

void
wlc_get_muagg_histo(wlc_info_t *wlc, muagg_histo_t *histo)
{
	muagg_histo_bucket_t *entry, *total;
	int i;

	total = &histo->bucket[MUAGG_SUM_BUCKET];
	memset(total, 0, sizeof(*total));
	histo->muagg_sum_ampdu = 0;
	if (wlc->hw->clk == 0) {
		return;
	}

	/* Read VHTMU histo data */
	entry = &histo->bucket[MUAGG_VHTMU_BUCKET];
	entry->sum = wlc_bmac_read_shm(wlc->hw, M_MUAGG_HISTO(wlc));
	total->sum += entry->sum;
	for (i = 0; i < MAX_VHTMU_USERS; i++) {
		entry->num_ppdu[i] = wlc_bmac_read_shm(wlc->hw,
			M_MUAGG_HISTO(wlc) + (i+1) * 2);
		total->num_ppdu[i] += entry->num_ppdu[i];
		histo->muagg_sum_ampdu += ((i+1) * entry->num_ppdu[i]);
	}

	/* Read HEMUMIMO histo data */
	entry = &histo->bucket[MUAGG_HEMUMIMO_BUCKET];
	entry->sum = wlc_bmac_read_shm(wlc->hw, M_HEMMUAGG_HISTO(wlc));
	total->sum += entry->sum;
	for (i = 0; i < MAX_HEMUMIMO_USERS; i++) {
		entry->num_ppdu[i] = wlc_bmac_read_shm(wlc->hw,
			M_HEMMUAGG_HISTO(wlc) + (i+1) * 2);
		total->num_ppdu[i] += entry->num_ppdu[i];
		histo->muagg_sum_ampdu += ((i+1) * entry->num_ppdu[i]);
	}

	/* Read DL-OFDMA histo data */
	entry = &histo->bucket[MUAGG_HEOFDMA_BUCKET];
	entry->sum = wlc_bmac_read_shm(wlc->hw, M_HEOMUAGG_HISTO(wlc));
	total->sum += entry->sum;
	for (i = 0; i < MAX_HEOFDMA_USERS; i++) {
		entry->num_ppdu[i] = wlc_bmac_read_shm(wlc->hw,
			M_HEOMUAGG_HISTO(wlc) + (i+1) * 2);
		total->num_ppdu[i] += entry->num_ppdu[i];
		histo->muagg_sum_ampdu += ((i+1) * entry->num_ppdu[i]);
	}
}

void
wlc_clear_muagg_histo(wlc_info_t *wlc)
{
	int i;
	for (i = 0; i <= MAX_VHTMU_USERS; i++) {
			wlc_bmac_write_shm(wlc->hw, M_MUAGG_HISTO(wlc) + (i * 2), 0);
	}
	for (i = 0; i <= MAX_HEMUMIMO_USERS; i++) {
		wlc_bmac_write_shm(wlc->hw, M_HEMMUAGG_HISTO(wlc) + (i * 2), 0);
	}
	for (i = 0; i <= MAX_HEOFDMA_USERS; i++) {
		wlc_bmac_write_shm(wlc->hw, M_HEOMUAGG_HISTO(wlc) + (i * 2), 0);
	}
	wlc_bmac_write_shm(wlc->hw, M_TXAMPDUSU_CNT(wlc), 0);
#ifdef WL_PSMX
	wlc_bmac_write_shmx(wlc->hw, MX_SCHED_MAX(wlc), 0);
#endif /* WL_PSMX */
}

/* Returns TX power back-off/offset in units of half-dB for beacons to
 * implement the bcnprs_txpwr_offset feature. txpwrs is half-dB back-off.
 */
uint8
wlc_get_bcnprs_txpwr_offset(wlc_info_t *wlc, uint8 txpwrs)
{
	bool override = 0;
	int8 max_txpwr_qdbm = 0; /* the configured TX power */
	int8 min_txpwr_qdbm;     /* min allowed beacon power */
	int8 base_txpwr_qdbm;    /* power back off reference */
	int8 tot_offset = 0;     /* Total offset in half dB */
	uint8 limit_offset = 0;
#ifdef WLOLPC
	int tssivisi_thresh_qdbm = wlc_phy_tssivisible_thresh(WLC_PI(wlc));
#endif /* WLOLPC */

	/* Convert to qdBm from dBm */
	min_txpwr_qdbm = WLC_BCNPRS_MIN_TXPWR << 2;

	/* Total offset in half dB - bcnprs_txpwr_offset in dB, txpwrs in half-dB. */
	tot_offset = (wlc->stf->bcnprs_txpwr_offset << 1) + txpwrs;
	if ((wlc->stf->bcnprs_txpwr_offset > 0) &&
	    (wlc_phy_txpower_get(WLC_PI(wlc), &max_txpwr_qdbm, &override) == BCME_OK)) {
#ifdef WLOLPC
		/* If target power is below TSSI visibility threshold, the pwr offset
		 * is relative to tssivisi threshold. Set the 'base txpwr' to
		 * tssivisi_thresh_qdbm. Otherwise, offset is relative to max_txpwr_qdbm.
		 */
		if (max_txpwr_qdbm < tssivisi_thresh_qdbm) {
			base_txpwr_qdbm = tssivisi_thresh_qdbm;
		} else
#endif /* WLOLPC */
		{
			base_txpwr_qdbm = max_txpwr_qdbm;
		}
		/* Restrict bcn/prs tx power >= min_txpwr_qdbm */
		if ((base_txpwr_qdbm - (2 * tot_offset)) < min_txpwr_qdbm) {
			/* Convert to half-dB */
			tot_offset = (base_txpwr_qdbm - min_txpwr_qdbm) >> 1;
		}
	} else {
		tot_offset = txpwrs;
	}
	/* limitation of supported backoff range */
	limit_offset = (D11REV_GE(wlc->pub->corerev, 64))? 0x3f:0x1f;
	tot_offset = (tot_offset >= limit_offset)? limit_offset: tot_offset;

	return tot_offset;
}

/**
 * Apply FIFO lifetime to TXD
 */
static INLINE void
txq_lifetime(wlc_info_t *wlc, wlc_pkttag_t *pkttag, d11txhdr_t *txh)
{
	if (D11REV_GE(wlc->pub->corerev, 128)) {
		/* rev128 expects remaining time, not abs time */
		uint32 now;
		int32 lifetime;

		now = wlc_lifetime_now(wlc);
		if (pkttag->flags & WLF_EXPTIME) {
			/* if less than 0, dropped by ucode */
			lifetime = MAX((int32)(pkttag->u.exptime - now), 0);
		} else if (wlc->lifetime_txfifo) {
			/* WLF_EXPTIME is set when enqueued to low TXQ (AMPDU and wlc_pull_q)
			 * so the above if () path is normally taken.
			 * This path is worth keeping to handle unanticipated cases.
			 */
			lifetime = wlc->lifetime_txfifo;
			/* Add absolute timeout in order to handle flush/sync cycles.
			 * if packet re-enters this path a second time, use original timeout
			 */
			pkttag->flags |= WLF_EXPTIME;
			pkttag->u.exptime = now + lifetime;
		} else {
			return;
		}

		/* lifetime is 10bits in 1024usec unit. First
		 * update to 1024 usec unit, then update the two
		 * related fields.
		 */
		lifetime >>= D11_REV128_LIFETIME_SHIFT;
		lifetime = MIN(lifetime, D11_REV128_LIFETIME_MAX);
		txh->rev128.IVOffset_Lifetime_lo |=
		    ((lifetime & D11_REV128_LIFETIME_LO_MASK) << D11_REV128_LIFETIME_LO_SHIFT);
		lifetime >>= (8 - D11_REV128_LIFETIME_LO_SHIFT);
		txh->rev128.Lifetime_hi = (uint8)lifetime;
		txh->rev128.EnqueueTimestamp_L  = htol16(now);
		txh->rev128.EnqueueTimestamp_ML = htol16(now>>16);
		*D11_TXH_GET_MACLOW_PTR(wlc, txh) |= htol16(D11AC_TXC_AGING);
	} else if (pkttag->flags & WLF_EXPTIME) {
		if (D11REV_GE(wlc->pub->corerev, 40)) {
			txh->rev40.PktInfo.Tstamp =
				htol16((uint16)(pkttag->u.exptime >> D11AC_TSTAMP_SHIFT));
			*D11_TXH_GET_MACLOW_PTR(wlc, txh) |= htol16(D11AC_TXC_AGING);
		} else {
			txh->pre40.TstampLow |= htol16((uint16)pkttag->u.exptime);
			txh->pre40.TstampHigh = htol16((uint16)(pkttag->u.exptime >> 16));
			*D11_TXH_GET_MACLOW_PTR(wlc, txh) |= htol16(TXC_LIFETIME);
		}
	}
}

#if defined(HWA_PKTPGR_BUILD)
static void
wlc_hwa_tx_fifo_shadow_account(wlc_info_t *wlc, txq_t *txq, uint fifo, uint8 flag)
{
	struct spktq pkt_list;	/**< single priority packet queue */
	struct spktq temp_q;	/**< single priority packet queue */
	txq_swq_t *txq_swq;	/**< a non-priority queue backing one d11 FIFO */
	uint16 pkt_cnt;
	void *pkt;

	spktq_init(&pkt_list, PKTQ_LEN_MAX);
	spktq_init(&temp_q, PKTQ_LEN_MAX);
	txq_swq = TXQ_SWQ(txq, fifo);
	pkt_cnt = 0;

	while (NULL != (pkt = wlc_bmac_dma_getnexttxp(wlc, fifo,
			HNDDMA_RANGE_ALL))) {
#ifdef WLC_MACDBG_FRAMEID_TRACE
		wlc_macdbg_frameid_trace_sync(wlc->macdbg, pkt);
#endif
		spktq_enq(&pkt_list, pkt);
	}

	// Move the pkts in low_txq to tempary queue before push the pkts to HDBM
	if (spktq_n_pkts(&txq_swq->spktq) > 0) {
		spktq_enq_list(&temp_q, &txq_swq->spktq);
	} else {
		/* Move swq_node from empty list to active list */
		dll_delete(&txq_swq->node);
		dll_append(&txq->active, &txq_swq->node);
	}

	/* enqueue the collected pkts from DMA ring
	* at the head of the low software TxQ
	*/
	if (spktq_n_pkts(&pkt_list) > 0) {
		spktq_enq_list(&txq_swq->spktq, &pkt_list);

		if (IS_EXCURSION_QUEUE(txq, wlc->txqi)) {
			/* Packets being sent during an excursion should only be valid
			 * for the duration of that excursion.	If any packets are still
			 * in the queue at the end of excursion should be flushed.
			 * So change the flag to FLUSHFIFO.
			 */
			flag = FLUSHFIFO;
		}

		/*
		* Account for the collective packets in low txq at this stage.
		* Need to suppress or flush all packets currently in low txq
		* depending on the argument 'flag'
		*/
		wlc_low_txq_account(wlc, txq, fifo, flag, &pkt_cnt, TRUE);

		if ((TXPKTPENDGET(wlc, fifo) > 0) && (pkt_cnt > 0)) {
			/* The pkt_cnt is the weighted pkt count for TXPKTPEND. Normally this
			 * is just a count of pkts, but for SW AMPDU, an entire AMPDU of pkts
			 * is a weighted value, txpkt_weight, typically 2.
			 */

			wlc_txfifo_complete(wlc, NULL, fifo, pkt_cnt);
		}

		if (spktq_n_pkts(&txq_swq->spktq) > 0) {
			spktq_enq_list(&pkt_list, &txq_swq->spktq);
			hwa_pktpgr_txfifo_shadow_push_to_host(WL_HWA_DEVP(wlc),
				WLC_HW_MAP_TXFIFO(wlc, fifo), &pkt_list, &txq_swq->hpktq);
		}
	}

	/* Delete swq_node from current list */
	dll_delete(&txq_swq->node);

	if (spktq_n_pkts(&temp_q) > 0) {
		spktq_enq_list(&txq_swq->spktq, &temp_q);
	}

	if ((spktq_n_pkts(&txq_swq->spktq) > 0) || (hpktq_n_pkts(txq_swq) > 0)) {
		/* Move swq_node to active list */
		dll_append(&txq->active, &txq_swq->node);
	} else {
		/* Move swq_node to empty list */
		dll_append(&txq->empty, &txq_swq->node);
	}

	spktq_deinit(&pkt_list);
	spktq_deinit(&temp_q);
}

static void
wlc_hwa_tx_fifo_sync_hpktq(wlc_info_t *wlc, txq_t *txq, uint fifo, uint8 flag, bool is_active_q)
{
	struct spktq pkt_list;	/**< single priority packet queue */
	struct spktq temp_q;	/**< single priority packet queue */
	txq_swq_t *txq_swq;	/**< a non-priority queue backing one d11 FIFO */
	hwa_pp_hdbm_pktq_t *hpq;
	hwa_pp_hdbm_pktq_t temp_hpq;
	uint16 pkt_cnt;
	bool empty;

	spktq_init(&pkt_list, PKTQ_LEN_MAX);
	spktq_init(&temp_q, PKTQ_LEN_MAX);
	txq_swq = TXQ_SWQ(txq, fifo);
	hpq = &txq_swq->hpktq;

	// Copy the hpktq content to the tempary one used for pull case.
	bcopy(hpq, &temp_hpq, sizeof(hwa_pp_hdbm_pktq_t));
	// Clear the hpktq content used for push case.
	bzero(hpq, sizeof(hwa_pp_hdbm_pktq_t));

	// Move the pkts in low_txq to tempary queue before push the pkts to HDBM
	if (spktq_n_pkts(&txq_swq->spktq) > 0) {
		spktq_enq_list(&temp_q, &txq_swq->spktq);
	} else {
		/* Move swq_node from empty list to active list */
		dll_delete(&txq_swq->node);
		dll_append(&txq->active, &txq_swq->node);
	}

	do {
		pkt_cnt = 0;

		empty = hwa_pktpgr_txfifo_shadow_pull_from_host(WL_HWA_DEVP(wlc),
			WLC_HW_MAP_TXFIFO(wlc, fifo), &pkt_list, &temp_hpq, TRUE);
		if (spktq_n_pkts(&pkt_list) > 0) {
			spktq_enq_list(&txq_swq->spktq, &pkt_list);

			/*
			* Account for the collective packets in low txq at this stage.
			* Need to suppress or flush all packets currently in low txq
			* depending on the argument 'flag'
			*/
			wlc_low_txq_account(wlc, txq, fifo, flag, &pkt_cnt, TRUE);

			if (is_active_q) {
				if ((TXPKTPENDGET(wlc, fifo) > 0) && (pkt_cnt > 0)) {
					/* The pkt_cnt is the weighted pkt count for TXPKTPEND.
					 * Normally this is just a count of pkts, but for SW AMPDU,
					 * an entire AMPDU of pkts is a weighted value,
					 * txpkt_weight, typically 2.
					 */
					wlc_txfifo_complete(wlc, NULL, fifo, pkt_cnt);
				}
			} else {
				__txq_swq_dec(txq_swq, fifo, pkt_cnt);
			}
		}

		if (spktq_n_pkts(&txq_swq->spktq) > 0) {
			spktq_enq_list(&pkt_list, &txq_swq->spktq);
			hwa_pktpgr_txfifo_shadow_push_to_host(WL_HWA_DEVP(wlc),
				WLC_HW_MAP_TXFIFO(wlc, fifo), &pkt_list, hpq);
		}
	} while (!empty);

	/* Delete swq_node from current list */
	dll_delete(&txq_swq->node);

	if (spktq_n_pkts(&temp_q) > 0) {
		spktq_enq_list(&txq_swq->spktq, &temp_q);
	}

	if ((spktq_n_pkts(&txq_swq->spktq) > 0) || (hpktq_n_pkts(txq_swq) > 0)) {
		/* Move swq_node to active list */
		dll_append(&txq->active, &txq_swq->node);
	} else {
		/* Move swq_node to empty list */
		dll_append(&txq->empty, &txq_swq->node);
	}

	spktq_deinit(&pkt_list);
	spktq_deinit(&temp_q);
}

void
wlc_hwa_tx_fifo_hpq_hw_fill(wlc_info_t *wlc, txq_t *txq, uint fifo)
{
	struct spktq pkt_list; /**< single priority packet queue */
	struct spktq temp_q; /**< single priority packet queue */
	txq_swq_t *txq_swq;	/**< a non-priority queue backing one d11 FIFO */
	hwa_pp_hdbm_pktq_t *hpq;
	bool empty;

	/* get the destination queue */
	txq_swq = TXQ_SWQ(txq, fifo);
	hpq = &txq_swq->hpktq;

	if (hpq->mpdu_count == 0) {
		return;
	}

	/* init a local pkt queue to shuffle pkts in this fn */
	spktq_init(&pkt_list, PKTQ_LEN_MAX);
	spktq_init(&temp_q, PKTQ_LEN_MAX);

	// Move the pkts in low_txq to tempary queue before pull pkts from HDBM
	if (spktq_n_pkts(&txq_swq->spktq) > 0) {
		spktq_enq_list(&temp_q, &txq_swq->spktq);
	} else {
		/* Move swq_node from empty list to active list */
		dll_delete(&txq_swq->node);
		dll_append(&txq->active, &txq_swq->node);
	}

	do {
		empty = hwa_pktpgr_txfifo_shadow_pull_from_host(WL_HWA_DEVP(wlc),
			WLC_HW_MAP_TXFIFO(wlc, fifo), &pkt_list, hpq, FALSE);
		if (spktq_n_pkts(&pkt_list) > 0) {
			spktq_enq_list(&txq_swq->spktq, &pkt_list);

			ASSERT(!__txq_swq_hw_stopped(txq_swq));

			/* Post packets to fifo */
			if (fifo == ULMU_TRIG_FIFO) {
				txq_hw_fill_utxd(wlc->txqi, txq);
			} else {
				txq_hw_fill(wlc->txqi, txq, fifo);
			}
		}
	} while (!empty);

	ASSERT(hpktq_n_pkts(txq_swq) == 0);

	/* Delete swq_node from current list */
	dll_delete(&txq_swq->node);

	if (spktq_n_pkts(&temp_q) > 0) {
		spktq_enq_list(&txq_swq->spktq, &temp_q);
		/* Move swq_node to active list */
		dll_append(&txq->active, &txq_swq->node);
	} else {
		/* Move swq_node to empty list */
		dll_append(&txq->empty, &txq_swq->node);
	}

	spktq_deinit(&pkt_list);
	spktq_deinit(&temp_q);
}

static void
wlc_hwa_hpktq_flush(wlc_info_t *wlc, txq_swq_t *txq_swq, uint32 fifo, uint16 *pktcnt)
{
	struct spktq pkt_list;	/**< single priority packet queue */
	bool empty;
	void *p;

	spktq_init(&pkt_list, PKTQ_LEN_MAX);

	do {
		empty = hwa_pktpgr_txfifo_shadow_pull_from_host(WL_HWA_DEVP(wlc),
			WLC_HW_MAP_TXFIFO(wlc, fifo), &pkt_list, &txq_swq->hpktq, FALSE);

		/* flush any pkts in the software queues */
		/* no need for a mutex protection! */
		/* start with the head of the list */
		while ((p = spktq_deq(&pkt_list)) != NULL) {
			/* delete this packet */
			wlc_txq_free_pkt(wlc, p, pktcnt);
		}
	} while (!empty);

	spktq_deinit(&pkt_list);
}

static void
wlc_hwa_hpktq_map_pkts(wlc_info_t *wlc, txq_swq_t *txq_swq, uint32 fifo,
	map_pkts_cb_fn cb, void *ctx)
{
	struct spktq pkt_list;	/**< single priority packet queue */
	hwa_pp_hdbm_pktq_t *hpq;
	hwa_pp_hdbm_pktq_t temp_hpq;
	void *head_pkt, *pkt;
	bool empty;

	spktq_init(&pkt_list, PKTQ_LEN_MAX);
	hpq = &txq_swq->hpktq;

	// Copy the hpktq content to the tempary one used for pull case.
	bcopy(hpq, &temp_hpq, sizeof(hwa_pp_hdbm_pktq_t));
	// Clear the hpktq content used for push case.
	bzero(hpq, sizeof(hwa_pp_hdbm_pktq_t));

	do {
		empty = hwa_pktpgr_txfifo_shadow_pull_from_host(WL_HWA_DEVP(wlc),
			WLC_HW_MAP_TXFIFO(wlc, fifo), &pkt_list, &temp_hpq, TRUE);

		head_pkt = NULL;
		while (spktq_peek(&pkt_list) != head_pkt) {
			pkt = spktq_deq(&pkt_list);
			ASSERT(pkt);
			cb(ctx, pkt);
			if (!head_pkt)
				head_pkt = pkt;
			spktq_enq(&pkt_list, pkt);
		}

		if (spktq_n_pkts(&pkt_list) > 0) {
			hwa_pktpgr_txfifo_shadow_push_to_host(WL_HWA_DEVP(wlc),
				WLC_HW_MAP_TXFIFO(wlc, fifo), &pkt_list, hpq);
		}
	} while (!empty);

	spktq_deinit(&pkt_list);
}
#endif /* HWA_PKTPGR_BUILD */

/* Flush packets on CQ for a specific scb */
void
wlc_scb_cq_flush(wlc_info_t *wlc, struct scb* scb)
{
	wlc_txq_pktq_scb_filter(wlc, WLC_GET_CQ(SCB_WLCIFP(scb)->qi), scb);
}
