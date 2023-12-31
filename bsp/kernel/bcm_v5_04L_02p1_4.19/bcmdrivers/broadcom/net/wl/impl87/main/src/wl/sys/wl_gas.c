/**
 * @file
 * @brief
 * Support bcm_gas 802.11u GAS (Generic Advertisement Service) state machine in the driver.
 * See bcm_gas for the API.
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
 * $Id: wl_gas.c 807663 2022-01-27 08:17:04Z $
 *
 */

/**
 * @file
 * @brief
 * There is a need for stations to query for information on network services provided by SSPNs or
 * other external networks beyond an AP (Access Point) before they associate to the wireless LAN.
 * GAS defines a generic container to advertise network services information over an IEEE 802.11
 * network. Public Action frames are used to transport this information.
 */

/**
 * @file
 * @brief
 * XXX Twiki: [HslServiceDiscovery]
 */

#include <wlc_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <osl.h>
#include <bcmutils.h>
#include <siutils.h>
#include <wlioctl.h>
#include <bcmendian.h>
#include <d11.h>
#include <phy_utils_api.h>
#include <wlc_rate.h>
#include <wlc_channel.h>
#include <wlc_pub.h>
#include <wlc_bsscfg.h>
#include <wlc.h>
#include <wlc_scan.h>

#if defined(P2PO)
#include <bcm_decode.h>
#include <bcm_decode_gas.h>
#include <bcm_decode_anqp.h>
#endif	/* P2PO */

#include <bcm_gas.h>
#include <wl_gas.h>
#include <wl_eventq.h>
#include <wlc_act_frame.h>
#include <wlc_iocv.h>

#ifdef DONGLEBUILD
#include <wl_rte_priv.h>
#else
#include <wl_linux.h>
#endif

/* gas private info structure */
struct wl_gas_info {
	wlc_info_t		*wlc;			/* Pointer back to wlc structure */
	wl_eventq_info_t	*wlevtq;		/* local event queue handler */
	gasEventCallback_t	gas_evt_cbinfo;		/* gas event callback info */
};

/* wlc_pub_t struct access macros */
#define WLCUNIT(x)	((x)->wlc->pub->unit)
#define WLCOSH(x)	((x)->wlc->osh)

/* Allocate memory */
void *
wl_gas_malloc(void *w, size_t size)
{
	void *p;
	uint8 *retp;
	uint32 *blksz;
	wlc_info_t *wlc = (wlc_info_t *)w;

	/* add 8 bytes for storing the size (use 8 instead of 4 to keep
	 * 64-bit address alignment on 64-bit OSes)
	 */
	size += 8;

	p = MALLOCZ(wlc->osh, size);
	if (p == NULL) {
		WL_ERROR((WLC_MALLOC_ERR, WLCWLUNIT(wlc), __FUNCTION__, (int)size,
			MALLOCED(wlc->osh)));
		return NULL;
	}

	/* Store the requested size at the start of the memory block */
	blksz = (uint32*) p;
	*blksz = (uint32) size;
	retp = (uint8 *)p + 8;

	WL_NONE(("wl_gas_malloc: retp=%p blksz=%p,%u\n",
		OSL_OBFUSCATE_BUF(retp), OSL_OBFUSCATE_BUF(blksz), *blksz));
	return retp;
}

void
wl_gas_free(void *w, void* memblk)
{
	wlc_info_t *wlc = (wlc_info_t *)w;
	uint8 *p;
	uint32 *blksz;
	uint32 size;

	/* Retrieve the requested size at the start of the memory block */
	p = (uint8*)memblk - 8;
	blksz = (uint32*)p;
	size = *blksz;

	WL_NONE(("wl_gas_free: mem=%p blksz=%p,%u\n",
		OSL_OBFUSCATE_BUF(memblk), OSL_OBFUSCATE_BUF(p), size));
	MFREE(wlc->osh, p, size);
}

