/*
 * ACPHY Miscellaneous modules implementation
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
 * $Id: phy_ac_misc.c 810784 2022-04-13 19:59:25Z $
 */

#include <phy_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <phy_dbg.h>
#include <phy_cache.h>
#include <phy_mem.h>
#include <bcm_math.h>
#include <phy_btcx.h>
#include <phy_misc.h>
#include <phy_misc_api.h>
#include <phy_temp.h>
#include "phy_type_misc.h"
#include <phy_ac.h>
#include <phy_ac_info.h>
#include <phy_ac_misc.h>
#include <phy_ac_antdiv.h>
#include <phy_ac_calmgr.h>
#include <phy_ac_radio.h>
#include <phy_ac_rxgcrs.h>
#include <wlc_phy_radio.h>
#include <wlc_phy_int.h>
#include <phy_stf.h>
#include <phy_rxgcrs_api.h>
/* ************************ */
/* Modules used by this module */
/* ************************ */
#include <phy_ac_rssi.h>
#include <wlc_phyreg_ac.h>
#include <phy_vasip_api.h>

#include <phy_utils_reg.h>
#include <hndpmu.h>
#include <phy_utils_var.h>
#include <phy_rstr.h>
#include <bcmdevs.h>
#include "wlc_radioreg_20693.h"
#include <wlc_radioreg_20698.h>
#include <wlc_radioreg_20704.h>
#include <wlc_radioreg_20707.h>
#include <wlc_radioreg_20708.h>
#include <wlc_radioreg_20710.h>

#define VASIP_MEMORY_SIZE_BYTE 0x8FFFF
#define VASIP_MEMORY_START 0x28410000

/* module private states */
struct phy_ac_misc_info {
	phy_info_t *pi;
	phy_ac_info_t *aci;
	phy_misc_info_t *cmn_info;
	uint16	bb_mult_save[PHY_CORE_MAX];
	uint16	saved_bbconf;
	uint16	AfePuCtrl;
	uint8	bb_mult_save_valid;
	bool	ac_rxldpc_override;		/* LDPC override for RX, both band */
	bool	rud_agc_enable;
	int16	iqest[PHY_MAX_CORES];
};

#define DEFAULT_SPB_DEPTH (512)

#ifdef PHY_DUMP_BINARY
/* AUTOGENRATED by the tool : phyreg.py
 * These values cannot be in const memory since
 * the const area might be over-written in case of
 * crash dump
 */
phyradregs_list_t dot11acphy_regs_rev24[] = {
	{0x000,  {0x7f, 0xff, 0xfc, 0x9f}},
	{0x01f,  {0x0, 0x2, 0x1f, 0xfb}},
	{0x040,  {0x0, 0x1, 0xff, 0xff}},
	{0x060,  {0x1f, 0xff, 0x0, 0xff}},
	{0x080,  {0x80, 0x0, 0x0, 0x29}},
	{0x0b0,  {0x80, 0x0, 0x0, 0x5d}},
	{0x120,  {0x0, 0x0, 0x7, 0xff}},
	{0x140,  {0x7d, 0xff, 0xff, 0xff}},
	{0x15f,  {0x67, 0xff, 0xff, 0xff}},
	{0x17e,  {0x7, 0x80, 0x0, 0x7}},
	{0x19d,  {0x7f, 0xfe, 0x7f, 0x87}},
	{0x1bc,  {0x80, 0x0, 0x0, 0x47}},
	{0x210,  {0x0, 0x0, 0x3, 0x7}},
	{0x230,  {0x7, 0xff, 0x3, 0xff}},
	{0x250,  {0x1, 0xff, 0xfb, 0xff}},
	{0x270,  {0x7f, 0xff, 0x0, 0x3f}},
	{0x28f,  {0x0, 0x1, 0xff, 0xff}},
	{0x2b0,  {0x7, 0xff, 0xf, 0xff}},
	{0x2d0,  {0x7d, 0xff, 0xff, 0xff}},
	{0x2ef,  {0x7f, 0xff, 0x87, 0xff}},
	{0x30e,  {0x80, 0x0, 0x0, 0x3d}},
	{0x34d,  {0x1f, 0xff, 0xff, 0xff}},
	{0x370,  {0x0, 0xf, 0x0, 0xff}},
	{0x390,  {0x7f, 0xfe, 0x1f, 0xff}},
	{0x3af,  {0x80, 0x0, 0x0, 0x44}},
	{0x400,  {0x3f, 0xff, 0xff, 0xff}},
	{0x420,  {0x7f, 0xff, 0x3e, 0x7f}},
	{0x43f,  {0x80, 0x0, 0x0, 0x5a}},
	{0x49b,  {0x30, 0x0, 0x0, 0x1f}},
	{0x4d6,  {0x70, 0x1, 0xff, 0xff}},
	{0x4f5,  {0x8, 0x0, 0x38, 0x7f}},
	{0x520,  {0x7f, 0xff, 0xf, 0xff}},
	{0x53f,  {0x1f, 0xfe, 0x0, 0x7}},
	{0x580,  {0x3f, 0xff, 0xff, 0xfd}},
	{0x5a0,  {0x80, 0x0, 0x0, 0x2b}},
	{0x5d0,  {0x0, 0x0, 0x3, 0xff}},
	{0x600,  {0x0, 0xff, 0xff, 0xff}},
	{0x620,  {0x0, 0x0, 0xf, 0xff}},
	{0x640,  {0x0, 0x0, 0xff, 0xff}},
	{0x660,  {0x7f, 0xff, 0x0, 0x7}},
	{0x67f,  {0x7e, 0xfe, 0x0, 0x3f}},
	{0x69e,  {0x7f, 0xff, 0xff, 0xff}},
	{0x6c0,  {0x7f, 0x74, 0xf, 0xff}},
	{0x6df,  {0x7f, 0x2f, 0xf7, 0xff}},
	{0x6fe,  {0x0, 0x3c, 0x0, 0x1}},
	{0x720,  {0x7f, 0xff, 0x3, 0xff}},
	{0x741,  {0x0, 0xff, 0x81, 0xff}},
	{0x760,  {0x7c, 0x0, 0x3, 0xff}},
	{0x780,  {0x3, 0xff, 0xff, 0xff}},
	{0x7a0,  {0x7e, 0x67, 0x3f, 0xff}},
	{0x7bf,  {0x7f, 0xff, 0xff, 0xff}},
	{0x7e0,  {0x7f, 0xff, 0x7, 0xff}},
	{0x800,  {0x0, 0xff, 0xff, 0xff}},
	{0x820,  {0x7f, 0xff, 0xf, 0xff}},
	{0x83f,  {0x7f, 0xff, 0xff, 0xff}},
	{0x860,  {0x7f, 0xff, 0x0, 0x7}},
	{0x87f,  {0x7e, 0xfe, 0x0, 0x3f}},
	{0x89e,  {0x7f, 0xff, 0xff, 0xff}},
	{0x8c0,  {0x7f, 0x74, 0xf, 0xff}},
	{0x8df,  {0x7f, 0x2f, 0xf7, 0xff}},
	{0x8fe,  {0x0, 0x3c, 0x0, 0xd}},
	{0x920,  {0x7f, 0xff, 0x3, 0xff}},
	{0x941,  {0x0, 0xff, 0x81, 0xff}},
	{0x960,  {0x7c, 0x0, 0x3, 0xff}},
	{0x980,  {0x3, 0xff, 0xff, 0xff}},
	{0x9a0,  {0x7e, 0x67, 0x3f, 0xff}},
	{0x9bf,  {0x7f, 0xff, 0xff, 0xff}},
	{0x9e0,  {0x7f, 0xff, 0x7, 0xff}},
	{0xb00,  {0x0, 0x0, 0xff, 0xfb}},
	{0xb6a,  {0x0, 0x0, 0x3f, 0xff}},
	{0xbd0,  {0x0, 0x3, 0xbf, 0xff}},
	{0xc00,  {0x1, 0xff, 0xf, 0xef}},
	{0xc30,  {0x0, 0x3f, 0x7f, 0xff}},
	{0xd00,  {0x7f, 0xfe, 0xff, 0xff}},
	{0xd20,  {0x0, 0xff, 0x0, 0xf}},
	{0xd40,  {0x80, 0x0, 0x0, 0x23}},
	{0xd6a,  {0x0, 0x0, 0x3f, 0xff}},
	{0xdb0,  {0x0, 0x0, 0xff, 0xff}},
	{0xdd0,  {0x0, 0x3, 0xbf, 0xff}},
	{0xf00,  {0x7f, 0xf0, 0x3f, 0xff}},
	{0xf1f,  {0x0, 0x0, 0x0, 0x1}},
	{0xfa6,  {0x0, 0x0, 0x0, 0x3}},
	{0xfea,  {0x0, 0x30, 0xff, 0xff}},
};

#endif /* PHY_DUMP_BINARY */
/* local functions */
static void enable_vasip_sc(phy_info_t *pi);

static void wlc_phy_srom_read_rxgainerr_acphy(phy_info_t *pi);
static void wlc_phy_srom_read_hwrssioffset_acphy(phy_info_t *pi);
static void phy_ac_misc_nvram_femctrl_read(phy_info_t *pi);
static void phy_ac_misc_nvram_femctrl_clb_read(phy_info_t *pi);

#ifdef PHY_DUMP_BINARY
static int phy_ac_misc_getlistandsize(phy_type_misc_ctx_t *ctx, phyradregs_list_t **phyreglist,
	uint16 *phyreglist_sz);
#endif
#if defined(BCMDBG) || defined(WLTEST)
static void phy_ac_init_test(phy_type_misc_ctx_t *ctx, bool encals);
static void phy_ac_misc_test_stop(phy_type_misc_ctx_t *ctx);
static int wlc_phy_freq_accuracy_acphy(phy_type_misc_ctx_t *ctx, int channel);
#endif
static uint32 phy_ac_rx_iq_est(phy_type_misc_ctx_t *ctx, uint8 samples, uint8 antsel,
	uint8 resolution, uint8 lpf_hpc, uint8 dig_lpf, uint8 gain_correct,
                      uint8 extra_gain_3dB, uint8 wait_for_crs, uint8 force_gain_type);
static void phy_ac_misc_deaf_mode(phy_type_misc_ctx_t *ctx, bool user_flag);
static int phy_ac_iovar_tx_tone(phy_type_misc_ctx_t *ctx, int32 int_val);
static void phy_ac_iovar_txlo_tone(phy_type_misc_ctx_t *ctx);
static int phy_ac_iovar_get_rx_iq_est(phy_type_misc_ctx_t *ctx, int32 *ret_int_ptr,
	int32 int_val, int err);
static int phy_ac_iovar_set_rx_iq_est(phy_type_misc_ctx_t *ctx, int32 int_val, int err);
static bool phy_ac_misc_get_rxgainerr(phy_type_misc_ctx_t *ctx, int16 *gainerr);
#ifdef ATE_BUILD
static void phy_ac_gpaio_gpaioconfig(phy_type_misc_ctx_t *ctx, wl_gpaio_option_t option, int core);
static void phy_type_misc_disable_ate_gpiosel(phy_type_misc_ctx_t *ctx);
static void phy_ac_iqdac_buf_dc_setup(phy_info_t *pi, int option);
static void phy_ac_iqdac_buf_dc_meas(phy_info_t *pi, int core);
static void phy_ac_iqdac_buf_dc_clear(phy_info_t *pi, int option);
typedef struct regs_save_dacbufiq_dc {
	uint16 txdac_reg0[PHY_CORE_MAX];
	uint16 txdac_reg1[PHY_CORE_MAX];
	uint16 tx2g_mx[PHY_CORE_MAX];
	uint16 tx5g_mx[PHY_CORE_MAX];
	uint16 pmu_op1[PHY_CORE_MAX];
	uint16 bg_reg10;
	uint16 bg_reg9;
	uint16 lpf_ovr1[PHY_CORE_MAX];
	uint16 lpf_ovr2[PHY_CORE_MAX];
	uint16 lpf_reg7[PHY_CORE_MAX];
	uint16 lpf_reg8[PHY_CORE_MAX];
	uint16 lpf_reg9[PHY_CORE_MAX];
	uint16 txiqlocal_d[PHY_CORE_MAX];
	uint16 bbmult[PHY_CORE_MAX];
} regs_save_dacbufiq_dc_t;

static regs_save_dacbufiq_dc_t save_regs_dc_meas;
#endif /* ATE_BUILD */

static void phy_ac_misc_nvram_attach(phy_ac_misc_info_t *misc_info, phy_info_t *pi);
static uint8 phy_ac_misc_get_bfe_ndp_recvstreams(phy_type_misc_ctx_t *ctx);
static void phy_update_rxldpc_acphy(phy_type_misc_ctx_t *ctx, bool ldpc);
static void phy_ac_misc_set_preamble_override(phy_type_misc_ctx_t *ctx, int8 val);
static void wlc_phy_runsamples_acphy(phy_info_t *pi, uint16 num_samps,
	enum tx_tone_iqcal_mode_e iqcal_mode);
static void wlc_phy_runsamples_acphy_with_counts(phy_info_t *pi, uint16 num_samps,
	enum tx_tone_iqcal_mode_e iqcal_mode, uint16 loops, uint16 wait);
static void wlc_phy_runsamples_acphy_mac_based(phy_info_t *pi, uint16 num_samps);

#ifdef WFD_PHY_LL
static void phy_ac_misc_wfdll_chan_active(phy_type_misc_ctx_t *ctx, bool chan_active);
#endif

static uint16
phy_ac_misc_classifier(phy_type_misc_ctx_t *ctx, uint16 mask, uint16 val);

/* register phy type specific implementation */
phy_ac_misc_info_t *
BCMATTACHFN(phy_ac_misc_register_impl)(phy_info_t *pi, phy_ac_info_t *aci,
	phy_misc_info_t *cmn_info)
{
	phy_ac_misc_info_t *misc_info;
	phy_type_misc_fns_t fns;

	PHY_TRACE(("%s\n", __FUNCTION__));

	/* allocate all storage together */
	if ((misc_info = phy_malloc(pi, sizeof(phy_ac_misc_info_t))) == NULL) {
		PHY_ERROR(("%s: phy_malloc failed\n", __FUNCTION__));
		goto fail;
	}
	misc_info->pi = pi;
	misc_info->aci = aci;
	misc_info->cmn_info = cmn_info;

	/* register PHY type specific implementation */
	bzero(&fns, sizeof(fns));
	fns.ctx = misc_info;
	fns.phy_type_misc_rx_iq_est = phy_ac_rx_iq_est;
	fns.phy_type_misc_set_deaf = phy_ac_misc_deaf_mode;
	fns.phy_type_misc_clear_deaf = phy_ac_misc_deaf_mode;
#if defined(BCMDBG) || defined(WLTEST)
	fns.phy_type_misc_test_init = phy_ac_init_test;
	fns.phy_type_misc_test_stop = phy_ac_misc_test_stop;
	fns.phy_type_misc_test_freq_accuracy = wlc_phy_freq_accuracy_acphy;
#endif
#ifdef PHY_DUMP_BINARY
	fns.phy_type_misc_getlistandsize = phy_ac_misc_getlistandsize;
#endif
	fns.phy_type_misc_iovar_tx_tone = phy_ac_iovar_tx_tone;
	fns.phy_type_misc_iovar_txlo_tone = phy_ac_iovar_txlo_tone;
	fns.phy_type_misc_iovar_get_rx_iq_est = phy_ac_iovar_get_rx_iq_est;
	fns.phy_type_misc_iovar_set_rx_iq_est = phy_ac_iovar_set_rx_iq_est;
#ifdef ATE_BUILD
	fns.gpaioconfig = phy_ac_gpaio_gpaioconfig;
	fns.disable_ate_gpiosel = phy_type_misc_disable_ate_gpiosel;
#endif
	fns.phy_type_misc_get_rxgainerr = phy_ac_misc_get_rxgainerr;
	fns.get_bfe_ndp_recvstreams = phy_ac_misc_get_bfe_ndp_recvstreams;
	fns.set_ldpc_override = phy_update_rxldpc_acphy;
	fns.set_preamble_override = phy_ac_misc_set_preamble_override;
#ifdef WFD_PHY_LL
	fns.set_wfdll_chan_active = phy_ac_misc_wfdll_chan_active;
#endif

	fns.phy_type_misc_classifier = phy_ac_misc_classifier;

	wlc_phy_srom_read_hwrssioffset_acphy(pi);
	wlc_phy_srom_read_rxgainerr_acphy(pi);
	phy_ac_misc_nvram_femctrl_read(pi);

	/* pre_init to ON, register POR default setting */
	misc_info->ac_rxldpc_override = ON;

	/* Read srom params from nvram */
	phy_ac_misc_nvram_attach(misc_info, pi);

	if (phy_misc_register_impl(cmn_info, &fns) != BCME_OK) {
		PHY_ERROR(("%s: phy_misc_register_impl failed\n", __FUNCTION__));
		goto fail;
	}

	phy_ac_misc_nvram_femctrl_clb_read(pi);

	/* Register the required scratch buffer size, revisit this if the tone_buf size conditions
	 * has changed in wlc_phy_gen_load_samples_acphy()
	 */
	phy_cache_register_reuse_size(pi->cachei, sizeof(math_cint32) * DEFAULT_SPB_DEPTH);

	return misc_info;

	/* error handling */
fail:
	if (misc_info != NULL)
		phy_mfree(pi, misc_info, sizeof(phy_ac_misc_info_t));
	return NULL;
}

void
BCMATTACHFN(phy_ac_misc_unregister_impl)(phy_ac_misc_info_t *misc_info)
{
	phy_info_t *pi;
	phy_misc_info_t *cmn_info;

	ASSERT(misc_info);
	pi = misc_info->pi;
	cmn_info = misc_info->cmn_info;

	PHY_TRACE(("%s\n", __FUNCTION__));

	/* unregister from common */
	phy_misc_unregister_impl(cmn_info);

	phy_mfree(pi, misc_info, sizeof(phy_ac_misc_info_t));
}

void
wlc_phy_conditional_suspend(phy_info_t *pi, bool *suspend)
{
	/* suspend mac before any phyreg access. http://jira.broadcom.com/browse/CRWLDOT11M-2177 */
	/* Suspend MAC if haven't done so */
	*suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
	if (!(*suspend)) {
		wlapi_suspend_mac_and_wait(pi->sh->physhim);
	}
}

void
wlc_phy_conditional_resume(phy_info_t *pi, bool *suspend)
{
	/* UnSuspend MAC if haven't done so */
	/* suspend mac before any phyreg access. http://jira.broadcom.com/browse/CRWLDOT11M-2177 */
		if (!(*suspend)) {
			wlapi_enable_mac(pi->sh->physhim);
		}
}

static uint32
phy_ac_rx_iq_est_res0(phy_ac_misc_info_t *info, uint32 cmplx_pwr[], uint8 extra_gain_1dB)
{
	uint32 result = 0;
	phy_info_t *pi = info->pi;
	uint8 core;
	int8 noise_dbm_ant[PHY_CORE_MAX];

	/* pi->phy_noise_win per antenna is updated inside */
	wlc_phy_noise_calc(pi, cmplx_pwr, noise_dbm_ant, extra_gain_1dB);

	pi->phynoise_state &= ~PHY_NOISE_STATE_MON;

	for (core = PHYCORENUM(pi->pubpi->phy_corenum); core >= 1; core--)
		result = (result << 8) | (noise_dbm_ant[core - 1] & 0xff);

	if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
	    (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
		for (core = 0; core < PHYCORENUM(pi->pubpi->phy_corenum); core++)
		    info->iqest[core] = noise_dbm_ant[core];
	}

	return result;
}

