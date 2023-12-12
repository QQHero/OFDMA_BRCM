/*
 * passhash_vectors.h
 * Password Hash test vectors
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
 * $Id: passhash_vectors.h 679390 2017-01-14 00:13:47Z $
 */

#include <typedefs.h>

/* Make sure all passphrases and salts are null-terminated so we can get
 * the sizes using sizeof(x) - 1
 */
char pass_01[] = "password";

uint8 salt_01[] = "IEEE";

uint8 ref_01[] = {
	0xf4, 0x2c, 0x6f, 0xc5, 0x2d, 0xf0, 0xeb, 0xef,
	0x9e, 0xbb, 0x4b, 0x90, 0xb3, 0x8a, 0x5f, 0x90,
	0x2e, 0x83, 0xfe, 0x1b, 0x13, 0x5a, 0x70, 0xe2,
	0x3a, 0xed, 0x76, 0x2e, 0x97, 0x10, 0xa1, 0x2e,
	0x48, 0x88, 0xf1, 0x97, 0xa3, 0x68, 0x04, 0x15
	};

char pass_02[] = "ThisIsAPassword";

uint8 salt_02[] = "ThisIsASSID";

uint8 ref_02[] = {
	0x0d, 0xc0, 0xd6, 0xeb, 0x90, 0x55, 0x5e, 0xd6,
	0x41, 0x97, 0x56, 0xb9, 0xa1, 0x5e, 0xc3, 0xe3,
	0x20, 0x9b, 0x63, 0xdf, 0x70, 0x7d, 0xd5, 0x08,
	0xd1, 0x45, 0x81, 0xf8, 0x98, 0x27, 0x21, 0xaf,
	0xca, 0xc2, 0x80, 0x6d, 0xe3, 0xfd, 0x47, 0xa8
	};