/** transmit an action frame */
int
wl_gas_tx_actframe(void *w, int bsscfg_idx, uint32 packet_id,
	chanspec_t chspec20, int32 dwell_time,
	struct ether_addr *BSSID, struct ether_addr *da,
	uint16 len, uint8 *data)
{
	wlc_info_t *wlc = (wlc_info_t *)w;
	wl_action_frame_t * action_frame;
	wl_af_params_t * af_params;
	struct ether_addr *bssid;
	int err = 0;
	struct wlc_if *wlcif = wl_gas_get_wlcif(wlc, bsscfg_idx);

	if (data == 0 || len == 0 || len > ACTION_FRAME_SIZE || !wlcif)
		return -1;

	if (ACT_FRAME_IN_PROGRESS(wlc, wlc_bsscfg_find_by_wlcif(wlc, wlcif))) {
		if (chspec20 != wf_chspec_ctlchspec(phy_utils_get_chanspec(wlc->pi))) {
			/* abort previous action frame which may be waiting on dwell time */
			wlc_actframe_abort_action_frame(wlc_bsscfg_find_by_wlcif(wlc, wlcif));
		}
	}

	if ((af_params = (wl_af_params_t *)MALLOC(wlc->osh, WL_WIFI_AF_PARAMS_SIZE)) == NULL) {
		return -1;
	}

	action_frame = &af_params->action_frame;
	action_frame->packetId = packet_id;
	memcpy(&action_frame->da, (char*)da, ETHER_ADDR_LEN);

	/* set default BSSID */
	bssid = da;
	if (BSSID != 0)
		bssid = BSSID;
	memcpy(&af_params->BSSID, (char*)bssid, ETHER_ADDR_LEN);
	WL_NONE(("%s: idx=%u bssid=%02x:%02x:%02x:%02x:%02x:%02x\n",
		__FUNCTION__, bsscfg_idx,
		bssid->octet[0], bssid->octet[1], bssid->octet[2],
		bssid->octet[3], bssid->octet[4], bssid->octet[5]));

	action_frame->len = len;
	af_params->channel = (chspec20 & WL_CHANSPEC_CHAN_MASK);
	af_params->bandtype = CHSPEC_BANDTYPE(chspec20);
	af_params->dwell_time = dwell_time;
	memcpy(action_frame->data, data, len);

	WL_NONE(("GAS tx AF ch=0x%x dt=%u\n", af_params->channel, af_params->dwell_time));

	/* tx action frame */
	err = wlc_iovar_op(wlc, "actframe", NULL, 0,
		(void *)af_params, WL_WIFI_AF_PARAMS_SIZE, IOV_SET,
		wlcif);

	MFREE(wlc->osh, af_params, WL_WIFI_AF_PARAMS_SIZE);

	return (err);
}

/* abort action frame */
int
wl_gas_abort_actframe(void *w, int bsscfg_idx)
{
	wlc_info_t *wlc = (wlc_info_t *)w;

	BCM_REFERENCE(bsscfg_idx);

	/* scan abort to abort action frame */
	wlc_scan_abort(wlc->scan, WLC_E_STATUS_ABORT);

	return 0;
}

/* get ethernet address */
int
wl_gas_get_etheraddr(wlc_info_t *wlc, int bsscfg_idx, struct ether_addr *outbuf)
{
	struct wlc_if *wlcif = wl_gas_get_wlcif(wlc, bsscfg_idx);

	if (wlcif && wlcif->type == WLC_IFTYPE_BSS &&
		wlcif->u.bsscfg) {
		memcpy(outbuf, &wlcif->u.bsscfg->cur_etheraddr, ETHER_ADDR_LEN);
		return 0;
	}
	else
		return -1;
}

/* get wlcif */
struct wlc_if *
wl_gas_get_wlcif(wlc_info_t *wlc, int bsscfgidx)
{
	int err;
	wlc_bsscfg_t	*bsscfg;
	struct wlc_if * retval = NULL;

	bsscfg = wlc_bsscfg_find(wlc, bsscfgidx, &err);

	/* If wlc_bsscfg_find not succeed, NULL will be return */
	if (bsscfg != NULL)
		retval = bsscfg->wlcif;
	else {
		ASSERT(err != BCME_OK);
		WL_ERROR(("wl%d: %s: status %d (%s)\n",
			WLCWLUNIT(wlc), __FUNCTION__, err, bcmerrorstr(err)));
	}
	return retval;
}

