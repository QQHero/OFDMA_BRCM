/**
 * @file
 * @brief
 * PHY module Power-per-rate API. Provides interface functions and definitions for opaque
 * ppr structure for use containing regulatory and board limits and tx power targets.
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

/** Twiki's: [MfgcPresentations] [PPRTxPowerCache] */

#ifndef _wlc_ppr_h_
#define _wlc_ppr_h_

#include <bcmwifi_rates.h>
#include <bcmutils.h>
#include <bcmwifi_channels.h>

#ifdef BCMDRIVER
#include <osl.h>

#ifdef BCMDBG_ERR
#define	PPR_ERROR(args)		printf args
#else
#define	PPR_ERROR(args)
#endif /* BCMDBG_ERR */
#else
#include <stdio.h>
#define osl_t void
#define	PPR_ERROR(args)		printf args
#endif /* BCMDRIVER */

/* Version of pprpbw_t */
/* version 1 start to support HE, HE_UB and HE_LUB */
#define PPR_VERSION 1
/* Version of pprpbw_ru_t */
/* pprpbw_ru_t is created from version 1 */
#define PPR_RU_VERSION 1

#ifndef BCMPHYCORENUM
#define PPR_MAX_TX_CHAINS 4
#else
#if (BCMPHYCORENUM < 1) || (BCMPHYCORENUM > 4)
#error Invalid number of PHY cores
#endif /* BCMPHYCORENUM < 1 || BCMPHYCORENUM > 4 */
#define PPR_MAX_TX_CHAINS (BCMPHYCORENUM)
#endif /* !defined(BCMPHYCORENUM) */

#define WL_RATE_GROUP_VHT_INDEX_MCS_7  7
#define WL_RATE_GROUP_VHT_INDEX_MCS_87 8
#define WL_RATE_GROUP_VHT_INDEX_MCS_88 9

#ifdef NO_EXTRA_BW_SUPPORT /* For ROM compatiblity */
#define PPR_CHSPEC_BW(x) (CHSPEC_IS80(x) ? WL_TX_BW_80 : \
	(CHSPEC_IS40(x) ? WL_TX_BW_40 : WL_TX_BW_20))
#else
#define PPR_CHSPEC_BW(x) ppr_chanspec_bw(x)
#endif /* NO_EXTRA_BW_SUPPORT */

#if defined(NDIS)
#define MAX_PPRSTRUCT_SIZE (1684) /* 80+80Mhz 4 Chains ppr size, just for Windows Build to pass */
#endif

/* Opaque PPR data - it keeps its structure to itself */
typedef struct ppr ppr_t;
typedef struct ppr_ru ppr_ru_t;
typedef struct ppr_ru_offset ppr_ru_offset_t;

/* Power values for DSSS 1, 2, 5.5, 11 */
typedef struct ppr_dsss_rateset { int8 pwr[WL_RATESET_SZ_DSSS]; } ppr_dsss_rateset_t;

/* Power values for OFDM 6, 9, 12... 54 */
typedef struct ppr_ofdm_rateset { int8 pwr[WL_RATESET_SZ_OFDM]; } ppr_ofdm_rateset_t;

/* Power values one set of 8 HT MCS values (0-7, 8-15, etc) */
typedef struct ppr_ht_mcs_rateset { int8 pwr[WL_RATESET_SZ_HT_MCS]; } ppr_ht_mcs_rateset_t;

/* Power values one set of 10 VHT MCS values (0-9) */

#ifdef NO_PROPRIETARY_VHT_RATES
typedef struct ppr_vht_mcs_rateset { int8 pwr[WL_RATESET_SZ_VHT_MCS]; } ppr_vht_mcs_rateset_t;
#else
typedef struct ppr_vht_mcs_rateset { int8 pwr[WL_RATESET_SZ_VHT_MCS_P]; } ppr_vht_mcs_rateset_t;
#endif

typedef struct ppr_he_mcs_rateset { int8 pwr[WL_RATESET_SZ_HE_MCS]; } ppr_he_mcs_rateset_t;
typedef struct ppr_he_ru_rl_rateset { int8 pwr[WL_TX_CHAINS_MAX*2]; } ppr_he_ru_rl_rateset_t;
/* API Routines */

/* Initialization routine for opaque PPR struct */
void ppr_init(ppr_t* pprptr, wl_tx_bw_t bw);

/* Reinitialization routine for opaque PPR struct */
void ppr_clear(ppr_t* pprptr);

