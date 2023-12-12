/*
   Copyright (c) 2015 Broadcom Corporation
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

:>
*/

/*
 *  Created on: Dec 2015
 *      Author: yuval.raviv@broadcom.com
 */

#ifndef __OS_DEP_H__
#define __OS_DEP_H__

/* uint32_t types */
#include "linux/types.h"

/* udelay */
#include <asm/delay.h>

/* NULL */
#if !defined(NULL)
#define NULL 0
#endif

/* memset */
#include "linux/string.h"

/* printk */
#include "linux/printk.h"

/* spinlock */
#include <linux/spinlock.h>

/* EXPORT_SYMBOL */
#include "linux/export.h"

#endif