char pass_03[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

uint8 salt_03[] = "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ";

uint8 ref_03[] = {
	0xbe, 0xcb, 0x93, 0x86, 0x6b, 0xb8, 0xc3, 0x83,
	0x2c, 0xb7, 0x77, 0xc2, 0xf5, 0x59, 0x80, 0x7c,
	0x8c, 0x59, 0xaf, 0xcb, 0x6e, 0xae, 0x73, 0x48,
	0x85, 0x00, 0x13, 0x00, 0xa9, 0x81, 0xcc, 0x62,
	0x1d, 0x20, 0xbd, 0x88, 0x0f, 0x55, 0x2c, 0xbb
	};

char pass_04[] = "password";

uint8 salt_04[] = { 0x12, 0x34, 0x56, 0x78, 0x78, 0x56, 0x34, 0x12, 0x00 };

uint8 ref_04[] = {
	0x08, 0x79, 0xef, 0x3a, 0x9e, 0xa7, 0xe0, 0x66,
	0x05, 0x39, 0x7f, 0xb4, 0x11, 0xb8, 0xf9, 0xf7,
	0x90, 0x7b, 0x24, 0xd2, 0xa0, 0x31, 0x01, 0x33,
	0x31, 0x40, 0x8a, 0x42, 0xde, 0xe2, 0x28, 0x26,
	0x1e, 0x3d, 0x64, 0x13, 0xc0, 0xb0, 0xe9, 0x8f
	};

char pass_05[] = "passphrase!";

uint8 salt_05[] = "wpa_psk";

uint8 ref_05[] = {
	0x08, 0xa3, 0x94, 0x8b, 0x74, 0x8d, 0xda, 0x04,
	0xdf, 0xd2, 0x4b, 0x4a, 0xda, 0x23, 0x1e, 0xbf,
	0xe5, 0xdd, 0x38, 0xd3, 0x9a, 0xdd, 0x54, 0xa6,
	0xc9, 0xed, 0x2a, 0x6c, 0x2c, 0x62, 0xc5, 0x15,
	0xa2, 0xb7, 0x3d, 0xc3, 0xd9, 0x53, 0x0f, 0xf0
	};

char pass_06[] = "PASSPHRASE";

uint8 salt_06[] = "wpa_psk";

uint8 ref_06[] = {
	0x2c, 0x84, 0x30, 0x3a, 0x59, 0x3b, 0x91, 0x7f,
	0x5a, 0x9f, 0x53, 0x73, 0xc8, 0x5f, 0xe0, 0x1c,
	0x57, 0x87, 0xda, 0x02, 0xb0, 0x3f, 0xc0, 0x3c,
	0x09, 0x76, 0xe1, 0x52, 0x6f, 0x9d, 0xcc, 0x13,
	0x9d, 0xb4, 0x79, 0x46, 0x98, 0xdd, 0x51, 0x63
	};

char pass_07[] = "PASSPHRASE1";

uint8 salt_07[] = "wpa_psk";

uint8 ref_07[] = {
	0xf9, 0xdb, 0xe5, 0x4b, 0x58, 0xb2, 0xb0, 0xce,
	0x8f, 0x6e, 0xa3, 0xe4, 0xa4, 0xcd, 0xc6, 0x4b,
	0x11, 0x24, 0xab, 0x80, 0x32, 0x69, 0x32, 0x8e,
	0xea, 0xef, 0x9c, 0x26, 0x65, 0x2d, 0x9f, 0x60,
	0x49, 0x52, 0x48, 0x47, 0x02, 0x1d, 0x75, 0xe8
	};

char pass_08[] = "PASSPHRASE11";

uint8 salt_08[] = "wpa_psk";

uint8 ref_08[] = {
	0x65, 0x4d, 0x60, 0xa5, 0x24, 0xb7, 0xa8, 0xec,
	0x12, 0xa5, 0xeb, 0x01, 0x9d, 0xa9, 0xda, 0x1f,
	0xd0, 0x82, 0x92, 0x8f, 0xfe, 0x13, 0x6f, 0x1a,
	0x9b, 0x8a, 0x7f, 0xe9, 0x92, 0x74, 0xba, 0xbb,
	0x86, 0xef, 0x50, 0x50, 0x8d, 0xe3, 0x59, 0xc1
	};

char pass_09[] = "PASSPHRASE111";

uint8 salt_09[] = "wpa_psk";

uint8 ref_09[] = {
	0xac, 0xb8, 0x2e, 0xfd, 0x02, 0xf1, 0xa3, 0x6c,
	0x99, 0xe9, 0x21, 0x16, 0x30, 0xcf, 0x11, 0x96,
	0x7e, 0x26, 0x0b, 0x01, 0x26, 0xb0, 0x12, 0x45,
	0xc6, 0xd2, 0x65, 0xed, 0x1c, 0x40, 0xe2, 0xf9,
	0x3a, 0xc4, 0x5c, 0x80, 0x5a, 0xf9, 0x48, 0xe4
	};

char pass_10[] = "PASSPHRASE1111";

uint8 salt_10[] = "wpa_psk";

uint8 ref_10[] = {
	0x0f, 0xba, 0xf8, 0x61, 0xa5, 0xf0, 0x9f, 0xc7,
	0xec, 0xda, 0xb6, 0x7e, 0x0d, 0x11, 0x63, 0xbe,
	0xb2, 0x01, 0x91, 0xa1, 0xba, 0xce, 0x08, 0xd1,
	0xf8, 0x2e, 0xaa, 0x7a, 0x02, 0xc1, 0x0e, 0x1e,
	0x22, 0x2c, 0x42, 0x3f, 0xf0, 0xfe, 0xd1, 0x78
	};

char pass_11[] = "*Doyouknowtheway";

uint8 salt_11[] = "wpa";

uint8 ref_11[] = {
	0x0e, 0xdb, 0xaa, 0x85, 0x68, 0x3a, 0x8d, 0xc0,
	0x84, 0x58, 0x97, 0x54, 0x79, 0x97, 0x48, 0x86,
	0x5a, 0x2b, 0x63, 0xa6, 0x42, 0x8c, 0x65, 0x91,
	0x59, 0x88, 0xe9, 0x24, 0x30, 0xe4, 0xf8, 0x33,
	0x7c, 0x63, 0xda, 0x22, 0x96, 0xee, 0xa5, 0x4d
	};

char pass_12[] = "ToSanJose&&";

uint8 salt_12[] = "wpa";

uint8 ref_12[] = {
	0xef, 0x1b, 0x56, 0xee, 0xca, 0xdb, 0x92, 0xcc,
	0x4a, 0x5e, 0x42, 0x29, 0xcf, 0x13, 0xba, 0x6b,
	0x4d, 0x12, 0x0d, 0x19, 0x4a, 0x48, 0xc6, 0xb9,
	0xa4, 0xf8, 0xa8, 0x5c, 0x38, 0xea, 0x77, 0x30,
	0x01, 0x17, 0x80, 0xdc, 0xcb, 0x3f, 0x58, 0x00
	};

char pass_13[] = "I-Left-My-Heart";

uint8 salt_13[] = "802.11b";

uint8 ref_13[] = {
	0x0f, 0x11, 0xe3, 0x27, 0xb5, 0xaf, 0x29, 0xa5,
	0x02, 0x1a, 0x77, 0x70, 0x28, 0xed, 0xc7, 0x48,
	0x09, 0x96, 0x93, 0x35, 0x9c, 0xcf, 0xc9, 0xba,
	0x63, 0x29, 0xdb, 0xf5, 0x6d, 0x2f, 0xce, 0x5f,
	0xcf, 0x43, 0xd2, 0x4a, 0xe7, 0xdb, 0x7f, 0xd6
	};

char pass_14[] = "1nSanFranc1sc0";

uint8 salt_14[] = "802.11b";

uint8 ref_14[] = {
	0x5b, 0x93, 0xd9, 0x52, 0xa7, 0x7c, 0xa2, 0x42,
	0xd4, 0x59, 0x3b, 0x1f, 0x5a, 0xa3, 0xcf, 0x86,
	0x90, 0x6f, 0x44, 0x11, 0x4b, 0xd9, 0x82, 0xd5,
	0x4f, 0x1e, 0x91, 0xe9, 0xc4, 0xa4, 0x34, 0x8d,
	0x35, 0xfe, 0x1d, 0x4a, 0x49, 0xf2, 0x14, 0xca
	};

char pass_15[] = "passphras";

uint8 salt_15[] = "wpa_psk";

uint8 ref_15[] = {
	0x35, 0x73, 0x10, 0xdd, 0xfc, 0x20, 0x51, 0x88,
	0xe5, 0xbd, 0x7a, 0xfd, 0x93, 0x7c, 0x3c, 0x80,
	0xce, 0x0f, 0xbe, 0x75, 0x23, 0xef, 0x89, 0x5e,
	0x26, 0xed, 0x9c, 0x5b, 0x1f, 0x5d, 0x5d, 0x9e,
	0xa3, 0x05, 0x1a, 0xc5, 0xcb, 0x63, 0xb3, 0xb6
	};

char pass_16[] = "1234567890123456789012345678901";

uint8 salt_16[] = "wpa_psk";

uint8 ref_16[] = {
	0x67, 0x8b, 0xb9, 0x3d, 0x34, 0xb6, 0x6a, 0xce,
	0x00, 0x0e, 0xba, 0x82, 0x24, 0x14, 0xe0, 0xe8,
	0xc3, 0x6f, 0x0b, 0x7c, 0x9b, 0x80, 0x5a, 0x5e,
	0x15, 0xa5, 0x58, 0x09, 0x67, 0x6f, 0x44, 0xe2,
	0x15, 0x1e, 0xa3, 0x73, 0x58, 0xc8, 0x60, 0x6a
	};

char pass_17[] = "12345678901234567890123456789012";

uint8 salt_17[] = "wpa_psk";

uint8 ref_17[] = {
	0x72, 0x45, 0xd3, 0x12, 0x28, 0x2b, 0x8f, 0x62,
	0xda, 0x28, 0xeb, 0x0b, 0xef, 0x8f, 0x82, 0x1f,
	0x7c, 0x86, 0x6d, 0x39, 0x5b, 0xa7, 0x95, 0x67,
	0x1f, 0xef, 0x9e, 0x2b, 0xfa, 0x9d, 0xba, 0xff,
	0xcf, 0xe0, 0x2e, 0x28, 0xf0, 0x03, 0x9e, 0x04
	};

char pass_18[] = "123456789012345678901234567890123";

uint8 salt_18[] = "wpa_psk";

uint8 ref_18[] = {
	0x67, 0xc2, 0xda, 0x52, 0xf5, 0xd5, 0x56, 0x5a,
	0xc6, 0xf7, 0x4c, 0x52, 0x12, 0xd5, 0xde, 0xb1,
	0xc5, 0xd3, 0x6e, 0x8a, 0xde, 0x42, 0x9d, 0xb3,
	0x6f, 0x37, 0xf8, 0xf2, 0x12, 0x7d, 0xb6, 0x3e,
	0x02, 0x33, 0xb7, 0x86, 0x25, 0x10, 0x92, 0x19
	};

typedef struct {
	int pl;
	char *pass;
	int sl;
	uint8 *salt;
	uint8 *ref;
} passhash_vector_t;

#define PASSHASH_VECTOR_ENTRY(x)	\
	{ sizeof(pass_##x)-1, pass_##x, sizeof(salt_##x)-1, salt_##x, ref_##x }

passhash_vector_t passhash_vec[] = {
	PASSHASH_VECTOR_ENTRY(01),
	PASSHASH_VECTOR_ENTRY(02),
	PASSHASH_VECTOR_ENTRY(03),
	PASSHASH_VECTOR_ENTRY(04),
	PASSHASH_VECTOR_ENTRY(05),
	PASSHASH_VECTOR_ENTRY(06),
	PASSHASH_VECTOR_ENTRY(07),
	PASSHASH_VECTOR_ENTRY(08),
	PASSHASH_VECTOR_ENTRY(09),
	PASSHASH_VECTOR_ENTRY(10),
	PASSHASH_VECTOR_ENTRY(11),
	PASSHASH_VECTOR_ENTRY(12),
	PASSHASH_VECTOR_ENTRY(13),
	PASSHASH_VECTOR_ENTRY(14),
	PASSHASH_VECTOR_ENTRY(15),
	PASSHASH_VECTOR_ENTRY(16),
	PASSHASH_VECTOR_ENTRY(17),
	PASSHASH_VECTOR_ENTRY(18)
};
#define NUM_PASSHASH_VECTORS	(sizeof(passhash_vec)/sizeof(passhash_vec[0]))
