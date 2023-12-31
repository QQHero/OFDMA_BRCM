/*
 * Security and Key Management WLC Module Public Interface
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
 * $Id: wlc_keymgmt.h 802604 2021-09-01 17:08:39Z $
 */

/* This file is the interface to WLC key management fucntionality
 * that is not specific to an individual key. It includes support for
 *		WLC Module Interface
 *		Key Index Management
 *		Key lookups based on  BSS and SCB
 *		Signaling Key Management related events
 *		Receiving Key Management relevant notifications
 */

#ifndef _wlc_keymgmt_h_
#define _wlc_keymgmt_h_

#include <typedefs.h>
#include <bcmutils.h>
#include <wlc_types.h>
#include <wlc_key.h>

/* Constants */
#define WLC_KEYMGMT_NUM_GROUP_KEYS 		4
#define WLC_KEYMGMT_NUM_STA_GROUP_KEYS 	2	/* limit for BSS and IBSS */
#define WLC_KEYMGMT_NUM_BSS_IGTK		2
#define WLC_KEYMGMT_IBSS_MAX_PEERS		16
#define WLC_KEYMGMT_NUM_WOWL_KEYS		(WLC_KEYMGMT_NUM_GROUP_KEYS + 1)
#define WLC_KEYMGMT_IBSS_MAX_PEERS		16

/* Support for Key Management events */
enum wlc_keymgmt_event {
	WLC_KEYMGMT_EVENT_NONE 			= 0,
	WLC_KEYMGMT_EVENT_KEY_CREATED	= 1,	/* after creation */
	WLC_KEYMGMT_EVENT_KEY_UPDATED	= 2,	/* after update - key but not iv */
	WLC_KEYMGMT_EVENT_REKEY 		= 2,	/* rekey = updated */
	WLC_KEYMGMT_EVENT_KEY_DESTROY	= 3,	/* before destroy */
	WLC_KEYMGMT_EVENT_DECODE_ERROR	= 4,	/* unacceptable packet format */
	WLC_KEYMGMT_EVENT_DECRYPT_ERROR	= 5,	/* crypto failed (PDU) */
	WLC_KEYMGMT_EVENT_MSDU_MIC_ERROR = 6,	/* TKIP and SMS4 */
	WLC_KEYMGMT_EVENT_REPLAY		= 7,
	WLC_KEYMGMT_EVENT_RESET			= 8,
	WLC_KEYMGMT_EVENT_TKIP_CM_ACTIVE = 9, 	/* TKIP countermeasures active */
	WLC_KEYMGMT_EVENT_LAST					/* add events before this line */
};
typedef enum wlc_keymgmt_event wlc_keymgmt_event_t;

/* Callback function that indicates the key to which the event applies.
 * The key is valid during the callback, but may be deleted after the
 * callback returns. Callbacks must check event type in the event
 * data and ignore events that were not subscribed to.
 */
struct wlc_keymgmt_event_data {
	wlc_keymgmt_event_t event;
	wlc_keymgmt_t 		*km;
	wlc_bsscfg_t		*bsscfg;
	wlc_key_t			*key;
	const void			*pkt;
};
typedef struct wlc_keymgmt_event_data wlc_keymgmt_event_data_t;

typedef void (*wlc_keymgmt_event_callback_t)(void *cb_ctx,
	wlc_keymgmt_event_data_t *event_data);

/* External notification's Key Management module cares about. Some of
 * these are received by subscription and included here for completeness
 */
