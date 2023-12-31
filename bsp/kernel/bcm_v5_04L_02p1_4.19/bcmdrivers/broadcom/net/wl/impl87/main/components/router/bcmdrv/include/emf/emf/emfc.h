/*
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
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: emfc.h 523133 2014-12-27 05:50:30Z $
 */

#ifndef _EMFC_H_
#define _EMFC_H_
#if defined(BCM_NBUFF_WLMCAST_IPV6)
#include <bcmipv6.h>
#endif
#define MFDB_HASHT_SIZE         8
#define MFDB_MGRP_HASH(m)       ((((m) >> 24) + ((m) >> 16) + \
				  ((m) >> 8) + ((m) & 0xff)) & 7)

#define IP_ISMULTI(a)           (((a) & 0xf0000000) == 0xe0000000)
#define MCAST_ADDR_LINKLOCAL(a) (((a) & 0xffffff00) == 0xe0000000)
#define MCAST_ADDR_UPNP_SSDP(a) ((a) == 0xeffffffa)

#define EMFC_STATS_INCR(emfc, member) (((emfc)->stats.member)++)

#define EMFC_PROT_STATS_INCR(emfc, proto, mem1, mem2) (((proto) == IP_PROT_IGMP) ? \
	                                               (((emfc)->stats.mem1)++) : \
	                                               (((emfc)->stats.mem2)++))

/*
 * Multicast Group entry of MFDB
 */
typedef struct emfc_mgrp
{
	clist_head_t     mgrp_hlist;    /* Multicast Groups hash list */
#if defined(BCM_NBUFF_WLMCAST_IPV6)
	union {
		uint32				mgrp_ip;	/* Multicast Group IP Address */
		struct ipv6_addr	mgrp_ipv6;	/* Multicast Group IP Address */
	};
#else
	uint32			 mgrp_ip;		/* Multicast Group IP Address */
#endif
	clist_head_t     mi_head;       /* List head of interfaces */
} emfc_mgrp_t;

/*
 * Multicast Interface entry of MFDB
 */
typedef struct emfc_mhif
{
	struct emfc_mhif *prev,*next;   /* Multicast host i/f prev and next */
	void             *mhif_ifp;     /* Interface pointer */
	uint32           mhif_data_fwd; /* Number of MCASTs sent on the i/f */
	uint32           mhif_ref;      /* Ref count of updates */
} emfc_mhif_t;

typedef struct emfc_mi
{
	clist_head_t     mi_list;       /* Multicast i/f list prev and next */
	emfc_mhif_t      *mi_mhif;      /* MH interface data */
	uint32           mi_ref;        /* Ref count of updates */
	uint32           mi_data_fwd;   /* Number of MCASTs of group fwd */
} emfc_mi_t;

/*
 * Interface list managed by EMF instance. Instead of having separate lists
 * for router ports and unregistered frames forwarding ports (uffp) we use
 * one interface list. Each interface list entry has flags (infact ref count)
 * indicating the entry type. An interface can be of type rtport, uffp or
 * both. As a general rule IGMP frames are forwarded on rtport type interfaces.
 * Unregistered data frames are forwareded on to rtport and uffp interfaces.
 */
typedef struct emfc_iflist
{
	struct emfc_iflist *next;       /* Interface list next */
	void               *ifp;        /* Interface pointer */
	uint32             uffp_ref;    /* Ref count for UFFP type */
	uint32             rtport_ref;  /* Ref count for rtport type */
} emfc_iflist_t;

/*
 * Multicast Forwarding Layer Information
 */
typedef struct emfc_info
{
	clist_head_t     emfc_list;     /* EMFC instance list prev and next */
	int8             inst_id[16];   /* EMFC instance identifier */
	                                /* Multicast forwarding database */
	clist_head_t     mgrp_fdb[MFDB_HASHT_SIZE];
	emfc_mhif_t      *mhif_head;    /* Multicast Host interface list */
	uint32           mgrp_cache_ip; /* Multicast Group IP Addr in cache */
	emfc_mgrp_t      *mgrp_cache;   /* Multicast Group cached entry */
	osl_lock_t       fdb_lock;      /* Lock for FDB access */
	void             *osh;          /* OS Layer handle */
	void             *emfi;         /* OS Specific MFL data */
	emf_stats_t      stats;         /* Multicast frames statistics */
	emfc_snooper_t	 *snooper;	/* IGMP Snooper data */
	emfc_wrapper_t   wrapper;       /* EMFC wrapper info  */
	bool			 emf_enable;	/* Enable/Disable EMF */
	bool			 mc_data_ind;	/* Indicate mcast data frames */
	osl_lock_t       iflist_lock;   /* Lock for UFFP list access */
	emfc_iflist_t    *iflist_head;  /* UFFP list head */

#if defined(BCM_NBUFF_WLMCAST_IPV6)
	osl_lock_t		 fdb_lock_ipv6;			/* Lock for FDB access */
	clist_head_t	 mgrp_fdb_ipv6[MFDB_HASHT_SIZE];
	emfc_mhif_t		 *mhif_head_ipv6;		/* Multicast Host interface list */
	struct ipv6_addr mgrp_cache_ipv6_addr;	/* ipv6 cached address */
	emfc_mgrp_t		 *mgrp_cache_ipv6_grp;	/* Multicast Group cached entry */
	emf_stats_t		  stats_ipv6;			/* Multicast frames statistics */
#endif
} emfc_info_t;

#if defined(BCM_NBUFF_WLMCAST_IPV6)
#define MFDB_MGRP_HASH_IPV6(m)		\
	((((m).s6_addr32[0])+((m).s6_addr32[1])+((m).s6_addr32[2])+((m).s6_addr32[3])) &7)
#define EMFC_STATS_INCR_IPV6(emfc, member) (((emfc)->stats_ipv6.member)++)
extern emfc_info_t *emfc_instance_find(char *inst_id);
#else
static emfc_mgrp_t *emfc_mfdb_group_find(emfc_info_t *emfc, uint32 mgrp_ip);
#endif

#endif /* _EMFC_H_ */