static void
phy_ac_rx_iq_est_res1_calrev0(phy_info_t *pi, int16 noise_dBm_ant_fine[])
{
	int8 subband_idx, core, bw_idx, tmp_range, ant;
	uint8 core_freq_segment_map;
	phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
	acphy_rssioffset_t *pi_ac_rssioffset = &pi_ac->sromi->rssioffset;

	if (CHSPEC_IS2G(pi->radio_chanspec)) {
		bw_idx = (CHSPEC_IS40(pi->radio_chanspec)) ? 1 : 0;
	} else {
		bw_idx = (CHSPEC_IS80(pi->radio_chanspec) ||
			PHY_AS_80P80(pi, pi->radio_chanspec)) ? 2 :
			(CHSPEC_IS160(pi->radio_chanspec)) ? 3 :
			(CHSPEC_IS40(pi->radio_chanspec)) ? 1 : 0;
	}

	/* Apply nvram based offset: */
	FOREACH_CORE(pi, core) {
		/* core_freq_segment_map is only required for 80P80 mode.
		For other modes, it is ignored
		*/
		core_freq_segment_map = phy_ac_chanmgr_get_data(pi_ac->chanmgri)
			->core_freq_mapping[core];
		ant = phy_get_rsdbbrd_corenum(pi, core);
		if (CHSPEC_IS2G(pi->radio_chanspec)) {
			noise_dBm_ant_fine[core] +=
			    pi_ac_rssioffset
			    ->rssi_corr_gain_delta_2g[ant][ACPHY_GAIN_DELTA_ELNA_ON][bw_idx] << 2;
			PHY_RXIQ(("In %s: rssi_gain_delta_offset for"
			          " core %d = %d (in dB) \n",
			          __FUNCTION__, core, pi_ac_rssioffset
			          ->rssi_corr_gain_delta_2g[ant]
			          [ACPHY_GAIN_DELTA_ELNA_ON][bw_idx]));
		} else {
			tmp_range = phy_ac_chanmgr_get_chan_freq_range(pi,
					pi->radio_chanspec, core_freq_segment_map);
			if ((tmp_range > 0) && (tmp_range < 5)) {
				subband_idx = tmp_range -1;
				noise_dBm_ant_fine[core] +=
				pi_ac_rssioffset
				->rssi_corr_gain_delta_5g[ant][0][bw_idx][subband_idx] << 2;
				PHY_RXIQ(("In %s: rssi_gain_delta_offset for"
				         " core %d = %d (in dB) \n",
				         __FUNCTION__, core, pi_ac_rssioffset
				         ->rssi_corr_gain_delta_5g[ant]
				         [ACPHY_GAIN_DELTA_ELNA_ON][bw_idx][subband_idx]));
			}
		}
	}
}

static void
phy_ac_rx_iq_est_res1_calrev1(phy_info_t *pi, int16 noise_dBm_ant_fine[], uint8 gain_correct,
	const phy_ac_rssi_data_t *rssi_data)
{
	phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
	int16 rssi_gain_delta_qdBm[PHY_CORE_MAX];
	int16 rssi_temp_delta_qdBm;
	uint8 core;

	bzero(rssi_gain_delta_qdBm, sizeof(rssi_gain_delta_qdBm));

	if (gain_correct == 4) {
		int16 curr_temp;
		int16 gain_temp_slope;

		/* This is absolute temp gain compensation
		 * This has to be applied only for Rudimentary AGC.
		 * For this, -g 4 option has to be given.
		 * For iqest cali and veri, we do not apply this
		 * absolute gain temp compensation.
		 */
		if (CHSPEC_IS2G(pi->radio_chanspec)) {
			gain_temp_slope = pi_ac->sromi->rxgain_tempadj_2g;
			/* 57 = 5.7 dB change in gain for 100 degrees change */
		} else {
			int8 tmp_range;

			if (ACMAJORREV_33(pi->pubpi->phy_rev) &&
					PHY_AS_80P80(pi, pi->radio_chanspec)) {
				uint8 chans[NUM_CHANS_IN_CHAN_BONDING];

				phy_ac_chanmgr_get_chan_freq_range_80p80(pi,
					pi->radio_chanspec, chans);
				tmp_range = chans[0];
				// FIXME - core0/1: chans[0], core2/3 chans[1]
			} else {
				tmp_range =  phy_ac_chanmgr_get_chan_freq_range(pi,
					pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
			}
			if (tmp_range == 2) {
				gain_temp_slope = pi_ac->sromi->rxgain_tempadj_5gl;
			} else if (tmp_range == 3) {
				gain_temp_slope = pi_ac->sromi->rxgain_tempadj_5gml;
			} else if (tmp_range == 4) {
				gain_temp_slope = pi_ac->sromi->rxgain_tempadj_5gmu;
			} else if (tmp_range == 5) {
				gain_temp_slope = pi_ac->sromi->rxgain_tempadj_5gh;
			} else {
				gain_temp_slope = 0;
			}
		}
		curr_temp = wlc_phy_tempsense_acphy(pi);

		if (curr_temp >= 0) {
			rssi_temp_delta_qdBm = (curr_temp * gain_temp_slope * 2 + 250)/500;
		} else {
			rssi_temp_delta_qdBm = (curr_temp * gain_temp_slope * 2 - 250)/500;
		}
	} else {
		/* SO, if it is here, -g 4 option was not provided.
		 * And both rssi_cal_rev and rxgaincal_rssical were TRUE.
		 * So, apply gain_cal_temp based compensation for tone
		 * calibration and verification. Hopefully for calibration
		 * the coeff are 0's and thus no compensation is applied.
		 */
		wlc_phy_tempsense_acphy(pi);
		wlc_phy_upd_gain_wrt_gain_cal_temp_phy(pi, &rssi_temp_delta_qdBm);
	}

	if (rssi_data->rssi_cal_rev == TRUE && rssi_data->rxgaincal_rssical == TRUE) {
		int8 subband_idx, bw_idx, ant;
		uint8 core_freq_segment_map;

		acphy_rssioffset_t *pi_ac_rssioffset = &pi_ac->sromi->rssioffset;
		if (CHSPEC_IS2G(pi->radio_chanspec)) {
			bw_idx = (CHSPEC_IS40(pi->radio_chanspec)) ? 1 : 0;
		} else {
			bw_idx = (CHSPEC_IS80(pi->radio_chanspec) ||
				PHY_AS_80P80(pi, pi->radio_chanspec)) ? 2 :
				(CHSPEC_IS160(pi->radio_chanspec)) ? 3 :
				(CHSPEC_IS40(pi->radio_chanspec)) ? 1 : 0;
		}

		/* Apply nvram based offset: */
		FOREACH_CORE(pi, core) {
			/* core_freq_segment_map is only required for
			80P80 mode. For other modes, it is ignored
			*/
			core_freq_segment_map = phy_ac_chanmgr_get_data(pi_ac->chanmgri)
				->core_freq_mapping[core];
			ant = phy_get_rsdbbrd_corenum(pi, core);
			subband_idx = wlc_phy_rssi_get_chan_freq_range_acphy(pi,
					core_freq_segment_map, core);
			if (CHSPEC_IS2G(pi->radio_chanspec)) {
				rssi_gain_delta_qdBm[core] =
				pi_ac_rssioffset->rssi_corr_gain_delta_2g_sub[ant]
				[ACPHY_GAIN_DELTA_ELNA_ON][bw_idx][subband_idx];
			} else {
				if ((subband_idx >= 0) && (subband_idx < 5)) {
					rssi_gain_delta_qdBm[core] =
					pi_ac_rssioffset->rssi_corr_gain_delta_5g_sub
					[ant][ACPHY_GAIN_DELTA_ELNA_ON][bw_idx][subband_idx];
				}
			}
		}
	}

	FOREACH_CORE(pi, core) {
		noise_dBm_ant_fine[core] += rssi_temp_delta_qdBm + rssi_gain_delta_qdBm[core];
		PHY_RXIQ(("In %s: | Core %d | temp_delta_qdBm = %d (qdB)"
		         "| gain_delta_qdBm = %d (qdB) | RXIQEST = %d (qdB)\n", __FUNCTION__, core,
		         rssi_temp_delta_qdBm, rssi_gain_delta_qdBm[core],
		         noise_dBm_ant_fine[core]));
	}
}

static uint32
phy_ac_rx_iq_est_res1(phy_ac_misc_info_t *info, uint8 gain_correct,
	int16 tot_gain[], uint32 cmplx_pwr[], uint8 extra_gain_1dB)
{
	uint32 result = 0;
	phy_info_t *pi = info->pi;
	uint8 core;
	int16 noise_dBm_ant_fine[PHY_CORE_MAX];
	uint16 crsmin_pwr[PHY_CORE_MAX];
	/* Reports power in finer resolution than 1 dB (currently 0.25 dB) */
	int16 noisefloor;
	phy_ac_rssi_data_t *rssi_data = phy_ac_rssi_get_data(pi->u.pi_acphy->rssii);

	bzero((uint16 *)noise_dBm_ant_fine, sizeof(noise_dBm_ant_fine));

	if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
		!ACMAJORREV_33(pi->pubpi->phy_rev) &&
		!(ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
		wlc_phy_noise_calc_fine_resln(pi, cmplx_pwr, crsmin_pwr,
			noise_dBm_ant_fine, extra_gain_1dB, tot_gain);
	}

	/* gainerr and temperature compensation.
	 * If rssi_cal_rev is FALSE, then rssi_gain_delta is in 1 dB steps
	 * So, this has to be adj in the tot_gain which is in 1 dB steps.
	 * Can't apply 0.25 dB steps as we can't apply comp with tot_gain
	 * So, have to apply it with tempsense comp which is in 0.25dB steps
	 */
	if ((gain_correct == 3 || gain_correct == 4) && rssi_data->rssi_cal_rev == FALSE) {
		phy_ac_rx_iq_est_res1_calrev0(pi, noise_dBm_ant_fine);
	}

	if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
		(ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
		wlc_phy_noise_calc_fine_resln(pi, cmplx_pwr, crsmin_pwr,
			noise_dBm_ant_fine, extra_gain_1dB, tot_gain);
	}

	/* This piece of code will be executed only if resolution is 1, => qdBm steps. */
	if ((gain_correct == 4) || ((rssi_data->rssi_cal_rev == TRUE) &&
		(rssi_data->rxgaincal_rssical == TRUE))) {
		phy_ac_rx_iq_est_res1_calrev1(pi, noise_dBm_ant_fine, gain_correct, rssi_data);
	}

	if (gain_correct == 1 || gain_correct == 2 || gain_correct == 3) {
		int16 gainerr[PHY_CORE_MAX];
		int16 gain_err_temp_adj;

		wlc_phy_get_rxgainerr_phy(pi, gainerr);

		/* make and apply temperature correction */
		/* Read and (implicitly) store current temperature */
		wlc_phy_tempsense_acphy(pi);
		wlc_phy_upd_gain_wrt_temp_phy(pi, &gain_err_temp_adj);

		FOREACH_CORE(pi, core) {
			/* gainerr is in 0.5dB units;
			 * need to convert to 0.25dB units
			 */
			if (gain_correct == 1) {
				gainerr[core] = gainerr[core] << 1;
				/* Apply gain correction */
				noise_dBm_ant_fine[core] -= gainerr[core];
			}
			noise_dBm_ant_fine[core] += gain_err_temp_adj;
		}
	}

	if (CHSPEC_IS40(pi->radio_chanspec))
		noisefloor = 4 * ACPHY_NOISE_FLOOR_40M;
	else if (CHSPEC_BW_LE20(pi->radio_chanspec))
		noisefloor = 4 * ACPHY_NOISE_FLOOR_20M;
	else
		noisefloor = 4 * ACPHY_NOISE_FLOOR_80M;

	/* DO NOT do flooring of estimate if the Chip is 4350 AND gain correct is 0.
	 * In other words,
	 * DO flooring for ALL chips other than 4350 AND
	 * DO flooring for 4350 if gain correct is done - ie, -g is 1/2/3/4.
	 */
	if (!(ACMAJORREV_2(pi->pubpi->phy_rev) && ACMINORREV_1(pi) && (gain_correct == 0))) {
		FOREACH_CORE(pi, core) {
			if (noise_dBm_ant_fine[core] < noisefloor) {
			        noise_dBm_ant_fine[core] = noisefloor;
			}
		}
	}

	if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
	    (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
		for (core = 0; core < PHYCORENUM(pi->pubpi->phy_corenum); core++)
		    info->iqest[core] = noise_dBm_ant_fine[core];
	}

	for (core = PHYCORENUM(pi->pubpi->phy_corenum); core >= 1; core--) {
		result = (result << 10) | (noise_dBm_ant_fine[core - 1] & 0x3ff);
	}
	pi->phynoise_state &= ~PHY_NOISE_STATE_MON;

	return result;
}

static uint32 phy_ac_rx_iq_est(phy_type_misc_ctx_t *ctx, uint8 samples, uint8 antsel,
	uint8 resolution, uint8 lpf_hpc, uint8 dig_lpf, uint8 gain_correct,
                      uint8 extra_gain_3dB, uint8 wait_for_crs, uint8 force_gain_type)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;

	phy_iq_est_t est[PHY_CORE_MAX];
	uint32 cmplx_pwr[PHY_CORE_MAX];
	int16	tot_gain[PHY_CORE_MAX];
	uint8 i, extra_gain_1dB;
	rxgain_t rxgain[PHY_CORE_MAX];
	rxgain_ovrd_t rxgain_ovrd[PHY_CORE_MAX];
	uint8 force_turnoff = 0;
	/* Local copy of phyrxchains & EnTx bits before overwrite */
	uint8 enRx = 0, enTx = 0;

	/* check if sampling is in progress */
	if (pi->phynoise_state != 0) {
		/* RB - http://wlan-rb.sj.broadcom.com/r/105244/ */
		PHY_INFORM(("%s: sampling_in_progress\n", __FUNCTION__));
		return 0;
	}

	pi->phynoise_state |= PHY_NOISE_STATE_MON;

	bzero((uint8 *)est, sizeof(est));
	bzero((uint8 *)cmplx_pwr, sizeof(cmplx_pwr));
	bzero((int16 *)tot_gain, sizeof(tot_gain));

	/* get IQ power measurements */

	phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);

	if ((ACMAJORREV_2((pi)->pubpi->phy_rev) && (ACMINORREV_0(pi) ||
		ACMINORREV_1(pi) || ACMINORREV_4(pi))) ||
	    ACMAJORREV_GE37((pi)->pubpi->phy_rev)) {
	    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
		if (antsel != stf_shdata->hw_phyrxchain) {
			/* Converting core 0/1/2 to coremask 1/2/4 */
			antsel = (1 << antsel);
			/* Save and overwrite Rx chains */
			wlc_phy_update_rxchains((wlc_phy_t *)pi, &enRx, &enTx,
			    antsel, stf_shdata->phytxchain);
			force_turnoff = 1;
		}
	}
	if (force_gain_type != 0) {
		wlc_phy_get_rxgain_acphy(pi, rxgain, tot_gain, force_gain_type);
		wlc_phy_rfctrl_override_rxgain_acphy(pi, 0, rxgain, rxgain_ovrd);
		PHY_RXIQ(("%s: For gain type = %d | Total gain being applied = %d\n",
			__FUNCTION__, force_gain_type, tot_gain[0]));
	} else {
		PHY_RXIQ(("%s: For gain type = %d | Total gain being applied = %d\n",
			__FUNCTION__, force_gain_type, ACPHY_NOISE_INITGAIN));
	}

#ifdef SAMPLE_COLLECT
	if ((!TINY_RADIO(pi) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
		ACMAJORREV_33(pi->pubpi->phy_rev)) && lpf_hpc) {
		phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

		/* Override the LPF high pass corners to their lowest values (0x1) */
		phy_ac_lpf_hpc_override(pi_ac, TRUE);
	}
#endif /* SAMPLE_COLLECT */

	/* Overide the digital LPF */
	if (!TINY_RADIO(pi) && dig_lpf && !IS_28NM_RADIO(pi) && !IS_16NM_RADIO(pi)) {
		wlc_phy_dig_lpf_override_acphy(pi, dig_lpf);
	}

	/* Increase INITgain if requested */
	if (extra_gain_3dB > 0) {
		extra_gain_3dB = wlc_phy_calc_extra_init_gain_acphy(pi, extra_gain_3dB, rxgain);

		/* Override higher INITgain if possible */
		if (extra_gain_3dB > 0) {
			wlc_phy_rfctrl_override_rxgain_acphy(pi, 0, rxgain, rxgain_ovrd);
		}
	}

	/* get IQ power measurements */
	wlc_phy_rx_iq_est_acphy(pi, est, 1 << samples, 32, wait_for_crs, FALSE);
	/* Disable the overrides if they were set */
	if (extra_gain_3dB > 0) {
		wlc_phy_rfctrl_override_rxgain_acphy(pi, 1, NULL, rxgain_ovrd);
	}

#ifdef SAMPLE_COLLECT
	if ((!TINY_RADIO(pi) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
		ACMAJORREV_33(pi->pubpi->phy_rev)) && lpf_hpc) {
		phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

		/* Restore LPF high pass corners to their original values */
		phy_ac_lpf_hpc_override(pi_ac, FALSE);
	}
#endif /* SAMPLE_COLLECT */

	/* Restore the digital LPF */
	if (!TINY_RADIO(pi) && dig_lpf && !IS_28NM_RADIO(pi) && !IS_16NM_RADIO(pi)) {
		wlc_phy_dig_lpf_override_acphy(pi, 0);
	}

	if (force_gain_type != 0) {
		if ((force_gain_type == 4) || (force_gain_type == 3)) {
			int16	tot_gain_dummy[PHY_CORE_MAX];

			/* Restore the tr sw setting for type 4 and 3 */
			/* Type 3 may not use elna off; restore anyway */
			wlc_phy_get_rxgain_acphy(pi, rxgain, tot_gain_dummy, 6);
		}
		wlc_phy_rfctrl_override_rxgain_acphy(pi, 1, NULL, rxgain_ovrd);
	}

	/* Restore Rx chains */
	if (force_turnoff) {
		wlc_phy_restore_rxchains((wlc_phy_t *)pi, enRx, enTx);
	}

	phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);

	/* sum I and Q powers for each core, average over num_samps with rounding */
	ASSERT(PHYCORENUM(pi->pubpi->phy_corenum) <= PHY_CORE_MAX);
	FOREACH_CORE(pi, i) {
		cmplx_pwr[i] = ((est[i].i_pwr + est[i].q_pwr) + (1U << (samples - 1))) >> samples;
	}

	/* convert in 1dB gain for gain adjustment */
	extra_gain_1dB = 3 * extra_gain_3dB;

	if (resolution == 0) {
		return phy_ac_rx_iq_est_res0(info, cmplx_pwr, extra_gain_1dB);
	}
	else if (resolution == 1) {
		return phy_ac_rx_iq_est_res1(info, gain_correct, tot_gain, cmplx_pwr,
		                             extra_gain_1dB);
	}

	pi->phynoise_state &= ~PHY_NOISE_STATE_MON;

	return 0;
}

/**
 * @param f_2kHz  Tone frequency in [2KHz] units
 */
static int
phy_ac_iovar_tx_tone(phy_type_misc_ctx_t *ctx, int32 f_2kHz)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;
	int ret = BCME_OK;

	pi->phy_tx_tone_freq = (int32) f_2kHz;

	if (pi->phy_tx_tone_freq == 0) {
		wlc_phy_stopplayback_acphy(pi, STOPPLAYBACK_W_CCA_RESET);
		phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
		wlapi_enable_mac(pi->sh->physhim);
	} else {
		pi->phy_tx_tone_freq = pi->phy_tx_tone_freq * 1000; /* convert to [2Hz] units */
		wlapi_suspend_mac_and_wait(pi->sh->physhim);
		phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
		ret = wlc_phy_tx_tone_acphy(pi, f_2kHz, 151, TX_TONE_IQCAL_MODE_OFF, TRUE);
	}

	return ret;
}

static void
phy_ac_misc_deaf_mode(phy_type_misc_ctx_t *ctx, bool user_flag)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;
	wlc_phy_deaf_acphy(pi, user_flag);
}

static void
phy_ac_iovar_txlo_tone(phy_type_misc_ctx_t *ctx)
{

	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;

	pi->phy_tx_tone_freq = 0;
	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
	wlc_phy_tx_tone_acphy(pi, 0, 151, TX_TONE_IQCAL_MODE_OFF, TRUE);
}

