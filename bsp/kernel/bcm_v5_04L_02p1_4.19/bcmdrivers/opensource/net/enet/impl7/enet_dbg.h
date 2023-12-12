/*
   <:copyright-BRCM:2015:DUAL/GPL:standard
   
      Copyright (c) 2015 Broadcom 
      All Rights Reserved
   
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
 *  Created on: Nov/2015
 *      Author: ido@broadcom.com
 */

#ifndef _ENET_DBG_H_
#define _ENET_DBG_H_

#define enet_err(fmt, ...) \
    printk(KERN_ERR "%s:L%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

//#define enet_dbg_enabled 
//#define enet_dbg_rx_enabled
//#define enet_dbg_tx_enabled
//#define enet_dbgv_enabled   /* verbose debug */


#if defined(enet_dbg_enabled)
#define enet_dbg(fmt, ...) \
    printk(KERN_ERR "%s:L%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define enet_dbg(fmt, ...) \
    {}
#endif

#if defined(enet_dbg_enabled) && defined(enet_dbg_rx_enabled)
#define enet_dbg_rx(fmt, ...) \
    printk(KERN_ERR "%s:L%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define enet_dbg_rx(fmt, ...) \
    {}
#endif

#if defined(enet_dbg_enabled) && defined(enet_dbg_tx_enabled)
#define enet_dbg_tx(fmt, ...) \
    printk(KERN_ERR "%s:L%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define enet_dbg_tx(fmt, ...) \
    {}
#endif

#if defined(enet_dbg_enabled) && defined(enet_dbgv_enabled)
#define enet_dbgv(fmt, ...) \
    printk(KERN_ERR " DBGV>%s:L%d "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define enet_dbgv(fmt, ...) \
    {}
#endif



#endif // _ENET_DBG_H_

