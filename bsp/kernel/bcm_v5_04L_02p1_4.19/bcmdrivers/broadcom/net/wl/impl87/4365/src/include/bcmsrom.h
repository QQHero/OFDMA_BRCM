/*
 * Misc useful routines to access NIC local SROM/OTP .
 *
 * Copyright (C) 2022, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: bcmsrom.h 708017 2017-06-29 14:11:45Z $
 */

#ifndef	_bcmsrom_h_
#define	_bcmsrom_h_

#include <typedefs.h>
#include <osl_decl.h>
#include <siutils.h>

#include <bcmsrom_fmt.h>

/* Prototypes */
extern int srom_var_init(si_t *sih, uint bus, void *curmap, osl_t *osh,
                         char **vars, uint *count);
extern void srom_var_deinit(si_t *sih);

extern int srom_read(si_t *sih, uint bus, void *curmap, osl_t *osh,
                     uint byteoff, uint nbytes, uint16 *buf,
                     bool check_crc);

extern int srom_write(si_t *sih, uint bus, void *curmap, osl_t *osh,
                      uint byteoff, uint nbytes, uint16 *buf);

extern int srom_write_short(si_t *sih, uint bustype, void *curmap, osl_t *osh,
                            uint byteoff, uint16 value);
extern int srom_otp_cisrwvar(si_t *sih, osl_t *osh, char *vars, int *count);
extern int srom_otp_write_region_crc(si_t *sih, uint nbytes, uint16* buf16, bool write);

/* parse standard PCMCIA cis, normally used by SB/PCMCIA/SDIO/SPI/OTP
 *   and extract from it into name=value pairs
 */
extern int srom_probe_boardtype(si_t *sih, uint8 *pcis[], uint ciscnt);
extern int srom_parsecis(si_t *sih, osl_t *osh, uint8 **pcis, uint ciscnt,
                         char **vars, uint *count);

#if defined(BCMUSBDEV) || defined(BCMSDIODEV)
/* Return sprom size in 16-bit words */
extern uint srom_size(si_t *sih, osl_t *osh);
#endif

#endif	/* _bcmsrom_h_ */