static int
phy_ac_iovar_get_rx_iq_est(phy_type_misc_ctx_t *ctx, int32 *ret_int_ptr,
	int32 int_val, int err)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;
	bool suspend;
	bool low_pwr = FALSE;
	uint16 r;
	int temp_dBm;
	int16 tmp;
	void *a;

	if (!pi->sh->up) {
		err = BCME_NOTUP;
		return err;
	}

	/* make sure bt-prisel is on WLAN side */
	wlc_phy_btcx_wlan_critical_enter(pi);

	suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
	if (!suspend) {
		wlapi_suspend_mac_and_wait(pi->sh->physhim);
	}

	phy_utils_phyreg_enter(pi);

	/* For 4350 Olympic program, -i 0 should behave exactly same as -i 1
	 * So, if there is force gain type is 0, then make it 1 for 4350
	 */
	if ((info->rud_agc_enable == TRUE) &&
	    (pi->phy_rxiq_force_gain_type == 0) &&
	    (pi->phy_rxiq_extra_gain_3dB == 0)) {
		pi->phy_rxiq_force_gain_type = 1;
	}
	/* get IQ power measurements */
	*ret_int_ptr = wlc_phy_rx_iq_est(pi, pi->phy_rxiq_samps, pi->phy_rxiq_antsel,
	                                 pi->phy_rxiq_resln, pi->phy_rxiq_lpfhpc,
	                                 pi->phy_rxiq_diglpf,
	                                 pi->phy_rxiq_gain_correct,
	                                 pi->phy_rxiq_extra_gain_3dB, 0,
	                                 pi->phy_rxiq_force_gain_type);

	if ((info->rud_agc_enable == TRUE) &&
	    (pi->phy_rxiq_force_gain_type == 1) && (pi->phy_rxiq_resln == 1)) {
		uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
		BCM_REFERENCE(phyrxchain);
		FOREACH_ACTV_CORE(pi, phyrxchain, r) {
			temp_dBm = *ret_int_ptr;
			temp_dBm = (temp_dBm >> (10*r)) & 0x3ff;
			temp_dBm = ((int16)(temp_dBm << 6)) >> 6; /* sign extension */
			if ((temp_dBm >> 2) < -82) {
				low_pwr = TRUE;
			}
			PHY_RXIQ(("In %s: | For core %d | iqest_dBm = %d"
			          " \n", __FUNCTION__, r, (temp_dBm >> 2)));
		}
		if (low_pwr) {
			pi->phy_rxiq_force_gain_type = 9;
			*ret_int_ptr = wlc_phy_rx_iq_est(pi, pi->phy_rxiq_samps,
			                                 pi->phy_rxiq_antsel,
			                                 pi->phy_rxiq_resln, pi->phy_rxiq_lpfhpc,
			                                 pi->phy_rxiq_diglpf,
			                                 pi->phy_rxiq_gain_correct,
			                                 pi->phy_rxiq_extra_gain_3dB, 0,
			                                 pi->phy_rxiq_force_gain_type);
		}
	}
	if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
	    (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
	  FOREACH_CORE(pi, r) {
		  if (pi->phy_rxiq_adcref) {
			/* adcref - For now ignore resolution */
			tmp = phy_ac_noise_get_data(pi->u.pi_acphy->noisei)->phy_noise_all_core[r] &
			  0xff;
			tmp = ((int16)(tmp << 8)) >> 8; /* sign extension */
		  } else {
			if (pi->phy_rxiq_resln) {
			  tmp = (info->iqest[r]) & 0x3ff;
			  tmp = ((int16)(tmp << 6)) >> 6; /* sign extension */
			} else {
				  tmp = (info->iqest[r]) & 0xff;
				  tmp = ((int16)(tmp << 8)) >> 8; /* sign extension */
			}
		  }
		  info->iqest[r] = tmp;
		}
		a = (int32*)ret_int_ptr;
		bcopy(info->iqest, a, PHY_MAX_CORES*sizeof(int16));
	}
	phy_utils_phyreg_exit(pi);

	if (!suspend)
	  wlapi_enable_mac(pi->sh->physhim);
	wlc_phy_btcx_wlan_critical_exit(pi);

	return err;
}

static int
phy_ac_iovar_set_rx_iq_est(phy_type_misc_ctx_t *ctx, int32 int_val, int err)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;
	uint8 samples, antenna, resolution, lpf_hpc, dig_lpf;
	uint8 gain_correct, extra_gain_3dB, force_gain_type;
	uint8 adcref;

	extra_gain_3dB = (int_val >> 28) & 0xf;
	gain_correct = (int_val >> 24) & 0xf;
	lpf_hpc = (int_val >> 20) & 0x3;
	dig_lpf = (int_val >> 22) & 0x3;
	adcref = (int_val >> 17) & 0x1;
	resolution = (int_val >> 16) & 0x1;
	samples = (int_val >> 8) & 0xff;
	antenna = int_val & 0xf;
	force_gain_type = (int_val >> 4) & 0xf;
	if (gain_correct > 4) {
		err = BCME_RANGE;
		return err;
	}

	if ((lpf_hpc != 0) && (lpf_hpc != 1)) {
		err = BCME_RANGE;
		return err;
	}
	if (dig_lpf > 2) {
		err = BCME_RANGE;
		return err;
	}

	if (samples < 10 || samples > 15) {
		err = BCME_RANGE;
		return err;
	}

	if ((antenna != 0) &&
		(antenna != 1) &&
		(antenna != 2) &&
		(antenna != ANT_RX_DIV_DEF)) {
			err = BCME_RANGE;
			return err;
	}

	pi->phy_rxiq_samps = samples;
	pi->phy_rxiq_antsel = antenna;
	pi->phy_rxiq_resln = resolution;
	pi->phy_rxiq_lpfhpc = lpf_hpc;
	pi->phy_rxiq_diglpf = dig_lpf;
	pi->phy_rxiq_gain_correct = gain_correct;
	pi->phy_rxiq_extra_gain_3dB = extra_gain_3dB;
	pi->phy_rxiq_force_gain_type = force_gain_type;
	pi->phy_rxiq_adcref = adcref;

	return err;
}

static void
BCMATTACHFN(phy_ac_misc_nvram_attach)(phy_ac_misc_info_t *misc_info, phy_info_t *pi)
{
	uint8 i;

	pi->sromi->dBpad = pi->sh->boardflags4 & BFL4_SROM12_4dBPAD;
	pi->sromi->txidxmincap2g = (int8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_txidxmincap2g, -1);
	pi->sromi->txidxmincap5g = (int8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_txidxmincap5g, -1);

	for (i = 0; i < 2; i++) {
		pi->sromi->txidxcaplow[i] = (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
			rstr_txidxcaplow, i, -40);
		pi->sromi->maxepagain[i] = (uint8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
			rstr_maxepagain, i, 0);
		pi->sromi->maxchipoutpower[i] = (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
			rstr_maxchipoutpower, i, -20);
	}

	misc_info->rud_agc_enable = (bool)PHY_GETINTVAR(pi, rstr_rud_agc_enable);
}

/* ********************************************* */
/*				External Definitions					*/
/* ********************************************* */
/* enable/disable receiving of LDPC frame */

void
wlc_phy_force_rfseq_acphy(phy_info_t *pi, uint8 cmd)
{
	uint16 trigger_mask, status_mask;
	uint16 orig_RfseqCoreActv;
	uint8 stall_val, fifo_rst_val = 0;
	uint32 phyctl_save;

	if (READ_PHYREGFLD(pi, OCLControl1, ocl_mode_enable) == 1) {
		if (cmd == ACPHY_RFSEQ_RESET2RX)
			cmd = ACPHY_RFSEQ_OCL_RESET2RX;
		if (cmd == ACPHY_RFSEQ_TX2RX)
			cmd = ACPHY_RFSEQ_OCL_TX2RX;
	}
	switch (cmd) {
	case ACPHY_RFSEQ_RX2TX:
		trigger_mask = ACPHY_RfseqTrigger_rx2tx_MASK(pi->pubpi->phy_rev);
		status_mask = ACPHY_RfseqStatus0_rx2tx_MASK(pi->pubpi->phy_rev);
		break;
	case ACPHY_RFSEQ_TX2RX:
		trigger_mask = ACPHY_RfseqTrigger_tx2rx_MASK(pi->pubpi->phy_rev);
		status_mask = ACPHY_RfseqStatus0_tx2rx_MASK(pi->pubpi->phy_rev);
		break;
	case ACPHY_RFSEQ_OCL_TX2RX:
		trigger_mask = ACPHY_RfseqTrigger_ocl_tx2rx_MASK(pi->pubpi->phy_rev);
		status_mask = ACPHY_RfseqStatus_Ocl_ocl_tx2rx_MASK(pi->pubpi->phy_rev);
		break;

	case ACPHY_RFSEQ_RESET2RX:
		trigger_mask = ACPHY_RfseqTrigger_reset2rx_MASK(pi->pubpi->phy_rev);
		status_mask = ACPHY_RfseqStatus0_reset2rx_MASK(pi->pubpi->phy_rev);
		break;
	case ACPHY_RFSEQ_OCL_RESET2RX:
		trigger_mask = ACPHY_RfseqTrigger_ocl_reset2rx_MASK(pi->pubpi->phy_rev);
		status_mask = ACPHY_RfseqStatus_Ocl_ocl_reset2rx_MASK(pi->pubpi->phy_rev);
		break;
	case ACPHY_RFSEQ_UPDATEGAINH:
		trigger_mask = ACPHY_RfseqTrigger_updategainh_MASK(pi->pubpi->phy_rev);
		status_mask = ACPHY_RfseqStatus0_updategainh_MASK(pi->pubpi->phy_rev);
		break;
	case ACPHY_RFSEQ_UPDATEGAINL:
		trigger_mask = ACPHY_RfseqTrigger_updategainl_MASK(pi->pubpi->phy_rev);
		status_mask = ACPHY_RfseqStatus0_updategainl_MASK(pi->pubpi->phy_rev);
		break;
	case ACPHY_RFSEQ_UPDATEGAINU:
		trigger_mask = ACPHY_RfseqTrigger_updategainu_MASK(pi->pubpi->phy_rev);
		status_mask = ACPHY_RfseqStatus0_updategainu_MASK(pi->pubpi->phy_rev);
		break;
	default:
		PHY_ERROR(("wl%d: %s: Unknown cmd %d\n", pi->sh->unit, __FUNCTION__, cmd));
		return;
	}

	/* Save */
	orig_RfseqCoreActv = READ_PHYREG(pi, RfseqMode);
	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	fifo_rst_val = READ_PHYREGFLD(pi, RxFeCtrl1, soft_sdfeFifoReset);
	phyctl_save = R_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi));

	/* Force gated clocks on */
	if ((pi->pubpi->phy_rev >= 32)) {
		wlapi_bmac_phyclk_fgc(pi->sh->physhim, ON);
		W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0x6); /* set reg(PHY_CTL) 0x6 */

		/* Disable Stalls */
		if (stall_val == 0)
			ACPHY_DISABLE_STALL(pi);
	}

	if (fifo_rst_val == 0)
		MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);
	if (pi->pubpi->phy_rev >= 32) {
		OSL_DELAY(1);
	}

	/* Trigger */
	phy_utils_or_phyreg(pi, ACPHY_RfseqMode(pi->pubpi->phy_rev),
		(ACPHY_RfseqMode_CoreActv_override_MASK(pi->pubpi->phy_rev) |
		ACPHY_RfseqMode_Trigger_override_MASK(pi->pubpi->phy_rev)));
	phy_utils_or_phyreg(pi, ACPHY_RfseqTrigger(pi->pubpi->phy_rev), trigger_mask);

	if ((cmd == ACPHY_RFSEQ_OCL_RESET2RX) || (cmd == ACPHY_RFSEQ_OCL_TX2RX)) {
		SPINWAIT((READ_PHYREG(pi, RfseqStatus_Ocl) & status_mask),
			ACPHY_SPINWAIT_RFSEQ_FORCE);
		if (READ_PHYREG(pi, RfseqStatus_Ocl) & status_mask) {
			PHY_FATAL_ERROR_MESG((" %s: SPINWAIT ERROR : RFseq status OCL invalid \n",
				__FUNCTION__));
			PHY_FATAL_ERROR(pi, PHY_RC_RFSEQ_STATUS_OCL_INVALID);
		}
	} else {
		SPINWAIT((READ_PHYREG(pi, RfseqStatus0) & status_mask), ACPHY_SPINWAIT_RFSEQ_FORCE);
		if (READ_PHYREG(pi, RfseqStatus0) & status_mask) {
			PHY_FATAL_ERROR_MESG((" %s: SPINWAIT ERROR : RFseq status invalid \n",
				__FUNCTION__));
			PHY_FATAL_ERROR(pi, PHY_RC_RFSEQ_STATUS_INVALID);
		}
	}

	/* Restore */
	MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, fifo_rst_val);
	if ((pi->pubpi->phy_rev >= 32)) {
		/* Undo Stalls */
		ACPHY_ENABLE_STALL(pi, stall_val);
		OSL_DELAY(1);

		/* Force gated clocks off */
		W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)phyctl_save); /* restore */
		wlapi_bmac_phyclk_fgc(pi->sh->physhim, OFF);
	}
	WRITE_PHYREG(pi, RfseqMode, orig_RfseqCoreActv);

	ASSERT((READ_PHYREG(pi, RfseqStatus0) & status_mask) == 0);
}

void
wlc_phy_deaf_acphy(phy_info_t *pi, bool mode)
{
	phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;

	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	if (mode) {
	  if (phy_ac_rxgcrs_get_deaf_count(pi_ac->rxgcrsi) == 0)
			phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
		else
			PHY_ERROR(("%s: Deafness already set\n", __FUNCTION__));
	}
	else {
		if (phy_ac_rxgcrs_get_deaf_count(pi_ac->rxgcrsi) > 0)
			phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
		else
			PHY_ERROR(("%s: Deafness already cleared\n", __FUNCTION__));
	}
	wlapi_enable_mac(pi->sh->physhim);
}

bool
wlc_phy_get_deaf_acphy(phy_info_t *pi)
{
	uint8 core;
	uint16 curr_classifctl, val;
	bool isDeaf = TRUE;
	/* Get current classifier and clip_detect settings */
	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	curr_classifctl = READ_PHYREG(pi, ClassifierCtrl) &
		ACPHY_ClassifierCtrl_classifierSel_MASK(pi->pubpi->phy_rev);

	/* XXX
	 * For deafness to be set, ofdm and cck classifiers must be disabled,
	 * AND adc_clip thresholds must be set to max (0xffff)
	 */
	if (curr_classifctl != 4) {
		isDeaf = FALSE;
	} else {
		if (ACREV_IS(pi->pubpi->phy_rev, 0)) {
			FOREACH_CORE(pi, core) {
				val = READ_PHYREGC(pi, Clip1Threshold, core);
				if (val != 0xffff) {
					isDeaf = FALSE;
					break;
				}
			}
		}
		else {
			FOREACH_CORE(pi, core) {
				val = READ_PHYREGFLDC(pi, computeGainInfo, core,
				                      disableClip1detect);
				if (val != 1) {
					isDeaf = FALSE;
					break;
				}
			}
	        }
	}

	wlapi_enable_mac(pi->sh->physhim);
	return isDeaf;
}

void
wlc_phy_gpiosel_acphy(phy_info_t *pi, uint16 sel, uint8 word_swap)
{
	uint16 save_gpioHiOutEn;

	save_gpioHiOutEn = READ_PHYREG(pi, gpioHiOutEn);
	save_gpioHiOutEn |= 0x8000;

	/* set up acphy GPIO sel */
	WRITE_PHYREG(pi, gpioSel, (word_swap<<8) | sel);
	WRITE_PHYREG(pi, gpioHiOutEn, save_gpioHiOutEn);
}

#if defined(BCMDBG) || defined(WLTEST)
static void
phy_ac_init_test(phy_type_misc_ctx_t *ctx, bool encals)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;

	/* Force WLAN antenna */
	wlc_phy_btcx_override_enable(pi);
	/* Disable tx power control */
	wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_OFF);
	/* Recalibrate for this channel */
	if (encals) {
		wlc_phy_cals_acphy(pi->u.pi_acphy->calmgri, PHY_PERICAL_UNDEF,
		                   PHY_CAL_SEARCHMODE_RESTART);
	}
	phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
}

#define ACPHY_TO_BPHY_OFF       0x3A1
#define ACPHY_BPHY_TEST         0x08
static void
phy_ac_misc_test_stop(phy_type_misc_ctx_t *ctx)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;
	if (CHSPEC_IS2G(pi->radio_chanspec)) {
		PHY_REG_LIST_START
			PHY_REG_AND_RAW_ENTRY(ACPHY_TO_BPHY_OFF + ACPHY_BPHY_TEST, 0xfc00)
			PHY_REG_WRITE_RAW_ENTRY(ACPHY_bphytestcontrol(pi->pubpi->phy_rev),
				0x0)
		PHY_REG_LIST_EXECUTE(pi);
	}
	phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
}

static int
wlc_phy_freq_accuracy_acphy(phy_type_misc_ctx_t *ctx, int channel)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;
	int bcmerror = BCME_OK;

	if (channel == 0) {
		wlc_phy_stopplayback_acphy(pi, STOPPLAYBACK_W_CCA_RESET);
		/* restore the old BBconfig, to restore resampler setting */
		WRITE_PHYREG(pi, BBConfig, info->saved_bbconf);
		WRITE_PHYREG(pi, AfePuCtrl, info->AfePuCtrl);
		wlc_phy_resetcca_acphy(pi);
	} else {
		/* Disable the re-sampler (in case we are in spur avoidance mode) */
		info->saved_bbconf = READ_PHYREG(pi, BBConfig);
		info->AfePuCtrl = READ_PHYREG(pi, AfePuCtrl);
		ACPHY_REG_LIST_START
			MOD_PHYREG_ENTRY(pi, AfePuCtrl, tssiSleepEn, 0)
			MOD_PHYREG_ENTRY(pi, bphyTest, dccomp, 0)
			MOD_PHYREG_ENTRY(pi, BBConfig, resample_clk160, 0)
		ACPHY_REG_LIST_EXECUTE(pi);
		/* use 151 since that should correspond to nominal tx output power */
		bcmerror = wlc_phy_tx_tone_acphy(pi, 0, 151, TX_TONE_IQCAL_MODE_OFF, TRUE);
	}
	return bcmerror;
}
#endif /* defined(BCMDBG) || defined(WLTEST) */

#if defined(WLTEST)
void
wlc_phy_test_scraminit_acphy(phy_info_t *pi, int8 init)
{
	/* PR 38226: routine to allow special testmodes where the scrambler is
	 * forced to a fixed init value, hence, the same bit sequence into
	 * the MAC produces the same constellation point sequence every
	 * time
	 */

	if (init < 0) {
		/* auto: clear Mode bit so that scrambler LFSR will be free
		 * running.  ok to leave scramindexctlEn and initState in
		 * whatever current condition, since their contents are unused
		 * when free running.
		 */
		MOD_PHYREG(pi, ScramSigCtrl, scramCtrlMode, 0);
	} else {
		/* fixed init: set Mode bit, clear scramindexctlEn, and write
		 * init to initState, so that scrambler LFSR will be
		 * initialized with specified value for each transmission.
		 */
		MOD_PHYREG(pi, ScramSigCtrl, initStateValue, init);
		MOD_PHYREG(pi, ScramSigCtrl, scramindexctlEn, 0);
		MOD_PHYREG(pi, ScramSigCtrl, scramCtrlMode, 1);
	}
}
#endif

void wlc_acphy_set_scramb_dyn_bw_en(phy_info_t *pi, bool enable)
{
	phy_utils_phyreg_enter(pi);
	MOD_PHYREG(pi, NsyncscramInit1, scramb_dyn_bw_en, (enable) ? 1 : 0);
	phy_utils_phyreg_exit(pi);
}

void
wlc_phy_cts2self(phy_info_t *pi, uint16 duration)
{
	int mac_depth = 0;
	int carrier_depth = 0;
	phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;

	/* phycrs would get stuck when phy is in deaf and mac tries to transmit pkt
	 * disable phy deaf before mac is unsuspended
	 */
	while ((carrier_depth < 100) && (phy_ac_rxgcrs_get_deaf_count(pi_ac->rxgcrsi) > 0)) {
		/* Re-enable classifier/detection */
		phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
		carrier_depth++;
	}
	while ((mac_depth < 100) && !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC)) {
		/* Unsuspend mac */
		wlapi_enable_mac(pi->sh->physhim);
		mac_depth++;
	}
	ASSERT((R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC) != 0);
	if (duration > 0)
		wlapi_bmac_write_shm(pi->sh->physhim, M_CTS_DURATION(pi), duration);
	while (mac_depth) {
		/* Leave the mac in its original state */
		wlapi_suspend_mac_and_wait(pi->sh->physhim);
		mac_depth--;
	}

	/* Make sure mac is suspended when this function is called and over */
	ASSERT((R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC) == 0);

	/* Make sure pktproc is not at tx state */
	SPINWAIT((READ_PHYREGFLD(pi, pktprocdebug, pktprocstate) != 0x11),
		ACPHY_SPINWAIT_PKTPROC_STATE_L);
	ASSERT(READ_PHYREGFLD(pi, pktprocdebug, pktprocstate) != 0x11);

	while (carrier_depth) {
		/* Leave the stay in carrier status as original */
		phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
		carrier_depth--;
	}
	wlc_phy_resetcca_acphy(pi);

	/* Disable Power control */
	wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_OFF);
}