/* Size routine for user alloc/dealloc */
uint32 ppr_size(wl_tx_bw_t bw);

/* Size routine for user serialization alloc */
uint32 ppr_ser_size(const ppr_t* pprptr);

/* Size routine for user serialization alloc for a given bw */
uint32 ppr_ser_size_by_bw(wl_tx_bw_t bw);

/* Init allocated memory with a ppr serialization head
 * This function must be called if serialization side has different
 * compilation conditions (e.g PPR_MAX_TX_CHAINS, WL_BEAMFORMING etc.)
 */
int ppr_init_ser_mem_by_bw(uint8* pbuf, wl_tx_bw_t bw, uint32 len);

/* Init allocated memory with a ppr serialization head for the given ppr pointer
 * This function must be called if serialization side has different
 * compilation conditions (e.g PPR_MAX_TX_CHAINS, WL_BEAMFORMING etc.)
 */
int ppr_init_ser_mem(uint8* pbuf, ppr_t * ppr, uint32 len);

/* Constructor routine for opaque PPR struct */
ppr_t* ppr_create(osl_t* osh, wl_tx_bw_t bw);

/* Constructor routine for opaque PPR struct on pre-alloc memory */
ppr_t* ppr_create_prealloc(wl_tx_bw_t bw, int8 *buf, uint len);

/* Update the bw for the given opaque PPR struct */
void ppr_set_ch_bw(ppr_t* pprptr, wl_tx_bw_t bw);

/* Destructor routine for opaque PPR struct */
void ppr_delete(osl_t* osh, ppr_t* pprptr);

void ppr_ru_set_same_value(ppr_ru_t* ppr_ru_ptr, int8 val);

/* Initialization routine for opaque PPR struct */
void ppr_ru_clear(ppr_ru_t* ppr_ru_ptr);

/* Constructor routine for opaque PPR RU struct */
ppr_ru_t* ppr_ru_create(osl_t* osh);

/* Constructor routine for opaque PPR RU struct on pre-alloc memory */
ppr_ru_t* ppr_ru_create_prealloc(int8 *buf, uint len);

/* Destructor routine for opaque PPR RU struct */
void ppr_ru_delete(osl_t* osh, ppr_ru_t* ppr_ru_ptr);

uint32 ppr_ru_size(void);

/* Initialization routine for opaque RU PPR offset struct */
void ppr_ru_offset_clear(ppr_ru_offset_t* ppr_ru_offset_ptr);

/* Constructor routine for opaque RU PPR offset struct */
ppr_ru_offset_t* ppr_ru_offset_create(osl_t* osh);

/* Destructor routine for opaque RU PPR offset struct */
void ppr_ru_offset_delete(osl_t* osh, ppr_ru_offset_t* ppr_ru_offset_ptr);

/* RL/BL variable setting routine for opaque RU PPR offset struct */
void ppr_set_he_ruoffset_rl(ppr_ru_offset_t* ppr_ru_offset_ptr, wl_tx_nss_t Nss,
	wl_tx_mode_t mode, wl_he_rate_type_t rusize, const int8 power);
void ppr_set_he_ruoffset_bl(ppr_ru_offset_t* ppr_ru_offset_ptr, wl_he_rate_type_t rusize,
	uint8 mcsidx, const int8 power);
int ppr_get_he_ruoffset(ppr_ru_offset_t* ppr_ru_offset_ptr, wl_he_rate_type_t rusize,
	uint8 mcsidx, wl_tx_nss_t Nss, bool txbf, bool RL);
/* Type routine for inferring opaque structure size */
wl_tx_bw_t ppr_get_ch_bw(const ppr_t* pprptr);

/* Type routine to get ppr supported maximum bw */
wl_tx_bw_t ppr_get_max_bw(void);
/* Get the maximum power in the dsss ppr set */
int8 ppr_get_dsss_max(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_chains_t tx_chains);
/* Get the DSSS values for the given number of tx_chains and 20, 20in40, etc. */
int ppr_get_dsss(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_chains_t tx_chains,
	ppr_dsss_rateset_t* dsss);

/* Get the OFDM values for the given number of tx_chains and 20, 20in40, etc. */
int ppr_get_ofdm(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_mode_t mode, wl_tx_chains_t tx_chains,
	ppr_ofdm_rateset_t* ofdm);

