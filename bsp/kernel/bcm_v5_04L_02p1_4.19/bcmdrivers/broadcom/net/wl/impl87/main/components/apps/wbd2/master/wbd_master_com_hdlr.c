/*
 * WBD Communication Related Definitions
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
 * $Id: wbd_master_com_hdlr.c 807941 2022-02-04 08:07:43Z $
 */

#include "wbd.h"
#include "wbd_shared.h"
#include "wbd_ds.h"
#include "wbd_com.h"
#include "wbd_sock_utility.h"
#include "wbd_json_utility.h"
#include "wbd_master_control.h"
#ifdef BCM_APPEVENTD
#include "wbd_appeventd.h"
#include "appeventd_wbd.h"
#endif /* BCM_APPEVENTD */

#include "ieee1905_tlv.h"
#include "ieee1905.h"
#include "wbd_tlv.h"
#include "wbd_master_com_hdlr.h"
#include "wbd_master_vndr_brcm.h"
#include <bcmparams.h>

/* Minimum 2 BSS are required for having the same client entry to create
 * the REMOVE_CLIENT_REQ timer
 */
#define WBD_MIN_BSS_TO_CREATE_TIMER	2

/* Timer restart count for unconfigured radios to send renew */
#define WBD_RENEW_TIMER_RESTART_COUNT	10

/* timeout to send channel selection reuqest after channel preference query */
#define WBD_TM_CHAN_SELECT_AFTER_QUERY	2

/* 5g channel maximum control channel */
#define MAX_5G_CONTROL_CHANNEL	165

/* 2.4G 40MHz regulatory classes */
#define REGCLASS_24G_40MHZ_UPPER	84
#define REGCLASS_24G_40MHZ_LOWER	83

/* structure to hold the params to remove stale client from BSS's assocl list i.e.
 * remove client from associated on the following condition:
 * client didnt sent disassoc to first BSS && client roam to another strong BSS
 */
typedef struct wbd_remove_client_arg {
	struct ether_addr parent_bssid;	/* Parent BSS's BSSID */
	struct ether_addr sta_mac;	/* STA mac address */
} wbd_remove_client_arg_t;

/* Flags used in the nvram_list_t structure */
#define WBD_NVRAM_FLAG_STR	0x00	/* NVRAM value is of type string */
#define WBD_NVRAM_FLAG_VALINT	0x01	/* NVRAM value is of type integer */

/* List of NVRAMs */
typedef struct nvram_list {
	char name[NVRAM_MAX_PARAM_LEN];	/* Name of the NVRAM */
	uint8 flags;			/* Flags of type WBD_NVRAM_FLAG_XXX */
	int def;			/* Default value in case WBD_NVRAM_FLAG_VALINT flag */
} nvram_list_t;

/* List of Radio/BSS NVRAMs */
typedef struct prefix_nvram_list {
	char name[NVRAM_MAX_PARAM_LEN];	/* Name of the NVRAM */
	uint8 band_flag;		/* For Which bands this NVRAM to be applied.
					 * WBD_BAND_LAN_XXX type
					 */
} prefix_nvram_list_t;

/* List of NVRAMs to be sent from controller to agents */
static nvram_list_t nvram_params[] = {
	{WBD_NVRAM_MCHAN_SLAVE, WBD_NVRAM_FLAG_VALINT, WBD_DEF_MCHAN_SLAVE},
	{WBD_NVRAM_TM_SLV_WD_TARGET_BSS, WBD_NVRAM_FLAG_VALINT, WBD_TM_SLV_WD_TARGET_BSS},
	{NVRAM_MAP_NO_MULTIHOP, WBD_NVRAM_FLAG_STR, 0},
	{NVRAM_MAP_2G_BSTA_BS_RSSI_THLD, WBD_NVRAM_FLAG_VALINT, WBD_DEF_2G_BSTA_BS_RSSI_THLD}
};

/* List of Radio NVRAMs to be sent from controller to agents.
 * NVRAMs are sent with Radio MAC address
 * For ex: If you want to set the NVRAM "dummy" to all the Radio's, Then define it like
 * {"dummy", WBD_BAND_LAN_ALL} and set the NVRAM like nvram set dummy="value"
 * If you want to set only for 2G Radio, then define it like {"dummy_2g", WBD_BAND_LAN_2G}
 * Set NVRAM like nvram set dummy_2g="value"
 */
static prefix_nvram_list_t radio_nvram_params[] = {
};

/* List of SSID NVRAMs(MAP BSS Names) to be sent from controller to agents.
 * NVRAMs are sent with Radio MAC and SSID. In agent, set NVRAM for SSID and radio combination.
 * NVRAMs needs to be defined based on the MAP BSS Names.
 * For ex: If you want to set the NVRAM "dummy" to all the fronthaul SSID's, Then define it like
 * {"dummy", WBD_BAND_LAN_ALL} and set the NVRAM like nvram set fh_dummy="value"
 * If you want to set only for 2G Fronthaul BSS, then define it like {"dummy_2g", WBD_BAND_LAN_2G}
 * Set NVRAM like nvram set fh_dummy_2g="value"
 */
static prefix_nvram_list_t ssid_nvram_params[] = {
};

#define WBD_GET_NVRAMS_COUNT()		(sizeof(nvram_params)/sizeof(nvram_params[0]))
#define WBD_GET_RADIO_NVRAMS_COUNT()	(sizeof(radio_nvram_params)/sizeof(radio_nvram_params[0]))
#define WBD_GET_SSID_NVRAMS_COUNT()	(sizeof(ssid_nvram_params)/sizeof(ssid_nvram_params[0]))

/* ------------------------------------ Static Declarations ------------------------------------ */

/* Do Initialize sequences for newly created Master Info for specific band */
static int
wbd_master_init_actions(uint8 bkt_id, wbd_master_info_t *master);
/* get tbss threshold configurations */
static int
wbd_master_get_tbss_config(wbd_tbss_thld_t *thld_cfg, char *thld_nvram,
	wbd_master_info_t *master);
/* Gets Master NVRAM settings */
static int
wbd_master_retrieve_nvram_config(uint8 bkt_id, wbd_master_info_t *master);
/* Master updates WEAK_CLIENT/STRONG_CLIENT Data */
static int
wbd_master_process_weak_strong_client_cmd_data(wbd_master_info_t *master_info,
	i5_dm_clients_type *i5_assoc_sta, wbd_cmd_weak_strong_client_resp_t *cmdclientresp,
	wbd_sta_status_t command);
/* Processes WEAK_CLIENT request */
static int
wbd_master_process_weak_client_cmd(wbd_master_info_t *master_info,
	i5_dm_clients_type *i5_assoc_sta, wbd_weak_sta_metrics_t *sta_stats,
	wbd_weak_sta_policy_t *metric_policy);
/* Processes STRONG_CLIENT request */
static int
wbd_master_process_strong_client_cmd(wbd_master_info_t *master_info,
	i5_dm_clients_type *i5_assoc_sta, wbd_strong_sta_metrics_t *strong_sta_stats,
	wbd_strong_sta_policy_t *strong_metric_policy);
/* Create the timer to remove stale client entry for BSS */
static int
wbd_controller_create_remove_client_timer(wbd_remove_client_arg_t* arg, int timeout);
/* Remove stale client entry form BSS, if required */
static void
wbd_controller_remove_client_timer_cb(bcm_usched_handle *hdl, void *arg);
/* Create AP chan report from topology */
static void wbd_master_create_ap_chan_report(i5_dm_network_topology_type *i5_topology,
	wbd_master_info_t *master_info);
/* Controller Find Interface on its device, Matching the band */
static int wbd_master_get_ifname_matching_band(int in_band_flag,
	char *out_ifname, int out_ifname_size);
/* -------------------------- Add New Functions above this -------------------------------- */

/* Processes the STEER CLI command */
static void
wbd_master_process_steer_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg);
/* Get the SLAVELIST CLI command data */
static int
wbd_master_process_bsslist_cli_cmd_data(wbd_info_t *info,
	wbd_cli_send_data_t *clidata,  char **outdataptr);
/* Processes the SLAVELIST CLI command */
static void
wbd_master_process_bsslist_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg);
/* Get the INFO CLI command data */
static int
wbd_master_process_info_cli_cmd_data(wbd_info_t *info,
	wbd_cli_send_data_t *clidata,  char **outdataptr);
/* Processes the INFO CLI command */
static void
wbd_master_process_info_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg);
/* API to Compare AP Metrics Query & stats Timestamps */
static int
wbd_master_ap_metrics_ts_compare(wbd_ap_metrics_query_args_t *apm_query_data);
/* API to Send AP Metrics Query to Agent */
static void
wbd_master_send_ap_metrics_query_to_agent(i5_dm_device_type *p_device,
	i5_dm_bss_type *p_bss, wbd_ap_metrics_query_args_t *apm_query_data);
/* Get the CLIENTLIST CLI command data */
static int
wbd_master_process_clientlist_cli_cmd_data(wbd_info_t *info,
	wbd_cli_send_data_t *clidata,  char **outdataptr);
/* Processes the CLIENTLIST CLI command */
static void
wbd_master_process_clientlist_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg);
/* Get the logs cli command data */
static int
wbd_master_process_logs_cli_cmd_data(wbd_info_t *info,
	wbd_cli_send_data_t *clidata,  char **outdataptr);
/* Process the logs cli command. */
static void
wbd_master_process_logs_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg);
/* Processes the BH OPT command */
static void
wbd_master_process_bh_opt_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg);
/* Process the exit cli command */
static void
wbd_master_process_exit_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg);
/* Process the reload cli command */
static void
wbd_master_process_reload_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg);
/* Callback for exception from communication handler for CLI */
static void
wbd_master_com_cli_exception(wbd_com_handle *hndl, void *arg, WBD_COM_STATUS status);
/* Register all the commands for master server to communication handle */
static int
wbd_master_register_server_command(wbd_info_t *info);
/* Callback fn to send CONFIG cmd to Slave which has just joined to Master */
static void
wbd_master_send_ap_autoconfig_renew_timer_cb(bcm_usched_handle *hdl, void *arg);
/* Processes BSS capability report message */
static void
wbd_master_process_bss_capability_report_cmd(unsigned char *neighbor_al_mac,
	unsigned char *tlv_data, unsigned short tlv_data_len);
/* Processes Steer Response Report message */
static void
wbd_master_process_steer_resp_report_cmd(unsigned char *neighbor_al_mac,
	unsigned char *tlv_data, unsigned short tlv_data_len);
/* Processes ZWDFS message */
static void
wbd_master_process_zwdfs_cmd(unsigned char *neighbor_al_mac,
	unsigned char *tlv_data, unsigned short tlv_data_len);
/* Send NVRAM set vendor specific message */
static int
wbd_master_send_vndr_nvram_set_cmd(i5_dm_device_type *i5_device, i5_dm_interface_type *i5_ifr);
#if !defined(MULTIAPR2)
/* create common chan info from intersection of all agent's chan info */
static wbd_dfs_chan_info_t*
wbd_master_get_common_chan_info(i5_dm_network_topology_type *i5_topology, uint8 band);
/* send intersection of all agent's same band chan info. Agent along with
 * this chan info and local chanspecs list prepare dfs list
 */
#endif /* !MULTIAPR2 */
static void
wbd_master_send_dfs_chan_info(wbd_dfs_chan_info_t *dfs_chan_info,
	uint8 band, unsigned char *al_mac, unsigned char *radio_mac);
/* Create intersection of all agent's chan info(with same band), and Broadcast
 * final chan_info to each agent.
 */
#if !defined(MULTIAPR2)
static void
wbd_master_process_intf_chan_info(unsigned char *src_al_mac, unsigned char *tlv_data,
	unsigned short tlv_data_len);
#endif /* !MULTIAPR2 */
#if defined(MULTIAPR2)
/* Send Channel Scan Request Message */
static void
wbd_master_send_chscan_req(unsigned char *al_mac, bool fresh_scan_req);
/* Obtain STA info and process wnm request message to populate the np_chan_entry_list */
static int wbd_master_process_wnm_request_mbo_sta(unsigned char *al_mac, unsigned char *sta_mac,
	uint8* data, uint8 body_len);
/* Master process Assoc/WNM request for Non preferred channel report */
static int wbd_master_process_mbo_np_chan_list(wbd_assoc_sta_item_t *sta, uint8* ibuf,
	uint8 ibuf_len, bool attr);
/* Read Non preferred channel report and create entry list used for tbss selection avoidance */
static int wbd_master_process_update_mbo_np_chan_list(wbd_assoc_sta_item_t* sta, uint8* ibuf,
	uint8 ibuf_len, bool attr, uint8 chan_list_len);
/* Get Non preferred subelement channel list length */
static int wbd_master_process_mbo_get_count_chan_from_non_pref_list(uint8* ibuf,
	uint8 ibuf_len, bool attr);
/* Obtain STA info and process assoc request message to populate the np_chan_entry_list */
static int wbd_master_process_assoc_request_mbo_sta(unsigned char *al_mac, unsigned char *sta_mac,
	uint8* data, uint32 body_len, bool is_reassoc);
#endif /* MULTIAPR2 */
/* ------------------------------------ Static Declarations ------------------------------------ */

/* Common algo to compare STA Stats and Thresholds, & identify if STA is Weak or not */
extern int wbd_weak_sta_identification(struct ether_addr *sta_mac,
	wbd_weak_sta_metrics_t *sta_stats, wbd_weak_sta_policy_t *thresholds,
	int *out_fail_cnt, int *out_weak_flag);

/* Reload configurations and push it to all the agents */
extern void wbd_master_reload();

/* Do Initialize sequences for newly created Master Info for specific band */
static int
wbd_master_init_actions(uint8 bkt_id, wbd_master_info_t *master)
{
	int ret = WBDE_OK;
	WBD_ENTER();

	/* Gets Master NVRAM settings */
	wbd_master_retrieve_nvram_config(bkt_id, master);

	WBD_INFO("BKTID[%d] Master[%p] Init Sequence Started %s\n",
		master->bkt_id, master, wbderrorstr(ret));
	WBD_EXIT();
	return ret;
}

/* get tbss threshold configurations */
static int
wbd_master_get_tbss_config(wbd_tbss_thld_t *thld_cfg, char *thld_nvram,
	wbd_master_info_t *master)
{
	int ret = WBDE_OK, num = 0;
	char *str;
	wbd_tbss_thld_t threshold_cfg;
	WBD_ENTER();

	/* read threshold from NVRAM */
	str = blanket_nvram_safe_get(thld_nvram);
	if (str) {
		num = sscanf(str, "%d %d %d %d %x %d",
			&threshold_cfg.t_rssi,
			&threshold_cfg.t_hops,
			&threshold_cfg.t_sta_cnt,
			&threshold_cfg.t_uplinkrate,
			&threshold_cfg.flags,
			&threshold_cfg.sof_algos);
		if (num == 6) {
			if ((threshold_cfg.sof_algos < 0) ||
				(threshold_cfg.sof_algos > WBD_MAX_SOF_ALGOS)) {
				threshold_cfg.sof_algos = WBD_SOF_ALGO_BEST_RSSI;
			}
			memcpy(thld_cfg, &threshold_cfg, sizeof(*thld_cfg));
		} else {
			WBD_WARNING("BKTID[%d] %s : %s = %s\n", master->bkt_id,
				wbderrorstr(WBDE_INV_NVVAL), thld_nvram, str);
		}
	}

	WBD_DEBUG("BKTID[%d] %s t_rssi[%d] t_hops[%d] t_sta_cnt[%d] t_uplinkrate[%d] flags[0x%X] "
		"sof_algo[%d]\n", master->bkt_id, thld_nvram, thld_cfg->t_rssi, thld_cfg->t_hops,
		thld_cfg->t_sta_cnt, thld_cfg->t_uplinkrate, thld_cfg->flags, thld_cfg->sof_algos);

	WBD_EXIT();
	return ret;
}

/* get tbss weightage configurations */
static int
wbd_master_get_tbss_weightage_config(wbd_tbss_wght_t *wght_cfg, char *wght_nvram,
	wbd_master_info_t *master)
{
	int ret = WBDE_OK, num = 0;
	char *str;
	wbd_tbss_wght_t weightage_cfg;
	WBD_ENTER();

	/* read weightages from NVRAM */
	str = blanket_nvram_safe_get(wght_nvram);
	if (str) {
		num = sscanf(str, "%d %d %d %d %d %d %d %x",
			&weightage_cfg.w_rssi,
			&weightage_cfg.w_hops,
			&weightage_cfg.w_sta_cnt,
			&weightage_cfg.w_phyrate,
			&weightage_cfg.w_nss,
			&weightage_cfg.w_tx_rate,
			&weightage_cfg.w_band,
			&weightage_cfg.flags);
		if (num == 8) {
			memcpy(wght_cfg, &weightage_cfg, sizeof(*wght_cfg));
		} else {
			WBD_WARNING("BKTID[%d] %s : %s = %s\n", master->bkt_id,
				wbderrorstr(WBDE_INV_NVVAL), wght_nvram, str);
		}
	}

	WBD_DEBUG("BKTID[%d] %s w_rssi[%d] w_hops[%d] w_sta_cnt[%d] w_phyrate[%d] w_band [%d] "
		"flags[0x%X]\n", master->bkt_id, wght_nvram, wght_cfg->w_rssi, wght_cfg->w_hops,
		wght_cfg->w_sta_cnt, wght_cfg->w_phyrate, wght_cfg->w_band, wght_cfg->flags);

	WBD_EXIT();
	return ret;
}

/* Gets Master NVRAM settings */
static int
wbd_master_retrieve_nvram_config(uint8 bkt_id, wbd_master_info_t *master)
{
	int ret = WBDE_OK;
	char thld_nvram[WBD_MAX_NVRAM_NAME];
	char *str;
	int num = 0;
	WBD_ENTER();

	WBD_DEBUG("Retrieve Config for BKTID[%d] BKTNAME[%s]\n", master->bkt_id, master->bkt_name);
	master->tbss_info.nws_adv_thld =
		blanket_get_config_val_int(NULL, WBD_NVRAM_ADV_THLD, WBD_TBSS_ADV_THLD);

	master->tbss_info.tbss_stacnt_thld = blanket_get_config_val_int(NULL,
		WBD_NVRAM_TBSS_STACNT_THLD, WBD_TBSS_MIN_STA_THLD);

	master->tbss_info.tbss_algo = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_TBSS_ALGO, 0);
	if (master->tbss_info.tbss_algo >= wbd_get_max_tbss_algo())
		master->tbss_info.tbss_algo = 0;

	master->tbss_info.tbss_algo_bh = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_TBSS_ALGO_BH, 0);
	if (master->tbss_info.tbss_algo_bh >= wbd_get_max_tbss_algo())
		master->tbss_info.tbss_algo_bh = 0;

	/* Read Weightage index and prepare weightage config from predefined entries */
	master->tbss_info.tbss_wght = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_TBSS_WGHT_IDX, 0);
	if (master->tbss_info.tbss_wght >= wbd_get_max_tbss_wght())
		master->tbss_info.tbss_wght = 0;
	memcpy(&master->tbss_info.wght_cfg, wbd_get_tbss_wght(master),
		sizeof(master->tbss_info.wght_cfg));

	/* Read Backhaul Weightage index and prepare weightage config from predefined entries */
	master->tbss_info.tbss_wght_bh = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_TBSS_WGHT_IDX_BH, 0);
	if (master->tbss_info.tbss_wght_bh >= wbd_get_max_tbss_wght_bh())
		master->tbss_info.tbss_wght_bh = 0;
	memcpy(&master->tbss_info.wght_cfg_bh, wbd_get_tbss_wght_bh(master),
		sizeof(master->tbss_info.wght_cfg_bh));

	/* Get NVRAM : ACTION fram intervval for home and off-channel weak STA */
	str = blanket_nvram_prefix_safe_get(NULL, WBD_NVRAM_TBSS_BOUNDARIES);
	if (str) {
		num = sscanf(str, "%d %d %d %d", &master->tbss_info.tbss_min_phyrate,
			&master->tbss_info.tbss_max_phyrate, &master->tbss_info.tbss_min_rssi,
			&master->tbss_info.tbss_max_rssi);
	}
	if (!str || num != 4) {
		/* 2G boundary is default for a blanket */
		master->tbss_info.tbss_min_phyrate = WBD_TBSS_MIN_PHYRATE_BOUNDARY_2G;
		master->tbss_info.tbss_max_phyrate = WBD_TBSS_MAX_PHYRATE_BOUNDARY_2G;
		master->tbss_info.tbss_min_rssi = WBD_TBSS_MIN_RSSI_BOUNDARY_2G;
		master->tbss_info.tbss_max_rssi = WBD_TBSS_MAX_RSSI_BOUNDARY_2G;
	}
	blanket_log_default_nvram("%s=%d %d %d %d\n", WBD_NVRAM_TBSS_BOUNDARIES,
		master->tbss_info.tbss_min_phyrate,
		master->tbss_info.tbss_max_phyrate,
		master->tbss_info.tbss_min_rssi, master->tbss_info.tbss_max_rssi);

	WBD_DEBUG("BKTID[%d] nws_adv_thld[%d] tbss_stacnt_thld[%d] tbss_algo[%d] tbss_algo_bh[%d] "
		"tbss_wght[%d] min_phyrate_boundary[%d] max_phyrate_boundary[%d] "
		"min_rssi_boundary[%d] max_rssi_boundary[%d]\n", master->bkt_id,
		master->tbss_info.nws_adv_thld, master->tbss_info.tbss_stacnt_thld,
		master->tbss_info.tbss_algo, master->tbss_info.tbss_algo_bh,
		master->tbss_info.tbss_wght, master->tbss_info.tbss_min_phyrate,
		master->tbss_info.tbss_max_phyrate, master->tbss_info.tbss_min_rssi,
		master->tbss_info.tbss_max_rssi);

	/* read weightages from NVRAM for fronthaul */
	wbd_master_get_tbss_weightage_config(&master->tbss_info.wght_cfg, WBD_NVRAM_TBSS_WGHT,
		master);

	/* read weightages from NVRAM for backhaul */
	wbd_master_get_tbss_weightage_config(&master->tbss_info.wght_cfg_bh, WBD_NVRAM_TBSS_WGHT_BH,
		master);

	/* Read threshold index and prepare threshold config from predefined entries - 2G */
	master->tbss_info.tbss_thld_2g = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_TBSS_THLD_IDX_2G, 0);
	if (master->tbss_info.tbss_thld_2g >= wbd_get_max_tbss_thld_2g())
		master->tbss_info.tbss_thld_2g = 0;
	memcpy(&master->tbss_info.thld_cfg_2g, wbd_get_tbss_thld_2g(master),
		sizeof(master->tbss_info.thld_cfg_2g));

	WBDSTRNCPY(thld_nvram, WBD_NVRAM_TBSS_THLD_2G, sizeof(thld_nvram));
	wbd_master_get_tbss_config(&master->tbss_info.thld_cfg_2g, thld_nvram, master);

	/* Read threshold index and prepare threshold config from predefined entries - 5G */
	master->tbss_info.tbss_thld_5g = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_TBSS_THLD_IDX_5G, 0);
	if (master->tbss_info.tbss_thld_5g >= wbd_get_max_tbss_thld_5g())
		master->tbss_info.tbss_thld_5g = 0;
	memcpy(&master->tbss_info.thld_cfg_5g, wbd_get_tbss_thld_5g(master),
		sizeof(master->tbss_info.thld_cfg_5g));

	WBDSTRNCPY(thld_nvram, WBD_NVRAM_TBSS_THLD_5G, sizeof(thld_nvram));
	wbd_master_get_tbss_config(&master->tbss_info.thld_cfg_5g, thld_nvram, master);

	/* Read threshold index and prepare threshold config from predefined entries - 6G */
	master->tbss_info.tbss_thld_6g = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_TBSS_THLD_IDX_6G, 0);
	if (master->tbss_info.tbss_thld_6g >= wbd_get_max_tbss_thld_6g())
		master->tbss_info.tbss_thld_6g = 0;
	memcpy(&master->tbss_info.thld_cfg_6g, wbd_get_tbss_thld_6g(master),
		sizeof(master->tbss_info.thld_cfg_6g));

	WBDSTRNCPY(thld_nvram, WBD_NVRAM_TBSS_THLD_6G, sizeof(thld_nvram));
	wbd_master_get_tbss_config(&master->tbss_info.thld_cfg_6g, thld_nvram, master);

	/* Read threshold index and prepare threshold config from predefined entries - Backhaul */
	master->tbss_info.tbss_thld_bh = (uint8)blanket_get_config_val_uint(NULL,
		WBD_NVRAM_TBSS_THLD_IDX_BH, 0);
	if (master->tbss_info.tbss_thld_bh >= wbd_get_max_tbss_thld_bh())
		master->tbss_info.tbss_thld_bh = 0;
	memcpy(&master->tbss_info.thld_cfg_bh, wbd_get_tbss_thld_bh(master),
		sizeof(master->tbss_info.thld_cfg_bh));

	WBDSTRNCPY(thld_nvram, WBD_NVRAM_TBSS_THLD_BH, sizeof(thld_nvram));
	wbd_master_get_tbss_config(&master->tbss_info.thld_cfg_bh, thld_nvram, master);

	/* Set sta based default threshhold */
	master->tbss_info.thld_sta.t_tx_rate = WBD_TBSS_STA_TX_RATE_THLD;
	master->tbss_info.thld_sta.t_tx_failures = WBD_TBSS_STA_TX_FAILURES_THLD;

	WBD_EXIT();
	return ret;
}

/* Called when some STA joins the BSS */
int
wbd_controller_refresh_blanket_on_sta_assoc(struct ether_addr *sta_mac,
	struct ether_addr *parent_bssid, wbd_wl_sta_stats_t *sta_stats)
{
	int ret = WBDE_OK, sta_found_count = 0;
	wbd_remove_client_arg_t *param = NULL;
	WBD_ENTER();

	/* If STA entry is found in Bounce Table && STA Status is STEERING, so this STA has
	 *  been steered from source to this TBSS. So let's increment the Bounce Count here.
	 */
	wbd_ds_increment_bounce_count_of_entry(wbd_get_ginfo()->wbd_master,
		sta_mac, parent_bssid, sta_stats);

	/* If remove_client is 0(not enabled), no need to send the REMOVE_CLIENT_REQ command */
	if (!WBD_RMCLIENT_ENAB((wbd_get_ginfo()->flags))) {
		goto end;
	}

	/* Count how many BSSes has the STA entry in Controller Topology */
	sta_found_count = wbd_ds_find_duplicate_sta_in_controller(sta_mac, NULL, NULL);

	WBD_INFO("STA["MACF"] Found in %d BSSes\n", ETHER_TO_MACF(*sta_mac), sta_found_count);
	/* Atleast 2 BSS should have the same client entry to start the timer,
	 * if more than 2 BSSes having the client entry, no need to start the timer
	 * again, as timer is already running
	 */
	if ((sta_found_count > 1) && (sta_found_count <= WBD_MIN_BSS_TO_CREATE_TIMER)) {
		param = (wbd_remove_client_arg_t*)wbd_malloc(sizeof(*param), &ret);
		WBD_ASSERT_MSG("STA["MACF"] BSS["MACF"] Failed to alloc REMOVE_CLIENT_REQ arg\n",
			ETHER_TO_MACF(*sta_mac), ETHER_TO_MACF(*parent_bssid));

		/* Create the timer to send REMOVE_CLIENT_REQ Vendor Msg to BSS */
		eacopy(sta_mac, &(param->sta_mac));
		eacopy(parent_bssid, &(param->parent_bssid));

		wbd_controller_create_remove_client_timer(param,
			wbd_get_ginfo()->max.tm_remove_client);
	}
end:
	WBD_EXIT();
	return ret;
}

/* Master updates WEAK_CLIENT/STRONG_CLIENT Data */
static int
wbd_master_process_weak_strong_client_cmd_data(wbd_master_info_t *master_info,
	i5_dm_clients_type *i5_assoc_sta, wbd_cmd_weak_strong_client_resp_t *cmdclientresp,
	wbd_sta_status_t command)
{
	int ret = WBDE_OK;
	wbd_blanket_master_t *wbd_master;
	wbd_assoc_sta_item_t *assoc_sta;
	wbd_sta_bounce_detect_t *bounce_sta_entry;
	wbd_bounce_detect_t *bounce_cfg;
	wbd_prb_sta_t *prbsta;
	i5_dm_interface_type *i5_sta_ifr;
	i5_dm_bss_type *i5_bss;
	unsigned int fh_bss_count, b_bss_count;
	WBD_ENTER();

	/* Validate arg */
	WBD_ASSERT_ARG(master_info, WBDE_INV_ARG);
	WBD_ASSERT_ARG(i5_assoc_sta->vndr_data, WBDE_INV_ARG);
	wbd_master = master_info->parent;

	i5_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_assoc_sta);

	/* Update the bouncing table */
	wbd_ds_update_sta_bounce_table(master_info->parent);

	/* Choose the bounce detect config based on the STA type */
	if (I5_CLIENT_IS_BSTA(i5_assoc_sta)) {
		bounce_cfg = &wbd_master->bounce_cfg_bh;
	} else {
		bounce_cfg = &wbd_master->bounce_cfg;
	}

	/* Fetch the STA entry from bouncing table,
	 * if sta is present and sta state is WBD_BOUNCE_DWELL_STATE send dwell time to slave
	 * which in turn will inform to BSD.
	 */
	bounce_sta_entry = wbd_ds_find_sta_in_bouncing_table(wbd_master,
		(struct ether_addr*)i5_assoc_sta->mac);
	if ((bounce_cfg->cnt == 0) ||
		((bounce_sta_entry != NULL) &&
		(bounce_sta_entry->state == WBD_BOUNCE_DWELL_STATE))) {
		cmdclientresp->dwell_time = ((bounce_cfg->cnt == 0) ?
			bounce_cfg->dwell_time :
			(bounce_cfg->dwell_time - bounce_sta_entry->run.dwell_time));
		WBD_INFO("BKTID[%d] Slave["MACDBG"] STA["MACDBG"] is bouncing STA till %d "
			"seconds\n",
			master_info->bkt_id, MAC2STRDBG(i5_bss->BSSID),
			MAC2STRDBG(i5_assoc_sta->mac), cmdclientresp->dwell_time);
		ret = WBDE_DS_BOUNCING_STA;
		goto end;
	}

	/* Get Assoc item pointer */
	assoc_sta = i5_assoc_sta->vndr_data;

	if (I5_CLIENT_IS_BSTA(i5_assoc_sta)) {
		/* If the backhaul optimization is running, then do not honor weak client */
		if (WBD_MINFO_IS_BH_OPT_RUNNING(master_info->flags)) {
			WBD_INFO("BKTID[%d] Slave["MACDBG"] For BHSTA["MACDBG"]. %s\n",
				master_info->bkt_id, MAC2STRDBG(i5_bss->BSSID),
				MAC2STRDBG(i5_assoc_sta->mac), wbderrorstr(WBDE_BH_OPT_RUNNING));
			ret = WBDE_BH_OPT_RUNNING;
			goto end;
		}

		/* Find the interface in the network matching backhaul STA MAC */
		if ((i5_sta_ifr = i5DmFindInterfaceFromNetwork(i5_assoc_sta->mac)) == NULL) {
			WBD_WARNING("bSTA "MACDBG" is not an interface in network. Will not "
				"steer him\n", MAC2STRDBG(i5_assoc_sta->mac));
			ret = WBDE_BH_OPT_RUNNING;
			goto end;
		}

		/* If there is not more than one backhaul BSS, cant steer the backhaul STA */
		b_bss_count = i5DmCountNumOfBSS(NULL, IEEE1905_MAP_FLAG_BACKHAUL);
		/* If the device with STA interface has the backhaul BSS, reduce the backhaul BSS
		 * count
		 */
		b_bss_count -= i5DmCountNumOfBSS(WBD_I5LL_PARENT(i5_sta_ifr),
			IEEE1905_MAP_FLAG_BACKHAUL);
		if (b_bss_count <= 1) {
			WBD_WARNING("BKTID[%d] Slave["MACDBG"] For BHSTA["MACDBG"] Count[%d]. %s\n",
				master_info->bkt_id, MAC2STRDBG(i5_bss->BSSID),
				MAC2STRDBG(i5_assoc_sta->mac), b_bss_count,
				wbderrorstr(WBDE_NO_SLAVE_TO_STEER));
			ret = WBDE_NO_SLAVE_TO_STEER;
			goto end;
		}
		/* Check if the STA can be steered to other backhaul BSS without forming any loop */
		if (!wbd_master_is_bsta_steering_possible(i5_assoc_sta, i5_sta_ifr)) {
			WBD_WARNING("BKTID[%d] Slave["MACDBG"] For BHSTA["MACDBG"] steering is not "
				"possible as it can form a loop\n",
				master_info->bkt_id, MAC2STRDBG(i5_bss->BSSID),
				MAC2STRDBG(i5_assoc_sta->mac));
			ret = WBDE_BH_STEER_LOOP;
			goto end;
		}
	} else {
		/* If there is not more than one fronthaul BSS, cant steer weak/strong client */
		if ((fh_bss_count = wbd_ds_count_fhbss()) <= 1) {
			WBD_WARNING("BKTID[%d] Slave["MACDBG"] For STA["MACDBG"] "
				"FHBSSCount[%d]. %s\n",
				master_info->bkt_id, MAC2STRDBG(i5_bss->BSSID),
				MAC2STRDBG(i5_assoc_sta->mac), fh_bss_count,
				wbderrorstr(WBDE_NO_SLAVE_TO_STEER));
			ret = WBDE_NO_SLAVE_TO_STEER;
			goto end;
		}
	}

	/* If its in ignore list */
	if (assoc_sta->status == WBD_STA_STATUS_IGNORE) {
		WBD_WARNING("BKTID[%d] Slave["MACDBG"] STA["MACDBG"] In Ignore list\n",
			master_info->bkt_id, MAC2STRDBG(i5_bss->BSSID),
			MAC2STRDBG(i5_assoc_sta->mac));
		ret = WBDE_IGNORE_STA;
		goto end;
	}

	/* refresh this STA on probe sta list */
	prbsta = wbd_ds_find_sta_in_probe_sta_table(master_info->parent->parent,
		(struct ether_addr*)i5_assoc_sta->mac, FALSE);
	if (prbsta) {
		prbsta->active = time(NULL);
	} else {
		WBD_WARNING("Warning: probe list has no STA["MACDBG"]\n",
			MAC2STRDBG(i5_assoc_sta->mac));
	}

	/* If the STA is in more than one BSS dont accept weak client. Accept weak client only
	 * After REMOVE_CLIENT_REQ Command is sent and it is removed from the other BSS.
	 * This is to fix the issue where we found that STA was not exists in assoclist of masters.
	 * This happens when we get WEAK_CLIENT from the old AP where the STA is associated. When
	 * we get WEAK_CLIENT, we are removing the STA from assoclist of other BSS's before
	 * adding it to the monitorlist.
	 */
	if (wbd_ds_find_duplicate_sta_in_controller((struct ether_addr*)i5_assoc_sta->mac,
		NULL, NULL) > 1) {
		WBD_WARNING("STA["MACDBG"] BSS["MACDBG"] Exists in More than One BSS.\n",
			MAC2STRDBG(i5_assoc_sta->mac), MAC2STRDBG(i5_bss->BSSID));
		ret = WBDE_DS_DUP_STA;
		goto end;
	}

	/* Add STA in all peer Slaves' Monitor STA List. If the STA is connected
	 * to Fronthaul, add only to Fronthaul BSS
	 */
	ret = wbd_ds_add_sta_in_peer_devices_monitorlist(master_info, i5_assoc_sta);
	if ((ret != WBDE_OK) && (ret != WBDE_DS_STA_EXST)) {
		WBD_INFO("BKTID[%d] Slave["MACDBG"] STA["MACDBG"]. Failed to add to peer slaves "
			"monitorlist\n\n", master_info->bkt_id, MAC2STRDBG(i5_bss->BSSID),
			MAC2STRDBG(i5_assoc_sta->mac));
	}

	/* Update STA Status = Weak/Strong */
	WBD_DS_UP_ASSOCSTA_STATUS(master_info, assoc_sta, command);

	/* Start Updating Last Reported RSSI, if STA is Weak/Strong,
	 * to have Hysterisis condition check
	*/
	if (command == WBD_STA_STATUS_WEAK) {
		assoc_sta->last_weak_rssi = assoc_sta->stats.rssi;
	} else if (command == WBD_STA_STATUS_STRONG) {
		assoc_sta->last_strong_rssi = assoc_sta->stats.rssi;
	}

end: /* Check Master Pointer before using it below */

	WBD_EXIT();
	return ret;
}

/* Prepares the beacon request fields for the STAs connected to Fronthaul BSS */
static int
wbd_master_prepare_beacon_request(wbd_glist_t *rclass_chan_list,
	i5_dm_clients_type *i5_assoc_sta, ieee1905_beacon_request *bcn_req)
{
	int ret = WBDE_OK;
	uint8 idx = 0, idx_ap_chan_report_len;
	i5_dm_interface_type *i5_ifr;
	i5_dm_bss_type *i5_bss;
	wbd_bcn_req_rclass_list_t *rclass_list;
	wbd_bcn_req_chan_list_t *chan_list;
	dll_t *rclass_item_p, *chan_item_p;
	unsigned char ap_chan_report[WBD_MAX_BUF_256];

	i5_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_assoc_sta);
	i5_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_bss);

	memcpy(bcn_req->sta_mac, i5_assoc_sta->mac, sizeof(bcn_req->sta_mac));
	bcn_req->opclass = i5_ifr->opClass;
	bcn_req->channel = 255;
	memcpy(bcn_req->bssid, &ether_bcast, sizeof(bcn_req->bssid));
	memcpy(&bcn_req->ssid, &i5_bss->ssid, sizeof(bcn_req->ssid));

	/* Prepare the AP channel report */
	memset(ap_chan_report, 0, sizeof(ap_chan_report));
	bcn_req->ap_chan_report_count = rclass_chan_list->count;

	/* Traverse br0 AP channel report */
	foreach_glist_item(rclass_item_p, *rclass_chan_list) {
		rclass_list = (wbd_bcn_req_rclass_list_t*)rclass_item_p;

		idx_ap_chan_report_len = idx; /* Place holder for length of one AP chan report */
		idx++;
		ap_chan_report[idx++] = rclass_list->rclass;
		/* Traverse channel list */
		foreach_glist_item(chan_item_p, rclass_list->chan_list) {
			chan_list = (wbd_bcn_req_chan_list_t*)chan_item_p;
			ap_chan_report[idx++] = chan_list->channel;
		}
		ap_chan_report[idx_ap_chan_report_len] = (idx - 1) - idx_ap_chan_report_len;
	}
	bcn_req->ap_chan_report_len = idx;
	bcn_req->ap_chan_report = (unsigned char*)wbd_malloc(idx, &ret);
	WBD_ASSERT();

	memcpy(bcn_req->ap_chan_report, ap_chan_report, idx);

end:
	WBD_EXIT();
	return ret;
}

/* Cretae the rclass and channel list for AP channel report for backhaul STA */
void
wbd_master_create_backhaul_bcn_req_ap_chan_report(wbd_glist_t *rclass_chan_list)
{
	i5_dm_network_topology_type *i5_topology;
	i5_dm_device_type *i5_iter_device;
	i5_dm_interface_type *i5_iter_ifr;
	i5_dm_bss_type *i5_iter_bss;
	WBD_ENTER();

	i5_topology = ieee1905_get_datamodel();

	/* Update the AP chan report with new channel values of all the backhaul interfaces */
	foreach_i5glist_item(i5_iter_device, i5_dm_device_type, i5_topology->device_list) {
		if (!I5_IS_MULTIAP_AGENT(i5_iter_device->flags)) {
			continue;
		}

		foreach_i5glist_item(i5_iter_ifr, i5_dm_interface_type,
			i5_iter_device->interface_list) {

			bool is_backhaul = FALSE;

			/* Only if there is atleast one backhaul BSS */
			foreach_i5glist_item(i5_iter_bss, i5_dm_bss_type,
				i5_iter_ifr->bss_list) {

				if (I5_IS_BSS_BACKHAUL(i5_iter_bss->mapFlags)) {
					is_backhaul = TRUE;
					break;
				}
			}

			if (is_backhaul) {
				if (i5_iter_ifr->opClass == 0 && i5_iter_ifr->chanspec != 0) {
					blanket_get_global_rclass(i5_iter_ifr->chanspec,
						&i5_iter_ifr->opClass);
					WBD_INFO("Device["MACDBG"] IFR["MACDBG"] rclass was empty "
						"calculated it, rclass[%d] chanspec[0x%x]\n",
						MAC2STRDBG(i5_iter_device->DeviceId),
						MAC2STRDBG(i5_iter_ifr->InterfaceId),
						i5_iter_ifr->opClass, i5_iter_ifr->chanspec);
				}
				wbd_master_update_ap_chan_report(rclass_chan_list, i5_iter_ifr);
			}
		}
	}
}

/* Prepares the beacon request fields for the backhaul STA */
static int
wbd_master_prepare_backhaul_beacon_request(i5_dm_clients_type *i5_assoc_sta,
	ieee1905_beacon_request *bcn_req)
{
	int ret = WBDE_OK;
	wbd_glist_t rclass_chan_list;

	/* For the backhaul STA create the AP channel report and free it after use as it is not
	 * used frequently. No need to store it
	 */
	wbd_ds_glist_init(&rclass_chan_list);
	wbd_master_create_backhaul_bcn_req_ap_chan_report(&rclass_chan_list);

	wbd_master_prepare_beacon_request(&rclass_chan_list, i5_assoc_sta, bcn_req);

	wbd_ds_ap_chan_report_cleanup(&rclass_chan_list);

	WBD_EXIT();
	return ret;
}

/* Check if the device is operating on a particular channel */
static int
wbd_master_is_agent_operating_on_channel(i5_dm_device_type *i5_device, uint8 channel)
{
	i5_dm_interface_type *i5_ifr;

	/* Iterate through wireless interfaces */
	foreach_i5glist_item(i5_ifr, i5_dm_interface_type, i5_device->interface_list) {
		if (!i5DmIsInterfaceWireless(i5_ifr->MediaType)) {
			continue;
		}

		/* Compare the channel */
		if (channel == wf_chspec_ctlchan(i5_ifr->chanspec)) {
			return 1;
		}
	}

	return 0;
}

/* Sends the Associated/Unassociated link metrics and beacon metrics for the STA */
int
wbd_master_send_link_metric_requests(wbd_master_info_t *master_info,
	i5_dm_clients_type *i5_assoc_sta)
{
	int ret = WBDE_OK;
	i5_dm_network_topology_type *i5_topology;
	i5_dm_device_type *i5_device, *i5_iter_device, *i5_self_device;
	i5_dm_interface_type *i5_ifr;
	i5_dm_bss_type *i5_bss;
	ieee1905_beacon_request bcn_req;
	ieee1905_unassoc_sta_link_metric_query uassoc_req;
	unassoc_query_per_chan_rqst *per_chan_rqst = NULL;
	time_t now = time(NULL);
	wbd_beacon_reports_t *wbd_bcn_rpt = NULL;
	wbd_device_item_t *device_vndr;
	wbd_info_t *info = master_info->parent->parent;
	WBD_ENTER();

	memset(&bcn_req, 0, sizeof(bcn_req));
	memset(&uassoc_req, 0, sizeof(uassoc_req));

	i5_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_assoc_sta);
	i5_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_bss);
	i5_device = (i5_dm_device_type*)WBD_I5LL_PARENT(i5_ifr);
	WBD_ASSERT_ARG(i5_bss->vndr_data, WBDE_INV_ARG);

	WBD_SAFE_GET_I5_SELF_DEVICE(i5_self_device, &ret);

	/* Send Associated STA Link Metrics Query message to the agent where STA associated */
	ieee1905_send_assoc_sta_link_metric_query(i5_device->DeviceId, i5_assoc_sta->mac);

	/* Check whether to send beacon metrics query or not */
	if (!WBD_BKT_DONT_SEND_BCN_QRY(master_info->parent->flags)) {
		/* Check if we already have beacon report from this sta */
		wbd_bcn_rpt = wbd_ds_find_item_fm_beacon_reports(info,
			(struct ether_addr*)&(i5_assoc_sta->mac), &ret);

		if (wbd_bcn_rpt &&
			(!eacmp(&(i5_device->DeviceId), &(wbd_bcn_rpt->neighbor_al_mac))) &&
			((now - wbd_bcn_rpt->timestamp) < info->max.tm_per_chan_bcn_req)) {
			/* Already present reports can be sent */
			WBD_DEBUG("Reports are available for sta["MACDBG"] on device["MACDBG"]"
				"now[%lu]  timestamp[%lu] diff[%lu]\n",
				MAC2STRDBG(i5_assoc_sta->mac), MAC2STRDBG(i5_device->DeviceId),
				now, wbd_bcn_rpt->timestamp, (now - wbd_bcn_rpt->timestamp));
		} else {
			if (wbd_bcn_rpt) {
				wbd_ds_remove_beacon_report(info,
					(struct ether_addr *)&(i5_assoc_sta->mac));
			}
			/* Send Beacon Metrics Query message to the agent where STA associated */
			if (I5_CLIENT_IS_BSTA(i5_assoc_sta)) {
				ret = wbd_master_prepare_backhaul_beacon_request(i5_assoc_sta,
					&bcn_req);
			} else {
				ret = wbd_master_prepare_beacon_request(
					&master_info->ap_chan_report, i5_assoc_sta, &bcn_req);
			}
			if (ret == WBDE_OK) {
				ieee1905_send_beacon_metrics_query(i5_device->DeviceId, &bcn_req);
			}
		}
	}

	/* Send Unassociated STA Link Metrics Query message to all the other agents */
	uassoc_req.opClass = i5_ifr->opClass;
	uassoc_req.chCount = 1;

	uassoc_req.data = (unassoc_query_per_chan_rqst*)wbd_malloc(
		(sizeof(unassoc_query_per_chan_rqst)), &ret);
	WBD_ASSERT();
	per_chan_rqst = uassoc_req.data;

	per_chan_rqst->chan = wf_chspec_ctlchan(i5_ifr->chanspec);
	per_chan_rqst->n_sta = 1;

	per_chan_rqst->mac_list = (unsigned char*)wbd_malloc(ETHER_ADDR_LEN, &ret);
	WBD_ASSERT();
	memcpy(per_chan_rqst->mac_list, i5_assoc_sta->mac, ETHER_ADDR_LEN);

	i5_topology = ieee1905_get_datamodel();
	i5_iter_device = (i5_dm_device_type *)WBD_I5LL_HEAD(i5_topology->device_list);
	/* Traverse Device List to send it to all the device in the network */
	while (i5_iter_device != NULL) {
		uint8 send_unassoc_query = 1;

		/* Send it only to agent */
		if (I5_IS_MULTIAP_AGENT(i5_iter_device->flags) && i5_iter_device->vndr_data) {
			device_vndr = i5_iter_device->vndr_data;

			/* Check if it supports unassociated STA link metrics reporting ot not */
			if (i5_iter_device->BasicCaps & IEEE1905_AP_CAPS_FLAGS_UNASSOC_RPT) {
				/* if the device does not support unassociated STA link metrics on
				 * the channel its BSSs are not working. Check is there any
				 * interface is working on the same channel. If not do not send
				 * unassociated STA link metrics query
				 */
				if (!(i5_iter_device->BasicCaps &
					IEEE1905_AP_CAPS_FLAGS_UNASSOC_RPT_NON_CH)) {
					if (!wbd_master_is_agent_operating_on_channel(
						i5_iter_device, per_chan_rqst->chan)) {
						send_unassoc_query = 0;
						WBD_INFO("Device["MACDBG"] Does not support "
							"Unassociated STA Link Metrics reporting "
							"on channels its BSSs are not currently "
							"operating on CAPS[0x%02x]. "
							"Requested[0x%x]\n",
							MAC2STRDBG(i5_iter_device->DeviceId),
							i5_iter_device->BasicCaps,
							i5_ifr->chanspec);
					}
				}
			} else {
				send_unassoc_query = 0;
				WBD_INFO("Device["MACDBG"] Does not support Unassociated STA Link "
					"Metrics reporting CAPS[0x%02x].\n",
					MAC2STRDBG(i5_iter_device->DeviceId),
					i5_iter_device->BasicCaps);
			}

			if (send_unassoc_query) {
				ieee1905_send_unassoc_sta_link_metric_query(
					i5_iter_device->DeviceId, &uassoc_req);
			}

			/* If the neighbor link metrics query is already sent to this device within
			 * TBSS time, dont send it again
			 */
			if ((now - device_vndr->nbrlinkmetrics_timestamp) >= info->max.tm_wd_tbss) {
				/* Send the neighbor link metrics query requesting the link metrics
				 * to the self device(Controller)
				 */
				ieee1905_send_neighbor_link_metric_query(i5_iter_device->DeviceId,
					0, i5_self_device->DeviceId);
				device_vndr->nbrlinkmetrics_timestamp = now;
			}
		}
		i5_iter_device = WBD_I5LL_NEXT(i5_iter_device);
	}

end:
	/* free allocated data */
	if (bcn_req.ap_chan_report) {
		free(bcn_req.ap_chan_report);
	}

	if (uassoc_req.data) {
		per_chan_rqst = uassoc_req.data;
		if (per_chan_rqst->mac_list) {
			free(per_chan_rqst->mac_list);
		}
		free(uassoc_req.data);
	}

	WBD_EXIT();
	return ret;
}

/* Send 1905 Vendor Specific Weak/Strong Client Response command, from Controller to the Agent */
static int
wbd_master_send_weak_strong_client_response_cmd(wbd_cmd_weak_strong_client_resp_t *cmdclientresp)
{
	int ret = WBDE_OK;
	ieee1905_vendor_data vndr_msg_data;
	WBD_ENTER();

	memset(&vndr_msg_data, 0x00, sizeof(vndr_msg_data));

	/* Allocate Dynamic mem for Vendor data from App */
	vndr_msg_data.vendorSpec_msg =
		(unsigned char *)wbd_malloc(IEEE1905_MAX_VNDR_DATA_BUF, &ret);
	WBD_ASSERT();

	/* Fill vndr_msg_data struct object to send Vendor Message */
	memcpy(vndr_msg_data.neighbor_al_mac,
		&cmdclientresp->cmdparam.dst_mac, IEEE1905_MAC_ADDR_LEN);

	WBD_INFO("Send Weak/strong Client Response from Device["MACDBG"] to Device["MACDBG"]\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(vndr_msg_data.neighbor_al_mac));

	/* Encode Vendor Specific TLV for Message : Weak/Strong Client Response to send */
	ret = wbd_tlv_encode_weak_strong_client_response((void *)cmdclientresp,
		vndr_msg_data.vendorSpec_msg, &vndr_msg_data.vendorSpec_len);
	WBD_ASSERT_MSG("Failed to encode Weak/Strong Client Response which needs to be sent "
		"from Device["MACDBG"] to Device["MACDBG"]\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(vndr_msg_data.neighbor_al_mac));

	/* Send Vendor Specific Message : Weak/strong Client Response */
	wbd_master_send_brcm_vndr_msg(&vndr_msg_data);

	WBD_CHECK_ERR_MSG(WBDE_DS_1905_ERR, "Failed to send Weak/strong "
		"Client Response from Device["MACDBG"] to Device["MACDBG"], Error : %s\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(vndr_msg_data.neighbor_al_mac),
		wbderrorstr(ret));

end:
	if (vndr_msg_data.vendorSpec_msg) {
		free(vndr_msg_data.vendorSpec_msg);
	}
	WBD_EXIT();
	return ret;
}

/* Processes WEAK_CLIENT request */
static int
wbd_master_process_weak_client_cmd(wbd_master_info_t *master_info, i5_dm_clients_type *i5_assoc_sta,
	wbd_weak_sta_metrics_t *sta_stats, wbd_weak_sta_policy_t *metric_policy)
{
	int ret = WBDE_OK;
	wbd_cmd_weak_strong_client_resp_t cmdweakclientresp;
	i5_dm_bss_type *i5_bss;
	i5_dm_interface_type *i5_ifr;
	i5_dm_device_type *i5_device;
	char logmsg[WBD_LOGS_BUF_128] = {0}, timestamp[WBD_MAX_BUF_32] = {0};
	WBD_ENTER();

	i5_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_assoc_sta);
	i5_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_bss);
	i5_device = (i5_dm_device_type*)WBD_I5LL_PARENT(i5_ifr);

	/* process the WEAK_CLIENT data */
	memset(&cmdweakclientresp, 0x00, sizeof(cmdweakclientresp));
	ret = wbd_master_process_weak_strong_client_cmd_data(master_info, i5_assoc_sta,
		&cmdweakclientresp, WBD_STA_STATUS_WEAK);
	WBD_ASSERT();

end: /* Check Master Pointer before using it below */

	if ((ret == WBDE_OK) && (master_info)) {
		/* Create and store weak client log. */
		snprintf(logmsg, sizeof(logmsg), CLI_CMD_LOGS_WEAK,
			wbd_get_formated_local_time(timestamp, sizeof(timestamp)),
			MAC2STRDBG(i5_assoc_sta->mac), MAC2STRDBG(i5_bss->BSSID),
			sta_stats->rssi, metric_policy->t_rssi,
			sta_stats->tx_rate, metric_policy->t_tx_rate,
			sta_stats->tx_failures, metric_policy->t_tx_failures);
		wbd_ds_add_logs_in_master(master_info->parent, logmsg);
	}

#ifdef BCM_APPEVENTD
	if (ret == WBDE_OK) {
		/* Send weak client event to appeventd. */
		wbd_appeventd_weak_sta(APP_E_WBD_MASTER_WEAK_CLIENT, " ",
			(struct ether_addr*)i5_assoc_sta->mac, sta_stats->rssi,
			sta_stats->tx_failures, sta_stats->tx_rate);
	}
#endif /* BCM_APPEVENTD */

	/* Send vendor specific response to the agent if the weak client is not successfull */
	if (ret != WBDE_OK) {
		eacopy((struct ether_addr*)&i5_device->DeviceId,
			&cmdweakclientresp.cmdparam.dst_mac);
		eacopy((struct ether_addr*)&i5_assoc_sta->mac, &cmdweakclientresp.sta_mac);
		eacopy((struct ether_addr*)&i5_bss->BSSID, &cmdweakclientresp.BSSID);
		/* Convert wbd error code to weak client response reason codes */
		cmdweakclientresp.error_code = wbd_error_to_wc_resp_reason_code(ret);
		if (wbd_master_send_weak_strong_client_response_cmd(&cmdweakclientresp) !=
			WBDE_OK) {
			WBD_WARNING("BKTID[%d] Slave["MACF"] STA["MACF"] WEAK_CLIENT_RESP : %s\n",
				master_info->bkt_id,
				ETHER_TO_MACF(cmdweakclientresp.BSSID),
				ETHER_TO_MACF(cmdweakclientresp.sta_mac),
				wbderrorstr(WBDE_SEND_RESP_FL));
		}
	}

	/* Now send the link metrics if required */
	if ((ret == WBDE_OK) && (master_info)) {
		ret = wbd_master_send_link_metric_requests(master_info, i5_assoc_sta);
	}

	WBD_EXIT();
	return ret;
}

/* Processes STRONG_CLIENT request */
static int
wbd_master_process_strong_client_cmd(wbd_master_info_t *master_info,
	i5_dm_clients_type *i5_assoc_sta, wbd_strong_sta_metrics_t *sta_stats,
	wbd_strong_sta_policy_t *metric_policy)
{
	int ret = WBDE_OK;
	wbd_cmd_weak_strong_client_resp_t cmdstrongclientresp;
	i5_dm_bss_type *i5_bss;
	i5_dm_interface_type *i5_ifr;
	i5_dm_device_type *i5_device;
	char logmsg[WBD_LOGS_BUF_128] = {0}, timestamp[WBD_MAX_BUF_32] = {0};
	WBD_ENTER();

	i5_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_assoc_sta);
	i5_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_bss);
	i5_device = (i5_dm_device_type*)WBD_I5LL_PARENT(i5_ifr);

	/* process the STRONG_CLIENT data */
	memset(&cmdstrongclientresp, 0x00, sizeof(cmdstrongclientresp));
	ret = wbd_master_process_weak_strong_client_cmd_data(master_info, i5_assoc_sta,
		&cmdstrongclientresp, WBD_STA_STATUS_STRONG);
	WBD_ASSERT();

end: /* Check Master Pointer before using it below */

	if ((ret == WBDE_OK) && (master_info)) {
		/* Create and store strong client log. */
		snprintf(logmsg, sizeof(logmsg), CLI_CMD_LOGS_STRONG,
			wbd_get_formated_local_time(timestamp, sizeof(timestamp)),
			MAC2STRDBG(i5_assoc_sta->mac), MAC2STRDBG(i5_bss->BSSID),
			sta_stats->rssi, metric_policy->t_rssi);
		wbd_ds_add_logs_in_master(master_info->parent, logmsg);
	}

#ifdef BCM_APPEVENTD
	if (ret == WBDE_OK) {
		/* Send Strong client event to appeventd. */
		wbd_appeventd_weak_sta(APP_E_WBD_MASTER_STRONG_CLIENT, " ",
			(struct ether_addr*)i5_assoc_sta->mac, sta_stats->rssi,	0, 0);
	}
#endif /* BCM_APPEVENTD */

	/* Send vendor specific response to the agent if the strong client is not successfull */
	if (ret != WBDE_OK) {
		eacopy((struct ether_addr*)&i5_device->DeviceId,
			&cmdstrongclientresp.cmdparam.dst_mac);
		eacopy((struct ether_addr*)&i5_assoc_sta->mac, &cmdstrongclientresp.sta_mac);
		eacopy((struct ether_addr*)&i5_bss->BSSID, &cmdstrongclientresp.BSSID);
		/* Convert wbd error code to strong client response reason codes */
		cmdstrongclientresp.error_code = wbd_error_to_wc_resp_reason_code(ret);
		if (wbd_master_send_weak_strong_client_response_cmd(&cmdstrongclientresp) !=
			WBDE_OK) {
			WBD_WARNING("BKTID[%d] Slave["MACF"] STA["MACF"] STRONG_CLIENT_RESP : %s\n",
				master_info->bkt_id,
				ETHER_TO_MACF(cmdstrongclientresp.BSSID),
				ETHER_TO_MACF(cmdstrongclientresp.sta_mac),
				wbderrorstr(WBDE_SEND_RESP_FL));
		}
	}

	/* Now send the link metrics if required */
	if ((ret == WBDE_OK) && (master_info)) {
		ret = wbd_master_send_link_metric_requests(master_info, i5_assoc_sta);
	}

	WBD_EXIT();
	return ret;
}

/* Processes WEAK_CANCEL request */
static void
wbd_master_process_weak_cancel_cmd(wbd_master_info_t *master, i5_dm_clients_type *i5_assoc_sta)
{
	int ret = WBDE_OK;
	wbd_cmd_weak_strong_client_resp_t cmdweakclientresp;
	i5_dm_bss_type *i5_bss;
	i5_dm_interface_type *i5_ifr;
	i5_dm_device_type *i5_device;
	wbd_assoc_sta_item_t *assoc_sta;
	WBD_ENTER();

	WBD_ASSERT_ARG(i5_assoc_sta->vndr_data, WBDE_INV_ARG);

	i5_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_assoc_sta);
	i5_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_bss);
	i5_device = (i5_dm_device_type*)WBD_I5LL_PARENT(i5_ifr);
	assoc_sta = i5_assoc_sta->vndr_data;

	memset(&cmdweakclientresp, 0x00, sizeof(cmdweakclientresp));

	/* Update STA Status = Normal */
	WBD_DS_UP_ASSOCSTA_STATUS(master, assoc_sta, WBD_STA_STATUS_NORMAL);
	WBD_DEBUG("BKTID[%d] Blanket Weak STA Count : %d\n", master->bkt_id,
		master->weak_client_count);

	/* Remove STA from all peer BSS' Monitor STA List */
	if (wbd_ds_remove_sta_fm_peer_devices_monitorlist(
		(struct ether_addr*)i5_bss->BSSID,
		(struct ether_addr*)i5_assoc_sta->mac, &i5_bss->ssid) != WBDE_OK) {
		WBD_INFO("BSS["MACDBG"] STA["MACDBG"]. Failed to remove STA from "
			"peer slaves monitorlist\n",
			MAC2STRDBG(i5_bss->BSSID), MAC2STRDBG(i5_assoc_sta->mac));
	}

	/* Send the response as failed by default as it helps when the STA is weak in the BSD but
	 * in the master it is not detected as weak for some reason. After BSD sends weak message,
	 * it will not try to send the weak message again unless there is a fail.
	 */
	eacopy((struct ether_addr*)&i5_device->DeviceId,
		&cmdweakclientresp.cmdparam.dst_mac);
	eacopy((struct ether_addr*)&i5_assoc_sta->mac, &cmdweakclientresp.sta_mac);
	eacopy((struct ether_addr*)&i5_bss->BSSID, &cmdweakclientresp.BSSID);
	/* Convert wbd error code to weak cancel response reason codes */
	cmdweakclientresp.error_code = wbd_error_to_wc_resp_reason_code(WBDE_FAIL_XX);
	if (wbd_master_send_weak_strong_client_response_cmd(&cmdweakclientresp) != WBDE_OK) {
		WBD_WARNING("BKTID[%d] Slave["MACF"] STA["MACF"] WEAK_CANCEL_RESP : %s\n",
			master->bkt_id,
			ETHER_TO_MACF(cmdweakclientresp.BSSID),
			ETHER_TO_MACF(cmdweakclientresp.sta_mac),
			wbderrorstr(WBDE_SEND_RESP_FL));
	}

end:
	WBD_EXIT();
}

#ifdef WLHOSTFBT
/* Get FBT_CONFIG_RESP Data from 1905 Device */
static int
wbd_master_get_fbt_config_resp_data(wbd_cmd_fbt_config_t *fbt_config_resp)
{
	int ret = WBDE_OK;
	dll_t *fbt_item_p = NULL;
	wbd_fbt_bss_entry_t *fbt_data = NULL;
	wbd_fbt_bss_entry_t new_fbt_bss;
	wbd_info_t *info = wbd_get_ginfo();
	wbd_cmd_fbt_config_t *ctlr_fbt_config = &(info->wbd_fbt_config);
	WBD_ENTER();

	/* Initialize fbt_config_resp's Total item count in caller  */
	WBD_INFO("Get FBT Config Response Data\n");

	/* Travese FBT Information of all BSSes in this Blanket */
	foreach_glist_item(fbt_item_p, ctlr_fbt_config->entry_list) {

		fbt_data = (wbd_fbt_bss_entry_t *)fbt_item_p;

		WBD_INFO("WBD_FBT_ENAB Enabled for  : "
			"Blanket[BR%d] DEVICE["MACDBG"] BRDG["MACDBG"] BSS["MACDBG"] "
			"MDID[%d] FT_CAP[%d] FT_REASSOC[%d]\n",
			fbt_data->blanket_id, MAC2STRDBG(fbt_data->al_mac),
			MAC2STRDBG(fbt_data->bss_br_mac), MAC2STRDBG(fbt_data->bssid),
			fbt_data->fbt_info.mdid,
			fbt_data->fbt_info.ft_cap_policy,
			fbt_data->fbt_info.tie_reassoc_interval);

		/* Prepare a new FBT BSS item for Controller's FBT Response List */
		memset(&new_fbt_bss, 0, sizeof(new_fbt_bss));
		memset(&(new_fbt_bss.fbt_info), 0, sizeof(new_fbt_bss.fbt_info));

		new_fbt_bss.blanket_id = fbt_data->blanket_id;
		memcpy(new_fbt_bss.al_mac, fbt_data->al_mac,
			sizeof(new_fbt_bss.al_mac));
		memcpy(new_fbt_bss.bssid, fbt_data->bssid,
			sizeof(new_fbt_bss.bssid));
		memcpy(new_fbt_bss.bss_br_mac, fbt_data->bss_br_mac,
			sizeof(new_fbt_bss.bss_br_mac));

		new_fbt_bss.len_r0kh_id = strlen(fbt_data->r0kh_id);
		memcpy(new_fbt_bss.r0kh_id, fbt_data->r0kh_id,
			sizeof(new_fbt_bss.r0kh_id));
		new_fbt_bss.len_r0kh_key = strlen(fbt_data->r0kh_key);
		memcpy(new_fbt_bss.r0kh_key, fbt_data->r0kh_key,
			sizeof(new_fbt_bss.r0kh_key));

		/* Assign FBT info, while sending FBT_CONFIG_RESP */
		new_fbt_bss.fbt_info.mdid = fbt_data->fbt_info.mdid;
		new_fbt_bss.fbt_info.ft_cap_policy = fbt_data->fbt_info.ft_cap_policy;
		new_fbt_bss.fbt_info.tie_reassoc_interval =
			fbt_data->fbt_info.tie_reassoc_interval;

		WBD_INFO("Adding new entry to FBT_CONFIG_RESP : "
			"Blanket[BR%d] Device["MACDBG"] "
			"BRDG["MACDBG"] BSS["MACDBG"] "
			"R0KH_ID[%s] LEN_R0KH_ID[%d] "
			"R0KH_Key[%s] LEN_R0KH_Key[%d] "
			"MDID[%d] FT_CAP[%d] FT_REASSOC[%d]\n",
			new_fbt_bss.blanket_id,
			MAC2STRDBG(new_fbt_bss.al_mac),
			MAC2STRDBG(new_fbt_bss.bss_br_mac),
			MAC2STRDBG(new_fbt_bss.bssid),
			new_fbt_bss.r0kh_id, new_fbt_bss.len_r0kh_id,
			new_fbt_bss.r0kh_key, new_fbt_bss.len_r0kh_key,
			new_fbt_bss.fbt_info.mdid,
			new_fbt_bss.fbt_info.ft_cap_policy,
			new_fbt_bss.fbt_info.tie_reassoc_interval);

		/* Add a FBT BSS item in a Slave's FBT Response List, Total item count++ */
		wbd_add_item_in_fbt_cmdlist(&new_fbt_bss,
			fbt_config_resp, NULL, FALSE);
	}

	WBD_EXIT();
	return ret;
}

/* Check if the device has sent FBT request or not. If it has sent FBT request, then AL MAC address
 * entry will be present in the FBT list. Note: more than one entry can have same AL MAC as it is
 * per BSS
 */
static int
wbd_is_device_sent_fbt_req(unsigned char *al_mac, wbd_cmd_fbt_config_t *fbt_config)
{
	dll_t *fbt_item_p = NULL;
	wbd_fbt_bss_entry_t *fbt_data = NULL;
	int found = 0;
	WBD_ENTER();

	/* Travese FBT Config Response List items */
	foreach_glist_item(fbt_item_p, fbt_config->entry_list) {

		fbt_data = (wbd_fbt_bss_entry_t *)fbt_item_p;

		/* Check if the AL MAC address is matching */
		if (memcmp(al_mac, fbt_data->al_mac, MAC_ADDR_LEN) == 0) {
			found = 1;
			goto end;
		}
	}

end:
	WBD_EXIT();
	return found;
}

/* Send 1905 Vendor Specific FBT_CONFIG_RESP command, from Controller to Agents */
static int
wbd_master_broadcast_fbt_config_resp_cmd()
{
	int ret = WBDE_OK;

	ieee1905_vendor_data vndr_msg_data, vndr_msg_data_split;
	wbd_cmd_fbt_config_t fbt_config_resp; /* FBT Config Response Data */
	i5_dm_network_topology_type *ctlr_topology;
	i5_dm_device_type *iter_dev, *self_device;
	wbd_cmd_fbt_config_t *ctlr_fbt_config = &(wbd_get_ginfo()->wbd_fbt_config);
	uint mem_len;
	WBD_ENTER();

	memset(&vndr_msg_data, 0, sizeof(vndr_msg_data));

	/* Initialize FBT Config Response Data */
	memset(&fbt_config_resp, 0, sizeof(fbt_config_resp));
	wbd_ds_glist_init(&(fbt_config_resp.entry_list));

	/* Get FBT Config Response Data from 1905 Device */
	wbd_master_get_fbt_config_resp_data(&fbt_config_resp);

	/* Calculate memory size to be allocated for FBT_CONFIG_RESP */
	mem_len = (fbt_config_resp.entry_list.count * WBD_FBT_BSS_ENTRY_LEN) +
		WBD_MAX_BUF_16;

	/* Allocate Dynamic mem for Vendor data from App */
	vndr_msg_data.vendorSpec_msg =
		(unsigned char *)wbd_malloc(IEEE1905_MAX_VNDR_DATA_BUF, &ret);
	WBD_ASSERT();

	/* Allocate Dynamic mem for Vendor data from App */
	vndr_msg_data_split.vendorSpec_msg =
		(unsigned char *)wbd_malloc(mem_len, &ret);
	WBD_ASSERT();

	/* Encode Vendor Specific TLV for Message : FBT_CONFIG_RESP to send */
	wbd_tlv_encode_fbt_config_response((void *)&fbt_config_resp,
		vndr_msg_data_split.vendorSpec_msg, &vndr_msg_data_split.vendorSpec_len, TRUE);

	/* Encode Vendor Specific TLV for Message : FBT_CONFIG_RESP to send to agents
	 * who dont support vendor message split
	*/
	wbd_tlv_encode_fbt_config_response((void *)&fbt_config_resp,
		vndr_msg_data.vendorSpec_msg, &vndr_msg_data.vendorSpec_len, FALSE);

	/* Get Topology of Controller from 1905 lib */
	ctlr_topology = (i5_dm_network_topology_type *)ieee1905_get_datamodel();

	WBD_SAFE_GET_I5_SELF_DEVICE(self_device, &ret);

	/* Loop for all the Devices in Controller, to send FBT_CONFIG_RESP to all Agents */
	foreach_i5glist_item(iter_dev, i5_dm_device_type, ctlr_topology->device_list) {

		/* Loop for, other than self (Controller) device, only for Agent Devices */
		if (memcmp(self_device->DeviceId, iter_dev->DeviceId, MAC_ADDR_LEN) == 0) {
			continue;
		}

		/* If this device not sent any FBT_CONFIG_REQ, do not send the response as the BSS
		 * information for that device will not be there in the list and it will
		 * unnecessarily disable the FBT
		 */
		if (!wbd_is_device_sent_fbt_req(iter_dev->DeviceId, ctlr_fbt_config)) {
			WBD_INFO("Do not Send FBT_CONFIG_RESP to Device["MACDBG"]. As this device "
				"never sent FBT_CONFIG_REQ\n\n", MAC2STRDBG(iter_dev->DeviceId));
			continue;
		}

		/* Fill Destination AL_MAC */
		memcpy(vndr_msg_data.neighbor_al_mac, iter_dev->DeviceId, IEEE1905_MAC_ADDR_LEN);
		memcpy(vndr_msg_data_split.neighbor_al_mac, iter_dev->DeviceId,
			IEEE1905_MAC_ADDR_LEN);

		WBD_INFO("Send FBT_CONFIG_RESP from Device["MACDBG"] to Device["MACDBG"]\n",
			MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(iter_dev->DeviceId));

		if (I5_IS_SPLIT_VNDR_MSG(iter_dev->flags)) {
			/* Send Vendor Specific Message : FBT_CONFIG_RESP */
			wbd_master_send_brcm_vndr_msg(&vndr_msg_data_split);
		} else {
			/* Send Vendor Specific Message : FBT_CONFIG_RESP
			 * to agents who dont support split vendor message
			*/
			wbd_master_send_brcm_vndr_msg(&vndr_msg_data);
		}

		WBD_CHECK_ERR_MSG(WBDE_DS_1905_ERR, "Failed to send "
			"FBT_CONFIG_RESP from Device["MACDBG"], Error : %s\n",
			MAC2STRDBG(ieee1905_get_al_mac()), wbderrorstr(ret));
	}

end:
	/* Remove all FBT Config Response data items */
	wbd_ds_glist_cleanup(&(fbt_config_resp.entry_list));

	if (vndr_msg_data.vendorSpec_msg) {
		free(vndr_msg_data.vendorSpec_msg);
	}

	if (vndr_msg_data_split.vendorSpec_msg) {
		free(vndr_msg_data_split.vendorSpec_msg);
	}

	WBD_EXIT();
	return ret;

}

/* Process 1905 Vendor Specific FBT_CONFIG_REQ Message */
static int
wbd_master_process_fbt_config_req_cmd(unsigned char *neighbor_al_mac,
	unsigned char *tlv_data, unsigned short tlv_data_len)
{
	int ret = WBDE_OK;
	wbd_cmd_fbt_config_t fbt_config_req; /* FBT Config Request Data */
	wbd_info_t *info = wbd_get_ginfo();
	wbd_cmd_fbt_config_t *ctlr_fbt_config = &(info->wbd_fbt_config);
	dll_t *fbt_item_p = NULL, *fbt_item_next_p = NULL;
	wbd_fbt_bss_entry_t *fbt_data = NULL;
	char logmsg[WBD_MAX_BUF_128] = {0}, timestamp[WBD_MAX_BUF_32] = {0};
	ieee1905_client_bssinfo_type *bss_table;
	i5_dm_bss_type *i5_bss;
	i5_dm_device_type *src_device;

	WBD_ENTER();

	/* Initialize FBT Config Request Data */
	memset(&fbt_config_req, 0, sizeof(fbt_config_req));
	wbd_ds_glist_init(&(fbt_config_req.entry_list));

	WBD_SAFE_FIND_I5_DEVICE(src_device, neighbor_al_mac, &ret);

	/* Decode Vendor Specific TLV for Message : FBT_CONFIG_REQ on receive */
	ret = wbd_tlv_decode_fbt_config_request((void *)&fbt_config_req, tlv_data, tlv_data_len,
		src_device);
	WBD_ASSERT_MSG("Failed to decode FBT Config Request From DEVICE["MACDBG"]\n",
		MAC2STRDBG(neighbor_al_mac));

	/* Travese FBT Config Request List items */
	foreach_glist_item(fbt_item_p, fbt_config_req.entry_list) {

		wbd_fbt_bss_entry_t *matching_fbt_bss = NULL;

		fbt_data = (wbd_fbt_bss_entry_t *)fbt_item_p;
		memcpy(fbt_data->al_mac, neighbor_al_mac, sizeof(fbt_data->al_mac));

		/* Find matching fbt_info for this bssid */
		matching_fbt_bss = wbd_find_fbt_bss_item_for_bssid(fbt_data->bssid,
			ctlr_fbt_config);

		/* If not FOUND, Create Add Received fbt_info for this bssid */
		if (matching_fbt_bss) {
			WBD_INFO("Master Found Matching AL_MAC["MACDBG"] BSSID["MACDBG"]\n",
				MAC2STRDBG(fbt_data->al_mac), MAC2STRDBG(fbt_data->bssid));
		} else {

			WBD_INFO("Master NOT Found Matching AL_MAC["MACDBG"] BSSID["MACDBG"]. "
				"ADD NEW FBT_INFO.\n",
				MAC2STRDBG(fbt_data->al_mac), MAC2STRDBG(fbt_data->bssid));

			bss_table = NULL;
			if (fbt_data->fbt_info.mdid != 0) {
				bss_table = i5DmFindMDIDInBSSTable(fbt_data->fbt_info.mdid);
			}

			if (!bss_table) {
				WBD_SAFE_FIND_I5_BSS_IN_DEVICE(
					fbt_data->al_mac, fbt_data->bssid, i5_bss, &ret);
				bss_table = i5DmFindSSIDInBSSTable(&i5_bss->ssid);
			}
			if (bss_table && bss_table->fbt_info.mdid) {
				fbt_data->fbt_info.mdid =
					bss_table->fbt_info.mdid;
				fbt_data->fbt_info.ft_cap_policy =
					bss_table->fbt_info.ft_cap_policy;
				fbt_data->fbt_info.tie_reassoc_interval =
					bss_table->fbt_info.tie_reassoc_interval;
			} else {
				WBD_INFO("No matching MDID or SSID found for this FBT Req BSS Entry"
					" Not including it in FBT Resp data.\n");
				continue;
			}

			/* Add a FBT BSS item in a Slave's FBT Request List */
			wbd_add_item_in_fbt_cmdlist(fbt_data, ctlr_fbt_config, NULL,
				I5_IS_SPLIT_VNDR_MSG(src_device->flags) ? FALSE : TRUE);
		}

		WBD_INFO("From Blanket[BR%d] DEVICE["MACDBG"] BRDG["MACDBG"] BSS["MACDBG"] "
			"Received FBT_CONFIG_REQ : "
			"R0KH_ID[%s] LEN_R0KH_ID[%zu] "
			"R0KH_Key[%s] LEN_R0KH_Key[%zu] "
			"MDID[%d] FT_CAP[%d] FT_REASSOC[%d]\n",
			fbt_data->blanket_id, MAC2STRDBG(fbt_data->al_mac),
			MAC2STRDBG(fbt_data->bss_br_mac), MAC2STRDBG(fbt_data->bssid),
			fbt_data->r0kh_id, strlen(fbt_data->r0kh_id),
			fbt_data->r0kh_key, strlen(fbt_data->r0kh_key),
			fbt_data->fbt_info.mdid,
			fbt_data->fbt_info.ft_cap_policy,
			fbt_data->fbt_info.tie_reassoc_interval);
	}

	/* Now traverse controllers list for a particular AL MAC and remove if the BSS is not
	 * present in the request
	 */
	foreach_safe_glist_item(fbt_item_p, ctlr_fbt_config->entry_list, fbt_item_next_p) {

		/* need to keep next item incase we remove node in between */
		fbt_item_next_p = dll_next_p(fbt_item_p);

		fbt_data = (wbd_fbt_bss_entry_t *)fbt_item_p;

		/* Compare AL MAC */
		if (!eacmp(neighbor_al_mac, fbt_data->al_mac) == 0) {
			continue;
		}

		if (wbd_find_fbt_bss_item_for_bssid(fbt_data->bssid, &fbt_config_req)) {
			continue;
		}

		WBD_INFO("Delete BSS["MACDBG"] entry from DEVICE["MACDBG"]\n",
			MAC2STRDBG(fbt_data->bssid), MAC2STRDBG(fbt_data->al_mac));
		wbd_ds_glist_delete(&ctlr_fbt_config->entry_list, fbt_item_p);
		free(fbt_data);
		fbt_data = NULL;
	}

	/* Send 1905 Vendor Specific FBT_CONFIG_RESP Message to all Agents */
	wbd_master_broadcast_fbt_config_resp_cmd();

end:
	/* Remove all FBT Config Request data items */
	wbd_ds_glist_cleanup(&(fbt_config_req.entry_list));

	/* Create and store MAP Init End log */
	snprintf(logmsg, sizeof(logmsg), CLI_CMD_LOGS_MAP_INIT_END,
		wbd_get_formated_local_time(timestamp, sizeof(timestamp)),
		MAC2STRDBG(ieee1905_get_al_mac()));
	wbd_ds_add_logs_in_master(info->wbd_master, logmsg);

#ifdef BCM_APPEVENTD
	/* Send MAP Init Start event to appeventd. */
	wbd_appeventd_map_init(APP_E_WBD_MASTER_MAP_INIT_END,
		(struct ether_addr*)ieee1905_get_al_mac(), MAP_INIT_END, MAP_APPTYPE_MASTER);
#endif /* BCM_APPEVENTD */

	WBD_EXIT();
	return ret;
}
#endif /* WLHOSTFBT */

/* Process 1905 Vendor Specific Associated STA Link Metrics Vendor TLV */
static int
wbd_master_process_assoc_sta_metric_vndr_tlv(unsigned char *neighbor_al_mac,
	unsigned char *tlv_data, unsigned short tlv_data_len)
{
	int ret = WBDE_OK;
	i5_dm_bss_type *i5_bss;
	wbd_assoc_sta_item_t *assoc_sta;
	wbd_cmd_vndr_assoc_sta_metric_t cmd; /* STA Metric Vndr Data */
	wbd_prb_sta_t *prbsta;
	WBD_ENTER();

	/* Initialize Vendor Specific Associated STA Link Metrics */
	memset(&cmd, 0, sizeof(cmd));

	/* Decode Vendor Specific TLV : Associated STA Link Metrics Vendor Data on receive */
	ret = wbd_tlv_decode_vndr_assoc_sta_metrics((void *)&cmd, tlv_data, tlv_data_len);
	WBD_ASSERT_MSG("Failed to decode Assoc STA Metrics From DEVICE["MACDBG"]\n",
		MAC2STRDBG(neighbor_al_mac));

	WBD_INFO("Data Received VNDR_ASSOC_STA_LINK_METRIC : "
		"SRC_BSSID["MACDBG"] STA_MAC["MACDBG"] "
		"Idle_rate[%d] Tx_failures[%d] STA Cap[0x%02x] Tx_tot_failures[%d]\n",
		MAC2STRDBG(cmd.src_bssid), MAC2STRDBG(cmd.sta_mac),
		cmd.vndr_metrics.idle_rate, cmd.vndr_metrics.tx_failures,
		cmd.vndr_metrics.sta_cap, cmd.vndr_metrics.tx_tot_failures);

	/* Store the data */
	WBD_SAFE_FIND_I5_BSS_IN_DEVICE(neighbor_al_mac, cmd.src_bssid, i5_bss, &ret);

	/* Check if the STA exists or not */
	wbd_ds_find_sta_in_bss_assoclist(i5_bss, (struct ether_addr*)cmd.sta_mac,
		&ret, &assoc_sta);
	WBD_ASSERT_MSG("Device["MACDBG"] BSSID["MACDBG"] STA["MACDBG"]. %s\n",
		MAC2STRDBG(neighbor_al_mac), MAC2STRDBG(i5_bss->BSSID), MAC2STRDBG(cmd.sta_mac),
		wbderrorstr(ret));

	assoc_sta->stats.tx_failures = cmd.vndr_metrics.tx_failures;
	/* tx_failures != 0 indicated readings from BSD. Since BSD provides
	 * total failures as well as failures in last few seconds update the
	 * last active time to current time
	 */
	if (cmd.vndr_metrics.tx_failures != 0) {
		assoc_sta->stats.active = time(NULL);
		assoc_sta->stats.old_tx_tot_failures = cmd.vndr_metrics.tx_tot_failures -
			cmd.vndr_metrics.tx_failures;
	} else {
		assoc_sta->stats.old_tx_tot_failures = assoc_sta->stats.tx_tot_failures;
	}
	assoc_sta->stats.tx_tot_failures = cmd.vndr_metrics.tx_tot_failures;
	assoc_sta->stats.idle_rate = cmd.vndr_metrics.idle_rate;

	if (cmd.vndr_metrics.sta_cap &
		(WBD_TLV_ASSOC_STA_CAPS_BAND_5G | WBD_TLV_ASSOC_STA_CAPS_BAND_6G)) {
		prbsta = wbd_ds_find_sta_in_probe_sta_table(wbd_get_ginfo(),
			(struct ether_addr*)cmd.sta_mac, FALSE);
		if (prbsta) {
			if (cmd.vndr_metrics.sta_cap & WBD_TLV_ASSOC_STA_CAPS_BAND_5G) {
				prbsta->band |= (WBD_BAND_LAN_5GL | WBD_BAND_LAN_5GH);
			}
			if (cmd.vndr_metrics.sta_cap & WBD_TLV_ASSOC_STA_CAPS_BAND_6G) {
				prbsta->band |= WBD_BAND_LAN_6G;
			}
		}
	}

end:
	WBD_EXIT();
	return ret;
}

/* Process 1905 Vendor Specific Messages at WBD Application Layer */
int
wbd_master_process_vendor_specific_msg(ieee1905_vendor_data *msg_data)
{
	int ret = WBDE_OK;
	unsigned char *tlv_data = NULL;
	unsigned short tlv_data_len, tlv_hdr_len;
	unsigned int pos;
	WBD_ENTER();

	/* Store TLV Hdr len */
	tlv_hdr_len = sizeof(i5_tlv_t);

	/* Initialize data pointers and counters */
	for (pos = 0, tlv_data_len = 0;

		/* Loop till we reach end of vendor data */
		(int)(msg_data->vendorSpec_len - pos) > 0;

		/* Increament the pointer with current TLV Header + TLV data */
		pos += tlv_hdr_len + tlv_data_len) {

		/* For TLv, Initialize data pointers and counters */
		tlv_data = &msg_data->vendorSpec_msg[pos];

		/* Pointer is at the next TLV */
		i5_tlv_t *ptlv = (i5_tlv_t *)tlv_data;

		/* Get next TLV's data length (Hdr bytes skipping done in fn wbd_tlv_decode_xxx) */
		tlv_data_len = ntohs(ptlv->length);

		WBD_DEBUG("vendorSpec_len[%d] tlv_hdr_len[%d] tlv_data_len[%d] pos[%d] type[%d]\n",
			msg_data->vendorSpec_len, tlv_hdr_len, tlv_data_len, pos, ptlv->type);

		switch (ptlv->type) {

			case WBD_TLV_FBT_CONFIG_REQ_TYPE:
				WBD_INFO("Processing %s vendorSpec_len[%d]\n",
					wbd_tlv_get_type_str(WBD_TLV_FBT_CONFIG_REQ_TYPE),
					msg_data->vendorSpec_len);

#ifdef WLHOSTFBT
				/* Process 1905 Vendor Specific FBT_CONFIG_REQ Message */
				wbd_master_process_fbt_config_req_cmd(
					msg_data->neighbor_al_mac, tlv_data, tlv_data_len);
#endif /* WLHOSTFBT */
				break;

			case WBD_TLV_VNDR_ASSOC_STA_METRICS_TYPE:
				WBD_INFO("Processing %s vendorSpec_len[%d]\n",
					wbd_tlv_get_type_str(WBD_TLV_VNDR_ASSOC_STA_METRICS_TYPE),
					msg_data->vendorSpec_len);

				/* Process 1905 Vendor Specific Assoc STA Metrics Vendor TLV */
				wbd_master_process_assoc_sta_metric_vndr_tlv(
					msg_data->neighbor_al_mac, tlv_data, tlv_data_len);

				break;

			case WBD_TLV_BSS_CAPABILITY_REPORT_TYPE:
				WBD_INFO("Processing %s vendorSpec_len[%d]\n",
					wbd_tlv_get_type_str(ptlv->type),
					msg_data->vendorSpec_len);
				/* Process 1905 Vendor Specific BSS Capability Report Message */
				wbd_master_process_bss_capability_report_cmd(
					msg_data->neighbor_al_mac, tlv_data, tlv_data_len);
				break;

			case WBD_TLV_STEER_RESP_REPORT_TYPE:
				WBD_INFO("Processing %s vendorSpec_len[%d]\n",
					wbd_tlv_get_type_str(ptlv->type),
					msg_data->vendorSpec_len);
				/* Process 1905 Vendor Specific Steer Response Report Message */
				wbd_master_process_steer_resp_report_cmd(
					msg_data->neighbor_al_mac, tlv_data, tlv_data_len);
				break;
			case WBD_TLV_ZWDFS_TYPE:
				WBD_INFO("Processing %s vendorSpec_len[%d]\n",
					wbd_tlv_get_type_str(ptlv->type),
					msg_data->vendorSpec_len);
				/* Process 1905 Vendor Specific Zero wait dfs Message */
				wbd_master_process_zwdfs_cmd(msg_data->neighbor_al_mac,
					tlv_data, tlv_data_len);
				break;
#if !defined(MULTIAPR2)
			case WBD_TLV_CHAN_INFO_TYPE:
				WBD_INFO("Processing %s vendorSpec_len[%d]\n",
					wbd_tlv_get_type_str(ptlv->type),
					msg_data->vendorSpec_len);

				/* Process 1905 Vendor Specific chan info and chanspec list */
				wbd_master_process_intf_chan_info(msg_data->neighbor_al_mac,
					tlv_data, tlv_data_len);
				break;
#endif /* !MULTIAPR2 */
			default:
				WBD_WARNING("Vendor TLV[%s] processing not Supported by Master.\n",
					wbd_tlv_get_type_str(ptlv->type));
				break;
		}
	}

	WBD_DEBUG("vendorSpec_len[%d] tlv_hdr_len[%d] tlv_data_len[%d] pos[%d]\n",
		msg_data->vendorSpec_len, tlv_hdr_len, tlv_data_len, pos);

	WBD_EXIT();
	return ret;
}

#if defined(MULTIAPR2)
/* Send Channel Scan Request Message */
static void
wbd_master_send_chscan_req(unsigned char *al_mac, bool fresh_scan_req)
{
	int ret = WBDE_OK, num_of_radios = 0;
	ieee1905_chscan_req_msg chscan_req;
	ieee1905_per_radio_opclass_list *radio_info = NULL;
	ieee1905_per_opclass_chan_list *opclass_info_1 = NULL, *opclass_info_2 = NULL;

	i5_dm_interface_type *ifr = NULL;
	i5_dm_device_type *dev = NULL;

	memset(&chscan_req, 0, sizeof(chscan_req));

	/* Initialize radio list */
	ieee1905_glist_init(&chscan_req.radio_list);

	WBD_SAFE_FIND_I5_DEVICE(dev, al_mac, &ret);

	/* Insert Channel Scan Request TLV Flags */
	if (fresh_scan_req) {
		chscan_req.chscan_req_msg_flag |= MAP_CHSCAN_REQ_FRESH_SCAN;
	}

	/* Insert Number of radios : upon which channel scans are requested : In the End */

	/* Insert details for each radio */
	foreach_i5glist_item(ifr, i5_dm_interface_type, dev->interface_list) {

		/* Loop for, only for wireless interfaces with valid chanspec & BSSes */
		if (!i5DmIsInterfaceWireless(ifr->MediaType) || !ifr->BSSNumberOfEntries) {
			WBD_INFO("Not a wireless interface "MACF" OR no BSSes\n",
				ETHERP_TO_MACF(ifr->InterfaceId));
			continue;
		}

		/* Allocate per_radio_opclass Info structure */
		radio_info = (ieee1905_per_radio_opclass_list*)
			wbd_malloc(sizeof(*radio_info), &ret);
		WBD_ASSERT();

		/* Initialize opclass list */
		ieee1905_glist_init(&radio_info->opclass_list);

		/* Insert Radio Unique Identifier of a radio of the Multi-AP Agent */
		memcpy(radio_info->radio_mac, ifr->InterfaceId, MAC_ADDR_LEN);

		/* If Fresh Scan is not Required, Number of Operating Classes = 0 */
		if (!fresh_scan_req) {

			/* Insert Number of Operating Classes */
			radio_info->num_of_opclass = 0;
		} else {

			/* Insert Number of Operating Classes */
			radio_info->num_of_opclass = 2;
		}

		/* If Fresh Scan is Required, specify Opclass & Channel Numbers,
		 * If Fresh Scan is not Required, skip this part
		 */
		if (fresh_scan_req) {

			/* Allocate per_opclass_chan Info structure */
			opclass_info_1 = (ieee1905_per_opclass_chan_list *)
				wbd_malloc(sizeof(*opclass_info_1), &ret);
			WBD_ASSERT();

			/* Allocate per_opclass_chan Info structure */
			opclass_info_2 = (ieee1905_per_opclass_chan_list *)
				wbd_malloc(sizeof(*opclass_info_2), &ret);
			WBD_ASSERT();

			if (CHSPEC_IS2G(ifr->chanspec)) {

				/* Insert Operating Class Value 1 */
				opclass_info_1->opclass_val = 81;
				/* Insert Number of Channels specified in the Channel List */
				opclass_info_1->num_of_channels = 3;
				/* Insert Octets of Channels specified in the Channel List */
				opclass_info_1->chan_list[0] = wf_chspec_ctlchan(0x1007);
				/* Insert Octets of Channels specified in the Channel List */
				opclass_info_1->chan_list[1] = wf_chspec_ctlchan(0x1008);
				/* Insert Octets of Channels specified in the Channel List */
				opclass_info_1->chan_list[2] = wf_chspec_ctlchan(0x1009);

				/* Insert Operating Class Value 2 */
				opclass_info_2->opclass_val = 82;
				/* Insert Number of Channels specified in the Channel List */
				opclass_info_2->num_of_channels = 1;
				/* Insert Octets of Channels specified in the Channel List */
				opclass_info_2->chan_list[0] = wf_chspec_ctlchan(0x100e);

				WBD_INFO("Send 2G ChannelScanReq IFR["MACF"] :\n"
				"opclass_1[%d] chan_1[%d] chan_2[%d] chan_3[%d]\n"
				"opclass_2[%d] chan_1[%d]\n",
				ETHERP_TO_MACF(ifr->InterfaceId),
				opclass_info_1->opclass_val, opclass_info_1->chan_list[0],
				opclass_info_1->chan_list[1], opclass_info_1->chan_list[2],
				opclass_info_2->opclass_val, opclass_info_2->chan_list[0]);

			} else {

				/* Insert Operating Class Value 1 */
				opclass_info_1->opclass_val = 115;
				/* Insert Number of Channels specified in the Channel List */
				opclass_info_1->num_of_channels = 2;
				/* Insert Octets of Channels specified in the Channel List */
				opclass_info_1->chan_list[0] = wf_chspec_ctlchan(0xd024);
				/* Insert Octets of Channels specified in the Channel List */
				opclass_info_1->chan_list[1] = wf_chspec_ctlchan(0xd028);

				/* Insert Operating Class Value 2 */
				opclass_info_2->opclass_val = 121;
				/* Insert Number of Channels specified in the Channel List */
				opclass_info_2->num_of_channels = 2;
				/* Insert Octets of Channels specified in the Channel List */
				opclass_info_2->chan_list[0] = wf_chspec_ctlchan(0xd064);
				/* Insert Octets of Channels specified in the Channel List */
				opclass_info_2->chan_list[1] = wf_chspec_ctlchan(0xd068);

				WBD_INFO("Send 5G ChannelScanReq IFR["MACF"] :\n"
				"opclass_1[%d] chan_1[%d] chan_2[%d]\n"
				"opclass_2[%d] chan_1[%d] chan_2[%d]\n",
				ETHERP_TO_MACF(ifr->InterfaceId),
				opclass_info_1->opclass_val, opclass_info_1->chan_list[0],
				opclass_info_1->chan_list[1], opclass_info_2->opclass_val,
				opclass_info_2->chan_list[0], opclass_info_2->chan_list[1]);

			}

			/* Append per_radio_opclass Info structure 1 */
			ieee1905_glist_append(&radio_info->opclass_list, (dll_t*)opclass_info_1);
			/* Append per_radio_opclass Info structure 2 */
			ieee1905_glist_append(&radio_info->opclass_list, (dll_t*)opclass_info_2);

		}

		/* Append per_radio_opclass Info structure */
		ieee1905_glist_append(&chscan_req.radio_list, (dll_t*)radio_info);

		num_of_radios++;
	}

	/* Insert Number of radios : upon which channel scans are requested : Fill now */
	chscan_req.num_of_radios = num_of_radios;

	WBD_INFO("Sending Requested { %s } Channel Scan Request Message to ["MACF"]\n",
		fresh_scan_req ? "FRESH" : "STORED", ETHERP_TO_MACF(al_mac));

	/* Send Channel Scan Request Message to a Multi AP Device */
	i5MessageChannelScanRequestSend(dev, &chscan_req);

end:
	/* Free the memory allocated for Channel Scan Request Msg structure */
	i5DmChannelScanRequestInfoFree(&chscan_req);

	return;
}
#endif /* MULTIAPR2 */

/* Set AP configured flag */
void wbd_master_set_ap_configured(unsigned char *al_mac, unsigned char *radio_mac, int if_band)
{
	int ret = WBDE_OK;
	i5_dm_interface_type *interface;
	i5_dm_device_type *i5_device;

	WBD_ENTER();

	if (!al_mac || !radio_mac) {
		WBD_ERROR("Invalid Device or Radio address\n");
		goto end;
	}

	interface = wbd_ds_get_i5_interface(al_mac, radio_mac, &ret);
	WBD_ASSERT();

	interface->isConfigured = 1;
	interface->band = if_band;

	/* Send some device speific message. Note: This callback comes for every interface on the
	 * device. Set a flag to indicate its already sent so that next time before sending one
	 * can check it
	 */
	i5_device = (i5_dm_device_type*)WBD_I5LL_PARENT(interface);
	/* Send NVRAM set vendor message if it is not sent */
	wbd_master_send_vndr_nvram_set_cmd(i5_device, interface);

end:
	WBD_EXIT();
}

/* Controller Update bSTA IFR's Channel, based on its Parent Interface */
static void
wbd_master_update_bsta_ifr_channel(i5_dm_interface_type *in_ifr)
{
	int ret = WBDE_OK;
	i5_dm_clients_type *i5_bsta;
	i5_dm_bss_type *i5_parent_bss;
	i5_dm_interface_type *i5_parent_ifr;
	WBD_ENTER();

	WBD_INFO("Ifr["MACF"] is Backhaul Link. Get Channel from bSTA's Parent IFR.\n",
		ETHERP_TO_MACF(in_ifr->InterfaceId));

	/* Find bSTA in Topology, Get Parent Interface */
	i5_bsta = wbd_ds_find_sta_in_topology(in_ifr->InterfaceId, &ret);
	if (!i5_bsta) {
		WBD_ERROR("Unexpected! bSTA["MACF"] not connected. Channel selection "
			"request still declined.\n", ETHERP_TO_MACF(in_ifr->InterfaceId));
		goto end;
	}
	i5_parent_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_bsta);
	i5_parent_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_parent_bss);

	/* Update Operating Channel of bSTA Interface */
	in_ifr->opClass = i5_parent_ifr->opClass;
	in_ifr->chanspec = i5_parent_ifr->chanspec;

end:
	WBD_INFO("Ifr "MACF" Get Channel from bSTA's Parent IFR : %s "
		"opClass: %d chanspec: 0x%x\n", ETHERP_TO_MACF(in_ifr->InterfaceId),
		(ret == WBDE_OK) ? "Success. UPDATED:" : "Failed. OLD:",
		in_ifr->opClass, in_ifr->chanspec);
	WBD_EXIT();
}

/* Callback from IEEE 1905 module when the controller gets Channel Selection Response */
void
wbd_master_process_chan_selection_response_cb(unsigned char *al_mac,
	unsigned char *interface_mac, t_I5_CHAN_SEL_RESP_CODE chan_sel_resp_code)
{
	i5_dm_interface_type *pdmif;
	int ret = WBDE_OK;
	WBD_ENTER();

	if (!al_mac || !interface_mac) {
		WBD_ERROR("Error: channel selection response : NULL device/pdmif\n");
		goto end;
	}
	pdmif = wbd_ds_get_i5_interface(al_mac, interface_mac, &ret);
	WBD_ASSERT();

	WBD_INFO("Received channel selection response for interface["MACF"] "
		"resp_code[%d]\n", ETHERP_TO_MACF(interface_mac), chan_sel_resp_code);

	/* If it is bSTA interface, and Agent Do not want to send Operating Channel Report
	 * Controller should Update bSTA IFR's Channel, based on its Parent Interface
	 */
	if (I5_IS_BSS_STA(pdmif->mapFlags) &&
		(chan_sel_resp_code == I5_CHAN_SEL_RESP_CODE_DECLINE_3)) {
		wbd_master_update_bsta_ifr_channel(pdmif);
	}

end:
	WBD_EXIT();
}

/* Controller Update Interface's Operating Channel, based on its Operating Channel Report
 * If it has any Backhaul BSS, It also Finds bSTA associated with it and update Operating Channel
 * of bSTA interfaces present in Controller Topology in a recursive manner
 */
static void
wbd_master_update_operating_channel(i5_dm_interface_type *in_ifr,
	uint16 in_chspec, unsigned char in_op_class)
{
	i5_dm_bss_type *iter_bss;
	i5_dm_clients_type *iter_sta;
	i5_dm_interface_type *i5_bsta_ifr;
	WBD_ENTER();

	/* Update Operating Channel of this Interface */
	WBD_INFO("Ifr "MACF" UPDATED: opClass: %d chanspec: 0x%x\n",
		ETHERP_TO_MACF(in_ifr->InterfaceId), in_op_class, in_chspec);
	in_ifr->opClass = in_op_class;
	in_ifr->chanspec = in_chspec;

	/* Do below Only for Interface with any Backhaul BSS */
	if (!I5_IS_BSS_BACKHAUL(in_ifr->mapFlags)) {
		goto end;
	}

	/* Loop for all the BSSs in this Interface */
	foreach_i5glist_item(iter_bss, i5_dm_bss_type, in_ifr->bss_list) {

		/* Do below Only for Backhaul BSS */
		if (!I5_IS_BSS_BACKHAUL(iter_bss->mapFlags)) {
			continue;
		}

		WBD_DEBUG("BSS "MACF" is Backhaul BSS. Get bSTA's Connected to it.\n",
			ETHERP_TO_MACF(iter_bss->BSSID));

		/* Loop for all the Backhaul Clients in this Backhaul BSS */
		foreach_i5glist_item(iter_sta, i5_dm_clients_type, iter_bss->client_list) {

			/* Find the interface in the network matching Backhaul STA MAC */
			i5_bsta_ifr = i5DmFindInterfaceFromNetwork(iter_sta->mac);

			if (i5_bsta_ifr == NULL) {
				WBD_ERROR("bSTA "MACF" is not an Interface "
					"in Network! Stale sta in entry in bss client list. "
					"Not Updating its Operating Channel.\n",
					ETHERP_TO_MACF(iter_sta->mac));
				continue;
			}

			WBD_DEBUG("Ifr "MACF" is Backhaul STA. Updating its Operating Channel.\n",
				ETHERP_TO_MACF(i5_bsta_ifr->InterfaceId));

			/* Update BH IFR's Operating Channel, & Recursively its childs' also */
			wbd_master_update_operating_channel(i5_bsta_ifr, in_chspec, in_op_class);
		}
	}
end:
	WBD_EXIT();
	return;
}

/* Handle operating channel report */
void wbd_master_process_operating_chan_report(wbd_info_t *info, unsigned char *al_mac,
	ieee1905_operating_chan_report *chan_report)
{
	wbd_master_info_t *master_info;
	i5_dm_network_topology_type *i5_topology;
	i5_dm_device_type *i5_iter_device;
	i5_dm_interface_type *i5_iter_ifr, *i5_ifr;
	i5_dm_device_type *self_device, *i5_device;
	i5_dm_interface_type *self_intf;
	uint8 channel = 0;
	uint bw = 0;
	uint16 chspec = 0;
	unsigned char op_class = 0;
	uint8 prim_channel = 0;
	uint highest_bw = 0;
	int ret = WBDE_OK;
	int agent_if_band, if_band;
	int i;
	wbd_device_item_t *device_vndr;
	WBD_ENTER();

	i5_topology = ieee1905_get_datamodel();
	if (!chan_report || !(chan_report->list) || !chan_report->n_op_class) {
		WBD_ERROR(" Invalid chan report, return \n");
		goto end;
	}
	/* following piece of code needs to add at onboarding time to take care of
	 * cross model working i.e. dual band <-> Tri band
	 */
	agent_if_band = if_band = BAND_INV;

	WBD_SAFE_GET_I5_IFR(al_mac, chan_report->radio_mac, i5_ifr, &ret);
	agent_if_band = i5_ifr->band;

	WBD_SAFE_FIND_I5_DEVICE(i5_device, al_mac, &ret);
	device_vndr = (wbd_device_item_t*)i5_device->vndr_data;

	if (!WBD_BAND_VALID(agent_if_band)) {
		WBD_ERROR(" Invalid band for agent, return \n");
		return;
	}

	WBD_SAFE_GET_MASTER_INFO(info, WBD_BKT_ID_BR0, master_info, &ret);

	WBD_INFO("Received operating channel report from Agent "MACF" for interface "MACF
		" Tx pwr: %d Number of opclasses: %d resp_code: %d\n", ETHERP_TO_MACF(al_mac),
		ETHERP_TO_MACF(i5_ifr->InterfaceId), chan_report->tx_pwr, chan_report->n_op_class,
		i5_ifr->chan_sel_resp_code);
	/* Dont Process Operating Channel Report, if Chan Select Resp code = DECLINE 1 or 2 or 3 */
	if ((i5_ifr->chan_sel_resp_code == I5_CHAN_SEL_RESP_CODE_DECLINE_1) ||
		(i5_ifr->chan_sel_resp_code == I5_CHAN_SEL_RESP_CODE_DECLINE_2) ||
		(i5_ifr->chan_sel_resp_code == I5_CHAN_SEL_RESP_CODE_DECLINE_3)) {

		WBD_DEBUG("Decline because request violates current or "
			"most recently reported preferences or would prevent operation"
			"of a currently operating backhaul link. Do not process this"
			"OpChannel Report further.\n");
		i5_ifr->chan_sel_resp_code = I5_CHAN_SEL_RESP_CODE_NONE;
		goto end;
	}
	i5_ifr->chan_sel_resp_code = I5_CHAN_SEL_RESP_CODE_NONE;
	for (i = 0; i < chan_report->n_op_class; i++) {
		WBD_DEBUG("[%d]: OpClass: %d Channel: %d\n", (i + 1),
			chan_report->list[i].op_class, chan_report->list[i].chan);
		blanket_get_bw_from_rc(chan_report->list[i].op_class, &bw);

		if (BW_LE20(bw)) {
			prim_channel = chan_report->list[i].chan;
		} else if (BW_LE40(bw) && !prim_channel) {
			prim_channel = chan_report->list[i].chan;
		}
		if (bw > highest_bw) {
			highest_bw = bw;
			op_class = chan_report->list[i].op_class;
		}
	}

	WBD_INFO("Highest BW: 0x%x Primary channel: %d\n", highest_bw, prim_channel);

	if (!highest_bw) {
		WBD_ERROR("Invalid operating class in operating channel report. "
		"# of opclass: %d\n", chan_report->n_op_class);
		return;
	}

	if (!prim_channel) {
		/* TODO: convert it error once it is fixed at sending side */
		prim_channel = chan_report->list[0].chan;
		WBD_WARNING("Couldn't find primary channel from operating channel report. "
			"using %d\n", prim_channel);
	}

	chspec = wf_channel2chspec(prim_channel, highest_bw, blanket_opclass_to_band(op_class));
	if (chspec == 0) {
		WBD_ERROR("get chspec failed. channel: %d rc: %d bw: %d\n",
			prim_channel, op_class, highest_bw);
		goto end;
	}

	/* Controller Update IFR and its child bSTA IFRs OpChannel, based on this OpChannel Rpt */
	wbd_master_update_operating_channel(i5_ifr, chspec, op_class);

	/* Create AP chan report */
	wbd_master_create_ap_chan_report(i5_topology, master_info);

	if (WBD_MCHAN_ENAB(info->flags)) {
		WBD_INFO("Multi chan mode is ON, no need to update other agent's chanspec \n");
		goto end;
	}

	if (WBD_IS_DEDICATED_BACKHAUL(i5_ifr->mapFlags) &&
		!WBD_IS_SCHAN_BH_ENABLED(info->flags)) {
		WBD_INFO("Interface "MACF" is dedicated backhaul. No need to update "
			"other agent's chanspec\n", ETHERP_TO_MACF(i5_ifr->InterfaceId));
		goto end;
	}

	if (device_vndr->flags &
		(WBD_DEV_FLAG_CHAN_SELECT_TIMER | WBD_DEV_FLAG_CHAN_PREFERENCE_TIMER)) {
		WBD_INFO("Channel selection request has not been sent to this agent,"
			"no need to update other agent's chanspec device_vndr->flags[%x]\n",
			device_vndr->flags);
		goto end;
	}

	WBD_SAFE_GET_I5_SELF_DEVICE(self_device, &ret);

	foreach_i5glist_item(self_intf, i5_dm_interface_type, self_device->interface_list) {
		int self_if_band;

		if ((self_intf->chanspec == 0) || !i5DmIsInterfaceWireless(self_intf->MediaType)) {
			continue;
		}

		/* get controller's interface matching with same band as requested in
		 * channel report, if chanspec of band is different update corresponding
		 * interface's chanspec
		 */
		channel = wf_chspec_ctlchan(self_intf->chanspec);
		self_if_band = ieee1905_get_band_from_channel(self_intf->opClass, channel);

		if (!(agent_if_band & self_if_band)) {

			WBD_DEBUG("Requested band=%d does not match with Controller's"
				" interface band=%d, look for other interface \n",
				agent_if_band, self_if_band);

			continue;
		}
		if (prim_channel == channel) {
			WBD_INFO("Agent "MACF" send operating channel report for interface "
				MACF" in reposne to channel selection request, Ignore\n",
				ETHERP_TO_MACF(al_mac), ETHERP_TO_MACF(i5_ifr->InterfaceId));
			return;
		} else {
			/* band is same, some agent changed the channel during normal
			 * operation. update controller's interface chanspec and inform
			 * all other agents for the new channel with channel selection
			 * request. For 80MHz and 160MHz channels, if the change is only in the
			 * primary channel, use the controller channel itself
			 */
			if (op_class <= REGCLASS_5G_40MHZ_LAST ||
				(blanket_mask_chanspec_sb(self_intf->chanspec) !=
				blanket_mask_chanspec_sb(chspec))) {
				WBD_INFO("update controller's %s chanspec to %x \n",
					self_intf->ifname, self_intf->chanspec);
				self_intf->chanspec = chspec;
			}

			break;
		}
	}

	if (self_intf == NULL) {
		/* No interface on master matching operating chan report credentials, return */
		WBD_WARNING("For Interface ["MACF"] Self Interface not present in controller for "
			"band 0x%x. updating controller chanspec w.r.t operating chan report\n",
			ETHERP_TO_MACF(i5_ifr->InterfaceId), agent_if_band);
		goto end;
	}

	if (self_intf->chanspec != chspec) {
		WBD_INFO("Only primary channel different in operating channel report "
			"from Agent "MACF" for interface "MACF". Controller chanspec [0x%x], "
			"Agent chanspec [0x%x]. Sending controller chanspec back to agent\n",
			ETHERP_TO_MACF(al_mac), ETHERP_TO_MACF(i5_ifr->InterfaceId),
			self_intf->chanspec, chspec);
		wbd_master_send_channel_selection_request(info, al_mac, chan_report->radio_mac);
		goto end;
	}

	foreach_i5glist_item(i5_iter_device, i5_dm_device_type, i5_topology->device_list) {
		if ((eacmp(i5_iter_device->DeviceId, al_mac) == 0) ||
			!I5_IS_MULTIAP_AGENT(i5_iter_device->flags)) {
			/* No need to send Channel selection request to
			 * originator of operating channel report message
			 */
			continue;
		}
		foreach_i5glist_item(i5_iter_ifr, i5_dm_interface_type,
			i5_iter_device->interface_list) {

			if ((i5_iter_ifr->chanspec == 0) ||
				!i5DmIsInterfaceWireless(i5_iter_ifr->MediaType) ||
				!i5_iter_ifr->BSSNumberOfEntries) {
				continue;
			}

			if_band = i5_iter_ifr->band;
			if (agent_if_band == if_band) {
				WBD_INFO("channel selection request to"MACF", chan[%d] band[%d]\n",
					ETHERP_TO_MACF(i5_iter_ifr->InterfaceId),
					wf_chspec_ctlchan(i5_iter_ifr->chanspec), if_band);

				/* send channel selection request for this radio mac */
				wbd_master_send_channel_selection_request(info,
					i5_iter_device->DeviceId, i5_iter_ifr->InterfaceId);
			}
		}
	}
end:
	WBD_EXIT();
}

/* Check if channel in regclass is invalid: invalid. 0 = valid, 1 = invalid */
int wbd_master_check_regclass_channel_invalid(uint8 chan,
	uint8 regclass, ieee1905_chan_pref_rc_map_array *cp)
{
	int i, j;
	for (i = 0; i < cp->rc_count; i++) {
		if (regclass != cp->rc_map[i].regclass) {
			continue;
		}

		/* If there is no channels and preference is 0, then its invalid channel */
		if (cp->rc_map[i].count == 0 && cp->rc_map[i].pref == 0) {
			return 1;
		}

		/* search for channel. If the preference is 0 then its invalid channel */
		for (j = 0; j < cp->rc_map[i].count; j++) {
			if (chan == cp->rc_map[i].channel[j]) {
				return ((cp->rc_map[i].pref == 0) ? 1 : 0);
			}
		}
		break;
	}
	return 0;
}

/* Send channel selection request */
/* Algorithm to send channel selection request:
 * Prepare channel prefernce report with:
 * - List of opclass which are not present in agent's radio capability
 * - Update agent's opclass with invalid channels based on controller's
 *   current chanspec and opclass
 * - Prepare list of channels for opclass present in agent's radio capability
 *   but are non preferred at the moment.
 * - Compare prepared channel preference report with controller and agent's
 *   channel preference database.
 *
 * - Final channel preference report should be with the following information:
 *   - List of all valid opclass in global operating class table
 *   - Each operating class object have either zero or non zero list of
 *     non preferred channels
 */
void wbd_master_send_channel_selection_request(wbd_info_t *info, unsigned char *al_mac,
	unsigned char *radio_mac)
{
	int ret = WBDE_OK;
	uint8 rc_first, rc_last;
	uint8 chan_first, chan_last, center_channel = 0;
	int channel = 0;
	ieee1905_chan_pref_rc_map_array chan_pref;
	ieee1905_chan_pref_rc_map *rc_map;
	i5_dm_interface_type *interface, *self_intf = NULL;
	i5_dm_device_type *self_device, *device;
	int i, j, k;
	ieee1905_radio_caps_type *RadioCaps;
	int rcaps_rc_count;
	int rcidx;
	wbd_cmd_vndr_set_chan_t vndr_set_chan;
	int if_band = BAND_INV;
	void *pmsg;
	unsigned char opClass;
	const i5_dm_rc_chan_map_type *rc_chan_map;
	unsigned int reg_class_count = 0;

	WBD_ENTER();

	rc_chan_map = i5DmGetRCChannelMap(&reg_class_count);

	memset(&chan_pref, 0, sizeof(chan_pref));

	if (!al_mac) {
		WBD_ERROR("Invalid Device address\n");
		goto end;
	}

	device = wbd_ds_get_i5_device(al_mac, &ret);
	WBD_ASSERT();

	/* Remove old channel selection vendor information and init */
	wbd_ds_glist_cleanup(&info->wbd_master->vndr_set_chan_list);
	wbd_ds_glist_init(&info->wbd_master->vndr_set_chan_list);

	pmsg = ieee1905_create_channel_selection_request(al_mac);
	if (pmsg == NULL) {
		WBD_ERROR("Channel selection request creation failed\n");
		goto end;
	}

	chan_pref.rc_map = (ieee1905_chan_pref_rc_map *)
		wbd_malloc(sizeof(ieee1905_chan_pref_rc_map) * reg_class_count, NULL);
	if (chan_pref.rc_map == NULL) {
		WBD_ERROR("malloc failed for rc_map\n");
		goto end;
	}

	foreach_i5glist_item(interface, i5_dm_interface_type, device->interface_list) {

		if (!i5DmIsInterfaceWireless(interface->MediaType) ||
				!interface->BSSNumberOfEntries) {
			WBD_INFO("Not a wireless interface "MACF" OR no BSSes\n",
				ETHERP_TO_MACF(interface->InterfaceId));
			continue;
		}
		if (radio_mac && (memcmp(interface->InterfaceId, radio_mac, MAC_ADDR_LEN) != 0)) {
			WBD_INFO("Radio mac specified ["MACF"] current ["MACF"]. Skip it\n",
				ETHERP_TO_MACF(radio_mac), ETHERP_TO_MACF(interface->InterfaceId));
			continue;
		}
		if (!interface->isConfigured) {
			WBD_INFO("Interface ["MACF"] not configured yet. Skip it\n",
				ETHERP_TO_MACF(interface->InterfaceId));
			continue;
		}

		chan_pref.rc_count = 0;
		memset(chan_pref.rc_map, 0, sizeof(ieee1905_chan_pref_rc_map) * reg_class_count);

		RadioCaps = &interface->ApCaps.RadioCaps;
		rcaps_rc_count = RadioCaps->List ? RadioCaps->List[0]: 0;

		/* Get the valid range of channels and regulatory class for the given
		 * band. 160Mhz is handled as special case. So not considered here
		 */
		if_band = ieee1905_get_band_from_radiocaps(RadioCaps);
		WBD_INFO("band from radiocaps: 0x%x\n", if_band);

		if (if_band == BAND_2G) {
			rc_first = REGCLASS_24G_FIRST;
			rc_last = REGCLASS_24G_LAST;
			chan_first = CHANNEL_24G_FIRST;
			chan_last = CHANNEL_24G_LAST;
		} else if (if_band == BAND_5GL) {
			rc_first = REGCLASS_5GL_FIRST;
			/* 5GL opclasses are not continuous. 115 -120 and 128 - 130.
			 * Handle 128 - 130 as special case
			 */
			rc_last = REGCLASS_5GL_40MHZ_LAST;
			chan_first = CHANNEL_5GL_FIRST;
			chan_last = CHANNEL_5GL_LAST;
		} else if (if_band == BAND_5GH) {
			rc_first = REGCLASS_5GH_FIRST;
			rc_last = REGCLASS_5GH_LAST;
			chan_first = CHANNEL_5GH_FIRST;
			chan_last = CHANNEL_5GH_LAST;
		} else if (if_band == (BAND_5GH | BAND_5GL)) {
			rc_first = REGCLASS_5G_FIRST;
			rc_last = REGCLASS_5G_LAST;
			chan_first = CHANNEL_5G_FIRST;
			chan_last = CHANNEL_5G_LAST;
		} else if (if_band == BAND_6G) {
			rc_first = REGCLASS_6G_FIRST;
			rc_last = REGCLASS_6G_LAST;
			chan_first = CHANNEL_6G_FIRST;
			chan_last = CHANNEL_6G_LAST;
		} else {
			WBD_ERROR("Invalid band : %d\n", if_band);
			goto end;
		}
		WBD_INFO("Interface: ["MACF"]Band: [%d], RegClass Range: [%d - %d] "
			"Channel Range: [%d -%d]\n", ETHERP_TO_MACF(interface->InterfaceId),
			if_band, rc_first, rc_last, chan_first, chan_last);

		/* Add all regulatory classes outside current band to the unpreferred list */
		/* Opclasses 128, 129 and 130 are special cases in 5G beacuse it has channel from
		 * both 5GH and 5GL band. So avoid adding them if the band is 5GL/H
		 */
		WBD_DEBUG("Regulatory classes outside current band:\n");
		for (i = 0; i < reg_class_count; i++) {
			if (rc_chan_map[i].regclass < rc_first ||
				rc_chan_map[i].regclass > rc_last) {
				/* Sepcial case for band 5GL, because the 5GL regulatory classes
				 * are not continuous. 115 - 120 and 128 - 130.
				 * Skip 128 - 130 for 5GL here
				 */
				if (if_band == BAND_5GL &&
					rc_chan_map[i].regclass > REGCLASS_5G_40MHZ_LAST &&
					rc_chan_map[i].regclass <= REGCLASS_5G_LAST) {
					continue;
				}
				chan_pref.rc_map[chan_pref.rc_count].regclass =
					rc_chan_map[i].regclass;
				chan_pref.rc_count++;
				WBD_DEBUG("[%d]:%d\n", chan_pref.rc_count, rc_chan_map[i].regclass);
			}
		}

		/* If Multi-channel feature is disabled, all agents should operate in
		 * the master channel. So find master channel in the given band
		 */
		if (WBD_MCHAN_ENAB(info->flags)) {
			WBD_INFO("Multi channel enabled\n");
			/* no need to add vendor tlv for multi chan operation */
			goto validate;
		}
		if (WBD_IS_DEDICATED_BACKHAUL(interface->mapFlags) &&
			!WBD_IS_SCHAN_BH_ENABLED(info->flags)) {
			WBD_INFO("Interface ["MACF"] is dedicated Backhaul. Don't force "
				"single channel\n", ETHERP_TO_MACF(interface->InterfaceId));
			goto validate;
		}
		self_device = wbd_ds_get_self_i5_device(&ret);
		WBD_ASSERT();

		for (self_intf = (i5_dm_interface_type *)WBD_I5LL_HEAD(self_device->interface_list);
			self_intf; self_intf = WBD_I5LL_NEXT(self_intf)) {

			if ((self_intf->chanspec == 0) ||
				!i5DmIsInterfaceWireless(self_intf->MediaType)) {
				continue;
			}

			blanket_get_global_rclass(self_intf->chanspec, &opClass);
			self_intf->opClass = opClass;
			channel = wf_chspec_ctlchan(self_intf->chanspec);
			center_channel = self_intf->chanspec & WL_CHANSPEC_CHAN_MASK;
			WBD_INFO("ifname: %s, channel: %d rclass: %d chanspec: 0x%x\n",
				self_intf->ifname, channel,
				self_intf->opClass, self_intf->chanspec);

			if ((if_band &
				ieee1905_get_band_from_channel(self_intf->opClass, channel))) {

				if (channel >= chan_first && channel <= chan_last) {
					break;
				}
			}
		}
		if (!self_intf) {
			WBD_INFO("For Interface ["MACF"] Band[0x%x] No matching master interface"
				"for channel range (%d - %d). Send send Channel Preference TLV "
				"with zero rclass so that agent can choose their best channel\n",
				ETHERP_TO_MACF(interface->InterfaceId), if_band,
				chan_first, chan_last);
			chan_pref.rc_count = 0;
			ieee1905_insert_channel_selection_request_tlv(pmsg,
				interface->InterfaceId, &chan_pref);
			continue;
			/* XXX: channels will not be synced between agents and it can affect STA
			 * monitoring and hence steering
			 */
		}

		WBD_DEBUG("Unprefered Regulatory classes OR Unprefered channel(s) "
			"in a valid regclass inside the band:\n");
		for (i = 0; (i < reg_class_count) &&
			(chan_pref.rc_count < reg_class_count); i++) {
			uint8 offset = 0;

			rc_map = &chan_pref.rc_map[chan_pref.rc_count];
			if (rc_chan_map[i].regclass < rc_first ||
				rc_chan_map[i].regclass > rc_last) {
				/* Special case for band 5GL, because the 5GL regulatory classes
				 * are not continuous.  Don't skip 128 - 130 for 5GL here
				 */
				if (if_band == BAND_5GL &&
					rc_chan_map[i].regclass > REGCLASS_5G_40MHZ_LAST &&
					rc_chan_map[i].regclass <= REGCLASS_5G_LAST) {
					/* Don't skip */
				} else {
					continue;
				}
			}

			/* Exclude all lesser bandwidth chanspecs with matching control channel
			 * also from unprefered list. This is for repeaters that support only
			 * lower bandwidth.
			 * So add all higher regulatory classes as a first step
			 */
			if (rc_chan_map[i].regclass > opClass) {
				rc_map->regclass = rc_chan_map[i].regclass;
				chan_pref.rc_count++;
				WBD_DEBUG("[%d]:%d(higher regclass)\n",
					chan_pref.rc_count, rc_chan_map[i].regclass);
				continue;
			}
			/* Special case: include opclass 83 if current opclass is 84 */
			if (opClass == REGCLASS_24G_40MHZ_UPPER &&
				rc_chan_map[i].regclass == REGCLASS_24G_40MHZ_LOWER) {
				rc_map->regclass = rc_chan_map[i].regclass;
				chan_pref.rc_count++;
				WBD_DEBUG("[%d]:%d(same bandwidth regclass)\n",
					chan_pref.rc_count, rc_chan_map[i].regclass);
				continue;
			}
			/* Now add regulatory classes that doesn't have matching control channel.
			 * For all regulatory classes that has control channel(2G and 5G
			 * regclasses with 20/40MHz bandwidth), check if control channel is present.
			 * If not skip that regulatory class
			 */
			if (rc_chan_map[i].regclass <= REGCLASS_5G_40MHZ_LAST) {
				for (j = 0; j < rc_chan_map[i].count; j++) {
					if (channel == rc_chan_map[i].channel[j]) {
						break;
					}
				}
				if (j == rc_chan_map[i].count) {
					rc_map->regclass = rc_chan_map[i].regclass;
					chan_pref.rc_count++;
					WBD_DEBUG("[%d]:%d (control channel %d not present)\n",
						chan_pref.rc_count, rc_chan_map[i].regclass,
						channel);
					continue;
				}
			}

			/* Now consider all regulatory classes which has center channels. Since
			 * these channels will have multiple control channels/center channels of
			 * lower bandwidth, calculate an offset and skip all the channels within
			 * the offset limit
			 * Note: For control channel cases, the offset will be zero.
			 */
			switch (opClass) {
				case REGCLASS_5G_160MHZ:
					if (rc_chan_map[i].regclass == REGCLASS_5G_80MHZ) {
						offset = 6;
					}
					break;
				case REGCLASS_6G_80MHZ:
					if (rc_chan_map[i].regclass == REGCLASS_6G_40MHZ) {
						offset = 2;
					}
					break;
				case REGCLASS_6G_160MHZ:
					if (rc_chan_map[i].regclass == REGCLASS_6G_80MHZ) {
						offset = 6;
					} else if (rc_chan_map[i].regclass == REGCLASS_6G_40MHZ) {
						offset = 2;
					}
					break;
			}

			/* Now skip the center/control channels within the offset */
			for (j = 0; j < rc_chan_map[i].count; j++) {
				if (rc_chan_map[i].regclass > REGCLASS_5G_40MHZ_LAST) {
					/* centre channel exact match.
					 * Note: 6G 20MHz also will come here. It is ok because
					 * both center and control channel are same in 20MHz
					 */
					if (center_channel == rc_chan_map[i].channel[j]) {
						WBD_DEBUG("Skip channel %d (regclass: %d)\n",
							rc_chan_map[i].channel[j],
							rc_chan_map[i].regclass);
						continue;
					}
				}
				if (abs(channel - rc_chan_map[i].channel[j]) <= offset) {
					WBD_DEBUG("Skip channel %d (regclass: %d) "
						"control channel %d offset %d\n",
						rc_chan_map[i].channel[j], rc_chan_map[i].regclass,
						channel, offset);
					continue;
				}

				if (rc_map->regclass != rc_chan_map[i].regclass) {
					rc_map->regclass = rc_chan_map[i].regclass;
					chan_pref.rc_count++;
					WBD_DEBUG("[%d]: %d Channels:\n", chan_pref.rc_count,
						rc_chan_map[i].regclass);
				}
				rc_map->channel[rc_map->count] = rc_chan_map[i].channel[j];
				WBD_DEBUG("%d\n", rc_map->channel[rc_map->count]);
				rc_map->count++;
			}
		}
validate:
		/* Check if at least one valid channel is present in the list,
		 * by comparing it agaist agent's channel preference
		 */
		for (i = 0; i < reg_class_count; i++) {
			int ch_count = 0;
			uint8 rc = 0;
			int invalid = 0;
			uint8 count = 0;
			const uint8 *chan_list = NULL;
			/* Find a regulatory class in Radio capabilities */
			for (j = 0, rcidx = 1; j < rcaps_rc_count && rcidx < RadioCaps->Len; j++) {
				rc = RadioCaps->List[rcidx++];

				rcidx++; /* Max Transmit power */
				ch_count = RadioCaps->List[rcidx++];
				if (rc_chan_map[i].regclass == rc) {
					break;
				} else {
					rcidx += ch_count;
				}
			}
			if (rc_chan_map[i].regclass != rc) {
				WBD_DEBUG("Regulatory Class [%d] not found in Radio Capabilities\n",
					rc_chan_map[i].regclass);
				continue;
			}

			count = rc_chan_map[i].count;
			chan_list = rc_chan_map[i].channel;

			for (j = 0; j < count; j++) {
				int chidx = rcidx;
				invalid = 0;
				for (k = 0; k < ch_count; k++, chidx++) {
					if (RadioCaps->List[chidx] == chan_list[j]) {
						WBD_DEBUG("Channel [%d] is invalid in RadioCaps\n",
							chan_list[j]);
						invalid = 1;
						break;
					}
				}
				if (invalid) {
					WBD_DEBUG("Not valid Channel [%d] in Radio capabilities\n",
						rc_chan_map[i].channel[j]);
					continue;
				}
				/* rc_chan_map[i].channel[j] is valid in Radio capabilities.
				 * check if it is valid in master and local preferences
				 */
				invalid = wbd_master_check_regclass_channel_invalid(chan_list[j],
					rc_chan_map[i].regclass, &chan_pref);
				if (invalid) {
					WBD_DEBUG("regclass[%d] Not valid Channel [%d] "
							"in Master preference\n",
							rc_chan_map[i].regclass, chan_list[j]);
					continue;
				}
				invalid = wbd_master_check_regclass_channel_invalid(chan_list[j],
					rc_chan_map[i].regclass, &interface->ChanPrefs);
				if (invalid == 0) {
					break;
				}
				WBD_DEBUG("Not valid Channel [%d] in Agent preference\n",
					rc_chan_map[i].channel[j]);

			}
			if (invalid) {
				continue;
			}

			WBD_INFO("Regulatory class [%d] channel [%d] is valid at both Master and "
				"Agent\n", rc_chan_map[i].regclass, chan_list[j]);

			ieee1905_insert_channel_selection_request_tlv(pmsg, interface->InterfaceId,
				&chan_pref);

			if (!WBD_MCHAN_ENAB(info->flags) &&
				!WBD_IS_DEDICATED_BACKHAUL(interface->mapFlags) &&
				!I5_IS_CTRL_CHAN_SELECT(device->flags) &&
				(opClass > REGCLASS_5G_40MHZ_LAST) &&
				(opClass != REGCLASS_6G_FIRST) && (opClass != REGCLASS_6G_LAST)) {
				/* Send control channel for global operating classes which has
				 * center channels in Table E-4 of spec. usng vendor message.
				 * Required only for single channel operation. BRCM agent
				 * use this information to set the channel and ignore channel
				 * preference report present in channel selection request.
				 */
				vndr_set_chan.cntrl_chan = channel;
				vndr_set_chan.rclass = opClass;
				memcpy(&vndr_set_chan.mac, interface->InterfaceId, ETHER_ADDR_LEN);
				wbd_ds_add_vndr_chan_set_info_to_list(
					&info->wbd_master->vndr_set_chan_list, &vndr_set_chan);
			}

			break;
		}
	}

	ieee1905_send_message(pmsg);

end:
	if (chan_pref.rc_map) {
		free(chan_pref.rc_map);
	}
	WBD_EXIT();
}

/* Process Non-operable Chspec Update of an IFR, & Send Consolidated Exclude list to ACSD */
int
wbd_master_process_nonoperable_channel_update(i5_dm_interface_type *in_ifr, int dynamic)
{
	int ret = WBDE_OK, cli_ver = 1, sock_options = 0x0000, final_ch_count = 0;
	char *s_chlist = NULL, *d_chlist = NULL, *final_chlist = NULL, *tmpbuf = NULL;
	int chlist_sz = 0, final_chlist_sz = 0, tmpbuf_sz = 0;
	i5_dm_network_topology_type *i5_topology = NULL;
	i5_dm_device_type *iter_dev = NULL;
	i5_dm_interface_type *iter_ifr = NULL;
	wbd_ifr_item_t *ifr_vndr_data = NULL;
	char ifname[IFNAMSIZ + 2] = {0};
	wbd_info_t *info = wbd_get_ginfo();
	wbd_com_handle *com_hndl = NULL;
	char excl_chspec[10] = {0}, *excl5g_list = NULL, *next;
	uint8 or_band = 0, and_band = 0, excl_band = 0;
	WBD_ENTER();

	/* Validate arg */
	WBD_ASSERT_ARG(in_ifr, WBDE_INV_ARG);

	/* Controller Find Interface on its device, Matching with Input IFR's Band */
	ret = wbd_master_get_ifname_matching_band(in_ifr->band,
		ifname, sizeof(ifname));
	WBD_ASSERT();

	/* Max Possible length of a comma separated string having Non-Operable Chanspecs in a radio.
	 * e.g. : 0xd024,0xd028,0xd032   and so on, for each entry 7 char needed including comma
	 */
	final_chlist_sz = chlist_sz = MAP_MAX_NONOP_CHSPEC * 7;
	tmpbuf_sz = WBD_MAX_BUF_256 + final_chlist_sz;

	s_chlist = (char*)wbd_malloc(chlist_sz, &ret);
	WBD_ASSERT();
	d_chlist = (char*)wbd_malloc(chlist_sz, &ret);
	WBD_ASSERT();
	final_chlist = (char*)wbd_malloc(final_chlist_sz, &ret);
	WBD_ASSERT();
	tmpbuf = (char*)wbd_malloc(tmpbuf_sz, &ret);
	WBD_ASSERT();

	/* Get the info data from i5 topology */
	i5_topology = ieee1905_get_datamodel();

	/* Loop for all the Devices in Controller Topology */
	foreach_i5glist_item(iter_dev, i5_dm_device_type, i5_topology->device_list) {

		if (!I5_IS_MULTIAP_AGENT(iter_dev->flags)) {
			continue; /* Devices other than Agents, Skip */
		}

		/* Loop for all the Interfaces in this Device */
		foreach_i5glist_item(iter_ifr, i5_dm_interface_type, iter_dev->interface_list) {

			if (!(iter_ifr->band & in_ifr->band)) {
				continue; /* Interfaces not matching with Input IFR's Band, Skip */
			}
			ifr_vndr_data = (wbd_ifr_item_t *)iter_ifr->vndr_data;
			if (!ifr_vndr_data) {
				continue; /* Interface's vendor data not available, Skip */
			}
			/* Clear STATIC, DYNAMIC Exclude Lists of this IFR */
			memset(s_chlist, 0, chlist_sz);
			memset(d_chlist, 0, chlist_sz);

			/* If Input IFR band Type is 5G, collect OR & AND of IFRs on all devices */
			if (WBD_BAND_TYPE_LAN_5G(iter_ifr->band) &&
				I5_IS_BSS_BACKHAUL(iter_ifr->mapFlags)) {
				or_band |= iter_ifr->band;
				and_band = and_band ? (and_band & iter_ifr->band) : iter_ifr->band;
			}

			/* Convert STATIC ChspecList to Str of Hex separated by space */
			ret = wbd_convert_chspeclist_to_string(s_chlist, chlist_sz,
				&ifr_vndr_data->static_exclude_chlist);

			/* Convert DYNAMIC ChspecList to Str of Hex separated by space */
			ret = wbd_convert_chspeclist_to_string(d_chlist, chlist_sz,
				&ifr_vndr_data->dynamic_exclude_chlist);

			/* Concat STATIC & DYNAMIC Lists to Final List by removing duplicates */
			wbd_concat_list(s_chlist, d_chlist, final_chlist, final_chlist_sz);
		}
	}

	/* For Dual+Tri Band combination at Root & Repeater, Exclude Neighbor Band Chanspecs */
	if ((or_band > 0) && (and_band > 0) && (or_band > and_band)) {

		excl_band = or_band - and_band;

		if ((excl_band == BAND_5GL) || (excl_band == BAND_5GH)) {
			excl5g_list = ((excl_band == BAND_5GL) ?
				CHSPEC_LIST_5GL : CHSPEC_LIST_5GH);
			foreach(excl_chspec, excl5g_list, next) {
				add_to_list(excl_chspec, final_chlist, final_chlist_sz);
			}
		}
	}
	/* Count total # of Exclude Chanspecs to be send to ACSD */
	final_ch_count = wbd_count_needles(final_chlist);

	/* Replace all occurrences of a char ' ' with ',' to send to ACSD */
	wbd_strrep(final_chlist, ' ', ',');
	WBD_DEBUG("IFR[%s] Send Exclude Chspeclist CNT[%d] len [%d] to ACSD : [%s]\n",
		ifname, final_ch_count, final_chlist_sz, final_chlist);

	snprintf(tmpbuf, tmpbuf_sz, "set&ifname=%s&param=excludechspeclist&value=%d"
		"&nexclude=%d&excludelist=%s",
		ifname, cli_ver, final_ch_count, final_chlist);

	sock_options = WBD_COM_FLAG_CLIENT | WBD_COM_FLAG_BLOCKING_SOCK
			| WBD_COM_FLAG_NO_RESPONSE;

	com_hndl = wbd_com_init(info->hdl, INVALID_SOCKET, sock_options, NULL, NULL, info);

	if (!com_hndl) {
		WBD_ERROR("IFR[%s] Failed to initialize the communication module to forward "
			"Exclude Chspeclist CNT[%d] to ACSD: [%s].\n",
			ifname, final_ch_count, final_chlist);
		ret = WBDE_COM_ERROR;
		goto end;
	}

	/* Send the command to ACSD */
	ret = wbd_com_connect_and_send_cmd(com_hndl, ACSD_DEFAULT_CLI_PORT,
		WBD_LOOPBACK_IP, tmpbuf, NULL);
	WBD_CHECK_MSG("IFR[%s] Failed to forward "
		"Exclude Chspeclist CNT[%d] to ACSD: [%s]. Error : %s\n",
		ifname, final_ch_count, final_chlist, wbderrorstr(ret));

end:
	if (com_hndl) {
		wbd_com_deinit(com_hndl);
	}
	if (s_chlist) {
		free(s_chlist);
	}
	if (d_chlist) {
		free(d_chlist);
	}
	if (final_chlist) {
		free(final_chlist);
	}
	if (tmpbuf) {
		free(tmpbuf);
	}
	WBD_EXIT();
	return ret;
}

#if defined(MULTIAPR2)
/* Unsuccessful association config policy value fetch from NVRAM "map_unsuccessful_assoc_policy" */
void
wbd_master_fetch_unsuccessful_association_nvram_config(void)
{
	int num = 0;
	int report_flag;
	ieee1905_unsuccessful_assoc_config_t unsuccessful_association;
	char *nvval;

	memset(&unsuccessful_association, 0, sizeof(unsuccessful_association));
	nvval = nvram_safe_get(MAP_NVRAM_UNSUCCESSFUL_ASSOC_POLICY);
	num = sscanf(nvval, "%d %d", &report_flag,
			&unsuccessful_association.max_reporting_rate);
	if (report_flag == 1) {
		unsuccessful_association.report_flag |= MAP_UNSUCCESSFUL_ASSOC_FLAG_REPORT;
	}
	if (num == 2) {
		ieee1905_add_unsuccessful_association_policy(&unsuccessful_association);
	}
	WBD_INFO("NVRAM[%s=%s] REPORT_FLAG[0x%02x] MAX_REPORTING_RATE[%d]\n ",
		MAP_NVRAM_UNSUCCESSFUL_ASSOC_POLICY, nvval, unsuccessful_association.report_flag,
		unsuccessful_association.max_reporting_rate);
}
#endif /* MULTIAPR2 */

/* Add the metric policy for a radio */
void
wbd_master_add_metric_report_policy(wbd_info_t *info,
	unsigned char* radio_mac, int if_band)
{
	int ret = WBDE_OK, chan_util_def = 0;
	wbd_master_info_t *master = NULL;
	ieee1905_ifr_metricrpt metricrpt;
	wbd_weak_sta_policy_t *metric_policy;
	char *nvname;
	WBD_ENTER();

	/* get the master info for the band */
	WBD_SAFE_GET_MASTER_INFO(info, WBD_BKT_ID_BR0, master, &ret);
	if (if_band & WBD_BAND_LAN_2G) {
		metric_policy = &master->metric_policy_2g;
		nvname = WBD_NVRAM_CHAN_UTIL_THLD_2G;
		chan_util_def = WBD_DEF_CHAN_UTIL_THLD_2G;
	} else if (if_band & WBD_BAND_LAN_6G) {
		metric_policy = &master->metric_policy_6g;
		nvname = WBD_NVRAM_CHAN_UTIL_THLD_6G;
		chan_util_def = WBD_DEF_CHAN_UTIL_THLD_2G;
	} else {
		metric_policy = &master->metric_policy_5g;
		nvname = WBD_NVRAM_CHAN_UTIL_THLD_5G;
		chan_util_def = WBD_DEF_CHAN_UTIL_THLD_5G;
	}

	/* Fill the metric policy for a radio to be added to MAP */
	memset(&metricrpt, 0, sizeof(metricrpt));
	memcpy(metricrpt.mac, radio_mac, sizeof(metricrpt.mac));
	metricrpt.sta_mtrc_policy_flag = (unsigned char)master->metric_policy_flags;
	metricrpt.sta_mtrc_rssi_thld = (unsigned char)WBD_RSSI_TO_RCPI(metric_policy->t_rssi);
	metricrpt.sta_mtrc_rssi_hyst = (unsigned char)metric_policy->t_hysterisis;
	metricrpt.ap_mtrc_chan_util =
		(unsigned char)blanket_get_config_val_int(NULL, nvname, chan_util_def);
	ieee1905_add_metric_reporting_policy_for_radio(
		master->metric_policy_ap_rpt_intvl, &metricrpt);
#if defined(MULTIAPR2)
	wbd_master_fetch_unsuccessful_association_nvram_config();
#endif /* MULTIAPR2 */
end:
	WBD_EXIT();
}

#if defined(MULTIAPR2)
/* Add the Channel Scan Reporting policy for a radio */
void
wbd_master_add_chscan_report_policy()
{
	ieee1905_chscanrpt_config chscanrpt;
	WBD_ENTER();

	/* Fill the Channel Scan Reporting Policy for a radio to be added to MAP */
	memset(&chscanrpt, 0, sizeof(chscanrpt));

	/* IF Independent Channel Scan Reporting is enabled */
	if (WBD_BKT_INDEP_CHSCAN_ENAB(wbd_get_ginfo()->wbd_master->flags)) {

		/* The Channel Scan Reporting Policy TLV's,  Report Independent Channel Scans
		 * Flag's bit 7 identifies whether a SmartMesh Agent should report the results
		 * of any Independent Channel Scan that it performs to the SmartMesh Controller.
		 * bit 7    1 - Report Independent Channel Scans
		 *          0 - Do not report Independent Channel Scans
		 */
		chscanrpt.chscan_rpt_policy_flag |= MAP_CHSCAN_RPT_POL_INDEP_SCAN;
	}

	ieee1905_add_chscan_reporting_policy_for_radio(&chscanrpt);

	WBD_EXIT();
}
#endif /* MULTIAPR2 */

/* Send Policy Configuration for a radio */
void
wbd_master_send_policy_config(wbd_info_t *info, unsigned char *al_mac,
	unsigned char* radio_mac, int if_band)
{
	WBD_ENTER();

	wbd_master_add_metric_report_policy(info, radio_mac, if_band);
#if defined(MULTIAPR2)
	wbd_master_add_chscan_report_policy();
#endif /* MULTIAPR2 */
	ieee1905_send_policy_config(al_mac);

	WBD_EXIT();
}

/* Get Vendor Specific TLV  for Metric Reporting Policy from Master */
void
wbd_master_get_vndr_tlv_metric_policy(ieee1905_vendor_data *out_vendor_tlv)
{
	int ret = WBDE_OK, band;
	i5_dm_device_type *dst_device;
	i5_dm_interface_type *iter_ifr;
	wbd_cmd_vndr_metric_policy_config_t policy_config; /* VNDR_METRIC_POLICY Data */
	wbd_metric_policy_ifr_entry_t new_policy_ifr;
	wbd_cmd_vndr_strong_sta_policy_config_t strong_sta_policy_config;
	wbd_strong_sta_policy_ifr_entry_t new_strong_sta_policy_ifr;
	wbd_master_info_t *master = NULL;
	WBD_ENTER();

	WBD_INFO("Get Vendor Specific TLV for Metric Reporting Policy for Device["MACDBG"]\n",
		MAC2STRDBG(out_vendor_tlv->neighbor_al_mac));

	/* Initialize Vendor Metric Policy struct Data */
	memset(&policy_config, 0, sizeof(policy_config));
	wbd_ds_glist_init(&(policy_config.entry_list));

	/* Initialize Vendor Strong Sta Policy struct Data */
	memset(&strong_sta_policy_config, 0, sizeof(strong_sta_policy_config));
	wbd_ds_glist_init(&(strong_sta_policy_config.entry_list));

	/* Get the Device from AL_MAC, for which, this Vendor TLV is required, to be filled */
	WBD_SAFE_FIND_I5_DEVICE(dst_device, out_vendor_tlv->neighbor_al_mac, &ret);

	/* Loop for all the Interfaces in this Device */
	foreach_i5glist_item(iter_ifr, i5_dm_interface_type, dst_device->interface_list) {

		wbd_weak_sta_policy_t *metric_policy;

		/* Loop for, only for wireless interfaces with valid chanspec & BSSes */
		if (!i5DmIsInterfaceWireless(iter_ifr->MediaType) ||
			!iter_ifr->BSSNumberOfEntries ||
			!iter_ifr->isConfigured) {
			continue;
		}

		/* Find matching Master for this Interface's Band, to get Policy Config */
		band = WBD_BAND_FROM_1905_BAND(iter_ifr->band);
		WBD_DS_GET_MASTER_INFO(wbd_get_ginfo(), WBD_BKT_ID_BR0, master, (&ret));
		if (ret != WBDE_OK) {
			ret = WBDE_OK;
			continue;
		}
		if (band & WBD_BAND_LAN_2G) {
			metric_policy = &master->metric_policy_2g;
		} else if (band & WBD_BAND_LAN_6G) {
			metric_policy = &master->metric_policy_6g;
		} else {
			metric_policy = &master->metric_policy_5g;
		}

		WBD_INFO("Adding new entry to VNDR_METRIC_POLICY : BKTID[%d] "
			"BSSNumberOfEntries[%d] RAdio_Configured[%d]\n",
			master->bkt_id, iter_ifr->BSSNumberOfEntries, iter_ifr->isConfigured);

		memset(&new_policy_ifr, 0, sizeof(new_policy_ifr));

		/* Prepare a new Metric Policy item for Vendor Metric Policy List */
		memcpy(new_policy_ifr.ifr_mac, iter_ifr->InterfaceId,
			sizeof(new_policy_ifr.ifr_mac));

		/* Copy Vendor Metric Policy for this Radio, in Vendor Metric Policy List */
		new_policy_ifr.vndr_policy.t_idle_rate = metric_policy->t_idle_rate;
		new_policy_ifr.vndr_policy.t_tx_rate = metric_policy->t_tx_rate;
		new_policy_ifr.vndr_policy.t_tx_failures = metric_policy->t_tx_failures;
		new_policy_ifr.vndr_policy.flags = metric_policy->flags;

		WBD_INFO("Adding new entry to VNDR_METRIC_POLICY for : "
			"Device["MACDBG"] IFR_MAC["MACDBG"] "
			"t_idle_rate[%d] t_tx_rate[%d] t_tx_failures[%d] Flags[0x%X]\n",
			MAC2STRDBG(out_vendor_tlv->neighbor_al_mac),
			MAC2STRDBG(new_policy_ifr.ifr_mac),
			new_policy_ifr.vndr_policy.t_idle_rate,
			new_policy_ifr.vndr_policy.t_tx_rate,
			new_policy_ifr.vndr_policy.t_tx_failures,
			new_policy_ifr.vndr_policy.flags);

		/* Add a Metric Policy item in a Metric Policy List */
		wbd_add_item_in_metric_policylist(&new_policy_ifr, &policy_config, NULL);

		/* Increament Total item count */
		policy_config.num_entries++;
	}

	/* Encode Vendor Specific TLV : Metric Reporting Policy Vendor Data to send */
	wbd_tlv_encode_vndr_metric_policy((void*)&policy_config, out_vendor_tlv->vendorSpec_msg,
		&(out_vendor_tlv->vendorSpec_len));

	/* Loop for all the Interfaces in this Device */
	foreach_i5glist_item(iter_ifr, i5_dm_interface_type, dst_device->interface_list) {

		wbd_strong_sta_policy_t *strong_sta_policy;

		/* Loop for, only for wireless interfaces with valid chanspec & BSSes */
		if (!i5DmIsInterfaceWireless(iter_ifr->MediaType) ||
			!iter_ifr->BSSNumberOfEntries ||
			!iter_ifr->isConfigured) {
			continue;
		}

		/* Find matching Master for this Interface's Band, to get Policy Config */
		band = WBD_BAND_FROM_1905_BAND(iter_ifr->band);
		WBD_DS_GET_MASTER_INFO(wbd_get_ginfo(), WBD_BKT_ID_BR0, master, (&ret));
		if (ret != WBDE_OK) {
			ret = WBDE_OK;
			continue;
		}
		if (band & WBD_BAND_LAN_2G) {
			strong_sta_policy = &master->strong_metric_policy_2g;
		} else if (band & WBD_BAND_LAN_6G) {
			continue;
		} else {
			strong_sta_policy = &master->strong_metric_policy_5g;
		}

		WBD_INFO("Adding new entry to VNDR_STRONG_STA_POLICY : BKTID[%d] "
			"BSSNumberOfEntries[%d] Radio_Configured[%d]\n",
			master->bkt_id, iter_ifr->BSSNumberOfEntries, iter_ifr->isConfigured);

		memset(&new_strong_sta_policy_ifr, 0, sizeof(new_strong_sta_policy_ifr));

		/* Prepare a new Strong Sta Policy item for Vendor Strong Sta Policy List */
		memcpy(new_strong_sta_policy_ifr.ifr_mac, iter_ifr->InterfaceId,
			sizeof(new_strong_sta_policy_ifr.ifr_mac));

		/* Copy Vendor Strong Sta Policy for this Radio in Vendor Strong Sta Policy List */
		new_strong_sta_policy_ifr.vndr_policy.t_idle_rate = strong_sta_policy->t_idle_rate;
		new_strong_sta_policy_ifr.vndr_policy.t_rssi = strong_sta_policy->t_rssi;
		new_strong_sta_policy_ifr.vndr_policy.t_hysterisis =
			strong_sta_policy->t_hysterisis;
		new_strong_sta_policy_ifr.vndr_policy.flags = strong_sta_policy->flags;

		WBD_INFO("Adding new entry to VNDR_STRONG_STA_POLICY for : "
			"Device["MACDBG"] IFR_MAC["MACDBG"] "
			"t_idle_rate[%d] t_rssi[%d] t_hysterisi[%d] Flags[0x%X]\n",
			MAC2STRDBG(out_vendor_tlv->neighbor_al_mac),
			MAC2STRDBG(new_strong_sta_policy_ifr.ifr_mac),
			new_strong_sta_policy_ifr.vndr_policy.t_idle_rate,
			new_strong_sta_policy_ifr.vndr_policy.t_rssi,
			new_strong_sta_policy_ifr.vndr_policy.t_hysterisis,
			new_strong_sta_policy_ifr.vndr_policy.flags);

		/* Add a Metric Policy item in a Metric Policy List */
		wbd_add_item_in_strong_sta_policylist(&new_strong_sta_policy_ifr,
			&strong_sta_policy_config, NULL);

		/* Increament Total item count */
		strong_sta_policy_config.num_entries++;
	}

	/* Encode Vendor Specific TLV : Metric Reporting Policy Vendor Data to send */
	wbd_tlv_encode_vndr_strong_sta_policy((void*)&strong_sta_policy_config,
		out_vendor_tlv->vendorSpec_msg,
		&(out_vendor_tlv->vendorSpec_len));
end:
	/* If Vendor Metric Policy List is filled up */
	if (policy_config.num_entries > 0) {

		/* Remove all Vendor Metric Policy data items */
		wbd_ds_glist_cleanup(&(policy_config.entry_list));
	}

	/* If Vendor Strong Sta Policy List is filled up */
	if (strong_sta_policy_config.num_entries > 0) {

		/* Remove all Vendor Strong Sta Policy data items */
		wbd_ds_glist_cleanup(&(strong_sta_policy_config.entry_list));
	}

	WBD_EXIT();
}

/* Create the timer to remove stale client entry for BSS */
static int
wbd_controller_create_remove_client_timer(wbd_remove_client_arg_t* arg, int timeout)
{
	int ret = WBDE_OK;
	WBD_ENTER();

	/* Validate arg */
	WBD_ASSERT_ARG(arg, WBDE_INV_ARG);

	/* Create a timer to send REMOVE_CLIENT_REQ cmd to selected BSS */
	ret = wbd_add_timers(wbd_get_ginfo()->hdl, arg,
		WBD_SEC_MICROSEC(timeout), wbd_controller_remove_client_timer_cb, 0);
	if (ret != WBDE_OK) {
		WBD_WARNING("Interval[%d] Failed to create REMOVE_CLIENT_REQ timer\n",
			timeout);
	}

end:

	WBD_EXIT();
	return ret;
}

/* Remove stale client entry form BSS, if required */
static void
wbd_controller_remove_client_timer_cb(bcm_usched_handle *hdl, void *arg)
{
	int ret = WBDE_OK, bss_count = 0;
	bool recreate = FALSE;
	i5_dm_network_topology_type *i5_topology = NULL;
	i5_dm_device_type *device = NULL;
	i5_dm_interface_type *i5_ifr;
	i5_dm_bss_type *i5_bss, *i5_recent_bss;
	i5_dm_clients_type *sta = NULL, *i5_recent_sta = NULL;
	wbd_cmd_remove_client_t cmd;
	ieee1905_vendor_data vndr_msg_data;
	wbd_remove_client_arg_t* remove_client_arg = NULL;
	struct timeval assoc_time = {0, 0};
	WBD_ENTER();

	memset(&cmd, 0, sizeof(cmd));
	memset(&vndr_msg_data, 0, sizeof(vndr_msg_data));

	/* Validate arg */
	WBD_ASSERT_ARG(arg, WBDE_INV_ARG);

	remove_client_arg = (wbd_remove_client_arg_t*)arg;

	/* Count how many BSSes has the STA entry in Controller Topology.
	 * Also get the assoc_time of STA which is associated to the BSS most recently.
	 */
	bss_count = wbd_ds_find_duplicate_sta_in_controller(&remove_client_arg->sta_mac,
		&assoc_time, &i5_recent_sta);
	/* No need to process further as only single BSS hold the client entry */
	if (bss_count <= 1 || i5_recent_sta == NULL) {
		goto end;
	}

	/* Copy STA MAC to REMOVE_CLIENT_REQ msg data */
	eacopy(&remove_client_arg->sta_mac, &cmd.stamac);

	i5_recent_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_recent_sta);
	WBD_INFO("STA "MACF" associated in BSS "MACF" is in assoclist of %d BSSs. Current BSS "
		"assoctime ["TIMEVALF"]. Recent BSS for the STA is "MACF"\n",
		ETHER_TO_MACF(remove_client_arg->sta_mac),
		ETHER_TO_MACF(remove_client_arg->parent_bssid), bss_count,
		TIMEVAL_TO_TIMEVALF(assoc_time), MAC2STRDBG(i5_recent_bss->BSSID));

	/* Copy bssid to REMOVE_CLIENT_REQ msg data. Copy the recent STA's BSSID */
	eacopy(&i5_recent_bss->BSSID, &cmd.bssid);

	/* Allocate Dynamic mem for Vendor data from App */
	vndr_msg_data.vendorSpec_msg =
		(unsigned char *)wbd_malloc(IEEE1905_MAX_VNDR_DATA_BUF, &ret);
	WBD_ASSERT();

	/* Encode Vendor Specific TLV for Message : REMOVE_CLIENT_REQ to send */
	wbd_tlv_encode_remove_client_request((void *)&cmd,
		vndr_msg_data.vendorSpec_msg, &vndr_msg_data.vendorSpec_len);

	/* More than one BSS has STA entry in assoclist, so send the REMOVE_CLIENT_REQ msg to
	 * all the BSSes whose assoc time is less than the latest associated BSSes
	 */
	i5_topology = ieee1905_get_datamodel();
	foreach_i5glist_item(device, i5_dm_device_type, i5_topology->device_list) {

		/* Loop only for Agent Devices */
		if (!I5_IS_MULTIAP_AGENT(device->flags)) {
			continue;
		}

		foreach_i5glist_item(i5_ifr, i5_dm_interface_type, device->interface_list) {
			foreach_i5glist_item(i5_bss, i5_dm_bss_type, i5_ifr->bss_list) {
				sta = i5DmFindClientInBSS(i5_bss,
					(unsigned char *)&remove_client_arg->sta_mac);
				if (sta == NULL) {
					continue;
				}

				WBD_INFO("STA["MACF"] BSS["MACF"] bss_count[%d] "
					"assoc_time["TIMEVALF"] "
					"sta->assoc_time["TIMEVALF"]\n",
					ETHER_TO_MACF(remove_client_arg->sta_mac),
					ETHER_TO_MACF(remove_client_arg->parent_bssid),
					bss_count, TIMEVAL_TO_TIMEVALF(assoc_time),
					TIMEVAL_TO_TIMEVALF(sta->assoc_tm));

				/* CSTYLED */
				if (timercmp(&sta->assoc_tm, &assoc_time, <)) {

					/* Fill Destination AL_MAC in Vendor data */
					memcpy(vndr_msg_data.neighbor_al_mac,
						device->DeviceId, IEEE1905_MAC_ADDR_LEN);
					WBD_INFO("Send REMOVE_CLIENT_REQ from Device["MACDBG"] "
						"to Device["MACDBG"]\n",
						MAC2STRDBG(ieee1905_get_al_mac()),
						MAC2STRDBG(device->DeviceId));

					/* Send Vendor Specific Message : REMOVE_CLIENT_REQ */
					ret = wbd_master_send_brcm_vndr_msg(&vndr_msg_data);

					/* if remove client message send failed. recreate the
					 * timer
					 */
					if (ret != WBDE_OK) {
						recreate = TRUE;
					}
				}
			}
		}
	}

end:
	if (remove_client_arg) {
		if (recreate) {
			/* Stop the timer to create it again with different time */
			wbd_remove_timers(hdl, wbd_controller_remove_client_timer_cb,
				remove_client_arg);

			/* Re-try creating timer to remove stale client entry for BSS */
			wbd_controller_create_remove_client_timer(remove_client_arg,
				wbd_get_ginfo()->max.tm_retry_remove_client);
		} else {
			free(remove_client_arg);
		}
	}
	if (vndr_msg_data.vendorSpec_msg) {
		free(vndr_msg_data.vendorSpec_msg);
	}
	WBD_EXIT();
}

/* Processes the STEER CLI command */
static void
wbd_master_process_steer_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;
	wbd_info_t *info = (wbd_info_t*)arg;
	char outdata[WBD_MAX_BUF_256] = {0};
	struct ether_addr mac;
	struct ether_addr bssid;
	int ret = WBDE_OK;
	i5_dm_bss_type *i5_bss = NULL, *i5_tbss = NULL;
	i5_dm_interface_type *i5_ifr = NULL;
	i5_dm_device_type *i5_device = NULL;
	i5_dm_clients_type *sta = NULL;
	char logmsg[WBD_MAX_BUF_128] = {0}, timestamp[WBD_MAX_BUF_32] = {0};

	/* Validate Message data */
	WBD_ASSERT_MSGDATA(clidata, "STEER_CLI");

	/* Get & Validate mac */
	WBD_GET_VALID_MAC(clidata->mac, &mac, "STEER_CLI", WBDE_INV_MAC);

	/* Get & Validate tbss's bssid */
	WBD_GET_VALID_MAC(clidata->bssid, &bssid, "STEER_CLI", WBDE_INV_BSSID);

	/* Check if the STA  exists or not */
	sta = wbd_ds_find_sta_in_topology((unsigned char*)&mac, &ret);
	WBD_ASSERT_MSG("STA["MACF"]. %s\n", ETHER_TO_MACF(mac), wbderrorstr(ret));

	i5_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(sta);
	i5_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_bss);
	i5_device = (i5_dm_device_type*)WBD_I5LL_PARENT(i5_ifr);

	i5_tbss = wbd_ds_get_i5_bss_in_topology((unsigned char *)&bssid, &ret);
	WBD_ASSERT_MSG("TBSS["MACF"]. %s\n", ETHER_TO_MACF(mac), wbderrorstr(ret));

	wbd_master_send_steer_req(i5_device->DeviceId, sta->mac, i5_bss->BSSID, i5_tbss);

	/* Create and store steer log. */
	snprintf(logmsg, sizeof(logmsg), CLI_CMD_LOGS_STEER,
		wbd_get_formated_local_time(timestamp, sizeof(timestamp)),
		MAC2STRDBG(sta->mac), MAC2STRDBG(i5_bss->BSSID), MAC2STRDBG(i5_tbss->BSSID));
	wbd_ds_add_logs_in_master(info->wbd_master, logmsg);

end: /* Check Master Pointer before using it below */

	snprintf(outdata, sizeof(outdata), "%s\n", wbderrorstr(ret));

	if (wbd_com_send_cmd(hndl, childfd, outdata, NULL) != WBDE_OK) {
		WBD_WARNING("STEER CLI : %s\n", wbderrorstr(WBDE_SEND_RESP_FL));
	}

	if (clidata)
		free(clidata);

	return;
}

/* -------------------------- Add New Functions above this -------------------------------- */

/* Get the SLAVELIST CLI command data */
static int
wbd_master_process_bsslist_cli_cmd_data(wbd_info_t *info, wbd_cli_send_data_t *clidata,
	char **outdataptr)
{
	int ret = WBDE_OK;
	char *outdata = NULL;
	int count = 0, band_check = 0, bssid_check = 0;
	int outlen = WBD_MAX_BUF_8192, len = 0;
	struct ether_addr bssid;
	i5_dm_network_topology_type *i5_topology;
	i5_dm_device_type *device;
	i5_dm_interface_type *ifr;
	i5_dm_bss_type *bss;

	/* Validate fn args */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);
	WBD_ASSERT_MSGDATA(clidata, "SLAVELIST_CLI");

	outdata = (char*)wbd_malloc(outlen, &ret);
	WBD_ASSERT();

	/* Check if Band is requested */
	if (WBD_BAND_VALID((clidata->band))) {
		band_check = 1;
	}

	if (strlen(clidata->bssid) > 0) {
		/* Get Validated Non-NULL BSSID */
		WBD_GET_VALID_MAC(clidata->bssid, &bssid, "SLAVELIST_CLI", WBDE_INV_BSSID);
		bssid_check = 1;
	}

	/* Get the info data from i5 topology */
	i5_topology = ieee1905_get_datamodel();
	foreach_i5glist_item(device, i5_dm_device_type, i5_topology->device_list) {
		ret = wbd_snprintf_i5_device(device, CLI_CMD_I5_DEV_FMT, &outdata, &outlen, &len,
			WBD_MAX_BUF_8192);
		foreach_i5glist_item(ifr, i5_dm_interface_type, device->interface_list) {
			/* Skip non-wireless or interfaces with zero bss count */
			if (!i5DmIsInterfaceWireless(ifr->MediaType) ||
				!ifr->BSSNumberOfEntries) {
				continue;
			}
			/* Band specific validation */
			if (band_check && clidata->band == ieee1905_get_band_from_channel(
				ifr->opClass, CHSPEC_CHANNEL(ifr->chanspec))) {
				continue;
			}
			foreach_i5glist_item(bss, i5_dm_bss_type, ifr->bss_list) {
				/* bssid check */
				if (bssid_check &&
					memcmp(&bssid, (struct ether_addr*)bss->BSSID,
						MAC_ADDR_LEN)) {
					continue;
				}

				ret = wbd_snprintf_i5_bss(bss, CLI_CMD_I5_BSS_FMT, &outdata,
					&outlen, &len, WBD_MAX_BUF_8192, FALSE, FALSE);
				count++;
			}
		}
	}

	if (!count) {
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"No entry found \n");
	}

end: /* Check Master Pointer before using it below */

	if (ret != WBDE_OK) {
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"%s\n", wbderrorstr(ret));
	}
	if (outdataptr) {
		*outdataptr = outdata;
	}
	return ret;
}

/* Processes the SLAVELIST CLI command */
static void
wbd_master_process_bsslist_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	int ret = WBDE_OK;
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;
	wbd_info_t *info = (wbd_info_t*)arg;
	char *outdata = NULL;
	BCM_REFERENCE(ret);

	ret = wbd_master_process_bsslist_cli_cmd_data(info, clidata, &outdata);

	if (outdata) {
		if (wbd_com_send_cmd(hndl, childfd, outdata, NULL) != WBDE_OK) {
			WBD_WARNING("SLAVELIST CLI : %s\n", wbderrorstr(WBDE_SEND_RESP_FL));
		}
		free(outdata);
	}

	if (clidata)
		free(clidata);

	return;
}

/* Get the INFO CLI command data */
static int
wbd_master_process_info_cli_cmd_data(wbd_info_t *info, wbd_cli_send_data_t *clidata,
	char **outdataptr)
{
	int ret = WBDE_OK, len = 0, outlen = WBD_MAX_BUF_8192;
	char *outdata = NULL;
	int count = 0, band_check = 0, bssid_check = 0;
	struct ether_addr bssid;
	i5_dm_network_topology_type *i5_topology;
	i5_dm_device_type *device;
	i5_dm_interface_type *ifr;
	i5_dm_bss_type *bss;

	/* Validate fn args */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);
	WBD_ASSERT_MSGDATA(clidata, "INFO_CLI");

	outdata = (char*)wbd_malloc(outlen, &ret);
	WBD_ASSERT();

	/* Check if Band is requested */
	if (WBD_BAND_VALID((clidata->band))) {
		band_check = 1;
	}

	if (strlen(clidata->bssid) > 0) {
		/* Get Validated Non-NULL BSSID */
		WBD_GET_VALID_MAC(clidata->bssid, &bssid, "INFO_CLI", WBDE_INV_BSSID);
		bssid_check = 1;
	}

	/* Get the info data from i5 topology */
	i5_topology = ieee1905_get_datamodel();
	foreach_i5glist_item(device, i5_dm_device_type, i5_topology->device_list) {
		ret = wbd_snprintf_i5_device(device, CLI_CMD_I5_DEV_FMT, &outdata, &outlen, &len,
			WBD_MAX_BUF_8192);
		foreach_i5glist_item(ifr, i5_dm_interface_type, device->interface_list) {
			/* Skip non-wireless or interfaces with zero bss count */
			if (!i5DmIsInterfaceWireless(ifr->MediaType) ||
				!ifr->BSSNumberOfEntries) {
				continue;
			}
			/* Band specific validation */
			if (band_check && clidata->band == ieee1905_get_band_from_channel(
				ifr->opClass, CHSPEC_CHANNEL(ifr->chanspec))) {
				continue;
			}
			foreach_i5glist_item(bss, i5_dm_bss_type, ifr->bss_list) {
				/* bssid check */
				if (bssid_check &&
					memcmp(&bssid, (struct ether_addr*)bss->BSSID,
						MAC_ADDR_LEN)) {
					continue;
				}
				ret = wbd_snprintf_i5_bss(bss, CLI_CMD_I5_BSS_FMT, &outdata,
					&outlen, &len, WBD_MAX_BUF_8192, TRUE, TRUE);
				count++;
			}
		}
	}

	if (!count) {
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"No entry found \n");
	}

end: /* Check Master Pointer before using it below */

	if (ret != WBDE_OK) {
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"%s\n", wbderrorstr(ret));
	}
	if (outdataptr) {
		*outdataptr = outdata;
	}
	return ret;
}

/* Processes the INFO CLI command */
static void
wbd_master_process_info_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;
	wbd_info_t *info = (wbd_info_t*)arg;
	char *outdata = NULL;
	int ret = WBDE_OK;

	BCM_REFERENCE(ret);

	if (clidata->flags & WBD_CLI_FLAGS_JSON) {
		outdata = wbd_json_create_cli_info(info, clidata);

	} else {
		ret = wbd_master_process_info_cli_cmd_data(info, clidata, &outdata);
	}

	if (outdata) {
		if (wbd_com_send_cmd(hndl, childfd, outdata, NULL) != WBDE_OK) {
			WBD_WARNING("INFO CLI : %s\n", wbderrorstr(WBDE_SEND_RESP_FL));
		}
		free(outdata);
	}

	if (clidata)
		free(clidata);

	return;
}

/* API to Compare AP Metrics Query & stats Timestamps */
static int
wbd_master_ap_metrics_ts_compare(wbd_ap_metrics_query_args_t *apm_query_data)
{
	if (!apm_query_data || !apm_query_data->query_bss) {
		return 0;
	}
	WBD_INFO("ts_query_sent[%lu] ts_stats_recvd[%lu]\n",
		apm_query_data->ts_query_sent,
		apm_query_data->query_bss->APMetric.ts_recvd);
	/* Compare Query BSS's AP Metrics Response's TS with AP Metrics Query TS */
	return (apm_query_data->ts_query_sent >
		apm_query_data->query_bss->APMetric.ts_recvd);
}

/* API to Send AP Metrics Query to Agent */
static void
wbd_master_send_ap_metrics_query_to_agent(i5_dm_device_type *p_device,
	i5_dm_bss_type *p_bss, wbd_ap_metrics_query_args_t *apm_query_data)
{
	/* Initialize AP Metrics Query TS & Query BSS */
	if (apm_query_data) {
		apm_query_data->ts_query_sent = 0;
		apm_query_data->query_bss = NULL;
	}

	/* IF PARENT BSS is found, Send AP Metrics Query to only this BSS */
	if (p_bss && p_device) {
		/* Send consolidated AP Metrics Query */
		ieee1905_send_ap_metrics_query(p_device->DeviceId, p_bss->BSSID, 1);
		/* Assign AP Metrics Query TS & Query BSS */
		if (apm_query_data) {
			apm_query_data->ts_query_sent = time(NULL);
			apm_query_data->query_bss = p_bss;
		}

	/* IF PARENT BSS not found, Send AP Metrics Query to all BSS in this Device */
	} else if (!p_bss && p_device) {
		int ret = WBDE_OK;
		i5_dm_interface_type *iter_ifr = NULL;
		i5_dm_bss_type *iter_bss = NULL;
		int max_bss_cnt = (WL_MAXBSSCFG * WL_MAXBSSCFG), bss_cnt = 0;
		unsigned char *bssids = NULL;

		bssids = (unsigned char*)wbd_malloc((max_bss_cnt * MAC_ADDR_LEN), &ret);
		if (!bssids) {
			goto end;
		}

		foreach_i5glist_item(iter_ifr, i5_dm_interface_type, p_device->interface_list) {
			/* Skip non-wireless or interfaces with zero bss count */
			if (!i5DmIsInterfaceWireless(iter_ifr->MediaType) ||
				!iter_ifr->BSSNumberOfEntries) {
				continue;
			}
			foreach_i5glist_item(iter_bss, i5_dm_bss_type, iter_ifr->bss_list) {
				/* Skip bss with zero sta count */
				if (!iter_bss->ClientsNumberOfEntries) {
					continue;
				}
				memcpy(&bssids[bss_cnt * MAC_ADDR_LEN], iter_bss->BSSID,
					MAC_ADDR_LEN);
				bss_cnt++;
				/* Assign AP Metrics Query TS & Query BSS */
				if (apm_query_data) {
					apm_query_data->ts_query_sent = time(NULL);
					apm_query_data->query_bss = iter_bss;
				}
			}
		}

		/* Send consolidated AP Metrics Query */
		ieee1905_send_ap_metrics_query(p_device->DeviceId, bssids, bss_cnt);

		if (bssids) {
			free(bssids);
		}
	}
end:
	return;
}

/* Get the CLIENTLIST CLI command data */
static int
wbd_master_process_clientlist_cli_cmd_data(wbd_info_t *info, wbd_cli_send_data_t *clidata,
	char **outdataptr)
{
	int ret = WBDE_OK, len = 0, outlen = WBD_MAX_BUF_8192, ret_cmp = 0;
	char *outdata = NULL;
	int count = 0;
	struct ether_addr cli_almac;
	struct ether_addr cli_bssid;
	struct ether_addr cli_mac;
	i5_dm_device_type *p_device = NULL;
	i5_dm_interface_type *p_ifr = NULL, *iter_ifr = NULL;
	i5_dm_bss_type *p_bss = NULL, *iter_bss = NULL;
	i5_dm_clients_type *p_sta = NULL, *iter_sta = NULL;
	wbd_ap_metrics_query_args_t *apm_query_data = &(info->wbd_master->apm_query_data);

	/* Validate fn args */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);
	WBD_ASSERT_ARG(outdataptr, WBDE_INV_ARG);
	WBD_ASSERT_MSGDATA(clidata, "CLIENTLIST_CLI");

	outdata = (char*)wbd_malloc(outlen, &ret);
	WBD_ASSERT();

	if (strlen(clidata->almac) > 0) {
		/* Get Validated Non-NULL ALMAC */
		WBD_GET_VALID_MAC(clidata->almac, &cli_almac, "CLIENTLIST_CLI", WBDE_DS_UN_DEV);

		/* Check if the ALMAC exists or not */
		p_device = wbd_ds_get_i5_device((unsigned char*)&cli_almac, &ret);
		WBD_ASSERT_MSG("ALMAC["MACF"]. %s\n", ETHER_TO_MACF(cli_almac), wbderrorstr(ret));

	} else if (strlen(clidata->bssid) > 0) {
		/* Get Validated Non-NULL BSSID */
		WBD_GET_VALID_MAC(clidata->bssid, &cli_bssid, "CLIENTLIST_CLI", WBDE_INV_BSSID);

		/* Check if the BSSID exists or not */
		p_bss = wbd_ds_get_i5_bss_in_topology((unsigned char*)&cli_bssid, &ret);
		WBD_ASSERT_MSG("BSSID["MACF"]. %s\n", ETHER_TO_MACF(cli_bssid), wbderrorstr(ret));
		p_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(p_bss);
		p_device = (i5_dm_device_type*)WBD_I5LL_PARENT(p_ifr);

	} else if (strlen(clidata->mac) > 0) {
		/* Get Validated Non-NULL STA MAC */
		WBD_GET_VALID_MAC(clidata->mac, &cli_mac, "CLIENTLIST_CLI", WBDE_INV_MAC);

		/* Check if the STA exists or not */
		p_sta = wbd_ds_find_sta_in_topology((unsigned char*)&cli_mac, &ret);
		WBD_ASSERT_MSG("STA["MACF"]. %s\n", ETHER_TO_MACF(cli_mac), wbderrorstr(ret));
		p_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(p_sta);
		p_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(p_bss);
		p_device = (i5_dm_device_type*)WBD_I5LL_PARENT(p_ifr);
	}

	/* IF called with option : FORCE
	 * Send AP Metrics Query to Agent, Send Dummy Response to CLI
	 */
	if (clidata->flags & WBD_CLI_FLAGS_FORCE) {

		/* API to Send AP Metrics Query to Agent */
		wbd_master_send_ap_metrics_query_to_agent(p_device, p_bss, apm_query_data);

		/* Prepare Dummy Response */
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"Fetching Latest STA stats from Device["MACDBG"] ...\n",
			MAC2STRDBG(p_device->DeviceId));
		ret = WBDE_OK;
		goto end;

	/* IF called with option : QUERY
	 * Compare AP Metrics Query & stats Timestamps, Send Comparison Result to CLI
	 */
	} else if (clidata->flags & WBD_CLI_FLAGS_QUERY) {

		/* API to Compare AP Metrics Query & stats Timestamps */
		ret_cmp = wbd_master_ap_metrics_ts_compare(apm_query_data);

		/* Prepare Response with Comparison Return Value */
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"%d", ret_cmp);
		ret = WBDE_OK;
		goto end;
	}

	/* IF called with NO option : !FORCE  AND  !QUERY
	 * Prepare PRINT buff of Updated/Last STA stats using CLI Args, Send PRINT buff to CLI
	 */

	/* Print PARENT DEVICE Item */
	ret = wbd_snprintf_i5_device(p_device, CLI_CMD_I5_DEV_FMT,
		&outdata, &outlen, &len, WBD_MAX_BUF_2048);

	foreach_i5glist_item(iter_ifr, i5_dm_interface_type, p_device->interface_list) {

		/* Skip non-wireless or interfaces with zero bss count */
		if (!i5DmIsInterfaceWireless(iter_ifr->MediaType) ||
			!iter_ifr->BSSNumberOfEntries) {
			continue;
		}
		/* Look for PARENT IFR */
		else if (p_ifr &&
			memcmp(p_ifr->InterfaceId, iter_ifr->InterfaceId, MAC_ADDR_LEN)) {
			continue;
		}

		foreach_i5glist_item(iter_bss, i5_dm_bss_type, iter_ifr->bss_list) {

			/* Skip bss with zero sta count */
			if (!p_bss && !iter_bss->ClientsNumberOfEntries) {
				continue;
			}
			/* Look for PARENT BSS */
			else if (p_bss &&
				memcmp(p_bss->BSSID, iter_bss->BSSID, MAC_ADDR_LEN)) {
				continue;
			}

			/* Print BSS Item */
			ret = wbd_snprintf_i5_bss(iter_bss, CLI_CMD_I5_BSS_FMT,
				&outdata, &outlen, &len, WBD_MAX_BUF_2048, FALSE, FALSE);

			foreach_i5glist_item(iter_sta, i5_dm_clients_type,
				iter_bss->client_list) {

				/* Look for SELF STA */
				if (p_sta &&
					memcmp(p_sta->mac, iter_sta->mac, MAC_ADDR_LEN)) {
					continue;
				}

				/* Print STA Item */
				count++;
				ret = wbd_snprintf_i5_client(iter_sta, CLI_CMD_I5_STA_FMT,
					&outdata, &outlen, &len, WBD_MAX_BUF_2048, count);
			}
		}
	}

	if (!count) {
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"No entry found \n");
	}

end: /* Check Master Pointer before using it below */

	if (ret != WBDE_OK) {
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"%s\n", wbderrorstr(ret));
	}
	if (outdataptr) {
		*outdataptr = outdata;
	}
	return ret;
}

/* Processes the CLIENTLIST CLI command */
static void
wbd_master_process_clientlist_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;
	wbd_info_t *info = (wbd_info_t*)arg;
	char *outdata = NULL;
	int ret;

	BCM_REFERENCE(ret);
	ret = wbd_master_process_clientlist_cli_cmd_data(info, clidata, &outdata);

	if (outdata) {
		if (wbd_com_send_cmd(hndl, childfd, outdata, NULL) != WBDE_OK) {
			WBD_WARNING("CLIENTLIST CLI : %s\n", wbderrorstr(WBDE_SEND_RESP_FL));
		}
		free(outdata);
	}

	if (clidata)
		free(clidata);

	return;
}

/* Get the LOGS CLI command data */
static int
wbd_master_process_logs_cli_cmd_data(wbd_info_t *info,
	wbd_cli_send_data_t *clidata, char **outdataptr)
{
	int ret = WBDE_OK;
	char *outdata = NULL;
	int outlen = WBD_MAX_BUF_8192 * 4;

	outdata = (char*)wbd_malloc(outlen, &ret);
	WBD_ASSERT();

	/* Validate arg */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);

	/* Validate Message data */
	WBD_ASSERT_MSGDATA(clidata, "LOGS_CLI");

	/* Fetch logs from master. */
	(void)wbd_ds_get_logs_from_master(info->wbd_master, outdata, outlen);

end:
	if (ret != WBDE_OK && outdata) {
		snprintf(outdata + strlen(outdata), outlen - strlen(outdata),
			"%s\n", wbderrorstr(ret));
	}
	if (outdataptr) {
		*outdataptr = outdata;
	}
	return ret;
}

static char*
wbd_master_clear_cli_logs(wbd_info_t *info, wbd_cli_send_data_t *clidata)
{
	int ret = WBDE_OK;
	char *outdata = NULL;
	int outlen = WBD_MAX_BUF_64;

	outdata = (char*)wbd_malloc(outlen, &ret);
	WBD_ASSERT();

	/* Validate arg */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);

	/* Validate Message data */
	WBD_ASSERT_MSGDATA(clidata, "LOGS_CLI");

	/* clear log messages. */
	memset(&(info->wbd_master->master_logs), 0,
		sizeof(info->wbd_master->master_logs));
	info->wbd_master->master_logs.buflen = ARRAYSIZE(info->wbd_master->master_logs.logmsgs);

end:
	if (outdata) {
		snprintf(outdata, outlen, "%s\n", wbderrorstr(ret));
	}

	return outdata;
}

/* Processes the LOGS CLI command */
static void
wbd_master_process_logs_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;
	wbd_info_t *info = (wbd_info_t*)arg;
	char *outdata = NULL;
	int ret;

	BCM_REFERENCE(ret);

	if (clidata->flags & WBD_CLI_CLEAR_LOGS) {
		outdata = wbd_master_clear_cli_logs(info, clidata);
	} else if (clidata->flags & WBD_CLI_FLAGS_JSON) {
		outdata = wbd_json_create_cli_logs(info, clidata);
	} else {
		ret = wbd_master_process_logs_cli_cmd_data(info, clidata, &outdata);
	}

	if (outdata) {
		if (wbd_com_send_cmd(hndl, childfd, outdata, NULL) != WBDE_OK) {
			WBD_WARNING("LOGS CLI : %s\n", wbderrorstr(WBDE_SEND_RESP_FL));
		}
		free(outdata);
	}

	if (clidata)
		free(clidata);

	return;
}

/* Processes the BH OPT command */
static void
wbd_master_process_bh_opt_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;
	wbd_info_t *info = (wbd_info_t*)arg;
	int ret = WBDE_OK;
	char *outdata = NULL;
	int outlen = WBD_MAX_BUF_64;

	outdata = (char*)wbd_malloc(outlen, &ret);
	WBD_ASSERT();

	/* Validate arg */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);

	/* Validate Message data */
	WBD_ASSERT_MSGDATA(clidata, "BH_OPT_CLI");

	if (clidata->disable == 1) {
		info->wbd_master->flags &= ~WBD_FLAGS_BKT_BH_OPT_ENABLED;
		WBD_INFO("Disabling backhaul optimizaion\n");
	} else {
		info->wbd_master->flags |= WBD_FLAGS_BKT_BH_OPT_ENABLED;
		WBD_INFO("Enabling backhaul optimizaion\n");
		wbd_master_create_bh_opt_timer(NULL);
	}

end:
	if (outdata) {
		snprintf(outdata, outlen, "%s. %s\n", wbderrorstr(ret),
			(WBD_BKT_BH_OPT_ENAB(info->wbd_master->flags) ? "Enabled" : "Disabled"));
		if (wbd_com_send_cmd(hndl, childfd, outdata, NULL) != WBDE_OK) {
			WBD_WARNING("BH_OPT CLI : %s\n", wbderrorstr(WBDE_SEND_RESP_FL));
		}
		free(outdata);
	}
	if (clidata) {
		free(clidata);
	}

	return;
}

/* Changes the multiap mode from controller  to agent */
void
wbd_master_update_multiap_mode_to_agent()
{
	int multiap_mode;
	char nvval[WBD_MAX_BUF_16] = {0};

	multiap_mode = blanket_get_config_val_int(NULL, WBD_NVRAM_MULTIAP_MODE,
		MAP_MODE_FLAG_DISABLED);
	multiap_mode &= ~MAP_MODE_FLAG_CONTROLLER;
	snprintf(nvval, sizeof(nvval), "%d", multiap_mode);
	blanket_nvram_prefix_set(NULL, WBD_NVRAM_MULTIAP_MODE, nvval);
}

/* Process the exit cli cmd */
static void
wbd_master_process_exit_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;

	wbd_master_update_multiap_mode_to_agent();

	if (wbd_com_send_cmd(hndl, childfd, "Wbd mater is shutting down\n", NULL) != WBDE_OK) {
		WBD_WARNING("Failed to send exit CLI response\n");
	}

	if (clidata)
		free(clidata);

	wbd_do_rc_restart_reboot(WBD_FLG_NV_COMMIT | WBD_FLG_RC_RESTART);
}

/* Process the reload cli cmd */
static void
wbd_master_process_reload_cli_cmd(wbd_com_handle *hndl, int childfd, void *cmddata, void *arg)
{
	wbd_cli_send_data_t *clidata = (wbd_cli_send_data_t*)cmddata;

	if (wbd_com_send_cmd(hndl, childfd, "Reloading WBD Master\n", NULL) != WBDE_OK) {
		WBD_WARNING("Failed to send reload CLI response\n");
	}

	if (clidata)
		free(clidata);

	wbd_master_reload();
}

/* Gets Master NVRAM settings based on primary prefix */
static int
wbd_master_retrieve_prefix_nvram_config(wbd_master_info_t *master, uint8 band)
{
	int ret = WBDE_OK, num = 0;
	wbd_weak_sta_policy_t wbd_metric;
	wbd_weak_sta_policy_t *metric_policy;
	wbd_strong_sta_policy_t wbd_strong_sta_metric, *strong_sta_metric_policy;
	char *nvval, nvname[WBD_MAX_NVRAM_NAME];
	char *strong_nvval, strong_nvname[WBD_MAX_NVRAM_NAME];
	char logmsg[WBD_MAX_BUF_128] = {0}, timestamp[WBD_MAX_BUF_32] = {0};
	static int map_init_start = 0;
	WBD_ENTER();

	if (band == WBD_BAND_LAN_2G) {
		metric_policy = &master->metric_policy_2g;
		WBDSTRNCPY(nvname, WBD_NVRAM_BKT_WEAK_STA_CFG_2G, sizeof(nvname));
	} else if (band == WBD_BAND_LAN_6G) {
		metric_policy = &master->metric_policy_6g;
		WBDSTRNCPY(nvname, WBD_NVRAM_BKT_WEAK_STA_CFG_6G, sizeof(nvname));
	} else {
		metric_policy = &master->metric_policy_5g;
		WBDSTRNCPY(nvname, WBD_NVRAM_BKT_WEAK_STA_CFG_5G, sizeof(nvname));
	}
	WBD_DEBUG("Retrieve Config for BKTID[%d] nvname[%s]\n",
		master->bkt_id, nvname);

	metric_policy->t_idle_rate = WBD_STA_METRICS_REPORTING_IDLE_RATE_THLD;
	metric_policy->t_rssi = WBD_STA_METRICS_REPORTING_RSSI_THLD;
	metric_policy->t_hysterisis = WBD_STA_METRICS_REPORTING_RSSI_HYSTERISIS_MARGIN;
	metric_policy->t_tx_rate = WBD_STA_METRICS_REPORTING_TX_RATE_THLD;
	metric_policy->t_tx_failures = WBD_STA_METRICS_REPORTING_TX_FAIL_THLD;
	metric_policy->flags = WBD_WEAK_STA_POLICY_FLAG_RSSI;

	/* Get NVRAM : Metric Reporting Policy flags */
	master->metric_policy_flags = (uint8)blanket_get_config_val_uint(NULL,
		NVRAM_MAP_MTRC_POL_FLAGS, WBD_STA_METRICS_REPORTING_FLAGS);
	/* Get NVRAM : Interval in Metric Reporting Policy to Periodically Report AP Metrics */
	master->metric_policy_ap_rpt_intvl = blanket_get_config_val_int(NULL,
		NVRAM_MAP_MTRC_POL_APMETRIC_INT, WBD_STA_METRICS_REPORTING_APMETRIC_INT);

	memset(&wbd_metric, 0, sizeof(wbd_metric));
	nvval = blanket_nvram_safe_get(nvname);
	num = sscanf(nvval, "%d %d %d %d %d %x", &wbd_metric.t_idle_rate, &wbd_metric.t_rssi,
		&wbd_metric.t_hysterisis, &wbd_metric.t_tx_rate,
		&wbd_metric.t_tx_failures, &wbd_metric.flags);
	if (num == 6) {
		memcpy(metric_policy, &wbd_metric, sizeof(*metric_policy));
	}

	if ((num != 6) && (map_init_start == 0)) {

		/* Create and store MAP Init Start log */
		snprintf(logmsg, sizeof(logmsg), CLI_CMD_LOGS_MAP_INIT_START,
			wbd_get_formated_local_time(timestamp, sizeof(timestamp)),
			MAC2STRDBG(ieee1905_get_al_mac()));
		wbd_ds_add_logs_in_master(wbd_get_ginfo()->wbd_master, logmsg);

#ifdef BCM_APPEVENTD
		/* Send MAP Init Start event to appeventd. */
		wbd_appeventd_map_init(APP_E_WBD_MASTER_MAP_INIT_START,
			(struct ether_addr*)ieee1905_get_al_mac(), MAP_INIT_START,
			MAP_APPTYPE_MASTER);
#endif /* BCM_APPEVENTD */
		map_init_start++;
	}

	WBD_INFO("BKTID[%d] NVRAM[%s=%s] IDLE_RATE[%d] RSSI[%d] HYSTERISIS[%d] "
		"PHY_RATE[%d] TX_FAILURES[%d] FLAGS[0x%X] MTRC_FLAGS[0x%X] APMETRIC_INT[%d]\n",
		master->bkt_id, nvname, nvval,
		metric_policy->t_idle_rate, metric_policy->t_rssi,
		metric_policy->t_hysterisis, metric_policy->t_tx_rate,
		metric_policy->t_tx_failures, metric_policy->flags,
		master->metric_policy_flags, master->metric_policy_ap_rpt_intvl);

	/* Get strong sta metric reporting policy */
	if (band == WBD_BAND_LAN_2G) {
		strong_sta_metric_policy = &master->strong_metric_policy_2g;
		WBDSTRNCPY(strong_nvname, WBD_NVRAM_BKT_STRONG_STA_CFG_2G, sizeof(strong_nvname));
	} else {
		strong_sta_metric_policy = &master->strong_metric_policy_5g;
		WBDSTRNCPY(strong_nvname, WBD_NVRAM_BKT_STRONG_STA_CFG_5G, sizeof(strong_nvname));
	}

	if (band == WBD_BAND_LAN_6G) {
		goto end;
	}

	WBD_DEBUG("Retrieve Config for BKTID[%d] strong_nvname[%s]\n",
		master->bkt_id, strong_nvname);

	strong_sta_metric_policy->t_idle_rate = WBD_STRONG_STA_METRICS_REPORTING_IDLE_RATE_THLD;
	strong_sta_metric_policy->t_rssi = WBD_STRONG_STA_METRICS_REPORTING_RSSI_THLD;
	strong_sta_metric_policy->t_hysterisis =
		WBD_STRONG_STA_METRICS_REPORTING_RSSI_HYSTERISIS_MARGIN;
	strong_sta_metric_policy->flags = WBD_STRONG_STA_POLICY_FLAG_RSSI;

	memset(&wbd_strong_sta_metric, 0, sizeof(wbd_strong_sta_metric));
	strong_nvval = blanket_nvram_safe_get(strong_nvname);
	num = sscanf(strong_nvval, "%d %d %d %x", &wbd_strong_sta_metric.t_idle_rate,
		&wbd_strong_sta_metric.t_rssi, &wbd_strong_sta_metric.t_hysterisis,
		&wbd_strong_sta_metric.flags);
	if (num == 4) {
		memcpy(strong_sta_metric_policy, &wbd_strong_sta_metric,
			sizeof(*strong_sta_metric_policy));
	}

	WBD_INFO("BKTID[%d] NVRAM[%s=%s] RSSI[%d] HYSTERISIS[%d]\n",
		master->bkt_id, strong_nvname, strong_nvval,
		strong_sta_metric_policy->t_rssi,
		strong_sta_metric_policy->t_hysterisis);

end:
	WBD_EXIT();
	return ret;
}

/* Creates the blanket master for the blanket ID */
void
wbd_master_create_master_info(wbd_info_t *info, uint8 bkt_id, char *bkt_name)
{
	int ret = WBDE_OK;
	wbd_master_info_t *master_info = NULL;
	WBD_ENTER();

	master_info = wbd_ds_find_master_in_blanket_master(info->wbd_master, bkt_id, (&ret));

	/* If Master Info NOT FOUND for Band */
	if (!master_info) {

		/* Allocate & Initialize Master for this blanket ID, add to blanket List */
		ret = wbd_ds_create_master_for_blanket_id(info->wbd_master,
			bkt_id, bkt_name, &master_info);
		WBD_ASSERT();
	}

end:
	WBD_EXIT();
}

/* Load configurations for each master info */
void
wbd_master_load_all_master_info_configurations(wbd_info_t *info)
{
	int ret = WBDE_OK;
	wbd_master_info_t *master_info = NULL;
	dll_t *master_item_p;
	WBD_ENTER();

	/* Traverse br0 Master List for this Blanket Master for each band */
	foreach_glist_item(master_item_p, info->wbd_master->blanket_list) {

		master_info = (wbd_master_info_t*)master_item_p;

		/* Do Initialize sequences for newly created Master for specific band */
		ret = wbd_master_init_actions(master_info->bkt_id, master_info);
		WBD_ASSERT();

		/* Read Master Band specific NVRAMs */
		wbd_master_retrieve_prefix_nvram_config(master_info, WBD_BAND_LAN_2G);
		wbd_master_retrieve_prefix_nvram_config(master_info, WBD_BAND_LAN_5GL);
		wbd_master_retrieve_prefix_nvram_config(master_info, WBD_BAND_LAN_6G);

		/* Read backhual NVRAM config */
		wbd_retrieve_backhaul_nvram_config(&master_info->metric_policy_bh);
	}

end:
	WBD_EXIT();
}

/* Got Associated STA link metric response */
void
wbd_master_process_assoc_sta_metric_resp(wbd_info_t *info, unsigned char *al_mac,
	unsigned char *bssid, unsigned char *sta_mac, ieee1905_sta_link_metric *metric)
{
	int ret = WBDE_OK, band, device_bands, sta_is_weak = 0, sta_is_strong = 0;
	i5_dm_device_type *i5_dev;
	i5_dm_interface_type *i5_ifr;
	i5_dm_bss_type *i5_bss;
	i5_dm_clients_type *i5_assoc_sta;
	wbd_master_info_t *master;
	wbd_assoc_sta_item_t *assoc_sta;
	wbd_weak_sta_metrics_t sta_stats;
	wbd_weak_sta_policy_t *metric_policy;
	time_t gap, now = time(NULL);
	wbd_strong_sta_metrics_t strong_sta_stats;
	wbd_strong_sta_policy_t *strong_metric_policy;
	wbd_prb_sta_t *prbsta;
	bool strong_sta_check = TRUE;
	WBD_ENTER();

	WBD_SAFE_FIND_I5_BSS_IN_DEVICE(al_mac, bssid, i5_bss, &ret);

	/* Check if the STA exists or not */
	i5_assoc_sta = wbd_ds_find_sta_in_bss_assoclist(i5_bss, (struct ether_addr*)sta_mac, &ret,
		&assoc_sta);
	WBD_ASSERT_MSG("Device["MACDBG"] BSSID["MACDBG"] STA["MACF"]. %s\n", MAC2STRDBG(al_mac),
		MAC2STRDBG(i5_bss->BSSID), ETHERP_TO_MACF(sta_mac), wbderrorstr(ret));

	i5_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_bss);
	i5_dev = (i5_dm_device_type*)WBD_I5LL_PARENT(i5_ifr);
	WBD_SAFE_GET_MASTER_INFO(info, WBD_BKT_ID_BR0, master, (&ret));

	band = WBD_BAND_FROM_1905_BAND(i5_ifr->band);
	device_bands = WBD_BAND_FROM_1905_BAND(i5_dev->bands);

	prbsta = wbd_ds_find_sta_in_probe_sta_table(info, (struct ether_addr*)sta_mac, FALSE);

	/* If backhaul STA, use backhaul metric policy */
	if (I5_CLIENT_IS_BSTA(i5_assoc_sta)) {
		metric_policy = &master->metric_policy_bh;
	} else {
		if (band & WBD_BAND_LAN_2G) {
			metric_policy = &master->metric_policy_2g;
		} else if (band & WBD_BAND_LAN_6G) {
			metric_policy = &master->metric_policy_6g;
		} else {
			metric_policy = &master->metric_policy_5g;
		}
	}

	if (band & WBD_BAND_LAN_2G) {
		strong_metric_policy = &master->strong_metric_policy_2g;
	} else {
		strong_metric_policy = &master->strong_metric_policy_5g;
	}

	/* Fill STA Stats, which needs to be compared for Weak Client Indentification */
	memset(&sta_stats, 0, sizeof(sta_stats));
	memset(&strong_sta_stats, 0, sizeof(strong_sta_stats));
	if (metric) {
		sta_stats.rssi = WBD_RCPI_TO_RSSI(metric->rcpi);
		sta_stats.tx_rate = metric->downlink_rate;
		assoc_sta->stats.sta_tm = time(NULL);
	}
	sta_stats.idle_rate = assoc_sta->stats.idle_rate;
	/* Ignore tx failure readings older than 10 seconds */
	gap = now - assoc_sta->stats.active;
	if (assoc_sta->stats.active > 0 && gap < WBD_METRIC_EXPIRY_TIME) {
		if (assoc_sta->stats.tx_tot_failures >= assoc_sta->stats.old_tx_tot_failures) {
			sta_stats.tx_failures = assoc_sta->stats.tx_tot_failures -
				assoc_sta->stats.old_tx_tot_failures;
		} else {
			/* Handle counter roll over */
			sta_stats.tx_failures = (-1) - assoc_sta->stats.old_tx_tot_failures +
				assoc_sta->stats.tx_tot_failures + 1;
		}
	} else {
		sta_stats.tx_failures = 0;
	}
	assoc_sta->stats.active = now;

	sta_stats.last_weak_rssi = assoc_sta->last_weak_rssi ?
		assoc_sta->last_weak_rssi : sta_stats.rssi;

	/* Common algo to compare STA Stats and Thresholds, & identify if STA is Weak or not */
	sta_is_weak = wbd_weak_sta_identification((struct ether_addr*)sta_mac, &sta_stats,
		metric_policy, NULL, NULL);

	/* If STA is not weak and not a backhaul STA check if STA is strong
	 * Check strong sta only for 2.4G or 5G band, higher bands may have better bss,
	 * Skip strong sta check on 6G band since it is highest band.
	 * Skip strong sta check if device doesnt have 6G radio.
	 * Skip strong sta check if STA is not 6G capable.
	*/
	if (sta_is_weak || I5_CLIENT_IS_BSTA(i5_assoc_sta) || (band & WBD_BAND_LAN_6G) ||
		(WBD_BAND_TYPE_LAN_5G(band) &&
		(!(device_bands & WBD_BAND_LAN_6G) ||
		!(prbsta && (prbsta->band & WBD_BAND_LAN_6G))))) {
		strong_sta_check = FALSE;
	}

	if (strong_sta_check) {
		strong_sta_stats.rssi = WBD_RCPI_TO_RSSI(metric->rcpi);
		strong_sta_stats.last_strong_rssi = assoc_sta->last_strong_rssi ?
			assoc_sta->last_strong_rssi : strong_sta_stats.rssi;
		sta_is_strong = wbd_strong_sta_identification((struct ether_addr*)sta_mac,
				&strong_sta_stats, strong_metric_policy);
	}

	WBD_INFO("For %sSTA["MACDBG"] idle_rate[%d] t_idle_rate[%d] rssi[%d] t_rss[%d] "
		"tx_rate[%d] t_tx_rate[%d] tx_failures[%d] t_tx_failures[%d] "
		"sta_status[%d] is_sta_weak[%s] is_sta_strong[%s]\n",
		I5_CLIENT_IS_BSTA(i5_assoc_sta) ? "BH" : "", MAC2STRDBG(sta_mac),
		sta_stats.idle_rate, metric_policy->t_idle_rate,
		sta_stats.rssi, metric_policy->t_rssi,
		sta_stats.tx_rate, metric_policy->t_tx_rate,
		sta_stats.tx_failures, metric_policy->t_tx_failures,
		assoc_sta->status, sta_is_weak ? "YES" : "NO",
		sta_is_strong ? "YES" : "NO");

	/* If the associated link metrics is for backhaul STA, and the backhaul optimization is
	 * running, no need to do anything
	 */
	if (I5_CLIENT_IS_BSTA(i5_assoc_sta) && WBD_MINFO_IS_BH_OPT_RUNNING(master->flags)) {
		WBD_DEBUG("Backhaul optimization is running. So, do not process assoc STA metrics "
			"response\n");
		goto end;
	}

	/* If it is weak STA or the backhaul optimization is running */
	if (sta_is_weak) {
		/* If not already weak process the weak client data */
		if (assoc_sta->status != WBD_STA_STATUS_WEAK) {
			ret = wbd_master_process_weak_client_cmd(master, i5_assoc_sta, &sta_stats,
				metric_policy);
			/* If steering a backhaul STA forms a loop, try to do backhaul
			 * optimization before handling weak backhaul STA. This can be possible
			 * only for max_bh_opt_try_on_weak times
			 */
			if (ret == WBDE_BH_STEER_LOOP &&
				(info->wbd_master->max_bh_opt_try_on_weak >
				assoc_sta->bh_opt_count_on_weak)) {
				/* Unset the done flag so that, backhaul optimization
				 * will start for all the backhaul STAs
				 */
				wbd_ds_unset_bh_opt_flags(WBD_FLAGS_ASSOC_ITEM_BH_OPT_DONE);
				wbd_master_start_bh_opt(NULL);
				assoc_sta->bh_opt_count_on_weak++;
			}
		}

		/* If the TBSS timer is not created for this STA. create it */
		if (ret == WBDE_OK && !WBD_ASSOC_ITEM_IS_TBSS_TIMER(assoc_sta->flags)) {
			wbd_master_create_identify_target_bss_timer(master, i5_assoc_sta);
		}
	} else if (sta_is_strong) {
		/* If not already strong process the strong client data */
		if ((assoc_sta->status != WBD_STA_STATUS_STRONG)) {
			ret = wbd_master_process_strong_client_cmd(master, i5_assoc_sta,
				&strong_sta_stats, strong_metric_policy);
		}

		/* If the TBSS timer is not created for this STA. create it */
		if (ret == WBDE_OK && !WBD_ASSOC_ITEM_IS_TBSS_TIMER(assoc_sta->flags)) {
			wbd_master_create_identify_target_bss_timer(master, i5_assoc_sta);
		}
	} else {
		/* If the status is not ignore */
		if (assoc_sta->status != WBD_STA_STATUS_IGNORE) {
			wbd_master_process_weak_cancel_cmd(master, i5_assoc_sta);
		}

		/* For 2G backhaul STA, check if the 2G RSSI is good enough to steer the STA
		 * to 5G backhaul
		 */
		if (I5_CLIENT_IS_BSTA(i5_assoc_sta) && WBD_BAND_TYPE_LAN_2G(i5_ifr->band)) {
			if (sta_stats.rssi > info->t_2g_bs_bsta_rssi) {
				WBD_INFO("For BHSTA["MACDBG"] Start backhaul optimization "
					"[%d > %d]\n", MAC2STRDBG(sta_mac),
					sta_stats.rssi, info->t_2g_bs_bsta_rssi);
				/* Unset the done flag so that, backhaul optimization
				 * will start for this backhaul STA
				 */
				assoc_sta->flags &= ~WBD_FLAGS_ASSOC_ITEM_BH_OPT_DONE;
				wbd_master_start_bh_opt(sta_mac);
			}
		}
	}

end:
	WBD_EXIT();
}

/* Add to monitor list of a BSS if the STA is weak */
static wbd_monitor_sta_item_t*
wbd_master_add_to_monitor_list(i5_dm_bss_type *i5_bss, unsigned char *sta_mac)
{
	int ret = WBDE_OK;
	i5_dm_bss_type *i5_assoc_bss;
	i5_dm_clients_type *i5_sta;
	wbd_assoc_sta_item_t *assoc_sta;
	wbd_monitor_sta_item_t *monitor_sta = NULL;

	/* Find the STA in the topology and check is its weak or not */
	i5_sta = wbd_ds_find_sta_in_topology(sta_mac, &ret);
	if (!i5_sta) {
		goto end;
	}

	assoc_sta = (wbd_assoc_sta_item_t*)i5_sta->vndr_data;
	/* Add to monitor list only if its weak or backhaul managed */
	if (assoc_sta && (assoc_sta->status == WBD_STA_STATUS_WEAK ||
		assoc_sta->status == WBD_STA_STATUS_STRONG ||
		WBD_ASSOC_ITEM_IS_BH_OPT(assoc_sta->flags))) {

		i5_assoc_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_sta);

		/* Don't add if the SSIDs are not matching */
		if (!WBD_SSIDS_MATCH(i5_bss->ssid, i5_assoc_bss->ssid)) {
			goto end;
		}

		ret = wbd_ds_add_sta_in_bss_monitorlist(i5_bss,
			(struct ether_addr*)i5_assoc_bss->BSSID, (struct ether_addr*)i5_sta->mac,
			&monitor_sta);
		if (ret != WBDE_OK) {
			WBD_WARNING("BSS["MACDBG"] STA["MACDBG"] Failed to Add STA to "
				"monitorlist Error : %s\n",
				MAC2STRDBG(i5_bss->BSSID), MAC2STRDBG(i5_sta->mac),
				wbderrorstr(ret));
			goto end;
		}
	} else {
		WBD_WARNING("BSS["MACDBG"] STA["MACDBG"] Is not weak/strong/not backhaul managed. "
			"So Don't add\n",
			MAC2STRDBG(i5_bss->BSSID), MAC2STRDBG(i5_sta->mac));
	}

end:
	return monitor_sta;
}

/* Got UnAssociated STA link metric response */
void
wbd_master_unassoc_sta_metric_resp(unsigned char *al_mac, ieee1905_unassoc_sta_link_metric *metric)
{
	int ret = WBDE_OK, rssi, band;
	i5_dm_device_type *i5_device;
	i5_dm_bss_type *i5_bss;
	ieee1905_unassoc_sta_link_metric_list *sta_item;
	dll_t *unassoc_sta_p;
	wbd_monitor_sta_item_t *monitor_sta = NULL;
	WBD_ENTER();

	WBD_SAFE_FIND_I5_DEVICE(i5_device, al_mac, &ret);

	/* Traverse Unassociated STA link metrics list */
	foreach_glist_item(unassoc_sta_p, metric->sta_list) {

		i5_dm_clients_type *i5_assoc_sta;
		i5_dm_bss_type *i5_assoc_bss;
		i5_dm_interface_type *i5_assoc_ifr;
		sta_item = (ieee1905_unassoc_sta_link_metric_list*)unassoc_sta_p;

		/* Find the STA in the topology */
		i5_assoc_sta = wbd_ds_find_sta_in_topology(sta_item->mac, &ret);
		if (!i5_assoc_sta) {
			WBD_INFO("Device["MACDBG"] STA["MACDBG"] : %s\n",
				MAC2STRDBG(al_mac), MAC2STRDBG(sta_item->mac), wbderrorstr(ret));
			continue;
		}

		i5_assoc_bss = (i5_dm_bss_type*)WBD_I5LL_PARENT(i5_assoc_sta);
		i5_assoc_ifr = (i5_dm_interface_type*)WBD_I5LL_PARENT(i5_assoc_bss);

		/* Get the BSS based on band to get the monitor list */
		band = ieee1905_get_band_from_channel(i5_assoc_ifr->opClass, sta_item->channel);
		WBD_DS_FIND_I5_BSS_IN_DEVICE_FOR_BAND_AND_SSID(i5_device, band, i5_bss,
			&i5_assoc_bss->ssid, &ret);
		if ((ret != WBDE_OK) || !i5_bss) {
			WBD_INFO("Device["MACDBG"] Band[%d]  STA["MACDBG"] : %s\n",
				MAC2STRDBG(al_mac), band, MAC2STRDBG(sta_item->mac),
				wbderrorstr(ret));
			continue;
		}

		/* Check in Monitor list */
		monitor_sta = wbd_ds_find_sta_in_bss_monitorlist(i5_bss,
			(struct ether_addr*)&sta_item->mac, &ret);
		/* If monitor STA item not exists add it */
		if (!monitor_sta) {
			WBD_WARNING("STA["MACDBG"] not found in BSS["MACDBG"]'s Monitor list. "
				"So Add it\n",
				MAC2STRDBG(sta_item->mac),
				MAC2STRDBG(i5_bss->BSSID));
			monitor_sta = wbd_master_add_to_monitor_list(i5_bss, sta_item->mac);
		}
		if (monitor_sta) {
			rssi = WBD_RCPI_TO_RSSI(sta_item->rcpi);
			monitor_sta->rssi = rssi;
			monitor_sta->channel = sta_item->channel;
			monitor_sta->regclass = metric->opClass;
			monitor_sta->monitor_tm = time(NULL);
			WBD_INFO("Device["MACDBG"] BSS["MACDBG"] STA["MACDBG"] OPClass[%x] "
				"Channel[%d] RSSI[%d]\n",
				MAC2STRDBG(al_mac), MAC2STRDBG(i5_bss->BSSID),
				MAC2STRDBG(sta_item->mac), metric->opClass,
				sta_item->channel, rssi);
		}
	}
end:
	WBD_EXIT();
}

/* This function returns the value it receivied is RSSI or RCPI. If it can't
 * predict it returns UNKNOWN. The logic is explained assuming rssi_max is
 * -10dBm and rssi_min is -95dBm
 *  rssi	2's compliment	rcpi
 *  --------------------------------
 *  -10dBm	246		200
 *  -95dBm	161		 30
 *  ------------------------------
 *  From the above table, overlap is between 161 - 200 (2's compliment of
 *  rssi_min and RCPI of rssi_max)
 *  Also, RCPI will never be an odd number, if rssi is not fraction. So odd
 *  values between 161 and 200 can also be safely assumed as RSSI
 */
static int
wbd_master_check_rssi_or_rcpi(int value)
{
	wbd_info_t *info = wbd_get_ginfo();
	uint8 overlap_min = (uint8)info->rssi_min;
	uint8 overlap_max = (uint8)WBD_RSSI_TO_RCPI(info->rssi_max);

	WBD_INFO("RSSI Range [%d - %d] OVERLAP Range [%d - %d] value: %d\n",
		info->rssi_max, info->rssi_min, overlap_min, overlap_max, value);
	if (value < overlap_min) {
		return WBD_VALUE_TYPE_RCPI;
	} else if ((value > overlap_max) || (value & 1)) {
		return WBD_VALUE_TYPE_RSSI;
	}
	return WBD_VALUE_TYPE_UNKNOWN;
}

/* Converts RCPI to RSSI conversion smartly. It considers RCPI received as RCPI
 * only if falls within the safe RCPI range. Otherwise it assumes the value as
 * RSSI itself, and just changes its sign. This is required beacuse most the
 * STAs in market send RSSI itself as RCPI
 */
static int8
wbd_master_conv_rcpi_to_rssi(uint8 *rssi_or_rcpi, uint8 rcpi)
{
	if (WBD_SMART_RCPI_CONV_ENAB((wbd_get_ginfo()->flags))) {
		if (*rssi_or_rcpi == WBD_VALUE_TYPE_UNKNOWN) {
			*rssi_or_rcpi = wbd_master_check_rssi_or_rcpi(rcpi);
		}
		if (*rssi_or_rcpi == WBD_VALUE_TYPE_RCPI) {
			return WBD_RCPI_TO_RSSI(rcpi);
		}
	}
	return (int8)rcpi;
}

/* Update beacon RSSI to monitor STA item */
static void
wbd_master_update_beacon_rssi(wbd_beacon_reports_t  *wbd_bcn_rpt, dot11_rmrep_bcn_t *rmrep_bcn,
	wbd_monitor_sta_item_t *monitor_sta)
{
	wbd_sta_bounce_detect_t *entry;
	if (!monitor_sta) {
		return;
	}

	/* Use bouncing table to store if the beacon report received has RSSI or RCPI */
	entry = wbd_ds_find_sta_in_bouncing_table(wbd_get_ginfo()->wbd_master,
			&monitor_sta->sta_mac);
	if (!entry) {
		WBD_WARNING("Warning: bounce table has no STA["MACF"]\n",
			ETHER_TO_MACF(monitor_sta->sta_mac));
		return;
	}

	monitor_sta->bcn_rpt_rssi = wbd_master_conv_rcpi_to_rssi(
		&entry->rssi_or_rcpi, rmrep_bcn->rcpi);
	WBD_INFO("Monitor STA["MACF"] RCPI recieved: [%d (%d)] converted RSSI: [%d] "
		"rssi_or_rcpi: %s\n", ETHER_TO_MACF(monitor_sta->sta_mac), rmrep_bcn->rcpi,
		WBD_RCPI_TO_RSSI(rmrep_bcn->rcpi), monitor_sta->bcn_rpt_rssi,
		(entry->rssi_or_rcpi == WBD_VALUE_TYPE_RCPI) ? "RCPI" : "RSSI");
	monitor_sta->channel = rmrep_bcn->channel;
	monitor_sta->regclass = rmrep_bcn->reg;
	monitor_sta->bcn_tm = wbd_bcn_rpt->timestamp;
}

/* Got Beacon Metrics metric response */
void
wbd_master_beacon_metric_resp(unsigned char *al_mac, ieee1905_beacon_report *report)
{
	int ret = WBDE_OK, len_read = 0, i;
	uint8 prb_band;
	i5_dm_device_type *i5_device;
	i5_dm_bss_type *i5_bss;
	i5_dm_clients_type *i5_assoc_sta;
	dot11_rm_ie_t *ie;
	dot11_rmrep_bcn_t *rmrep_bcn;
	wbd_assoc_sta_item_t *assoc_sta = NULL;
	wbd_monitor_sta_item_t *monitor_sta = NULL;
	wbd_prb_sta_t *prbsta;
	wbd_beacon_reports_t *wbd_bcn_rpt = NULL;
	WBD_ENTER();

	WBD_SAFE_FIND_I5_DEVICE(i5_device, al_mac, &ret);

	wbd_bcn_rpt = wbd_ds_find_item_fm_beacon_reports(wbd_get_ginfo(),
		(struct ether_addr*)report->sta_mac, &ret);

	if (!wbd_bcn_rpt) {
		WBD_WARNING("No beacon report found for STA["MACDBG"]\n",
			MAC2STRDBG(report->sta_mac));
		goto end;
	}

	for (i = 0; i < report->report_element_count; i++) {
		ie = (dot11_rm_ie_t *)(report->report_element + len_read);
		len_read += (2 + ie->len);
		rmrep_bcn = (dot11_rmrep_bcn_t *)&ie[1];

		/* Update the probe table with band and active timestamp */
		prb_band = ieee1905_get_band_from_channel(rmrep_bcn->reg, rmrep_bcn->channel);
		if (!WBD_BAND_TYPE_LAN_2G(prb_band)) {
			/* update the band in probe STA table */
			prbsta = wbd_ds_find_sta_in_probe_sta_table(wbd_get_ginfo(),
				(struct ether_addr*)report->sta_mac, FALSE);
			if (prbsta) {
				prbsta->active = time(NULL);
				prbsta->band |= prb_band;
			} else {
				WBD_WARNING("Warning: probe list has no STA["MACDBG"]\n",
					MAC2STRDBG(report->sta_mac));
			}
		}

		WBD_INFO("From Device["MACDBG"] STA["MACDBG"] BEACON EVENT, regclass: %d, "
			"channel: %d, rcpi: %d, bssid["MACF"]\n",
			MAC2STRDBG(i5_device->DeviceId), MAC2STRDBG(report->sta_mac),
			rmrep_bcn->reg, rmrep_bcn->channel, rmrep_bcn->rcpi,
			ETHER_TO_MACF(rmrep_bcn->bssid));
		i5_bss = wbd_ds_get_i5_bss_in_topology((unsigned char*)&rmrep_bcn->bssid, &ret);
		if (ret != WBDE_OK) {
			WBD_INFO("BEACON EVENT from Device["MACDBG"] for BSS["MACF"]. %s\n",
				MAC2STRDBG(al_mac), ETHER_TO_MACF(rmrep_bcn->bssid),
				wbderrorstr(ret));
			continue;
		}

		/* Check in Monitor list */
		monitor_sta = wbd_ds_find_sta_in_bss_monitorlist(i5_bss,
			(struct ether_addr*)&report->sta_mac, &ret);
		if (monitor_sta) {
			wbd_master_update_beacon_rssi(wbd_bcn_rpt, rmrep_bcn, monitor_sta);
			continue;
		}

		/* Find it in assoc list of the BSS. This is for the BSS where the STA is
		 * associated to store the beacon report RSSI
		 */
		assoc_sta = NULL;
		i5_assoc_sta = wbd_ds_find_sta_in_bss_assoclist(i5_bss,
			(struct ether_addr*)report->sta_mac, &ret, &assoc_sta);
		if (i5_assoc_sta && assoc_sta) {
			wbd_sta_bounce_detect_t *entry = wbd_ds_find_sta_in_bouncing_table(
				wbd_get_ginfo()->wbd_master, (struct ether_addr*)report->sta_mac);
			if (!entry) {
				WBD_WARNING("No bouncing table entry for STA["MACDBG"] to check if"
					"it sends RSSI or RCPI\n", MAC2STRDBG(report->sta_mac));
				continue;
			}
			assoc_sta->stats.bcn_rpt_rssi = wbd_master_conv_rcpi_to_rssi(
				&entry->rssi_or_rcpi, rmrep_bcn->rcpi);
			assoc_sta->stats.bcn_tm = wbd_bcn_rpt->timestamp;
			WBD_INFO("Assoc STA["MACDBG"] RCPI recieved: [%d (%d)] converted RSSI: [%d]"
				"rssi_or_rcpi: %s\n", MAC2STRDBG(report->sta_mac),
				rmrep_bcn->rcpi, WBD_RCPI_TO_RSSI(rmrep_bcn->rcpi),
				assoc_sta->stats.bcn_rpt_rssi,
				(entry->rssi_or_rcpi == WBD_VALUE_TYPE_RCPI) ? "RCPI" : "RSSI");
		} else {
			WBD_WARNING("STA["MACDBG"] not found in BSS["MACDBG"]'s "
				"Monitor list and assoc list. Add it to monitor list\n",
				MAC2STRDBG(report->sta_mac), MAC2STRDBG(i5_bss->BSSID));
			/* If monitor STA item not exists add it */
			monitor_sta = wbd_master_add_to_monitor_list(i5_bss, report->sta_mac);
			if (monitor_sta) {
				wbd_master_update_beacon_rssi(wbd_bcn_rpt, rmrep_bcn, monitor_sta);
			}
		}
	}

end:
	WBD_EXIT();
}

/* Update the AP channel report */
void
wbd_master_update_ap_chan_report(wbd_glist_t *ap_chan_report, i5_dm_interface_type *i5_ifr)
{
	int ret = WBDE_OK;
	unsigned char channel;
	wbd_bcn_req_rclass_list_t *rclass_list = NULL;
	wbd_bcn_req_chan_list_t *chan_list = NULL;
	WBD_ENTER();

	/* if the rclass is present needs to add channel to the rclass list */
	rclass_list = wbd_ds_find_rclass_in_ap_chan_report(ap_chan_report, i5_ifr->opClass, &ret);
	if (rclass_list == NULL) {
		ret = wbd_ds_add_rclass_in_ap_chan_report(ap_chan_report, i5_ifr->opClass,
			&rclass_list);
		WBD_ASSERT();
	}

	/* If channel present no need to add the channel */
	channel = wf_chspec_ctlchan(i5_ifr->chanspec);
	chan_list = wbd_ds_find_channel_in_rclass_list(rclass_list, channel, &ret);
	if (chan_list == NULL) {
		ret = wbd_ds_add_channel_in_rclass_list(rclass_list, channel, NULL);
	}

end:
	WBD_EXIT();
}

/* Create AP chan report from topology */
static void
wbd_master_create_ap_chan_report(i5_dm_network_topology_type *i5_topology,
	wbd_master_info_t *master_info)
{
	int ret = WBDE_OK;
	i5_dm_device_type *i5_iter_device;
	i5_dm_interface_type *i5_iter_ifr;
	WBD_ENTER();

	WBD_ASSERT_ARG(i5_topology, WBDE_INV_ARG);
	WBD_ASSERT_ARG(master_info, WBDE_INV_ARG);

	/* Clean up the AP channel report */
	wbd_ds_ap_chan_report_cleanup(&master_info->ap_chan_report);

	/* Update the AP chan report with new channel values */
	foreach_i5glist_item(i5_iter_device, i5_dm_device_type, i5_topology->device_list) {
		if (!I5_IS_MULTIAP_AGENT(i5_iter_device->flags)) {
			continue;
		}

		foreach_i5glist_item(i5_iter_ifr, i5_dm_interface_type,
			i5_iter_device->interface_list) {

			if ((i5_iter_ifr->chanspec == 0) ||
				!i5DmIsInterfaceWireless(i5_iter_ifr->MediaType) ||
				!i5_iter_ifr->BSSNumberOfEntries ||
				WBD_IS_DEDICATED_BACKHAUL(i5_iter_ifr->mapFlags)) {
				continue;
			}
			wbd_master_update_ap_chan_report(&master_info->ap_chan_report, i5_iter_ifr);
		}
	}
end:
	WBD_EXIT();
}

/* Callback from IEEE 1905 module to get interface info */
int
wbd_master_get_interface_info_cb(char *ifname, ieee1905_ifr_info *info)
{
	WBD_ENTER();

	/* Check interface (for valid BRCM wl interface) */
	(void)blanket_probe(ifname);

	blanket_get_chanspec(ifname, &info->chanspec);

	WBD_EXIT();
	return WBDE_OK;
}

/* Callback for exception from communication handler for CLI */
static void
wbd_master_com_cli_exception(wbd_com_handle *hndl, void *arg, WBD_COM_STATUS status)
{
	WBD_ERROR("Exception from CLI server\n");
}

/* Register all the commands for master server to communication handle */
static int
wbd_master_register_server_command(wbd_info_t *info)
{
	/* Now register CLI commands */
	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_VERSION,
		wbd_process_version_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_SLAVELIST,
		wbd_master_process_bsslist_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_INFO,
		wbd_master_process_info_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_CLIENTLIST,
		wbd_master_process_clientlist_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_STEER,
		wbd_master_process_steer_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_LOGS,
		wbd_master_process_logs_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_MSGLEVEL,
		wbd_process_set_msglevel_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_BHOPT,
		wbd_master_process_bh_opt_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_EXIT,
		wbd_master_process_exit_cli_cmd, info, wbd_json_parse_cli_cmd);

	wbd_com_register_cmd(info->com_cli_hdl, WBD_CMD_CLI_RELOAD,
		wbd_master_process_reload_cli_cmd, info, wbd_json_parse_cli_cmd);

	return WBDE_OK;
}

/* Initialize the communication module for master */
int
wbd_init_master_com_handle(wbd_info_t *info)
{
	/* Initialize communication module for CLI */
	info->com_cli_hdl = wbd_com_init(info->hdl, info->cli_server_fd, 0x0000,
		wbd_json_parse_cli_cmd_name, wbd_master_com_cli_exception, info);
	if (!info->com_cli_hdl) {
		WBD_ERROR("Failed to initialize the communication module for CLI server\n");
		return WBDE_COM_ERROR;
	}

	wbd_master_register_server_command(info);

	return WBDE_OK;
}

/* Callback fn to send autoconfig renew message to unconfigured slaves on master restart */
static void
wbd_master_send_ap_autoconfig_renew_timer_cb(bcm_usched_handle *hdl, void *arg)
{
	int ret = WBDE_OK, configured = 0;
	i5_dm_network_topology_type *i5_topology;
	i5_dm_device_type *i5_device;
	i5_dm_interface_type *i5_ifr;
	static int restart_count = WBD_RENEW_TIMER_RESTART_COUNT;
	wbd_info_t *info = (wbd_info_t *)arg;

	WBD_ENTER();
	BCM_REFERENCE(ret);

	WBD_ASSERT_ARG(info, WBDE_INV_ARG);

	i5_topology = ieee1905_get_datamodel();

	/* Traverse Device List to send it to unconfigured devices as unicast msg in the network */
	foreach_i5glist_item(i5_device, i5_dm_device_type, i5_topology->device_list) {
		if (!(I5_IS_MULTIAP_AGENT(i5_device->flags))) {
			WBD_DEBUG("Device["MACDBG"] not slave. continue...\n",
				MAC2STRDBG(i5_device->DeviceId));
			continue;
		}
		foreach_i5glist_item(i5_ifr, i5_dm_interface_type, i5_device->interface_list) {
			if (i5_ifr->isConfigured == 1) {
				WBD_DEBUG("Device["MACDBG"] Interface["MACDBG"] is already "
					"configured\n", MAC2STRDBG(i5_device->DeviceId),
					MAC2STRDBG(i5_ifr->InterfaceId));
				configured = 1;
			}
			if (i5DmIsInterfaceWireless(i5_ifr->MediaType) &&
				i5_ifr->BSSNumberOfEntries && !i5_ifr->isConfigured) {
				WBD_DEBUG("Device["MACDBG"] Interface["MACDBG"] is not "
					"configured\n", MAC2STRDBG(i5_device->DeviceId),
					MAC2STRDBG(i5_ifr->InterfaceId));
				configured = 0;
				break;
			}
		}
		if (!configured) {
			/* If the interface is not configured for any device, just send renew msg
			 * to that particular device only instead of to all the devices.
			 */
			WBD_INFO("Device["MACDBG"] not configured. Send renew\n",
				MAC2STRDBG(i5_device->DeviceId));
			ieee1905_send_ap_autoconfig_renew(i5_device->DeviceId);
		}
	}

	restart_count--;
	if (restart_count > 0) {
		wbd_remove_timers(info->hdl, wbd_master_send_ap_autoconfig_renew_timer_cb, arg);
		ret = wbd_add_timers(info->hdl, info,
			WBD_SEC_MICROSEC(info->max.tm_autoconfig_renew),
			wbd_master_send_ap_autoconfig_renew_timer_cb, 0);
		if (ret != WBDE_OK) {
			WBD_WARNING("Interval[%d] Failed to create AP autoconfig renew timer "
				"Error: %d\n", info->max.tm_autoconfig_renew, ret);
		}
	}
end:
	WBD_EXIT();
}

/* Handle master restart; Send renew if the agent didn't AP Auto configuration */
int wbd_master_create_ap_autoconfig_renew_timer(wbd_info_t *info)
{
	int ret = WBDE_OK;
	WBD_ENTER();

	/* Validate arg */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);

	/* Postpone the timer by removing the existing timer and creating it again. If not exist
	 * also it will throw the warning. So no harm in removing the nonexisting timer
	 */
	wbd_remove_timers(info->hdl, wbd_master_send_ap_autoconfig_renew_timer_cb, info);

	WBD_INFO("Timer created for auto config renew, Interval [%d]\n",
		info->max.tm_autoconfig_renew);
	ret = wbd_add_timers(info->hdl, info,
		WBD_SEC_MICROSEC(info->max.tm_autoconfig_renew),
		wbd_master_send_ap_autoconfig_renew_timer_cb, 0);
	if (ret != WBDE_OK) {
		WBD_WARNING("Interval[%d] Failed to create AP autoconfig renew timer "
			"Error: %d\n", info->max.tm_autoconfig_renew, ret);
	}

end:
	WBD_EXIT();
	return ret;
}

/* Callback fn to send channel selection request to configured agents */
void
wbd_master_send_channel_select_cb(bcm_usched_handle *hdl, void *arg)
{
	int ret = WBDE_OK;
	i5_dm_device_type *device = (i5_dm_device_type *)arg;
	wbd_device_item_t *device_vndr;

	WBD_ENTER();
	BCM_REFERENCE(ret);

	WBD_ASSERT_ARG(device, WBDE_INV_ARG);
	WBD_ASSERT_ARG(device->vndr_data, WBDE_INV_ARG);

	device_vndr = (wbd_device_item_t*)device->vndr_data;
	device_vndr->flags &= ~WBD_DEV_FLAG_CHAN_SELECT_TIMER;

	WBD_INFO("Sending channel selection request to ["MACF"]\n",
		ETHERP_TO_MACF(device->DeviceId));
	wbd_master_send_channel_selection_request(wbd_get_ginfo(),
		device->DeviceId, NULL);
end:
	WBD_EXIT();
}

/* Callback fn to send channel prefernce query to configured agents */
void
wbd_master_send_channel_preference_query_cb(bcm_usched_handle *hdl, void *arg)
{
	int ret = WBDE_OK;
	i5_dm_device_type *device = (i5_dm_device_type *)arg;
	wbd_device_item_t *device_vndr;

	WBD_ENTER();
	BCM_REFERENCE(ret);

	WBD_ASSERT_ARG(device, WBDE_INV_ARG);
	WBD_ASSERT_ARG(device->vndr_data, WBDE_INV_ARG);

	device_vndr = (wbd_device_item_t*)device->vndr_data;
	device_vndr->flags &= ~WBD_DEV_FLAG_CHAN_PREFERENCE_TIMER;

	WBD_INFO("Sending channel preference query to ["MACF"]\n",
		ETHERP_TO_MACF(device->DeviceId));
	ieee1905_send_channel_preference_query(device->DeviceId);

	if (device_vndr->flags & WBD_DEV_FLAG_CHAN_SELECT_TIMER)  {
		WBD_DEBUG("Remove channel select timer for ["MACF"]\n",
			ETHERP_TO_MACF(device->DeviceId));
		wbd_remove_timers(hdl, wbd_master_send_channel_select_cb, device);
		device_vndr->flags &= ~WBD_DEV_FLAG_CHAN_SELECT_TIMER;
	}

	/* start the timer to send channel selection request */
	ret = wbd_add_timers(wbd_get_ginfo()->hdl, device,
		WBD_SEC_MICROSEC(WBD_TM_CHAN_SELECT_AFTER_QUERY),
		wbd_master_send_channel_select_cb, 0);
	if (ret != WBDE_OK) {
		WBD_WARNING("Timeout[%d] Failed to create channel select timer "
			"Error: %d\n", WBD_TM_CHAN_SELECT_AFTER_QUERY, ret);
	} else {
		device_vndr->flags |= WBD_DEV_FLAG_CHAN_SELECT_TIMER;
	}
end:
	WBD_EXIT();
}

/* Create channel select timer */
int
wbd_master_create_channel_select_timer(wbd_info_t *info, unsigned char *al_mac)
{
	int ret = WBDE_OK;
	i5_dm_device_type *device;
	wbd_device_item_t *device_vndr;
	WBD_ENTER();

	/* Validate arg */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);
	WBD_ASSERT_ARG(al_mac, WBDE_INV_ARG);

	device = wbd_ds_get_i5_device(al_mac, &ret);
	WBD_ASSERT();
	WBD_ASSERT_ARG(device->vndr_data, WBDE_INV_ARG);

	device_vndr = (wbd_device_item_t*)device->vndr_data;
	if (device_vndr->flags & WBD_DEV_FLAG_CHAN_PREFERENCE_TIMER)  {
		WBD_DEBUG("Remove channel preference timer for ["MACF"]\n", ETHERP_TO_MACF(al_mac));
		wbd_remove_timers(info->hdl, wbd_master_send_channel_preference_query_cb, device);
		device_vndr->flags &= ~WBD_DEV_FLAG_CHAN_PREFERENCE_TIMER;
	}

	WBD_INFO("Creating timer for channel select, timeout [%d] AL mac ["MACF"]\n",
		info->max.tm_channel_select, ETHERP_TO_MACF(al_mac));
	ret = wbd_add_timers(info->hdl, device,
		WBD_SEC_MICROSEC(info->max.tm_channel_select),
		wbd_master_send_channel_preference_query_cb, 0);
	if (ret != WBDE_OK) {
		WBD_WARNING("Timeout[%d] Failed to create channel select timer "
			"Error: %d\n", info->max.tm_channel_select, ret);
	} else {
		device_vndr->flags |= WBD_DEV_FLAG_CHAN_PREFERENCE_TIMER;
	}

end:
	WBD_EXIT();
	return ret;
}

#if defined(MULTIAPR2)
/* Callback fn to send OnBoot Channel Scan Request from Controller to Agent */
void
wbd_master_send_onboot_channel_scan_req_cb(bcm_usched_handle *hdl, void *arg)
{
	int ret = WBDE_OK;
	i5_dm_device_type *device = (i5_dm_device_type *)arg;
	wbd_device_item_t *device_vndr;

	WBD_ENTER();
	BCM_REFERENCE(ret);

	WBD_ASSERT_ARG(device, WBDE_INV_ARG);
	WBD_ASSERT_ARG(device->vndr_data, WBDE_INV_ARG);

	device_vndr = (wbd_device_item_t*)device->vndr_data;
	device_vndr->flags &= ~WBD_DEV_FLAG_ONBOOT_CHSCAN_TIMER;

	if (WBD_ONBOOT_NOT_FORCED_ON_CTRLAGENT(device->flags)) {
		WBD_INFO("Skip Requesting Onboot Channel Scan Report for "
			"Controller Agent Device.\n");
		goto end;
	}

	WBD_INFO("Sending OnBoot Channel Scan Request Message to ["MACF"]\n",
		ETHERP_TO_MACF(device->DeviceId));

	/* Send Channel Scan Request Message */
	wbd_master_send_chscan_req(device->DeviceId, 0);

end:
	WBD_EXIT();
}

/* Create OnBoot Channel Scan Request timer to Get Channel Scan Report from this Agent */
int
wbd_master_create_onboot_channel_scan_req_timer(wbd_info_t *info, unsigned char *al_mac)
{
	int ret = WBDE_OK;
	i5_dm_device_type *device, *self_device;
	wbd_device_item_t *device_vndr;
	WBD_ENTER();

	/* Validate arg */
	WBD_ASSERT_ARG(info, WBDE_INV_ARG);
	WBD_ASSERT_ARG(al_mac, WBDE_INV_ARG);
	WBD_SAFE_GET_I5_SELF_DEVICE(self_device, &ret);
	device = wbd_ds_get_i5_device(al_mac, &ret);
	WBD_ASSERT();
	WBD_ASSERT_ARG(device->vndr_data, WBDE_INV_ARG);

	device_vndr = (wbd_device_item_t*)device->vndr_data;

	if (eacmp(self_device->DeviceId, device->DeviceId) == 0) {
		goto end;
	}

	if (device_vndr->flags & WBD_DEV_FLAG_ONBOOT_CHSCAN_TIMER)  {
		WBD_DEBUG("OnBoot Channel Scan Request timer for ["MACF"] already exists\n",
			ETHERP_TO_MACF(al_mac));
		goto end;
	}

	WBD_INFO("Master Creating timer for OnBoot Channel Scan Request, timeout[%d] AL["MACF"]\n",
		info->max.tm_onboot_chscan_req, ETHERP_TO_MACF(al_mac));

	/* Add the timer */
	ret = wbd_add_timers(info->hdl, device,
		WBD_SEC_MICROSEC(info->max.tm_onboot_chscan_req),
		wbd_master_send_onboot_channel_scan_req_cb, 0);

	/* If Add Timer not Succeeded */
	if (ret != WBDE_OK) {
		if (ret == WBDE_USCHED_TIMER_EXIST) {
			WBD_INFO("OnBoot Channel Scan Request timer already exist\n");
		} else {
			WBD_WARNING("Timeout[%d] Failed to create OnBoot Channel Scan Request "
				"timer Error: %d\n", info->max.tm_onboot_chscan_req, ret);
		}
	} else {
		/* Set Onboot Channel Scan Request Timer creation flag */
		device_vndr->flags |= WBD_DEV_FLAG_ONBOOT_CHSCAN_TIMER;
	}

end:
	WBD_EXIT();
	return ret;
}
#endif /* MULTIAPR2 */

/* Update the lowest Tx Power of all BSS */
void
wbd_master_update_lowest_tx_pwr(wbd_info_t *info, unsigned char *al_mac,
	ieee1905_operating_chan_report *chan_report)
{
	int ret = WBDE_OK;
	i5_dm_interface_type *i5_ifr;
	WBD_ENTER();

	WBD_SAFE_GET_I5_IFR(al_mac, chan_report->radio_mac, i5_ifr, &ret);
	i5_ifr->TxPowerLimit = chan_report->tx_pwr;

	if (i5_ifr->TxPowerLimit == 0) {
		WBD_WARNING("tx power limit not receivied\n");
	} else if (info->base_txpwr <= 0 || info->base_txpwr > i5_ifr->TxPowerLimit) {
		info->base_txpwr = i5_ifr->TxPowerLimit;
	}

end:
	WBD_EXIT();
}

/* Store beacon reports sent from sta. Only one report from sta should be present in list.
 * These reports can be checked for timestamp before sending any new beacon metric query
 * If the sta is still associated to same device.
*/
void
wbd_master_store_beacon_metric_resp(wbd_info_t *info, unsigned char *al_mac,
	ieee1905_beacon_report *report)
{
	int ret = WBDE_OK;
	wbd_beacon_reports_t *wbd_bcn_rpt = NULL;
	uint8 *tmpbuf = NULL;
	uint16 len = 0;
	time_t now = 0;
	WBD_ENTER();

	/* Skip storing this report if response flags is not success
	 * or report count or report length is 0
	*/
	WBD_INFO("STA["MACDBG"] Device["MACDBG"] Beacon Report details "
		"response[%d] element_count[%d] element_len[%d]\n",
		MAC2STRDBG(report->sta_mac), MAC2STRDBG(al_mac), report->response,
		report->report_element_count, report->report_element_len);

	if ((report->response != IEEE1905_BEACON_REPORT_RESP_FLAG_SUCCESS) ||
		!report->report_element_count || !report->report_element_len) {
		goto end;
	}

	/* Check whether we already have reports recieved from this STA */
	wbd_bcn_rpt = wbd_ds_find_item_fm_beacon_reports(info,
		(struct ether_addr*)&(report->sta_mac), &ret);

	now = time(NULL);

	if (wbd_bcn_rpt && (!eacmp((struct ether_addr*)&al_mac, &(wbd_bcn_rpt->neighbor_al_mac))) &&
		((now - wbd_bcn_rpt->timestamp) < info->max.tm_per_chan_bcn_req)) {
		/* Since we already have beacon report from sta, this might be a case of
		 * multiple beacon responses from agent, update the present beacon report
		*/

		WBD_INFO("Beacon report already present. Append to it \n");

		len = report->report_element_len + wbd_bcn_rpt->report_element_len;
		tmpbuf = (uint8*)wbd_malloc(len, &ret);
		WBD_ASSERT_MSG(" STA["MACDBG"] Beacon report element malloc failed\n",
			MAC2STRDBG(report->sta_mac));

		memcpy(tmpbuf, wbd_bcn_rpt->report_element, wbd_bcn_rpt->report_element_len);
		free(wbd_bcn_rpt->report_element);
		wbd_bcn_rpt->report_element = NULL;

		memcpy(tmpbuf + wbd_bcn_rpt->report_element_len, report->report_element,
				report->report_element_len);

		wbd_bcn_rpt->report_element = tmpbuf;
		wbd_bcn_rpt->report_element_count += report->report_element_count;
		wbd_bcn_rpt->report_element_len = len;

		goto end;
	} else {
		if (wbd_bcn_rpt) {
			/* Remove the old beacon report */
			wbd_ds_remove_beacon_report(info, (struct ether_addr*)&(report->sta_mac));
		}
	}

	/* Add report to glist */
	wbd_bcn_rpt = wbd_ds_add_item_to_beacon_reports(info, (struct ether_addr*)al_mac, now,
		(struct ether_addr*)&report->sta_mac);
	if (!wbd_bcn_rpt) {
		WBD_WARNING("STA["MACDBG"] Beacon report store failed\n",
			MAC2STRDBG(report->sta_mac));
		goto end;
	}

	if (wbd_ds_find_sta_in_bouncing_table(info->wbd_master,
			(struct ether_addr*)&report->sta_mac) == NULL) {
		WBD_INFO("STA["MACDBG"] Not in bouncing table. Adding it since "
			"beacon report received\n", MAC2STRDBG(report->sta_mac));
		/* Sending bssid as NULL to skip steering related initializations */
		wbd_ds_add_sta_to_bounce_table(info->wbd_master,
			(struct ether_addr*)&report->sta_mac, NULL, 0, 0);
	}

	wbd_bcn_rpt->report_element_count = report->report_element_count;
	wbd_bcn_rpt->report_element_len = report->report_element_len;

	wbd_bcn_rpt->report_element = (uint8*)wbd_malloc(report->report_element_len, &ret);
	WBD_ASSERT_MSG(" STA["MACDBG"] Beacon report element malloc failed\n",
		MAC2STRDBG(report->sta_mac));

	memcpy(wbd_bcn_rpt->report_element, report->report_element, report->report_element_len);

end:
	WBD_EXIT();
}

/* Send BSS capability query message */
int
wbd_master_send_bss_capability_query(wbd_info_t *info, unsigned char *al_mac,
	unsigned char* radio_mac)
{
	int ret = WBDE_OK;
	ieee1905_vendor_data vndr_msg_data;
	WBD_ENTER();

	memset(&vndr_msg_data, 0x00, sizeof(vndr_msg_data));

	/* Allocate Dynamic mem for Vendor data from App */
	vndr_msg_data.vendorSpec_msg =
		(unsigned char *)wbd_malloc(IEEE1905_MAX_VNDR_DATA_BUF, &ret);
	WBD_ASSERT();

	/* Fill vndr_msg_data struct object to send Vendor Message */
	memcpy(vndr_msg_data.neighbor_al_mac, al_mac, IEEE1905_MAC_ADDR_LEN);

	WBD_INFO("Send BSS capability query from Device["MACDBG"] to Device["MACDBG"]\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(vndr_msg_data.neighbor_al_mac));

	/* Encode Vendor Specific TLV for Message : BSS capability query to send */
	ret = wbd_tlv_encode_bss_capability_query((void *)radio_mac,
		vndr_msg_data.vendorSpec_msg, &vndr_msg_data.vendorSpec_len);
	WBD_ASSERT_MSG("Failed to encode BSS capability query which needs to be sent "
		"from Device["MACDBG"] to Device["MACDBG"]\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(vndr_msg_data.neighbor_al_mac));

	/* Send Vendor Specific Message : BSS capability query */
	ret = wbd_master_send_brcm_vndr_msg(&vndr_msg_data);

	WBD_CHECK_ERR_MSG(WBDE_DS_1905_ERR, "Failed to send "
		"BSS capability query from Device["MACDBG"] to Device["MACDBG"], Error : %s\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(vndr_msg_data.neighbor_al_mac),
		wbderrorstr(ret));

end:
	if (vndr_msg_data.vendorSpec_msg) {
		free(vndr_msg_data.vendorSpec_msg);
	}
	WBD_EXIT();
	return ret;
}

/* Processes BSS capability report message */
static void
wbd_master_process_bss_capability_report_cmd(unsigned char *neighbor_al_mac,
	unsigned char *tlv_data, unsigned short tlv_data_len)
{
	int ret = WBDE_OK;
	i5_dm_device_type *i5_device = NULL;
	WBD_ENTER();

	/* Validate Message data */
	WBD_ASSERT_MSGDATA(tlv_data, "BSS Capability Report");

	WBD_SAFE_FIND_I5_DEVICE(i5_device, neighbor_al_mac, &ret);

	/* Decode Vendor Specific TLV for Message : BSS capability report on receive */
	ret = wbd_tlv_decode_bss_capability_report((void *)i5_device, tlv_data, tlv_data_len);
	if (ret != WBDE_OK) {
		WBD_WARNING("Failed to decode the BSS capability Report TLV\n");
		goto end;
	}

end:
	WBD_EXIT();
}

/* Processes steer response report message */
static void
wbd_master_process_steer_resp_report_cmd(unsigned char *neighbor_al_mac,
	unsigned char *tlv_data, unsigned short tlv_data_len)
{
	int ret = WBDE_OK;
	wbd_cmd_steer_resp_rpt_t steer_resp_rpt;
	char logmsg[WBD_MAX_BUF_128] = {0}, timestamp[WBD_MAX_BUF_32] = {0};

	WBD_ENTER();

	/* Validate Message data */
	WBD_ASSERT_MSGDATA(tlv_data, "Steer Response Report");

	memset(&steer_resp_rpt, 0, sizeof(steer_resp_rpt));

	/* Decode Vendor Specific TLV for Message : Steer Response report on receive */
	ret = wbd_tlv_decode_steer_resp_report(&steer_resp_rpt, tlv_data, tlv_data_len);
	WBD_ASSERT_MSG("Failed to decode the Steer Response Report TLV From Device["MACDBG"]\n",
		MAC2STRDBG(neighbor_al_mac));

	/* Create and store steer resp log */
	snprintf(logmsg, sizeof(logmsg), CLI_CMD_LOGS_STEER_NO_RESP,
		wbd_get_formated_local_time(timestamp, sizeof(timestamp)),
		MAC2STRDBG(steer_resp_rpt.sta_mac));
	wbd_ds_add_logs_in_master(wbd_get_ginfo()->wbd_master, logmsg);
end:
	WBD_EXIT();
}

/* skip zwdfs command for dedicated backhaul, else broadcast to all agents
 * operating in same band with interface originally intiated the request
 */
static void
wbd_master_process_zwdfs_cmd(unsigned char *src_al_mac, unsigned char *tlv_data,
	unsigned short tlv_data_len)
{
	int ret = WBDE_OK;
	wbd_info_t *info = wbd_get_ginfo();
	wbd_cmd_vndr_zwdfs_msg_t zwdfs_msg;
	i5_dm_interface_type *pdmif = NULL;

	WBD_ENTER();

	if (WBD_MCHAN_ENAB(info->flags)) {
		WBD_INFO("Multi chan mode is ON, skip inform ZWDFS msg to other agents \n");
		goto end;
	}

	memset(&zwdfs_msg, 0, sizeof(zwdfs_msg));

	ret = wbd_tlv_decode_zwdfs_msg((void*)&zwdfs_msg, tlv_data, tlv_data_len);
	WBD_ASSERT();

	pdmif = wbd_ds_get_i5_interface(src_al_mac, (uchar*)&zwdfs_msg.mac, &ret);
	WBD_ASSERT();

	WBD_INFO("Rcvd ZWDFS msg for reason[%d] from: SRC_AL_MAC["MACDBG"] SRC interface["MACF"]"
		" cntrl_chan[%d], opclass[%d]\n", zwdfs_msg.reason, MAC2STRDBG(src_al_mac),
		ETHERP_TO_MACF(pdmif->InterfaceId), zwdfs_msg.cntrl_chan, zwdfs_msg.opclass);

	ret = wbd_master_broadcast_vendor_msg_zwdfs(src_al_mac, &zwdfs_msg,
		ieee1905_get_band_from_channel(zwdfs_msg.opclass, zwdfs_msg.cntrl_chan));
	if (ret != WBDE_OK) {
		WBD_ERROR(" error in informing ZWDFS msg to agents \n");
	}
end:
	WBD_EXIT();
}

/* Function to send CAC request message to the give device and interface.
 * TODO: Add a vendor TLV to send control channel to support single channel operation
 */
static void
wbd_master_send_cac_request_message(uint8 *al_mac, uint8 *radio_mac,
	wl_chan_change_reason_t reason, uint8 cntrl_chan, uint8 opclass)
{
	ieee1905_cac_rqst_list_t cac_list;
	ieee1905_radio_cac_rqst_t *cac_rqst;

	WBD_ENTER();

	memset(&cac_list, 0, sizeof(cac_list));

	/* Supports only one CAC request TLV */
	cac_list.count = 1;
	cac_rqst = cac_list.params;
	memcpy(cac_rqst->mac, radio_mac, ETHER_ADDR_LEN);
	cac_rqst->opclass = opclass;

	if (opclass <= REGCLASS_5G_40MHZ_LAST) {
		cac_rqst->chan = cntrl_chan;
	} else {
		uint bw = 0;
		blanket_get_bw_from_rc(opclass, &bw);
		cac_rqst->chan = CHSPEC_CHANNEL(wf_channel2chspec(
			cntrl_chan, bw, WL_CHANSPEC_BAND_5G));
	}
	/* Set request method and CAC completion action based on the reason received.
	 * method: Full CAC or 3+1, Action: Change channel or stunt
	 */
	switch (reason) {
		case WL_CHAN_REASON_CSA_TO_DFS_CHAN_FOR_CAC_ONLY:
			SET_MAP_CAC_METHOD(cac_rqst->flags, MAP_CAC_METHOD_CONTINOUS_CAC);
			SET_MAP_CAC_COMPLETION_ACTION(cac_rqst->flags,
				MAP_CAC_ACTION_RETURN_TO_PREV_CHANNEL);
			break;
		case WL_CHAN_REASON_CSA:
			SET_MAP_CAC_METHOD(cac_rqst->flags, MAP_CAC_METHOD_CONTINOUS_CAC);
			SET_MAP_CAC_COMPLETION_ACTION(cac_rqst->flags,
				MAP_CAC_ACTION_STAY_ON_NEW_CHANNEL);
			break;
		case WL_CHAN_REASON_DFS_AP_MOVE_START:
			SET_MAP_CAC_METHOD(cac_rqst->flags,
				MAP_CAC_METHOD_MIMO_DIMENSION_REDUCED_CAC);
			SET_MAP_CAC_COMPLETION_ACTION(cac_rqst->flags,
				MAP_CAC_ACTION_STAY_ON_NEW_CHANNEL);
			break;
		case WL_CHAN_REASON_DFS_AP_MOVE_STUNT:
			SET_MAP_CAC_METHOD(cac_rqst->flags,
				MAP_CAC_METHOD_MIMO_DIMENSION_REDUCED_CAC);
			SET_MAP_CAC_COMPLETION_ACTION(cac_rqst->flags,
				MAP_CAC_ACTION_RETURN_TO_PREV_CHANNEL);
			break;
		default:
			WBD_WARNING("Unknown CAC reason [%d]\n", reason);
			return;
	}
	WBD_INFO("Sending CAC request to device ["MACF"] interface ["MACF"] for opclass [%d] "
		"channel [%d] reason [%d] flag [0x%x]\n", ETHERP_TO_MACF(al_mac),
		ETHERP_TO_MACF(cac_rqst->mac), cac_rqst->opclass, cac_rqst->chan, reason,
		cac_rqst->flags);
	ieee1905_send_cac_request(al_mac, &cac_list);

	WBD_EXIT();
}

/* Function to send CAC termination message to the give device and interface.
 * TODO: Add a vendor TLV to send control channel to support single channel operation
 */
void
wbd_master_send_cac_termination_message(uint8 *al_mac, uint8 *radio_mac,
	uint8 cntrl_chan, uint8 opclass)
{
	ieee1905_cac_termination_list_t cac_term_list;
	ieee1905_radio_cac_params_t *cac_term;

	WBD_ENTER();

	memset(&cac_term_list, 0, sizeof(cac_term_list));

	/* Supports only one CAC request TLV */
	cac_term_list.count = 1;
	cac_term = cac_term_list.params;
	memcpy(cac_term->mac, radio_mac, ETHER_ADDR_LEN);
	cac_term->opclass = opclass;

	if (opclass <= REGCLASS_5G_40MHZ_LAST) {
		cac_term->chan = cntrl_chan;
	} else {
		uint bw = 0;
		blanket_get_bw_from_rc(opclass, &bw);
		cac_term->chan = CHSPEC_CHANNEL(wf_channel2chspec(
			cntrl_chan, bw, WL_CHANSPEC_BAND_5G));
	}
	WBD_INFO("Sending CAC termination to device ["MACF"] interface ["MACF"] for opclass [%d] "
		"channel [%d]\n", ETHERP_TO_MACF(al_mac),
		ETHERP_TO_MACF(cac_term->mac), cac_term->opclass, cac_term->chan);
	ieee1905_send_cac_termination(al_mac, &cac_term_list);

	WBD_EXIT();
}

/* Send 1905 Vendor Specific Zero wait DFS command, from Controller to Agents.
 * For R2 agents it uses cac request/termination messages instead of vendor comamnds
 */
int
wbd_master_broadcast_vendor_msg_zwdfs(uint8 *src_al_mac, wbd_cmd_vndr_zwdfs_msg_t *msg, uint8 band)
{
	int ret = WBDE_OK;
	ieee1905_vendor_data vndr_msg_data;
	i5_dm_network_topology_type *ctlr_topology;
	wbd_cmd_vndr_zwdfs_msg_t zwdfs_msg;
	i5_dm_device_type *i5_iter_device;
	i5_dm_interface_type *i5_iter_ifr;

	WBD_ENTER();

	memset(&vndr_msg_data, 0, sizeof(vndr_msg_data));

	/* prepare common elements for all agents */
	memset(&zwdfs_msg, 0, sizeof(zwdfs_msg));

	zwdfs_msg.cntrl_chan = msg->cntrl_chan;
	zwdfs_msg.opclass = msg->opclass;
	zwdfs_msg.reason = msg->reason;
	/* Allocate Dynamic mem for Vendor data from App */
	vndr_msg_data.vendorSpec_msg =
		(unsigned char *)wbd_malloc((sizeof(wbd_cmd_vndr_zwdfs_msg_t) + sizeof(i5_tlv_t)),
			&ret);
	WBD_ASSERT();

	/* Get Topology of Controller from 1905 lib */
	ctlr_topology = (i5_dm_network_topology_type *)ieee1905_get_datamodel();

	/* Loop for all the Devices in Controller, to send zero wait dfs msg to all Agents */
	foreach_i5glist_item(i5_iter_device, i5_dm_device_type, ctlr_topology->device_list) {
		if (i5DmDeviceIsSelf(i5_iter_device->DeviceId)) {
			WBD_DEBUG(" self device, ignore continue \n");
			continue;
		}
		if (eacmp(i5_iter_device->DeviceId, src_al_mac) == 0) {
			WBD_DEBUG("same device as SRC AL MAC , ignore, continue\n");
			continue;
		}
		foreach_i5glist_item(i5_iter_ifr, i5_dm_interface_type,
			i5_iter_device->interface_list) {

			if ((i5_iter_ifr->chanspec == 0) ||
				!i5DmIsInterfaceWireless(i5_iter_ifr->MediaType)) {
				continue;
			}
			if (!(band &
				ieee1905_get_band_from_radiocaps(&i5_iter_ifr->ApCaps.RadioCaps))) {
				WBD_INFO("skip broadcast zwdfs msg for required Mismatch band[%d],"
					" with agent["MACF"] if_band[%d]\n", band,
					ETHERP_TO_MACF(i5_iter_ifr->InterfaceId),
					ieee1905_get_band_from_radiocaps(
					&i5_iter_ifr->ApCaps.RadioCaps));
				continue;
			}
			if (i5_iter_device->profile < ieee1905_map_profile2) {
				/* Fill Destination AL_MAC */
				memcpy(vndr_msg_data.neighbor_al_mac, i5_iter_device->DeviceId,
					IEEE1905_MAC_ADDR_LEN);

				WBD_INFO("Send ZWDFS MSG for reason[%d] from Device["MACDBG"] to"
					"Device["MACDBG"] for interface["MACF"] cntrl_chan[%d],"
					" opclass[%d]\n",
					zwdfs_msg.reason, MAC2STRDBG(ieee1905_get_al_mac()),
					MAC2STRDBG(i5_iter_device->DeviceId),
					ETHERP_TO_MACF(i5_iter_ifr->InterfaceId),
					zwdfs_msg.cntrl_chan, zwdfs_msg.opclass);

				memcpy(&zwdfs_msg.mac, i5_iter_ifr->InterfaceId, ETHER_ADDR_LEN);

				wbd_tlv_encode_zwdfs_msg((void *)&zwdfs_msg,
					vndr_msg_data.vendorSpec_msg,
					&vndr_msg_data.vendorSpec_len);

				WBD_DEBUG("zwdfs msg len[%d]\n", vndr_msg_data.vendorSpec_len);
				/* Send Vendor Specific Message with Zero wait dfs TLV */
				wbd_master_send_brcm_vndr_msg(&vndr_msg_data);
				continue;
			}
			if ((zwdfs_msg.reason == WL_CHAN_REASON_DFS_AP_MOVE_RADAR_FOUND) ||
				(zwdfs_msg.reason == WL_CHAN_REASON_DFS_AP_MOVE_ABORTED)) {
				wbd_master_send_cac_termination_message(
					i5_iter_device->DeviceId, i5_iter_ifr->InterfaceId,
					zwdfs_msg.cntrl_chan, zwdfs_msg.opclass);
			} else {
				wbd_master_send_cac_request_message(
					i5_iter_device->DeviceId, i5_iter_ifr->InterfaceId,
					zwdfs_msg.reason, zwdfs_msg.cntrl_chan, zwdfs_msg.opclass);
			}
		}
	}

end:
	if (vndr_msg_data.vendorSpec_msg) {
		free(vndr_msg_data.vendorSpec_msg);
	}

	WBD_EXIT();
	return ret;

}

/* Send 1905 Vendor Specific backhaul STA mertric policy command, from Controller to Agent */
int
wbd_master_send_backhaul_sta_metric_policy_vndr_cmd(wbd_info_t *info,
	unsigned char *neighbor_al_mac, unsigned char *radio_mac)
{
	int ret = WBDE_OK;
	i5_dm_interface_type *i5_ifr;
	ieee1905_vendor_data vndr_msg_data;
	wbd_master_info_t *master;
	WBD_ENTER();

	memset(&vndr_msg_data, 0, sizeof(vndr_msg_data));

	if (!neighbor_al_mac || !radio_mac) {
		WBD_ERROR("Invalid Device or Radio address\n");
		goto end;
	}

	/* Send the backhaul STA metric policy vendor message only if there is a backhaul BSS */
	i5_ifr = wbd_ds_get_i5_interface(neighbor_al_mac, radio_mac, &ret);
	WBD_ASSERT();

	/* If there is no backhaul BSS on this interface, do not send the backhaul policy */
	if (!I5_IS_BSS_BACKHAUL(i5_ifr->mapFlags)) {
		WBD_DEBUG("In Device["MACDBG"] IFR["MACDBG"] mapFlags[%x], No backhaul BSS\n",
			MAC2STRDBG(neighbor_al_mac), MAC2STRDBG(radio_mac), i5_ifr->mapFlags);
		goto end;
	}

	WBD_SAFE_GET_MASTER_INFO(info, WBD_BKT_ID_BR0, master, (&ret));

	/* Allocate Dynamic mem for Vendor data from App */
	vndr_msg_data.vendorSpec_msg =
		(unsigned char *)wbd_malloc(IEEE1905_MAX_VNDR_DATA_BUF, &ret);
	WBD_ASSERT();

	/* Encode Vendor Specific TLV for Message : backhaul STA mertric policy to send */
	wbd_tlv_encode_backhaul_sta_metric_report_policy((void *)&master->metric_policy_bh,
		vndr_msg_data.vendorSpec_msg, &vndr_msg_data.vendorSpec_len);

	/* Fill Destination AL_MAC */
	memcpy(vndr_msg_data.neighbor_al_mac, neighbor_al_mac, IEEE1905_MAC_ADDR_LEN);

	WBD_INFO("Send backhaul STA mertric policy from Device["MACDBG"] to Device["MACDBG"]\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(neighbor_al_mac));

	/* Send Vendor Specific Message : backhaul STA mertric policy */
	ret = wbd_master_send_brcm_vndr_msg(&vndr_msg_data);
	WBD_CHECK_ERR_MSG(WBDE_DS_1905_ERR, "Failed to send backhaul STA mertric policy from "
		"Device["MACDBG"] to Device["MACDBG"], Error : %s\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(vndr_msg_data.neighbor_al_mac),
		wbderrorstr(ret));

end:
	if (vndr_msg_data.vendorSpec_msg) {
		free(vndr_msg_data.vendorSpec_msg);
	}

	WBD_EXIT();
	return ret;
}

/* Create the NVRAM list with SSID for a particular interface */
static int
wbd_master_create_ssid_nvrams(wbd_cmd_vndr_nvram_set_t *cmd,
	i5_dm_interface_type *i5_ifr, int *count)
{
	int ret = WBDE_OK, idx = 0, ssid_idx = 0, n_allocated;
	wbd_cmd_vndr_nvram_t *nvrams;
	wbd_cmd_vndr_ssid_nvram_t *ssid_nvrams;
	char prefix[NVRAM_MAX_PARAM_LEN+2] = {0}, *nvval, tmp[(2 * NVRAM_MAX_PARAM_LEN) + 2];
	char name[NVRAM_MAX_VALUE_LEN], *bss_names, *next = NULL;
	WBD_ENTER();

	if (WBD_GET_SSID_NVRAMS_COUNT() <= 0) {
		WBD_DEBUG("No NVRAMs to send\n");
		goto end;
	}

	/* Read BSS info names */
	bss_names = blanket_nvram_safe_get(NVRAM_MAP_BSS_NAMES);
	if (strlen(bss_names) <= 0) {
		WBD_DEBUG("NVRAM[%s] Not set. Cannot read BSS details\n", NVRAM_MAP_BSS_NAMES);
		goto end;
	}

	/* Get the number of SSIDs */
	foreach(name, bss_names, next) {
		cmd->n_ssid++;
	}

	if (cmd->n_ssid == 0) {
		WBD_DEBUG("Number of SSID's are %d\n", cmd->n_ssid);
		goto end;
	}

	/* Copy the interface MAC address */
	memcpy(cmd->mac, i5_ifr->InterfaceId, sizeof(cmd->mac));

	cmd->ssid_nvrams = (wbd_cmd_vndr_ssid_nvram_t*)
		wbd_malloc((sizeof(wbd_cmd_vndr_ssid_nvram_t) * cmd->n_ssid), &ret);
	WBD_ASSERT();

	ssid_nvrams = cmd->ssid_nvrams;

	/* For each bss info names add NVRAMs */
	foreach(name, bss_names, next) {

		int nvram_idx = 0;

		snprintf(prefix, sizeof(prefix), "%s_", name);

		nvval = blanket_nvram_prefix_safe_get(prefix, NVRAM_SSID);
		memcpy(ssid_nvrams[ssid_idx].ssid.SSID, nvval,
			sizeof(ssid_nvrams[ssid_idx].ssid.SSID));
		ssid_nvrams[ssid_idx].ssid.SSID_len = strlen(nvval);

		/* Allocate memory for the BSS NVRAMs */
		n_allocated = WBD_GET_SSID_NVRAMS_COUNT();
		ssid_nvrams[ssid_idx].nvrams = (wbd_cmd_vndr_nvram_t*)wbd_malloc(
			(sizeof(wbd_cmd_vndr_nvram_t) * n_allocated), &ret);
		WBD_ASSERT();

		nvrams = ssid_nvrams[ssid_idx].nvrams;

		/* For each BSS NVRAMs */
		for (idx = 0; idx < n_allocated; idx++) {

			/* If the band is not matching do not send */
			if (!(ssid_nvram_params[idx].band_flag & i5_ifr->band)) {
				continue;
			}

			nvval = nvram_get(strcat_r(prefix,
				ssid_nvram_params[idx].name, tmp));
			/* If the NVRAM is not defined, do not include */
			if (!nvval || (nvval[0] == '\0')) {
				WBD_DEBUG("idx %d Radio["MACDBG"] SSID[%s] NVRAM %s not defined\n",
					nvram_idx, MAC2STRDBG(i5_ifr->InterfaceId),
					ssid_nvrams[ssid_idx].ssid.SSID, tmp);
				continue;
			}

			memcpy(nvrams[nvram_idx].name, ssid_nvram_params[idx].name,
				strlen(ssid_nvram_params[idx].name));
			snprintf(nvrams[nvram_idx].value,
				sizeof(nvrams[nvram_idx].value), "%s", nvval);
			nvram_idx++;
		}
		ssid_nvrams[ssid_idx].n_nvrams = nvram_idx;
		ssid_idx++;
		*count = *count + nvram_idx;
	}

end:
	WBD_EXIT();
	return ret;
}

/* Create the NVRAM list for Radio */
static int
wbd_master_create_radio_nvrams(wbd_cmd_vndr_nvram_set_t *cmd,
	i5_dm_interface_type *i5_ifr, int *count)
{
	int ret = WBDE_OK, idx = 0, radio_idx = 0, nvram_idx = 0, n_allocated;
	wbd_cmd_vndr_nvram_t *nvrams;
	wbd_cmd_vndr_prefix_nvram_t *radio_nvrams;
	char *nvval;
	WBD_ENTER();

	if (WBD_GET_RADIO_NVRAMS_COUNT() <= 0) {
		WBD_DEBUG("No NVRAMs to send\n");
		goto end;
	}

	cmd->n_radios = (uint8)1;
	if (cmd->n_radios == 0) {
		WBD_DEBUG("Zero radio's\n");
		goto end;
	}

	cmd->radio_nvrams = (wbd_cmd_vndr_prefix_nvram_t*)
		wbd_malloc((sizeof(wbd_cmd_vndr_prefix_nvram_t) * cmd->n_radios), &ret);
	WBD_ASSERT();

	radio_nvrams = cmd->radio_nvrams;

	/* Allocate memory for the Radio NVRAMs */
	n_allocated = WBD_GET_RADIO_NVRAMS_COUNT();
	radio_nvrams[radio_idx].nvrams = (wbd_cmd_vndr_nvram_t*)wbd_malloc(
		(sizeof(wbd_cmd_vndr_nvram_t) * n_allocated), &ret);
	WBD_ASSERT();

	nvrams = radio_nvrams[radio_idx].nvrams;
	memcpy(radio_nvrams[radio_idx].mac, i5_ifr->InterfaceId,
		sizeof(radio_nvrams[radio_idx].mac));

	/* For each Radio NVRAMs */
	for (idx = 0; idx < n_allocated; idx++) {

		/* If the band is not matching do not send */
		if (!(radio_nvram_params[idx].band_flag & i5_ifr->band)) {
			continue;
		}

		nvval = blanket_nvram_safe_get(radio_nvram_params[idx].name);
		/* If the NVRAM is not defined, do not include */
		if (!nvval || (nvval[0] == '\0')) {
			WBD_DEBUG("idx %d Radio["MACDBG"] NVRAM %s not defined\n",
				nvram_idx, MAC2STRDBG(i5_ifr->InterfaceId),
				radio_nvram_params[idx].name);
			continue;
		}

		memcpy(nvrams[nvram_idx].name, radio_nvram_params[idx].name,
			strlen(radio_nvram_params[idx].name));
		snprintf(nvrams[nvram_idx].value, sizeof(nvrams[nvram_idx].value),
			"%s", nvval);
		nvram_idx++;
	}
	radio_nvrams[radio_idx].n_nvrams = nvram_idx;
	radio_idx++;

	*count = *count + nvram_idx;

end:
	WBD_EXIT();
	return ret;
}

/* Create the NVRAM list for device */
static int
wbd_master_create_device_nvrams(wbd_cmd_vndr_nvram_set_t *cmd, int *count)
{
	int ret = WBDE_OK, idx = 0;
	WBD_ENTER();

	/* Allocate memory for the NVRAMs without prefix */
	cmd->n_common_nvrams = WBD_GET_NVRAMS_COUNT();
	cmd->common_nvrams = (wbd_cmd_vndr_nvram_t*)
		wbd_malloc((sizeof(wbd_cmd_vndr_nvram_t) * cmd->n_common_nvrams), &ret);
	WBD_ASSERT();

	/* For each NVRAMs */
	for (idx = 0; idx < WBD_GET_NVRAMS_COUNT(); idx++) {
		memcpy(cmd->common_nvrams[idx].name, nvram_params[idx].name,
			strlen(nvram_params[idx].name));
		/* Integer value NVRAMs */
		if (nvram_params[idx].flags & WBD_NVRAM_FLAG_VALINT) {
			snprintf(cmd->common_nvrams[idx].value,
				sizeof(cmd->common_nvrams[idx].value),
				"%d", blanket_get_config_val_int(NULL, nvram_params[idx].name,
				nvram_params[idx].def));
		} else { /* String value NVRAMs */
			snprintf(cmd->common_nvrams[idx].value,
				sizeof(cmd->common_nvrams[idx].value),
				"%s", blanket_nvram_safe_get(nvram_params[idx].name));
		}
	}
	*count = *count + idx;

end:
	WBD_EXIT();
	return ret;
}

/* Send NVRAM set vendor specific message */
static int
wbd_master_send_vndr_nvram_set_cmd(i5_dm_device_type *i5_device, i5_dm_interface_type *i5_ifr)
{
	int ret = WBDE_OK, count = 0;
	ieee1905_vendor_data vndr_msg_data;
	wbd_cmd_vndr_nvram_set_t cmd;
	wbd_device_item_t *device_vndr;
	WBD_ENTER();

	memset(&vndr_msg_data, 0, sizeof(vndr_msg_data));
	memset(&cmd, 0, sizeof(cmd));

	device_vndr = (wbd_device_item_t*)i5_device->vndr_data;

	WBD_DEBUG("NVRAM set vendor message for Device["MACDBG"] and radio["MACDBG"]\n",
		MAC2STRDBG(i5_device->DeviceId), MAC2STRDBG(i5_ifr->InterfaceId));

	/* Allocate Dynamic mem for Vendor data from App */
	vndr_msg_data.vendorSpec_msg =
		(unsigned char *)wbd_malloc(IEEE1905_MAX_VNDR_DATA_BUF, &ret);
	WBD_ASSERT();

	/* If already sent, dont add */
	if (!(device_vndr->flags & WBD_DEV_FLAG_NVRAM_SET)) {
		ret = wbd_master_create_device_nvrams(&cmd, &count);
		WBD_ASSERT();
	}

	ret = wbd_master_create_radio_nvrams(&cmd, i5_ifr, &count);
	WBD_ASSERT();

	ret = wbd_master_create_ssid_nvrams(&cmd, i5_ifr, &count);
	WBD_ASSERT();

	/* If there is no NVRAMs to send. Do not send message */
	if (count <= 0) {
		WBD_INFO("No NVRAMs to send\n");
		goto end;
	}

	/* Encode Vendor Specific TLV for Message : NVRAM set to send */
	wbd_tlv_encode_nvram_set((void *)&cmd, vndr_msg_data.vendorSpec_msg,
		&vndr_msg_data.vendorSpec_len);

	/* Fill Destination AL_MAC */
	memcpy(vndr_msg_data.neighbor_al_mac, i5_device->DeviceId, IEEE1905_MAC_ADDR_LEN);

	WBD_INFO("Send NVRAM set vendor message from Device["MACDBG"] to Device["MACDBG"]\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(i5_device->DeviceId));

	/* Send Vendor Specific Message : NVRAM set */
	ret = wbd_master_send_brcm_vndr_msg(&vndr_msg_data);
	WBD_CHECK_ERR_MSG(WBDE_DS_1905_ERR, "Failed to send NVRAM set vendor message from "
		"Device["MACDBG"] to Device["MACDBG"], Error : %s\n",
		MAC2STRDBG(ieee1905_get_al_mac()), MAC2STRDBG(vndr_msg_data.neighbor_al_mac),
		wbderrorstr(ret));
	device_vndr->flags |= WBD_DEV_FLAG_NVRAM_SET;

end:
	if (vndr_msg_data.vendorSpec_msg) {
		free(vndr_msg_data.vendorSpec_msg);
	}

	wbd_free_nvram_sets(&cmd);

	WBD_EXIT();
	return ret;
}

/* send master's dfs chan info i.e. intersection of all agent's chan info */
static void
wbd_master_send_dfs_chan_info(wbd_dfs_chan_info_t *dfs_chan_info,
	uint8 band, unsigned char *al_mac, unsigned char *radio_mac)
{
	ieee1905_vendor_data vndr_msg;
	wbd_cmd_vndr_controller_dfs_chan_info_t chan_info_msg;
	int ret = WBDE_OK;

	WBD_ENTER();

	memset(&vndr_msg, 0, sizeof(vndr_msg));
	memset(&chan_info_msg, 0, sizeof(chan_info_msg));

	vndr_msg.vendorSpec_msg =
		(unsigned char *)wbd_malloc(IEEE1905_MAX_VNDR_DATA_BUF, &ret);
	WBD_ASSERT();
	memcpy(vndr_msg.neighbor_al_mac, al_mac, IEEE1905_MAC_ADDR_LEN);

	memcpy(&chan_info_msg.mac, radio_mac, ETHER_ADDR_LEN);
	chan_info_msg.band = band;
	chan_info_msg.chan_info = dfs_chan_info;

	WBD_DEBUG("send msg to MAC["MACF"] band[%d] count[%d] \n",
		ETHERP_TO_MACF(&chan_info_msg.mac), chan_info_msg.band,
		chan_info_msg.chan_info->count);

	wbd_tlv_encode_dfs_chan_info((void*)&chan_info_msg, vndr_msg.vendorSpec_msg,
		&vndr_msg.vendorSpec_len);

	WBD_DEBUG("CHAN_INFO vendor msg len[%d] MACF["MACF"] band[%d]\n",
		vndr_msg.vendorSpec_len, ETHERP_TO_MACF(&chan_info_msg.mac),
		chan_info_msg.band);

	wbd_master_send_brcm_vndr_msg(&vndr_msg);

	free(vndr_msg.vendorSpec_msg);

end:
	WBD_EXIT();
}

#if !defined(MULTIAPR2)
/* Verify chan info from valid interface's chan info, Get common dfs chan info
 * and Broadcast to each agent based on band matching.
 *
 * DFS chan info only valid for 2G.
 *
 */
static void
wbd_master_process_intf_chan_info(unsigned char *src_al_mac, unsigned char *tlv_data,
	unsigned short tlv_data_len)
{
	i5_dm_network_topology_type *i5_topology = NULL;
	i5_dm_device_type *i5_iter_device = NULL;
	i5_dm_interface_type *pdmif = NULL;
	i5_dm_interface_type *i5_iter_ifr = NULL;
	i5_dm_bss_type *bss = NULL;
	wbd_info_t *info = NULL;
	wbd_ifr_item_t *ifr_vndr_info = NULL;
	wbd_cmd_vndr_intf_chan_info_t chan_info_msg;
	wbd_dfs_chan_info_t *dfs_chan_info = NULL;
	int ret = WBDE_OK;
	bool fronthaul_bss_configured = FALSE;
	bool free_chan_info = TRUE;

	WBD_ENTER();

	memset(&chan_info_msg, 0, sizeof(chan_info_msg));

	i5_topology = ieee1905_get_datamodel();
	if (!i5_topology) {
		WBD_ERROR("unexpected ... no topology from ieee1905_get_datamodel \n");
		goto end;
	}

	info = wbd_get_ginfo();
	/* DFS channel forced implementation for:
	 * - Topology operating in single channel mode
	 */
	if (WBD_MCHAN_ENAB(info->flags)) {
		WBD_INFO("Multi chan mode is ON, process interface's chan info"
			"only for single chan mode \n");
		goto end;
	}

	ret = wbd_tlv_decode_chan_info((void*)&chan_info_msg, tlv_data,
		tlv_data_len);
	WBD_ASSERT();

	WBD_DEBUG("chan info msg : mac["MACF"] band[%d] chan_info count[%d]\n",
		ETHERP_TO_MACF(&chan_info_msg.mac), chan_info_msg.band,
		chan_info_msg.chan_info->count);

	pdmif = wbd_ds_get_i5_interface(src_al_mac, (uchar*)&chan_info_msg.mac, &ret);
	WBD_ASSERT();

	WBD_DEBUG(" chan info rcvd for interface ["MACF"] \n", ETHERP_TO_MACF(&pdmif->InterfaceId));

	if (pdmif->band == WBD_BAND_LAN_2G) {
		WBD_ERROR("unlikely event..2G agent should not send chan info, exit \n");
		goto end;
	}
	/* dont process if fronthaul bss in not configured on interface */
	foreach_i5glist_item(bss, i5_dm_bss_type, pdmif->bss_list) {
		if (I5_IS_BSS_FRONTHAUL(bss->mapFlags)) {
			fronthaul_bss_configured = TRUE;
			break;
		}
	}
	if (!fronthaul_bss_configured) {
		WBD_DEBUG("No fronthaul interface configured, exit \n");
		goto end;
	}

	ifr_vndr_info = (wbd_ifr_item_t *)pdmif->vndr_data;

	/* wbd_ds_interface_init_cb not registered for controller, only used
	 * by Agents.
	 *
	 * For first chan info from agent's interface, neither vndr_info nor
	 * chan info exist, allocate vndr_info and use chan info from
	 * wbd_tlv_decode_chan_info and save it. Let vndr_info->chan_info
	 * point to chan_info_msg.chan_info.
	 *
	 * Dont free this chan_info_msg.chan_info, logic free this memory
	 * when same agent's interface again send chan info.
	 */
	if (!ifr_vndr_info) {
		ifr_vndr_info = (wbd_ifr_item_t *) wbd_malloc(sizeof(*ifr_vndr_info), &ret);
		WBD_ASSERT_MSG("Failed to allocate memory for ifr_vndr_data\n");
	}

	if (ifr_vndr_info->chan_info) {
		/* release earlier list and save new chan info */
		free(ifr_vndr_info->chan_info);
	}

	ifr_vndr_info->chan_info = chan_info_msg.chan_info;

	pdmif->vndr_data = ifr_vndr_info;
	free_chan_info = FALSE;

	WBD_DEBUG("pdmif band [%d] chan info [%p] \n", pdmif->band, ifr_vndr_info->chan_info);

	dfs_chan_info = wbd_master_get_common_chan_info(i5_topology, pdmif->band);
	if ((dfs_chan_info == NULL) || (dfs_chan_info->count == 0)) {
		WBD_DEBUG(" No common chan info or only one agent's chan info is present, exit\n");
		goto end;
	}

	WBD_DEBUG("send chan info, count[%d] to agent \n", dfs_chan_info->count);

	/* broadcast this dfs forced chan list to every brcm agent */
	foreach_i5glist_item(i5_iter_device, i5_dm_device_type, i5_topology->device_list) {
		if (!I5_IS_MULTIAP_AGENT(i5_iter_device->flags)) {
			continue;
		}
		foreach_i5glist_item(i5_iter_ifr, i5_dm_interface_type,
			i5_iter_device->interface_list) {

			if (!(i5_iter_ifr->isConfigured) ||
				!i5DmIsInterfaceWireless(i5_iter_ifr->MediaType)) {
				continue;
			}
			/* send to matching 5G band interfaces, not valid for 2g band */
			if ((pdmif->band) & (i5_iter_ifr->band)) {
				WBD_INFO("DFS forced channel list to: Device["MACF"] "
					"IFR["MACF"], chan[%d] band[%d]\n",
					ETHERP_TO_MACF(i5_iter_device->DeviceId),
					ETHERP_TO_MACF(i5_iter_ifr->InterfaceId),
					wf_chspec_ctlchan(i5_iter_ifr->chanspec),
					i5_iter_ifr->band);

				wbd_master_send_dfs_chan_info(dfs_chan_info, pdmif->band,
					i5_iter_device->DeviceId, i5_iter_ifr->InterfaceId);
			}
		}
	}
end:
	if (free_chan_info && (chan_info_msg.chan_info)) {
		/* May be chan info invalid or invalid interface
		 * free memory
		 */
		free(chan_info_msg.chan_info);
	}
	if (dfs_chan_info) {
		free(dfs_chan_info);
	}
	WBD_EXIT();
}

/* prepare common chan info from all agent's chan info */
static wbd_dfs_chan_info_t*
wbd_master_get_common_chan_info(i5_dm_network_topology_type *i5_topology, uint8 band)
{
	i5_dm_device_type *i5_iter_device = NULL;
	i5_dm_interface_type *i5_iter_ifr = NULL;
	wbd_ifr_item_t *ifr_vndr_info = NULL;
	wbd_dfs_chan_info_t *dfs_chan_info = NULL;
	uint8 slv_chnl = 0;
	uint8 setbitBuff[256];
	uint8 iter_setbitBuff[256];
	uint16 *pbuf = NULL;
	uint32 i = 0;
	uint32 *ptr_setbitBuff = NULL;
	uint32 *ptr_iter_setbitBuff = NULL;
	int ret = WBDE_OK;
	uint8 n_agent_found = 0;
	uint8 index = 0;

	WBD_ASSERT_ARG(i5_topology, WBDE_INV_ARG);

	pbuf = (uint16*)wbd_malloc(WBD_MAX_BUF_512, &ret);
	WBD_ASSERT();

	ptr_setbitBuff = (uint32*)&setbitBuff;
	ptr_iter_setbitBuff = (uint32*)&iter_setbitBuff;

	memset(&setbitBuff, 0xff, sizeof(setbitBuff));

	/* set bit in iter_setbitBuff corresponding to each channel number in chan info of
	 * each device's agent interface matching with input band. Compare every iteration
	 * bitmap with global bitbuff i.e. setbitBuff.
	 *
	 * pbuf holds the information of each channel bitmap present in channel info of
	 * agent. Every iteration validates new bitmap with existing bitmap present
	 * for the channel. If not same clear the channel bit from iter_setbitBuff.
	 *
	 */
	foreach_i5glist_item(i5_iter_device, i5_dm_device_type, i5_topology->device_list) {
		if (!I5_IS_MULTIAP_AGENT(i5_iter_device->flags)) {
			continue;
		}
		memset(&iter_setbitBuff, 0x00, sizeof(iter_setbitBuff));

		foreach_i5glist_item(i5_iter_ifr, i5_dm_interface_type,
			i5_iter_device->interface_list) {

			if (!i5DmIsInterfaceWireless(i5_iter_ifr->MediaType) ||
				(i5_iter_ifr->band != band) || !(i5_iter_ifr->isConfigured)) {
				continue;
			}
			ifr_vndr_info = (wbd_ifr_item_t *)i5_iter_ifr->vndr_data;

			if (!ifr_vndr_info || !(ifr_vndr_info->chan_info)) {
				WBD_INFO("Interface's vndr, chan info band[%d] not initialized\n",
					i5_iter_ifr->band);
				break; /* look for other device */
			}

			WBD_DEBUG("interface["MACF"] chan count[%d] \n",
				ETHERP_TO_MACF(&i5_iter_ifr->InterfaceId),
				ifr_vndr_info->chan_info->count);

			for (i = 0; i < ifr_vndr_info->chan_info->count; i++) {
				slv_chnl = ifr_vndr_info->chan_info->chinfo[i].channel;
				setbit(&iter_setbitBuff, slv_chnl);

				WBD_DEBUG("set bit in iter_bitbuff for channel[%d] \n", slv_chnl);

				if (n_agent_found == 0) {
					pbuf[slv_chnl] |=
						ifr_vndr_info->chan_info->chinfo[i].bitmap;

					WBD_DEBUG("chan[%d] first agent's bitmap[%x] \n", slv_chnl,
						ifr_vndr_info->chan_info->chinfo[i].bitmap);
				} else {
					/* compare already stored bitmap in pbuf[slv_chnl] with new
					 * bitmap. If different, ignore current channel
					 */
					uint16 bitmap = pbuf[slv_chnl];

					WBD_DEBUG("chan[%d] bitmap[%x] compare with existing"
						"bitmap [%x] \n", slv_chnl, bitmap,
						ifr_vndr_info->chan_info->chinfo[i].bitmap);

					if (bitmap != ifr_vndr_info->chan_info->chinfo[i].bitmap) {
						WBD_DEBUG("clear bit for channel[%d] as bitmap[%x]"
							"is different than previous bitmap\n",
							slv_chnl, bitmap);

						clrbit(&iter_setbitBuff, slv_chnl);
					}
				}
			}
			n_agent_found++;
		}
		for (i = 0; i < (sizeof(setbitBuff)/4); i++) {
			/* save agent iter_setbitBuff chan info in setbitBuff */
			ptr_setbitBuff[i] &= ptr_iter_setbitBuff[i];
		}
	}

	WBD_DEBUG("total agents found[%d] \n", n_agent_found);
	if (n_agent_found <= 1) {
		/* intersection is not possible, return */
		goto end;
	}

	WBD_DEBUG(" create chan info from setbit \n");
	/* use setbitBuff */
	dfs_chan_info = (wbd_dfs_chan_info_t*)wbd_malloc(sizeof(wbd_dfs_chan_info_t),
		&ret);
	WBD_ASSERT();

	for (i = 0; i <= MAX_5G_CONTROL_CHANNEL; i++) {
		if (!isset(&setbitBuff, i)) {
			continue;
		}
		dfs_chan_info->channel[index] = i;
		dfs_chan_info->count++;
		WBD_DEBUG("DFS common chan [%d] \n", dfs_chan_info->channel[index]);
		index++;
	}
	WBD_DEBUG("total chann info count [%d] \n", dfs_chan_info->count);
end:
	if (pbuf) {
		free(pbuf);
	}
	return dfs_chan_info;
}
#endif /* !MULTIAPR2 */

#if defined(MULTIAPR2)
static int
wbd_master_process_mbo_get_count_chan_from_non_pref_list(uint8* ibuf, uint8 ibuf_len, bool attr)
{
	uint8 *ptr = ibuf;
	uint8 len = 0, ptr_len = ibuf_len;
	uint8 empty_list_len = 0;

	WBD_ENTER();

	if (!ptr) {
		WBD_ERROR("NULL data pointer : \n");
		goto end;
	}
	ptr++;
	ptr_len--;
	len = *ptr;

	if (ptr_len < len) {
		WBD_ERROR("ibuf_len not matching np_chan_rpt_len : \n");
		goto end;
	}

	if (attr) {
		/* if len == 0, No chan list if present in this Attribute
		 * return
		 */
		if (len != 0) {
			empty_list_len = (len - WBD_MBO_NP_ATTR_FIXED_LEN);
		}
	} else {
		/* possible values in length byte:
		 * 0x04 or variable
		 * 0x04 - No chan list provided, empty element return with 0 len
		 */
		if (ibuf[WBD_MBO_ATTRIBUTE_LEN_OFFSET] > WBD_MBO_EMPTY_SUBELEMENT_LIST_LEN) {
			empty_list_len = (len - WBD_MBO_NP_SUBELEMENT_FIXED_LEN);
		}
	}
end:
	WBD_EXIT();
	return empty_list_len;
}

static int
wbd_master_process_update_mbo_np_chan_list(wbd_assoc_sta_item_t *sta, uint8* ibuf,
	uint8 ibuf_len, bool attr, uint8 chan_list_len)
{
	wbd_sta_mbo_np_chan_list_t_g *new_channel_list;
	uint8 *ptr = ibuf;
	uint8 i, len = 0, ptr_len = ibuf_len;
	int ret = WBDE_OK;

	WBD_ENTER();

	new_channel_list = (wbd_sta_mbo_np_chan_list_t_g*)
				wbd_malloc(sizeof(wbd_sta_mbo_np_chan_list_t_g), &ret);
	if (!new_channel_list) {
		WBD_ERROR("out of mem : \n");
		ret = WBDE_MALLOC_FL;
		goto end;
	}
	new_channel_list->list = (uint8*) wbd_malloc(chan_list_len * sizeof(uint8), &ret);

	if (!new_channel_list->list) {
		free(new_channel_list);
		WBD_ERROR("out of mem : \n");
		ret = WBDE_MALLOC_FL;
		goto end;
	}
	/*  Non preferred chan attribute report information
	 *  --------------------------------------------------------------------------------
	 *  attribute_id | length | operating class | channel list | preference | reason code
	 *   1 byte        1 byte    1 byte              variable     1 byte       1 byte
	 *  --------------------------------------------------------------------------------
	 */
	/*  Non preferred subelement information (*number of bytes)
	 *  --------------------------------------------------------------------------------
	 *    id(1) | length(1) | OUI(3)  | OUI TYPE(1) | operating class(1) | channel list(VAR)
	 *	| preference(1) | reason code(1)
	 *  --------------------------------------------------------------------------------
	 */
	ptr++;
	ptr_len--;
	len = *ptr;

	if (ptr_len < len) {
		WBD_ERROR("ibuf_len not matching np_chan_rpt_len : \n");
		ret = WBDE_FAIL_XX;
		goto end;
	}

	if (attr) {
		/* Update Opclass from Non preferred chan Attribute */
		ptr++;
		if (len) {
			new_channel_list->opclass = *ptr;
			len--;
		}
	} else {
		/* update Opclass from Non preferred chan Subelement */
		ptr = ptr + WBD_MBO_NP_SUBELEMENT_OPCLASS_OFFSET;
		len = len - (WBD_MBO_OUI_LEN + WBD_MBO_OUI_TYPE_LEN);
		if (len) {
			new_channel_list->opclass = *ptr;
			len--;
		}
	}
	ptr++;
	if (chan_list_len > 0 && chan_list_len <= WBD_MBO_CHAN_LIST) {
		/* Store chan list_len to list_len for the number of channels */
		new_channel_list->list_len = chan_list_len;
		for (i = 0; i < chan_list_len; i++) {
			if (len) {
				new_channel_list->list[i] = *ptr++;
				len--;
			}
		}
	}
	if (len) {
		new_channel_list->pref = *ptr++;
		len--;
	}
	if (len) {
		new_channel_list->reason = *ptr;
		len--;
	}

	wbd_ds_glist_append(&sta->np_chan_entry_list, (dll_t *)new_channel_list);

	WBD_INFO("STA Non preferred entry list count [%d] ibuf_len %d\n",
		sta->np_chan_entry_list.count, ibuf_len);
end:
	WBD_EXIT();
	return ret;
}

static int
wbd_master_process_mbo_np_chan_list(wbd_assoc_sta_item_t *sta, uint8* ibuf,
	uint8 ibuf_len, bool attr)
{
	uint8 chan_list_len = 0;
	int ret = WBDE_FAIL_XX;

	WBD_ENTER();

	chan_list_len = wbd_master_process_mbo_get_count_chan_from_non_pref_list(ibuf,
		ibuf_len, attr);
	if (chan_list_len <= 0) {
		/* It is possible to receive Attribute or subelement
		 * having zero chan list, skip further processing
		 */
		goto end;
	}

	/* Include ID and len of element for debug */
	prhex(" MBO Non preferred element data  ==>",
		ibuf, (ibuf[WBD_MBO_ATTRIBUTE_LEN_OFFSET] + 2));

	ret = wbd_master_process_update_mbo_np_chan_list(sta, ibuf, ibuf_len,
		attr, chan_list_len);
end:
	WBD_EXIT();
	return ret;
}

static int
wbd_master_process_wnm_request_mbo_sta(unsigned char *al_mac, unsigned char *sta_mac,
	uint8 *data, uint8 body_len)
{
	int ret = WBDE_OK;
	i5_dm_device_type *i5_self_device = NULL;
	i5_dm_clients_type *sta = NULL;
	wbd_assoc_sta_item_t *assoc_sta = NULL;
	dot11_wnm_notif_req_t *wnm_notif;
	uint8* ptr = NULL;
	uint8 bytes_rd = 0, nbytes = 0, len = 0, ptr_len = 0;
	uint8 oui_type_offset = 0;
	bool do_wnm_chan_list_cleanup = TRUE;

	WBD_ENTER();

	/* Get device to find STA entry in assoclist */
	WBD_SAFE_FIND_I5_DEVICE(i5_self_device, al_mac, &ret);

	sta = wbd_ds_find_sta_in_device(i5_self_device,	sta_mac, &ret);
	WBD_ASSERT_MSG("STA["MACDBG"]. %s\n", MAC2STRDBG(sta_mac), wbderrorstr(ret));
	WBD_ASSERT_ARG(sta->vndr_data, WBDE_INV_ARG);

	assoc_sta = (wbd_assoc_sta_item_t*)sta->vndr_data;
	/* check body_len to verify it has minimum wnm notify req len */
	if (body_len < DOT11_WNM_NOTIF_REQ_LEN) {
		WBD_ERROR("WNM notification request frame with invalid length\n");
		ret = WBDE_INV_ARG;
		goto end;
	}

	wnm_notif = (dot11_wnm_notif_req_t*)data;
	ptr = &(wnm_notif->data[WBD_MBO_ATTRIBUTE_ID_OFFSET]);
	oui_type_offset = WBD_MBO_ATTRIBUTE_OUI_OFFSET + WFA_OUI_LEN;
	/* body_len is whole notification request frame len, update body_len to
	 * have only MBO specific number of bytes
	 */
	body_len = body_len - DOT11_WNM_NOTIF_REQ_LEN;
	/* parse number of subelements if present */
	while ((body_len - bytes_rd) > (WBD_MBO_WNM_NOTIFICATION_MIN_SUBELEMENT_LEN
			+ WFA_OUI_LEN)) {
		/* Confirm WFA OUI tag 0X50,0X6F,0X9A */
		if (memcmp(&ptr[WBD_MBO_ATTRIBUTE_OUI_OFFSET], WFA_OUI, WFA_OUI_LEN) != 0) {
			return BCME_IE_NOTFOUND;
		}
		if (ptr[oui_type_offset] == WBD_MBO_NP_SUBELEMENT_CHAN_OUI_TYPE) {
			/* WFA MBO standard(3.2) Tech spec
			 * Every time an MBO STA informs an MBO AP of its channel and
			 * band preferences, either via the inclusion of at least one
			 * Non-preferred Channel Report Attribute in a (Re)Association
			 * frame or the inclusion of at least one Non-preferred Channel
			 * Report Subelement in a WNM-Notification Request frame, the MBO
			 * AP shall replace all (if any) previously stored information
			 * (irrespective of Operating Class) with the most current
			 * information as indicated by the MBO STA
			 */
			if (do_wnm_chan_list_cleanup) {
				wbd_ds_glist_cleanup(&assoc_sta->np_chan_entry_list);
				wbd_ds_glist_init(&assoc_sta->np_chan_entry_list);
				do_wnm_chan_list_cleanup = FALSE;
			}
			ptr_len = body_len - bytes_rd;
			ret = wbd_master_process_mbo_np_chan_list(assoc_sta, ptr,
				ptr_len, FALSE);
			if (ret != WBDE_OK) {
				goto end;
			}
		}
		/* continue to look out for more subelements if any in WNM notification frame */
		len = ptr[WBD_MBO_ATTRIBUTE_LEN_OFFSET];
		nbytes = (len + WBD_MBO_WNM_SUBELEMENT_ID_AND_LEN);
		ptr += nbytes;
		bytes_rd += nbytes;
	}
end:
	WBD_EXIT();
	return ret;
}

static int
wbd_master_process_assoc_request_mbo_sta(unsigned char *al_mac, unsigned char *sta_mac,
	uint8 *data, uint32 datalen, bool is_reassoc)
{
	int ret = WBDE_OK;
	i5_dm_device_type *i5_self_device = NULL;
	i5_dm_clients_type *sta = NULL;
	wbd_assoc_sta_item_t *assoc_sta = NULL;
	uint8* ptr = NULL;
	uint8 bytes_rd = 0, nbytes = 0, len = 0, ptr_len = 0;
	bool do_assoc_chan_list_cleanup = TRUE;
	uint8 ie_type = WFA_OUI_TYPE_MBO;
	bcm_tlv_t *mbo_ie = NULL;
	uint8 *ies, body_len;
	uint32 ies_len;

	WBD_ENTER();

	/* Get device to find STA entry in assoclist */
	WBD_SAFE_FIND_I5_DEVICE(i5_self_device, al_mac, &ret);

	sta = wbd_ds_find_sta_in_device(i5_self_device,	sta_mac, &ret);
	WBD_ASSERT_MSG("STA["MACDBG"]. %s\n", MAC2STRDBG(sta_mac), wbderrorstr(ret));
	WBD_ASSERT_ARG(sta->vndr_data, WBDE_INV_ARG);

	assoc_sta = (wbd_assoc_sta_item_t*)sta->vndr_data;
	/* From tunneled message type get assoc type for parsing ie in an (re)assoc frame */
	if (is_reassoc) {
		ies = data + DOT11_REASSOC_REQ_FIXED_LEN;
		ies_len = datalen - DOT11_REASSOC_REQ_FIXED_LEN;
	} else {
		ies = data + DOT11_ASSOC_REQ_FIXED_LEN;
		ies_len = datalen - DOT11_ASSOC_REQ_FIXED_LEN;
	}

	/* Match Vendor Specific ID with WFA OUI Type MBO */
	mbo_ie = wbd_wl_find_ie(DOT11_MNG_VS_ID, ies, ies_len,
		WFA_OUI, &ie_type, 1);
	/* check ie len to verfiy minimum mbo non preferred channel report attribute req len */
	if (mbo_ie && mbo_ie->len > MIN_MBO_IE_LEN && mbo_ie->len <= ies_len) {
		ptr = &(mbo_ie->data[WBD_MBO_ASSOC_ATTRIBUTE_ID_OFFSET]);
		/* body_len is whole mbo non preferred frame len, update body_len to
		 * have only MBO specific number of bytes
		 */
		body_len = mbo_ie->len;
		body_len = mbo_ie->len - (WFA_OUI_LEN + WBD_MBO_OUI_TYPE_LEN);

		/* parse number of subelements if present */
		while ((body_len - bytes_rd) > (WBD_MBO_ASSOC_MIN_SUBELEMENT_LEN
				+ WFA_OUI_LEN) && ((body_len - bytes_rd) <= ies_len)) {
			if (ptr[WBD_MBO_ATTRIBUTE_ID_OFFSET] ==
				WBD_MBO_NP_SUBELEMENT_CHAN_OUI_TYPE) {
				/* WFA MBO standard(3.2) Tech spec
				 * Every time an MBO STA informs an MBO AP of its channel and
				 * band preferences, either via the inclusion of at least one
				 * Non-preferred Channel Report Attribute in a (Re)Association
				 * frame or the inclusion of at least one Non-preferred Channel
				 * Report Subelement in a WNM-Notification Request frame, the MBO
				 * AP shall replace all (if any) previously stored information
				 * (irrespective of Operating Class) with the most current
				 * information as indicated by the MBO STA
				 */
				if (do_assoc_chan_list_cleanup) {
					wbd_ds_glist_cleanup(&assoc_sta->np_chan_entry_list);
					wbd_ds_glist_init(&assoc_sta->np_chan_entry_list);
					do_assoc_chan_list_cleanup = FALSE;
				}
				ptr_len = body_len - bytes_rd;
				ret = wbd_master_process_mbo_np_chan_list(assoc_sta, ptr,
					ptr_len, TRUE);
				if (ret != WBDE_OK) {
					goto end;
				}
			}
			/* continue to look out for more subelements if any in mbo ie frame */
			len = ptr[WBD_MBO_ATTRIBUTE_LEN_OFFSET];
			nbytes = (len + WBD_MBO_ASSOC_SUBELEMENT_ID_AND_LEN);
			ptr += nbytes;
			bytes_rd += nbytes;
		}
	}

end:
	WBD_EXIT();
	return ret;
}

/* Process tunnel message based on message type */
void wbd_master_process_tunneled_msg(wbd_info_t *info, unsigned char *al_mac,
	ieee1905_tunnel_msg_t *msg)
{
	WBD_ENTER();

	switch (msg->payload_type) {
		case ieee1905_tunnel_msg_payload_wnm_rqst:
		{
			wbd_master_process_wnm_request_mbo_sta(al_mac,
				msg->source_mac, (uint8 *)msg->payload,
				(uint8) msg->payload_len);
			break;
		}
		case ieee1905_tunnel_msg_payload_assoc_rqst:
		case ieee1905_tunnel_msg_payload_re_assoc_rqst:
		{
			bool is_reassoc = FALSE;
			WBD_INFO("payload type[%d] with tunnel msg from sta["MACF"]"
				"payload_len[%d]\n", msg->payload_type,
				ETHERP_TO_MACF(msg->source_mac), msg->payload_len);

			if (msg->payload_type == ieee1905_tunnel_msg_payload_re_assoc_rqst) {
				is_reassoc = TRUE;
			}

			wbd_master_process_assoc_request_mbo_sta(al_mac,
				msg->source_mac, (uint8 *)msg->payload,
				msg->payload_len, is_reassoc);
			break;
		}
		case ieee1905_tunnel_msg_payload_btm_query:
		case ieee1905_tunnel_msg_payload_anqp_rqst:
		{
			/* TODO: how to use this information ... */
			WBD_INFO("payload type[%d] with tunnel msg from sta["MACF"]"
				"payload_len[%d] \n", msg->payload_type,
				ETHERP_TO_MACF(msg->source_mac), msg->payload_len);
			break;
		}
		default:
		{
			WBD_ERROR("unknown payload type[%d] with tunnel msg from sta["MACF"]"
				" with payload_len[%d] \n", msg->payload_type,
				ETHERP_TO_MACF(msg->source_mac), msg->payload_len);
		}
	}
	WBD_EXIT();
}

/* API to forward Channel Scan Report came from Agent to ACSD for improved Channel Selection */
static int
wbd_master_fwd_channelscan_rpt_to_acsd(unsigned char *src_al_mac,
	ieee1905_chscan_result_item *emt_p)
{
	int ret = WBDE_OK, cli_ver = 1;
	wbd_info_t *info = NULL;
	wbd_com_handle *com_hndl = NULL;
	char tmpbuf[WBD_MAX_BUF_512] = {0};
	char s_src_al_mac[WBD_STR_MAC_LEN] = {0}, s_radio_mac[WBD_STR_MAC_LEN] = {0};
	int sock_options = 0x0000;
	WBD_ENTER();

	/* Validate arg */
	WBD_ASSERT_ARG(emt_p, WBDE_INV_ARG);

	info = wbd_get_ginfo();

	WBD_DEBUG("IFR[%s] Send ChannelScan Report from Device["MACF"] "
		"Radio_Mac["MACF"] Chanspec[0x%x] to ACSD.\n",
		emt_p->ifname, ETHERP_TO_MACF(src_al_mac),
		ETHERP_TO_MACF(emt_p->radio_mac), emt_p->chanspec_20);

	wbd_ether_etoa(src_al_mac, s_src_al_mac);
	wbd_ether_etoa(emt_p->radio_mac, s_radio_mac);

	snprintf(tmpbuf, sizeof(tmpbuf), "set&ifname=%s&param=channelscanrpt&value=%d&devid=%s"
		"&radiomac=%s&chspec20=%d&nctrl=%d&next20=%d&next40=%d&next80=%d&util=%d&noise=%d",
		emt_p->ifname, cli_ver, s_src_al_mac, s_radio_mac, emt_p->chanspec_20,
		emt_p->pry_info.n_ctrl, emt_p->pry_info.n_ext20,
		emt_p->pry_info.n_ext40, emt_p->pry_info.n_ext80,
		emt_p->utilization, emt_p->noise);

	sock_options = WBD_COM_FLAG_CLIENT | WBD_COM_FLAG_BLOCKING_SOCK
			| WBD_COM_FLAG_NO_RESPONSE;

	com_hndl = wbd_com_init(info->hdl, INVALID_SOCKET, sock_options, NULL, NULL, info);

	if (!com_hndl) {
		WBD_ERROR("IFR[%s] Failed to initialize the communication module to forward "
			"ChannelScan Report from Device["MACF"] Chanspec[0x%x] to ACSD.\n",
			emt_p->ifname, ETHERP_TO_MACF(src_al_mac), emt_p->chanspec_20);
		ret = WBDE_COM_ERROR;
		goto end;
	}

	/* Send the command to ACSD */
	ret = wbd_com_connect_and_send_cmd(com_hndl, ACSD_DEFAULT_CLI_PORT,
		WBD_LOOPBACK_IP, tmpbuf, NULL);
	WBD_CHECK_MSG("IFR[%s] Failed to forward "
		"ChannelScan Report from Device["MACF"] Chanspec[0x%x] to ACSD. Error : %s\n",
			emt_p->ifname, ETHERP_TO_MACF(src_al_mac),
			emt_p->chanspec_20, wbderrorstr(ret));

end:
	if (com_hndl) {
		wbd_com_deinit(com_hndl);
	}

	WBD_EXIT();
	return ret;
}

/* Average Stats of All Existing Repeaters for same 20 MHz Channel, and Send it to ACSD */
int
wbd_master_average_and_send_pry_info_to_acsd(unsigned char *src_al_mac,
	i5_dm_device_type *leaving_dev, ieee1905_chscan_report_msg *chscan_rpt)
{
	int ret = WBDE_OK, eo_report;
	float f_rep_count, f_util, f_noise, f_ctrl, f_ext20, f_ext40, f_ext80;
	ieee1905_chscan_result_item *emt_p, *per_dev_p, *ave_p, *next_p;
	i5_dm_network_topology_type *i5_topology;
	i5_dm_device_type *i5_iter_device, *ctrlr_dev;
	WBD_ENTER();

	i5_topology = ieee1905_get_datamodel();
	if (!i5_topology) {
		WBD_ERROR("unexpected ... no topology from ieee1905_get_datamodel \n");
		goto end;
	}
	WBD_SAFE_GET_I5_SELF_DEVICE(ctrlr_dev, &ret);

	WBD_DEBUG("Average ChannelScan Stats of All Existing Repeaters for same 20MHz Channel\n");

	/* For each Channel Scan Result Item */
	foreach_safe_iglist_item(emt_p, ieee1905_chscan_result_item,
		chscan_rpt->chscan_result_list, next_p) {

		eo_report = 0, f_rep_count = 0, f_util = 0, f_noise = 0;
		f_ctrl = 0, f_ext20 = 0, f_ext40 = 0, f_ext80 = 0;

		/* Fetch next item, before any continue */
		next_p = ((ieee1905_chscan_result_item *)dll_next_p((dll_t*)(emt_p)));

		/* Do below activity only for Valid SRC Channel Scan Result Items */
		if ((strlen(emt_p->ifname) <= 0) ||
			(emt_p->scan_status_code != MAP_CHSCAN_STATUS_SUCCESS)) {
			WBD_INFO("Chan[%d] IF[%s] not Valid on Device["MACF"] Or "
				"Scan Result Code[%d] != Success.\n",
				emt_p->channel, emt_p->ifname, ETHERP_TO_MACF(src_al_mac),
				emt_p->scan_status_code);
			continue;
		}

		/* If next item = NULL or IFname changed; Report Ended for this Radio */
		if (!next_p || (strcmp(emt_p->ifname, next_p->ifname) != 0)) {
			eo_report = 1;
		}

		/* Loop for all the Existing Devices in Topology */
		foreach_i5glist_item(i5_iter_device, i5_dm_device_type, i5_topology->device_list) {

			/* Do below activity only for Agent Devices of Repeaters */
			if (I5_IS_MULTIAP_CONTROLLER(i5_iter_device->flags) ||
				WBD_ONBOOT_NOT_FORCED_ON_CTRLAGENT(i5_iter_device->flags) ||
				/* Ignore Device getting LEAVE */
				((leaving_dev != NULL) &&
				eacmp(i5_iter_device->DeviceId, leaving_dev->DeviceId) == 0)) {
				continue;
			}

			/* Find Valid Channel Scan Result on Each Repeater Dev, to DO Average */
			per_dev_p = i5DmFindChannelInScanResult(
				&i5_iter_device->stored_chscan_results,
				emt_p->opclass, emt_p->channel);
			if (!per_dev_p || (strlen(per_dev_p->ifname) <= 0) ||
				(strcmp(emt_p->ifname, per_dev_p->ifname) != 0) ||
				(per_dev_p->scan_status_code != MAP_CHSCAN_STATUS_SUCCESS)) {
				WBD_DEBUG("Chan[%d] Chan_IF[%s] per_dev_IF[%s] not Valid on "
					"Device["MACF"] Or Scan Result Code[%d] != Success.\n",
					emt_p->channel, emt_p->ifname,
					(per_dev_p ? per_dev_p->ifname : ""),
					ETHERP_TO_MACF(i5_iter_device->DeviceId),
					(per_dev_p ? per_dev_p->scan_status_code : -1));
				continue;
			}

			/* Add Each Rep pry_info, util, noise to Average, rep_count++ to devide */
			f_util  += per_dev_p->utilization;
			f_noise += per_dev_p->noise;
			f_ctrl  += per_dev_p->pry_info.n_ctrl;
			f_ext20 += per_dev_p->pry_info.n_ext20;
			f_ext40 += per_dev_p->pry_info.n_ext40;
			f_ext80 += per_dev_p->pry_info.n_ext80;
			f_rep_count++;
		}

		/* Avoid Divide by 0 Error */
		if (f_rep_count == 0) {
			WBD_INFO("Chan[%d] not present on all Existing Repeaters. "
				"Skip this Channel for Averaging & Sending to ACSD.\n",
				emt_p->channel);
			continue;
		}

		/* Find Valid Channel Scan Result on Controller Device, to STORE Average Stats */
		ave_p = i5DmFindChannelInScanResult(&ctrlr_dev->stored_chscan_results,
			emt_p->opclass, emt_p->channel);
		if (!ave_p) {
			WBD_DEBUG("Chan[%d] not present on Controller Device."
				"Appended.\n", emt_p->channel);
			/* Create Empty Channel Scan Result Item as UnComplete,
			 * and Append it to Controller's Channel Scan Result List
			 */
			ave_p = i5DmAppendEmptyChScanResult(emt_p->radio_mac, emt_p->opclass,
				emt_p->channel, MAP_CHSCAN_STATUS_UNCOMPLETE,
				&ctrlr_dev->stored_chscan_results);
		}

		/* Find & STORE Average pry_info, util, noise, deviding by rep_count */
		ave_p->utilization	= (uint8)(f_util /f_rep_count);
		ave_p->noise	        = (uint8)(f_noise/f_rep_count);
		ave_p->pry_info.n_ctrl  = (uint8)(f_ctrl /f_rep_count);
		ave_p->pry_info.n_ext20 = (uint8)(f_ext20/f_rep_count);
		ave_p->pry_info.n_ext40 = (uint8)(f_ext40/f_rep_count);
		ave_p->pry_info.n_ext80 = (uint8)(f_ext80/f_rep_count);

		/* Update radio_mac = FFs to indicate End of ChScan Report, 00s otherwise */
		eacopy((unsigned char*)(eo_report ? &ether_bcast : &ether_null), ave_p->radio_mac);
		memcpy(ave_p->ifname, emt_p->ifname, I5_MAX_IFNAME);
		ave_p->scan_status_code = MAP_CHSCAN_STATUS_SUCCESS;
		ave_p->chanspec_20 = emt_p->chanspec_20;

		WBD_DEBUG("Sending Average Stats of Tot_Rep #[%u] : IF[%s] Ch[%d] Ch_20[0x%X] : "
			"Util[%u] Noise[%u] nCtrl[%u] nExt20[%u] nExt40[%u] nExt80[%u]\n",
			(uint8)f_rep_count, ave_p->ifname, ave_p->channel, ave_p->chanspec_20,
			ave_p->utilization, ave_p->noise, ave_p->pry_info.n_ctrl,
			ave_p->pry_info.n_ext20, ave_p->pry_info.n_ext40, ave_p->pry_info.n_ext80);

		/* Fwd Channel Scan Report came from Agent to ACSD for improved Chan Selection */
		wbd_master_fwd_channelscan_rpt_to_acsd(src_al_mac, ave_p);
	}
end:
	WBD_EXIT();
	return ret;
}

/* Update pry_info for that Radio's ChScan Results */
static int
wbd_master_update_channel_scan_pry_info(char* ifname, chanspec_t *chan_list, int chan_count,
	ieee1905_chscan_report_msg *chscan_rpt)
{
	int ret = WBDE_OK, iter_pry = 0;
	ieee1905_chscan_result_item *emt_p = NULL;
	ieee1905_chan_pry_t chan;
	ieee1905_chan_pry_t *chan_p = &chan;
	chanspec_t cur_chspec = 0;
	ieee1905_chan_pry_info_t *pry_list = NULL;
	WBD_ENTER();

	/* Validate arg */
	WBD_ASSERT_ARG(chan_list, WBDE_INV_ARG);
	WBD_ASSERT_ARG(chan_count, WBDE_INV_ARG);
	WBD_ASSERT_ARG(chscan_rpt, WBDE_INV_ARG);

	pry_list = (ieee1905_chan_pry_info_t *) wbd_malloc(
		(sizeof(ieee1905_chan_pry_info_t) * chan_count), &ret);

	/* For each Pry_info in List */
	for (iter_pry = 0; iter_pry < chan_count; iter_pry ++) {

		bzero(&pry_list[iter_pry], sizeof(ieee1905_chan_pry_info_t));

		/* Set channel range centered by the scan channel */
		pry_list[iter_pry].channel = CHSPEC_CHANNEL(chan_list[iter_pry]);
		WBD_DEBUG("IFR[%s] Channel[%u]: Chan_Count[%d] ",
			ifname, pry_list[iter_pry].channel, iter_pry + 1);

		/* For each Channel Scan Result Item */
		foreach_iglist_item(emt_p, ieee1905_chscan_result_item,
			chscan_rpt->chscan_result_list) {

			ieee1905_chscan_result_nbr_item *emt_nbr_p = NULL;

			/* Skip Results which are not for this radio, or has # of neighbors = 0 */
			if ((strcmp(emt_p->ifname, ifname) != 0) ||
				(emt_p->num_of_neighbors == 0)) {
				continue;
			}

			/* For each Neighbor in this Channel Scan Result Item */
			foreach_iglist_item(emt_nbr_p, ieee1905_chscan_result_nbr_item,
				emt_p->neighbor_list) {

				/* Parse Chanspec For Pry Info pointer */
				cur_chspec = wf_channel2chspec(emt_p->channel,
					i5DmGetBandWidthFromChScanStr((char *)emt_nbr_p->ch_bw),
					blanket_opclass_to_band(emt_p->opclass));
				i5DmParseChanspecForPry(cur_chspec, chan_p);

				/* Find and Increment Pry Counts for this Channel */
				i5DmIncrementPryCount(&pry_list[iter_pry],
					chan_p, pry_list[iter_pry].channel);

			}
		}

		WBD_DEBUG("nCtrl[%u] nExt20[%u] nExt40[%u] nExt80[%u]\n",
			pry_list[iter_pry].n_ctrl, pry_list[iter_pry].n_ext20,
			pry_list[iter_pry].n_ext40, pry_list[iter_pry].n_ext80);
	}

	/* For each Channel Scan Result Item */
	iter_pry = 0;
	foreach_iglist_item(emt_p, ieee1905_chscan_result_item, chscan_rpt->chscan_result_list) {

		/* Skip Results which are not for this radio */
		if (strcmp(emt_p->ifname, ifname) != 0) {
			continue;
		}

		/* Make Sure, we are going to Update Pry Info of Correct ChScan Result Item */
		if (emt_p->channel != pry_list[iter_pry].channel) {
			WBD_DEBUG("Error : IFR[%s] Channel in ChScan Result[%d] != "
				"Channel in Pry_List[%d]\n",
				ifname, emt_p->channel, pry_list[iter_pry].channel);
			ret = WBDE_INV_ARG;
			goto end;
		}

		/* Store Updated Pry_info in this ChScan Result Item, Get Next Updated Pry Info */
		memcpy(&emt_p->pry_info, &pry_list[iter_pry], sizeof(emt_p->pry_info));
		iter_pry++;
	}

end:
	if (pry_list) {
		free(pry_list);
	}
	WBD_EXIT();
	return ret;
}

/* Controller Find Interface on its device, Matching the band */
static int
wbd_master_get_ifname_matching_band(int in_band_flag, char *out_ifname, int out_ifname_size)
{
	int ret = WBDE_INV_IFNAME;
	i5_dm_device_type *ctrlagent_dev;
	i5_dm_interface_type *iter_ifr, *self_ifr;
	WBD_ENTER();

	WBD_SAFE_GET_I5_CTRLAGENT_DEVICE(ctrlagent_dev, &ret);

	/* Traverse in Controller Agent Device, look for same band */
	foreach_i5glist_item(iter_ifr, i5_dm_interface_type, ctrlagent_dev->interface_list) {

		if ((iter_ifr->chanspec == 0) || !i5DmIsInterfaceWireless(iter_ifr->MediaType)) {
			continue;
		}
		if (!(in_band_flag & iter_ifr->band)) {
			WBD_DEBUG("Requested band=%d does not match with Controller Agent's"
				"interface band=%d, look for other interface.\n",
				in_band_flag, iter_ifr->band);

			continue;
		}

		/* If same band FOUND, Look for this IFR MAC in Self Device */
		WBD_SAFE_GET_I5_SELF_IFR(iter_ifr->InterfaceId, self_ifr, &ret);

		/* From Self IFR, Get the ifname */
		if (out_ifname) {
			WBDSTRNCPY(out_ifname, self_ifr->ifname, out_ifname_size);
		}
		ret = WBDE_OK;
		goto end;
	}

end:
	WBD_EXIT();
	return ret;
}

/* Processes Onboot Channel Scan Report by Controller */
static void
wbd_master_process_onboot_channel_scan_rpt(unsigned char *src_al_mac, time_t ts_chscan_rpt,
	ieee1905_chscan_report_msg *chscan_rpt)
{
	int ret = WBDE_OK, band_flag = BAND_INV, prev_band_flag = BAND_INV;
	ieee1905_chscan_result_item *emt_p = NULL;
	char ssidbuf[SSID_FMT_BUF_LEN] = "", ifname[IFNAMSIZ + 2] = {0};
	char ifname_list[WBD_MAX_BUF_128] = {0}, name[NVRAM_MAX_VALUE_LEN] = {0}, *next = NULL;
	i5_dm_device_type *i5_device = NULL;
	WBD_ENTER();

	WBD_SAFE_FIND_I5_DEVICE(i5_device, src_al_mac, &ret);

	WBD_DEBUG("Onboot Channel Scan Report Received. Process it further.\n");

	/* Extract details for each Channel Scan Result Item */
	foreach_iglist_item(emt_p, ieee1905_chscan_result_item, chscan_rpt->chscan_result_list) {

		ieee1905_chscan_result_nbr_item *emt_nbr_p = NULL;
		band_flag = BAND_INV;

		/* Extract Radio Unique Identifier of a radio of the Multi-AP Agent */
		WBD_DEBUG("IFR["MACF"] ", ETHERP_TO_MACF(emt_p->radio_mac));

		/* Extract Operating Class */
		WBD_DEBUG("OpClass = [%d] ", emt_p->opclass);

		/* Extract Channel */
		emt_p->chanspec_20 = CH20MHZ_CHSPEC(emt_p->channel,
			blanket_opclass_to_band(emt_p->opclass));
		WBD_DEBUG("Channel = [%d] Chanspec_20 = [0x%X] ",
			emt_p->channel, emt_p->chanspec_20);

		/* Get IEEE1905 Band Enumeration from Input Channel */
		band_flag |= ieee1905_get_band_from_channel(emt_p->opclass, emt_p->channel);

		/* Check if Previous Result's Band flag and Ifname are same as Current */
		if (prev_band_flag != band_flag) {

			/* If not, Controller Find Interface on its device, Matching the band */
			ret = wbd_master_get_ifname_matching_band(band_flag,
				ifname, sizeof(ifname));
			if ((strlen(ifname) <= 0) || (ret != WBDE_OK)) {
				WBD_ERROR("Chan[%d] Controller couldn't find matching "
					"Ifname. Do not Process it further.\n", emt_p->channel);
				goto end;
			}

			/* Add new ifname to Ifname List, Update Prev band_flag as Current */
			add_to_list(ifname, ifname_list, sizeof(ifname_list));
			prev_band_flag = band_flag;
		}

		/* Save Ifname to ChScan Result Item */
		WBDSTRNCPY(emt_p->ifname, ifname, sizeof(emt_p->ifname));
		WBD_DEBUG("Band_flag = [0x%X] Ifname = [%s] ", band_flag, ifname);

		/* Extract Scan Status Code */
		WBD_DEBUG("Scan Status Code = [%d] ", emt_p->scan_status_code);

		/* Check for Scan Status Code Success & Following Fields presence */
		if (emt_p->scan_status_code != MAP_CHSCAN_STATUS_SUCCESS) {
			continue;
		}

		/* Extract Timestamp Length */
		WBD_DEBUG("Timestamp Length = [%d] ", emt_p->timestamp_length);

		/* Extract Timestamp */
		if (emt_p->timestamp_length > 0) {
			WBD_DEBUG("Timestamp [%s] ", emt_p->timestamp);
		}

		/* Extract Utilization */
		WBD_DEBUG("Utilization = [%d] ", emt_p->utilization);

		/* Extract Noise */
		WBD_DEBUG("Noise = [%d] ", emt_p->noise);

		/* Extract AggregateScanDuration */
		WBD_DEBUG("AggregateScanDuration = [%d] ", emt_p->aggregate_scan_duration);

		/* Extract Channel Scan Request TLV Flags */
		WBD_DEBUG("ChScanResult_Flags = [0x%X] ", emt_p->chscan_result_flag);

		/* Extract Number Of Neighbors */
		WBD_DEBUG("Number Of Neighbors = [%d] \n", emt_p->num_of_neighbors);

		/* Extract details for each Neighbor */
		foreach_iglist_item(emt_nbr_p, ieee1905_chscan_result_nbr_item,
			emt_p->neighbor_list) {

			/* Extract BSSID indicated by the Neighboring BSS */
			WBD_DEBUG("NBR_BSSID["MACF"] ", ETHERP_TO_MACF(emt_nbr_p->nbr_bssid));

			/* Extract SSID of the Neighboring BSS */
			if (emt_nbr_p->nbr_ssid.SSID_len > 0) {
				wbd_wl_format_ssid(ssidbuf, emt_nbr_p->nbr_ssid.SSID,
					emt_nbr_p->nbr_ssid.SSID_len);
				WBD_DEBUG("NBR_SSID[%s] ", ssidbuf);
				WBD_DEBUG("NBR_SSID_Len = [%d] ", emt_nbr_p->nbr_ssid.SSID_len);
			}

			/* Extract Neighboring RSSI */
			WBD_DEBUG("Neighbor RSSI = [%d] ", WBD_RCPI_TO_RSSI(emt_nbr_p->nbr_rcpi));

			/* Extract ChannelBandwidth */
			if (emt_nbr_p->ch_bw_length > 0) {
				WBD_DEBUG("ch_bw [%s] ch_bw_length [%d] ",
					emt_nbr_p->ch_bw, emt_nbr_p->ch_bw_length);
			}

			/* Extract ChanUtil & StaCnt, if BSSLoad Element Present. Else omitted */
			if (emt_nbr_p->chscan_result_nbr_flag &
				MAP_CHSCAN_RES_NBR_BSSLOAD_PRESENT) {

				WBD_DEBUG("ChannelUtil = [%d] ", emt_nbr_p->channel_utilization);
				WBD_DEBUG("StationCount = [%d] \n", emt_nbr_p->station_count);
			}

			/* Extract Channel Scan  Result TLV Neighbor Flags */
			WBD_DEBUG("ChScanResult_NBR_Flags = [0x%X] \n",
				emt_nbr_p->chscan_result_nbr_flag);
		}
	}

	/* Do below activity only for Agent Devices of Repeaters */
	if (WBD_ONBOOT_NOT_FORCED_ON_CTRLAGENT(i5_device->flags)) {
		WBD_INFO("Onboot Channel Scan Report Received for "
			"Controller Agent Device. Do not Process it further.\n");
		goto end;
	}

	/* For each Ifname in ifname_list */
	foreach(name, ifname_list, next) {

		chanspec_t chan_list[IEEE1905_MAX_CH_IN_OPCLASS];
		int chan_count = 0;

		/* For each Channel Scan Result Item */
		foreach_iglist_item(emt_p, ieee1905_chscan_result_item,
			chscan_rpt->chscan_result_list) {

			/* Store Chanspecs of matching ifname in chan_list, increase chan_count */
			if (strcmp(emt_p->ifname, name) == 0) {
				chan_list[chan_count] = emt_p->chanspec_20;
				chan_count++;
			}
		}

		/* Update pry_info for that Radio's ChScan Results */
		wbd_master_update_channel_scan_pry_info(name, chan_list, chan_count, chscan_rpt);
	}

	/* Copy Channel Scan Results to Results stored in this Device, excluding neighbors */
	i5DmCopyChScanResults(chscan_rpt, &(i5_device->stored_chscan_results), 0, FALSE);

	/* Average Stats of All Existing Repeaters for same 20 MHz Channel, and Send it to ACSD */
	wbd_master_average_and_send_pry_info_to_acsd(src_al_mac, NULL, chscan_rpt);

end:
	WBD_EXIT();
}

/* Processes Channel Scan Report by Controller */
void
wbd_master_process_channel_scan_rpt_cb(unsigned char *src_al_mac, time_t ts_chscan_rpt,
	ieee1905_chscan_report_msg *chscan_rpt)
{
	int ret = WBDE_OK;
	i5_dm_device_type *i5_device = NULL;
	wbd_device_item_t *device_vndr = NULL;
	WBD_ENTER();

	/* Validate Message data */
	WBD_ASSERT_MSGDATA(chscan_rpt, "Channel Scan Report");

	/* Extract Number of Results */
	WBD_DEBUG("Received Channel Scan Report: Timestamp[%lu] Num_Results[%d] \n",
		(unsigned long)ts_chscan_rpt, chscan_rpt->num_of_results);

	i5_device = wbd_ds_get_i5_device(src_al_mac, &ret);
	WBD_ASSERT();

	/* Fetch Vendor Device Data */
	device_vndr = i5_device->vndr_data;
	if (!device_vndr) {
		goto end;
	}

	/* Processes Onboot Channel Scan Report by Controller */
	wbd_master_process_onboot_channel_scan_rpt(src_al_mac,
		ts_chscan_rpt, chscan_rpt);

end:
	WBD_EXIT();
}

/* Prepare and send commonf dfs channel info to all the agents */
void
wbd_master_prepare_and_send_dfs_chan_info(i5_dm_device_type *src_device)
{
	i5_dm_network_topology_type *i5_topology;
	i5_dm_interface_type *src_ifr = NULL;
	wbd_dfs_chan_info_t *dfs_chan_info = NULL;
	uint8 setbitBuff[64];
	uint8 iter_setbitBuff[64];
	uint16 *pbuf = NULL;
	uint32 *ptr_setbitBuff = NULL;
	uint32 *ptr_iter_setbitBuff = NULL;
	int ret = WBDE_OK;

	if (!src_device) {
		WBD_ERROR("NULL source device for CAC status\n");
		goto end;
	}
	WBD_INFO("CAC status received from Device ["MACF"]\n",
		ETHERP_TO_MACF(src_device->DeviceId));

	i5_topology = ieee1905_get_datamodel();
	if (!i5_topology) {
		WBD_ERROR("unexpected ... no topology from ieee1905_get_datamodel \n");
		goto end;
	}

	pbuf = (uint16 *)wbd_malloc(WBD_MAX_BUF_512, &ret);
	WBD_ASSERT();

	dfs_chan_info = (wbd_dfs_chan_info_t *)wbd_malloc(sizeof(wbd_dfs_chan_info_t), &ret);
	WBD_ASSERT();

	ptr_setbitBuff = (uint32*)&setbitBuff;
	ptr_iter_setbitBuff = (uint32*)&iter_setbitBuff;

	/* Prepre dfs channel info for all the 5G interfaces in source device and send to all
	 * devices which has the same band 5G interfaces
	 */
	foreach_i5glist_item(src_ifr, i5_dm_interface_type, src_device->interface_list) {
		i5_dm_device_type *iter_device = NULL;
		i5_dm_interface_type *iter_ifr = NULL;
		uint32 i;
		uint8 n_agent_found = 0;
		uint8 index = 0;

		if (!i5DmIsInterfaceWireless(src_ifr->MediaType) ||
			(src_ifr->band == WBD_BAND_LAN_6G) ||
			(src_ifr->band == WBD_BAND_LAN_2G) ||
			!(src_ifr->isConfigured)) {
			WBD_DEBUG("Interface ["MACF"] is not wireless OR not configured OR 2G/6G "
				"continue\n", ETHERP_TO_MACF(src_ifr->InterfaceId));
			continue;
		}

		/* dont process if fronthaul bss in not configured on interface */
		if (!I5_IS_BSS_FRONTHAUL(src_ifr->mapFlags)) {
			WBD_DEBUG("No fronthaul interface configured on interface ["MACF"]. "
				"continue\n", ETHERP_TO_MACF(src_ifr->InterfaceId));
			continue;
		}

		memset(setbitBuff, 0xff, sizeof(setbitBuff));
		memset(pbuf, 0, WBD_MAX_BUF_512);

		WBD_INFO("Prepare common channel list for band [%d]. source interface ["MACF"]\n",
			src_ifr->band, ETHERP_TO_MACF(src_ifr->InterfaceId));

		foreach_i5glist_item(iter_device, i5_dm_device_type, i5_topology->device_list) {
			if (!I5_IS_MULTIAP_AGENT(iter_device->flags)) {
				continue;
			}
			if (iter_device->active_info == NULL) {
				WBD_DEBUG("CAC status not received for device ["MACF"]\n",
					ETHERP_TO_MACF(iter_device->DeviceId));
				continue;
			}
			WBD_DEBUG("Device ["MACF"] active chan count[%d] \n",
				ETHERP_TO_MACF(iter_device->DeviceId),
				iter_device->active_info->n_count);
			memset(&iter_setbitBuff, 0x00, sizeof(iter_setbitBuff));

			foreach_i5glist_item(iter_ifr, i5_dm_interface_type,
				iter_device->interface_list) {

				if (!i5DmIsInterfaceWireless(iter_ifr->MediaType) ||
					(iter_ifr->band != src_ifr->band) ||
					!(iter_ifr->isConfigured)) {
					continue;
				}

				WBD_DEBUG("Device ["MACF"] has Interface ["MACF"] with matching "
					"band [%d]\n", ETHERP_TO_MACF(iter_device->DeviceId),
					ETHERP_TO_MACF(iter_ifr->InterfaceId), iter_ifr->band);

				for (i = 0; i < iter_device->active_info->n_count; i++) {
					ieee1905_opclass_chan_info_t *info =
						&iter_device->active_info->info[i];

					if (!(iter_ifr->band &
						ieee1905_get_band_from_channel(info->opclass,
							info->chan))) {
						WBD_DEBUG("Out of band channel [%d], Skipping\n",
							info->chan);
						continue;
					}
					setbit(&iter_setbitBuff, info->chan);

					WBD_DEBUG("set bit in iter_bitbuff for channel[%d] \n",
						info->chan);

				}
				n_agent_found++;
			}
			for (i = 0; i < (sizeof(setbitBuff)/4); i++) {
				/* save agent iter_setbitBuff chan info in setbitBuff */
				ptr_setbitBuff[i] &= ptr_iter_setbitBuff[i];
			}
		}

		WBD_DEBUG("total agents found[%d]\n", n_agent_found);
		if (n_agent_found <= 1) {
			WBD_INFO("intersection is not possible, continue\n");
			continue;
		}

		memset(dfs_chan_info, 0, sizeof(wbd_dfs_chan_info_t));
		for (i = 0; i <= MAX_5G_CONTROL_CHANNEL; i++) {
			if (!isset(&setbitBuff, i)) {
				continue;
			}
			dfs_chan_info->channel[index] = i;
			dfs_chan_info->count++;
			WBD_DEBUG("DFS common chan [%d] \n", dfs_chan_info->channel[index]);
			index++;
		}

		foreach_i5glist_item(iter_device, i5_dm_device_type, i5_topology->device_list) {
			if (!I5_IS_MULTIAP_AGENT(iter_device->flags)) {
				continue;
			}
			foreach_i5glist_item(iter_ifr, i5_dm_interface_type,
				iter_device->interface_list) {

				if (!(iter_ifr->isConfigured) ||
					!i5DmIsInterfaceWireless(iter_ifr->MediaType)) {
					continue;
				}
				/* send to matching 5G band interfaces, not valid for 2g band */
				if ((src_ifr->band) & (iter_ifr->band)) {
					WBD_INFO("DFS forced channel list to: Device["MACF"] "
						"IFR["MACF"], chan[%d] band[%d]\n",
						ETHERP_TO_MACF(iter_device->DeviceId),
						ETHERP_TO_MACF(iter_ifr->InterfaceId),
						wf_chspec_ctlchan(iter_ifr->chanspec),
						iter_ifr->band);

					wbd_master_send_dfs_chan_info(dfs_chan_info, src_ifr->band,
						iter_device->DeviceId, iter_ifr->InterfaceId);
				}
			}
		}
		WBD_DEBUG("total chann info count [%d] \n", dfs_chan_info->count);
	}
end:
	if (pbuf) {
		free(pbuf);
	}
	if (dfs_chan_info) {
		free(dfs_chan_info);
	}
}

void
wbd_master_set_dfs_channel_inactive(i5_dm_device_type *src_device)
{
	int i;
	WBD_ENTER();

	if (!src_device) {
		WBD_ERROR("NULL source device in CAC status\n");
		goto end;
	}

	if (WBD_MCHAN_ENAB(wbd_get_ginfo()->flags)) {
		WBD_INFO("Multi channel operation. No need to mark radar detected "
			"channels in repeater as inactive in Root AP\n");
		goto end;
	}

	if (!src_device->inactive_info) {
		WBD_DEBUG("No inactive (radar detected) channels on device ["MACF"]\n",
			ETHERP_TO_MACF(src_device->DeviceId));
		goto end;
	}

	for (i = 0; i < src_device->inactive_info->n_count; i++) {
		chanspec_t chanspec;
		uint bw = 0;
		int agent_if_band;
		ieee1905_opclass_chan_info_t *info = &src_device->inactive_info->info[i];
		char ifname[IFNAMSIZ + 2] = {0};
		uint bitmap = 0;
		uint16 channel;

		agent_if_band = ieee1905_get_band_from_channel(info->opclass, info->chan);
		if (!(agent_if_band & (BAND_5GL | BAND_5GH))) {
			WBD_WARNING("DFS channel are only in 5G band. Skip setting "
				"radar detected on regclass [%d] band [%d] channel [%d\n",
				info->opclass, agent_if_band, info->chan);
			continue;
		}

		blanket_get_bw_from_rc(info->opclass, &bw);
		chanspec = blanket_prepare_chanspec(info->chan, info->opclass, bw,
			WL_CHANSPEC_BAND_5G);

		wbd_master_get_ifname_matching_band(agent_if_band, ifname, sizeof(ifname));

		FOREACH_20_SB(chanspec, channel) {
			bitmap = 0x00;
			blanket_get_chan_info(ifname, channel, WL_CHANSPEC_BAND_5G, &bitmap);

			if (bitmap & WL_CHAN_INACTIVE) {
				break;
			}
		}
		if (bitmap & WL_CHAN_INACTIVE) {
			WBD_INFO("%s: regclass [%d] channel [%d] is already marked as "
				"inactive(radar detected)\n",
				ifname, info->opclass, info->chan);
			continue;
		}
		WBD_INFO("%s:Setting radar detected on channel [%d] chanspec [0x%x]\n",
			ifname, info->chan, chanspec);
		blanket_set_chan_info(ifname, chanspec, WL_CHAN_INACTIVE);
	}
end:
	WBD_EXIT();
}
#endif /* MULTIAPR2 */
