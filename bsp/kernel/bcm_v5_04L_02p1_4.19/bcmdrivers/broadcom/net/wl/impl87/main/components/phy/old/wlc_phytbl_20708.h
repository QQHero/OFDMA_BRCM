/*
 * Radio 20708 table definition header file
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
 * $Id$
 */

#ifndef _WLC_PHYTBL_20708_H_
#define _WLC_PHYTBL_20708_H_

/* XXX: Define wlc_cfg.h to be the first header file included as some builds
 * get their feature flags thru this file.
 */
#include <wlc_cfg.h>
#include <typedefs.h>

#include "wlc_phy_int.h"
#include "phy_ac_rxgcrs.h"

typedef struct _chan_info_radio20708_rffe_2G {
	/* 2G tuning data */
	uint8 RFP0_LOGEN_REG2_logen_ctune;
	uint8 RFP1_LOGEN_REG2_logen_ctune;
	uint8 RF0_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF1_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF2_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF3_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF0_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF1_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF2_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF3_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF0_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF1_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF2_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF3_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF0_RX5G_REG1_rxdb_lna_tune;
	uint8 RF1_RX5G_REG1_rxdb_lna_tune;
	uint8 RF2_RX5G_REG1_rxdb_lna_tune;
	uint8 RF3_RX5G_REG1_rxdb_lna_tune;
	uint8 RF0_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF1_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF2_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF3_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF0_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF1_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF2_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF3_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF0_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF1_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF2_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF3_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF0_RX5G_REG4_rxdb_gm_cc;
	uint8 RF1_RX5G_REG4_rxdb_gm_cc;
	uint8 RF2_RX5G_REG4_rxdb_gm_cc;
	uint8 RF3_RX5G_REG4_rxdb_gm_cc;
} chan_info_radio20708_rffe_2G_t;

typedef struct _chan_info_radio20708_rffe_5G {
	/* 5G tuning data */
	uint8 RFP0_LOGEN_REG2_logen_ctune;
	uint8 RFP1_LOGEN_REG2_logen_ctune;
	uint8 RF0_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF1_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF2_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF3_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF0_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF1_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF2_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF3_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF0_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF1_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF2_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF3_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF0_RX5G_REG1_rxdb_lna_tune;
	uint8 RF1_RX5G_REG1_rxdb_lna_tune;
	uint8 RF2_RX5G_REG1_rxdb_lna_tune;
	uint8 RF3_RX5G_REG1_rxdb_lna_tune;
	uint8 RF0_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF1_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF2_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF3_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF0_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF1_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF2_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF3_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF0_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF1_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF2_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF3_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF0_RX5G_REG4_rxdb_gm_cc;
	uint8 RF1_RX5G_REG4_rxdb_gm_cc;
	uint8 RF2_RX5G_REG4_rxdb_gm_cc;
	uint8 RF3_RX5G_REG4_rxdb_gm_cc;
} chan_info_radio20708_rffe_5G_t;

typedef struct _chan_info_radio20708_rffe_6G {
	/* 6G tuning data */
	uint8 RFP0_LOGEN_REG2_logen_ctune;
	uint8 RFP1_LOGEN_REG2_logen_ctune;
	uint8 RF0_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF1_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF2_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF3_LOGEN_CORE_REG1_logen_core_buf_ctune;
	uint8 RF0_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF1_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF2_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF3_TX2G_MIX_REG4_txdb_mx_tune;
	uint8 RF0_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF1_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF2_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF3_TXDB_PAD_REG3_tx_pad_tune;
	uint8 RF0_RX5G_REG1_rxdb_lna_tune;
	uint8 RF1_RX5G_REG1_rxdb_lna_tune;
	uint8 RF2_RX5G_REG1_rxdb_lna_tune;
	uint8 RF3_RX5G_REG1_rxdb_lna_tune;
	uint8 RF0_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF1_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF2_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF3_TX2G_PAD_REG2_tx_pad_xfmr_sw;
	uint8 RF0_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF1_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF2_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF3_TX2G_MIX_REG0_tx_mx_xfmr_sw_s;
	uint8 RF0_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF1_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF2_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF3_TX2G_MIX_REG0_tx_mx_xfmr_sw_p;
	uint8 RF0_RX5G_REG4_rxdb_gm_cc;
	uint8 RF1_RX5G_REG4_rxdb_gm_cc;
	uint8 RF2_RX5G_REG4_rxdb_gm_cc;
	uint8 RF3_RX5G_REG4_rxdb_gm_cc;
} chan_info_radio20708_rffe_6G_t;

