/*
 * aesgcm.h
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

#ifndef _AESGCM_H_
#define _AESGCM_H_

#include <gcm.h>
#include <aes.h>

struct aes_gcm_ctx {
	int       nrounds;	/* number of rounds */
	uint32    rkey[(AES_MAXROUNDS + 1) << 2];	/* round key */
	gcm_ctx_t gcm;								/* gcm context */
};

typedef struct aes_gcm_ctx aes_gcm_ctx_t;

void aes_gcm_init(aes_gcm_ctx_t *ctx, gcm_op_type_t op_type,
	const uint8 *key, size_t key_len,
	const uint8 *nonce, size_t nonce_len,
	const uint8 *aad, size_t aad_len);

/* see gcm_update. data_len must be a multiple of AES_BLOCK_SZ */
void aes_gcm_update(aes_gcm_ctx_t *ctx, uint8 *data, size_t data_len);

/* finalize aes gcm - data_len need not be a multiple of AES_BLOCK_SZ */
void aes_gcm_final(aes_gcm_ctx_t *ctx, uint8 *data, size_t data_len,
	uint8 *mac, size_t mac_len);

/* convenience functions */

/* plain text to cipher text, mac. data is modified in place */
void aes_gcm_encrypt(const uint8 *key, size_t key_len,
    const uint8 *nonce, size_t nonce_len, const uint8 *aad, size_t aad_len,
	uint8 *data, size_t data_len, uint8 *mac, size_t mac_len);

/* plain text to mac. data is not modified */
void aes_gcm_mac(const uint8 *key, size_t key_len,
    const uint8 *nonce, size_t nonce_len, const uint8 *aad, size_t aad_len,
	/* const */ uint8 *data, size_t data_len,
	uint8 *mac, size_t mac_len);

/* cipher text to plain text. data in modified in place. expected mac is
 * verified and returns non-zero on success
 */
int aes_gcm_decrypt(const uint8 *key, size_t key_len,
    const uint8 *nonce, size_t nonce_len, const uint8 *aad, size_t aad_len,
	uint8 *data, size_t data_len, const uint8 *expected_mac, size_t mac_len);

/* using plain text, verifies expected mac - return non-zero on success */
int aes_gcm_verify(const uint8 *key, size_t key_len,
    const uint8 *nonce, size_t nonce_len, const uint8 *aad, size_t aad_len,
	/* const */ uint8 *data, size_t data_len,
	const uint8 *expected_mac, size_t mac_len);

#endif /* _AESGCM_H_ */