/* Get the HT MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_get_ht_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, ppr_ht_mcs_rateset_t* mcs);

/* Get the VHT MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_get_vht_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
 wl_tx_chains_t tx_chains, ppr_vht_mcs_rateset_t* mcs);

/* Get the HE MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_get_he_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, ppr_he_mcs_rateset_t *he, wl_he_rate_type_t type);

/* Get the HE RU MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_get_he_ru_mcs(ppr_ru_t* ppr_ru_ptr, wl_he_rate_type_t t, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, ppr_he_mcs_rateset_t* mcs);

/* Get the minimum power for a VHT MCS rate specified by Nss, with the given bw and tx chains.
 * Disabled rates are ignored
 */
int ppr_get_vht_mcs_min(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
 wl_tx_chains_t tx_chains, int8* mcs_min);

/* Get the minimum power for a 11ag rate specified with the given bw and tx chains.
 * Disabled rates are ignored
 */
int ppr_get_ofdm_min(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_mode_t mode, wl_tx_chains_t tx_chains,
	int8* mcs_min);

/* Routines to set target powers per rate for a whole rate set */

/* Set the DSSS values for the given number of tx_chains and 20, 20in40, etc. */
int ppr_set_dsss(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_chains_t tx_chains,
	const ppr_dsss_rateset_t* dsss);

/* Set the OFDM values for the given number of tx_chains and 20, 20in40, etc. */
int ppr_set_ofdm(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_mode_t mode, wl_tx_chains_t tx_chains,
	const ppr_ofdm_rateset_t* ofdm);

/* Set the HT MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_set_ht_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, const ppr_ht_mcs_rateset_t* mcs);

/* Set the VHT MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_set_vht_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, const ppr_vht_mcs_rateset_t* mcs);

/* Set the HE MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_set_he_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, const ppr_he_mcs_rateset_t *he, wl_he_rate_type_t type);

/* Set the HE RU MCS values for the group specified by Nss, with the given bw, mode and tx chains */
int ppr_set_he_ru_mcs(ppr_ru_t* ppr_ru_ptr, wl_he_rate_type_t t, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, const ppr_he_mcs_rateset_t* mcs);

/* Set the HE RU MCS values for the group specified by the given RU size and tx chains */
void ppr_set_he_ru_mcs_allmode(ppr_ru_t* ppr_ru_ptr, wl_he_rate_type_t t, wl_tx_chains_t tx_chains,
	const ppr_he_mcs_rateset_t* mcs);

/* Routines to set a whole rate set to a single target value */

/* Set the DSSS values for the given number of tx_chains and 20, 20in40, etc. */
int ppr_set_same_dsss(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_chains_t tx_chains, const int8 power);

/* Set the OFDM values for the given number of tx_chains and 20, 20in40, etc. */
int ppr_set_same_ofdm(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_mode_t mode, wl_tx_chains_t tx_chains,
	const int8 power);

/* Set the HT MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_set_same_ht_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, const int8 power);

/* Set the VHT MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_set_same_vht_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, const int8 power);

int ppr_set_same_he_mcs(ppr_t* pprptr, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains, const int8 power, wl_he_rate_type_t type);

/* Set the HE RU MCS values for the group specified by Nss, with the given bw and tx chains */
int ppr_set_same_he_ru_mcs(ppr_ru_t* ppr_ru_ptr, wl_he_rate_type_t t, wl_tx_nss_t Nss,
	wl_tx_mode_t mode, wl_tx_chains_t tx_chains, const int8 power);

/* Helper routines to operate on the entire ppr set */

/* Ensure no rate limit is greater than the specified maximum */
uint ppr_apply_max(ppr_t* pprptr, int8 max);

/* Make disabled rates explicit. If one rate in a group is disabled, disable the whole group */
int ppr_force_disabled(ppr_t* pprptr, int8 threshold);

/* Explicitly disable all VHT-only MIMO rates - i.e. vht8SS2 and above. */
int ppr_disable_vht_mimo_rates(ppr_t* pprptr);

/*
 * Reduce total transmitted power to level of constraint.
 * For two chain rates, the per-antenna power must be halved.
 * For three chain rates, it must be a third of the constraint.
 */
uint ppr_apply_constraint_total_tx(ppr_t* pprptr, int8 constraint);

/* Ensure no rate limit is lower than the specified minimum */
uint ppr_apply_min(ppr_t* pprptr, int8 min);

/* Ensure no rate limit in this ppr set is greater than the corresponding limit in ppr_cap */
uint ppr_apply_vector_ceiling(ppr_t* pprptr, const ppr_t* ppr_cap);