void
wlc_phy_force_rfseq_noLoleakage_acphy(phy_info_t *pi)
{
	uint8 core;
	phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
	uint16 rfctrlIntc[PHY_CORE_MAX];

	BCM_REFERENCE(stf_shdata);

	/* Disable PA during rfseq setting (save/mod) */
	FOREACH_CORE(pi, core)
		rfctrlIntc[core] = READ_PHYREGCE(pi, RfctrlIntc, core);
	FOREACH_CORE(pi, core) {
		MOD_PHYREGCE(pi, RfctrlIntc, core, override_ext_pa, 1);
		MOD_PHYREGCE(pi, RfctrlIntc, core, ext_5g_papu, 0);
		MOD_PHYREGCE(pi, RfctrlIntc, core, tr_sw_tx_pu, 0);
		MOD_PHYREGCE(pi, RfctrlIntc, core, tr_sw_rx_pu, 1);
		MOD_PHYREGCE(pi, RfctrlIntc, core, override_tr_sw, 1);
	}

	if (PHY_IPA(pi)) {
		/* Turn Off iPA in override mode */
		FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
			MOD_PHYREGCE(pi, RfctrlOverrideTxPus, core, pa_pwrup, 1);
			MOD_PHYREGCE(pi, RfctrlCoreTxPus, core, pa_pwrup, 0);
		}
	}
	wlc_phy_force_femreset_acphy(pi, TRUE);

	wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_RX2TX);
	wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_TX2RX);

	if (PHY_IPA(pi)) {
		/* Remove override for iPA power up */
		FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
			MOD_PHYREGCE(pi, RfctrlOverrideTxPus, core, pa_pwrup, 0);
		}
	}
	wlc_phy_force_femreset_acphy(pi, FALSE);

	/* Restore rfctrlIntc value after reseq setting */
	FOREACH_CORE(pi, core)
		WRITE_PHYREGCE(pi, RfctrlIntc, core, rfctrlIntc[core]);
}

void
wlc_phy_force_femreset_acphy(phy_info_t *pi, bool ovr)
{
	uint8 core;
	phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

	BCM_REFERENCE(stf_shdata);

	if (ovr) {
		FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
			/* Force reset state by zeroing out the FEM ctrl inputs */
			WRITE_PHYREGCE(pi, RfctrlIntc, core, 0x1c00);
		}
		MOD_PHYREG(pi, AntSelConfig, AntCfg_OverrideEn, 1);
		MOD_PHYREG(pi, AntSelConfig, AntCfg_Override, 0);
	} else {
		FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
			/* Remove overrides */
			WRITE_PHYREGCE(pi, RfctrlIntc, core, 0x0000);
		}
		MOD_PHYREG(pi, AntSelConfig, AntCfg_OverrideEn, 0);
	}
}

/* Inter-module interfaces */

/**
 * @param[in] f_2kHz     < -50:   Tone frequency in [2KHz] units.
 *			 -1 or 0: Transmit tone at 0 Khz offset.
 *                       > 50:    Tone frequency in [2KHz] units.
 *                       Frequency must be an integer multiple of the (bandwidth dependent)
 *                       sample frequency.
 *                       All other values: invalid.
 *
 * @param[in] max_val    Maximum amplitude of the IQ signal
 */
static uint16
wlc_phy_gen_load_samples_acphy(phy_info_t *pi, int32 f_2kHz, uint16 max_val)
{
	/* XXX For at least ACMAJORREV=51, variable fs_spb_mhz is actually in [2MHz] instead
	 * of [1MHz] units. So perhaps for other major revs as well.
	 */
	uint8 fs_spb_mhz; /**< sample clock frequency of sample buffer playback */
	uint16 spb_depth = DEFAULT_SPB_DEPTH; /**< sample buffer depth in #[IQ samples] */
	uint16 num_samps = 0, t;
	math_fixed theta = 0, rot = 0;
	uint16 tbl_len, norm_val;
	math_cint32* tone_buf = NULL;

	if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
		spb_depth = 256;
	}

	/* check phy_bw */
	if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 1) {
		if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
			fs_spb_mhz = 80;
		} else if (CHSPEC_IS160(pi->radio_chanspec)) {
			fs_spb_mhz = 160;
		} else if (CHSPEC_IS80(pi->radio_chanspec) ||
		           (CHSPEC_IS8080(pi->radio_chanspec) &&
		           !ACMAJORREV_33(pi->pubpi->phy_rev)))
			fs_spb_mhz = (ACMAJORREV_32(pi->pubpi->phy_rev) ||
				ACMAJORREV_33(pi->pubpi->phy_rev) ||
				(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
				!ACMAJORREV_128(pi->pubpi->phy_rev))) ? 80 : 160;
		else if (CHSPEC_IS40(pi->radio_chanspec))
			fs_spb_mhz = (ACMAJORREV_32(pi->pubpi->phy_rev) ||
				ACMAJORREV_33(pi->pubpi->phy_rev) ||
				(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
				!ACMAJORREV_128(pi->pubpi->phy_rev))) ? 40 : 80;
		else
			fs_spb_mhz = (ACMAJORREV_32(pi->pubpi->phy_rev) ||
				ACMAJORREV_33(pi->pubpi->phy_rev) ||
				(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
				!ACMAJORREV_128(pi->pubpi->phy_rev))) ? 20 : 40;
	} else if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 2) {
		fs_spb_mhz = (ACMAJORREV_32(pi->pubpi->phy_rev) ||
			ACMAJORREV_33(pi->pubpi->phy_rev) ||
			(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
			!ACMAJORREV_128(pi->pubpi->phy_rev))) ? 80 : 160;
	} else { /* dac mode 3 */
		fs_spb_mhz = (ACMAJORREV_32(pi->pubpi->phy_rev) ||
			ACMAJORREV_33(pi->pubpi->phy_rev) ||
			(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
			!ACMAJORREV_128(pi->pubpi->phy_rev))) ? 40 : 80;
	}

	if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
	    ACMAJORREV_33(pi->pubpi->phy_rev) ||
	    ACMAJORREV_47(pi->pubpi->phy_rev) ||
	    ACMAJORREV_129(pi->pubpi->phy_rev) ||
	    ACMAJORREV_130(pi->pubpi->phy_rev)) {
		// The code below does not work correct for tones like 1.25 MHz, use the code in
		// the else to have that working
		tbl_len = fs_spb_mhz * 2;
		if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
			if (CHSPEC_IS160(pi->radio_chanspec)) {
				if (max_val == 0) {
					norm_val = 0;
				} else {
					norm_val = ((1<<16) / max_val) & 0x3ff;
					MOD_PHYREG(pi, sampleCmd1, norm, norm_val);
				}
			}
		}
	} else {
		uint16 max_periods, num_periods = 1;

		/* In driver, frequency (in KHz) argument to phy_tx_tone IOVAR is used to stop
		 * tone transmission and not actually transmit a tone at 0 KHz offset.
	         * Tone at 0 KHz offset is required for phase noise test in ATE. Hence,
	         * f_2kHz = -1 is treated as a special case to transmit tone at 0 Khz offset.
		 */
		if ((f_2kHz == 0) || (f_2kHz == -1)) {
			tbl_len = fs_spb_mhz;
			f_2kHz = 0;
		} else {
			int fs_spb_khz = fs_spb_mhz * 1000;
			/* max_periods: maximum # of tone periods that would fit in the sample
			 * buffer in case the Niquist criterium would barely be met (so 2 samples
			 * per tone period).
			 */
			max_periods = (spb_depth * ABS(f_2kHz)) / fs_spb_khz;
			/* The playback should not have undesired side effects when it jumps back
			 * from the last sample to the first sample. That means that an integer
			 * number of tone periods must be found for which the total duration is an
			 * integer multiple of the sample frequency.
			 */
			for (t = 1; t <= max_periods; t++) {
				if (((fs_spb_khz * t) % ABS(f_2kHz)) == 0) {
					num_periods = t;
					break;
				}
			}

			if (max_periods == 0 || t > max_periods) {
				PHY_ERROR((
				"%s ERROR: Unable to fit tone period within table boundary\n",
				__FUNCTION__));
				PHY_ERROR(("sample play freq = %d inum_period=%d tone=%dKHz\n",
				           fs_spb_mhz, num_periods, 2 * f_2kHz));
				return num_samps; /* which is zero */
			}

			tbl_len = (fs_spb_khz * num_periods) / ABS(f_2kHz);
			if (CHSPEC_IS160(pi->radio_chanspec)) {
				// For 160 MHz, the HW transmits I*I and Q*Q, so scale back
				if (max_val) {
					norm_val = ((1<<16) / max_val) & 0x3ff;
					MOD_PHYREG(pi, sampleCmd1, norm, norm_val);
				}
			}
		}
	}

	/* Acquire the memory buffer, phy_cache_register_reuse_size() is called during attach
	 * time to request the maximum possible size for tone_buf.
	 */
	tone_buf = phy_cache_acquire_reuse_buffer(pi->cachei, sizeof(*tone_buf) * tbl_len);

	if (ACMAJORREV_33(pi->pubpi->phy_rev) && PHY_AS_80P80(pi, pi->radio_chanspec)) {
		fs_spb_mhz *= 2;
	}

	/* set up params to generate tone */
	num_samps  = (uint16)tbl_len;
	rot = FIXED((f_2kHz * 36) / fs_spb_mhz) / 100; /* 2*pi*f/bw/1000  Note: f in KHz */
	theta = 0; /* start angle 0 */

	/* tone freq = f_c MHz ; phy_bw = phy_bw MHz ; # samples = phy_bw (1us) */
	for (t = 0; t < num_samps; t++) {
		/* compute phasor */
		math_cmplx_cordic(theta, &tone_buf[t]);
		/* update rotation angle - runs from FIXED(0) to FIXED(360) */
		theta += rot;
		/* produce sample values for play buffer */
		tone_buf[t].q = (int32)FLOAT(tone_buf[t].q * max_val);
		tone_buf[t].i = (int32)FLOAT(tone_buf[t].i * max_val);
		if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
		    !ACMAJORREV_33(pi->pubpi->phy_rev) &&
		    !(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
		    !ACMAJORREV_128(pi->pubpi->phy_rev))) {
			if (pi->phytxtone_symm) {
			        tone_buf[t].q = 0;
			}
		}
	}

	/* load sample table */
	wlc_phy_loadsampletable_acphy(pi, tone_buf, num_samps, FALSE);

	/* Release the memory buffer */
	phy_cache_release_reuse_buffer(pi->cachei, tone_buf);

	return num_samps;
} /* wlc_phy_gen_load_samples_acphy */

#define ACPHY_MAX_SAMPLEPLAY_BUF_LEN 512
void
wlc_phy_loadsampletable_acphy(phy_info_t *pi, math_cint32 *tone_buf, const uint16 num_samps,
        bool conj)
{
	uint16 t;
	uint32* data_buf = NULL;
	int32 sgn = 1;

	if (num_samps > ACPHY_MAX_SAMPLEPLAY_BUF_LEN) {
		PHY_FATAL_ERROR(pi, PHY_RC_SAMPLEPLAY_LIMIT);
	}

	data_buf = (uint32*)tone_buf;

	if (conj) {
		sgn = -1;
	}

	/* load samples into sample play buffer */
	for (t = 0; t < num_samps; t++) {
		data_buf[t] = ((((unsigned int)tone_buf[t].i) & 0x3ff) << 10) |
			(((unsigned int)(sgn * tone_buf[t].q)) & 0x3ff);
	}
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SAMPLEPLAY, num_samps, 0, 32, data_buf);
}

static void
phy_ac_misc_modify_bbmult(phy_ac_misc_info_t *misci, uint16 max_val, bool modify_bbmult)
{
	phy_info_t *pi = misci->pi;
	uint8 core;
	uint16 bb_mult;
	phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

	BCM_REFERENCE(stf_shdata);

	if (misci->bb_mult_save_valid == 0) {
		FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
			wlc_phy_get_tx_bbmult_acphy(pi, &misci->bb_mult_save[core], core);
		}
		misci->bb_mult_save_valid = 1;
	}

	/* XXX	max_val = 0, set bbmult = 0
	 * elseif modify_bbmult = 1,
	 * set samp_play (default mag 186) power @ DAC = OFDM packet power @ DAC (9.5-bit RMS)
	 * by setting bb_mult (2.6 format) to
	 * 64/64 for bw = 20, 40, 80MHz
	 */
	if (max_val == 0 || modify_bbmult) {
		if (max_val == 0) {
			bb_mult = 0;
		} else {
			if (CHSPEC_IS80(pi->radio_chanspec) ||
				PHY_AS_80P80(pi, pi->radio_chanspec))
				bb_mult = 64;
			else if (CHSPEC_IS160(pi->radio_chanspec))
				bb_mult = 64;
			else if (CHSPEC_IS40(pi->radio_chanspec))
				bb_mult = 64;
			else
				bb_mult = 64;
		}
		FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
			wlc_phy_set_tx_bbmult_acphy(pi, &bb_mult, core);
		}
	}
}

/**
 * PHY-based TX Tone
 *
 * @param[in] f_2kHz     < -50:   Frequency in [2KHz] units.
 *			 -1 or 0: Transmit tone at 0 Khz offset.
 *                       > 50:    Frequency in [2KHz] units.
 *                       Frequency must be an integer multiple of the (bandwidth dependent)
 *                       sample frequency.
 *                       All other values: invalid.
 *
 * @param[in] max_val    Maximum amplitude of the IQ signal
 * @param[in] iqcal_mode BCAWLAN-25715. If TRUE, disables a fix for phycal (on the glacial timer)
 *                       interfering with sample playback.
 */
int
wlc_phy_tx_tone_acphy(phy_info_t *pi, int32 f_2kHz, uint16 max_val,
                      enum tx_tone_iqcal_mode_e iqcal_mode, bool modify_bbmult)
{
	uint16 num_samps;
	phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;

	PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

	if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
		pi_ac->tx_pwr_ctrl_status = pi->txpwrctrl;
		wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_OFF);
	}

	if (max_val == 0) {
		/* XXX PR90390: For a zero amplitude signal, bypass loading samples
		 * and set bbmult = 0
		 */
		num_samps = 1;
	} else if ((num_samps = wlc_phy_gen_load_samples_acphy(pi, f_2kHz, max_val)) == 0) {
		return BCME_ERROR;
	}

	phy_ac_misc_modify_bbmult(pi_ac->misci, max_val, modify_bbmult);

	if (ACMAJORREV_5(pi->pubpi->phy_rev) && ACMINORREV_0(pi) &&
	    (BFCTL(pi_ac) == 3) &&
	    (BF3_FEMCTRL_SUB(pi_ac) == 0 || BF3_FEMCTRL_SUB(pi_ac) == 3)) {
		/* 43602a0 router boards with PAVREF WAR: turn on PA */
		si_pmu_regcontrol(pi->sh->sih, 0, 0x7, 7);
	}

	wlc_phy_runsamples_acphy(pi, num_samps, iqcal_mode);

	return BCME_OK;
}

int
wlc_phy_tx_tone_acphy_mac_based(phy_info_t *pi, int32 f_2kHz, uint16 max_val, uint8 iqmode,
	bool modify_bbmult)
{
	uint16 num_samps;
	phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;

	PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
	UNUSED_PARAMETER(iqmode);  /* to keep function prototype same as the PHY-based version */

	if (max_val == 0) {
		/* XXX PR90390: For a zero ampltitude signal, bypass loading samples
		 * and set bbmult = 0
		 */
		num_samps = 1;
	} else if ((num_samps = wlc_phy_gen_load_samples_acphy(pi, f_2kHz, max_val)) == 0) {
		return BCME_ERROR;
	}

	phy_ac_misc_modify_bbmult(pi_ac->misci, max_val, modify_bbmult);

	/* Now, play the samples */
	wlc_phy_runsamples_acphy_mac_based(pi, num_samps);

	return BCME_OK;
}

void
wlc_phy_stopplayback_acphy(phy_info_t *pi, bool no_reset)
{
	phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
	uint16 playback_status, phy_ctl, SampleCollectPlayCtrl;
	uint8 mac_sample_play_on = 0;
	uint16 mask, fineclkgatectrl = 0;
	uint8 stall_val;

	if (ACMAJORREV_5(pi->pubpi->phy_rev) && ACMINORREV_0(pi) &&
		(BFCTL(pi_ac) == 3) &&
		(BF3_FEMCTRL_SUB(pi_ac) == 0 || BF3_FEMCTRL_SUB(pi_ac) == 3)) {
		/* 43602a0 router boards with PAVREF WAR: turn off PA */
		si_pmu_regcontrol(pi->sh->sih, 0, 0x7, 0);
	}

	/* Find out if its a mac based sample play or phy based sample play */
	/* If its mac based sample play, unset the appropriate bits based on d11rev */
	if (D11REV_IS(pi->sh->corerev, 50) || D11REV_GE(pi->sh->corerev, 53)) {
		SampleCollectPlayCtrl =
			R_REG(pi->sh->osh, D11_SMP_CTRL(pi));
		mac_sample_play_on = (SampleCollectPlayCtrl >>
			SAMPLE_COLLECT_PLAY_CTRL_PLAY_START_SHIFT) & 1;
		if (mac_sample_play_on == 1) {
			mask = ~(1 << SAMPLE_COLLECT_PLAY_CTRL_PLAY_START_SHIFT);
			SampleCollectPlayCtrl &=  mask;
			W_REG(pi->sh->osh, D11_SMP_CTRL(pi),
				SampleCollectPlayCtrl);
		}
	} else {
		phy_ctl = R_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi));
		mac_sample_play_on = (phy_ctl >> PHYCTRL_SAMPLEPLAYSTART_SHIFT) & 1;
		if (mac_sample_play_on == 1) {
			mask = ~(1 << PHYCTRL_SAMPLEPLAYSTART_SHIFT);
			phy_ctl &= mask;
			W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), phy_ctl);
		}
	}

	if (mac_sample_play_on == 0) {
		if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
		  /* Need this, otherwise stop playback doesn't go thru fully */
		  fineclkgatectrl = READ_PHYREG(pi, fineclockgatecontrol);
		  MOD_PHYREG(pi, fineclockgatecontrol, forcetxgatedClksOn, 1);
		}

		/* check status register */
		playback_status = READ_PHYREG(pi, sampleStatus);
		if (playback_status & 0x1) {
			/* Disable stall before issue the sample play stop
			as the stall can cause it to miss the trigger
			JIRA:CRDOT11ACPHY-1099
			*/
			stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
			ACPHY_DISABLE_STALL(pi);
			phy_utils_or_phyreg(pi, ACPHY_sampleCmd(pi->pubpi->phy_rev),
				ACPHY_sampleCmd_stop_MASK(pi->pubpi->phy_rev));
			ACPHY_ENABLE_STALL(pi, stall_val);
		} else if (playback_status & 0x2) {
			phy_utils_and_phyreg(pi, ACPHY_iqloCalCmdGctl(pi->pubpi->phy_rev),
				(uint16)~ACPHY_iqloCalCmdGctl_iqlo_cal_en_MASK(pi->pubpi->phy_rev));
		} else {
			PHY_CAL(("wlc_phy_stopplayback_acphy: already disabled\n"));
		}

		if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
		  /* restore */
		  WRITE_PHYREG(pi, fineclockgatecontrol, fineclkgatectrl);
		}
	}

	/* if bb_mult_save does exist, restore bb_mult and undef bb_mult_save */
	if (pi_ac->misci->bb_mult_save_valid != 0) {
		uint8 core;

		FOREACH_CORE(pi, core) {
			wlc_phy_set_tx_bbmult_acphy(pi, &pi_ac->misci->bb_mult_save[core], core);
		}
		pi_ac->misci->bb_mult_save_valid = 0;
	}

	if (ACMAJORREV_GE40(pi->pubpi->phy_rev)) {
		SPINWAIT(((READ_PHYREG(pi, sampleStatus) & 0x7) != 0),
			ACPHY_SPINWAIT_RUNSAMPLE);
		if ((READ_PHYREG(pi, sampleStatus) & 0x7) != 0) {
			W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0);
			PHY_FATAL_ERROR_MESG((" %s: SPINWAIT ERROR : Sample play stop failed \n",
				__FUNCTION__));
			PHY_FATAL_ERROR(pi, PHY_RC_RX2TX_FAILED);
		}
		stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
		ACPHY_DISABLE_STALL(pi);
		MOD_PHYREG(pi, sampleCmd, enable, 0x0);
		ACPHY_ENABLE_STALL(pi, stall_val);
	}
	if (no_reset == FALSE) {
		wlc_phy_resetcca_acphy(pi);
	}
	if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
		wlc_phy_txpwrctrl_enable_acphy(pi, pi_ac->tx_pwr_ctrl_status);
	}
}

