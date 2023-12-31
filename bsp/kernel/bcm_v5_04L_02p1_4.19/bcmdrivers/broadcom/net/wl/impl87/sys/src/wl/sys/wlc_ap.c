/*
 * AP Module
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
 * $Id: wlc_ap.c 810739 2022-04-13 06:48:38Z $
 */

/* XXX: Define wlc_cfg.h to be the first header file included as some builds
 * get their feature flags thru this file.
 */
#include <wlc_cfg.h>

#ifndef AP
#error "AP must be defined to include this module"
#endif

#include <typedefs.h>
#include <bcmdefs.h>

#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <bcmendian.h>
#include <802.1d.h>
#include <802.11.h>
#include <802.11e.h>
#include <sbconfig.h>
#include <wlioctl.h>
#include <eapol.h>
#include <bcmwpa.h>
#include <wep.h>
#include <sbhndpio.h>
#include <sbhnddma.h>
#include <hnddma.h>
#include <bcmdevs.h>
#include <d11.h>
#include <wlc_rate.h>
#include <wlc_pub.h>
#include <wlc_keymgmt.h>
#include <wlc_bsscfg.h>
#include <wlc_mbss.h>
#include <wlc.h>
#include <wlc_dbg.h>
#include <wlc_hw.h>
#include <wlc_apps.h>
#include <wlc_scb.h>
#include <wlc_scb_ratesel.h>
#include <wlc_phy_hal.h>
#include <phy_utils_api.h>
#include <wlc_led.h>
#include <wlc_event.h>
#include <wl_export.h>
#include <wlc_stf.h>
#include <wlc_ap.h>
#include <wlc_scan.h>
#include <wlc_ampdu_cmn.h>
#include <wlc_ampdu.h>
#include <wlc_amsdu.h>
#ifdef	WLCAC
#include <wlc_cac.h>
#endif
#include <wlc_btcx.h>
#include <wlc_bmac.h>
#ifdef BCMAUTH_PSK
#include <wlc_auth.h>
#endif
#include <wlc_assoc.h>
#ifdef WLMCNX
#include <wlc_mcnx.h>
#endif
#ifdef WLP2P
#include <wlc_p2p.h>
#endif
#ifdef WLMCHAN
#include <wlc_mchan.h>
#endif
#include <wlc_lq.h>
#include <wlc_11h.h>
#include <wlc_tpc.h>
#include <wlc_csa.h>
#include <wlc_quiet.h>
#include <wlc_dfs.h>
#include <wlc_prot_g.h>
#include <wlc_prot_n.h>
#if defined(BCMWAPI_WPI) || defined(BCMWAPI_WAI)
#include <wlc_wapi.h>
#endif
#include <wlc_pcb.h>
#ifdef PSTA
#include <wlc_psta.h>
#endif
#ifdef WL11AC
#include <wlc_vht.h>
#include <wlc_txbf.h>
#endif
#include <wlc_he.h>
#if defined(WL11K) || BAND6G || defined(WL_OCE_AP)
#include <wlc_rrm.h>
#endif /* WL11K */
#ifdef MFP
#include <wlc_mfp.h>
#endif
#ifdef WLTOEHW
#include <wlc_tso.h>
#endif /* WLTOEHW */
#ifdef WL_RELMCAST
#include "wlc_relmcast.h"
#endif
#include <wlc_ie_mgmt.h>
#include <wlc_ie_mgmt_ft.h>
#include <wlc_ie_mgmt_vs.h>
#include <wlc_ie_helper.h>
#include <wlc_ie_reg.h>
#ifdef WLWNM
#include <wlc_wnm.h>
#endif
#include <wlc_stamon.h>
#ifdef WDS
#include <wlc_wds.h>
#endif
#include <wlc_ht.h>
#include <wlc_obss.h>
#include "wlc_txc.h"
#include <wlc_tx.h>
#include <wlc_macfltr.h>
#ifdef PROP_TXSTATUS
#include <wlc_ampdu.h>
#include <wlc_apps.h>
#include <wlc_wlfc.h>
#endif /* PROP_TXSTATUS */
#include <wlc_smfs.h>
#include <wlc_pspretend.h>
#include <wlc_msch.h>
#include <wlc_qoscfg.h>
#include <wlc_chanctxt.h>
#ifdef WL_MODESW
#include <wlc_modesw.h>
#endif
#ifdef WL_AIR_IQ
#include <wlc_airiq.h>
#endif /* WL_AIR_IQ */
#if defined(STA)
#include <wlc_rm.h>
#endif
#if defined(WL_OCE) && defined(WL_OCE_AP)
#include <oce.h>
#include <wlc_oce.h>
#include <wlc_scan_utils.h>
#endif /* WL_OCE && WL_OCE_AP */
#include <wlc_rspec.h>
#include <wlc_event_utils.h>
#include <wlc_rx.h>
#include <wlc_dump.h>
#include <wlc_iocv.h>
#include <phy_chanmgr_api.h>
#include <phy_calmgr_api.h>
#include <wlc_dbg.h>
#include <wlc_rpsnoa.h>
#if defined(WL_MBO) && defined(MBO_AP)
#include <wlc_mbo.h>
#endif /* WL_MBO && MBO_AP */
#ifdef WL_PROXDETECT
#include <wlc_ftm.h>
#endif
#include <wlc_ulmu.h>
#include <wlc_hw_priv.h>
#ifdef BCM_CSIMON
#include <wlc_csimon.h>
#endif
#include <wlc_macdbg.h>
#include <wlc_ratelinkmem.h>
#include <wlc_hrt.h>

/* Default pre tbtt time for non mbss case */
#define	PRE_TBTT_DEFAULT_us		2

#if defined(RXCHAIN_PWRSAVE) || defined(RADIO_PWRSAVE)

#define PWRSAVE_RXCHAIN		1
#define PWRSAVE_RADIO		2

/* Bitmap definitions for rxchain power save */
/* rxchain power save enable */
#define PWRSAVE_ENB		0x01

/**
 * Enter power save when no STA associated to AP, this flag is valid under the condition
 * PWRSAVE_ENB is on
 */
#define NONASSOC_PWRSAVE_ENB	0x02

#endif /* RXCHAIN_PWRSAVE or RADIO_PWRSAVE */

#define DFS_DEFAULT_SUBBANDS	0x00FFu
#define SCB_LONG_TIMEOUT	3600	/**< # seconds of idle time after which we proactively
					 * free an authenticated SCB
					 */
#define SCB_MARK_DEL_TIMEOUT	5	/**< # delay before freeing  marked for delete scb */
#define SCB_GRACE_ATTEMPTS	10	/**< # attempts to probe sta beyond scb_activity_time */
#define SCB_SUPPORT_GLOBAL_RCLASS	0X01
#define WLC_BW_80_160_DELAY	10	/* Delay in seconds for 160-80 BW switch check */

#define SCB_SUPPORTS_GLOBAL_RCLASS(cubby)  (cubby->flags & SCB_SUPPORT_GLOBAL_RCLASS)

#define STA_DECRYPT_ERROR_DETECTION_TIMEOUT 30
#define STA_DECRYPT_ERROR_RECOVERY_TIMEOUT  60

#define WLC_AP_TXBCN_TIMEOUT		4	/**< # seconds: interval to detect beacon loss */
#define WLC_AP_TXBCN_EDCRS_TIMEOUT	200	/**< # seconds: interval to detect beacon loss */
#define WLC_AP_EDCRS_BCN_INACT_THRESH_DEFAULT	2 /**< # seconds: interval to send event to apps */
#define WLC_AP_EDCRS_IN_DRIVER_WAIT_COUNT 2 /**< # events: change ch in-driver after num events */
#define WLC_AP_CCA_MIN_SAMPLE_MS	(TSF_TICKS_PER_MS * 9 / 10) /* 900ms cca required */
#define WLC_AP_CCA_MAX_SAMPLE_MS	(TSF_TICKS_PER_MS * 11 / 10) /* 1100ms cca maximum */
#define WLC_AP_CCA_EDCRS_DUR_THRESH_US	(US_PER_SECOND/3) /* cca EDCRS is high if above this */
#define WLC_AP_CCA_EDCRS_LOG_THRESH_US  (US_PER_SECOND/100) /* reg log EDCRS on crossing this */

/**
 * This is a generic structure for power save implementations
 * Defines parameters for packets per second threshold based power save
 */
typedef struct wlc_pwrsave {
	bool	in_power_save;		/**< whether we are in power save mode or not */
	uint8	power_save_check;	/**< Whether power save mode check need to be done */
	uint8	stas_assoc_check;	/**< check for associated STAs before going to power save */
	uint	in_power_save_counter;	/**< how many times in the power save mode */
	uint	in_power_save_secs;	/**< how many seconds in the power save mode */
	uint	quiet_time_counter;	/**< quiet time before we enter the  power save mode */
	uint	prev_pktcount;		/**< total pkt count from the previous second */
	uint	quiet_time;		/**< quiet time in the network before we go to power save */
	uint	pps_threshold;		/**< pps threshold for power save */
} wlc_pwrsave_t;

typedef struct wlc_rxchain_pwrsave {
	/* need to save rx_stbc HT capability before enter rxchain_pwrsave mode */
	uint8		ht_cap_rx_stbc;	/**< configured rx_stbc HT capability */
	uint		rxchain;	/**< configured rxchains */
	wlc_pwrsave_t	pwrsave;
} wlc_rxchain_pwrsave_t;

typedef struct wlc_radio_pwrsave {
	uint8		level;		/**< Low, Medium or High Power Savings */
	uint16		on_time;	/**< number of  TUs radio is 'on' */
	uint16		off_time;	/**< number of  TUs radio is 'off' */
	int		radio_disabled;	/**< Whether the radio needs to be disabled now */
	uint		pwrsave_state;	/**< radio pwr save state */
	int32		tbtt_skip;	/**< num of tbtt to skip */
	bool		cncl_bcn;	/**< whether to stop bcn or not in level 1 & 2. */
	struct wl_timer	*timer;		/**< timer to keep track of duty cycle */
	wlc_pwrsave_t	pwrsave;
} wlc_radio_pwrsave_t;

#define AP_D11_MAX_AID			2007
#define AP_MAX_BSSCFG_AID		WLC_MAXBSSCFG
#define AP_MAX_SCB_AID			MAXSCB
#define AP_MAX_AID_REUSE_BLOCK		16
#define AP_MAX_AID			ROUNDUP((AP_MAX_BSSCFG_AID + AP_MAX_SCB_AID + \
						AP_MAX_AID_REUSE_BLOCK + 1), NBBY)
#define AP_AIDMAPSZ			(AP_MAX_AID / NBBY)

#ifdef RXCHAIN_PWRSAVE
#define RXCHAIN_PWRSAVE_ENAB_BRCM_NONHT	1
#define RXCHAIN_PWRSAVE_ENAB_ALL	2
#endif

#ifdef RADIO_PWRSAVE
#define RADIO_PWRSAVE_LOW		0
#define RADIO_PWRSAVE_MEDIUM		1
#define RADIO_PWRSAVE_HIGH		2
#define RADIO_PWRSAVE_TIMER_LATENCY	15
#endif

#define TRANS_PWR_LOCAL_EIRP        0x0
#define TRANS_PWR_LOCAL_EIRP_PSD    0x8
#define TRANS_PWR_REG_EIRP          0x10
#define TRANS_PWR_REG_EIRP_PSD      0x18

#define  TRANS_PWR_CAT_DEFAULT       0x00
#define  TRANS_PWR_CAT_SUBORDINATE   0x40

#define AP_CLIENT_DIFF_DB                       6

#define WLC_TPE_PWR_FACTOR 2 /* Divide to get 0.5 dB unit */

typedef struct wlc_supp_chan_set {
	uint8 first_channel;			/* first channel supported */
	uint8 num_channels;			/* number of channels suuported */
} wlc_supp_chan_set_t;

struct wlc_supp_channels {
	uint8 count;				/* count of supported channels */
	uint8 chanset[(MAXCHANNEL + 7) / 8];	/* bitmap of Supported channel set */
};

/*
* The operational mode capabilities for STAs required to associate
* to the BSS.
* NOTE: The order of values is important. They must be kept in
* increasing order of capabilities, because the enums are compared
* numerically.
* That is: OMC_HE > OMC_VHT > OMC_HT > OMC_ERP > OMC_NONE.
*/
typedef enum _opmode_cap_t {

	OMC_NONE = 0,	/**< no requirements for STA to associate to BSS */

	OMC_ERP = 1,	/**< STA must advertise ERP (11g) capabilities
			  * to be allowed to associate to 2G band BSS.
			  */
	OMC_HT = 2,	/**< STA must advertise HT (11n) capabilities to
			  * be allowed to associate to the BSS.
			  */
	OMC_VHT = 3,	/**< Devices must advertise VHT (11ac) capabilities
			  * to be allowed to associate to the BSS.
			  */
	OMC_HE = 4,	/**< Devices must advertise HE (11ax) capabilities
			  * to be allowed to associate to the BSS.
			  */
	OMC_MAX
} opmode_cap_t;

#ifdef WLDEAUTH_INTRANSIT_FILTER
/* The maximum number of deauth frames that may be pending in firmware,
 * to avoid out-of-memory issues
 */
#ifndef MAX_DEAUTH
#define MAX_DEAUTH 96
#endif

#define DEAUTH_EA_HASH(ea) ((((uint8*)(ea))[5] + ((uint8*)(ea))[4] + ((uint8*)(ea))[3] +\
	((uint8*)(ea))[2] + ((uint8*)(ea))[1]) % MAX_DEAUTH)

#define DEAUTH_EA_CMP(ea1, ea2) \
	((((uint16 *)(ea1))[0] ^ ((uint16 *)(ea2))[0]) | \
	 (((uint16 *)(ea1))[1] ^ ((uint16 *)(ea2))[1]) | \
	 (((uint16 *)(ea1))[2] ^ ((uint16 *)(ea2))[2]))

/* This structure will be used to store ethernet address (destination in deauth frame) in
 * a linked list
 */
typedef struct deauth_ea {
	struct ether_addr mac;
	struct deauth_ea *next;
} deauth_ea_t;
#endif /* WLDEAUTH_INTRANSIT_FILTER */

/* bsscfg cubby */
typedef struct ap_bsscfg_cubby {
	/* operational mode capabilities required for STA association acceptance with the BSS. */
	opmode_cap_t	opmode_cap_reqd;
	bool		authresp_macfltr;	/**< enable/disable suppr. auth resp MAC filter */
	wlc_msch_req_handle_t *msch_req_hdl;
	wlc_msch_req_param_t req;
	uint32		msch_state;
	uint16		flags;
#ifdef WLDEAUTH_INTRANSIT_FILTER
	deauth_ea_t	*intransit_maclist[MAX_DEAUTH];	/**< deauth hash table */
#endif /* WLDEAUTH_INTRANSIT_FILTER */
	int8		assoc_rej_rssi_thd;
} ap_bsscfg_cubby_t;

#define AP_BSSCFG_CUBBY(__apinfo, __cfg) \
	((ap_bsscfg_cubby_t *)BSSCFG_CUBBY((__cfg), (__apinfo)->cfgh))

#define BSS_FLAG_UCAST_DISASSOC_ON_DOWN	1

#define BSS_UCAST_DISASSOC_ON_DOWN(__apinfo, __cfg) \
	(AP_BSSCFG_CUBBY((__apinfo), (__cfg))->flags & BSS_FLAG_UCAST_DISASSOC_ON_DOWN)

/* AP Channel Ctx states (msch_state) */
typedef enum {
	AP_SCHED_UNREG =     0x1,
	AP_SCHED_PRE_INIT =  0x2,
	AP_SCHED_ON_CHAN =   0x3,
	AP_SCHED_OFF_CHAN =  0x4,
	AP_SCHED_OFF_CHAN_SKIP =  0x5
} ap_sched_state_t;

static wlc_bsscfg_t* wlc_ap_iface_create(void *if_module_ctx, wl_interface_create_t *if_buf,
	wl_interface_info_t *wl_info, int32 *err);
static int32 wlc_ap_iface_remove(wlc_info_t *wlc, wlc_if_t *wlcif);

typedef struct
{
	uint32	rxframe;
	uint32	txframe;
	uint32	rxbytes;
	uint32	txbytes;
} data_snapshot_t;

/* threshold of continuous DMA pktpool buffer empty, to trigger big hammer */
#define DMA_PKTPOOL_EMPTY_CNT	5

typedef struct
{
	uint32 time_ms;					// timestamp in ms

	/* from bmac CCA stats */
	uint32 txdur;					// TX
	uint32 inbss, obss, noctg, nopkt, edcrs, wifi;	// all RX
	uint32 doze, txop;				// idle
} wlc_ap_cca_t;

/** Private AP data structure */
typedef struct
{
	struct wlc_ap_info	appub;		/**< Public AP interface: MUST BE FIRST */
	wlc_info_t	*wlc;
	wlc_pub_t	*pub;
	uint32		maxassoc;		/**< Max # associations to allow */
	wlc_radio_pwrsave_t radio_pwrsave;	/**< radio duty cycle power save structure */
	uint16		txbcn_inactivity;	/**< txbcn inactivity counter */
	uint16		txbcn_snapshot;		/**< snapshot of txbcnfrm register */
	uint16		txbcn_timeout;		/**< txbcn inactivity timeout */
	uint16		txbcn_edcrs_timeout;	/**< txbcn inactivity timeout with edcrs */
	uint16		pre_tbtt_us;		/**< Current pre-TBTT us value */
	int		cfgh;			/**< ap bsscfg cubby handle */
	bool		ap_sta_onradar;		/**< local AP and STA are on overlapping
						 * radar channel?
						 */
	int		as_scb_handle;		/**< scb cubby handle for assocation handling */
	int		ap_scb_handle;		/**< scb cubby handle */

	wlc_rxchain_pwrsave_t rxchain_pwrsave;	/**< rxchain reduction power save structure */
	/** force beacon ratespec (in unit of 500kbps) initiated by the wl utility */
	ratespec_t	user_forced_bcn_rspec;
	/** force beacon ratespec (in unit of 500kbps) initiated by OCE */
	ratespec_t	oce_forced_bcn_rspec;
	uint		scb_timeout;		/**< inactivity timeout for associated STAs */
	uint8		scb_mark_del_timeout;	/**< delay before freeing marked for delete scb */
	uint		scb_activity_time;	/**< skip probe if activity during this time */
	uint		scb_max_probe;		/**< max number of probes to be conducted */
	bool		reprobe_scb;		/**< to let watchdog know there are scbs to probe */

	chanspec_t	chanspec_selected;	/**< chan# selected by WLC_START_CHANNEL_SEL */

	int		scb_supp_chan_handle;	/**< To get list of sta supported channels */
	uint32		recheck_160_80;		/**< relative sec to pub.now to recheck 160/80 */
	data_snapshot_t	snapshot;		/**< snapshot for interface traffic */
	uint16		dma_pktpool_empty_cnt;	/**< cnt of continuous DMA pktpool empty */
	uint8		aidmap[AP_AIDMAPSZ];	/**< aid map */
	uint16		aidblock[AP_MAX_AID_REUSE_BLOCK]; /** AIDs which are blocked for re-use */
	uint16		aidblock_idx;		/**< write index in aidblock array */
	uint16          max_bss_count;
	bool            block_nonmbssid;
#ifdef WLDEAUTH_INTRANSIT_FILTER
	uint32		intransit_inuse;		/**< number of intransit deauth frames */
	deauth_ea_t	deauth_intransit_pool[MAX_DEAUTH];	/**< pool of deauth_ea objects */
	deauth_ea_t	*deauth_intransit_freelist;	/**< list of available deauth_ea objects */
#endif /* WLDEAUTH_INTRANSIT_FILTER */
	wl_reg_info_t	reg_info;		/* regulatory info details */
	int8		edcrs_hi_event_mode;   /* user mode for EDCRS_HI events */
	int8		edcrs_hi_event_status; /* status of EDCRS_HI event generation */
	uint8		edcrs_hi_sim_secs;	/* number of seconds to simulate edcrs_hi */

	uint8		edcrs_txbcn_inact_count;
	uint8		edcrs_txbcn_inact_thresh;
	uint8		edcrs_hi_on_ch_count;
	uint8		edcrs_hi_in_driver_wait_thresh;
	chanspec_t	edcrs_txbcn_inact_chspec;
	wlc_ap_cca_t	cca;
	uint32		edcrs_dur_thresh_us;
} wlc_ap_info_pvt_t;

/* assoc scb cubby */
typedef struct wlc_assoc_req
{
	wlc_bsscfg_t	*bsscfg;
	struct dot11_management_header *hdr;
	uint8		*body;
	uint		body_len;
	uint		buf_len;
	struct scb	*scb;
	void		*e_data;
	int		e_datalen;
	wlc_rateset_t	req_rates;
	bool		reassoc;
	bool		short_preamble;
	uint16		listen;
	uint16		status;
} wlc_assoc_req_t;

#define AS_SCB_CUBBY_LOC(appvt, scb) (wlc_assoc_req_t **)SCB_CUBBY(scb, (appvt)->as_scb_handle)
#define AS_SCB_CUBBY(appvt, scb) *AS_SCB_CUBBY_LOC(appvt, scb)

/* general scb cubby */
typedef struct {
	uint8	*wpaie;		/**< WPA IE */
	uint16	wpaie_len;	/**< Length of wpaie */
	uint16	grace_attempts;	/**< Additional attempts beyond scb_timeout before scb is removed */
	uint16	psp_attempts;	/**< Counter, tracking PSPretend probing */
	uint8	*challenge;	/**< pointer to shared key challenge info element */
	struct scb *psta_prim;	/**< pointer to primary proxy sta */
	uint32	rx_decrypt_succeeds_prev;	/* saved snapshot */
	uint32	rx_decrypt_failures_prev;	/* saved snapshot */
	uint32	time_in_decrypt_failure_state;	/* monitor sta error state */
	uint8	flags; /* bitwise properties e.g. can communicate in global reg class */
	uint8	bands;				/* Frequency Bands supported by STA */
} ap_scb_cubby_t;

#define AP_SCB_CUBBY_LOC(appvt, scb) (ap_scb_cubby_t **)SCB_CUBBY(scb, (appvt)->ap_scb_handle)
#define AP_SCB_CUBBY(appvt, scb) *AP_SCB_CUBBY_LOC(appvt, scb)

#define SCB_SUPP_CHAN_CUBBY(appvt, scb) \
	(wlc_supp_channels_t*)SCB_CUBBY((scb), (appvt)->scb_supp_chan_handle)

/* IOVar table */

/* Parameter IDs, for use only internally to wlc -- in the wlc_ap_iovars
 * table and by the wlc_ap_doiovar() function.  No ordering is imposed:
 * the table is keyed by name, and the function uses a switch.
 */
enum wlc_ap_iov {
	IOV_AP_ISOLATE = 1,
	IOV_SCB_ACTIVITY_TIME = 2,
	IOV_AUTHE_STA_LIST = 3,
	IOV_AUTHO_STA_LIST = 4,
	IOV_WME_STA_LIST = 5,
	IOV_BSS = 6,
	IOV_MAXASSOC = 7,
	IOV_BSS_MAXASSOC = 8,
	IOV_CLOSEDNET = 9,
	IOV_AP = 10,
	IOV_APSTA = 11,				/**< enable simultaneously active AP/STA */
	IOV_AP_ASSERT = 12,			/**< User forced crash */
	IOV_RXCHAIN_PWRSAVE_ENABLE = 13,	/**< Power Save with single rxchain enable */
	IOV_RXCHAIN_PWRSAVE_QUIET_TIME = 14,	/**< Power Save with single rxchain quiet time */
	IOV_RXCHAIN_PWRSAVE_PPS = 15,		/**< single rxchain packets per second */
	IOV_RXCHAIN_PWRSAVE = 16,		/**< Current power save mode */
	IOV_RXCHAIN_PWRSAVE_STAS_ASSOC_CHECK = 17,	/**< Whether to check for associated stas */
	IOV_RADIO_PWRSAVE_ENABLE = 18,		/**< Radio duty cycle Power Save enable */
	IOV_RADIO_PWRSAVE_QUIET_TIME = 19,	/**< Radio duty cycle Power Save */
	IOV_RADIO_PWRSAVE_PPS = 20,		/**< Radio power save packets per second */
	IOV_RADIO_PWRSAVE = 21,			/**< Whether currently in power save or not */
	IOV_RADIO_PWRSAVE_LEVEL = 22,		/**< Radio power save duty cycle on time */
	IOV_RADIO_PWRSAVE_STAS_ASSOC_CHECK = 23,	/**< Whether to check for associated stas */
	IOV_AP_RESET = 24,	/**< User forced reset */
	IOV_BCMDCS = 25,	/**< dynamic channel switch (management) */
	IOV_DYNBCN = 26,	/**< Dynamic beaconing */
	IOV_SCB_LASTUSED = 27,	/**< time (s) elapsed since any of the associated scb is used */
	IOV_SCB_PROBE = 28,	/**< get/set scb probe parameters */
	IOV_SCB_ASSOCED = 29,	/**< if it has associated SCBs at phy if level */
	IOV_ACS_UPDATE = 30,		/**< update after acs_scan and chanspec selection */
	IOV_PROBE_RESP_INFO = 31,	/**< get probe response management packet */
	IOV_MODE_REQD = 32,	/*
				 * operational mode capabilities required for STA
				 * association acceptance with the BSS
				 */
	IOV_BSS_RATESET = 33,		/**< Set rateset per BSS */
	IOV_FORCE_BCN_RSPEC = 34, /**< Setup Beacon rate from lowest basic to specific basic rate */
	IOV_AUTHRESP_MACFLTR = 36,	/**< enable/disable suppressing auth resp by MAC filter */
	IOV_PROXY_ARP_ADVERTISE = 37, /**< Update beacon, probe response frames for proxy arp bit */
	IOV_WOWL_PKT = 38,		/**< Generate a wakeup packet */
	IOV_SET_RADAR = 39,		/**< Set DFS radar detection. */
	IOV_MCAST_FILTER_NOSTA = 40,	/**< drop mcast frames on BSS if no STAs associated */
	IOV_CEVENT = 41,		/**< enable/disable cevent levels */
	IOV_STA_SCB_TIMEOUT = 42,	/**< SCB timeout per STA */
	IOV_ODAP_ACK_TIMEOUT = 43,	/**< outdoor AP ack timeout */
	IOV_ODAP_SLOT_TIME = 44,	/**< outdoor AP slot time */
	IOV_TRF_THOLD = 45,		/**< set thresholds for traffic failure */
	IOV_MAP = 46,			/**< Set/Unset multiap. */
	IOV_FAR_STA_RSSI = 47,		/**< Setting far_sta rssi. */
	IOV_KEEP_AP_UP = 48,	/**< Set/Unset AP can up if bsscfg->disable_ap_up is TRUE */
	IOV_BW_SWITCH_160 = 49,		/* 160-80 BW Switch. 0-Disabled, 1-TDCS WAR, 2-BW Switch */
	IOV_SCB_MARK_DEL_TIMEOUT = 50,	/**< delay before freeing marked for delete  scb */
	IOV_BCNPRS_TXPWR_OFFSET = 51,	/**< Specify additional txpwr backoff for bcn/prs in dB. */
	IOV_TXBCN_TIMEOUT = 52, /**< interval to trigger tx beacon loss */
	IOV_SCB_IDLE_POLL_THRESH = 53,  /**< Send BlockAckRequest to idle SCBs */
	IOV_TXBCN_EDCRS_TIMEOUT = 54,
	IOV_MAP_PROFILE = 55,		/**< get/set multiap profile */
	IOV_MAP_8021Q_SETTINGS = 56,	/**< get/set multiap default 802.1Q Settings */
	IOV_RNR_UPDATE_METHOD = 57,	/** < method to create reduced neighbor report */
	IOV_FD_PRB_RSP_PERIOD = 58,
	IOV_UCAST_DISASSOC_ON_BSS_DOWN = 59,	/**< Disassoc STAs when bss is going down */
	IOV_BLOCK_NONMBSSID = 60,
	IOV_UPR_FD_SW = 61,		/** < UPR/FD from software or ucode */
	IOV_IAPP = 62,			/**< get/set 802.11f IAPP support */
	IOV_REG_INFO = 63,
	IOV_OVERRIDE_CLM_TPE = 64,
	IOV_RNR_TBTT_LEN = 65,
	IOV_TRANSMIT_BSS = 66,
	IOV_CHECK_SHORT_SSID = 67,
	IOV_ASSOC_REJECT_RSSI_TH = 68,
	IOV_EDCRS_HI_EVENT_MODE = 69,
	IOV_EDCRS_HI_EVENT_STATUS = 70,
	IOV_EDCRS_TXBCN_INACT_THRESH = 71,
	IOV_EDCRS_HI_SIMULATE = 72, /* for extensible EDCRS_HI simulation */
	IOV_EDCRS_DUR_THRESH_US = 73,
	IOV_BEACON_LEN = 74,
	IOV_LAST,		/**< In case of a need to check max ID number */
};

/** AP IO Vars */
static const bcm_iovar_t wlc_ap_iovars[] = {
	{"ap", IOV_AP,
	0, 0, IOVT_INT32, 0
	},
	{"ap_isolate", IOV_AP_ISOLATE,
	(0), 0, IOVT_BOOL, 0
	},
	{"scb_activity_time", IOV_SCB_ACTIVITY_TIME,
	(IOVF_NTRL), 0, IOVT_UINT32, 0
	},
	{"authe_sta_list", IOV_AUTHE_STA_LIST,
	(IOVF_SET_UP), 0, IOVT_BUFFER, sizeof(uint32)
	},
	{"autho_sta_list", IOV_AUTHO_STA_LIST,
	(IOVF_SET_UP), 0, IOVT_BUFFER, sizeof(uint32)
	},
	{"wme_sta_list", IOV_WME_STA_LIST,
	(0), 0, IOVT_BUFFER, sizeof(uint32)
	},
	{"maxassoc", IOV_MAXASSOC,
	(IOVF_WHL), 0, IOVT_UINT32, 0
	},
	{"bss_maxassoc", IOV_BSS_MAXASSOC,
	(IOVF_NTRL), 0, IOVT_UINT32, 0
	},
	{"bss", IOV_BSS,
	(0), 0, IOVT_INT32, 0
	},
	{"closednet", IOV_CLOSEDNET,
	(0), 0, IOVT_BOOL, 0
	},
#ifdef RXCHAIN_PWRSAVE
	{"rxchain_pwrsave_enable", IOV_RXCHAIN_PWRSAVE_ENABLE,
	(0), 0, IOVT_UINT8, 0
	},
	{"rxchain_pwrsave_quiet_time", IOV_RXCHAIN_PWRSAVE_QUIET_TIME,
	(0), 0, IOVT_UINT32, 0
	},
	{"rxchain_pwrsave_pps", IOV_RXCHAIN_PWRSAVE_PPS,
	(0), 0, IOVT_UINT32, 0
	},
	{"rxchain_pwrsave", IOV_RXCHAIN_PWRSAVE,
	(0), 0, IOVT_UINT8, 0
	},
	{"rxchain_pwrsave_stas_assoc_check", IOV_RXCHAIN_PWRSAVE_STAS_ASSOC_CHECK,
	(0), 0, IOVT_UINT8, 0
	},
#endif /* RXCHAIN_PWRSAVE */
#ifdef RADIO_PWRSAVE
	{"radio_pwrsave_enable", IOV_RADIO_PWRSAVE_ENABLE,
	(0), 0, IOVT_UINT8, 0
	},
	{"radio_pwrsave_quiet_time", IOV_RADIO_PWRSAVE_QUIET_TIME,
	(0), 0, IOVT_UINT32, 0
	},
	{"radio_pwrsave_pps", IOV_RADIO_PWRSAVE_PPS,
	(0), 0, IOVT_UINT32, 0
	},
	{"radio_pwrsave_level", IOV_RADIO_PWRSAVE_LEVEL,
	(0), 0, IOVT_UINT8, 0
	},
	{"radio_pwrsave", IOV_RADIO_PWRSAVE,
	(0), 0, IOVT_UINT8, 0
	},
	{"radio_pwrsave_stas_assoc_check", IOV_RADIO_PWRSAVE_STAS_ASSOC_CHECK,
	(0), 0, IOVT_UINT8, 0
	},
#endif /* RADIO_PWRSAVE */
#if defined(STA) /* APSTA */
	{"apsta", IOV_APSTA,
	(IOVF_SET_DOWN), 0, IOVT_BOOL, 0,
	},
#endif /* APSTA */
#ifdef BCM_DCS
	{"bcm_dcs", IOV_BCMDCS,
	(0), 0, IOVT_BOOL, 0
	},
#endif /* BCM_DCS */
	{"dynbcn", IOV_DYNBCN,
	(0), 0, IOVT_BOOL, 0
	},
	{"scb_lastused", IOV_SCB_LASTUSED, (0), 0, IOVT_UINT32, 0},
	{"scb_probe", IOV_SCB_PROBE,
	(IOVF_SET_UP), 0, IOVT_BUFFER, sizeof(wl_scb_probe_t)
	},
	{"scb_assoced", IOV_SCB_ASSOCED, (0), 0, IOVT_BOOL, 0},
	{"acs_update", IOV_ACS_UPDATE,
	(IOVF_SET_UP), 0, IOVT_BOOL, 0
	},
	{"probe_resp_info", IOV_PROBE_RESP_INFO,
	(IOVF_OPEN_ALLOW), 0, IOVT_BUFFER, sizeof(uint32),
	},
	{"mode_reqd", IOV_MODE_REQD,
	(IOVF_OPEN_ALLOW), 0, IOVT_UINT8, 0
	},
	{"bss_rateset", IOV_BSS_RATESET,
	(0), 0, IOVT_INT32, 0
	},
	{"force_bcn_rspec", IOV_FORCE_BCN_RSPEC,
	IOVF_SET_DOWN, 0, IOVT_UINT32, 0
	},
#ifdef WLAUTHRESP_MAC_FILTER
	{"authresp_mac_filter", IOV_AUTHRESP_MACFLTR,
	(0), 0, IOVT_BOOL, 0
	},
#endif /* WLAUTHRESP_MAC_FILTER */
	{"proxy_arp_advertise", IOV_PROXY_ARP_ADVERTISE,
	(0), 0, IOVT_BOOL, 0
	},
	{"wowl_pkt", IOV_WOWL_PKT, 0, 0, IOVT_BUFFER, 0},
	{"radar", IOV_SET_RADAR,
	(0), 0, IOVT_BOOL, 0
	},
#ifdef WL_MCAST_FILTER_NOSTA
	{"mcast_filter_nosta", IOV_MCAST_FILTER_NOSTA,
	(0), 0, IOVT_BOOL, 0
	},
#endif
#ifdef BCM_CEVENT
	{"cevent", IOV_CEVENT, 0, 0,  IOVT_UINT32, 0},
#endif /* BCM_CEVENT */
#ifdef WL_TRAFFIC_THRESH
	{"traffic_thresh", IOV_TRF_THOLD, 0, 0, IOVT_BUFFER, sizeof(wl_traffic_thresh_t)},
#endif /* WL_TRAFFIC_THRESH */
#ifdef MULTIAP
	{"map", IOV_MAP, (IOVF_SET_DOWN), 0, IOVT_UINT8, 0},
	{"map_profile", IOV_MAP_PROFILE, (IOVF_SET_DOWN), 0, IOVT_UINT8, 0},
	{"map_8021q_settings", IOV_MAP_8021Q_SETTINGS, 0, 0, IOVT_UINT16, 0},
#endif	/* MULTIAP */
	{"far_sta_rssi", IOV_FAR_STA_RSSI, 0, 0, IOVT_INT8, 0},
	{"keep_ap_up", IOV_KEEP_AP_UP, 0, 0, IOVT_UINT8, 0},
	{"bw_switch_160", IOV_BW_SWITCH_160, (IOVF_SET_DOWN), 0, IOVT_UINT8, 0},
	{"scb_markdel_time", IOV_SCB_MARK_DEL_TIMEOUT, 0, 0,  IOVT_UINT8, 0},
	{"bcnprs_txpwr_offset", IOV_BCNPRS_TXPWR_OFFSET, 0, 0, IOVT_UINT8, 0},
	{"txbcn_timeout", IOV_TXBCN_TIMEOUT, 0, 0, IOVT_UINT32, 0},
	{"txbcn_edcrs_timeout", IOV_TXBCN_EDCRS_TIMEOUT, 0, 0, IOVT_UINT32, 0},
	{"rnr_update_method", IOV_RNR_UPDATE_METHOD, 0, 0, IOVT_UINT8, 0},
	{"fd_prb_rsp_period", IOV_FD_PRB_RSP_PERIOD, 0, 0, IOVT_UINT8, 0},
	{"ucast_disassoc_on_bss_down", IOV_UCAST_DISASSOC_ON_BSS_DOWN, 0, 0, IOVT_BOOL, 0},
	{"block_nonmbssid", IOV_BLOCK_NONMBSSID, (IOVF_SET_DOWN), 0, IOVT_BOOL, 0},
	{"upr_fd_sw", IOV_UPR_FD_SW, (0), 0, IOVT_UINT8, 0},
	{"reg_info", IOV_REG_INFO, 0, 0, IOVT_BUFFER, sizeof(wl_reg_info_t)},
#ifdef WL_IAPP
	{"iapp", IOV_IAPP, (IOVF_SET_DOWN), 0, IOVT_BOOL, 0},
#endif /* WL_IAPP */
	{"override_clm_tpe", IOV_OVERRIDE_CLM_TPE, (IOVF_SET_DOWN), 0, IOVT_UINT32, 0},
	{"rnr_tbtt_len", IOV_RNR_TBTT_LEN, 0, 0, IOVT_UINT8, 0},
#ifdef WL_MBSSID
	{"transmit_bss", IOV_TRANSMIT_BSS, 0, 0, IOVT_BOOL, 0},
#endif /* WL_MBSSID */
	{"check_short_ssid", IOV_CHECK_SHORT_SSID, 0, 0, IOVT_UINT8, 0},
	{"assoc_rssi_th", IOV_ASSOC_REJECT_RSSI_TH, 0, 0, IOVT_INT8, 0},
	{"edcrs_hi_event_mode", IOV_EDCRS_HI_EVENT_MODE, 0, 0, IOVT_INT8, 0},
	{"edcrs_hi_event_status", IOV_EDCRS_HI_EVENT_STATUS, 0, 0, IOVT_INT8, 0},
	{"edcrs_txbcn_inact_thresh", IOV_EDCRS_TXBCN_INACT_THRESH, 0, 0, IOVT_UINT8, 0},
	{"edcrs_hi_simulate", IOV_EDCRS_HI_SIMULATE, 0, 0, IOVT_INT32, 0},
	{"edcrs_dur_thresh_us", IOV_EDCRS_DUR_THRESH_US, 0, 0, IOVT_UINT32, 0},
#ifdef WL_MBSSID
	{"beacon_len", IOV_BEACON_LEN, (IOVF_GET_DOWN), 0, IOVT_UINT32, 0},
#endif
	{NULL, 0, 0, 0, 0, 0 }
};

/**
 * To read specific CCA stat.
 * Call with wlc->hw and TYPE may be EDCRSDUR, INBSS, OBSS< WIFI, TXOP, OBSS, ...
 * See M_CCA_EDCRSDUR_L, M_CCA_WIFI_L, ... for example.
 */
#define CCA_READ(WLC_HW, TYPE) wlc_bmac_read_counter(WLC_HW, 0, \
		M_CCA_ ## TYPE ## _L(WLC_HW), M_CCA_ ## TYPE ## _H(WLC_HW))

/* similarly for MAC variants. eg. M_MAC_SLPDUR_L or H */
#define MAC_READ(WLC_HW, TYPE) wlc_bmac_read_counter(WLC_HW, 0, \
		M_MAC_ ## TYPE ## _L(WLC_HW), M_MAC_ ## TYPE ## _H(WLC_HW))

#define SLOTTIME(WLC) (((WLC)->band->gmode && !(WLC)->shortslot) ? BPHY_SLOT_TIME : APHY_SLOT_TIME)

/* Local Prototypes */
static void wlc_ap_watchdog(void *arg);
static void wlc_ap_get_cca(wlc_info_t *wlc, wlc_ap_cca_t *cca);
static bool wlc_ap_watchdog_infer_incumbent(wlc_ap_info_pvt_t *appvt);
static void wlc_ap_consider_indriver_channel_change(wlc_ap_info_pvt_t *appvt);
static void wlc_ap_watchdog_beacon_inactivity_check(wlc_ap_info_pvt_t *appvt);
static int wlc_ap_wlc_up(void *ctx);
static int wlc_ap_wlc_down(void *ctx);
static void wlc_assoc_notify(wlc_ap_info_t *ap, struct ether_addr *sta, wlc_bsscfg_t *cfg);
static void wlc_reassoc_notify(wlc_ap_info_t *ap, struct ether_addr *sta, wlc_bsscfg_t *cfg);
static void wlc_ap_sta_probe(wlc_ap_info_t *ap, struct scb *scb);
static void wlc_ap_sta_probe_complete(wlc_info_t *wlc, uint txstatus, struct scb *scb, void *pkt);
static void wlc_ap_stas_timeout(wlc_ap_info_t *ap);
static void wlc_ap_wsec_stas_timeout(wlc_ap_info_t *ap);
static void wlc_ap_scb_unusable_stas_timeout(wlc_ap_info_t *ap);
static int wlc_authenticated_sta_check_cb(struct scb *scb);
static int wlc_authorized_sta_check_cb(struct scb *scb);
static int wlc_wme_sta_check_cb(struct scb *scb);
static int wlc_sta_list_get(wlc_ap_info_t *ap, wlc_bsscfg_t *cfg, uint8 *buf,
                            int len, int (*sta_check)(struct scb *scb));
#ifdef BCMDBG
static int wlc_dump_ap(wlc_ap_info_t *ap, struct bcmstrbuf *b);
#ifdef RXCHAIN_PWRSAVE
static int wlc_dump_rxchain_pwrsave(wlc_ap_info_t *ap, struct bcmstrbuf *b);
#endif
#ifdef RADIO_PWRSAVE
static int wlc_dump_radio_pwrsave(wlc_ap_info_t *ap, struct bcmstrbuf *b);
#endif
#if defined(RXCHAIN_PWRSAVE) || defined(RADIO_PWRSAVE)
static void wlc_dump_pwrsave(wlc_pwrsave_t *pwrsave, struct bcmstrbuf *b);
#endif
#endif /* BCMDBG */

#ifdef WLDEAUTH_INTRANSIT_FILTER
static void wlc_ap_intransit_filter_clear_bss(wlc_ap_info_pvt_t *appvt, wlc_bsscfg_t *cfg);
#endif /* WLDEAUTH_INTRANSIT_FILTER */

static int wlc_ap_doiovar(void *hdl, uint32 actionid,
	void *params, uint p_len, void *arg, uint len, uint val_size, struct wlc_if *wlcif);
static int wlc_ap_ioctl(void *hdl, uint cmd, void *arg, uint len, struct wlc_if *wlcif);

static void wlc_ap_up_upd(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg, bool state);

static void wlc_ap_acs_update(wlc_info_t *wlc);
static int wlc_ap_set_opmode_cap_reqd(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
	opmode_cap_t opmode);
static opmode_cap_t wlc_ap_get_opmode_cap_reqd(wlc_info_t* wlc, wlc_bsscfg_t *bsscfg);
static int wlc_ap_bsscfg_init(void *context, wlc_bsscfg_t *cfg);
static void wlc_ap_bsscfg_deinit(void *context, wlc_bsscfg_t *cfg);
#if defined(BCMDBG)
static void wlc_ap_bsscfg_dump(void *ctx, wlc_bsscfg_t *cfg, struct bcmstrbuf *b);
#else
#define wlc_ap_bsscfg_dump NULL
#endif
#if defined(RXCHAIN_PWRSAVE) || defined(RADIO_PWRSAVE)
#ifdef RXCHAIN_PWRSAVE
static void wlc_disable_pwrsave(wlc_ap_info_t *ap, int type);
#ifdef WDS
static bool wlc_rxchain_wds_detection(wlc_info_t *wlc);
#endif /* WDS */
#endif /* RXCHAIN_PWRSAVE */
static void wlc_reset_pwrsave_mode(wlc_ap_info_t *ap, int type);
static void wlc_pwrsave_mode_check(wlc_ap_info_t *ap, int type);
#endif /* RXCHAIN_PWRSAVE || RADIO_PWRSAVE */
#ifdef RADIO_PWRSAVE
static void wlc_radio_pwrsave_update_quiet_ie(wlc_ap_info_t *ap, uint8 count);
static void wlc_reset_radio_pwrsave_mode(wlc_ap_info_t *ap);
static void wlc_radio_pwrsave_off_time_start(wlc_ap_info_t *ap);
static void wlc_radio_pwrsave_timer(void *arg);
#endif /* RADIO_PWRSAVE */

static int wlc_as_scb_init(void *context, struct scb *scb);
static void wlc_as_scb_deinit(void *context, struct scb *scb);
static uint wlc_as_scb_secsz(void *context, struct scb *scb);

static void wlc_ap_probe_complete(wlc_info_t *wlc, void *pkt, uint txs);

static int wlc_ap_scb_init(void *context, struct scb *scb);
static void wlc_ap_scb_deinit(void *context, struct scb *scb);
static uint wlc_ap_scb_secsz(void *context, struct scb *scb);

/* IE mgmt */
static uint wlc_auth_calc_chlng_ie_len(void *ctx, wlc_iem_calc_data_t *data);
static int wlc_auth_write_chlng_ie(void *ctx, wlc_iem_build_data_t *data);
static uint wlc_assoc_calc_sup_rates_ie_len(void *ctx, wlc_iem_calc_data_t *data);
static int wlc_assoc_write_sup_rates_ie(void *ctx, wlc_iem_build_data_t *data);
static uint wlc_assoc_calc_ext_rates_ie_len(void *ctx, wlc_iem_calc_data_t *data);
static int wlc_assoc_write_ext_rates_ie(void *ctx, wlc_iem_build_data_t *data);
static int wlc_auth_parse_chlng_ie(void *ctx, wlc_iem_parse_data_t *data);
static int wlc_assoc_parse_ssid_ie(void *ctx, wlc_iem_parse_data_t *data);
static int wlc_assoc_parse_sup_rates_ie(void *ctx, wlc_iem_parse_data_t *data);
static int wlc_assoc_parse_ext_rates_ie(void *ctx, wlc_iem_parse_data_t *data);
static int wlc_assoc_parse_supp_chan_ie(void *ctx, wlc_iem_parse_data_t *data);
static int wlc_assoc_parse_wps_ie(void *ctx, wlc_iem_parse_data_t *data);
static int wlc_assoc_parse_psta_ie(void *ctx, wlc_iem_parse_data_t *data);

#ifdef WLDFS
static bool wlc_ap_on_radarchan(wlc_ap_info_t *ap);
#endif
static int wlc_user_forced_bcn_rspec_upd(wlc_info_t *wlc, wlc_bsscfg_t *cfg, wlc_ap_info_t *ap,
	uint32 value);

static void wlc_ap_auth_tx_complete(wlc_info_t *wlc, uint txstatus, void *arg);

static bool wlc_ap_assreq_verify_rates(wlc_assoc_req_t *param, wlc_iem_ft_pparm_t *ftpparm,
	opmode_cap_t opmode_cap_reqd, uint16 capability, wlc_rateset_t *req_rates);
static bool wlc_ap_assreq_verify_authmode(wlc_info_t *wlc, struct scb *scb,
	wlc_bsscfg_t *bsscfg, uint16 capability, bool akm_ie_included,
	struct dot11_management_header *hdr);

static int wlc_ap_msch_clbk(void* handler_ctxt, wlc_msch_cb_info_t *cb_info);
static void wlc_ap_return_home_channel(wlc_bsscfg_t *cfg);
static void wlc_ap_prepare_off_channel(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
static void wlc_ap_off_channel_done(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
static void wlc_ap_upd_onchan(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg);

static void wlc_ap_authresp_deauth(wlc_info_t *wlc, struct scb *scb);
#ifdef WL_SAE
static uint wlc_sae_parse_auth(wlc_bsscfg_t *cfg, struct dot11_management_header *hdr,
		uint8 *body, uint body_len);
#endif /* WL_SAE */

static int wlc_assoc_parse_regclass_ie(void *ctx, wlc_iem_parse_data_t *data);
#ifdef WL_GLOBAL_RCLASS
static void wlc_ap_scb_no_gbl_rclass_upd(wlc_info_t *wlc, struct scb *scb,
	uint8 scb_assoced, uint8 oldstate);
#endif /* WL_GLOBAL_RCLASS */
static void wlc_ap_scb_state_upd_cb(void *ctx, scb_state_upd_data_t *notif_data);

#ifdef WL_MODESW
static int wlc_ap_160_80_bw_switch(wlc_ap_info_t *ap);
#endif /* WL_MODESW */
#if BAND6G || defined(WL_OCE_AP)
static uint wlc_rnr_element_calc_len(void *ctx, wlc_iem_calc_data_t *data);
static int wlc_rnr_element_build_fn(void *ctx, wlc_iem_build_data_t *data);
static uint16 wlc_write_rnr_element(wlc_bsscfg_t *bsscfg, uint8 *cp, wlc_iem_build_data_t *data);

static void wlc_ap_upr_fd_timer_cb(void *ctx);
static void wlc_ap_send_bcast_prb_rsp(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
static void wlc_ap_send_fd(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg);
#endif /* WL_WIFI_6GHZ || WL_OCE_AP */

static void wlc_ap_update_edcrs_hi_event_status(wlc_ap_info_pvt_t* appvt);
static int wlc_ap_set_edcrs_hi_event_mode(wlc_ap_info_pvt_t* appvt, int val);

static uint wlc_calc_pwr_env_ie_len(void *ctx, wlc_iem_calc_data_t *data);
static int wlc_write_pwr_env_ie(void *ctx, wlc_iem_build_data_t *data);

static void wlc_write_client_eirp(uint8 **cp, wlc_txpwr_eirp_t *eirp, uint8 pwr_interpretation,
	uint8 pwr_category, chanspec_t chspec, int buflen);
static void wlc_write_client_eirp_psd(uint8 **cp,
	wlc_txpwr_eirp_psd_t *eirp_psd, uint8 pwr_interpretation,
	uint8 pwr_category, chanspec_t chspec, int buflen);
static uint wlc_csw_calc_pwr_env_ie_len(void *ctx, wlc_iem_calc_data_t *calc);
static int wlc_csw_write_pwr_env_ie(void *ctx, wlc_iem_build_data_t *build);

void wlc_check_override_clm(wlc_info_t *wlc, wlc_txpwr_eirp_t *eirp,
	wlc_txpwr_eirp_psd_t *eirp_psd);

void wlc_set_override_clm_tpe(wlc_info_t *wlc, wlc_ioctl_tx_pwr_t *tx_pwr);
void wlc_get_override_clm_tpe(wlc_info_t *wlc, wlc_ioctl_tx_pwr_t *tx_pwr, int mode);

static uint8 *
wlc_write_transmit_power_envelope_ie(wlc_info_t *wlc,
	chanspec_t chspec, uint8 *cp, int buflen);

static void wlc_ap_bss_updown(void *ctx, bsscfg_up_down_event_data_t *evt);

#ifdef WL_TRAFFIC_THRESH
int wlc_traffic_thresh_set_val(wlc_info_t* wlc, wlc_bsscfg_t* bsscfg,
	wl_traffic_thresh_t *req);
int wlc_traffic_thresh_get_val(wlc_info_t* wlc, wlc_bsscfg_t* bsscfg,
	wl_traffic_thresh_t *req, struct get_traffic_thresh *res);

int
wlc_traffic_thresh_set_val(wlc_info_t* wlc, wlc_bsscfg_t* bsscfg,
	wl_traffic_thresh_t *req)
{
	int i;
	int err = BCME_OK;
	struct scb *scb;
	struct scb_iter scbiter;
	wlc_bsscfg_t* cfg;
	wlc_trf_data_t *ptr;
	wlc_intfer_params_t *val;
	switch (req->mode) {
	case WL_TRF_MODE_CFG_FEATURE:
	{
		if (req->enable) {
			if (!bsscfg->traffic_thresh_enab) {
				if (wlc->pub->_traffic_thresh) {
					err = wlc_bsscfg_traffic_thresh_init(wlc, bsscfg);
					if (err != BCME_OK) {
						wlc_bsscfg_traffic_thresh_deinit(wlc, bsscfg);
						return err;
					}
				}
				bsscfg->traffic_thresh_enab = 1;
			}
		} else {
			if (bsscfg->traffic_thresh_enab) {
				wlc_bsscfg_traffic_thresh_deinit(wlc, bsscfg);
				bsscfg->traffic_thresh_enab = 0;
			}
		}
	}
	break;
	case WL_TRF_MODE_WL_FEATURE:
	{
		if (req->enable) {
			if (!wlc->pub->_traffic_thresh) {
				FOREACH_BSS(wlc, i, cfg) {
					if (cfg->traffic_thresh_enab) {
						err = wlc_bsscfg_traffic_thresh_init(wlc, cfg);
						if (err != BCME_OK) {
							wlc_bsscfg_traffic_thresh_deinit(wlc,
								cfg);
							return err;
						}
					}
				}
				wlc->pub->_traffic_thresh = 1;
			}
		} else {
			if (wlc->pub->_traffic_thresh) {
				FOREACH_BSS(wlc, i, cfg) {
					if (cfg->traffic_thresh_enab) {
						wlc_bsscfg_traffic_thresh_deinit(wlc, cfg);
					}
				}
				wlc->pub->_traffic_thresh = 0;
			}
		}
	}
	break;
	case WL_TRF_MODE_AP:
	{
		if (req->type & WL_TRF_AE) {
			req->type &= 0x0f;
			if (req->enable) {
				bsscfg->trf_cfg_enable |= (1 << req->type);
			} else {
				bsscfg->trf_cfg_enable &= ~(1 << req->type);
				if (wlc->pub->_traffic_thresh && bsscfg->traffic_thresh_enab) {
					ptr = &bsscfg->trf_cfg_data[req->type];
					val = &bsscfg->trf_cfg_params[req->type];
					/* reset the data as feature is disabled */
					ptr->cur = 0;
					ptr->count = 0;
					for (i = 0; i < val->num_secs; i++) {
						bsscfg->trf_cfg_data[req->type].num_data[i] = 0;
					}
				}
			}
			break;
		}
		/* delete old num_data */
		ptr = &bsscfg->trf_cfg_data[req->type];
		val = &bsscfg->trf_cfg_params[req->type];
		if (val->num_secs == req->num_secs) {
			if ((val->thresh != req->thresh) && (req->thresh)) {
				/* reset the data as thresh is changed */
				ptr->cur = 0;
				ptr->count = 0;
				for (i = 0; i < val->num_secs; i++) {
					bsscfg->trf_cfg_data[req->type].num_data[i] = 0;
				}
				val->thresh = req->thresh;
			} else {
				err = BCME_USAGE_ERROR;
			}
			break;
		}
		/* now the memory needs to reallocated */
		if (ptr->num_data != NULL) {
			MFREE(wlc->osh, ptr->num_data,
					sizeof(uint16) * val->num_secs);
			ptr->num_data = NULL;
		}
		if (req->num_secs) {
			if (wlc->pub->_traffic_thresh && bsscfg->traffic_thresh_enab) {
				if ((ptr->num_data = MALLOCZ(wlc->osh,
						sizeof(uint16) * req->num_secs))
						== NULL) {
					WL_ERROR(("wl%d: not enough memory\n",
						wlc->pub->unit));
					err = BCME_NORESOURCE;
					break;
				}
			}
			val->thresh = req->thresh;
		} else {
			val->thresh = 0;
		}
		ptr->cur = 0;
		ptr->count = 0;
		val->num_secs = req->num_secs;
	}
	break;
	case WL_TRF_MODE_STA:
	{
		if (req->type & WL_TRF_AE) {
			req->type &= 0x0f;
			if (req->enable) {
				if (!(bsscfg->trf_scb_enable & (1 << req->type))) {
					FOREACH_BSS_SCB(wlc->scbstate,
							&scbiter, bsscfg, scb) {
						scb->trf_enable_flag |= (1 << req->type);
					}
					bsscfg->trf_scb_enable |= (1 << req->type);
				}
			} else {
				if (bsscfg->trf_scb_enable & (1 << req->type)) {
					FOREACH_BSS_SCB(wlc->scbstate,
							&scbiter, bsscfg, scb) {
						scb->trf_enable_flag &= ~(1 << req->type);
						scb->scb_trf_data[req->type].timestamp = 0;
						scb->scb_trf_data[req->type].idx = 0;
						scb->scb_trf_data[req->type].histo[0] = 0;
					}
					bsscfg->trf_scb_enable &= ~(1 << req->type);
				}
			}
			break;
		}
		/* delete old num_data */
		val = &bsscfg->trf_scb_params[req->type];
		if (val->num_secs == req->num_secs) {
			if (val->thresh == req->thresh) {
				/* nothing to change */
				err = BCME_OK;
				break;
			} else if (req->thresh) {
				/* reset the data as thresh is changed */
				val->thresh = req->thresh;
				FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
					scb->scb_trf_data[req->type].timestamp = 0;
					scb->scb_trf_data[req->type].idx = 0;
					scb->scb_trf_data[req->type].histo[0] = 0;
				}
			} else {
				err = BCME_USAGE_ERROR;
			}
			break;
		}
		if (req->num_secs) {
			val->thresh = req->thresh;
			/* reset the data as params are changed */
			FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
				scb->scb_trf_data[req->type].timestamp = 0;
				scb->scb_trf_data[req->type].idx = 0;
				scb->scb_trf_data[req->type].histo[0] = 0;
			}
		} else {
			val->thresh = 0;
		}
		if (req->num_secs >= WL_TRF_MAX_SECS) {
			val->num_secs = WL_TRF_MAX_SECS;
		} else {
			val->num_secs = req->num_secs;
		}
	}
	break;
	case WL_TRF_MODE_MAC:
	{
		FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
			if (!ether_cmp(&scb->ea, &req->mac)) {
				req->type &= 0x0f;
				if (req->enable) {
					if (!(scb->trf_enable_flag & (1 << req->type))) {
						scb->trf_enable_flag |= (1 << req->type);
					}
				} else {
					if (scb->trf_enable_flag & (1 << req->type)) {
						/* reset the data and disable flag */
						scb->scb_trf_data[req->type].timestamp = 0;
						scb->scb_trf_data[req->type].idx = 0;
						scb->scb_trf_data[req->type].histo[0] = 0;
						scb->trf_enable_flag &= ~(1 << req->type);
					}
				}
				err = BCME_OK;
				break;
			} else {
				err = BCME_NORESOURCE;
			}
		}
	}
		break;
	default:
	err = BCME_USAGE_ERROR;
	break;
}
return err;
} /* wlc_traffic_thresh_set_val */

int
wlc_traffic_thresh_get_val(wlc_info_t* wlc, wlc_bsscfg_t* bsscfg,
	wl_traffic_thresh_t *req, struct get_traffic_thresh *res)
{
	int i, err = BCME_OK;
	wl_traffic_thresh_t *data = res->data;
	wlc_intfer_params_t *ptr = NULL;
	res->intf_enab = wlc->pub->_traffic_thresh;
	res->bss_enab = bsscfg->traffic_thresh_enab;
	switch (req->mode) {
	case WL_TRF_MODE_AP:
	{
		ptr = bsscfg->trf_cfg_params;
		res->auto_enable = bsscfg->trf_cfg_enable;
		res->mode = WL_TRF_MODE_AP;
		for (i = 0; i < WL_TRF_MAX_QUEUE; i++) {
			data[i].thresh = ptr[i].thresh;
			data[i].num_secs = ptr[i].num_secs;
		}
		err = BCME_OK;
	}
	break;
	case WL_TRF_MODE_STA:
	{
		ptr = bsscfg->trf_scb_params;
		res->auto_enable = bsscfg->trf_scb_enable;
		res->mode = WL_TRF_MODE_STA;
		for (i = 0; i < WL_TRF_MAX_QUEUE; i++) {
			data[i].thresh = ptr[i].thresh;
			data[i].num_secs = ptr[i].num_secs;
		}
		err = BCME_OK;
	}
	break;
	case  WL_TRF_MODE_MAC:
	{
		struct scb *scb;
		struct scb_iter scbiter;
		res->mode = WL_TRF_MODE_MAC;
		FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
			if (!ether_cmp(&scb->ea, &req->mac)) {
				res->auto_enable = scb->trf_enable_flag;
				err =  BCME_OK;
				break;
			}
			err = BCME_NORESOURCE;
		}
	}
	break;
	default:
	{
		err = BCME_USAGE_ERROR;
		break;
	}

	}
	return err;
} /* wlc_traffic_thresh_get_val */
#endif /* WL_TRAFFIC_THRESH */

#ifdef WL_MODESW
static int
wlc_ap_160_80_bw_switch(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*)ap;
	wlc_info_t *wlc = appvt->wlc;
	wlc_bsscfg_t *bsscfg = wlc->primary_bsscfg;
	uint8 new_oper_mode = 0, curr_oper_mode, bw = 0, nss;
	uint16 num_160mhz_assocs = wlc->num_160mhz_assocs;
	bool upgrd_160 = FALSE;
	chanspec_t chspec_160;
	int idx;

	appvt->recheck_160_80 = 0;

	/* When primary interface is STA, return */
	if (BSSCFG_STA(bsscfg)) {
		return BCME_UNSUPPORTED;
	}

	if (BAND_2G(wlc->band->bandtype)) {
		return BCME_BADBAND;
	}

	if (!WLC_BW_SWITCH_ENABLED(wlc)) {
		return BCME_UNSUPPORTED;
	}

	WL_MODE_SWITCH(("wl%d:%s chanspec 0x%x num_160mhz_assocs %d, any_sta_in_160mhz: %d\n",
		wlc->pub->unit,	__FUNCTION__, wlc->chanspec, num_160mhz_assocs,
		wlc->any_sta_in_160mhz));

	if (num_160mhz_assocs && CHSPEC_IS80(wlc->chanspec)) {
		uint16 center_ch = CHSPEC_CHANNEL(wlc->chanspec);
		if (center_ch < 36 || center_ch > 128) {
			WL_MODE_SWITCH(("wl%d Bad chspec 0x%x center_ch %d\n",
					wlc->pub->unit, wlc->chanspec, center_ch));
			return BCME_OK;
		}
		chspec_160 = wf_channel2chspec(wf_chspec_ctlchan(wlc->chanspec),
				WL_CHANSPEC_BW_160, CHSPEC_BAND(wlc->chanspec));
		WL_MODE_SWITCH(("wl%d chspec_160 0x%x center_ch %d\n",
				wlc->pub->unit, chspec_160, center_ch));
		if (!WL_BW_CAP_160MHZ(wlc->band->bw_cap) ||
				!CHSPEC_IS160(chspec_160) ||
				!wlc_valid_chanspec_db(wlc->cmi, chspec_160)) {
			return BCME_OK;
		}

		if (wlc_quiet_chanspec(wlc->cmi, chspec_160)) {
			wl_event_req_bw_upgd_t evt_data;

			memset(&evt_data, 0, sizeof(evt_data));
			evt_data.version = WL_EVENT_REQ_BW_UPGD_VER_1;
			evt_data.length = WL_EVENT_REQ_BW_UPGD_LEN;
			evt_data.upgrd_chspec = chspec_160;
			/* send an event so that ACSD app can trigger BGDFS */
			wlc_bss_mac_event(wlc, bsscfg, WLC_E_REQ_BW_CHANGE, NULL, 0,
				0, 0, (void *)&evt_data, sizeof(evt_data));
			WL_MODE_SWITCH(("wl%d 160Mhz upgrade by WLC_E_REQ_BW_CHANGE\n",
				wlc->pub->unit));
			return BCME_OK;
		}
		upgrd_160 = TRUE;
	} else if (!num_160mhz_assocs && !wlc->any_sta_in_160mhz && CHSPEC_IS160(wlc->chanspec)) {
		upgrd_160 = FALSE;
	} else {
		return BCME_OK;
	}

	curr_oper_mode = wlc_modesw_derive_opermode(wlc->modesw,
			bsscfg->current_bss->chanspec, bsscfg,
			wlc->stf->op_rxstreams);
	nss = DOT11_OPER_MODE_RXNSS(curr_oper_mode);
	bw = DOT11_OPER_MODE_CHANNEL_WIDTH(curr_oper_mode);
	new_oper_mode = DOT11_D8_OPER_MODE(0, nss, 0, upgrd_160, bw);
	WL_MODE_SWITCH(("wl%d Attempting %sMhz by opmode 0x%x\n",
			WLCWLUNIT(wlc), (upgrd_160) ? "160":"80",
			new_oper_mode));
	FOREACH_UP_AP(wlc, idx, bsscfg) {
		WL_MODE_SWITCH(("cfg:%d %s modesw_entry\n",
			bsscfg->_idx, __FUNCTION__));
		wlc_modesw_handle_oper_mode_notif_request(wlc->modesw,
			bsscfg, new_oper_mode, TRUE,
			MODESW_CTRL_OPMODE_IE_REQD_OVERRIDE);
	}

	return BCME_OK;
} /* wlc_ap_160_80_bw_switch */
#endif /* WL_MODESW */

static void
wlc_ap_160mhz_upd_bw_check(wlc_info_t *wlc, struct scb *scb, uint8 scb_assoced)
{
	uint prev_160mhz_assocs;

	if (scb_assoced) {
		/* count 160 capable STAs associated to our AP (any BSS) */
		prev_160mhz_assocs = wlc->num_160mhz_assocs;
		if (scb->flags3 & (SCB3_IS_80_80 | SCB3_IS_160)) {
			wlc->num_160mhz_assocs++;
		} else {
			wlc->num_non160mhz_assocs++;
		}
	} else {
		/* count 160 capable STAs associated to our AP (any BSS) */
		prev_160mhz_assocs = wlc->num_160mhz_assocs;
		if (scb->flags3 & (SCB3_IS_80_80 | SCB3_IS_160)) {
			ASSERT(prev_160mhz_assocs > 0);
			wlc->num_160mhz_assocs--;
		} else {
			ASSERT(wlc->num_non160mhz_assocs > 0);
			wlc->num_non160mhz_assocs--;
		}
	}
	BCM_REFERENCE(prev_160mhz_assocs);

#ifdef WL_MODESW
	WL_MODE_SWITCH(("wl%d %s prev_160mhz_assocs %d 160mhz_assocs %d non160mhz_assocs %d"
			" wlc->chanspec 0x%x\n",
			wlc->pub->unit, __FUNCTION__, prev_160mhz_assocs,
			wlc->num_160mhz_assocs, wlc->num_non160mhz_assocs, wlc->chanspec));
	if (WLC_BW_SWITCH_ENABLED(wlc) && BAND_5G(wlc->band->bandtype)) {
		/* Schedule a timer for a first 160Mhz client 160/80Mhz BW switch attempt.
		 * When there are no 160Mhz clients and atleast 1 80Mhz client,
		 * switch to 80Mhz chanspec.
		 * When chanspec is 80Mhz, and client is 160Mhz capable attempt switch to 160Mhz
		 */
		if ((!prev_160mhz_assocs && wlc->num_160mhz_assocs > 0 &&
				CHSPEC_IS80(wlc->chanspec)) ||
			(!wlc->num_160mhz_assocs && wlc->num_non160mhz_assocs &&
				CHSPEC_IS160(wlc->chanspec) && !wlc->any_sta_in_160mhz)) {
			wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) wlc->ap;
			WL_MODE_SWITCH(("wl%d Schedule a BW switch during %s after a delay\n",
					WLCWLUNIT(wlc), scb_assoced ? "Assoc" : "Disassoc"));
			appvt->recheck_160_80 = wlc->pub->now + WLC_BW_80_160_DELAY;
		}
		wlc_update_phy_tdcs(wlc);
	} else
#endif /* WL_MODESW */
	{
		wlc_update_phy_tdcs(wlc);
	}
} /* wlc_ap_160mhz_upd_bw_check */

/* This includes the auto generated ROM IOCTL/IOVAR patch handler C source file (if auto patching is
 * enabled). It must be included after the prototypes and declarations above (since the generated
 * source file may reference private constants, types, variables, and functions).
 */
#include <wlc_patch.h>

wlc_ap_info_t*
BCMATTACHFN(wlc_ap_attach)(wlc_info_t *wlc)
{
	int err = 0;
	wlc_ap_info_pvt_t *appvt;
	wlc_ap_info_t* ap;
	wlc_pub_t *pub = wlc->pub;
	scb_cubby_params_t cubby_params;
	bsscfg_cubby_params_t bss_cubby_params;
	uint16 arsfstbmp = FT2BMP(FC_ASSOC_RESP) | FT2BMP(FC_REASSOC_RESP);
	uint16 arqfstbmp = FT2BMP(FC_ASSOC_REQ) | FT2BMP(FC_REASSOC_REQ);
	uint16 bcnfstbmp = FT2BMP(FC_BEACON) | FT2BMP(FC_PROBE_RESP);
#ifdef WLDEAUTH_INTRANSIT_FILTER
	deauth_ea_t *stap;
	uint i;
#endif /* WLDEAUTH_INTRANSIT_FILTER */

#if BAND6G || defined(WL_OCE_AP)
	uint16 rnr_elm_build_fstbmp = FT2BMP(FC_PROBE_RESP) | FT2BMP(FC_BEACON);
#endif /* WL_WIFI_6GHZ || WL_OCE_AP */
#ifdef RXCHAIN_PWRSAVE
	char *var;
#endif /* RXCHAIN_PWRSAVE */

#ifdef BCMDBG
	STATIC_ASSERT(AP_MAX_AID < AP_D11_MAX_AID);
#endif /* BCMDBG */

	appvt = (wlc_ap_info_pvt_t*)MALLOCZ(pub->osh, sizeof(wlc_ap_info_pvt_t));
	if (appvt == NULL) {
		WL_ERROR(("%s: MALLOC wlc_ap_info_pvt_t failed\n", __FUNCTION__));
		goto fail;
	}

	ap = &appvt->appub;
	ap->shortslot_restrict = FALSE;

	appvt->scb_timeout = SCB_TIMEOUT;
	appvt->scb_mark_del_timeout = SCB_MARK_DEL_TIMEOUT;
	appvt->scb_activity_time = SCB_ACTIVITY_TIME;
	appvt->scb_max_probe = SCB_GRACE_ATTEMPTS;

	appvt->wlc = wlc;
	appvt->pub = pub;
	appvt->maxassoc = wlc_ap_get_maxassoc_limit(ap);
	pub->_iapp = 1;

#ifndef WL_MBSSID
	appvt->max_bss_count = AP_MAX_BSSCFG_AID;
#else
	if ((appvt->max_bss_count = next_larger_power2(AP_MAX_BSSCFG_AID)) > AP_D11_MAX_AID) {
		appvt->max_bss_count = (next_larger_power2(AP_D11_MAX_AID) >> 1);

	}
#endif /* WL_MBSSID */

#ifdef WLDEAUTH_INTRANSIT_FILTER
	for (i = 0; i < MAX_DEAUTH; i++) {
		stap = &appvt->deauth_intransit_pool[i];
		stap->next = appvt->deauth_intransit_freelist;
		appvt->deauth_intransit_freelist = stap;
	}
#endif /* WLDEAUTH_INTRANSIT_FILTER */

	/* reserve cubby in the bsscfg container for per-bsscfg private data */
	bzero(&bss_cubby_params, sizeof(bss_cubby_params));
	bss_cubby_params.context = appvt;
	bss_cubby_params.fn_init = wlc_ap_bsscfg_init;
	bss_cubby_params.fn_deinit = wlc_ap_bsscfg_deinit;
	bss_cubby_params.fn_dump = wlc_ap_bsscfg_dump;

	/* reserve cubby in the bsscfg container for per-bsscfg private data */
	appvt->cfgh = wlc_bsscfg_cubby_reserve_ext(wlc, sizeof(ap_bsscfg_cubby_t),
		&bss_cubby_params);

	if (appvt->cfgh < 0) {
		WL_ERROR(("wl%d: %s: wlc_bsscfg_cubby_reserve() failed\n",
			wlc->pub->unit, __FUNCTION__));
		goto fail;
	}

#ifdef RXCHAIN_PWRSAVE
#ifdef BCMDBG
	wlc_dump_register(pub, "rxchain_pwrsave", (dump_fn_t)wlc_dump_rxchain_pwrsave, (void *)ap);
#endif
	var = getvar(NULL, "wl_nonassoc_rxchain_pwrsave_enable");
	if (var) {
		if (!bcm_strtoul(var, NULL, 0))
			appvt->rxchain_pwrsave.pwrsave.power_save_check &= ~NONASSOC_PWRSAVE_ENB;
		else
			appvt->rxchain_pwrsave.pwrsave.power_save_check |= NONASSOC_PWRSAVE_ENB;
	}
#endif /* RXCHAIN_PWRSAVE */

#ifdef RADIO_PWRSAVE
	if (!(appvt->radio_pwrsave.timer =
		wl_init_timer(wlc->wl, wlc_radio_pwrsave_timer, ap, "radio_pwrsave"))) {
		WL_ERROR(("%s: wl_init_timer for radio powersave timer failed\n", __FUNCTION__));
		goto fail;
	}
#ifdef BCMDBG
	wlc_dump_register(pub, "radio_pwrsave", (dump_fn_t)wlc_dump_radio_pwrsave, (void *)ap);
#endif
#endif /* RADIO_PWRSAVE */

#ifdef BCMDBG
	wlc_dump_register(pub, "ap", (dump_fn_t)wlc_dump_ap, (void *)ap);
#endif

	/* bsscfg up/down callback */
	if (wlc_bsscfg_updown_register(wlc, wlc_ap_bss_updown, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_bsscfg_updown_register() failed\n",
			wlc->pub->unit, __FUNCTION__));
		goto fail;
	}

	/* Add client callback to the scb state notification list */
	if (wlc_scb_state_upd_register(wlc, wlc_ap_scb_state_upd_cb, wlc) != BCME_OK) {
		WL_ERROR(("wl%d:%s: unable to register callback %p\n",
		          wlc->pub->unit, __FUNCTION__,
			OSL_OBFUSCATE_BUF(wlc_ap_scb_state_upd_cb)));
		goto fail;
	}

	err = wlc_module_register(pub, wlc_ap_iovars, "ap", appvt, wlc_ap_doiovar,
	                          wlc_ap_watchdog, wlc_ap_wlc_up, wlc_ap_wlc_down);
	if (err) {
		WL_ERROR(("%s: wlc_module_register failed\n", __FUNCTION__));
		goto fail;
	}

	err = wlc_module_add_ioctl_fn(wlc->pub, (void *)ap, wlc_ap_ioctl, 0, NULL);
	if (err) {
		WL_ERROR(("%s: wlc_module_add_ioctl_fn err=%d\n",
		          __FUNCTION__, err));
		goto fail;
	}

	/* enable bw_switch_160 only for 43684b1 */
	if (D11REV_IS(wlc->pub->corerev, 129) && D11MINORREV_LT(wlc->pub->corerev_minor, 2)) {
		pub->_bw_switch_160 = WLC_BW_SWITCH_TDCS;
	} else {
		pub->_bw_switch_160 = WLC_BW_SWITCH_DISABLE;
	}

	/* reserve cubby in the scb container for per-scb private data */
	bzero(&cubby_params, sizeof(cubby_params));

	cubby_params.context = appvt;
	cubby_params.fn_init = wlc_as_scb_init;
	cubby_params.fn_deinit = wlc_as_scb_deinit;
	cubby_params.fn_secsz = wlc_as_scb_secsz;

	appvt->as_scb_handle = wlc_scb_cubby_reserve_ext(wlc, sizeof(wlc_assoc_req_t *),
	                                                 &cubby_params);

	if (appvt->as_scb_handle < 0) {
		WL_ERROR(("%s: wlc_scb_cubby_reserve for as req failed\n", __FUNCTION__));
		goto fail;
	}

	/* reserve cubby in the scb container for per-scb private data */
	bzero(&cubby_params, sizeof(cubby_params));

	cubby_params.context = appvt;
	cubby_params.fn_init = wlc_ap_scb_init;
	cubby_params.fn_deinit = wlc_ap_scb_deinit;
	cubby_params.fn_secsz = wlc_ap_scb_secsz;

	appvt->ap_scb_handle = wlc_scb_cubby_reserve_ext(wlc, sizeof(ap_scb_cubby_t *),
	                                                 &cubby_params);

	if (appvt->ap_scb_handle < 0) {
		WL_ERROR(("%s: wlc_scb_cubby_reserve for ap failed\n", __FUNCTION__));
		goto fail;
	}

	appvt->scb_supp_chan_handle = wlc_scb_cubby_reserve(wlc,
		sizeof(wlc_supp_channels_t), NULL, NULL, NULL, appvt);

	if (appvt->scb_supp_chan_handle < 0) {
		WL_ERROR(("%s: wlc_scb_cubby_reserve failed\n", __FUNCTION__));
		goto fail;
	}

	/* register packet class callback */
	err = wlc_pcb_fn_set(wlc->pcb, 0, WLF2_PCB1_STA_PRB, wlc_ap_probe_complete);
	if (err != BCME_OK) {
		WL_ERROR(("%s: wlc_pcb_fn_set err=%d\n", __FUNCTION__, err));
		goto fail;
	}

	/* Attach APPS module */
	if (wlc_apps_attach(wlc)) {
		WL_ERROR(("%s: wlc_apps_attach failed\n", __FUNCTION__));
		goto fail;
	}
#if BAND6G || defined(WL_OCE_AP)
	wlc->rnr_update_timer = wl_init_timer((struct wl_info *)wlc->wl,
		wlc_rnr_update_timer, wlc, "rnr_update_timer");

	wlc->rnr_update_period = RNR_UPDATE_STATIC_PERIOD;

	if (wlc->rnr_update_timer == NULL) {
		WL_ERROR(("wl%d: %s: wl_init_timer for rnr scan timer failed\n",
			wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	wlc->upr_fd_info = (wlc_upr_fd_info_t*)MALLOCZ(pub->osh, sizeof(wlc_upr_fd_info_t));
	if (wlc->upr_fd_info == NULL) {
		WL_ERROR(("%s: MALLOC wlc_upr_fd_info_t failed\n", __FUNCTION__));
		goto fail;
	}

	wlc->upr_fd_info->period = UNSOLICITED_BCAST_PRB_RSP_TX_PERIOD;

	wlc->upr_fd_info->timer = wlc_hrt_alloc_timeout(wlc->hrti);
	if (!wlc->upr_fd_info->timer) {
		WL_ERROR(("wl%d: %s: wlc_hrt_alloc_timeout failed\n",
			wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
#endif /* WL_WIFI_6GHZ || WL_OCE_AP */
	wlc->check_short_ssid = FALSE; /* Ignore short ssid if present in probe request */
	/* register IE mgmt callback */
	/* calc/build */
	if (wlc_iem_add_build_fn(wlc->iemi, FC_AUTH, DOT11_MNG_CHALLENGE_ID,
	      wlc_auth_calc_chlng_ie_len, wlc_auth_write_chlng_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_add_build_fn failed, chlng in auth\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}

	/* assocresp/reassocresp */
	if (wlc_iem_add_build_fn_mft(wlc->iemi, arsfstbmp, DOT11_MNG_RATES_ID,
	      wlc_assoc_calc_sup_rates_ie_len, wlc_assoc_write_sup_rates_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_add_build_fn failed, sup rates in assocresp\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	if (wlc_iem_add_build_fn_mft(wlc->iemi, arsfstbmp, DOT11_MNG_EXT_RATES_ID,
	      wlc_assoc_calc_ext_rates_ie_len, wlc_assoc_write_ext_rates_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_add_build_fn failed, ext rates in assocresp\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	/* bcn/prbrsp */
	if (wlc_iem_add_build_fn_mft(wlc->iemi, bcnfstbmp, DOT11_MNG_VHT_TRANSMIT_POWER_ENVELOPE_ID,
		wlc_calc_pwr_env_ie_len, wlc_write_pwr_env_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_iem_add_build_fn failed, pwr env ie\n",
			wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	/* bcn/prbrsp */
	if (wlc_ier_add_build_fn(wlc->ier_csw, DOT11_MNG_VHT_TRANSMIT_POWER_ENVELOPE_ID,
	      wlc_csw_calc_pwr_env_ie_len, wlc_csw_write_pwr_env_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_ier_add_build_fn failed, pwr env ie in csw\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
#if BAND6G || defined(WL_OCE)
	/* register RNR element build callbacks */
	if (wlc_iem_add_build_fn_mft(wlc->iemi,
			rnr_elm_build_fstbmp, DOT11_MNG_RNR_ID,
			wlc_rnr_element_calc_len, wlc_rnr_element_build_fn, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_iem_add_build_fn_mft failed, RNR ID in beacon, prb rsp\n",
			wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
#endif /* WL_WIFI_6GHZ || WL_OCE */
	/* parse */
	/* auth */
	if (wlc_iem_add_parse_fn(wlc->iemi, FC_AUTH, DOT11_MNG_CHALLENGE_ID,
	                         wlc_auth_parse_chlng_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_add_parse_fn failed, chlng in auth\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}

	/* assocreq/reassocreq */
	if (wlc_iem_add_parse_fn_mft(wlc->iemi, arqfstbmp, DOT11_MNG_SSID_ID,
	                             wlc_assoc_parse_ssid_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_add_parse_fn failed, ssid in assocreq\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	if (wlc_iem_add_parse_fn_mft(wlc->iemi, arqfstbmp, DOT11_MNG_RATES_ID,
	                             wlc_assoc_parse_sup_rates_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_add_parse_fn failed, sup rates in assocreq\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	if (wlc_iem_add_parse_fn_mft(wlc->iemi, arqfstbmp, DOT11_MNG_EXT_RATES_ID,
	                             wlc_assoc_parse_ext_rates_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_add_parse_fn failed, ext rates in assocreq\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	if (wlc_iem_add_parse_fn_mft(wlc->iemi, arqfstbmp, DOT11_MNG_SUPP_CHANNELS_ID,
	                             wlc_assoc_parse_supp_chan_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_add_parse_fn failed, ext rates in assocreq\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	if (wlc_iem_vs_add_parse_fn_mft(wlc->iemi, arqfstbmp, WLC_IEM_VS_IE_PRIO_WPS,
	                                wlc_assoc_parse_wps_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_vs_add_parse_fn failed, wps in assocreq\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	if (wlc_iem_vs_add_parse_fn_mft(wlc->iemi, arqfstbmp, WLC_IEM_VS_IE_PRIO_BRCM_PSTA,
	                                wlc_assoc_parse_psta_ie, wlc) != BCME_OK) {
		WL_ERROR(("wl%d: %s wlc_iem_vs_add_parse_fn failed, psta in assocreq\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	if (wlc_iem_add_parse_fn_mft(wlc->iemi, arqfstbmp, DOT11_MNG_REGCLASS_ID,
		wlc_assoc_parse_regclass_ie, wlc) != BCME_OK) {

		WL_ERROR(("wl%d: %s wlc_iem_vs_add_parse_fn failed, regclass in assocreq\n",
		          wlc->pub->unit, __FUNCTION__));
		goto fail;
	}
	appvt->txbcn_timeout = WLC_AP_TXBCN_TIMEOUT;
	appvt->txbcn_edcrs_timeout = WLC_AP_TXBCN_EDCRS_TIMEOUT;
	appvt->edcrs_txbcn_inact_thresh = WLC_AP_EDCRS_BCN_INACT_THRESH_DEFAULT;
	appvt->edcrs_hi_in_driver_wait_thresh = WLC_AP_EDCRS_IN_DRIVER_WAIT_COUNT;
	appvt->edcrs_dur_thresh_us = WLC_AP_CCA_EDCRS_DUR_THRESH_US;
	memset(&appvt->reg_info, 0, sizeof(appvt->reg_info));
	appvt->reg_info.ver = WL_REG_INFO_VER;
	appvt->reg_info.len = sizeof(appvt->reg_info);
	appvt->reg_info.reg_info_field = WL_INDOOR_ACCESS_POINT; /* default till AFC/SP */
	appvt->reg_info.reg_info_override = WL_REG_INFO_RESERVED;
	memset(appvt->reg_info.afc_eirp, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.afc_eirp));
	memset(appvt->reg_info.afc_psd, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.afc_psd));
	memset(appvt->reg_info.lpi_eirp, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.lpi_eirp));
	memset(appvt->reg_info.lpi_psd, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.lpi_psd));
	memset(appvt->reg_info.sp_eirp, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.sp_eirp));
	memset(appvt->reg_info.sp_psd, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.sp_psd));
	wlc_ap_set_edcrs_hi_event_mode(appvt, WL_EDCRS_HI_EVENT_AUTO);

	/* Add callback function for AP interface creation */
	if (wlc_bsscfg_iface_register(wlc, WL_INTERFACE_TYPE_AP, wlc_ap_iface_create,
		wlc_ap_iface_remove, appvt) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_bsscfg_iface_register() failed\n",
			WLCWLUNIT(wlc), __FUNCTION__));
		goto fail;
	}
	/* mandatory in 6G to support mbssid */
	if (CHSPEC_IS6G(wlc->chanspec)) {
		appvt->block_nonmbssid = 1;
	} else {
		appvt->block_nonmbssid = 0;
	}

	return (wlc_ap_info_t*)appvt;

fail:
	MODULE_DETACH_TYPECASTED((wlc_ap_info_t*)appvt, wlc_ap_detach);

	return NULL;
} /* wlc_ap_attach */

void
BCMATTACHFN(wlc_ap_detach)(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t* appvt;
	wlc_info_t *wlc;
	wlc_pub_t *pub;

	if (ap == NULL) {
		return;
	}
	appvt = (wlc_ap_info_pvt_t*) ap;
	wlc = appvt->wlc;
	pub = appvt->pub;

	wlc_apps_detach(wlc);
	wlc_scb_state_upd_unregister(wlc, wlc_ap_scb_state_upd_cb, wlc);

#ifdef RADIO_PWRSAVE
	if (appvt->radio_pwrsave.timer) {
		wl_del_timer(wlc->wl, appvt->radio_pwrsave.timer);
		wl_free_timer(wlc->wl, appvt->radio_pwrsave.timer);
		appvt->radio_pwrsave.timer = NULL;
	}
#endif
#if BAND6G || defined(WL_OCE_AP)
	if (wlc->rnr_update_timer) {
		RNR_UPDATE_FREE_TIMER(wlc);
	}
	if (wlc->upr_fd_info) {
		if (wlc->upr_fd_info->pkt) {
			PKTFREE(wlc->osh, wlc->upr_fd_info->pkt, TRUE);
			wlc->upr_fd_info->one_time_alloc--;
		}
		if (wlc->upr_fd_info->timer) {
			wlc_hrt_free_timeout(wlc->upr_fd_info->timer);
		}
		MFREE(wlc->osh, wlc->upr_fd_info, sizeof(wlc_upr_fd_info_t));
	}

#endif /* WL_WIFI_6GHZ || WL_OCE_AP */

	/* Unregister the AP interface during detach */
	if (wlc_bsscfg_iface_unregister(wlc, WL_INTERFACE_TYPE_AP) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_bsscfg_iface_unregister() failed\n",
			WLCWLUNIT(wlc), __FUNCTION__));
	}

	wlc_module_unregister(pub, "ap", appvt);

	(void)wlc_module_remove_ioctl_fn(wlc->pub, (void *)ap);

	MFREE(pub->osh, appvt, sizeof(wlc_ap_info_pvt_t));
}

#ifdef WLDEAUTH_INTRANSIT_FILTER
/* Add the destination address of the deauth frame to the intransit hash table.
 * If hash already in use, creates a linked list.
 * returns TRUE if deauth frame has been added to the intransit hash table.
 * returns FALSE if maximum nr of deauth frames are in transit or malloc failed.
 */
bool
wlc_ap_intransit_filter_add_da(wlc_ap_info_t *ap, wlc_bsscfg_t *cfg,
	struct ether_addr *da)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*)ap;
	ap_bsscfg_cubby_t *bpi = AP_BSSCFG_CUBBY(appvt, cfg);
	uint8 hash;
	deauth_ea_t *stap;

	if ((stap = appvt->deauth_intransit_freelist) == NULL) {
		/* Maximum nr of deauth frames are in transit */
		ASSERT(appvt->intransit_inuse == MAX_DEAUTH);
		return FALSE;
	}

	appvt->deauth_intransit_freelist = stap->next;
	eacopy(da, &stap->mac);
	hash = DEAUTH_EA_HASH(da);
	stap->next = bpi->intransit_maclist[hash];
	bpi->intransit_maclist[hash] = stap;
	appvt->intransit_inuse++;
	ASSERT(appvt->intransit_inuse <= MAX_DEAUTH);

	return TRUE;
}

/* Remove the destination address of the deauth frame from the intransit hash table.
 * At calculated index traverse linked list to find the address.
 */
void
wlc_ap_intransit_filter_rem_da(wlc_ap_info_t *ap, wlc_bsscfg_t *cfg,
	struct ether_addr *da)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*)ap;
	ap_bsscfg_cubby_t *bpi = AP_BSSCFG_CUBBY(appvt, cfg);
	uint8 hash;
	deauth_ea_t *stap;
	deauth_ea_t *p;
	deauth_ea_t *prevp;

	hash = DEAUTH_EA_HASH(da);
	prevp = stap = bpi->intransit_maclist[hash];
	while (stap != NULL) {
		p = stap->next;
		if (!DEAUTH_EA_CMP(&stap->mac, da)) {
			if (stap == bpi->intransit_maclist[hash]) {
				bpi->intransit_maclist[hash] = p;
			} else {
				prevp->next = p;
			}
			stap->next = appvt->deauth_intransit_freelist;
			appvt->deauth_intransit_freelist = stap;
			ASSERT(appvt->intransit_inuse > 0);
			appvt->intransit_inuse--;
			break;
		}
		prevp = stap;
		stap = p;
	}
}

/* This function is used as a filter and will be called when a deauth frame is attempted.
 * When called, this function will check if the destination address of the deauth frame is present
 * in the intransit hash table.
 * returns TRUE if deauth frame is already intransit for this station.
 * returns FALSE when it is allowed to send a deauth frame.
 */
bool
wlc_ap_intransit_filter_check_da(wlc_ap_info_t *ap, wlc_bsscfg_t *cfg, struct ether_addr *da)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_bsscfg_cubby_t *bpi = AP_BSSCFG_CUBBY(appvt, cfg);
	uint8 hash;
	deauth_ea_t *stap;
	deauth_ea_t *p;

	ASSERT(bpi != NULL);

	hash = DEAUTH_EA_HASH(da);
	stap = bpi->intransit_maclist[hash];
	while (stap != NULL) {
		p = stap->next;
		if (!DEAUTH_EA_CMP(&stap->mac, da)) {
			return TRUE;
		}
		stap = p;
	}

	return FALSE;
}

/* This function is called on 'wl down' and when bss is removed.
 * When called, this function will clear the intransit table in bsscfg cubby.
 */
static void
wlc_ap_intransit_filter_clear_bss(wlc_ap_info_pvt_t *appvt, wlc_bsscfg_t *cfg)
{
	ap_bsscfg_cubby_t *bpi = AP_BSSCFG_CUBBY(appvt, cfg);
	uint8 hash;
	deauth_ea_t *stap;
	deauth_ea_t *p;

	for (hash = 0; hash < MAX_DEAUTH; hash++) {
		stap = bpi->intransit_maclist[hash];
		while (stap != NULL) {
			p = stap->next;

			stap->next = appvt->deauth_intransit_freelist;
			appvt->deauth_intransit_freelist = stap;

			stap = p;
		}
		bpi->intransit_maclist[hash] = NULL;
	}
}
#endif /* WLDEAUTH_INTRANSIT_FILTER */

static int
wlc_ap_wlc_up(void *ctx)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ctx;
	wlc_ap_info_t *ap = &appvt->appub;

	(void)ap;

#ifdef RXCHAIN_PWRSAVE
	wlc_reset_rxchain_pwrsave_mode(ap);
#endif
#ifdef RADIO_PWRSAVE
	wlc_reset_radio_pwrsave_mode(ap);
#endif
#if BAND6G || defined(WL_OCE_AP)
	/* let first scan happen as early as possible */
	RNR_UPDATE_ADD_TIMER(appvt->wlc, RNR_UPDATE_START_PERIOD);
#endif /* WL_WIFI_6GHZ || WL_OCE_AP */

	return BCME_OK;
}

static int
wlc_ap_wlc_down(void *ctx)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ctx;
	wlc_info_t *wlc = appvt->wlc;
	wlc_bsscfg_t *cfg;
	int idx;
#if BAND6G || defined(WL_OCE_AP)
	uint16 val = 0x00;
	/* let first scan happen as early as possible */
	RNR_UPDATE_DEL_TIMER(wlc);

	if (wlc->upr_fd_info) {
		if (wlc->upr_fd_info->pkt) {
			PKTFREE(wlc->osh, wlc->upr_fd_info->pkt, TRUE);
			wlc->upr_fd_info->one_time_alloc--;
			wlc->upr_fd_info->pkt = NULL;
		}
		if (wlc->upr_fd_info->timer) {
			wlc_hrt_del_timeout(wlc->upr_fd_info->timer);
		}
	}
	if (WL_UPR_FD_UCODE_ENAB(wlc)) {
		val = wlc_bmac_mhf_get(wlc->hw, (uint8)0x01, WLC_BAND_AUTO);
		val &= ~MHF2_UNSOL_PRS;
		/* disable UPR from ucode */
		wlc_mhf(wlc, (uint8)0x01, 0xffff, val, WLC_BAND_AUTO);
	}
#endif /* WL_WIFI_6GHZ || WL_OCE_AP */

#ifdef WLDEAUTH_INTRANSIT_FILTER
	appvt->intransit_inuse = 0;
#endif /* WLDEAUTH_INTRANSIT_FILTER */

	FOREACH_AP(wlc, idx, cfg) {
		/* Unregister the requested MSCH operating channel */
		wlc_ap_timeslot_unregister(cfg);
#ifdef WLDEAUTH_INTRANSIT_FILTER
		/* clear intransit_filter table */
		wlc_ap_intransit_filter_clear_bss(appvt, cfg);
#endif /* WLDEAUTH_INTRANSIT_FILTER */
	}

#if defined(BCMDBG)
	wlc_apps_wlc_down(wlc);
#endif /* BCMDBG */

	return BCME_OK;
}

int
wlc_ap_up(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*)ap;
	wlc_info_t *wlc = appvt->wlc;
	wlcband_t *band;
	wlc_bss_info_t *target_bss = bsscfg->target_bss;
#ifdef WLMCHAN
	chanspec_t chspec;
#endif

#if defined(BCMDBG) || defined(BCMDBG_ERR) || defined(WLMSG_INFORM)
	char chanbuf[CHANSPEC_STR_LEN];
#endif /* BCMDBG || BCMDBG_ERR || WLMSG_INFORM */
	int ret = BCME_OK;
	chanspec_t radar_chanspec;

	/* update radio parameters from default_bss */
	/* adopt the default BSS parameters as our BSS */
	/* adopt the default BSS params as the target's BSS params */
	bcopy(wlc->default_bss, target_bss, sizeof(wlc_bss_info_t));

	/* set some values to be appropriate for AP operation */
	target_bss->bss_type = DOT11_BSSTYPE_INFRASTRUCTURE;
	target_bss->atim_window = 0;
	target_bss->capability = DOT11_CAP_ESS;
#ifdef WL11K_AP
	if (WL11K_ENAB(wlc->pub) &&
	    wlc_rrm_enabled(wlc->rrm_info, bsscfg)) {
		target_bss->capability |= DOT11_CAP_RRM;
	}
#endif /* WL11K_AP */

	bcopy((char*)&bsscfg->cur_etheraddr, (char*)&target_bss->BSSID, ETHER_ADDR_LEN);

#ifdef WLMCHAN
	/* allow P2P GO or soft-AP to run on a possible different channel */
	if (MCHAN_ENAB(wlc->pub) &&
	    !wlc_mchan_stago_is_disabled(wlc->mchan) &&
#ifdef WLP2P
	    BSS_P2P_ENAB(wlc, bsscfg) &&
#endif
	    (chspec = wlc_mchan_configd_go_chanspec(wlc->mchan, bsscfg)) != 0) {
		WL_INFORM(("wl%d: use cfg->chanspec 0x%04x in BSS "MACF"\n", wlc->pub->unit,
			chspec, ETHER_TO_MACF(target_bss->BSSID)));
		target_bss->chanspec = chspec;
		/* continue to validate the chanspec (channel and channel width)...
		 * they may become invalid due to other user configurations happened
		 * between GO bsscfg creation and now...
		 */
	} else
#endif /* WLMCHAN */
	/* use the current operating channel if any */
	if (wlc->pub->associated) {
		/* AP is being brought up in same channel/band as STA */
		WL_INFORM(("wl%d: share wlc->home_chanspec 0x%04x in BSS "MACF"\n", wlc->pub->unit,
			wlc->home_chanspec, ETHER_TO_MACF(target_bss->BSSID)));
		target_bss->chanspec = wlc->home_chanspec;
		goto sradar_check;
	}

	radar_chanspec = wlc_radar_chanspec(wlc->cmi, target_bss->chanspec);

	/* validate or fixup default channel value */
	/* also, fixup for GO if radar channel */
	if (!wlc_valid_chanspec_db(wlc->cmi, target_bss->chanspec) ||
	    wlc_restricted_chanspec(wlc->cmi, target_bss->chanspec) ||
#ifdef WLP2P
	   (BSS_P2P_ENAB(wlc, bsscfg) &&
	    BSS_11H_AP_NORADAR_CHAN_ENAB(wlc, bsscfg) &&
	    radar_chanspec != 0) ||
#endif
	    FALSE) {
		chanspec_t chspec_local = wlc_default_chanspec(wlc->cmi, FALSE);
		if ((chspec_local == INVCHANSPEC) ||
		    wlc_restricted_chanspec(wlc->cmi, chspec_local)) {
			WL_ERROR(("wl%d: cannot create BSS on chanspec %s\n",
				wlc->pub->unit,
				wf_chspec_ntoa_ex(target_bss->chanspec, chanbuf)));
			ret = BCME_BADCHAN;
			goto exit;
		}
		target_bss->chanspec = chspec_local;
		WL_INFORM(("wl%d: use default chanspec %s in BSS "MACF"\n",
			wlc->pub->unit, wf_chspec_ntoa_ex(chspec_local, chanbuf),
			ETHER_TO_MACF(target_bss->BSSID)));
	}

	/* Validate the channel bandwidth */
	band = wlc->bandstate[CHSPEC_BANDUNIT(target_bss->chanspec)];
	if (CHSPEC_IS40(target_bss->chanspec) &&
	    (!N_ENAB(wlc->pub) ||
	     (wlc_channel_locale_flags_in_band(wlc->cmi, band->bandunit) & WLC_NO_40MHZ) ||
	     !WL_BW_CAP_40MHZ(band->bw_cap))) {
		uint channel = wf_chspec_ctlchan(target_bss->chanspec);
		target_bss->chanspec = CH20MHZ_CHSPEC(channel,
		                                       CHSPEC_BAND(target_bss->chanspec));
		WL_INFORM(("wl%d: use 20Mhz channel width in BSS "MACF"\n",
		           wlc->pub->unit, ETHER_TO_MACF(target_bss->BSSID)));
	}

sradar_check:
	radar_chanspec = wlc_radar_chanspec(wlc->cmi, target_bss->chanspec);

	/* for softap and extap, following special radar rules */
	/* return bad channel error if radar channel */
	/* when no station associated */
	/* won't allow soft/ext ap to be started on radar channel */
	if (BSS_11H_SRADAR_ENAB(wlc, bsscfg) &&
	    radar_chanspec != 0 &&
#ifdef STA
	    !wlc->stas_associated &&
#endif
	    TRUE) {
		WL_ERROR(("no assoc STA and starting soft or ext AP on radar chanspec %s\n",
		          wf_chspec_ntoa_ex(target_bss->chanspec, chanbuf)));
		ret = BCME_BADCHAN;
		goto exit;
	}

	/* for softap and extap with AP_NORADAR_CHAN flag set, don't allow
	 * bss to start if on a radar channel.
	 */
	if (BSS_11H_AP_NORADAR_CHAN_ENAB(wlc, bsscfg) &&
	    radar_chanspec != 0) {
		WL_ERROR(("AP_NORADAR_CHAN flag set, disallow ap on radar chanspec %s\n",
		          wf_chspec_ntoa_ex(target_bss->chanspec, chanbuf)));
		ret = BCME_BADCHAN;
		goto exit;
	}

exit:

	return ret;
} /* wlc_ap_up */

int
wlc_ap_down(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	wlc_info_t *wlc = appvt->wlc;
#ifdef WDS
	wlc_if_t *wlcif;
#endif
	int callback = 0; /* Need to fix this timer callback propagation; error prone right now */
	struct scb_iter scbiter;
	struct scb *scb;
	int assoc_scb_count = 0;
#if BAND6G || defined(WL_OCE_AP)
	uint16 val = 0x00;
#endif /* BAND6G || WL_OCE_AP */

#if defined(WL11K_AP)
	wlc_rrm_del_pilot_timer(wlc, bsscfg);
#endif
#ifdef WLMCNX
	if (MCNX_ENAB(wlc->pub)) {
		wlc_mcnx_bss_upd(wlc->mcnx, bsscfg, FALSE);
	}
#endif
#if BAND6G || defined(WL_OCE_AP)
	/* release broadcat probe response packet if hold */
	if (wlc->upr_fd_info) {
		if (wlc->upr_fd_info->timer) {
			wlc_hrt_del_timeout(wlc->upr_fd_info->timer);
		}
		/* release packet if hold */
		if (wlc->upr_fd_info->pkt) {
			PKTFREE(wlc->osh, wlc->upr_fd_info->pkt, TRUE);
			wlc->upr_fd_info->pkt = NULL;
			wlc->upr_fd_info->flags = 0;
			wlc->upr_fd_info->one_time_alloc--;
		}
	}
	if (WL_UPR_FD_UCODE_ENAB(wlc)) {
		val = wlc_bmac_mhf_get(wlc->hw, (uint8)0x01, WLC_BAND_AUTO);
		val &= ~MHF2_UNSOL_PRS;
		/* disable UPR from ucode */
		wlc_mhf(wlc, (uint8)0x01, 0xffff, val, WLC_BAND_AUTO);
	}
#endif /* BAND6G || WL_OCE_AP */
	wlc_ap_timeslot_unregister(bsscfg);

	FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
		if (!SCB_LEGACY_WDS(scb)) {
			if (SCB_ASSOCIATED(scb)) {
				assoc_scb_count++;
				if (BSS_UCAST_DISASSOC_ON_DOWN(appvt, bsscfg)) {
					/* send unicast disassoc request to all stas */
					wlc_senddisassoc(wlc, bsscfg, scb, &scb->ea,
						&bsscfg->BSSID, &bsscfg->cur_etheraddr,
						DOT11_RC_DISASSOC_LEAVING);
				}
			}
			wlc_scbfree(wlc, scb);
		}
#ifdef WDS
		else if ((wlcif = SCB_WDS(scb)) != NULL) {
			/* send WLC_E_LINK event with status DOWN to WDS interface */
			wlc_wds_create_link_event(wlc, wlcif->u.scb, FALSE);
		}
#endif
	}

	if (assoc_scb_count) {
		if (!BSS_UCAST_DISASSOC_ON_DOWN(appvt, bsscfg)) {
			/* send broadcast disassoc request to all stas */
			wlc_senddisassoc(wlc, bsscfg, WLC_BCMCSCB_GET(wlc, bsscfg),
					&ether_bcast, &bsscfg->BSSID, &bsscfg->cur_etheraddr,
					DOT11_RC_DISASSOC_LEAVING);
		}
		/* send up a broadcast deauth mac event if there were any
		 * associated STAs
		 */
		wlc_deauth_complete(wlc, bsscfg, WLC_E_STATUS_SUCCESS, &ether_bcast,
		                    DOT11_RC_DEAUTH_LEAVING, 0);
	}
	/* adjust associated state(s) accordingly */
	wlc_suspend_mac_and_wait(wlc);
	wlc_ap_up_upd(ap, bsscfg, FALSE);
	/* Fix up mac control */
	wlc_macctrl_init(wlc, NULL);
	wlc_enable_mac(wlc);
	wlc_txqueue_end(wlc, bsscfg);

	WL_APSTA_UPDN(("Reporting link down on config %d (AP disabled)\n",
		WLC_BSSCFG_IDX(bsscfg)));

	wlc_link(wlc, FALSE, &bsscfg->cur_etheraddr, bsscfg, WLC_E_LINK_BSSCFG_DIS);

#ifdef RADIO_PWRSAVE
	wlc_reset_radio_pwrsave_mode(ap);
#endif

#ifdef WLTPC
	/* reset per bss link margin values */
	wlc_ap_bss_tpc_setup(wlc->tpc, bsscfg);
#endif

	/* Clear the BSSID */
	bzero(bsscfg->BSSID.octet, ETHER_ADDR_LEN);

	/* WES XXX: on every bsscfg down, need to clear SSID in ucode to stop probe
	 * responses if this is the bsscfg_prb_idx
	 */

	/* WES XXX: if we take down the bsscfg_bcn_idx cfg, what beacon should be in
	 * the ucode template? Should we just leave it as is, or update to another so
	 * that an active SSID and security params show up in the beacon?
	 */

	/* WES XXX: on last bsscfg down, need to stop beaconing.
	 * Maybe just clear maccontrol AP bit
	 */

#ifdef STA
	if (wlc->pub->up && !AP_ACTIVE(wlc)) {
		wlc_resync_sta(wlc);
	}
#endif

	return callback;
} /* wlc_ap_down */

static int
wlc_authenticated_sta_check_cb(struct scb *scb)
{
	return SCB_AUTHENTICATED(scb);
}

static int
wlc_authorized_sta_check_cb(struct scb *scb)
{
	return SCB_AUTHORIZED(scb);
}

static int
wlc_wme_sta_check_cb(struct scb *scb)
{
	return SCB_WME(scb);
}

/* Returns a maclist of all scbs that pass the provided check function.
 * The buf is formatted as a struct maclist on return, and may be unaligned.
 * buf must be at least 4 bytes long to hold the maclist->count value.
 * If the maclist is too long for the supplied buffer, BCME_BUFTOOSHORT is returned
 * and the maclist->count val is set to the number of MAC addrs that would
 * have been returned. This allows the caller to allocate the needed space and
 * call again.
 */
static int
wlc_sta_list_get(wlc_ap_info_t *ap, wlc_bsscfg_t *cfg, uint8 *buf,
                 int len, int (*sta_check_cb)(struct scb *scb))
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;

	int err = 0;
	uint c = 0;
	uint8 *dst;
	struct scb *scb;
	struct scb_iter scbiter;
	ASSERT(len >= (int)sizeof(uint));

	/* make room for the maclist count */
	dst = buf + sizeof(uint);
	len -= sizeof(uint);
	FOREACH_BSS_SCB(wlc->scbstate, &scbiter, cfg, scb) {
		if (sta_check_cb(scb)) {
			c++;
			if (len >= ETHER_ADDR_LEN) {
				bcopy(scb->ea.octet, dst, ETHER_ADDR_LEN);
				dst += sizeof(struct ether_addr);
				len -= sizeof(struct ether_addr);
			} else {
				err = BCME_BUFTOOSHORT;
			}
		}
	}

	/* copy the actual count even if the buffer is too short */
	bcopy(&c, buf, sizeof(uint));

	return err;
}

/* age out STA associated but waiting on AS for authorization */
static void
wlc_ap_wsec_stas_timeout(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	struct scb *scb;
	struct scb_iter scbiter;

	FOREACHSCB(wlc->scbstate, &scbiter, scb) {
		wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);

#ifdef WL_HAPD_WDS
		if (SCB_LEGACY_WDS(scb) && !SCB_AUTHORIZED(scb)) {
				wlc_wds_check_association(wlc, scb);
		}
#endif
		/* Filter out the permanent SCB, AP SCB */
		if (scb->permanent ||
		    cfg == NULL || !BSSCFG_AP(cfg))
			continue;

		/* Filter out scbs not associated or marked for deletion */
		if (!SCB_ASSOCIATED(scb) || SCB_MARKED_DEL(scb))
			continue;

		if (WSEC_ENABLED(cfg->wsec) && !SCB_AUTHORIZED(scb) &&
			!(scb->wsec == WEP_ENABLED)) {
			scb->wsec_auth_timeout++;
			if (scb->wsec_auth_timeout > SCB_AUTH_TIMEOUT) {
				scb->wsec_auth_timeout = 0;
				WL_ERROR(("wl%d.%d: authentication delay, send deauth to "MACF"\n",
						wlc->pub->unit, WLC_BSSCFG_IDX(cfg),
						ETHER_TO_MACF(scb->ea)));
				wlc_scb_set_auth(wlc, cfg, scb, FALSE, AUTHENTICATED,
					DOT11_RC_UNSPECIFIED);
			}
		}
	}
}

/* age out STA w/ marked_del and not-connect */
static void
wlc_ap_scb_unusable_stas_timeout(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	struct scb *scb;
	struct scb_iter scbiter;

	FOREACHSCB(wlc->scbstate, &scbiter, scb) {
		wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);

		/* Filter out the permanent SCB, AP SCB */
		if (scb->permanent ||
		    cfg == NULL || !BSSCFG_AP(cfg))
			continue;

		if (SCB_IS_UNUSABLE(scb)) {
			if (SCB_AUTHENTICATED(scb)) {
				/* If SCB is in authenticated state then
				 * notify the station that we are deauthenticating it,
				 * when deauth has been acked clear the ASSOCIATED,
				 * AUTHENTICATED and AUTHORIZED bits in the flag
				 * before freeing the SCB
				 */
				if (SCB_FIRST_TO(scb) && !scb->sent_deauth) {
					WL_ASSOC(("wl%d.%d: %s: send deauth to "MACF" with"
						" reason %d\n", wlc->pub->unit,	WLC_BSSCFG_IDX(cfg),
						__FUNCTION__, ETHER_TO_MACF(scb->ea),
						DOT11_RC_INACTIVITY));
					wlc_send_deauth(wlc, cfg, scb, &scb->ea, &cfg->BSSID,
						&cfg->cur_etheraddr, DOT11_RC_INACTIVITY);
				} else if (scb->sent_deauth && scb->sent_deauth->deauth_acked) {
					uint deauth_reason = scb->sent_deauth->reason_code;
					WL_ASSOC(("wl%d.%d: %s: deauth to "MACF" with"
						" reason %d complete, clear state bits\n",
						 wlc->pub->unit, WLC_BSSCFG_IDX(cfg),
						__FUNCTION__, ETHER_TO_MACF(scb->ea),
						deauth_reason));
					wlc_deauth_sendcomplete_clean(wlc, scb);
					wlc_scb_clearstatebit(wlc, scb,
						ASSOCIATED | AUTHENTICATED | AUTHORIZED);
					wlc_deauth_complete(wlc, cfg, WLC_E_STATUS_SUCCESS,
						&scb->ea, deauth_reason, 0);
				} else if (!SCB_FIRST_TO(scb) && !scb->sent_deauth) {
					/* In this case skip deauth transmission and
					 * clear state bits.
					 */
					wlc_scb_clearstatebit(wlc, scb,
						ASSOCIATED | AUTHENTICATED | AUTHORIZED);
				}
			}
			if ((!SCB_AUTHENTICATED(scb) || !SCB_FIRST_TO(scb)) &&
				((wlc->pub->now - scb->used) >= appvt->scb_mark_del_timeout)) {
				WL_ASSOC(("wl%d.%d: %s Timeout Free scb:"MACF" \n", wlc->pub->unit,
					WLC_BSSCFG_IDX(cfg), __FUNCTION__, ETHER_TO_MACF(scb->ea)));
				wlc_scbfree(wlc, scb);
			} else {
				scb->mark |= SCB_UNUSABLE_STA_TO;
			}
		}
	}
}

static void
wlc_ap_stas_timeout(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	struct scb *scb;
	struct scb_iter scbiter;

	WL_INFORM(("%s: run at time = %d\n", __FUNCTION__, wlc->pub->now));
	FOREACHSCB(wlc->scbstate, &scbiter, scb) {
		wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);
		ap_scb_cubby_t *ap_scb = AP_SCB_CUBBY(appvt, scb);

		/* Don't age out the permanent SCB and AP SCB */
		if (scb->permanent ||
		    cfg == NULL || !BSSCFG_AP(cfg))
			continue;

		/* kill any other band scb */
		if (TRUE &&
#ifdef WLMCHAN
		    !MCHAN_ENAB(wlc->pub) &&
#endif
		    wlc_scbband(wlc, scb) != wlc->band) {
			wlc_scbfree(wlc, scb);
			continue;
		}

		/* probe associated stas if idle for scb_activity_time or reprobe them */
		if (SCB_ASSOCIATED(scb) &&
			((appvt->scb_activity_time &&
				((wlc->pub->now - scb->used) >= appvt->scb_activity_time)) ||
				(appvt->reprobe_scb && ap_scb->grace_attempts))) {
			wlc_ap_sta_probe(ap, scb);
		}

		/* Authenticated but idle for long time free it now */
		if ((scb->state == AUTHENTICATED) &&
		    ((wlc->pub->now - scb->used) >= SCB_LONG_TIMEOUT)) {
			/* Notify HOST with a Deauth, this will ensure
			 * clearing any obsolete STA entry created
			 * during previous association of the SCB
			 */
			wlc_deauth_complete(wlc, cfg, WLC_E_STATUS_SUCCESS,
					&scb->ea, DOT11_RC_INACTIVITY, 0);
			wlc_scbfree(wlc, scb);
			continue;
		}
	}
} /* wlc_ap_stas_timeout */

uint
wlc_ap_stas_associated(wlc_ap_info_t *ap)
{
	wlc_info_t *wlc = ((wlc_ap_info_pvt_t *)ap)->wlc;
	int i;
	wlc_bsscfg_t *bsscfg;
	uint count = 0;

	FOREACH_UP_AP(wlc, i, bsscfg)
		count += wlc_bss_assocscb_getcnt(wlc, bsscfg);

	return count;
}

#ifdef WL_SAE
/*
 * Called at the top of wlc_authresp_client.
 * Parse the auth frame, identify Commit and Confirms
 */
static uint
wlc_sae_parse_auth(wlc_bsscfg_t *cfg, struct dot11_management_header *hdr,
		uint8 *body, uint body_len)
{
	wlc_info_t *wlc = cfg->wlc;
	struct scb *scb;
	struct dot11_auth *auth = (struct dot11_auth *) body;
	uint16 auth_alg, auth_seq;
	int status = DOT11_SC_SUCCESS;

	auth_alg = ltoh16(auth->alg);
	auth_seq = ltoh16(auth->seq);

	if (body_len < sizeof(struct dot11_auth)) {
		WL_ERROR(("wl%d: %s: Bad argument\n", wlc->pub->unit, __FUNCTION__));
		status = BCME_BADARG;
		goto done;
	}

	WL_ASSOC(("wl%d: %s: alg : %d seq %d len %d\n", wlc->pub->unit, __FUNCTION__,
		auth_alg, auth_seq, body_len));
	if ((auth_alg != DOT11_SAE) || ((auth_seq != WL_SAE_COMMIT) &&
		(auth_seq != WL_SAE_CONFIRM))) {
		/* not an SAE auth. Fail. */
		status = DOT11_SC_FAILURE;
		goto done;
	}

	if ((scb = wlc_scbfind(wlc, cfg, (struct ether_addr *)&hdr->sa)) == NULL) {
		status = DOT11_SC_FAILURE;
		WL_ERROR(("wl%d: %s: NO SCB\n", wlc->pub->unit, __FUNCTION__));
		goto done;
	}

done:
	return status;
}
#endif /* WL_SAE */
#ifdef MFP
static bool wlc_is_scb_need_saquery(wlc_info_t *wlc,
	wlc_bsscfg_t *cfg, struct scb *scb, uint16 auth_alg)
{
	bool ret = FALSE;

	if (!(WLC_MFP_ENAB(wlc->pub) && SCB_MFP(scb) &&
		SCB_AUTHENTICATED(scb) && SCB_ASSOCIATED(scb) && SCB_AUTHORIZED(scb)) ||
		SCB_MARKED_DEL(scb)) {
		/* Don't send SA Query if station is not connected and MFP disabled */
	} else if (auth_alg == DOT11_SAE) {
		 /* Send SA Query for SAE Auth frame */
		ret = TRUE;
	}
	return ret;
}
#endif /* MFP */

/* Process Auth alg, only if the cfg->WPA_auth supports */
static bool wlc_is_valid_auth_frame(wlc_bsscfg_t *bsscfg, uint16 auth_alg)
{
	switch (auth_alg) {
		case DOT11_SAE:
			if (!(bsscfg->WPA_auth & (WPA3_AUTH_SAE_PSK | WPA3_AUTH_SAE_FBT)))
				return FALSE;
			break;
		case DOT11_SHARED_KEY:
			if (!(WSEC_ENABLED(bsscfg->wsec) && WSEC_WEP_ENABLED(bsscfg->wsec)))
				return FALSE;
			break;
		case DOT11_FAST_BSS:
			if (!(bsscfg->WPA_auth & (WPA2_AUTH_FT | WPA3_AUTH_SAE_FBT)))
				return FALSE;
			break;
		case DOT11_OPEN_SYSTEM:
			break;
	}
	return TRUE;
}

void
wlc_ap_authresp(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg,
	struct dot11_management_header *hdr,  uint8 *body, uint body_len,
	void *p, bool short_preamble, d11rxhdr_t *rxh)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	wlc_info_t *wlc = appvt->wlc;
	struct dot11_auth *auth;
	struct scb *scb = NULL;
	ap_scb_cubby_t *ap_scb;
	wlc_bsscfg_t *cfg;
	uint16 auth_alg, auth_seq;
	uint16 status = DOT11_SC_SUCCESS;
	int addr_match;
	uint i, j;
	wlc_key_id_t key_id;
	uint16 fc;
	int err;
	wlc_iem_upp_t upp;
	wlc_iem_ft_pparm_t ftpparm;
	wlc_iem_pparm_t pparm;
	wlc_key_t *key;
	uint8 *body_start = body;
	uint body_len_start = body_len;

#if defined(BCMDBG) || defined(BCMDBG_ERR) || defined(WLMSG_ASSOC) || \
	defined(WLMSG_BTA)
	char eabuf[ETHER_ADDR_STR_LEN], *sa;

	sa = bcm_ether_ntoa(&hdr->sa, eabuf);
#endif

	WL_TRACE(("wl%d: wlc_authresp_ap\n", WLCWLUNIT(wlc)));

	ASSERT(BSSCFG_AP(bsscfg));
	ASSERT(bsscfg->up);

#ifdef BCM_CEVENT
	if (CEVENT_STATE(wlc->pub)) {
		wlc_send_cevent(wlc, bsscfg, &hdr->sa, 0, 0, 0, NULL, 0,
			CEVENT_D2C_MT_AUTH_RX, CEVENT_FRAME_DIR_RX);
	}
#endif /* BCM_CEVENT */

	/* PR38742 WAR: do not process auth frames while AP scan is in
	 * progress, it may cause frame going out on a different
	 * band/channel
	 */
	if (SCAN_IN_PROGRESS(wlc->scan)) {
		WL_ASSOC(("wl%d: AP Scan in progress, abort auth\n", wlc->pub->unit));
		status = SMFS_CODE_IGNORED;
		goto smf_stats;
	}

#ifdef BCMAUTH_PSK
	/* check for tkip countermesures */
	if (WSEC_TKIP_ENABLED(bsscfg->wsec) && wlc_keymgmt_tkip_cm_enabled(wlc->keymgmt, bsscfg)) {
		return;
	}
#endif /* BCMAUTH_PSK */

#ifdef PSTA
	/* Don't allow more than psta max assoc limit */
	if (PSTA_ENAB(wlc->pub) && (wlc_ap_stas_associated(ap) >= PSTA_MAX_ASSOC(wlc))) {
	        WL_ERROR(("wl%d: %s denied association due to max psta association limit\n",
	                  wlc->pub->unit, sa));
		status = SMFS_CODE_IGNORED;
		goto smf_stats;
	}
#endif /* PSTA */

#if BAND5G
	/* Reject auth & assoc while  performing a channel switch */
	if (wlc->block_datafifo & DATA_BLOCK_QUIET) {
		WL_REGULATORY(("wl%d: %s: Authentication denied while in radar"
		               " avoidance mode\n", wlc->pub->unit, __FUNCTION__));
		status = SMFS_CODE_IGNORED;
		goto smf_stats;
	}
#endif /* BAND5G */

#ifdef WLAUTHRESP_MAC_FILTER
	/* Suppress auth resp by MAC filter if authresp_macfltr is enabled */
	if (AP_BSSCFG_CUBBY(appvt, bsscfg)->authresp_macfltr) {
		addr_match = wlc_macfltr_addr_match(wlc->macfltr, bsscfg, &hdr->sa);
		if ((addr_match == WLC_MACFLTR_ADDR_DENY) ||
			(addr_match == WLC_MACFLTR_ADDR_NOT_ALLOW)) {
			WL_ASSOC(("wl%d: auth resp to %s suppressed by MAC filter\n",
			wlc->pub->unit, sa));
#if defined(WLC_MACFLTR_STATS)
			wlc_macfltr_authcount_incr(wlc->macfltr, bsscfg, &hdr->sa);
#endif /* WLC_MACFLTR_STATS */
			wlc_bss_mac_event(wlc, bsscfg, WLC_E_PRUNE,
				&hdr->sa, 0, WLC_E_PRUNE_AUTH_RESP_MAC, 0, 0, 0);
			status = SMFS_CODE_IGNORED;
			goto smf_stats;
		}
	}
#endif /* WLAUTHRESP_MAC_FILTER */

#ifdef WL11AX
	if (BSSCFG_IS_BLOCK_HE_MAC(bsscfg)) {
		if (wlc_isblocked_hemac(bsscfg, &hdr->sa)) {
			WL_ASSOC((" wl%d.%d: auth response blocked \n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			return;
		}
	}
#endif /* WL11AX */

	scb = wlc_scbfind(wlc, bsscfg, (struct ether_addr *)&hdr->sa);
	fc = ltoh16(hdr->fc);
	if (fc & FC_WEP) {
		/* frame is protected, assume shared key */
		auth_alg = DOT11_SHARED_KEY;
		auth_seq = 3;

		/* Check if it is logical to have an encrypted packet here */
		if (scb == NULL) {
			WL_ERROR(("wl%d: %s: auth resp frame encrypted"
				  "from unknown STA %s\n", wlc->pub->unit, __FUNCTION__, sa));
			status = SMFS_CODE_MALFORMED;
			goto smf_stats;
		}

		ap_scb = AP_SCB_CUBBY(appvt, scb);
		ASSERT(ap_scb != NULL);

		if (ap_scb->challenge == NULL) {
			WL_ERROR(("wl%d: %s: auth resp frame encrypted "
				"with no challenge recorded from %s\n",
				wlc->pub->unit, __FUNCTION__, sa));
			status = SMFS_CODE_MALFORMED;
			goto smf_stats;
		}

		/* Processing a WEP encrypted AUTH frame:
		 * BSS config0 is allowed to use the HW default keys, all other BSS configs require
		 * software decryption of AUTH frames.  For simpler code:
		 *
		 * If the frame has been decrypted(a default HW key is present at the right index),
		 * always re-encrypt the frame with the key used by HW and then use the BSS config
		 * specific WEP key to decrypt. This means that all WEP encrypted AUTH frames will
		 * be decrypted in software.
		 */

		WL_ASSOC(("wl%d: %s: received wep from %sassociated scb\n",
		          wlc->pub->unit, __FUNCTION__, SCB_ASSOCIATED(scb) ? "" : "non-"));

		WLPKTTAGSCBSET(p, scb);

		key_id = body[3] >> DOT11_KEY_INDEX_SHIFT;
		key = wlc_keymgmt_get_bss_key(wlc->keymgmt, bsscfg, key_id, NULL);

		/* if the frame was incorrectly decrypted with default bss by h/w, rx
		 * processing will encrypt it back and find the right key and
		 * decrypt it.
		 */
		err = wlc_key_rx_mpdu(key, p, rxh);
		if (err != BCME_OK) {
			WL_ASSOC(("wl%d.%d: %s: rx from %s failed with error %d\n",
				WLCWLUNIT(wlc),  WLC_BSSCFG_IDX(bsscfg),
				__FUNCTION__, sa, err));

			/* free the challenge text */
			MFREE(wlc->osh, ap_scb->challenge, 2 + ap_scb->challenge[1]);
			ap_scb->challenge = NULL;
			status = DOT11_SC_AUTH_CHALLENGE_FAIL;
			goto send_result;
		}

		WL_ASSOC(("wl%d: %s: ICV pass : %s: BSSCFG = %d\n",
			WLCWLUNIT(wlc), __FUNCTION__, sa, WLC_BSSCFG_IDX(bsscfg)));

		/* Skip IV */
		body += DOT11_IV_LEN;
		body_len -= DOT11_IV_LEN;

		/* Remove ICV */
		body_len -= DOT11_ICV_LEN;
	}
#ifdef MFP
	else if (WLC_MFP_ENAB(wlc->pub) && (scb) && SCB_MFP(scb) && SCB_AUTHENTICATED(scb) &&
		!SCB_MARKED_DEL(scb) &&
		(ltoh16(((struct dot11_auth *)body)->alg) == DOT11_OPEN_SYSTEM)) {
		/* flush is needed on re-assoc, ampdu deactivation, etc. */
		scb_ampdu_tx_flush(wlc->ampdu_tx, scb);
		/* send auth packet */
		wlc_sendauth(bsscfg, &scb->ea, &bsscfg->target_bss->BSSID, scb,
			DOT11_OPEN_SYSTEM, 2, DOT11_SC_SUCCESS, NULL, short_preamble, NULL, NULL);
		return;
	}
#endif /* MFP */

	auth = (struct dot11_auth *)body;
	auth_alg = ltoh16(auth->alg);
	auth_seq = ltoh16(auth->seq);

	/* if sequence number would be out of range, do nothing.
	 * And we are not supporting any auth_alg > DOT11_SAE now
	 */
	if (auth_seq >= 4 || auth_alg > DOT11_SAE) {
		status = SMFS_CODE_MALFORMED;
		WL_ERROR(("wl%d: %s: Invalid Auth alg (%d) "
			"Or invalid seq (%d) %s\n", wlc->pub->unit,
			__FUNCTION__, auth_alg, auth_seq, sa));
		goto smf_stats;
	}

	if (wlc_is_valid_auth_frame(bsscfg, auth_alg) != TRUE) {
		status = SMFS_CODE_MALFORMED;
		WL_ERROR(("wl%d: %s: bsscfg->WPA_auth (%d) and Auth alg (%d) %s "
			"Mismatching\n", wlc->pub->unit, __FUNCTION__,
			bsscfg->WPA_auth, auth_alg, sa));
		goto smf_stats;
	}

	body += sizeof(struct dot11_auth);
	body_len -= sizeof(struct dot11_auth);

	if (fc & FC_WEP)
		goto parse_ies;

	if (auth_seq == 1) {
		/* free all scbs for this sta
		 * PR38890:Check if scb for the STA already exists, if so then
		 * free it to resurrect scb from the scratch.
		 */
		FOREACH_WLC_BAND(wlc, i) {
			FOREACH_BSS(wlc, j, cfg) {
				scb = wlc_scbfindband(wlc, cfg, (struct ether_addr *)&hdr->sa, i);
				if (scb) {
					WL_ASSOC(("wl%d: %s: scb for the STA-%s already exists"
						"\n", wlc->pub->unit, __FUNCTION__, sa));
#ifdef MFP
					if (wlc_is_scb_need_saquery(wlc, cfg, scb,
						auth_alg)) {
						WL_ASSOC(("wl%d: %s: Sending SA Query to %s"
							"\n", wlc->pub->unit, __FUNCTION__, sa));

						wlc_mfp_start_sa_query(wlc->mfp, bsscfg, scb);
						goto smf_stats;
					} else
#endif /* MFP */
					{
						/* Notify HOST with a Deauth, this will ensure
						 * clearing any flow-rings (PCIe case) created
						 * during previous association of the SCB
						 */
						wlc_ap_authresp_deauth(wlc, scb);
						/* call early to cleanup any pkts in common q
						 * for scb before wlc_scbfree; then record
						 * remaining pktpend cnt.
						 */
						WLPKTTAGSCBSET(p, NULL);
						wlc_scbfree(wlc, scb);
						scb = NULL;
					}
				}
			} /* FOREACH_BSS */
		} /* FOREACH_WLC_BAND */
	}

	switch (auth_seq) {
	/* case 1 covers both SAE and other WPA2 auth including FBT */
	case 1:
		/* allocate an scb */
		if (!(scb = wlc_scblookup(wlc, bsscfg, (struct ether_addr *) &hdr->sa))) {
			/* To address new requirement i.e. Send Auth and Assoc response
			 * with reject status for client if client is in Mac deny list
			 * of AP.
			 * Send auth response with status "insufficient bandwidth"
			 */
			addr_match = wlc_macfltr_addr_match(wlc->macfltr, bsscfg,
				(struct ether_addr*)&hdr->sa);

			if ((addr_match == WLC_MACFLTR_ADDR_DENY) ||
				(addr_match == WLC_MACFLTR_ADDR_NOT_ALLOW)) {

				WL_ERROR(("wl%d: %s: Reject auth request from %s suppressed"
					" by MAC filter\n", wlc->pub->unit, __FUNCTION__, sa));
				wlc_sendauth(bsscfg, &hdr->sa, &bsscfg->BSSID, NULL, auth_alg,
					auth_seq + 1, DOT11_SC_INSUFFICIENT_BANDWIDTH, NULL,
					short_preamble, NULL, NULL);
				wlc_bss_mac_event(wlc, bsscfg, WLC_E_ASSOC_FAIL, &hdr->sa,
					WLC_E_STATUS_SUCCESS, DOT11_SC_INSUFFICIENT_BANDWIDTH,
					auth_alg, body_start, body_len_start);
				break;
			} else {
				WL_ERROR(("wl%d: %s: out of scbs for %s\n",  wlc->pub->unit,
					__FUNCTION__, sa));
				status = DOT11_SC_FAILURE;
				break;
			}
		}

		if (scb->flags & SCB_MYAP) {
			if (APSTA_ENAB(wlc->pub)) {
				WL_APSTA(("wl%d: Reject AUTH request from AP %s\n",
				          wlc->pub->unit, sa));
				status = DOT11_SC_FAILURE;
				break;
			}
			scb->flags &= ~SCB_MYAP;
		}

		wlc_scb_disassoc_cleanup(wlc, scb);

		/* auth_alg is coming from the STA, not us */
		scb->auth_alg = (uint8)auth_alg;
		switch (auth_alg) {
		case DOT11_OPEN_SYSTEM:

			if ((WLC_BSSCFG_AUTH(bsscfg) == DOT11_OPEN_SYSTEM) && (!status)) {
				wlc_scb_setstatebit(wlc, scb, AUTHENTICATED);
			} else {
				wlc_scb_clearstatebit(wlc, scb, AUTHENTICATED);
			}

			/* At this point, we should have at least one valid authentication in open
			 * system
			 */
			if (!(SCB_AUTHENTICATED(scb))) {
				WL_ERROR(("wl%d: %s: Open System auth attempted "
					  "from %s but only Shared Key supported\n",
					  wlc->pub->unit, __FUNCTION__, sa));
				status = DOT11_SC_AUTH_MISMATCH;
				wlc_bss_mac_event(wlc, bsscfg, WLC_E_PRUNE, &hdr->sa, 0,
					WLC_E_PRUNE_ENCR_MISMATCH, 0, 0, 0);
			}
			break;
		case DOT11_SHARED_KEY:
			break;
		case DOT11_FAST_BSS:
			break;
#ifdef WL_SAE
		case DOT11_SAE:
			break;
#endif /* WL_SAE */
		default:
			WL_ERROR(("wl%d: %s: unhandled algorithm %d from %s\n",
				wlc->pub->unit, __FUNCTION__, auth_alg, sa));
			status = DOT11_SC_AUTH_MISMATCH;
			wlc_bss_mac_event(wlc, bsscfg, WLC_E_PRUNE, &hdr->sa, 0,
				WLC_E_PRUNE_ENCR_MISMATCH, 0, 0, 0);
			break;
		}
		break;

#ifdef WL_SAE
	case WL_SAE_CONFIRM:
		if (scb == NULL) {
			WL_ERROR(("wl%d: %s: scb is NULL for %s\n",
				wlc->pub->unit, __FUNCTION__, sa));
			status = DOT11_SC_FAILURE;
			break;
		} else {
			scb->auth_alg = (uint8)auth_alg;

			if (!(SCB_AUTHENTICATING(scb))) {
				WL_ERROR(("wl%d: %s: Unsolicited Confirm frame auth = %d\n",
					wlc->pub->unit, __FUNCTION__, auth_alg));
				status = WLC_E_STATUS_UNSOLICITED;
				goto smf_stats;
			}

			switch (auth_alg) {
			case DOT11_SAE:
				break;
			default:
				WL_ERROR(("wl%d: %s: unhandled algorithm %d \n",
					wlc->pub->unit, __FUNCTION__, auth_alg));
				status = DOT11_SC_AUTH_MISMATCH;
				wlc_bss_mac_event(wlc, bsscfg, WLC_E_PRUNE, &hdr->sa, 0,
					WLC_E_PRUNE_ENCR_MISMATCH, 0, 0, 0);
				break;
			}
		}
		break;
#endif /* WL_SAE */

	default:

		WL_ERROR(("wl%d: %s: unexpected authentication sequence %d from %s\n",
			wlc->pub->unit, __FUNCTION__, auth_seq, sa));
		status = DOT11_SC_AUTH_SEQ;
		break;
	}

	if (scb == NULL) {
		goto smf_stats;
	}

parse_ies:
#if defined(BCMHWA) && defined(HWA_RXFILL_BUILD)
	/* Update auth Rx timestamp.
	 * Ctrl/Mgmt packets are from fifo-2 and wl dpc handle it but
	 * Data packets are handled by hwa dpc.  So SW may process a
	 * NULL data packet between auth and assoc which cause SW
	 * to send disassoc to the station.
	 * WAR: We drop the stall data packets between scb auth and assoc state.
	 */

	/* RxTsfTimeH is overloaded with aggregation stats for data frames so check for that */
	if (FC_TYPE(fc) != FC_TYPE_DATA) {
		wlc_read_tsf(wlc, &scb->rx_auth_tsf_reference, NULL);

		scb->rx_auth_tsf = (uint16)D11RXHDR_GE129_ACCESS_VAL(rxh, RxTsfTimeH);
		scb->rx_auth_tsf = scb->rx_auth_tsf << 16;
		scb->rx_auth_tsf |= (uint16)D11RXHDR_GE129_ACCESS_VAL(rxh, RxTSFTime);
	} else {
		ASSERT(0);
	}
#endif /* BCMHWA && HWA_RXFILL_BUILD */

#ifdef WL_SAE
	/* If running SAE algorithm, by-pass tests that might not apply */
	if ((bsscfg->WPA_auth & (WPA3_AUTH_SAE_PSK | WPA3_AUTH_SAE_FBT)) &&
		(auth_alg == DOT11_SAE)) {

		/* sae_parse_auth will send auth frame to authenticator. */
		status = wlc_sae_parse_auth(bsscfg, hdr, body_start, body_len_start);
		if (status == DOT11_SC_SUCCESS) {
			/* set scb state to PENDING_AUTH */
			wlc_scb_setstatebit(wlc, scb, PENDING_AUTH);
			wlc_bss_mac_event(wlc, bsscfg, WLC_E_AUTH, &scb->ea, WLC_E_STATUS_SUCCESS,
				DOT11_SC_SUCCESS, auth_alg, body_start, body_len_start);
		}
		goto smf_stats;
	}
#endif /* WL_SAE */

	if (status != DOT11_SC_SUCCESS) {
		WL_INFORM(("wl%d: %s: skip IE parse, status %u\n",
			wlc->pub->unit, __FUNCTION__, status));
		goto send_result;
	}

	/* prepare IE mgmt calls */
	wlc_iem_parse_upp_init(wlc->iemi, &upp);
	bzero(&ftpparm, sizeof(ftpparm));
	ftpparm.auth.alg = auth_alg;
	ftpparm.auth.seq = auth_seq;
	ftpparm.auth.scb = scb;
	ftpparm.auth.status = status;
	bzero(&pparm, sizeof(pparm));
	pparm.ft = &ftpparm;

	/* parse IEs */
	if (wlc_iem_parse_frame(wlc->iemi, bsscfg, FC_AUTH, &upp, &pparm,
	                        body, body_len) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_iem_parse_frame failed\n",
		          wlc->pub->unit, __FUNCTION__));
		/* Don't bail out, send response... */
	}
	status = ftpparm.auth.status;

send_result:
	if ((status == DOT11_SC_SUCCESS) && (auth_alg == DOT11_FAST_BSS) &&
		SCB_AUTHENTICATING(scb)) {
#ifdef RXCHAIN_PWRSAVE
		wlc_reset_rxchain_pwrsave_mode(ap);
#endif /* RXCHAIN_PWRSAVE */
	} else {
		wlc_ap_sendauth(ap, bsscfg, scb, auth_alg, auth_seq + 1, status, short_preamble);
	}

smf_stats:
	WL_ASSOC_LT(("%s: status %d\n", __FUNCTION__, status));
	if (BSS_SMFS_ENAB(wlc, bsscfg)) {
		(void)wlc_smfs_update(wlc->smfs, bsscfg, SMFS_TYPE_AUTH, status);
	}
} /* wlc_ap_authresp */

static void wlc_ap_process_assocreq_done(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg,
	uint8 reject_data, wlc_assoc_req_t *param);
static void wlc_ap_process_assocreq_exit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
	wlc_assoc_req_t *param);

static void wl_smf_stats(wlc_ap_info_pvt_t *appvt, wlc_bsscfg_t *bsscfg, wlc_assoc_req_t *param)
{
	wlc_info_t *wlc = appvt->wlc;

	BCM_REFERENCE(wlc);

	if (BSS_SMFS_ENAB(wlc, bsscfg)) {
		uint8 type;

		type = (param->reassoc ? SMFS_TYPE_REASSOC : SMFS_TYPE_ASSOC);
		(void)wlc_smfs_update(wlc->smfs, bsscfg, type, param->status);
	}
}

/* verify if the STA capabilities meet the BSS requirements */
static bool
wlc_ap_bss_membership_verify(wlc_info_t *wlc, wlc_bsscfg_t *cfg, struct scb *scb)
{
	wlc_mbsp_sel_t mbsp_req;

	if ((mbsp_req = wlc_bss_membership_get(wlc, cfg)) == 0) {
		return TRUE;
	}

	if ((mbsp_req & WLC_MBSP_SEL_HT) && !SCB_HT_CAP(scb)) {
		return FALSE;
	}
	if ((mbsp_req & WLC_MBSP_SEL_VHT) && !SCB_VHT_CAP(scb)) {
		return FALSE;
	}
	if ((mbsp_req & WLC_MBSP_SEL_HE) && !SCB_HE_CAP(scb)) {
		return FALSE;
	}

	return TRUE;
}

static bool
wlc_ap_assreq_verify_rates(wlc_assoc_req_t *param, wlc_iem_ft_pparm_t *ftpparm,
	opmode_cap_t opmode_cap_reqd, uint16 capability, wlc_rateset_t *req_rates)
{
	struct scb *scb = param->scb;
	wlc_bsscfg_t *bsscfg = param->bsscfg;
	wlc_info_t *wlc = bsscfg->wlc;
	wlc_bss_info_t *current_bss = bsscfg->current_bss;
	opmode_cap_t opmode_cap_curr = OMC_NONE;
	bool erp_sta;
	uint8 req_rates_lookup[WLC_MAXRATE+1];
	uint i;
	uint8 r;
#if defined(BCMDBG_ERR) || defined(WLMSG_ASSOC)
	struct dot11_management_header *hdr = param->hdr;
	char eabuf[ETHER_ADDR_STR_LEN], *sa = bcm_ether_ntoa(&hdr->sa, eabuf);
#endif
	ht_cap_ie_t *ht_cap_p = NULL;

	/* get the requester's rates into a lookup table and record ERP capability */
	erp_sta = FALSE;
	bzero(req_rates_lookup, sizeof(req_rates_lookup));
	for (i = 0; i < req_rates->count; i++) {
		r = req_rates->rates[i] & RATE_MASK;
		if ((r > WLC_MAXRATE) || (rate_info[r] == 0)) {
			continue;
		}
		req_rates_lookup[r] = r;
		if (RATE_ISOFDM(r))
			erp_sta = TRUE;
	}

	if (erp_sta)
		opmode_cap_curr = OMC_ERP;

	/* update the scb's capability flags */
	scb->flags &= ~(SCB_NONERP | SCB_LONGSLOT | SCB_SHORTPREAMBLE);
	if (wlc->band->gmode && !erp_sta)
		scb->flags |= SCB_NONERP;
	if (wlc->band->gmode && (!(capability & DOT11_CAP_SHORTSLOT)))
		scb->flags |= SCB_LONGSLOT;
	if (capability & DOT11_CAP_SHORT)
		scb->flags |= SCB_SHORTPREAMBLE;

	/* check the required rates */
	for (i = 0; i < current_bss->rateset.count; i++) {
		/* check if the rate is required */
		r = current_bss->rateset.rates[i];
		if (r & WLC_RATE_FLAG) {
			if (req_rates_lookup[r & RATE_MASK] == 0) {
				/* a required rate was not available */
				WL_ERROR(("wl%d: %s does not support required rate %d\n",
				        wlc->pub->unit, sa, r & RATE_MASK));
				return FALSE;
			}
		}
	}

	/* verify mcs basic rate settings */
	/* XXX need to rearchitect wlc_ht_update_scbstate() and wlc_vht_update_scbstate()
	 * to take relevant IEs independently in order to move these validations/updates
	 * into each IEs' handlers.
	 */
	if (N_ENAB(wlc->pub)) {
		/* find the HT cap IE, if found copy the mcs set into the requested rates */
		if (ftpparm->assocreq.ht_cap_ie != NULL) {
			ht_cap_p = wlc_read_ht_cap_ie(wlc, ftpparm->assocreq.ht_cap_ie,
			        TLV_HDR_LEN + ftpparm->assocreq.ht_cap_ie[TLV_LEN_OFF]);

			wlc_ht_update_scbstate(wlc->hti, scb, ht_cap_p, NULL, NULL);
			if (ht_cap_p != NULL) {
				bcopy(ht_cap_p->supp_mcs, req_rates->mcs, MCSSET_LEN);
				opmode_cap_curr = OMC_HT;
			}
		}

#ifdef WL11AC
		if (VHT_ENAB_BAND(wlc->pub, wlc->band->bandtype)) {
			vht_cap_ie_t *vht_cap_p = NULL;
			vht_cap_ie_t vht_cap;
			vht_op_ie_t *vht_op_p = NULL;
			vht_op_ie_t vht_op;

			if (ftpparm->assocreq.vht_cap_ie != NULL)
				vht_cap_p = wlc_vht_copy_cap_ie(wlc->vhti,
					ftpparm->assocreq.vht_cap_ie,
				        TLV_HDR_LEN + ftpparm->assocreq.vht_cap_ie[TLV_LEN_OFF],
				        &vht_cap);
			if (ftpparm->assocreq.vht_op_ie != NULL)
				vht_op_p =  wlc_vht_copy_op_ie(wlc->vhti,
					ftpparm->assocreq.vht_op_ie,
				        TLV_HDR_LEN + ftpparm->assocreq.vht_op_ie[TLV_LEN_OFF],
				        &vht_op);

			wlc_vht_update_scb_state(wlc->vhti, wlc->band->bandtype, scb,
			                vht_cap_p, vht_op_p, ftpparm->assocreq.vht_ratemask);

			if (vht_cap_p != NULL)
				opmode_cap_curr = OMC_VHT;
		}
#endif /* WL11AC */
#ifdef WL11AX
		if (HE_ENAB_BAND(wlc->pub, wlc->band->bandtype)) {
			wlc_he_update_scb_state(wlc->hei, wlc->band->bandtype, scb,
				(he_cap_ie_t *)	ftpparm->assocreq.he_cap_ie, NULL,
				(he_6g_band_caps_ie_t*)ftpparm->assocreq.he_6g_caps_ie);
		}
		if (SCB_HE_CAP(scb))
			opmode_cap_curr = OMC_HE;
#endif /* WL11AX */
	}

	/*
	 * Verify whether the operation capabilities of current STA
	 * acceptable for this BSS based on required setting.
	 */
	if (opmode_cap_curr < opmode_cap_reqd) {
		WL_ASSOC(("wl%d: %s: Assoc is rejected due to oper capability "
			"mode mismatch. Reqd mode=%d, client's mode=%d\n",
			wlc->pub->unit, sa, opmode_cap_reqd, opmode_cap_curr));
		/*
		 * XXX: the standard doesn't define a status codes for this case, so
		 * returning this one.
		 */
		return FALSE;
	}

	/* verify if the STA meets bss membership selector requirements */
	if (!wlc_ap_bss_membership_verify(wlc, bsscfg, scb)) {
		WL_ASSOC(("wl%d: mismatch Membership from (Re)Assoc Request packet from %s\n",
		          wlc->pub->unit, sa));
		return FALSE;
	}

	return TRUE;
} /* wlc_ap_assreq_verify_rates */

static bool
wlc_ap_assreq_verify_authmode(wlc_info_t *wlc, struct scb *scb, wlc_bsscfg_t *bsscfg,
	uint16 capability, bool akm_ie_included, struct dot11_management_header *hdr)
{

#if defined(BCMDBG_ERR)
	char eabuf[ETHER_ADDR_STR_LEN], *sa = bcm_ether_ntoa(&hdr->sa, eabuf);
#endif
	if ((bsscfg->WPA_auth != WPA_AUTH_DISABLED && WSEC_ENABLED(bsscfg->wsec)) ||
		WSEC_SES_OW_ENABLED(bsscfg->wsec)) {
		/* WPA/RSN IE is parsed in the registered IE mgmt callbacks and
		 * the scb->WPA_auth and scb->wsec should have been setup by now,
		 * otherwise it signals an abnormality in the STA assoc req frame.
		 */
		if (scb->WPA_auth == WPA_AUTH_DISABLED || !WSEC_ENABLED(scb->wsec)) {
			/* check for transition mode */
			if (!WSEC_WEP_ENABLED(bsscfg->wsec) && !WSEC_SES_OW_ENABLED(bsscfg->wsec)) {
				WL_ERROR(("wl%d: %s: "
				          "deny transition mode assoc req from %s... "
				          "transition mode not enabled on AP\n",
				          wlc->pub->unit, __FUNCTION__, sa));
				return FALSE;
			}
		}
		WL_WSEC(("wl%d: %s: %s WPA_auth 0x%x\n", wlc->pub->unit, __FUNCTION__, sa,
		         scb->WPA_auth));
	}

	/* check the capabilities.
	 * In case OW_ENABLED, allow privacy bit to be set even if !WSEC_ENABLED if
	 * there is no wpa or rsn IE in request.
	 * This covers Microsoft doing WPS association with sec bit set even when we are
	 * in open mode..
	 */
	/* by far scb->WPA_auth should be setup if the STA has sent us appropriate request */
	if ((capability & DOT11_CAP_PRIVACY) && !WSEC_ENABLED(bsscfg->wsec) &&
	    (!WSEC_SES_OW_ENABLED(bsscfg->wsec) || akm_ie_included)) {
		WL_ERROR(("wl%d: %s is requesting privacy but encryption is not enabled on the"
		        " AP. SES OW %d WPS IE %d\n", wlc->pub->unit, sa,
		        WSEC_SES_OW_ENABLED(bsscfg->wsec), akm_ie_included));
		wlc_bss_mac_event(wlc, bsscfg, WLC_E_PRUNE, &hdr->sa,
			0, WLC_E_PRUNE_ENCR_MISMATCH, 0, 0, 0);
		return FALSE;
	}

	return TRUE;
} /* wlc_ap_assreq_verify_authmode */

/* respond to association and reassociation requests */
void
wlc_ap_process_assocreq(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg,
	wlc_rx_data_desc_t *rx_data_desc, struct scb *scb, bool short_preamble)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	ap_bsscfg_cubby_t *ap_cfg;
	struct dot11_management_header *hdr = rx_data_desc->hdr;
	wlc_d11rxhdr_t *wrxh = rx_data_desc->wrxh;
	uint8 *body = rx_data_desc->body;
	uint body_len = rx_data_desc->body_len;
#if defined(BCMDBG_ERR) || defined(WLMSG_ASSOC)
	char eabuf[ETHER_ADDR_STR_LEN], *sa = bcm_ether_ntoa(&hdr->sa, eabuf);
#endif
	struct dot11_assoc_req *req = (struct dot11_assoc_req *) body;
	wlc_rateset_t req_rates;
	wlc_rateset_t sup_rates, ext_rates;
	bool reassoc;
	uint16 capability;
	uint16 status = DOT11_SC_SUCCESS;
	int idx;
	assoc_decision_t decision;
	uint8 reject_data;
	wlc_assoc_req_t * param;
	/* Used to encapsulate data to a generate event */
	bool akm_ie_included = FALSE;
	uint16 type;
	opmode_cap_t opmode_cap_reqd;
	wlc_iem_upp_t upp;
	wlc_iem_pparm_t pparm;
	wlc_iem_ft_pparm_t ftpparm;
	uint8 *tlvs;
	uint tlvs_len;
	ap_scb_cubby_t *ap_scb;
	wlc_supp_channels_t supp_chan;
	wlc_supp_channels_t *supp_chan_cubby;
	bool free_scb = FALSE;
	sta_vendor_oui_t assoc_oui;

	if (scb == NULL)
		return;

	ap_scb = AP_SCB_CUBBY(appvt, scb);
	ASSERT(ap_scb != NULL);

	WL_TRACE(("wl%d: %s\n", wlc->pub->unit, __FUNCTION__));

	ASSERT(bsscfg != NULL);
	ASSERT(bsscfg->up);
	ASSERT(BSSCFG_AP(bsscfg));
	ASSERT(bsscfg == scb->bsscfg);

	ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);
	opmode_cap_reqd = ap_cfg->opmode_cap_reqd;

	param = AS_SCB_CUBBY(appvt, scb);
	/*
	 * ignore the req if there is one pending, based on the assumption
	 * that param was zeroed on allocation (as part of scbpub in wlc_userscb_alloc)
	 *
	 * free this memory in case host misses this event, or never respond
	 */
	if (param->body != NULL) {
		wl_smf_stats(appvt, bsscfg, param);
		return;
	}

	bzero(param, sizeof(*param));
	reject_data = 0;

	param->buf_len = body_len + sizeof(struct dot11_management_header);
	param->body = MALLOC(wlc->osh, param->buf_len);
	if (param->body == NULL) {
		WL_ERROR(("wl%d: wlc_process_assocreq - no memory for len %d\n",
			wlc->pub->unit, body_len));
		return;
	}
	bcopy(body, param->body, body_len);
	param->hdr = (struct dot11_management_header *)((char *)param->body + body_len);
	bcopy(hdr, param->hdr, sizeof(struct dot11_management_header));

	body = param->body;

	param->bsscfg = bsscfg;
	param->body_len = body_len;
	param->scb = scb;
	param->short_preamble = short_preamble;

#ifdef RXCHAIN_PWRSAVE
	/* fast switch back from rxchain_pwrsave state upon association */
	wlc_reset_rxchain_pwrsave_mode(ap);
#endif /* RXCHAIN_PWRSAVE */

	bzero(&req_rates, sizeof(req_rates));
	param->reassoc = ((ltoh16(hdr->fc) & FC_KIND_MASK) == FC_REASSOC_REQ);
	reassoc = param->reassoc;
	if (SCB_AUTHENTICATED(scb) && SCB_ASSOCIATED(scb) && SCB_MFP(scb)) {
		status = DOT11_SC_ASSOC_TRY_LATER;
		goto done;
	}
	param->listen = ltoh16(req->listen);

	/* PR38742 WAR: do not process auth frames while AP scan is in
	 * progress, it may cause frame going out on a different
	 * band/channel
	 */
	if (SCAN_IN_PROGRESS(wlc->scan)) {
		WL_ASSOC(("wl%d: AP Scan in progress, abort association\n", wlc->pub->unit));
		status = DOT11_SC_ASSOC_BUSY_FAIL;
		goto exit;
	}

#ifdef BCMAUTH_PSK
	if (AP_ENAB(wlc->pub) && WSEC_TKIP_ENABLED(bsscfg->wsec) &&
		wlc_keymgmt_tkip_cm_enabled(wlc->keymgmt, bsscfg))
		return;
#endif /* BCMAUTH_PSK */

	if ((reassoc && body_len < DOT11_REASSOC_REQ_FIXED_LEN) ||
	    (!reassoc && body_len < DOT11_ASSOC_REQ_FIXED_LEN)) {
		status = DOT11_SC_FAILURE;
		goto exit;
	}

	/* set up some locals to hold info from the (re)assoc packet */
	if (!reassoc) {
		tlvs = body + DOT11_ASSOC_REQ_FIXED_LEN;
		tlvs_len = body_len - DOT11_ASSOC_REQ_FIXED_LEN;
	} else {
		tlvs = body + DOT11_REASSOC_REQ_FIXED_LEN;
		tlvs_len = body_len - DOT11_REASSOC_REQ_FIXED_LEN;
	}

	/* Init per scb WPA_auth and wsec.
	 * They will be reinitialized when parsing the AKM IEs.
	 */
	scb->WPA_auth = WPA_AUTH_DISABLED;
	scb->wsec = 0;

	/* send up all IEs in the WLC_E_ASSOC_IND/WLC_E_REASSOC_IND event */
	param->e_data = tlvs;
	param->e_datalen = tlvs_len;

	if (!bcm_valid_tlv((bcm_tlv_t *)tlvs, tlvs_len)) {
		status = DOT11_SC_FAILURE;
		goto exit;
	}

#ifdef WL11AX
	if (BSSCFG_BLOCK_HE_ENABLED(bsscfg)) {
		if (bcm_find_tlv_ext(tlvs, tlvs_len, 255, EXT_MNG_HE_CAP_ID) != NULL) {
			if (BSSCFG_IS_BLOCK_HE_MAC(bsscfg)) {
				wlc_addto_heblocklist(bsscfg, &scb->ea);
			}
			WL_ASSOC((" wl%d.%d: assoc response blocked \n",
				wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			goto exit;
		}
	}
#endif /* WL11AX */

#ifdef WL_MBSSID
	if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype) && wlc_ap_get_block_mbssid(wlc)) {
		dot11_extcap_ie_t *extcap_ie_tlv;
		dot11_extcap_t *cap;
		extcap_ie_tlv = (dot11_extcap_ie_t *)bcm_find_tlv(tlvs,
				tlvs_len, DOT11_MNG_EXT_CAP_ID);

		if (extcap_ie_tlv && extcap_ie_tlv->len >= DOT11_EXTCAP_LEN_MBSSID) {
			cap = (dot11_extcap_t*)extcap_ie_tlv->cap;
			if (!isset(cap->extcap, DOT11_EXT_CAP_MBSSID)) {
				WL_ASSOC((" wl%d.%d: assoc  blocked as"
						"sta doesn't support mbssid \n",
						wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
				goto exit;
			}
		} else {
			WL_ASSOC((" wl%d.%d: assoc  blocked as sta doesn't support mbssid \n",
					wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
				goto exit;
		}
	}
#endif /* WL_MBSSID */

	bzero(&assoc_oui, sizeof(assoc_oui));
	/* prepare IE mgmt parse calls */
	wlc_iem_parse_upp_init(wlc->iemi, &upp);
	bzero(&ftpparm, sizeof(ftpparm));
	ftpparm.assocreq.sup = &sup_rates;
	ftpparm.assocreq.ext = &ext_rates;
	ftpparm.assocreq.scb = scb;
	ftpparm.assocreq.status = status;
	ftpparm.assocreq.supp_chan = &supp_chan;
	ftpparm.assocreq.assoc_oui = &assoc_oui;
	bzero(&pparm, sizeof(pparm));
	pparm.ft = &ftpparm;
	pparm.ht = N_ENAB(wlc->pub);
#ifdef WL11AC
	pparm.vht = VHT_ENAB(wlc->pub);
#endif
#ifdef WL11AX
	pparm.he = HE_ENAB(wlc->pub);
#endif

	/* parse IEs */
	type = reassoc ? FC_REASSOC_REQ : FC_ASSOC_REQ;
	if (wlc_iem_parse_frame(wlc->iemi, bsscfg, type, &upp, &pparm, tlvs, tlvs_len) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_iem_parse_frame failed\n", wlc->pub->unit, __FUNCTION__));
		status = ftpparm.assocreq.status;

		if (status != DOT11_SC_SUCCESS) {
			/* Send an association response */
			goto done;
		}
	}
	status = ftpparm.assocreq.status;

	/* store the advertised capability field */
	capability = ltoh16(req->capability);
	scb->cap = capability;

	/* store the advertised supported channels list */
	supp_chan_cubby = SCB_SUPP_CHAN_CUBBY(appvt, scb);

	if (supp_chan_cubby) {
		bzero(supp_chan_cubby, sizeof(*supp_chan_cubby));
		if (supp_chan.count && (supp_chan.count <= WL_NUMCHANNELS)) {
			supp_chan_cubby->count = supp_chan.count;
			bcopy(supp_chan.chanset, supp_chan_cubby->chanset,
				sizeof(supp_chan.chanset));
		}
	}

	wlc_iem_store_sta_info_vendor_oui(wlc, scb, &assoc_oui);

	/* qualify WPS IE */
	if (ap_scb->wpaie != NULL && !(scb->flags & SCB_WPSCAP))
		akm_ie_included = TRUE;

	/* ===== Assoc Request Validations Start Here ===== */
	if ((ap_cfg->assoc_rej_rssi_thd) && ((wrxh->rssi < ap_cfg->assoc_rej_rssi_thd))) {
		reject_data = ap_cfg->assoc_rej_rssi_thd - wrxh->rssi;
		status = DOT11_SC_POOR_CHAN_CONDITION;
		goto done;
	}
	/* rates validation */
	if (wlc_combine_rateset(wlc, &sup_rates, &ext_rates, &req_rates) != BCME_OK ||
	    /* filter rateset by hwrs rates - also sort and sanitize them */
	    !wlc_rate_hwrs_filter_sort_validate(&req_rates /* [in+out] */,
			&wlc->band->hw_rateset /* [in] */, FALSE, wlc->stf->op_txstreams)) {
		WL_ASSOC(("wl%d: invalid rateset in (Re)Assoc Request "
		          "packet from %s\n", wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_RATE_MISMATCH;
		goto done;
	}
#ifdef WL11AX
	/* HE has its intersected rates stored in scb. Copy in req_rates so they dont get lost */
	wlc_rateset_he_cp(&req_rates, &scb->rateset);
#endif /* WL11AX */

	/* catch the case of an already assoc. STA */
	if (SCB_ASSOCIATED(scb)) {
		/* If we are in PS mode then return to non PS mode as there is a state mismatch
		 * between the STA and the AP
		 */
		if (SCB_PS(scb))
			wlc_apps_scb_ps_off(wlc, scb, TRUE);

#ifdef WLFBT
		if (scb->auth_alg != DOT11_FAST_BSS)
#endif /* WLFBT */
		{
			/* return STA to auth state and check the (re)assoc pkt */
			wlc_scb_clearstatebit(wlc, scb, ~AUTHENTICATED);
		}
	}

	/* check if we are authenticated w/ this particular bsscfg */
	/*
	  get the index for that bsscfg. Would be nicer to find a way to represent
	  the authentication status directly.
	*/
	idx = WLC_BSSCFG_IDX(bsscfg);
	if (idx < 0) {
		WL_ERROR(("wl%d: %s: association request for non existent bsscfg\n",
			wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_FAIL;
		free_scb = TRUE;
		goto done;
	}

	if (!SCB_AUTHENTICATED(scb)) {
		WL_ASSOC(("wl%d: %s: association request for non-authenticated station\n",
			wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_FAIL;
		goto done;
	}

	/* validate 11h */
	if (WL11H_ENAB(wlc) &&
	    (wlc_11h_get_spect(wlc->m11h) == SPECT_MNGMT_STRICT_11H) &&
	    ((capability & DOT11_CAP_SPECTRUM) == 0)) {

		/* send a failure association response to this STA */
		WL_REGULATORY(("wl%d: %s: association denied as spectrum management is required\n",
			wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_SPECTRUM_REQUIRED;
		goto done;
	}

#if BAND5G
	/* Previously detected radar and are in the
	 * process of switching to a new channel
	 */
	if (wlc->block_datafifo & DATA_BLOCK_QUIET) {
		/* send a failure association response to this node */
		WL_REGULATORY(("wl%d: %s: association denied while in radar avoidance mode\n",
			wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_FAIL;
		goto done;
	}
#endif /* BAND5G */
#ifdef WL11K_AP
	if (WL11K_ENAB(wlc->pub) && wlc_rrm_enabled(wlc->rrm_info, bsscfg)) {
		scb->flags3 &= ~SCB3_RRM;
		if (capability & DOT11_CAP_RRM)
			scb->flags3 |= SCB3_RRM;
	}
#endif /* WL11K_AP */

	/* get the requester's rates into a lookup table and record ERP capability */
	if (!wlc_ap_assreq_verify_rates(param, &ftpparm, opmode_cap_reqd, capability, &req_rates)) {
		status = DOT11_SC_ASSOC_RATE_MISMATCH;
		goto done;
	}
	/*
	 * If in an auth mode that wants a WPA info element, look for one.
	 * If found, check to make sure what it requests is supported.
	 */

	if (!wlc_ap_assreq_verify_authmode(wlc, scb, bsscfg, capability, akm_ie_included, hdr)) {
		status = DOT11_SC_ASSOC_FAIL;
		goto done;
	}
	/* When WEP is enabled along with the WPA we'll deny STA request,
	 * if STA attempts with WPA IE and shared key 802.11 authentication,
	 * by deauthenticating the STA...
	 */
	if ((bsscfg->WPA_auth != WPA_AUTH_DISABLED) && WSEC_ENABLED(bsscfg->wsec) &&
	    (WSEC_WEP_ENABLED(bsscfg->wsec))) {
		/*
		 * WPA with 802.11 open authentication is required, or 802.11 shared
		 * key authentication without WPA authentication attempt (no WPA IE).
		 */
		/* attempt WPA auth with 802.11 shared key authentication */
		if (scb->auth_alg == DOT11_SHARED_KEY && scb->WPA_auth != WPA_AUTH_DISABLED) {
			WL_ERROR(("wl%d: WPA auth attempt with 802.11 shared key auth from %s, "
			          "deauthenticating...\n", wlc->pub->unit, sa));
			(void)wlc_senddeauth(wlc, bsscfg, scb, &scb->ea, &bsscfg->BSSID,
			                     &bsscfg->cur_etheraddr, DOT11_RC_AUTH_INVAL, FALSE);
			status = DOT11_SC_ASSOC_FAIL;
			goto done;
		}
	}

	/* If not APSTA, deny association to stations unable to support 802.11b short preamble
	 * if short network.  In APSTA, we don't enforce short preamble on the AP side since it
	 * might have been set by the STA side.  We need an extra flag for enforcing short
	 * preamble independently.
	 */
	if (!APSTA_ENAB(wlc->pub) && BAND_2G(wlc->band->bandtype) &&
	    bsscfg->PLCPHdr_override == WLC_PLCP_SHORT && !(capability & DOT11_CAP_SHORT)) {
		WL_ERROR(("wl%d: %s does not support short preambles\n", wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_SHORT_REQUIRED;
		goto done;
	}
	/* deny association to stations unable to support 802.11g short slot timing
	 * if shortslot exclusive network
	 */
	if ((BAND_2G(wlc->band->bandtype)) &&
	    ap->shortslot_restrict && !(capability & DOT11_CAP_SHORTSLOT)) {
		WL_ERROR(("wl%d: %s does not support ShortSlot\n", wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_SHORTSLOT_REQUIRED;
		goto done;
	}
	/* check the max association limit */
	if (wlc_ap_stas_associated(wlc->ap) >= appvt->maxassoc) {
		WL_ERROR(("wl%d: %s denied association due to max association limit\n",
			wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_BUSY_FAIL;
		free_scb = TRUE;
		goto done;
	}

	if (SCB_MAP_CAP(scb)) {
		if (!SCB_MAP_P2_CAP(scb) && IS_MAP_PROF1_STA_ASSOC_DISALLOWED(bsscfg)) {
			WL_ERROR(("wl%d.%d: %s denied multiap backhaul sta association due to "
				"profile 1 disallowed being set on the bss.\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), sa));
			status = DOT11_SC_ASSOC_FAIL;
			free_scb = TRUE;
			goto done;
		}

		if (SCB_MAP_P2_CAP(scb) && IS_MAP_PROF2_STA_ASSOC_DISALLOWED(bsscfg)) {
			WL_ERROR(("wl%d.%d: %s denied multiap backhaul sta association due to "
				"profile 2 disallowed being set on the bss.\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), sa));
			status = DOT11_SC_ASSOC_FAIL;
			free_scb = TRUE;
			goto done;
		}
	}

	if ((SCB_MAP_CAP(scb) || SCB_DWDS_CAP(scb)) && wl_max_if_reached(wlc->wl)) {
		WL_ERROR(("wl%d: %s denied DWDS association due to max interface limit\n",
			wlc->pub->unit, sa));
		status = DOT11_SC_ASSOC_BUSY_FAIL;
		free_scb = TRUE;
		goto done;
	}
	if (BSSCFG_BLOCK_STA_ON_MAPBH(bsscfg) && (!SCB_MAP_CAP(scb))) {
		WL_ERROR(("wl%d.%d: not allowed for normal sta to join backhaul BSS\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		status = DOT11_SC_ASSOC_BUSY_FAIL;
		free_scb = TRUE;
		goto done;
	}

#if defined(MBSS) || defined(WLP2P)
	if (wlc_bss_assocscb_getcnt(wlc, bsscfg) >= bsscfg->maxassoc) {
		WL_ERROR(("wl%d.%d %s denied association due to max BSS association "
			"limit\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), sa));
		status = DOT11_SC_ASSOC_BUSY_FAIL;
		free_scb = TRUE;
		goto done;
	}
#endif /* MBSS || WLP2P */

#ifdef WLP2P
	if (BSS_P2P_ENAB(wlc, bsscfg)) {
		if (wlc_p2p_process_assocreq(wlc->p2p, scb, tlvs, tlvs_len) != BCME_OK) {
			status = DOT11_SC_ASSOC_FAIL;
			goto done;
		}
	}
#endif

#if defined(WL_MBO) && defined(MBO_AP)
	if ((BSS_MBO_ENAB(wlc, bsscfg)) && (wlc_mbo_reject_assoc_req(wlc, bsscfg))) {
		status = DOT11_SC_ASSOC_BUSY_FAIL;
		goto done;
	}
#endif /* WL_MBO && !WL_MBO_DISABLED && MBO_AP */

#ifdef PSTA
	/* Send WLC_E_PSTA_PRIMARY_INTF_IND event to indicate psta primary mac addr */
	if (ap_scb->psta_prim != NULL) {
		wl_psta_primary_intf_event_t psta_prim_evet;

		WL_ASSOC(("wl%d.%d scb:"MACF", psta_prim:"MACF"\n",
			wlc->pub->unit,	WLC_BSSCFG_IDX(bsscfg),
			ETHER_TO_MACF(scb->ea), ETHER_TO_MACF(ap_scb->psta_prim->ea)));

		memcpy(&psta_prim_evet.prim_ea, &ap_scb->psta_prim->ea, sizeof(struct ether_addr));
		wlc_bss_mac_event(wlc, bsscfg, WLC_E_PSTA_PRIMARY_INTF_IND,
			&scb->ea, WLC_E_STATUS_SUCCESS, 0, 0,
			&psta_prim_evet, sizeof(wl_psta_primary_intf_event_t));
	}
#endif /* PSTA */

	param->status = status;
	bcopy(&req_rates, &param->req_rates, sizeof(wlc_rateset_t));
	/* Copy supported mcs index bit map */
	bcopy(req_rates.mcs, scb->rateset.mcs, MCSSET_LEN);

#ifdef SPLIT_ASSOC
	if (SPLIT_ASSOC_REQ(bsscfg)) {
		/* indicate pre-(re)association event to OS and get association
		* decision from OS from the same thread
		* vif doesn't use this mac event and needs auto-reply of assoc req
		*/
		wlc_bss_mac_event(wlc, bsscfg,
			reassoc ? WLC_E_PRE_REASSOC_IND : WLC_E_PRE_ASSOC_IND,
			&scb->ea, WLC_E_STATUS_SUCCESS, status, 0, body, body_len);
		WL_ASSOC(("wl%d: %s: sa(%s) SPLIT reassoc(%d) systime(%u)\n",
			wlc->pub->unit, __FUNCTION__, sa, reassoc, OSL_SYSUPTIME()));
	} else
#endif /* SPLIT_ASSOC */
	{
		bzero(&decision, sizeof(decision));
		decision.assoc_approved = TRUE;
		decision.reject_reason = DOT11_SC_SUCCESS;
		bcopy(&scb->ea, &decision.da, ETHER_ADDR_LEN);
		wlc_ap_process_assocreq_decision(wlc, bsscfg, &decision);
	}

	return;

	/* ===== Assoc Request Validations End ===== */

done:
	param->status = status;
	wlc_ap_process_assocreq_done(ap, bsscfg, reject_data, param);

	if (free_scb) {
		/* Clear states and mark the scb for deletion. SCB free will happen
		 * from the inactivity timeout context in wlc_ap_scb_unusable_stas_timeout()
		 * Mark the scb for deletion first as some scb state change notify callback
		 * functions need to be informed that the scb is about to be deleted.
		 * (For example wlc_cfp_scb_state_upd)
		 */
		SCB_MARK_DEL(scb);
		wlc_scb_clearstatebit(wlc, scb, ASSOCIATED | AUTHORIZED);
	}

	return;

exit:
	wl_smf_stats(appvt, bsscfg, param);
	/* fall through */

	if (param->body) {
		MFREE(wlc->osh, param->body, param->buf_len);
		bzero(param, sizeof(*param));
	}

	return;
} /* wlc_ap_process_assocreq */

void
wlc_ap_process_assocreq_decision(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, assoc_decision_t *dc)
{
	wlc_ap_info_t *ap;
	scb_t *scb;
	wlc_assoc_req_t *param;
#if  defined(BCMDBG_ERR) || defined(BCMDBG) || defined(WLMSG_ASSOC)
	char sabuf[ETHER_ADDR_STR_LEN], *sa = bcm_ether_ntoa(&dc->da, sabuf);
#endif /* BCMDBG_ERR || BCMDBG || WLMSG_ASSOC */

	WL_TRACE(("wl%d: %s\n", wlc->pub->unit, __FUNCTION__));

	ASSERT(bsscfg != NULL);
	ASSERT(bsscfg->up);
	ASSERT(BSSCFG_AP(bsscfg));

	ap = wlc->ap;
	scb = wlc_scbfind(wlc, bsscfg, &dc->da);
	if (scb == NULL) {
		WL_ERROR(("wl%d.%d %s could not find scb\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), sa));
		return;
	}

	param = AS_SCB_CUBBY((wlc_ap_info_pvt_t *)ap, scb);
	if (!param || !param->body) {
		WL_ERROR(("wl%d.%d assocreq NULL CUBBY body\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		return;
	}

	if (scb != param->scb) {
		WL_ERROR(("wl%d.%d %s wrong scb found\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), sa));
		return;
	}

	if (SCB_MARKED_DEL(scb) || SCB_DEL_IN_PROGRESS(scb)) {
		return;
	}

	if (!dc->assoc_approved) {
		param->status = dc->reject_reason;
		WL_ERROR(("wl%d.%d %s denied association due to status(%d)\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), sa, dc->reject_reason));
		goto done;
	}

	/* Deauth could have happened, verify AUTH state */
	if (!SCB_AUTHENTICATED(scb)) {
		/* Ignore completely, while processing the assoc (via hostapd) the STA deauthed
		 * no response is to be sent at all
		 */
		return;
	}

	if (scb->state & PENDING_ASSOC) {
		/* This is response to pending assoc state. Clear state */
		wlc_scb_clearstatebit(wlc, scb, PENDING_ASSOC);

		/* STA has chosen SAE AKM? Is this result of hostapd reply? */
		if ((param->status == DOT11_SC_SUCCESS) &&
			((scb->WPA_auth == WPA3_AUTH_SAE_PSK) ||
			(scb->WPA_auth == WPA3_AUTH_DPP) ||
			(scb->WPA_auth == WPA_AUTH_OWE) ||
			(scb->WPA_auth == WPA3_AUTH_SAE_FBT))) {
			goto pending_assoc_done;
		}
	}

	/* WEP encryption */
	if ((scb->WPA_auth == WPA_AUTH_DISABLED) && (WSEC_WEP_ENABLED(bsscfg->wsec))) {
		scb->wsec = WEP_ENABLED;
	}

	/* XXX JQL
	 * we should have a callback mechanism to call registered scb cubby owners
	 * to reset any relevant data for the (re)association.
	 */
	wlc_ap_tpc_assoc_reset(wlc->tpc, scb);

	/* When a HT, VHT, or HE STA using TKIP or WEP only unicast cipher suite tries
	 * to associate, exclude HT, VHT, and HE IEs from assoc response to force
	 * the STA to operate in legacy mode.
	 */
	if (WSEC_ENABLED(bsscfg->wsec)) {
		bool keep_ht = TRUE;

		ASSERT(param->req_rates.count > 0);
		if (scb->wsec == TKIP_ENABLED &&
		    (wlc_ht_get_wsec_restriction(wlc->hti) & WLC_HT_TKIP_RESTRICT)) {
			keep_ht = FALSE;
		} else if (scb->wsec == WEP_ENABLED &&
		           (wlc_ht_get_wsec_restriction(wlc->hti) & WLC_HT_WEP_RESTRICT)) {
			keep_ht = FALSE;
		}

		if (!keep_ht) {
			if (N_ENAB(wlc->pub) && SCB_HT_CAP(scb))
				wlc_ht_update_scbstate(wlc->hti, scb, NULL, NULL, NULL);
#ifdef WL11AC
			if (VHT_ENAB_BAND(wlc->pub, wlc->band->bandtype) && SCB_VHT_CAP(scb))
				wlc_vht_update_scb_state(wlc->vhti, wlc->band->bandtype,
					scb, NULL, NULL, 0);
#endif /* WL11AC */
#ifdef WL11AX
			if (HE_ENAB_BAND(wlc->pub, wlc->band->bandtype) && SCB_HE_CAP(scb))
				wlc_he_update_scb_state(wlc->hei, wlc->band->bandtype,
					scb, NULL, NULL, NULL);
#endif
		}
	}

	wlc_prot_g_cond_upd(wlc->prot_g, scb);
	wlc_prot_n_cond_upd(wlc->prot_n, scb);

#ifdef RADIO_PWRSAVE
	if (RADIO_PWRSAVE_ENAB(wlc->ap) && wlc_radio_pwrsave_in_power_save(wlc->ap)) {
		wlc_radio_pwrsave_exit_mode(wlc->ap);
		WL_INFORM(("We have an assoc request, going right out of the power save mode!\n"));
	}
#endif

	if (wlc->band->gmode)
		wlc_prot_g_mode_upd(wlc->prot_g, bsscfg);

	if (N_ENAB(wlc->pub))
		wlc_prot_n_mode_upd(wlc->prot_n, bsscfg);

	WL_ASSOC(("AP: Checking if WEP key needs to be inserted\n"));
	/* If Multi-SSID is enabled, and Legacy WEP is in use for this bsscfg, a "pairwise" key must
	   be created by copying the default key from the bsscfg.
	*/
	if (bsscfg->WPA_auth == WPA_AUTH_DISABLED)
		WL_ASSOC(("WPA disabled \n"));

	/* Since STA is declaring privacy w/o WPA IEs => WEP */
	if ((scb->wsec == 0) && (scb->cap & DOT11_CAP_PRIVACY) &&
	    WSEC_WEP_ENABLED(bsscfg->wsec))
		scb->wsec = WEP_ENABLED;

	if (WSEC_WEP_ENABLED(bsscfg->wsec))
		WL_ASSOC(("WEP enabled \n"));
	if (MBSS_ENAB(wlc->pub))
		WL_ASSOC(("MBSS on \n"));
	if ((MBSS_ENAB(wlc->pub) || PSTA_ENAB(wlc->pub) || !BSSCFG_IS_PRIMARY(bsscfg)) &&
	    bsscfg->WPA_auth == WPA_AUTH_DISABLED && WSEC_WEP_ENABLED(bsscfg->wsec)) {
		wlc_key_t *key;
		wlc_key_info_t key_info;
		wlc_key_algo_t algo;
		wlc_key_t *scb_key;
		uint8 data[WEP128_KEY_SIZE];
		size_t data_len;
		int err;

		key = wlc_keymgmt_get_bss_tx_key(wlc->keymgmt, bsscfg, FALSE, &key_info);
		algo = key_info.algo;
		if (algo == CRYPTO_ALGO_OFF)
			goto defkey_done;

		WL_ASSOC(("Def key installed \n"));
		if  (algo != CRYPTO_ALGO_WEP1 && algo != CRYPTO_ALGO_WEP128)
			goto defkey_done;

		WL_ASSOC(("wl%d: %s Inserting key \n", WLCWLUNIT(wlc), sa));
		err = wlc_key_get_data(key, data, sizeof(data), &data_len);
		if (err != BCME_OK) {
			WL_ASSOC(("wl%d: %s error %d getting default key data, key idx %d\n",
				WLCWLUNIT(wlc), sa, err, key_info.key_idx));
			goto defkey_done;
		}

		scb_key = wlc_keymgmt_get_scb_key(wlc->keymgmt, scb,
			WLC_KEY_ID_PAIRWISE, WLC_KEY_FLAG_NONE, &key_info);
		ASSERT(scb_key != NULL);
		err = wlc_key_set_data(scb_key, algo, data, data_len);
		if (err != BCME_OK) {
			WL_ERROR(("wl%d.%d: Error %d inserting key for sa %s, key idx %d\n",
				WLCWLUNIT(wlc), WLC_BSSCFG_IDX(bsscfg), err, sa, key_info.key_idx));
		}
	}

defkey_done:

	/* scb->wsec is the specific unicast algo being used.
	 * It should be a subset of the whole bsscfg wsec
	 */
	ASSERT(!N_ENAB(wlc->pub) || ((bsscfg->wsec & scb->wsec) == scb->wsec));

	/* Based on wsec for STA, update AMPDU feature
	 * By spec, 11n device can send AMPDU only with Open or CCMP crypto
	 */
	if (N_ENAB(wlc->pub) && SCB_AMPDU(scb)) {
		if ((scb->wsec == WEP_ENABLED) ||
			(scb->wsec == TKIP_ENABLED) ||
			SCB_MFP(scb)) {
			wlc_scb_ampdu_disable(wlc, scb);
		} else {
			wlc_scb_ampdu_enable(wlc, scb);
		}
	}
done:
	if ((param->status == DOT11_SC_SUCCESS) &&
		((scb->WPA_auth == WPA3_AUTH_SAE_PSK) ||
		(scb->WPA_auth == WPA3_AUTH_DPP) ||
		(scb->WPA_auth == WPA_AUTH_OWE) ||
		(scb->WPA_auth == WPA3_AUTH_SAE_FBT))) {
		/* set scb state to PENDING_ASSOC */
		wlc_scb_setstatebit(wlc, scb, PENDING_ASSOC);
		if ((scb->WPA_auth & WPA3_AUTH_SAE_FBT) && (scb->auth_alg == DOT11_FAST_BSS)) {
			/* Send WLC_E_REASSOC event to cfg80211 layer */
			wlc_bss_mac_event(wlc, bsscfg, WLC_E_REASSOC, &scb->ea,
					WLC_E_STATUS_SUCCESS, 0, scb->auth_alg, param->e_data,
					param->e_datalen);
		} else {
			/* Send WLC_E_ASSOC event to cfg80211 layer */
			wlc_bss_mac_event(wlc, bsscfg, WLC_E_ASSOC, &scb->ea,
					WLC_E_STATUS_SUCCESS, 0, scb->auth_alg, param->e_data,
					param->e_datalen);
		}
		return;
	}
pending_assoc_done:
	wlc_ap_process_assocreq_done(ap, bsscfg, dc->reject_data, param);

	return;
} /* wlc_ap_process_assocreq_decision */

/* ===== Prepare Assoc Response ===== */
static void
wlc_ap_process_assocreq_done(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg, uint8 reject_data,
	wlc_assoc_req_t *param)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	scb_t *scb = param->scb;
#if defined(BCMDBG) || defined(WLMSG_ASSOC)
	char eabuf[ETHER_ADDR_STR_LEN], *sa = bcm_ether_ntoa(&scb->ea, eabuf);
#endif
	wlc_rateset_t sup_rates, ext_rates;
	struct ether_addr *reassoc_ap = NULL;
	wlc_bss_info_t *current_bss;
	struct dot11_assoc_resp *resp;
	uint8 *rsp = NULL;
	uint rsp_len;
	uint8 rates;
	bool reassoc;
	uint8 mcsallow;
	wlc_iem_ft_cbparm_t ftcbparm;
	wlc_iem_cbparm_t cbparm;
	uint16 type;
	void *pkt;
	wlc_rateset_t *req_rates = &param->req_rates;
#ifdef MBO_AP
	int i;
	wlc_bsscfg_t *cfg;
#endif /* MBO_AP */

	current_bss = bsscfg->current_bss;
	reassoc = param->reassoc;

	if (reassoc) {
		struct dot11_reassoc_req *reassoc_req = (struct dot11_reassoc_req *) param->body;
		reassoc_ap = &reassoc_req->ap;
	}

	/* create the supported rates and extended supported rates elts */
	bzero(&sup_rates, sizeof(wlc_rateset_t));
	bzero(&ext_rates, sizeof(wlc_rateset_t));
	/* check for a supported rates override */
	if (wlc->sup_rates_override.count > 0)
		bcopy(&wlc->sup_rates_override, &sup_rates, sizeof(wlc_rateset_t));
	wlc_rateset_elts(wlc, bsscfg, &current_bss->rateset, &sup_rates, &ext_rates);

	/* filter rateset 'req_rates' for the BSS supported rates.  */
	wlc_rate_hwrs_filter_sort_validate(req_rates /* [in+out] */,
		&current_bss->rateset /* [in] */, FALSE, wlc->stf->op_txstreams);

	rsp_len = DOT11_ASSOC_RESP_FIXED_LEN;
	WL_ASSOC_LT(("%s status %d\n", __FUNCTION__, param->status));

	/* prepare IE mgmt calls */
	bzero(&ftcbparm, sizeof(ftcbparm));
	ftcbparm.assocresp.mcs = req_rates->mcs;
	ftcbparm.assocresp.scb = scb;
	ftcbparm.assocresp.status = param->status;
	if (param->status != DOT11_SC_SUCCESS)
		ftcbparm.assocresp.status_data = reject_data;
	ftcbparm.assocresp.sup = &sup_rates;
	ftcbparm.assocresp.ext = &ext_rates;
	bzero(&cbparm, sizeof(cbparm));
	cbparm.ft = &ftcbparm;
	cbparm.bandunit = CHSPEC_BANDUNIT(current_bss->chanspec);
	cbparm.ht = SCB_HT_CAP(scb);
#ifdef WL11AC
	cbparm.vht = SCB_VHT_CAP(scb) && (cbparm.bandunit != BAND_6G_INDEX);
#endif /* WL11AC */
#ifdef WL11AX
	cbparm.he = SCB_HE_CAP(scb);
#endif

	/* calculate IEs' length */
	type = reassoc ? FC_REASSOC_RESP : FC_ASSOC_RESP;
	rsp_len += wlc_iem_calc_len(wlc->iemi, bsscfg, type, NULL, &cbparm);

	/* alloc a packet */
	if ((pkt = wlc_frame_get_mgmt(wlc, type, &scb->ea, &bsscfg->cur_etheraddr,
	                              &bsscfg->BSSID, rsp_len, &rsp)) == NULL) {
		param->status = DOT11_SC_ASSOC_BUSY_FAIL;
		WL_ASSOC(("wl%d.%d %s %sassociation failed rsp_len %d\n", wlc->pub->unit,
			WLC_BSSCFG_IDX(bsscfg), sa, reassoc ? "re" : "", rsp_len));
		rsp_len = 0;
		goto smf_stats;
	}
	ASSERT(rsp && ISALIGNED(rsp, sizeof(uint16)));
	resp = (struct dot11_assoc_resp *) rsp;

	/* fill out the association response body */
	resp->capability = DOT11_CAP_ESS;
	if (BAND_2G(wlc->band->bandtype) &&
	    bsscfg->PLCPHdr_override == WLC_PLCP_SHORT)
		resp->capability |= DOT11_CAP_SHORT;
	if (WSEC_ENABLED(bsscfg->wsec) && bsscfg->wsec_restrict)
		resp->capability |= DOT11_CAP_PRIVACY;
	if (wlc->shortslot && wlc->band->gmode)
		resp->capability |= DOT11_CAP_SHORTSLOT;

#ifdef WL11K_AP
	if (WL11K_ENAB(wlc->pub) && wlc_rrm_enabled(wlc->rrm_info, bsscfg))
		resp->capability |= DOT11_CAP_RRM;
#endif /* WL11K_AP */

	resp->capability = htol16(resp->capability);
	resp->status = htol16(param->status);
	/* Set the highest two bits (MSB) of the Association ID. This is to maintain
	 * compatibility with the Duration/ID field in the MAC header. Do NOTE: this is not
	 * explicitely written anymore in the latest specification, but it has to be done !!
	 */
	resp->aid = htol16(scb->aid | ~DOT11_AID_MASK);

	rsp += DOT11_ASSOC_RESP_FIXED_LEN;
	rsp_len -= DOT11_ASSOC_RESP_FIXED_LEN;

	/* write IEs in the frame */
	if (wlc_iem_build_frame(wlc->iemi, bsscfg, type, NULL, &cbparm,
	                        rsp, rsp_len) != BCME_OK) {
		WL_ERROR(("wl%d: %s: wlc_iem_build_frame failed\n",
		          wlc->pub->unit, __FUNCTION__));
		goto exit;
	}

#if defined(BCMDBG) || defined(WLMSG_PRPKT)
	WL_ASSOC(("wl%d: JOIN: sending %s RESP ...\n", WLCWLUNIT(wlc),
	          reassoc ? "REASSOC" : "ASSOC"));

	if (WL_ASSOC_ON()) {
		struct dot11_management_header *h =
		        (struct dot11_management_header *)PKTDATA(wlc->osh, pkt);
		uint l = PKTLEN(wlc->osh, pkt);
		wlc_print_assoc_resp(wlc, h, l);
	}
#endif
#ifdef WL_TRAFFIC_THRESH
	if (bsscfg->trf_scb_enable) {
		/* copying auto enable flags and zeroing data */
		if (!SCB_ASSOCIATED(scb)) {
			scb->trf_enable_flag = bsscfg->trf_scb_enable;
			memset(&scb->scb_trf_data[0], 0, sizeof(scb->scb_trf_data));
		}
	}
#endif /* WL_TRAFFIC_THRESH */
	/* send the association response */
	wlc_queue_80211_frag(wlc, pkt, bsscfg->wlcif->qi, scb, bsscfg,
		param->short_preamble, NULL, WLC_LOWEST_SCB_RSPEC(scb));

#ifdef BCM_CEVENT
	if (CEVENT_STATE(wlc->pub)) {
		wlc_send_cevent(wlc, bsscfg, SCB_EA(scb), param->status,
				0, 0, NULL, 0, CEVENT_D2C_MT_ASSOC_TX,
				CEVENT_D2C_FLAG_QUEUED | CEVENT_FRAME_DIR_TX);
	}
#endif /* BCM_CEVENT */

	/* ===== Post Processing ===== */

	/* XXX why we are doing these after initiating the Response TX???
	 * and what if the queuing of Response frame fail???
	 */

	rsp -= DOT11_ASSOC_RESP_FIXED_LEN;
	rsp_len += DOT11_ASSOC_RESP_FIXED_LEN;

#ifdef MFP
	if (WLC_MFP_ENAB(wlc->pub) && SCB_MFP(scb) &&
	    SCB_AUTHENTICATED(scb) && SCB_ASSOCIATED(scb)) {
		wlc_mfp_start_sa_query(wlc->mfp, bsscfg, scb);
		goto smf_stats;
	}
#endif

	/* if error, we're done */
	if (param->status != DOT11_SC_SUCCESS) {
		wlc_bss_mac_event(wlc, bsscfg, reassoc ? WLC_E_REASSOC_FAIL :
			WLC_E_ASSOC_FAIL, &scb->ea, WLC_E_STATUS_SUCCESS, param->status,
			scb->auth_alg, param->e_data, param->e_datalen);
		goto smf_stats;
	}

	/*
	 * FIXME: We should really set a callback and only update association state if
	 * the assoc resp packet tx is successful..
	 */

	/* update scb state */

	WL_ASSOC(("wl%d.%d %s %sassociated\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), sa,
		reassoc ? "re" : ""));

	/*
	 * scb->listen is used by the AP for timing out PS pkts,
	 * ensure pkts are held for at least one dtim period
	 */
	wlc_apps_set_listen_prd(wlc, scb, MAX(current_bss->dtim_period, param->listen));

	scb->assoctime = wlc->pub->now;

	wlc_scb_setstatebit(wlc, scb, ASSOCIATED);

	/* copy sanitized set to scb */
#ifdef WLP2P
	if (BSS_P2P_ENAB(wlc, bsscfg))
		rates = WLC_RATES_OFDM;
	else
#endif
	rates = WLC_RATES_CCK_OFDM;
	mcsallow = 0;

	if (BSS_N_ENAB(wlc, bsscfg) && SCB_HT_CAP(scb))
		mcsallow |= WLC_MCS_ALLOW_HT;
	if (BSS_VHT_ENAB(wlc, bsscfg) && SCB_VHT_CAP(scb))
		mcsallow |= WLC_MCS_ALLOW_VHT;
	if (WLPROPRIETARY_11N_RATES_ENAB(wlc->pub)) {
		if (wlc->pub->ht_features != WLC_HT_FEATURES_PROPRATES_DISAB &&
			SCB_HT_PROP_RATES_CAP(scb))
			mcsallow |= WLC_MCS_ALLOW_PROP_HT;
	}
	if ((mcsallow & WLC_MCS_ALLOW_VHT) &&
		WLC_VHT_FEATURES_GET(wlc->pub, WL_VHT_FEATURES_1024QAM) &&
		SCB_1024QAM_CAP(scb))
		mcsallow |= WLC_MCS_ALLOW_1024QAM;

	if (BSS_HE_ENAB(wlc, bsscfg) && SCB_HE_CAP(scb))
		mcsallow |= WLC_MCS_ALLOW_HE;

	/* req_rates => scb->rateset */
	wlc_rateset_filter(req_rates, &scb->rateset, FALSE, rates, RATE_MASK, mcsallow);

	/* re-initialize rate info
	 * Note that this wipes out any previous rate stats on the STA. Since this
	 * being called at Association time, it does not seem like a big loss.
	 */
	wlc_scb_ratesel_init(wlc, scb);

	/* Start beaconing if this is first STA */
	if (DYNBCN_ENAB(bsscfg) && wlc_bss_assocscb_getcnt(wlc, bsscfg) == 1) {
		wlc_bsscfg_bcn_enable(wlc, bsscfg);
	}

	/* notify other APs on the DS that this station has roamed */
	if (reassoc && bcmp((char*)&bsscfg->BSSID, reassoc_ap->octet, ETHER_ADDR_LEN))
		wlc_reassoc_notify(ap, &scb->ea, bsscfg);

	/* 802.11f assoc. announce pkt */
	wlc_assoc_notify(ap, &scb->ea, bsscfg);

#ifdef WLP2P
	if (BSS_P2P_ENAB(wlc, bsscfg))
		wlc_p2p_enab_upd(wlc->p2p, bsscfg);
#endif

	/* Enable BTCX PS protection */
	wlc_btc_set_ps_protection(wlc, bsscfg); /* enable */

exit:
	wlc_ap_process_assocreq_exit(wlc, bsscfg, param);
#ifdef MBO_AP
	/* check the max association limit */
	if (wlc_ap_stas_associated(wlc->ap) >= appvt->maxassoc) {
		FOREACH_UP_AP(wlc, i, cfg) {
			wlc_mbo_update_attr_assoc_disallowed(cfg,
				MBO_ASSOC_DISALLOWED_REASON_MAX_STA_LIMIT_REACHED);
		}
	} else if (wlc_bss_assocscb_getcnt(wlc, bsscfg) >= bsscfg->maxassoc) {
		wlc_mbo_update_attr_assoc_disallowed(bsscfg,
			MBO_ASSOC_DISALLOWED_REASON_MAX_STA_LIMIT_REACHED);
	}
#endif /* MBO_AP */

smf_stats:
	wl_smf_stats(appvt, bsscfg, param);

	MFREE(wlc->osh, param->body, param->buf_len);
	bzero(param, sizeof(*param));

	return;
} /* wlc_ap_process_assocreq_done */

/* decryption error detection and recovery */
static int
wlc_ap_wsec_decrypt_error_recovery(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	struct scb *scb;
	struct scb_iter scbiter;
	int number_of_authorized_sta = 0;
	int stas_in_decrypt_failure_states = 0;
	unsigned int success_delta;
	unsigned int failure_delta;
	wlc_scb_stats_t *scbstats;
	ap_scb_cubby_t *ap_scb;

	FOREACHSCB(wlc->scbstate, &scbiter, scb) {
		wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);

		/* Filter out the permanent SCB, AP SCB */
		if (scb->permanent ||
		    cfg == NULL || !BSSCFG_AP(cfg))
			continue;

		/* Filter out scbs not associated or marked for deletion */
		if (!SCB_ASSOCIATED(scb) || SCB_MARKED_DEL(scb))
			continue;

		/* if multiple scb see consistent decrypt errors recover using wlc_fatal_error
		 * for a single scb having error, de-auth after recovery error timeout
		 * spurious/random error (normal due to fcs not detecting packet error)
		 * ignored by the trigger
		 */

		if (WSEC_ENABLED(scb->bsscfg->wsec) && SCB_AUTHORIZED(scb)) {
			scbstats = &scb->scb_stats;
			ap_scb = AP_SCB_CUBBY(appvt, scb);

			if (!ap_scb)
				continue;

			number_of_authorized_sta++;
			/* find scb in decrypt error state stuck for 30 watchdog iterations */
			success_delta = scbstats->rx_decrypt_succeeds -
				ap_scb->rx_decrypt_succeeds_prev;
			ap_scb->rx_decrypt_succeeds_prev = scbstats->rx_decrypt_succeeds;
			failure_delta = scbstats->rx_decrypt_failures -
				ap_scb->rx_decrypt_failures_prev;
			ap_scb->rx_decrypt_failures_prev = scbstats->rx_decrypt_failures;

			/* but if we seen any successful decode, reset state */
			if (success_delta) {
				ap_scb->time_in_decrypt_failure_state = 0;
				continue;
			}
			if (ap_scb->time_in_decrypt_failure_state || failure_delta)
				ap_scb->time_in_decrypt_failure_state++;
			if (ap_scb->time_in_decrypt_failure_state >
				STA_DECRYPT_ERROR_DETECTION_TIMEOUT)
				stas_in_decrypt_failure_states++;
			if (ap_scb->time_in_decrypt_failure_state >
				STA_DECRYPT_ERROR_RECOVERY_TIMEOUT) {
				WL_ERROR(("wl%d: decryption error for sta, send deauth to sta "
					MACF"\n", wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
				wlc_scb_set_auth(wlc, scb->bsscfg, scb, FALSE, AUTHENTICATED,
					DOT11_RC_UNSPECIFIED);
				ap_scb->time_in_decrypt_failure_state = 0;
				stas_in_decrypt_failure_states--;
				WLCNTINCR(wlc->pub->_ap_cnt->stadecerr);
			}
		}
	}

	if (stas_in_decrypt_failure_states &&
		((number_of_authorized_sta == 1) ||
		(stas_in_decrypt_failure_states > 1))) {

		/* clear the SCB error state before resetting wl */
		FOREACHSCB(wlc->scbstate, &scbiter, scb) {
			wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);
			ap_scb = AP_SCB_CUBBY(appvt, scb);

			/* Filter out the permanent SCB, AP SCB */
			if (scb->permanent ||
				cfg == NULL || !BSSCFG_AP(cfg))
				continue;

			if (!ap_scb)
				continue;

			/* Filter out scbs not associated or marked for deletion */
			if (!SCB_ASSOCIATED(scb) || SCB_MARKED_DEL(scb))
				continue;

			if (WSEC_ENABLED(scb->bsscfg->wsec) && SCB_AUTHORIZED(scb)) {
				if (ap_scb->time_in_decrypt_failure_state)
					ap_scb->time_in_decrypt_failure_state =
						STA_DECRYPT_ERROR_RECOVERY_TIMEOUT;
			}
		}

		WL_ERROR(("wl%d: ap decryption error state detected\n",
			wlc->pub->unit));
		/* trigger a PSMWD if 'psmwd_reason' iovar is set with decerr */
		wlc_macdbg_psmwd_trigger(wlc->macdbg, WLC_MACDBG_PSMWD_DECERR);
		wlc_fatal_error(wlc);
		WLCNTINCR(wlc->pub->_ap_cnt->apdecerr);
		return BCME_ERROR;
	}
	return BCME_OK;
}

static void
wlc_ap_process_assocreq_exit(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, wlc_assoc_req_t *param)
{
	struct dot11_management_header *hdr = param->hdr;
	uint8 *ext_assoc_pkt;
	uint ext_assoc_pkt_len;
	scb_t *scb = param->scb;

	/* send WLC_E_REASSOC_IND/WLC_E_ASSOC_IND to interested App and/or non-WIN7 OS */
	/* Passing scb->WPA_auth intentionally to handle assoc offload case where
	 * we will drop this event in cfg80211 glue layer for such cases.
	 */
	wlc_bss_mac_event(wlc, bsscfg, param->reassoc ? WLC_E_REASSOC_IND : WLC_E_ASSOC_IND,
		&hdr->sa, WLC_E_STATUS_SUCCESS, param->status, scb->WPA_auth,
		param->e_data, param->e_datalen);
	/* Send WLC_E_ASSOC_REASSOC_IND_EXT to interested App */
	ext_assoc_pkt_len = sizeof(*hdr) + param->body_len;
	if ((ext_assoc_pkt = (uint8*)MALLOC(bsscfg->wlc->osh, ext_assoc_pkt_len)) == NULL) {
		WL_ERROR(("wl%d.%d: %s: MALLOC(%d) failed, malloced %d bytes\n",
			WLCWLUNIT(bsscfg->wlc), WLC_BSSCFG_IDX(bsscfg), __FUNCTION__,
			ext_assoc_pkt_len, MALLOCED(bsscfg->wlc->osh)));
	} else {
		memcpy(ext_assoc_pkt, hdr, sizeof(*hdr));
		memcpy(ext_assoc_pkt + sizeof(*hdr), param->body, param->body_len);

		wlc_bss_mac_event(wlc, bsscfg, WLC_E_ASSOC_REASSOC_IND_EXT,
			&hdr->sa, WLC_E_STATUS_SUCCESS, param->status, scb->auth_alg,
			ext_assoc_pkt, ext_assoc_pkt_len);
		MFREE(bsscfg->wlc->osh, ext_assoc_pkt, ext_assoc_pkt_len);
	}
	/* Suspend STA sniffing if it is associated to AP  */
	if (STAMON_ENAB(wlc->pub) && STA_MONITORING(wlc, &hdr->sa))
		wlc_stamon_sta_sniff_enab(wlc->stamon_info, &hdr->sa, FALSE);
#ifdef WLWNM_AP
	if (WLWNM_ENAB(wlc->pub) && WNM_MAXIDLE_ENABLED(wlc_wnm_get_cap(wlc, bsscfg)))
			wlc_wnm_rx_tstamp_update(wlc, scb);
#endif /* WLWNM_AP */
} /* wlc_ap_process_assocreq_exit */

int
wlc_sta_supp_chan(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, const struct ether_addr *ea,
	void *buf, int len)
{
	wlc_supp_channels_t* supp_chan = NULL;
	wlc_supp_chan_list_t supp_chan_list;
	struct scb *scb;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)(wlc->ap);

	ASSERT(ea != NULL);
	if (ea == NULL) {
		return (BCME_BADARG);
	}

	ASSERT(bsscfg != NULL);

	if ((scb = wlc_scbfind(wlc, bsscfg, ea)) == NULL) {
		return (BCME_BADADDR);
	}

	if (len < sizeof(supp_chan_list)) {
		return BCME_BUFTOOSHORT;
	}

	bzero(&supp_chan_list, sizeof(supp_chan_list));

	supp_chan_list.version = WL_STA_SUPP_CHAN_VER;

	supp_chan = SCB_SUPP_CHAN_CUBBY(appvt, scb);
	if (supp_chan) {
		supp_chan_list.count = supp_chan->count;
		bcopy(supp_chan->chanset, &(supp_chan_list.chanset),
			sizeof(supp_chan->chanset));
	}

	supp_chan_list.length =	sizeof(supp_chan_list);
	bcopy(&supp_chan_list, buf, sizeof(supp_chan_list));

	return BCME_OK;
}

#ifdef RXCHAIN_PWRSAVE
/*
 * Reset the rxchain power save related counters and modes
 */
void
wlc_reset_rxchain_pwrsave_mode(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;

	WL_INFORM(("Resetting the rxchain power save counters\n"));
	/* Come out of the power save mode if we are in it */
	if (appvt->rxchain_pwrsave.pwrsave.in_power_save) {
		wlc_stf_rxchain_set(wlc, appvt->rxchain_pwrsave.rxchain, TRUE);
		/* need to restore rx_stbc HT capability after exit rxchain_pwrsave mode */
		wlc_stf_exit_rxchain_pwrsave(wlc, appvt->rxchain_pwrsave.ht_cap_rx_stbc);
	}
	wlc_reset_pwrsave_mode(ap, PWRSAVE_RXCHAIN);
}

void
wlc_disable_rxchain_pwrsave(wlc_ap_info_t *ap)
{
	wlc_disable_pwrsave(ap, PWRSAVE_RXCHAIN);

	return;
}

uint8
wlc_rxchain_pwrsave_get_rxchain(wlc_info_t *wlc)
{
	uint8 rxchain = wlc->stf->rxchain;
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	if ((appvt != NULL) && appvt->rxchain_pwrsave.pwrsave.in_power_save) {
		rxchain = appvt->rxchain_pwrsave.rxchain;
	}
	return rxchain;
}

/*
 * get rx_stbc HT capability, if in rxchain_pwrsave mode, return saved rx_stbc value,
 * because rx_stbc capability may be changed when enter rxchain_pwrsave mode
 */
uint8
wlc_rxchain_pwrsave_stbc_rx_get(wlc_info_t *wlc)
{
	uint8 ht_cap_rx_stbc = wlc_ht_stbc_rx_get(wlc->hti);
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	if ((appvt != NULL) && appvt->rxchain_pwrsave.pwrsave.in_power_save) {
		ht_cap_rx_stbc = appvt->rxchain_pwrsave.ht_cap_rx_stbc;
	}
	return ht_cap_rx_stbc;
}

/*
 * Check whether we are in rxchain power save mode
 */
int
wlc_ap_in_rxchain_power_save(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;

	return (appvt->rxchain_pwrsave.pwrsave.in_power_save);
}
#endif /* RXCHAIN_PWRSAVE */

#ifdef RADIO_PWRSAVE
/*
 * Reset the radio power save related counters and modes
 */
static void
wlc_reset_radio_pwrsave_mode(wlc_ap_info_t *ap)
{
	uint8 dtim_period;
	uint16 beacon_period;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	wlc_bsscfg_t *bsscfg = wlc->primary_bsscfg;

	if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
		wlc_reset_rpsnoa(wlc);
		wlc_reset_pwrsave_mode(ap, PWRSAVE_RADIO);
		/* Set WAKE bit
		 * if in_power_save flag is unset, wake is set in AP mode
		 */
		wlc_set_wake_ctrl(wlc);
	} else {
		if (bsscfg->associated) {
			dtim_period = bsscfg->current_bss->dtim_period;
			beacon_period = bsscfg->current_bss->beacon_period;
		} else {
			dtim_period = wlc->default_bss->dtim_period;
			beacon_period = wlc->default_bss->beacon_period;
		}

		wl_del_timer(wlc->wl, appvt->radio_pwrsave.timer);
		wlc_reset_pwrsave_mode(ap, PWRSAVE_RADIO);
		appvt->radio_pwrsave.pwrsave_state = 0;
		appvt->radio_pwrsave.radio_disabled = FALSE;
		appvt->radio_pwrsave.cncl_bcn = FALSE;
		switch (appvt->radio_pwrsave.level) {
		case RADIO_PWRSAVE_LOW:
			appvt->radio_pwrsave.on_time = 2*beacon_period*dtim_period/3;
			appvt->radio_pwrsave.off_time = beacon_period*dtim_period/3;
			break;
		case RADIO_PWRSAVE_MEDIUM:
			appvt->radio_pwrsave.on_time = beacon_period*dtim_period/2;
			appvt->radio_pwrsave.off_time = beacon_period*dtim_period/2;
			break;
		case RADIO_PWRSAVE_HIGH:
			appvt->radio_pwrsave.on_time = beacon_period*dtim_period/3;
			appvt->radio_pwrsave.off_time = 2*beacon_period*dtim_period/3;
			break;
		default:
			ASSERT(0);
			break;
		}
	}
}
#endif /* RADIO_PWRSAVE */

#if defined(RXCHAIN_PWRSAVE) || defined(RADIO_PWRSAVE)
/*
 * Reset power save related counters and modes
 */
void
wlc_reset_pwrsave_mode(wlc_ap_info_t *ap, int type)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_pwrsave_t *pwrsave = NULL;
	int enable = 0;

	switch (type) {
#ifdef RXCHAIN_PWRSAVE
		case PWRSAVE_RXCHAIN:
			enable = ap->rxchain_pwrsave_enable;
			pwrsave = &appvt->rxchain_pwrsave.pwrsave;
			if (pwrsave) {
				if (!enable)
					pwrsave->power_save_check &= ~PWRSAVE_ENB;
				else
					pwrsave->power_save_check |= PWRSAVE_ENB;
			}
			break;
#endif
#ifdef RADIO_PWRSAVE
		case PWRSAVE_RADIO:
			enable = ap->radio_pwrsave_enable;
			pwrsave = &appvt->radio_pwrsave.pwrsave;
			appvt->radio_pwrsave.pwrsave_state = 0;
			if (pwrsave)
				pwrsave->power_save_check = enable;
			break;
#endif
		default:
			ASSERT(0);
			break;
	}

	WL_INFORM(("Resetting the rxchain power save counters\n"));
	if (pwrsave) {
		pwrsave->quiet_time_counter = 0;
		pwrsave->in_power_save = FALSE;
	}
}

#ifdef RXCHAIN_PWRSAVE
static void
wlc_disable_pwrsave(wlc_ap_info_t *ap, int type)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_pwrsave_t *pwrsave = NULL;

	switch (type) {
#ifdef RXCHAIN_PWRSAVE
		case PWRSAVE_RXCHAIN:
			pwrsave = &appvt->rxchain_pwrsave.pwrsave;
			if (pwrsave)
				pwrsave->power_save_check &= ~PWRSAVE_ENB;
			break;
#endif
#ifdef RADIO_PWRSAVE
		case PWRSAVE_RADIO:
			pwrsave = &appvt->radio_pwrsave.pwrsave;
			appvt->radio_pwrsave.pwrsave_state = 0;
			if (pwrsave)
				pwrsave->power_save_check = FALSE;
			break;
#endif
		default:
			break;
	}

	WL_INFORM(("Disabling power save mode\n"));
	if (pwrsave)
		pwrsave->in_power_save = FALSE;
}

#ifdef WDS
/* Detect if WDS or DWDS is configured */
static bool
wlc_rxchain_wds_detection(wlc_info_t *wlc)
{
	struct scb_iter scbiter;
	struct scb *scb;
	wlc_bsscfg_t *bsscfg;
	int32 idx;

	FOREACH_BSS(wlc, idx, bsscfg) {
		FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
			if ((SCB_WDS(scb) != NULL) || (SCB_DWDS(scb)))
				return TRUE;
		}
	}
	return FALSE;
}
#endif /* WDS */
#endif /* RXCHAIN_PWRSAVE */

#endif /* RXCHAIN_PWRSAVE || RADIO_PWRSAVE */

#ifdef WL_TRAFFIC_THRESH
static void
wlc_traffic_thresh_mgmt(wlc_info_t *wlc)
{
	wlc_bsscfg_t *bsscfg;
	int32 idx;
	int i;
	if (wlc->pub->_traffic_thresh) {
		FOREACH_BSS(wlc, idx, bsscfg) {
			wlc_trf_data_t *ptr = bsscfg->trf_cfg_data;
			wlc_intfer_params_t  *params;
			if (bsscfg->traffic_thresh_enab) {
				for (i = 0; i < WL_TRF_MAX_QUEUE; i++) {
					params = &bsscfg->trf_cfg_params[i];
					if ((bsscfg->trf_cfg_enable & (1 << i)) &&
						(ptr[i].num_data != NULL)) {
						(ptr[i].cur)++;
						if (ptr[i].cur >= params->num_secs) {
							ptr[i].cur = 0;
						}
						ptr[i].count -= ptr[i].num_data[ptr[i].cur];
						ptr[i].num_data[ptr[i].cur] = 0;
					}
				}
			}
		}
	}
}
#endif /* WL_TRAFFIC_THRESH */

bool
wlc_apsta_on_radar_channel(wlc_ap_info_t *ap)
{
	ASSERT(ap);
	return ((wlc_ap_info_pvt_t *)ap)->ap_sta_onradar;
}

#ifdef WLDFS
static bool
wlc_ap_on_radarchan(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	int ap_idx;
	wlc_bsscfg_t *ap_cfg;
	bool ap_onradar = FALSE;

	if (AP_ACTIVE(wlc)) {
		FOREACH_UP_AP(wlc, ap_idx, ap_cfg) {
			wlc_bss_info_t *current_bss = ap_cfg->current_bss;

			if (wlc_radar_chanspec(wlc->cmi, current_bss->chanspec) == TRUE) {
				ap_onradar = TRUE;
				break;
			}
		}
	}

	return ap_onradar;
}
#endif /* WLDFS */

#ifdef STA
/* Check if any local AP/STA chanspec overlap with STA/local AP
 * radar chanspec.
 */
void
wlc_ap_sta_onradar_upd(wlc_bsscfg_t *cfg)
{
	wlc_info_t *wlc = cfg->wlc;
	wlc_bss_info_t *current_bss = cfg->current_bss;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) wlc->ap;
	bool dfs_slave_present = FALSE;

	/* Only when on radar channel we look for overlapping AP + STA.
	 * The moment we switch to non-radar we turn off dfs_slave_present.
	 */
	if (wlc_radar_chanspec(wlc->cmi, current_bss->chanspec) == TRUE)
	{
		int ap_idx;
		wlc_bsscfg_t *ap_cfg;

		FOREACH_UP_AP(wlc, ap_idx, ap_cfg) {
			int sta_idx;
			wlc_bsscfg_t *sta_cfg;
			wlc_bss_info_t *ap_bss = ap_cfg->current_bss;
			uint8 ap_ctlchan = wf_chspec_ctlchan(ap_bss->chanspec);

			FOREACH_AS_STA(wlc, sta_idx, sta_cfg) {
				wlc_bss_info_t *sta_bss = sta_cfg->current_bss;
				uint8 sta_ctlchan = wf_chspec_ctlchan(sta_bss->chanspec);

				if (ap_ctlchan == sta_ctlchan) {
					dfs_slave_present = TRUE;
					break;
				}
			}

			if (dfs_slave_present) break;
		}
	}

	appvt->ap_sta_onradar = dfs_slave_present;
}
#endif /* STA */

void
wlc_restart_ap(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	int i;
	wlc_bsscfg_t *bsscfg;
#ifdef RADAR
	wlc_bsscfg_t *bsscfg_ap = NULL;
	uint bss_radar_flags = 0;
#endif /* RADAR */

	WL_TRACE(("wl%d: %s:\n", wlc->pub->unit, __FUNCTION__));

	if (wlc->state == WLC_STATE_GOING_DOWN || !wlc->pub->up) {
		WL_ERROR(("wl%d: %s: wl is going down or is not up\n",
			wlc->pub->unit, __FUNCTION__));
		return;
	}

	if (APSTA_ENAB(wlc->pub) && (MAP_ENAB(wlc->primary_bsscfg) ||
			DWDS_ENAB(wlc->primary_bsscfg) ||
#if defined(WET) || defined(WET_DONGLE)
			(WET_ENAB(wlc) || WET_DONGLE_ENAB(wlc)) ||
#endif /* WET || WET_DONGLE */
			(wlc->primary_bsscfg->_psta)) && wlc->primary_bsscfg->disable_ap_up) {
		/* Defer the bsscfg_up operation for AP cfg, as primary STA cfg
		 * interface is either not up or roaming to find out the target beacon,
		 * this defer operation enabled by roam_complete routine once STA
		 * interface losses the number of beacons greater than threshold
		 */
		goto bring_down_aps;
	}

#if defined(STA) && defined(AP)
	/* Check if it is feasible to
	* bringup the AP.
	*/
	if (APSTA_ENAB(wlc->pub) && !wlc_apup_allowed(wlc)) {
		goto bring_down_aps;
	}
#endif /* STA && AP */

#ifdef RADAR
	if (RADAR_ENAB(wlc->pub) && WL11H_AP_ENAB(wlc)) {
		FOREACH_AP(wlc, i, bsscfg) {
			bss_radar_flags |= (bsscfg)->flags & (WLC_BSSCFG_SRADAR_ENAB |
				WLC_BSSCFG_AP_NORADAR_CHAN |
				WLC_BSSCFG_USR_RADAR_CHAN_SELECT);
			if (bsscfg_ap == NULL) { /* store the first AP cfg */
				bsscfg_ap = bsscfg;
			}

			/* Make sure these flags are are never combined together
			 * either within single ap bss or across all ap-bss.
			 */
			ASSERT(!(bss_radar_flags & (bss_radar_flags - 1)));
		}

		/* No random channel selection in any of bss_radar_flags modes */
		if (bsscfg_ap != NULL && !bss_radar_flags &&
		    (wlc_channel_locale_flags_in_band(wlc->cmi, BAND_5G_INDEX) & WLC_DFS_TPC)) {
			wlc_dfs_sel_chspec(wlc->dfs, TRUE, bsscfg_ap);
		}
	}
#endif /* RADAR */

	appvt->pre_tbtt_us = (MBSS_ENAB(wlc->pub)) ? MBSS_PRE_TBTT_DEFAULT_us : PRE_TBTT_DEFAULT_us;
	/* Bring up any enabled AP configs which aren't up yet */
	FOREACH_AP(wlc, i, bsscfg) {
		if (bsscfg->enable) {
			uint wasup = bsscfg->up;
			WL_APSTA_UPDN(("wl%d: wlc_restart_ap -> wlc_bsscfg_up on bsscfg %d%s\n",
			               appvt->pub->unit, i, (bsscfg->up ? "(already up)" : "")));
			/* Clearing association state to let the beacon phyctl0
			 * and phyctl1 parameters be updated in shared memory.
			 * The phyctl0 and phyctl1 would be cleared from shared
			 * memory after the big hammer is executed.
			 * The wlc_bsscfg_up function below will update the
			 * associated state accordingly.
			 */
			bsscfg->associated = 0;
			if ((BCME_OK != wlc_bsscfg_up(wlc, bsscfg)) && wasup) {
				wlc_bsscfg_down(wlc, bsscfg);
			}
		}
	}
#ifdef RXCHAIN_PWRSAVE
	wlc_reset_rxchain_pwrsave_mode(ap);
#endif
#ifdef RADIO_PWRSAVE
	wlc_reset_radio_pwrsave_mode(ap);
#endif

	BCM_REFERENCE(ap);
	BCM_REFERENCE(bsscfg);
	return;

bring_down_aps:
	FOREACH_UP_AP(wlc, i, bsscfg) {
		wlc_bsscfg_down(wlc, bsscfg);
		WL_ASSOC(("wl%d.%d : %s: bring down bsscfg %d .\n", wlc->pub->unit,
			WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, i));
	}
	wlc->aps_associated = 0;
} /* wlc_restart_ap */

int
wlc_bss_up(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	wlc_bss_info_t *target_bss = bsscfg->target_bss;
	int ret;

	WL_TRACE(("wl%d: %s:\n", wlc->pub->unit, __FUNCTION__));
	ASSERT(bsscfg);
	ASSERT(BSSCFG_AP(bsscfg));

	/* Adjust target bss rateset according to target channel bandwidth */
	wlc_rateset_ht_bw_mcs_filter(&target_bss->rateset,
		WL_BW_CAP_40MHZ(wlc->band->bw_cap)?CHSPEC_WLC_BW(target_bss->chanspec):0);

	/* Register the operating channel on MSCH scheduler. */
	ret = wlc_ap_timeslot_register(bsscfg);
	/* Other init configuration related with radio
	 * channel status will get called by 1st MSCH ON_CHAN
	 */
	return ret;
}

/* This function only cover before MSCH ON_CHAN schedule.
 * wlc_ap_down() will handle rest of down configurations
 */
int
wlc_bss_down(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg)
{
	int ret = BCME_OK;
	UNUSED_PARAMETER(ap);

	/* Unregister the requested MSCH operating channel */
	wlc_ap_timeslot_unregister(bsscfg);
	return ret;
}

static void
wlc_ap_up_upd(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg, bool state)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	bsscfg_state_upd_data_t st_data;
	DBGONLY(char chanbuf[CHANSPEC_STR_LEN]; )

	bzero(&st_data, sizeof(st_data));
	st_data.cfg = bsscfg;
	st_data.old_enable = bsscfg->enable;
	st_data.old_up = bsscfg->up;

	/* Assume MAC is suspended already. DO NOT suspend MAC here */
	ASSERT(BSSCFG_AP(bsscfg));

	bsscfg->up = bsscfg->associated = state;

	WL_REGULATORY(("wl%d.%d: %s: chanspec %s, up %d\n", wlc->pub->unit,
		WLC_BSSCFG_IDX(bsscfg), __FUNCTION__,
		wf_chspec_ntoa(bsscfg->current_bss->chanspec, chanbuf), state));

	wlc->aps_associated = (uint8)AP_BSS_UP_COUNT(wlc);

	wlc_ap_sta_onradar_upd(bsscfg);

	wlc_mac_bcn_promisc(wlc);
	if ((wlc->aps_associated == 0) || ((wlc->aps_associated == 1))) {
		/* No need beyond 1 */
		wlc_bmac_enable_tbtt(wlc->hw, TBTT_AP_MASK,
			wlc->aps_associated ? TBTT_AP_MASK : 0);
	}
	wlc->pub->associated = wlc->aps_associated > 0 || wlc->stas_associated > 0;

	if (state) {
		uint8 constraint;
		/* Set the power limits for this locale after computing
		 * any 11h local tx power constraints.
		 */
		constraint = wlc_tpc_get_local_constraint_qdbm(wlc->tpc);
		wlc_channel_set_txpower_limit(wlc->cmi, constraint);
	}

#ifdef BCMPCIEDEV
	if (BCMPCIEDEV_ENAB()) {
		if (bsscfg->associated) {
			wlc_cfg_set_pmstate_upd(bsscfg, FALSE);
		} else {
			wlc_cfg_set_pmstate_upd(bsscfg, TRUE);
		}
	}
#endif /* BCMPCIEDEV */
	/* Mark beacon template as invalid; Just toggle respective bits */
	if (!state) {
		wlc_bmac_psm_maccommand(wlc->hw, (MCMD_BCN0VLD | MCMD_BCN1VLD));
	}

	wlc_bsscfg_notif_signal(bsscfg, &st_data);
} /* wlc_ap_up_upd */

/* known reassociation magic packets */

struct lu_reassoc_pkt {
	struct ether_header eth;
	struct dot11_llc_snap_header snap;
	uint8 unknown_field[2];
	uint8 data[36];
};

static
const struct lu_reassoc_pkt lu_reassoc_template = {
	{ { 0x01, 0x60, 0x1d, 0x00, 0x01, 0x00 },
	{ 0 },
	HTON16(sizeof(struct lu_reassoc_pkt) - sizeof(struct ether_header)) },
	{ 0xaa, 0xaa, 0x03, { 0x00, 0x60, 0x1d }, HTON16(0x0001) },
	{ 0x00, 0x04 },
	"Lucent Technologies Station Announce"
};

struct csco_reassoc_pkt {
	struct ether_header eth;
	struct dot11_llc_snap_header snap;
	uint8 unknown_field[4];
	struct ether_addr ether_dhost, ether_shost, a1, a2, a3;
	uint8 pad[4];
};
/* WES - I think the pad[4] at the end of the struct above should be
 * dropped, it appears to just be the ethernet padding to 64
 * bytes. This would fix the length calculation below, (no more -4).
 * It matches with the 0x22 field in the 'unknown_field' which appears
 * to be the length of the encapsulated packet starting after the snap
 * header.
 */

static
const struct csco_reassoc_pkt csco_reassoc_template = {
	{ { 0x01, 0x40, 0x96, 0xff, 0xff, 0x00 },
	{ 0 },
	HTON16(sizeof(struct csco_reassoc_pkt) - sizeof(struct ether_header) - 4) },
	{ 0xaa, 0xaa, 0x03, { 0x00, 0x40, 0x96 }, HTON16(0x0000) },
	{ 0x00, 0x22, 0x02, 0x02 },
	{ { 0x01, 0x40, 0x96, 0xff, 0xff, 0x00 } },
	{ { 0 } },
	{ { 0 } },
	{ { 0 } },
	{ { 0 } },
	{ 0x00, 0x00, 0x00, 0x00 }
};

bool
wlc_roam_check(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg, struct ether_header *eh, uint len)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	struct lu_reassoc_pkt *lu = (struct lu_reassoc_pkt *) eh;
	struct csco_reassoc_pkt *csco = (struct csco_reassoc_pkt *) eh;
	struct ether_addr *sta = NULL;
	struct scb *scb;

	/* check for Lucent station announce packet */
	if (!bcmp(eh->ether_dhost, (const char*)lu_reassoc_template.eth.ether_dhost,
	          ETHER_ADDR_LEN) &&
	    len >= sizeof(struct lu_reassoc_pkt) &&
	    !bcmp((const char*)&lu->snap, (const char*)&lu_reassoc_template.snap,
	              DOT11_LLC_SNAP_HDR_LEN))
		sta = (struct ether_addr *) lu->eth.ether_shost;

	/* check for Cisco station announce packet */
	else if (!bcmp(eh->ether_dhost, (const char*)csco_reassoc_template.eth.ether_dhost,
	               ETHER_ADDR_LEN) &&
	         len >= sizeof(struct csco_reassoc_pkt) &&
	         !bcmp((const char*)&csco->snap, (const char*)&csco_reassoc_template.snap,
	               DOT11_LLC_SNAP_HDR_LEN))
		sta = &csco->a1;

	/* not a magic packet */
	else
		return (FALSE);

	/* disassociate station */
	if ((scb = wlc_scbfind(wlc, bsscfg, sta)) && SCB_ASSOCIATED(scb)) {
		WL_ERROR(("wl%d: "MACF": roamed\n", wlc->pub->unit, ETHERP_TO_MACF(sta)));
		if (APSTA_ENAB(wlc->pub) && (scb->flags & SCB_MYAP)) {
			WL_APSTA(("wl%d: Ignoring roam report from my AP.\n", wlc->pub->unit));
			return (FALSE);
		}
		/* FIXME: (WES) Why are we sending a dissassoc here?
		 * If it just for wpa_msg, fix by moving wpa to WLC_E_DISASSOC* case
		 */

		wlc_senddisassoc(wlc, bsscfg, scb, sta, &bsscfg->BSSID,
		                 &bsscfg->cur_etheraddr, DOT11_RC_NOT_AUTH);
		wlc_scb_resetstate(wlc, scb);
		wlc_scb_setstatebit(wlc, scb, AUTHENTICATED);

		wlc_bss_mac_event(wlc, bsscfg, WLC_E_DISASSOC_IND, sta, WLC_E_STATUS_SUCCESS,
			DOT11_RC_DISASSOC_LEAVING, 0, 0, 0);
		/* Resume sniffing of this STA frames if the STA has been
	     * configured to be monitored before association.
	     */
		if (STAMON_ENAB(wlc->pub) && STA_MONITORING(wlc, sta))
			wlc_stamon_sta_sniff_enab(wlc->stamon_info, sta, TRUE);
	}

	return (TRUE);
} /* wlc_roam_check */

static void
wlc_assoc_notify(wlc_ap_info_t *ap, struct ether_addr *sta, wlc_bsscfg_t *cfg)
{
#ifdef WL_IAPP
	/* XXX, Why WL initial a "DATA" packet to Host?
	 * This 802.11f IAPP packet can trigger Host to update bridge ARP table
	 * so that M1 in new 4-way handshake for STA switch to other MBSS can
	 * work properly.
	 * This kind of notify packet need to use HWA_RPH_RESERVE_COUNT and
	 * we do support it now if HWA1A enabled.
	 */
	const uchar pkt80211f[] = {0x00, 0x01, 0xAF, 0x81, 0x01, 0x00};
	struct ether_header *eh;
	uint16 len = sizeof(pkt80211f);
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	void *p;

	if (!IAPP_ENAB(wlc->pub))
		return;

	/* prepare 802.11f IAPP announce packet. This should look like a wl rx packet since it sent
	   along the same path. Some work should be done to evaluate the real need for
	   extra headroom (how much), but the alignement enforced by wlc->hwrxoff_pktget must be
	   preserved.
	*/
	if ((p = PKTGET(wlc->osh, sizeof(pkt80211f) + BCMEXTRAHDROOM + wlc->hwrxoff_pktget +
		ETHER_HDR_LEN, FALSE)) == NULL)
		goto err;
	ASSERT(ISALIGNED(PKTDATA(wlc->osh, p), sizeof(uint32)));
	PKTPULL(wlc->osh, p, BCMEXTRAHDROOM + wlc->hwrxoff_pktget);

	eh = (struct ether_header *) PKTDATA(wlc->osh, p);
	bcopy(&ether_bcast, eh->ether_dhost, ETHER_ADDR_LEN);
	bcopy(sta, eh->ether_shost, ETHER_ADDR_LEN);
	eh->ether_type = hton16(len);
	bcopy(pkt80211f, &eh[1], len);
	WL_PRPKT("802.11f assoc", PKTDATA(wlc->osh, p), PKTLEN(wlc->osh, p));

	wlc_sendup_msdus(wlc, cfg, NULL, p);

	return;

err:
	WL_ERROR(("wl%d: %s: pktget error\n", wlc->pub->unit, __FUNCTION__));
	WLCNTINCR(wlc->pub->_cnt->rxnobuf);
	WLCNTINCR(cfg->wlcif->_cnt->rxnobuf);
#endif /* WL_IAPP */
	return;
} /* wlc_assoc_notify */

static void
wlc_reassoc_notify(wlc_ap_info_t *ap, struct ether_addr *sta, wlc_bsscfg_t *cfg)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	void *p;
	osl_t *osh;
	struct lu_reassoc_pkt *lu;
	struct csco_reassoc_pkt *csco;
	int len;

	osh = wlc->osh;

	/* prepare Lucent station announce packet */
	len = sizeof(struct lu_reassoc_pkt);
	if ((p = PKTGET(osh, len + BCMEXTRAHDROOM + wlc->hwrxoff_pktget +
		ETHER_HDR_LEN, FALSE)) == NULL)
		goto err;
	ASSERT(ISALIGNED(PKTDATA(osh, p), sizeof(uint32)));
	PKTPULL(osh, p, BCMEXTRAHDROOM + wlc->hwrxoff_pktget);

	lu = (struct lu_reassoc_pkt*) PKTDATA(osh, p);
	bcopy((const char*)&lu_reassoc_template, (char*) lu, sizeof(struct lu_reassoc_pkt));
	bcopy((const char*)sta, (char*)&lu->eth.ether_shost, ETHER_ADDR_LEN);
	WL_PRPKT("lu", PKTDATA(osh, p), PKTLEN(osh, p));

	wlc_sendup_msdus(wlc, cfg, NULL, p);

	/* prepare Cisco station announce packets */
	len = sizeof(struct csco_reassoc_pkt);
	if ((p = PKTGET(osh, len + BCMEXTRAHDROOM + wlc->hwrxoff_pktget +
		ETHER_HDR_LEN, FALSE)) == NULL)
		goto err;
	ASSERT(ISALIGNED(PKTDATA(osh, p), sizeof(uint32)));
	PKTPULL(osh, p, BCMEXTRAHDROOM + wlc->hwrxoff_pktget);

	csco = (struct csco_reassoc_pkt *) PKTDATA(osh, p);
	bcopy((const char*)&csco_reassoc_template, (char*)csco, len);
	bcopy((char*)sta, (char*)&csco->eth.ether_shost, ETHER_ADDR_LEN);
	bcopy((char*)sta, (char*)&csco->ether_shost, ETHER_ADDR_LEN);
	bcopy((char*)sta, (char*)&csco->a1, ETHER_ADDR_LEN);
	bcopy((char*)&cfg->BSSID, (char*)&csco->a2, ETHER_ADDR_LEN);
	bcopy((char*)&cfg->BSSID, (char*)&csco->a3, ETHER_ADDR_LEN);
	WL_PRPKT("csco1", PKTDATA(osh, p), PKTLEN(osh, p));

	wlc_sendup_msdus(wlc, cfg, NULL, p);

	/*
	 * Below packet causes L2 forward loop in the Linux kernel,
	 * because source mac addr is BSSID, as as WiFi interface's mac addr.
	 * Do we really need to send up this?
	 */

	return;

err:
	WL_ERROR(("wl%d: %s: pktget error\n", wlc->pub->unit, __FUNCTION__));
	WLCNTINCR(wlc->pub->_cnt->rxnobuf);
	WLCNTINCR(cfg->wlcif->_cnt->rxnobuf);
} /* wlc_reassoc_notify */

ratespec_t
wlc_lowest_basicrate_get(wlc_bsscfg_t *cfg)
{
	uint8 i, rate = 0;
	wlc_bss_info_t *current_bss = cfg->current_bss;

	for (i = 0; i < current_bss->rateset.count; i++) {
		if (current_bss->rateset.rates[i] & WLC_RATE_FLAG) {
			rate = current_bss->rateset.rates[i] & RATE_MASK;
			break;
		}
	}

	/* These are basic legacy rates */
	return LEGACY_RSPEC(rate);
}

static void
wlc_ap_probe_complete(wlc_info_t *wlc, void *pkt, uint txs)
{
	struct scb *scb;

	if ((scb = WLPKTTAGSCBGET(pkt)) == NULL)
		return;

	scb_sanitycheck(wlc->scbstate, scb);

#ifdef WDS
	if (SCB_LEGACY_WDS(scb))
		wlc_ap_wds_probe_complete(wlc, txs, scb);
	else
#endif
	wlc_ap_sta_probe_complete(wlc, txs, scb, pkt);
}

static int
wlc_ap_sendnulldata_cb(wlc_info_t *wlc, wlc_bsscfg_t *cfg, void *pkt, void *data)
{
	BCM_REFERENCE(wlc);
	BCM_REFERENCE(data);
	BCM_REFERENCE(cfg);

	/* register packet callback */
	WLF2_PCB1_REG(pkt, WLF2_PCB1_STA_PRB);
	/* reset expiry timer setting for keepalive packets */
	/* in congested environments these fail to go out and fail to tear down associated peer */
	WLF_RESET_EXP_TIME(pkt);
	return BCME_OK;
}

static void
wlc_ap_sta_probe(wlc_ap_info_t *ap, struct scb *scb)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	ratespec_t rate_override;

	if (SCB_MARKED_DEL(scb)) {
		return;
	}

	/* If a probe is still pending, don't send another one */
	if (scb->flags & SCB_PENDING_PROBE)
		return;

	/* use the lowest basic rate */
	rate_override = wlc_lowest_basicrate_get(scb->bsscfg);

	ASSERT(VALID_RATE(wlc, rate_override));

	if (!wlc_sendnulldata(wlc, scb->bsscfg, &scb->ea, rate_override, 0,
		PRIO_8021D_BE, wlc_ap_sendnulldata_cb, NULL)) {

		WL_ERROR(("wl%d: wlc_ap_sta_probe: wlc_sendnulldata failed\n",
		          wlc->pub->unit));
		return;
	}

	scb->flags |= SCB_PENDING_PROBE;
}

static void
wlc_ap_sta_probe_complete(wlc_info_t *wlc, uint txstatus, struct scb *scb, void *pkt)
{
	bool infifoflush = FALSE;
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_scb_cubby_t *ap_scb;
	wlc_bsscfg_t *cfg;

	ASSERT(scb != NULL);
	cfg = SCB_BSSCFG(scb);

	BCM_REFERENCE(cfg);

	ap_scb = AP_SCB_CUBBY(appvt, scb);
	if (!ap_scb) {
		return;
	}

	/* Return if SCB is already being deleted (pcb path) */
	if (SCB_IS_UNUSABLE(scb)) {
		return;
	}

	scb->flags &= ~(SCB_PENDING_PROBE | SCB_PSPRETEND_PROBE);

	/* Reprobe if the pkt is freed in the middle of fifo flush.
	 * SCB shouldn't be deleted then.
	 */
#ifdef WLC_LOW
	infifoflush = (wlc->hw->mac_suspend_depth > 0);
#endif /* WLC_LOW */
	BCM_REFERENCE(infifoflush);
	if (wlc->txfifo_detach_pending) {
		WL_ERROR(("wl%d.%d: STA "MACF" probe bailed out. cnt %d/%d txs 0x%x macsusp %d\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(cfg), ETHER_TO_MACF(scb->ea),
			ap_scb->grace_attempts, ap_scb->psp_attempts, txstatus, infifoflush));
		appvt->reprobe_scb = TRUE;
		return;
	}

#ifdef PSPRETEND
	if (SCB_PS_PRETEND_PROBING(scb)) {
		if ((txstatus & TX_STATUS_MASK) == TX_STATUS_ACK_RCV) {
			/* probe response OK - exit PS Pretend state */
			WL_PS(("wl%d.%d: received ACK to ps pretend probe "MACF" (count %d)\n",
			        wlc->pub->unit, WLC_BSSCFG_IDX(cfg),
			        ETHER_TO_MACF(scb->ea), ap_scb->psp_attempts));

			/* Assert check that the fifo was cleared before exiting ps mode */
			if (SCB_PS_PRETEND_BLOCKED(scb)) {
				WL_ERROR(("wl%d.%d: %s: SCB_PS_PRETEND_BLOCKED, "
				          "expected to see PMQ PPS entry\n", wlc->pub->unit,
				          WLC_BSSCFG_IDX(cfg), __FUNCTION__));
			}

#if defined(WL_PS_SCB_TXFIFO_BLK)
			if ((!AUXPMQ_ENAB(wlc->pub) && !wlc->ps_scb_txfifo_blk) ||
				(AUXPMQ_ENAB(wlc->pub) && (SCB_PS_TXFIFO_BLK(scb) == FALSE)))
#else /* ! WL_PS_SCB_TXFIFO_BLK */
			if (!(wlc->block_datafifo & DATA_BLOCK_PS))
#endif /* ! WL_PS_SCB_TXFIFO_BLK */
			{
				wlc_apps_scb_ps_off(wlc, scb, FALSE);
			} else {
				/* STA should transition to PS-OFF once PS flush done */
				wlc_apps_ps_trans_upd(wlc, scb);
				WL_PS(("wl%d.%d: %s: PS flush in progress.\n",
				     wlc->pub->unit, WLC_BSSCFG_IDX(cfg),
				     __FUNCTION__));
			}
		} else {
			++ap_scb->psp_attempts;
			WL_PS(("wl%d.%d: no response to ps pretend probe "MACF" (count %d)\n",
			        wlc->pub->unit, WLC_BSSCFG_IDX(cfg),
			        ETHER_TO_MACF(scb->ea), ap_scb->psp_attempts));
		}
		/* we re-probe using ps pretend probe timer if not stalled,
		* so return from here
		*/
		return;
	}
#endif /* PSPRETEND */
	/* ack indicates the sta should not be removed or we might have missed the ACK but if there
	 * was some activity after sending the probe then it indicates there is life out there in
	 * scb.
	 */
	if (((txstatus & TX_STATUS_MASK) != TX_STATUS_NO_ACK) ||
	    (wlc->pub->now - scb->used < appvt->scb_activity_time) ||
	    appvt->scb_activity_time == 0) {
		ap_scb->grace_attempts = 0;
		/* update the primary PSTA also */
		if (ap_scb->psta_prim != NULL)
			SCB_UPDATE_USED(wlc, ap_scb->psta_prim);
		return;
	}

	/* If still more grace_attempts are left, then probe the STA again */
	if (++ap_scb->grace_attempts < appvt->scb_max_probe) {
		appvt->reprobe_scb = TRUE;
		return;
	}

	WL_ASSOC(("wl%d: %s: no ACK from "MACF" for Null Data, txstatus 0x%04x, PS %d\n",
		wlc->pub->unit, __FUNCTION__, ETHER_TO_MACF(scb->ea), txstatus, SCB_PS(scb)));
#ifdef BCMDBG
	if (SCB_PS(scb) || SCB_TWTPS(scb)) {
		wlc_apps_print_scb_info(wlc, scb);
	}
#endif /* BCMDBG */

	/* Mark the scb for deletion. Sending deauth and SCB free will happen
	 * from the inactivity timeout context in wlc_ap_scb_unusable_stas_timeout()
	 */
	SCB_MARK_DEL(scb);
} /* wlc_ap_sta_probe_complete */

int
wlc_ap_get_bcnprb(wlc_info_t *wlc, wlc_bsscfg_t *cfg, bool bcn, void *buf, uint *buflen)
{
	int rc = BCME_OK;
	int hdr_skip = 0;
	int pkt_len = 0;
	uint8 *pkt_data = NULL;

	ASSERT(buf);
	ASSERT(buflen);

	if (MBSSID_ENAB(wlc->pub, wlc->band->bandtype) && !BSSCFG_IS_MAIN_AP(cfg)) {
		return BCME_ERROR;
	}
	if (MBSS_BCN_ENAB(wlc, cfg)) {
#if defined(MBSS)
		wlc_pkt_t pkt;

		/* Fetch packet of the correct type */
		if (bcn) {
			pkt = wlc_mbss_get_bcn_template(wlc, cfg);
#ifdef WLTOEHW
			{
				uint16 tso_hdr_size;
				d11ac_tso_t *tso_hdr;
				/* Pull off space so that d11hdrs below works */
				tso_hdr = (d11ac_tso_t *) PKTDATA(wlc->osh, pkt);
				tso_hdr_size = (uint16) (wlc->toe_bypass ? 0 :
						wlc_tso_hdr_length(tso_hdr));
				hdr_skip += tso_hdr_size;
			}
#endif /* WLTOEHW */

		} else {
			pkt = wlc_mbss_get_probe_template(wlc, cfg);
		}

		/* Select the data, but skip the D11 header part */
		if (pkt != NULL) {
			if (D11REV_GE(wlc->pub->corerev, 128) && bcn) {
				hdr_skip = D11_PHY_HDR_LEN;
			} else {
				hdr_skip += D11_TXH_LEN_EX(wlc);
			}
			pkt_len = PKTLEN(wlc->osh, pkt);
			pkt_data = (uint8 *)PKTDATA(wlc->osh, pkt);
		}
#endif /* MBSS */
	} else if (HWBCN_ENAB(cfg)) {
		uint type;
		ratespec_t rspec;

		pkt_len = wlc->pub->bcn_tmpl_len;
		pkt_data = (uint8*)buf;

		/* Bail if not enough room */
		if ((int)*buflen < pkt_len) {
			return BCME_BUFTOOSHORT;
		}

		if (bcn) {
			/* For beacon_info */
			type = FC_BEACON;
			hdr_skip = wlc->pub->d11tpl_phy_hdr_len;
			rspec = wlc_lowest_basic_rspec(wlc, &cfg->current_bss->rateset);
		} else {
			/* For probe_resp_info */
			type = FC_PROBE_RESP;
			rspec = (ratespec_t)0;

			if (D11REV_GE(wlc->pub->corerev, 40)) {
				hdr_skip = 0;
			} else {
				hdr_skip = wlc->pub->d11tpl_phy_hdr_len;
			}
		}
		/* Generate the appropriate packet template */
		wlc_bcn_prb_template(wlc, type, rspec, cfg, (uint16 *)pkt_data, &pkt_len);
		if (type == FC_BEACON) {
			wlc_beacon_upddur(wlc, rspec, pkt_len);
		}
	}

	/* Return management frame if able */
	pkt_data += hdr_skip;
	pkt_len -= MIN(hdr_skip, pkt_len);

	if ((int)*buflen >= pkt_len) {
		memcpy((uint8*)buf, pkt_data, pkt_len);
		*buflen = (int)pkt_len;
	} else {
		rc = BCME_BUFTOOSHORT;
	}

	return rc;
} /* wlc_ap_get_bcnprb */

#ifdef BCMDBG
static int
wlc_dump_ap(wlc_ap_info_t *ap, struct bcmstrbuf *b)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;

	bcm_bprintf(b, "\n");

	bcm_bprintf(b, " shortslot_restrict %d scb_timeout %d\n",
		ap->shortslot_restrict, appvt->scb_timeout);

	bcm_bprintf(b, "tbtt %d pre-tbtt-us %u. max latency %u. "
		"min threshold %u. block datafifo %d "
		"\n",
		WLCNTVAL(wlc->pub->_cnt->tbtt),
		appvt->pre_tbtt_us, MBSS_PRE_TBTT_MAX_LATENCY_us,
		MBSS_PRE_TBTT_MIN_THRESH_us, wlc->block_datafifo);

#if defined(WL_PS_SCB_TXFIFO_BLK)
	bcm_bprintf(b, "ps_scb_txfifo_blk %d   ps_txfifo_blk_scb_cnt %d\n",
		wlc->ps_scb_txfifo_blk, wlc->ps_txfifo_blk_scb_cnt);
#endif /* WL_PS_SCB_TXFIFO_BLK */

#if BAND6G || defined(WL_OCE_AP)
	if (wlc->upr_fd_info) {
		bcm_bprintf(b, "broadcast probe response count=%d (failed=%d), alloc_fail=%d"
			" n_pktget[%d] n_pktfree[%d] in_flight[%d]\n",
			wlc->upr_fd_info->n_pkts_xmit, wlc->upr_fd_info->n_pkts_xmit_fail,
			wlc->upr_fd_info->alloc_fail, wlc->upr_fd_info->n_pktget,
			wlc->upr_fd_info->n_pktfree, wlc->upr_fd_info->in_flight);
	}
#endif /* BAND6G || WL_OCE_AP */
	return 0;
}

#ifdef RXCHAIN_PWRSAVE
static int
wlc_dump_rxchain_pwrsave(wlc_ap_info_t *ap, struct bcmstrbuf *b)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;

	wlc_dump_pwrsave(&appvt->rxchain_pwrsave.pwrsave, b);

	return 0;
}
#endif /* RXCHAIN_PWRSAVE */

#ifdef RADIO_PWRSAVE
static int
wlc_dump_radio_pwrsave(wlc_ap_info_t *ap, struct bcmstrbuf *b)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;

	wlc_dump_pwrsave(&appvt->radio_pwrsave.pwrsave, b);

	return 0;
}
#endif /* RADIO_PWRSAVE */

#if defined(RXCHAIN_PWRSAVE) || defined(RADIO_PWRSAVE)
static void
wlc_dump_pwrsave(wlc_pwrsave_t *pwrsave, struct bcmstrbuf *b)
{
	bcm_bprintf(b, "\n");

	bcm_bprintf(b, " in_power_save %d\n",
		pwrsave->in_power_save);

	bcm_bprintf(b, " no: of times in power save mode %d\n",
		pwrsave->in_power_save_counter);

	bcm_bprintf(b, " power save time (in secs) %d\n",
		pwrsave->in_power_save_secs);

	return;
}
#endif /* RXCHAIN_PWRSAVE or RADIO_PWRSAVE */
#endif /* BCMDBG */

#if defined(STA) && defined(AP)
bool
wlc_apup_allowed(wlc_info_t *wlc)
{
	bool modesw_in_prog = FALSE;
	bool busy = FALSE;
	wlc_bsscfg_t *as_cfg;
#if defined(WL_MODESW) && !defined(WL_MODESW_DISABLED)
	modesw_in_prog = (WLC_MODESW_ENAB(wlc->pub) && MODE_SWITCH_IN_PROGRESS(wlc->modesw)) ?
	TRUE:FALSE;
#endif /* WL_MODESW && !WL_MODESW_DISABLED */
	busy = SCAN_IN_PROGRESS(wlc->scan) ||
#ifdef WL11K
		wlc_rrm_inprog(wlc) ||
#endif /* WL11K */
		WLC_RM_IN_PROGRESS(wlc) ||
		modesw_in_prog;
	as_cfg = AS_IN_PROGRESS_CFG(wlc);
	busy = busy || wlc->apsta_muted || (AS_IN_PROGRESS(wlc) &&
		(as_cfg->assoc->state != AS_WAIT_FOR_AP_CSA) &&
		(as_cfg->assoc->state != AS_WAIT_FOR_AP_CSA_ROAM_FAIL));

#ifdef BCMDBG
	if (busy) {
		WL_APSTA_UPDN(("wl%d: wlc_apup_allowed: defer AP UP, STA associating: "
			"stas/aps/associated %d/%d/%d, AS_IN_PROGRESS() %d, scan %d, rm %d"
			"modesw = %d apsta_muted %d\n", wlc->pub->unit, wlc->stas_associated,
			wlc->aps_associated, wlc->pub->associated, AS_IN_PROGRESS(wlc),
			SCAN_IN_PROGRESS(wlc->scan), WLC_RM_IN_PROGRESS(wlc), modesw_in_prog,
			wlc->apsta_muted));
	}
#endif
	return !busy;
}
#endif /* STA */

static void
wlc_ap_dynfb(wlc_ap_info_pvt_t *appvt)
{
	wlc_info_t *wlc = appvt->wlc;
	data_snapshot_t *snapshot = &appvt->snapshot;
	uint32 txframe_snapshot, rxframe_snapshot;
	uint32 txbytes_snapshot, rxbytes_snapshot;
	uint32 delta_txframe, delta_rxframe;
	uint32 delta_txbytes, delta_rxbytes;
	uint32 tx_pktsz = 0, rx_pktsz = 0;

	rxframe_snapshot = wlc->pub->_cnt->rxframe;
	txframe_snapshot = wlc->pub->_cnt->txframe;
	rxbytes_snapshot = wlc->pub->_cnt->rxbyte;
	txbytes_snapshot = wlc->pub->_cnt->txbyte;

	delta_txframe = txframe_snapshot - snapshot->txframe;
	delta_rxframe = rxframe_snapshot - snapshot->rxframe;
	delta_txbytes = txbytes_snapshot - snapshot->txbytes;
	delta_rxbytes = rxbytes_snapshot - snapshot->rxbytes;

	if (delta_txframe > 0)
		tx_pktsz = (delta_txbytes) / (delta_txframe);
	if (delta_rxframe > 0)
		rx_pktsz = (delta_rxbytes) / (delta_rxframe);

	if ((tx_pktsz >= wlc->active_bidir_thresh) && (rx_pktsz >= wlc->active_bidir_thresh) &&
		((wlc->active_udpv6 == TRUE) ||
		(BAND_2G(wlc->band->bandtype) && (wlc->active_udpv4 == TRUE))) &&
		(wlc->active_tcp == FALSE)) {
		wlc->active_bidir = TRUE;
		wlc->bidir_countdown = ACTIVE_BIDIR_DELAY;
	} else {
		if (wlc->bidir_countdown > 0)
			wlc->bidir_countdown--;
	}

	WL_TRACE(("band<%d> frames<%u:%u> pktsz<%u:%u> udpv6<%s> udpv4<%s> tcp<%s> "
		"bidir<%s> count<%d>\n",
		wlc->band->bandtype,
		delta_txframe, delta_rxframe,
		tx_pktsz, rx_pktsz,
		wlc->active_udpv6?"true":"false",
		wlc->active_udpv4?"true":"false",
		wlc->active_tcp?"true":"false",
		wlc->active_bidir?"TRUE":"FALSE",
		wlc->bidir_countdown));

	ASSERT(wlc->bidir_countdown <= ACTIVE_BIDIR_DELAY && wlc->bidir_countdown >= 0);
	if (wlc->bidir_countdown == 0)
		wlc->active_bidir = FALSE;
	wlc->active_udpv6 = FALSE;
	wlc->active_udpv4 = FALSE;
	wlc->active_tcp = FALSE;

	snapshot->txframe = txframe_snapshot;
	snapshot->rxframe = rxframe_snapshot;
	snapshot->txbytes = txbytes_snapshot;
	snapshot->rxbytes = rxbytes_snapshot;
}

static void
wlc_ap_consider_indriver_channel_change(wlc_ap_info_pvt_t *appvt)
{
	wlc_info_t *wlc = appvt->wlc;
	chanspec_t rand_chspec;

	if (!(appvt->edcrs_hi_event_status & WL_EDCRS_HI_EVENT_INDRIVER)) {
		return;
	}
	/* delay by count if event is also enabled */
	if ((appvt->edcrs_hi_event_status & WL_EDCRS_HI_EVENT_ENABLED) != 0 &&
			appvt->edcrs_hi_on_ch_count < appvt->edcrs_hi_in_driver_wait_thresh) {
		return;
	}

	rand_chspec = wlc_channel_rand_chanspec(wlc, TRUE, FALSE);
	WL_REGULATORY(("wl%d: ch:0x%04x, edcrs_hi events:%d, attempting rand_ch:0x%04x\n",
			wlc->pub->unit, wlc->chanspec,
			appvt->edcrs_hi_on_ch_count, rand_chspec));
	if (!rand_chspec) {
		return;
	}

	/* set chanspec and call acs_update to avoid CSA during high EDCRS */
	(void) wlc_validate_set_chanspec(wlc, rand_chspec);
	if (wlc->pub->up && (appvt->chanspec_selected != 0) &&
			(phy_utils_get_chanspec(WLC_PI(wlc)) != appvt->chanspec_selected)) {
		wlc_ap_acs_update(wlc);
	}
}

static void
wlc_ap_get_cca(wlc_info_t *wlc, wlc_ap_cca_t *cca)
{
	wlc_hw_info_t *wlc_hw = wlc->hw;

	ASSERT(cca != NULL);

	if (D11REV_LE(wlc->pub->corerev, 40)) {
		memset(cca, 0, sizeof(*cca));
		return;
	}

	cca->time_ms = OSL_SYSUPTIME();

	/* tx */
	cca->txdur = CCA_READ(wlc_hw, TXDUR);

	/* all RX */
	cca->inbss = CCA_READ(wlc_hw, INBSS);
	cca->obss  = CCA_READ(wlc_hw, OBSS);
	cca->noctg = CCA_READ(wlc_hw, NOCTG);
	cca->nopkt = CCA_READ(wlc_hw, NOPKT);
	cca->edcrs = CCA_READ(wlc_hw, EDCRSDUR);
	cca->wifi  = CCA_READ(wlc_hw, WIFI);

	/* idle */
	cca->doze  = MAC_READ(wlc_hw, SLPDUR);
	cca->txop  = CCA_READ(wlc_hw, TXOP) * SLOTTIME(wlc); // converting to us
}

/**
 * Attempts to infer if incumbents may be present based on metrics in CCA stats
 *   - EDCRS is reported high
 * To be called once per watchdog tick.
 */
static bool
wlc_ap_watchdog_infer_incumbent(wlc_ap_info_pvt_t *appvt)
{
	wlc_info_t *wlc = appvt->wlc;
	wlc_ap_cca_t cca_cur = {0}, *cca_last = &appvt->cca;
	uint32 time_ms_diff, txdur_diff, inbss_diff, obss_diff, noctg_diff, nopkt_diff, edcrs_diff,
	       wifi_diff, doze_diff, txop_diff, sum_of_diffs;

	if (!appvt->edcrs_dur_thresh_us) {	// special case; zero threshold configured
		WL_REGULATORY(("wl%d: %s: edcrs_dur_thresh_us = %u us. Returning true.\n",
				wlc->pub->unit, __FUNCTION__, appvt->edcrs_dur_thresh_us));
		return TRUE;
	}

	wlc_ap_get_cca(wlc, &cca_cur);

	if (!cca_last->edcrs || !cca_cur.edcrs) {			// invalid edcrs diff
		*cca_last = cca_cur;
		return FALSE;
	}

	time_ms_diff = cca_cur.time_ms - cca_last->time_ms;

	/* uint32 cca stats/counters may wrap-around but diff will be fine for single wraps */
	txdur_diff = cca_cur.txdur - cca_last->txdur;

	inbss_diff = cca_cur.inbss - cca_last->inbss;
	obss_diff  = cca_cur.obss  - cca_last->obss;
	noctg_diff = cca_cur.noctg - cca_last->noctg;
	nopkt_diff = cca_cur.nopkt - cca_last->nopkt;
	edcrs_diff = cca_cur.edcrs - cca_last->edcrs;
	wifi_diff  = cca_cur.wifi  - cca_last->wifi;

	doze_diff  = cca_cur.doze  - cca_last->doze;
	txop_diff  = cca_cur.txop  - cca_last->txop;

	sum_of_diffs = txdur_diff + inbss_diff + obss_diff + noctg_diff + nopkt_diff + edcrs_diff +
			wifi_diff + doze_diff + txop_diff;

	*cca_last = cca_cur;

	if (appvt->edcrs_hi_event_status && time_ms_diff && ((wl_msg_level & WL_INFORM_VAL) != 0 ||
			edcrs_diff > WLC_AP_CCA_EDCRS_LOG_THRESH_US)) {
		WL_REGULATORY(("wl%d: DIFF time:%-4ums, cca(us):: edcrs:%-6u + txdur:%-6u + "
				"inbss:%-6u + obss:%-6u + noctg:%-6u + nopkt:%-6u + doze:%-6u + "
				"txop:%-6u + wifi:%-6u = sum:%-7uus or %ums\n",
				wlc->pub->unit, time_ms_diff, edcrs_diff, txdur_diff,
				inbss_diff, obss_diff, noctg_diff, nopkt_diff, doze_diff, txop_diff,
				wifi_diff, sum_of_diffs, (sum_of_diffs/1000)));
	}

	if (time_ms_diff < WLC_AP_CCA_MIN_SAMPLE_MS ||			// insufficient sample
			time_ms_diff > WLC_AP_CCA_MAX_SAMPLE_MS ||	// excess sample duration
			(sum_of_diffs >> 10) > WLC_AP_CCA_MAX_SAMPLE_MS || // excess sample sum
			edcrs_diff < appvt->edcrs_dur_thresh_us) {	// low edcrs
		return FALSE;
	}

	WL_REGULATORY(("wl%d: %s: edcrs_diff %u us >= thresh %u us. Returning true.\n",
			wlc->pub->unit, __FUNCTION__, edcrs_diff, appvt->edcrs_dur_thresh_us));

	return TRUE;
}

/**
 * If no beacons are transmitted for txbcn timeout period then some thing has gone bad,
 * do a big-hammer. For EDCRS_HI case, generate events and/or attempt channel switch.
 * To be called once per watchdog tick.
 */
static void
wlc_ap_watchdog_beacon_inactivity_check(wlc_ap_info_pvt_t *appvt)
{
	wlc_info_t *wlc = appvt->wlc;
	wlc_bsscfg_t *cfg;
	/* simulate single-shot edcrs_hi event */
	const bool simulate_edcrs_hi_event = ((appvt->edcrs_hi_event_status &
			WL_EDCRS_HI_EVENT_SIMULATE) != 0);
	/* inferring incumbent from EDCRSDUR MAC CCA stats */
	const bool edcrs_hi = wlc_ap_watchdog_infer_incumbent(appvt);
	/* WAR for RTL bug not providing high EDCRS in upper 80-in-160 */
	/* simulating edcrs_hi phy state */
	const bool edcrs_hi_sim = (appvt->edcrs_hi_sim_secs > 0);
	/* combining EDCRS_HI from PHY, WAR, simulation */
	const bool edcrs = edcrs_hi || edcrs_hi_sim;
	const uint16 txbcn_snapshot = wlc_read_shm(wlc, MACSTAT_ADDR(wlc, MCSTOFF_TXBCNFRM));

	if (appvt->edcrs_hi_sim_secs > 0) {
		appvt->edcrs_hi_sim_secs--;
	}

	/* see if any beacons are transmitted since last watchdog timeout. */
	if (txbcn_snapshot != appvt->txbcn_snapshot && !simulate_edcrs_hi_event &&
			!edcrs_hi_sim) {
		/* all good and active */
		appvt->txbcn_inactivity = 0;
		appvt->edcrs_txbcn_inact_count = 0;
		/* save the txbcn counter */
		appvt->txbcn_snapshot = txbcn_snapshot;
		if (appvt->edcrs_txbcn_inact_thresh) {
			appvt->edcrs_hi_on_ch_count = 0;
			return;
		}
	} else {
		/* increment inactivity count */
		appvt->txbcn_inactivity++;
		if (edcrs) {
			appvt->edcrs_txbcn_inact_count++;
		}
	}

	if (appvt->edcrs_hi_event_status) {
		if (appvt->txbcn_inactivity || edcrs) {
			WL_REGULATORY(("wl%d: %s: txbcn:%d, inact:%d, edcrs:%d, edcrs count:%d, "
					"on_ch_count:%d\n", wlc->pub->unit, __FUNCTION__,
					txbcn_snapshot, appvt->txbcn_inactivity, edcrs,
					appvt->edcrs_txbcn_inact_count,
					appvt->edcrs_hi_on_ch_count));
		}
		if ((appvt->txbcn_inactivity && simulate_edcrs_hi_event) ||
				(edcrs && appvt->edcrs_txbcn_inact_count >=
				appvt->edcrs_txbcn_inact_thresh)) {
			if ((appvt->edcrs_hi_event_status & WL_EDCRS_HI_EVENT_ENABLED) != 0) {
				wl_edcrs_hi_event_t edcrs_hi_event = {0};
				edcrs_hi_event.version = WLC_E_EDCRS_HI_VER;
				edcrs_hi_event.length = WLC_E_EDCRS_HI_LEN;
				edcrs_hi_event.type = (uint16) simulate_edcrs_hi_event;
				edcrs_hi_event.status = appvt->txbcn_inactivity;
				cfg = wlc_bsscfg_find_by_wlcif(wlc, wlc->wlcif_list);
				wlc_bss_mac_event(wlc, cfg, WLC_E_EDCRS_HI_EVENT, NULL, 0, 0, 0,
						(void *)&edcrs_hi_event, sizeof(edcrs_hi_event));
				WL_REGULATORY(("wl%d: WLC_E_EDCRS_HI_EVENT generated\n",
						wlc->pub->unit));
			}
			appvt->edcrs_txbcn_inact_count = 0;
			if (appvt->edcrs_txbcn_inact_chspec == wlc->chanspec) {
				appvt->edcrs_hi_on_ch_count++;
			} else {
				appvt->edcrs_hi_on_ch_count = 1;
			}
			appvt->edcrs_txbcn_inact_chspec = wlc->chanspec;
			wlc_ap_consider_indriver_channel_change(appvt);
			/* reset single-shot edcrs_hi event simulation */
			appvt->edcrs_hi_event_status &= ~(WL_EDCRS_HI_EVENT_SIMULATE);
		}
	}
	if (((appvt->txbcn_inactivity >= appvt->txbcn_timeout) && !edcrs) ||
			((appvt->txbcn_inactivity >= appvt->txbcn_edcrs_timeout))) {

		WL_ERROR(("wl%d: bcn inactivity detected\n", wlc->pub->unit));
		WL_INFORM(("wl%d: txbcnfrm %d prev txbcnfrm %d txbcn inactivity %d "
				"timeout %d edcrs_timeout %d macctrl 0x%x tbtt %d edcrs %d\n",
				wlc->pub->unit, txbcn_snapshot,
				appvt->txbcn_snapshot, appvt->txbcn_inactivity,
				appvt->txbcn_timeout, appvt->txbcn_edcrs_timeout,
				R_REG(wlc->osh, D11_MACCONTROL(wlc)),
				wlc->pub->_cnt->tbtt, edcrs));
		wlc_macdbg_psmwd_trigger(wlc->macdbg, WLC_MACDBG_PSMWD_TXBCN);
		appvt->txbcn_inactivity = 0;
		appvt->txbcn_snapshot = 0;
		WLCNTINCR(wlc->pub->_ap_cnt->txbcnloss);
#if !(defined(BCMDBG) || defined(WL_MACDBG))
		wlc->hw->need_reinit = WL_REINIT_RC_AP_BEACON;
		wlc_fatal_error(wlc);
#endif /* !defined(DONGLEBUILD) || !(defined(BCMDBG) || defined(WL_MACDBG)) */
		return;
	}

	/* save the txbcn counter */
	appvt->txbcn_snapshot = txbcn_snapshot;
}

static void
wlc_ap_watchdog(void *arg)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) arg;
	wlc_ap_info_t *ap = &appvt->appub;
	wlc_info_t *wlc = appvt->wlc;
#ifdef RXCHAIN_PWRSAVE
	bool done = 0;
#endif
	int idx;
	wlc_bsscfg_t *cfg;

#if BAND6G || defined(WL_OCE_AP)
	if (WL_UPR_FD_SW_ENAB(wlc) &&
		((isset(&(wlc->nbr_discovery_cap), WLC_NBR_DISC_CAP_20TU_BCAST_PRB_RSP)) ||
		(isset(&(wlc->nbr_discovery_cap), WLC_NBR_DISC_CAP_20TU_FILS_DISCOVERY)))) {
		WL_INFORM(("wl%d: UPR/FD packets in_flight[%d] n_pktget[%d] n_pktfree[%d]"
			" one_time_alloc[%d]\n", wlc->pub->unit, wlc->upr_fd_info->in_flight,
			wlc->upr_fd_info->n_pktget, wlc->upr_fd_info->n_pktfree,
			wlc->upr_fd_info->one_time_alloc));
	}
#endif /* WL_WIFI_6GHZ || WL_OCE_AP */
	/* part 1 */
	if (AP_ENAB(wlc->pub)) {
		struct scb *scb;
		struct scb_iter scbiter;

#if defined(STA)
	/* start AP if operation were pending on SCAN_IN_PROGRESS() or WLC_RM_IN_PROGRESS() */
	/* Find AP's that are enabled but not up to restart */
		if (APSTA_ENAB(wlc->pub) && wlc_apup_allowed(wlc)) {
			bool startap = FALSE;

			FOREACH_AP(wlc, idx, cfg) {
				/* find the first ap that is enabled but not up */
				if (cfg->enable && !cfg->up) {
					startap = TRUE;
					break;
				}
			}

			if (startap) {
				WL_APSTA_UPDN(("wl%d: wlc_watchdog -> restart downed ap\n",
				               wlc->pub->unit));
				wlc_restart_ap(ap);
			}
		}
#endif /* STA */

#if BAND6G || defined(WL_OCE_AP)
		if ((wlc_ap_check_tx_bss_for_upr_fd(wlc) != BCME_OK)) {
			WL_INFORM(("wl%d: Error in configuring TX BSS AP\n", wlc->pub->unit));
		}
#endif /* BAND6G || WL_OCE_AP */
		if (wlc->active_bidir_thresh > 0) {
			/* dynamic frameburst is enabled only when active_bidir_thresh > 0 */
			wlc_ap_dynfb(appvt);
		}

		/* before checking for stuck tx beacons make sure atleast one
		 * ap bss is up phy is not muted(for whatever reason) and beaconing.
		 */
		if (!phy_utils_ismuted(WLC_PI(wlc)) && !SCAN_IN_PROGRESS(wlc->scan) &&
				(appvt->txbcn_timeout > 0) && (AP_BSS_UP_COUNT(wlc) > 0)) {
			wlc_ap_watchdog_beacon_inactivity_check(appvt);
		}

		/* if DMA pktpool empty for contibnuous 5 sec, call big hammer */
		if (wlc_bmac_pktpool_empty(wlc))
			appvt->dma_pktpool_empty_cnt += 1;
		else
			appvt->dma_pktpool_empty_cnt = 0;

		if (appvt->dma_pktpool_empty_cnt >= DMA_PKTPOOL_EMPTY_CNT) {
			WL_ERROR(("wl%d: dma pktpool empty detected\n", wlc->pub->unit));
			appvt->dma_pktpool_empty_cnt = 0;
			wlc->hw->need_reinit = WL_REINIT_RC_RX_DMA_BUF_EMPTY;
			wlc_fatal_error(wlc);
			return;
		}

		/* run decrypt error detection and recovery */
		wlc_ap_wsec_decrypt_error_recovery(ap);

		/* deauth rate limiting - enable sending one deauth every second */
		FOREACHSCB(wlc->scbstate, &scbiter, scb) {
#ifdef RXCHAIN_PWRSAVE
			if ((ap->rxchain_pwrsave_enable == RXCHAIN_PWRSAVE_ENAB_BRCM_NONHT) &&
			    !done) {
				if (SCB_ASSOCIATED(scb) && !(scb->flags & SCB_BRCM) &&
				    SCB_HT_CAP(scb)) {
					if (appvt->rxchain_pwrsave.pwrsave.in_power_save) {
						wlc_stf_rxchain_set(wlc,
							appvt->rxchain_pwrsave.rxchain, TRUE);
						/* need to restore rx_stbc HT capability
						 * after exit rxchain_pwrsave mode
						 */
						wlc_stf_exit_rxchain_pwrsave(wlc,
							appvt->rxchain_pwrsave.ht_cap_rx_stbc);
						appvt->rxchain_pwrsave.pwrsave.in_power_save =
							FALSE;
					}
					appvt->rxchain_pwrsave.pwrsave.power_save_check &=
						~PWRSAVE_ENB;
					done = 1;
				}
			}
#endif /* RXCHAIN_PWRSAVE */
			/* clear scb deauth. sent flag so new deauth allows to be sent */
			scb->flags &= ~SCB_DEAUTH;

			/* Check parewise key pending time, reset the flag when reaching to 0 */
			if (scb->dropblock_dur > 0) {
				scb->dropblock_dur--;
				if (scb->dropblock_dur == 0) {
					WL_ASSOC(("wl%d WD: "MACF" blockdrop_dur reaches 0\n",
						wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
				}
			}

		}
#ifdef RXCHAIN_PWRSAVE
		if (!done) {
			if (!ap->rxchain_pwrsave_enable)
				appvt->rxchain_pwrsave.pwrsave.power_save_check &= ~PWRSAVE_ENB;
			else
				appvt->rxchain_pwrsave.pwrsave.power_save_check |= PWRSAVE_ENB;
		}
#endif
	}

	/* part 2 */
	if (AP_ENAB(wlc->pub)) {

		/* process age-outs only when not in scan progress */
		if (!SCAN_IN_PROGRESS(wlc->scan)) {
			/* age out ps queue packets */
			wlc_apps_psq_ageing(wlc);
			if (appvt->scb_timeout &&
			    (((wlc->pub->now - wlc->pub->pending_now -1) % appvt->scb_timeout) >
			     (wlc->pub->now % appvt->scb_timeout))) {
				/* Appropriate aging time is misssed by scanning.
				 * set reprobe_scb to probing now.
				 */
				appvt->reprobe_scb = TRUE;
			}
			wlc->pub->pending_now = 0;

			/* ageout unusable STAs */
			wlc_ap_scb_unusable_stas_timeout(ap);

			/* age out stas */
			if ((appvt->scb_timeout &&
			     ((wlc->pub->now % appvt->scb_timeout) == 0)) ||
#ifdef WLP2P
			    (wlc->p2p && wlc_p2p_go_scb_timeout(wlc->p2p)) ||
#endif /* WLP2P */
			    appvt->reprobe_scb) {
				wlc_ap_stas_timeout(ap);
				appvt->reprobe_scb = FALSE;
			}
			/* age out secure STAs not in authorized state */
			wlc_ap_wsec_stas_timeout(ap);
		} else {
			/* This variable is that how many times aging jumped by scan operation.
			 * This variable reset when age-out starts again after scan.
			 */
			wlc->pub->pending_now++;
		}

		if (WLC_CHANIM_ENAB(wlc->pub) && WLC_CHANIM_MODE_ACT(wlc->chanim_info))
			wlc_lq_chanim_upd_act(wlc);

#ifdef RXCHAIN_PWRSAVE
		/* Do the wl power save checks */
		if (appvt->rxchain_pwrsave.pwrsave.power_save_check & PWRSAVE_ENB)
			wlc_pwrsave_mode_check(ap, PWRSAVE_RXCHAIN);

		if (appvt->rxchain_pwrsave.pwrsave.in_power_save)
			appvt->rxchain_pwrsave.pwrsave.in_power_save_secs++;
#endif

#ifdef RADIO_PWRSAVE
		if (appvt->radio_pwrsave.pwrsave.power_save_check)
			wlc_pwrsave_mode_check(ap, PWRSAVE_RADIO);

		if (appvt->radio_pwrsave.pwrsave.in_power_save)
			appvt->radio_pwrsave.pwrsave.in_power_save_secs++;
#endif
	}

	/* Part 4 */

	FOREACH_UP_AP(wlc, idx, cfg) {
		/* update brcm_ie and our beacon */
		if (wlc_bss_update_brcm_ie(wlc, cfg)) {
			WL_APSTA_BCN(("wl%d.%d: wlc_watchdog() calls wlc_update_beacon()\n",
			              wlc->pub->unit, WLC_BSSCFG_IDX(cfg)));
			wlc_bss_update_beacon(wlc, cfg, FALSE);
			wlc_bss_update_probe_resp(wlc, cfg, TRUE);
		}
	}
#ifdef WL_TRAFFIC_THRESH
	wlc_traffic_thresh_mgmt(wlc);
#endif /* WL_TRAFFIC_THRESH */

#ifdef WL_MODESW
	if (appvt->recheck_160_80 != 0 && appvt->recheck_160_80 < wlc->pub->now) {
		wlc_ap_160_80_bw_switch(wlc->ap);
	}
#endif /* WL_MODESW */
} /* wlc_ap_watchdog */

int
wlc_ap_get_maxassoc(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;

	return appvt->maxassoc;
}

void
wlc_ap_set_maxassoc(wlc_ap_info_t *ap, int val)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;

	appvt->maxassoc = val;
}

int
wlc_ap_get_maxassoc_limit(wlc_ap_info_t *ap)
{
	wlc_info_t *wlc = ((wlc_ap_info_pvt_t *)ap)->wlc;

#if defined(MAXASSOC_LIMIT)
	if (MAXASSOC_LIMIT <= wlc->pub->tunables->maxscb)
		return MAXASSOC_LIMIT;
	else
#endif /* MAXASSOC_LIMIT */
		return wlc->pub->tunables->maxscb;
}

static int
wlc_ap_ioctl(void *hdl, uint cmd, void *arg, uint len, struct wlc_if *wlcif)
{
	wlc_ap_info_t *ap = (wlc_ap_info_t *) hdl;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;
	int val = 0, *pval;
	bool bool_val;
	int bcmerror = 0;
	struct maclist *maclist;
	wlc_bsscfg_t *bsscfg;
	struct scb_iter scbiter;
	struct scb *scb = NULL;

	/* update bsscfg pointer */
	bsscfg = wlc_bsscfg_find_by_wlcif(wlc, wlcif);
	ASSERT(bsscfg != NULL);

	/* update wlcif pointer */
	if (wlcif == NULL)
		wlcif = bsscfg->wlcif;
	ASSERT(wlcif != NULL);

	/* default argument is generic integer */
	pval = (int *) arg;
	/* This will prevent the misaligned access */
	if (pval && (uint32)len >= sizeof(val))
		bcopy(pval, &val, sizeof(val));
	/* bool conversion to avoid duplication below */
	bool_val = (val != 0);

	switch (cmd) {

	case WLC_GET_SHORTSLOT_RESTRICT:
		if (AP_ENAB(wlc->pub)) {
			ASSERT(pval != NULL);
			*pval = ap->shortslot_restrict;
		} else {
			bcmerror = BCME_NOTAP;
		}
		break;

	case WLC_SET_SHORTSLOT_RESTRICT:
		if (AP_ENAB(wlc->pub))
			ap->shortslot_restrict = bool_val;
		else
			bcmerror = BCME_NOTAP;
		break;

#ifdef BCMDBG
	case WLC_GET_IGNORE_BCNS:
		if (AP_ENAB(wlc->pub)) {
			ASSERT(pval != NULL);
			*pval = wlc->ignore_bcns;
		} else {
			bcmerror = BCME_NOTAP;
		}
		break;

	case WLC_SET_IGNORE_BCNS:
		if (AP_ENAB(wlc->pub))
			wlc->ignore_bcns = bool_val;
		else
			bcmerror = BCME_NOTAP;
		break;
#endif /* BCMDBG */

	case WLC_GET_SCB_TIMEOUT:
		if (AP_ENAB(wlc->pub)) {
			ASSERT(pval != NULL);
			*pval = appvt->scb_timeout;
		} else {
			bcmerror = BCME_NOTAP;
		}
		break;

	case WLC_SET_SCB_TIMEOUT:
		if (AP_ENAB(wlc->pub))
			appvt->scb_timeout = val;
		else
			bcmerror = BCME_NOTAP;
		break;

	case WLC_GET_ASSOCLIST:
		ASSERT(arg != NULL);
		maclist = (struct maclist *) arg;
		ASSERT(maclist);

		/* returns a list of STAs associated with a specific bsscfg */
		if (len < (int)(sizeof(maclist->count) + (MAXSCB * ETHER_ADDR_LEN))) {
			bcmerror = BCME_BUFTOOSHORT;
			break;
		}
		val = 0;
		FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
			if (SCB_ASSOCIATED(scb)) {
				bcopy((void*)&scb->ea, (void*)&maclist->ea[val],
					ETHER_ADDR_LEN);
				val++;
			}

		}
		maclist->count = val;
		break;

	case WLC_TKIP_COUNTERMEASURES:
		if (BSSCFG_AP(bsscfg) && WSEC_TKIP_ENABLED(bsscfg->wsec))
			(void)wlc_keymgmt_tkip_set_cm(wlc->keymgmt, bsscfg, (val != 0));
		else
			bcmerror = BCME_BADARG;
		break;

#ifdef RADAR
	case WLC_SET_RADAR:
		bcmerror = RADAR_ENAB(wlc->pub) ?
				wlc_iovar_op(wlc, "radar", NULL, 0, arg, len, IOV_SET, wlcif) :
				BCME_UNSUPPORTED;
		break;

	case WLC_GET_RADAR:
		ASSERT(pval != NULL);
		if (RADAR_ENAB(wlc->pub)) {
			*pval = (int32)wlc_dfs_get_radar(wlc->dfs);
		} else {
				bcmerror = BCME_UNSUPPORTED;
		}
		break;
#endif /* RADAR */

	default:
		bcmerror = BCME_UNSUPPORTED;
		break;
	}

	return (bcmerror);
} /* wlc_ap_ioctl */

/* in auto mode, enable EDCRS_HI event generation for 6GHz FCC LPI */
static void
wlc_ap_update_edcrs_hi_event_status(wlc_ap_info_pvt_t* appvt)
{
	wlc_info_t *wlc = appvt->wlc;

	if (appvt->edcrs_hi_event_mode != WL_EDCRS_HI_EVENT_AUTO) {
		return;
	}

	if (CHSPEC_IS6G(wlc->chanspec) && !wlc->is_edcrs_eu &&
			appvt->reg_info.reg_info_field == WL_INDOOR_ACCESS_POINT) {
		appvt->edcrs_hi_event_status = WL_EDCRS_HI_EVENT_BOTH;
	} else {
		appvt->edcrs_hi_event_status = WL_EDCRS_HI_EVENT_DISABLED;
	}
}

static int
wlc_ap_set_edcrs_hi_event_mode(wlc_ap_info_pvt_t* appvt, int val)
{
	wlc_info_t *wlc = appvt->wlc;
	BCM_REFERENCE(wlc);

	switch (val) {
	case WL_EDCRS_HI_EVENT_AUTO:
		appvt->edcrs_hi_event_mode = val;
		wlc_ap_update_edcrs_hi_event_status(appvt);
		break;
	case WL_EDCRS_HI_EVENT_DISABLED:
	case WL_EDCRS_HI_EVENT_ENABLED:
	case WL_EDCRS_HI_EVENT_INDRIVER:
	case WL_EDCRS_HI_EVENT_BOTH:
		appvt->edcrs_hi_event_mode = appvt->edcrs_hi_event_status = val;
		break;
	case WL_EDCRS_HI_EVENT_SIMULATE:
		if (appvt->edcrs_hi_event_status) {
			appvt->edcrs_hi_event_status |= val;
		}
		break;
	default:
		WL_ERROR(("wl%d: %s: unhandled edcrs_hi event mode: %d\n",
			wlc->pub->unit, __FUNCTION__, val));
		return BCME_BADARG;
	}

	WL_REGULATORY(("wl%d: %s: edcrs_hi event mode: %d, status: %d\n",
			wlc->pub->unit, __FUNCTION__, appvt->edcrs_hi_event_mode,
			appvt->edcrs_hi_event_status));
	return BCME_OK;
}

static int
wlc_ap_doiovar(void *hdl, uint32 actionid,
	void *params, uint p_len, void *arg, uint len, uint val_size, struct wlc_if *wlcif)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) hdl;
	wlc_ap_info_t* ap = &appvt->appub;
	wlc_info_t *wlc = appvt->wlc;
	wlc_bsscfg_t *bsscfg;
	int err = 0;
	int32 int_val = 0;
	int32 int_val2 = 0;
	int32 *ret_int_ptr;
	bool bool_val;
	bool bool_val2;

	BCM_REFERENCE(val_size);

	/* update bsscfg w/provided interface context */
	bsscfg = wlc_bsscfg_find_by_wlcif(wlc, wlcif);
	ASSERT(bsscfg != NULL);

	/* convenience int and bool vals for first 8 bytes of buffer */
	if (p_len >= (int)sizeof(int_val))
		bcopy(params, &int_val, sizeof(int_val));

	if (p_len >= (int)sizeof(int_val) * 2)
		bcopy((void*)((uintptr)params + sizeof(int_val)), &int_val2, sizeof(int_val));

	/* convenience int ptr for 4-byte gets (requires int aligned arg) */
	ret_int_ptr = (int32 *)arg;

	bool_val = (int_val != 0) ? TRUE : FALSE;
	bool_val2 = (int_val2 != 0) ? TRUE : FALSE;
	BCM_REFERENCE(bool_val2);

	/* update wlcif pointer */
	if (wlcif == NULL)
		wlcif = bsscfg->wlcif;
	ASSERT(wlcif != NULL);

	/* Do the actual parameter implementation */
	switch (actionid) {

	case IOV_GVAL(IOV_AUTHE_STA_LIST):
		/* return buf is a maclist */
		err = wlc_sta_list_get(ap, bsscfg, (uint8*)arg, len,
		                       wlc_authenticated_sta_check_cb);
		break;

	case IOV_GVAL(IOV_AUTHO_STA_LIST):
		/* return buf is a maclist */
		err = wlc_sta_list_get(ap, bsscfg, (uint8*)arg, len,
		                       wlc_authorized_sta_check_cb);
		break;

	case IOV_GVAL(IOV_WME_STA_LIST):	/* Deprecated; use IOV_STA_INFO */
		/* return buf is a maclist */
		err = wlc_sta_list_get(ap, bsscfg, (uint8*)arg, len,
		                       wlc_wme_sta_check_cb);
		break;

	case IOV_GVAL(IOV_MAXASSOC):
		*(uint32*)arg = wlc_ap_get_maxassoc(wlc->ap);
		break;

	case IOV_SVAL(IOV_MAXASSOC):
		if (int_val > wlc_ap_get_maxassoc_limit(wlc->ap)) {
			err = BCME_RANGE;
			goto exit;
		}
		wlc_ap_set_maxassoc(wlc->ap, int_val);
		break;

#if defined(MBSS) || defined(WLP2P)
	case IOV_GVAL(IOV_BSS_MAXASSOC):
		*(uint32*)arg = bsscfg->maxassoc;
		break;

	case IOV_SVAL(IOV_BSS_MAXASSOC):
		if (int_val > wlc->pub->tunables->maxscb) {
			err = BCME_RANGE;
			goto exit;
		}
		bsscfg->maxassoc = int_val;
		break;
#endif /* MBSS || WLP2P */

	case IOV_GVAL(IOV_AP_ISOLATE):
		*ret_int_ptr = (int32)bsscfg->ap_isolate;
		break;

	case IOV_GVAL(IOV_AP):
		*((uint*)arg) = BSSCFG_AP(bsscfg);
		break;

#if defined(STA) && defined(AP)
	case IOV_SVAL(IOV_AP): {
		wlc_bsscfg_type_t type = {BSSCFG_TYPE_GENERIC, BSSCFG_SUBTYPE_NONE};
		if (!APSTA_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		type.subtype = bool_val ? BSSCFG_GENERIC_AP : BSSCFG_GENERIC_STA;
		err = wlc_bsscfg_reinit(wlc, bsscfg, &type, 0);
		break;
	}
#endif /* defined(STA) && defined(AP) */

	case IOV_SVAL(IOV_AP_ISOLATE):
		if (!BCMPCIEDEV_ENAB())
			bsscfg->ap_isolate = (uint8)int_val;
		break;
	case IOV_GVAL(IOV_SCB_ACTIVITY_TIME):
		*ret_int_ptr = (int32)appvt->scb_activity_time;
		break;

	case IOV_SVAL(IOV_SCB_ACTIVITY_TIME):
		appvt->scb_activity_time = (uint32)int_val;
		break;

	case IOV_GVAL(IOV_CLOSEDNET):
		*ret_int_ptr = bsscfg->closednet;
		break;

	case IOV_SVAL(IOV_CLOSEDNET):
		/* "closednet" control two functionalities: hide ssid in bcns
		 * and don't respond to broadcast probe requests
		 */
		bsscfg->closednet = bool_val;
		if (BSSCFG_AP(bsscfg) && bsscfg->up) {
			wlc_bss_update_beacon(wlc, bsscfg, TRUE);
			wlc_bss_update_probe_resp(wlc, bsscfg, TRUE);
#if defined(MBSS)
			if (MBSS_ENAB(wlc->pub))
				wlc_mbss16_upd_closednet(wlc, bsscfg);
			else
#endif
				if (D11REV_LT(wlc->pub->corerev, 128)) {
					wlc_mctrl(wlc, MCTL_CLOSED_NETWORK,
						(bool_val ? MCTL_CLOSED_NETWORK : 0));
				} else {
					wlc_mhf(wlc, MHF4, MHF4_CLOSED_NETWORK,
						bool_val ? MHF4_CLOSED_NETWORK : 0, WLC_BAND_ALL);
				}
		}
		break;

	case IOV_GVAL(IOV_BSS):
		if (p_len < (int)sizeof(int)) {
			err = BCME_BUFTOOSHORT;
			break;
		}
		if (int_val >= 0) {
			bsscfg = wlc_bsscfg_find(wlc, int_val, &err);
		} /* otherwise, use the value from the wlif object */

		if (bsscfg)
			*ret_int_ptr = bsscfg->up;
		else if (err == BCME_NOTFOUND)
			*ret_int_ptr = 0;
		else
			break;
		break;

	case IOV_SVAL(IOV_BSS): {
		/* int_val  : bssidx
		 * int_val2 : operation (up, down, etc)
		 */
		bool sta = TRUE;
		wlc_bsscfg_type_t type = {BSSCFG_TYPE_GENERIC, BSSCFG_SUBTYPE_NONE};

		if (len < (int)(2 * sizeof(int))) {
			err = BCME_BUFTOOSHORT;
			break;
		}
#ifdef STA
		if (int_val2 == WLC_AP_IOV_OP_MANUAL_AP_BSSCFG_CREATE) {
			sta = FALSE;
		}
		else if (int_val2 == WLC_AP_IOV_OP_MANUAL_STA_BSSCFG_CREATE) {
			sta = TRUE;
		}
#endif
		/* On an 'up', use wlc_bsscfg_alloc() to create a bsscfg if it does not exist,
		 * but on a 'down', just find the bsscfg if it already exists
		 */
		if (int_val >= 0) { /* means: caller references an exisisting bss */
			bsscfg = wlc_bsscfg_find(wlc, int_val, &err);
			if (int_val2 > WLC_AP_IOV_OP_DISABLE &&
			    bsscfg == NULL && err == BCME_NOTFOUND) {
				type.subtype = !sta ? BSSCFG_GENERIC_AP : BSSCFG_GENERIC_STA;
				bsscfg = wlc_bsscfg_alloc(wlc, int_val, &type, 0, 0, NULL);
				if (bsscfg == NULL)
					err = BCME_NOMEM;
				else if ((err = wlc_bsscfg_init(wlc, bsscfg))) {
					WL_ERROR(("wl%d: wlc_bsscfg_init %s failed (%d)\n",
					          wlc->pub->unit, sta ? "STA" : "AP", err));
					wlc_bsscfg_free(wlc, bsscfg);
					break;
				}
			} else if (bsscfg != NULL) {
				/*
				 * This means a bsscfg was already found for the given
				 * index. The user is trying to change the role, change
				 * it only if needed. If request is to become AP and we
				 * are STA OR if the request is to become STA and we are AP
				 * change the role
				 * Reinit only on explicit role change parameter
				 */

				if (((int_val2 == WLC_AP_IOV_OP_MANUAL_AP_BSSCFG_CREATE) ||
					(int_val2 == WLC_AP_IOV_OP_MANUAL_STA_BSSCFG_CREATE)) &&
					((!sta && BSSCFG_STA(bsscfg)) ||
					(sta && BSSCFG_AP(bsscfg)))) {
					type.subtype = !sta ? BSSCFG_GENERIC_AP :
						BSSCFG_GENERIC_STA;
					wlc_bsscfg_reinit(wlc, bsscfg, &type, 0);
					wlc_set_wake_ctrl(wlc);
				}
			} /* bsscfg is found */
		}

#ifdef STA
		if (int_val2 == WLC_AP_IOV_OP_MANUAL_STA_BSSCFG_CREATE)
			break;
		else if (int_val2 == WLC_AP_IOV_OP_MANUAL_AP_BSSCFG_CREATE)
			break;
#endif
		if (bsscfg == NULL) {
			/* do not error on a 'down' of a nonexistent bsscfg */
			if (err == BCME_NOTFOUND && int_val2 == WLC_AP_IOV_OP_DISABLE)
				err = 0;
			break;
		}

		if (int_val2 > WLC_AP_IOV_OP_DISABLE) {
			if (bsscfg->up) {
				WL_APSTA_UPDN(("wl%d: Ignoring UP, bsscfg %d already UP\n",
					wlc->pub->unit, int_val));
				break;
			}
			if (mboolisset(wlc->pub->radio_disabled, WL_RADIO_HW_DISABLE) ||
				mboolisset(wlc->pub->radio_disabled, WL_RADIO_SW_DISABLE)) {
				WL_APSTA_UPDN(("wl%d: Ignoring UP, bsscfg %d; radio off\n",
					wlc->pub->unit, int_val));
				err = BCME_RADIOOFF;
				break;
			}

			WL_APSTA_UPDN(("wl%d: BSS up cfg %d (%s) -> wlc_bsscfg_enable()\n",
				wlc->pub->unit, int_val, (BSSCFG_AP(bsscfg) ? "AP" : "STA")));
			if (BSSCFG_AP(bsscfg))
				err = wlc_bsscfg_enable(wlc, bsscfg);
#ifdef WLP2P
			else if (BSS_P2P_ENAB(wlc, bsscfg))
				err = BCME_ERROR;
#endif
#ifdef STA
			else if (BSSCFG_STA(bsscfg) && (bsscfg->SSID_len != 0))
				wlc_join(wlc, bsscfg, bsscfg->SSID, bsscfg->SSID_len,
				         NULL, NULL, 0);
#endif
			if (err)
				break;
		} else {
			if (!bsscfg->enable) {
				WL_APSTA_UPDN(("wl%d: Ignoring DOWN, bsscfg %d already DISABLED\n",
					wlc->pub->unit, int_val));
				break;
			}
			WL_APSTA_UPDN(("wl%d: BSS down on %d (%s) -> wlc_bsscfg_disable()\n",
				wlc->pub->unit, int_val, (BSSCFG_AP(bsscfg) ? "AP" : "STA")));
			wlc_bsscfg_disable(wlc, bsscfg);
#ifdef WLDFS
			/* Turn of radar_detect if none of AP's are on radar chanspec */
			if (WLDFS_ENAB(wlc->pub) && WL11H_AP_ENAB(wlc)) {
				if (!wlc_ap_on_radarchan(wlc->ap) && !(BSSCFG_SRADAR_ENAB(bsscfg) ||
					BSSCFG_AP_NORADAR_CHAN_ENAB(bsscfg)))
					wlc_set_dfs_cacstate(wlc->dfs, OFF, bsscfg);
			}
#endif
		}
		break;
	}

#if defined(STA) /* APSTA */
	case IOV_GVAL(IOV_APSTA):
		*ret_int_ptr = APSTA_ENAB(wlc->pub);
		break;

	case IOV_SVAL(IOV_APSTA): {
		wlc_bsscfg_type_t type = {BSSCFG_TYPE_GENERIC, BSSCFG_SUBTYPE_NONE};

		/* Flagged for no set while up */
		if (bool_val == APSTA_ENAB(wlc->pub)) {
			WL_APSTA(("wl%d: No change to APSTA mode\n", wlc->pub->unit));
			break;
		}

		bsscfg = wlc->primary_bsscfg;
		if (bool_val) {
			/* Turning on APSTA, force various other items:
			 *   Global AP, cfg (wlc->primary_bsscfg) STA, not IBSS.
			 *   Make beacon/probe AP config bsscfg[1].
			 *   Force off 11D.
			 */
			WL_APSTA(("wl%d: Enabling APSTA mode\n", wlc->pub->unit));
			if (bsscfg->enable)
				wlc_bsscfg_disable(wlc, bsscfg);
		} else {
			/* Turn off APSTA: make global AP and cfg[0] same */
			WL_APSTA(("wl%d: Disabling APSTA mode\n", wlc->pub->unit));
		}
		type.subtype = bool_val ? BSSCFG_GENERIC_STA : BSSCFG_GENERIC_AP;
		err = wlc_bsscfg_reinit(wlc, bsscfg, &type, 0);
		if (err)
			break;
		wlc->pub->_ap = TRUE;
		if (bool_val) {
			wlc->default_bss->bss_type = DOT11_BSSTYPE_INFRASTRUCTURE;
		}
		wlc->pub->_apsta = bool_val;

		/* Act similarly to WLC_SET_AP */
		wlc_ap_upd(wlc, bsscfg);
		wlc->wet = FALSE;
		wlc->wet_dongle = FALSE;
		wlc_radio_mpc_upd(wlc);
		break;
	}
#endif /* APSTA */

#ifdef RXCHAIN_PWRSAVE
	case IOV_GVAL(IOV_RXCHAIN_PWRSAVE_ENABLE):
		*ret_int_ptr = ap->rxchain_pwrsave_enable;
		break;

	case IOV_SVAL(IOV_RXCHAIN_PWRSAVE_ENABLE):
		if (!int_val)
			appvt->rxchain_pwrsave.pwrsave.power_save_check &= ~PWRSAVE_ENB;
		else
			appvt->rxchain_pwrsave.pwrsave.power_save_check |= PWRSAVE_ENB;

		ap->rxchain_pwrsave_enable = int_val;
		if (!int_val)
			wlc_reset_rxchain_pwrsave_mode(ap);

#ifdef WLPM_BCNRX
		if (PM_BCNRX_ENAB(wlc->pub) && bool_val) {
			/* Avoid ucode interference if AP enables this power-save mode */
			wlc_pm_bcnrx_disable(wlc);
		}
#endif
		break;
	case IOV_GVAL(IOV_RXCHAIN_PWRSAVE_QUIET_TIME):
	        *ret_int_ptr = appvt->rxchain_pwrsave.pwrsave.quiet_time;
		break;

	case IOV_SVAL(IOV_RXCHAIN_PWRSAVE_QUIET_TIME):
		appvt->rxchain_pwrsave.pwrsave.quiet_time = int_val;
		break;
	case IOV_GVAL(IOV_RXCHAIN_PWRSAVE_PPS):
	        *ret_int_ptr = appvt->rxchain_pwrsave.pwrsave.pps_threshold;
		break;

	case IOV_SVAL(IOV_RXCHAIN_PWRSAVE_PPS):
		appvt->rxchain_pwrsave.pwrsave.pps_threshold = int_val;
		break;
	case IOV_GVAL(IOV_RXCHAIN_PWRSAVE):
	        *ret_int_ptr = appvt->rxchain_pwrsave.pwrsave.in_power_save;
		break;
	case IOV_GVAL(IOV_RXCHAIN_PWRSAVE_STAS_ASSOC_CHECK):
	        *ret_int_ptr = appvt->rxchain_pwrsave.pwrsave.stas_assoc_check;
		break;
	case IOV_SVAL(IOV_RXCHAIN_PWRSAVE_STAS_ASSOC_CHECK):
		appvt->rxchain_pwrsave.pwrsave.stas_assoc_check = int_val;
		if (int_val && ap->rxchain_pwrsave_enable &&
		    appvt->rxchain_pwrsave.pwrsave.in_power_save &&
		    wlc_ap_stas_associated(wlc->ap)) {
			wlc_reset_rxchain_pwrsave_mode(ap);
		}
		break;
#endif /* RXCHAIN_PWRSAVE */
#ifdef RADIO_PWRSAVE
	case IOV_GVAL(IOV_RADIO_PWRSAVE_ENABLE):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		*ret_int_ptr = ap->radio_pwrsave_enable;
		break;

	case IOV_SVAL(IOV_RADIO_PWRSAVE_ENABLE):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		if (!MBSS_ENAB(wlc->pub)) {
			err = BCME_EPERM;
			WL_ERROR(("wl%d: Radio pwrsave not supported in non-mbss case yet.\n",
				wlc->pub->unit));
			break;
		}
		ap->radio_pwrsave_enable = appvt->radio_pwrsave.pwrsave.power_save_check = int_val;
		wlc_reset_radio_pwrsave_mode(ap);

#ifdef WLPM_BCNRX
		if (PM_BCNRX_ENAB(wlc->pub) && bool_val) {
			/* Avoid ucode interference if AP enables this power-save mode */
			wlc_pm_bcnrx_disable(wlc);
		}
#endif
		break;
	case IOV_GVAL(IOV_RADIO_PWRSAVE_QUIET_TIME):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		*ret_int_ptr = appvt->radio_pwrsave.pwrsave.quiet_time;
		break;

	case IOV_SVAL(IOV_RADIO_PWRSAVE_QUIET_TIME):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		appvt->radio_pwrsave.pwrsave.quiet_time = int_val;
		break;
	case IOV_GVAL(IOV_RADIO_PWRSAVE_PPS):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		*ret_int_ptr = appvt->radio_pwrsave.pwrsave.pps_threshold;
		break;

	case IOV_SVAL(IOV_RADIO_PWRSAVE_PPS):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		appvt->radio_pwrsave.pwrsave.pps_threshold = int_val;
		break;
	case IOV_GVAL(IOV_RADIO_PWRSAVE):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		*ret_int_ptr = appvt->radio_pwrsave.pwrsave.in_power_save;
		break;
	case IOV_SVAL(IOV_RADIO_PWRSAVE_LEVEL):{
		uint8 dtim_period;
		uint16 beacon_period;

		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		bsscfg = wlc->primary_bsscfg;

		if (bsscfg->associated) {
			dtim_period = bsscfg->current_bss->dtim_period;
			beacon_period = bsscfg->current_bss->beacon_period;
		} else {
			dtim_period = wlc->default_bss->dtim_period;
			beacon_period = wlc->default_bss->beacon_period;
		}

		if (int_val > RADIO_PWRSAVE_HIGH) {
			err = BCME_RANGE;
			goto exit;
		}

		if (dtim_period == 1) {
			err = BCME_ERROR;
			goto exit;
		}

		appvt->radio_pwrsave.level = int_val;
		switch (appvt->radio_pwrsave.level) {
		case RADIO_PWRSAVE_LOW:
			appvt->radio_pwrsave.on_time = 2*beacon_period*dtim_period/3;
			appvt->radio_pwrsave.off_time = beacon_period*dtim_period/3;
			break;
		case RADIO_PWRSAVE_MEDIUM:
			appvt->radio_pwrsave.on_time = beacon_period*dtim_period/2;
			appvt->radio_pwrsave.off_time = beacon_period*dtim_period/2;
			break;
		case RADIO_PWRSAVE_HIGH:
			appvt->radio_pwrsave.on_time = beacon_period*dtim_period/3;
			appvt->radio_pwrsave.off_time = 2*beacon_period*dtim_period/3;
			break;
		}
		break;
	}
	case IOV_GVAL(IOV_RADIO_PWRSAVE_LEVEL):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		*ret_int_ptr = appvt->radio_pwrsave.level;
		break;
	case IOV_SVAL(IOV_RADIO_PWRSAVE_STAS_ASSOC_CHECK):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		appvt->radio_pwrsave.pwrsave.stas_assoc_check = int_val;
		if (int_val && RADIO_PWRSAVE_ENAB(wlc->ap) &&
		    wlc_radio_pwrsave_in_power_save(wlc->ap) &&
		    wlc_ap_stas_associated(wlc->ap)) {
			wlc_radio_pwrsave_exit_mode(wlc->ap);
			WL_INFORM(("Going out of power save as there are associated STASs!\n"));
		}
		break;
	case IOV_GVAL(IOV_RADIO_PWRSAVE_STAS_ASSOC_CHECK):
		if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
			err = BCME_UNSUPPORTED;
			break;
		}
		*ret_int_ptr = appvt->radio_pwrsave.pwrsave.stas_assoc_check;
		break;
#endif /* RADIO_PWRSAVE */
#ifdef BCM_DCS
	case IOV_GVAL(IOV_BCMDCS):
		*ret_int_ptr = ap->dcs_enabled ? TRUE : FALSE;
		break;

	case IOV_SVAL(IOV_BCMDCS):
		ap->dcs_enabled = bool_val;
		break;

#endif /* BCM_DCS */
	case IOV_GVAL(IOV_DYNBCN):
		*ret_int_ptr = (int32)((bsscfg->flags & WLC_BSSCFG_DYNBCN) == WLC_BSSCFG_DYNBCN);
		break;
	case IOV_SVAL(IOV_DYNBCN):
		if (!BSSCFG_AP(bsscfg)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		if (int_val && !DYNBCN_ENAB(bsscfg)) {
			bsscfg->flags |= WLC_BSSCFG_DYNBCN;

			/* Disable beacons if no sta is associated */
			if (wlc_bss_assocscb_getcnt(wlc, bsscfg) == 0)
				wlc_bsscfg_bcn_disable(wlc, bsscfg);
		} else if (!int_val && DYNBCN_ENAB(bsscfg)) {
			bsscfg->flags &= ~WLC_BSSCFG_DYNBCN;
			wlc_bsscfg_bcn_enable(wlc, bsscfg);
		}
		break;

	case IOV_GVAL(IOV_SCB_LASTUSED): {
		uint elapsed = 0;
		uint min_val = (uint)-1;
		struct scb *scb;
		struct scb_iter scbiter;

		FOREACHSCB(wlc->scbstate, &scbiter, scb) {
			if (SCB_ASSOCIATED(scb)) {
				elapsed = wlc->pub->now - scb->used;
				if (elapsed < min_val)
					min_val = elapsed;
			}
		}
		*ret_int_ptr = min_val;
		break;
	}

	case IOV_GVAL(IOV_SCB_PROBE): {
		wl_scb_probe_t scb_probe;

		scb_probe.scb_timeout = appvt->scb_timeout;
		scb_probe.scb_activity_time = appvt->scb_activity_time;
		scb_probe.scb_max_probe = appvt->scb_max_probe;

		bcopy((char *)&scb_probe, (char *)arg, sizeof(wl_scb_probe_t));
		break;
	}

	case IOV_SVAL(IOV_SCB_PROBE): {
		wl_scb_probe_t *scb_probe = (wl_scb_probe_t *)arg;

		if (!scb_probe->scb_timeout || (!scb_probe->scb_max_probe)) {
			err = BCME_BADARG;
			break;
		}

		appvt->scb_timeout = scb_probe->scb_timeout;
		appvt->scb_activity_time = scb_probe->scb_activity_time;
		appvt->scb_max_probe = scb_probe->scb_max_probe;
		break;
	}

	case IOV_GVAL(IOV_SCB_ASSOCED): {

		bool assoced = TRUE;
		struct scb *scb;
		struct scb_iter scbiter;

		FOREACHSCB(wlc->scbstate, &scbiter, scb) {
			if (SCB_ASSOCIATED(scb))
				break;
		}

		if (!scb)
			assoced = FALSE;

		*ret_int_ptr = (uint32)assoced;
		break;
	}

	case IOV_SVAL(IOV_ACS_UPDATE):

		if (SCAN_IN_PROGRESS(wlc->scan)) {
			err = BCME_BUSY;
			break;
		}

		if (!wlc_valid_chanspec_db(wlc->cmi, appvt->chanspec_selected)) {
			WL_ERROR(("wl%d: %s: IOV_ACS_UPDATE: Failure. Invalid chanspec 0x%X\n",
				wlc->pub->unit, __FUNCTION__, appvt->chanspec_selected));
			err = BCME_BADCHAN;
			break;
		}

		if (wlc->pub->up && (appvt->chanspec_selected != 0) &&
		    (phy_utils_get_chanspec(WLC_PI(wlc)) != appvt->chanspec_selected))
			wlc_ap_acs_update(wlc);

#ifdef WLDFS
		if (WLDFS_ENAB(wlc->pub) && WL11H_AP_ENAB(wlc) && AP_ACTIVE(wlc)) {
			if (wlc_radar_chanspec(wlc->cmi, appvt->chanspec_selected))
				wlc_set_dfs_cacstate(wlc->dfs, ON, bsscfg);
			else
				wlc_set_dfs_cacstate(wlc->dfs, OFF, bsscfg);
		}
#endif
		break;

	case IOV_GVAL(IOV_PROBE_RESP_INFO): {
		uint tmp_len = len - sizeof(int_val);

		err = wlc_ap_get_bcnprb(wlc, bsscfg, FALSE, ((int32*)arg + 1), &tmp_len);
		if (err == BCME_OK) {
			*ret_int_ptr = (int32)tmp_len;
		}

	        break;
	}
	case IOV_GVAL(IOV_MODE_REQD):
		*ret_int_ptr = (int32)wlc_ap_get_opmode_cap_reqd(wlc, bsscfg);
		break;
	case IOV_SVAL(IOV_MODE_REQD):
		err = wlc_ap_set_opmode_cap_reqd(wlc, bsscfg, int_val);
		break;

	case IOV_GVAL(IOV_BSS_RATESET):
		if (bsscfg)
			*ret_int_ptr = (int32)bsscfg->rateset;
		else {
			*ret_int_ptr = 0;
			err = BCME_BADARG;
		}
		break;

	case IOV_SVAL(IOV_BSS_RATESET):
		if (!bsscfg || int_val < WLC_BSSCFG_RATESET_DEFAULT ||
		            int_val > WLC_BSSCFG_RATESET_MAX)
			err = BCME_BADARG;
		else if (bsscfg->up)
			/* do not change rateset while this bss is up */
			err = BCME_NOTDOWN;
		else
			bsscfg->rateset = (uint8)int_val;
		break;

	case IOV_GVAL(IOV_FORCE_BCN_RSPEC):
		*ret_int_ptr = appvt->user_forced_bcn_rspec;
		break;

	case IOV_SVAL(IOV_FORCE_BCN_RSPEC): {
		err = wlc_user_forced_bcn_rspec_upd(wlc, bsscfg, ap, int_val);
		break;
	}
	break;

#ifdef WLAUTHRESP_MAC_FILTER
	case IOV_GVAL(IOV_AUTHRESP_MACFLTR):
		if (BSSCFG_AP(bsscfg))
			*ret_int_ptr = (int32)(AP_BSSCFG_CUBBY(appvt, bsscfg)->authresp_macfltr);
		else
			err = BCME_NOTAP;
		break;

	case IOV_SVAL(IOV_AUTHRESP_MACFLTR):
		if (BSSCFG_AP(bsscfg))
			AP_BSSCFG_CUBBY(appvt, bsscfg)->authresp_macfltr = bool_val;
		else
			err = BCME_NOTAP;
		break;
#endif /* WLAUTHRESP_MAC_FILTER */

	case IOV_GVAL(IOV_PROXY_ARP_ADVERTISE):
		if (BSSCFG_AP(bsscfg)) {
			*ret_int_ptr = isset(bsscfg->ext_cap, DOT11_EXT_CAP_PROXY_ARP) ? 1 : 0;
		} else {
			err = BCME_NOTAP;
		}
		break;

	case IOV_SVAL(IOV_PROXY_ARP_ADVERTISE):
		if (BSSCFG_AP(bsscfg)) {
			/* update extend capabilities */
			wlc_bsscfg_set_ext_cap(bsscfg, DOT11_EXT_CAP_PROXY_ARP, bool_val);
			if (bsscfg->up) {
				/* update proxy arp service bit in probe response and beacons */
				wlc_bss_update_beacon(wlc, bsscfg, TRUE);
				wlc_bss_update_probe_resp(wlc, bsscfg, TRUE);
			}
		} else {
			err = BCME_NOTAP;
		}
		break;

	case IOV_SVAL(IOV_WOWL_PKT): {
	        /* Send a Wake packet to a ea */
	        /* The format of this iovar params is
		 *   pkt_len
		 *   dst
		 *   Type of pkt - WL_WOWL_MAGIC or WL_WOWL_NET
		 *   for net pkt, wl_wowl_pattern_t
		 *   for magic pkt, dst ea, sta ea
		 */
		struct type_len_ea {
			uint16 pktlen;
			struct ether_addr dst;
			uint16 type;
		} tlea;

		wl_wowl_pattern_t wl_pattern;
		void *pkt;
		uint loc = 0;
		struct ether_header *eh;

		bcopy((uint8*)arg, (uint8 *) &tlea, sizeof(struct type_len_ea));
		loc += sizeof(struct type_len_ea);

		if (tlea.type  != WL_WOWL_NET &&
		    tlea.type  != WL_WOWL_MAGIC &&
			tlea.type  != WL_WOWL_EAPID) {
			err = BCME_BADARG;
			break;
		}

		if (tlea.type == WL_WOWL_NET) {
			uint8 *pattern;
			uint8 *buf;
			uint8 *wakeup_reason;

			bcopy(((uint8 *)arg + loc),
				(uint8 *) &wl_pattern, sizeof(wl_wowl_pattern_t));
			if (tlea.pktlen <
					(wl_pattern.offset + wl_pattern.patternsize
						+ wl_pattern.reasonsize)) {
				err = BCME_RANGE;
				break;
			}

			if ((pkt = PKTGET(wlc->osh, tlea.pktlen +
			                  sizeof(struct ether_header) + wlc->txhroff,
			                  TRUE)) == NULL) {
				err = BCME_NOMEM;
				break;
			}

			WLPKTTAG(pkt)->flags3 |= WLF3_BYPASS_AMPDU;
			PKTPULL(wlc->osh, pkt, wlc->txhroff);

			eh = (struct ether_header *)PKTDATA(wlc->osh, pkt);
			buf = (uint8 *)eh;

			/* Go to end to find pattern */
			pattern = ((uint8*)arg + loc + wl_pattern.patternoffset);
			bcopy(pattern, &buf[wl_pattern.offset], wl_pattern.patternsize);
	                if (wl_pattern.reasonsize) {
				wakeup_reason = ((uint8*)pattern + wl_pattern.patternsize);
				bcopy(wakeup_reason,
					&buf[wl_pattern.offset + wl_pattern.patternsize],
					wl_pattern.reasonsize);
			}
			bcopy((char *)&wlc->pub->cur_etheraddr, (char *)eh->ether_shost,
			      ETHER_ADDR_LEN);
			if (wl_pattern.offset >= ETHER_ADDR_LEN)
				bcopy((char *)&tlea.dst, (char *)eh->ether_dhost, ETHER_ADDR_LEN);

			wl_msg_level |= WL_PRPKT_VAL;
			wlc_sendpkt(wlc, pkt, NULL);
			wl_msg_level &= ~WL_PRPKT_VAL;
		} else if (tlea.type == WL_WOWL_MAGIC) {
			struct ether_addr ea;
			uint8 *buf, ptr, extended_magic_pattern[6];
			bool extended_magic = FALSE;
			int i, j;

			if (tlea.pktlen < MAGIC_PKT_MINLEN) {
				err = BCME_RANGE;
				break;
			}

			bcopy(((uint8 *)arg + loc), (char *)&ea, ETHER_ADDR_LEN);

			if (wlc_iovar_op(wlc, "wowl_ext_magic", NULL, 0,
				extended_magic_pattern, sizeof(extended_magic_pattern),
				IOV_GET, NULL) == BCME_OK)
				extended_magic = TRUE;

			if ((pkt = PKTGET(wlc->osh, tlea.pktlen +
				(extended_magic ? sizeof(extended_magic_pattern) : 0) +
			                  sizeof(struct ether_header) + wlc->txhroff,
			                  TRUE)) == NULL) {
				err = BCME_NOMEM;
				break;
			}
			WLPKTTAG(pkt)->flags3 |= WLF3_BYPASS_AMPDU;
			PKTPULL(wlc->osh, pkt, wlc->txhroff);
			eh = (struct ether_header *)PKTDATA(wlc->osh, pkt);
			bcopy((char *)&wlc->pub->cur_etheraddr, (char *)eh->ether_shost,
			      ETHER_ADDR_LEN);
			bcopy((char *)&tlea.dst, (char*)eh->ether_dhost, ETHER_ADDR_LEN);
			eh->ether_type = hton16(ETHER_TYPE_MIN);
			buf = ((uint8 *)eh + sizeof(struct ether_header));
			for (i = 0; i < 6; i++)
				buf[i] = 0xff;
			ptr = 6;
			for (j = 0; j < 16; j++) {
				bcopy(&ea, buf + ptr, ETHER_ADDR_LEN);
				ptr += 6;
			}
			if (extended_magic)
				bcopy(extended_magic_pattern,
					buf + ptr,
					sizeof(extended_magic_pattern));
			wl_msg_level |= WL_PRPKT_VAL;
			wlc_sendpkt(wlc, pkt, NULL);
			wl_msg_level &= ~WL_PRPKT_VAL;
		} else if (tlea.type == WL_WOWL_EAPID) {
			uint8 id_hdr[] = {0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01};
			uint8 id_len = *((uint8 *)arg + loc);
			uint16 body_len = 5 + id_len;
			uint8 *buf;

			if ((pkt = PKTGET(wlc->osh, sizeof(id_hdr) + id_len +
			                  sizeof(struct ether_header) + wlc->txhroff,
			                  TRUE)) == NULL) {
				err = BCME_NOMEM;
				break;
			}
			WLPKTTAG(pkt)->flags3 |= WLF3_BYPASS_AMPDU;
			PKTPULL(wlc->osh, pkt, wlc->txhroff);

			eh = (struct ether_header *)PKTDATA(wlc->osh, pkt);
			bcopy((char *)&wlc->pub->cur_etheraddr, (char *)eh->ether_shost,
			      ETHER_ADDR_LEN);
			bcopy((char *)&tlea.dst, (char*)eh->ether_dhost, ETHER_ADDR_LEN);
			eh->ether_type = hton16(ETHER_TYPE_802_1X);

			*((uint16*)&id_hdr[2]) = hton16(body_len);
			*((uint16*)&id_hdr[6]) = hton16(body_len);

			buf = (uint8 *)eh + sizeof(struct ether_header);
			bcopy(id_hdr, buf, sizeof(id_hdr));
			buf += sizeof(id_hdr);
			bcopy((uint8*)arg + loc + 1, buf, id_len);

			wl_msg_level |= WL_PRPKT_VAL;
			wlc_sendpkt(wlc, pkt, NULL);
			wl_msg_level &= ~WL_PRPKT_VAL;
		} else
			err = BCME_UNSUPPORTED;
		break;
	}
	case IOV_SVAL(IOV_SET_RADAR): {
		uint subband;
#ifndef WLDFS
		BCM_REFERENCE(subband);
#endif /* WLDFS */
		if (p_len >= sizeof(int) * 2) {
			subband = (uint) int_val2;
		} else {
			subband = DFS_DEFAULT_SUBBANDS;
		}
		if (WLDFS_ENAB(wlc->pub)) {
			ASSERT(ret_int_ptr != NULL);
			*ret_int_ptr = wlc_dfs_set_radar(wlc->dfs, int_val, subband);
			if (*ret_int_ptr != BCME_OK) {
				err = *ret_int_ptr;
			}
		} else {
			err = BCME_UNSUPPORTED;
		}
		break;
	}
	case IOV_GVAL(IOV_SET_RADAR):
		ASSERT(ret_int_ptr != NULL);
		if (WLDFS_ENAB(wlc->pub)) {
			*ret_int_ptr = (int32)wlc_dfs_get_radar(wlc->dfs);
		} else {
			err = BCME_UNSUPPORTED;
		}
		break;

#ifdef WL_MCAST_FILTER_NOSTA
	case IOV_SVAL(IOV_MCAST_FILTER_NOSTA):
		bsscfg->mcast_filter_nosta = bool_val;
		break;
	case IOV_GVAL(IOV_MCAST_FILTER_NOSTA):
		*ret_int_ptr = (int32)bsscfg->mcast_filter_nosta;
		break;
#endif

#if defined(BCM_CEVENT) && !defined(BCM_CEVENT_DISABLED)
	case IOV_SVAL(IOV_CEVENT):
		wlc->pub->_cevent = (uint32)int_val;
		break;

	case IOV_GVAL(IOV_CEVENT):
		*ret_int_ptr = wlc->pub->_cevent;
		break;
#endif /* BCM_CEVENT && ! BCM_CEVENT_DISABLED */

#ifdef WL_TRAFFIC_THRESH
	case IOV_SVAL(IOV_TRF_THOLD):
	{
		wl_traffic_thresh_t *req = (wl_traffic_thresh_t *) params;
		if (p_len < sizeof(wl_traffic_thresh_t)) {
			WL_ERROR(("wl%d: size error\n", wlc->pub->unit));
			err = BCME_USAGE_ERROR;
			break;
		}
		if (BSSCFG_STA(bsscfg)) {
			WL_INFORM(("wl%d: traffic_thresh settings not supported for sta\n",
					wlc->pub->unit));
			err = BCME_UNSUPPORTED;
			break;
		}
		err = wlc_traffic_thresh_set_val(wlc, bsscfg, req);
	}
		break;
	case IOV_GVAL(IOV_TRF_THOLD):
	{
		wl_traffic_thresh_t *req = (wl_traffic_thresh_t *) params;
		struct get_traffic_thresh *res = (struct get_traffic_thresh *) arg;
		if (p_len < sizeof(wl_traffic_thresh_t)) {
			WL_ERROR(("wl%d: size error\n", wlc->pub->unit));
			err = BCME_USAGE_ERROR;
			break;
		}
		if (req->type == WL_TRF_DU) {
			err = wlc_traffic_thresh_get_val(wlc, bsscfg, req, res);
		}
	}

		break;
#endif /* WL_TRAFFIC_THRESH */
#ifdef MULTIAP
	case IOV_GVAL(IOV_MAP):
		*ret_int_ptr = 0;
		if (bsscfg->map_attr & MAP_EXT_ATTR_FRNTHAUL_BSS) {
			setbit(ret_int_ptr, 0);
		}
		if (bsscfg->map_attr & MAP_EXT_ATTR_BACKHAUL_BSS) {
			setbit(ret_int_ptr, 1);
		}
		if (bsscfg->map_attr & MAP_EXT_ATTR_BACKHAUL_STA) {
			setbit(ret_int_ptr, 2);
		}
		if (bsscfg->map_attr & MAP_EXT_ATTR_PROFILE1_STA_ASSOC_DISALLOWED) {
			setbit(ret_int_ptr, 5);
		}
		if (bsscfg->map_attr & MAP_EXT_ATTR_PROFILE2_STA_ASSOC_DISALLOWED) {
			setbit(ret_int_ptr, 6);
		}
		if (bsscfg->flags2 & WLC_BSSCFG_FL2_BLOCK_STA_ON_MAPBH) {
			setbit(ret_int_ptr, 7);
		}
		break;
	case IOV_SVAL(IOV_MAP):
	{
		uint8 flags = 0;

		if ((isset(&int_val, 0) || isset(&int_val, 1)) && !BSSCFG_AP(bsscfg)) {
			WL_ERROR(("wl%d.%d: Error. Trying to set Multi-AP Fronthaul or Backhaul AP"
				"on a non AP interface\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			err = BCME_UNSUPPORTED;
			break;
		}

		if (isset(&int_val, 2) && !BSSCFG_STA(bsscfg)) {
			WL_ERROR(("wl%d.%d: Error. Trying to set Multi-AP Backhaul STA on a non "
				"STA interface\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			err = BCME_UNSUPPORTED;
			break;
		}

		if ((isset(&int_val, 5) || isset(&int_val, 6)) && !isset(&int_val, 1)) {
			WL_ERROR(("wl%d.%d: Error. Trying to set Multi-AP Profile 1/2  disallowed "
				"attributes on a non backhaul bss\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
			err = BCME_UNSUPPORTED;
			break;
		}

		if (isset(&int_val, 0)) {
			flags |= MAP_EXT_ATTR_FRNTHAUL_BSS;
		}
		if (isset(&int_val, 1)) {
			flags |= MAP_EXT_ATTR_BACKHAUL_BSS;
		}
		if (isset(&int_val, 2)) {
			flags |= MAP_EXT_ATTR_BACKHAUL_STA;
		}
		if (isset(&int_val, 5) && (bsscfg->map_profile >= MAP_PROFILE_2)) {
			flags |= MAP_EXT_ATTR_PROFILE1_STA_ASSOC_DISALLOWED;
		}
		if (isset(&int_val, 6) && (bsscfg->map_profile >= MAP_PROFILE_2)) {
			flags |= MAP_EXT_ATTR_PROFILE2_STA_ASSOC_DISALLOWED;
		}
		if (isset(&int_val, 7) && (flags & (MAP_EXT_ATTR_FRNTHAUL_BSS |
			MAP_EXT_ATTR_BACKHAUL_STA))) {
			WL_ERROR(("wl%d: Error. Block Non-MAP STAs feature is only for backhaul "
				"BSS\n", wlc->pub->unit));
			err = BCME_UNSUPPORTED;
			break;
		}
		bsscfg->flags2 &= ~WLC_BSSCFG_FL2_BLOCK_STA_ON_MAPBH;
		if (isset(&int_val, 7)) {
			bsscfg->flags2 |= WLC_BSSCFG_FL2_BLOCK_STA_ON_MAPBH;
		}
		bsscfg->_map = flags ? TRUE : FALSE;
		bsscfg->map_attr = flags;
		(void)wlc_update_multiap_ie(wlc, bsscfg);
	}
	break;
	case IOV_GVAL(IOV_MAP_PROFILE):
		*ret_int_ptr = 0;
		if (bsscfg->map_profile & MAP_PROFILE_1) {
			setbit(ret_int_ptr, 0);
		}
		if (bsscfg->map_profile & MAP_PROFILE_2) {
			setbit(ret_int_ptr, 1);
		}
		break;
	case IOV_SVAL(IOV_MAP_PROFILE):
	{
		uint8 flags = 0;

		if (isset(&int_val, 0)) {
			flags |= MAP_PROFILE_1;
		}
		if (isset(&int_val, 1)) {
			flags |= MAP_PROFILE_2;
		}
		bsscfg->map_profile = flags;
		(void)wlc_update_multiap_ie(wlc, bsscfg);
	}
	break;
	case IOV_GVAL(IOV_MAP_8021Q_SETTINGS):
		*ret_int_ptr = bsscfg->map_prim_vlan_id;
	break;
	case IOV_SVAL(IOV_MAP_8021Q_SETTINGS):
	{
		/* Do not set VLAN ID if the mode is not Backhaul BSS. Also allow to set the
		 * value 0 because, During mode changes, if the new mode is not backhaul bss,
		 * it will rejects wlconf's set to 0 operation and it may have an old value
		 */
		if (!(bsscfg->map_attr & MAP_EXT_ATTR_BACKHAUL_BSS) &&
			(int_val > 0)) {
			WL_ERROR(("Error. Trying to set Multi-AP Default 802.1Q Settings"
				" on a non Backhaul AP interface\n"));
			err = BCME_UNSUPPORTED;
			break;
		}

		bsscfg->map_prim_vlan_id = (uint16)int_val;
		(void)wlc_update_multiap_ie(wlc, bsscfg);
	}
	break;

#endif	/* MULTIAP */
	case IOV_SVAL(IOV_FAR_STA_RSSI):
	if (int_val <= 0) {
		bsscfg->far_sta_rssi = int_val;
	}
	break;

	case IOV_GVAL(IOV_FAR_STA_RSSI):
	*ret_int_ptr = (int32)bsscfg->far_sta_rssi;
	break;
	case IOV_GVAL(IOV_KEEP_AP_UP):
		*ret_int_ptr = wlc->keep_ap_up;
	break;
	case IOV_SVAL(IOV_KEEP_AP_UP):
	{
		wlc->keep_ap_up = (int8)int_val;
		if (APSTA_ENAB(wlc->pub) &&
			((wlc->keep_ap_up == WLC_KEEP_AP_UP_ALWAYS) ||
			((wlc->keep_ap_up == WLC_KEEP_AP_UP_ON_NON_DFS) &&
			(!wlc_radar_chanspec(wlc->cmi, wlc->home_chanspec))))) {
			wlc->primary_bsscfg->disable_ap_up = FALSE;
		}
		break;
	}
#if BAND6G || defined(WL_OCE_AP)
	case IOV_GVAL(IOV_RNR_UPDATE_METHOD):
		*ret_int_ptr = wlc->rnr_update_method;
		break;
	case IOV_SVAL(IOV_RNR_UPDATE_METHOD):
		if (wlc->rnr_update_method == (int8)int_val) {
			break;
		}
		wlc->rnr_update_method = (int8)int_val;
		if (wlc->rnr_update_method == WLC_RNR_UPDATE_METHOD_STATIC) {
			wlc->rnr_update_period = RNR_UPDATE_STATIC_PERIOD;
		} else {
			wlc->rnr_update_period = RNR_UPDATE_SCAN_PERIOD;
		}
		RNR_UPDATE_DEL_TIMER(wlc);
		RNR_UPDATE_ADD_TIMER(wlc, wlc->rnr_update_period);
		break;

	case IOV_GVAL(IOV_FD_PRB_RSP_PERIOD):
		*ret_int_ptr = wlc->upr_fd_info ? wlc->upr_fd_info->period : 0;
		break;

	case IOV_SVAL(IOV_FD_PRB_RSP_PERIOD):
		if (wlc->upr_fd_info) {
			if (wlc->upr_fd_info->period != (int8)int_val) {
				wlc_hrt_del_timeout(wlc->upr_fd_info->timer);
			}
			wlc->upr_fd_info->period = (int8)int_val;

		} else {
			WL_ERROR(("wl%d: Invalid argument to set FD/20TU BCAST PRB RSP\n",
				wlc->pub->unit));
			err = BCME_BADARG;
		}
		break;
	case IOV_GVAL(IOV_UPR_FD_SW):
		*ret_int_ptr = (int32)wlc->pub->_upr_fd_sw;
		break;
	case IOV_SVAL(IOV_UPR_FD_SW):
		if (wlc->pub->_upr_fd_sw != (int8)int_val) {
			wlc->pub->_upr_fd_sw = (int8)int_val;

			if (wlc->pub->up && AP_ENAB(wlc->pub)) {
				/* ensure templates are ready */
				wlc_update_probe_resp(wlc, TRUE);
			}
		}
		break;
	case IOV_GVAL(IOV_RNR_TBTT_LEN):
		*ret_int_ptr = wlc->rnr_tbtt_len;
		break;
	case IOV_SVAL(IOV_RNR_TBTT_LEN):
		if (wlc->rnr_tbtt_len == (int8)int_val) {
			break;
		}
		wlc->rnr_tbtt_len = (int8)int_val;
		break;
#endif /* BAND6G || WL_OCE_AP */
	case IOV_GVAL(IOV_CHECK_SHORT_SSID):
		*ret_int_ptr = wlc->check_short_ssid;
		break;
	case IOV_SVAL(IOV_CHECK_SHORT_SSID):
		wlc->check_short_ssid = (int8)int_val;
		break;
	case IOV_GVAL(IOV_BW_SWITCH_160):
		*ret_int_ptr = wlc->pub->_bw_switch_160;
	break;

	case IOV_SVAL(IOV_BW_SWITCH_160):
		if (int_val < WLC_BW_SWITCH_DISABLE || int_val > WLC_BW_SWITCH_ENABLE) {
			err = BCME_RANGE;
			break;
		}
		if (D11REV_IS(wlc->pub->corerev, 129) &&
			D11MINORREV_LT(wlc->pub->corerev_minor, 2)) {
			wlc->pub->_bw_switch_160 = int_val;
		} else {
			err = BCME_UNSUPPORTED;
		}
	break;

	case IOV_GVAL(IOV_SCB_MARK_DEL_TIMEOUT):
		*ret_int_ptr = appvt->scb_mark_del_timeout;
	break;

	case IOV_SVAL(IOV_SCB_MARK_DEL_TIMEOUT):
		appvt->scb_mark_del_timeout = int_val;
	break;

	case IOV_GVAL(IOV_BCNPRS_TXPWR_OFFSET): {
		/* Return db unit */
		if (BSSCFG_AP(bsscfg) && bsscfg->up) {
#if defined(MBSS)
		if (D11REV_GE(wlc->pub->corerev, 128) && MBSS_ENAB(wlc->pub)) {
			uint8 ucidx = ((bsscfg->cur_etheraddr).octet[5] &
				(WLC_MAX_AP_BSS(wlc->pub->corerev)-1)) & 0xf;
			/* Read the lower byte of shmem since the offset is set */
			/*  in 2 bytes duplicated for 43684. */
			*ret_int_ptr = (wlc_read_shm(wlc,
				M_BSS_BCNPRS_PWR_BLK(wlc) + 2 * ucidx) & 0xff) >> 2;
		}
		else
#endif
			*ret_int_ptr = wlc->stf->bcnprs_txpwr_offset;
		}
		else {
			*ret_int_ptr = 0;
			err = BCME_NOTUP;
		}
	break;
	}

	case IOV_SVAL(IOV_BCNPRS_TXPWR_OFFSET): {
		uint8   offset = (uint8)(int_val & 0xff);
		uint8   limit_offset = 20;

		if (BSSCFG_AP(bsscfg) && bsscfg->up && wlc->clk) {
#if defined(MBSS)
		if (D11REV_GE(wlc->pub->corerev, 128) && MBSS_ENAB(wlc->pub)) {
			/* Use last 4 bits for ucode index */
			uint8 ucidx = ((bsscfg->cur_etheraddr).octet[5] &
			 (WLC_MAX_AP_BSS(wlc->pub->corerev)-1)) & 0xf;
			offset = (int_val >= limit_offset) ? limit_offset : int_val;
			/* Write the offset in two bytes in shmem */
			/* since one byte only control two cores in 43684 */
			/* e.g.: offset 5 => 43684 uses quarter dB backoff:20 => shmem:0x1414 */
			wlc_write_shm(wlc, M_BSS_BCNPRS_PWR_BLK(wlc) + 2 * ucidx,
				(uint16)((offset << 2) + (offset << 10)));
			wlc_mhf(wlc, MHF1, MHF1_PERBSSTXPWR_EN, MHF1_PERBSSTXPWR_EN, WLC_BAND_ALL);
		}
		else
#endif
		{

			/* limitation of supported backoff range: 31.5 vs 15.5dB */
			limit_offset = (D11REV_GE(wlc->pub->corerev, 64)) ? 0x1F : 0xF;
			offset = (offset >= limit_offset)? limit_offset : offset;

			wlc->stf->bcnprs_txpwr_offset = offset;

			if (D11REV_IS(wlc->pub->corerev, 30)) {
				/* limitation. Mimophy rev7, Lcnxnphy rev0 uses 4.1 format */
				wlc_write_shm(wlc, M_BCN_POWER_ADJUST(wlc),
					((uint8)((offset << 1) & 0xf)));
				wlc_write_shm(wlc, M_PRS_POWER_ADJUST(wlc),
					((uint8)((offset << 1) & 0xf)));
			} else if (D11REV_GE(wlc->pub->corerev, 40)) {
				wlc_suspend_mac_and_wait(wlc);
				wlc_beacon_phytxctl(wlc, wlc->bcn_rspec, wlc->chanspec);
				wlc_enable_mac(wlc);
			} else {
				err = BCME_UNSUPPORTED;
			}
		}
		}
		else
			err = BCME_NOTUP;
		break;
		}

	case IOV_GVAL(IOV_TXBCN_TIMEOUT):
		*ret_int_ptr = appvt->txbcn_timeout;
		break;

	case IOV_SVAL(IOV_TXBCN_TIMEOUT):
		appvt->txbcn_timeout = int_val;
		if (appvt->txbcn_timeout > appvt->txbcn_edcrs_timeout)
			appvt->txbcn_edcrs_timeout = appvt->txbcn_timeout;
		break;

	case IOV_GVAL(IOV_TXBCN_EDCRS_TIMEOUT):
		*ret_int_ptr = appvt->txbcn_edcrs_timeout;
		break;

	case IOV_SVAL(IOV_TXBCN_EDCRS_TIMEOUT):
		appvt->txbcn_edcrs_timeout = int_val;
		if (appvt->txbcn_timeout > appvt->txbcn_edcrs_timeout)
			appvt->txbcn_edcrs_timeout = appvt->txbcn_timeout;
		break;

	case IOV_GVAL(IOV_UCAST_DISASSOC_ON_BSS_DOWN):
		if (BSSCFG_AP(bsscfg))
			*ret_int_ptr = BSS_UCAST_DISASSOC_ON_DOWN(appvt, bsscfg);
		else
			err = BCME_NOTAP;
		break;

	case IOV_SVAL(IOV_UCAST_DISASSOC_ON_BSS_DOWN):
		if (BSSCFG_AP(bsscfg)) {
			if (int_val > 0) {
				AP_BSSCFG_CUBBY(appvt, bsscfg)->flags |=
					BSS_FLAG_UCAST_DISASSOC_ON_DOWN;
			} else {
				AP_BSSCFG_CUBBY(appvt, bsscfg)->flags &=
					~BSS_FLAG_UCAST_DISASSOC_ON_DOWN;
			}
		}
		else
			err = BCME_NOTAP;
		break;

	case IOV_GVAL(IOV_BLOCK_NONMBSSID):
		if (BSSCFG_AP(bsscfg)) {
			*ret_int_ptr = appvt->block_nonmbssid;
		} else	{
			err = BCME_NOTAP;
		}
		break;
	case IOV_SVAL(IOV_EDCRS_HI_EVENT_MODE):
		err = wlc_ap_set_edcrs_hi_event_mode(appvt, int_val);
		break;
	case IOV_GVAL(IOV_EDCRS_HI_EVENT_MODE):
		*ret_int_ptr = appvt->edcrs_hi_event_mode;
		break;
	case IOV_GVAL(IOV_EDCRS_HI_EVENT_STATUS):
		*ret_int_ptr = appvt->edcrs_hi_event_status;
		break;
	case IOV_SVAL(IOV_EDCRS_TXBCN_INACT_THRESH):
		if (int_val >= 0) {
			appvt->edcrs_txbcn_inact_thresh = (uint8)int_val;
		} else {
			err = BCME_RANGE;
		}
		break;
	case IOV_GVAL(IOV_EDCRS_TXBCN_INACT_THRESH):
		*ret_int_ptr = appvt->edcrs_txbcn_inact_thresh;
		break;
	case IOV_GVAL(IOV_EDCRS_HI_SIMULATE):
		/* maps to event simulation mode */
		err = wlc_ap_set_edcrs_hi_event_mode(appvt, WL_EDCRS_HI_EVENT_SIMULATE);
		break;
	case IOV_SVAL(IOV_EDCRS_HI_SIMULATE):
		appvt->edcrs_hi_sim_secs = (uint8) int_val;
		break;
	case IOV_SVAL(IOV_EDCRS_DUR_THRESH_US):
		if (int_val >= 0 && int_val <= US_PER_SECOND) {
			appvt->edcrs_dur_thresh_us = (uint32)int_val;
		} else {
			err = BCME_RANGE;
		}
		break;
	case IOV_GVAL(IOV_EDCRS_DUR_THRESH_US):
		*ret_int_ptr = appvt->edcrs_dur_thresh_us;
		break;
	case IOV_GVAL(IOV_REG_INFO):
		if (!arg || len < sizeof(wl_reg_info_t)) {
			err = BCME_BUFTOOSHORT;
		} else {
			wl_reg_info_t *reg_info = (wl_reg_info_t *) arg;
			memcpy(reg_info, &appvt->reg_info, sizeof(*reg_info));
		}
		break;
	case IOV_SVAL(IOV_BLOCK_NONMBSSID):
		if (CHSPEC_IS6G(wlc->chanspec)) {
			if (int_val) {
				appvt->block_nonmbssid = int_val;
			} else	{
				err = BCME_BADARG;
			}

		} else {
			appvt->block_nonmbssid = int_val;
		}
		break;
#ifdef WL_IAPP
	case IOV_GVAL(IOV_IAPP):
		*ret_int_ptr = wlc->pub->_iapp;
		break;
	case IOV_SVAL(IOV_IAPP):
		wlc->pub->_iapp = int_val;
		break;
#endif /* WL_IAPP */

	case IOV_GVAL(IOV_OVERRIDE_CLM_TPE):
		wlc_get_override_clm_tpe(wlc, (wlc_ioctl_tx_pwr_t *)arg, int_val);
		break;
	case IOV_SVAL(IOV_OVERRIDE_CLM_TPE):
		wlc_set_override_clm_tpe(wlc, (wlc_ioctl_tx_pwr_t *)arg);
		break;
#ifdef WL_MBSSID
	case IOV_GVAL(IOV_TRANSMIT_BSS):
		if (!MBSSID_ENAB(wlc->pub, wlc->band->bandtype)) {
			err = BCME_ERROR;
			break;
		}
		if (wlc->main_ap_bsscfg == bsscfg) {
			*ret_int_ptr = 1;
		} else {
			*ret_int_ptr = 0;
		}
		break;
#endif /* WL_MBSSID */
	case IOV_GVAL(IOV_ASSOC_REJECT_RSSI_TH):
	{
		int8 rssi = 0;
		if ((err = wlc_ap_get_assoc_rej_rssi_th(wlc, bsscfg, &rssi)) != BCME_OK) {
			break;
		}
		*ret_int_ptr = rssi;
		break;
	}
	case IOV_SVAL(IOV_ASSOC_REJECT_RSSI_TH):
	{
		err = wlc_ap_set_assoc_rej_rssi_th(wlc, bsscfg, (int8)int_val);
		break;
	}

#ifdef WL_MBSSID
	case IOV_GVAL(IOV_BEACON_LEN):
	{
		int bcn_len = 0;
		if ((err = wlc_calc_beacon_len(wlc, &bcn_len)) == BCME_OK) {
			*ret_int_ptr = bcn_len;
		}
		break;
	}
#endif /* WL_MBSSID */

	default:
		err = BCME_UNSUPPORTED;
		break;
	}

exit:
	return err;
} /* wlc_ap_doiovar */

/* initialize BSS when radio/phy set the target channel */
static void
wlc_ap_upd_onchan(wlc_ap_info_t* ap, wlc_bsscfg_t *cfg)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	wlc_info_t *wlc = appvt->wlc;
	int32 ret = BCME_OK;

#ifdef WDS
	wlc_if_t *wlcif;
	struct scb_iter scbiter;
	struct scb *scb;
#endif
	WL_INFORM(("wl%d: BSS %d is up\n", wlc->pub->unit, cfg->_idx));

	wlc_suspend_mac_and_wait(wlc);
	wlc_BSSinit(wlc, cfg->target_bss, cfg, WLC_BSS_START);
	wlc_ap_up_upd(wlc->ap, cfg, TRUE);

	/* For corerev 30 (eg. 43217), on switching from 20 to 40MHz, update control ch position */
	if (D11REV_IS(wlc->pub->corerev, 30)) {
		uint16 bcn_offset;
		if (HWBCN_ENAB(cfg)) {
			wlc_beacon_phytxctl(wlc, wlc->bcn_rspec, wlc->chanspec);
		}
		bcn_offset = wlc_compute_bcntsfoff(wlc, wlc->bcn_rspec, FALSE, TRUE);
		ASSERT(bcn_offset != 0);
		wlc->band->bcntsfoff = bcn_offset;
		wlc_write_shm(wlc, M_BCN_TXTSF_OFFSET(wlc), bcn_offset);
	}
	wlc_enable_mac(wlc);

	/* update current_bss SSID */
	cfg->current_bss->SSID_len = cfg->SSID_len;
	bcopy(cfg->SSID, cfg->current_bss->SSID, cfg->SSID_len);

#ifdef WLMCNX
	if (MCNX_ENAB(wlc->pub)) {
		wlc_mcnx_bss_upd(wlc->mcnx, cfg, TRUE);
	}
#endif
	if (BSS_WME_ENAB(wlc, cfg)) {
		wlc_edcf_acp_apply(wlc, cfg, TRUE);
	}
	wlc_led_event(wlc->ledh);

	/* Interface up notification */
	WL_APSTA_UPDN(("Reporting link up on config %d (AP enabled)\n",
		WLC_BSSCFG_IDX(cfg)));
#ifdef WL_AP_CHAN_CHANGE_EVENT
	wlc_channel_send_chan_event(wlc, cfg, WL_CHAN_REASON_ANY, wlc->chanspec);
#endif /* WL_AP_CHAN_CHANGE_EVENT */

	wlc_link(wlc, TRUE, &cfg->cur_etheraddr, cfg, 0);
#ifdef WDS
	FOREACH_BSS_SCB(wlc->scbstate, &scbiter, cfg, scb) {
		if ((wlcif = SCB_WDS(scb)) != NULL) {
			/* send WLC_E_LINK event with status UP to WDS interface */
			wlc_wds_create_link_event(wlc, wlcif->u.scb, TRUE);
		}
	}
#endif
#ifdef WL11K_AP
	wlc_rrm_add_pilot_timer(wlc, cfg);
#endif
	if ((ret = wlc_bsscfg_bcmc_scb_init(wlc, cfg)) != BCME_OK) {
		ASSERT(!(ret == BCME_NOMEM));
	}
} /* wlc_ap_upd_onchan */

/** automatic channel selection */
static void
wlc_ap_acs_update(wlc_info_t *wlc)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)wlc->ap;

	WL_INFORM(("wl%d: %s: changing chanspec to %d\n",
		wlc->pub->unit, __FUNCTION__, appvt->chanspec_selected));
	wlc_set_home_chanspec(wlc, appvt->chanspec_selected);
	wlc_suspend_mac_and_wait(wlc);

	wlc_set_chanspec(wlc, appvt->chanspec_selected, CHANSW_REASON(CHANSW_IOVAR));
	if (AP_ENAB(wlc->pub)) {
		wlc->bcn_rspec = wlc_lowest_basic_rspec(wlc,
			&wlc->primary_bsscfg->current_bss->rateset);
		if (CHSPEC_IS6G(wlc->chanspec) &&
			((wlc->lpi_mode == AUTO && wlc->stf->psd_limit_indicator) ||
			(wlc->lpi_mode == ON))) {
			wlc->bcn_rspec &= ~WL_RSPEC_BW_MASK;
			wlc->bcn_rspec |= CHSPECBW_TO_RSPECBW(CHSPEC_BW(wlc->chanspec));
		}
		ASSERT(wlc_valid_rate(wlc, wlc->bcn_rspec,
			CHSPEC_BANDTYPE(wlc->primary_bsscfg->current_bss->chanspec), TRUE));
		wlc_beacon_phytxctl(wlc, wlc->bcn_rspec, wlc->chanspec);
		wlc_beacon_upddur(wlc, wlc->bcn_rspec, wlc->bcn_len);
	}

	if (wlc->pub->associated) {
		wlc_update_beacon(wlc);
		wlc_update_probe_resp(wlc, FALSE);
	}
	wlc_enable_mac(wlc);
}

/*
 * Set the operational capabilities for STAs required to associate to the BSS.
 */
static int
wlc_ap_set_opmode_cap_reqd(wlc_info_t* wlc, wlc_bsscfg_t *bsscfg,
	opmode_cap_t opmode)
{
	int err = 0;

	wlc_ap_info_pvt_t *appvt;
	ap_bsscfg_cubby_t *ap_cfg;

	ASSERT(wlc != NULL);
	ASSERT(bsscfg != NULL);

	appvt = (wlc_ap_info_pvt_t *) wlc->ap;
	ASSERT(appvt != NULL);
	ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);
	ASSERT(ap_cfg != NULL);

	if (opmode >= OMC_MAX) {
		err = BCME_RANGE;
		goto exit;
	}

	/* can only change setting if the BSS is down */
	if (bsscfg->up) {
		err = BCME_ASSOCIATED;
		goto exit;
	}

	/* apply setting */
	ap_cfg->opmode_cap_reqd = opmode;

exit:
	return err;
}

/*
 * Get the operational capabilities for STAs required to associate to the BSS.
 */
static opmode_cap_t
wlc_ap_get_opmode_cap_reqd(wlc_info_t* wlc, wlc_bsscfg_t *bsscfg)
{
	wlc_ap_info_pvt_t *appvt;
	ap_bsscfg_cubby_t *ap_cfg;

	ASSERT(wlc != NULL);
	ASSERT(bsscfg != NULL);

	appvt = (wlc_ap_info_pvt_t *) wlc->ap;
	ASSERT(appvt != NULL);
	ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);
	ASSERT(ap_cfg != NULL);

	/* return the setting */
	return ap_cfg->opmode_cap_reqd;
}

static int wlc_ap_bsscfg_init(void *context, wlc_bsscfg_t *cfg)
{
	wlc_ap_info_pvt_t *appvt;
	ap_bsscfg_cubby_t *ap_cfg;

	ASSERT(context != NULL);
	ASSERT(cfg != NULL);

	appvt = (wlc_ap_info_pvt_t *)context;
	ap_cfg = AP_BSSCFG_CUBBY(appvt, cfg);
	ASSERT(ap_cfg != NULL);

	/* The operational mode capabilities init */
	ap_cfg->opmode_cap_reqd = OMC_NONE;

#if BAND6G || defined(WL_OCE_AP)
	if (!BSSCFG_AP(cfg)) {
		return 0;
	}
	cfg->rnr_nbr_ap_info = MALLOCZ(appvt->wlc->osh, BCM_TLV_MAX_DATA_SIZE);
	if (cfg->rnr_nbr_ap_info == NULL) {
		WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
			appvt->wlc->pub->unit, __FUNCTION__, MALLOCED(appvt->wlc->osh)));
		return BCME_NOMEM;
	}
#endif /* WL_WIFI_6GHZ || WL_OCE_AP */
	return 0;
}

static void
wlc_ap_bsscfg_deinit(void *context, wlc_bsscfg_t *cfg)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)context;
	ap_bsscfg_cubby_t *ap_cfg;

	ASSERT(appvt != NULL);
	ASSERT(cfg != NULL);

	ap_cfg = AP_BSSCFG_CUBBY(appvt, cfg);
	if (ap_cfg == NULL)
		return;

	if (ap_cfg->msch_req_hdl) {
		wlc_msch_timeslot_unregister(appvt->wlc->msch_info, &ap_cfg->msch_req_hdl);
		ap_cfg->msch_req_hdl = NULL;
	}
#if BAND6G || defined(WL_OCE_AP)
	if (cfg->rnr_nbr_ap_info) {
		MFREE(appvt->wlc->osh, cfg->rnr_nbr_ap_info, BCM_TLV_MAX_DATA_SIZE);
		cfg->rnr_nbr_ap_info = NULL;
	}
#endif /* WL_WIFI_6GHZ || WL_OCE_AP */

#ifdef WLDEAUTH_INTRANSIT_FILTER
	/* clear intransit_filter table */
	wlc_ap_intransit_filter_clear_bss(appvt, cfg);
#endif /* WLDEAUTH_INTRANSIT_FILTER */
}

#if defined(BCMDBG)
static void
wlc_ap_bsscfg_dump(void *ctx, wlc_bsscfg_t *cfg, struct bcmstrbuf *b)
{
	wlc_ap_info_pvt_t *appvt = ctx;
	ap_bsscfg_cubby_t *ap_cfg;

	ap_cfg = AP_BSSCFG_CUBBY(appvt, cfg);
	if (ap_cfg == NULL) {
		return;
	}

	bcm_bprintf(b, "     msch state %u\n", ap_cfg->msch_state);
	if (BSSCFG_AP(cfg)) {
		bcm_bprhex(b, "     aidmap: ", TRUE, appvt->aidmap, AP_AIDMAPSZ);
	}
}
#endif

/* Return the max. AID plus 1 the AP could assign.
 * The usage of the return value is for PVB update/validation.
 * See wlc_apps.c for usage.
 */
uint16
wlc_ap_aid_max(wlc_ap_info_t *ap)
{
	return AP_MAX_AID;
}

/* Allocate a new AID, return 0 if no AID available
 */
static uint16
wlc_ap_aid_new(wlc_ap_info_pvt_t *appvt, wlc_bsscfg_t *cfg)
{
	uint16 aid, block;

	ASSERT(cfg);
	BCM_REFERENCE(cfg);

	/* Get an unused number from aidmap, start at AP_MAX_BSSCFG_AID, as the lower AIDs are
	 * reserved for Multiple BSSID BCMC.
	 */
	for (aid = appvt->max_bss_count; aid < AP_MAX_AID; aid++) {
		/* AID still in use? */
		if (isset(appvt->aidmap, aid)) {
			continue;
		}
		/* Is AID blocked due to recent usage? */
		for (block = 0; block < AP_MAX_AID_REUSE_BLOCK; block++) {
			if (appvt->aidblock[block] == aid) {
				break;
			}
		}
		if (block < AP_MAX_AID_REUSE_BLOCK) {
			continue;
		}
		WL_ASSOC(("%s marking bit = %d for bsscfg %d in AIDMAP\n", __FUNCTION__, aid,
			WLC_BSSCFG_IDX(cfg)));
		/* mark the position being used */
		setbit(appvt->aidmap, aid);

		return aid;
	}

	/* reached full capacity */
	WL_ERROR(("%s Failed to allocate AID\n", __FUNCTION__));
	return 0;
}

/* Free an AID.
 */
static void
wlc_ap_aid_free(wlc_ap_info_pvt_t *appvt, scb_t *scb)
{
	uint16 aid;

	aid = scb->aid;

	WL_ASSOC(("%s releasing bit = %d for bsscfg %d in AIDMAP\n", __FUNCTION__, aid,
		WLC_BSSCFG_IDX(SCB_BSSCFG(scb))));

	ASSERT(aid < AP_MAX_AID);
	ASSERT(isset(appvt->aidmap, aid));

	clrbit(appvt->aidmap, aid);

	appvt->aidblock[appvt->aidblock_idx] = aid;
	appvt->aidblock_idx++;
	if (appvt->aidblock_idx == AP_MAX_AID_REUSE_BLOCK) {
		appvt->aidblock_idx = 0;
	}
}

static int
wlc_ap_scb_init(void *context, struct scb *scb)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t*)context;
	wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);

	if (BSSCFG_AP(cfg) && !SCB_INTERNAL(scb)) {
		ap_scb_cubby_t **pap_scb = AP_SCB_CUBBY_LOC(appvt, scb);
		ap_scb_cubby_t *ap_scb = AP_SCB_CUBBY(appvt, scb);
		wlc_info_t *wlc = appvt->wlc;

		ASSERT(ap_scb == NULL);

		if ((ap_scb = wlc_scb_sec_cubby_alloc(wlc, scb, sizeof(*ap_scb))) == NULL) {
			WL_ERROR(("wl%d: %s: mem alloc failed, allocated %d bytes\n",
			          wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
			return BCME_NOMEM;
		}
		*pap_scb = ap_scb;

		scb->aid = wlc_ap_aid_new(appvt, cfg);
		if (scb->aid == 0) {
			return BCME_NORESOURCE;
		}
#ifdef BCM_CSIMON
		{
			int retval;
			/* CSIMON SCB Initialization */
			if ((retval = wlc_csimon_scb_init(wlc, scb)) != BCME_OK) {
				return retval;
			}
		}
#endif /* BCM_CSIMON */
	}
	return BCME_OK;
}

static int
wlc_as_scb_init(void *context, struct scb *scb)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t*)context;
	wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);
	wlc_info_t *wlc = appvt->wlc;

	BCM_REFERENCE(wlc);
	BCM_REFERENCE(cfg);

	if (BSSCFG_AP(cfg) && !SCB_INTERNAL(scb)) {
		wlc_assoc_req_t **pas_scb = AS_SCB_CUBBY_LOC(appvt, scb);
		wlc_assoc_req_t *as_scb = AS_SCB_CUBBY(appvt, scb);

		ASSERT(as_scb == NULL);

		if ((as_scb = wlc_scb_sec_cubby_alloc(wlc, scb, sizeof(*as_scb))) == NULL) {
			WL_ERROR(("wl%d: %s: mem alloc failed, allocated %d bytes\n",
				wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
			return BCME_NOMEM;
		}
		*pas_scb = as_scb;
	}
	return BCME_OK;
}

static void
wlc_as_scb_deinit(void *context, struct scb *scb)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t*)context;
	wlc_info_t *wlc = appvt->wlc;
	wlc_assoc_req_t **pas_scb = AS_SCB_CUBBY_LOC(appvt, scb);
	wlc_assoc_req_t *param = AS_SCB_CUBBY(appvt, scb);

	/* free param body if not freed during assoc stage */
	if (param != NULL) {
		if (param->body != NULL) {
			MFREE(wlc->osh, param->body, param->buf_len);
		}
		wlc_scb_sec_cubby_free(wlc, scb, param);
	}
	*pas_scb = NULL;
}

static uint
wlc_as_scb_secsz(void *context, struct scb *scb)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t*)context;
	wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);
	wlc_info_t *wlc = appvt->wlc;

	BCM_REFERENCE(wlc);
	BCM_REFERENCE(cfg);

	if (BSSCFG_AP(cfg) && !SCB_INTERNAL(scb)) {
		return sizeof(wlc_assoc_req_t);
	}

	return 0;
}

static void
wlc_ap_scb_deinit(void *context, struct scb *scb)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t*)context;
	wlc_info_t *wlc = appvt->wlc;
	wlc_bsscfg_t *bsscfg = SCB_BSSCFG(scb);
	ap_scb_cubby_t *ap_scb = AP_SCB_CUBBY(appvt, scb);
	scb_t* other_scb;
	struct scb_iter scbiter;
#ifdef BCM_CSIMON
#ifdef CSIMON_PER_STA_TIMER
	if ((BSSCFG_AP(bsscfg)) && (wlc_csimon_enabled(wlc, scb)) &&
		!wlc_csimon_timer_isnull(scb)) {
		/* Stop and free the CSIMON timer */
		wlc_csimon_timer_del(wlc, scb);
		CSIMON_DEBUG("wl%d: stopped/freed CSI timer for SCB DA "MACF"\n",
		              wlc->pub->unit, ETHER_TO_MACF(scb->ea));
	}
#endif /* CSIMON_PER_STA_TIMER */
#endif /* BCM_CSIMON */
	if (BSSCFG_AP(bsscfg)) {
		if (SCB_ASSOCIATED(scb)) {
			WL_ASSOC(("wl%d: AP: scb free: indicate disassoc for the STA-" MACF "\n",
				wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
			wlc_bss_mac_event(wlc, bsscfg, WLC_E_DISASSOC_IND, &scb->ea,
				WLC_E_STATUS_SUCCESS, DOT11_RC_DISASSOC_LEAVING, 0, 0, 0);
		} else if (SCB_AUTHENTICATED(scb)) {
			WL_ASSOC(("wl%d: AP: scb free: indicate deauth for the STA-" MACF "\n",
				wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
			wlc_bss_mac_event(wlc, bsscfg, WLC_E_DEAUTH, &scb->ea,
				WLC_E_STATUS_SUCCESS, DOT11_RC_DEAUTH_LEAVING, 0, 0, 0);
		}
	}

	if (ap_scb != NULL) {
		ap_scb_cubby_t **pap_scb = AP_SCB_CUBBY_LOC(appvt, scb);

		/* mark the aid unused */
		if (scb->aid) {
			wlc_ap_aid_free(appvt, scb);
		}

		/* free any leftover authentication state */
		if (ap_scb->challenge) {
			MFREE(wlc->osh, ap_scb->challenge, 2 + ap_scb->challenge[1]);
			ap_scb->challenge = NULL;
		}

		/* free wpaie if stored */
		if (ap_scb->wpaie) {
			MFREE(wlc->osh, ap_scb->wpaie, ap_scb->wpaie_len);
			ap_scb->wpaie_len = 0;
			ap_scb->wpaie = NULL;
		}

		wlc_scb_sec_cubby_free(wlc, scb, ap_scb);
		*pap_scb = NULL;
	}
	/* clean up psta_prim references */
	FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, other_scb) {
		ap_scb_cubby_t *other_ap_scb = AP_SCB_CUBBY(appvt, other_scb);
		if ((other_ap_scb != NULL) && (other_ap_scb->psta_prim == scb)) {
			other_ap_scb->psta_prim = NULL;
		}
	}
}

static uint
wlc_ap_scb_secsz(void *context, struct scb *scb)
{
	wlc_bsscfg_t *cfg = SCB_BSSCFG(scb);

	BCM_REFERENCE(context);
	BCM_REFERENCE(cfg);

	if (BSSCFG_AP(cfg) &&
	    !SCB_INTERNAL(scb)) {
		return sizeof(ap_scb_cubby_t);
	}

	return 0;
}

void
wlc_ap_bsscfg_scb_cleanup(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
	struct scb_iter scbiter;
	struct scb *scb;

	FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
		if (SCB_ASSOCIATED(scb)) {
			if (!scb->permanent) {
				wlc_scbfree(wlc, scb);
			}
		}
	}
}

#if defined(RXCHAIN_PWRSAVE) || defined(RADIO_PWRSAVE)
/*
 * Returns true if check for associated STAs is enabled
 * and there are STAs associated
 */
static bool
wlc_pwrsave_stas_associated_check(wlc_ap_info_t *ap, int type)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	bool check_assoc_stas = FALSE;

	switch (type) {
#ifdef RXCHAIN_PWRSAVE
		case PWRSAVE_RXCHAIN:
			check_assoc_stas = appvt->rxchain_pwrsave.pwrsave.stas_assoc_check;
			break;
#endif
#ifdef RADIO_PWRSAVE
		case PWRSAVE_RADIO:
			check_assoc_stas = appvt->radio_pwrsave.pwrsave.stas_assoc_check;
			break;
#endif
		default:
			break;
	}
	return (check_assoc_stas && (wlc_ap_stas_associated(ap) > 0));
}

/*
 * At every watchdog tick we update the power save
 * data structures and see if we can go into a power
 * save mode
 */
static void
wlc_pwrsave_mode_check(wlc_ap_info_t *ap, int type)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	wlc_info_t *wlc = appvt->wlc;
	wlc_pwrsave_t *pwrsave = NULL;
	uint pkts_per_second, total_pktcount;

	switch (type) {
#ifdef RXCHAIN_PWRSAVE
		case PWRSAVE_RXCHAIN:
			pwrsave = &appvt->rxchain_pwrsave.pwrsave;
			/* Exit power save mode if channel is quiet or
			 * if BGDFS is in progress
			 */
			if (wlc_quiet_chanspec(wlc->cmi, phy_utils_get_chanspec(WLC_PI(wlc))) ||
#if defined(WLDFS) && defined(BGDFS)
					wlc_dfs_scan_in_progress(wlc->dfs) ||
#endif /* WLDFS && BGDFS */
					FALSE) {
				wlc_reset_rxchain_pwrsave_mode(ap);
				return;
			}
			break;
#endif /* RXCHAIN_PWRSAVE */
#ifdef RADIO_PWRSAVE
		case PWRSAVE_RADIO:
			pwrsave = &appvt->radio_pwrsave.pwrsave;
			break;
#endif /* RADIO_PWRSAVE */
		default:
			WL_ERROR(("Invalid poer save type:%d\n", type));
			return;
	}

#ifdef RXCHAIN_PWRSAVE
	/* Enter rxchain_pwrsave mode when there is no STA associated to AP
	 * and BSS is already up
	 */
	if ((appvt->rxchain_pwrsave.pwrsave.power_save_check & NONASSOC_PWRSAVE_ENB) &&
#ifdef WDS
		!wlc_rxchain_wds_detection(wlc) &&
#endif /* WDS */
		(type == PWRSAVE_RXCHAIN) && (wlc_ap_stas_associated(ap) == 0) &&
		!pwrsave->in_power_save && wlc->primary_bsscfg->up) {
		appvt->rxchain_pwrsave.rxchain = wlc->stf->rxchain;
		/* need to save and disable rx_stbc HT capability
		 * before enter rxchain_pwrsave mode
		 */
		appvt->rxchain_pwrsave.ht_cap_rx_stbc = wlc_stf_enter_rxchain_pwrsave(wlc);
		/* set in_power_save before actual rxchain switching to ensure
		 * validity of wlc_rxchain_pwrsave_get_rxchain() output
		 */
		pwrsave->in_power_save = TRUE;
		wlc_stf_rxchain_set(wlc, 0x1, FALSE);
		pwrsave->in_power_save_counter++;
		return;
	}
#endif /* RXCHAIN_PWRSAVE */

	/* Total pkt count - forwarded packets + packets the os has given + sendup packets */
	total_pktcount =  WLPWRSAVERXFVAL(wlc) + WLPWRSAVETXFVAL(wlc);

	/* Calculate the packets per second */
	pkts_per_second = total_pktcount - pwrsave->prev_pktcount;

	/* Save the current packet count for next second */
	pwrsave->prev_pktcount = total_pktcount;

	if (pkts_per_second < pwrsave->pps_threshold) {
		/* When the packets are below the threshold we just
		 * increment our timeout counter
		 */
		if (!pwrsave->in_power_save) {
			if ((pwrsave->quiet_time_counter >= pwrsave->quiet_time) &&
			    (! wlc_pwrsave_stas_associated_check(ap, type))) {
				WL_INFORM(("Entering power save mode pps is %d\n",
					pkts_per_second));
#ifdef RXCHAIN_PWRSAVE
				if (type == PWRSAVE_RXCHAIN) {
					/* Save current configured rxchains */
					appvt->rxchain_pwrsave.rxchain = wlc->stf->rxchain;
					/* need to save and disable rx_stbc HT capability
					 * before enter rxchain_pwrsave mode
					 */
					appvt->rxchain_pwrsave.ht_cap_rx_stbc =
						wlc_stf_enter_rxchain_pwrsave(wlc);
					/* set in_power_save before rxchain switching to ensure
					 * validity of wlc_rxchain_pwrsave_get_rxchain()
					 */
					pwrsave->in_power_save = TRUE;
					wlc_stf_rxchain_set(wlc, 0x1, TRUE);
				} else
#endif /* RXCHAIN_PWRSAVE */
				{
#ifdef RADIO_PWRSAVE
					pwrsave->in_power_save = TRUE;
#endif /* RADIO_PWRSAVE */
				}
				pwrsave->in_power_save_counter++;

				if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
					if (wlc_set_rpsnoa(wlc)) {
						pwrsave->in_power_save = FALSE;
						WL_ERROR(("wlc_pwrsave_mode_check, "
							"rps_set_noa error\n"));
					} else {
						/* Clear WAKE bit
						 * if in_power_save flag is set,
						 * wake is cleared in AP mode
						 */
						wlc_set_wake_ctrl(wlc);
					}
				}

				return;
			}
		}
		pwrsave->quiet_time_counter++;
	} else {
		/* If we are already in the wait mode counting
		 * up then just reset the counter since
		 * packets have gone above threshold
		 */
		pwrsave->quiet_time_counter = 0;
		WL_INFORM(("Resetting quiet time\n"));
		if (pwrsave->in_power_save) {
			if (type == PWRSAVE_RXCHAIN) {
#ifdef RXCHAIN_PWRSAVE
				wlc_stf_rxchain_set(wlc, appvt->rxchain_pwrsave.rxchain, TRUE);
				/* need to restore rx_stbc HT capability
				 * after exit rxchain_pwrsave mode
				 */
				wlc_stf_exit_rxchain_pwrsave(wlc,
					appvt->rxchain_pwrsave.ht_cap_rx_stbc);
#endif /* RXCHAIN_PWRSAVE */
			}
#ifdef RADIO_PWRSAVE
			else if (type == PWRSAVE_RADIO) {
				if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
					wlc_reset_rpsnoa(wlc);
					wlc_reset_pwrsave_mode(ap, PWRSAVE_RADIO);
					/* Set WAKE bit
					 * if in_power_save flag is unset, wake is set in AP mode
					 */
					wlc_set_wake_ctrl(wlc);
				} else {
					wl_del_timer(wlc->wl, appvt->radio_pwrsave.timer);
					if (appvt->radio_pwrsave.radio_disabled) {
						wlc_bmac_radio_hw(wlc->hw, TRUE, FALSE);
						appvt->radio_pwrsave.radio_disabled = FALSE;
					}
					appvt->radio_pwrsave.pwrsave_state = 0;
					appvt->radio_pwrsave.cncl_bcn = FALSE;
				}
			}
#endif /* RADIO_PWRSAVE */
			WL_INFORM(("Exiting power save mode pps is %d\n", pkts_per_second));
			pwrsave->in_power_save = FALSE;
		}
	}
} /* wlc_pwrsave_mode_check */
#endif /* RXCHAIN_PWRSAVE || RADIO_PWRSAVE */

#ifdef RADIO_PWRSAVE

/*
 * Routine that enables/disables the radio for the duty cycle
 */
static void
wlc_radio_pwrsave_timer(void *arg)
{
	wlc_ap_info_t *ap = (wlc_ap_info_t*)arg;
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	wlc_info_t *wlc = appvt->wlc;

	if (appvt->radio_pwrsave.radio_disabled) {
		WL_INFORM(("wl power timer OFF period end. enabling radio\n"));

		if (appvt->radio_pwrsave.level) {
			appvt->radio_pwrsave.cncl_bcn = TRUE;
		}

		wlc_bmac_radio_hw(wlc->hw, TRUE, FALSE);
		appvt->radio_pwrsave.radio_disabled = FALSE;
		/* Re-enter power save state */
		appvt->radio_pwrsave.pwrsave_state = 2;
	} else {
		WL_INFORM(("wl power timer OFF period starts. disabling radio\n"));
		wlc_radio_pwrsave_off_time_start(ap);
		wlc_bmac_radio_hw(wlc->hw, FALSE, FALSE);
		appvt->radio_pwrsave.radio_disabled = TRUE;
	}
}

/*
 * Start the on time of the beacon interval
 */
void
wlc_radio_pwrsave_on_time_start(wlc_ap_info_t *ap, bool dtim)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	wlc_info_t *wlc = appvt->wlc;
	uint32 on_time, state;

	/* Update quite ie only the first time we enter power save mode.
	 * If we are entering power save for the first time set the count
	 * such that quiet time starts after 3 bcn intervals.
	 *
	 * state 0 - initial state / just exited pwr save
	 * state 1 - entering pwr save - sent quiet ie with count n
	 * state 2 - re-entering pwr save - sent quiet ie with count 1
	 * state 3 - in pwr save - radio on/off
	 *
	 * ---> 0 ----> 1 ------> 3
	 *      ^       |    ^    |
	 *      |       |    |    |
	 *      |       v    |    v
	 *      +<---------- 2 <--+
	 *      |                 |
	 *      |                 |
	 *      |                 |
	 *      +<----------------+
	 */
	state = appvt->radio_pwrsave.pwrsave_state;
	if (state == 0) {
		wlc_radio_pwrsave_update_quiet_ie(ap, 3);
		/* Enter power save state */
		appvt->radio_pwrsave.tbtt_skip = 3;
		appvt->radio_pwrsave.pwrsave_state = 1;
	}

	/* We are going to start radio on/off only after counting
	 * down tbtt_skip bcn intervals.
	 */
	if (appvt->radio_pwrsave.tbtt_skip-- > 0) {
		WL_INFORM(("wl%d: tbtt skip %d\n", wlc->pub->unit,
		           appvt->radio_pwrsave.tbtt_skip));
		return;
	}

	if (!dtim)
		return;

	if (appvt->radio_pwrsave.level) {
		appvt->radio_pwrsave.cncl_bcn = FALSE;
	}

	/* Schedule the timer to turn off the radio after on_time msec */
	on_time = appvt->radio_pwrsave.on_time * DOT11_TU_TO_US;
	on_time += appvt->pre_tbtt_us;

	appvt->radio_pwrsave.tbtt_skip = ((on_time / DOT11_TU_TO_US) /
	                                          wlc->primary_bsscfg->current_bss->beacon_period);
	on_time /= 1000;
	/* acc for extra phy and mac suspend delays, it seems to be huge. Need
	 * to extract out the exact delays.
	 */
	on_time += RADIO_PWRSAVE_TIMER_LATENCY;

	WL_INFORM(("wl%d: adding timer to disable phy after %d ms state %d, skip %d\n",
	           wlc->pub->unit, on_time, appvt->radio_pwrsave.pwrsave_state,
	           appvt->radio_pwrsave.tbtt_skip));

	/* In case pre-tbtt intr arrived before the timer that disables the radio */
	wl_del_timer(wlc->wl, appvt->radio_pwrsave.timer);

	wl_add_timer(wlc->wl, appvt->radio_pwrsave.timer, on_time, FALSE);

	/* Update bcn and probe resp to send quiet ie starting from next
	 * tbtt intr.
	 */
	wlc_radio_pwrsave_update_quiet_ie(ap, appvt->radio_pwrsave.tbtt_skip);
} /* wlc_radio_pwrsave_on_time_start */

/*
 * Start the off time of the beacon interval
 */
static void
wlc_radio_pwrsave_off_time_start(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	wlc_info_t *wlc = appvt->wlc;
	uint32 off_time;

	/* Calcuate the delay after which to schedule timer to turn on
	 * the radio. Also take in to account the phy enabling latency.
	 * We have to make sure the phy is enabled by the next pre-tbtt
	 * interrupt time.
	 */
	off_time = appvt->radio_pwrsave.off_time;

	off_time *= DOT11_TU_TO_US;
	off_time -= (appvt->pre_tbtt_us + PHY_DISABLE_MAX_LATENCY_us);

	/* In power save state */
	appvt->radio_pwrsave.pwrsave_state = 3;
	off_time /= 1000;

	/* acc for extra phy and mac suspend delays, it seems to be huge. Need
	 * to extract out the exact delays.
	 */
	off_time -= RADIO_PWRSAVE_TIMER_LATENCY;

	WL_INFORM(("wl%d: add timer to enable phy after %d msec state %d\n",
	           wlc->pub->unit, off_time, appvt->radio_pwrsave.pwrsave_state));

	/* Schedule the timer to turn on the radio after off_time msec */
	wl_add_timer(wlc->wl, appvt->radio_pwrsave.timer, off_time, FALSE);
}

/*
 * Check whether we are in radio power save mode
 */
int
wlc_radio_pwrsave_in_power_save(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;

	return (appvt->radio_pwrsave.pwrsave.in_power_save);
}

/*
 * Enter radio power save
 */
void
wlc_radio_pwrsave_enter_mode(wlc_info_t *wlc, bool dtim)
{
	/* If AP is in radio power save mode, we need to start the duty
	 * cycle with TBTT
	 */
	if (AP_ENAB(wlc->pub) && RADIO_PWRSAVE_ENAB(wlc->ap) &&
	    wlc_radio_pwrsave_in_power_save(wlc->ap)) {
		if (!RADIONOA_PWRSAVE_ENAB(wlc->pub))
			wlc_radio_pwrsave_on_time_start(wlc->ap, dtim);
	}
}

/*
 * Exit out of the radio power save if we are in it
 */
void
wlc_radio_pwrsave_exit_mode(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	wlc_info_t *wlc = appvt->wlc;

	if (RADIONOA_PWRSAVE_ENAB(wlc->pub)) {
		wlc_reset_rpsnoa(wlc);
		wlc_reset_pwrsave_mode(ap, PWRSAVE_RADIO);
		/* Set WAKE bit
		 * if in_power_save flag is unset, wake is set in AP mode
		 */
		wlc_set_wake_ctrl(wlc);
	} else {
		appvt->radio_pwrsave.pwrsave.quiet_time_counter = 0;
		appvt->radio_pwrsave.pwrsave.in_power_save = FALSE;
		wl_del_timer(wlc->wl, appvt->radio_pwrsave.timer);
		if (appvt->radio_pwrsave.radio_disabled) {
			wlc_bmac_radio_hw(wlc->hw, TRUE, FALSE);
			appvt->radio_pwrsave.radio_disabled = FALSE;
		}
		appvt->radio_pwrsave.pwrsave_state = 0;
		appvt->radio_pwrsave.cncl_bcn = FALSE;
	}
}

/*
 * Update the beacon with quiet IE
 */
static void
wlc_radio_pwrsave_update_quiet_ie(wlc_ap_info_t *ap, uint8 count)
{
#ifdef WL11H
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	wlc_info_t *wlc = appvt->wlc;
	uint32 duration;
	wlc_bsscfg_t *cfg = wlc->primary_bsscfg;
	dot11_quiet_t quiet_cmd;

	if (WL11H_ENAB(wlc))
		return;

	duration = appvt->radio_pwrsave.off_time;
	duration *= DOT11_TU_TO_US;
	duration -= (appvt->pre_tbtt_us + PHY_ENABLE_MAX_LATENCY_us +
	             PHY_DISABLE_MAX_LATENCY_us);
	duration /= DOT11_TU_TO_US;

	/* Setup the quiet command */
	quiet_cmd.period = 0;
	quiet_cmd.count = count;
	quiet_cmd.duration = (uint16)duration;
	quiet_cmd.offset = appvt->radio_pwrsave.on_time % cfg->current_bss->beacon_period;
	WL_INFORM(("wl%d: quiet cmd: count %d, dur %d, offset %d\n",
	            wlc->pub->unit, quiet_cmd.count,
	            quiet_cmd.duration, quiet_cmd.offset));

	wlc_quiet_do_quiet(wlc->quiet, cfg, &quiet_cmd);
#endif /* WL11H */
}

bool
wlc_radio_pwrsave_bcm_cancelled(const wlc_ap_info_t *ap)
{
	const wlc_ap_info_pvt_t *appvt = (const wlc_ap_info_pvt_t *)ap;

	return appvt->radio_pwrsave.cncl_bcn;
}

#endif /* RADIO_PWRSAVE */

/* ======================= IE mgmt routines ===================== */
/* ============================================================== */
/* Supported Rates IE */
static uint
wlc_assoc_calc_sup_rates_ie_len(void *ctx, wlc_iem_calc_data_t *data)
{
	wlc_iem_ft_cbparm_t *ftcbparm = data->cbparm->ft;

	BCM_REFERENCE(ctx);

	return TLV_HDR_LEN + ftcbparm->assocresp.sup->count;
}

static int
wlc_assoc_write_sup_rates_ie(void *ctx, wlc_iem_build_data_t *data)
{
	wlc_iem_ft_cbparm_t *ftcbparm = data->cbparm->ft;

	BCM_REFERENCE(ctx);

	bcm_write_tlv(DOT11_MNG_RATES_ID, ftcbparm->assocresp.sup->rates,
		ftcbparm->assocresp.sup->count, data->buf);

	return BCME_OK;
}

/* Extended Supported Rates IE */
static uint
wlc_assoc_calc_ext_rates_ie_len(void *ctx, wlc_iem_calc_data_t *data)
{
	wlc_iem_ft_cbparm_t *ftcbparm = data->cbparm->ft;

	BCM_REFERENCE(ctx);

	if (ftcbparm->assocresp.ext->count == 0) {
		return 0;
	}

	return TLV_HDR_LEN + ftcbparm->assocresp.ext->count;
}

static int
wlc_assoc_write_ext_rates_ie(void *ctx, wlc_iem_build_data_t *data)
{
	wlc_iem_ft_cbparm_t *ftcbparm = data->cbparm->ft;

	BCM_REFERENCE(ctx);

	bcm_write_tlv(DOT11_MNG_EXT_RATES_ID, ftcbparm->assocresp.ext->rates,
		ftcbparm->assocresp.ext->count, data->buf);

	return BCME_OK;
}
#if BAND6G || defined(WL_OCE_AP)
/* Update/Gather Static Neighbor information for AP and store in rnr_nbr_ap_info.
 * rnr_update_timer use this information to prepare RNR IE while preparing
 * Beacon and Probe response
 */
void
wlc_ap_update_static_rnr_nbr_info(wlc_info_t* wlc, wlc_bsscfg_t *bsscfg)
{
	ASSERT(bsscfg);

	if (!wlc->rnr_update_period) {
		WL_INFORM(("wl%d: Skip adding RNR IE as rnr_update_period[%d] OR"
			" OCE_ENAB[%d] is set, disable RNR update timer\n",
			wlc->pub->unit,	wlc->rnr_update_period, BSS_OCE_ENAB(wlc, bsscfg)));
		RNR_UPDATE_DEL_TIMER(wlc);
		return;
	}

	bsscfg->rnr_nbr_ap_info_size = 0;

	/* clear the old one */
	bzero(bsscfg->rnr_nbr_ap_info, BCM_TLV_MAX_DATA_SIZE);
#ifdef WL_OCE_AP
	wlc_oce_reset_ssid_list(bsscfg);
#endif /* WL_OCE_AP */
#ifdef WL11K_NBR_MEAS
#ifdef WL11K_AP
#if BAND6G || defined(WL_OCE_AP)
	wlc_rrm_fill_static_rnr_info(wlc, bsscfg, bsscfg->rnr_nbr_ap_info,
		BCM_TLV_MAX_DATA_SIZE, &bsscfg->rnr_nbr_ap_info_size, FALSE);
#endif /* BAND6G || WL_OCE_AP */
#endif /* WL11K_AP */
#endif /* WL11K_NBR_MEAS */
	WL_INFORM(("wl%d: static nbr info size[%d] to prepare RNR\n",
		wlc->pub->unit, bsscfg->rnr_nbr_ap_info_size));
}

/* RNR update Timer expiration calls rnr prepare method based on
 * configuration i.e. static or scan based.
 * For OCE certification: use already developed scan method.
 * Otherwise: Prepare list of neighbors from static list in RRM module.
 */
void
wlc_rnr_update_timer(void *ctx)
{
	wlc_info_t *wlc = (wlc_info_t*) ctx;
	wlc_bsscfg_t *bsscfg;
#if defined(WL_OCE) && defined(WL_OCE_AP)
	wlc_ssid_t ssid = {0};
	int err;
#endif /* WL_OCE && WL_OCE_AP */
	int i;

	if (!wlc->rnr_update_period || !isset(&wlc->nbr_discovery_cap, WLC_NBR_DISC_CAP_RNR_IE)) {
		WL_INFORM(("wl%d: Skip adding RNR IE as rnr_update_period[%d] OR"
			" bit 0 of cap[%x] is not set\n",
			wlc->pub->unit,	wlc->rnr_update_period, wlc->nbr_discovery_cap));

		goto end;
	}
#if defined(WL_OCE) && defined(WL_OCE_AP)
	if (wlc->rnr_update_method == WLC_RNR_UPDATE_METHOD_SCAN) {
		FOREACH_UP_AP(wlc, i, bsscfg) {
			bzero(&ssid, sizeof(ssid));

			WL_INFORM(("wl%d: use scan method to prepare reduced neighbor list\n",
				wlc->pub->unit));
			err = wlc_scan_request(wlc, DOT11_BSSTYPE_ANY, &ether_bcast, 1,
					&ssid, 0, NULL, DOT11_SCANTYPE_PASSIVE, -1, -1, -1, -1,
					NULL, 0, 0, FALSE, wlc_oce_scan_complete_cb, wlc,
					WLC_ACTION_SCAN, FALSE, bsscfg, NULL, NULL);

			if (err) {
				WL_ERROR(("wl%d: %s: RNR scan failed [%d]\n",
					wlc->pub->unit, __FUNCTION__, err));
			}

			/* just one bss scan is ok */
			break;
		}
	}
#endif /* WL_OCE && WL_OCE_AP */
	if (wlc->rnr_update_method == WLC_RNR_UPDATE_METHOD_STATIC) {
		FOREACH_UP_AP(wlc, i, bsscfg) {
			WL_INFORM(("wl%d: use RRM module's static list of neighbor to"
				" prepare reduced neighbor list\n", wlc->pub->unit));
			wlc_ap_update_static_rnr_nbr_info(wlc, bsscfg);
			break; /* should we send RNR IE in each BSS's beacon of MBSS ? */
		}
	}
end:
	RNR_UPDATE_ADD_TIMER(wlc, wlc->rnr_update_period);
} /* wlc_rnr_update_timer */

/* release old malloc, create new buffer, update with updated probe response body template
 * wlc_ap_upr_fd_timer_cb use this new template to transmit in air
 */
void
wlc_ap_bcast_probe_rsp_update_required(wlc_info_t* wlc, wlc_bsscfg_t *cfg)
{
	int len;
	uint8 *pbody;

	if (!wlc->upr_fd_info) {
		WL_ERROR(("wl%d: upr_fd_info block not configured, Return\n", wlc->pub->unit));
		return;
	}

	if (wlc->upr_fd_info->cfg && (cfg != wlc->upr_fd_info->cfg)) {
		/* in case of MBSS, only one AP cfg is required to
		 * transmit UPR or FD
		 */
		return;
	}
	if (!WL_UPR_FD_SW_ENAB(wlc)) {
		return;
	}

	if (wlc->upr_fd_info->pkt) {
		PKTFREE(wlc->osh, wlc->upr_fd_info->pkt, TRUE);
		wlc->upr_fd_info->one_time_alloc--;
	}

	if (!BSSCFG_AP(cfg)) {
		return;
	}

	wlc->upr_fd_info->cfg = cfg;

#ifdef WL_OCE_AP
	wlc_oce_set_short_ssid(wlc, cfg);
#endif /* WL_OCE_AP */

	if (isset(&(wlc->nbr_discovery_cap), WLC_NBR_DISC_CAP_20TU_FILS_DISCOVERY)) {
		return;
	}
	len = wlc->pub->bcn_tmpl_len;
	wlc->upr_fd_info->pkt = wlc_frame_get_mgmt(wlc, FC_PROBE_RESP, &ether_bcast,
		&cfg->cur_etheraddr, &cfg->BSSID, len, &pbody);

	if (wlc->upr_fd_info->pkt == NULL) {
		WL_ERROR(("wl%d.%d: %s: wlc_frame_get_mgmt failed\n",
		wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__));
		return;
	}
	wlc->upr_fd_info->one_time_alloc++;
	WL_INFORM(("wl%d: Software UPR or FD from cfg index[%d]\n", wlc->pub->unit,
		WLC_BSSCFG_IDX(cfg)));
	/* Generate probe response body */
	wlc_bcn_prb_body(wlc, FC_PROBE_RESP, cfg, pbody, &len, NULL);
	PKTSETLEN(wlc->osh, wlc->upr_fd_info->pkt, len + DOT11_MGMT_HDR_LEN);
}

/* Add RNR element in beacon/probe resp per 802.11ai-2016 amendment-1,
 * 9.4.2.171.1 and WFA OCE Spec v18, 3.4.
 */
static uint
wlc_rnr_element_calc_len(void *ctx, wlc_iem_calc_data_t *data)
{
	wlc_bsscfg_t *cfg = data->cfg;

	ASSERT(cfg);
	/* cfg->rnr_nbr_ap_info_size holds the list of neighbor ap info filled
	 * from neighbor list present with RRM module.
	 * For both hidden ssid and wildcard probe request, assumption is that
	 * nbr list will be same except same_ssid bit set/reset for neighbor ap
	 * info.
	 */
	if (!BSSCFG_AP(cfg) || !cfg->rnr_nbr_ap_info_size ||
		!isset(&cfg->wlc->nbr_discovery_cap, WLC_NBR_DISC_CAP_RNR_IE)) {
		return 0;
	}

	return cfg->rnr_nbr_ap_info_size + BCM_TLV_HDR_SIZE;
}

/* Add RNR element in beacon/probe resp per 802.11ai-2016 amendment-1,
 * 9.4.2.171.1 and WFA OCE Spec v18, 3.4.
 */
static int
wlc_rnr_element_build_fn(void *ctx, wlc_iem_build_data_t *data)
{
	wlc_bsscfg_t *cfg = data->cfg;
	uint8 *cp = NULL;

	ASSERT(cfg);

	if (!cfg->rnr_nbr_ap_info_size) {
		return BCME_OK;
	}

	cp = data->buf;
	switch (data->ft) {
		case FC_BEACON:
		case FC_PROBE_RESP:
			(void)wlc_write_rnr_element(cfg, cp, data);
			break;
		default:
			break;
	}

	return BCME_OK;
}

static uint16
wlc_write_rnr_element(wlc_bsscfg_t *bsscfg, uint8 *cp, wlc_iem_build_data_t *data)
{
	wlc_bsscfg_t *cfg = data->cfg;
	wlc_ssid_t *ssid = NULL;
	void *nbr_ap_info = NULL;
	uint8 bytes_rd = 0;

	if (data->ft != FC_PROBE_RESP) {
		goto use_rnr_nbr_ap_info;
	}
	if (data->cbparm && data->cbparm->ft && data->cbparm->ft->bcn.ssid) {
		ssid = data->cbparm->ft->bcn.ssid;

		if ((ssid->SSID_len == cfg->SSID_len) &&
			(bcmp(ssid->SSID, cfg->SSID, ssid->SSID_len) == 0)) {

#ifdef WL11K_NBR_MEAS
#ifdef WL11K_AP
#if BAND6G || defined(WL_OCE_AP)
			if (!wlc_rrm_any_nbr_hidden(bsscfg)) {
				/* no neighbor found operating with hidden ssid, no need
				 * to set/reset same_ssid bit in TBTT information field.
				 * use existing bsscfg's nbr_ap_info buffer
				 */
				goto use_rnr_nbr_ap_info;
			}
#endif /* BAND6G || WL_OCE_AP */
#endif /* WL11K_AP */
#endif /* WL11K_NBR_MEAS */
			if (!wlc_rrm_ssid_is_in_nbr_list(bsscfg, ssid,
					data->cbparm->ft->bcn.short_ssid,
					data->cbparm->ft->bcn.short_ssid_len)) {
				/* no neighbor found with matching ssid, no need to
				 * recalculate neighbor ap info buffer. Use existing
				 * bsscfg's rnr_nbr_ap_info
				 */
				goto use_rnr_nbr_ap_info;
			}
			nbr_ap_info = MALLOCZ(bsscfg->wlc->osh, BCM_TLV_MAX_DATA_SIZE);

			if (nbr_ap_info == NULL) {
				WL_ERROR(("wl%d: %s: out of mem, malloced %d bytes\n",
					bsscfg->wlc->pub->unit, __FUNCTION__,
					MALLOCED(bsscfg->wlc->osh)));
				return BCME_NOMEM;
			}

#ifdef WL11K_NBR_MEAS
#ifdef WL11K_AP
#if BAND6G || defined(WL_OCE_AP)
			wlc_rrm_fill_static_rnr_info(bsscfg->wlc, bsscfg, nbr_ap_info,
				BCM_TLV_MAX_DATA_SIZE, &bytes_rd, TRUE);
#endif /* BAND6G || WL_OCE_AP */
#endif /* WL11K_AP */
#endif /* WL11K_NBR_MEAS */
			bcm_write_tlv(DOT11_MNG_RNR_ID, nbr_ap_info, bytes_rd, cp);

			MFREE(bsscfg->wlc->osh, nbr_ap_info, BCM_TLV_MAX_DATA_SIZE);
			nbr_ap_info = NULL;
			return bytes_rd + BCM_TLV_HDR_SIZE;
		}
	}
use_rnr_nbr_ap_info:

	ASSERT(data->buf_len >= (bsscfg->rnr_nbr_ap_info_size + BCM_TLV_HDR_SIZE));

	bcm_write_tlv(DOT11_MNG_RNR_ID, bsscfg->rnr_nbr_ap_info, bsscfg->rnr_nbr_ap_info_size, cp);

	return bsscfg->rnr_nbr_ap_info_size + BCM_TLV_HDR_SIZE;
}

static void
wlc_ap_upr_fd_timer_cb(void *ctx)
{
	wlc_info_t *wlc = (wlc_info_t*)ctx;
	wlc_upr_fd_info_t *upr_fd_info = NULL;
	wlc_bsscfg_t *bsscfg;
	bool send_bcast_prb_rsp = FALSE;
	bool send_fd = FALSE;

	upr_fd_info = wlc->upr_fd_info;

	if (isset(&(wlc->nbr_discovery_cap), WLC_NBR_DISC_CAP_20TU_BCAST_PRB_RSP)) {
		send_bcast_prb_rsp = TRUE;
	}

	if (isset(&(wlc->nbr_discovery_cap), WLC_NBR_DISC_CAP_20TU_FILS_DISCOVERY)) {
		send_fd = TRUE;
	}

	if (send_bcast_prb_rsp && send_fd) {
		WL_ERROR(("wl%d: both UPR and FD requested, not supported. Exit\n",
			wlc->pub->unit));
		return;
	}

	if ((!send_fd && !send_bcast_prb_rsp) || !upr_fd_info->period) {
		/* time to stop the timer, exit */
		WL_INFORM(("wl%d: %d bcast probe response packet transmitted since"
			" timer started, Now stopping the timer\n", wlc->pub->unit,
			upr_fd_info->n_pkts_xmit));
		return;
	}

	bsscfg = upr_fd_info->cfg;
	ASSERT(bsscfg);

	if (send_bcast_prb_rsp) {
		wlc_ap_send_bcast_prb_rsp(wlc, bsscfg);
	}
	if (send_fd) {
		wlc_ap_send_fd(wlc, bsscfg);
	}
	upr_fd_info->count++;
	if (upr_fd_info->count < upr_fd_info->max_per_interval) {
		wlc_hrt_add_timeout(wlc->upr_fd_info->timer,
			((upr_fd_info->period) * DOT11_TU_TO_US),
			wlc_ap_upr_fd_timer_cb, wlc);
	} else {
		wlc_hrt_del_timeout(wlc->upr_fd_info->timer);
	}
}

static void
wlc_ap_send_bcast_prb_rsp(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
	osl_t *osh = wlc->osh;
	void *p;

	/* If bcast probe response generation failed then abort */
	if (!wlc->upr_fd_info->pkt) {
		wlc->upr_fd_info->n_pkts_xmit_fail++;
		return;
	}

	if (wlc->upr_fd_info->in_flight >= WLC_UPR_FD_MAX_PKTS_IN_FLIGHT) {
		WL_INFORM(("wl%d: skipping SW UPR, 1 packet still in Queue\n",
			wlc->pub->unit));
		return;
	}
	p = PKTGET(osh, wlc->txhroff + PKTLEN(osh, wlc->upr_fd_info->pkt), TRUE);
	if (!p) {
		WL_ERROR(("wl%d: %s: PKTGET for broadcast probe response failed\n",
			wlc->pub->unit, __FUNCTION__));
		wlc->upr_fd_info->alloc_fail++;
		return;
	}
	wlc->upr_fd_info->n_pktget++;
	PKTPULL(osh, p, wlc->txhroff);
	PKTSETLEN(osh, p, PKTLEN(osh, wlc->upr_fd_info->pkt));
	memcpy(PKTDATA(osh, p), PKTDATA(osh, wlc->upr_fd_info->pkt),
		PKTLEN(osh, wlc->upr_fd_info->pkt));

	/* Set MAX Prio for MGMT packets */
	PKTSETPRIO(p, MAXPRIO);
	WLPKTTAG(p)->flags |= WLF_PSDONTQ;
	WLPKTTAG(p)->flags |= WLF_CTLMGMT;
	WLPKTTAG(p)->flags3 |= WLF3_TXQ_SHORT_LIFETIME;
	WLPKTTAG(p)->flags |= WLF_TX_PKT_UPR_FD;

	/* life time of pkt 20 msec */
	wlc_lifetime_set(wlc, p, wlc->upr_fd_info->period * 1000);

	if (wlc_queue_80211_frag(wlc, p, bsscfg->wlcif->qi, wlc->band->hwrs_scb, bsscfg, FALSE,
		NULL, WLC_RATE_6M)) {
		wlc->upr_fd_info->n_pkts_xmit++;
		wlc->upr_fd_info->in_flight++;
	} else {
		wlc->upr_fd_info->n_pkts_xmit_fail++;
		wlc->upr_fd_info->n_pktfree++;
	}
	return;
}

static void
wlc_ap_send_fd(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg)
{
#if defined(WL_OCE) && defined(WL_OCE_AP)
	wlc_oce_send_fd_frame(wlc->oce, bsscfg);
	return;
#endif /* OCE && WL_OCE_AP */
}

void
wlc_ap_start_upr_fd_timer(wlc_info_t *wlc)
{
	wlc_upr_fd_info_t *upr_fd_info = NULL;
	uint8 psc_chan = 0;
	bool send_upr_or_fd = FALSE;

	if (!WL_UPR_FD_SW_ENAB(wlc)) {
		return;
	}

	upr_fd_info = wlc->upr_fd_info;
	ASSERT(upr_fd_info);

	if (!upr_fd_info->cfg) {
		/* No cfg configured to send UPR/FD. Let wlc_update_probe_response
		 * to configure this
		 */
		return;
	}

	/* make common timer for both FD and BCAST PRSP */
	if (isset(&(wlc->nbr_discovery_cap), WLC_NBR_DISC_CAP_20TU_BCAST_PRB_RSP)) {
		send_upr_or_fd = TRUE;
		if (BAND_6G(wlc->band->bandtype)) {
			psc_chan = wf_chspec_primary20_chspec(
				upr_fd_info->cfg->current_bss->chanspec);
			if (!WF_IS_6G_PSC_CHAN(psc_chan)) {
				WL_ERROR(("wl%d:%d SW UPR not enabled on NON PSC channel\n",
					wlc->pub->unit,	WLC_BSSCFG_IDX(wlc->main_ap_bsscfg)));
				return;
			}
		}
	}

	if (isset(&(wlc->nbr_discovery_cap), WLC_NBR_DISC_CAP_20TU_FILS_DISCOVERY)) {
		send_upr_or_fd = TRUE;
	}

	if (!send_upr_or_fd) {
		return;
	}

	upr_fd_info->n_pkts_xmit = 0;
	upr_fd_info->count = 0;
	upr_fd_info->max_per_interval =
		(upr_fd_info->cfg->current_bss->beacon_period / upr_fd_info->period) - 1;
#ifdef ENABLE_FOR_6G_CERT
	/* take care of different interval period than 20 ms, e.g for 17 ms
	 * used in 6G staut test case
	 */
	if ((upr_fd_info->cfg->current_bss->beacon_period -
		(upr_fd_info->count * upr_fd_info->period)) > upr_fd_info->period) {
		upr_fd_info->max_per_interval++;
	}
#endif /* for certification enable */

	wlc_hrt_add_timeout(wlc->upr_fd_info->timer,
		((upr_fd_info->period)* DOT11_TU_TO_US),
		wlc_ap_upr_fd_timer_cb, wlc);
	WL_INFORM(("wl%d: %d TU timer started\n", wlc->pub->unit, upr_fd_info->period));
}
#endif /* BAND6G || (WL_OCE && WL_OCE_AP) */

/* SSID */
static int
wlc_assoc_parse_ssid_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	wlc_bsscfg_t *cfg = data->cfg;
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;

	BCM_REFERENCE(wlc);

	/* special case handling where some IoT devices send reassoc req with empty ssid. */
	if (data->ie_len == TLV_BODY_OFF) {
		WL_ASSOC(("wl%d: "MACF": association with empty SSID\n",
			WLCWLUNIT(wlc), ETHER_TO_MACF(ftpparm->assocreq.scb->ea)));
		ftpparm->assocreq.status = DOT11_SC_SUCCESS;
		return BCME_OK;
	}

	if (data->ie == NULL || data->ie_len < TLV_BODY_OFF) {
		WL_ASSOC(("wl%d: "MACF": attempted association with no SSID\n",
			WLCWLUNIT(wlc), ETHER_TO_MACF(ftpparm->assocreq.scb->ea)));
		ftpparm->assocreq.status = DOT11_SC_FAILURE;
		return BCME_ERROR;
	}

	/* failure if the SSID does not match any active AP config */
	if (!WLC_IS_MATCH_SSID(cfg->SSID, &data->ie[TLV_BODY_OFF],
		cfg->SSID_len, data->ie[TLV_LEN_OFF])) {
#if defined(BCMDBG) || defined(WLMSG_ASSOC)
		char ssidbuf[SSID_FMT_BUF_LEN];

		wlc_format_ssid(ssidbuf, &data->ie[TLV_BODY_OFF], data->ie[TLV_LEN_OFF]);
		WL_ASSOC(("wl%d: "MACF": attempted association with incorrect SSID ID\"%s\"\n",
			WLCWLUNIT(wlc), ETHER_TO_MACF(ftpparm->assocreq.scb->ea),
			ssidbuf));
#endif
		ftpparm->assocreq.status = DOT11_SC_ASSOC_FAIL;
		return BCME_ERROR;
	}

	ftpparm->assocreq.status = DOT11_SC_SUCCESS;

	return BCME_OK;
}

static int
wlc_assoc_parse_sup_rates_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;
	wlc_rateset_t *sup = ftpparm->assocreq.sup;

	BCM_REFERENCE(ctx);

	bzero(sup, sizeof(*sup));

	if (data->ie == NULL || data->ie_len <= TLV_BODY_OFF) {
#if defined(BCMDBG) || defined(WLMSG_ASSOC)
		struct scb *scb = ftpparm->assocreq.scb;
		wlc_info_t *wlc = (wlc_info_t *)ctx;

		WL_ASSOC(("wl%d: "MACF": attempted association with no Supported Rates IE\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
#endif
		/* Ignore enforcing mandatory Support Rates IE check to
		 * workaround some APs in the field not including the IE,
		 * The validation will be done by wlc_combine_rateset()
		 */
		return BCME_OK;
	}

#ifdef BCMDBG
	if (data->ie[TLV_LEN_OFF] > WLC_NUMRATES) {
		wlc_info_t *wlc = (wlc_info_t *)ctx;

		WL_ERROR(("wl%d: %s: IE contains too many rates, truncate\n",
		          wlc->pub->unit, __FUNCTION__));
	}
#endif

	sup->count = MIN(data->ie[TLV_LEN_OFF], WLC_NUMRATES);
	bcopy(&data->ie[TLV_BODY_OFF], sup->rates, sup->count);

	ftpparm->assocreq.status = DOT11_SC_SUCCESS;

	return BCME_OK;
}

static int
wlc_assoc_parse_ext_rates_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;
	wlc_rateset_t *ext = ftpparm->assocreq.ext;

	BCM_REFERENCE(ctx);

	bzero(ext, sizeof(*ext));

	if (data->ie == NULL || data->ie_len <= TLV_BODY_OFF) {
#if defined(BCMDBG) || defined(WLMSG_ASSOC)
		struct scb *scb = ftpparm->assocreq.scb;
		wlc_info_t *wlc = (wlc_info_t *)ctx;

		WL_ASSOC(("wl%d: "MACF": attempted association with "
			"no Extended Supported Rates IE\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
#endif
		return BCME_OK;
	}

#ifdef BCMDBG
	if (data->ie[TLV_LEN_OFF] > WLC_NUMRATES) {
		wlc_info_t *wlc = (wlc_info_t *)ctx;

		WL_ERROR(("wl%d: %s: IE contains too many rates, truncate\n",
			wlc->pub->unit, __FUNCTION__));
	}
#endif

	ext->count = MIN(data->ie[TLV_LEN_OFF], WLC_NUMRATES);
	bcopy(&data->ie[TLV_BODY_OFF], ext->rates, ext->count);

	ftpparm->assocreq.status = DOT11_SC_SUCCESS;

	return BCME_OK;
}

static int
wlc_assoc_parse_supp_chan_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;
	wlc_supp_channels_t *supp_chan = ftpparm->assocreq.supp_chan;
	wlc_supp_chan_set_t *chanset = NULL;
	uint8 *iedata = NULL;
	uint8 datalen;
	int count;
	int i = 0, j = 0;
	uint8 channel, chan_sep;

	bzero(supp_chan, sizeof(*supp_chan));

	if (data->ie == NULL || data->ie_len <= TLV_BODY_OFF) {
#if defined(BCMDBG) || defined(WLMSG_ASSOC)
		struct scb *scb = ftpparm->assocreq.scb;
		wlc_info_t *wlc = (wlc_info_t *)ctx;

		WL_ASSOC(("wl%d: "MACF": attempted association with no Supported Channels IE\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
#endif /* BCMDBG || WLMSG_ASSOC */
		return BCME_OK;
	}

#ifdef BCMDBG
	if (data->ie[TLV_LEN_OFF] > (WL_NUMCHANNELS * sizeof(wlc_supp_chan_set_t))) {
		wlc_info_t *wlc = (wlc_info_t *)ctx;

		WL_ERROR(("wl%d: %s: IE contains too many channels, truncate\n",
		          wlc->pub->unit, __FUNCTION__));
	}
#endif /* BCMDBG */

	datalen = MIN(data->ie[TLV_LEN_OFF], (WL_NUMCHANNELS * sizeof(wlc_supp_chan_set_t)));
	count = datalen / sizeof(wlc_supp_chan_set_t);
	iedata = &data->ie[TLV_BODY_OFF];

	for (i = 0; i < count; i++) {
		chanset = (wlc_supp_chan_set_t *)iedata;
		supp_chan->count += chanset->num_channels;

		channel = chanset->first_channel;
		chan_sep = (channel > CH_MAX_2G_CHANNEL) ?
			CH_20MHZ_APART : CH_5MHZ_APART;

		for (j = 0; j < chanset->num_channels; j++) {
			if (CH_NUM_VALID_RANGE(channel)) {
				setbit((void *)supp_chan->chanset, channel);
			} else {
				break;
			}
			channel += chan_sep;
		}
		iedata += sizeof(wlc_supp_chan_set_t);
	}

	ftpparm->assocreq.status = DOT11_SC_SUCCESS;
	return BCME_OK;
}

static int
wlc_assoc_parse_wps_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;
	wlc_bsscfg_t *cfg = data->cfg;

	if (WSEC_SES_OW_ENABLED(cfg->wsec)) {
		bcm_tlv_t *wpsie = (bcm_tlv_t *)data->ie;
		struct scb *scb = ftpparm->assocreq.scb;

		if (wpsie == NULL)
			return BCME_OK;

		if (wpsie->len < 5) {
			WL_ERROR(("wl%d: wlc_assocresp: unsupported request in WPS IE from "
				MACF"\n", wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
			ftpparm->assocreq.status = DOT11_SC_ASSOC_FAIL;
			return BCME_ERROR;
		}

		ftpparm->assocreq.wps_ie = (uint8 *)wpsie;
		scb->flags |= SCB_WPSCAP;

		return wlc_scb_save_wpa_ie(wlc, scb, wpsie);
	}

	return BCME_OK;
}

/** Called when e.g. the beacon rate needs to be programmed into the d11 core */
ratespec_t
wlc_ap_forced_bcn_rspec_get(wlc_info_t *wlc)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*)wlc->ap;

	if (appvt->user_forced_bcn_rspec != 0) {
		return appvt->user_forced_bcn_rspec;
	}

	return appvt->oce_forced_bcn_rspec;
}

/** Called only if the OCE module forces a beacon rate */
void
wlc_ap_oce_forced_bcn_rspec_set(wlc_info_t *wlc, ratespec_t rspec)
{
	wlc_ap_info_pvt_t* appvt;

	/* Don't validate if force is being reset(0). */
	if (rspec) {
		ASSERT(wlc_valid_rate(wlc, rspec, WLC_BAND_AUTO, TRUE));
	}

	appvt = (wlc_ap_info_pvt_t*)wlc->ap;
	appvt->oce_forced_bcn_rspec = rspec;
}

/**
 * Called only if the user (wl utility) forces a beacon rate.
 *
 * @param[in] value   Either a rate in [500Kbps], or a ratespec.
 */
static int
wlc_user_forced_bcn_rspec_upd(wlc_info_t *wlc, wlc_bsscfg_t *cfg, wlc_ap_info_t *ap, uint32 value)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	ratespec_t rspec;

	if (value != 0 && (value & ~RATE_MASK) == 0) { /* then the value was in [500Kbps] units */
		uint8 rate = (value & RATE_MASK);
		uint i;

		rspec = LEGACY_RSPEC(rate);
		/* check rspec is valid */
		if (!wlc_valid_rate(wlc, rspec, CHSPEC_BANDTYPE(cfg->current_bss->chanspec),
		    TRUE)) {
			return BCME_BADRATESET;
		}

		/* check rspec is basic rate */
		for (i = 0; i < cfg->current_bss->rateset.count; i++) {
			if ((cfg->current_bss->rateset.rates[i] & WLC_RATE_FLAG) &&
			    (cfg->current_bss->rateset.rates[i] & RATE_MASK) == rate) {
				wlc_rspec_txexp_upd(wlc, &rspec);
				appvt->user_forced_bcn_rspec = rspec;
				break;
			}
		}

		if (i == cfg->current_bss->rateset.count)
			return BCME_UNSUPPORTED;
	} else { /* then the value was either 0 or a ratespec */
		rspec = value;
		if (rspec != 0 && RSPEC_BW(rspec) == WL_RSPEC_BW_UNSPECIFIED) {
			rspec |= WL_RSPEC_BW_20MHZ;
		}

		if (rspec != 0 &&
		    !wlc_valid_rate(wlc, rspec, CHSPEC_BANDTYPE(cfg->current_bss->chanspec), TRUE))
		{
			return BCME_BADRATESET;
		} else {
			appvt->user_forced_bcn_rspec = rspec;
		}
	}

	/* update beacon rate */
	wlc_update_beacon(wlc);

	return BCME_OK;
} /* wlc_user_forced_bcn_rspec_upd */

int
wlc_ap_sendauth(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg, struct scb *scb,
	int auth_alg, int auth_seq, int status, bool short_preamble)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) ap;
	wlc_info_t *wlc = appvt->wlc;

	/* send authentication response */
	WL_ASSOC(("wl%d.%d: %s: "MACF" authenticated\n", wlc->pub->unit,
		WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, ETHER_TO_MACF(scb->ea)));

	if ((status == DOT11_SC_SUCCESS) && scb && SCB_AUTHENTICATED(scb)) {
		wlc_bss_mac_event(wlc, bsscfg, WLC_E_AUTH_IND, &scb->ea,
			WLC_E_STATUS_SUCCESS, DOT11_SC_SUCCESS, auth_alg, 0, 0);

#ifdef RXCHAIN_PWRSAVE
		/* fast switch back from rxchain_pwrsave state upon authentication */
		wlc_reset_rxchain_pwrsave_mode(ap);
#endif /* RXCHAIN_PWRSAVE */
	}
#ifdef BCMAUTH_PSK
	if (BSS_AUTH_PSK_ACTIVE(wlc, bsscfg))
		wlc_auth_authreq_recv_update(wlc, bsscfg);
#endif /* BCMAUTH_PSK */

	wlc_sendauth(bsscfg, &scb->ea, &bsscfg->BSSID, scb, auth_alg, auth_seq, status,
		NULL, short_preamble, wlc_ap_auth_tx_complete, scb);

	return BCME_OK;
}

void
wlc_bsscfg_bcn_disable(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
	WL_APSTA_UPDN(("wl%d: wlc_bsscfg_bcn_disable %p #of stas %d\n",
	          wlc->pub->unit, OSL_OBFUSCATE_BUF(cfg),
		wlc_bss_assocscb_getcnt(wlc, cfg)));

	cfg->flags &= ~WLC_BSSCFG_HW_BCN;
	if (cfg->up) {
		wlc_suspend_mac_and_wait(wlc);
		wlc_bmac_write_ihr(wlc->hw, 0x47, 3);
		wlc_enable_mac(wlc);
	}
}

void
wlc_bsscfg_bcn_enable(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
	WL_APSTA_UPDN(("wl%d: wlc_bsscfg_bcn_enable %p #of stas %d\n",
	          wlc->pub->unit, OSL_OBFUSCATE_BUF(cfg),
		wlc_bss_assocscb_getcnt(wlc, cfg)));

	cfg->flags |= WLC_BSSCFG_HW_BCN;
	wlc_bss_update_beacon(wlc, cfg, FALSE);
}

void
wlc_ap_reset_psp_attempt(wlc_ap_info_t *ap, struct scb *scb, uint16 cnt)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_scb_cubby_t *ap_scb = AP_SCB_CUBBY(appvt, scb);

	if (ap_scb != NULL && ap_scb->psp_attempts > cnt) {
		ap_scb->psp_attempts = cnt;
	}
}

bool
wlc_ap_pspretend_probe(wlc_ap_info_t *ap, wlc_bsscfg_t *cfg, struct scb *scb)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	wlc_info_t *wlc = appvt->wlc;
	ratespec_t rate_override;

	/* Only one pspretend probe at a time */
	if (scb->flags & SCB_PSPRETEND_PROBE) {
		return FALSE;
	}

	/* If there is fifo drain in process, we do not send the probe. Partly because
	 * sending a probe might interfere with the fifo drain, but mainly because
	 * even if the data probe was successful, we cannot exit ps pretend state
	 * whilst the fifo drain was still happening. So, waiting for the fifo drain
	 * to finish first is clean, predictable and consistent.
	 * Note that when the SCB_PS_TXFIFO_BLK & DATA_BLOCK_PS condition clears,
	 * a probe is sent regardless.
	 */
#if defined(WL_PS_SCB_TXFIFO_BLK)
	if (SCB_PS_TXFIFO_BLK(scb)) {
		WL_PS(("wl%d.%d: %s: "MACF" SCB_PS_TXFIFO_BLK pending %d pkts\n",
		       wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__,
		       ETHER_TO_MACF(scb->ea), SCB_TOT_PKTS_INFLT_FIFOCNT_VAL(scb)));
		return FALSE;
	}
#endif /* WL_PS_SCB_TXFIFO_BLK */

	if (wlc->block_datafifo & DATA_BLOCK_PS) {
		WL_PS(("wl%d.%d: %s: "MACF" DATA_BLOCK_PS pending %d pkts\n",
		       wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__,
		       ETHER_TO_MACF(scb->ea), TXPKTPENDTOT(wlc)));
		return FALSE;
	}

	/* use the lowest basic rate */
	rate_override = wlc_lowest_basicrate_get(cfg);
	ASSERT(VALID_RATE(wlc, rate_override));

	WL_PS(("wl%d.%d: pps probe to "MACF"\n", wlc->pub->unit,
	        WLC_BSSCFG_IDX(cfg), ETHER_TO_MACF(scb->ea)));

	if (!wlc_sendnulldata(wlc, cfg, &scb->ea, rate_override,
			WLF_PSDONTQ, PRIO_8021D_VO, wlc_ap_sendnulldata_cb, NULL)) {
		WL_ERROR(("wl%d.%d: %s: "MACF" failed\n",
		          wlc->pub->unit, WLC_BSSCFG_IDX(cfg),
		          __FUNCTION__, ETHER_TO_MACF(scb->ea)));
		return FALSE;
	}

	scb->flags |= SCB_PSPRETEND_PROBE;

	return TRUE;
}

/* Challenge Text */
static uint
wlc_auth_calc_chlng_ie_len(void *ctx, wlc_iem_calc_data_t *data)
{
	wlc_iem_ft_cbparm_t *ftcbparm = data->cbparm->ft;

	BCM_REFERENCE(ctx);
	if (ftcbparm->auth.alg == DOT11_SHARED_KEY) {
		if (ftcbparm->auth.seq == 2) {
			return TLV_HDR_LEN + DOT11_CHALLENGE_LEN;
		}
	}

	return 0;
}

static int
wlc_auth_write_chlng_ie(void *ctx, wlc_iem_build_data_t *data)
{
	wlc_iem_ft_cbparm_t *ftcbparm = data->cbparm->ft;

	if (ftcbparm->auth.alg == DOT11_SHARED_KEY) {
		wlc_info_t *wlc = (wlc_info_t *)ctx;
		uint16 status = DOT11_SC_SUCCESS;

		if (ftcbparm->auth.seq == 2) {
			wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)wlc->ap;
			uint8 *chlng;
			wlc_bsscfg_t *cfg = data->cfg;
			struct scb *scb;

			uint i;
			ap_scb_cubby_t *ap_scb;

			scb = ftcbparm->auth.scb;
			if (!scb || !(ap_scb = AP_SCB_CUBBY(appvt, scb))) {
				goto exit;
			}

			if (cfg->WPA_auth != WPA_AUTH_DISABLED) {
				WL_ERROR(("wl%d: %s: unhandled algo Shared Key from "MACF"\n",
					wlc->pub->unit, __FUNCTION__,
					ETHER_TO_MACF(scb->ea)));
				status = DOT11_SC_AUTH_MISMATCH;
				goto exit;
			}

			if (ap_scb->challenge != NULL) {
				MFREE(wlc->osh, ap_scb->challenge, 2 + ap_scb->challenge[1]);
				ap_scb->challenge = NULL;
			}

			/* create the challenge text */
			if ((chlng = MALLOC(wlc->osh, 2 + DOT11_CHALLENGE_LEN)) == NULL) {
				WL_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n",
				          wlc->pub->unit, __FUNCTION__, MALLOCED(wlc->osh)));
				status = DOT11_SC_FAILURE;
				goto exit;
			}
			chlng[0] = DOT11_MNG_CHALLENGE_ID;
			chlng[1] = DOT11_CHALLENGE_LEN;
			for (i = 0; i < DOT11_CHALLENGE_LEN; i++) {
				uint16 l_rand = R_REG(wlc->osh, D11_TSF_RANDOM(wlc));
				chlng[i+2] = (uint8)l_rand;
			}

			/* write to frame */
			bcopy(chlng, data->buf, 2 + DOT11_CHALLENGE_LEN);
#ifdef BCMDBG
			if (WL_ASSOC_ON()) {
				prhex("Auth challenge text #2", chlng, 2 + DOT11_CHALLENGE_LEN);
			}
#endif
			ap_scb->challenge = chlng;

		exit:
			;
		}

		ftcbparm->auth.status = status;
	}

	return BCME_OK;
} /* wlc_auth_write_chlng_ie */

static int
wlc_auth_parse_chlng_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;

	if (ftpparm->auth.alg == DOT11_SHARED_KEY) {
		wlc_info_t *wlc = (wlc_info_t *)ctx;
		uint16 status = DOT11_SC_SUCCESS;

		if (ftpparm->auth.seq == 3) {
			wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)wlc->ap;
			uint8 *chlng = data->ie;

			struct scb *scb;
			ap_scb_cubby_t *ap_scb;

			scb = ftpparm->auth.scb;
			ASSERT(scb != NULL);

			ap_scb = AP_SCB_CUBBY(appvt, scb);
			ASSERT(ap_scb != NULL);

			/* Check length */
			if (data->ie_len != TLV_HDR_LEN + DOT11_CHALLENGE_LEN) {
				WL_ASSOC(("wl%d: wrong length WEP Auth Challenge from "MACF"\n",
					wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
				status = DOT11_SC_AUTH_CHALLENGE_FAIL;
				goto cleanup;
			}

			/* No Challenge Text */
			if (chlng == NULL) {
				WL_ASSOC(("wl%d: no WEP Auth Challenge from "MACF"\n",
					wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
				status = DOT11_SC_AUTH_CHALLENGE_FAIL;
				goto cleanup;
			}

			/* Failed Challenge Text comparison */
			if (bcmp(&ap_scb->challenge[2], &chlng[2], chlng[1]) != 0) {
				WL_ERROR(("wl%d: failed verify WEP Auth Challenge from "MACF"\n",
					wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
				wlc_scb_clearstatebit(wlc, scb, AUTHENTICATED);
				status = DOT11_SC_AUTH_CHALLENGE_FAIL;
				goto cleanup;
			}

			WL_ASSOC(("wl%d: WEP Auth Challenge success from "MACF"\n",
				wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
			wlc_scb_setstatebit(wlc, scb, AUTHENTICATED);
			scb->auth_alg = DOT11_SHARED_KEY;

		cleanup:
			/* free the challenge text */
			MFREE(wlc->osh, ap_scb->challenge, 2 + ap_scb->challenge[1]);
			ap_scb->challenge = NULL;
		}

		ftpparm->auth.status = status;
		if (status != DOT11_SC_SUCCESS) {
			WL_INFORM(("wl%d: %s: signal to stop parsing IEs, status %u\n",
			           wlc->pub->unit, __FUNCTION__, status));
			return BCME_ERROR;
		}
	}

	return BCME_OK;
} /* wlc_auth_parse_chlng_ie */

int
wlc_scb_save_wpa_ie(wlc_info_t *wlc, struct scb *scb, bcm_tlv_t *ie)
{
	uint ie_len = 0;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)wlc->ap;
	ap_scb_cubby_t *ap_scb = AP_SCB_CUBBY(appvt, scb);

	if (ap_scb == NULL) {
		return BCME_NOTAP;
	}

	if (ie != NULL) {
		ie_len = TLV_HDR_LEN + ie->len;

		/* Optimization */
		if (ap_scb->wpaie != NULL && ie != NULL &&
		    ap_scb->wpaie_len == ie_len)
			goto cp;
	}

	/* Free old WPA IE if one exists */
	if (ap_scb->wpaie != NULL) {
		MFREE(wlc->osh, ap_scb->wpaie, ap_scb->wpaie_len);
		ap_scb->wpaie_len = 0;
		ap_scb->wpaie = NULL;
	}

	if (ie != NULL) {
		/* Store the WPA IE for later retrieval */
		if ((ap_scb->wpaie = MALLOCZ(wlc->osh, ie_len)) == NULL) {
			WL_ERROR((WLC_MALLOC_ERR, WLCWLUNIT(wlc), __FUNCTION__, (int)ie_len,
			          MALLOCED(wlc->osh)));
			return BCME_NOMEM;
		}

	cp:	/* copy */
		bcopy(ie, ap_scb->wpaie, ie_len);
		ap_scb->wpaie_len = (uint16)ie_len;
	}

	return BCME_OK;
}

/* copy wpaie to output buffer. buf_len is buffer size at input; it's wpaie length
 * at output including the TLV header.
 */
void
wlc_ap_find_wpaie(wlc_ap_info_t *ap, struct scb *scb, uint8 **wpaie, uint *wpaie_len)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_scb_cubby_t *ap_scb = AP_SCB_CUBBY(appvt, scb);

	if (ap_scb != NULL) {
		*wpaie = ap_scb->wpaie;
		*wpaie_len = ap_scb->wpaie_len;
	}
	else {
		*wpaie = NULL;
		*wpaie_len = 0;
	}
}

uint
wlc_ap_get_activity_time(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	return appvt->scb_activity_time;
}

uint
wlc_ap_get_pre_tbtt(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	return appvt->pre_tbtt_us;
}

void
wlc_ap_set_chanspec(wlc_ap_info_t *ap, chanspec_t chspec)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	appvt->chanspec_selected = chspec;
}

struct scb *
wlc_ap_get_psta_prim(wlc_ap_info_t *ap, struct scb *scb)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_scb_cubby_t *ap_scb = AP_SCB_CUBBY(appvt, scb);
	return ap_scb != NULL ? ap_scb->psta_prim : NULL;
}

static int
wlc_assoc_parse_psta_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;
	wlc_bsscfg_t *cfg = data->cfg;
	struct scb *scb = ftpparm->assocreq.scb;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)wlc->ap;
	ap_scb_cubby_t *ap_scb = AP_SCB_CUBBY(appvt, scb);

	if (!BSSCFG_AP(cfg)) {
		return BCME_OK;
	}

	if (data->ie == NULL) {
		WL_ASSOC(("wl%d: "MACF": attempted association with no primary PSTA information\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
		return BCME_OK;
	}

	ASSERT(ap_scb != NULL);

	if (data->ie[TLV_LEN_OFF] == MEMBER_OF_BRCM_PROP_IE_LEN) {
		member_of_brcm_prop_ie_t *member_of_brcm_prop_ie =
		        (member_of_brcm_prop_ie_t *)data->ie;

		ap_scb->psta_prim = wlc_scbfindband(wlc, cfg, &member_of_brcm_prop_ie->ea,
			CHSPEC_BANDUNIT(cfg->current_bss->chanspec));
	}

	/* WAR to fix non-primary associating before primary interface assoc */
	if (ap_scb->psta_prim == NULL) {
		WL_ASSOC(("wl%d: "MACF": attempted association with no primary PSTA\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea)));
		ftpparm->assocreq.status = DOT11_SC_ASSOC_FAIL;
		return BCME_ERROR;
	}

	if (ap_scb->psta_prim == scb) {
		/* This is the primary PSTA so make the pointer NULL to indicate it */
		ap_scb->psta_prim = NULL;
	}

	ftpparm->assocreq.status = DOT11_SC_SUCCESS;

	return BCME_OK;
} /* wlc_assoc_parse_psta_ie */

void
wlc_ap_channel_switch(wlc_ap_info_t *ap, wlc_bsscfg_t *cfg)
{
	ASSERT((ap && cfg));

	if (BSSCFG_AP(cfg)) {
		wlc_ap_timeslot_register(cfg);
	}
}

int
wlc_ap_timeslot_register(wlc_bsscfg_t *bsscfg)
{
	wlc_info_t *wlc;
	wlc_ap_info_pvt_t *appvt;
	ap_bsscfg_cubby_t *ap_cfg;
	wlc_msch_req_param_t *req;
	uint32 tbtt_h, tbtt_l;
	uint32 err = 0;
	uint32 min_dur;
	uint32 max_away_dur, passive_time;
	uint64 start_time;
	bool prev_awake;
	DBGONLY(char chanbuf[CHANSPEC_STR_LEN]; )

	ASSERT(bsscfg);
	wlc = bsscfg->wlc;

	if (!BSSCFG_AP(bsscfg)) {
		return BCME_ERROR;
	}

	WL_INFORM(("wl%d.%d: %s: chanspec %s\n", wlc->pub->unit,
		WLC_BSSCFG_IDX(bsscfg), __FUNCTION__,
		wf_chspec_ntoa(bsscfg->target_bss->chanspec, chanbuf)));

	ASSERT(bsscfg->current_bss);

	appvt = (wlc_ap_info_pvt_t *)wlc->ap;
	ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);
	ASSERT(ap_cfg != NULL);

	if (ap_cfg->msch_req_hdl) {
		/* clean up the prev status and make new start ready.
		 */
		ap_cfg->msch_state = AP_SCHED_UNREG;
		wlc_ap_prepare_off_channel(wlc, bsscfg);
		wlc_txqueue_end(wlc, bsscfg);
		wlc_msch_timeslot_unregister(wlc->msch_info, &ap_cfg->msch_req_hdl);
		ap_cfg->msch_req_hdl = NULL;
	}

	req = &ap_cfg->req;
	memset(req, 0, sizeof(wlc_msch_req_param_t));

	req->req_type = MSCH_RT_BOTH_FLEX;
	req->flags = 0;
	req->duration = bsscfg->current_bss->beacon_period;
	req->duration <<= MSCH_TIME_UNIT_1024US;
	req->interval = req->duration;
	req->priority = MSCH_RP_CONNECTION;

	wlc_force_ht(wlc, TRUE, &prev_awake);
	wlc_read_tsf(wlc, &tbtt_l, &tbtt_h);
	wlc_force_ht(wlc, prev_awake, NULL);
#ifdef WLMCNX
	if (MCNX_ENAB(wlc->pub))
		wlc_mcnx_l2r_tsf64(wlc->mcnx, bsscfg, tbtt_h, tbtt_l, &tbtt_h, &tbtt_l);
#endif
	wlc_tsf64_to_next_tbtt64(bsscfg->current_bss->beacon_period, &tbtt_h, &tbtt_l);
#ifdef WLMCNX
	if (MCNX_ENAB(wlc->pub))
		wlc_mcnx_r2l_tsf64(wlc->mcnx, bsscfg, tbtt_h, tbtt_l, &tbtt_h, &tbtt_l);
#endif
	start_time = msch_tsf_to_mschtime(wlc->msch_info, tbtt_l, tbtt_h, &req->start_time_l,
		&req->start_time_h);

	err = wlc_iovar_op(wlc, "scan_home_time", NULL, 0,
			&min_dur, sizeof(min_dur), IOV_GET, NULL);
	if (err != BCME_OK) {
		min_dur = WLC_SCAN_HOME_TIME;
	}
	min_dur = MS_TO_USEC(min_dur);

	err = wlc_iovar_op(wlc, "scan_home_away_time", NULL, 0,
			&max_away_dur, sizeof(max_away_dur), IOV_GET, NULL);
	if (err != BCME_OK) {
		max_away_dur = WLC_SCAN_AWAY_LIMIT;
	}
	max_away_dur = MS_TO_USEC(max_away_dur);

	err = wlc_iovar_op(wlc, "scan_passive_time", NULL, 0,
			&passive_time, sizeof(passive_time), IOV_GET, NULL);
	if (err != BCME_OK) {
		passive_time = WLC_SCAN_PASSIVE_TIME;
	}
	passive_time = MS_TO_USEC(passive_time) + MSCH_MIN_ONCHAN_TIME;

	req->flex.bf.min_dur = min_dur;
	req->flex.bf.max_away_dur = MAX(max_away_dur, passive_time) +
		MSCH_EXTRA_DELAY_FOR_MAX_AWAY_DUR;
	req->flex.bf.hi_prio_time_l = (uint32)(start_time & 0xFFFFFFFFU);
	req->flex.bf.hi_prio_time_h = (uint32)(start_time >> 32);
	req->flex.bf.hi_prio_interval = req->duration;

	err = wlc_msch_timeslot_register(wlc->msch_info,
		&bsscfg->target_bss->chanspec, 1,
		wlc_ap_msch_clbk, bsscfg, req, &ap_cfg->msch_req_hdl);

	if (err	== BCME_OK) {
		wlc_msch_set_chansw_reason(wlc->msch_info, ap_cfg->msch_req_hdl, CHANSW_SOFTAP);
		WL_INFORM(("wl%d.%d: %s: request success\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__));
	} else {
		WL_ERROR(("wl%d.%d: %s: request failed error %d\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__, err));
		ASSERT(0);
	}

	return err;
} /* wlc_ap_timeslot_register */

void
wlc_ap_timeslot_unregister(wlc_bsscfg_t *bsscfg)
{
	wlc_info_t *wlc;
	wlc_ap_info_pvt_t *appvt;
	ap_bsscfg_cubby_t *ap_cfg;
	DBGONLY(char chanbuf[CHANSPEC_STR_LEN]; )

	ASSERT(bsscfg);
	wlc = bsscfg->wlc;

	if (!BSSCFG_AP(bsscfg)) {
		return;
	}

	WL_INFORM(("wl%d.%d: %s: chanspec %s\n", wlc->pub->unit,
		WLC_BSSCFG_IDX(bsscfg), __FUNCTION__,
		wf_chspec_ntoa(bsscfg->current_bss->chanspec, chanbuf)));

	appvt = (wlc_ap_info_pvt_t *)wlc->ap;
	ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);

	if ((ap_cfg != NULL) && (ap_cfg->msch_req_hdl)) {
		wlc_msch_timeslot_unregister(wlc->msch_info, &ap_cfg->msch_req_hdl);
		ap_cfg->msch_req_hdl = NULL;
#ifdef WL_DTS
		/* disable DTS tx suppression */
		if (DTS_ENAB(wlc->pub)) {
			wlc_set_cfg_tx_stop_time(wlc, bsscfg, -1);
		}
#endif
	}

}

void
wlc_ap_timeslot_update(wlc_bsscfg_t *bsscfg, uint32 start_tsf, uint32 interval)
{
	wlc_info_t *wlc;
	wlc_ap_info_pvt_t *appvt;
	ap_bsscfg_cubby_t *ap_cfg;
	wlc_msch_req_param_t *req;
	uint32 update_mask;
	uint32 tsf_l;
	int to;
	uint64 start_time;
	bool prev_awake;
	DBGONLY(char chanbuf[CHANSPEC_STR_LEN]; )

	ASSERT(bsscfg);
	wlc = bsscfg->wlc;

	if (!BSSCFG_AP(bsscfg)) {
		ASSERT(FALSE);
		return;
	}

	appvt = (wlc_ap_info_pvt_t *)wlc->ap;
	ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);

	ASSERT(ap_cfg != NULL);
	ASSERT(ap_cfg->msch_req_hdl);

	req = &ap_cfg->req;

	update_mask = MSCH_UPDATE_START_TIME;
	if (interval != req->interval) {
		update_mask |= MSCH_UPDATE_INTERVAL;
	}

	wlc_force_ht(wlc, TRUE, &prev_awake);
	wlc_read_tsf(wlc, &tsf_l, NULL);
	wlc_force_ht(wlc, prev_awake, NULL);

	to = (int)(start_tsf + MSCH_ONCHAN_PREPARE - tsf_l);
	if (to < MSCH_MIN_ONCHAN_TIME) {
		to = MSCH_MIN_ONCHAN_TIME;
	}

	start_time = msch_future_time(wlc->msch_info, (uint32)to);

	WL_INFORM(("wl%d.%d: %s: chanspec %s, start_tsf %d, tsf %d, to %d,"
		"interval %d\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg), __FUNCTION__,
		wf_chspec_ntoa(wlc_get_chanspec(wlc, bsscfg), chanbuf),
		start_tsf, tsf_l, to, interval));

	req->start_time_l = (uint32)(start_time & 0xFFFFFFFFU);
	req->start_time_h = (uint32)(start_time >> 32);
	req->interval = interval;

	wlc_msch_timeslot_update(wlc->msch_info, ap_cfg->msch_req_hdl, req, update_mask);
} /* wlc_ap_timeslot_update */

/** Called back by the multichannel scheduler (msch) */
static int
wlc_ap_msch_clbk(void* handler_ctxt, wlc_msch_cb_info_t *cb_info)
{
	wlc_bsscfg_t *cfg = (wlc_bsscfg_t *)handler_ctxt;
	uint32 type = cb_info->type;
	wlc_info_t *wlc;
	wlc_ap_info_pvt_t *appvt;
	ap_bsscfg_cubby_t *ap_cfg;
	chanspec_t cb_chanspec = cb_info->chanspec;
#ifdef WL_PROXDETECT
	chanspec_t home_chanspec;
#endif
	DBGONLY(char chanbuf[CHANSPEC_STR_LEN]; )

	/* validation */
	if (cfg) {
		wlc = cfg->wlc;
	} else {
		/* The request has been cancelled, ignore the Clbk */
		return BCME_OK;
	}
	if (!BSSCFG_AP(cfg) || !cfg->enable) {
		return BCME_ERROR;
	}

#ifdef WL_PROXDETECT
	home_chanspec = wlc_get_home_chanspec(cfg);
#endif

	appvt = (wlc_ap_info_pvt_t *)wlc->ap;
	ap_cfg = AP_BSSCFG_CUBBY(appvt, cfg);
	ASSERT(ap_cfg != NULL);

	WL_INFORM(("wl%d.%d: %s: chanspec %s, type 0x%04x\n",
		wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__,
		wf_chspec_ntoa(cb_chanspec, chanbuf), type));

#ifdef WL_MODESW
	/* For 4366, Enable Operating Mode when 160Mhz capable */
	if (WL_BW_CAP_160MHZ(wlc->band->bw_cap) && WLC_PHY_160_HALF_NSS(wlc)) {
		cfg->oper_mode_enabled = TRUE;
	}
	WL_MODE_SWITCH(("wl%d: %s oper_mode 0x%x op_mode_enabled %d chanspec 0x%x"
			" type 0x%04x target_bsschspec 0x%x phychspec 0x%x\n",
			wlc->pub->unit, __FUNCTION__, cfg->oper_mode, cfg->oper_mode_enabled,
			cb_chanspec, type, cfg->target_bss->chanspec,
			phy_utils_get_chanspec(WLC_PI(wlc))));
#endif /* WL_MODESW */

#ifdef WLMCHAN
	if (MCHAN_ENAB(wlc->pub) && MCHAN_ACTIVE(wlc->pub)) {
		wlc_mchan_msch_clbk(handler_ctxt, cb_info);
	}
#endif /* WLMCHN */

	/* event handling fuction start */

	/* Just before switching to the new requested channel,
	 * unless in edcrs_eu, mark the current chanspec as passive.
	 * MSCH_CT_PRE_ONCHAN to be used in callback before switching
	 * to the requested channel. Phy chanspec is still the older chanspec.
	 */
	if (type & MSCH_CT_PRE_ONCHAN) {
		if (WL11H_ENAB(wlc) &&
				!wlc->is_edcrs_eu &&
				wlc_radar_chanspec(wlc->cmi, cb_chanspec)) {
			wlc_set_quiet_chanspec_exclude(wlc->cmi, cb_chanspec,
					cfg->target_bss->chanspec);
		}
	}

	if (type & MSCH_CT_REQ_START) {
		ap_cfg->msch_state = AP_SCHED_PRE_INIT;
#ifdef PHYCAL_CACHING
		phy_chanmgr_create_ctx(WLC_PI(wlc),
			cb_chanspec);
#endif
		if (!wlc_radar_chanspec(wlc->cmi, cb_chanspec) &&
				PHYMODE(wlc) != PHYMODE_BGDFS &&
#ifdef WL_AIR_IQ
				!wlc_airiq_scan_in_progress(wlc) &&
#endif /* WL_AIR_IQ */
#if defined(WLDFS) && defined(BGDFS)
				!wlc_dfs_scan_in_progress(wlc->dfs) &&
#endif /* WLDFS && BGDFS */
				TRUE) {
			wlc_full_phy_cal(wlc, cfg, PHY_PERICAL_UP_BSS);
		}
	}

	if (type & MSCH_CT_ON_CHAN) {
#ifdef WL_PROXDETECT
		if (ap_cfg->msch_state == AP_SCHED_OFF_CHAN_SKIP) {
			/* never left the channel, so do nothing */
			ap_cfg->msch_state = AP_SCHED_ON_CHAN;
			WL_INFORM(("wl%d: %s: never left the channel, skip off-chan processing\n",
				wlc->pub->unit, __FUNCTION__));
			return BCME_OK;
		}
#endif /* WL_PROXDETECT */

		/* first thing to do */
		wlc_txqueue_start(wlc, cfg, cb_chanspec);

		/* post initialization for the first call */
		if (ap_cfg->msch_state == AP_SCHED_PRE_INIT) {
			wlc_ap_upd_onchan(wlc->ap, cfg);
		}
#ifdef RADAR
		if (!SCAN_IN_PROGRESS(wlc->scan)) {
			if (RADAR_ENAB(wlc->pub) && WL11H_AP_ENAB(wlc) && AP_ACTIVE(wlc) &&
					wlc_radar_chanspec(wlc->cmi, cb_chanspec)) {
				wlc_set_dfs_cacstate(wlc->dfs, ON, cfg);
			} else {
				wlc_set_dfs_cacstate(wlc->dfs, OFF, cfg);
			}
		}
#endif /* RADAR */
		if (!SCAN_IN_PROGRESS(wlc->scan) && appvt->edcrs_txbcn_inact_chspec !=
				cb_chanspec) {
			appvt->edcrs_hi_on_ch_count = 0;
			appvt->edcrs_hi_sim_secs = 0;
		}
		wlc_ap_get_cca(wlc, &appvt->cca);
		ap_cfg->msch_state = AP_SCHED_ON_CHAN;
#ifdef WL_DTS
		if (DTS_ENAB(wlc->pub)) {
			wlc_set_cfg_tx_stop_time(wlc, cfg,
				msch_calc_slot_duration(wlc->msch_info, cb_info));
		}
#endif /* WL_DTS */
		wlc_ap_return_home_channel(cfg);

		/* If we are switching back to radar home_chanspec
		 * because:
		 * 1. STA scans (normal/Join/Roam) aborted with
		 * atleast one local 11H AP in radar channel,
		 * 2. Scan is not join/roam.
		 * turn radar_detect ON.
		 * NOTE: For Join/Roam radar_detect ON is done
		 * at later point in wlc_roam_complete() or
		 * wlc_set_ssid_complete(), when STA succesfully
		 * associates to upstream AP.
		 */
		if (WL11H_AP_ENAB(wlc) && WLC_APSTA_ON_RADAR_CHANNEL(wlc) &&
			cfg->disable_ap_up &&
			wlc_radar_chanspec(wlc->cmi, cb_chanspec)) {
			WL_REGULATORY(("wl%d.%d: %s scan completed, back"
				"to home channel dfs ON cb_chanspec 0x%x\n",
				wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__, cb_chanspec));
			wlc_set_dfs_cacstate(wlc->dfs, ON, cfg);
		}
#ifdef  WL_MODESW
		/* callback to modesw */
		WL_MODE_SWITCH(("wl%d.%d: %s: callback to modesw\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__));
		if (!SCAN_IN_PROGRESS(wlc->scan)) {
			wlc_modesw_ap_bwchange_notif(cfg);
			if (RATELINKMEM_ENAB(wlc->pub)) {
				wlc_ratelinkmem_update_link_entry_all(wlc, NULL, FALSE,
					TRUE /* clr_txbf_stats=1 in mreq */);
			}
		}
#endif /* WL_MODESW */
	}

	if (type & MSCH_CT_OFF_CHAN) {
#ifdef WL_PROXDETECT
		if (cb_chanspec == home_chanspec && PROXD_ENAB(wlc->pub) &&
		    wlc_ftm_num_sessions_inprog(wlc_ftm_get_handle(wlc))) {
			/* channel is not changing, skip off channel prep */
			ap_cfg->msch_state = AP_SCHED_OFF_CHAN_SKIP;
		}
		else
#endif /* WL_PROXDETECT */
		{
			wlc_ap_prepare_off_channel(wlc, cfg);
		}
	}

	if (type & MSCH_CT_OFF_CHAN_DONE) {
		/* if channel is not changing, skip off channel prep */
		if (ap_cfg->msch_state != AP_SCHED_OFF_CHAN_SKIP) {
			wlc_txqueue_end(wlc, cfg);
			wlc_ap_off_channel_done(wlc, cfg);
			ap_cfg->msch_state = AP_SCHED_OFF_CHAN;
		}
	}

	if (type & MSCH_CT_REQ_END) {
		/* The msch hdl is no more valid */
		WL_INFORM(("wl%d.%d: %s: The msch hdl is no more valid\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__));
		ap_cfg->msch_req_hdl = NULL;
	}

	/* NOTE: assuming that there is no multiple ch/slot control.
	 * SoftAP mode, for example, only can be activated stand-alone
	 * SLOT_SKIP should not happen in this reason.
	 */
	if (type & MSCH_CT_SLOT_START) {
	}
	if (type & MSCH_CT_SLOT_END) {
		/* TBD : pre-tbtt handling */
	}
	if (type & MSCH_CT_SLOT_SKIP) {
		/* Do Nothing for STA */
	}

	return BCME_OK;
} /* wlc_ap_msch_clbk */

/* prepare to leave home channel */
static void
wlc_ap_prepare_off_channel(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
	WL_INFORM(("wl%d.%d: %s\n", wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__));

	/* Must disable AP beacons and probe responses first before going away from home channel */
	wlc_ap_mute(wlc, TRUE, cfg, -1);

#ifdef PROP_TXSTATUS
	if (PROP_TXSTATUS_ENAB(wlc->pub)) {
		wlc_wlfc_mchan_interface_state_update(wlc, cfg,
			WLFC_CTL_TYPE_INTERFACE_CLOSE, FALSE);
	}
#endif /* PROP_TXSTATUS */
}

static void
wlc_ap_return_home_channel(wlc_bsscfg_t *cfg)
{
	wlc_info_t *wlc = cfg->wlc;
#ifdef WLMCHAN
	int btc_flags = wlc_bmac_btc_flags_get(wlc->hw);
	uint16 protections = 0;
	uint16 active = 0;
#endif /* WLMCHAN */
	WL_INFORM(("wl%d.%d: %s\n", wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__));

#ifdef PROP_TXSTATUS
	if (PROP_TXSTATUS_ENAB(wlc->pub)) {
		/* Open the active interface so that host
		 * start sending pkts to dongle
		 */
		wlc_wlfc_mchan_interface_state_update(wlc,
			cfg, WLFC_CTL_TYPE_INTERFACE_OPEN,
			!MCHAN_ACTIVE(wlc->pub));
	}
#endif /* PROP_TXSTATUS */

	wlc_suspend_mac_and_wait(wlc);

	/* validate the phytxctl for the beacon before turning it on */
	wlc_validate_bcn_phytxctl(wlc, NULL);

	/* Unmute - AP when in SoftAP/GO channel */
	wlc_ap_mute(wlc, FALSE, cfg, -1);

#ifdef PROP_TXSTATUS
	if (PROP_TXSTATUS_ENAB(wlc->pub)) {
		/* send_bar on interface being opened */
		if (AMPDU_ENAB(wlc->pub)) {
			struct scb_iter scbiter;
			struct scb *scb = NULL;

			FOREACH_BSS_SCB(wlc->scbstate, &scbiter, cfg, scb) {
				wlc_ampdu_send_bar_cfg(wlc->ampdu_tx, scb);
			}
		}
		wlfc_sendup_ctl_info_now(wlc->wlfc);
	}
#endif /* PROP_TXSTATUS */

#ifdef WLMCHAN
	/*
	 * Setup BTCX protection mode according to the BSS that is
	 * being switched in.
	 */
	if (btc_flags & WL_BTC_FLAG_ACTIVE_PROT)
		active = MHF3_BTCX_ACTIVE_PROT;

	if (cfg->up) {
		protections = active;
	}
#endif /* WLMCHAN */

#ifdef WLMCNX
	if (MCNX_ENAB(wlc->pub)) {
		int bss_idx;
		/* Set the current BSS's Index in SHM (bits 10:8 of M_P2P_GO_IND_BMP
		   contain the current BSS Index
		 */
		bss_idx = wlc_mcnx_BSS_idx(wlc->mcnx, cfg);
		wlc_mcnx_shm_bss_idx_set(wlc->mcnx, bss_idx);
	}
#endif /* WLMCNX */
#ifdef WLMCHAN
	if (MCHAN_ACTIVE(wlc->pub))
		wlc_mhf(wlc, MHF3, MHF3_BTCX_ACTIVE_PROT | MHF3_BTCX_PS_PROTECT,
			protections, WLC_BAND_2G);
	else
#endif /* WLMCHAN */
		wlc_btc_set_ps_protection(wlc, cfg); /* enable */

	wlc_enable_mac(wlc);

	wlc_update_phy_tdcs(wlc);
} /* wlc_ap_return_home_channel */

/* prepare to leave home channel */
static void
wlc_ap_off_channel_done(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
	WL_INFORM(("wl%d.%d: %s\n", wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__));

#ifdef PROP_TXSTATUS
	if (PROP_TXSTATUS_ENAB(wlc->pub)) {
		wlc_wlfc_mchan_interface_state_update(wlc, cfg,
			WLFC_CTL_TYPE_INTERFACE_CLOSE, FALSE);
		wlc_wlfc_flush_pkts_to_host(wlc, cfg);
		wlfc_sendup_ctl_info_now(wlc->wlfc);
	}
#endif /* PROP_TXSTATUS */

	/* If we are switching away from radar home_chanspec
	 * because STA scans (normal/Join/Roam) with
	 * atleast one local 11H AP in radar channel,
	 * turn of radar_detect.
	 * NOTE: Implied that upstream AP assures this radar
	 * channel is clear.
	 */
	if (WL11H_AP_ENAB(wlc) && WLC_APSTA_ON_RADAR_CHANNEL(wlc) &&
		wlc_radar_chanspec(wlc->cmi, wlc->home_chanspec)) {
		WL_REGULATORY(("wl%d.%d: %s Moving from home channel dfs OFF\n",
			wlc->pub->unit, WLC_BSSCFG_IDX(cfg), __FUNCTION__));
		wlc_set_dfs_cacstate(wlc->dfs, OFF, cfg);
	}
#ifdef WL_DTS
	if (DTS_ENAB(wlc->pub)) {
		wlc_set_cfg_tx_stop_time(wlc, cfg, -1);
	}
#endif /* WL_DTS */
	if (WLC_BW_SWITCH_TDCS_EN(wlc)) {
		phy_chanmgr_tdcs_enable_160m(wlc->pi, FALSE);
	}
}

uint32
wlc_ap_getdtim_count(wlc_info_t *wlc, wlc_bsscfg_t *cfg)
{
	uint32 dtim_cnt;
#ifdef WLMCNX
	if (P2P_ENAB(wlc->pub)) {
		int idx;
		idx = wlc_mcnx_BSS_idx(wlc->mcnx, cfg);
		dtim_cnt = wlc_mcnx_read_shm(wlc->mcnx, M_P2P_BSS_DTIM_CNT(wlc, idx));
	}
	else
#endif
	{
		wlc_bmac_copyfrom_objmem(wlc->hw, (S_DOT11_DTIMCOUNT << 2),
			&dtim_cnt, sizeof(dtim_cnt), OBJADDR_SCR_SEL);
	}
	return dtim_cnt;
}

/* This function returns the no of
* infra AP's running in the wlc.
*/
uint8
wlc_ap_count(wlc_ap_info_t *ap, bool include_p2p)
{
	wlc_ap_info_pvt_t* appvt = (wlc_ap_info_pvt_t*) ap;
	wlc_info_t *wlc = appvt->wlc;
	uint8 idx, infra_ap_count = 0, p2p_go_count = 0;
	wlc_bsscfg_t *cfg;
	FOREACH_UP_AP(wlc, idx, cfg) {
		if (BSS_P2P_ENAB(wlc, cfg))
			p2p_go_count++;
		else
			infra_ap_count++;
	}

	return (include_p2p ? (p2p_go_count + infra_ap_count) : infra_ap_count);
}

bool
wlc_ap_on_chan(wlc_ap_info_t *ap, wlc_bsscfg_t *bsscfg)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_bsscfg_cubby_t *ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);

	ASSERT(ap_cfg);

	return (ap_cfg->msch_state == AP_SCHED_ON_CHAN);
}

/*
 * Function:	wlc_ap_iface_create
 *
 * Purpose:	Function to create ap interface through interface create command.
 *
 * Parameters:
 * module_ctx	: Module context
 * if_buf	: interface create buffer
 * wl_info	: out parameter of created interface
 * err		: pointer to store the error status
 *
 * Returns:	cfg pointer - If success
 *		NULL on failure
 */
static wlc_bsscfg_t*
wlc_ap_iface_create(void *if_module_ctx, wl_interface_create_t *if_buf,
	wl_interface_info_t *wl_info, int32 *err)
{
	wlc_bsscfg_t *cfg;
	wlc_ap_info_pvt_t *appvt = if_module_ctx;
	wlc_bsscfg_type_t type = {BSSCFG_TYPE_GENERIC, BSSCFG_GENERIC_AP};

	ASSERT(appvt->wlc);

	cfg = wlc_iface_create_generic_bsscfg(appvt->wlc, if_buf, &type, err);
	if (cfg != NULL) {
		wlc_set_iface_info(cfg, wl_info, if_buf);
	}
	return (cfg);
}

/*
 * Function:	wlc_ap_iface_remove
 *
 * Purpose:	Function to remove ap interface(s) through interface_remove IOVAR.
 *
 * Input Parameters:
 *	wlc	: Pointer to wlc
 *	bsscfg	: Pointer to bsscfg corresponding to the interface
 *
 * Returns:	BCME_OK - If success
 *		Respective error code on failures.
 */
static int32
wlc_ap_iface_remove(wlc_info_t *wlc, wlc_if_t *wlcif)
{
	int32 ret;
	wlc_bsscfg_t *bsscfg = wlc_bsscfg_find_by_wlcif(wlc, wlcif);

	if (bsscfg == NULL) {
		ret = BCME_NOTFOUND;
		goto done;
	}
	if (bsscfg->subtype != BSSCFG_GENERIC_AP) {
		ret = BCME_NOTAP;
	} else {
		if (bsscfg->enable) {
			wlc_bsscfg_disable(wlc, bsscfg);
		}
		wlc_bsscfg_free(wlc, bsscfg);
		ret = BCME_OK;
	}
done:
	return (ret);
}

void
wlc_ap_update_bw(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
	chanspec_t old_chanspec, chanspec_t chanspec)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)wlc->ap;
	ap_bsscfg_cubby_t *ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);
	ASSERT(ap_cfg != NULL);
	if (ap_cfg->msch_req_hdl == NULL) {
		return;
	}
	msch_update_bw(wlc->msch_info, ap_cfg->msch_req_hdl, old_chanspec, chanspec);
}

static void wlc_ap_auth_tx_complete(wlc_info_t *wlc, uint txstatus, void *arg)
{
	struct scb *scb = (struct scb *)arg;

	if (SCB_AUTHENTICATED(scb) && (txstatus & TX_STATUS_ACK_RCV)) {
		/* Check if peer was blacklisted here.
		* If Peer STA is blacklisted clear states and mark the scb for deletion.
		*/
	}
	WL_ASSOC(("wl%d %s: "MACF" AUTH txstatus 0x%04x\n", wlc->pub->unit, __FUNCTION__,
		ETHER_TO_MACF(scb->ea), txstatus));

#ifdef BCM_CSIMON
	if (wlc_csimon_enabled(wlc, scb)) {
		/* association timestamp as a reference - using TSF register */
		wlc_csimon_assocts_set(wlc, scb);

#ifdef CSIMON_PER_STA_TIMER
		/* Start CSI timer */
		wlc_csimon_timer_add(wlc, scb);
		CSIMON_DEBUG("wl%d: started CSI timer for SCB DA "MACF" \n",
		             wlc->pub->unit, ETHER_TO_MACF(scb->ea));
#endif /* CSIMON_PER_STA_TIMER */
	}
#endif /* BCM_CSIMON */

}

static void
wlc_ap_authresp_deauth(wlc_info_t *wlc, struct scb *scb)
{
	/* Notify HOST with a Deauth, this will ensure
	 * clearing any flow-rings (PCIe case) created during previous
	 * association of the SCB
	 */
	if (SCB_AUTHENTICATED(scb)) {
		/* If SCB is in authenticated state then clear the
		 * ASSOCIATED, AUTHENTICATED and
		 * AUTHORIZED bits in the flag before freeing the SCB
		 */
		wlc_deauth_sendcomplete_clean(wlc, scb);
		wlc_scb_clearstatebit(wlc, scb,
			ASSOCIATED | AUTHENTICATED | AUTHORIZED);
		wlc_deauth_complete(wlc, SCB_BSSCFG(scb),
			WLC_E_STATUS_SUCCESS, &scb->ea,
			DOT11_RC_UNSPECIFIED, 0);
	}
}

#ifdef RADIONOA_PWRSAVE
void
wlc_radio_pwrsave_update_params(wlc_ap_info_t *ap, bool enable,
	uint32 pps, uint32 quiet, bool assoc_check)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	wlc_info_t *wlc = appvt->wlc;

	/* PPS */
	appvt->radio_pwrsave.pwrsave.pps_threshold = pps;

	/* Quiet time */
	appvt->radio_pwrsave.pwrsave.quiet_time = quiet;

	/* Assoc check */
	appvt->radio_pwrsave.pwrsave.stas_assoc_check = assoc_check;
	if (assoc_check && wlc_radio_pwrsave_in_power_save(wlc->ap) &&
		wlc_ap_stas_associated(wlc->ap)) {
		wlc_radio_pwrsave_exit_mode(wlc->ap);
		WL_INFORM(("Going out of power save as there are associated STASs!\n"));
	}

	/* Enable */
	ap->radio_pwrsave_enable = appvt->radio_pwrsave.pwrsave.power_save_check = enable;
	wlc_reset_radio_pwrsave_mode(ap);
}
#endif /* RADIONOA_PWRSAVE */

#ifdef WL_GLOBAL_RCLASS
bool
wlc_ap_scb_support_global_rclass(wlc_info_t *wlc, struct scb *scb)
{
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_scb_cubby_t *ap_scb = NULL;

	if (!appvt) {
		return FALSE;
	}

	ap_scb = AP_SCB_CUBBY(appvt, scb);
	if (!ap_scb) {
		return FALSE;
	}

	return SCB_SUPPORTS_GLOBAL_RCLASS(ap_scb);
}
#endif /* WL_GLOBAL_RCLASS */

uint8
wlc_ap_scb_supported_bands(wlc_info_t *wlc, struct scb *scb)
{
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_scb_cubby_t *ap_scb = NULL;

	if (!appvt) {
		return 0;
	}

	ap_scb = AP_SCB_CUBBY(appvt, scb);
	if (!ap_scb) {
		return 0;
	}

	return ap_scb->bands;
}

static int
wlc_assoc_parse_regclass_ie(void *ctx, wlc_iem_parse_data_t *data)
{
	wlc_info_t *wlc = ctx;
	wlc_iem_ft_pparm_t *ftpparm = data->pparm->ft;
	struct scb *scb =  ftpparm->assocreq.scb;
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_scb_cubby_t *ap_scb = NULL;
	uint8 *cp = NULL;

	cp = data->ie;
	ap_scb = AP_SCB_CUBBY(appvt, scb);

	switch (data->ft) {
		case FC_ASSOC_REQ:
		case FC_REASSOC_REQ:
			{
				bcm_tlv_t* ie;
				uint8 i, ret = 0;
				rcvec_t rcvec;

				ie = bcm_find_tlv(cp, data->ie_len, DOT11_MNG_REGCLASS_ID);
				if ((!ie) || (ie->len < 2)) {
					return BCME_OK;
				}
#ifdef BCMDBG
				prhex(" supported class data:\n", ie->data, ie->len);
#endif /* BCMDBG */
				bzero(&rcvec, sizeof(rcvec));

				/* skip first octet indicating current operating class */
				cp = &ie->data[1];
				for (i = 1; i < ie->len; i++) {
					/*
					 * operating classes field ends when a 130-delimiter
					 * field zero-delimiter field is observed. Remaining
					 * info is specific for 80+80 so we can ignore it.
					 */
					if (!(*cp) || *cp == 130)
						break;

					/* ignore invalid/unsupported operating classes */
					if (*cp > MAXREGCLASS)
						continue;

					setbit(rcvec.vec, *cp);
					cp++;
				}
#ifdef WL_GLOBAL_RCLASS
				/* Check global support from list of operating class values,
				 * these list of supported operating class holds operating band
				 * capability. Update scb band capability.
				 */
				ret = wlc_sta_supports_global_rclass(rcvec.vec);
				if (ret) {
					ap_scb->flags |= SCB_SUPPORT_GLOBAL_RCLASS;
#if defined(WL_MBO) && defined(MBO_AP)
					if (BSS_MBO_ENAB(wlc, (scb->bsscfg))) {
						wlc_mbo_update_scb_band_cap(wlc, scb, &ret);
					}
#endif	/* WL_MBO && MBO_AP */
				} else
#endif /* WL_GLOBAL_RCLASS */
				{
					bcmwifi_rclass_type_t rc_type;
					rc_type = bcmwifi_rclass_get_rclasstype_from_cntrycode(
							wlc_channel_country_abbrev(wlc->cmi));
					ret = wlc_sta_supported_bands_from_rclass(rc_type,
							rcvec.vec);
				}
				ap_scb->bands = ret;
			}
		break;
	}

	return BCME_OK;
} /* wlc_assoc_parse_regclass_ie */

static void
wlc_ap_scb_state_upd_cb(void *ctx, scb_state_upd_data_t *notif_data)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	struct scb *scb;
	uint8 oldstate;
	uint8 assoc_state_change = 0x00;
	uint8 scb_assoced;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *) (wlc->ap);
#ifdef MBO_AP
	int i;
	wlc_bsscfg_t *cfg;
	uint8 assoc_disallowed = 0;
#endif /* MBO_AP */

	ASSERT(notif_data != NULL);

	scb = notif_data->scb;
	ASSERT(scb != NULL);

	if (!BSSCFG_AP(scb->bsscfg)) {
		return;
	}

	BCM_REFERENCE(appvt);

	oldstate = notif_data->oldstate;
	scb_assoced = SCB_ASSOCIATED(scb);

	assoc_state_change = ((oldstate & ASSOCIATED) ^ scb_assoced);

	if (!assoc_state_change) {
		WL_INFORM(("wl%d: No change in assoc state of scb["MACF"], return\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea)));

		return;
	}

	/* Bookkeeping of all 160Mhz capable STA's is done.
	 * 160-80Mhz Bandwidth switch is triggered.
	 */
	wlc_ap_160mhz_upd_bw_check(wlc, scb, scb_assoced);

#ifdef WL_GLOBAL_RCLASS
	wlc_ap_scb_no_gbl_rclass_upd(wlc, scb, scb_assoced, oldstate);
#endif /* WL_GLOBAL_RCLASS */

#ifdef MBO_AP
	/* Once we reach maxassoc limit and set MBO_ASSOC_DISALLOWED_REASON_MAX_STA_LIMIT_REACHED
	 * Now we are releasing the scb. Now while releasing scb no. maxassoc, set the mbo
	 * attribute assoc disallowed to 0 for each ap bss.
	*/
	if (wlc_ap_stas_associated(wlc->ap) < appvt->maxassoc) {
		/* Update mbo attr to assoc allowed */
		FOREACH_UP_AP(wlc, i, cfg) {
			assoc_disallowed = wlc_mbo_get_attr_assoc_disallowed(cfg);
			if ((wlc_bss_assocscb_getcnt(wlc, cfg) < cfg->maxassoc) &&
			(assoc_disallowed == MBO_ASSOC_DISALLOWED_REASON_MAX_STA_LIMIT_REACHED)) {
				wlc_mbo_update_attr_assoc_disallowed(cfg, 0);
			}
		}
	}
#endif /* MBO_AP */
}

#ifdef WL_GLOBAL_RCLASS
static void
wlc_ap_scb_no_gbl_rclass_upd(wlc_info_t *wlc, struct scb *scb,
	uint8 scb_assoced, uint8 oldstate)
{
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	ap_scb_cubby_t *ap_scb = NULL;

	ap_scb = AP_SCB_CUBBY(appvt, scb);

	if (ap_scb == NULL) {
		WL_INFORM(("wl%d:ap cubby of scb ["MACF"] NULL, Dump:state[%x] oldstate[%x]"
			"bsscfg->scb_without_gbl_rclass[%d]\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea),
			scb->state, oldstate, scb->bsscfg->scb_without_gbl_rclass));
		return;
	}

	if (CHSPEC_BAND(scb->bsscfg->current_bss->chanspec) == WL_CHANSPEC_BAND_6G) {
		if (SCB_SUPPORTS_GLOBAL_RCLASS(ap_scb)) {
			WL_INFORM(("wl%d: scb["MACF"] 6GHz sta supports mandatory gbl rclass, "
				"total sta count not supporting global rclass[%d] must be Zero\n",
				wlc->pub->unit, ETHER_TO_MACF(scb->ea),
				scb->bsscfg->scb_without_gbl_rclass));
		} else {
		/* Keep using Global Operating Classes for 6GHz even if associated STA did not
		 * include Supported Global Operating Classes in its association request.
		 */
			WL_ERROR(("wl%d: scb["MACF"] 6GHz sta has no mandatory gbl rclass support, "
				"total sta count not supporting global rclass[%d] must be Zero\n",
				wlc->pub->unit, ETHER_TO_MACF(scb->ea),
				scb->bsscfg->scb_without_gbl_rclass));
		}
		ASSERT(!scb->bsscfg->scb_without_gbl_rclass);
		return;
	}

	if (SCB_SUPPORTS_GLOBAL_RCLASS(ap_scb)) {
		WL_INFORM(("wl%d: scb["MACF"] support gbl rclass [%d] no need to process,"
			"total sta count not support global rclass[%d] return\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea),
			SCB_SUPPORTS_GLOBAL_RCLASS(ap_scb),
			scb->bsscfg->scb_without_gbl_rclass));
		return;
	}

	WL_INFORM(("wl%d: scb["MACF"] state[%x] oldstate[%x] support_gbl_rclass[%d],"
		"bsscfg->scb_without_gbl_rclass[%d]\n", wlc->pub->unit,
		ETHER_TO_MACF(scb->ea), scb->state,
		oldstate, SCB_SUPPORTS_GLOBAL_RCLASS(ap_scb),
		scb->bsscfg->scb_without_gbl_rclass));

	if (scb_assoced) {
		scb->bsscfg->scb_without_gbl_rclass++;
		WL_INFORM((" wl%d: scb["MACF"] assoc, bsscfg->scb_without_gbl_rclass[%d]\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea),
			scb->bsscfg->scb_without_gbl_rclass));
	} else {
		if (scb->bsscfg->scb_without_gbl_rclass) {
			scb->bsscfg->scb_without_gbl_rclass--;
		} else {
			WL_INFORM(("Unlikely situation:\n"
				"scb["MACF"] not support_gbl_rclass[%d], but "
				" bsscfg->scb_without_gbl_rclass[%d] is Zero\n",
				ETHER_TO_MACF(scb->ea),
				SCB_SUPPORTS_GLOBAL_RCLASS(ap_scb),
				scb->bsscfg->scb_without_gbl_rclass));
			ASSERT(0);
		}
		WL_INFORM(("wl%d: scb["MACF"] disassoc ,bsscfg->scb_without_gbl_rclass[%d]\n",
			wlc->pub->unit, ETHER_TO_MACF(scb->ea),
			scb->bsscfg->scb_without_gbl_rclass));
	}

	wlc_bsscfg_update_rclass(wlc);
} /* wlc_ap_scb_no_gbl_rclass_upd */
#endif /* WL_GLOBAL_RCLASS */

#ifdef WL_MBSSID
uint16
wlc_ap_get_maxbss_count(wlc_ap_info_t *ap)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	return appvt->max_bss_count;
}

bool
wlc_ap_get_block_mbssid(wlc_info_t *wlc)
{
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;
	return appvt->block_nonmbssid;
}
#endif /* WL_MBSSID */

#if (defined(WLTEST) && !defined(WLTEST_DISABLED)) || defined(WLPKTENG)
/* addsta of pkteng_cmd will override the AID of scb, cause AID leak */
void
wlc_test_clear_aid(wlc_info_t *wlc, struct scb *scb)
{
	wlc_ap_info_t *ap = wlc->ap;
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)ap;

	ASSERT(scb != NULL);
	wlc_ap_aid_free(appvt, scb);
}
#endif
static uint8 *
wlc_write_local_maximum_transmit_pwr(uint8 min_pwr, uint8* cp, int buflen)
{
	BCM_REFERENCE(buflen);

	*cp = min_pwr;
	cp += sizeof(uint8);

	return cp;
}

static uint
wlc_calc_pwr_env_ie_len(void *ctx, wlc_iem_calc_data_t *data)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	wlc_bsscfg_t *cfg = data->cfg;
	uint8 buf[257];

	/* TODO: needs a better way to calculate the IE length */
	if (!data->cbparm->vht && !data->cbparm->he)
		return BCME_OK;

	if (BAND_2G(wlc->band->bandtype))
		return BCME_OK;

	if (!cfg->BSS)
		return BCME_OK;

	if (CHSPEC_IS6G(cfg->current_bss->chanspec)) {
		return (uint)(wlc_write_transmit_power_envelope_ie_6g(wlc,
				cfg->current_bss->chanspec,
				buf, sizeof(buf)) - buf);
	} else {
		return (uint)(wlc_write_transmit_power_envelope_ie(wlc,
				cfg->current_bss->chanspec,
				buf, sizeof(buf)) - buf);
	}
}

/** VHT/HE Power envelope */
static int
wlc_write_pwr_env_ie(void *ctx, wlc_iem_build_data_t *data)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	wlc_bsscfg_t *cfg = data->cfg;

	if (!data->cbparm->vht && !data->cbparm->he)
		return BCME_OK;

	if (BAND_2G(wlc->band->bandtype))
		return BCME_OK;

	if (!cfg->BSS)
		return BCME_OK;

	if (CHSPEC_IS6G(cfg->current_bss->chanspec)) {
		wlc_write_transmit_power_envelope_ie_6g(wlc, cfg->current_bss->chanspec,
				data->buf, data->buf_len);
	} else {
		wlc_write_transmit_power_envelope_ie(wlc, cfg->current_bss->chanspec,
				data->buf, data->buf_len);
	}
	return BCME_OK;
}
static uint8 *
wlc_write_transmit_power_envelope_ie(wlc_info_t *wlc,
	chanspec_t chspec, uint8 *cp, int buflen)
{
	int max_pwr = wlc->tpe_eirp;
	dot11_vht_transmit_power_envelope_ie_t *vht_transmit_power_ie;

	vht_transmit_power_ie = (dot11_vht_transmit_power_envelope_ie_t *)cp;
	vht_transmit_power_ie->id = DOT11_MNG_VHT_TRANSMIT_POWER_ENVELOPE_ID;

	cp += sizeof(dot11_vht_transmit_power_envelope_ie_t);

	/* max_pwr_eirp should be db in the units  of 0.5 db */
	max_pwr /= WLC_TPE_PWR_FACTOR;

	vht_transmit_power_ie->local_max_transmit_power_20 = max_pwr;

	if (CHSPEC_IS20(chspec)) {
		vht_transmit_power_ie->transmit_power_info = 0;
	} else if (CHSPEC_IS40(chspec)) {
		vht_transmit_power_ie->transmit_power_info = 1;
		cp = wlc_write_local_maximum_transmit_pwr(max_pwr, cp, buflen);
	} else if (CHSPEC_IS80(chspec)) {
		vht_transmit_power_ie->transmit_power_info = 2;
		cp = wlc_write_local_maximum_transmit_pwr(max_pwr, cp, buflen);
		cp = wlc_write_local_maximum_transmit_pwr(max_pwr, cp, buflen);
	} else if (CHSPEC_IS8080(chspec) || CHSPEC_IS160(chspec)) {
		vht_transmit_power_ie->transmit_power_info = 3;
		cp = wlc_write_local_maximum_transmit_pwr(max_pwr, cp, buflen);
		cp = wlc_write_local_maximum_transmit_pwr(max_pwr, cp, buflen);
		cp = wlc_write_local_maximum_transmit_pwr(max_pwr, cp, buflen);
	} else {
		WL_ERROR(("%s: wrong chanspec 0x%04x\n", __FUNCTION__, chspec));
		ASSERT(FALSE);
	}

	vht_transmit_power_ie->len =
	        (sizeof(dot11_vht_transmit_power_envelope_ie_t) - TLV_HDR_LEN) +
	        vht_transmit_power_ie->transmit_power_info;

	return cp;
}

void
wlc_check_override_clm(wlc_info_t *wlc, wlc_txpwr_eirp_t *eirp, wlc_txpwr_eirp_psd_t *eirp_psd)
{
	wlc_txpwr_eirp_t     *override_eirp;
	wlc_txpwr_eirp_psd_t  *override_eirp_psd;
	int i;
	override_eirp = &wlc->pub->_override_clm_eirp;
	override_eirp_psd = &wlc->pub->_override_clm_eirp_psd;

	if (override_eirp->txpwr_20 != WLC_OVERRIDE_CLM_DISABLE) {
		eirp->txpwr_20 = (uint8) (override_eirp->txpwr_20 & 0xFFu);
	}
	if (override_eirp->txpwr_40 != WLC_OVERRIDE_CLM_DISABLE) {
		eirp->txpwr_40 = (uint8) (override_eirp->txpwr_40 & 0xFFu);
	}
	if (override_eirp->txpwr_80 != WLC_OVERRIDE_CLM_DISABLE) {
		eirp->txpwr_80 = (uint8) (override_eirp->txpwr_80 & 0xFFu);
	}
	if (override_eirp->txpwr_160 != WLC_OVERRIDE_CLM_DISABLE) {
		eirp->txpwr_160 = (uint8) (override_eirp->txpwr_160 & 0xFFu);
	}
	for (i = 0; i < 8; i ++) {
		if (override_eirp_psd->txpwr_psd[i] != WLC_OVERRIDE_CLM_DISABLE) {
			eirp_psd->txpwr_psd[i] = override_eirp_psd->txpwr_psd[i];
		}
	}
}

uint8 *
wlc_write_transmit_power_envelope_ie_6g(wlc_info_t *wlc,
	chanspec_t chspec, uint8 *cp, int buflen)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)(wlc->ap);
	wlc_txpwr_eirp_t      client_eirp;
	wlc_txpwr_eirp_psd_t  client_eirp_psd;
	wlc_txpwr_eirp_psd_t  subordinate_eirp_psd;
	clm_tpe_regulatory_limits_t limits_client;
	clm_tpe_regulatory_limits_t limits_subord;
	clm_country_locales_t locales;
	wlc_cm_info_t *wlc_cmi = wlc->cmi;
	clm_country_t country;
	clm_result_t ret;
	int i = 0;
	clm_device_category_t power_category = 0xFF;
	int8 reg_info_field = BCME_ERROR;
	uint16 sp_bmp = 0, lpi_bmp = 0;
	int8 *lpi_eirp = appvt->reg_info.lpi_eirp;
	int8 *sp_eirp = appvt->reg_info.sp_eirp;
	int8 *lpi_psd = appvt->reg_info.lpi_psd;
	int8 *sp_psd = appvt->reg_info.sp_psd;
	int8 lpi_sub_pwr_psd[TPE_PSD_COUNT];
	uint channel;
	uint last_channel;
	chanspec_t local_chanspec;
	chanspec_t band = CHSPEC_BAND(chspec);
	int8 psd_pwr = WL_RATE_DISABLED;
	int8 eirp_pwr = WL_RATE_DISABLED;

	memset(lpi_eirp, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.lpi_eirp));
	memset(lpi_psd, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.lpi_psd));
	memset(sp_eirp, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.sp_eirp));
	memset(sp_psd, (uint8)WL_RATE_DISABLED, sizeof(appvt->reg_info.sp_psd));
	memset(lpi_sub_pwr_psd, (uint8)WL_RATE_DISABLED, sizeof(lpi_sub_pwr_psd));

	country = wlc_channel_country(wlc_cmi);
	if (country == CLM_ITER_NULL) {
		ret = wlc_country_lookup_direct(wlc_channel_ccode(wlc_cmi),
			wlc_channel_regrev(wlc_cmi), &country);
		if (ret != CLM_RESULT_OK) {
			wlc_channel_setcountry(wlc_cmi, CLM_ITER_NULL);
			return cp;
		} else {
			wlc_channel_setcountry(wlc_cmi, country);
		}
	}

	ret = wlc_get_locale(country, &locales);
	if (ret != CLM_RESULT_OK) {
		return cp;
	}

	FOREACH_20_SB_EFF(chspec, channel, last_channel) {
		local_chanspec = wf_channel2chspec(channel, WL_CHANSPEC_BW_20, band);
		ret = wlc_get_6g_tpe_reg_max_power(wlc_cmi, &locales, &limits_client,
				CLM_REGULATORY_LIMIT_DEST_LOCAL,
				CLM_DEVICE_CATEGORY_SP, local_chanspec);
		if (limits_client.limit[0][0] != WL_RATE_DISABLED) {
			sp_eirp[i] = limits_client.limit[0][0] - AP_CLIENT_DIFF_DB;
		} else {
			sp_eirp[i] = WL_RATE_DISABLED;
		}
		if (limits_client.limit[0][1] != WL_RATE_DISABLED) {
			sp_psd[i] = limits_client.limit[0][1] - AP_CLIENT_DIFF_DB;
		} else {
			sp_psd[i] = WL_RATE_DISABLED;
		}
		if (sp_eirp[i] > WL_RATE_DISABLED) {
			power_category = CLM_DEVICE_CATEGORY_SP;
		}
		sp_bmp = (sp_bmp << 1) | (limits_client.limit[0][0] > WL_RATE_DISABLED);
		i ++;
	}

	i = 0;
	FOREACH_20_SB_EFF(chspec, channel, last_channel) {
		local_chanspec = wf_channel2chspec(channel, WL_CHANSPEC_BW_20, band);
		ret = wlc_get_6g_tpe_reg_max_power(wlc_cmi, &locales, &limits_client,
				CLM_REGULATORY_LIMIT_DEST_CLIENT,
				CLM_DEVICE_CATEGORY_LP, local_chanspec);
		lpi_eirp[i] = limits_client.limit[0][0];
		lpi_psd[i] = limits_client.limit[0][1];
		if (power_category == 0xFF) {
			if (lpi_eirp[i] > WL_RATE_DISABLED) {
				power_category = CLM_DEVICE_CATEGORY_LP;
			}
		}
		lpi_bmp = (lpi_bmp << 1) | (limits_client.limit[0][0] > WL_RATE_DISABLED);
		i ++;
	}
	/* Max of eirp_pwr and (sp_eirp[i] and lpi_eirp[i] of all 20 Mhz's
	 * in a bigger bw.
	 * eirp_pwr is used to publish the eirp TPE. Clients have to
	 * use min of eirp TPE and eirp PSD TPE.
	 */
	i = 0;
	FOREACH_20_SB_EFF(chspec, channel, last_channel) {
		eirp_pwr = ((eirp_pwr > sp_eirp[i] && eirp_pwr > lpi_eirp[i]) ?
				eirp_pwr : (sp_eirp[i] > lpi_eirp[i]) ?
				sp_eirp[i] : lpi_eirp[i]);
		i ++;
	}

	client_eirp.txpwr_20 = eirp_pwr * WLC_TPE_PWR_FACTOR;
	client_eirp.txpwr_40 = eirp_pwr * WLC_TPE_PWR_FACTOR;
	client_eirp.txpwr_80 = eirp_pwr * WLC_TPE_PWR_FACTOR;
	client_eirp.txpwr_160 = eirp_pwr * WLC_TPE_PWR_FACTOR;

	for (i = 0; i < TPE_PSD_COUNT; i++) { /* psd = power spectrum density */
		if ((sp_psd[i] == WL_RATE_DISABLED) && (lpi_psd[i] == WL_RATE_DISABLED)) {
			psd_pwr = WL_RATE_DISABLED;
		}
		if (sp_psd[i] > lpi_psd[i]) {
			psd_pwr = sp_psd[i];
		} else {
			psd_pwr = lpi_psd[i];
		}
		client_eirp_psd.txpwr_psd[i] = psd_pwr * WLC_TPE_PWR_FACTOR;
	}
	wlc_check_override_clm(wlc, &client_eirp, &client_eirp_psd);
	wlc_write_client_eirp(&cp, &client_eirp, TRANS_PWR_REG_EIRP,
			TRANS_PWR_CAT_DEFAULT, chspec, buflen);
	i = 0;
	if (power_category == CLM_DEVICE_CATEGORY_LP) {
		FOREACH_20_SB_EFF(chspec, channel, last_channel) {
			local_chanspec = wf_channel2chspec(channel, WL_CHANSPEC_BW_20, band);
			wlc_get_6g_tpe_reg_max_power(wlc_cmi, &locales, &limits_subord,
					CLM_REGULATORY_LIMIT_DEST_SUBORDINATE,
					CLM_DEVICE_CATEGORY_LP, local_chanspec);
			lpi_sub_pwr_psd[i] = limits_subord.limit[0][1];
			i ++;
		}

		for (i = 0; i < TPE_PSD_COUNT; i++) {
			subordinate_eirp_psd.txpwr_psd[i] =
				lpi_sub_pwr_psd[i] * WLC_TPE_PWR_FACTOR;
		}
	}

	if (power_category == CLM_DEVICE_CATEGORY_LP) {
		wlc_write_client_eirp_psd(&cp, &client_eirp_psd, TRANS_PWR_REG_EIRP_PSD,
				TRANS_PWR_CAT_DEFAULT, chspec, buflen);
		wlc_write_client_eirp_psd(&cp, &subordinate_eirp_psd,
				TRANS_PWR_REG_EIRP_PSD,
				TRANS_PWR_CAT_SUBORDINATE, chspec, buflen);
	}

	if (power_category == CLM_DEVICE_CATEGORY_SP) {
		wlc_write_client_eirp_psd(&cp, &client_eirp_psd, TRANS_PWR_REG_EIRP_PSD,
				TRANS_PWR_CAT_DEFAULT, chspec, buflen);
	}

	if (sp_bmp && lpi_bmp) {
		reg_info_field = WL_INDOOR_STANDARD_PWR_ACCESS_POINT;
	} else if (sp_bmp && !lpi_bmp) {
		reg_info_field = WL_STANDARD_PWR_ACCESS_POINT;
	} else if (!sp_bmp && lpi_bmp) {
		reg_info_field = WL_INDOOR_ACCESS_POINT;
	} else {
		WL_ERROR(("wl%d: Neither SP nor LP powers found\n", wlc->pub->unit));
	}

	appvt->reg_info.chspec = chspec;
	appvt->reg_info.sp_bmp = sp_bmp;
	appvt->reg_info.lpi_bmp = lpi_bmp;
	appvt->reg_info.reg_info_field = reg_info_field;
	wlc_ap_update_edcrs_hi_event_status(appvt);

	return cp;
} /* wlc_write_transmit_power_envelope_ie_6g */

/* Currently all the EIRP values are hard coded to the regulatory values of FCC
 * TODO:6GHZ Once these value are available from the clm, These value will be fetched
 *  from clm and updated.
 *  FCC regulatory values of LPI EIRP
 *  20 MHZ: 12 db
 *  40 MHZ: 15 db
 *  80 MHZ: 18 db
 *  160 MHZ : 21db
 */
static void
wlc_write_client_eirp(uint8 **cp, wlc_txpwr_eirp_t *eirp, uint8 pwr_interpretation,
	uint8 pwr_category, chanspec_t chspec, int buflen)
{
	dot11_vht_transmit_power_envelope_ie_t *vht_transmit_power_ie;
	uint8 power_count = 0;
	vht_transmit_power_ie = (dot11_vht_transmit_power_envelope_ie_t *)*cp;
	vht_transmit_power_ie->id = DOT11_MNG_VHT_TRANSMIT_POWER_ENVELOPE_ID;

	*cp += sizeof(dot11_vht_transmit_power_envelope_ie_t);
	vht_transmit_power_ie->local_max_transmit_power_20 = eirp->txpwr_20;

	if (CHSPEC_IS20(chspec)) {
		vht_transmit_power_ie->transmit_power_info |= 0;
	} else if (CHSPEC_IS40(chspec)) {
		vht_transmit_power_ie->transmit_power_info |= 1;
		power_count = 1;
		*cp = wlc_write_local_maximum_transmit_pwr(eirp->txpwr_40, *cp, buflen);
	} else if (CHSPEC_IS80(chspec)) {
		vht_transmit_power_ie->transmit_power_info |= 2;
		power_count = 2;
		*cp = wlc_write_local_maximum_transmit_pwr(eirp->txpwr_40, *cp, buflen);
		*cp = wlc_write_local_maximum_transmit_pwr(eirp->txpwr_80, *cp, buflen);
	} else if (CHSPEC_IS8080(chspec) || CHSPEC_IS160(chspec)) {
		vht_transmit_power_ie->transmit_power_info |= 3;
		power_count = 3;
		*cp = wlc_write_local_maximum_transmit_pwr(eirp->txpwr_40, *cp, buflen);
		*cp = wlc_write_local_maximum_transmit_pwr(eirp->txpwr_80, *cp, buflen);
		*cp = wlc_write_local_maximum_transmit_pwr(eirp->txpwr_160, *cp, buflen);
	} else {
		WL_ERROR(("%s: wrong chanspec 0x%04x\n", __FUNCTION__, chspec));
		ASSERT(FALSE);
	}

	vht_transmit_power_ie->transmit_power_info |= pwr_interpretation;
	vht_transmit_power_ie->transmit_power_info |= pwr_category;
	vht_transmit_power_ie->len =
		(sizeof(dot11_vht_transmit_power_envelope_ie_t) - TLV_HDR_LEN) +
		power_count;
}

static void
wlc_write_client_eirp_psd(uint8 **cp, wlc_txpwr_eirp_psd_t *eirp_psd, uint8 pwr_interpretation,
	uint8 pwr_category, chanspec_t chspec, int buflen)
{
	dot11_vht_transmit_power_envelope_ie_t *vht_transmit_power_ie;
	uint8 power_count = 0;
	bool all_20_same;
	int i;
	vht_transmit_power_ie = (dot11_vht_transmit_power_envelope_ie_t *)*cp;
	vht_transmit_power_ie->id = DOT11_MNG_VHT_TRANSMIT_POWER_ENVELOPE_ID;

	all_20_same = TRUE;

	for (i = 0; i < TPE_PSD_COUNT; i++) {
		if (eirp_psd->txpwr_psd[0] == eirp_psd->txpwr_psd[i]) {
			continue;
		}
		all_20_same = FALSE;
		break;
	}

	*cp += sizeof(dot11_vht_transmit_power_envelope_ie_t);

	/* using a static value 11 dbm. This has to replace by a value read from ** */
	vht_transmit_power_ie->local_max_transmit_power_20 = eirp_psd->txpwr_psd[0];
	vht_transmit_power_ie->transmit_power_info = 0;
	power_count = 0;

	vht_transmit_power_ie->transmit_power_info |= pwr_interpretation;
	vht_transmit_power_ie->transmit_power_info |= pwr_category;
	if (!all_20_same) {
		if (CHSPEC_IS20(chspec)) {
			vht_transmit_power_ie->transmit_power_info |= 1;
		} else if (CHSPEC_IS40(chspec)) {
			vht_transmit_power_ie->transmit_power_info |= 2;
			power_count = 1;
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[1],
				*cp, buflen);
		} else if (CHSPEC_IS80(chspec)) {
			vht_transmit_power_ie->transmit_power_info |= 3;
			power_count = 3;
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[1],
					*cp, buflen);
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[2],
					*cp, buflen);
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[3],
					*cp, buflen);
		} else if (CHSPEC_IS8080(chspec) || CHSPEC_IS160(chspec)) {
			vht_transmit_power_ie->transmit_power_info |= 4;
			power_count = 7;
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[1],
					*cp, buflen);
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[2],
					*cp, buflen);
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[3],
					*cp, buflen);
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[4],
					*cp, buflen);
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[5],
					*cp, buflen);
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[6],
					*cp, buflen);
			*cp = wlc_write_local_maximum_transmit_pwr(eirp_psd->txpwr_psd[7],
					*cp, buflen);
		} else {
			WL_ERROR(("%s: wrong chanspec 0x%04x\n", __FUNCTION__, chspec));
			ASSERT(FALSE);

		}
	}
	vht_transmit_power_ie->len =
		(sizeof(dot11_vht_transmit_power_envelope_ie_t) - TLV_HDR_LEN) +
		power_count;

}

/* VHT Power Envelop IE in CS Wrapper IE */
static uint
wlc_csw_calc_pwr_env_ie_len(void *ctx, wlc_iem_calc_data_t *calc)
{
	wlc_iem_ft_cbparm_t *ftcbparm = calc->cbparm->ft;
	chanspec_t chspec = ftcbparm->csw.chspec;
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	uint8 buf[257];

	if (BAND_2G(wlc->band->bandtype))
		return BCME_OK;
	if (BAND_6G(wlc->band->bandtype)) {
		return (uint)(wlc_write_transmit_power_envelope_ie_6g(wlc,
				chspec,	buf, sizeof(buf)) - buf);
	} else {
		return (uint)(wlc_write_transmit_power_envelope_ie(wlc,
				chspec,	buf, sizeof(buf)) - buf);
	}
}

static int
wlc_csw_write_pwr_env_ie(void *ctx, wlc_iem_build_data_t *build)
{
	wlc_info_t *wlc = (wlc_info_t *)ctx;
	wlc_iem_ft_cbparm_t *ftcbparm = build->cbparm->ft;
	chanspec_t chspec = ftcbparm->csw.chspec;

	if (BAND_2G(wlc->band->bandtype))
		return BCME_OK;

	if (BAND_6G(wlc->band->bandtype)) {
		wlc_write_transmit_power_envelope_ie_6g(wlc, chspec,
				build->buf, build->buf_len);
	} else {
		wlc_write_transmit_power_envelope_ie(wlc, chspec,
				build->buf, build->buf_len);
	}

	return BCME_OK;
}

#ifdef WL11AC
uint
wlc_csa_action_calc_pwr_env_ie_len(wlc_info_t *wlc, chanspec_t chspec)
{
	uint8 buf[257];

	if (BAND_2G(wlc->band->bandtype))
		return BCME_OK;
	if (BAND_6G(wlc->band->bandtype)) {
		return (uint)(wlc_write_transmit_power_envelope_ie_6g(wlc,
				chspec,	buf, sizeof(buf)) - buf);
	} else {
		return (uint)(wlc_write_transmit_power_envelope_ie(wlc,
				chspec,	buf, sizeof(buf)) - buf);
	}
}

int
wlc_csa_action_write_pwr_env_ie(wlc_info_t *wlc, chanspec_t chspec, uint8 *cp, int buflen)
{
	if (BAND_2G(wlc->band->bandtype))
		return BCME_OK;

	if (BAND_6G(wlc->band->bandtype)) {
		wlc_write_transmit_power_envelope_ie_6g(wlc, chspec,
			cp, buflen);
	} else {
		wlc_write_transmit_power_envelope_ie(wlc, chspec,
			cp, buflen);
	}
	return BCME_OK;
}
#endif /* WL11AC */

void
wlc_get_override_clm_tpe(wlc_info_t *wlc, wlc_ioctl_tx_pwr_t *tx_pwr, int mode)
{
	wlc_pub_t *pub = wlc->pub;
	int i;
	if (mode == WL_OVERRIDE_EIRP) {
		tx_pwr->pwr[0] = pub->_override_clm_eirp.txpwr_20;
		tx_pwr->pwr[1] = pub->_override_clm_eirp.txpwr_40;
		tx_pwr->pwr[2] = pub->_override_clm_eirp.txpwr_80;
		tx_pwr->pwr[3] = pub->_override_clm_eirp.txpwr_160;
		tx_pwr->len = 4;
	}
	if (mode == WL_OVERRIDE_EIRP_PSD) {
		for (i = 0; i < TPE_PSD_COUNT; i++) {
			tx_pwr->pwr[i] = pub->_override_clm_eirp_psd.txpwr_psd[i];
		}
		tx_pwr->len = TPE_PSD_COUNT;
	}
}

void
wlc_set_override_clm_tpe(wlc_info_t *wlc, wlc_ioctl_tx_pwr_t *tx_pwr)
{
	wlc_pub_t *pub = wlc->pub;
	int i;
	if (tx_pwr->mode == WL_OVERRIDE_EIRP) {
		if (tx_pwr->len >= 1) {
			pub->_override_clm_eirp.txpwr_20 = tx_pwr->pwr[0];
		}
		if (tx_pwr->len >= 2) {
			pub->_override_clm_eirp.txpwr_40 = tx_pwr->pwr[1];
		}
		if (tx_pwr->len >= 3) {
			pub->_override_clm_eirp.txpwr_80 = tx_pwr->pwr[2];
		}
		if (tx_pwr->len == 4) {
			pub->_override_clm_eirp.txpwr_160 = tx_pwr->pwr[3];
		}
	}
	if (tx_pwr->mode == WL_OVERRIDE_EIRP_PSD) {
		for (i = 0; i < tx_pwr->len; i++) {
			pub->_override_clm_eirp_psd.txpwr_psd[i] = tx_pwr->pwr[i];
		}
	}
}

int
wlc_ap_set_assoc_rej_rssi_th(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, int8 rssi)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)(wlc->ap);
	ap_bsscfg_cubby_t *ap_cfg;

	if (!BSSCFG_AP(bsscfg)) {
		WL_ERROR(("wl%d:%d configure rssi threshold for association request applicable"
			" only for AP interface\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		return BCME_NOTAP;
	}
	if (rssi > 0) {
		WL_ERROR(("wl%d:%d Invalid rssi param to configure threshold for rejection of"
			" association request\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		return BCME_RANGE;
	}

	ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);
	if (!ap_cfg) {
		WL_ERROR(("wl%d:%d NULL ap_bsscfg_cubby at setting assoc rej threshold rssi,"
			" check bsscfg argument\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		return BCME_ERROR;
	}
	ap_cfg->assoc_rej_rssi_thd = rssi;

	return BCME_OK;
}

int
wlc_ap_get_assoc_rej_rssi_th(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, int8 *rssi)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)(wlc->ap);
	ap_bsscfg_cubby_t *ap_cfg;

	if (!BSSCFG_AP(bsscfg)) {
		WL_ERROR(("wl%d:%d Get rssi threshold for association request applicable"
			" only for AP interface\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		return BCME_NOTAP;
	}
	if (!rssi) {
		WL_ERROR(("wl%d:%d, argument rssi is NULL, return\n", wlc->pub->unit,
			WLC_BSSCFG_IDX(bsscfg)));
		return BCME_BADARG;
	}

	ap_cfg = AP_BSSCFG_CUBBY(appvt, bsscfg);
	if (!ap_cfg) {
		WL_ERROR(("wl%d:%d NULL ap_bsscfg_cubby at getting assoc rej threshold rssi,"
			" check bsscfg argument\n", wlc->pub->unit, WLC_BSSCFG_IDX(bsscfg)));
		return BCME_ERROR;
	}

	*rssi = ap_cfg->assoc_rej_rssi_thd;
	return BCME_OK;
}

#if BAND6G || defined(WL_OCE_AP)
int
wlc_ap_check_tx_bss_for_upr_fd(wlc_info_t *wlc)
{
	uint16 val = 0;

	if (!wlc->upr_fd_info || !wlc->main_ap_bsscfg) {
		return BCME_ERROR;
	}

	if (wlc->main_ap_bsscfg == wlc->upr_fd_info->cfg) {
		return BCME_OK;
	}

	/* TODO: ***** check for pref_transmit_bss IOVAR if configured */
	wlc->upr_fd_info->cfg = NULL;
	if (WL_UPR_FD_UCODE_ENAB(wlc)) {
		/* disable UPR/FD at ucdoe and reset with new cfg */
		val = wlc_bmac_mhf_get(wlc->hw, (uint8)0x01, WLC_BAND_AUTO);
		val &= ~MHF2_UNSOL_PRS;
		wlc_mhf(wlc, (uint8)0x01, 0xffff, val, WLC_BAND_AUTO);
	}
	wlc_update_probe_resp(wlc, FALSE);
	return BCME_OK;
}

#endif /* BAND6G || WL_OCE_AP */

#ifdef BAND6G
int8
wlc_update_regulatory_info(wlc_info_t *wlc, chanspec_t chspec)
{
	clm_tpe_regulatory_limits_t limits_client;
	clm_country_locales_t locales;
	wlc_cm_info_t *wlc_cmi = wlc->cmi;
	clm_country_t country;
	clm_result_t ret;
	int8 reg_info_field = BCME_ERROR;
	uint16 sp_bmp = 0, lpi_bmp = 0;
	uint channel;
	uint last_channel;
	chanspec_t local_chanspec;
	chanspec_t band = CHSPEC_BAND(chspec);

	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)(wlc->ap);

	country = wlc_channel_country(wlc_cmi);
	if (country == CLM_ITER_NULL) {
		ret = wlc_country_lookup_direct(wlc_channel_ccode(wlc_cmi),
				wlc_channel_regrev(wlc_cmi), &country);
		if (ret != CLM_RESULT_OK) {
			wlc_channel_setcountry(wlc_cmi, CLM_ITER_NULL);
			goto exit;
		} else {
			wlc_channel_setcountry(wlc_cmi, country);
		}
	}

	ret = wlc_get_locale(country, &locales);
	if (ret != CLM_RESULT_OK) {
		goto exit;
	}
	FOREACH_20_SB_EFF(chspec, channel, last_channel) {
		local_chanspec = wf_channel2chspec(channel, WL_CHANSPEC_BW_20, band);
		ret = wlc_get_6g_tpe_reg_max_power(wlc_cmi, &locales, &limits_client,
				CLM_REGULATORY_LIMIT_DEST_LOCAL,
				CLM_DEVICE_CATEGORY_SP, local_chanspec);
		sp_bmp = (sp_bmp << 1) | (limits_client.limit[0][0] > WL_RATE_DISABLED);
	}

	FOREACH_20_SB_EFF(chspec, channel, last_channel) {
		local_chanspec = wf_channel2chspec(channel, WL_CHANSPEC_BW_20, band);
		ret = wlc_get_6g_tpe_reg_max_power(wlc_cmi, &locales, &limits_client,
				CLM_REGULATORY_LIMIT_DEST_CLIENT,
				CLM_DEVICE_CATEGORY_LP, local_chanspec);
		lpi_bmp = (lpi_bmp << 1) | (limits_client.limit[0][0] > WL_RATE_DISABLED);
	}

	if (sp_bmp && lpi_bmp) {
		reg_info_field = WL_INDOOR_STANDARD_PWR_ACCESS_POINT;
	} else if (sp_bmp && !lpi_bmp) {
		reg_info_field = WL_STANDARD_PWR_ACCESS_POINT;
	} else if (!sp_bmp && lpi_bmp) {
		reg_info_field = WL_INDOOR_ACCESS_POINT;
	} else {
		WL_ERROR(("wl%d: Neither SP nor LP powers found\n", wlc->pub->unit));
	}

	appvt->reg_info.chspec = chspec;
	appvt->reg_info.sp_bmp = sp_bmp;
	appvt->reg_info.lpi_bmp = lpi_bmp;
	appvt->reg_info.reg_info_field = reg_info_field;
	wlc_ap_update_edcrs_hi_event_status(appvt);

exit:
	return reg_info_field;
}

/**
 * Get regulatory info. See details in IEEE 802.11RevME (Annex E Table E-12)
 * 0: LPI AP, 1: SP AP, 2: VLP AP, 3: Indoor Enabled AP, 4: Indoor SP AP
 */
int8
wlc_get_regulatory_info(wlc_info_t *wlc)
{
	wlc_ap_info_pvt_t *appvt = (wlc_ap_info_pvt_t *)(wlc->ap);

	return appvt->reg_info.reg_info_field;
}
#endif /* BAND6G */

#ifdef WL_OCE_AP
int
wlc_ap_update_retry_prb_rsp(wlc_info_t *wlc)
{
	wlc_bsscfg_t	*cfg = NULL;
	uint8		idx = 0;
	uint16		new_retry_limit = MAX_PRS_RTX_DEF;

	FOREACH_UP_AP(wlc, idx, cfg) {
		if (BSS_OCE_ENAB(wlc, cfg)) {
			new_retry_limit = MAX_PRS_RTX_MIN;
			break;
		}
	}

	if (wlc->probe_rtx_limit == new_retry_limit) {
		WL_INFORM(("wl%d, no change in probe response retry limit[%d]\n",
			wlc->pub->unit, wlc->probe_rtx_limit));
		return BCME_OK;
	}

	wlc->probe_rtx_limit = new_retry_limit;
	wlc_write_shm(wlc, M_PRS_RETRY_THR(wlc), wlc->probe_rtx_limit);
	return BCME_OK;
}
#endif /* WL_OCE_AP */

static void
wlc_ap_bss_updown(void *ctx, bsscfg_up_down_event_data_t *evt)
{
	int		ret = BCME_OK;

	ASSERT(evt);

	if (!BSSCFG_AP(evt->bsscfg)) {
		return;
	}
#ifdef WL_OCE_AP
	if ((ret = wlc_ap_update_retry_prb_rsp(evt->bsscfg->wlc)) != BCME_OK) {
		WL_ERROR(("wl%d:%d Error in updating probe reseponse retry limit\n",
			evt->bsscfg->wlc->pub->unit, WLC_BSSCFG_IDX(evt->bsscfg)));
	}
#endif /* WL_OCE_AP */
	BCM_REFERENCE(ret);
}