enum wlc_keymgmt_notif {
	WLC_KEYMGMT_NOTIF_NONE					= 0,
	WLC_KEYMGMT_NOTIF_WLC_UP				= 1,
	WLC_KEYMGMT_NOTIF_WLC_DOWN				= 2,
	WLC_KEYMGMT_NOTIF_BSS_UP				= 3,
	WLC_KEYMGMT_NOTIF_BSS_DOWN				= 4,
	WLC_KEYMGMT_NOTIF_BSS_CREATE			= 5,
	WLC_KEYMGMT_NOTIF_BSS_DESTROY			= 6,
	WLC_KEYMGMT_NOTIF_BSS_WSEC_CHANGED		= 7,
	WLC_KEYMGMT_NOTIF_SCB_CREATE			= 8,
	WLC_KEYMGMT_NOTIF_SCB_DESTROY			= 9,
	WLC_KEYMGMT_NOTIF_KEY_UPDATE			= 10,
	WLC_KEYMGMT_NOTIF_KEY_DELETE			= 11,
	WLC_KEYMGMT_NOTIF_M1_RX					= 12,
	WLC_KEYMGMT_NOTIF_M4_TX					= 13,
	WLC_KEYMGMT_NOTIF_WOWL					= 14,
	WLC_KEYMGMT_NOTIF_SCB_BSSCFG_CHANGED	= 15,
	WLC_KEYMGMT_NOTIF_DECODE_ERROR 			= 16,
	WLC_KEYMGMT_NOTIF_DECRYPT_ERROR 		= 17,
	WLC_KEYMGMT_NOTIF_MSDU_MIC_ERROR 		= 18,	/* includes TKIP */
	WLC_KEYMGMT_NOTIF_TKIP_CM_REPORTED		= 19,	/* tkip CM reported to AP */
	WLC_KEYMGMT_NOTIF_OFFLOAD				= 20,
	WLC_KEYMGMT_NOTIF_BSSID_UPDATE			= 21,
	WLC_KEYMGMT_NOTIF_WOWL_MICERR			= 22,
	WLC_KEYMGMT_NOTIF_NEED_PKTFETCH			= 23,
	WLC_KEYMGMT_NOTIF_LAST					/* add new notif before this */
};
typedef enum wlc_keymgmt_notif wlc_keymgmt_notif_t;

/* Time in seconds with reference same as wlc->pub->now */
typedef int32 wlc_keymgmt_time_t;
struct wlc_keymgmt_wsec_info {
	wlc_key_algo_mask_t		cur_algos;	/* algorithms configured */
	/* Add more fields */
};
typedef struct wlc_keymgmt_wsec_info wlc_keymgmt_wsec_info_t;
#ifdef WLWSEC
/* Interface definition, functions returning an int return BCME_* status
 * unless otherwise specifiied.
 */

/* Attach Key Management module */
wlc_keymgmt_t* wlc_keymgmt_attach(wlc_info_t *wlc);

/* Detach Key Management module */
void wlc_keymgmt_detach(wlc_keymgmt_t *km);

/* Reset Key Management state inclduing associated h/w state for the specified
 * BSS or SCB. If neither is specified reset all. Reset preseves the key
 * indicies and resets the algorithm for the key to CRYPTO_ALGO_NONE
 */
void wlc_keymgmt_reset(wlc_keymgmt_t *km, wlc_bsscfg_t *bsscfg,
	scb_t *scb);

/* Register for an Key Management event. Callback should check that the event
 * received matches subscription, and ignore ones that do not match.
 */
int wlc_keymgmt_event_register(wlc_keymgmt_t *km, wlc_keymgmt_event_t event,
	wlc_keymgmt_event_callback_t cb, void *cb_ctx);

/* Unregister for an Key Management event */
int wlc_keymgmt_event_unregister(wlc_keymgmt_t *km, wlc_keymgmt_event_t event,
	wlc_keymgmt_event_callback_t cb, void *cb_ctx);

/* Key lookup functions. When a Key is returned, the same call
 * provides an optional output parameter (key_info arg, can be NULL)
 * to obtain additional information about the Key
 */

/* Get (default) Key corresponding to the key ID for a BSS. Returns NULL
 * for IBSS. Multi-STA Group keys are also accessible via this call
 * using corresponding Key ID. IGTK may also be obtained.
 */
wlc_key_t* wlc_keymgmt_get_bss_key(wlc_keymgmt_t *km,
	const wlc_bsscfg_t *bsscfg, wlc_key_id_t key_id,
	wlc_key_info_t *key_info);

/* Get the TX/Primary Key for a BSS. IGTK TX Key (AP) can also
 * be requested rather than BSS TX Key.
 */
wlc_key_t* wlc_keymgmt_get_bss_tx_key(wlc_keymgmt_t *km,
	const wlc_bsscfg_t *bsscfg, bool igtk, wlc_key_info_t *key_info);