#define SAMPLE_LOOP_COUNT_ENDLESS	0xFFFF
#define SAMPLE_WAIT_COUNT	60	/* Should be at least 60 for farrow FIFO depth to settle.
					 * 60 is to support 80mhz mode.  Units are in 25 ns.
					 */
static void
wlc_phy_runsamples_acphy(phy_info_t *pi, uint16 num_samps, enum tx_tone_iqcal_mode_e iqcal_mode)
{
	wlc_phy_runsamples_acphy_with_counts(pi, num_samps, iqcal_mode,
		SAMPLE_LOOP_COUNT_ENDLESS, SAMPLE_WAIT_COUNT);
}

/**
 * @param[in] iqcal_mode BCAWLAN-25715. If TRUE, disables a fix for phycal (on the glacial timer)
 *                       interfering with sample playback.
 */
static void
wlc_phy_runsamples_acphy_with_counts(phy_info_t *pi, uint16 num_samps,
	enum tx_tone_iqcal_mode_e iqcal_mode, uint16 loops, uint16 wait)
{
	uint16 orig_RfseqCoreActv;
	const uint phy_rev = pi->pubpi->phy_rev;
	uint8 stall_val, core;

	/* The phy_rev parameter is unused in embedded builds as the compiler optimises it away.
	 * Mark the param as unused to avoid compiler warnings.
	 */
	UNUSED_PARAMETER(phy_rev);

	if (iqcal_mode == TX_TONE_IQCAL_MODE_OFF) {
		phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
	}

	/* Delay for proper RX2TX in sample play ow spurious emissions,radar FD */
	if (!ACMAJORREV_32(phy_rev) && !ACMAJORREV_33(phy_rev) && !ACMAJORREV_GE47(phy_rev)) {
		OSL_DELAY(15);
	}

	if (ACMAJORREV_GE33(pi->pubpi->phy_rev)) {
		stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
		ACPHY_DISABLE_STALL(pi);
		if (READ_PHYREGFLD(pi, sampleCmd, enable)) {
			MOD_PHYREG(pi, sampleCmd, stop, 0x1);
			OSL_DELAY(10);
		}
		MOD_PHYREG(pi, sampleCmd, enable, 0x1);
		ACPHY_ENABLE_STALL(pi, stall_val);
	}

	phy_utils_and_phyreg(pi, ACPHY_macbasedDACPlay(phy_rev),
		~ACPHY_macbasedDACPlay_macBasedDACPlayEn_MASK(phy_rev));

	/* configure sample play buffer */
	WRITE_PHYREG(pi, sampleDepthCount, num_samps-1);

	if (loops == SAMPLE_LOOP_COUNT_ENDLESS) {	/* 0xffff means: keep looping forever */
		WRITE_PHYREG(pi, sampleLoopCount, SAMPLE_LOOP_COUNT_ENDLESS);
	} else {
		WRITE_PHYREG(pi, sampleLoopCount, loops - 1);
	}

	/* Wait time should be atleast 60 for farrow FIFO depth to settle
	 * 60 is to support 80mhz mode.
	 * Though 20 is even for 20mhz mode, and 40 for 80mhz mode,
	 * but just giving some extra wait time
	 */
	WRITE_PHYREG(pi, sampleInitWaitCount, wait);	/* units are in 25 ns */

	PHY_TRACE(("Starting PHY based Sample Play\n"));

	/* start sample play buffer (in regular mode or iqcal mode) */
	orig_RfseqCoreActv = READ_PHYREG(pi, RfseqMode);
	phy_utils_or_phyreg(pi, ACPHY_RfseqMode(phy_rev),
		ACPHY_RfseqMode_CoreActv_override_MASK(phy_rev));
	if (READ_PHYREGFLD(pi, iqloCalCmdGctl, iqlo_cal_en)) {
		phy_utils_and_phyreg(pi, ACPHY_iqloCalCmdGctl(phy_rev), 0x3FFF);
		OSL_DELAY(20);
	}

	stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
	ACPHY_DISABLE_STALL(pi);
	if (iqcal_mode == TX_TONE_IQCAL_MODE_ON) {
		MOD_PHYREG(pi, iqloCalCmdGctl, iqlo_cal_en, 0x1);
	} else {
		/* Disable stall before issue the sample play start
		as the stall can cause it to miss the start
		*/
		MOD_PHYREG(pi, sampleCmd, start, 0x1);
	}

	/* Wait till the Rx2Tx sequencing is done */
	SPINWAIT(((READ_PHYREG(pi, RfseqStatus0) & 0x1) == 1), ACPHY_SPINWAIT_RUNSAMPLE);
	if ((READ_PHYREG(pi, RfseqStatus0) & 0x1) == 1) {
		PHY_FATAL_ERROR_MESG((" %s: SPINWAIT ERROR : Rx to Tx failed \n", __FUNCTION__));
		PHY_FATAL_ERROR(pi, PHY_RC_RX2TX_FAILED);
	}

	ACPHY_ENABLE_STALL(pi, stall_val);
	/* restore mimophyreg(RfseqMode.CoreActv_override) */
	WRITE_PHYREG(pi, RfseqMode, orig_RfseqCoreActv);

	if (ACMAJORREV_130_131(phy_rev)) {
		/* Tone starts before rx2tx is over, and so needs a fifo/farrow reset */
		FOREACH_CORE(pi, core) {
			MOD_PHYREGCE(pi, TxFIFOReset, core, tx_fifo_reset, 1);
		}
	}

	if (iqcal_mode == TX_TONE_IQCAL_MODE_OFF) {
		phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
	}
}

static void
wlc_phy_runsamples_acphy_mac_based(phy_info_t *pi, uint16 num_samps)
{
	const uint phy_rev = pi->pubpi->phy_rev;
	uint8 stall_val;

	/* The phy_rev parameter is unused in embedded builds as the compiler optimises it away.
	 * Mark the param as unused to avoid compiler warnings.
	 */
	UNUSED_PARAMETER(phy_rev);

	phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);

	/* Delay for proper RX2TX in sample play ow spurious emissions,radar FD */
	if (!ACMAJORREV_32(phy_rev) && !ACMAJORREV_33(phy_rev) &&
	    !(ACMAJORREV_GE47(phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
		OSL_DELAY(15);
	}

	if (ACMAJORREV_GE37(phy_rev)) {
		stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
		ACPHY_DISABLE_STALL(pi);
		MOD_PHYREG(pi, sampleCmd, enable, 0x1);
		ACPHY_ENABLE_STALL(pi, stall_val);
	}

	if (ACMAJORREV_33(phy_rev)) {
		MOD_PHYREG(pi, sampleCmd, enable, 0x1);
	}

	phy_utils_or_phyreg(pi, ACPHY_macbasedDACPlay(phy_rev),
		ACPHY_macbasedDACPlay_macBasedDACPlayEn_MASK(phy_rev));

	if (CHSPEC_IS80(pi->radio_chanspec) || PHY_AS_80P80(pi, pi->radio_chanspec)) {
		phy_utils_or_phyreg(pi, ACPHY_macbasedDACPlay(phy_rev),
			ACPHY_macbasedDACPlay_macBasedDACPlayMode_MASK(phy_rev) & (0x3 <<
			ACPHY_macbasedDACPlay_macBasedDACPlayMode_SHIFT(phy_rev)));
	} else if (CHSPEC_IS160(pi->radio_chanspec)) {
		// FIXME
		ASSERT(0);
	} else if (CHSPEC_IS40(pi->radio_chanspec)) {
		phy_utils_or_phyreg(pi, ACPHY_macbasedDACPlay(phy_rev),
			ACPHY_macbasedDACPlay_macBasedDACPlayMode_MASK(phy_rev) & (0x2 <<
			ACPHY_macbasedDACPlay_macBasedDACPlayMode_SHIFT(phy_rev)));
	} else {
		phy_utils_or_phyreg(pi, ACPHY_macbasedDACPlay(phy_rev),
			ACPHY_macbasedDACPlay_macBasedDACPlayMode_MASK(phy_rev) & (0x1 <<
			ACPHY_macbasedDACPlay_macBasedDACPlayMode_SHIFT(phy_rev)));
	}

	PHY_TRACE(("Starting MAC based Sample Play"));
	wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_RX2TX);

	if (D11REV_IS(pi->sh->corerev, 50) || D11REV_GE(pi->sh->corerev, 53)) {
		uint16 SampleCollectPlayCtrl = R_REG(pi->sh->osh, D11_SMP_CTRL(pi));
		SampleCollectPlayCtrl |= (1 << SAMPLE_COLLECT_PLAY_CTRL_PLAY_START_SHIFT);
		W_REG(pi->sh->osh, D11_SMP_CTRL(pi), SampleCollectPlayCtrl);
	} else {
		uint16 phy_ctl;
		phy_ctl = (1 << PHYCTRL_SAMPLEPLAYSTART_SHIFT)
			| (1 << PHYCTRL_MACPHYFORCEGATEDCLKSON_SHIFT);
		W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), phy_ctl);
	}

	phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
}

static void
BCMATTACHFN(wlc_phy_srom_read_hwrssioffset_acphy)(phy_info_t *pi)
{
	/* read hwrssi offset from srom */

	int8 tmp[PHY_CORE_NUM_4], tmp_empty[PHY_CORE_NUM_4];
	uint8 coreidx[4] = {0, 1, 2, 3};
	uint8 sb;

	(void)memset(tmp_empty, -1, sizeof(tmp_empty));  /* empty SROM values */

	/* 2G: */
	(void)memcpy(tmp, tmp_empty, sizeof(tmp));
	tmp[0] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_hwrssioffset_cmn_2g_0)) << 4) >> 4;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		tmp[1] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_hwrssioffset_cmn_2g_1)) << 4) >> 4;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		tmp[2] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_hwrssioffset_cmn_2g_2)) << 4) >> 4;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		tmp[3] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_hwrssioffset_cmn_2g_3)) << 4) >> 4;

	if (memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) {
		/* Disable correction in case of empty SROM */
		(void)memset(tmp, 0, sizeof(tmp));
	}
	pi->hwrssioffset_cmn_2g[coreidx[0]] = tmp[0];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		pi->hwrssioffset_cmn_2g[coreidx[1]] = tmp[1];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		pi->hwrssioffset_cmn_2g[coreidx[2]] = tmp[2];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		pi->hwrssioffset_cmn_2g[coreidx[3]] = tmp[3];

	(void)memcpy(tmp, tmp_empty, sizeof(tmp));
	tmp[0] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_hwrssioffset_trt_2g_0)) << 4) >> 4;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		tmp[1] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_hwrssioffset_trt_2g_1)) << 4) >> 4;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		tmp[2] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_hwrssioffset_trt_2g_2)) << 4) >> 4;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		tmp[3] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_hwrssioffset_trt_2g_3)) << 4) >> 4;

	if (memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) {
		/* Disable correction in case of empty SROM */
		(void)memset(tmp, 0, sizeof(tmp));
	}
	pi->hwrssioffset_trt_2g[coreidx[0]] = tmp[0];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		pi->hwrssioffset_trt_2g[coreidx[1]] = tmp[1];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		pi->hwrssioffset_trt_2g[coreidx[2]] = tmp[2];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		pi->hwrssioffset_trt_2g[coreidx[3]] = tmp[3];

	/* 5g & 6G Share Same Address 7 Subbands */
	for (sb = 0; sb < NUM_6G_SUBBANDS; sb++) {
		(void)memcpy(tmp, tmp_empty, sizeof(tmp));
		tmp[0] = (int8)(((int8)getintvararray(pi->vars,
			rstr_hwrssioffset_cmn_5g_6g_0, sb)) << 4) >> 4;
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
			tmp[1] = (int8)(((int8)getintvararray(pi->vars,
				rstr_hwrssioffset_cmn_5g_6g_1, sb)) << 4) >> 4;
		}
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
			tmp[2] = (int8)(((int8)getintvararray(pi->vars,
				rstr_hwrssioffset_cmn_5g_6g_2, sb)) << 4) >> 4;
		}
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
			tmp[3] = (int8)(((int8)getintvararray(pi->vars,
				rstr_hwrssioffset_cmn_5g_6g_3, sb)) << 4) >> 4;
		}
		if (memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) {
			/* Disable correction in case of empty SROM */
			(void)memset(tmp, 0, sizeof(tmp));
		}

		pi->hwrssioffset_cmn_5g_6g[coreidx[0]][sb] = tmp[0];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
			pi->hwrssioffset_cmn_5g_6g[coreidx[1]][sb] = tmp[1];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
			pi->hwrssioffset_cmn_5g_6g[coreidx[2]][sb] = tmp[2];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
			pi->hwrssioffset_cmn_5g_6g[coreidx[3]][sb] = tmp[3];

		(void)memcpy(tmp, tmp_empty, sizeof(tmp));
		tmp[0] = (int8)(((int8)getintvararray(pi->vars,
			rstr_hwrssioffset_trt_5g_6g_0, sb)) << 4) >> 4;
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
			tmp[1] = (int8)(((int8)getintvararray(pi->vars,
				rstr_hwrssioffset_trt_5g_6g_1, sb)) << 4) >> 4;
		}
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
			tmp[2] = (int8)(((int8)getintvararray(pi->vars,
				rstr_hwrssioffset_trt_5g_6g_2, sb)) << 4) >> 4;
		}
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
			tmp[3] = (int8)(((int8)getintvararray(pi->vars,
				rstr_hwrssioffset_trt_5g_6g_3, sb)) << 4) >> 4;
		}

		if (memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) {
			/* Disable correction in case of empty SROM */
			(void)memset(tmp, 0, sizeof(tmp));
		}

		pi->hwrssioffset_trt_5g_6g[coreidx[0]][sb] = tmp[0];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
			pi->hwrssioffset_trt_5g_6g[coreidx[1]][sb] = tmp[1];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
			pi->hwrssioffset_trt_5g_6g[coreidx[2]][sb] = tmp[2];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
			pi->hwrssioffset_trt_5g_6g[coreidx[3]][sb] = tmp[3];
	}
}

static void
BCMATTACHFN(wlc_phy_srom_read_rxgainerr_acphy)(phy_info_t *pi)
{
	/* read and uncompress gain-error values for rx power reporting */

	int8 tmp[PHY_CORE_NUM_4], tmp_empty[PHY_CORE_NUM_4];
	uint8 coreidx[4] = {0, 1, 2, 3};
	uint8 sb;
	int16 tmp2;

	(void)memset(tmp_empty, -1, sizeof(tmp_empty));  /* empty SROM values */

	if (phy_get_phymode(pi) == PHYMODE_RSDB) {
		if (phy_get_current_core(pi) == PHY_RSBD_PI_IDX_CORE0) {
			/* update pi[0] to hold pwrdet params for all cores */
			/* This is required for mimo operation */
			pi->pubpi->phy_corenum <<= 1;
		} else {
			coreidx[1] = 0;
		}
	}

	tmp2 = pi->srom_rawtempsense;
	if (tmp2 == 255) {
		/* set to -1, since nothing was written to SROM */
		tmp2 = -1;
	}

	/* 2G: */
	/* read and sign-extend */
	(void)memcpy(tmp, tmp_empty, sizeof(tmp));
	tmp[0] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_rxgainerr2ga0)) << 2) >> 2;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		tmp[1] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_rxgainerr2ga1)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		tmp[2] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_rxgainerr2ga2)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		tmp[3] = (int8)(((int8)PHY_GETINTVAR(pi, rstr_rxgainerr2ga3)) << 3) >> 3;

	if ((memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) && (tmp2 == -1)) {
		/* If all srom values are -1, then possibly
		 * no gainerror info was written to srom
		 */
		(void)memset(tmp, 0, sizeof(tmp));
		pi->rxgainerr2g_isempty = TRUE;
	} else {
		pi->rxgainerr2g_isempty = FALSE;
	}
	/* gain errors for cores 1 and 2 are stored in srom as deltas relative to core 0: */
	pi->rxgainerr_2g[coreidx[0]] = tmp[0];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		pi->rxgainerr_2g[coreidx[1]] = tmp[0] + tmp[1];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		pi->rxgainerr_2g[coreidx[2]] = tmp[0] + tmp[2];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		pi->rxgainerr_2g[coreidx[3]] = tmp[0] + tmp[3];

	/* 5G low: */
	/* read and sign-extend */
	(void)memcpy(tmp, tmp_empty, sizeof(tmp));
	tmp[0] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga0, 0)) << 2) >> 2;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		tmp[1] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga1, 0)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		tmp[2] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga2, 0)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		tmp[3] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga3, 0)) << 3) >> 3;

	if ((memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) && (tmp2 == -1)) {
		/* If all srom values are -1, then possibly
		 * no gainerror info was written to srom
		 */
		(void)memset(tmp, 0, sizeof(tmp));
		pi->rxgainerr5gl_isempty = TRUE;
	} else {
		pi->rxgainerr5gl_isempty = FALSE;
	}
	/* gain errors for cores 1 and 2 are stored in srom as deltas relative to core 0: */
	pi->rxgainerr_5gl[coreidx[0]] = tmp[0];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		pi->rxgainerr_5gl[coreidx[1]] = tmp[0] + tmp[1];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		pi->rxgainerr_5gl[coreidx[2]] = tmp[0] + tmp[2];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		pi->rxgainerr_5gl[coreidx[3]] = tmp[0] + tmp[3];

	/* 5G mid: */
	/* read and sign-extend */
	(void)memcpy(tmp, tmp_empty, sizeof(tmp));
	tmp[0] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga0, 1)) << 2) >> 2;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		tmp[1] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga1, 1)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		tmp[2] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga2, 1)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		tmp[3] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga3, 1)) << 3) >> 3;

	if ((memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) && (tmp2 == -1)) {
		/* If all srom values are -1, then possibly
		 * no gainerror info was written to srom
		 */
		(void)memset(tmp, 0, sizeof(tmp));
		pi->rxgainerr5gm_isempty = TRUE;
	} else {
		pi->rxgainerr5gm_isempty = FALSE;
	}
	/* gain errors for cores 1 and 2 are stored in srom as deltas relative to core 0: */
	pi->rxgainerr_5gm[coreidx[0]] = tmp[0];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		pi->rxgainerr_5gm[coreidx[1]] = tmp[0] + tmp[1];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		pi->rxgainerr_5gm[coreidx[2]] = tmp[0] + tmp[2];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		pi->rxgainerr_5gm[coreidx[3]] = tmp[0] + tmp[3];

	/* 5G high: */
	/* read and sign-extend */
	(void)memcpy(tmp, tmp_empty, sizeof(tmp));
	tmp[0] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga0, 2)) << 2) >> 2;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		tmp[1] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga1, 2)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		tmp[2] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga2, 2)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		tmp[3] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga3, 2)) << 3) >> 3;

	if ((memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) && (tmp2 == -1)) {
		/* If all srom values are -1, then possibly
		 * no gainerror info was written to srom
		 */
		(void)memset(tmp, 0, sizeof(tmp));
		pi->rxgainerr5gh_isempty = TRUE;
	} else {
		pi->rxgainerr5gh_isempty = FALSE;
	}
	/* gain errors for cores 1 and 2 are stored in srom as deltas relative to core 0: */
	pi->rxgainerr_5gh[coreidx[0]] = tmp[0];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		pi->rxgainerr_5gh[coreidx[1]] = tmp[0] + tmp[1];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		pi->rxgainerr_5gh[coreidx[2]] = tmp[0] + tmp[2];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		pi->rxgainerr_5gh[coreidx[3]] = tmp[0] + tmp[3];

	/* 5G upper: */
	/* read and sign-extend */
	(void)memcpy(tmp, tmp_empty, sizeof(tmp));
	tmp[0] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga0, 3)) << 2) >> 2;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		tmp[1] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga1, 3)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		tmp[2] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga2, 3)) << 3) >> 3;
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		tmp[3] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr5ga3, 3)) << 3) >> 3;

	if ((memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) && (tmp2 == -1)) {
		/* If all srom values are -1, then possibly
		 * no gainerror info was written to srom
		 */
		(void)memset(tmp, 0, sizeof(tmp));
		pi->rxgainerr5gu_isempty = TRUE;
	} else {
		pi->rxgainerr5gu_isempty = FALSE;
	}
	/* gain errors for cores 1 and 2 are stored in srom as deltas relative to core 0: */
	pi->rxgainerr_5gu[coreidx[0]] = tmp[0];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
		pi->rxgainerr_5gu[coreidx[1]] = tmp[0] + tmp[1];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
		pi->rxgainerr_5gu[coreidx[2]] = tmp[0] + tmp[2];
	if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
		pi->rxgainerr_5gu[coreidx[3]] = tmp[0] + tmp[3];

	/* 6G: 7 subbands */
	/* read and sign-extend */
	for (sb = 0; sb < NUM_6G_SUBBANDS; sb++) {
		(void)memcpy(tmp, tmp_empty, sizeof(tmp));
		tmp[0] = (int8)(((int8)getintvararray(pi->vars, rstr_rxgainerr6ga0, sb)) << 2) >> 2;
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
			tmp[1] = (int8)(((int8)getintvararray(pi->vars,
					rstr_rxgainerr6ga1, sb)) << 3) >> 3;
		}
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
			tmp[2] = (int8)(((int8)getintvararray(pi->vars,
					rstr_rxgainerr6ga2, sb)) << 3) >> 3;
		}
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
			tmp[3] = (int8)(((int8)getintvararray(pi->vars,
					rstr_rxgainerr6ga3, sb)) << 3) >> 3;
		}

		if ((memcmp(tmp, tmp_empty, sizeof(tmp)) == 0) && (tmp2 == -1)) {
			/* If all srom values are -1, then possibly
			* no gainerror info was written to srom
			*/
			(void)memset(tmp, 0, sizeof(tmp));
			pi->rxgainerr6g_isempty[sb] = TRUE;
		} else {
			pi->rxgainerr6g_isempty[sb] = FALSE;
		}
		/* gain errors for cores 1 and 2 are stored in srom as deltas relative to core 0: */
		pi->rxgainerr_6g[coreidx[0]][sb] = tmp[0];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
			pi->rxgainerr_6g[coreidx[1]][sb] = tmp[0] + tmp[1];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
			pi->rxgainerr_6g[coreidx[2]][sb] = tmp[0] + tmp[2];
		if (PHYCORENUM(pi->pubpi->phy_corenum) > 3)
			pi->rxgainerr_6g[coreidx[3]][sb] = tmp[0] + tmp[3];

	}

	if ((phy_get_phymode(pi) == PHYMODE_RSDB) &&
		(phy_get_current_core(pi) == PHY_RSBD_PI_IDX_CORE0))
	{
		/* update pi[0] to hold pwrdet params for all cores */
		/* This is required for mimo operation */
		pi->pubpi->phy_corenum >>= 1;
	}

}