/* Ensure no rate limit in this ppr set is lower than the corresponding limit in ppr_min */
uint ppr_apply_vector_floor(ppr_t* pprptr, const ppr_t* ppr_min);

/* Get the maximum power in the ppr set */
int8 ppr_get_max(ppr_t* pprptr);

/* Get the maximum power in the ppr_ru set */
int8 ppr_ru_get_max(ppr_ru_t* pprptr);

/*
 * Get the minimum power in the ppr set excluding disallowed
 * rates and powers set to the minimum for the phy
 */
int8 ppr_get_min(ppr_t* pprptr, int8 floor);

/*
 * Get the minimum power in the ppr_r set, excluding disallowed
 * rates and (possibly) powers set to the minimum for the phy
 */
int8 ppr_ru_get_min(ppr_ru_t* pprptr, int8 floor);

/* Get the maximum power for a given bandwidth in the ppr set */
int8 ppr_get_max_for_bw(ppr_t* pprptr, wl_tx_bw_t bw);

/* Get the minimum power for a given bandwidth  in the ppr set */
int8 ppr_get_min_for_bw(ppr_t* pprptr, wl_tx_bw_t bw);

typedef void (*ppr_mapfn_t)(void *context, uint8 *a, uint8 *b);

/* Map the given function with its context value over the two power vectors */
void ppr_map_vec_dsss(ppr_mapfn_t fn, void* context, ppr_t* pprptr1,
	ppr_t* pprptr2, wl_tx_bw_t bw, wl_tx_chains_t tx_chains);

/* Map the given function with its context value over the two power vectors */
void ppr_map_vec_ofdm(ppr_mapfn_t fn, void* context, ppr_t* pprptr1,
	ppr_t* pprptr2, wl_tx_bw_t bw, wl_tx_mode_t mode, wl_tx_chains_t tx_chains);

/* Map the given function with its context value over the two power vectors */
void ppr_map_vec_ht_mcs(ppr_mapfn_t fn, void* context, ppr_t* pprptr1,
	ppr_t* pprptr2, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains);

/* Map the given function with its context value over the two power vectors */
void ppr_map_vec_vht_mcs(ppr_mapfn_t fn, void* context, ppr_t* pprptr1,
	ppr_t* pprptr2, wl_tx_bw_t bw, wl_tx_nss_t Nss, wl_tx_mode_t mode,
	wl_tx_chains_t tx_chains);

/* Map the given function with its context value over the two power vectors */
void ppr_map_vec_all(ppr_mapfn_t fn, void* context, ppr_t* pprptr1, ppr_t* pprptr2);

/* Map the given function with its context value over the two power vectors */
bool ppr_ru_map_vec_all(ppr_mapfn_t fn, void *context, ppr_ru_t *pprptr1, ppr_ru_t *pprptr2);

/* Get the first index that is larger than a given power level */
int16 ppr_get_idx(ppr_t* pprptr, int8 pwr);

/* Set PPR struct to a certain power level */
void ppr_set_cmn_val(ppr_t* pprptr, int8 val);

void ppr_copy_ru_struct(ppr_ru_t *s, ppr_ru_t *d);

/* Make an identical copy of a ppr structure (for ppr_bw==all case) */
void ppr_copy_struct(ppr_t* pprptr_s, ppr_t* pprptr_d);

/* Subtract each power from a common value and re-store */
void ppr_cmn_val_minus(ppr_t* pprptr, int8 val);

/* Compare two ppr ru variables s1 and s2, save the min value of each
 * contents to variable d
 */
void ppr_ru_compare_min(ppr_ru_t *d, ppr_ru_t *s1, ppr_ru_t *s2);

/* Calculate new power offset (tx_pwr_max - *rptr_target)
 * Updtae to txpwr_offset (MAX(*rptr_offset, (tx_pwr_max - *rptr_target)))
 */
void ppr_ru_update_txpwr_offset(ppr_ru_t *txpwr_offset, ppr_ru_t *txpwr_target, int8 tx_pwr_max);

/* Compare each power in pprptr with a common value, save the min value of each
 * contents to pprptr
 */
void ppr_ru_compare_min_cmn_val(ppr_ru_t *pprptr, int8 val);

/* Compare two ppr variables p1 and p2, save the min. value of each
 * contents to variable p1
 */
void ppr_compare_min(ppr_t* p1, ppr_t* p2);

/* compare two ppr variables p1 and p2, save the max. value of each
 * contents to variable p1
 */
