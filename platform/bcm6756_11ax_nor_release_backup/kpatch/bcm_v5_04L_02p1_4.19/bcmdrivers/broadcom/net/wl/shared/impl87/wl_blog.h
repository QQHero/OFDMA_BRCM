/*
    Copyright (c) 2017 Broadcom
    All Rights Reserved

    <:label-BRCM:2017:DUAL/GPL:standard

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

#ifndef _wl_blog_h_
#define _wl_blog_h_

#ifdef mips
#undef ABS
#endif

#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/nbuff.h>

struct wl_info;
struct wl_if;
struct wlc_event;

extern struct sk_buff *wl_xlate_to_skb(struct wl_info *wl, struct sk_buff *s);
extern int wl_handle_blog_emit(struct wl_info *wl, struct wl_if *wlif, struct sk_buff *skb,
    struct net_device *dev);
extern int wl_handle_blog_sinit(struct wl_info *wl, struct sk_buff *skb);

extern void wl_handle_blog_event(struct wl_info *wl, struct wlc_event *e);
#endif /* _wl_blog_h_ */