static void
BCMATTACHFN(phy_ac_misc_nvram_femctrl_clb_read)(phy_info_t *pi)
{
	phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

	pi_ac->sromi->nvram_femctrl_clb.map_2g[0][0] =
		(uint32) PHY_GETINTVAR_DEFAULT(pi, rstr_clb2gslice0core0, 0x3ff);
	pi_ac->sromi->nvram_femctrl_clb.map_2g[1][0] =
		(uint32) PHY_GETINTVAR_DEFAULT(pi, rstr_clb2gslice1core0, 0x3ff);

	if (PHYCORENUM((pi)->pubpi->phy_corenum) >= 2) {
		pi_ac->sromi->nvram_femctrl_clb.map_2g[0][1] =
			(uint32) PHY_GETINTVAR_DEFAULT(pi, rstr_clb2gslice0core1, 0x3ff);
		pi_ac->sromi->nvram_femctrl_clb.map_2g[1][1] =
			(uint32) PHY_GETINTVAR_DEFAULT(pi, rstr_clb2gslice1core1, 0x3ff);
	}

	if (PHY_BAND5G_ENAB(pi)) {
		pi_ac->sromi->nvram_femctrl_clb.map_5g[0][0] =
			(uint32) PHY_GETINTVAR_DEFAULT(pi, rstr_clb5gslice0core0, 0x3ff);
		pi_ac->sromi->nvram_femctrl_clb.map_5g[1][0] =
			(uint32) PHY_GETINTVAR_DEFAULT(pi, rstr_clb5gslice1core0, 0x3ff);

		if (PHYCORENUM((pi)->pubpi->phy_corenum) >= 2) {
			pi_ac->sromi->nvram_femctrl_clb.map_5g[0][1] =
				(uint32) PHY_GETINTVAR_DEFAULT(pi, rstr_clb5gslice0core1, 0x3ff);
			pi_ac->sromi->nvram_femctrl_clb.map_5g[1][1] =
				(uint32) PHY_GETINTVAR_DEFAULT(pi, rstr_clb5gslice1core1, 0x3ff);
		}
	}

	pi_ac->sromi->nvram_femctrl_clb.btc_prisel_mask =
		(uint8) PHY_GETINTVAR_DEFAULT(pi, rstr_btc_prisel_mask, 0);

	pi_ac->sromi->nvram_femctrl_clb.btc_prisel_ant_mask =
		(uint8) PHY_GETINTVAR_DEFAULT(pi, rstr_btc_prisel_ant_mask, 0x3);

	pi_ac->sromi->clb_swctrl_smask_ant0 =
		(uint16) PHY_GETINTVAR_DEFAULT(pi, rstr_clb_swctrl_smask_ant0, 0x3ff);

	pi_ac->sromi->clb_swctrl_smask_ant1 =
		(uint16) PHY_GETINTVAR_DEFAULT(pi, rstr_clb_swctrl_smask_ant1, 0x3ff);
}

static void
BCMATTACHFN(phy_ac_misc_nvram_femctrl_read)(phy_info_t *pi)
{
	uint8 i;
	phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

	if (ACPHY_FEMCTRL_ACTIVE(pi)) {
		return;
	}
	if (PHY_GETVAR_SLICE(pi, rstr_swctrlmap_2g)) {
		for (i = 0; i < ACPHY_SWCTRL_NVRAM_PARAMS; i++) {
			pi_ac->sromi->nvram_femctrl.swctrlmap_2g[i] =
				(uint32) PHY_GETINTVAR_ARRAY_SLICE(pi,
				rstr_swctrlmap_2g, i);
		}
	} else {
		PHY_ERROR(("%s: Switch control map(%s) is NOT found\n",
		           __FUNCTION__, rstr_swctrlmap_2g));
	}

	if (PHY_GETVAR_SLICE(pi, rstr_swctrlmapext_2g)) {
			for (i = 0; i < ACPHY_SWCTRL_NVRAM_PARAMS; i++) {
				pi_ac->sromi->nvram_femctrl.swctrlmapext_2g[i] =
					(uint32) PHY_GETINTVAR_ARRAY_SLICE(pi,
					rstr_swctrlmapext_2g, i);
			}
	}

	if (PHY_GETVAR_SLICE(pi, rstr_swctrlmap_5g)) {
			for (i = 0; i < ACPHY_SWCTRL_NVRAM_PARAMS; i++) {
				pi_ac->sromi->nvram_femctrl.swctrlmap_5g[i] =
					(uint32) PHY_GETINTVAR_ARRAY_SLICE(pi,
					rstr_swctrlmap_5g, i);
			}
	} else {
		PHY_ERROR(("%s: Switch control map(%s) is NOT found\n",
		           __FUNCTION__, rstr_swctrlmap_5g));
	}

	if (PHY_GETVAR_SLICE(pi, rstr_swctrlmapext_5g)) {
			for (i = 0; i < ACPHY_SWCTRL_NVRAM_PARAMS; i++) {
				pi_ac->sromi->nvram_femctrl.swctrlmapext_5g[i] =
					(uint32) PHY_GETINTVAR_ARRAY_SLICE(pi,
					"swctrlmapext_5g", i);
			}
	}

	pi_ac->sromi->nvram_femctrl.txswctrlmap_2g =
		(uint32) PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_txswctrlmap_2g, PAMODE_HI_LIN);

	pi_ac->sromi->nvram_femctrl.txswctrlmap_2g_mask =
		(uint16) PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_txswctrlmap_2g_mask, 0x3fff);

	pi_ac->sromi->nvram_femctrl.txswctrlmap_5g =
		(uint32) PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_txswctrlmap_5g, PAMODE_HI_LIN);

	if (PHY_GETVAR(pi, rstr_fem_table_init_val)) {
		pi_ac->sromi->femctrl_init_val_2g =
			(uint32) PHY_GETINTVAR_ARRAY(pi, rstr_fem_table_init_val, 0);
		pi_ac->sromi->femctrl_init_val_5g =
			(uint32) PHY_GETINTVAR_ARRAY(pi, rstr_fem_table_init_val, 1);
	} else {
		pi_ac->sromi->femctrl_init_val_2g = 0;
		pi_ac->sromi->femctrl_init_val_5g = 0;
	}
}