void ppr_compare_max(ppr_t* p1, ppr_t* p2);

/* Serialize the contents of the opaque ppr struct.
 * Writes number of bytes copied, zero on error.
 * Returns error code, BCME_OK if successful.
 */
int ppr_serialize(const ppr_t* pprptr, uint8* buf, uint buflen, uint* bytes_copied);

/* Deserialize the contents of a buffer into an existing opaque ppr struct.
 * The ppr struct *must* be of the same type as the one that was serialized.
 * Returns error code, BCME_OK if successful.
 */
int ppr_deserialize(ppr_t* pprptr, const uint8* buf, uint buflen, uint16 ppr_ver);

/* Deserialize the contents of a buffer into a new opaque ppr struct.
 * Creates an opaque structure referenced by *pptrptr, NULL on error.
 * Returns error code, BCME_OK if successful.
 */
int ppr_deserialize_create(osl_t* osh, const uint8* buf, uint buflen, ppr_t** pprptr,
	uint16 ppr_ver);

/* Subtract a common value from each power and re-store */
void ppr_ru_minus_cmn_val(ppr_ru_t* ru_pprptr, int8 val);

/* Subtract a common value from each power and re-store */
void ppr_minus_cmn_val(ppr_t* pprptr, int8 val);

/* Add a common value to the given bw struct components */
void ppr_plus_cmn_val(ppr_t* pprptr, int8 val);

/* Multiply a percentage */
void ppr_multiply_percentage(ppr_t* pprptr, uint8 val);

/* Get transmit channel bandwidths from chanspec */
wl_tx_bw_t ppr_chanspec_bw(chanspec_t chspec);

#if defined(WL_EXPORT_CURPOWER) || !defined(BCMDRIVER)
/* Convert ppr structure to TLV data */
void ppr_convert_to_tlv(ppr_t* pprptr, wl_tx_bw_t bw, uint8 *to_tlv_buf, uint32 tlv_buf_len,
	wl_tx_chains_t max_chain);

/* Convert TLV data to ppr structure */
int ppr_convert_from_tlv(ppr_t* pprptr, uint8 *from_tlv_buf, uint32 tlv_buf_len);

void ppr_ru_convert_to_tlv(ppr_ru_t* ppr_ru_ptr, uint8 *to_tlv_buf, uint32 tlv_buf_len,
	wl_tx_chains_t max_chain);

/* Convert TLV data to ppr_ru structure */
int ppr_ru_convert_from_tlv(ppr_ru_t* ppr_ru_ptr, uint8 *from_tlv_buf, uint32 tlv_buf_len);

/* Get the total HE (Type SU, UB, LUB) TLV buffer size
 * for the given ppr data of bandwidth and max chains
 */
uint32 ppr_get_he_tlv_size(ppr_t* pprptr, wl_tx_bw_t bw, uint32 max_chain);

/* Get the total ppr TLV buffer size for given bandwidth and chain number */
uint32 ppr_get_tlv_size(ppr_t* pprptr, wl_tx_bw_t bw, uint32 max_chain);

/* Get the total ppr ru TLV buffer size for given chain number */
uint32 ppr_ru_get_size(ppr_ru_t* ppr_ru_ptr, uint32 max_chain);

/* Get current PPR TLV version */
uint32 ppr_get_tlv_ver(void);
#endif /* WL_EXPORT_CURPOWER || !BCMDRIVER */

/* Forward declaration */
typedef struct tx_pwr_cache_entry tx_pwr_cache_entry_t;

#ifdef WLTXPWR_CACHE

/**
 * Usage of the WLTXPWR_CACHE API:
 * 1. Reserve one or more entries in the cache for a specific chanspec:
 * 		wlc_phy_txpwr_setup_entry(chanspec1);
 * 		wlc_phy_txpwr_setup_entry(chanspec2);
 * 2. Use any of the getter/setter functions. Note that they may return an error on a chanspec
 *    that was not reserved. For setter functions, for pointer type arguments (e.g. ppr_t), the
 *    cache maintains a reference to the caller provided object rather than copying it.
 * 3. Use wlc_phy_txpwr_cache_clear() or wlc_phy_txpwr_cache_invalidate() to get rid of cache
 *    entries. Note that this clears, amongst others, ppr_t structs.
 *
 * For non-BMAC builds, only the cache is allowed to delete the ppr_t object, and the driver code is
 * not allowed to reuse it for another channel as it can when the cache is not being used.
 *
 * For BMAC builds, the cache has to be made to release the stf offsets object to avoid some nasty
 * race conditions.
 *
 * Mike thinks when this is reimplemented for trunk he would consider having a reference count
 * associated with each object to make this cleaner.
 *
 * Note that the functions start with wlc_phy_* despite not residing in the PHY code. This is
 * probably for historical reasons.
 */