typedef struct _chan_info_radio20708_rffe {
	uint16 channel;
	uint16 freq;
	union {
		/* In this union, make sure the largest struct is at the top. */
		chan_info_radio20708_rffe_6G_t val_6G;
		chan_info_radio20708_rffe_5G_t val_5G;
		chan_info_radio20708_rffe_2G_t val_2G;
	} u;
} chan_info_radio20708_rffe_t;

uint32 phy_get_chan_tune_tbl_20708(phy_info_t *pi, chanspec_band_t band,
	const chan_info_radio20708_rffe_t **chan_info_tbl);

#if (defined(BCMDBG) && defined(DBG_PHY_IOV)) || defined(BCMDBG_PHYDUMP)
extern const radio_20xx_dumpregs_t dumpregs_20708_rev0[];
#endif

/* Radio referred values tables */
extern const radio_20xx_prefregs_t prefregs_20708_rev0[];

extern int8 BCMATTACHDATA(lna12_gain_tbl_2g_20708r0)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_5g_20708r0)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_6g_20708r0)[2][N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_2g_20708r0)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_5g_20708r0)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_6g_20708r0)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_2g_20708r0)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_5g_20708r0)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_6g_20708r0)[N_LNA12_GAINS];

extern int8 BCMATTACHDATA(lna12_gain_tbl_2g_20708r2)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_2g_20708r2_ilnatbl1)[2][N_LNA12_GAINS]; // mch2
extern int8 BCMATTACHDATA(lna12_gain_tbl_5g_20708r2)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_5g_20708r2_ilnatbl1)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_6g_low_20708r2)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_6g_low2_20708r2)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_6g_mid_20708r2)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_6g_hi_20708r2)[2][N_LNA12_GAINS];
extern int8 BCMATTACHDATA(lna12_gain_tbl_6g_hi2_20708r2)[2][N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_2g_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_2g_20708r2_ilnatbl1)[N_LNA12_GAINS]; // mch2
extern uint8 BCMATTACHDATA(lna1_rout_map_5g_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_5g_20708r2_ilnatbl1)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_6g_low_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_6g_low2_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_6g_mid_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_6g_hi_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_rout_map_6g_hi2_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_2g_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_2g_20708r2_ilnatbl1)[N_LNA12_GAINS]; // mch2
extern uint8 BCMATTACHDATA(lna1_gain_map_5g_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_5g_20708r2_ilnatbl1)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_6g_low_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_6g_low2_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_6g_mid_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_6g_hi_20708r2)[N_LNA12_GAINS];
extern uint8 BCMATTACHDATA(lna1_gain_map_6g_hi2_20708r2)[N_LNA12_GAINS];

extern int8 BCMATTACHDATA(gainlimit_tbl_20708r0)[RXGAIN_CONF_ELEMENTS][MAX_RX_GAINS_PER_ELEM];
extern int8 BCMATTACHDATA(tia_gain_tbl_20708r0)[N_TIA_GAINS];
extern int8 BCMATTACHDATA(tia_gainbits_tbl_20708r0)[N_TIA_GAINS];
extern int8 BCMATTACHDATA(biq01_gain_tbl_20708r0)[2][N_BIQ01_GAINS];
extern int8 BCMATTACHDATA(biq01_gainbits_tbl_20708r0)[2][N_BIQ01_GAINS];

extern int8 BCMATTACHDATA(tia_gain_tbl_20708r2)[N_TIA_GAINS];
extern int8 BCMATTACHDATA(tia_gainbits_tbl_20708r2)[N_TIA_GAINS];

#endif	/* _WLC_PHYTBL_20708_H_ */