static void
wl_gas_event_cb(void *ctx,
	uint32 event_type, wl_event_msg_t *wl_event_msg, uint8 *data, uint32 length)
{
	wl_gas_info_t *gas = (wl_gas_info_t *)ctx;

	if (gas == 0)
		return;

	bcm_gas_process_wlan_event(gas->wlc, event_type, wl_event_msg, data, length);
}

int wl_gas_start_eventq(wl_gas_info_t *gas)
{
	uint32 events[] = { WLC_E_ACTION_FRAME_RX,
		WLC_E_ACTION_FRAME_COMPLETE,
		WLC_E_ACTION_FRAME_OFF_CHAN_COMPLETE };

	/* register callback and events for local event q */
	return wl_eventq_register_event_cb(gas->wlevtq,
		events, ARRAYSIZE(events), wl_gas_event_cb, (void *)gas);
}

void wl_gas_stop_eventq(wl_gas_info_t *gas)
{
	/* unregister callback for local event q */
	wl_eventq_unregister_event_cb(gas->wlevtq, wl_gas_event_cb);
}

/*
 * GAS module down function
 * Does not do anything now other than keep wlc_module_register() happy.
 */
static
int wl_gas_down(void *hdl)
{
	BCM_REFERENCE(hdl);

	return 0;
}

/*
 * initialize gas private context.
 * returns a pointer to the gas private context, NULL on failure.
 */
wl_gas_info_t *
BCMATTACHFN(wl_gas_attach)(wlc_info_t *wlc, wl_eventq_info_t *wlevtq)
{
	wl_gas_info_t *gas;

	/* allocate gas private info struct */
	gas = MALLOCZ(wlc->osh, sizeof(wl_gas_info_t));
	if (!gas) {
		WL_ERROR((WLC_MALLOC_ERR, WLCWLUNIT(wlc), __FUNCTION__, (int)sizeof(wl_gas_info_t),
			MALLOCED(wlc->osh)));
		return NULL;
	}

	/* init gas private info struct */
	gas->wlc = wlc;
	gas->wlevtq = wlevtq;

	/* register module */
	if (wlc_module_register(wlc->pub, NULL, "gas", gas, NULL,
	                        NULL, NULL, wl_gas_down)) {
		WL_ERROR(("wl%d: %s wlc_module_register() failed\n",
		          WLCWLUNIT(wlc), __FUNCTION__));
		return NULL;
	}

	return gas;
}

/* cleanup gas private context */
void
BCMATTACHFN(wl_gas_detach)(wl_gas_info_t *gas)
{
	wl_info_t *wl = NULL;

	WL_INFORM(("wl%d: gas_detach()\n", WLCUNIT(gas)));

	if (!gas)
		return;

	/* Get gas instance of wlc for free */
	wl = gas->wlc->wl;

	wlc_module_unregister(gas->wlc->pub, "gas", gas);
	MFREE(WLCOSH(gas), gas, sizeof(wl_gas_info_t));

	gas = NULL;
	wl->gas = NULL;
}

/* wl set gas instance event callback info */
void
wl_gas_set_gasEventCallback(wlc_info_t *wlc, void *gasi)
{
	wl_info_t *wl = wlc->wl;
	wl->gas = (wl_gas_info_t *)gasi;
}

/* get wlc gas event callback info */
gasEventCallback_t*
BCMRAMFN(wl_gas_get_gasEventCallback)(void *context)
{
	wlc_info_t *wlc = (wlc_info_t *) context;
	wl_info_t *wl = wlc->wl;
	wl_gas_info_t *gasi = wl->gas;
	if (!gasi) {
		return NULL;
	}
	return &(gasi->gas_evt_cbinfo);
}