enum txpwr_cache_info_type {
	TXPWR_CACHE_STF_OFFSETS,	/* PPR offsets for stf */
	TXPWR_CACHE_POWER_OFFSETS,	/* PPR offsets for phy */
	TXPWR_CACHE_NUM_TYPES,	/* How many types of pwr info */
};

#define TXPWR_STF_PWR_MIN_INVALID 0x80
#define TXPWR_STF_TARGET_PWR_MIN_INVALID 0x40
#define TXPWR_STF_TARGET_PWR_NOT_CACHED 0x20
extern bool wlc_phy_get_cached_pwr(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
		uint pwr_type, ppr_t* pprptr);

extern ppr_t* wlc_phy_get_cached_ppr_ptr(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint pwr_type);

extern int wlc_phy_set_cached_pwr(osl_t* osh, tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint pwr_type, ppr_t* pwrptr);

extern int8 wlc_phy_get_cached_boardlim(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint core);

extern int wlc_phy_set_cached_boardlim(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint core, int8 board_lim);

extern uint8 wlc_phy_get_cached_pwr_max(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint core);

extern int wlc_phy_set_cached_pwr_max(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint core, uint8 max_pwr);

extern uint8 wlc_phy_get_cached_pwr_min(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint core);

extern int wlc_phy_set_cached_pwr_min(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint core, uint8 min_pwr);

extern int wlc_phy_get_cached_stf_target_pwr_min(tx_pwr_cache_entry_t* cacheptr,
	chanspec_t chanspec);

extern int wlc_phy_set_cached_stf_target_pwr_min(tx_pwr_cache_entry_t* cacheptr,
	chanspec_t chanspec, int min_pwr);

extern int8 wlc_phy_get_cached_txchain_offsets(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint core);

extern int wlc_phy_set_cached_txchain_offsets(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint core, int8 offset);

extern bool wlc_phy_txpwr_cache_is_cached(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec);

extern bool wlc_phy_is_pwr_cached(tx_pwr_cache_entry_t* cacheptr, uint pwr_type, ppr_t* pwrptr);

extern void wlc_phy_uncache_pwr(tx_pwr_cache_entry_t* cacheptr, uint pwr_type, ppr_t* pwrptr);

extern chanspec_t wlc_phy_txpwr_cache_find_other_cached_chanspec(tx_pwr_cache_entry_t* cacheptr,
	chanspec_t chanspec);

extern tx_pwr_cache_entry_t* wlc_phy_txpwr_cache_create(osl_t* osh);

extern void wlc_phy_txpwr_cache_clear(osl_t* osh, tx_pwr_cache_entry_t* cacheptr,
	chanspec_t chanspec);

extern void wlc_phy_txpwr_cache_invalidate(tx_pwr_cache_entry_t* cacheptr);

extern void wlc_phy_txpwr_cache_close(osl_t* osh, tx_pwr_cache_entry_t* cacheptr);

extern int wlc_phy_txpwr_setup_entry(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec);

#ifdef WL_SARLIMIT
void wlc_phy_set_cached_sar_lims(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint32 sar_lims);

uint32 wlc_phy_get_cached_sar_lims(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec);
#endif

#ifndef WLTXPWR_CACHE_PHY_ONLY
extern bool wlc_phy_get_stf_ppr_cached(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec);

extern void wlc_phy_set_stf_ppr_cached(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	bool bcached);

extern int wlc_phy_get_cached_stf_pwr_min_dbm(tx_pwr_cache_entry_t* cacheptr);

extern void wlc_phy_set_cached_stf_pwr_min_dbm(tx_pwr_cache_entry_t* cacheptr, int min_pwr);

extern void wlc_phy_set_cached_stf_max_offset(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec,
	uint8 max_offset);

extern uint8 wlc_phy_get_cached_stf_max_offset(tx_pwr_cache_entry_t* cacheptr, chanspec_t chanspec);
#endif /* WLTXPWR_CACHE_PHY_ONLY */
#endif /* WLTXPWR_CACHE */
#endif	/* _wlc_ppr_h_ */