/* Get TX key ID corresponding for a BSS, or IGTK */
wlc_key_id_t wlc_keymgmt_get_bss_tx_key_id(wlc_keymgmt_t *km,
	const wlc_bsscfg_t *bsscfg, bool igtk);

/* Get the BSS key algorithm */
wlc_key_algo_t wlc_keymgmt_get_bss_key_algo(wlc_keymgmt_t *km,
	const wlc_bsscfg_t *bsscfg, bool igtk);

/* Get the Key for the SCB - ID 0 for primary Key, 0..3 for group keys.
 * IBSS peer group keys or BSS group keys for the SCB are selected by using
 * the WLC_KEY_FLAG_GROUP or WLC_KEY_FLAG_IBSS_PEER_GROUP flag
 * respectively. An SCB may have multiple pairwise keys (e.g. SMS4)
 * each with a different key ID
 */
wlc_key_t* wlc_keymgmt_get_scb_key(wlc_keymgmt_t *km, scb_t *scb,
	wlc_key_id_t key_id, wlc_key_flags_t flags, wlc_key_info_t *key_info);

/* Get the key for any generic tx case, given the scb */
wlc_key_t* wlc_keymgmt_get_tx_key(wlc_keymgmt_t *km, scb_t *scb,
	wlc_bsscfg_t *bsscfg, wlc_key_info_t *key_info);

/* get the amt index reserved for scb; allocate it if necessary;
 * return valid amt index or BCM error status (negative)
 */
int wlc_keymgmt_get_scb_amt_idx(wlc_keymgmt_t *km, scb_t *scb);

void wlc_keymgmt_restore_scb_amt_entry(wlc_keymgmt_t *km, scb_t *scb);

int wlc_keymgmt_alloc_amt(wlc_keymgmt_t *km);
void wlc_keymgmt_free_amt(wlc_keymgmt_t *km, int *amt_idx);
void wlc_keymgmt_amt_reserve(wlc_keymgmt_t *km,
	uint8 amt_idx, size_t count, bool reserve);

/* Get the key based on a peer (SCB) address. This also allows selection
 * of IBSS peer Group Keys. If the address matches BSSID for the BSS
 * or is NULL, the TX (default) Key for the BSS is returned.
 */
wlc_key_t* wlc_keymgmt_get_key_by_addr(wlc_keymgmt_t *km,
	wlc_bsscfg_t *bsscfg, const struct ether_addr *addr,
	wlc_key_flags_t flags, wlc_key_info_t *key_info);

/* Get the key index. Simpler alternative to wlc_key_get_info */
wlc_key_index_t wlc_keymgmt_get_key_index(wlc_keymgmt_t *km,
	wlc_key_t *key);

/* Get the key corresponding to a key index */
wlc_key_t* wlc_keymgmt_get_key(wlc_keymgmt_t *km,
	wlc_key_index_t key_index, wlc_key_info_t *key_info);

/* End Key  lookup functions */

/* Set TX key ID for a BSS to that corresponding to the key ID. note that
 * his may be set on a  STA for gtk/igtk, although is not valid for TX.
 * that is controlled by WLC_KEY_FLAG_TX
 */
int wlc_keymgmt_set_bss_tx_key_id(wlc_keymgmt_t *km,
	wlc_bsscfg_t *bsscfg, wlc_key_id_t key_id, bool igtk);

/* Receive a notification and handle it */
void wlc_keymgmt_notify(wlc_keymgmt_t *km, wlc_keymgmt_notif_t notif,
	wlc_bsscfg_t *bsscfg, scb_t *scb, wlc_key_t *key, void *pkt);

/* Check if TKIP Countermeasures are enabled, and return whether they are active */
bool wlc_keymgmt_tkip_cm_enabled(wlc_keymgmt_t *km, const wlc_bsscfg_t *bsscfg);

/* enable or disable tkip countermeasures. No effect if already enabled */
void wlc_keymgmt_tkip_set_cm(wlc_keymgmt_t *km, wlc_bsscfg_t *bsscfg, bool enable);

/* Returns whether B4/M4 Key buffering for is enabled for a BSS */
bool wlc_keymgmt_b4m4_enabled(wlc_keymgmt_t *km, wlc_bsscfg_t *bsscfg);