#ifdef ATE_BUILD
void
wlc_phy_gpaio_acphy(phy_info_t *pi, wl_gpaio_option_t option, int core)
{
	if (TINY_RADIO(pi)) {
		/* powerup gpaio block */
		MOD_RADIO_REG_TINY(pi, GPAIO_SEL2, core, gpaio_pu, 1);
		/* powerdown rcal, otherwise it conflicts */
		MOD_RADIO_REG_TINY(pi, RCAL_CFG_NORTH, core, rcal_pu, 0);

		/* To bring out various radio test signals on gpaio. */
		if (option == GPAIO_PMU_CLEAR)
			MOD_RADIO_REG_TINY(pi, GPAIO_SEL0, core, gpaio_sel_0to15_port, (0x1 << 0));
		else if (option == GPAIO_ICTAT_CAL) {
			MOD_RADIO_REG_TINY(pi, GPAIO_SEL0, core,
					gpaio_sel_0to15_port, 0x0);
			MOD_RADIO_REG_TINY(pi, GPAIO_SEL1, core,
					gpaio_sel_16to31_port, (0x1 << 11));
		}
		else
			MOD_RADIO_REG_TINY(pi, GPAIO_SEL0, core, gpaio_sel_0to15_port, (0x1 << 14));

		if (option != GPAIO_ICTAT_CAL)
			MOD_RADIO_REG_TINY(pi, GPAIO_SEL1, core, gpaio_sel_16to31_port, 0x0);
		switch (option) {
			case (GPAIO_PMU_AFELDO): {
				MOD_RADIO_REG_TINY(pi, PMU_CFG3, core, wlpmu_tsten, 0x01);
				MOD_RADIO_REG_TINY(pi, PMU_CFG1, core, wlpmu_ana_mux, 0x00);
				break;
			}
			case (GPAIO_PMU_TXLDO): {
				MOD_RADIO_REG_TINY(pi, PMU_CFG3, core, wlpmu_tsten, 0x01);
				MOD_RADIO_REG_TINY(pi, PMU_CFG1, core, wlpmu_ana_mux, 0x01);
				break;
			}
			case (GPAIO_PMU_VCOLDO): {
				MOD_RADIO_REG_TINY(pi, PMU_CFG3, core, wlpmu_tsten, 0x01);
				MOD_RADIO_REG_TINY(pi, PMU_CFG1, core, wlpmu_ana_mux, 0x02);
				break;
			}
			case GPAIO_PMU_LNALDO: {
				MOD_RADIO_REG_TINY(pi, PMU_CFG3, core, wlpmu_tsten, 0x01);
				MOD_RADIO_REG_TINY(pi, PMU_CFG1, core, wlpmu_ana_mux, 0x03);
				MOD_RADIO_REG_TINY(pi, PMU_CFG3, core, wlpmu_ana_mux_high, 0x00);
				break;
			}
			case GPAIO_PMU_ADCLDO: {
				MOD_RADIO_REG_TINY(pi, PMU_CFG3, core, wlpmu_tsten, 0x01);
				MOD_RADIO_REG_TINY(pi, PMU_CFG1, core, wlpmu_ana_mux, 0x03);
				MOD_RADIO_REG_TINY(pi, PMU_CFG3, core, wlpmu_ana_mux_high, 0x01);
				break;
			}
			case GPAIO_PMU_CLEAR: {
				  MOD_RADIO_REG_TINY(pi, PMU_CFG3, core, wlpmu_tsten, 0x00);
				  break;
			}
			case GPAIO_OFF: {
					MOD_RADIO_REG_TINY(pi, GPAIO_SEL2, core, gpaio_pu, 0);
					break;
			}
			default:
					break;
		}
	} else {
		if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
			uint8 c;
			/* Powerdown gpaio top and other GPAIO blocks, powerdown rcal,
			   clear all test point selection;
			 * powering up of gpaio blocks will be done inside the respective blocks
			 */
			MOD_RADIO_PLLREG_20698(pi, GPAIO_SEL6, 0,
				gpaio_top_pu, 0x0);
			MOD_RADIO_REG_20698(pi, GPAIO_SEL8, 1,
				gpaio_rcal_pu, 0x0);
			MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
				toptestmux_pu, 0x0);
			for (c = 0; c < 4; c++) {
				MOD_RADIO_REG_20698(pi, GPAIO_SEL2, c,
					anatestmux_pu, 0x0);
				MOD_RADIO_REG_20698(pi, GPAIO_SEL2, c,
					gpaio_pu, 0x0);
				if (c < 2) {
					MOD_RADIO_PLLREG_20698(pi, PLL_HVLDO4, c,
						ldo_1p8_vout_gpaio_test_en, 0x0);
					MOD_RADIO_PLLREG_20698(pi, PLL_REFDOUBLER1, 0,
						RefDoubler_ldo_1p8_vout_gpaio_test_en, 0x0);
					MOD_RADIO_PLLREG_20698(pi, PLL_LF1, c,
						rfpll_lf_en_vctrl_tp, 0x0);
					MOD_RADIO_PLLREG_20698(pi, PLL_VCO7, c,
						rfpll_vco_en_ampdet_vco1, 0x0);
					MOD_RADIO_PLLREG_20698(pi, PLL_VCO7, c,
						rfpll_vco_en_ampdet_vco2, 0x0);
					MOD_RADIO_PLLREG_20698(pi, PLL_VCO5, c,
						rfpll_vco_tmx_mode, 0x0);
				}
			}
			/* To bring out various radio test signals on gpaio. */
			switch (option) {
				case (GPAIO_RCAL): {
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						toptestmux_pu, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL8, 1,
						gpaio_rcal_pu, 0x1);
					MOD_RADIO_PLLREG_20698(pi, BG_REG3, 0,
						bg_rcal_pu, 0x1);
					MOD_RADIO_PLLREG_20698(pi, RCAL_CFG_NORTH, 0,
						rcal_pu, 0x1);
					break;
				}
				case (GPAIO_PMU_AFELDO): {
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						toptestmux_pu, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						gpaio2test2_sel, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 12);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_ana_mux, 0x0);
					break;
				}
				case (GPAIO_PMU_LPFTXLDO): {
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						toptestmux_pu, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						gpaio2test2_sel, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 12);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_ana_mux, 0x1);
					break;
				}
				case (GPAIO_PMU_LOGENLDO): {
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						toptestmux_pu, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						gpaio2test2_sel, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 12);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_ana_mux, 0x2);
					break;
				}
				case GPAIO_PMU_RXLDO2G: // no break
				case GPAIO_PMU_RXLDO5G: {
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						toptestmux_pu, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						gpaio2test2_sel, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 12);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_ana_mux, 0x3);
					MOD_RADIO_REG_20698(pi, RX2G_REG4, core,
						rx_ldo_out_en, 0x1);
					break;
				}
				case GPAIO_PMU_ADCLDO:{
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						toptestmux_pu, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						gpaio2test2_sel, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 12);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_ana_mux, 0x4);
					break;
				}
				case GPAIO_PMU_LDO1P6:{
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						toptestmux_pu, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						gpaio2test2_sel, 0x1);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 12);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					MOD_RADIO_REG_20698(pi, PMU_OP1, core,
						wlpmu_ana_mux, 0x5);
					break;
				}
				case GPAIO_IQDAC_BUF_DC_SETUP:{
					phy_ac_iqdac_buf_dc_setup(pi, core);
					break;
				}
				case GPAIO_IQDAC_BUF_DC_MEAS:{
					phy_ac_iqdac_buf_dc_meas(pi, core);
					break;
				}
				case GPAIO_IQDAC_BUF_DC_CLEAR:{
					phy_ac_iqdac_buf_dc_clear(pi, core);
					break;
				}
				case GPAIO_OFF: {
					MOD_RADIO_PLLREG_20698(pi, GPAIO_SEL6, 0,
						gpaio_top_pu, 0x0);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL8, 1,
						gpaio_rcal_pu, 0x0);
					MOD_RADIO_PLLREG_20698(pi, RCAL_CFG_NORTH, 0,
						rcal_pu, 0x0);
					MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2,
						toptestmux_pu, 0x0);
					for (c = 0; c < 4; c++) {
						MOD_RADIO_REG_20698(pi, GPAIO_SEL2, c,
							anatestmux_pu, 0x0);
						MOD_RADIO_REG_20698(pi, GPAIO_SEL2, c,
							gpaio_pu, 0x0);
						MOD_RADIO_REG_20698(pi, GPAIO_SEL0, c,
							gpaio_sel_0to15_port, 1 << 14);
						MOD_RADIO_REG_20698(pi, LOGEN_CORE_REG1, c,
							logen_pdet_en, 0x0);
						MOD_RADIO_REG_20698(pi, LOGEN_CORE_REG1, c,
							logen_2g_tx_pdet_en, 0x0);
						MOD_RADIO_REG_20698(pi, LOGEN_CORE_REG1, c,
							logen_2g_rx_pdet_en, 0x0);
						MOD_RADIO_REG_20698(pi, LOGEN_CORE_REG1, c,
							logen_5g_tx_rccr_pdet_en, 0x0);
						MOD_RADIO_REG_20698(pi, LOGEN_CORE_REG1, c,
							logen_5g_rx_rccr_pdet_en, 0x0);
						MOD_RADIO_REG_20698(pi, TX2G_MIX_REG0, c,
							tx2g_mx_pu_replica, 0x0);
						MOD_RADIO_REG_20698(pi, TX2G_PAD_REG0, c,
							pad2g_gpio_sw_pu, 0x0);
						MOD_RADIO_REG_20698(pi, TX5G_PAD_REG3, c,
							tx5g_pad_vcas_monitor_sw, 0x0);
						MOD_RADIO_REG_20698(pi, TX5G_PA_REG4, c,
							tx5g_pa_gpaio_seriessw, 0x0);
						MOD_RADIO_REG_20698(pi, LPF_REG8, c,
							lpf_sw_test_gpaio, 0x0);
						if (c < 2) {
							MOD_RADIO_PLLREG_20698(pi, PLL_HVLDO4, c,
								ldo_1p8_vout_gpaio_test_en, 0x0);
				MOD_RADIO_PLLREG_20698(pi, PLL_REFDOUBLER1, 0,
				                       RefDoubler_ldo_1p8_vout_gpaio_test_en,
				                       0x0);
							MOD_RADIO_PLLREG_20698(pi, PLL_LF1, c,
								rfpll_lf_en_vctrl_tp, 0x0);
							MOD_RADIO_PLLREG_20698(pi, PLL_VCO7, c,
								rfpll_vco_en_ampdet_vco1, 0x0);
							MOD_RADIO_PLLREG_20698(pi, PLL_VCO7, c,
								rfpll_vco_en_ampdet_vco2, 0x0);
							MOD_RADIO_PLLREG_20698(pi, PLL_VCO5, c,
								rfpll_vco_tmx_mode, 0x0);
						}
					}
					break;
				}
			default:
				break;
			}
		}  else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
			uint8 c;
			/* Powerdown gpaio top and other GPAIO blocks, powerdown rcal,
			   clear all test point selection;
			 * powering up of gpaio blocks will be done inside the respective blocks
			 */
			MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL4, 0, gpaio_top_sel_1, 0x0);
			MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL6, 0, gpaio_top_pu, 0x0);
			MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1, gpaio2test1_sel, 0x0);
			MOD_RADIO_REG_20708(pi, GPAIO_SEL8, 1, gpaio_rcal_pu, 0x0);
			MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1, gpaio2test2_sel, 0x0);
			MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1, gpaio2test3_sel, 0x0);
			MOD_RADIO_PLLREG_20708(pi, BG_REG2, 0, bg_ate_rcal_trim_en, 0x0);
			MOD_RADIO_PLLREG_20708(pi, BG_REG1, 0, bg_vbg_gpaio_sel, 0x0);
			MOD_RADIO_REG_20708(pi, RCCAL_CFG0, 1, rccal_gpio_en, 0x0);
			MOD_RADIO_PLLREG_20708(pi, BG_REG12, 0, ldo1p8_puok, 0x0);
			MOD_RADIO_PLLREG_20708(pi, RCAL_CFG_NORTH, 0, rcal_pu, 0x0);

			for (c = 0; c < 4; c++) {
				MOD_RADIO_REG_20708(pi, GPAIO_SEL0, c, gpaio_sel_0to15_port, 0x0);
				MOD_RADIO_REG_20708(pi, GPAIO_SEL2, c, gpaio_pu, 0x0);
				MOD_RADIO_REG_20708(pi, TX2G_MIX_REG0, c, tx_mx_gpaio_gmdb, 0x0);
				MOD_RADIO_REG_20708(pi, TX2G_MIX_REG0, c, tx_mx_gpaio_gmgb, 0x0);
				MOD_RADIO_REG_20708(pi, TX2G_MIX_REG0, c, tx_mx_gpaio_casgb, 0x0);
				MOD_RADIO_REG_20708(pi, TX2G_MIX_REG0, c, txdb_mx_pu_replica, 0x0);
				MOD_RADIO_REG_20708(pi, TX2G_PAD_REG0, c, tx_pad_gpaio_gmdb, 0x0);
				MOD_RADIO_REG_20708(pi, TX2G_PAD_REG0, c, tx_pad_gpaio_gmgb, 0x0);
				MOD_RADIO_REG_20708(pi, TX2G_PAD_REG1, c, tx_pad_gpaio_casgb, 0x0);
				MOD_RADIO_REG_20708(pi, TX2G_PAD_REG0, c, txdb_pad_gpio_sw_pu, 0x0);
				MOD_RADIO_REG_20708(pi, LPF_REG8, c, lpf_sw_test_gpaio, 0x0);
				MOD_RADIO_REG_20708(pi, RX2G_REG4, c, rx_ldo_out_en, 0x0);
				MOD_RADIO_REG_20708(pi, TIA_REG9, c,
					tia_nbrssi12_outi_ctrl_gpaio, 0);
				MOD_RADIO_REG_20708(pi, TESTBUF_CFG1, c, testbuf_GPIO_EN, 0x0);
				MOD_RADIO_REG_20708(pi, PMU_OP1, c, wlpmu_mux_tsel, 0x0);
				MOD_RADIO_REG_20708(pi, PMU_OP1, c, wlpmu_tsten, 0x0);

				if (c < 2) {
					MOD_RADIO_PLLREG_20708(pi, PLL_VCO5, c,
						rfpll_vco_tmx_mode, 0x0);
					MOD_RADIO_PLLREG_20708(pi, PLL_LF1, 0,
						rfpll_lf_en_vctrl_tp, 0x0);
					MOD_RADIO_PLLREG_20708(pi, PLL_CFG2, c,
						rfpll_gpaio_sel, 0x0);
					MOD_RADIO_PLLREG_20708(pi, LOGEN_REG4, c,
						logen_gpaio_pu, 0x0);
					MOD_RADIO_PLLREG_20708(pi, LOGEN_REG4, c,
						logen_pdet_pu, 0x0);
				}
			}
			/* To bring out various radio test signals on gpaio. */
			switch (option) {
				case (GPAIO_RCAL): {
					MOD_RADIO_REG_20708(pi, GPAIO_SEL8, 1,
						gpaio_rcal_pu, 0x1);
					MOD_RADIO_PLLREG_20708(pi, BG_REG3, 0,
						bg_rcal_pu, 0x1);
					MOD_RADIO_PLLREG_20708(pi, RCAL_CFG_NORTH, 0,
						rcal_pu, 0x1);
					break;
				}
				case (GPAIO_PMU_AFELDO): {
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 9);
					MOD_RADIO_REG_20708(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					break;
				}
				case (GPAIO_PMU_LPFTXLDO): {
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 9);
					MOD_RADIO_REG_20708(pi, PMU_OP1, core,
						wlpmu_mux_tsel, 0x1);
					MOD_RADIO_REG_20708(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					break;
				}
				case (GPAIO_PMU_LOGENLDO): {
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 9);
					MOD_RADIO_REG_20708(pi, PMU_OP1, core,
						wlpmu_mux_tsel, 0x2);
					MOD_RADIO_REG_20708(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					break;
				}
				case GPAIO_PMU_RXLDO2G: // no break
				case GPAIO_PMU_RXLDO5G: {
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 8);
					MOD_RADIO_REG_20708(pi, RX2G_REG4, core,
						rx_ldo_out_en, 0x1);
					MOD_RADIO_REG_20708(pi, RX2G_REG4, core,
						rx_ldo_out_en, 0x1);
					break;
				}
				case GPAIO_PMU_ADCLDO:{
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20708(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					break;
				}
				case GPAIO_PMU_LDO1P6:{
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 1 << 9);
					MOD_RADIO_REG_20708(pi, PMU_OP1, core,
						wlpmu_mux_tsel, 0x3);
					MOD_RADIO_REG_20708(pi, PMU_OP1, core,
						wlpmu_tsten, 0x1);
					break;
				}
				case GPAIO_BG_VCTAT_UNCAL:{
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL6, 0,
						gpaio_top_pu, 0x1);
					MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL4, 0,
						gpaio_top_sel_1, 0x2);
					MOD_RADIO_PLLREG_20708(pi, BG_REG1, 0,
						bg_vbg_gpaio_sel, 0x2);
					break;
				}
				case GPAIO_BG_VCTAT_CAL:{
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL6, 0,
						gpaio_top_pu, 0x1);
					MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL4, 0,
						gpaio_top_sel_1, 0xb);
					MOD_RADIO_PLLREG_20708(pi, BG_REG1, 0,
						bg_vbg_gpaio_sel, 0x1);
					break;
				}
				case GPAIO_BG_VPTAT:{
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL6, 0,
						gpaio_top_pu, 0x1);
					MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL4, 0,
						gpaio_top_sel_1, 0x4);
					MOD_RADIO_PLLREG_20708(pi, BG_REG1, 0,
						bg_vbg_gpaio_sel, 0x4);
					break;
				}
				case GPAIO_TEMPSENSE_VBG_P:{
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 0x20);
					MOD_RADIO_REG_20708(pi, TEMPSENSE_OVR1, core,
						ovr_tempsense_pu, 0x1);
					MOD_RADIO_REG_20708(pi, TEMPSENSE_OVR1, core,
						ovr_tempsense_sel_Vbe_Vbg, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_OVR1, core,
						ovr_testbuf_PU, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_OVR1, core,
						ovr_testbuf_sel_test_port, 0x1);
					MOD_RADIO_REG_20708(pi, TEMPSENSE_CFG, core,
						tempsense_pu, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_CFG1, core,
						testbuf_PU, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_CFG1, core,
						testbuf_GPIO_EN, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_CFG1, core,
						testbuf_sel_test_port, 0x1);
					MOD_RADIO_REG_20708(pi, TEMPSENSE_CFG, core,
						tempsense_sel_Vbe_Vbg, 0x0);
					break;
				}
				case GPAIO_TEMPSENSE_VBE_P:{
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL2, core,
						gpaio_pu, 0x1);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL0, core,
						gpaio_sel_0to15_port, 0x20);
					MOD_RADIO_REG_20708(pi, TEMPSENSE_OVR1, core,
						ovr_tempsense_pu, 0x1);
					MOD_RADIO_REG_20708(pi, TEMPSENSE_OVR1, core,
						ovr_tempsense_sel_Vbe_Vbg, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_OVR1, core,
						ovr_testbuf_PU, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_OVR1, core,
						ovr_testbuf_sel_test_port, 0x1);
					MOD_RADIO_REG_20708(pi, TEMPSENSE_CFG, core,
						tempsense_pu, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_CFG1, core,
						testbuf_PU, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_CFG1, core,
						testbuf_GPIO_EN, 0x1);
					MOD_RADIO_REG_20708(pi, TESTBUF_CFG1, core,
						testbuf_sel_test_port, 0x1);
					MOD_RADIO_REG_20708(pi, TEMPSENSE_CFG, core,
						tempsense_sel_Vbe_Vbg, 0x1);
					break;
				}
				case GPAIO_IQDAC_BUF_DC_SETUP:{
					phy_ac_iqdac_buf_dc_setup(pi, core);
					break;
				}
				case GPAIO_IQDAC_BUF_DC_MEAS:{
					phy_ac_iqdac_buf_dc_meas(pi, core);
					break;
				}
				case GPAIO_IQDAC_BUF_DC_CLEAR:{
					phy_ac_iqdac_buf_dc_clear(pi, core);
					break;
				}
				case GPAIO_OFF: {
					MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL4, 0,
						gpaio_top_sel_1, 0x0);
					MOD_RADIO_PLLREG_20708(pi, GPAIO_SEL6, 0,
						gpaio_top_pu, 0x0);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test1_sel, 0x0);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL8, 1,
						gpaio_rcal_pu, 0x0);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test2_sel, 0x0);
					MOD_RADIO_REG_20708(pi, GPAIO_SEL9, 1,
						gpaio2test3_sel, 0x0);
					MOD_RADIO_PLLREG_20708(pi, BG_REG2, 0,
						bg_ate_rcal_trim_en, 0x0);
					MOD_RADIO_PLLREG_20708(pi, BG_REG1, 0,
						bg_vbg_gpaio_sel, 0x0);
					MOD_RADIO_REG_20708(pi, RCCAL_CFG0, 1,
						rccal_gpio_en, 0x0);
					MOD_RADIO_PLLREG_20708(pi, BG_REG12, 0,
						ldo1p8_puok, 0x0);
					MOD_RADIO_PLLREG_20708(pi, RCAL_CFG_NORTH, 0,
						rcal_pu, 0x0);

					for (c = 0; c < 4; c++) {
						MOD_RADIO_REG_20708(pi, GPAIO_SEL2, c,
							txrxiq_bus_sel, 0x0);
						MOD_RADIO_REG_20708(pi, GPAIO_SEL0, c,
							gpaio_sel_0to15_port, 0);
						MOD_RADIO_REG_20708(pi, GPAIO_SEL2, c,
							gpaio_pu, 0x0);
						MOD_RADIO_REG_20708(pi, TX2G_MIX_REG0, c,
							tx_mx_gpaio_gmdb, 0x0);
						MOD_RADIO_REG_20708(pi, TX2G_MIX_REG0, c,
							tx_mx_gpaio_gmgb, 0x0);
						MOD_RADIO_REG_20708(pi, TX2G_MIX_REG0, c,
							tx_mx_gpaio_casgb, 0x0);
						MOD_RADIO_REG_20708(pi, TX2G_MIX_REG0, c,
							txdb_mx_pu_replica, 0x0);
						MOD_RADIO_REG_20708(pi, TX2G_PAD_REG0, c,
							tx_pad_gpaio_gmdb, 0x0);
						MOD_RADIO_REG_20708(pi, TX2G_PAD_REG0, c,
							tx_pad_gpaio_gmgb, 0x0);
						MOD_RADIO_REG_20708(pi, TX2G_PAD_REG1, c,
							tx_pad_gpaio_casgb, 0x0);
						MOD_RADIO_REG_20708(pi, TX2G_PAD_REG0, c,
							txdb_pad_gpio_sw_pu, 0x0);
						MOD_RADIO_REG_20708(pi, LPF_REG8, c,
							lpf_sw_test_gpaio, 0x0);
						MOD_RADIO_REG_20708(pi, RX2G_REG4, c,
							rx_ldo_out_en, 0x0);
						MOD_RADIO_REG_20708(pi, TIA_REG9, c,
							tia_nbrssi12_outi_ctrl_gpaio, 0x0);
						MOD_RADIO_REG_20708(pi, TESTBUF_CFG1, c,
							testbuf_GPIO_EN, 0x0);
						MOD_RADIO_REG_20708(pi, PMU_OP1, core,
							wlpmu_mux_tsel, 0x0);
						MOD_RADIO_REG_20708(pi, PMU_OP1, core,
							wlpmu_tsten, 0x0);

						if (c < 2) {
							MOD_RADIO_PLLREG_20708(pi, PLL_VCO5, c,
								rfpll_vco_tmx_mode, 0x0);
							MOD_RADIO_PLLREG_20708(pi, PLL_LF1, c,
								rfpll_lf_en_vctrl_tp, 0x0);
							MOD_RADIO_PLLREG_20708(pi, PLL_CFG2, c,
								rfpll_gpaio_sel, 0x0);
							MOD_RADIO_PLLREG_20708(pi, LOGEN_REG4, c,
								logen_gpaio_pu, 0x0);
							MOD_RADIO_PLLREG_20708(pi, LOGEN_REG4, c,
								logen_pdet_pu, 0x0);
						}
					}
					break;
				}
			default:
				break;
			}
		} else {
			PHY_ERROR(("wlc_phy_gpaio_acphy not supported in this rev."));
			ASSERT(FALSE);
		}
	}
}

static void
phy_ac_gpaio_gpaioconfig(phy_type_misc_ctx_t *ctx, wl_gpaio_option_t option, int core)
{
	bool suspend;
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;

	/* Suspend MAC if haven't done so */
	wlc_phy_conditional_suspend(pi, &suspend);
	wlc_phy_gpaio_acphy(pi, option, core);

	/* Resume MAC */
	wlc_phy_conditional_resume(pi, &suspend);

	return;
}

static void phy_ac_iqdac_buf_dc_setup(phy_info_t *pi, int option)
{
	/* Input argument option was re-purposed from "core" of wlc_phy_gpaio_acphy. */
	uint8 core_idx;
	uint16 didq = 0;
	uint16 bbmult = 0;

	/* Save regs before they are modified */
	FOREACH_CORE(pi, core_idx) {
	    save_regs_dc_meas.txdac_reg0[core_idx] = READ_RADIO_REG_20698(pi, TXDAC_REG0, core_idx);
	    save_regs_dc_meas.txdac_reg1[core_idx] = READ_RADIO_REG_20698(pi, TXDAC_REG1, core_idx);
	    save_regs_dc_meas.tx2g_mx[core_idx] = READ_RADIO_REG_20698(pi, TX2G_MIX_REG2, core_idx);
	    save_regs_dc_meas.tx5g_mx[core_idx] = READ_RADIO_REG_20698(pi, TX5G_MIX_REG1, core_idx);
	    save_regs_dc_meas.pmu_op1[core_idx] = READ_RADIO_REG_20698(pi, PMU_OP1, core_idx);
	    save_regs_dc_meas.lpf_reg8[core_idx] = READ_RADIO_REG_20698(pi, LPF_REG8, core_idx);
	    save_regs_dc_meas.lpf_reg9[core_idx] = READ_RADIO_REG_20698(pi, LPF_REG9, core_idx);

	    if ((option == 2) || (option == 4)) {
	        save_regs_dc_meas.lpf_ovr1[core_idx] = READ_RADIO_REG_20698(pi, LPF_OVR1, core_idx);
	        save_regs_dc_meas.lpf_ovr2[core_idx] = READ_RADIO_REG_20698(pi, LPF_OVR2, core_idx);
	        save_regs_dc_meas.lpf_reg7[core_idx] = READ_RADIO_REG_20698(pi, LPF_REG7, core_idx);
	    }

	    save_regs_dc_meas.txiqlocal_d[core_idx] = wlc_acphy_get_tx_locc(pi, core_idx);
	    wlc_phy_get_tx_bbmult_acphy(pi, &save_regs_dc_meas.bbmult[core_idx], core_idx);
	}
	save_regs_dc_meas.bg_reg10 = READ_RADIO_PLLREG_20698(pi, BG_REG10, 0);
	save_regs_dc_meas.bg_reg9 = READ_RADIO_PLLREG_20698(pi, BG_REG9, 0);

	FOREACH_CORE(pi, core_idx) {
	    MOD_RADIO_REG_20698(pi, TXDAC_REG0, core_idx, iqdac_buf_cmsel, 1);
	    MOD_RADIO_REG_20698(pi, TXDAC_REG0, core_idx, iqdac_buf_bw, 4);
	    MOD_RADIO_REG_20698(pi, TXDAC_REG1, core_idx, iqdac_lowcm_en, 1);
	    MOD_RADIO_REG_20698(pi, TXDAC_REG0, core_idx, iqdac_attn, 3);
	    MOD_RADIO_REG_20698(pi, TXDAC_REG1, core_idx, iqdac_buf_op_cur, 0);
	    MOD_RADIO_REG_20698(pi, TXDAC_REG1, core_idx, iqdac_buf_suref_ctrl, 0);
	    MOD_RADIO_REG_20698(pi, TX2G_MIX_REG2, core_idx, tx2g_mx_idac_bb, 15);
	    MOD_RADIO_REG_20698(pi, TX5G_MIX_REG1, core_idx, tx5g_idac_mx_bbdc, 26);
	    if ((option == 1) || (option == 2)) {
	        MOD_RADIO_REG_20698(pi, PMU_OP1, core_idx, wlpmu_TXldo_adj, 7);
	    } else if ((option == 3) || (option == 4)) {
	        MOD_RADIO_REG_20698(pi, PMU_OP1, core_idx, wlpmu_TXldo_adj, 4);
	    }

	    if ((option == 2) || (option == 4)) {
	        MOD_RADIO_REG_20698(pi, LPF_REG7, core_idx, lpf_sw_bq1_bq2, 0x0);
	        MOD_RADIO_REG_20698(pi, LPF_OVR2, core_idx, ovr_lpf_sw_bq1_bq2,	0x1);
	        MOD_RADIO_REG_20698(pi, LPF_REG7, core_idx, lpf_sw_bq2_adc, 0x0);
	        MOD_RADIO_REG_20698(pi, LPF_OVR1, core_idx, ovr_lpf_sw_bq2_adc,	0x1);
	        MOD_RADIO_REG_20698(pi, LPF_REG7, core_idx, lpf_sw_bq1_adc, 0x0);
	        MOD_RADIO_REG_20698(pi, LPF_OVR2, core_idx, ovr_lpf_sw_bq1_adc,	0x1);
	        MOD_RADIO_REG_20698(pi, LPF_REG7, core_idx, lpf_sw_dac_bq2, 0x0);
	        MOD_RADIO_REG_20698(pi, LPF_OVR1, core_idx, ovr_lpf_sw_dac_bq2,	0x1);
	        MOD_RADIO_REG_20698(pi, LPF_REG7, core_idx, lpf_sw_bq2_rc, 0x0);
	        MOD_RADIO_REG_20698(pi, LPF_OVR1, core_idx, ovr_lpf_sw_bq2_rc,	0x1);
	        MOD_RADIO_REG_20698(pi, LPF_REG7, core_idx, lpf_sw_dac_rc, 0x0);
	        MOD_RADIO_REG_20698(pi, LPF_OVR1, core_idx, ovr_lpf_sw_dac_rc,	0x1);
	        MOD_RADIO_REG_20698(pi, LPF_REG7, core_idx, lpf_sw_aux_adc, 0x0);
	        MOD_RADIO_REG_20698(pi, LPF_OVR2, core_idx, ovr_lpf_sw_aux_adc,	0x1);
	    }

	    wlc_acphy_set_tx_locc(pi, didq, core_idx);
	    wlc_phy_set_tx_bbmult_acphy(pi, &bbmult, core_idx);
	}

	MOD_RADIO_PLLREG_20698(pi, BG_REG10, 0, bg_wlpmu_bypcal, 1);
	if ((option == 1) || (option == 2)) {
	    MOD_RADIO_PLLREG_20698(pi, BG_REG9, 0, bg_wlpmu_cal_mancodes, 0);
	} else if ((option == 3) || (option == 4)) {
	    MOD_RADIO_PLLREG_20698(pi, BG_REG9, 0, bg_wlpmu_cal_mancodes, 0x20);
	}
}

