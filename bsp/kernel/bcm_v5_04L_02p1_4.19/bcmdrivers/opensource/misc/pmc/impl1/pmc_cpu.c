/*
<:copyright-BRCM:2020:DUAL/GPL:standard

   Copyright (c) 2020 Broadcom 
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
#include "pmc_drv.h"
#include "pmc_cpu.h"
#include "BPCM.h"

int pmc_cpu_init(unsigned int cpu_id)
{
	BPCM_PWR_ZONE_N_CONTROL zctl;
	int rc;

	rc = ReadZoneRegister(cpu_pmb[cpu_id], 0, BPCMZoneOffset(control), &zctl.Reg32);
	if (rc == 0 && zctl.Bits.reset_state) 
		PowerOnZone(cpu_pmb[cpu_id], 0);

    return rc;
}

int pmc_cpu_shutdown(unsigned int cpu_id)
{
	return PowerOffZone(cpu_pmb[cpu_id], 0); // XXX repower flag ignored
}