/* Returns the corresponding wlc, bsscfg, scb. if key is shared between
 * scb's across multiple bands, one of the scbs is returned.
 */
wlc_info_t *wlc_keymgmt_get_wlc(wlc_keymgmt_t *km);
wlc_bsscfg_t *wlc_keymgmt_get_bsscfg(wlc_keymgmt_t *km,
	wlc_key_index_t key_idx);
scb_t *wlc_keymgmt_get_scb(wlc_keymgmt_t *km,
	wlc_key_index_t key_idx);

/* Map key algo to h/w algo. This mapping depends on corerev */
wlc_key_hw_algo_t wlc_keymgmt_algo_to_hw_algo(wlc_keymgmt_t *km,
	wlc_key_algo_t algo);

/* Map h/w algo to key algo */
wlc_key_algo_t
wlc_keymgmt_hw_algo_to_algo(wlc_keymgmt_t *km, wlc_key_hw_algo_t algo);

/* utility -  rx frame support */
int wlc_keymgmt_recvdata(wlc_keymgmt_t *km, wlc_frminfo_t *f);

#if defined(BCMDBG) || defined(WLMSG_WSEC)
/* Get event name */
const char* wlc_keymgmt_event_name(wlc_keymgmt_event_t event);

/* get notification name */
const char* wlc_keymgmt_notif_name(wlc_keymgmt_notif_t notif);

/* get algo names */
const char *wlc_keymgmt_get_algo_name(wlc_keymgmt_t *km, wlc_key_algo_t algo);
const char *wlc_keymgmt_get_hw_algo_name(wlc_keymgmt_t *km, wlc_key_hw_algo_t algo, int mode);

#endif

#ifdef BRCMAPIVTW
/* ivtw support */
int wlc_keymgmt_ivtw_enable(wlc_keymgmt_t *km, scb_t *scb, bool enable);
#endif /* BRCMAPIVTW */
int wlc_keymgmt_preattach(void);
#if defined(ACKSUPR_MAC_FILTER) || defined(PSTA) || defined(WL_CCA_STATS_MESH)
bool wlc_keymgmt_amt_idx_isset(wlc_keymgmt_t *km, int amt_idx);
#endif /* ACKSUPR_MAC_FILTER */
int wlc_keymgmt_wsec(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, uint32 val);
int wlc_keymgmt_get_cur_wsec_algo(wlc_keymgmt_t *km, wlc_bsscfg_t *bsscfg,
	uint32 key_size, wlc_key_algo_t *key_algo);
#else /* WLWSEC */
#define wlc_keymgmt_reset(km, bsscfg, scb) do {} while (0)
#define wlc_keymgmt_get_bss_key(km, bsscfg, key_id, key_info) NULL
#define wlc_keymgmt_notify(km, notif, bsscfg, scb, key, pkt) do {} while (0)
#define wlc_keymgmt_tkip_cm_enabled(km, bsscfg) NULL
#define wlc_keymgmt_get_bss_tx_key(km, bsscfg, igtk, key_info) NULL
#define wlc_keymgmt_get_key_by_addr(km, bsscfg, addr, flags, key_info) NULL
#define wlc_keymgmt_get_scb_key(km, scb, key_id, flags, key_info) NULL
#define wlc_keymgmt_recvdata(km, f) NULL
#define wlc_keymgmt_b4m4_enabled(km, bsscfg) NULL
#define wlc_keymgmt_alloc_amt(km) 0
#define wlc_keymgmt_get_tx_key(km, scb, bsscfg, key_info) NULL
#define wlc_keymgmt_free_amt(km, amt_idx) do {} while (0)
#define wlc_keymgmt_wsec(wlc, bsscfg, val) BCME_UNSUPPORTED
#endif /* WLWSEC */
void wlc_keymgmt_get_wsec_info(wlc_keymgmt_t *km, wlc_bsscfg_t *bsscfg,
	wlc_keymgmt_wsec_info_t * wsec_info);
#ifdef WLCFP
union d11txhdr;
extern void key_aes_tx_mpdu_fast(wlc_key_t *key, void *pkt, uint8 *body,
	union d11txhdr *txd);
#endif

#endif /* _wlc_keymgmt_h_ */