static void phy_ac_iqdac_buf_dc_meas(phy_info_t *pi, int core)
{
	int core_idx = (core >> 2);
	int rail = core & 3;
	uint16 lpf_sw_rc_test_val = 0, lpf_sw_test_gpaio_val = 0;

	/*
	 * Variable core is overloaded with:
	 * 	Bits 2~3:	actual core id;
	 *	Bit 1:   	rail (I/Q);
	 *	Bit 0:   	swap.
	 * swap = 0 ==>, (I/Q)p goes to GPAIO, (I/Q)n goes to eTSSI
	 * swap = 1 ==>, (I/Q)n goes to GPAIO, (I/Q)p goes to eTSSI
	*/
	/* rail == 0 ==> I, rail == 1 ==> I swap */
	/* rail == 2 ==> Q, rail == 3 ==> Q swap */

	ASSERT((core >= 0) && (core <= 15));

	switch (rail) {
		case 0:
			lpf_sw_rc_test_val = 1;
			lpf_sw_test_gpaio_val = 1;
			break;
		case 1:
			lpf_sw_rc_test_val = 0x20;
			lpf_sw_test_gpaio_val = 0x2;
			break;
		case 2:
			lpf_sw_rc_test_val = 0x400;
			lpf_sw_test_gpaio_val = 0x4;
			break;
		case 3:
			lpf_sw_rc_test_val = 0x8000;
			lpf_sw_test_gpaio_val = 0x8;
			break;
	}

	MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2, toptestmux_pu, 1);
	MOD_RADIO_REG_20698(pi, GPAIO_SEL9, 2, gpaio2test2_sel, 1);
	MOD_RADIO_REG_20698(pi, GPAIO_SEL0, core_idx, gpaio_sel_0to15_port, 64);
	MOD_RADIO_REG_20698(pi, GPAIO_SEL2, core_idx, gpaio_pu, 1);
	MOD_RADIO_REG_20698(pi, LPF_REG9, core_idx, lpf_sw_rc_test, lpf_sw_rc_test_val);
	MOD_RADIO_REG_20698(pi, LPF_REG8, core_idx, lpf_sw_adc_test, 0);
	MOD_RADIO_REG_20698(pi, LPF_REG8, core_idx, lpf_sw_test_gpaio, lpf_sw_test_gpaio_val);
}

static void phy_ac_iqdac_buf_dc_clear(phy_info_t *pi, int option)
{
	/* Input argument option was re-purposed from "core" of wlc_phy_gpaio_acphy. */
	uint8 core;

	FOREACH_CORE(pi, core) {
	    WRITE_RADIO_REG_20698(pi, TXDAC_REG0, core, save_regs_dc_meas.txdac_reg0[core]);
	    WRITE_RADIO_REG_20698(pi, TXDAC_REG1, core, save_regs_dc_meas.txdac_reg1[core]);
	    WRITE_RADIO_REG_20698(pi, TX2G_MIX_REG2, core, save_regs_dc_meas.tx2g_mx[core]);
	    WRITE_RADIO_REG_20698(pi, TX5G_MIX_REG1, core, save_regs_dc_meas.tx5g_mx[core]);
	    WRITE_RADIO_REG_20698(pi, PMU_OP1, core, save_regs_dc_meas.pmu_op1[core]);
	    WRITE_RADIO_REG_20698(pi, LPF_REG8, core, save_regs_dc_meas.lpf_reg8[core]);
	    WRITE_RADIO_REG_20698(pi, LPF_REG9, core, save_regs_dc_meas.lpf_reg9[core]);

	    if ((option == 2) || (option == 4)) {
	        WRITE_RADIO_REG_20698(pi, LPF_OVR1, core, save_regs_dc_meas.lpf_ovr1[core]);
	        WRITE_RADIO_REG_20698(pi, LPF_OVR2, core, save_regs_dc_meas.lpf_ovr2[core]);
	        WRITE_RADIO_REG_20698(pi, LPF_REG7, core, save_regs_dc_meas.lpf_reg7[core]);
	    }

	    wlc_acphy_set_tx_locc(pi, save_regs_dc_meas.txiqlocal_d[core], core);
	    wlc_phy_set_tx_bbmult_acphy(pi, &save_regs_dc_meas.bbmult[core], core);
	}
	WRITE_RADIO_PLLREG_20698(pi, BG_REG10, 0, save_regs_dc_meas.bg_reg10);
	WRITE_RADIO_PLLREG_20698(pi, BG_REG9, 0, save_regs_dc_meas.bg_reg9);
}
#endif /* ATE_BUILD */

void
wlc_phy_cals_mac_susp_en_other_cr(phy_info_t *pi, bool suspend)
{
}

static bool
phy_ac_misc_get_rxgainerr(phy_type_misc_ctx_t *ctx, int16 *gainerr)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;
	bool srom_isempty[PHY_CORE_MAX] = { 0 };
	uint8 core;
#if BAND5G || BAND6G
	uint8 core_freq_segment_map, sb;
	uint16 channel;
	chanspec_t chanspec = pi->radio_chanspec;
	uint8 ch5g_sb[3] = {48, 64, 128};
	channel = CHSPEC_CHANNEL(chanspec);
	FOREACH_CORE(pi, core) {
		/* For 80P80, retrieve Primary/Secondary based on the mapping */
		if (CHSPEC_IS8080(chanspec)) {
			core_freq_segment_map = phy_ac_chanmgr_get_data
				(info->aci->chanmgri)->core_freq_mapping[core];
			if (PRIMARY_FREQ_SEGMENT == core_freq_segment_map) {
				channel = wf_chspec_primary80_channel(chanspec);
			} else if (SECONDARY_FREQ_SEGMENT == core_freq_segment_map) {
				channel = wf_chspec_secondary80_channel(chanspec);
			}
		}

		if (CHSPEC_IS6G(pi->radio_chanspec)) {
			if ((ACMAJORREV_130(pi->pubpi->phy_rev)) && (RADIOMAJORREV(pi) >= 2)) {
				sb = (CHSPEC_CHANNEL(pi->radio_chanspec) < 33) ? 0:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 65) ? 1:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 101) ? 2:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 141) ? 3:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 161) ? 4:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 201) ? 5: 6;
			} else {
				sb = (CHSPEC_CHANNEL(pi->radio_chanspec) < 33) ? 0:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 65) ? 1:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 97) ? 2:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 129) ? 3:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 161) ? 4:
				     (CHSPEC_CHANNEL(pi->radio_chanspec) < 193) ? 5: 6;
			}
			gainerr[core] = (int16) pi->rxgainerr_6g[core][sb];
			pi->hwrssioffset_cmn[core] = pi->hwrssioffset_cmn_5g_6g[core][sb];
			pi->hwrssioffset_trt[core] = pi->hwrssioffset_trt_5g_6g[core][sb];
			srom_isempty[core] = pi->rxgainerr6g_isempty[sb];
		} else if (CHSPEC_IS5G(pi->radio_chanspec)) {
			/* 5G */
			if ((ACMAJORREV_130(pi->pubpi->phy_rev)) && (RADIOMAJORREV(pi) >= 2)) {
				ch5g_sb[2] = 144;
			}
			if (channel <= ch5g_sb[0]) {
				/* 5G-low: channels 36 through 48 */
				gainerr[core] = (int16) pi->rxgainerr_5gl[core];
				pi->hwrssioffset_cmn[core] = pi->hwrssioffset_cmn_5g_6g[core][0];
				pi->hwrssioffset_trt[core] = pi->hwrssioffset_trt_5g_6g[core][0];
				srom_isempty[core] = pi->rxgainerr5gl_isempty;
			} else if (channel <= ch5g_sb[1]) {
				/* 5G-mid: channels 52 through 64 */
				gainerr[core] = (int16) pi->rxgainerr_5gm[core];
				pi->hwrssioffset_cmn[core] = pi->hwrssioffset_cmn_5g_6g[core][1];
				pi->hwrssioffset_trt[core] = pi->hwrssioffset_trt_5g_6g[core][1];
				srom_isempty[core] = pi->rxgainerr5gm_isempty;
			} else if (channel <= ch5g_sb[2]) {
				/* 5G-high: channels 100 through 128 */
				gainerr[core] = (int16) pi->rxgainerr_5gh[core];
				pi->hwrssioffset_cmn[core] = pi->hwrssioffset_cmn_5g_6g[core][2];
				pi->hwrssioffset_trt[core] = pi->hwrssioffset_trt_5g_6g[core][2];
				srom_isempty[core] = pi->rxgainerr5gh_isempty;
			} else {
				/* 5G-upper: channels 132 and above */
				gainerr[core] = (int16) pi->rxgainerr_5gu[core];
				pi->hwrssioffset_cmn[core] = pi->hwrssioffset_cmn_5g_6g[core][3];
				pi->hwrssioffset_trt[core] = pi->hwrssioffset_trt_5g_6g[core][3];
				srom_isempty[core] = pi->rxgainerr5gu_isempty;
			}
		} else {
			/* 2G */
			gainerr[core] = (int16) pi->rxgainerr_2g[core];
			pi->hwrssioffset_cmn[core] = pi->hwrssioffset_cmn_2g[core];
			pi->hwrssioffset_trt[core] = pi->hwrssioffset_trt_2g[core];
			srom_isempty[core] = pi->rxgainerr2g_isempty;
		}
	}
#else
	/* 2G */
	FOREACH_CORE(pi, core) {
		gainerr[core] = (int16) pi->rxgainerr_2g[core];
		pi->hwrssioffset_cmn[core] = pi->hwrssioffset_cmn_2g[core];
		pi->hwrssioffset_trt[core] = pi->hwrssioffset_trt_2g[core];
		srom_isempty[core] = pi->rxgainerr2g_isempty;
	}
#endif /* BAND5G */
	/* For 80P80, retrun only primary channel value */

	if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
		/* Parins "-GainErr S7.0" to uCode for Trigger frame RSSI Compensation */
		int16 tmp;
		tmp = ((gainerr[0] < 0 ? (-gainerr[0] >> 1) : -(gainerr[0] >> 1)) & 0x00FF) +
			((gainerr[1] < 0 ? (-gainerr[1] << 7) : -(gainerr[1] << 7)) & 0xFF00);
		wlapi_bmac_write_shm(pi->sh->physhim, M_RSSICOR_BLK(pi), tmp);
		tmp = ((gainerr[2] < 0 ? (-gainerr[2] >> 1) : -(gainerr[2] >> 1)) & 0x00FF) +
			((gainerr[3] < 0 ? (-gainerr[3] << 7) : -(gainerr[3] << 7)) & 0xFF00);
		wlapi_bmac_write_shm(pi->sh->physhim, (M_RSSICOR_BLK(pi)+2), tmp);
	}
	return srom_isempty[0];
}

#ifdef PHY_DUMP_BINARY
/* The function is forced to RAM since it accesses non-const tables */
static int
BCMRAMFN(phy_ac_misc_getlistandsize)(phy_type_misc_ctx_t *ctx,
         phyradregs_list_t **phyreglist, uint16 *phyreglist_sz)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;
	BCM_REFERENCE(pi);
	if (ACREV_IS(pi->pubpi->phy_rev, 24)) {
		*phyreglist = (phyradregs_list_t *) &dot11acphy_regs_rev24[0];
		*phyreglist_sz = sizeof(dot11acphy_regs_rev24);
	} else {
		PHY_INFORM(("%s: wl%d: unsupported AC phy rev %d\n",
			__FUNCTION__,  pi->sh->unit,  pi->pubpi->phy_rev));
		return BCME_UNSUPPORTED;
	}

	return BCME_OK;
}
#endif /* PHY_DUMP_BINARY */

int
phy_ac_misc_set_rud_agc_enable(phy_ac_misc_info_t *misci, int32 int_val)
{
	misci->rud_agc_enable = (bool)int_val;
	return BCME_OK;
}

int
phy_ac_misc_get_rud_agc_enable(phy_ac_misc_info_t *misci, int32 *ret_int_ptr)
{
	*ret_int_ptr = misci->rud_agc_enable;
	return BCME_OK;
}

#ifdef ATE_BUILD
static void
phy_type_misc_disable_ate_gpiosel(phy_type_misc_ctx_t *ctx)
{
	PHY_ERROR(("phy_type_misc_disable_ate_gpiosel not supported in this rev."));
	/* Not supported for other PHYs yet */
	ASSERT(FALSE);
}
#endif /* ATE_BUILD */

static uint8
phy_ac_misc_get_bfe_ndp_recvstreams(phy_type_misc_ctx_t *ctx)
{
	phy_ac_misc_info_t *misc_info = (phy_ac_misc_info_t *) ctx;
	phy_info_t *pi = misc_info->pi;

	BCM_REFERENCE(pi);

	/* AC major 4, 32 and 40 can recv 3 */
	if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
		ACMAJORREV_33(pi->pubpi->phy_rev) || ACMAJORREV_GE37(pi->pubpi->phy_rev)) {
		return 3;
	} else {
		return 2;
	}
}

static void
phy_update_rxldpc_acphy(phy_type_misc_ctx_t *ctx, bool ldpc)
{
	phy_ac_misc_info_t *misc_info = (phy_ac_misc_info_t *) ctx;
	phy_info_t *pi = misc_info->pi;
	phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
	bool suspend = FALSE;

	PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

	if (ldpc != pi_ac->misci->ac_rxldpc_override) {
		pi_ac->misci->ac_rxldpc_override = ldpc;

		/* Suspend MAC if haven't done so */
		wlc_phy_conditional_suspend(pi, &suspend);

		MOD_PHYREG(pi, HTSigTones, support_ldpc, (ldpc) ? 1 : 0);

		/* Resume MAC */
		wlc_phy_conditional_resume(pi, &suspend);
	}
}

static void
phy_ac_misc_set_preamble_override(phy_type_misc_ctx_t *ctx, int8 val)
{
	phy_ac_misc_info_t *misc_info = (phy_ac_misc_info_t *) ctx;
	phy_info_t *pi = misc_info->pi;

	if (val != WLC_N_PREAMBLE_MIXEDMODE) {
		PHY_ERROR(("wl%d:%s: AC Phy: Ignore request to set preamble mode %d\n",
			pi->sh->unit, __FUNCTION__, val));
		return;
	}
	pi->n_preamble_override = val;
}

#ifdef WFD_PHY_LL
static void
phy_ac_misc_wfdll_chan_active(phy_type_misc_ctx_t *ctx, bool chan_active)
{
	phy_ac_misc_info_t *misc_info = (phy_ac_misc_info_t *) ctx;
	phy_info_t *pi = misc_info->pi;

	pi->wfd_ll_chan_active = chan_active;
}
#endif /* WFD_PHY_LL */

static uint16
phy_ac_misc_classifier(phy_type_misc_ctx_t *ctx, uint16 mask, uint16 val)
{
	phy_ac_misc_info_t *info = (phy_ac_misc_info_t *)ctx;
	phy_info_t *pi = info->pi;

	BCM_REFERENCE(mask);

	return phy_rxgcrs_sel_classifier(pi, val);
}

static void
enable_vasip_sc(phy_info_t *pi)
{
	const uint16 fix0 = 0;
	const uint16 fix1 = 0x22;
	const uint16 start_addr = 0x0800;
	const uint16 fix4 = 1;

	uint16 end_addr;
	if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
		/* 63178: max VASIP addr=0x20000 (16-bit), this addr is 256-bit */
		end_addr = 0x1fff;
	} else {
		end_addr = 0x47ff;
	}

	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 0, 16, &fix0);
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 1, 16, &fix1);
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 2, 16, &start_addr);
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 3, 16, &end_addr);
	wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 0, 16, &fix4);
	/*
	#reset svmp sample collect
	axphy_write_table svmpsampcol { 0x0} 0x0 ;
	axphy_write_table svmpsampcol { 0x22 } 0x1 ; #set packing mode to 2
	#start address
	axphy_write_table svmpsampcol { 0x0800} 0x2 ;
	#end address -for total samples = 65528
	axphy_write_table svmpsampcol { 0x47ff } 0x3 ;
	#axphy_write_table svmpsampcol { 0x0} 0x4 ;
	axphy_write_table svmpsampcol { 0x1} 0x0 ;
	*/
	MOD_PHYREG(pi, SvmpSampColphy1_svmp_ip_phy1_mux_sel, toa_en, 1);
	MOD_PHYREG(pi, SpecAnaControl, toa_en, 1);
	MOD_PHYREG(pi, dacClkCtrl, vasipClkEn, 1);
	MOD_PHYREG(pi, dacClkCtrl, vasipAutoClkEn, 1);
	MOD_PHYREG(pi, rx1misc_0, rx1toaClkEn, 1);
	MOD_PHYREG(pi, rx1misc_0, rx1sampColClkEn, 1);
	MOD_PHYREG(pi, dacClkCtrl, svmpsampcolClkEn, 1);
}

/*      The following function loads the VASIP sample capture.
	It will be called from: wl phy_vasip_sc {arg}
	{arg} sets the capture config and it has 32 bits
	bits 0-3 determine the second MUX value: 0 to 13 are currently defined.
	bits 4&5 determine the trigger: 00 means force trigger, 01 means packet proc transition
	bits 6&7 determinies the packing: 00-->No packign, 01-->2, 10-->4, 11-->8
	bits 8,9 are uses for core selection. Core selection is
	semi-statistically done: core-->0, 1-->1, 2-->2, 3-->3
	bits 10-14 are the initial packet proc state for packet proc transition
	bits 15-19 are the final state for packet proc transtion
	bits 20-23 are the mainMUX
*/
void
wlc_vasip_sample_capture_set(phy_info_t *pi, int32 val)
{
	const uint16 pack0 = 0x20;
	const uint16 pack1 = 0x21;
	const uint16 pack2 = 0x22;
	const uint16 pack3 = 0x23;
	uint8 mainMUX;

	int32 secMux, trigType, packingFactor, core, initialState, finalState;
	secMux = val & 0xf;
	trigType = (val>>4) & 0x3;
	packingFactor = (val>>6) & 0x3;
	core = (val >> 8) & 0x3;
	initialState = (val >> 10) & 0x1f;
	finalState = (val >> 15) & 0x1f;
	mainMUX = (val >> 20) & 0xf;

	if (mainMUX == 8) {
		// Mode 5_28nm for WBPAPD.
		MOD_PHYREG(pi, SdFeClkStatus, en_wbcal_capture_fullrate, 1);

		// If not put into reset, VASIP activity interferes with sample capture.
		// Most likely, other sample capture modes using SVMP memory also need the reset.
		// But for safety against regression, only reset VASIP in 5_28nm mode for now.
		phy_vasip_reset_proc(pi, 1);
	}

	enable_vasip_sc(pi);

	if (packingFactor == 0x0) {
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 1, 16, &pack0);
	} else if (packingFactor == 0x1) {
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 1, 16, &pack1);
	} else if (packingFactor == 0x2) {
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 1, 16, &pack2);
	} else if (packingFactor == 0x3) {
		wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPSAMPCOL, 1, 1, 16, &pack3);
	}

	if (mainMUX == 8) {
		MOD_PHYREG(pi, SvmpSampColSpecAnaControl, toa_en, 1);
		MOD_PHYREG(pi, SvmpSampColSpecAnaControl, toa_sampling_rate, 1); // allow full rate
		MOD_PHYREG(pi, SvmpSampColphy1_svmp_ip_phy1_mux_sel, phy1_svmp_ip_phy1_mux_sel,
			mainMUX);
	} else {
		MOD_PHYREG(pi, SvmpSampColphy1_svmp_ip_phy1_mux_sel, phy1_svmp_ip_phy1_mux_sel, 5);
		MOD_PHYREG(pi, SvmpSampColphy1_svmp_ip_phy1_mux_sel, phy2vasip_legSampleCollect, 1);
		MOD_PHYREG(pi, SpecAnaControl, toa_en, 0);
		MOD_PHYREG(pi, SpecAnaDataCollect, specAnaMode, 0);
	}
	MOD_PHYREG(pi, AdcDataCollect, sampSel, secMux);
	MOD_PHYREG(pi, SampleCapture, en_43684b0_sample_capture, 1);
	MOD_PHYREG(pi, SampleCapture, sample_capture_en, 1);
	MOD_PHYREG(pi, SampleCapture, core_sel_0, core);
	if (!(mainMUX == 8)) {
		MOD_PHYREG(pi, SampleCapture, core_sel_1, 1);
		MOD_PHYREG(pi, SampleCapture, core_sel_2, 2);
		MOD_PHYREG(pi, SampleCapture, core_sel_3, 3);
	}
	MOD_PHYREG(pi, SvmpSampColaoatrig1caplen, aoatrig1caplen, 0);
	MOD_PHYREG(pi, SvmpSampColaoatrig1caplenHigh, aoatrig1caplenHigh, 1);
	if (trigType == 0) {
		//Force trigger once
		MOD_PHYREG(pi, SvmpSampColSpecAnaControl, force_trigger, 0);
		MOD_PHYREG(pi, SvmpSampColSpecAnaControl, force_trigger, 1);
		OSL_DELAY(100000);
		MOD_PHYREG(pi, SvmpSampColSpecAnaControl, force_trigger, 0);
	} else if (trigType == 1) {
		MOD_PHYREG(pi, SvmpSampColaoatrig1transt, aoatrig1transt1, initialState);
		//pktproc state transition FROM
		MOD_PHYREG(pi, SvmpSampColaoatrig1transt, aoatrig1transt2, finalState);
		//pktproc state transition To
	} else {
		printf("not valid capture mode!\n");
	}
}
