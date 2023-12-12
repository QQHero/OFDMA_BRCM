/*
<:copyright-BRCM:2019:DUAL/GPL:standard

   Copyright (c) 2019 Broadcom 
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

#ifndef __BCM63146_MAP_PART_H
#define __BCM63146_MAP_PART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bcmtypes.h"
#include "bcm_io_map.h"

#define CHIP_FAMILY_ID_HEX 0x63146

#ifndef __ASSEMBLER__
enum 
{
    PERF_IDX,
    PERF1_IDX,
    AHB_IDX,
    AHB_A0_IDX,
    ARM_UART_IDX,
    NANDFLASH_IDX,
    PCM_IDX,
    PCMBUS_IDX,
    MEMC_IDX,
#if defined(_ATF_)
    PMC_IDX,
#endif
    WAN_IDX,
    GPON_IDX,
    NTR_IDX,
    NGPON2_IDX,
    EPON_IDX,
    WANBBH_IDX,
    XRDP_IDX,
    ETH_PHYS_IDX,
    SATA_IDX,
    USBH_IDX,
    DSLPHY_IDX,
    DSLLMEM_IDX,
    DSLXMEM_IDX,
    SYSPORT_IDX,
    CRYPTO_IDX,
    BIUCFG_IDX,
    SAR_IDX,
    XPORT_IDX,
    XLIF_IDX,
    CCI500_IDX,
    UBUS_MAPPED_IDX,
#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
    MPM_IDX,
#endif
    MST_PORT_NODE_PER_IDX,
    MST_PORT_NODE_USB_IDX,
    MST_PORT_NODE_B53_IDX,    
    MST_PORT_NODE_PCIE0_IDX,
    MST_PORT_NODE_PCIE1_IDX,
    MST_PORT_NODE_PCIE2_IDX,
    MST_PORT_NODE_DSL_IDX,    
    MST_PORT_NODE_DSLCPU_IDX,
    MST_PORT_NODE_RQ0_IDX,
    MST_PORT_NODE_DMA0_IDX,
    MST_PORT_NODE_DMA1_IDX,
    MST_PORT_NODE_QM_IDX,
    UBUS4_COHERENCY_PORT_IDX,
    CAPTURE_PORT_NODE_0_IDX, 
    CAPTURE_PORT_NODE_1_IDX, 
#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
    MST_PORT_NODE_MPM_IDX,
#endif
    LAST_IDX
};
#endif

/* Perf block base address and size */
#define PERF_PHYS_BASE        0xff800000
#define PERF_SIZE             0x3000 

/* perf block offset */
#define PERF_OFFSET           0x0000   /* chip control */
#define INTSET0_OFFSET        0x0100   /* Legacy Interrupt control */
#define GPIO_OFFSET           0x0500   /* gpio registers */
#define UART_OFFSET           0x0640   /* A0 uart registers */
#define UART1_OFFSET          0x0660
#define UART2_OFFSET          0x06c0
#define LED_OFFSET            0x0800   /* led registers */
#define RNG_OFFSET            0x0b80   /* rng registers */
#define SOTP_OFFSET           0x5000
#define FSR_OFFSET            0x5300
#define JTAG_IOTP_OFFSET      0x0e80
#define HSSPIM_OFFSET         0x1000   /* High-Speed SPI registers */
#define NAND_REG_OFFSET       0x1800   /* nand interrupt control */
#define NAND_CACHE_OFFSET     0x1c00   /* NAND cache register */
#define NAND_INTR_OFFSET      0x2000   /* NAND int register */
#define I2S_OFFSET            0x2080   /* I2S regsiters */
#define I2C_OFFSET            0x2100   /* I2C regsiters */
#define MISC_OFFSET           0x2600   /* Miscellaneous Registers */

#define TIMR_OFFSET           0x0400   /* 64 bit timer registers */
#define JTAG_OTP_OFFSET       0x2800

#define TIMR_PHYS_BASE        (PERF_PHYS_BASE+TIMR_OFFSET)
#define GPIO_PHYS_BASE        (PERF_PHYS_BASE+GPIO_OFFSET)
#define UART_PHYS_BASE        (PERF_PHYS_BASE+UART_OFFSET)
#define UART1_PHYS_BASE       (PERF_PHYS_BASE+UART1_OFFSET)
#define UART2_PHYS_BASE       (PERF_PHYS_BASE+UART2_OFFSET)
#define LED_PHYS_BASE         (PERF_PHYS_BASE+LED_OFFSET)
#define RNG_PHYS_BASE         (PERF_PHYS_BASE+RNG_OFFSET)
#define SOTP_PHYS_BASE        (PERF_PHYS_BASE+SOTP_OFFSET)
#define JTAG_OTP_PHYS_BASE    (PERF_PHYS_BASE+JTAP_OTP_OFFSET)
#define JTAG_IOTP_PHYS_BASE   (PERF_PHYS_BASE+JTAP_IOTP_OFFSET)
#define HSSPIM_PHYS_BASE      (PERF_PHYS_BASE+HSSPIM_OFFSET)
#define NAND_REG_PHYS_BASE    (PERF_PHYS_BASE+NAND_REG_OFFSET)
#define NAND_CACHE_PHYS_BASE  (PERF_PHYS_BASE+NAND_CACHE_OFFSET)
#define NAND_INTR_PHYS_BASE   (PERF_PHYS_BASE+NAND_INTR_OFFSET)
#define I2S_PHYS_BASE         (PERF_PHYS_BASE+I2S_OFFSET)
#define I2C_PHYS_BASE         (PERF_PHYS_BASE+I2C_OFFSET)
#define MISC_PHYS_BASE        (PERF_PHYS_BASE+MISC_OFFSET)

/* Perf1 block base address and size */
#define PERF1_PHYS_BASE       0xff85a000
#define PERF1_SIZE            0x1000
/* perf1 block offset */
#define PERF1_OFFSET          0x0000
#define TOP_CONTROL_OFFSET    0x0000
#define I2C_2_OFFSET          0x0800

#define TOP_CONTROL_PHYS_BASE (PERF1_PHYS_BASE+TOP_CONTROL_OFFSET)
#define I2C_2_PHYS_BASE       (PERF1_PHYS_BASE+I2C_2_OFFSET)

#define AHB_PHYS_BASE_A0      0xff858000
#define AHB_PHYS_BASE         0xff810000
#define AHB_SIZE              0x1300
#define AHB_OFFSET            0x0000 
#define EMMC_HOSTIF_OFFSET    0x0000 
#define EMMC_TOP_CFG_OFFSET   0x0100
#define EMMC_BOOT_OFFSET      0x0200 
#define AHBSS_CTRL_OFFSET     0x0300
#define PL081_DMA_OFFSET      0x1000

/* ARM UART for B0. Can merge to AHB block when A0 support is removed*/
#define ARM_UART_PHYS_BASE    0xff812000
#define ARM_UART_SIZE         0x3000

#define ARM_UART_OFFSET       0x0000
#define ARM_UART1_OFFSET      0x1000
#define ARM_UART2_OFFSET      0x2000

#define ARM_UART1_PHYS_BASE   (ARM_UART_PHYS_BASE+ARM_UART1_OFFSET)
#define ARM_UART2_PHYS_BASE   (ARM_UART_PHYS_BASE+ARM_UART2_OFFSET)

/* EMMC direct access window */
#define EMMCFLASH_PHYS_BASE   0xffc00000
#define EMMCFLASH_SIZE        0x100000
#define EMMCFLASH_OFFSET      0x0000 

/* SPI NOR direct access window */
#define SPIFLASH_PHYS_BASE    0xffd00000
#define SPIFLASH_SIZE         0x100000
#define SPIFLASH_OFFSET       0x0000 

/* nand flash direct access address */
#define NANDFLASH_PHYS_BASE   0xffe00000
#define NANDFLASH_SIZE        0x100000
#define NANDFLASH_OFFSET      0x0000 

#define PCM_PHYS_BASE         0xff860000
#define APM_CORE_OFFSET       0x00000000
#define PCM_CORE_OFFSET       0x00000C00
#define PCM_DMA_OFFSET        0x00001800
#define PCM_SIZE              0x2000
#define PCMBUS_PHYS_BASE      0x83010A00
#define PCMBUS_OFFSET         0x00000000
#define PCMBUS_SIZE           0x100

#define MEMC_PHYS_BASE        0x80040000  /* MC2 TOP */
#define MEMC_SIZE             0x00040000
#define MEMC_OFFSET           0x00000000

#define PCIE0_PHYS_BASE       0x80110000
#define PCIE0_SIZE            0x0000A000
#define PCIE1_PHYS_BASE       0x80120000
#define PCIE1_SIZE            0x0000A000
#define PCIE2_PHYS_BASE       0x80130000
#define PCIE2_SIZE            0x0000A000

#define PCIE0_MEM_PHYS_BASE   0xc0000000
#define PCIE0_MEM_SIZE        0x10000000
#define PCIE1_MEM_PHYS_BASE   0xd0000000
#define PCIE1_MEM_SIZE        0x10000000
#define PCIE2_MEM_PHYS_BASE   0xe0000000
#define PCIE2_MEM_SIZE        0x10000000

#define PMC_PHYS_BASE         0xffa00000
#define PMC_SIZE              0x00200000
#define PMC_OFFSET            0x00100000
#define PROC_MON_OFFSET       0x00100000
#define PMB_OFFSET            0x00120100

#define ETH_PHYS_BASE         0x83400000  
#define ETH_PHYS_SIZE           0x400000
#define ETH_PHYS_SERDES_0_CORE_OFFSET  0
#define ETH_PHYS_SERDES_0_MISC_OFFSET  0x3ff500
#define ETH_PHYS_SERDES_1_CORE_OFFSET  0x40000
#define ETH_SERDES_R2PMI_LP_0_INDIR_CNTRL_OFFSET  0x3ff5f0
#define ETH_PHYS_REG_OFFSET     0x3ff000
#define ETH_PHYS_MDIO_OFFSET    0x3ffd00

#define SYSPORT_PHYS_BASE         0x80490000  
#define SYSPORT_SIZE              0x10000
#define SYSPORT_OFFSET            0x00000
#define SYSPORT_SYSBUSCFG_OFFSET  0x00040
#define SYSPORT_INTR_0_OFFSET     0x200
#define SYSPORT_INTR_1_OFFSET     0x240
#define SYSPORT_RXCHK_OFFSET      0x300
#define SYSPORT_TXCHK_OFFSET      0x380
#define SYSPORT_RBUF_OFFSET       0x400
#define SYSPORT_TBUF_OFFSET       0x600
#define SYSPORT_UMAC_OFFSET       0x800
    #define SYSPORT_MIB_OFFSET   (SYSPORT_UMAC_OFFSET + 0x400)
#define SYSPORT_RDMA_OFFSET       0x2000
#define SYSPORT_TDMA_OFFSET       0x4000

#define SATA_PHYS_BASE        0x80008000
#define SATA_SIZE             0x4000
#define SATA_OFFSET           0x0000

#define USBH_PHYS_BASE        0x80100000
#define USBH_SIZE             0xffff
#define XHCI_OFFSET           0x0000     /* USB host registers */
#define XHCI_EC_OFFSET        0x880    /* XHCI extended registers */
#define XHCI_GBL_OFFSET       0xc100
#define XHCI_GBL_SIZE         0x544
#define USBH_OFFSET           0xf000
#define CFG_OFFSET            0xf200

#define USB_XHCI_PHYS_BASE    (USBH_PHYS_BASE+XHCI_OFFSET)

#define SAR_PHYS_BASE         0x80600000
#define SAR_SIZE              0x4000
#define SAR_OFFSET            0x0000

/* to support non-DT pltaform device add below defs */
#define WAN_PHYS_BASE         0x80144000
#define WAN_SIZE              0x1000
#define GPON_PHYS_BASE        0x80148000
#define GPON_SIZE             0x8b00
#define NTR_PHYS_BASE         0x80158000
#define NTR_SIZE              0x0400
#define NTR_RESET_OFFSET      0x00000000
#define NTR_NCO_OFFSET        0x300
#define NTR_CLK_PRG_SWCH_OFFSET 0x340
#define NGPON2_PHYS_BASE      0x80160000
#define NGPON2_SIZE           0xE000
#define EPON_PHYS_BASE        0x80140000
#define EPON_SIZE             0x4000
#define WANBBH_PHYS_BASE      0x80170000
#define WANBBH_SIZE           0x9000


#define XRDP_PHYS_BASE        0x82000000
#define XRDP_SIZE             0xCAF004
#define XRDP_OFFSET           0x0000

#define DSLPHY_PHYS_BASE      0x80750000
#define DSLPHY_SIZE           0x20000
#define DSLPHY_OFFSET         0x0000
#define DSLLMEM_PHYS_BASE     0x80800000
#define DSLLMEM_SIZE          0xe0000
#define DSLLMEM_OFFSET        0x0000
#define DSLXMEM_PHYS_BASE     0x809A0000
#define DSLXMEM_SIZE          0x660000
#define DSLXMEM_OFFSET        0x0000

#define GIC_PHYS_BASE        0x81000000
#define GIC_SIZE             0x10000
#define GIC_OFFSET           0x0000
#define GICD_OFFSET          0x1000
#define GICC_OFFSET          0x2000

#define BIUCFG_PHYS_BASE     0x81060000
#define BIUCFG_SIZE          0x3000
#define BIUCFG_OFFSET        0x0000

#define CCI500_PHYS_BASE     0x81100000
#define CCI500_SIZE          0x91000
#define CCI500_OFFSET        0x0000

#define CRYPTO_PHYS_BASE     0x8001c000
#define CRYPTO_SIZE          0x4000
#define CRYPTO_OFFSET        0x0000
#define CRYPTO_PDC_OFFSET    0x0UL
#define CRYPTO_SPU_OFFSET    0x1000UL
#define CRYPTO_SPU_KC_OFFSET 0x1800UL

#define UNIMAC_OFFSET        0x008a8000
#define UNIMAC_CFG_OFFSET    (UNIMAC_OFFSET + 0x00000000)
#define UNIMAC_MIB_OFFSET    (UNIMAC_OFFSET + 0x00000400)
#define UNIMAC_MPD_OFFSET    (UNIMAC_OFFSET + 0x00000700)
#define UNIMAC_XIB_OFFSET    (UNIMAC_OFFSET + 0x00000740)
#define UNIMAC_TCTL_OFFSET   (UNIMAC_OFFSET + 0x00000780)
#define UNIMAC_TOP_OFFSET    (UNIMAC_OFFSET + 0x00008000)

#define XPORT_PHYS_BASE         0x80138000
#define XPORT_OFFSET            0x00000
#define XPORT_SIZE              0x0331C
#define XPORT_XLMAC_CORE_OFFSET 0x00000
#define XPORT_XLMAC_CORE_SIZE   0x00DBC
#define XPORT_MIB_CORE_OFFSET   0x01000
#define XPORT_MIB_CORE_SIZE     0x00F64
#define XPORT_REG_OFFSET        0x02000
#define XPORT_REG_SIZE          0x0020C
#define XPORT_XLMAC_REG_OFFSET  0x03000
#define XPORT_XLMAC_REG_SIZE    0x00044
#define XPORT_MIB_REG_OFFSET    0x03100
#define XPORT_MIB_REG_SIZE      0x00068
#define XPORT_INTRL2_OFFSET     0x03200
#define XPORT_INTRL2_SIZE       0x00030
#define XPORT_MAB_REG_OFFSET    0x03300
#define XPORT_MAB_REG_SIZE      0x0001C

#define XLIF_PHYS_BASE      0x80147800
#define XLIF_OFFSET         0x00000
#define XLIF_SIZE           0x00800

#define UBUS_MAPPED_PHYS_BASE               0x83000000
#define UBUS_MAPPED_SIZE                    0x1000
#define SYS_CLK_CTRL_OFFSET                 0x80
#define SYS_ERR_PORT_CFG_OFFSET             0x100
#define UBUS_SYS_MODULE_REGISTRATION_OFFSET 0x200

#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
#define MPM_PHYS_BASE		0x80020000
#define MPM_SIZE		0x4000
#define MPM_DMA_BALLOC_OFFSET	0x0
#define MPM_DMA_BFREE_OFFSET	0x2000
#define MPM_DMA_COMMON_OFFSET	0x2D00
#define MPM_DMA_SMA_OFFSET	0x2F00
#define MPM_CORE_OFFSET		0x3000
#define MPM_COMMON_OFFSET	0x3D00
#define MPM_INTRL2_OFFSET	0x3E00
#define MPM_BB_OFFSET    	0x3F00
#endif

#define MST_PORT_NODE_PER_PHYS_BASE         0x83010000
#define MST_PORT_NODE_PER_SIZE              0x4000
#define MST_PORT_NODE_USB_PHYS_BASE         0x83018000
#define MST_PORT_NODE_USB_SIZE              0x4000
#define MST_PORT_NODE_B53_PHYS_BASE         0x83020000
#define MST_PORT_NODE_B53_SIZE              0x4000
#define MST_PORT_NODE_PCIE0_PHYS_BASE       0x83028000
#define MST_PORT_NODE_PCIE0_SIZE            0x4000
#define MST_PORT_NODE_PCIE1_PHYS_BASE       0x83030000
#define MST_PORT_NODE_PCIE1_SIZE            0x4000
#define MST_PORT_NODE_PCIE2_PHYS_BASE       0x83038000
#define MST_PORT_NODE_PCIE2_SIZE            0x4000
#define MST_PORT_NODE_DSL_PHYS_BASE         0x83048000
#define MST_PORT_NODE_DSL_SIZE              0x4000
#define MST_PORT_NODE_DSLCPU_PHYS_BASE      0x83050000
#define MST_PORT_NODE_DSLCPU_SIZE           0x4000
#define MST_PORT_NODE_RQ0_PHYS_BASE         0x83058000
#define MST_PORT_NODE_RQ0_SIZE              0x4000
#define MST_PORT_NODE_DMA0_PHYS_BASE        0x83060000
#define MST_PORT_NODE_DMA0_SIZE             0x4000
#define MST_PORT_NODE_DMA1_PHYS_BASE        0x83068000
#define MST_PORT_NODE_DMA1_SIZE             0x4000
#define MST_PORT_NODE_QM_PHYS_BASE          0x83070000
#define MST_PORT_NODE_QM_SIZE               0x4000
#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
#define MST_PORT_NODE_MPM_PHYS_BASE         0x83080000
#define MST_PORT_NODE_MPM_SIZE              0x4000
#endif

#define UBUS4_COHERENCY_PORT_PHYS_BASE      0x810A0000
#define UBUS4_COHERENCY_PORT_SIZE           0x1000
#define UBUS4_RANGE_CHK_SETUP_OFFSET        0x0
#define UBUS4_RANGE_CHK_CONFIG_OFFSET       0x310
#define UBUS4_COHERENCY_PORT_CONFIG_OFFSET  0x400

#define CAPTURE_PORT_NODE_0_PHYS_BASE       0x8307C000
#define CAPTURE_PORT_NODE_0_SIZE            0x0400
#define CAPTURE_PORT_NODE_1_PHYS_BASE       0x83078000
#define CAPTURE_PORT_NODE_1_SIZE            0x0400

/* Physical and access(could be virtual or physical) bases address for
 * all the registers */
#define PERF_BASE          BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, PERF_OFFSET)
#define INTSET0_BASE       BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, INTSET0_OFFSET)
#define TIMR_BASE          BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, TIMR_OFFSET)
#define GPIO_BASE          BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, GPIO_OFFSET)
#define UART_BASE          BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, UART_OFFSET)
#define UART1_BASE         BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, UART1_OFFSET)
#define UART2_BASE         BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, UART2_OFFSET)
#define LED_BASE           BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, LED_OFFSET)
#define RNG_BASE           BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, RNG_OFFSET)
#define SOTP_BASE          BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, SOTP_OFFSET)
#define JTAG_OTP_BASE      BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, JTAG_OTP_OFFSET)
#define JTAG_IOTP_BASE     BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, JTAG_IOTP_OFFSET)
#define HSSPIM_BASE        BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, HSSPIM_OFFSET)
#define NAND_REG_BASE      BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, NAND_REG_OFFSET)
#define NAND_CACHE_BASE    BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, NAND_CACHE_OFFSET)
#define NAND_INTR_BASE     BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, NAND_INTR_OFFSET)
#define I2S_BASE           BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, I2S_OFFSET)
#define I2C_BASE           BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, I2C_OFFSET)
#define MISC_BASE          BCM_IO_MAP(PERF_IDX, PERF_PHYS_BASE, MISC_OFFSET)

#define PERF1_BASE         BCM_IO_MAP(PERF1_IDX, PERF1_PHYS_BASE, PERF1_OFFSET)
#define TOP_CONTROL_BASE   BCM_IO_MAP(PERF1_IDX, PERF1_PHYS_BASE, TOP_CONTROL_OFFSET)
#define I2C_2_BASE         BCM_IO_MAP(PERF1_IDX, PERF1_PHYS_BASE, I2C_2_OFFSET)

#define AHB_BASE           BCM_IO_MAP(AHB_IDX, AHB_PHYS_BASE, AHB_OFFSET)
#define EMMC_HOSTIF_BASE   BCM_IO_MAP(AHB_IDX, AHB_PHYS_BASE, EMMC_HOSTIF_OFFSET)
#define EMMC_TOP_CFG_BASE  BCM_IO_MAP(AHB_IDX, AHB_PHYS_BASE, EMMC_TOP_CFG_OFFSET)
#define EMMC_BOOT_BASE     BCM_IO_MAP(AHB_IDX, AHB_PHYS_BASE, EMMC_BOOT_OFFSET)
#define AHBSS_CTRL_BASE    BCM_IO_MAP(AHB_IDX, AHB_PHYS_BASE, AHBSS_CTRL_OFFSET)
#define PL081_DMA_BASE     BCM_IO_MAP(AHB_IDX, AHB_PHYS_BASE, PL081_DMA_OFFSET)
#define ARM_UART_BASE      BCM_IO_MAP(ARM_UART_IDX, ARM_UART_PHYS_BASE, ARM_UART_OFFSET)

#define AHB_BASE_A0          BCM_IO_MAP(AHB_A0_IDX, AHB_PHYS_BASE_A0, AHB_OFFSET)
#define EMMC_HOSTIF_BASE_A0  BCM_IO_MAP(AHB_A0_IDX, AHB_PHYS_BASE_A0, EMMC_HOSTIF_OFFSET)
#define EMMC_TOP_CFG_BASE_A0 BCM_IO_MAP(AHB_A0_IDX, AHB_PHYS_BASE_A0, EMMC_TOP_CFG_OFFSET)
#define EMMC_BOOT_BASE_A0    BCM_IO_MAP(AHB_A0_IDX, AHB_PHYS_BASE_A0, EMMC_BOOT_OFFSET)
#define AHBSS_CTRL_BASE_A0   BCM_IO_MAP(AHB_A0_IDX, AHB_PHYS_BASE_A0, AHBSS_CTRL_OFFSET)
#define PL081_DMA_BASE_A0    BCM_IO_MAP(AHB_A0_IDX, AHB_PHYS_BASE_A0, PL081_DMA_OFFSET)

#define NANDFLASH_BASE     BCM_IO_MAP(NANDFLASH_IDX, NANDFLASH_PHYS_BASE, NANDFLASH_OFFSET)
#define MEMC_BASE          BCM_IO_MAP(MEMC_IDX, MEMC_PHYS_BASE, MEMC_OFFSET)


#if defined(_ATF_)
#define PMC_BASE           BCM_IO_MAP(PMC_IDX, PMC_PHYS_BASE, PMC_OFFSET)
#define PROC_MON_BASE      BCM_IO_MAP(PMC_IDX, PMC_PHYS_BASE, PROC_MON_OFFSET)
#define PMB_BASE           BCM_IO_MAP(PMC_IDX, PMC_PHYS_BASE, PMB_OFFSET)
#endif
#define WAN_BASE           BCM_IO_MAP(WAN_IDX, WAN_PHYS_BASE, 0)
#define GPON_BASE          BCM_IO_MAP(GPON_IDX, GPON_PHYS_BASE, 0)
#define NTR_RESET_BASE     BCM_IO_MAP(NTR_IDX, NTR_PHYS_BASE, NTR_RESET_OFFSET)
#define NTR_NCO_BASE       BCM_IO_MAP(NTR_IDX, NTR_PHYS_BASE, NTR_NCO_OFFSET)
#define NTR_CLK_PRG_SWCH_BASE BCM_IO_MAP(NTR_IDX, NTR_PHYS_BASE, NTR_CLK_PRG_SWCH_OFFSET)
#define NGPON2_BASE        BCM_IO_MAP(NGPON2_IDX, NGPON2_PHYS_BASE, 0)
#define EPON_BASE          BCM_IO_MAP(EPON_IDX, EPON_PHYS_BASE, 0)
#define WANBBH_BASE        BCM_IO_MAP(WANBBH_IDX, WANBBH_PHYS_BASE, 0)
#define XLIF_BASE          BCM_IO_MAP(XLIF_IDX, XLIF_PHYS_BASE, XLIF_OFFSET)
#define XRDP_BASE          BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, XRDP_OFFSET)
#define ETH_PHYS_TOP_BASE  BCM_IO_MAP(ETH_PHYS_IDX, ETH_PHYS_BASE, 0)
#define ETH_PHYS_REG_BASE  BCM_IO_MAP(ETH_PHYS_IDX, ETH_PHYS_BASE, ETH_PHYS_REG_OFFSET)
#define ETH_PHYS_MDIO_BASE BCM_IO_MAP(ETH_PHYS_IDX, ETH_PHYS_BASE, ETH_PHYS_MDIO_OFFSET)
#define ETH_SERDES_0_MISC_BASE  BCM_IO_MAP(ETH_PHYS_IDX, ETH_PHY_BASE, ETH_PHYS_SERDES_0_MISC_OFFSET)
#define ETH_SERDES_R2PMI_LP_0_INDIR_CNTRL  BCM_IO_MAP(ETH_PHYS_IDX, ETH_PHY_BASE, ETH_SERDES_R2PMI_LP_0_INDIR_CNTRL_OFFSET)
#define SWITCH_MDIO_BASE   ETH_PHYS_MDIO_BASE
#define SYSPORT_BASE       BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_OFFSET)
#define SYSPORT_SYSBUSCFG_BASE  BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_SYSBUSCFG_OFFSET)
#define SYSPORT_INTR_0_BASE     BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_INTR_0_OFFSET)
#define SYSPORT_INTR_1_BASE     BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_INTR_1_OFFSET)
#define SYSPORT_RXCHK_BASE      BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_RXCHK_OFFSET)
#define SYSPORT_TXCHK_BASE      BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_TXCHK_OFFSET)
#define SYSPORT_UMAC_BASE       BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_UMAC_OFFSET)
#define SYSPORT_RDMA_BASE       BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_RDMA_OFFSET)
#define SYSPORT_RBUF_BASE       BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_RBUF_OFFSET)
#define SYSPORT_TDMA_BASE       BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_TDMA_OFFSET)
#define SYSPORT_TBUF_BASE       BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_TBUF_OFFSET)
#define SYSPORT_MIB_BASE        BCM_IO_MAP(SYSPORT_IDX, SYSPORT_PHYS_BASE, SYSPORT_MIB_OFFSET)

#define XPORT_BASE         BCM_IO_MAP(XPORT_IDX, XPORT_PHYS_BASE, XPORT_OFFSET)
#define XPORT_REG_BASE     BCM_IO_MAP(XPORT_IDX, XPORT_PHYS_BASE, XPORT_REG_OFFSET)

#define SATA_BASE          BCM_IO_MAP(SATA_IDX, SATA_PHYS_BASE, SATA_OFFSET)

#define DSLPHY_BASE        BCM_IO_MAP(DSLPHY_IDX, DSLPHY_PHYS_BASE, DSLPHY_OFFSET)
#define DSLLMEM_BASE       BCM_IO_MAP(DSLLMEM_IDX, DSLLMEM_PHYS_BASE, DSLLMEM_OFFSET)
#define DSLXMEM_BASE       BCM_IO_MAP(DSLXMEM_IDX, DSLXMEM_PHYS_BASE, DSLXMEM_OFFSET)
#define BIUCFG_BASE        BCM_IO_MAP(BIUCFG_IDX, BIUCFG_PHYS_BASE, BIUCFG_OFFSET)
#define CRYPTO_BASE        BCM_IO_MAP(CRYPTO_IDX, CRYPTO_PHYS_BASE, CRYPTO_OFFSET)
#define CRYPTO_PDC_BASE    BCM_IO_MAP(CRYPTO_IDX, CRYPTO_PHYS_BASE, CRYPTO_PDC_OFFSET)
#define CRYPTO_SPU_BASE    BCM_IO_MAP(CRYPTO_IDX, CRYPTO_PHYS_BASE, CRYPTO_SPU_OFFSET)
#define CRYPTO_SPU_KC_BASE BCM_IO_MAP(CRYPTO_IDX, CRYPTO_PHYS_BASE, CRYPTO_SPU_KC_OFFSET)
#define SAR_BASE           BCM_IO_MAP(SAR_IDX, SAR_PHYS_BASE, SAR_OFFSET)
#define TXPAF_PROCESSOR_BASE    BCM_IO_MAP(DSLPHY_IDX, DSLPHY_PHYS_BASE, 0x7800)

#define APM_BASE           BCM_IO_MAP(PCM_IDX, PCM_PHYS_BASE, APM_CORE_OFFSET)
#define PCM_BASE           BCM_IO_MAP(PCM_IDX, PCM_PHYS_BASE, PCM_CORE_OFFSET)
#define PCM_DMA_BASE       BCM_IO_MAP(PCM_IDX, PCM_PHYS_BASE, PCM_DMA_OFFSET)
#define PCM_BUS_BASE       BCM_IO_MAP(PCMBUS_IDX, PCMBUS_PHYS_BASE, PCMBUS_OFFSET)


#define UBUS_SYS_MODULE_BASE        BCM_IO_MAP(UBUS_MAPPED_IDX,      		 UBUS_MAPPED_PHYS_BASE, 0)
#define UBUS_SYS_MODULE_REGISTRATION_BASE	BCM_IO_MAP(UBUS_MAPPED_IDX, 	 UBUS_MAPPED_PHYS_BASE, UBUS_SYS_MODULE_REGISTRATION_OFFSET)
#define UBUS_MAPPED_BASE            BCM_IO_MAP(UBUS_MAPPED_IDX,      		 UBUS_MAPPED_PHYS_BASE,      SYS_CLK_CTRL_OFFSET)
#define MST_PORT_NODE_PER_BASE      BCM_IO_MAP(MST_PORT_NODE_PER_IDX, MST_PORT_NODE_PER_PHYS_BASE, 0)
#define MST_PORT_NODE_USB_BASE      BCM_IO_MAP(MST_PORT_NODE_USB_IDX, MST_PORT_NODE_USB_PHYS_BASE, 0)
#define MST_PORT_NODE_B53_BASE      BCM_IO_MAP(MST_PORT_NODE_B53_IDX, MST_PORT_NODE_B53_PHYS_BASE, 0)
#define MST_PORT_NODE_PCIE0_BASE    BCM_IO_MAP(MST_PORT_NODE_PCIE0_IDX, MST_PORT_NODE_PCIE0_PHYS_BASE, 0)
#define MST_PORT_NODE_PCIE1_BASE    BCM_IO_MAP(MST_PORT_NODE_PCIE1_IDX, MST_PORT_NODE_PCIE1_PHYS_BASE, 0)
#define MST_PORT_NODE_PCIE2_BASE    BCM_IO_MAP(MST_PORT_NODE_PCIE2_IDX, MST_PORT_NODE_PCIE2_PHYS_BASE, 0)
#define MST_PORT_NODE_DSL_BASE      BCM_IO_MAP(MST_PORT_NODE_DSL_IDX, MST_PORT_NODE_DSL_PHYS_BASE, 0)
#define MST_PORT_NODE_DSLCPU_BASE   BCM_IO_MAP(MST_PORT_NODE_DSLCPU_IDX, MST_PORT_NODE_DSLCPU_PHYS_BASE, 0)
#define MST_PORT_NODE_RQ0_BASE      BCM_IO_MAP(MST_PORT_NODE_RQ0_IDX, MST_PORT_NODE_RQ0_PHYS_BASE, 0)
#define MST_PORT_NODE_DMA0_BASE     BCM_IO_MAP(MST_PORT_NODE_DMA0_IDX, MST_PORT_NODE_DMA0_PHYS_BASE, 0)
#define MST_PORT_NODE_DMA1_BASE     BCM_IO_MAP(MST_PORT_NODE_DMA1_IDX, MST_PORT_NODE_DMA1_PHYS_BASE, 0)
#define MST_PORT_NODE_QM_BASE       BCM_IO_MAP(MST_PORT_NODE_QM_IDX, MST_PORT_NODE_QM_PHYS_BASE, 0)
#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
#define MST_PORT_NODE_MPM_BASE      BCM_IO_MAP(MST_PORT_NODE_MPM_IDX, MST_PORT_NODE_MPM_PHYS_BASE, 0)  
#endif

#define UBUS_RANGE_CHK_SETUP_BASE   	BCM_IO_MAP(UBUS4_COHERENCY_PORT_IDX,UBUS4_COHERENCY_PORT_PHYS_BASE, UBUS4_RANGE_CHK_SETUP_OFFSET)
#define UBUS_RANGE_CHK_CFG_BASE     	BCM_IO_MAP(UBUS4_COHERENCY_PORT_IDX,UBUS4_COHERENCY_PORT_PHYS_BASE, UBUS4_RANGE_CHK_CONFIG_OFFSET)
#define UBUS_COHERENCY_PORT_CFG_BASE   	BCM_IO_MAP(UBUS4_COHERENCY_PORT_IDX,UBUS4_COHERENCY_PORT_PHYS_BASE, UBUS4_COHERENCY_PORT_CONFIG_OFFSET)
#define CAPTURE_PORT_NODE_0_BASE    BCM_IO_MAP(CAPTURE_PORT_NODE_0_IDX, CAPTURE_PORT_NODE_0_PHYS_BASE, 0)
#define CAPTURE_PORT_NODE_1_BASE    BCM_IO_MAP(CAPTURE_PORT_NODE_1_IDX, CAPTURE_PORT_NODE_1_PHYS_BASE, 0)

#define CCI500_BASE                 BCM_IO_MAP(CCI500_IDX, CCI500_PHYS_BASE, CCI500_OFFSET)

#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
#define MPM_BASE      	  			BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, 0)
#define MPM_DMA_BALLOC_BASE			BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, MPM_DMA_BALLOC_OFFSET)
#define MPM_DMA_BFREE_BASE			BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, MPM_DMA_BFREE_OFFSET)
#define MPM_DMA_COMMON_BASE			BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, MPM_DMA_COMMON_OFFSET)
#define MPM_DMA_SUBSYS_MASTER_ACCESS_BASE	BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, MPM_DMA_SMA_OFFSET)
#define MPM_CORE_BASE				BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, MPM_CORE_OFFSET)
#define MPM_COMMON_BASE				BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, MPM_COMMON_OFFSET)
#define MPM_INTRL2_BASE				BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, MPM_INTRL2_OFFSET)
#define MPM_BB_BASE    				BCM_IO_MAP(MPM_IDX,MPM_PHYS_BASE, MPM_BB_OFFSET)
#endif

// XRDP RCQ_GENERAL_CONFIG for UBUS4_DCM
#define XRDP_RCQ_COMMON_REGS_RCQ_GENERAL_CONFIG 0xD08500
#define XRDP_RCQ_GEN_CFG       \
        BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, \
            XRDP_RCQ_COMMON_REGS_RCQ_GENERAL_CONFIG)
#define XRDP_RCQ_GENERAL_DMA_ARB_CFG_CONGEST_THRESHOLD_MASK 0x000001f0
#define XRDP_RCQ_GENERAL_DMA_ARB_CFG_CONGEST_THRESHOLD_SHIFT 4

/* These block uses DT or not used by linux at all, no need to map for the legacy support */
#define GIC_BASE           BCM_IO_NOMAP(-1, GIC_PHYS_BASE, GIC_OFFSET)
#define GICC_BASE          BCM_IO_NOMAP(-1, GIC_PHYS_BASE, GICC_OFFSET)
#define GICD_BASE          BCM_IO_NOMAP(-1, GIC_PHYS_BASE, GICD_OFFSET)
#define ARM_UART1_BASE     BCM_IO_NOMAP(-1, ARM_UART_PHYS_BASE, ARM_UART1_OFFSET)
#define ARM_UART2_BASE     BCM_IO_NOMAP(-1, ARM_UART_PHYS_BASE, ARM_UART2_OFFSET)
#define SPIFLASH_BASE      BCM_IO_NOMAP(-1, SPIFLASH_PHYS_BASE, SPIFLASH_OFFSET)
#define EMMCFLASH_BASE     BCM_IO_NOMAP(-1, EMMCFLASH_PHYS_BASE, EMMCFLASH_OFFSET)


/* XRDP UNIMAC */
#define UNIMAC_BASE         BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, UNIMAC_OFFSET)
#define UNIMAC_CFG_BASE     BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, UNIMAC_CFG_OFFSET)
#define UNIMAC_MIB_BASE     BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, UNIMAC_MIB_OFFSET)
#define UNIMAC_MPD_BASE     BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, UNIMAC_MPD_OFFSET)
#define UNIMAC_XIB_BASE     BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, UNIMAC_XIB_OFFSET)
#define UNIMAC_TCTL_BASE    BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, UNIMAC_TCTL_OFFSET)
#define UNIMAC_TOP_BASE     BCM_IO_MAP(XRDP_IDX, XRDP_PHYS_BASE, UNIMAC_TOP_OFFSET)

#ifndef __ASSEMBLER__

#ifdef __BOARD_DRV_AARCH64__
// add here any legacy driver's (driver that have no device tree node) IO memory to be mapped
BCM_IO_BLOCKS bcm_io_blocks[] = 
{
    {PERF_IDX, PERF_SIZE, PERF_PHYS_BASE},
    {PERF1_IDX, PERF1_SIZE, PERF1_PHYS_BASE},
    {AHB_IDX, AHB_SIZE, AHB_PHYS_BASE},
    {ARM_UART_IDX, ARM_UART_SIZE, ARM_UART_PHYS_BASE},
    {AHB_A0_IDX, AHB_SIZE, AHB_PHYS_BASE_A0},
    {NANDFLASH_IDX, NANDFLASH_SIZE, NANDFLASH_PHYS_BASE},
    {PCM_IDX, PCM_SIZE, PCM_PHYS_BASE},
    {PCMBUS_IDX, PCMBUS_SIZE, PCMBUS_PHYS_BASE},
    {MEMC_IDX, MEMC_SIZE, MEMC_PHYS_BASE},
#if defined(_ATF_)
    {PMC_IDX, PMC_SIZE, PMC_PHYS_BASE},
#endif
    {WAN_IDX, WAN_SIZE, WAN_PHYS_BASE},
    {GPON_IDX, GPON_SIZE, GPON_PHYS_BASE},
    {NTR_IDX, NTR_SIZE, NTR_PHYS_BASE},
    {NGPON2_IDX, NGPON2_SIZE, NGPON2_PHYS_BASE},
    {EPON_IDX, EPON_SIZE, EPON_PHYS_BASE},
    {WANBBH_IDX, WANBBH_SIZE, WANBBH_PHYS_BASE},
    {XRDP_IDX, XRDP_SIZE, XRDP_PHYS_BASE},
    {ETH_PHYS_IDX, ETH_PHYS_SIZE, ETH_PHYS_BASE},
    {SATA_IDX, SATA_SIZE, SATA_PHYS_BASE},
    {DSLPHY_IDX, DSLPHY_SIZE, DSLPHY_PHYS_BASE},
    {DSLLMEM_IDX, DSLLMEM_SIZE, DSLLMEM_PHYS_BASE},
    {DSLXMEM_IDX, DSLXMEM_SIZE, DSLXMEM_PHYS_BASE},
    {SYSPORT_IDX, SYSPORT_SIZE, SYSPORT_PHYS_BASE},
    {BIUCFG_IDX, BIUCFG_SIZE, BIUCFG_PHYS_BASE},
    {CRYPTO_IDX, CRYPTO_SIZE, CRYPTO_PHYS_BASE},
    {SAR_IDX, SAR_SIZE, SAR_PHYS_BASE},
    {XPORT_IDX, XPORT_SIZE, XPORT_PHYS_BASE},
    {XLIF_IDX, XLIF_SIZE, XLIF_PHYS_BASE},
    {CCI500_IDX, CCI500_SIZE, CCI500_PHYS_BASE},
    {UBUS_MAPPED_IDX, UBUS_MAPPED_SIZE, UBUS_MAPPED_PHYS_BASE},
#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
    {MPM_IDX, MPM_SIZE, MPM_PHYS_BASE},
#endif
    {MST_PORT_NODE_PER_IDX, MST_PORT_NODE_PER_SIZE, MST_PORT_NODE_PER_PHYS_BASE},
    {MST_PORT_NODE_USB_IDX, MST_PORT_NODE_USB_SIZE, MST_PORT_NODE_USB_PHYS_BASE},
    {MST_PORT_NODE_B53_IDX, MST_PORT_NODE_B53_SIZE, MST_PORT_NODE_B53_PHYS_BASE},
    {MST_PORT_NODE_PCIE0_IDX, MST_PORT_NODE_PCIE0_SIZE, MST_PORT_NODE_PCIE0_PHYS_BASE},
    {MST_PORT_NODE_PCIE1_IDX, MST_PORT_NODE_PCIE1_SIZE, MST_PORT_NODE_PCIE1_PHYS_BASE},
    {MST_PORT_NODE_PCIE2_IDX, MST_PORT_NODE_PCIE2_SIZE, MST_PORT_NODE_PCIE2_PHYS_BASE},
    {MST_PORT_NODE_DSL_IDX, MST_PORT_NODE_DSL_SIZE, MST_PORT_NODE_DSL_PHYS_BASE},
    {MST_PORT_NODE_DSLCPU_IDX, MST_PORT_NODE_DSLCPU_SIZE, MST_PORT_NODE_DSLCPU_PHYS_BASE},
    {MST_PORT_NODE_RQ0_IDX, MST_PORT_NODE_RQ0_SIZE, MST_PORT_NODE_RQ0_PHYS_BASE},
    {MST_PORT_NODE_DMA0_IDX, MST_PORT_NODE_DMA0_SIZE, MST_PORT_NODE_DMA0_PHYS_BASE},
    {MST_PORT_NODE_DMA1_IDX, MST_PORT_NODE_DMA1_SIZE, MST_PORT_NODE_DMA1_PHYS_BASE},
    {MST_PORT_NODE_QM_IDX, MST_PORT_NODE_QM_SIZE, MST_PORT_NODE_QM_PHYS_BASE},
#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
    {MST_PORT_NODE_MPM_IDX, MST_PORT_NODE_MPM_SIZE, MST_PORT_NODE_MPM_PHYS_BASE},    
#endif
    {UBUS4_COHERENCY_PORT_IDX, UBUS4_COHERENCY_PORT_SIZE, UBUS4_COHERENCY_PORT_PHYS_BASE},
    {CAPTURE_PORT_NODE_0_IDX, CAPTURE_PORT_NODE_0_SIZE, CAPTURE_PORT_NODE_0_PHYS_BASE},
    {CAPTURE_PORT_NODE_1_IDX, CAPTURE_PORT_NODE_1_SIZE, CAPTURE_PORT_NODE_1_PHYS_BASE},
};
unsigned long bcm_io_block_address[LAST_IDX];
#else
extern BCM_IO_BLOCKS bcm_io_blocks[];
extern unsigned long bcm_io_block_address[];
#endif

typedef struct MC2_GlobalControl {	/* 0x00 - 0x3f */
	uint32 acc;			/* 0x00 */
	uint32 vers;			/* 0x04 */
	uint32 gcfg;                    /* 0x08 */
#define MEMC_GLB_GCFG_SREF_SLOW_CLK_SHIFT	26
#define MEMC_GLB_GCFG_SREF_SLOW_CLK_MASK	(1<<MEMC_GLB_GCFG_SREF_SLOW_CLK_SHIFT)

	uint32 auto_self_refresh;	/* 0x0c */
#define MEMC_DDR_AUTO_SELFREFRESH_EN		(1 << 31)
#define MEMC_DDR_AUTO_SR_IDLE_CNT_MASK		(0x3FFFFFFF)

	uint32 pwr_mgr;			/* 0x10 */
	uint8 unused[0x40-0x14];	/* 0x14 - 0x3f */
} MC2_GlobalControl;

typedef struct MC2_AXIInterface {	/* 0x40 - 0x7f */
	uint32 acc;			/* 0x40 */
	uint32 ver;			/* 0x44 */
	uint32 cfg;                     /* 0x48 */
	uint32 rep_arb_mode;		/* 0x4c */
	uint32 queue_cfg;		/* 0x50 */
	uint32 queue_size0;		/* 0x54 */
	uint32 queue_map0;		/* 0x58 */
	uint32 unused0;			/* 0x5c */
	uint32 scratch;			/* 0x60 */
	uint32 axi_debug_0_0;		/* 0x64 */
	uint32 axi_debug_1_0;		/* 0x68 */
	uint32 axi_debug_misc;		/* 0x6c */
	uint8 unused[0x80-0x70];	/* 0x70 - 0x7f */
} MC2_AXIInterface;

typedef struct MC2_UBUSInterface {	/* 0x80 - 0xbf */
	uint32 acc;			/* 0x80 */
	uint32 cfg;			/* 0x84 */
	uint32 esrcid_cfg;		/* 0x88 */
	uint32 queue_cfg_queue_cfg;	/* 0x8c */
	uint32 queue_cfg_queue_map0;	/* 0x90 */
	uint32 queue_cfg_queue_map1;	/* 0x94 */
	uint32 queue_cfg_queue_map2;	/* 0x98 */
	uint32 queue_cfg_queue_map3;	/* 0x9c */
	uint32 queue_cfg_queue_size0;	/* 0xa0 */
	uint32 queue_cfg_queue_size1;	/* 0xa4 */
	uint32 queue_cfg_queue_size2;	/* 0xa8 */
	uint32 queue_cfg_queue_size3;	/* 0xac */
	uint32 diag_ctrl;		/* 0xb0 */
	uint32 unused;			/* 0xb4 */
	uint32 scratch;			/* 0xb8 */
	uint32 debug_ro;		/* 0xbc */
} MC2_UBUSInterface;

typedef struct DDR34_PHYCommon {	/* 0x20000 - 0x201ff */
	uint8 to_be_refined[0x200];
} DDR34_PHYCommon; 

typedef struct DDR34_PHYControl {	/* 0x20200 - 0x205ff */
	uint32 IDLE_PAD_CONTROL;	/* 0x20200 */
	uint32 IDLE_PAD_EN0;		/* 0x20204 */
	uint32 IDLE_PAD_EN1;		/* 0x20208 */
	uint8 to_be_refined[0x600-0x20c];
} DDR34_PHYControl; 

typedef struct DDR34_PHYByteLane {	/* 0x20600 - 0x207ff */
	uint8 to_be_refined[0x71c-0x600];
	uint32 IDLE_PAD_CTRL;		/* 0x2071c */
	uint8 to_be_refined1[0x73c-0x720];
	uint32 CLOCK_IDLE;		/* 0x2073c */
	uint8 to_be_refined2[0x800-0x740];
} DDR34_PHYByteLane; 

typedef struct MEMCControl {
	MC2_GlobalControl glb;		/* 0x00 - 0x3f */
	MC2_AXIInterface axi;		/* 0x40 - 0x7f */
	MC2_UBUSInterface ubus;		/* 0x80 - 0xbf */
	uint8 to_be_refined[0xc00-0xc0];	/* 0xc0 - 0xbff */
	uint8 sta[0xe00-0xc00];			/* 0xc00 - 0xdff */
	uint32 SEC_INTR2_CPU_STATUS;		/* 0xe00 */
	uint32 SEC_INTR2_CPU_SET;		/* 0xe04 */
	uint32 SEC_INTR2_CPU_CLEAR;		/* 0xe08 */
	uint32 SEC_INTR2_CPU_MASK_STATUS;	/* 0xe0c */
	uint32 SEC_INTR2_CPU_MASK_SET;		/* 0xe10 */
	uint32 SEC_INTR2_CPU_MASK_CLEAR;	/* 0xe14 */
	uint8 to_be_refined1[0x20000-0xe18];	/* 0xe00 - 0x1ffff */
	DDR34_PHYCommon PhyCommon;	/* 0x20000 - 0x201ff */
	DDR34_PHYControl PhyControl;	/* 0x20200 - 0x205ff */
	DDR34_PHYByteLane PhyByteLane0Control;	/* 0x20600 - 0x207ff */
	DDR34_PHYByteLane PhyByteLane1Control;	/* 0x20800 - 0x209ff */
} MEMCControl;

#define MEMC ((volatile MEMCControl * const) MEMC_BASE)

/*
 * Crypto SKP Control Set Definitions.
 */
typedef struct SkpCntrl {
   uint32 skpCmd; 		/* 0x00 */
#define SKP_CNTRL_SKP_CMD_OPCODE_PROCESS      (1 << 28 ) // bits [30:28] = 3'b001 = Process Command
#define SKP_CNTRL_SKP_CMD_OPCODE_SW_XFER      (2 << 28 ) // bits [30:28] = 3'b010 = sw transfer Command
#define SKP_CNTRL_SKP_CMD_OPCODE_HW_XFER      (3 << 28 ) // bits [30:28] = 3'b011 = sw transfer Command
#define SKP_CNTRL_SKP_CMD_OUTPUT_DATA_KT_0    (0 << 20 ) // bits [22:20] = 3'b000 = load the generated output data into key table slot 0
#define SKP_CNTRL_SKP_CMD_OUTPUT_DATA_KT_1    (1 << 20 ) // bits [22:20] = 3'b001 = load the generated output data into key table slot 1
#define SKP_CNTRL_SKP_CMD_OUTPUT_DATA_KT_2    (2 << 20 ) // bits [22:20] = 3'b010 = load the generated output data into key table slot 2
#define SKP_CNTRL_SKP_CMD_OUTPUT_DATA_KT_3    (3 << 20 ) // bits [22:20] = 3'b011 = load the generated output data into key table slot 3
#define SKP_CNTRL_SKP_CMD_OUTPUT_DATA_KT_4    (4 << 20 ) // bits [22:20] = 3'b100 = load the generated output data into key table slot 4
#define SKP_CNTRL_SKP_CMD_INPUT_DATA_KT_0      (0 << 16 ) // bits [18:16] = 3'b000 = read the input data from key table slot 0
#define SKP_CNTRL_SKP_CMD_INPUT_DATA_KT_1      (1 << 16 ) // bits [18:16] = 3'b001 = read the input data from key table slot 1
#define SKP_CNTRL_SKP_CMD_INPUT_DATA_KT_2      (2 << 16 ) // bits [18:16] = 3'b010 = read the input data from key table slot 2
#define SKP_CNTRL_SKP_CMD_INPUT_KEY_1          (1 <<  8 ) // bits [13:8]  = 0x01 = Key index to universal key 1, or key table slot 1
#define SKP_CNTRL_SKP_CMD_INPUT_KEY_2          (2 <<  8 ) // bits [13:8]  = 0x02 = Key index to universal key 2, or key table slot 2
#define SKP_CNTRL_SKP_CMD_INPUT_KEY_3          (3 <<  8 ) // bits [13:8]  = 0x03 = Key index to universal key 3, or key table slot 3
#define SKP_CNTRL_SKP_CMD_INPUT_KEY_4          (4 <<  8 ) // bits [13:8]  = 0x04 = Key index to universal key 4, or key table slot 4
#define SKP_CNTRL_SKP_CMD_KEY_SRC_KEY_A        (0 <<  4 ) // bits  [5:4]  = 2'b00 = Key A
#define SKP_CNTRL_SKP_CMD_KEY_SRC_KEY_B        (1 <<  4 ) // bits  [5:4]  = 2'b01 = Key B
#define SKP_CNTRL_SKP_CMD_KEY_SRC_UNI_KEY      (2 <<  4 ) // bits  [5:4]  = 2'b10 = Universal Key
#define SKP_CNTRL_SKP_CMD_STAGE_0              (0 <<  0 ) // bits  [2:0]  = 3'b000 = Generate Key 0
#define SKP_CNTRL_SKP_CMD_STAGE_1              (1 <<  0 ) // bits  [2:0]  = 3'b001 = Generate Key 1
#define SKP_CNTRL_SKP_CMD_STAGE_2              (2 <<  0 ) // bits  [2:0]  = 3'b010 = Generate Key 2
#define SKP_CNTRL_SKP_CMD_STAGE_3              (3 <<  0 ) // bits  [2:0]  = 3'b011 = Generate Key 3
#define SKP_CNTRL_SKP_CMD_STAGE_4              (4 <<  0 ) // bits  [2:0]  = 3'b100 = Generate Key 4
   uint32 skpStatus;		/* 0x04 */
   uint32 skpClrError;		/* 0x08 */
   uint32 skpMisc;		/* 0x0c */
   uint32 skpNotUsedOne[8];	/* 0x10 */
   uint32 skpDataIn[4];		/* 0x30 */
   uint32 skpKeyOut[4];		/* 0x40 */
   uint32 skpSwTagOut;		/* 0x50 */
   uint32 skpNotUsedTwo[3];	/* 0x54 */
   uint32 skpKtTag[8];		/* 0x60 */
} SkpCntrl;

#define SKPCTRL ((volatile SkpCntrl * const) SKP_CNTRL_BASE)

/*
 * Crypto SKP OTR Set Definitions.
 */
typedef struct SkpOtr {
   uint32 skpOtrvKeyA[4]; 	/* 0x00 */
   uint32 skpOtrvKeyB[4];	/* 0x10 */
   uint32 skpOtrvKeyCidA[2];	/* 0x20 */
   uint32 skpOtrvKeyCidB[2];	/* 0x28 */
   uint32 skpOtrvKeyCidU[2];	/* 0x30 */
   uint32 skpOtrvKeyCidC[2];	/* 0x38 */
   uint32 skpOtrvValid;		/* 0x40 */
#define SKP_OTR_SKP_OTRV_VALID_CID_U_VALID	(1 << 4)
   uint32 skpOtrNotUsed[3];	/* 0x44 */
   uint32 skpOtrc;		/* 0x50 */
#define SKP_OTR_SKP_OTRC_U_DIS             	(3 << 20)
} SkpOtr;

#define SKPOTR ((volatile SkpOtr * const) SKP_OTR_BASE)


typedef struct CryptoSPUCtrl {
    uint32 SPU_SPU_CONTROL; /* 0x000 */
    uint32 unused0[3];
    uint32 SPU_SPU_STATUS; /* 0x010 */
    uint32 unused1[3];
    uint32 SPU_SPU_KC_ECC_CSR; /* 0x020 */
    uint32 unused2[3];
    uint32 SPU_SPU_OFIFO_ECC_CSR; /* 0x030 */
    uint32 unused3[3];
    uint32 SPU_SPU_OFIFO_CTRL; /* 0x040 */
    uint32 unused4[3];
    uint32 SPU_SPU_IVGEN_CTRL; /* 0x050 */
    uint32 unused5[3];
    uint32 SPU_SPU_IV_SEED; /* 0x060 */
} CryptoSPUCtrl_t;
typedef struct CryptoSPUKeyCache {
    uint32 SPU_MEM_SPU_KEY_CACHE[256]; /* 0x800 */
} CryptoSPUKeyCache_t;

typedef struct CryptoPDCCtrl {
    uint32 PDC_DEVCONTROL; /* 0x000 */
    uint32 PDC_DEVSTATUS; /* 0x004 */
    uint32 unused0[1];
    uint32 PDC_BISTSTATUS; /* 0x00c */
    uint32 unused1[4];
    uint32 PDC_INTSTATUS; /* 0x020 */
    uint32 PDC_INTMASK; /* 0x024 */
    uint32 PDC_GPTIMER; /* 0x028 */
    uint32 unused2[1];
    uint32 PDC_INTRCVLAZY_0; /* 0x030 */
    uint32 PDC_INTRCVLAZY_1; /* 0x034 */
    uint32 PDC_INTRCVLAZY_2; /* 0x038 */
    uint32 PDC_INTRCVLAZY_3; /* 0x03c */
    uint32 unused3[49];
    uint32 PDC_FLOWCNTL_TH; /* 0x104 */
    uint32 PDC_TXARB_WRR_TH; /* 0x108 */
    uint32 PDC_GMACIDLE_CNT_TH; /* 0x10c */
    uint32 unused4[4];
    uint32 PDC_FIFOACCESSADDR; /* 0x120 */
    uint32 PDC_FIFOACCESSBYTE; /* 0x124 */
    uint32 PDC_FIFOACCESSDATA; /* 0x128 */
    uint32 unused5[21];
    uint32 PDC_PHYACCESS; /* 0x180 */
    uint32 unused6[1];
    uint32 PDC_PHYCONTROL; /* 0x188 */
    uint32 PDC_TXQCONTROL; /* 0x18c */
    uint32 PDC_RXQCONTROL; /* 0x190 */
    uint32 PDC_GPIOSELECT; /* 0x194 */
    uint32 PDC_GPIOOUTPUTEN; /* 0x198 */
    uint32 unused7[1];
    uint32 PDC_TXQRXQMEMORYCONTROL; /* 0x1a0 */
    uint32 PDC_MEMORYECCSTATUS; /* 0x1a4 */
    uint32 PDC_SERDESCONTROL; /* 0x1a8 */
    uint32 PDC_SERDESSTATUS0; /* 0x1ac */
    uint32 PDC_SERDESSTATUS1; /* 0x1b0 */
    uint32 unused8[11];
    uint32 PDC_CLOCKCONTROLSTATUS; /* 0x1e0 */
    uint32 unused9[1];
    uint32 PDC_POWERCONTROL; /* 0x1e8 */
    uint32 unused10[5];
    struct CryptoPDCXmtCtrl {
           uint32 PDC_XMTCONTROL; /* 0x200 + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_XMTPTR; /* 0x204  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_XMTADDR_LOW; /* 0x208  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_XMTADDR_HIGH; /* 0x20c  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_XMTSTATUS0; /* 0x210  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_XMTSTATUS1; /* 0x214  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 unused11[2];
           uint32 PDC_RCVCONTROL; /* 0x220  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_RCVPTR; /* 0x224  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_RCVADDR_LOW; /* 0x228  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_RCVADDR_HIGH; /* 0x22c  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_RCVSTATUS0; /* 0x230  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 PDC_RCVSTATUS1; /* 0x234  + chanNo*sizeof(struct_ctrl_s)*/
           uint32 unused12[2];
    } xmt_ctrl[4];
    uint32 PDC_TX_GD_OCTETS_LO; /* 0x300 */
    uint32 PDC_TX_GD_OCTETS_HI; /* 0x304 */
    uint32 PDC_TX_GD_PKTS; /* 0x308 */
    uint32 PDC_TX_ALL_OCTETS_LO; /* 0x30c */
    uint32 PDC_TX_ALL_OCTETS_HI; /* 0x310 */
    uint32 PDC_TX_ALL_PKTS; /* 0x314 */
    uint32 PDC_TX_BRDCAST; /* 0x318 */
    uint32 PDC_TX_MULT; /* 0x31c */
    uint32 PDC_TX_64; /* 0x320 */
    uint32 PDC_TX_65_127; /* 0x324 */
    uint32 PDC_TX_128_255; /* 0x328 */
    uint32 PDC_TX_256_511; /* 0x32c */
    uint32 PDC_TX_512_1023; /* 0x330 */
    uint32 PDC_TX_1024_1522; /* 0x334 */
    uint32 PDC_TX_1523_2047; /* 0x338 */
    uint32 PDC_TX_2048_4095; /* 0x33c */
    uint32 PDC_TX_4096_8191; /* 0x340 */
    uint32 PDC_TX_8192_MAX; /* 0x344 */
    uint32 PDC_TX_JAB; /* 0x348 */
    uint32 PDC_TX_OVER; /* 0x34c */
    uint32 PDC_TX_FRAG; /* 0x350 */
    uint32 PDC_TX_UNDERRUN; /* 0x354 */
    uint32 PDC_TX_COL; /* 0x358 */
    uint32 PDC_TX_1_COL; /* 0x35c */
    uint32 PDC_TX_M_COL; /* 0x360 */
    uint32 PDC_TX_EX_COL; /* 0x364 */
    uint32 PDC_TX_LATE; /* 0x368 */
    uint32 PDC_TX_DEF; /* 0x36c */
    uint32 PDC_TX_CRS; /* 0x370 */
    uint32 PDC_TX_PAUS; /* 0x374 */
    uint32 PDC_TXUNICASTPKT; /* 0x378 */
    uint32 PDC_TXQOSQ0PKT; /* 0x37c */
    uint32 PDC_TXQOSQ0OCTET_LO; /* 0x380 */
    uint32 PDC_TXQOSQ0OCTET_HI; /* 0x384 */
    uint32 PDC_TXQOSQ1PKT; /* 0x388 */
    uint32 PDC_TXQOSQ1OCTET_LO; /* 0x38c */
    uint32 PDC_TXQOSQ1OCTET_HI; /* 0x390 */
    uint32 PDC_TXQOSQ2PKT; /* 0x394 */
    uint32 PDC_TXQOSQ2OCTET_LO; /* 0x398 */
    uint32 PDC_TXQOSQ2OCTET_HI; /* 0x39c */
    uint32 PDC_TXQOSQ3PKT; /* 0x3a0 */
    uint32 PDC_TXQOSQ3OCTET_LO; /* 0x3a4 */
    uint32 PDC_TXQOSQ3OCTET_HI; /* 0x3a8 */
    uint32 unused19[1];
    uint32 PDC_RX_GD_OCTETS_LO; /* 0x3b0 */
    uint32 PDC_RX_GD_OCTETS_HI; /* 0x3b4 */
    uint32 PDC_RX_GD_PKTS; /* 0x3b8 */
    uint32 PDC_RX_ALL_OCTETS_LO; /* 0x3bc */
    uint32 PDC_RX_ALL_OCTETS_HI; /* 0x3c0 */
    uint32 PDC_RX_ALL_PKTS; /* 0x3c4 */
    uint32 PDC_RX_BRDCAST; /* 0x3c8 */
    uint32 PDC_RX_MULT; /* 0x3cc */
    uint32 PDC_RX_64; /* 0x3d0 */
    uint32 PDC_RX_65_127; /* 0x3d4 */
    uint32 PDC_RX_128_255; /* 0x3d8 */
    uint32 PDC_RX_256_511; /* 0x3dc */
    uint32 PDC_RX_512_1023; /* 0x3e0 */
    uint32 PDC_RX_1024_1522; /* 0x3e4 */
    uint32 PDC_RX_1523_2047; /* 0x3e8 */
    uint32 PDC_RX_2048_4095; /* 0x3ec */
    uint32 PDC_RX_4096_8191; /* 0x3f0 */
    uint32 PDC_RX_8192_MAX; /* 0x3f4 */
    uint32 PDC_RX_JAB; /* 0x3f8 */
    uint32 PDC_RX_OVR; /* 0x3fc */
    uint32 PDC_RX_FRAG; /* 0x400 */
    uint32 PDC_RX_DROP; /* 0x404 */
    uint32 PDC_RX_CRC_ALIGN; /* 0x408 */
    uint32 PDC_RX_UND; /* 0x40c */
    uint32 PDC_RX_CRC; /* 0x410 */
    uint32 PDC_RX_ALIGN; /* 0x414 */
    uint32 PDC_RX_SYM; /* 0x418 */
    uint32 PDC_RX_PAUS; /* 0x41c */
    uint32 PDC_RX_CNTRL; /* 0x420 */
    uint32 PDC_RXSACHANGES; /* 0x424 */
    uint32 PDC_RXUNICASTPKTS; /* 0x428 */
    uint32 unused20[1];
    uint32 PDC_MEM_PRE_BUF_TM; /* 0x430 */
    uint32 PDC_MEM_RXQ_BUF_TM; /* 0x434 */
    uint32 PDC_MEM_TXQ_BUF_TM; /* 0x438 */
    uint32 PDC_MEM_UNIMAC_TXRX_TM; /* 0x43c */
    uint32 PDC_MEM_UNIMAC_RTSM_TM; /* 0x440 */
    uint32 PDC_MEM_GMAC_JTAG_TM; /* 0x444 */
} CryptoPDCCtrl_t;

#define CRYPTO_PDC ((volatile CryptoPDCCtrl_t * const) CRYPTO_PDC_BASE)
#define CRYPTO_SPU ((volatile CryptoSPUCtrl_t * const) CRYPTO_SPU_BASE)
#define CRYPTO_SPU_KC ((volatile CryptoSPUKeyCache_t * const) CRYPTO_SPU_KC_BASE)

/*
 * Peripheral Controller
 */

typedef struct DMAIrqCfg {
   uint32 DMAIrqStatus;       /* 0x00 */
   uint32 DMAIrqSet;          /* 0x04 */
   uint32 DMAIrqClear;        /* 0x08 */
   uint32 DMAIrqMaskStatus;   /* 0x0c */
   uint32 DMAIrqMaskSet;      /* 0x10 */
   uint32 DMAIrqMaskClear;    /* 0x14 */
}DMAIrqCfg;

typedef struct PerfControl {
   uint32 RevID;        /* 0x00 */
#define CHIP_ID_SHIFT   12
#define CHIP_ID_MASK    (0xfffff << CHIP_ID_SHIFT)
#define REV_ID_MASK     0xfff
   uint32 reserved0[7]; /* 0x04 - 0x1f */
   uint32 ExtIrqCtrl;   /* 0x20 */
#define EI_LVLSTICKY_SHFT   0
#define EI_SENSE_SHFT   8
#define EI_INSENS_SHFT  16
#define EI_LEVEL_SHFT   24
   uint32 ExtIrqStatus;  /* 0x24 */
#define EI_STATUS_SHFT  0
#define EI_STATUS_MASK  0xff
   uint32 ExtIrqSet;     /* 0x28 */
   uint32 ExtIrqClear;   /* 0x2c */
   uint32 ExtIrqMaskStatus;/* 0x30 */
   uint32 ExtIrqMaskSet; /* 0x34 */
   uint32 ExtIrqMaskClear; /* 0x38 */
   uint32 reserved1[2]; /* 0x3c - 0x43 */
   uint32 ExtIrqMuxSel0; /* 0x44 */
#define EXT_IRQ_SLOT_SIZE             16
#define EXT_IRQ_MUX_SEL0_SHIFT        4
#define EXT_IRQ_MUX_SEL0_MASK         0xf
   uint32 ExtIrqMuxSel1;   /* 0x48 */
#define EXT_IRQ_MUX_SEL1_SHIFT        4
#define EXT_IRQ_MUX_SEL1_MASK         0xf
   uint32 IrqPeriphStatus; /* 0x4c */
   uint32 IrqPeriphMask;   /* 0x50 */
   uint32 reserved2[8];    /* 0x54 - 0x73 */  
   DMAIrqCfg dmaIrqCfg[3]; /* 0x74 - 0xbb */
} PerfControl;

#define PERF ((volatile PerfControl * const) PERF_BASE)

/* 
 * Legacy Interrupt sense/mask/status registers.
 * These are still hooked up to valid irq signals
 * and can be used to monitor realtime interrupt 
 * status.
 */
typedef struct IntSet {
   uint32 IrqSense[4];   /* 0x00 - 0x0f */
   uint32 IrqMask0[4];   /* 0x10 - 0x1f */
   uint32 IrqMask1[4];   /* 0x20 - 0x2f */
   uint32 IrqMask2[4];   /* 0x30 - 0x3f */
   uint32 IrqMask3[4];   /* 0x40 - 0x4f */
   uint32 IrqStatus0[4]; /* 0x50 - 0x5f */
   uint32 IrqStatus1[4]; /* 0x60 - 0x6f */
   uint32 IrqStatus2[4]; /* 0x70 - 0x7f */
   uint32 IrqStatus3[4]; /* 0x80 - 0x8f */
} IntSet;
#define INTSET ((volatile IntSet * const) INTSET0_BASE)


/*
 * Timer
 */
#define TIMER_64BIT
typedef struct Timer {
    uint64        TimerCtl0; /* 0x00 */
    uint64        TimerCtl1; /* 0x08 */
    uint64        TimerCtl2; /* 0x10 */
    uint64        TimerCtl3; /* 0x18 */
#define TIMERENABLE     (1ULL << 63)
#define RSTCNTCLR       (1ULL << 62)
    uint64        TimerCnt0; /* 0x20 */
    uint64        TimerCnt1; /* 0x28 */
    uint64        TimerCnt2; /* 0x30 */
    uint64        TimerCnt3; /* 0x38 */
#define TIMER_COUNT_MASK   0x3FFFFFFFFFFFFFFFULL
    uint32        TimerMask; /* 0x40 */
#define TIMER0EN        0x01
#define TIMER1EN        0x02
#define TIMER2EN        0x04
#define TIMER3EN        0x08
    uint32        TimerInts; /* 0x44 */
#define TIMER0          0x01
#define TIMER1          0x02
#define TIMER2          0x04
#define TIMER3          0x08
    uint32        ResetReason;/* 0x4c */
    uint32        spare[3];   /* 0x50 - 0x5b */
    uint32        reserved1[9]; /* 0x5c - 0x7f */
} Timer;

#define TIMER ((volatile Timer * const) TIMR_BASE)

/*
 * Top Control
 */
typedef struct TopControl {
   uint32 SGMIIFiberDetect;   /* 0x00 */
   uint32 DgSensePadCtl;     /* 0x04 */
#define DG_EN_SHIFT                 3
   uint32 TPDirOverRide[2];   /* 0x08 */
   uint32 RescaleReadData[2]; /* 0x10 */
   uint32 RescalIPCtrl;       /* 0x18 */
#define RESCAL_RSTB_SHIFT           1
#define RESCAL_RSTB                 (0x1<<RESCAL_RSTB_SHIFT)
#define RESCAL_PWRDN_SHIFT          0
#define RESCAL_PWRDN                (0x1<<RESCAL_PWRDN_SHIFT)

   uint32 ResetStatus;        /* 0x1c */
#define PCIE_RESET_STATUS       0x10000000
#define SW_RESET_STATUS         0x20000000
#define HW_RESET_STATUS         0x40000000
#define POR_RESET_STATUS        0x80000000
#define RESET_STATUS_MASK       0xF0000000

   uint32 TODsync;            /* 0x20 */
   uint32 MDIOmasterSelect;   /* 0x24 */
   uint32 AdslClockSample;    /* 0x28 */
   uint32 OrionIntPending;    /* 0x2c */
} TopControl;

#define TOPCTRL ((volatile TopControl * const) TOP_CONTROL_BASE)

/*
 * Gpio Controller
 */
typedef struct GpioControl {
   uint32 GPIODir[8];   /* 0x00-0x1f */
   uint32 GPIOio[8];    /* 0x20-0x3f */
   uint32 PadControl;    /* 0x40 */
   uint32 SpiSlaveCfg;   /* 0x44 */
   uint32 TestControl;   /* 0x48 */
   uint32 TestPortBlockEnMSB; /* 0x4c */
   uint32 TestPortBlockEnLSB; /* 0x50 */
   uint32 TestPortBlockDataMSB;  /* 0x54 */
   uint32 TestPortBlockDataLSB;  /* 0x58 */
#define PINMUX_ADDR_SHIFT       0
#define PINMUX_ADDR_MASK        (0xfff<<PINMUX_ADDR_SHIFT)
#define PINMUX_DATA_SHIFT       12
#define PINMUX_DATA_MASK        (0x3f<<PINMUX_DATA_SHIFT)
#define PINMUX_0                0
#define PINMUX_1                1
#define PINMUX_2                2
#define PINMUX_3                3
#define PINMUX_4                4
#define PINMUX_5                5
#define PINMUX_6                6
#define PINMUX_MSPI             PINMUX_0
#define PINMUX_MSPI_SS          PINMUX_1
#define PINMUX_PCM              PINMUX_1
  /*  GPIO pad control are done indirectly using TP cmd */
#define PAD_CTRL_SHIFT          12
#define PAD_CTRL_MASK           (0x3f<<PAD_CTRL_SHIFT)
#define PAD_DRIVE_STRENGTH_SHIFT   12
#define PAD_DRIVE_STRENGTH_MASK    (0x7<<TP_XMII_PAD_DRV_STRENGTH_SHIFT)
#define PAD_SLEW_RATE_CTRL      0x8000    /* RGMII pad amp en */
#define PAD_INPUT_DISABLE       0x10000
#define PAD_HYST_ENABLE         0x20000   /* RGMII pad sel gmii */
   uint32 TestPortCmd;        /* 0x5c */
#define LOAD_MUX_REG_CMD        0x21
#define LOAD_PAD_CTRL_CMD       0x22
   uint32 DiagReadBack;       /* 0x60 */
   uint32 DiagReadBackHi;     /* 0x64 */
   uint32 GeneralPurpose;     /* 0x68 */
   uint32 spare[3];           /* 0x6c-0x77 */
} GpioControl;

#define GPIO ((volatile GpioControl * const) GPIO_BASE)

/* Number to mask conversion macro used for GPIODir and GPIOio */
#define GPIO_NUM_MAX               256
#define GPIO_NUM_TO_ARRAY_IDX(X)   ((((X) & BP_GPIO_NUM_MASK) < GPIO_NUM_MAX) ? ((((X) & BP_GPIO_NUM_MASK) >> 5) & 0x0f) : (0))
#define GPIO_NUM_TO_MASK(X)        ((((X) & BP_GPIO_NUM_MASK) < GPIO_NUM_MAX) ? (1 << (((X) & BP_GPIO_NUM_MASK) & 0x1f)) : (0))
#define GPIO_NUM_TO_ARRAY_SHIFT(X) (((X) & BP_GPIO_NUM_MASK) & 0x1f)

/*
 * Misc Register Set Definitions.
 */
typedef struct Misc {
   uint32 miscStrapBus; /* 0x00 */

    /* boot select bits 3-5 */
#define BOOT_SEL_STRAP_NAND_2K_PAGE             0x00
#define BOOT_SEL_STRAP_NAND_4K_PAGE             0x08
#define BOOT_SEL_STRAP_NAND_8K_PAGE             0x10
#define BOOT_SEL_STRAP_NAND_512B_PAGE           0x18
#define BOOT_SEL_STRAP_SPI_NOR                  0x38
#define BOOT_SEL_STRAP_EMMC                     0x30
#define BOOT_SEL_STRAP_SPI_NAND                 0x28

#define BOOT_SEL_STRAP_BOOT_SEL_MASK            (0x38)
#define BOOT_SEL_STRAP_PAGE_SIZE_MASK           (0x7)

#define MISC_STRAP_BUS_BOOT_SEL_SHIFT           0
#define MISC_STRAP_BUS_BOOT_SEL_MASK            (0x38 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_SPI_NOR             (0x38 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_EMMC                (0x30 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_SPI_NAND            (0x28 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_SEL_NAND_MASK       (0x20 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND                (0x00 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_SEL_PAGE_MASK       (0x18 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_2K_PAGE        (0x00 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_4K_PAGE        (0x08 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_8K_PAGE        (0x10 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_512_PAGE       (0x18 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_SEL_ECC_MASK        (0x7 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_ECC_DISABLE    (0x0 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_ECC_1_BIT      (0x1 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_ECC_4_BIT      (0x2 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_ECC_8_BIT      (0x3 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_ECC_12_BIT     (0x4 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_ECC_24_BIT     (0x5 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_ECC_40_BIT     (0x6 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_BOOT_NAND_ECC_60_BIT     (0x7 << MISC_STRAP_BUS_BOOT_SEL_SHIFT)
#define MISC_STRAP_BUS_PCIE0_RC_MODE            (0x1 << 6)
#define MISC_STRAP_BUS_LS_SPI_SLAVE_DISABLE     (0x1 << 7)
#define MISC_STRAP_BUS_B53_BOOT_N               (0x1 << 8)
/* When ROM BOOT OTP bits are 2b'11, always boot rom secure boot, this strap bit is don't care.
   When ROM BOOT OTP bits are are not 2b'11, this trap bit determine the following:
   1: boot rom non-secure boot
   0: XIP boot
*/
#define MISC_STRAP_BUS_BOOTROM_BOOT             (0x1 << 12)
#define MISC_STRAP_BUS_SW_RESERVE_MASK          (0x3 << 14)
#define MISC_STRAP_BUS_CPU_SLOW_FREQ_SHIFT      16
#define MISC_STRAP_BUS_CPU_SLOW_FREQ            (0x1 << MISC_STRAP_BUS_CPU_SLOW_FREQ_SHIFT)
   uint32 miscStrapOverride;     /* 0x04 */
   uint32 miscMaskUBUSErr;       /* 0x08 */
   uint32 miscPeriphCtrl;        /* 0x0c */
#define MISC_PERIPH_CTRL_SDIO_EMMC_CLK_SEL_SHIFT 16   
#define MISC_PERIPH_CTRL_SDIO_EMMC_CLK_SEL_MASK  (0x1 << MISC_PERIPH_CTRL_SDIO_EMMC_CLK_SEL_SHIFT)   
   uint32 miscSpiMasterCtrl;     /* 0x10 */
   uint32 reserved0;             /* 0x14 */
   uint32 miscPeriphMiscCtrl;    /* 0x18 */
   uint32 miscPeriphMiscStat;    /* 0x1c */
   uint32 miscSoftResetB;        /* 0x20 */
   uint32 miscSpare0;            /* 0x24 */
   uint32 miscSWdebugNW[2];      /* 0x28 */
   uint32 miscWDResetCtrl;       /* 0x30 */
} Misc;

#define MISC ((volatile Misc * const) MISC_BASE)


typedef struct Rng {
   uint32 ctrl0;                 /* 0x00 */
   uint32 rngSoftReset;          /* 0x04 */
   uint32 rbgSoftReset;          /* 0x08 */
   uint32 totalBitCnt;           /* 0x0c */
   uint32 totalBitCntThreshold;  /* 0x10 */
   uint32 revId;                 /* 0x14 */
   uint32 intStatus;             /* 0x18 */
#define RNG_INT_STATUS_NIST_FAIL       (0x1<<5)
#define RNG_INT_STATUS_FIFO_FULL       (0x1<<2)
   uint32 intEn;                 /* 0x1c */
   uint32 rngFifoData;           /* 0x20 */
   uint32 fifoCnt;               /* 0x24 */
#define RNG_PERM_ALLOW_SECURE_ACCESS           0xCC
#define RNG_PERM_ALLOW_NONSEC_ACCESS           0x33
   uint32 perm;                  /* 0x28 */
} Rng;

#define RNG ((volatile Rng * const) RNG_BASE)

/*
 * ARM UART Peripheral
 */

typedef struct UartArm {
   uint32 dr;                    /* 0x00 */
   uint32 rsr;                   /* 0x04 */
   uint32 rsrvd1[4];             /* 0x08 */
   uint32 fr;                    /* 0x18 */
#define FR_TXFE           0x80
#define FR_RXFF           0x40
#define FR_TXFF           0x20
#define FR_RXFE           0x10
#define FR_BUSY           0x04
   uint32 rsrvd2[1];	         /* 0x1c */
   uint32 ilpr;                  /* 0x20 */
   uint32 ibrd;                  /* 0x24 */
   uint32 fbrd;                  /* 0x28 */
   uint32 lcr_h;                 /* 0x2c */
#define LCR_H_SPS         0x80
#define LCR_H_SPS_SHIFT   7
#define LCR_H_WLEN_MASK   0x60
#define LCR_H_WLEN_SHIFT  0x05
#define LCR_H_WLEN_8BIT   0x60
#define LCR_H_WLEN_7BIT   0x40
#define LCR_H_WLEN_6BIT   0x20
#define LCR_H_WLEN_5BIT   0x00
#define LCR_H_FEN         0x10
#define LCR_H_FEN_SHIFT   4
#define LCR_H_STP2        0x08
#define LCR_H_STP2_SHIFT  3
#define LCR_H_EPS         0x04
#define LCR_H_EPS_SHIFT   0x02
#define LCR_H_PEN         0x02
#define LCR_H_PEN_SHIFT   0x01
#define LCR_H_BRK         0x01
#define LCR_H_BRK_SHIFT   0x00
   uint32 cr;                    /* 0x30 */
#define CR_CTSE           0x8000
#define CR_RTSE           0x4000
#define CR_OUT2           0x2000
#define CR_OUT1           0x1000
#define CR_RTS            0x800
#define CR_DTR            0x400
#define CR_RXE            0x200
#define CR_RXE_SHIFT      9
#define CR_TXE            0x100
#define CR_TXE_SHIFT      8 
#define CR_LBE            0x80
#define CR_RSV            0x78
#define CR_SIRLP          0x4
#define CR_SIRE           0x2
#define CR_EN             0x1
#define CR_EN_SHIFT       0
   uint32 ifls;                 /* 0x34 */
   uint32 imsc;                 /* 0x38 */
   uint32 ris;                  /* 0x3c */
   uint32 mis;                  /* 0x40 */
   uint32 icr;                  /* 0x44 */
   uint32 dmacr;                /* 0x48 */
   uint32 rsrvd3[13];           /* 0x4c */
   uint32 tcr;                  /* 0x80 */
   uint32 itip;                 /* 0x84 */
   uint32 itop;                 /* 0x88 */
   uint32 ttdr;                 /* 0x8c */
} UartArm;

#define ARM_UART ((volatile UartArm * const) ARM_UART_BASE)

typedef struct UartChannel {
   byte fifoctl;  /* 0x00 */
#define RSTTXFIFOS   0x80
#define RSTRXFIFOS   0x40
   /* 5-bit TimeoutCnt is in low bits of this register.
    *  This count represents the number of characters
    *  idle times before setting receive Irq when below threshold
    */
   byte config;
#define XMITBREAK 0x40
#define BITS5SYM  0x00
#define BITS6SYM  0x10
#define BITS7SYM  0x20
#define BITS8SYM  0x30
#define ONESTOP      0x07
#define TWOSTOP      0x0f
   /* 4-LSBS represent STOP bits/char
    * in 1/8 bit-time intervals.  Zero
    * represents 1/8 stop bit interval.
    * Fifteen represents 2 stop bits.
    */
   byte control;
#define BRGEN     0x80  /* Control register bit defs */
#define TXEN      0x40
#define RXEN      0x20
#define LOOPBK    0x10
#define TXPARITYEN   0x08
#define TXPARITYEVEN 0x04
#define RXPARITYEN   0x02
#define RXPARITYEVEN 0x01
   byte unused0;
          
   uint32 baudword;  /* 0x04 */
   /* When divide SysClk/2/(1+baudword) we should get 32*bit-rate
    */

   /* 0x08 */
   byte  prog_out;  /* Set value of DTR (Bit0), RTS (Bit1)
                *  if these bits are also enabled to
                *  GPIO_o
                */
#define ARMUARTEN 0x04
#define RTSEN     0x02
#define DTREN     0x01
   byte fifocfg;  /* Upper 4-bits are TxThresh, Lower are
                * RxThreshold.  Irq can be asserted
                * when rx fifo> thresh, txfifo<thresh
                */

   byte rxf_levl;  /* Read-only fifo depth */
   byte txf_levl;  /* Read-only fifo depth */

   /* 0x0c */
   byte DeltaIP_SyncIP;  /* Upper 4 bits show which bits
                   *  have changed (may set IRQ).
                   *  read automatically clears
                   *  bit.
                   *  Lower 4 bits are actual
                   *  status
                   */
   byte DeltaIPConfig_Mask; /* Upper 4 bits: 1 for posedge
                   * sense 0 for negedge sense if
                   * not configured for edge
                   * insensitive (see above)
                   * Lower 4 bits: Mask to enable
                   * change detection IRQ for
                   * corresponding GPIO_i
                   */
   byte DeltaIPEdgeNoSense; /* Low 4-bits, set corr bit to
                   * 1 to detect irq on rising
                   * AND falling edges for
                   * corresponding GPIO_i
                   * if enabled (edge insensitive)
                   */
   byte unused1;

   uint16 intStatus; /* 0x10 */
#define DELTAIP         0x0001
#define TXUNDERR        0x0002
#define TXOVFERR        0x0004
#define TXFIFOTHOLD     0x0008
#define TXREADLATCH     0x0010
#define TXFIFOEMT       0x0020
#define RXUNDERR        0x0040
#define RXOVFERR        0x0080
#define RXTIMEOUT       0x0100
#define RXFIFOFULL      0x0200
#define RXFIFOTHOLD     0x0400
#define RXFIFONE        0x0800
#define RXFRAMERR       0x1000
#define RXPARERR        0x2000
#define RXBRK           0x4000
   uint16 intMask;

   uint16 Data;    /* 0x14  Write to TX, Read from RX */
                        /* bits 10:8 are BRK,PAR,FRM errors */
   uint16 unused2;
} Uart;

#define UART ((volatile Uart * const) UART_BASE)


/*
 * LedControl Register Set Definitions.
 */
typedef struct LedControl {
   uint32 glbCtrl;           /* 0x00 */
   uint32 mask;              /* 0x04 */
   uint32 hWLedEn;           /* 0x08 */
   uint32 serialLedShiftSel; /* 0x0c */
   uint32 flashRateCtrl[4];  /* 0x10-0x1c */
   uint32 brightCtrl[4];     /* 0x20-0x2c */
   uint32 powerLedCfg;       /* 0x30 */
   uint32 pledLut[2][16];    /* 0x34-0x70, 0x74-0xb0 */
   uint32 HwPolarity;        /* 0xb4 */
   uint32 SwData;            /* 0xb8 */
   uint32 SwPolarity;        /* 0xbc */
   uint32 ParallelLedPolarity;/* 0xc0 */
   uint32 SerialLedPolarity; /* 0xc4 */
   uint32 HwLedStatus;       /* 0xc8 */
   uint32 FlashCtrlStatus;   /* 0xcc */
   uint32 FlashBrtCtrl;      /* 0xd0 */
   uint32 FlashPLedOutStatus;/* 0xd4 */
   uint32 FlashSLedOutStatus;/* 0xd8 */
} LedControl;

#define LED_NUM_LEDS              32
#define LED_NUM_PWM_LEDS          2
#define LED ((volatile LedControl * const) LED_BASE)
#define LED_NUM_TO_MASK(X)  (1 << ((X) & (LED_NUM_LEDS-1)))

typedef struct NtrReset {
   uint32 NtrResetReg;/* 0x0 */
#define CLK_DIV_RST_N           (1 << 1)    
#define NTR_RST_N               (1 << 0)    
} NtrReset;
#define NTR_RESET ((volatile NtrReset * const) NTR_RESET_BASE)

typedef struct NtrNco {
   uint32 Cfg;
   uint32 Int;
   uint32 Msk;
   uint32 Cfg1ppsPeriod;
   uint32 Cfg8khzPeriod;
   uint32 CenterFrequency;
   uint32 IntGain;
   uint32 ProGain;
   uint32 Cnt;
   uint32 Cfg1ppsHalf;
   uint32 Cfg8khzHalf;
   uint32 PeriodCnt;
   uint32 PhsErrCnt;
} NtrNco;
#define NTR_NCO ((volatile NtrNco * const) NTR_NCO_BASE)

typedef struct NtrClkPrgSwch {
   uint32 I2sPrgDivCfg1;       /* 0x00 I2S programmable divider cfg */
   uint32 I2sPrgDivCfg2;       /* 0x04 I2S programmable divider cfg */
   uint32 PcmPrgDivCfg1;       /* 0x08 PCM programmable divider cfg */
   uint32 PcmPrgDivCfg2;       /* 0x0c PCM programmable divider cfg */
#define CLK_SEL_250MHZ_SYNCE_PLL 0 
#define CLK_SEL_PON_SERDES       1
#define CLK_SEL_DSL_RDP          2       
#define CLK_SEL_SGPHY_UBUS       3       
#define CLK_SEL_LAN_SERDES       4
#define CLK_SEL_50MHZ_XTAL       5
#define CLK_SEL_PON_SERDES_DIV11 6
#define CLK_SEL_SHIFT            29
#define CLK_SEL_MASK             0xF0000000
#define CLK_DIV_MASK             (~CLK_SEL_MASK)
#define CLK_DENOM_MASK           0x0000FFFF

} NtrClkPrgSwch;
#define NTR_CLK_PRG_SWCH ((volatile NtrClkPrgSwch * const) NTR_CLK_PRG_SWCH_BASE)

typedef struct I2s {
   uint32 cfg;              /* 0x00 Config Register    */
#define I2S_ENABLE               (1 << 31)    
#define I2S_MCLK_RATE_SHIFT      20 
#define I2S_OUT_R                (1 << 19)    
#define I2S_OUT_L                (1 << 18)    
#define I2S_CLKSEL_SHIFT         16 
#define I2S_CLK_100MHZ           0
#define I2S_CLK_50MHZ            1
#define I2S_CLK_25MHZ            2
#define I2S_CLK_PLL              3
#define I2S_MCLK_CLKSEL_CLR_MASK 0xFF0CFFFF
#define I2S_BITS_PER_SAMPLE_SHIFT 10 
#define I2S_BITS_PER_SAMPLE_32   0 
#define I2S_BITS_PER_SAMPLE_24   24
#define I2S_BITS_PER_SAMPLE_20   20
#define I2S_BITS_PER_SAMPLE_18   18
#define I2S_BITS_PER_SAMPLE_16   16
#define I2S_SCLK_POLARITY        (1 << 9)    
#define I2S_LRCK_POLARITY        (1 << 8)    
#define I2S_SCLKS_PER_1FS_DIV32_SHIFT  4 
#define I2S_DATA_JUSTIFICATION   (1 << 3)    
#define I2S_DATA_ALIGNMENT       (1 << 2)    
#define I2S_DATA_ENABLE          (1 << 1)    
#define I2S_CLOCK_ENABLE         (1 << 0)
   
   uint32 intr;             /* 0x04 Interrupt Register */
#define I2S_DESC_OFF_LEVEL_SHIFT    12
#define I2S_DESC_IFF_LEVEL_SHIFT    8    
#define I2S_DESC_LEVEL_MASK         0x0F
#define I2S_DESC_OFF_OVERRUN_INTR   (1 << 3)
#define I2S_DESC_IFF_UNDERRUN_INTR  (1 << 2)
#define I2S_DESC_OFF_INTR           (1 << 1)
#define I2S_DESC_IFF_INTR           (1 << 0)
#define I2S_INTR_MASK               0x0F
   
   uint32 intr_en;          /* 0x08 Interrupt Enables Register */
#define I2S_DESC_INTR_TYPE_SEL        (1 << 4)
#define I2S_DESC_OFF_OVERRUN_INTR_EN  (1 << 3)
#define I2S_DESC_IFF_UNDERRUN_INTR_EN (1 << 2)
#define I2S_DESC_OFF_INTR_EN          (1 << 1)
#define I2S_DESC_IFF_INTR_EN          (1 << 0)
   
   uint32 intr_iff_thld;    /* 0x0c Descriptor Input FIFO Interrupt Threshold Register  */
   uint32 intr_off_thld;    /* 0x10 Descriptor Output FIFO Interrupt Threshold Register */
#define I2S_DESC_IFF_INTR_THLD_MASK    0x07
   
   uint32 desc_iff_addr;    /* 0x14 Descriptor Input FIFO Address  */
   uint32 desc_iff_len;     /* 0x18 Descriptor Input FIFO Length   */
   uint32 desc_off_addr;    /* 0x1c Descriptor Output FIFO Address */
   uint32 desc_off_len;     /* 0x20 Descriptor Output FIFO Length  */
#define I2S_DESC_EOP             (1 << 31)                                 
#define I2S_DESC_FIFO_DEPTH      8
#define I2S_DMA_BUFF_MAX_LEN     0xFFFF
#define I2S_DESC_LEN_MASK        I2S_DMA_BUFF_MAX_LEN
   
} I2s;

#define I2S ((volatile I2s * const) I2S_BASE)

typedef struct
{
   uint32 lut[32];           /* 0x00 */
   uint32 queue_depth[4];    /* 0x80 */
   uint32 cbs_thresh[8];     /* 0x90 */
   uint32 cir_incr[4];       /* 0xb0 */
   uint32 ref_cnt[4];        /* 0xc0 */
   uint32 max_bonus[2];      /* 0xd0 */
#define QUE_ID_NUM_BITS     (4) /* Number of queue_id bits per source port */
#define DEPTH_NUM_BITS      (8) /* Number of depth bits per queue id */
#define CBS_NUM_BITS        (16)/* Number of CBS bits per queue id */
#define CIR_INCR_NUM_BITS   (8) /* Number of CIS_INCR bits per queue id */
#define REF_CNT_NUM_BITS    (8) /* Number of refresh cnt bits per queue id */
#define MAX_BONUS_NUM_BITS  (4) /* Number of max bonus bits per queue id
                                   Actually 3-bits but 4th is reserved */

#define MAX_WLU_SRCPID_NUM                    256
#define MAX_WLU_SRCPID_REG_NUM                8
#define WLU_SRCPID_TO_REG_OFFSET(srcpid)      ((srcpid)>>5)
#define WLU_SRCPID_TO_REG_BIT(srcpid)         ((srcpid)%32)
   uint32 wlu_srcpid[MAX_WLU_SRCPID_REG_NUM];     /* 0xd8 */
   uint32 qos_reg[2];        /* 0xf8 */
}CoherencyPortCfgReg_t;
/*
** High-Speed SPI Controller
*/
#define __mask(end, start)      (((1 << ((end - start) + 1)) - 1) << start)
typedef struct HsSpiControl {
   uint32 hs_spiGlobalCtrl;   /* 0x00 */
#define HS_SPI_MOSI_IDLE   (1 << 18)
#define HS_SPI_CLK_POLARITY   (1 << 17)
#define HS_SPI_CLK_GATE_SSOFF (1 << 16)
#define HS_SPI_PLL_CLK_CTRL   (8)
#define HS_SPI_PLL_CLK_CTRL_MASK __mask(15, HS_SPI_PLL_CLK_CTRL)
#define HS_SPI_SS_POLARITY (0)
#define HS_SPI_SS_POLARITY_MASK     __mask(7, HS_SPI_SS_POLARITY)

   uint32 hs_spiExtTrigCtrl;  /* 0x04 */
#define HS_SPI_TRIG_RAW_STATE (24)
#define HS_SPI_TRIG_RAW_STATE_MASK  __mask(31, HS_SPI_TRIG_RAW_STATE)
#define HS_SPI_TRIG_LATCHED   (16)
#define HS_SPI_TRIG_LATCHED_MASK __mask(23, HS_SPI_TRIG_LATCHED)
#define HS_SPI_TRIG_SENSE  (8)
#define HS_SPI_TRIG_SENSE_MASK      __mask(15, HS_SPI_TRIG_SENSE)
#define HS_SPI_TRIG_TYPE   (0)
#define HS_SPI_TRIG_TYPE_MASK    __mask(7, HS_SPI_TRIG_TYPE)
#define HS_SPI_TRIG_TYPE_EDGE (0)
#define HS_SPI_TRIG_TYPE_LEVEL   (1)

   uint32 hs_spiIntStatus;    /* 0x08 */
#define HS_SPI_IRQ_PING1_USER (28)
#define HS_SPI_IRQ_PING1_USER_MASK  __mask(31, HS_SPI_IRQ_PING1_USER)
#define HS_SPI_IRQ_PING0_USER (24)
#define HS_SPI_IRQ_PING0_USER_MASK  __mask(27, HS_SPI_IRQ_PING0_USER)

#define HS_SPI_IRQ_PING1_CTRL_INV   (1 << 12)
#define HS_SPI_IRQ_PING1_POLL_TOUT  (1 << 11)
#define HS_SPI_IRQ_PING1_TX_UNDER   (1 << 10)
#define HS_SPI_IRQ_PING1_RX_OVER (1 << 9)
#define HS_SPI_IRQ_PING1_CMD_DONE   (1 << 8)

#define HS_SPI_IRQ_PING0_CTRL_INV   (1 << 4)
#define HS_SPI_IRQ_PING0_POLL_TOUT  (1 << 3)
#define HS_SPI_IRQ_PING0_TX_UNDER   (1 << 2)
#define HS_SPI_IRQ_PING0_RX_OVER (1 << 1)
#define HS_SPI_IRQ_PING0_CMD_DONE   (1 << 0)

   uint32 hs_spiIntStatusMasked; /* 0x0C */
#define HS_SPI_IRQSM__PING1_USER (28)
#define HS_SPI_IRQSM__PING1_USER_MASK  __mask(31, HS_SPI_IRQSM__PING1_USER)
#define HS_SPI_IRQSM__PING0_USER (24)
#define HS_SPI_IRQSM__PING0_USER_MASK  __mask(27, HS_SPI_IRQSM__PING0_USER)

#define HS_SPI_IRQSM__PING1_CTRL_INV   (1 << 12)
#define HS_SPI_IRQSM__PING1_POLL_TOUT  (1 << 11)
#define HS_SPI_IRQSM__PING1_TX_UNDER   (1 << 10)
#define HS_SPI_IRQSM__PING1_RX_OVER (1 << 9)
#define HS_SPI_IRQSM__PING1_CMD_DONE   (1 << 8)

#define HS_SPI_IRQSM__PING0_CTRL_INV   (1 << 4)
#define HS_SPI_IRQSM__PING0_POLL_TOUT  (1 << 3)
#define HS_SPI_IRQSM__PING0_TX_UNDER   (1 << 2)
#define HS_SPI_IRQSM__PING0_RX_OVER (1 << 1)
#define HS_SPI_IRQSM__PING0_CMD_DONE   (1 << 0)

   uint32 hs_spiIntMask;      /* 0x10 */
#define HS_SPI_IRQM_PING1_USER      (28)
#define HS_SPI_IRQM_PING1_USER_MASK __mask(31, HS_SPI_IRQM_PING1_USER)
#define HS_SPI_IRQM_PING0_USER      (24)
#define HS_SPI_IRQM_PING0_USER_MASK __mask(27, HS_SPI_IRQM_PING0_USER)

#define HS_SPI_IRQM_PING1_CTRL_INV  (1 << 12)
#define HS_SPI_IRQM_PING1_POLL_TOUT (1 << 11)
#define HS_SPI_IRQM_PING1_TX_UNDER  (1 << 10)
#define HS_SPI_IRQM_PING1_RX_OVER   (1 << 9)
#define HS_SPI_IRQM_PING1_CMD_DONE  (1 << 8)

#define HS_SPI_IRQM_PING0_CTRL_INV  (1 << 4)
#define HS_SPI_IRQM_PING0_POLL_TOUT (1 << 3)
#define HS_SPI_IRQM_PING0_TX_UNDER  (1 << 2)
#define HS_SPI_IRQM_PING0_RX_OVER   (1 << 1)
#define HS_SPI_IRQM_PING0_CMD_DONE  (1 << 0)

#define HS_SPI_INTR_CLEAR_ALL    (0xFF001F1F)

   uint32 hs_spiFlashCtrl;    /* 0x14 */
#define HS_SPI_FCTRL_MB_ENABLE      (23)
#define HS_SPI_FCTRL_SS_NUM      (20)
#define HS_SPI_FCTRL_SS_NUM_MASK __mask(22, HS_SPI_FCTRL_SS_NUM)
#define HS_SPI_FCTRL_PROFILE_NUM (16)
#define HS_SPI_FCTRL_PROFILE_NUM_MASK  __mask(18, HS_SPI_FCTRL_PROFILE_NUM)
#define HS_SPI_FCTRL_DUMMY_BYTES (10)
#define HS_SPI_FCTRL_DUMMY_BYTES_MASK  __mask(11, HS_SPI_FCTRL_DUMMY_BYTES)
#define HS_SPI_FCTRL_ADDR_BYTES     (8)
#define HS_SPI_FCTRL_ADDR_BYTES_MASK   __mask(9, HS_SPI_FCTRL_ADDR_BYTES)
#define HS_SPI_FCTRL_ADDR_BYTES_2   (0)
#define HS_SPI_FCTRL_ADDR_BYTES_3   (1)
#define HS_SPI_FCTRL_ADDR_BYTES_4   (2)
#define HS_SPI_FCTRL_READ_OPCODE (0)
#define HS_SPI_FCTRL_READ_OPCODE_MASK  __mask(7, HS_SPI_FCTRL_READ_OPCODE)

   uint32 hs_spiFlashAddrBase;   /* 0x18 */
} HsSpiControl;

typedef struct HsSpiPingPong {
   uint32 command;      /* 0x00 */
#define HS_SPI_SS_NUM      (12)
#define ZSI_SPI_DEV_ID     7     // SS_N[7] connected to APM/PCM block for use by MSIF/ZDS interfaces
#define HS_SPI_PROFILE_NUM (8)
#define HS_SPI_TRIGGER_NUM (4)
#define HS_SPI_COMMAND_VALUE  (0)
    #define HS_SPI_COMMAND_NOOP     (0)
    #define HS_SPI_COMMAND_START_NOW   (1)
    #define HS_SPI_COMMAND_START_TRIGGER (2)
    #define HS_SPI_COMMAND_HALT     (3)
    #define HS_SPI_COMMAND_FLUSH (4)

   uint32 status;    /* 0x04 */
#define HS_SPI_ERROR_BYTE_OFFSET (16)
#define HS_SPI_WAIT_FOR_TRIGGER     (2)
#define HS_SPI_SOURCE_BUSY    (1)
#define HS_SPI_SOURCE_GNT     (0)

   uint32 fifo_status;  /* 0x08 */
   uint32 control;      /* 0x0c */
} HsSpiPingPong;

typedef struct HsSpiProfile {
   uint32 clk_ctrl;  /* 0x00 */
#define HS_SPI_ACCUM_RST_ON_LOOP (15)
#define HS_SPI_SPI_CLK_2X_SEL    (14)
#define HS_SPI_FREQ_CTRL_WORD    (0)

   uint32 signal_ctrl;  /* 0x04 */
#define  HS_SPI_ASYNC_INPUT_PATH (1 << 16)
#define  HS_SPI_LAUNCH_RISING (1 << 13)
#define  HS_SPI_LATCH_RISING  (1 << 12)

   uint32 mode_ctrl; /* 0x08 */
#define HS_SPI_PREPENDBYTE_CNT      (24)
#define HS_SPI_MODE_ONE_WIRE     (20)
#define HS_SPI_MULTIDATA_WR_SIZE (18)
#define HS_SPI_MULTIDATA_RD_SIZE (16)
#define HS_SPI_MULTIDATA_WR_STRT (12)
#define HS_SPI_MULTIDATA_RD_STRT (8)
#define HS_SPI_FILLBYTE       (0)

   uint32 unused[5];
} HsSpiProfile;

#define HS_SPI_OP_CODE 13
    #define HS_SPI_OP_SLEEP (0)
    #define HS_SPI_OP_READ_WRITE (1)
    #define HS_SPI_OP_WRITE (2)
    #define HS_SPI_OP_READ (3)
    #define HS_SPI_OP_SETIRQ (4)

#define HS_SPI ((volatile HsSpiControl * const) HSSPIM_BASE)
#define HS_SPI_PINGPONG0 ((volatile HsSpiPingPong * const) (HSSPIM_BASE + 0x80))
#define HS_SPI_PINGPONG1 ((volatile HsSpiPingPong * const) (HSSPIM_BASE + 0xc0))
#define HS_SPI_PROFILES ((volatile HsSpiProfile * const) (HSSPIM_BASE + 0x100))
#define HS_SPI_FIFO0 ((volatile uint8 * const) (HSSPIM_BASE + 0x200))
#define HS_SPI_FIFO1 ((volatile uint8 * const) (HSSPIM_BASE + 0x400))

/*
** NAND Controller Registers
*/
typedef struct NandCtrlRegs {
    uint32 NandRevision;                   /* 0x00 */
    uint32 NandCmdStart;                   /* 0x04 */
#define NCMD_MASK                    0x0000001f
#define NCMD_BLOCK_ERASE_MULTI       0x15
#define NCMD_PROGRAM_PAGE_MULTI      0x13
#define NCMD_STS_READ_MULTI          0x12
#define NCMD_PAGE_READ_MULTI         0x11
#define NCMD_LOW_LEVEL_OP            0x10
#define NCMD_PARAM_CHG_COL           0x0f
#define NCMD_PARAM_READ              0x0e
#define NCMD_BLK_LOCK_STS            0x0d
#define NCMD_BLK_UNLOCK              0x0c
#define NCMD_BLK_LOCK_DOWN           0x0b
#define NCMD_BLK_LOCK                0x0a
#define NCMD_FLASH_RESET             0x09
#define NCMD_BLOCK_ERASE             0x08
#define NCMD_DEV_ID_READ             0x07
#define NCMD_COPY_BACK               0x06
#define NCMD_PROGRAM_SPARE           0x05
#define NCMD_PROGRAM_PAGE            0x04
#define NCMD_STS_READ                0x03
#define NCMD_SPARE_READ              0x02
#define NCMD_PAGE_READ               0x01

    uint32 NandCmdExtAddr;                 /* 0x08 */
    uint32 NandCmdAddr;                    /* 0x0c */
    uint32 NandCmdEndAddr;                 /* 0x10 */
    uint32 NandIntfcStatus;                /* 0x14 */
#define NIS_CTLR_READY         (1 << 31)
#define NIS_FLASH_READY        (1 << 30)
#define NIS_CACHE_VALID        (1 << 29)
#define NIS_SPARE_VALID        (1 << 28)
#define NIS_FLASH_STS_MASK     0x000000ff
#define NIS_WRITE_PROTECT      0x00000080
#define NIS_DEV_READY          0x00000040
#define NIS_PGM_ERASE_ERROR    0x00000001


    uint32 NandNandBootConfig;             /* 0x18 */
#define NBC_CS_LOCK            (1 << 31)
#define NBC_AUTO_DEV_ID_CFG    (1 << 30)
#define NBC_WR_PROT_BLK0       (1 << 28)
#define NBC_EBI_CS7_USES_NAND  (1<<15)
#define NBC_EBI_CS6_USES_NAND  (1<<14)
#define NBC_EBI_CS5_USES_NAND  (1<<13)
#define NBC_EBI_CS4_USES_NAND  (1<<12)
#define NBC_EBI_CS3_USES_NAND  (1<<11)
#define NBC_EBI_CS2_USES_NAND  (1<<10)
#define NBC_EBI_CS1_USES_NAND  (1<< 9)
#define NBC_EBI_CS0_USES_NAND  (1<< 8)
#define NBC_EBC_CS7_SEL        (1<< 7)
#define NBC_EBC_CS6_SEL        (1<< 6)
#define NBC_EBC_CS5_SEL        (1<< 5)
#define NBC_EBC_CS4_SEL        (1<< 4)
#define NBC_EBC_CS3_SEL        (1<< 3)
#define NBC_EBC_CS2_SEL        (1<< 2)
#define NBC_EBC_CS1_SEL        (1<< 1)
#define NBC_EBC_CS0_SEL        (1<< 0)

    uint32 NandCsNandXor;                   /* 0x1c */
    uint32 NandLlOpNand;                    /* 0x20 */
    uint32 NandMplaneBaseExtAddr;           /* 0x24 */
    uint32 NandMplaneBaseAddr;              /* 0x28 */
    uint32 NandReserved1[9];                /* 0x2c-0x4f */
    uint32 NandAccControl;                  /* 0x50 */
#define NAC_RD_ECC_EN          (1 << 31)
#define NAC_WR_ECC_EN          (1 << 30)
#define NAC_CE_CARE_EN         (1 << 28)
#define NAC_RD_ERASED_ECC_EN   (1 << 27)
#define NAC_PARTIAL_PAGE_EN    (1 << 26)
#define NAC_WR_PREEMPT_EN      (1 << 25)
#define NAC_PAGE_HIT_EN        (1 << 24)
#define NAC_PREFETCH_EN        (1 << 23)
#define NAC_CACHE_MODE_EN      (1 << 22)
#define NAC_ECC_LVL_SHIFT      16
#define NAC_ECC_LVL_MASK       0x001f0000
#define NAC_ECC_LVL_DISABLE    0
#define NAC_ECC_LVL_BCH_1      1
#define NAC_ECC_LVL_BCH_2      2
#define NAC_ECC_LVL_BCH_3      3
#define NAC_ECC_LVL_BCH_4      4
#define NAC_ECC_LVL_BCH_5      5
#define NAC_ECC_LVL_BCH_6      6
#define NAC_ECC_LVL_BCH_7      7
#define NAC_ECC_LVL_BCH_8      8
#define NAC_ECC_LVL_BCH_9      9
#define NAC_ECC_LVL_BCH_10     10
#define NAC_ECC_LVL_BCH_11     11
#define NAC_ECC_LVL_BCH_12     12
#define NAC_ECC_LVL_BCH_13     13
#define NAC_ECC_LVL_BCH_14     14
#define NAC_ECC_LVL_HAMMING    15  /* Hamming if spare are size = 16, BCH15 otherwise */
#define NAC_ECC_LVL_BCH15      15
#define NAC_ECC_LVL_BCH_16     16
#define NAC_ECC_LVL_BCH_17     17
/* BCH18 to 30 for sector size = 1K. To be added when we need it */
#define NAC_SECTOR_SIZE_1K     (1 << 7)
#define NAC_SPARE_SZ_SHIFT     0
#define NAC_SPARE_SZ_MASK      0x0000007f

    uint32 NandConfigExt;                   /* 0x54 */ /* Nand Flash Config Ext*/
#define NC_BLK_SIZE_MASK       (0xff << 4)
#define NC_BLK_SIZE_8192K      (0xa << 4)
#define NC_BLK_SIZE_4096K      (0x9 << 4)
#define NC_BLK_SIZE_2048K      (0x8 << 4)
#define NC_BLK_SIZE_1024K      (0x7 << 4)
#define NC_BLK_SIZE_512K       (0x6 << 4)
#define NC_BLK_SIZE_256K       (0x5 << 4)
#define NC_BLK_SIZE_128K       (0x4 << 4)
#define NC_BLK_SIZE_64K        (0x3 << 4)
#define NC_BLK_SIZE_32K        (0x2 << 4)
#define NC_BLK_SIZE_16K        (0x1 << 4)
#define NC_BLK_SIZE_8K         (0x0 << 4)
#define NC_PG_SIZE_MASK        (0xf << 0)
#define NC_PG_SIZE_16K         (0x5 << 0)
#define NC_PG_SIZE_8K          (0x4 << 0)
#define NC_PG_SIZE_4K          (0x3 << 0)
#define NC_PG_SIZE_2K          (0x2 << 0)
#define NC_PG_SIZE_1K          (0x1 << 0)
#define NC_PG_SIZE_512B        (0x0 << 0)

    uint32 NandConfig;                       /* 0x58 */ /* Nand Flash Config */
#define NC_CONFIG_LOCK         (1 << 31)
#define NC_DEV_SIZE_SHIFT      24
#define NC_DEV_SIZE_MASK       (0x0f << NC_DEV_SIZE_SHIFT)
#define NC_DEV_WIDTH_MASK      (1 << 23)
#define NC_DEV_WIDTH_16        (1 << 23)
#define NC_DEV_WIDTH_8         (0 << 23)
#define NC_FUL_ADDR_SHIFT      16
#define NC_FUL_ADDR_MASK       (0x7 << NC_FUL_ADDR_SHIFT)
#define NC_BLK_ADDR_SHIFT      8
#define NC_BLK_ADDR_MASK       (0x07 << NC_BLK_ADDR_SHIFT)

    uint32 NandTiming1;                      /* 0x5c */ /* Nand Flash Timing Parameters 1 */
#define NT_TREH_MASK           0x000f0000
#define NT_TREH_SHIFT          16
#define NT_TRP_MASK            0x00f00000
#define NT_TRP_SHIFT           20
    uint32 NandTiming2;                      /* 0x60 */ /* Nand Flash Timing Parameters 2 */
#define NT_TREAD_MASK          0x0000000f
#define NT_TREAD_SHIFT         0
    /* 0x64 */
    uint32 NandAccControlCs1;               /* Nand Flash Access Control */
    uint32 NandConfigExtCs1;                /* Nand Flash Config Ext*/
    uint32 NandConfigCs1;                   /* Nand Flash Config */
    uint32 NandTiming1Cs1;                  /* Nand Flash Timing Parameters 1 */
    uint32 NandTiming2Cs1;                  /* Nand Flash Timing Parameters 2 */
    /* 0x78 */
    uint32 NandAccControlCs2;               /* Nand Flash Access Control */
    uint32 NandConfigExtCs2;                /* Nand Flash Config Ext*/
    uint32 NandConfigCs2;                   /* Nand Flash Config */
    uint32 NandTiming1Cs2;                  /* Nand Flash Timing Parameters 1 */
    uint32 NandTiming2Cs2;                  /* Nand Flash Timing Parameters 2 */
    /* 0x8c */
    uint32 NandAccControlCs3;               /* Nand Flash Access Control */
    uint32 NandConfigExtCs3;                /* Nand Flash Config Ext*/
    uint32 NandConfigCs3;                   /* Nand Flash Config */
    uint32 NandTiming1Cs3;                  /* Nand Flash Timing Parameters 1 */
    uint32 NandTiming2Cs3;                  /* Nand Flash Timing Parameters 2 */
    /* 0xa0 */
    uint32 NandAccControlCs4;               /* Nand Flash Access Control */
    uint32 NandConfigExtCs4;                /* Nand Flash Config Ext*/
    uint32 NandConfigCs4;                   /* Nand Flash Config */
    uint32 NandTiming1Cs4;                  /* Nand Flash Timing Parameters 1 */
    uint32 NandTiming2Cs4;                  /* Nand Flash Timing Parameters 2 */
    /* 0xb4 */
    uint32 NandAccControlCs5;              /* Nand Flash Access Control */
    uint32 NandConfigExtCs5;               /* Nand Flash Config Ext*/
    uint32 NandConfigCs5;                  /* Nand Flash Config */
    uint32 NandTiming1Cs5;                 /* Nand Flash Timing Parameters 1 */
    uint32 NandTiming2Cs5;                 /* Nand Flash Timing Parameters 2 */
    /* 0xc8 */
    uint32 NandAccControlCs6;              /* Nand Flash Access Control */
    uint32 NandConfigExtCs6;               /* Nand Flash Config Ext*/
    uint32 NandConfigCs6;                  /* Nand Flash Config */
    uint32 NandTiming1Cs6;                 /* Nand Flash Timing Parameters 1 */
    uint32 NandTiming2Cs6;                 /* Nand Flash Timing Parameters 2 */

    /* 0xdc */
    uint32 NandCorrStatThreshold;          /* Correctable Error Reporting Threshold */
    uint32 NandCorrStatThresholdExt;       /* Correctable Error Reporting
                                            * Threshold */
    uint32 NandBlkWrProtect;               /* Block Write Protect Enable and Size */
                                           /*   for EBI_CS0b */
    uint32 NandMplaneOpcode1;

    /* 0xec */
    uint32 NandMplaneOpcode2;
    uint32 NandMplaneCtrl;
    uint32 NandReserved2[2];
    uint32 NandUncorrErrorCount;

    /* 0x100 */
    uint32 NandCorrErrorCount;
    uint32 NandReadErrorCount;           /* Read Error Count */
    uint32 NandBlockLockStatus;          /* Nand Flash Block Lock Status */
    uint32 NandEccCorrExtAddr;           /* ECC Correctable Error Extended Address*/
    /* 0x110 */
    uint32 NandEccCorrAddr;              /* ECC Correctable Error Address */
    uint32 NandEccUncExtAddr;            /* ECC Uncorrectable Error Extended Addr */
    uint32 NandEccUncAddr;               /* ECC Uncorrectable Error Address */
    uint32 NandFlashReadExtAddr;         /* Flash Read Data Extended Address */
    /* 0x120 */
    uint32 NandFlashReadAddr;            /* Flash Read Data Address */
    uint32 NandProgramPageExtAddr;       /* Page Program Extended Address */
    uint32 NandProgramPageAddr;          /* Page Program Address */
    uint32 NandCopyBackExtAddr;          /* Copy Back Extended Address */
    /* 0x130 */
    uint32 NandCopyBackAddr;             /* Copy Back Address */
    uint32 NandBlockEraseExtAddr;        /* Block Erase Extended Address */
    uint32 NandBlockEraseAddr;           /* Block Erase Address */
    uint32 NandInvReadExtAddr;           /* Flash Invalid Data Extended Address */
    /* 0x140 */
    uint32 NandInvReadAddr;              /* Flash Invalid Data Address */
    uint32 NandInitStatus;
    uint32 NandOnfiStatus;               /* ONFI Status */
    uint32 NandOnfiDebugData;            /* ONFI Debug Data */

    uint32 NandSemaphore;                /* 0x150 */ /* Semaphore */
    uint32 NandReserved3[16];            /* 0x154-0x193 */

    /* 0x194 */
    uint32 NandFlashDeviceId;            /* Nand Flash Device ID */
    uint32 NandFlashDeviceIdExt;         /* Nand Flash Extended Device ID */
    uint32 NandLlRdData;                 /* Nand Flash Low Level Read Data */

    uint32 NandReserved4[24];            /* 0x1a0 - 0x1ff */

    /* 0x200 */
    uint32 NandSpareAreaReadOfs0;        /* Nand Flash Spare Area Read Bytes 0-3 */
    uint32 NandSpareAreaReadOfs4;        /* Nand Flash Spare Area Read Bytes 4-7 */
    uint32 NandSpareAreaReadOfs8;        /* Nand Flash Spare Area Read Bytes 8-11 */
    uint32 NandSpareAreaReadOfsC;        /* Nand Flash Spare Area Read Bytes 12-15*/
    /* 0x210 */
    uint32 NandSpareAreaReadOfs10;       /* Nand Flash Spare Area Read Bytes 16-19 */
    uint32 NandSpareAreaReadOfs14;       /* Nand Flash Spare Area Read Bytes 20-23 */
    uint32 NandSpareAreaReadOfs18;       /* Nand Flash Spare Area Read Bytes 24-27 */
    uint32 NandSpareAreaReadOfs1C;       /* Nand Flash Spare Area Read Bytes 28-31*/
    /* 0x220 */
    uint32 NandSpareAreaReadOfs20;       /* Nand Flash Spare Area Read Bytes 32-35 */
    uint32 NandSpareAreaReadOfs24;       /* Nand Flash Spare Area Read Bytes 36-39 */
    uint32 NandSpareAreaReadOfs28;       /* Nand Flash Spare Area Read Bytes 40-43 */
    uint32 NandSpareAreaReadOfs2C;       /* Nand Flash Spare Area Read Bytes 44-47*/
    /* 0x230 */
    uint32 NandSpareAreaReadOfs30;       /* Nand Flash Spare Area Read Bytes 48-51 */
    uint32 NandSpareAreaReadOfs34;       /* Nand Flash Spare Area Read Bytes 52-55 */
    uint32 NandSpareAreaReadOfs38;       /* Nand Flash Spare Area Read Bytes 56-59 */
    uint32 NandSpareAreaReadOfs3C;       /* Nand Flash Spare Area Read Bytes 60-63*/

    uint32 NandReserved5[16];            /* 0x240-0x27f */

    /* 0x280 */
    uint32 NandSpareAreaWriteOfs0;       /* Nand Flash Spare Area Write Bytes 0-3 */
    uint32 NandSpareAreaWriteOfs4;       /* Nand Flash Spare Area Write Bytes 4-7 */
    uint32 NandSpareAreaWriteOfs8;       /* Nand Flash Spare Area Write Bytes 8-11 */
    uint32 NandSpareAreaWriteOfsC;       /* Nand Flash Spare Area Write Bytes 12-15 */
    /* 0x290 */
    uint32 NandSpareAreaWriteOfs10;      /* Nand Flash Spare Area Write Bytes 16-19 */
    uint32 NandSpareAreaWriteOfs14;      /* Nand Flash Spare Area Write Bytes 20-23 */
    uint32 NandSpareAreaWriteOfs18;      /* Nand Flash Spare Area Write Bytes 24-27 */
    uint32 NandSpareAreaWriteOfs1C;      /* Nand Flash Spare Area Write Bytes 28-31 */
    /* 0x2a0 */
    uint32 NandSpareAreaWriteOfs20;      /* Nand Flash Spare Area Write Bytes 32-35 */
    uint32 NandSpareAreaWriteOfs24;      /* Nand Flash Spare Area Write Bytes 36-39 */
    uint32 NandSpareAreaWriteOfs28;      /* Nand Flash Spare Area Write Bytes 40-43 */
    uint32 NandSpareAreaWriteOfs2C;      /* Nand Flash Spare Area Write Bytes 44-47 */
    /* 0x2b0 */
    uint32 NandSpareAreaWriteOfs30;      /* Nand Flash Spare Area Write Bytes 48-51 */
    uint32 NandSpareAreaWriteOfs34;      /* Nand Flash Spare Area Write Bytes 52-55 */
    uint32 NandSpareAreaWriteOfs38;      /* Nand Flash Spare Area Write Bytes 56-59 */
    uint32 NandSpareAreaWriteOfs3C;      /* Nand Flash Spare Area Write Bytes 60-63 */
    /* 0x2c0 */
    uint32 NandDdrTiming;
    uint32 NandDdrNcdlCalibCtl;
    uint32 NandDdrNcdlCalibPeriod;
    uint32 NandDdrNcdlCalibStat;
    /* 0x2d0 */
    uint32 NandDdrNcdlMode;
    uint32 NandDdrNcdlOffset;
    uint32 NandDdrPhyCtl;
    uint32 NandDdrPhyBistCtl;
    /* 0x2e0 */
    uint32 NandDdrPhyBistStat;
    uint32 NandDdrDiagStat0;
    uint32 NandDdrDiagStat1;
    uint32 NandReserved6[69];            /* 0x2ec-0x3ff */

    /* 0x400 */
    uint32 NandFlashCache[128];          /* 0x400-0x5ff */
} NandCtrlRegs;

#define NAND ((volatile NandCtrlRegs * const) NAND_REG_BASE)

/*
** NAND Interrupt Controller Registers
*/
typedef struct NandIntrCtrlRegs {
    uint32 NandInterrupt;
#define NINT_STS_MASK           0x00000fff
#define NINT_ECC_ERROR_CORR_SEC 0x00000800
#define NINT_ECC_ERROR_UNC_SEC  0x00000400
#define NINT_CTRL_READY_SEC     0x00000200
#define NINT_INV_ACC_SEC        0x00000100
#define NINT_ECC_ERROR_CORR     0x00000080
#define NINT_ECC_ERROR_UNC      0x00000040
#define NINT_DEV_RBPIN          0x00000020
#define NINT_CTRL_READY         0x00000010
#define NINT_PAGE_PGM           0x00000008
#define NINT_COPY_BACK          0x00000004
#define NINT_BLOCK_ERASE        0x00000002
#define NINT_NP_READ            0x00000001
    uint32 NandInterruptEn;
#define NINT_ENABLE_MASK        0x0000ffff
    uint32 NandBaseAddr0;   /* Default address when booting from NAND flash */
    uint32 NandBaseAddr1;   /* Secondary base address for NAND flash */
} NandIntrCtrlRegs;

#define NAND_INTR ((volatile NandIntrCtrlRegs * const) NAND_INTR_BASE)
#define NAND_CACHE ((volatile uint8 * const) NAND_CACHE_BASE)

/*
 * EMMC control registers
 */
typedef struct EmmcHostIfRegs {
   uint32 emmc_host_sdma;                  /* 0x00 System DMA Address Register                                     */
/***************************************************************************
 *SDMA - System DMA Address Register
 ***************************************************************************/
/* EMMC_HOSTIF :: SDMA :: ADDRESS [31:00] */
#define EMMC_HOSTIF_SDMA_ADDRESS_MASK                         0xffffffff
#define EMMC_HOSTIF_SDMA_ADDRESS_SHIFT                        0
#define EMMC_HOSTIF_SDMA_ADDRESS_DEFAULT                      0x00000000
   
   uint32 emmc_host_block;                 /* 0x04 Block Reset and Count Register                                  */
/***************************************************************************
 *BLOCK - Block Reset and Count Register
 ***************************************************************************/
/* EMMC_HOSTIF :: BLOCK :: TRANSFER_BLOCK_COUNT [31:16] */
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_COUNT_MASK           0xffff0000
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_COUNT_SHIFT          16
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_COUNT_DEFAULT        0x00000000

/* EMMC_HOSTIF :: BLOCK :: TRANSFER_BLOCK_SIZE_MSB [15:15] */
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_SIZE_MSB_MASK        0x00008000
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_SIZE_MSB_SHIFT       15
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_SIZE_MSB_DEFAULT     0x00000000

/* EMMC_HOSTIF :: BLOCK :: HOST_BUFFER_SIZE [14:12] */
#define EMMC_HOSTIF_BLOCK_HOST_BUFFER_SIZE_MASK               0x00007000
#define EMMC_HOSTIF_BLOCK_HOST_BUFFER_SIZE_SHIFT              12
#define EMMC_HOSTIF_BLOCK_HOST_BUFFER_SIZE_DEFAULT            0x00000000

/* EMMC_HOSTIF :: BLOCK :: TRANSFER_BLOCK_SIZE [11:00] */
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_SIZE_MASK            0x00000fff
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_SIZE_SHIFT           0
#define EMMC_HOSTIF_BLOCK_TRANSFER_BLOCK_SIZE_DEFAULT         0x00000000
   
   uint32 emmc_host_argument;              /* 0x08 Argument Register                                               */
/***************************************************************************
 *ARGUMENT - Argument Register
 ***************************************************************************/
/* EMMC_HOSTIF :: ARGUMENT :: CMD_ARG1 [31:00] */
#define EMMC_HOSTIF_ARGUMENT_CMD_ARG1_MASK                    0xffffffff
#define EMMC_HOSTIF_ARGUMENT_CMD_ARG1_SHIFT                   0
#define EMMC_HOSTIF_ARGUMENT_CMD_ARG1_DEFAULT                 0x00000000
   
   uint32 emmc_host_cmd_mode;              /* 0x0c Command and Mode Register                                       */
/***************************************************************************
 *CMD_MODE - Command and Mode Register
 ***************************************************************************/
/* EMMC_HOSTIF :: CMD_MODE :: reserved0 [31:30] */
#define EMMC_HOSTIF_CMD_MODE_reserved0_MASK                   0xc0000000
#define EMMC_HOSTIF_CMD_MODE_reserved0_SHIFT                  30

/* EMMC_HOSTIF :: CMD_MODE :: CMD_INDEX [29:24] */
#define EMMC_HOSTIF_CMD_MODE_CMD_INDEX_MASK                   0x3f000000
#define EMMC_HOSTIF_CMD_MODE_CMD_INDEX_SHIFT                  24
#define EMMC_HOSTIF_CMD_MODE_CMD_INDEX_DEFAULT                0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: CMD_TYPE [23:22] */
#define EMMC_HOSTIF_CMD_MODE_CMD_TYPE_MASK                    0x00c00000
#define EMMC_HOSTIF_CMD_MODE_CMD_TYPE_SHIFT                   22
#define EMMC_HOSTIF_CMD_MODE_CMD_TYPE_DEFAULT                 0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: DATA_PRESENT [21:21] */
#define EMMC_HOSTIF_CMD_MODE_DATA_PRESENT_MASK                0x00200000
#define EMMC_HOSTIF_CMD_MODE_DATA_PRESENT_SHIFT               21
#define EMMC_HOSTIF_CMD_MODE_DATA_PRESENT_DEFAULT             0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: CMD_INDEX_CHECK [20:20] */
#define EMMC_HOSTIF_CMD_MODE_CMD_INDEX_CHECK_MASK             0x00100000
#define EMMC_HOSTIF_CMD_MODE_CMD_INDEX_CHECK_SHIFT            20
#define EMMC_HOSTIF_CMD_MODE_CMD_INDEX_CHECK_DEFAULT          0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: CMD_CRC_CHECK [19:19] */
#define EMMC_HOSTIF_CMD_MODE_CMD_CRC_CHECK_MASK               0x00080000
#define EMMC_HOSTIF_CMD_MODE_CMD_CRC_CHECK_SHIFT              19
#define EMMC_HOSTIF_CMD_MODE_CMD_CRC_CHECK_DEFAULT            0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: reserved1 [18:18] */
#define EMMC_HOSTIF_CMD_MODE_reserved1_MASK                   0x00040000
#define EMMC_HOSTIF_CMD_MODE_reserved1_SHIFT                  18

/* EMMC_HOSTIF :: CMD_MODE :: RESPONSE_TYPE [17:16] */
#define EMMC_HOSTIF_CMD_MODE_RESPONSE_TYPE_MASK               0x00030000
#define EMMC_HOSTIF_CMD_MODE_RESPONSE_TYPE_SHIFT              16
#define EMMC_HOSTIF_CMD_MODE_RESPONSE_TYPE_DEFAULT            0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: reserved2 [15:07] */
#define EMMC_HOSTIF_CMD_MODE_reserved2_MASK                   0x0000ff80
#define EMMC_HOSTIF_CMD_MODE_reserved2_SHIFT                  7

/* EMMC_HOSTIF :: CMD_MODE :: reserved_for_eco3 [06:06] */
#define EMMC_HOSTIF_CMD_MODE_reserved_for_eco3_MASK           0x00000040
#define EMMC_HOSTIF_CMD_MODE_reserved_for_eco3_SHIFT          6
#define EMMC_HOSTIF_CMD_MODE_reserved_for_eco3_DEFAULT        0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: MULTI_BLOCK [05:05] */
#define EMMC_HOSTIF_CMD_MODE_MULTI_BLOCK_MASK                 0x00000020
#define EMMC_HOSTIF_CMD_MODE_MULTI_BLOCK_SHIFT                5
#define EMMC_HOSTIF_CMD_MODE_MULTI_BLOCK_DEFAULT              0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: TRANFER_WRITE [04:04] */
#define EMMC_HOSTIF_CMD_MODE_TRANFER_WRITE_MASK               0x00000010
#define EMMC_HOSTIF_CMD_MODE_TRANFER_WRITE_SHIFT              4
#define EMMC_HOSTIF_CMD_MODE_TRANFER_WRITE_DEFAULT            0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: AUTO_CMD_ENA [03:02] */
#define EMMC_HOSTIF_CMD_MODE_AUTO_CMD_ENA_MASK                0x0000000c
#define EMMC_HOSTIF_CMD_MODE_AUTO_CMD_ENA_SHIFT               2
#define EMMC_HOSTIF_CMD_MODE_AUTO_CMD_ENA_DEFAULT             0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: BLOCK_COUNT_ENABLE [01:01] */
#define EMMC_HOSTIF_CMD_MODE_BLOCK_COUNT_ENABLE_MASK          0x00000002
#define EMMC_HOSTIF_CMD_MODE_BLOCK_COUNT_ENABLE_SHIFT         1
#define EMMC_HOSTIF_CMD_MODE_BLOCK_COUNT_ENABLE_DEFAULT       0x00000000

/* EMMC_HOSTIF :: CMD_MODE :: DMA_ENABLE [00:00] */
#define EMMC_HOSTIF_CMD_MODE_DMA_ENABLE_MASK                  0x00000001
#define EMMC_HOSTIF_CMD_MODE_DMA_ENABLE_SHIFT                 0
#define EMMC_HOSTIF_CMD_MODE_DMA_ENABLE_DEFAULT               0x00000000
   
   uint32 emmc_host_resp_01;               /* 0x10 Response Word 0 and 1                                           */
/***************************************************************************
 *RESP_01 - Response Word 0 and 1
 ***************************************************************************/
/* EMMC_HOSTIF :: RESP_01 :: RESP_HI [31:16] */
#define EMMC_HOSTIF_RESP_01_RESP_HI_MASK                      0xffff0000
#define EMMC_HOSTIF_RESP_01_RESP_HI_SHIFT                     16
#define EMMC_HOSTIF_RESP_01_RESP_HI_DEFAULT                   0x00000000

/* EMMC_HOSTIF :: RESP_01 :: RESP_LO [15:00] */
#define EMMC_HOSTIF_RESP_01_RESP_LO_MASK                      0x0000ffff
#define EMMC_HOSTIF_RESP_01_RESP_LO_SHIFT                     0
#define EMMC_HOSTIF_RESP_01_RESP_LO_DEFAULT                   0x00000000
   
   uint32 emmc_host_resp_23;               /* 0x14 Response Word 2 and 3                                           */
/***************************************************************************
 *RESP_23 - Response Word 2 and 3
 ***************************************************************************/
/* EMMC_HOSTIF :: RESP_23 :: RESP_HI [31:16] */
#define EMMC_HOSTIF_RESP_23_RESP_HI_MASK                      0xffff0000
#define EMMC_HOSTIF_RESP_23_RESP_HI_SHIFT                     16
#define EMMC_HOSTIF_RESP_23_RESP_HI_DEFAULT                   0x00000000

/* EMMC_HOSTIF :: RESP_23 :: RESP_LO [15:00] */
#define EMMC_HOSTIF_RESP_23_RESP_LO_MASK                      0x0000ffff
#define EMMC_HOSTIF_RESP_23_RESP_LO_SHIFT                     0
#define EMMC_HOSTIF_RESP_23_RESP_LO_DEFAULT                   0x00000000
   
   uint32 emmc_host_resp_45;               /* 0x18 Response Word 4 and 5                                           */
/***************************************************************************
 *RESP_45 - Response Word 4 and 5
 ***************************************************************************/
/* EMMC_HOSTIF :: RESP_45 :: RESP_HI [31:16] */
#define EMMC_HOSTIF_RESP_45_RESP_HI_MASK                      0xffff0000
#define EMMC_HOSTIF_RESP_45_RESP_HI_SHIFT                     16
#define EMMC_HOSTIF_RESP_45_RESP_HI_DEFAULT                   0x00000000

/* EMMC_HOSTIF :: RESP_45 :: RESP_LO [15:00] */
#define EMMC_HOSTIF_RESP_45_RESP_LO_MASK                      0x0000ffff
#define EMMC_HOSTIF_RESP_45_RESP_LO_SHIFT                     0
#define EMMC_HOSTIF_RESP_45_RESP_LO_DEFAULT                   0x00000000
   
   uint32 emmc_host_resp_67;               /* 0x1c Response Word 6 and 7                                           */
/***************************************************************************
 *RESP_67 - Response Word 6 and 7
 ***************************************************************************/
/* EMMC_HOSTIF :: RESP_67 :: RESP_HI [31:16] */
#define EMMC_HOSTIF_RESP_67_RESP_HI_MASK                      0xffff0000
#define EMMC_HOSTIF_RESP_67_RESP_HI_SHIFT                     16
#define EMMC_HOSTIF_RESP_67_RESP_HI_DEFAULT                   0x00000000

/* EMMC_HOSTIF :: RESP_67 :: RESP_LO [15:00] */
#define EMMC_HOSTIF_RESP_67_RESP_LO_MASK                      0x0000ffff
#define EMMC_HOSTIF_RESP_67_RESP_LO_SHIFT                     0
#define EMMC_HOSTIF_RESP_67_RESP_LO_DEFAULT                   0x00000000
   
   uint32 emmc_host_buffdata;              /* 0x20 Buffer Data Port for PIO Tranfers                               */
/***************************************************************************
 *BUFFDATA - Buffer Data Port for PIO Tranfers
 ***************************************************************************/
/* EMMC_HOSTIF :: BUFFDATA :: PORT [31:00] */
#define EMMC_HOSTIF_BUFFDATA_PORT_MASK                        0xffffffff
#define EMMC_HOSTIF_BUFFDATA_PORT_SHIFT                       0
#define EMMC_HOSTIF_BUFFDATA_PORT_DEFAULT                     0x00000000

   
   uint32 emmc_host_state;                 /* 0x24 Present State of Controller                                     */
/***************************************************************************
 *STATE - Present State of Controller
 ***************************************************************************/
/* EMMC_HOSTIF :: STATE :: reserved0 [31:29] */
#define EMMC_HOSTIF_STATE_reserved0_MASK                      0xe0000000
#define EMMC_HOSTIF_STATE_reserved0_SHIFT                     29

/* EMMC_HOSTIF :: STATE :: LINE_7TO4 [28:25] */
#define EMMC_HOSTIF_STATE_LINE_7TO4_MASK                      0x1e000000
#define EMMC_HOSTIF_STATE_LINE_7TO4_SHIFT                     25
#define EMMC_HOSTIF_STATE_LINE_7TO4_DEFAULT                   0x0000000f

/* EMMC_HOSTIF :: STATE :: LINE_CMD [24:24] */
#define EMMC_HOSTIF_STATE_LINE_CMD_MASK                       0x01000000
#define EMMC_HOSTIF_STATE_LINE_CMD_SHIFT                      24
#define EMMC_HOSTIF_STATE_LINE_CMD_DEFAULT                    0x00000001

/* EMMC_HOSTIF :: STATE :: LINE_3TO0 [23:20] */
#define EMMC_HOSTIF_STATE_LINE_3TO0_MASK                      0x00f00000
#define EMMC_HOSTIF_STATE_LINE_3TO0_SHIFT                     20
#define EMMC_HOSTIF_STATE_LINE_3TO0_DEFAULT                   0x0000000f

/* EMMC_HOSTIF :: STATE :: WP_LEVEL [19:19] */
#define EMMC_HOSTIF_STATE_WP_LEVEL_MASK                       0x00080000
#define EMMC_HOSTIF_STATE_WP_LEVEL_SHIFT                      19

/* EMMC_HOSTIF :: STATE :: CD_LEVEL [18:18] */
#define EMMC_HOSTIF_STATE_CD_LEVEL_MASK                       0x00040000
#define EMMC_HOSTIF_STATE_CD_LEVEL_SHIFT                      18

/* EMMC_HOSTIF :: STATE :: CD_STABLE [17:17] */
#define EMMC_HOSTIF_STATE_CD_STABLE_MASK                      0x00020000
#define EMMC_HOSTIF_STATE_CD_STABLE_SHIFT                     17

/* EMMC_HOSTIF :: STATE :: CARD_INSERTED [16:16] */
#define EMMC_HOSTIF_STATE_CARD_INSERTED_MASK                  0x00010000
#define EMMC_HOSTIF_STATE_CARD_INSERTED_SHIFT                 16

/* EMMC_HOSTIF :: STATE :: reserved1 [15:12] */
#define EMMC_HOSTIF_STATE_reserved1_MASK                      0x0000f000
#define EMMC_HOSTIF_STATE_reserved1_SHIFT                     12
   /* EMMC_HOSTIF :: STATE :: BUFF_RDEN [11:11] */
#define EMMC_HOSTIF_STATE_BUFF_RDEN_MASK                      0x00000800
#define EMMC_HOSTIF_STATE_BUFF_RDEN_SHIFT                     11
#define EMMC_HOSTIF_STATE_BUFF_RDEN_DEFAULT                   0x00000000

/* EMMC_HOSTIF :: STATE :: BUFF_WREN [10:10] */
#define EMMC_HOSTIF_STATE_BUFF_WREN_MASK                      0x00000400
#define EMMC_HOSTIF_STATE_BUFF_WREN_SHIFT                     10
#define EMMC_HOSTIF_STATE_BUFF_WREN_DEFAULT                   0x00000000

/* EMMC_HOSTIF :: STATE :: RD_ACTIVE [09:09] */
#define EMMC_HOSTIF_STATE_RD_ACTIVE_MASK                      0x00000200
#define EMMC_HOSTIF_STATE_RD_ACTIVE_SHIFT                     9
#define EMMC_HOSTIF_STATE_RD_ACTIVE_DEFAULT                   0x00000000

/* EMMC_HOSTIF :: STATE :: WR_ACTIVE [08:08] */
#define EMMC_HOSTIF_STATE_WR_ACTIVE_MASK                      0x00000100
#define EMMC_HOSTIF_STATE_WR_ACTIVE_SHIFT                     8
#define EMMC_HOSTIF_STATE_WR_ACTIVE_DEFAULT                   0x00000000

/* EMMC_HOSTIF :: STATE :: reserved2 [07:04] */
#define EMMC_HOSTIF_STATE_reserved2_MASK                      0x000000f0
#define EMMC_HOSTIF_STATE_reserved2_SHIFT                     4

/* EMMC_HOSTIF :: STATE :: RE_TUNING_REQUEST [03:03] */
#define EMMC_HOSTIF_STATE_RE_TUNING_REQUEST_MASK              0x00000008
#define EMMC_HOSTIF_STATE_RE_TUNING_REQUEST_SHIFT             3
#define EMMC_HOSTIF_STATE_RE_TUNING_REQUEST_DEFAULT           0x00000000

/* EMMC_HOSTIF :: STATE :: DAT_ACTIVE [02:02] */
#define EMMC_HOSTIF_STATE_DAT_ACTIVE_MASK                     0x00000004
#define EMMC_HOSTIF_STATE_DAT_ACTIVE_SHIFT                    2
#define EMMC_HOSTIF_STATE_DAT_ACTIVE_DEFAULT                  0x00000000

/* EMMC_HOSTIF :: STATE :: CMD_INHIBIT_DAT [01:01] */
#define EMMC_HOSTIF_STATE_CMD_INHIBIT_DAT_MASK                0x00000002
#define EMMC_HOSTIF_STATE_CMD_INHIBIT_DAT_SHIFT               1
#define EMMC_HOSTIF_STATE_CMD_INHIBIT_DAT_DEFAULT             0x00000000

/* EMMC_HOSTIF :: STATE :: CMD_INHIBIT_CMD [00:00] */
#define EMMC_HOSTIF_STATE_CMD_INHIBIT_CMD_MASK                0x00000001
#define EMMC_HOSTIF_STATE_CMD_INHIBIT_CMD_SHIFT               0
#define EMMC_HOSTIF_STATE_CMD_INHIBIT_CMD_DEFAULT             0x00000000

   uint32 emmc_host_ctrl_set0;             /* 0x28 SD Standard Control Registers for Host, Power, BlockGap, WakeUp */
/***************************************************************************
 *CTRL_SET0 - SD Standard Control Registers for Host, Power, BlockGap, WakeUp
 ***************************************************************************/
/* EMMC_HOSTIF :: CTRL_SET0 :: reserved0 [31:27] */
#define EMMC_HOSTIF_CTRL_SET0_reserved0_MASK                  0xf8000000
#define EMMC_HOSTIF_CTRL_SET0_reserved0_SHIFT                 27

/* EMMC_HOSTIF :: CTRL_SET0 :: WAKE_ON_REMOVAL [26:26] */
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_REMOVAL_MASK            0x04000000
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_REMOVAL_SHIFT           26
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_REMOVAL_DEFAULT         0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: WAKE_ON_INSERTION [25:25] */
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_INSERTION_MASK          0x02000000
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_INSERTION_SHIFT         25
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_INSERTION_DEFAULT       0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: WAKE_ON_INTERRUPT [24:24] */
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_INTERRUPT_MASK          0x01000000
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_INTERRUPT_SHIFT         24
#define EMMC_HOSTIF_CTRL_SET0_WAKE_ON_INTERRUPT_DEFAULT       0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: BOOT_ACK [23:23] */
#define EMMC_HOSTIF_CTRL_SET0_BOOT_ACK_MASK                   0x00800000
#define EMMC_HOSTIF_CTRL_SET0_BOOT_ACK_SHIFT                  23
#define EMMC_HOSTIF_CTRL_SET0_BOOT_ACK_DEFAULT                0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: ALT_BOOT_EN [22:22] */
#define EMMC_HOSTIF_CTRL_SET0_ALT_BOOT_EN_MASK                0x00400000
#define EMMC_HOSTIF_CTRL_SET0_ALT_BOOT_EN_SHIFT               22
#define EMMC_HOSTIF_CTRL_SET0_ALT_BOOT_EN_DEFAULT             0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: BOOT_EN [21:21] */
#define EMMC_HOSTIF_CTRL_SET0_BOOT_EN_MASK                    0x00200000
#define EMMC_HOSTIF_CTRL_SET0_BOOT_EN_SHIFT                   21
#define EMMC_HOSTIF_CTRL_SET0_BOOT_EN_DEFAULT                 0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: SPI_MODE [20:20] */
#define EMMC_HOSTIF_CTRL_SET0_SPI_MODE_MASK                   0x00100000
#define EMMC_HOSTIF_CTRL_SET0_SPI_MODE_SHIFT                  20
#define EMMC_HOSTIF_CTRL_SET0_SPI_MODE_DEFAULT                0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: INT_AT_BLOCK_GAP [19:19] */
#define EMMC_HOSTIF_CTRL_SET0_INT_AT_BLOCK_GAP_MASK           0x00080000
#define EMMC_HOSTIF_CTRL_SET0_INT_AT_BLOCK_GAP_SHIFT          19
#define EMMC_HOSTIF_CTRL_SET0_INT_AT_BLOCK_GAP_DEFAULT        0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: READ_WAIT_CTRL [18:18] */
#define EMMC_HOSTIF_CTRL_SET0_READ_WAIT_CTRL_MASK             0x00040000
#define EMMC_HOSTIF_CTRL_SET0_READ_WAIT_CTRL_SHIFT            18
#define EMMC_HOSTIF_CTRL_SET0_READ_WAIT_CTRL_DEFAULT          0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: CONTINUE_REQUESTS [17:17] */
#define EMMC_HOSTIF_CTRL_SET0_CONTINUE_REQUESTS_MASK          0x00020000
#define EMMC_HOSTIF_CTRL_SET0_CONTINUE_REQUESTS_SHIFT         17
#define EMMC_HOSTIF_CTRL_SET0_CONTINUE_REQUESTS_DEFAULT       0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: STOP_AT_BLOCK_GAP [16:16] */
#define EMMC_HOSTIF_CTRL_SET0_STOP_AT_BLOCK_GAP_MASK          0x00010000
#define EMMC_HOSTIF_CTRL_SET0_STOP_AT_BLOCK_GAP_SHIFT         16
#define EMMC_HOSTIF_CTRL_SET0_STOP_AT_BLOCK_GAP_DEFAULT       0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: reserved1 [15:13] */
#define EMMC_HOSTIF_CTRL_SET0_reserved1_MASK                  0x0000e000
#define EMMC_HOSTIF_CTRL_SET0_reserved1_SHIFT                 13

/* EMMC_HOSTIF :: CTRL_SET0 :: HW_RESET [12:12] */
#define EMMC_HOSTIF_CTRL_SET0_HW_RESET_MASK                   0x00001000
#define EMMC_HOSTIF_CTRL_SET0_HW_RESET_SHIFT                  12
#define EMMC_HOSTIF_CTRL_SET0_HW_RESET_DEFAULT                0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: SD_BUS_VOLTAGE_SELECT [11:09] */
#define EMMC_HOSTIF_CTRL_SET0_SD_BUS_VOLTAGE_SELECT_MASK      0x00000e00
#define EMMC_HOSTIF_CTRL_SET0_SD_BUS_VOLTAGE_SELECT_SHIFT     9
#define EMMC_HOSTIF_CTRL_SET0_SD_BUS_VOLTAGE_SELECT_DEFAULT   0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: SD_BUS_POWER [08:08] */
#define EMMC_HOSTIF_CTRL_SET0_SD_BUS_POWER_MASK               0x00000100
#define EMMC_HOSTIF_CTRL_SET0_SD_BUS_POWER_SHIFT              8
#define EMMC_HOSTIF_CTRL_SET0_SD_BUS_POWER_DEFAULT            0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: CARD_DETECT_SELECT [07:07] */
#define EMMC_HOSTIF_CTRL_SET0_CARD_DETECT_SELECT_MASK         0x00000080
#define EMMC_HOSTIF_CTRL_SET0_CARD_DETECT_SELECT_SHIFT        7
#define EMMC_HOSTIF_CTRL_SET0_CARD_DETECT_SELECT_DEFAULT      0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: CARD_DETECT_TEST [06:06] */
#define EMMC_HOSTIF_CTRL_SET0_CARD_DETECT_TEST_MASK           0x00000040
#define EMMC_HOSTIF_CTRL_SET0_CARD_DETECT_TEST_SHIFT          6
#define EMMC_HOSTIF_CTRL_SET0_CARD_DETECT_TEST_DEFAULT        0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: SD_8BIT_MODE [05:05] */
#define EMMC_HOSTIF_CTRL_SET0_SD_8BIT_MODE_MASK               0x00000020
#define EMMC_HOSTIF_CTRL_SET0_SD_8BIT_MODE_SHIFT              5
#define EMMC_HOSTIF_CTRL_SET0_SD_8BIT_MODE_DEFAULT            0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: DMA_SELECT [04:03] */
#define EMMC_HOSTIF_CTRL_SET0_DMA_SELECT_MASK                 0x00000018
#define EMMC_HOSTIF_CTRL_SET0_DMA_SELECT_SHIFT                3
#define EMMC_HOSTIF_CTRL_SET0_DMA_SELECT_DEFAULT              0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: HIGH_SPEED_ENABLE [02:02] */
#define EMMC_HOSTIF_CTRL_SET0_HIGH_SPEED_ENABLE_MASK          0x00000004
#define EMMC_HOSTIF_CTRL_SET0_HIGH_SPEED_ENABLE_SHIFT         2
#define EMMC_HOSTIF_CTRL_SET0_HIGH_SPEED_ENABLE_DEFAULT       0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: SD_4BIT_MODE [01:01] */
#define EMMC_HOSTIF_CTRL_SET0_SD_4BIT_MODE_MASK               0x00000002
#define EMMC_HOSTIF_CTRL_SET0_SD_4BIT_MODE_SHIFT              1
#define EMMC_HOSTIF_CTRL_SET0_SD_4BIT_MODE_DEFAULT            0x00000000

/* EMMC_HOSTIF :: CTRL_SET0 :: LED_CONTROL [00:00] */
#define EMMC_HOSTIF_CTRL_SET0_LED_CONTROL_MASK                0x00000001
#define EMMC_HOSTIF_CTRL_SET0_LED_CONTROL_SHIFT               0
#define EMMC_HOSTIF_CTRL_SET0_LED_CONTROL_DEFAULT             0x00000000
   
   uint32 emmc_host_ctrl_set1;             /* 0x2c SD Standard Control Registers for Clock, Timeout, Resets        */
/***************************************************************************
 *CTRL_SET1 - SD Standard Control Registers for Clock, Timeout, Resets
 ***************************************************************************/
/* EMMC_HOSTIF :: CTRL_SET1 :: reserved0 [31:27] */
#define EMMC_HOSTIF_CTRL_SET1_reserved0_MASK                  0xf8000000
#define EMMC_HOSTIF_CTRL_SET1_reserved0_SHIFT                 27

/* EMMC_HOSTIF :: CTRL_SET1 :: SOFT_RESET_DAT [26:26] */
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_DAT_MASK             0x04000000
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_DAT_SHIFT            26
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_DAT_DEFAULT          0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: SOFT_RESET_CMD [25:25] */
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_CMD_MASK             0x02000000
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_CMD_SHIFT            25
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_CMD_DEFAULT          0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: SOFT_RESET_CORE [24:24] */
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_CORE_MASK            0x01000000
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_CORE_SHIFT           24
#define EMMC_HOSTIF_CTRL_SET1_SOFT_RESET_CORE_DEFAULT         0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: reserved1 [23:20] */
#define EMMC_HOSTIF_CTRL_SET1_reserved1_MASK                  0x00f00000
#define EMMC_HOSTIF_CTRL_SET1_reserved1_SHIFT                 20

/* EMMC_HOSTIF :: CTRL_SET1 :: TIMEOUT_COUNT [19:16] */
#define EMMC_HOSTIF_CTRL_SET1_TIMEOUT_COUNT_MASK              0x000f0000
#define EMMC_HOSTIF_CTRL_SET1_TIMEOUT_COUNT_SHIFT             16
#define EMMC_HOSTIF_CTRL_SET1_TIMEOUT_COUNT_DEFAULT           0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: FREQ_CTRL [15:08] */
#define EMMC_HOSTIF_CTRL_SET1_FREQ_CTRL_MASK                  0x0000ff00
#define EMMC_HOSTIF_CTRL_SET1_FREQ_CTRL_SHIFT                 8
#define EMMC_HOSTIF_CTRL_SET1_FREQ_CTRL_DEFAULT               0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: MS_CLK_FREQ [07:06] */
#define EMMC_HOSTIF_CTRL_SET1_MS_CLK_FREQ_MASK                0x000000c0
#define EMMC_HOSTIF_CTRL_SET1_MS_CLK_FREQ_SHIFT               6
#define EMMC_HOSTIF_CTRL_SET1_MS_CLK_FREQ_DEFAULT             0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: CLK_GEN_SEL [05:05] */
#define EMMC_HOSTIF_CTRL_SET1_CLK_GEN_SEL_MASK                0x00000020
#define EMMC_HOSTIF_CTRL_SET1_CLK_GEN_SEL_SHIFT               5
#define EMMC_HOSTIF_CTRL_SET1_CLK_GEN_SEL_DEFAULT             0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: reserved2 [04:03] */
#define EMMC_HOSTIF_CTRL_SET1_reserved2_MASK                  0x00000018
#define EMMC_HOSTIF_CTRL_SET1_reserved2_SHIFT                 3

/* EMMC_HOSTIF :: CTRL_SET1 :: SD_CLK_ENA [02:02] */
#define EMMC_HOSTIF_CTRL_SET1_SD_CLK_ENA_MASK                 0x00000004
#define EMMC_HOSTIF_CTRL_SET1_SD_CLK_ENA_SHIFT                2
#define EMMC_HOSTIF_CTRL_SET1_SD_CLK_ENA_DEFAULT              0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: INTERNAL_CLK_STABLE [01:01] */
#define EMMC_HOSTIF_CTRL_SET1_INTERNAL_CLK_STABLE_MASK        0x00000002
#define EMMC_HOSTIF_CTRL_SET1_INTERNAL_CLK_STABLE_SHIFT       1
#define EMMC_HOSTIF_CTRL_SET1_INTERNAL_CLK_STABLE_DEFAULT     0x00000000

/* EMMC_HOSTIF :: CTRL_SET1 :: INTERNAL_CLK_ENA [00:00] */
#define EMMC_HOSTIF_CTRL_SET1_INTERNAL_CLK_ENA_MASK           0x00000001
#define EMMC_HOSTIF_CTRL_SET1_INTERNAL_CLK_ENA_SHIFT          0
#define EMMC_HOSTIF_CTRL_SET1_INTERNAL_CLK_ENA_DEFAULT        0x00000000

   uint32 emmc_host_int_status;            /* 0x30 Interrupt Status for Normal and Error conditions                */
/***************************************************************************
 *INT_STATUS - Interrupt Status for Normal and Error conditions
 ***************************************************************************/
/* EMMC_HOSTIF :: INT_STATUS :: reserved0 [31:29] */
#define EMMC_HOSTIF_INT_STATUS_reserved0_MASK                 0xe0000000
#define EMMC_HOSTIF_INT_STATUS_reserved0_SHIFT                29

/* EMMC_HOSTIF :: INT_STATUS :: TARGET_RESP_ERR_INT [28:28] */
#define EMMC_HOSTIF_INT_STATUS_TARGET_RESP_ERR_INT_MASK       0x10000000
#define EMMC_HOSTIF_INT_STATUS_TARGET_RESP_ERR_INT_SHIFT      28
#define EMMC_HOSTIF_INT_STATUS_TARGET_RESP_ERR_INT_DEFAULT    0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: reserved1 [27:27] */
#define EMMC_HOSTIF_INT_STATUS_reserved1_MASK                 0x08000000
#define EMMC_HOSTIF_INT_STATUS_reserved1_SHIFT                27

/* EMMC_HOSTIF :: INT_STATUS :: TUNE_ERR [26:26] */
#define EMMC_HOSTIF_INT_STATUS_TUNE_ERR_MASK                  0x04000000
#define EMMC_HOSTIF_INT_STATUS_TUNE_ERR_SHIFT                 26
#define EMMC_HOSTIF_INT_STATUS_TUNE_ERR_DEFAULT               0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: ADMA_ERR_INT [25:25] */
#define EMMC_HOSTIF_INT_STATUS_ADMA_ERR_INT_MASK              0x02000000
#define EMMC_HOSTIF_INT_STATUS_ADMA_ERR_INT_SHIFT             25
#define EMMC_HOSTIF_INT_STATUS_ADMA_ERR_INT_DEFAULT           0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: AUTO_CMD_ERR_INT [24:24] */
#define EMMC_HOSTIF_INT_STATUS_AUTO_CMD_ERR_INT_MASK          0x01000000
#define EMMC_HOSTIF_INT_STATUS_AUTO_CMD_ERR_INT_SHIFT         24
#define EMMC_HOSTIF_INT_STATUS_AUTO_CMD_ERR_INT_DEFAULT       0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: CURRENT_LIMIT_ERR_INT [23:23] */
#define EMMC_HOSTIF_INT_STATUS_CURRENT_LIMIT_ERR_INT_MASK     0x00800000
#define EMMC_HOSTIF_INT_STATUS_CURRENT_LIMIT_ERR_INT_SHIFT    23
#define EMMC_HOSTIF_INT_STATUS_CURRENT_LIMIT_ERR_INT_DEFAULT  0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: DATA_END_BIT_ERR_INT [22:22] */
#define EMMC_HOSTIF_INT_STATUS_DATA_END_BIT_ERR_INT_MASK      0x00400000
#define EMMC_HOSTIF_INT_STATUS_DATA_END_BIT_ERR_INT_SHIFT     22
#define EMMC_HOSTIF_INT_STATUS_DATA_END_BIT_ERR_INT_DEFAULT   0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: DATA_CRC_ERR_INT [21:21] */
#define EMMC_HOSTIF_INT_STATUS_DATA_CRC_ERR_INT_MASK          0x00200000
#define EMMC_HOSTIF_INT_STATUS_DATA_CRC_ERR_INT_SHIFT         21
#define EMMC_HOSTIF_INT_STATUS_DATA_CRC_ERR_INT_DEFAULT       0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: DATA_TIMEOUT_ERR_INT [20:20] */
#define EMMC_HOSTIF_INT_STATUS_DATA_TIMEOUT_ERR_INT_MASK      0x00100000
#define EMMC_HOSTIF_INT_STATUS_DATA_TIMEOUT_ERR_INT_SHIFT     20
#define EMMC_HOSTIF_INT_STATUS_DATA_TIMEOUT_ERR_INT_DEFAULT   0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: CMD_INDEX_ERR_INT [19:19] */
#define EMMC_HOSTIF_INT_STATUS_CMD_INDEX_ERR_INT_MASK         0x00080000
#define EMMC_HOSTIF_INT_STATUS_CMD_INDEX_ERR_INT_SHIFT        19
#define EMMC_HOSTIF_INT_STATUS_CMD_INDEX_ERR_INT_DEFAULT      0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: CMD_END_BIT_ERR_INT [18:18] */
#define EMMC_HOSTIF_INT_STATUS_CMD_END_BIT_ERR_INT_MASK       0x00040000
#define EMMC_HOSTIF_INT_STATUS_CMD_END_BIT_ERR_INT_SHIFT      18
#define EMMC_HOSTIF_INT_STATUS_CMD_END_BIT_ERR_INT_DEFAULT    0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: CMD_CRC_ERR_INT [17:17] */
#define EMMC_HOSTIF_INT_STATUS_CMD_CRC_ERR_INT_MASK           0x00020000
#define EMMC_HOSTIF_INT_STATUS_CMD_CRC_ERR_INT_SHIFT          17
#define EMMC_HOSTIF_INT_STATUS_CMD_CRC_ERR_INT_DEFAULT        0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: CMD_TIMEOUT_ERR_INT [16:16] */
#define EMMC_HOSTIF_INT_STATUS_CMD_TIMEOUT_ERR_INT_MASK       0x00010000
#define EMMC_HOSTIF_INT_STATUS_CMD_TIMEOUT_ERR_INT_SHIFT      16
#define EMMC_HOSTIF_INT_STATUS_CMD_TIMEOUT_ERR_INT_DEFAULT    0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: ERROR_INT [15:15] */
#define EMMC_HOSTIF_INT_STATUS_ERROR_INT_MASK                 0x00008000
#define EMMC_HOSTIF_INT_STATUS_ERROR_INT_SHIFT                15
#define EMMC_HOSTIF_INT_STATUS_ERROR_INT_DEFAULT              0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: BOOT_TERM_INT [14:14] */
#define EMMC_HOSTIF_INT_STATUS_BOOT_TERM_INT_MASK             0x00004000
#define EMMC_HOSTIF_INT_STATUS_BOOT_TERM_INT_SHIFT            14
#define EMMC_HOSTIF_INT_STATUS_BOOT_TERM_INT_DEFAULT          0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: BOOT_ACK_RCV_INT [13:13] */
#define EMMC_HOSTIF_INT_STATUS_BOOT_ACK_RCV_INT_MASK          0x00002000
#define EMMC_HOSTIF_INT_STATUS_BOOT_ACK_RCV_INT_SHIFT         13
#define EMMC_HOSTIF_INT_STATUS_BOOT_ACK_RCV_INT_DEFAULT       0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: RETUNE_EVENT [12:12] */
#define EMMC_HOSTIF_INT_STATUS_RETUNE_EVENT_MASK              0x00001000
#define EMMC_HOSTIF_INT_STATUS_RETUNE_EVENT_SHIFT             12
#define EMMC_HOSTIF_INT_STATUS_RETUNE_EVENT_DEFAULT           0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: INT_C [11:11] */
#define EMMC_HOSTIF_INT_STATUS_INT_C_MASK                     0x00000800
#define EMMC_HOSTIF_INT_STATUS_INT_C_SHIFT                    11
#define EMMC_HOSTIF_INT_STATUS_INT_C_DEFAULT                  0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: INT_B [10:10] */
#define EMMC_HOSTIF_INT_STATUS_INT_B_MASK                     0x00000400
#define EMMC_HOSTIF_INT_STATUS_INT_B_SHIFT                    10
#define EMMC_HOSTIF_INT_STATUS_INT_B_DEFAULT                  0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: INT_A [09:09] */
#define EMMC_HOSTIF_INT_STATUS_INT_A_MASK                     0x00000200
#define EMMC_HOSTIF_INT_STATUS_INT_A_SHIFT                    9
#define EMMC_HOSTIF_INT_STATUS_INT_A_DEFAULT                  0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: CARD_INT [08:08] */
#define EMMC_HOSTIF_INT_STATUS_CARD_INT_MASK                  0x00000100
#define EMMC_HOSTIF_INT_STATUS_CARD_INT_SHIFT                 8
#define EMMC_HOSTIF_INT_STATUS_CARD_INT_DEFAULT               0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: CAR_REMOVAL_INT [07:07] */
#define EMMC_HOSTIF_INT_STATUS_CAR_REMOVAL_INT_MASK           0x00000080
#define EMMC_HOSTIF_INT_STATUS_CAR_REMOVAL_INT_SHIFT          7
#define EMMC_HOSTIF_INT_STATUS_CAR_REMOVAL_INT_DEFAULT        0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: CAR_INSERT_INT [06:06] */
#define EMMC_HOSTIF_INT_STATUS_CAR_INSERT_INT_MASK            0x00000040
#define EMMC_HOSTIF_INT_STATUS_CAR_INSERT_INT_SHIFT           6
#define EMMC_HOSTIF_INT_STATUS_CAR_INSERT_INT_DEFAULT         0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: BUFFER_READ_INT [05:05] */
#define EMMC_HOSTIF_INT_STATUS_BUFFER_READ_INT_MASK           0x00000020
#define EMMC_HOSTIF_INT_STATUS_BUFFER_READ_INT_SHIFT          5
#define EMMC_HOSTIF_INT_STATUS_BUFFER_READ_INT_DEFAULT        0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: BUFFER_WRITE_INT [04:04] */
#define EMMC_HOSTIF_INT_STATUS_BUFFER_WRITE_INT_MASK          0x00000010
#define EMMC_HOSTIF_INT_STATUS_BUFFER_WRITE_INT_SHIFT         4
#define EMMC_HOSTIF_INT_STATUS_BUFFER_WRITE_INT_DEFAULT       0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: DMA_INT [03:03] */
#define EMMC_HOSTIF_INT_STATUS_DMA_INT_MASK                   0x00000008
#define EMMC_HOSTIF_INT_STATUS_DMA_INT_SHIFT                  3
#define EMMC_HOSTIF_INT_STATUS_DMA_INT_DEFAULT                0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: BLOCK_GAP_INT [02:02] */
#define EMMC_HOSTIF_INT_STATUS_BLOCK_GAP_INT_MASK             0x00000004
#define EMMC_HOSTIF_INT_STATUS_BLOCK_GAP_INT_SHIFT            2
#define EMMC_HOSTIF_INT_STATUS_BLOCK_GAP_INT_DEFAULT          0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: TRANSFER_COMPLETE_INT [01:01] */
#define EMMC_HOSTIF_INT_STATUS_TRANSFER_COMPLETE_INT_MASK     0x00000002
#define EMMC_HOSTIF_INT_STATUS_TRANSFER_COMPLETE_INT_SHIFT    1
#define EMMC_HOSTIF_INT_STATUS_TRANSFER_COMPLETE_INT_DEFAULT  0x00000000

/* EMMC_HOSTIF :: INT_STATUS :: COMMAND_COMPLETE_INT [00:00] */
#define EMMC_HOSTIF_INT_STATUS_COMMAND_COMPLETE_INT_MASK      0x00000001
#define EMMC_HOSTIF_INT_STATUS_COMMAND_COMPLETE_INT_SHIFT     0
#define EMMC_HOSTIF_INT_STATUS_COMMAND_COMPLETE_INT_DEFAULT   0x00000000

   uint32 emmc_host_int_status_ena;        /* 0x34 Interrupt Enables for Normal and Error conditions               */
/***************************************************************************
 *INT_STATUS_ENA - Interrupt Enables for Normal and Error conditions
 ***************************************************************************/
/* EMMC_HOSTIF :: INT_STATUS_ENA :: reserved0 [31:30] */
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved0_MASK             0xc0000000
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved0_SHIFT            30

/* EMMC_HOSTIF :: INT_STATUS_ENA :: reserved_for_eco1 [29:29] */
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved_for_eco1_MASK     0x20000000
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved_for_eco1_SHIFT    29
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved_for_eco1_DEFAULT  0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: TARGET_RESP_ERR_INT_ENA [28:28] */
#define EMMC_HOSTIF_INT_STATUS_ENA_TARGET_RESP_ERR_INT_ENA_MASK 0x10000000
#define EMMC_HOSTIF_INT_STATUS_ENA_TARGET_RESP_ERR_INT_ENA_SHIFT 28
#define EMMC_HOSTIF_INT_STATUS_ENA_TARGET_RESP_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: reserved2 [27:27] */
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved2_MASK             0x08000000
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved2_SHIFT            27

/* EMMC_HOSTIF :: INT_STATUS_ENA :: TUNE_ERR_STAT_EN [26:26] */
#define EMMC_HOSTIF_INT_STATUS_ENA_TUNE_ERR_STAT_EN_MASK      0x04000000
#define EMMC_HOSTIF_INT_STATUS_ENA_TUNE_ERR_STAT_EN_SHIFT     26
#define EMMC_HOSTIF_INT_STATUS_ENA_TUNE_ERR_STAT_EN_DEFAULT   0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: ADMA_ERR_INT_ENA [25:25] */
#define EMMC_HOSTIF_INT_STATUS_ENA_ADMA_ERR_INT_ENA_MASK      0x02000000
#define EMMC_HOSTIF_INT_STATUS_ENA_ADMA_ERR_INT_ENA_SHIFT     25
#define EMMC_HOSTIF_INT_STATUS_ENA_ADMA_ERR_INT_ENA_DEFAULT   0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: AUTO_CMD12_ERR_INT_ENA [24:24] */
#define EMMC_HOSTIF_INT_STATUS_ENA_AUTO_CMD12_ERR_INT_ENA_MASK 0x01000000
#define EMMC_HOSTIF_INT_STATUS_ENA_AUTO_CMD12_ERR_INT_ENA_SHIFT 24
#define EMMC_HOSTIF_INT_STATUS_ENA_AUTO_CMD12_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: CURRENT_LIMIT_ERR_INT_ENA [23:23] */
#define EMMC_HOSTIF_INT_STATUS_ENA_CURRENT_LIMIT_ERR_INT_ENA_MASK 0x00800000
#define EMMC_HOSTIF_INT_STATUS_ENA_CURRENT_LIMIT_ERR_INT_ENA_SHIFT 23
#define EMMC_HOSTIF_INT_STATUS_ENA_CURRENT_LIMIT_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: DATA_END_BIT_ERR_INT_ENA [22:22] */
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_END_BIT_ERR_INT_ENA_MASK 0x00400000
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_END_BIT_ERR_INT_ENA_SHIFT 22
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_END_BIT_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: DATA_CRC_ERR_INT_ENA [21:21] */
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_CRC_ERR_INT_ENA_MASK  0x00200000
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_CRC_ERR_INT_ENA_SHIFT 21
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_CRC_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: DATA_TIMEOUT_ERR_INT_ENA [20:20] */
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_TIMEOUT_ERR_INT_ENA_MASK 0x00100000
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_TIMEOUT_ERR_INT_ENA_SHIFT 20
#define EMMC_HOSTIF_INT_STATUS_ENA_DATA_TIMEOUT_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: CMD_INDEX_ERR_INT_ENA [19:19] */
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_INDEX_ERR_INT_ENA_MASK 0x00080000
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_INDEX_ERR_INT_ENA_SHIFT 19
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_INDEX_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: CMD_END_BIT_ERR_INT_ENA [18:18] */
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_END_BIT_ERR_INT_ENA_MASK 0x00040000
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_END_BIT_ERR_INT_ENA_SHIFT 18
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_END_BIT_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: CMD_CRC_ERR_INT_ENA [17:17] */
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_CRC_ERR_INT_ENA_MASK   0x00020000
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_CRC_ERR_INT_ENA_SHIFT  17
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_CRC_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: CMD_TIMEOUT_ERR_INT_ENA [16:16] */
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_TIMEOUT_ERR_INT_ENA_MASK 0x00010000
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_TIMEOUT_ERR_INT_ENA_SHIFT 16
#define EMMC_HOSTIF_INT_STATUS_ENA_CMD_TIMEOUT_ERR_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: reserved3 [15:15] */
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved3_MASK             0x00008000
#define EMMC_HOSTIF_INT_STATUS_ENA_reserved3_SHIFT            15

/* EMMC_HOSTIF :: INT_STATUS_ENA :: BOOT_TERM_EN [14:14] */
#define EMMC_HOSTIF_INT_STATUS_ENA_BOOT_TERM_EN_MASK          0x00004000
#define EMMC_HOSTIF_INT_STATUS_ENA_BOOT_TERM_EN_SHIFT         14
#define EMMC_HOSTIF_INT_STATUS_ENA_BOOT_TERM_EN_DEFAULT       0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: BOOT_ACK_RCV_EN [13:13] */
#define EMMC_HOSTIF_INT_STATUS_ENA_BOOT_ACK_RCV_EN_MASK       0x00002000
#define EMMC_HOSTIF_INT_STATUS_ENA_BOOT_ACK_RCV_EN_SHIFT      13
#define EMMC_HOSTIF_INT_STATUS_ENA_BOOT_ACK_RCV_EN_DEFAULT    0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: RETUNE_EVENT_EN [12:12] */
#define EMMC_HOSTIF_INT_STATUS_ENA_RETUNE_EVENT_EN_MASK       0x00001000
#define EMMC_HOSTIF_INT_STATUS_ENA_RETUNE_EVENT_EN_SHIFT      12
#define EMMC_HOSTIF_INT_STATUS_ENA_RETUNE_EVENT_EN_DEFAULT    0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: INT_C_EN [11:11] */
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_C_EN_MASK              0x00000800
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_C_EN_SHIFT             11
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_C_EN_DEFAULT           0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: INT_B_EN [10:10] */
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_B_EN_MASK              0x00000400
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_B_EN_SHIFT             10
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_B_EN_DEFAULT           0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: INT_A_EN [09:09] */
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_A_EN_MASK              0x00000200
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_A_EN_SHIFT             9
#define EMMC_HOSTIF_INT_STATUS_ENA_INT_A_EN_DEFAULT           0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: CARD_INT_ENA [08:08] */
#define EMMC_HOSTIF_INT_STATUS_ENA_CARD_INT_ENA_MASK          0x00000100
#define EMMC_HOSTIF_INT_STATUS_ENA_CARD_INT_ENA_SHIFT         8
#define EMMC_HOSTIF_INT_STATUS_ENA_CARD_INT_ENA_DEFAULT       0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: CAR_REMOVAL_INT_ENA [07:07] */
#define EMMC_HOSTIF_INT_STATUS_ENA_CAR_REMOVAL_INT_ENA_MASK   0x00000080
#define EMMC_HOSTIF_INT_STATUS_ENA_CAR_REMOVAL_INT_ENA_SHIFT  7
#define EMMC_HOSTIF_INT_STATUS_ENA_CAR_REMOVAL_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: CAR_INSERT_INT_ENA [06:06] */
#define EMMC_HOSTIF_INT_STATUS_ENA_CAR_INSERT_INT_ENA_MASK    0x00000040
#define EMMC_HOSTIF_INT_STATUS_ENA_CAR_INSERT_INT_ENA_SHIFT   6
#define EMMC_HOSTIF_INT_STATUS_ENA_CAR_INSERT_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: BUFFER_READ_INT_ENA [05:05] */
#define EMMC_HOSTIF_INT_STATUS_ENA_BUFFER_READ_INT_ENA_MASK   0x00000020
#define EMMC_HOSTIF_INT_STATUS_ENA_BUFFER_READ_INT_ENA_SHIFT  5
#define EMMC_HOSTIF_INT_STATUS_ENA_BUFFER_READ_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: BUFFER_WRITE_INT_ENA [04:04] */
#define EMMC_HOSTIF_INT_STATUS_ENA_BUFFER_WRITE_INT_ENA_MASK  0x00000010
#define EMMC_HOSTIF_INT_STATUS_ENA_BUFFER_WRITE_INT_ENA_SHIFT 4
#define EMMC_HOSTIF_INT_STATUS_ENA_BUFFER_WRITE_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: DMA_INT_ENA [03:03] */
#define EMMC_HOSTIF_INT_STATUS_ENA_DMA_INT_ENA_MASK           0x00000008
#define EMMC_HOSTIF_INT_STATUS_ENA_DMA_INT_ENA_SHIFT          3
#define EMMC_HOSTIF_INT_STATUS_ENA_DMA_INT_ENA_DEFAULT        0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: BLOCK_GAP_INT_ENA [02:02] */
#define EMMC_HOSTIF_INT_STATUS_ENA_BLOCK_GAP_INT_ENA_MASK     0x00000004
#define EMMC_HOSTIF_INT_STATUS_ENA_BLOCK_GAP_INT_ENA_SHIFT    2
#define EMMC_HOSTIF_INT_STATUS_ENA_BLOCK_GAP_INT_ENA_DEFAULT  0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: TRANSFER_COMPLETE_INT_ENA [01:01] */
#define EMMC_HOSTIF_INT_STATUS_ENA_TRANSFER_COMPLETE_INT_ENA_MASK 0x00000002
#define EMMC_HOSTIF_INT_STATUS_ENA_TRANSFER_COMPLETE_INT_ENA_SHIFT 1
#define EMMC_HOSTIF_INT_STATUS_ENA_TRANSFER_COMPLETE_INT_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_STATUS_ENA :: COMMAND_COMPLETE_INT_ENA [00:00] */
#define EMMC_HOSTIF_INT_STATUS_ENA_COMMAND_COMPLETE_INT_ENA_MASK 0x00000001
#define EMMC_HOSTIF_INT_STATUS_ENA_COMMAND_COMPLETE_INT_ENA_SHIFT 0
#define EMMC_HOSTIF_INT_STATUS_ENA_COMMAND_COMPLETE_INT_ENA_DEFAULT 0x00000000

   uint32 emmc_host_int_signal_ena;        /* 0x38 Interrupt Signal Enables for Normal and Error conditions        */
/***************************************************************************
 *INT_SIGNAL_ENA - Interrupt Signal Enables for Normal and Error conditions
 ***************************************************************************/
/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: reserved0 [31:30] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved0_MASK             0xc0000000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved0_SHIFT            30

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: reserved_for_eco1 [29:29] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved_for_eco1_MASK     0x20000000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved_for_eco1_SHIFT    29
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved_for_eco1_DEFAULT  0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: TARGET_RESP_ERR_INT_SIG_ENA [28:28] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TARGET_RESP_ERR_INT_SIG_ENA_MASK 0x10000000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TARGET_RESP_ERR_INT_SIG_ENA_SHIFT 28
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TARGET_RESP_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: reserved2 [27:27] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved2_MASK             0x08000000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved2_SHIFT            27

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: TUNE_ERR_SIG_EN [26:26] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TUNE_ERR_SIG_EN_MASK       0x04000000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TUNE_ERR_SIG_EN_SHIFT      26
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TUNE_ERR_SIG_EN_DEFAULT    0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: ADMA_ERR_INT_SIG_ENA [25:25] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_ADMA_ERR_INT_SIG_ENA_MASK  0x02000000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_ADMA_ERR_INT_SIG_ENA_SHIFT 25
#define EMMC_HOSTIF_INT_SIGNAL_ENA_ADMA_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: AUTO_CMD12_ERR_INT_SIG_ENA [24:24] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_AUTO_CMD12_ERR_INT_SIG_ENA_MASK 0x01000000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_AUTO_CMD12_ERR_INT_SIG_ENA_SHIFT 24
#define EMMC_HOSTIF_INT_SIGNAL_ENA_AUTO_CMD12_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: CURRENT_LIMIT_ERR_INT_SIG_ENA [23:23] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CURRENT_LIMIT_ERR_INT_SIG_ENA_MASK 0x00800000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CURRENT_LIMIT_ERR_INT_SIG_ENA_SHIFT 23
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CURRENT_LIMIT_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: DATA_END_BIT_ERR_INT_SIG_ENA [22:22] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_END_BIT_ERR_INT_SIG_ENA_MASK 0x00400000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_END_BIT_ERR_INT_SIG_ENA_SHIFT 22
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_END_BIT_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: DATA_CRC_ERR_INT_SIG_ENA [21:21] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_CRC_ERR_INT_SIG_ENA_MASK 0x00200000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_CRC_ERR_INT_SIG_ENA_SHIFT 21
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_CRC_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: DATA_TIMEOUT_ERR_INT_SIG_ENA [20:20] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_TIMEOUT_ERR_INT_SIG_ENA_MASK 0x00100000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_TIMEOUT_ERR_INT_SIG_ENA_SHIFT 20
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DATA_TIMEOUT_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: CMD_INDEX_ERR_INT_SIG_ENA [19:19] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_INDEX_ERR_INT_SIG_ENA_MASK 0x00080000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_INDEX_ERR_INT_SIG_ENA_SHIFT 19
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_INDEX_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: CMD_END_BIT_ERR_INT_SIG_ENA [18:18] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_END_BIT_ERR_INT_SIG_ENA_MASK 0x00040000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_END_BIT_ERR_INT_SIG_ENA_SHIFT 18
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_END_BIT_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: CMD_CRC_ERR_INT_SIG_ENA [17:17] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_CRC_ERR_INT_SIG_ENA_MASK 0x00020000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_CRC_ERR_INT_SIG_ENA_SHIFT 17
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_CRC_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: CMD_TIMEOUT_ERR_INT_SIG_ENA [16:16] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_TIMEOUT_ERR_INT_SIG_ENA_MASK 0x00010000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_TIMEOUT_ERR_INT_SIG_ENA_SHIFT 16
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CMD_TIMEOUT_ERR_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: reserved3 [15:15] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved3_MASK             0x00008000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_reserved3_SHIFT            15

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: BOOT_TERM_INT_SIG_ENA [14:14] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BOOT_TERM_INT_SIG_ENA_MASK 0x00004000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BOOT_TERM_INT_SIG_ENA_SHIFT 14
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BOOT_TERM_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: BOOT_ACK_RCV_INT_SIG_ENA [13:13] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BOOT_ACK_RCV_INT_SIG_ENA_MASK 0x00002000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BOOT_ACK_RCV_INT_SIG_ENA_SHIFT 13
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BOOT_ACK_RCV_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: RETUNE_EVENT_EN [12:12] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_RETUNE_EVENT_EN_MASK       0x00001000
#define EMMC_HOSTIF_INT_SIGNAL_ENA_RETUNE_EVENT_EN_SHIFT      12
#define EMMC_HOSTIF_INT_SIGNAL_ENA_RETUNE_EVENT_EN_DEFAULT    0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: INT_C_EN [11:11] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_C_EN_MASK              0x00000800
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_C_EN_SHIFT             11
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_C_EN_DEFAULT           0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: INT_B_EN [10:10] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_B_EN_MASK              0x00000400
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_B_EN_SHIFT             10
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_B_EN_DEFAULT           0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: INT_A_EN [09:09] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_A_EN_MASK              0x00000200
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_A_EN_SHIFT             9
#define EMMC_HOSTIF_INT_SIGNAL_ENA_INT_A_EN_DEFAULT           0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: CARD_INT_SIG_ENA [08:08] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CARD_INT_SIG_ENA_MASK      0x00000100
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CARD_INT_SIG_ENA_SHIFT     8
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CARD_INT_SIG_ENA_DEFAULT   0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: CAR_REMOVAL_INT_SIG_ENA [07:07] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CAR_REMOVAL_INT_SIG_ENA_MASK 0x00000080
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CAR_REMOVAL_INT_SIG_ENA_SHIFT 7
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CAR_REMOVAL_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: CAR_INSERT_INT_SIG_ENA [06:06] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CAR_INSERT_INT_SIG_ENA_MASK 0x00000040
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CAR_INSERT_INT_SIG_ENA_SHIFT 6
#define EMMC_HOSTIF_INT_SIGNAL_ENA_CAR_INSERT_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: BUFFER_READ_INT_SIG_ENA [05:05] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BUFFER_READ_INT_SIG_ENA_MASK 0x00000020
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BUFFER_READ_INT_SIG_ENA_SHIFT 5
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BUFFER_READ_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: BUFFER_WRITE_INT_SIG_ENA [04:04] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BUFFER_WRITE_INT_SIG_ENA_MASK 0x00000010
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BUFFER_WRITE_INT_SIG_ENA_SHIFT 4
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BUFFER_WRITE_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: DMA_INT_SIG_ENA [03:03] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DMA_INT_SIG_ENA_MASK       0x00000008
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DMA_INT_SIG_ENA_SHIFT      3
#define EMMC_HOSTIF_INT_SIGNAL_ENA_DMA_INT_SIG_ENA_DEFAULT    0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: BLOCK_GAP_INT_SIG_ENA [02:02] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BLOCK_GAP_INT_SIG_ENA_MASK 0x00000004
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BLOCK_GAP_INT_SIG_ENA_SHIFT 2
#define EMMC_HOSTIF_INT_SIGNAL_ENA_BLOCK_GAP_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: TRANSFER_COMPLETE_INT_SIG_ENA [01:01] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TRANSFER_COMPLETE_INT_SIG_ENA_MASK 0x00000002
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TRANSFER_COMPLETE_INT_SIG_ENA_SHIFT 1
#define EMMC_HOSTIF_INT_SIGNAL_ENA_TRANSFER_COMPLETE_INT_SIG_ENA_DEFAULT 0x00000000

/* EMMC_HOSTIF :: INT_SIGNAL_ENA :: COMMAND_COMPLETE_INT_SIG_ENA [00:00] */
#define EMMC_HOSTIF_INT_SIGNAL_ENA_COMMAND_COMPLETE_INT_SIG_ENA_MASK 0x00000001
#define EMMC_HOSTIF_INT_SIGNAL_ENA_COMMAND_COMPLETE_INT_SIG_ENA_SHIFT 0
#define EMMC_HOSTIF_INT_SIGNAL_ENA_COMMAND_COMPLETE_INT_SIG_ENA_DEFAULT 0x00000000

   uint32 emmc_host_autocmd12_stat;        /* 0x3c Auto Cmd12 Error Status                                         */
/***************************************************************************
 *AUTOCMD12_STAT - Auto Cmd12 Error Status
 ***************************************************************************/
/* EMMC_HOSTIF :: AUTOCMD12_STAT :: PRESENT_VALUE_EN [31:31] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_PRESENT_VALUE_EN_MASK      0x80000000
#define EMMC_HOSTIF_AUTOCMD12_STAT_PRESENT_VALUE_EN_SHIFT     31
#define EMMC_HOSTIF_AUTOCMD12_STAT_PRESENT_VALUE_EN_DEFAULT   0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: ASYNC_INT_EN [30:30] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_ASYNC_INT_EN_MASK          0x40000000
#define EMMC_HOSTIF_AUTOCMD12_STAT_ASYNC_INT_EN_SHIFT         30
#define EMMC_HOSTIF_AUTOCMD12_STAT_ASYNC_INT_EN_DEFAULT       0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: reserved0 [29:24] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_reserved0_MASK             0x3f000000
#define EMMC_HOSTIF_AUTOCMD12_STAT_reserved0_SHIFT            24

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: SAMPLE_CLK_SEL [23:23] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_SAMPLE_CLK_SEL_MASK        0x00800000
#define EMMC_HOSTIF_AUTOCMD12_STAT_SAMPLE_CLK_SEL_SHIFT       23
#define EMMC_HOSTIF_AUTOCMD12_STAT_SAMPLE_CLK_SEL_DEFAULT     0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: EXECUTE_TUNE [22:22] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_EXECUTE_TUNE_MASK          0x00400000
#define EMMC_HOSTIF_AUTOCMD12_STAT_EXECUTE_TUNE_SHIFT         22
#define EMMC_HOSTIF_AUTOCMD12_STAT_EXECUTE_TUNE_DEFAULT       0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: DRIVER_STRENGTH [21:20] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_DRIVER_STRENGTH_MASK       0x00300000
#define EMMC_HOSTIF_AUTOCMD12_STAT_DRIVER_STRENGTH_SHIFT      20
#define EMMC_HOSTIF_AUTOCMD12_STAT_DRIVER_STRENGTH_DEFAULT    0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: 1P8_SIG_EN [19:19] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_1P8_SIG_EN_MASK            0x00080000
#define EMMC_HOSTIF_AUTOCMD12_STAT_1P8_SIG_EN_SHIFT           19
#define EMMC_HOSTIF_AUTOCMD12_STAT_1P8_SIG_EN_DEFAULT         0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: UHS_MODE [18:16] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_UHS_MODE_MASK              0x00070000
#define EMMC_HOSTIF_AUTOCMD12_STAT_UHS_MODE_SHIFT             16
#define EMMC_HOSTIF_AUTOCMD12_STAT_UHS_MODE_DEFAULT           0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: reserved1 [15:08] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_reserved1_MASK             0x0000ff00
#define EMMC_HOSTIF_AUTOCMD12_STAT_reserved1_SHIFT            8

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: CMD12_NOT_ISSUED [07:07] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD12_NOT_ISSUED_MASK      0x00000080
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD12_NOT_ISSUED_SHIFT     7
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD12_NOT_ISSUED_DEFAULT   0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: reserved2 [06:05] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_reserved2_MASK             0x00000060
#define EMMC_HOSTIF_AUTOCMD12_STAT_reserved2_SHIFT            5

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: CMD_INDEX_ERR [04:04] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_INDEX_ERR_MASK         0x00000010
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_INDEX_ERR_SHIFT        4
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_INDEX_ERR_DEFAULT      0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: CMD_END_ERR [03:03] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_END_ERR_MASK           0x00000008
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_END_ERR_SHIFT          3
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_END_ERR_DEFAULT        0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: CMD_CRC_ERR [02:02] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_CRC_ERR_MASK           0x00000004
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_CRC_ERR_SHIFT          2
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_CRC_ERR_DEFAULT        0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: CMD_TIMEOUT_ERR [01:01] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_TIMEOUT_ERR_MASK       0x00000002
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_TIMEOUT_ERR_SHIFT      1
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD_TIMEOUT_ERR_DEFAULT    0x00000000

/* EMMC_HOSTIF :: AUTOCMD12_STAT :: CMD12_NOT_EXEC_ERR [00:00] */
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD12_NOT_EXEC_ERR_MASK    0x00000001
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD12_NOT_EXEC_ERR_SHIFT   0
#define EMMC_HOSTIF_AUTOCMD12_STAT_CMD12_NOT_EXEC_ERR_DEFAULT 0x00000000

   uint32 emmc_host_capable;               /* 0x40 Host Controller Capabilities to Software                        */
/***************************************************************************
 *CAPABLE - Host Controller Capabilities to Software
 ***************************************************************************/
/* EMMC_HOSTIF :: CAPABLE :: SLOT_TYPE [31:30] */
#define EMMC_HOSTIF_CAPABLE_SLOT_TYPE_MASK                    0xc0000000
#define EMMC_HOSTIF_CAPABLE_SLOT_TYPE_SHIFT                   30

/* EMMC_HOSTIF :: CAPABLE :: INT_MODE [29:29] */
#define EMMC_HOSTIF_CAPABLE_INT_MODE_MASK                     0x20000000
#define EMMC_HOSTIF_CAPABLE_INT_MODE_SHIFT                    29

/* EMMC_HOSTIF :: CAPABLE :: SYS_BUT_64BIT [28:28] */
#define EMMC_HOSTIF_CAPABLE_SYS_BUT_64BIT_MASK                0x10000000
#define EMMC_HOSTIF_CAPABLE_SYS_BUT_64BIT_SHIFT               28

/* EMMC_HOSTIF :: CAPABLE :: reserved0 [27:27] */
#define EMMC_HOSTIF_CAPABLE_reserved0_MASK                    0x08000000
#define EMMC_HOSTIF_CAPABLE_reserved0_SHIFT                   27

/* EMMC_HOSTIF :: CAPABLE :: VOLTAGE_1P8V [26:26] */
#define EMMC_HOSTIF_CAPABLE_VOLTAGE_1P8V_MASK                 0x04000000
#define EMMC_HOSTIF_CAPABLE_VOLTAGE_1P8V_SHIFT                26

/* EMMC_HOSTIF :: CAPABLE :: VOLTAGE_3P0V [25:25] */
#define EMMC_HOSTIF_CAPABLE_VOLTAGE_3P0V_MASK                 0x02000000
#define EMMC_HOSTIF_CAPABLE_VOLTAGE_3P0V_SHIFT                25

/* EMMC_HOSTIF :: CAPABLE :: VOLTAGE_3P3V [24:24] */
#define EMMC_HOSTIF_CAPABLE_VOLTAGE_3P3V_MASK                 0x01000000
#define EMMC_HOSTIF_CAPABLE_VOLTAGE_3P3V_SHIFT                24

/* EMMC_HOSTIF :: CAPABLE :: SUSPEND_RESUME [23:23] */
#define EMMC_HOSTIF_CAPABLE_SUSPEND_RESUME_MASK               0x00800000
#define EMMC_HOSTIF_CAPABLE_SUSPEND_RESUME_SHIFT              23

/* EMMC_HOSTIF :: CAPABLE :: SDMA [22:22] */
#define EMMC_HOSTIF_CAPABLE_SDMA_MASK                         0x00400000
#define EMMC_HOSTIF_CAPABLE_SDMA_SHIFT                        22

/* EMMC_HOSTIF :: CAPABLE :: HIGH_SPEED [21:21] */
#define EMMC_HOSTIF_CAPABLE_HIGH_SPEED_MASK                   0x00200000
#define EMMC_HOSTIF_CAPABLE_HIGH_SPEED_SHIFT                  21

/* EMMC_HOSTIF :: CAPABLE :: reserved1 [20:20] */
#define EMMC_HOSTIF_CAPABLE_reserved1_MASK                    0x00100000
#define EMMC_HOSTIF_CAPABLE_reserved1_SHIFT                   20

/* EMMC_HOSTIF :: CAPABLE :: ADMA2 [19:19] */
#define EMMC_HOSTIF_CAPABLE_ADMA2_MASK                        0x00080000
#define EMMC_HOSTIF_CAPABLE_ADMA2_SHIFT                       19

/* EMMC_HOSTIF :: CAPABLE :: EXTENDED_MEDIA [18:18] */
#define EMMC_HOSTIF_CAPABLE_EXTENDED_MEDIA_MASK               0x00040000
#define EMMC_HOSTIF_CAPABLE_EXTENDED_MEDIA_SHIFT              18

/* EMMC_HOSTIF :: CAPABLE :: MAX_BLOCK_LEN [17:16] */
#define EMMC_HOSTIF_CAPABLE_MAX_BLOCK_LEN_MASK                0x00030000
#define EMMC_HOSTIF_CAPABLE_MAX_BLOCK_LEN_SHIFT               16

/* EMMC_HOSTIF :: CAPABLE :: BASE_CLK_FREQ [15:08] */
#define EMMC_HOSTIF_CAPABLE_BASE_CLK_FREQ_MASK                0x0000ff00
#define EMMC_HOSTIF_CAPABLE_BASE_CLK_FREQ_SHIFT               8

/* EMMC_HOSTIF :: CAPABLE :: TIMEOUT_UNIT [07:07] */
#define EMMC_HOSTIF_CAPABLE_TIMEOUT_UNIT_MASK                 0x00000080
#define EMMC_HOSTIF_CAPABLE_TIMEOUT_UNIT_SHIFT                7

/* EMMC_HOSTIF :: CAPABLE :: reserved2 [06:06] */
#define EMMC_HOSTIF_CAPABLE_reserved2_MASK                    0x00000040
#define EMMC_HOSTIF_CAPABLE_reserved2_SHIFT                   6

/* EMMC_HOSTIF :: CAPABLE :: TIMEOUT_CLK_FREQ [05:00] */
#define EMMC_HOSTIF_CAPABLE_TIMEOUT_CLK_FREQ_MASK             0x0000003f
#define EMMC_HOSTIF_CAPABLE_TIMEOUT_CLK_FREQ_SHIFT            0

   uint32 emmc_host_capable_1;             /* 0x44 Future Host Controller Capabilities to Software                 */
/***************************************************************************
 *CAPABLE_1 - Future Host Controller Capabilities to Software
 ***************************************************************************/
/* EMMC_HOSTIF :: CAPABLE_1 :: reserved0 [31:26] */
#define EMMC_HOSTIF_CAPABLE_1_reserved0_MASK                  0xfc000000
#define EMMC_HOSTIF_CAPABLE_1_reserved0_SHIFT                 26

/* EMMC_HOSTIF :: CAPABLE_1 :: SPI_BLOCK_MODE [25:25] */
#define EMMC_HOSTIF_CAPABLE_1_SPI_BLOCK_MODE_MASK             0x02000000
#define EMMC_HOSTIF_CAPABLE_1_SPI_BLOCK_MODE_SHIFT            25

/* EMMC_HOSTIF :: CAPABLE_1 :: SPI_MODE [24:24] */
#define EMMC_HOSTIF_CAPABLE_1_SPI_MODE_MASK                   0x01000000
#define EMMC_HOSTIF_CAPABLE_1_SPI_MODE_SHIFT                  24

/* EMMC_HOSTIF :: CAPABLE_1 :: CLK_MULT [23:16] */
#define EMMC_HOSTIF_CAPABLE_1_CLK_MULT_MASK                   0x00ff0000
#define EMMC_HOSTIF_CAPABLE_1_CLK_MULT_SHIFT                  16

/* EMMC_HOSTIF :: CAPABLE_1 :: RETUNING_MODE [15:14] */
#define EMMC_HOSTIF_CAPABLE_1_RETUNING_MODE_MASK              0x0000c000
#define EMMC_HOSTIF_CAPABLE_1_RETUNING_MODE_SHIFT             14

/* EMMC_HOSTIF :: CAPABLE_1 :: TUNE_SDR50 [13:13] */
#define EMMC_HOSTIF_CAPABLE_1_TUNE_SDR50_MASK                 0x00002000
#define EMMC_HOSTIF_CAPABLE_1_TUNE_SDR50_SHIFT                13

/* EMMC_HOSTIF :: CAPABLE_1 :: reserved1 [12:12] */
#define EMMC_HOSTIF_CAPABLE_1_reserved1_MASK                  0x00001000
#define EMMC_HOSTIF_CAPABLE_1_reserved1_SHIFT                 12

/* EMMC_HOSTIF :: CAPABLE_1 :: TIME_RETUNE [11:08] */
#define EMMC_HOSTIF_CAPABLE_1_TIME_RETUNE_MASK                0x00000f00
#define EMMC_HOSTIF_CAPABLE_1_TIME_RETUNE_SHIFT               8

/* EMMC_HOSTIF :: CAPABLE_1 :: reserved2 [07:07] */
#define EMMC_HOSTIF_CAPABLE_1_reserved2_MASK                  0x00000080
#define EMMC_HOSTIF_CAPABLE_1_reserved2_SHIFT                 7

/* EMMC_HOSTIF :: CAPABLE_1 :: DRIVER_D [06:06] */
#define EMMC_HOSTIF_CAPABLE_1_DRIVER_D_MASK                   0x00000040
#define EMMC_HOSTIF_CAPABLE_1_DRIVER_D_SHIFT                  6

/* EMMC_HOSTIF :: CAPABLE_1 :: DRIVER_C [05:05] */
#define EMMC_HOSTIF_CAPABLE_1_DRIVER_C_MASK                   0x00000020
#define EMMC_HOSTIF_CAPABLE_1_DRIVER_C_SHIFT                  5

/* EMMC_HOSTIF :: CAPABLE_1 :: DRIVER_A [04:04] */
#define EMMC_HOSTIF_CAPABLE_1_DRIVER_A_MASK                   0x00000010
#define EMMC_HOSTIF_CAPABLE_1_DRIVER_A_SHIFT                  4

/* EMMC_HOSTIF :: CAPABLE_1 :: reserved3 [03:03] */
#define EMMC_HOSTIF_CAPABLE_1_reserved3_MASK                  0x00000008
#define EMMC_HOSTIF_CAPABLE_1_reserved3_SHIFT                 3

/* EMMC_HOSTIF :: CAPABLE_1 :: DDR50 [02:02] */
#define EMMC_HOSTIF_CAPABLE_1_DDR50_MASK                      0x00000004
#define EMMC_HOSTIF_CAPABLE_1_DDR50_SHIFT                     2

/* EMMC_HOSTIF :: CAPABLE_1 :: SDR104 [01:01] */
#define EMMC_HOSTIF_CAPABLE_1_SDR104_MASK                     0x00000002
#define EMMC_HOSTIF_CAPABLE_1_SDR104_SHIFT                    1

/* EMMC_HOSTIF :: CAPABLE_1 :: SDR50 [00:00] */
#define EMMC_HOSTIF_CAPABLE_1_SDR50_MASK                      0x00000001
#define EMMC_HOSTIF_CAPABLE_1_SDR50_SHIFT                     0

   uint32 emmc_host_power_capable;         /* 0x48 Host Controller Power Capabilities to Software                  */
/***************************************************************************
 *POWER_CAPABLE - Host Controller Power Capabilities to Software
 ***************************************************************************/
/* EMMC_HOSTIF :: POWER_CAPABLE :: reserved0 [31:24] */
#define EMMC_HOSTIF_POWER_CAPABLE_reserved0_MASK              0xff000000
#define EMMC_HOSTIF_POWER_CAPABLE_reserved0_SHIFT             24

/* EMMC_HOSTIF :: POWER_CAPABLE :: MAX_CURRENT_1P8V [23:16] */
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_1P8V_MASK       0x00ff0000
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_1P8V_SHIFT      16
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_1P8V_DEFAULT    0x00000000

/* EMMC_HOSTIF :: POWER_CAPABLE :: MAX_CURRENT_3P0V [15:08] */
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_3P0V_MASK       0x0000ff00
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_3P0V_SHIFT      8
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_3P0V_DEFAULT    0x00000000

/* EMMC_HOSTIF :: POWER_CAPABLE :: MAX_CURRENT_3P3V [07:00] */
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_3P3V_MASK       0x000000ff
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_3P3V_SHIFT      0
#define EMMC_HOSTIF_POWER_CAPABLE_MAX_CURRENT_3P3V_DEFAULT    0x00000001

   uint32 emmc_host_power_capable_rsvd;    /* 0x4c Future Host Controller Power Capabilities to Software           */
/***************************************************************************
 *POWER_CAPABLE_RSVD - Future Host Controller Power Capabilities to Software
 ***************************************************************************/
/* EMMC_HOSTIF :: POWER_CAPABLE_RSVD :: POWERCAPS_RSVD [31:00] */
#define EMMC_HOSTIF_POWER_CAPABLE_RSVD_POWERCAPS_RSVD_MASK    0xffffffff
#define EMMC_HOSTIF_POWER_CAPABLE_RSVD_POWERCAPS_RSVD_SHIFT   0
#define EMMC_HOSTIF_POWER_CAPABLE_RSVD_POWERCAPS_RSVD_DEFAULT 0x00000000

   uint32 emmc_host_force_events;          /* 0x50 Force Events on Error Status Bits                               */
/***************************************************************************
 *FORCE_EVENTS - Force Events on Error Status Bits
 ***************************************************************************/
/* EMMC_HOSTIF :: FORCE_EVENTS :: reserved0 [31:30] */
#define B0_EMMC_HOSTIF_FORCE_EVENTS_reserved0_MASK            0xc0000000
#define B0_EMMC_HOSTIF_FORCE_EVENTS_reserved0_SHIFT           30

/* EMMC_HOSTIF :: FORCE_EVENTS :: reserved0 [29:29] */
#define EMMC_HOSTIF_FORCE_EVENTS_reserved0_MASK               0x20000000
#define EMMC_HOSTIF_FORCE_EVENTS_reserved0_SHIFT              29

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_TARGET_RESP_ERR_INT [28:28] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_TARGET_RESP_ERR_INT_MASK 0x10000000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_TARGET_RESP_ERR_INT_SHIFT 28
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_TARGET_RESP_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: reserved1 [27:26] */
#define EMMC_HOSTIF_FORCE_EVENTS_reserved1_MASK               0x0c000000
#define EMMC_HOSTIF_FORCE_EVENTS_reserved1_SHIFT              26

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_ADMA_ERR_INT [25:25] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_ADMA_ERR_INT_MASK      0x02000000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_ADMA_ERR_INT_SHIFT     25
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_ADMA_ERR_INT_DEFAULT   0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_AUTO_CMD_ERR_INT [24:24] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_AUTO_CMD_ERR_INT_MASK  0x01000000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_AUTO_CMD_ERR_INT_SHIFT 24
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_AUTO_CMD_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CURRENT_LIMIT_ERR_INT [23:23] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CURRENT_LIMIT_ERR_INT_MASK 0x00800000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CURRENT_LIMIT_ERR_INT_SHIFT 23
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CURRENT_LIMIT_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_DATA_END_BIT_ERR_INT [22:22] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_END_BIT_ERR_INT_MASK 0x00400000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_END_BIT_ERR_INT_SHIFT 22
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_END_BIT_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_DATA_CRC_ERR_INT [21:21] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_CRC_ERR_INT_MASK  0x00200000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_CRC_ERR_INT_SHIFT 21
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_CRC_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_DATA_TIMEOUT_ERR_INT [20:20] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_TIMEOUT_ERR_INT_MASK 0x00100000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_TIMEOUT_ERR_INT_SHIFT 20
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_DATA_TIMEOUT_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_INDEX_ERR_INT [19:19] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_INDEX_ERR_INT_MASK 0x00080000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_INDEX_ERR_INT_SHIFT 19
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_INDEX_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_END_BIT_ERR_INT [18:18] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_END_BIT_ERR_INT_MASK 0x00040000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_END_BIT_ERR_INT_SHIFT 18
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_END_BIT_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_CRC_ERR_INT [17:17] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_CRC_ERR_INT_MASK   0x00020000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_CRC_ERR_INT_SHIFT  17
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_CRC_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_TIMEOUT_ERR_INT [16:16] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_TIMEOUT_ERR_INT_MASK 0x00010000
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_TIMEOUT_ERR_INT_SHIFT 16
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_TIMEOUT_ERR_INT_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: reserved2 [15:08] */
#define EMMC_HOSTIF_FORCE_EVENTS_reserved2_MASK               0x0000ff00
#define EMMC_HOSTIF_FORCE_EVENTS_reserved2_SHIFT              8

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD12_NOT_ISSUED [07:07] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD12_NOT_ISSUED_MASK  0x00000080
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD12_NOT_ISSUED_SHIFT 7
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD12_NOT_ISSUED_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: reserved3 [06:05] */
#define EMMC_HOSTIF_FORCE_EVENTS_reserved3_MASK               0x00000060
#define EMMC_HOSTIF_FORCE_EVENTS_reserved3_SHIFT              5

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_INDEX_ERR [04:04] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_INDEX_ERR_MASK     0x00000010
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_INDEX_ERR_SHIFT    4
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_INDEX_ERR_DEFAULT  0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_END_ERR [03:03] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_END_ERR_MASK       0x00000008
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_END_ERR_SHIFT      3
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_END_ERR_DEFAULT    0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_CRC_ERR [02:02] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_CRC_ERR_MASK       0x00000004
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_CRC_ERR_SHIFT      2
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_CRC_ERR_DEFAULT    0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_TIMEOUT_ERR [01:01] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_TIMEOUT_ERR_MASK   0x00000002
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_TIMEOUT_ERR_SHIFT  1
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_TIMEOUT_ERR_DEFAULT 0x00000000

/* EMMC_HOSTIF :: FORCE_EVENTS :: FORCE_CMD_NOT_EXEC_ERR [00:00] */
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_NOT_EXEC_ERR_MASK  0x00000001
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_NOT_EXEC_ERR_SHIFT 0
#define EMMC_HOSTIF_FORCE_EVENTS_FORCE_CMD_NOT_EXEC_ERR_DEFAULT 0x00000000

   uint32 emmc_host_adma_err_stat;         /* 0x54 ADMA Error Status Bits                                          */
/***************************************************************************
 *ADMA_ERR_STAT - ADMA Error Status Bits
 ***************************************************************************/
/* EMMC_HOSTIF :: ADMA_ERR_STAT :: reserved0 [31:03] */
#define EMMC_HOSTIF_ADMA_ERR_STAT_reserved0_MASK              0xfffffff8
#define EMMC_HOSTIF_ADMA_ERR_STAT_reserved0_SHIFT             3

/* EMMC_HOSTIF :: ADMA_ERR_STAT :: LENGTH_MATCH_ERR [02:02] */
#define EMMC_HOSTIF_ADMA_ERR_STAT_LENGTH_MATCH_ERR_MASK       0x00000004
#define EMMC_HOSTIF_ADMA_ERR_STAT_LENGTH_MATCH_ERR_SHIFT      2
#define EMMC_HOSTIF_ADMA_ERR_STAT_LENGTH_MATCH_ERR_DEFAULT    0x00000000

/* EMMC_HOSTIF :: ADMA_ERR_STAT :: STATE_ERR [01:00] */
#define EMMC_HOSTIF_ADMA_ERR_STAT_STATE_ERR_MASK              0x00000003
#define EMMC_HOSTIF_ADMA_ERR_STAT_STATE_ERR_SHIFT             0
#define EMMC_HOSTIF_ADMA_ERR_STAT_STATE_ERR_DEFAULT           0x00000000

   uint32 emmc_host_adma_sysaddr_lo;       /* 0x58 ADMA System Address Low Bits                                    */
/***************************************************************************
 *ADMA_SYSADDR_LO - ADMA System Address Low Bits
 ***************************************************************************/
/* EMMC_HOSTIF :: ADMA_SYSADDR_LO :: ADMA_SYSADDR_LO [31:00] */
#define EMMC_HOSTIF_ADMA_SYSADDR_LO_ADMA_SYSADDR_LO_MASK      0xffffffff
#define EMMC_HOSTIF_ADMA_SYSADDR_LO_ADMA_SYSADDR_LO_SHIFT     0
#define EMMC_HOSTIF_ADMA_SYSADDR_LO_ADMA_SYSADDR_LO_DEFAULT   0x00000000

   uint32 emmc_host_adma_sysaddr_hi;       /* 0x5c ADMA System Address High Bits                                   */
/***************************************************************************
 *ADMA_SYSADDR_HI - ADMA System Address High Bits
 ***************************************************************************/
/* EMMC_HOSTIF :: ADMA_SYSADDR_HI :: ADMA_SYSADDR_HI [31:00] */
#define EMMC_HOSTIF_ADMA_SYSADDR_HI_ADMA_SYSADDR_HI_MASK      0xffffffff
#define EMMC_HOSTIF_ADMA_SYSADDR_HI_ADMA_SYSADDR_HI_SHIFT     0
#define EMMC_HOSTIF_ADMA_SYSADDR_HI_ADMA_SYSADDR_HI_DEFAULT   0x00000000

   uint32 emmc_host_preset_init_default;   /* 0x60 Preset Values for init and default speed                        */
/***************************************************************************
 *PRESET_INIT_DEFAULT - Preset Values for init and default speed
 ***************************************************************************/
/* EMMC_HOSTIF :: PRESET_INIT_DEFAULT :: PRESET_DEFAULT_SPEED [31:16] */
#define EMMC_HOSTIF_PRESET_INIT_DEFAULT_PRESET_DEFAULT_SPEED_MASK 0xffff0000
#define EMMC_HOSTIF_PRESET_INIT_DEFAULT_PRESET_DEFAULT_SPEED_SHIFT 16
#define EMMC_HOSTIF_PRESET_INIT_DEFAULT_PRESET_DEFAULT_SPEED_DEFAULT 0x00000002

/* EMMC_HOSTIF :: PRESET_INIT_DEFAULT :: PRESET_INIT [15:00] */
#define EMMC_HOSTIF_PRESET_INIT_DEFAULT_PRESET_INIT_MASK      0x0000ffff
#define EMMC_HOSTIF_PRESET_INIT_DEFAULT_PRESET_INIT_SHIFT     0
#define EMMC_HOSTIF_PRESET_INIT_DEFAULT_PRESET_INIT_DEFAULT   0x00000002

   uint32 emmc_host_preset_high_speed;     /* 0x64 Preset Values for high speed and SDR12                          */
/***************************************************************************
 *PRESET_HIGH_SPEED - Preset Values for high speed and SDR12
 ***************************************************************************/
/* EMMC_HOSTIF :: PRESET_HIGH_SPEED :: PRESET_SDR12 [31:16] */
#define EMMC_HOSTIF_PRESET_HIGH_SPEED_PRESET_SDR12_MASK       0xffff0000
#define EMMC_HOSTIF_PRESET_HIGH_SPEED_PRESET_SDR12_SHIFT      16
#define EMMC_HOSTIF_PRESET_HIGH_SPEED_PRESET_SDR12_DEFAULT    0x00000003

/* EMMC_HOSTIF :: PRESET_HIGH_SPEED :: PRESET_HIGH_SPEED [15:00] */
#define EMMC_HOSTIF_PRESET_HIGH_SPEED_PRESET_HIGH_SPEED_MASK  0x0000ffff
#define EMMC_HOSTIF_PRESET_HIGH_SPEED_PRESET_HIGH_SPEED_SHIFT 0
#define EMMC_HOSTIF_PRESET_HIGH_SPEED_PRESET_HIGH_SPEED_DEFAULT 0x00000001

   uint32 emmc_host_preset_sdr25_50;       /* 0x68 Preset Values for SDR25 and SDR50                               */
/***************************************************************************
 *PRESET_SDR25_50 - Preset Values for SDR25 and SDR50
 ***************************************************************************/
/* EMMC_HOSTIF :: PRESET_SDR25_50 :: PRESET_SDR50 [31:16] */
#define EMMC_HOSTIF_PRESET_SDR25_50_PRESET_SDR50_MASK         0xffff0000
#define EMMC_HOSTIF_PRESET_SDR25_50_PRESET_SDR50_SHIFT        16
#define EMMC_HOSTIF_PRESET_SDR25_50_PRESET_SDR50_DEFAULT      0x00000000

/* EMMC_HOSTIF :: PRESET_SDR25_50 :: PRESET_SDR25 [15:00] */
#define EMMC_HOSTIF_PRESET_SDR25_50_PRESET_SDR25_MASK         0x0000ffff
#define EMMC_HOSTIF_PRESET_SDR25_50_PRESET_SDR25_SHIFT        0
#define EMMC_HOSTIF_PRESET_SDR25_50_PRESET_SDR25_DEFAULT      0x00000002

   uint32 emmc_host_preset_sdr104_ddr50;   /* 0x6c Preset Values for SDR104 and DDR50                              */
/***************************************************************************
 *PRESET_SDR104_DDR50 - Preset Values for SDR104 and DDR50
 ***************************************************************************/
/* EMMC_HOSTIF :: PRESET_SDR104_DDR50 :: PRESET_SDR50 [31:16] */
#define EMMC_HOSTIF_PRESET_SDR104_DDR50_PRESET_SDR50_MASK     0xffff0000
#define EMMC_HOSTIF_PRESET_SDR104_DDR50_PRESET_SDR50_SHIFT    16
#define EMMC_HOSTIF_PRESET_SDR104_DDR50_PRESET_SDR50_DEFAULT  0x00000002

/* EMMC_HOSTIF :: PRESET_SDR104_DDR50 :: PRESET_SDR25 [15:00] */
#define EMMC_HOSTIF_PRESET_SDR104_DDR50_PRESET_SDR25_MASK     0x0000ffff
#define EMMC_HOSTIF_PRESET_SDR104_DDR50_PRESET_SDR25_SHIFT    0
#define EMMC_HOSTIF_PRESET_SDR104_DDR50_PRESET_SDR25_DEFAULT  0x00000000

   uint32 emmc_host_boot_timeout;          /* 0x70 DAT line inactivity timeout on boot                             */
/***************************************************************************
 *BOOT_TIMEOUT - DAT line inactivity timeout on boot
 ***************************************************************************/
/* EMMC_HOSTIF :: BOOT_TIMEOUT :: BOOT_TIMEOUT [31:00] */
#define EMMC_HOSTIF_BOOT_TIMEOUT_BOOT_TIMEOUT_MASK            0xffffffff
#define EMMC_HOSTIF_BOOT_TIMEOUT_BOOT_TIMEOUT_SHIFT           0
#define EMMC_HOSTIF_BOOT_TIMEOUT_BOOT_TIMEOUT_DEFAULT         0x00000000

   uint32 emmc_host_debug_select;          /* 0x74 Debug probe output selection                                    */
/***************************************************************************
 *DEBUG_SELECT - Debug probe output selection
 ***************************************************************************/
/* EMMC_HOSTIF :: DEBUG_SELECT :: reserved0 [31:01] */
#define EMMC_HOSTIF_DEBUG_SELECT_reserved0_MASK               0xfffffffe
#define EMMC_HOSTIF_DEBUG_SELECT_reserved0_SHIFT              1

/* EMMC_HOSTIF :: DEBUG_SELECT :: DEBUG_SEL [00:00] */
#define EMMC_HOSTIF_DEBUG_SELECT_DEBUG_SEL_MASK               0x00000001
#define EMMC_HOSTIF_DEBUG_SELECT_DEBUG_SEL_SHIFT              0
#define EMMC_HOSTIF_DEBUG_SELECT_DEBUG_SEL_DEFAULT            0x00000000

   uint32 unused1[26];                     /* 0x78 - 0xdc                                                          */
   
   uint32 emmc_host_shared_bus_ctrl;       /* 0xe0 shared bus control                                              */
/***************************************************************************
 *SHARED_BUS_CTRL - shared bus control
 ***************************************************************************/
/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: reserved0 [31:31] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved0_MASK            0x80000000
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved0_SHIFT           31

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: BACK_END_PWR_CTRL [30:24] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_BACK_END_PWR_CTRL_MASK    0x7f000000
#define EMMC_HOSTIF_SHARED_BUS_CTRL_BACK_END_PWR_CTRL_SHIFT   24
#define EMMC_HOSTIF_SHARED_BUS_CTRL_BACK_END_PWR_CTRL_DEFAULT 0x00000000

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: reserved1 [23:23] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved1_MASK            0x00800000
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved1_SHIFT           23

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: INT_PIN_SEL [22:20] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_INT_PIN_SEL_MASK          0x00700000
#define EMMC_HOSTIF_SHARED_BUS_CTRL_INT_PIN_SEL_SHIFT         20
#define EMMC_HOSTIF_SHARED_BUS_CTRL_INT_PIN_SEL_DEFAULT       0x00000000

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: reserved2 [19:19] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved2_MASK            0x00080000
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved2_SHIFT           19

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: CLK_PIN_SEL [18:16] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_CLK_PIN_SEL_MASK          0x00070000
#define EMMC_HOSTIF_SHARED_BUS_CTRL_CLK_PIN_SEL_SHIFT         16
#define EMMC_HOSTIF_SHARED_BUS_CTRL_CLK_PIN_SEL_DEFAULT       0x00000000

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: reserved3 [15:15] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved3_MASK            0x00008000
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved3_SHIFT           15

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: BUS_WIDTH_PRESET [14:08] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_BUS_WIDTH_PRESET_MASK     0x00007f00
#define EMMC_HOSTIF_SHARED_BUS_CTRL_BUS_WIDTH_PRESET_SHIFT    8
#define EMMC_HOSTIF_SHARED_BUS_CTRL_BUS_WIDTH_PRESET_DEFAULT  0x00000000

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: reserved4 [07:06] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved4_MASK            0x000000c0
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved4_SHIFT           6

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: NUM_INT_PINS [05:04] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_NUM_INT_PINS_MASK         0x00000030
#define EMMC_HOSTIF_SHARED_BUS_CTRL_NUM_INT_PINS_SHIFT        4
#define EMMC_HOSTIF_SHARED_BUS_CTRL_NUM_INT_PINS_DEFAULT      0x00000000

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: reserved5 [03:03] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved5_MASK            0x00000008
#define EMMC_HOSTIF_SHARED_BUS_CTRL_reserved5_SHIFT           3

/* EMMC_HOSTIF :: SHARED_BUS_CTRL :: NUM_CLK_PINS [02:00] */
#define EMMC_HOSTIF_SHARED_BUS_CTRL_NUM_CLK_PINS_MASK         0x00000007
#define EMMC_HOSTIF_SHARED_BUS_CTRL_NUM_CLK_PINS_SHIFT        0
#define EMMC_HOSTIF_SHARED_BUS_CTRL_NUM_CLK_PINS_DEFAULT      0x00000000

   uint32 unused2[3];                      /* 0xe4 - 0xec                                                          */
   
   uint32 emmc_host_spi_interrupt;         /* 0xf0 SPI Interrupt support                                           */
/***************************************************************************
 *SPI_INTERRUPT - SPI Interrupt support
 ***************************************************************************/
/* EMMC_HOSTIF :: SPI_INTERRUPT :: reserved0 [31:08] */
#define EMMC_HOSTIF_SPI_INTERRUPT_reserved0_MASK              0xffffff00
#define EMMC_HOSTIF_SPI_INTERRUPT_reserved0_SHIFT             8

/* EMMC_HOSTIF :: SPI_INTERRUPT :: SPI_INT [07:00] */
#define EMMC_HOSTIF_SPI_INTERRUPT_SPI_INT_MASK                0x000000ff
#define EMMC_HOSTIF_SPI_INTERRUPT_SPI_INT_SHIFT               0
#define EMMC_HOSTIF_SPI_INTERRUPT_SPI_INT_DEFAULT             0x00000000

   uint32 unused3[2];                      /* 0xf4 - 0xf8                                                          */
   
   uint32 emmc_host_version_status;        /* 0xfc Controller Version and Slot Status                              */   
/***************************************************************************
 *VERSION_STATUS - Controller Version and Slot Status
 ***************************************************************************/
/* EMMC_HOSTIF :: VERSION_STATUS :: VENDOR_VERSION [31:24] */
#define EMMC_HOSTIF_VERSION_STATUS_VENDOR_VERSION_MASK        0xff000000
#define EMMC_HOSTIF_VERSION_STATUS_VENDOR_VERSION_SHIFT       24
#define EMMC_HOSTIF_VERSION_STATUS_VENDOR_VERSION_DEFAULT     0x000000a9

/* EMMC_HOSTIF :: VERSION_STATUS :: CONTROLLER_VERSION [23:16] */
#define EMMC_HOSTIF_VERSION_STATUS_CONTROLLER_VERSION_MASK    0x00ff0000
#define EMMC_HOSTIF_VERSION_STATUS_CONTROLLER_VERSION_SHIFT   16
#define EMMC_HOSTIF_VERSION_STATUS_CONTROLLER_VERSION_DEFAULT 0x00000002

/* EMMC_HOSTIF :: VERSION_STATUS :: reserved0 [15:08] */
#define EMMC_HOSTIF_VERSION_STATUS_reserved0_MASK             0x0000ff00
#define EMMC_HOSTIF_VERSION_STATUS_reserved0_SHIFT            8

/* EMMC_HOSTIF :: VERSION_STATUS :: SLOT_INTS [07:00] */
#define EMMC_HOSTIF_VERSION_STATUS_SLOT_INTS_MASK             0x000000ff
#define EMMC_HOSTIF_VERSION_STATUS_SLOT_INTS_SHIFT            0
#define EMMC_HOSTIF_VERSION_STATUS_SLOT_INTS_DEFAULT          0x00000000

} EmmcHostIfRegs;
#define EMMC_HOSTIF  ((volatile EmmcHostIfRegs *const) EMMC_HOSTIF_BASE)
 
typedef struct EmmcTopCfgRegs {
   uint32 emmc_top_cfg_sdio_emmc_ctrl1;    /* 0x00 SDIO EMMC Control Register                        */
/***************************************************************************
 *SDIO_EMMC_CTRL1 - SDIO EMMC Control Register
 ***************************************************************************/
/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: SDCD_N_TEST_SEL_EN [31:31] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SDCD_N_TEST_SEL_EN_MASK    0x80000000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SDCD_N_TEST_SEL_EN_SHIFT   31
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SDCD_N_TEST_SEL_EN_DEFAULT 0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: SDCD_N_TEST_LEV [30:30] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SDCD_N_TEST_LEV_MASK       0x40000000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SDCD_N_TEST_LEV_SHIFT      30
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SDCD_N_TEST_LEV_DEFAULT    0x00000001

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: reserved0 [29:29] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_reserved0_MASK             0x20000000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_reserved0_SHIFT            29

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: RETUNING_REQ [28:28] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_RETUNING_REQ_MASK          0x10000000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_RETUNING_REQ_SHIFT         28
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_RETUNING_REQ_DEFAULT       0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: DDR_TAP_DELAY [27:24] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DDR_TAP_DELAY_MASK         0x0f000000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DDR_TAP_DELAY_SHIFT        24
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DDR_TAP_DELAY_DEFAULT      0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: DELAY_CTRL [23:21] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DELAY_CTRL_MASK            0x00e00000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DELAY_CTRL_SHIFT           21
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DELAY_CTRL_DEFAULT         0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: HREADY_IDLE_ENA [20:20] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_HREADY_IDLE_ENA_MASK       0x00100000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_HREADY_IDLE_ENA_SHIFT      20
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_HREADY_IDLE_ENA_DEFAULT    0x00000001

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: HREADY_IDLE_PULSE [19:19] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_HREADY_IDLE_PULSE_MASK     0x00080000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_HREADY_IDLE_PULSE_SHIFT    19
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_HREADY_IDLE_PULSE_DEFAULT  0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: DATA_PENDING [18:18] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DATA_PENDING_MASK          0x00040000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DATA_PENDING_SHIFT         18
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_DATA_PENDING_DEFAULT       0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: WR_FLUSH [17:17] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_WR_FLUSH_MASK              0x00020000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_WR_FLUSH_SHIFT             17
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_WR_FLUSH_DEFAULT           0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: MF_NUM_WR [16:16] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_MF_NUM_WR_MASK             0x00010000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_MF_NUM_WR_SHIFT            16
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_MF_NUM_WR_DEFAULT          0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: WORD_ABO [15:15] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_WORD_ABO_MASK              0x00008000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_WORD_ABO_SHIFT             15
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_WORD_ABO_DEFAULT           0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: FRAME_NBO [14:14] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_FRAME_NBO_MASK             0x00004000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_FRAME_NBO_SHIFT            14
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_FRAME_NBO_DEFAULT          0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: FRAME_NHW [13:13] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_FRAME_NHW_MASK             0x00002000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_FRAME_NHW_SHIFT            13
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_FRAME_NHW_DEFAULT          0x00000001

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: BUFFER_ABO [12:12] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_BUFFER_ABO_MASK            0x00001000
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_BUFFER_ABO_SHIFT           12
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_BUFFER_ABO_DEFAULT         0x00000001

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: SCB_BUF_ACC [11:11] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_BUF_ACC_MASK           0x00000800
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_BUF_ACC_SHIFT          11
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_BUF_ACC_DEFAULT        0x00000001

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: SCB_SEQ_EN [10:10] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_SEQ_EN_MASK            0x00000400
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_SEQ_EN_SHIFT           10
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_SEQ_EN_DEFAULT         0x00000001

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: SCB_RD_THRESH [09:05] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_RD_THRESH_MASK         0x000003e0
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_RD_THRESH_SHIFT        5
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_RD_THRESH_DEFAULT      0x00000002

/* SDIO_0_CFG :: SDIO_EMMC_CTRL1 :: SCB_SIZE [04:00] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_SIZE_MASK              0x0000001f
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_SIZE_SHIFT             0
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL1_SCB_SIZE_DEFAULT           0x00000004

   uint32 emmc_top_cfg_sdio_emmc_ctrl2;    /* 0x04 SDIO EMMC Control Register                        */
/***************************************************************************
 *SDIO_EMMC_CTRL2 - SDIO EMMC Control Register
 ***************************************************************************/
/* SDIO_0_CFG :: SDIO_EMMC_CTRL2 :: reserved0 [31:08] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_reserved0_MASK             0xffffff00
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_reserved0_SHIFT            8

/* SDIO_0_CFG :: SDIO_EMMC_CTRL2 :: REG_ADDR_MAP_BYTE [07:06] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_ADDR_MAP_BYTE_MASK     0x000000c0
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_ADDR_MAP_BYTE_SHIFT    6
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_ADDR_MAP_BYTE_DEFAULT  0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL2 :: reserved1 [05:05] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_reserved1_MASK             0x00000020
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_reserved1_SHIFT            5

/* SDIO_0_CFG :: SDIO_EMMC_CTRL2 :: REG_ADDR_MAP_HW [04:04] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_ADDR_MAP_HW_MASK       0x00000010
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_ADDR_MAP_HW_SHIFT      4
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_ADDR_MAP_HW_DEFAULT    0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL2 :: REG_DATA_SWAP_RD [03:02] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_DATA_SWAP_RD_MASK      0x0000000c
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_DATA_SWAP_RD_SHIFT     2
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_DATA_SWAP_RD_DEFAULT   0x00000000

/* SDIO_0_CFG :: SDIO_EMMC_CTRL2 :: REG_DATA_SWAP_WR [01:00] */
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_DATA_SWAP_WR_MASK      0x00000003
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_DATA_SWAP_WR_SHIFT     0
#define EMMC_TOP_CFG_SDIO_EMMC_CTRL2_REG_DATA_SWAP_WR_DEFAULT   0x00000000

   uint32 emmc_top_cfg_tp_out_sel;         /* 0x08 SDIO TP_OUT Control Register                      */
/***************************************************************************
 *TP_OUT_SEL - SDIO TP_OUT Control Register
 ***************************************************************************/
/* SDIO_0_CFG :: TP_OUT_SEL :: reserved0 [31:01] */
#define EMMC_TOP_CFG_TP_OUT_SEL_reserved0_MASK                  0xfffffffe
#define EMMC_TOP_CFG_TP_OUT_SEL_reserved0_SHIFT                 1

/* SDIO_0_CFG :: TP_OUT_SEL :: TP_OUT_SELECT [00:00] */
#define EMMC_TOP_CFG_TP_OUT_SEL_TP_OUT_SELECT_MASK              0x00000001
#define EMMC_TOP_CFG_TP_OUT_SEL_TP_OUT_SELECT_SHIFT             0
#define EMMC_TOP_CFG_TP_OUT_SEL_TP_OUT_SELECT_DEFAULT           0x00000000

   uint32 emmc_top_cfg_cap_reg_override;   /* 0x0c SDIO CAPABILITIES override Register               */
   
   uint32 emmc_top_cfg_cap_reg0;           /* 0x10 SDIO CAPABILITIES override Register[31:0]         */
/***************************************************************************
 *CAP_REG0 - SDIO CAPABILITIES override Register
 ***************************************************************************/
/* SDIO_0_CFG :: CAP_REG0 :: DDR50_SUPPORT [31:31] */
#define EMMC_TOP_CFG_CAP_REG0_DDR50_SUPPORT_MASK                0x80000000
#define EMMC_TOP_CFG_CAP_REG0_DDR50_SUPPORT_SHIFT               31
#define EMMC_TOP_CFG_CAP_REG0_DDR50_SUPPORT_DEFAULT             0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: SD104_SUPPORT [30:30] */
#define EMMC_TOP_CFG_CAP_REG0_SD104_SUPPORT_MASK                0x40000000
#define EMMC_TOP_CFG_CAP_REG0_SD104_SUPPORT_SHIFT               30
#define EMMC_TOP_CFG_CAP_REG0_SD104_SUPPORT_DEFAULT             0x00000000

/* SDIO_0_CFG :: CAP_REG0 :: SDR50 [29:29] */
#define EMMC_TOP_CFG_CAP_REG0_SDR50_MASK                        0x20000000
#define EMMC_TOP_CFG_CAP_REG0_SDR50_SHIFT                       29
#define EMMC_TOP_CFG_CAP_REG0_SDR50_DEFAULT                     0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: SLOT_TYPE [28:27] */
#define EMMC_TOP_CFG_CAP_REG0_SLOT_TYPE_MASK                    0x18000000
#define EMMC_TOP_CFG_CAP_REG0_SLOT_TYPE_SHIFT                   27
#define EMMC_TOP_CFG_CAP_REG0_SLOT_TYPE_DEFAULT                 0x00000002

/* SDIO_0_CFG :: CAP_REG0 :: ASYNCH_INT_SUPPORT [26:26] */
#define EMMC_TOP_CFG_CAP_REG0_ASYNCH_INT_SUPPORT_MASK           0x04000000
#define EMMC_TOP_CFG_CAP_REG0_ASYNCH_INT_SUPPORT_SHIFT          26
#define EMMC_TOP_CFG_CAP_REG0_ASYNCH_INT_SUPPORT_DEFAULT        0x00000000

/* SDIO_0_CFG :: CAP_REG0 :: 64B_SYS_BUS_SUPPORT [25:25] */
#define EMMC_TOP_CFG_CAP_REG0_64B_SYS_BUS_SUPPORT_MASK          0x02000000
#define EMMC_TOP_CFG_CAP_REG0_64B_SYS_BUS_SUPPORT_SHIFT         25
#define EMMC_TOP_CFG_CAP_REG0_64B_SYS_BUS_SUPPORT_DEFAULT       0x00000000

/* SDIO_0_CFG :: CAP_REG0 :: 1_8V_SUPPORT [24:24] */
#define EMMC_TOP_CFG_CAP_REG0_1_8V_SUPPORT_MASK                 0x01000000
#define EMMC_TOP_CFG_CAP_REG0_1_8V_SUPPORT_SHIFT                24
#define EMMC_TOP_CFG_CAP_REG0_1_8V_SUPPORT_DEFAULT              0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: 3_0V_SUPPORT [23:23] */
#define EMMC_TOP_CFG_CAP_REG0_3_0V_SUPPORT_MASK                 0x00800000
#define EMMC_TOP_CFG_CAP_REG0_3_0V_SUPPORT_SHIFT                23
#define EMMC_TOP_CFG_CAP_REG0_3_0V_SUPPORT_DEFAULT              0x00000000

/* SDIO_0_CFG :: CAP_REG0 :: 3_3V_SUPPORT [22:22] */
#define EMMC_TOP_CFG_CAP_REG0_3_3V_SUPPORT_MASK                 0x00400000
#define EMMC_TOP_CFG_CAP_REG0_3_3V_SUPPORT_SHIFT                22
#define EMMC_TOP_CFG_CAP_REG0_3_3V_SUPPORT_DEFAULT              0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: SUSP_RES_SUPPORT [21:21] */
#define EMMC_TOP_CFG_CAP_REG0_SUSP_RES_SUPPORT_MASK             0x00200000
#define EMMC_TOP_CFG_CAP_REG0_SUSP_RES_SUPPORT_SHIFT            21
#define EMMC_TOP_CFG_CAP_REG0_SUSP_RES_SUPPORT_DEFAULT          0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: SDMA_SUPPORT [20:20] */
#define EMMC_TOP_CFG_CAP_REG0_SDMA_SUPPORT_MASK                 0x00100000
#define EMMC_TOP_CFG_CAP_REG0_SDMA_SUPPORT_SHIFT                20
#define EMMC_TOP_CFG_CAP_REG0_SDMA_SUPPORT_DEFAULT              0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: HIGH_SPEED_SUPPORT [19:19] */
#define EMMC_TOP_CFG_CAP_REG0_HIGH_SPEED_SUPPORT_MASK           0x00080000
#define EMMC_TOP_CFG_CAP_REG0_HIGH_SPEED_SUPPORT_SHIFT          19
#define EMMC_TOP_CFG_CAP_REG0_HIGH_SPEED_SUPPORT_DEFAULT        0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: ADMA2_SUPPORT [18:18] */
#define EMMC_TOP_CFG_CAP_REG0_ADMA2_SUPPORT_MASK                0x00040000
#define EMMC_TOP_CFG_CAP_REG0_ADMA2_SUPPORT_SHIFT               18
#define EMMC_TOP_CFG_CAP_REG0_ADMA2_SUPPORT_DEFAULT             0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: EXTENDED_MEDIA_SUPPORT [17:17] */
#define EMMC_TOP_CFG_CAP_REG0_EXTENDED_MEDIA_SUPPORT_MASK       0x00020000
#define EMMC_TOP_CFG_CAP_REG0_EXTENDED_MEDIA_SUPPORT_SHIFT      17
#define EMMC_TOP_CFG_CAP_REG0_EXTENDED_MEDIA_SUPPORT_DEFAULT    0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: MAX_BL [16:15] */
#define EMMC_TOP_CFG_CAP_REG0_MAX_BL_MASK                       0x00018000
#define EMMC_TOP_CFG_CAP_REG0_MAX_BL_SHIFT                      15
#define EMMC_TOP_CFG_CAP_REG0_MAX_BL_DEFAULT                    0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: BASE_FREQ [14:07] */
#define EMMC_TOP_CFG_CAP_REG0_BASE_FREQ_MASK                    0x00007f80
#define EMMC_TOP_CFG_CAP_REG0_BASE_FREQ_SHIFT                   7
#define EMMC_TOP_CFG_CAP_REG0_BASE_FREQ_DEFAULT                 0x00000064

/* SDIO_0_CFG :: CAP_REG0 :: TIMEOUT_CLK_UNIT [06:06] */
#define EMMC_TOP_CFG_CAP_REG0_TIMEOUT_CLK_UNIT_MASK             0x00000040
#define EMMC_TOP_CFG_CAP_REG0_TIMEOUT_CLK_UNIT_SHIFT            6
#define EMMC_TOP_CFG_CAP_REG0_TIMEOUT_CLK_UNIT_DEFAULT          0x00000001

/* SDIO_0_CFG :: CAP_REG0 :: TIMEOUT_FREQ [05:00] */
#define EMMC_TOP_CFG_CAP_REG0_TIMEOUT_FREQ_MASK                 0x0000003f
#define EMMC_TOP_CFG_CAP_REG0_TIMEOUT_FREQ_SHIFT                0
#define EMMC_TOP_CFG_CAP_REG0_TIMEOUT_FREQ_DEFAULT              0x00000032

   uint32 emmc_top_cfg_cap_reg1;           /* 0x14 SDIO CAPABILITIES override Register[63:32]        */
/***************************************************************************
 *CAP_REG1 - SDIO CAPABILITIES override Register
 ***************************************************************************/
/* SDIO_0_CFG :: CAP_REG1 :: CAP_REG_OVERRIDE [31:31] */
#define EMMC_TOP_CFG_CAP_REG1_CAP_REG_OVERRIDE_MASK             0x80000000
#define EMMC_TOP_CFG_CAP_REG1_CAP_REG_OVERRIDE_SHIFT            31
#define EMMC_TOP_CFG_CAP_REG1_CAP_REG_OVERRIDE_DEFAULT          0x00000000

/* SDIO_0_CFG :: CAP_REG1 :: reserved0 [30:21] */
#define EMMC_TOP_CFG_CAP_REG1_reserved0_MASK                    0x7fe00000
#define EMMC_TOP_CFG_CAP_REG1_reserved0_SHIFT                   21

/* SDIO_0_CFG :: CAP_REG1 :: CAP_1_stuff [20:20] */
#define EMMC_TOP_CFG_CAP_REG1_CAP_1_stuff_MASK                  0x00100000
#define EMMC_TOP_CFG_CAP_REG1_CAP_1_stuff_SHIFT                 20
#define EMMC_TOP_CFG_CAP_REG1_CAP_1_stuff_DEFAULT               0x00000000

/* SDIO_0_CFG :: CAP_REG1 :: SPI_BLK_MODE [19:19] */
#define EMMC_TOP_CFG_CAP_REG1_SPI_BLK_MODE_MASK                 0x00080000
#define EMMC_TOP_CFG_CAP_REG1_SPI_BLK_MODE_SHIFT                19
#define EMMC_TOP_CFG_CAP_REG1_SPI_BLK_MODE_DEFAULT              0x00000000

/* SDIO_0_CFG :: CAP_REG1 :: SPI_MODE [18:18] */
#define EMMC_TOP_CFG_CAP_REG1_SPI_MODE_MASK                     0x00040000
#define EMMC_TOP_CFG_CAP_REG1_SPI_MODE_SHIFT                    18
#define EMMC_TOP_CFG_CAP_REG1_SPI_MODE_DEFAULT                  0x00000001

/* SDIO_0_CFG :: CAP_REG1 :: CLK_MULT [17:10] */
#define EMMC_TOP_CFG_CAP_REG1_CLK_MULT_MASK                     0x0003fc00
#define EMMC_TOP_CFG_CAP_REG1_CLK_MULT_SHIFT                    10
#define EMMC_TOP_CFG_CAP_REG1_CLK_MULT_DEFAULT                  0x00000000

/* SDIO_0_CFG :: CAP_REG1 :: RETUNING_MODES [09:08] */
#define EMMC_TOP_CFG_CAP_REG1_RETUNING_MODES_MASK               0x00000300
#define EMMC_TOP_CFG_CAP_REG1_RETUNING_MODES_SHIFT              8
#define EMMC_TOP_CFG_CAP_REG1_RETUNING_MODES_DEFAULT            0x00000002

/* SDIO_0_CFG :: CAP_REG1 :: USE_TUNING [07:07] */
#define EMMC_TOP_CFG_CAP_REG1_USE_TUNING_MASK                   0x00000080
#define EMMC_TOP_CFG_CAP_REG1_USE_TUNING_SHIFT                  7
#define EMMC_TOP_CFG_CAP_REG1_USE_TUNING_DEFAULT                0x00000001

/* SDIO_0_CFG :: CAP_REG1 :: RETUNING_TIMER [06:03] */
#define EMMC_TOP_CFG_CAP_REG1_RETUNING_TIMER_MASK               0x00000078
#define EMMC_TOP_CFG_CAP_REG1_RETUNING_TIMER_SHIFT              3
#define EMMC_TOP_CFG_CAP_REG1_RETUNING_TIMER_DEFAULT            0x0000000a

/* SDIO_0_CFG :: CAP_REG1 :: Driver_D_SUPPORT [02:02] */
#define EMMC_TOP_CFG_CAP_REG1_Driver_D_SUPPORT_MASK             0x00000004
#define EMMC_TOP_CFG_CAP_REG1_Driver_D_SUPPORT_SHIFT            2
#define EMMC_TOP_CFG_CAP_REG1_Driver_D_SUPPORT_DEFAULT          0x00000000

/* SDIO_0_CFG :: CAP_REG1 :: Driver_C_SUPPORT [01:01] */
#define EMMC_TOP_CFG_CAP_REG1_Driver_C_SUPPORT_MASK             0x00000002
#define EMMC_TOP_CFG_CAP_REG1_Driver_C_SUPPORT_SHIFT            1
#define EMMC_TOP_CFG_CAP_REG1_Driver_C_SUPPORT_DEFAULT          0x00000000

/* SDIO_0_CFG :: CAP_REG1 :: Driver_A_SUPPORT [00:00] */
#define EMMC_TOP_CFG_CAP_REG1_Driver_A_SUPPORT_MASK             0x00000001
#define EMMC_TOP_CFG_CAP_REG1_Driver_A_SUPPORT_SHIFT            0
#define EMMC_TOP_CFG_CAP_REG1_Driver_A_SUPPORT_DEFAULT          0x00000000

   uint32 emmc_top_cfg_preset1;            /* 0x18 SDIO PRESET_INIT/PRESET_DS override Register      */
/***************************************************************************
 *PRESET1 - SDIO CAPABILITIES override Register
 ***************************************************************************/
/* SDIO_0_CFG :: PRESET1 :: PRESET1_OVERRIDE [31:31] */
#define EMMC_TOP_CFG_PRESET1_PRESET1_OVERRIDE_MASK              0x80000000
#define EMMC_TOP_CFG_PRESET1_PRESET1_OVERRIDE_SHIFT             31
#define EMMC_TOP_CFG_PRESET1_PRESET1_OVERRIDE_DEFAULT           0x00000000

/* SDIO_0_CFG :: PRESET1 :: reserved0 [30:29] */
#define EMMC_TOP_CFG_PRESET1_reserved0_MASK                     0x60000000
#define EMMC_TOP_CFG_PRESET1_reserved0_SHIFT                    29

/* SDIO_0_CFG :: PRESET1 :: PRESET100 [28:16] */
#define EMMC_TOP_CFG_PRESET1_PRESET100_MASK                     0x1fff0000
#define EMMC_TOP_CFG_PRESET1_PRESET100_SHIFT                    16
#define EMMC_TOP_CFG_PRESET1_PRESET100_DEFAULT                  0x00000000

/* SDIO_0_CFG :: PRESET1 :: reserved1 [15:13] */
#define EMMC_TOP_CFG_PRESET1_reserved1_MASK                     0x0000e000
#define EMMC_TOP_CFG_PRESET1_reserved1_SHIFT                    13

/* SDIO_0_CFG :: PRESET1 :: PRESET50 [12:00] */
#define EMMC_TOP_CFG_PRESET1_PRESET50_MASK                      0x00001fff
#define EMMC_TOP_CFG_PRESET1_PRESET50_SHIFT                     0
#define EMMC_TOP_CFG_PRESET1_PRESET50_DEFAULT                   0x00000001

   uint32 emmc_top_cfg_preset2;            /* 0x1c SDIO PRESET_HS/PRESET_SDR12 override Register     */
/***************************************************************************
 *PRESET2 - SDIO CAPABILITIES override Register
 ***************************************************************************/
/* SDIO_0_CFG :: PRESET2 :: PRESET2_OVERRIDE [31:31] */
#define EMMC_TOP_CFG_PRESET2_PRESET2_OVERRIDE_MASK              0x80000000
#define EMMC_TOP_CFG_PRESET2_PRESET2_OVERRIDE_SHIFT             31
#define EMMC_TOP_CFG_PRESET2_PRESET2_OVERRIDE_DEFAULT           0x00000000

/* SDIO_0_CFG :: PRESET2 :: reserved0 [30:29] */
#define EMMC_TOP_CFG_PRESET2_reserved0_MASK                     0x60000000
#define EMMC_TOP_CFG_PRESET2_reserved0_SHIFT                    29

/* SDIO_0_CFG :: PRESET2 :: PRESET25 [28:16] */
#define EMMC_TOP_CFG_PRESET2_PRESET25_MASK                      0x1fff0000
#define EMMC_TOP_CFG_PRESET2_PRESET25_SHIFT                     16
#define EMMC_TOP_CFG_PRESET2_PRESET25_DEFAULT                   0x00000002

/* SDIO_0_CFG :: PRESET2 :: reserved1 [15:13] */
#define EMMC_TOP_CFG_PRESET2_reserved1_MASK                     0x0000e000
#define EMMC_TOP_CFG_PRESET2_reserved1_SHIFT                    13

/* SDIO_0_CFG :: PRESET2 :: PRESET12P5 [12:00] */
#define EMMC_TOP_CFG_PRESET2_PRESET12P5_MASK                    0x00001fff
#define EMMC_TOP_CFG_PRESET2_PRESET12P5_SHIFT                   0
#define EMMC_TOP_CFG_PRESET2_PRESET12P5_DEFAULT                 0x00000003

   uint32 emmc_top_cfg_preset3;            /* 0x20 SDIO PRESET_SDR25/PRESET_SDR50 override Register  */
   
   uint32 emmc_top_cfg_preset4;            /* 0x24 SDIO PRESET_SDR104/PRESET_DDR50 override Register */
   
   uint32 emmc_top_cfg_sd_clock_delay;     /* 0x28 SDIO Clock delay register                         */
/***************************************************************************
 *SD_CLOCK_DELAY - SDIO Clock delay register
 ***************************************************************************/
/* SDIO_0_CFG :: SD_CLOCK_DELAY :: reserved0 [31:31] */
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_reserved0_MASK              0x80000000
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_reserved0_SHIFT             31

/* SDIO_0_CFG :: SD_CLOCK_DELAY :: CLOCK_DELAY_OVERRIDE [30:30] */
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_CLOCK_DELAY_OVERRIDE_MASK   0x40000000
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_CLOCK_DELAY_OVERRIDE_SHIFT  30
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_CLOCK_DELAY_OVERRIDE_DEFAULT 0x00000001

/* SDIO_0_CFG :: SD_CLOCK_DELAY :: INPUT_CLOCK_SEL [29:29] */
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INPUT_CLOCK_SEL_MASK        0x20000000
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INPUT_CLOCK_SEL_SHIFT       29
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INPUT_CLOCK_SEL_DEFAULT     0x00000000

/* SDIO_0_CFG :: SD_CLOCK_DELAY :: reserved1 [28:12] */
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_reserved1_MASK              0x1ffff000
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_reserved1_SHIFT             12

/* SDIO_0_CFG :: SD_CLOCK_DELAY :: OUTPUT_CLOCK_DELAY [11:08] */
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_OUTPUT_CLOCK_DELAY_MASK     0x00000f00
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_OUTPUT_CLOCK_DELAY_SHIFT    8
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_OUTPUT_CLOCK_DELAY_DEFAULT  0x00000000

/* SDIO_0_CFG :: SD_CLOCK_DELAY :: INTERNAL_CLOCK_DELAY [07:04] */
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INTERNAL_CLOCK_DELAY_MASK   0x000000f0
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INTERNAL_CLOCK_DELAY_SHIFT  4
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INTERNAL_CLOCK_DELAY_DEFAULT 0x0000000f

/* SDIO_0_CFG :: SD_CLOCK_DELAY :: INPUT_CLOCK_DELAY [03:00] */
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INPUT_CLOCK_DELAY_MASK      0x0000000f
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INPUT_CLOCK_DELAY_SHIFT     0
#define EMMC_TOP_CFG_SD_CLOCK_DELAY_INPUT_CLOCK_DELAY_DEFAULT   0x0000000f

   uint32 emmc_top_cfg_sd_pad_drv;         /* 0x2c SDIO Clock delay register                         */
/***************************************************************************
 *SD_PAD_DRV - SDIO Clock delay register
 ***************************************************************************/
/* SDIO_0_CFG :: SD_PAD_DRV :: OVERRIDE_EN [31:31] */
#define EMMC_TOP_CFG_SD_PAD_DRV_OVERRIDE_EN_MASK                0x80000000
#define EMMC_TOP_CFG_SD_PAD_DRV_OVERRIDE_EN_SHIFT               31
#define EMMC_TOP_CFG_SD_PAD_DRV_OVERRIDE_EN_DEFAULT             0x00000000

/* SDIO_0_CFG :: SD_PAD_DRV :: reserved0 [30:23] */
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved0_MASK                  0x7f800000
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved0_SHIFT                 23

/* SDIO_0_CFG :: SD_PAD_DRV :: CLK_VAL [22:20] */
#define EMMC_TOP_CFG_SD_PAD_DRV_CLK_VAL_MASK                    0x00700000
#define EMMC_TOP_CFG_SD_PAD_DRV_CLK_VAL_SHIFT                   20
#define EMMC_TOP_CFG_SD_PAD_DRV_CLK_VAL_DEFAULT                 0x00000005

/* SDIO_0_CFG :: SD_PAD_DRV :: reserved1 [19:19] */
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved1_MASK                  0x00080000
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved1_SHIFT                 19

/* SDIO_0_CFG :: SD_PAD_DRV :: CMD_VAL [18:16] */
#define EMMC_TOP_CFG_SD_PAD_DRV_CMD_VAL_MASK                    0x00070000
#define EMMC_TOP_CFG_SD_PAD_DRV_CMD_VAL_SHIFT                   16
#define EMMC_TOP_CFG_SD_PAD_DRV_CMD_VAL_DEFAULT                 0x00000005

/* SDIO_0_CFG :: SD_PAD_DRV :: reserved2 [15:15] */
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved2_MASK                  0x00008000
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved2_SHIFT                 15

/* SDIO_0_CFG :: SD_PAD_DRV :: DAT3_VAL [14:12] */
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT3_VAL_MASK                   0x00007000
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT3_VAL_SHIFT                  12
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT3_VAL_DEFAULT                0x00000005

/* SDIO_0_CFG :: SD_PAD_DRV :: reserved3 [11:11] */
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved3_MASK                  0x00000800
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved3_SHIFT                 11

/* SDIO_0_CFG :: SD_PAD_DRV :: DAT2_VAL [10:08] */
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT2_VAL_MASK                   0x00000700
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT2_VAL_SHIFT                  8
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT2_VAL_DEFAULT                0x00000005

/* SDIO_0_CFG :: SD_PAD_DRV :: reserved4 [07:07] */
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved4_MASK                  0x00000080
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved4_SHIFT                 7

/* SDIO_0_CFG :: SD_PAD_DRV :: DAT1_VAL [06:04] */
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT1_VAL_MASK                   0x00000070
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT1_VAL_SHIFT                  4
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT1_VAL_DEFAULT                0x00000005

/* SDIO_0_CFG :: SD_PAD_DRV :: reserved5 [03:03] */
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved5_MASK                  0x00000008
#define EMMC_TOP_CFG_SD_PAD_DRV_reserved5_SHIFT                 3

/* SDIO_0_CFG :: SD_PAD_DRV :: DAT0_VAL [02:00] */
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT0_VAL_MASK                   0x00000007
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT0_VAL_SHIFT                  0
#define EMMC_TOP_CFG_SD_PAD_DRV_DAT0_VAL_DEFAULT                0x00000005

   uint32 emmc_top_cfg_ip_dly;             /* 0x30 SDIO Host input delay register                    */ 
/***************************************************************************
 *IP_DLY - SDIO Host input delay register
 ***************************************************************************/
/* SDIO_0_CFG :: IP_DLY :: IP_TAP_EN [31:31] */
#define EMMC_TOP_CFG_IP_DLY_IP_TAP_EN_MASK                      0x80000000
#define EMMC_TOP_CFG_IP_DLY_IP_TAP_EN_SHIFT                     31
#define EMMC_TOP_CFG_IP_DLY_IP_TAP_EN_DEFAULT                   0x00000000

/* SDIO_0_CFG :: IP_DLY :: FORCE_USE_IP_TUNE_CLK [30:30] */
#define EMMC_TOP_CFG_IP_DLY_FORCE_USE_IP_TUNE_CLK_MASK          0x40000000
#define EMMC_TOP_CFG_IP_DLY_FORCE_USE_IP_TUNE_CLK_SHIFT         30
#define EMMC_TOP_CFG_IP_DLY_FORCE_USE_IP_TUNE_CLK_DEFAULT       0x00000000

/* SDIO_0_CFG :: IP_DLY :: reserved0 [29:18] */
#define EMMC_TOP_CFG_IP_DLY_reserved0_MASK                      0x3ffc0000
#define EMMC_TOP_CFG_IP_DLY_reserved0_SHIFT                     18

/* SDIO_0_CFG :: IP_DLY :: IP_DELAY_CTRL [17:16] */
#define EMMC_TOP_CFG_IP_DLY_IP_DELAY_CTRL_MASK                  0x00030000
#define EMMC_TOP_CFG_IP_DLY_IP_DELAY_CTRL_SHIFT                 16
#define EMMC_TOP_CFG_IP_DLY_IP_DELAY_CTRL_DEFAULT               0x00000003

/* SDIO_0_CFG :: IP_DLY :: reserved1 [15:06] */
#define EMMC_TOP_CFG_IP_DLY_reserved1_MASK                      0x0000ffc0
#define EMMC_TOP_CFG_IP_DLY_reserved1_SHIFT                     6

/* SDIO_0_CFG :: IP_DLY :: IP_TAP_DELAY [05:00] */
#define EMMC_TOP_CFG_IP_DLY_IP_TAP_DELAY_MASK                   0x0000003f
#define EMMC_TOP_CFG_IP_DLY_IP_TAP_DELAY_SHIFT                  0
#define EMMC_TOP_CFG_IP_DLY_IP_TAP_DELAY_DEFAULT                0x00000028

   uint32 emmc_top_cfg_op_dly;             /* 0x34 SDIO Host output delay register                   */ 
/***************************************************************************
 *OP_DLY - SDIO Host output delay register
 ***************************************************************************/
/* SDIO_0_CFG :: OP_DLY :: OP_TAP_EN [31:31] */
#define EMMC_TOP_CFG_OP_DLY_OP_TAP_EN_MASK                      0x80000000
#define EMMC_TOP_CFG_OP_DLY_OP_TAP_EN_SHIFT                     31
#define EMMC_TOP_CFG_OP_DLY_OP_TAP_EN_DEFAULT                   0x00000000

/* SDIO_0_CFG :: OP_DLY :: reserved0 [30:18] */
#define EMMC_TOP_CFG_OP_DLY_reserved0_MASK                      0x7ffc0000
#define EMMC_TOP_CFG_OP_DLY_reserved0_SHIFT                     18

/* SDIO_0_CFG :: OP_DLY :: OP_DELAY_CTRL [17:16] */
#define EMMC_TOP_CFG_OP_DLY_OP_DELAY_CTRL_MASK                  0x00030000
#define EMMC_TOP_CFG_OP_DLY_OP_DELAY_CTRL_SHIFT                 16
#define EMMC_TOP_CFG_OP_DLY_OP_DELAY_CTRL_DEFAULT               0x00000000

/* SDIO_0_CFG :: OP_DLY :: reserved1 [15:04] */
#define EMMC_TOP_CFG_OP_DLY_reserved1_MASK                      0x0000fff0
#define EMMC_TOP_CFG_OP_DLY_reserved1_SHIFT                     4

/* SDIO_0_CFG :: OP_DLY :: OP_TAP_DELAY [03:00] */
#define EMMC_TOP_CFG_OP_DLY_OP_TAP_DELAY_MASK                   0x0000000f
#define EMMC_TOP_CFG_OP_DLY_OP_TAP_DELAY_SHIFT                  0
#define EMMC_TOP_CFG_OP_DLY_OP_TAP_DELAY_DEFAULT                0x00000000

   uint32 emmc_top_cfg_tuning;             /* 0x38 SDIO Host tuning configuration register           */ 
/***************************************************************************
 *TUNING - SDIO Host tuning configuration register
 ***************************************************************************/
/* SDIO_0_CFG :: TUNING :: reserved0 [31:04] */
#define EMMC_TOP_CFG_TUNING_reserved0_MASK                      0xfffffff0
#define EMMC_TOP_CFG_TUNING_reserved0_SHIFT                     4

/* SDIO_0_CFG :: TUNING :: TUNING_CMD_SUCCESS_CNT [03:00] */
#define EMMC_TOP_CFG_TUNING_TUNING_CMD_SUCCESS_CNT_MASK         0x0000000f
#define EMMC_TOP_CFG_TUNING_TUNING_CMD_SUCCESS_CNT_SHIFT        0
#define EMMC_TOP_CFG_TUNING_TUNING_CMD_SUCCESS_CNT_DEFAULT      0x00000008

   uint32 emmc_top_cfg_volt_ctrl;          /* 0x3c SDIO Host 1p8V control logic select register      */
/***************************************************************************
 *VOLT_CTRL - SDIO Host 1p8V control logic select register
 ***************************************************************************/
/* SDIO_0_CFG :: VOLT_CTRL :: reserved0 [31:05] */
#define EMMC_TOP_CFG_VOLT_CTRL_reserved0_MASK                   0xffffffe0
#define EMMC_TOP_CFG_VOLT_CTRL_reserved0_SHIFT                  5

/* SDIO_0_CFG :: VOLT_CTRL :: POW_INV_EN [04:04] */
#define EMMC_TOP_CFG_VOLT_CTRL_POW_INV_EN_MASK                  0x00000010
#define EMMC_TOP_CFG_VOLT_CTRL_POW_INV_EN_SHIFT                 4
#define EMMC_TOP_CFG_VOLT_CTRL_POW_INV_EN_DEFAULT               0x00000000

/* SDIO_0_CFG :: VOLT_CTRL :: 1P8V_VAL [03:03] */
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_VAL_MASK                    0x00000008
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_VAL_SHIFT                   3
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_VAL_DEFAULT                 0x00000000

/* SDIO_0_CFG :: VOLT_CTRL :: 1P8V_INV_EN [02:02] */
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_INV_EN_MASK                 0x00000004
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_INV_EN_SHIFT                2
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_INV_EN_DEFAULT              0x00000000

/* SDIO_0_CFG :: VOLT_CTRL :: 1P8V_CTRL_SEL [01:00] */
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_CTRL_SEL_MASK               0x00000003
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_CTRL_SEL_SHIFT              0
#define EMMC_TOP_CFG_VOLT_CTRL_1P8V_CTRL_SEL_DEFAULT            0x00000000

   uint32 emmc_top_cfg_debug_tap_dly;      /* 0x40 Debug TAP delay setting register                  */
   
   uint32 unused1[3];                      /* 0x44 - 0x50                                            */
   
   uint32 emmc_top_cfg_sd_pin_sel;         /* 0x54 SD Pin Select                                     */
   
   uint32 emmc_top_cfg_max_current;        /* 0x58 Max Current Override                              */
   
   uint32 unused2[37];                     /* 0x5c - 0xec                                            */
   
   uint32 emmc_top_cfg_version;            /* 0xf0 SDIO VERSION Register                             */
/***************************************************************************
 *VERSION - SDIO VERSION Register
 ***************************************************************************/
/* SDIO_0_CFG :: VERSION :: SD_VER [31:24] */
#define EMMC_TOP_CFG_VERSION_SD_VER_MASK                        0xff000000
#define EMMC_TOP_CFG_VERSION_SD_VER_SHIFT                       24
#define EMMC_TOP_CFG_VERSION_SD_VER_DEFAULT                     0x00000030

/* SDIO_0_CFG :: VERSION :: MMC_VER [23:16] */
#define EMMC_TOP_CFG_VERSION_MMC_VER_MASK                       0x00ff0000
#define EMMC_TOP_CFG_VERSION_MMC_VER_SHIFT                      16
#define EMMC_TOP_CFG_VERSION_MMC_VER_DEFAULT                    0x00000044

/* SDIO_0_CFG :: VERSION :: REV [15:08] */
#define EMMC_TOP_CFG_VERSION_REV_MASK                           0x0000ff00
#define EMMC_TOP_CFG_VERSION_REV_SHIFT                          8
#define EMMC_TOP_CFG_VERSION_REV_DEFAULT                        0x000000a9

/* SDIO_0_CFG :: VERSION :: A2S_VER [07:00] */
#define EMMC_TOP_CFG_VERSION_A2S_VER_MASK                       0x000000ff
#define EMMC_TOP_CFG_VERSION_A2S_VER_SHIFT                      0
#define EMMC_TOP_CFG_VERSION_A2S_VER_DEFAULT                    0x00000001
   uint32 unused3[2];                      /* 0xf4 - 0xf8                                            */
   
   uint32 emmc_top_cfg_scratch;            /* 0xfc SDIO Scratch Register                             */   
/***************************************************************************
 *SCRATCH - SDIO Scratch Register
 ***************************************************************************/
/* SDIO_0_CFG :: SCRATCH :: SCRATCH_BITS [31:00] */
#define EMMC_TOP_CFG_SCRATCH_SCRATCH_BITS_MASK                  0xffffffff
#define EMMC_TOP_CFG_SCRATCH_SCRATCH_BITS_SHIFT                 0
#define EMMC_TOP_CFG_SCRATCH_SCRATCH_BITS_DEFAULT               0x00000000
} EmmcTopCfgRegs;
#define EMMC_TOP_CFG ((volatile EmmcTopCfgRegs *const) EMMC_TOP_CFG_BASE)

typedef struct EmmcBootRegs {
   uint32 emmc_boot_main_ctl;         /* 0x00 Main control register */   
#define EMMC_BOOT_ENABLE   (1 << 0)   
/***************************************************************************
 *MAIN_CTL - Main control register
 ***************************************************************************/
/* SDIO_1_BOOT :: MAIN_CTL :: reserved0 [31:03] */
#define EMMC_BOOT_MAIN_CTL_reserved0_MASK                   0xfffffff8
#define EMMC_BOOT_MAIN_CTL_reserved0_SHIFT                  3

/* SDIO_1_BOOT :: MAIN_CTL :: DivSpeedUp [02:02] */
#define EMMC_BOOT_MAIN_CTL_DivSpeedUp_MASK                  0x00000004
#define EMMC_BOOT_MAIN_CTL_DivSpeedUp_SHIFT                 2
#define EMMC_BOOT_MAIN_CTL_DivSpeedUp_DEFAULT               0x00000000

/* SDIO_1_BOOT :: MAIN_CTL :: reserved1 [01:01] */
#define EMMC_BOOT_MAIN_CTL_reserved1_MASK                   0x00000002
#define EMMC_BOOT_MAIN_CTL_reserved1_SHIFT                  1

/* SDIO_1_BOOT :: MAIN_CTL :: BootEna [00:00] */
#define EMMC_BOOT_MAIN_CTL_BootEna_MASK                     0x00000001
#define EMMC_BOOT_MAIN_CTL_BootEna_SHIFT                    0
#define EMMC_BOOT_MAIN_CTL_BootEna_DEFAULT                  0x00000000

   uint32 emmc_boot_status;           /* 0x04 Status                */     
#define EMMC_BOOT_MODE_MASK (1 << 0)    
/***************************************************************************
 *STATUS - Status
 ***************************************************************************/
/* SDIO_1_BOOT :: STATUS :: reserved0 [31:11] */
#define EMMC_BOOT_STATUS_reserved0_MASK                     0xfffff800
#define EMMC_BOOT_STATUS_reserved0_SHIFT                    11

/* SDIO_1_BOOT :: STATUS :: Boot_Rbus_Error [10:10] */
#define EMMC_BOOT_STATUS_Boot_Rbus_Error_MASK               0x00000400
#define EMMC_BOOT_STATUS_Boot_Rbus_Error_SHIFT              10
#define EMMC_BOOT_STATUS_Boot_Rbus_Error_DEFAULT            0x00000000

/* SDIO_1_BOOT :: STATUS :: AHB_Slave_Error [09:09] */
#define EMMC_BOOT_STATUS_AHB_Slave_Error_MASK               0x00000200
#define EMMC_BOOT_STATUS_AHB_Slave_Error_SHIFT              9
#define EMMC_BOOT_STATUS_AHB_Slave_Error_DEFAULT            0x00000000

/* SDIO_1_BOOT :: STATUS :: AHB_Master_Error [08:08] */
#define EMMC_BOOT_STATUS_AHB_Master_Error_MASK              0x00000100
#define EMMC_BOOT_STATUS_AHB_Master_Error_SHIFT             8
#define EMMC_BOOT_STATUS_AHB_Master_Error_DEFAULT           0x00000000

/* SDIO_1_BOOT :: STATUS :: SDIO_Host_Error [07:07] */
#define EMMC_BOOT_STATUS_SDIO_Host_Error_MASK               0x00000080
#define EMMC_BOOT_STATUS_SDIO_Host_Error_SHIFT              7
#define EMMC_BOOT_STATUS_SDIO_Host_Error_DEFAULT            0x00000000

/* SDIO_1_BOOT :: STATUS :: BusWidth [06:05] */
#define EMMC_BOOT_STATUS_BusWidth_MASK                      0x00000060
#define EMMC_BOOT_STATUS_BusWidth_SHIFT                     5

/* SDIO_1_BOOT :: STATUS :: BigEndian [04:04] */
#define EMMC_BOOT_STATUS_BigEndian_MASK                     0x00000010
#define EMMC_BOOT_STATUS_BigEndian_SHIFT                    4

/* SDIO_1_BOOT :: STATUS :: FetchActive [03:03] */
#define EMMC_BOOT_STATUS_FetchActive_MASK                   0x00000008
#define EMMC_BOOT_STATUS_FetchActive_SHIFT                  3
#define EMMC_BOOT_STATUS_FetchActive_DEFAULT                0x00000000

/* SDIO_1_BOOT :: STATUS :: RamValid1 [02:02] */
#define EMMC_BOOT_STATUS_RamValid1_MASK                     0x00000004
#define EMMC_BOOT_STATUS_RamValid1_SHIFT                    2
#define EMMC_BOOT_STATUS_RamValid1_DEFAULT                  0x00000000

/* SDIO_1_BOOT :: STATUS :: RamValid0 [01:01] */
#define EMMC_BOOT_STATUS_RamValid0_MASK                     0x00000002
#define EMMC_BOOT_STATUS_RamValid0_SHIFT                    1
#define EMMC_BOOT_STATUS_RamValid0_DEFAULT                  0x00000000

/* SDIO_1_BOOT :: STATUS :: BootMode [00:00] */
#define EMMC_BOOT_STATUS_BootMode_MASK                      0x00000001
#define EMMC_BOOT_STATUS_BootMode_SHIFT                     0
    
   uint32 emmc_boot_version;          /* 0x08 Version               */      
/***************************************************************************
 *VERSION - Version
 ***************************************************************************/
/* SDIO_1_BOOT :: VERSION :: reserved0 [31:24] */
#define EMMC_BOOT_VERSION_reserved0_MASK                    0xff000000
#define EMMC_BOOT_VERSION_reserved0_SHIFT                   24

/* SDIO_1_BOOT :: VERSION :: MajorRev [23:16] */
#define EMMC_BOOT_VERSION_MajorRev_MASK                     0x00ff0000
#define EMMC_BOOT_VERSION_MajorRev_SHIFT                    16
#define EMMC_BOOT_VERSION_MajorRev_DEFAULT                  0x00000001

/* SDIO_1_BOOT :: VERSION :: MinorRev [15:08] */
#define EMMC_BOOT_VERSION_MinorRev_MASK                     0x0000ff00
#define EMMC_BOOT_VERSION_MinorRev_SHIFT                    8
#define EMMC_BOOT_VERSION_MinorRev_DEFAULT                  0x00000000

/* SDIO_1_BOOT :: VERSION :: reserved1 [07:04] */
#define EMMC_BOOT_VERSION_reserved1_MASK                    0x000000f0
#define EMMC_BOOT_VERSION_reserved1_SHIFT                   4

/* SDIO_1_BOOT :: VERSION :: MetalRev [03:00] */
#define EMMC_BOOT_VERSION_MetalRev_MASK                     0x0000000f
#define EMMC_BOOT_VERSION_MetalRev_SHIFT                    0
#define EMMC_BOOT_VERSION_MetalRev_DEFAULT                  0x00000000

   uint32 unused1[1];                 /* 0x0c                       */
   uint32 emmc_boot_clk_div;          /* 0x10 Clock Divide Override */      
/***************************************************************************
 *CLK_DIV - Clock Divide Override
 ***************************************************************************/
/* SDIO_1_BOOT :: CLK_DIV :: reserved0 [31:12] */
#define EMMC_BOOT_CLK_DIV_reserved0_MASK                    0xfffff000
#define EMMC_BOOT_CLK_DIV_reserved0_SHIFT                   12

/* SDIO_1_BOOT :: CLK_DIV :: CmdDiv [11:08] */
#define EMMC_BOOT_CLK_DIV_CmdDiv_MASK                       0x00000f00
#define EMMC_BOOT_CLK_DIV_CmdDiv_SHIFT                      8
#define EMMC_BOOT_CLK_DIV_CmdDiv_DEFAULT                    0x00000000

/* SDIO_1_BOOT :: CLK_DIV :: reserved1 [07:04] */
#define EMMC_BOOT_CLK_DIV_reserved1_MASK                    0x000000f0
#define EMMC_BOOT_CLK_DIV_reserved1_SHIFT                   4

/* SDIO_1_BOOT :: CLK_DIV :: DataDiv [03:00] */
#define EMMC_BOOT_CLK_DIV_DataDiv_MASK                      0x0000000f
#define EMMC_BOOT_CLK_DIV_DataDiv_SHIFT                     0
#define EMMC_BOOT_CLK_DIV_DataDiv_DEFAULT                   0x00000000

   uint32 emmc_boot_reset_cnt;        /* 0x14 Reset Count           */    
/***************************************************************************
 *RESET_CNT - Reset Count
 ***************************************************************************/
/* SDIO_1_BOOT :: RESET_CNT :: reserved0 [31:16] */
#define EMMC_BOOT_RESET_CNT_reserved0_MASK                  0xffff0000
#define EMMC_BOOT_RESET_CNT_reserved0_SHIFT                 16

/* SDIO_1_BOOT :: RESET_CNT :: ResetCnt [15:00] */
#define EMMC_BOOT_RESET_CNT_ResetCnt_MASK                   0x0000ffff
#define EMMC_BOOT_RESET_CNT_ResetCnt_SHIFT                  0
#define EMMC_BOOT_RESET_CNT_ResetCnt_DEFAULT                0x00000000

   uint32 emmc_boot_ram_fill;         /* 0x18 Ram Fill              */     
/***************************************************************************
 *RAM_FILL - Ram Fill
 ***************************************************************************/
/* SDIO_1_BOOT :: RAM_FILL :: reserved0 [31:22] */
#define EMMC_BOOT_RAM_FILL_reserved0_MASK                   0xffc00000
#define EMMC_BOOT_RAM_FILL_reserved0_SHIFT                  22

/* SDIO_1_BOOT :: RAM_FILL :: FillAddr [21:00] */
#define EMMC_BOOT_RAM_FILL_FillAddr_MASK                    0x003fffff
#define EMMC_BOOT_RAM_FILL_FillAddr_SHIFT                   0
#define EMMC_BOOT_RAM_FILL_FillAddr_DEFAULT                 0x00000000

   uint32 emmc_boot_error_addr;       /* 0x1c Error Address         */   
/***************************************************************************
 *ERROR_ADDR - Error Address
 ***************************************************************************/
/* SDIO_1_BOOT :: ERROR_ADDR :: ErrorAddr [31:00] */
#define EMMC_BOOT_ERROR_ADDR_ErrorAddr_MASK                 0xffffffff
#define EMMC_BOOT_ERROR_ADDR_ErrorAddr_SHIFT                0
#define EMMC_BOOT_ERROR_ADDR_ErrorAddr_DEFAULT              0x00000000

   uint32 emmc_boot_base_addr0;       /* 0x20 RAM Base address      */   
/***************************************************************************
 *BASE_ADDR0 - RAM Base address
 ***************************************************************************/
/* SDIO_1_BOOT :: BASE_ADDR0 :: reserved0 [31:22] */
#define EMMC_BOOT_BASE_ADDR0_reserved0_MASK                 0xffc00000
#define EMMC_BOOT_BASE_ADDR0_reserved0_SHIFT                22

/* SDIO_1_BOOT :: BASE_ADDR0 :: BaseAddr [21:00] */
#define EMMC_BOOT_BASE_ADDR0_BaseAddr_MASK                  0x003fffff
#define EMMC_BOOT_BASE_ADDR0_BaseAddr_SHIFT                 0
#define EMMC_BOOT_BASE_ADDR0_BaseAddr_DEFAULT               0x00000000

   uint32 emmc_boot_base_addr1;       /* 0x24 RAM Base address      */   
/***************************************************************************
 *BASE_ADDR1 - RAM Base address
 ***************************************************************************/
/* SDIO_1_BOOT :: BASE_ADDR1 :: reserved0 [31:22] */
#define EMMC_BOOT_BASE_ADDR1_reserved0_MASK                 0xffc00000
#define EMMC_BOOT_BASE_ADDR1_reserved0_SHIFT                22

/* SDIO_1_BOOT :: BASE_ADDR1 :: BaseAddr [21:00] */
#define EMMC_BOOT_BASE_ADDR1_BaseAddr_MASK                  0x003fffff
#define EMMC_BOOT_BASE_ADDR1_BaseAddr_SHIFT                 0
#define EMMC_BOOT_BASE_ADDR1_BaseAddr_DEFAULT               0x00000000

   uint32 emmc_boot_ram_fill_cnt;     /* 0x28 RAM Fill Cnt          */ 
/***************************************************************************
 *RAM_FILL_CNT - RAM Fill Cnt
 ***************************************************************************/
/* SDIO_1_BOOT :: RAM_FILL_CNT :: reserved0 [31:11] */
#define EMMC_BOOT_RAM_FILL_CNT_reserved0_MASK               0xfffff800
#define EMMC_BOOT_RAM_FILL_CNT_reserved0_SHIFT              11

/* SDIO_1_BOOT :: RAM_FILL_CNT :: RamFillCnt [10:00] */
#define EMMC_BOOT_RAM_FILL_CNT_RamFillCnt_MASK              0x000007ff
#define EMMC_BOOT_RAM_FILL_CNT_RamFillCnt_SHIFT             0
#define EMMC_BOOT_RAM_FILL_CNT_RamFillCnt_DEFAULT           0x00000000

   uint32 emmc_boot_data_access_time; /* 0x2c Time for Data Fetch   */
/***************************************************************************
 *DATA_ACCESS_TIME - Time for Data Fetch
 ***************************************************************************/
/* SDIO_1_BOOT :: DATA_ACCESS_TIME :: reserved0 [31:16] */
#define EMMC_BOOT_DATA_ACCESS_TIME_reserved0_MASK           0xffff0000
#define EMMC_BOOT_DATA_ACCESS_TIME_reserved0_SHIFT          16

/* SDIO_1_BOOT :: DATA_ACCESS_TIME :: DataAccessTime [15:00] */
#define EMMC_BOOT_DATA_ACCESS_TIME_DataAccessTime_MASK      0x0000ffff
#define EMMC_BOOT_DATA_ACCESS_TIME_DataAccessTime_SHIFT     0
#define EMMC_BOOT_DATA_ACCESS_TIME_DataAccessTime_DEFAULT   0x00000000

   uint32 unused2[3];                 /* 0x30-0x38                  */ 
   uint32 emmc_boot_debug;            /* 0x3c Debug                 */           
/***************************************************************************
 *DEBUG - Debug
 ***************************************************************************/
/* SDIO_1_BOOT :: DEBUG :: Debug [31:00] */
#define EMMC_BOOT_DEBUG_Debug_MASK                          0xffffffff
#define EMMC_BOOT_DEBUG_Debug_SHIFT                         0
} EmmcBootRegs;
#define EMMC_BOOT    ((volatile EmmcBootRegs *const) EMMC_BOOT_BASE) 
  
typedef struct AhbssCtrlRegs {
   uint32 ahbss_ctrl_cfg;    /* 0x00 AHB Subsystem Control Register */   
#define FORCE_EMMC_BOOT_STRAP    0x00000001   
} AhbssCtrlRegs;
#define AHBSS_CTRL   ((volatile AhbssCtrlRegs *const) AHBSS_CTRL_BASE)
  
/*
 * PL081 DMA controller ctrl
 */
typedef struct Pl081DmaCtrlRegs {
   uint32 dmacintstat;     /* 0xd000 interrupt status              */
   uint32 dmacinttcstat;   /* 0xd004 tc interrupt status           */
   uint32 dmacinttcclr;    /* 0xd008 tc interrupt clear            */
   uint32 dmacinterrstat;  /* 0xd00c err interrupt status          */
   uint32 dmacinterrclr;   /* 0xd010 err interrupt clear           */
   uint32 dmacrawintc;     /* 0xd014 Raw tc interrupt status       */
   uint32 dmacrawinterr;   /* 0xd018 Raw err interrupt status      */  
   uint32 dmacenbldchns;   /* 0xd01c channel Enables               */
   uint32 dmacsoftbreq;    /* 0xd020 Soft DMA burst Request        */
   uint32 dmacsoftsreq;    /* 0xd024 Soft DMA single Request       */
   uint32 dmacsoftlbreq;   /* 0xd028 Soft DMA L-burst Request      */
   uint32 dmacsoftlsreq;   /* 0xd02c Soft DMA L-single Request     */
   uint32 dmacconfig;      /* 0xd030 DMAC configuration            */
   uint32 dmacsync;        /* 0xd034 DMAC requests select control  */
   uint32 unused1[50];     /* 0xd038 - 0xd0ff                      */
   uint32 dmacc0srcaddr;   /* 0xd100 Source Address register0      */
   uint32 dmacc0destaddr;  /* 0xd104 Destination Address reg0      */
   uint32 dmacc0llireg;    /* 0xd108 Next Linked list Address0     */
   uint32 dmacc0control;   /* 0xd10c channel control register0     */
   uint32 dmacc0config;    /* 0xd110 channel config register0      */
   uint32 unused2[3];      /* 0xd114 - 0xd11f                      */
   uint32 dmacc1srcaddr;   /* 0xd120 Source Address register1      */
   uint32 dmacc1destaddr;  /* 0xd124 Destination Address reg1      */
   uint32 dmacc1llireg;    /* 0xd128 Next Linked list Address1     */
   uint32 dmacc1control;   /* 0xd12c channel control register1     */
   uint32 dmacc1config;    /* 0xd130 channel config register1      */
   uint32 unused3[51];     /* 0xd134 - 0xd1ff                      */
#define Pl081_DMACCxControl_TCINT_EN         (1 << 31)
#define Pl081_DMACCxControl_PROT_PRIVILEGED  (1 << 28)
#define Pl081_DMACCxControl_PROT_BUFF        (1 << 29)
#define Pl081_DMACCxControl_PROT_CACHE       (1 << 30)
#define Pl081_DMACCxControl_DI               (1 << 27)
#define Pl081_DMACCxControl_SI               (1 << 26)

#define Pl081_DMACCxControl_DWIDTH_B      0
#define Pl081_DMACCxControl_DWIDTH_H      1
#define Pl081_DMACCxControl_DWIDTH_W      2
#define Pl081_DMACCxControl_DWIDTH_SHIFT  21
                                          
#define Pl081_DMACCxControl_SWIDTH_B      0
#define Pl081_DMACCxControl_SWIDTH_H      1
#define Pl081_DMACCxControl_SWIDTH_W      2
#define Pl081_DMACCxControl_SWIDTH_SHIFT  18
                                          
#define Pl081_DMACCxControl_DBSIZE_1      0
#define Pl081_DMACCxControl_DBSIZE_4      1
#define Pl081_DMACCxControl_DBSIZE_8      2
#define Pl081_DMACCxControl_DBSIZE_16     3
#define Pl081_DMACCxControl_DBSIZE_32     4
#define Pl081_DMACCxControl_DBSIZE_64     5
#define Pl081_DMACCxControl_DBSIZE_128    6
#define Pl081_DMACCxControl_DBSIZE_256    7
#define Pl081_DMACCxControl_DBSIZE_SHIFT  15
                                          
#define Pl081_DMACCxControl_SBSIZE_1      0
#define Pl081_DMACCxControl_SBSIZE_4      1
#define Pl081_DMACCxControl_SBSIZE_8      2
#define Pl081_DMACCxControl_SBSIZE_16     3
#define Pl081_DMACCxControl_SBSIZE_32     4
#define Pl081_DMACCxControl_SBSIZE_64     5
#define Pl081_DMACCxControl_SBSIZE_128    6
#define Pl081_DMACCxControl_SBSIZE_SHIFT  12
  
   uint32 dmactcr;         /* 0xd200 Test control                  */ 
   uint32 dmacitop1;       /* 0xd204 Output Set/Read               */
   uint32 dmacitop2;       /* 0xd208 Output Set/Read               */
   uint32 dmacitop3;       /* 0xd20c Output Set/Read               */
   uint32 unused4[52];     /* 0xd210 - 0xd2df                     */
   uint32 dmacperiphid0;   /* 0xd2e0 Peripheral Identification     */
   uint32 dmacperiphid1;   /* 0xd2e4 Peripheral Identification     */
   uint32 dmacperiphid2;   /* 0xd2e8 Peripheral Identification     */
   uint32 dmacperiphid3;   /* 0xd2ec Peripheral Identification     */
   uint32 dmacpcellid0;    /* 0xd2f0 PrimeCell Identification      */
   uint32 dmacpcellid1;    /* 0xd2f4 PrimeCell Identification      */
   uint32 dmacpcellid2;    /* 0xd2f8 PrimeCell Identification      */
   uint32 dmacpcellid3;    /* 0xd2fc PrimeCell Identification      */    
} Pl081DmaCtrlRegs;

#define PL081_DMA_CHAN_HS_UART_TX   "DMA_TO_HS_UART_TX"
#define PL081_DMA_CHAN_HS_UART_RX   "DMA_FROM_HS_UART_RX"

#define PL081_DMA ((volatile Pl081DmaCtrlRegs * const) PL081_DMA_BASE)

#if defined(_ATF_)
/*
 * Power Management Control
 */
typedef struct PmcCtrlReg {
    uint32 gpTmr0Ctl;           /* 0x018 */
    uint32 gpTmr0Cnt;           /* 0x01c */
    uint32 gpTmr1Ctl;           /* 0x020 */
    uint32 gpTmr1Cnt;           /* 0x024 */
    uint32 hostMboxIn;          /* 0x028 */
    uint32 hostMboxOut;         /* 0x02c */
    uint32 reserved[4];         /* 0x030 */
    uint32 dmaCtrl;             /* 0x040 */
    uint32 dmaStatus;           /* 0x044 */
    uint32 dma0_3FifoStatus;    /* 0x048 */
    uint32 reserved1[4];	    /* 0x04c */
    uint32 diagControl;         /* 0x05c */
    uint32 diagHigh;            /* 0x060 */
    uint32 diagLow;             /* 0x064 */
    uint32 reserved8;           /* 0x068 */
    uint32 addr1WndwMask;       /* 0x06c */
    uint32 addr1WndwBaseIn;     /* 0x070 */
    uint32 addr1WndwBaseOut;    /* 0x074 */
    uint32 addr2WndwMask;       /* 0x078 */
    uint32 addr2WndwBaseIn;     /* 0x07c */
    uint32 addr2WndwBaseOut;    /* 0x080 */
    uint32 scratch;             /* 0x084 */
    uint32 reserved9;           /* 0x088 */
    uint32 softResets;          /* 0x08c */
    uint32 reserved2;           /* 0x090 */
    uint32 m4keCoreStatus;      /* 0x094 */
    uint32 reserved3;           /* 0x098 */
    uint32 ubSlaveTimeout;      /* 0x09c */
    uint32 diagEn;              /* 0x0a0 */
    uint32 devTimeout;          /* 0x0a4 */
    uint32 ubusErrorOutMask;    /* 0x0a8 */
    uint32 diagCaptStopMask;    /* 0x0ac */
    uint32 revId;               /* 0x0b0 */
    uint32 reserved4[4];        /* 0x0b4 */
    uint32 diagCtrl;            /* 0x0c4 */
    uint32 diagStat;            /* 0x0c8 */
    uint32 diagMask;            /* 0x0cc */
    uint32 diagRslt;            /* 0x0d0 */
    uint32 diagCmp;             /* 0x0d4 */
    uint32 diagCapt;            /* 0x0d8 */    
    uint32 diagCnt;             /* 0x0dc */
    uint32 diagEdgeCnt;         /* 0x0e0 */
    uint32 reserved5[4];	    /* 0x0e4 */
    uint32 smisc_bus_config;    /* 0x0f4 */
    uint32 lfsr;                /* 0x0f8 */
    uint32 dqm_pac_lock;        /* 0x0fc */
    uint32 l1_irq_4ke_mask;     /* 0x100 */
    uint32 l1_irq_4ke_status;   /* 0x104 */
    uint32 l1_irq_mips_mask;    /* 0x108 */
    uint32 l1_irq_mips_status;  /* 0x10c */
    uint32 l1_irq_mips1_mask;   /* 0x110 */
    uint32 reserved6[3];        /* 0x114 */
    uint32 l2_irq_gp_mask;      /* 0x120 */
    uint32 l2_irq_gp_status;    /* 0x124 */
    uint32 l2_irq_gp_set;       /* 0x128 */
    uint32 reserved7;           /* 0x12c */
    uint32 gp_in_irq_mask;      /* 0x130 */
    uint32 gp_in_irq_status;    /* 0x134 */
    uint32 gp_in_irq_set;       /* 0x138 */
    uint32 gp_in_irq_sense;     /* 0x13c */
    uint32 gp_in;               /* 0x140 */
    uint32 gp_out;              /* 0x144 */
} PmcCtrlReg;

typedef struct PmcDmaReg {
	/* 0x00 */
	uint32 src;
	uint32 dest;
	uint32 cmdList;
	uint32 lenCtl;
	/* 0x10 */
	uint32 rsltSrc;
	uint32 rsltDest;
	uint32 rsltHcs;
	uint32 rsltLenStat;
} PmcDmaReg;

typedef struct PmcTokenReg {
	/* 0x00 */
	uint32 bufSize;
	uint32 bufBase;
	uint32 idx2ptrIdx;
	uint32 idx2ptrPtr;
	/* 0x10 */
	uint32 unused[2];
	uint32 bufSize2;
} PmcTokenReg;

typedef struct PmcPerfPowReg {
	uint32 freqScalarCtrl; /* 0x3c */
	uint32 freqScalarMask; /* 0x40 */
} PmcPerfPowReg;

typedef struct PmcDQMPac {
    uint32 dqmPac[32];
} PmcDQMPac;

typedef struct PmcDQMReg {
	uint32 cfg;                     /* 0x1c00 */
	uint32 _4keLowWtmkIrqMask;      /* 0x1c04 */
	uint32 mipsLowWtmkIrqMask;      /* 0x1c08 */
	uint32 lowWtmkIrqMask;          /* 0x1c0c */
	uint32 _4keNotEmptyIrqMask;     /* 0x1c10 */
	uint32 mipsNotEmptyIrqMask;     /* 0x1c14 */
	uint32 notEmptyIrqSts;          /* 0x1c18 */
	uint32 queueRst;                /* 0x1c1c */
	uint32 notEmptySts;             /* 0x1c20 */
	uint32 nextAvailMask;           /* 0x1c24 */
	uint32 nextAvailQueue;          /* 0x1c28 */
	uint32 mips1LowWtmkIrqMask;     /* 0x1c2c */
	uint32 mips1NotEmptyIrqMask;    /* 0x1c30 */
	uint32 autoSrcPidInsert;        /* 0x1c34 */
    uint32 timerIrqStatus;          /* 0x1c38 */
    uint32 timerStatus;             /* 0x1c3c */
    uint32 _4keTimerIrqMask;        /* 0x1c40 */
    uint32 mipsTimerIrqMask;        /* 0x1c44 */
    uint32 mips1TimerIrqMask;       /* 0x1c48 */
} PmcDQMReg;

typedef struct PmcCntReg {
	uint32 cntr[10];
	uint32 unused[6];	/* 0x28-0x3f */
	uint32 cntrIrqMask;
	uint32 cntrIrqSts;
} PmcCntReg;

typedef struct PmcDqmQCtrlReg {
	uint32 size;
	uint32 cfga;
	uint32 cfgb;
	uint32 cfgc;
} PmcDqmQCtrlReg;

typedef struct PmcDqmQDataReg {
	uint32 word[4];
} PmcDqmQDataReg;

typedef struct PmcDqmQMibReg {
	uint32 qNumFull[32];
	uint32 qNumEmpty[32];
	uint32 qNumPushed[32];
} PmcDqmQMibReg;

typedef struct SSBMaster {
    uint32 ssbmControl;     /* 0x0060 */
    uint32 ssbmWrData;      /* 0x0064 */
    uint32 ssbmRdData;      /* 0x0068 */
    uint32 ssbmStatus;      /* 0x006c */
} SSBMaster;

typedef struct PmmReg {
    uint32 memPowerCtrl;            /* 0x0000 */
    uint32 regSecurityConfig;       /* 0x0004 */
} PmmReg;

typedef struct keyholeReg {
    uint32 ctrlSts;
    uint32 wrData;
    uint32 mutex;
    uint32 rdData;
} keyholeReg;

typedef struct PmbBus {
    uint32 config;          /* 0x0100 */
    uint32 arbiter;         /* 0x0104 */
    uint32 timeout;         /* 0x0108 */
    uint32 unused1;         /* 0x010c */
    keyholeReg keyhole[4];  /* 0x0110-0x014f */
    uint32 unused2[44];     /* 0x0150-0x01ff */
    uint32 map[64];         /* 0x0200-0x02ff */ 
}PmbBus;

typedef struct  CoreCtrl {
    uint32  coreEnable;         /* 0x0400 */
    uint32  autoresetControl;   /* 0x0404 */
    uint32  coreIdle;           /* 0x0408 */
    uint32  coreResetCause;     /* 0x040c */
    uint32  memPwrDownCtrl0;    /* 0x0410 */
    uint32  memPwrDownSts0;     /* 0x0414 */
    uint32  memPwrDownCtrl1;    /* 0x0418 */
    uint32  memPwrDownSts1;     /* 0x041c */
    uint32  sysFlg0Status;      /* 0x0420 */
    uint32  sysFlg0Set;         /* 0x0424 */
    uint32  sysFlg0Clear;       /* 0x0428 */
    uint32  unused1;            /* 0x042c */
    uint32  usrFlg0Status;      /* 0x0430 */
    uint32  usrFlg0Set;         /* 0x0434 */
    uint32  usrFlg0Clear;       /* 0x0438 */
    uint32  unused2;            /* 0x043c */
    uint32  subsystemRev;       /* 0x0440 */
    uint32  resetVector;        /* 0x0444 */
} CoreCtrl;

typedef struct  CoreState {
    uint32  sysMbx[8];          /* 0x0480 */
    uint32  usrMbx[8];          /* 0x04a0 */
    uint32  sysMtx[4];          /* 0x04c0 */
    uint32  usrMtx[8];          /* 0x04d0 */
} CoreState;

typedef struct  CoreIntr {
    uint32  irqStatus;          /* 0x0500 */
    uint32  irqSet;             /* 0x0504 */
    uint32  irqClear;           /* 0x0508 */
    uint32  unused1;            /* 0x050c */
    uint32  srqStatus;          /* 0x0510 */
    uint32  srqSet;             /* 0x0514 */
    uint32  srqClear;           /* 0x0518 */
    uint32  unused2;            /* 0x051c */
    uint32  drqStatus;          /* 0x0520 */
    uint32  drqSet;             /* 0x0524 */
    uint32  drqClear;           /* 0x0528 */
    uint32  unused3;            /* 0x052c */
    uint32  frqStatus;          /* 0x0530 */
    uint32  frqSet;             /* 0x0534 */
    uint32  frqClear;           /* 0x0538 */
    uint32  unused4;            /* 0x053c */
    uint32  hostIrqLatched;     /* 0x0540 */
    uint32  hostIrqSet;         /* 0x0544 */
    uint32  hostIrqClear;       /* 0x0548 */
    uint32  hostIrqEnable;      /* 0x054c */
    uint32  obusFaultStatus;    /* 0x0550 */
    uint32  obusFaultClear;     /* 0x0554 */
    uint32  obusFaultAddr;      /* 0x0558 */
} CoreIntr;

typedef struct CoreProfile {
    uint32  mutex;              /* 0x0580 */
    uint32  lastConfPcLo;       /* 0x0584 */
    uint32  lastConfPcHi;       /* 0x0588 */
    uint32  lastPcLo;           /* 0x058c */
    uint32  lastPcHi;           /* 0x0590 */
    uint32  braTargetPc0Lo;     /* 0x0594 */
    uint32  braTargetPc0Hi;     /* 0x0598 */
    uint32  braTargetPc1Lo;     /* 0x059c */
    uint32  braTargetPc1Hi;     /* 0x05a0 */
    uint32  braTargetPc2Lo;     /* 0x05a4 */
    uint32  braTargetPc2Hi;     /* 0x05a8 */
    uint32  braTargetPc3Lo;     /* 0x05ac */
    uint32  braTargetPc3Hi;     /* 0x05b0 */
    uint32  unused[3];          /* 0x05b4-0x05bf */
    uint32  profSampleW[4];     /* 0x05c0 */
} CoreProfile;

typedef struct MaestroMisc {
    CoreCtrl coreCtrl;          /* 0x0400 */
    uint32   unused1[14];       /* 0x0448-0x047f */
    CoreState coreState;        /* 0x0480 */
    uint32   unused2[4];        /* 0x04f0-0x04ff */
    CoreIntr interrupt;         /* 0x0500 */
    uint32   unused3[9];        /* 0x055c-0x057f */
    CoreProfile profile;        /* 0x0580 */
} MaestroMisc;

typedef struct Pmc {
    uint32 unused0[1030];
    PmcCtrlReg ctrl;		            /* 0x1018 */
    uint32 unused1[622];	            /* 0x1148-0x1cff */
    PmcDQMPac dqmPac;                   /* 0x1b00 */
    uint32 unused5[32];                 /* 0x1b80-0x1bff */
    PmcDQMReg dqm;			            /* 0x1c00 */
    uint32 unused6[749];		        /* 0x1c4c-0x27ff */
    uint32 qStatus[32];		            /* 0x2800 */
    uint32 unused7[480];	            /* 0x2880-0x2fff */
    PmcDqmQMibReg qMib;		            /* 0x3000 */
    uint32 unused8[928];	            /* 0x3180-0x3fff */
    PmcDqmQCtrlReg dqmQCtrl[8]; 	    /* 0x4000 */
    uint32 unused9[992];                /* 0x4080-0x4fff */
    PmcDqmQDataReg dqmQData[8]; 	    /* 0x5000 */
} Pmc;
#define PMC ((volatile Pmc * const) PMC_BASE)

typedef struct Procmon {
    uint32 unused00[256];
    MaestroMisc maestroReg;             /* 0x00400 */
    uint32 unused10[32396];             /* 0x005d0-0x1ffff */
    PmmReg  pmm;                        /* 0x20000 */
    uint32 unused11[22];                /* 0x20008-0x2005f */
    SSBMaster ssbMasterCtrl;            /* 0x20060 */
    uint32 unused12[36];                /* 0x20070-0x200ff */
    PmbBus pmb;                         /* 0x20100 */
    uint32 unused13[32576];             /* 0x20300-0x3ffff */
    uint32 qsm[128];                    /* 0x40000-0x401ff */
    uint32 unused14[65408];             /* 0x40200-0x7ffff */
    uint32 dtcm[1024];                  /* 0x80000-0x80fff */
    uint32 unused15[64512];             /* 0x81000-0xbffff */
    uint32 itcm[4096];                  /* 0xc0000-0xc3fff */
} Procmon;
#define PROCMON ((volatile Procmon * const) PROC_MON_BASE)

typedef struct PMSSBMasterControl {
	uint32 control;
	uint32 wr_data;
	uint32 rd_data;
} PMSSBMasterControl;

typedef struct
{
    uint32  control;
#define PMC_PMBM_START		            (1 << 31)
#define PMC_PMBM_TIMEOUT	            (1 << 30)
#define PMC_PMBM_SLAVE_ERR	            (1 << 29)
#define PMC_PMBM_BUSY		            (1 << 28)
#define PMC_PMBM_BUS_SHIFT              (20)
#define PMC_PMBM_Read		            (0 << 24)
#define PMC_PMBM_Write		            (1 << 24)
    uint32  wr_data;
    uint32  mutex;
    uint32  rd_data;
} PMB_keyhole_reg;

typedef struct PMBMaster {
    uint32 config;
#define PMB_NUM_REGS_SHIFT (20)
#define PMB_NUM_REGS_MASK  (0x3ff)
    uint32 arbitger;
    uint32 timeout;
    uint32 reserved;
    PMB_keyhole_reg keyhole[4];
    uint32 reserved1[44];
    uint32 map[64];
} PMBMaster;
#define PMB ((volatile PMBMaster * const) PMB_BASE)
#endif

#define SWITCH_SINGLE_SERDES_STAT 0
    #define SWITCH_REG_SSER_LINK_STAT   (1<<0)
    #define SWITCH_REG_SSER_RXSIG_DET   (1<<1)
    #define SWITCH_REG_SSER_RXSIG_1G    (1<<2)
    #define SWITCH_REG_SSER_SGMII       (1<<3)
    #define SWITCH_REG_SSER_SYNC_STAT   (1<<4)
    #define SWITCH_REG_SSER_POLL_LOCK   (1<<5)
    #define SWITCH_REG_SSER_EXTFB_DET   (1<<6)

#define SWITCH_REG_LED_WAN_CNTRL_LED    0
//#define SWITCH_REG_LED_WAN_CNTRL_LED    (&ETHSW_REG->led_wan_ctrl)
#define SWITCH_REG_LED_WAN_TX_EN      (1<<1)
#define SWITCH_REG_LED_WAN_RX_EN      (1<<0)

/*
** Eth Switch Registers
*/
typedef struct {
    unsigned int led_f;
    unsigned int reserved;
} LED_F;

typedef struct EthernetSwitchCore
{
	uint64 port_traffic_ctrl[9];
	uint8 dummy1[8];
	uint64 SWITCH_CORE_RX_GLOBAL_CTL;
#define ETHSW_SM_RETRY_LIMIT_DIS                  0x04
#define ETHSW_SM_FORWARDING_EN                    0x02
#define ETHSW_SM_MANAGED_MODE                     0x01
	uint32 switch_mode;
	uint8 dummy2[148];
	uint32 SWITCH_CORE_DEBUG_REG;
	uint8 dummy3[20];
	uint64 SWITCH_CORE_NEW_CTRL;
	uint32 switch_ctrl;
#define ETHSW_SC_MII_DUMP_FORWARDING_EN           0x40
#define ETHSW_SC_MII2_VOL_SEL                     0x02
	uint8 dummy4[12];
	uint32 SWITCH_CORE_PROTECTED_SEL;
	uint8 dummy5[12];
	uint32 SWITCH_CORE_WAN_PORT_SEL;
	uint8 dummy6[68];
	uint32 SWITCH_CORE_RSV_MCAST_CTRL;
	uint8 dummy7[12];
	uint64 SWITCH_CORE_TXQ_FLUSH_MODE;
	uint32 SWITCH_CORE_ULF_DROP_MAP;
	uint8 dummy8[12];
	uint32 SWITCH_CORE_MLF_DROP_MAP;
	uint8 dummy9[12];
	uint32 SWITCH_CORE_MLF_IPMC_FWD_MAP;
	uint8 dummy10[12];
	uint32 SWITCH_CORE_RX_PAUSE_PASS;
	uint8 dummy11[12];
	uint32 SWITCH_CORE_TX_PAUSE_PASS;
	uint8 dummy12[12];
	uint32 SWITCH_CORE_DIS_LEARN;
	uint8 dummy13[12];
	uint32 SWITCH_CORE_SFT_LRN_CTL;
	uint8 dummy14[12];
	uint32 SWITCH_CORE_LOW_POWER_EXP1;
	uint8 dummy15[156];
	uint32 SWITCH_CORE_CTLREG_REG_SPARE;
	uint8 dummy16[292];
	uint64 software_reset;
	uint32 SWITCH_CORE_WATCH_DOG_RPT1;
	uint8 dummy17[12];
	uint32 SWITCH_CORE_WATCH_DOG_RPT2;
	uint8 dummy18[12];
	uint32 SWITCH_CORE_WATCH_DOG_RPT3;
	uint8 dummy19[12];
	uint64 SWITCH_CORE_PAUSE_FRM_CTRL;
	uint32 SWITCH_CORE_PAUSE_ST_ADDR;
	uint8 dummy20[52];
	uint64 SWITCH_CORE_FAST_AGE_CTRL;
	uint64 SWITCH_CORE_FAST_AGE_PORT;
	uint32 SWITCH_CORE_FAST_AGE_VID;
	uint8 dummy21[668];
	uint32 SWITCH_CORE_LOW_POWER_CTRL;
	uint8 dummy22[76];
	uint32 SWITCH_CORE_TCAM_CTRL;
	uint8 dummy23[12];
	uint32 SWITCH_CORE_TCAM_CHKSUM_STS;
	uint8 dummy24[12];
	uint32 SWITCH_CORE_LIGHTSTACK_CTRL;
	uint8 dummy25[156];
	uint32 SWITCH_CORE_LNKSTS;
	uint8 dummy26[12];
	uint32 SWITCH_CORE_LNKSTSCHG;
	uint8 dummy27[12];
	uint32 SWITCH_CORE_SPDSTS;
	uint8 dummy28[28];
	uint32 SWITCH_CORE_DUPSTS;
	uint8 dummy29[12];
	uint32 SWITCH_CORE_PAUSESTS;
	uint8 dummy30[28];
	uint32 SWITCH_CORE_SRCADRCHG;
	uint8 dummy31[12];
	uint32 SWITCH_CORE_LSA_PORT_P0;
	uint8 dummy32[44];
	uint32 SWITCH_CORE_LSA_PORT_P1;
	uint8 dummy33[44];
	uint32 SWITCH_CORE_LSA_PORT_P2;
	uint8 dummy34[44];
	uint32 SWITCH_CORE_LSA_PORT_P3;
	uint8 dummy35[44];
	uint32 SWITCH_CORE_LSA_PORT_P4;
	uint8 dummy36[44];
	uint32 SWITCH_CORE_LSA_PORT_P5;
	uint8 dummy37[44];
	uint32 SWITCH_CORE_LSA_PORT_P6;
	uint8 dummy38[44];
	uint32 SWITCH_CORE_LSA_PORT_P7;
	uint8 dummy39[44];
	uint32 SWITCH_CORE_LSA_PORT_P8;
	uint8 dummy40[44];
	uint32 SWITCH_CORE_BIST_STS0;
	uint8 dummy41[44];
	uint32 SWITCH_CORE_BIST_STS1;
	uint8 dummy42[28];
	uint32 SWITCH_CORE_PBPTRFIFO_0;
	uint8 dummy43[44];
	uint32 SWITCH_CORE_PBPTRFIFO_1;
	uint8 dummy44[460];
	uint32 SWITCH_CORE_RESET_STATUS;
	uint8 dummy45[124];
	uint32 SWITCH_CORE_STREG_REG_SPARE0;
	uint8 dummy46[28];
	uint32 SWITCH_CORE_STREG_REG_SPARE1;
	uint8 dummy47[732];
	uint64 SWITCH_CORE_GMNGCFG;
	uint64 SWITCH_CORE_IMP0_PRT_ID;
	uint64 SWITCH_CORE_IMP1_PRT_ID;
	uint32 brcm_hdr_ctrl;
	uint8 dummy48[20];
	uint32 SWITCH_CORE_SPTAGT;
	uint8 dummy49[28];
	uint32 SWITCH_CORE_BRCM_HDR_CTRL2;
	uint8 dummy50[12];
	uint32 SWITCH_CORE_IPG_SHRNK_CTRL;
	uint8 dummy51[28];
	uint32 SWITCH_CORE_MIRCAPCTL;
	uint8 dummy52[12];
	uint32 SWITCH_CORE_IGMIRCTL;
	uint8 dummy53[12];
	uint32 SWITCH_CORE_IGMIRDIV;
	uint8 dummy54[12];
	uint32 SWITCH_CORE_IGMIRMAC;
	uint8 dummy55[44];
	uint32 SWITCH_CORE_EGMIRCTL;
	uint8 dummy56[12];
	uint32 SWITCH_CORE_EGMIRDIV;
	uint8 dummy57[12];
	uint32 SWITCH_CORE_EGMIRMAC;
	uint8 dummy58[44];
	uint32 SWITCH_CORE_SPANCTL;
	uint8 dummy59[28];
	uint32 SWITCH_CORE_RSPANVLAN;
	uint8 dummy60[300];
	uint32 SWITCH_CORE_HL_PRTC_CTRL;
	uint8 dummy61[28];
	uint32 SWITCH_CORE_RST_MIB_CNT_EN;
	uint8 dummy62[28];
	uint32 SWITCH_CORE_IPG_SHRINK_2G_WA;
	uint8 dummy63[60];
	uint32 SWITCH_CORE_BRCM_HDR_RX_DIS;
	uint8 dummy64[12];
	uint32 SWITCH_CORE_BRCM_HDR_TX_DIS;
	uint8 dummy65[108];
	uint32 SWITCH_CORE_MNGMODE_REG_SPARE0;
	uint8 dummy66[28];
	uint32 SWITCH_CORE_MNGMODE_REG_SPARE1;
	uint8 dummy67[1116];
	uint32 SWITCH_CORE_INT_STS;
	uint8 dummy68[60];
	uint32 SWITCH_CORE_INT_EN;
	uint8 dummy69[60];
	uint32 SWITCH_CORE_SLEEP_TIMER_IMP;
	uint8 dummy70[12];
	uint32 SWITCH_CORE_PORT7_SLEEP_TIMER;
	uint8 dummy71[12];
	uint32 SWITCH_CORE_WAN_SLEEP_TIMER;
	uint8 dummy72[28];
	uint32 SWITCH_CORE_PORT_SLEEP_STS;
	uint8 dummy73[92];
	uint32 SWITCH_CORE_LINK_STS_INT_EN;
	uint8 dummy74[28];
	uint32 SWITCH_CORE_ENG_DET_INT_EN;
	uint8 dummy75[12];
	uint32 SWITCH_CORE_LPI_STS_CHG_INT_EN;
	uint8 dummy76[428];
	uint32 SWITCH_CORE_MEM_ECC_ERR_INT_STS;
	uint8 dummy77[12];
	uint32 SWITCH_CORE_MEM_ECC_ERR_INT_EN;
	uint8 dummy78[12];
	uint32 SWITCH_CORE_PORT_EVT_ECC_ERR_STS;
	uint8 dummy79[12];
	uint32 SWITCH_CORE_PORT_MIB_ECC_ERR_STS;
	uint8 dummy80[12];
	uint32 SWITCH_CORE_PORT_TXQ_ECC_ERR_STS;
	uint8 dummy81[60];
	uint32 SWITCH_CORE_PROBE_BUS_CTL;
	uint8 dummy82[28];
	uint32 SWITCH_CORE_MDC_EXTEND_CTRL;
	uint8 dummy83[92];
	uint32 SWITCH_CORE_PPPOE_SESSION_PARSE_EN;
	uint8 dummy84[124];
	uint32 SWITCH_CORE_CTLREG_1_REG_SPARE0;
	uint8 dummy85[28];
	uint32 SWITCH_CORE_CTLREG_1_REG_SPARE1;
	uint8 dummy86[860];
	uint32 SWITCH_CORE_GARLCFG;
	uint8 dummy87[28];
	uint32 SWITCH_CORE_BPDU_MCADDR;
	uint8 dummy88[76];
	uint32 SWITCH_CORE_MULTI_PORT_CTL;
	uint8 dummy89[12];
	uint32 SWITCH_CORE_MULTIPORT_ADDR0;
	uint8 dummy90[60];
	uint32 SWITCH_CORE_MPORTVEC0;
	uint8 dummy91[60];
	uint32 SWITCH_CORE_MULTIPORT_ADDR1;
	uint8 dummy92[60];
	uint32 SWITCH_CORE_MPORTVEC1;
	uint8 dummy93[60];
	uint32 SWITCH_CORE_MULTIPORT_ADDR2;
	uint8 dummy94[60];
	uint32 SWITCH_CORE_MPORTVEC2;
	uint8 dummy95[60];
	uint32 SWITCH_CORE_MULTIPORT_ADDR3;
	uint8 dummy96[60];
	uint32 SWITCH_CORE_MPORTVEC3;
	uint8 dummy97[60];
	uint32 SWITCH_CORE_MULTIPORT_ADDR4;
	uint8 dummy98[60];
	uint32 SWITCH_CORE_MPORTVEC4;
	uint8 dummy99[60];
	uint32 SWITCH_CORE_MULTIPORT_ADDR5;
	uint8 dummy100[60];
	uint32 SWITCH_CORE_MPORTVEC5;
	uint8 dummy101[60];
	uint32 SWITCH_CORE_ARL_BIN_FULL_CNTR;
	uint8 dummy102[28];
	uint32 SWITCH_CORE_ARL_BIN_FULL_FWD;
	uint8 dummy103[12];
	uint32 SWITCH_CORE_ARL_SEED;
	uint8 dummy104[76];
	uint32 SWITCH_CORE_ARLCTL_REG_SPARE0;
	uint8 dummy105[28];
	uint32 SWITCH_CORE_ARLCTL_REG_SPARE1;
	uint8 dummy106[92];
	uint32 SWITCH_CORE_ARL_TCAM_CTRL;
	uint8 dummy107[28];
	uint32 SWITCH_CORE_ARL_TCAM_STS;
	uint8 dummy108[28];
	uint32 SWITCH_CORE_ARL_TCAM_FULL_CNTR;
	uint8 dummy109[28];
	uint32 SWITCH_CORE_ARL_CPU_PORTMAP;
	uint8 dummy110[796];
	uint32 SWITCH_CORE_ARLA_RWCTL;
	uint8 dummy111[12];
	uint32 SWITCH_CORE_ARLA_MAC;
	uint8 dummy112[44];
	uint32 SWITCH_CORE_ARLA_VID;
	uint8 dummy113[60];
	uint32 SWITCH_CORE_ARLA_MACVID_ENTRY0;
	uint8 dummy114[60];
	uint32 SWITCH_CORE_ARLA_FWD_ENTRY0;
	uint8 dummy115[60];
	uint32 SWITCH_CORE_ARLA_MACVID_ENTRY1;
	uint8 dummy116[60];
	uint32 SWITCH_CORE_ARLA_FWD_ENTRY1;
	uint8 dummy117[60];
	uint32 SWITCH_CORE_ARLA_MACVID_ENTRY2;
	uint8 dummy118[60];
	uint32 SWITCH_CORE_ARLA_FWD_ENTRY2;
	uint8 dummy119[60];
	uint32 SWITCH_CORE_ARLA_MACVID_ENTRY3;
	uint8 dummy120[60];
	uint32 SWITCH_CORE_ARLA_FWD_ENTRY3;
	uint8 dummy121[60];
	uint64 SWITCH_CORE_ARLA_SRCH_CTL;
	uint32 SWITCH_CORE_ARLA_SRCH_ADR;
	uint8 dummy122[116];
	uint32 SWITCH_CORE_ARLA_SRCH_RSLT_0_MACVID;
	uint8 dummy123[60];
	uint32 SWITCH_CORE_ARLA_SRCH_RSLT_0;
	uint8 dummy124[60];
	uint32 SWITCH_CORE_ARLA_SRCH_RSLT_1_MACVID;
	uint8 dummy125[60];
	uint32 SWITCH_CORE_ARLA_SRCH_RSLT_1;
	uint8 dummy126[60];
	uint64 SWITCH_CORE_ARLA_VTBL_RWCTRL;
	uint32 SWITCH_CORE_ARLA_VTBL_ADDR;
	uint8 dummy127[12];
	uint32 SWITCH_CORE_ARLA_VTBL_ENTRY;
	uint8 dummy128[100];
	uint32 SWITCH_CORE_ARLACCS_REG_SPARE0;
	uint8 dummy129[28];
	uint32 SWITCH_CORE_ARLACCS_REG_SPARE1;
	uint8 dummy130[4956];
	uint64 SWITCH_CORE_MEM_CTRL;
	uint32 SWITCH_CORE_MEM_ADDR;
	uint8 dummy131[52];
	uint32 SWITCH_CORE_MEM_DEBUG_DATA_0_0;
	uint8 dummy132[60];
	uint32 SWITCH_CORE_MEM_DEBUG_DATA_0_1;
	uint8 dummy133[12];
	uint32 SWITCH_CORE_MEM_DEBUG_DATA_1_0;
	uint8 dummy134[60];
	uint32 SWITCH_CORE_MEM_DEBUG_DATA_1_1;
	uint8 dummy135[44];
	uint32 SWITCH_CORE_MEM_FRM_ADDR;
	uint8 dummy136[124];
	uint32 SWITCH_CORE_MEM_FRM_DATA0;
	uint8 dummy137[60];
	uint32 SWITCH_CORE_MEM_FRM_DATA1;
	uint8 dummy138[60];
	uint32 SWITCH_CORE_MEM_FRM_DATA2;
	uint8 dummy139[60];
	uint32 SWITCH_CORE_MEM_FRM_DATA3;
	uint8 dummy140[60];
	uint32 SWITCH_CORE_MEM_BTM_DATA0;
	uint8 dummy141[60];
	uint32 SWITCH_CORE_MEM_BTM_DATA1;
	uint8 dummy142[60];
	uint32 SWITCH_CORE_MEM_BFC_ADDR;
	uint8 dummy143[12];
	uint32 SWITCH_CORE_MEM_BFC_DATA;
	uint8 dummy144[108];
	uint64 SWITCH_CORE_PRS_FIFO_DEBUG_CTRL;
	uint32 SWITCH_CORE_PRS_FIFO_DEBUG_DATA;
	uint8 dummy145[116];
	uint32 SWITCH_CORE_MIBKILLOVR;
	uint8 dummy146[316];
	uint32 SWITCH_CORE_MEM_REG_SPARE0;
	uint8 dummy147[28];
	uint32 SWITCH_CORE_MEM_REG_SPARE1;
	uint8 dummy148[28];
	uint32 SWITCH_CORE_MEM_MISC_CTRL;
	uint8 dummy149[28];
	uint32 SWITCH_CORE_MEM_TEST_CTRL0;
	uint8 dummy150[28];
	uint32 SWITCH_CORE_MEM_TEST_CTRL1;
	uint8 dummy151[28];
	uint32 SWITCH_CORE_MEM_TEST_CTRL2;
	uint8 dummy152[28];
	uint32 SWITCH_CORE_MEM_TEST_CTRL3;
	uint8 dummy153[28];
	uint32 SWITCH_CORE_MEM_TEST_CTRL4;
	uint8 dummy154[28];
	uint32 SWITCH_CORE_MEM_TEST_CTRL5;
	uint8 dummy155[188];
	uint32 SWITCH_CORE_MEM_PSM_VDD_CTRL;
	uint8 dummy156[252];
	uint32 SWITCH_CORE_PORT0_DEBUG;
	uint8 dummy157[124];
	uint32 SWITCH_CORE_PORT1_DEBUG;
	uint8 dummy158[124];
	uint32 SWITCH_CORE_PORT2_DEBUG;
	uint8 dummy159[124];
	uint32 SWITCH_CORE_PORT3_DEBUG;
	uint8 dummy160[124];
	uint32 SWITCH_CORE_PORT4_DEBUG;
	uint8 dummy161[124];
	uint32 SWITCH_CORE_PORT5_DEBUG;
	uint8 dummy162[124];
	uint32 SWITCH_CORE_PORT6_DEBUG;
	uint8 dummy163[124];
	uint32 SWITCH_CORE_PORT7_DEBUG;
	uint8 dummy164[124];
	uint32 SWITCH_CORE_PORT8_DEBUG;
	uint8 dummy165[1020];
	uint32 SWITCH_CORE_FC_DIAG_CTRL;
	uint8 dummy166[12];
	uint64 SWITCH_CORE_FC_CTRL_MODE;
	uint64 SWITCH_CORE_FC_CTRL_PORT;
	uint32 SWITCH_CORE_FC_OOB_PAUSE_EN;
	uint8 dummy167[92];
	uint32 SWITCH_CORE_PAUSE_TIME_MAX;
	uint8 dummy168[12];
	uint32 SWITCH_CORE_PAUSE_TIME_MIN;
	uint8 dummy169[12];
	uint32 SWITCH_CORE_PAUSE_TIME_RESET_THD;
	uint8 dummy170[12];
	uint32 SWITCH_CORE_PAUSE_TIME_UPDATE_PERIOD;
	uint8 dummy171[12];
	uint32 SWITCH_CORE_PAUSE_TIME_DEFAULT;
	uint8 dummy172[12];
	uint32 SWITCH_CORE_FC_MCAST_DROP_CTRL;
	uint8 dummy173[12];
	uint32 SWITCH_CORE_FC_PAUSE_DROP_CTRL;
	uint8 dummy174[12];
	uint32 SWITCH_CORE_FC_TXQ_THD_PAUSE_OFF;
	uint8 dummy175[12];
	uint32 SWITCH_CORE_FC_RX_RUNOFF;
	uint8 dummy176[12];
	uint32 SWITCH_CORE_FC_RX_RSV_THD;
	uint8 dummy177[12];
	uint32 SWITCH_CORE_FC_RX_HYST_THD;
	uint8 dummy178[12];
	uint32 SWITCH_CORE_FC_RX_MAX_PTR;
	uint8 dummy179[12];
	uint32 SWITCH_CORE_FC_SPARE_ZERO_REG;
	uint8 dummy180[12];
	uint32 SWITCH_CORE_FC_SPARE_ONE_REG;
	uint8 dummy181[44];
	uint32 SWITCH_CORE_FC_MON_TX_Q0;
	uint8 dummy182[12];
	uint32 SWITCH_CORE_FC_MON_TX_Q1;
	uint8 dummy183[12];
	uint32 SWITCH_CORE_FC_MON_TX_Q2;
	uint8 dummy184[12];
	uint32 SWITCH_CORE_FC_MON_TX_Q3;
	uint8 dummy185[12];
	uint32 SWITCH_CORE_FC_MON_TX_Q4;
	uint8 dummy186[12];
	uint32 SWITCH_CORE_FC_MON_TX_Q5;
	uint8 dummy187[12];
	uint32 SWITCH_CORE_FC_MON_TX_Q6;
	uint8 dummy188[12];
	uint32 SWITCH_CORE_FC_MON_TX_Q7;
	uint8 dummy189[12];
	uint32 SWITCH_CORE_FC_PEAK_TX_Q0;
	uint8 dummy190[12];
	uint32 SWITCH_CORE_FC_PEAK_TX_Q1;
	uint8 dummy191[12];
	uint32 SWITCH_CORE_FC_PEAK_TX_Q2;
	uint8 dummy192[12];
	uint32 SWITCH_CORE_FC_PEAK_TX_Q3;
	uint8 dummy193[12];
	uint32 SWITCH_CORE_FC_PEAK_TX_Q4;
	uint8 dummy194[12];
	uint32 SWITCH_CORE_FC_PEAK_TX_Q5;
	uint8 dummy195[12];
	uint32 SWITCH_CORE_FC_PEAK_TX_Q6;
	uint8 dummy196[12];
	uint32 SWITCH_CORE_FC_PEAK_TX_Q7;
	uint8 dummy197[12];
	uint32 SWITCH_CORE_FC_PEAK_TOTAL_USED;
	uint8 dummy198[12];
	uint32 SWITCH_CORE_FC_TOTAL_USED;
	uint8 dummy199[12];
	uint32 SWITCH_CORE_FC_PEAK_RX_CNT;
	uint8 dummy200[12];
	uint32 SWITCH_CORE_FC_LINK_PORTMAP;
	uint8 dummy201[12];
	uint32 SWITCH_CORE_FC_GIGA_PORTMAP;
	uint8 dummy202[60];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P0;
	uint8 dummy203[12];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P1;
	uint8 dummy204[12];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P2;
	uint8 dummy205[12];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P3;
	uint8 dummy206[12];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P4;
	uint8 dummy207[12];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P5;
	uint8 dummy208[12];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P6;
	uint8 dummy209[12];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P7;
	uint8 dummy210[12];
	uint32 SWITCH_CORE_FC_CONG_PORTMAP_P8;
	uint8 dummy211[60];
	uint32 SWITCH_CORE_FC_PAUSE_HIS;
	uint8 dummy212[12];
	uint32 SWITCH_CORE_FC_TX_QUANTUM_PAUSE_HIS;
	uint8 dummy213[12];
	uint32 SWITCH_CORE_FC_RX_PAUSE_HIS;
	uint8 dummy214[12];
	uint32 SWITCH_CORE_FC_RXBUF_ERR_HIS;
	uint8 dummy215[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P0;
	uint8 dummy216[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P1;
	uint8 dummy217[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P2;
	uint8 dummy218[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P3;
	uint8 dummy219[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P4;
	uint8 dummy220[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P5;
	uint8 dummy221[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P6;
	uint8 dummy222[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P7;
	uint8 dummy223[12];
	uint32 SWITCH_CORE_FC_TXQ_CONG_PORTMAP_P8;
	uint8 dummy224[76];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P0;
	uint8 dummy225[12];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P1;
	uint8 dummy226[12];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P2;
	uint8 dummy227[12];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P3;
	uint8 dummy228[12];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P4;
	uint8 dummy229[12];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P5;
	uint8 dummy230[12];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P6;
	uint8 dummy231[12];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P7;
	uint8 dummy232[12];
	uint32 SWITCH_CORE_FC_TOTAL_CONG_PORTMAP_P8;
	uint8 dummy233[684];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_RSV_Q0;
	uint8 dummy234[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_RSV_Q1;
	uint8 dummy235[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_RSV_Q2;
	uint8 dummy236[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_RSV_Q3;
	uint8 dummy237[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_RSV_Q4;
	uint8 dummy238[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_RSV_Q5;
	uint8 dummy239[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_RSV_Q6;
	uint8 dummy240[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_RSV_Q7;
	uint8 dummy241[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_HYST_Q0;
	uint8 dummy242[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_HYST_Q1;
	uint8 dummy243[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_HYST_Q2;
	uint8 dummy244[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_HYST_Q3;
	uint8 dummy245[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_HYST_Q4;
	uint8 dummy246[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_HYST_Q5;
	uint8 dummy247[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_HYST_Q6;
	uint8 dummy248[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_HYST_Q7;
	uint8 dummy249[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_PAUSE_Q0;
	uint8 dummy250[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_PAUSE_Q1;
	uint8 dummy251[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_PAUSE_Q2;
	uint8 dummy252[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_PAUSE_Q3;
	uint8 dummy253[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_PAUSE_Q4;
	uint8 dummy254[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_PAUSE_Q5;
	uint8 dummy255[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_PAUSE_Q6;
	uint8 dummy256[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_PAUSE_Q7;
	uint8 dummy257[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_DROP_Q0;
	uint8 dummy258[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_DROP_Q1;
	uint8 dummy259[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_DROP_Q2;
	uint8 dummy260[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_DROP_Q3;
	uint8 dummy261[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_DROP_Q4;
	uint8 dummy262[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_DROP_Q5;
	uint8 dummy263[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_DROP_Q6;
	uint8 dummy264[12];
	uint32 SWITCH_CORE_FC_LAN_TXQ_THD_DROP_Q7;
	uint8 dummy265[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_HYST_Q0;
	uint8 dummy266[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_HYST_Q1;
	uint8 dummy267[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_HYST_Q2;
	uint8 dummy268[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_HYST_Q3;
	uint8 dummy269[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_HYST_Q4;
	uint8 dummy270[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_HYST_Q5;
	uint8 dummy271[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_HYST_Q6;
	uint8 dummy272[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_HYST_Q7;
	uint8 dummy273[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_PAUSE_Q0;
	uint8 dummy274[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_PAUSE_Q1;
	uint8 dummy275[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_PAUSE_Q2;
	uint8 dummy276[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_PAUSE_Q3;
	uint8 dummy277[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_PAUSE_Q4;
	uint8 dummy278[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_PAUSE_Q5;
	uint8 dummy279[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_PAUSE_Q6;
	uint8 dummy280[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_PAUSE_Q7;
	uint8 dummy281[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_DROP_Q0;
	uint8 dummy282[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_DROP_Q1;
	uint8 dummy283[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_DROP_Q2;
	uint8 dummy284[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_DROP_Q3;
	uint8 dummy285[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_DROP_Q4;
	uint8 dummy286[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_DROP_Q5;
	uint8 dummy287[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_DROP_Q6;
	uint8 dummy288[12];
	uint32 SWITCH_CORE_FC_LAN_TOTAL_THD_DROP_Q7;
	uint8 dummy289[1164];
	uint32 SWITCH_CORE_P0_DEBUG_MUX;
	uint8 dummy290[28];
	uint32 SWITCH_CORE_P1_DEBUG_MUX;
	uint8 dummy291[28];
	uint32 SWITCH_CORE_P2_DEBUG_MUX;
	uint8 dummy292[28];
	uint32 SWITCH_CORE_P3_DEBUG_MUX;
	uint8 dummy293[28];
	uint32 SWITCH_CORE_P4_DEBUG_MUX;
	uint8 dummy294[28];
	uint32 SWITCH_CORE_P5_DEBUG_MUX;
	uint8 dummy295[28];
	uint32 SWITCH_CORE_P6_DEBUG_MUX;
	uint8 dummy296[28];
	uint32 SWITCH_CORE_DEBUG_MUX_P7;
	uint8 dummy297[28];
	uint32 SWITCH_CORE_DEBUG_MUX_IMP;
	uint8 dummy298[28];
	uint32 SWITCH_CORE_CFP_DEBUG_BUS_0;
	uint8 dummy299[28];
	uint32 SWITCH_CORE_CFP_DEBUG_BUS_1;
	uint8 dummy300[28];
	uint32 SWITCH_CORE_WRED_DEBUG_0;
	uint8 dummy301[28];
	uint32 SWITCH_CORE_WRED_DEBUG_1;
	uint8 dummy302[28];
	uint32 SWITCH_CORE_TOP_MISC_DEBUG_0;
	uint8 dummy303[28];
	uint32 SWITCH_CORE_TOP_MISC_DEBUG_1;
	uint8 dummy304[28];
	uint32 SWITCH_CORE_DIAGREG_BUFCON;
	uint8 dummy305[28];
	uint32 SWITCH_CORE_TESTBUS_P1588;
	uint8 dummy306[28];
	uint32 SWITCH_CORE_FLOWCON_DEBUG_BUS;
	uint8 dummy307[1500];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_RSV_Q0;
	uint8 dummy308[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_RSV_Q1;
	uint8 dummy309[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_RSV_Q2;
	uint8 dummy310[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_RSV_Q3;
	uint8 dummy311[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_RSV_Q4;
	uint8 dummy312[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_RSV_Q5;
	uint8 dummy313[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_RSV_Q6;
	uint8 dummy314[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_RSV_Q7;
	uint8 dummy315[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_HYST_Q0;
	uint8 dummy316[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_HYST_Q1;
	uint8 dummy317[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_HYST_Q2;
	uint8 dummy318[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_HYST_Q3;
	uint8 dummy319[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_HYST_Q4;
	uint8 dummy320[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_HYST_Q5;
	uint8 dummy321[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_HYST_Q6;
	uint8 dummy322[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_HYST_Q7;
	uint8 dummy323[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_PAUSE_Q0;
	uint8 dummy324[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_PAUSE_Q1;
	uint8 dummy325[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_PAUSE_Q2;
	uint8 dummy326[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_PAUSE_Q3;
	uint8 dummy327[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_PAUSE_Q4;
	uint8 dummy328[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_PAUSE_Q5;
	uint8 dummy329[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_PAUSE_Q6;
	uint8 dummy330[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_PAUSE_Q7;
	uint8 dummy331[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_DROP_Q0;
	uint8 dummy332[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_DROP_Q1;
	uint8 dummy333[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_DROP_Q2;
	uint8 dummy334[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_DROP_Q3;
	uint8 dummy335[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_DROP_Q4;
	uint8 dummy336[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_DROP_Q5;
	uint8 dummy337[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_DROP_Q6;
	uint8 dummy338[12];
	uint32 SWITCH_CORE_FC_IMP0_TXQ_THD_DROP_Q7;
	uint8 dummy339[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_HYST_Q0;
	uint8 dummy340[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_HYST_Q1;
	uint8 dummy341[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_HYST_Q2;
	uint8 dummy342[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_HYST_Q3;
	uint8 dummy343[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_HYST_Q4;
	uint8 dummy344[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_HYST_Q5;
	uint8 dummy345[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_HYST_Q6;
	uint8 dummy346[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_HYST_Q7;
	uint8 dummy347[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_PAUSE_Q0;
	uint8 dummy348[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_PAUSE_Q1;
	uint8 dummy349[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_PAUSE_Q2;
	uint8 dummy350[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_PAUSE_Q3;
	uint8 dummy351[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_PAUSE_Q4;
	uint8 dummy352[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_PAUSE_Q5;
	uint8 dummy353[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_PAUSE_Q6;
	uint8 dummy354[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_PAUSE_Q7;
	uint8 dummy355[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_DROP_Q0;
	uint8 dummy356[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_DROP_Q1;
	uint8 dummy357[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_DROP_Q2;
	uint8 dummy358[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_DROP_Q3;
	uint8 dummy359[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_DROP_Q4;
	uint8 dummy360[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_DROP_Q5;
	uint8 dummy361[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_DROP_Q6;
	uint8 dummy362[12];
	uint32 SWITCH_CORE_FC_IMP0_TOTAL_THD_DROP_Q7;
	uint8 dummy363[12];
	uint32 SWITCH_CORE_FC_IMP0_REG_SPARE0;
	uint8 dummy364[12];
	uint32 SWITCH_CORE_FC_IMP0_REG_SPARE1;
	uint8 dummy365[1132];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_RSV_Q0;
	uint8 dummy366[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_RSV_Q1;
	uint8 dummy367[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_RSV_Q2;
	uint8 dummy368[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_RSV_Q3;
	uint8 dummy369[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_RSV_Q4;
	uint8 dummy370[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_RSV_Q5;
	uint8 dummy371[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_RSV_Q6;
	uint8 dummy372[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_RSV_Q7;
	uint8 dummy373[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_HYST_Q0;
	uint8 dummy374[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_HYST_Q1;
	uint8 dummy375[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_HYST_Q2;
	uint8 dummy376[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_HYST_Q3;
	uint8 dummy377[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_HYST_Q4;
	uint8 dummy378[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_HYST_Q5;
	uint8 dummy379[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_HYST_Q6;
	uint8 dummy380[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_HYST_Q7;
	uint8 dummy381[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_PAUSE_Q0;
	uint8 dummy382[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_PAUSE_Q1;
	uint8 dummy383[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_PAUSE_Q2;
	uint8 dummy384[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_PAUSE_Q3;
	uint8 dummy385[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_PAUSE_Q4;
	uint8 dummy386[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_PAUSE_Q5;
	uint8 dummy387[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_PAUSE_Q6;
	uint8 dummy388[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_PAUSE_Q7;
	uint8 dummy389[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_DROP_Q0;
	uint8 dummy390[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_DROP_Q1;
	uint8 dummy391[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_DROP_Q2;
	uint8 dummy392[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_DROP_Q3;
	uint8 dummy393[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_DROP_Q4;
	uint8 dummy394[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_DROP_Q5;
	uint8 dummy395[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_DROP_Q6;
	uint8 dummy396[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TXQ_THD_DROP_Q7;
	uint8 dummy397[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_HYST_Q0;
	uint8 dummy398[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_HYST_Q1;
	uint8 dummy399[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_HYST_Q2;
	uint8 dummy400[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_HYST_Q3;
	uint8 dummy401[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_HYST_Q4;
	uint8 dummy402[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_HYST_Q5;
	uint8 dummy403[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_HYST_Q6;
	uint8 dummy404[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_HYST_Q7;
	uint8 dummy405[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_PAUSE_Q0;
	uint8 dummy406[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_PAUSE_Q1;
	uint8 dummy407[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_PAUSE_Q2;
	uint8 dummy408[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_PAUSE_Q3;
	uint8 dummy409[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_PAUSE_Q4;
	uint8 dummy410[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_PAUSE_Q5;
	uint8 dummy411[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_PAUSE_Q6;
	uint8 dummy412[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_PAUSE_Q7;
	uint8 dummy413[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_DROP_Q0;
	uint8 dummy414[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_DROP_Q1;
	uint8 dummy415[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_DROP_Q2;
	uint8 dummy416[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_DROP_Q3;
	uint8 dummy417[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_DROP_Q4;
	uint8 dummy418[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_DROP_Q5;
	uint8 dummy419[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_DROP_Q6;
	uint8 dummy420[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_TOTAL_THD_DROP_Q7;
	uint8 dummy421[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_REG_SPARE0;
	uint8 dummy422[12];
	uint32 SWITCH_CORE_FC_WAN_IMP1_REG_SPARE1;
	uint8 dummy423[35948];
	uint32 SWITCH_CORE_TxOctets_P0;
	uint8 dummy424[60];
	uint32 SWITCH_CORE_TxDropPkts_P0;
	uint8 dummy425[28];
	uint32 SWITCH_CORE_TxQPKTQ0_P0;
	uint8 dummy426[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_P0;
	uint8 dummy427[28];
	uint32 SWITCH_CORE_TxMulticastPkts_P0;
	uint8 dummy428[28];
	uint32 SWITCH_CORE_TxUnicastPkts_P0;
	uint8 dummy429[28];
	uint32 SWITCH_CORE_TxCollisions_P0;
	uint8 dummy430[28];
	uint32 SWITCH_CORE_TxSingleCollision_P0;
	uint8 dummy431[28];
	uint32 SWITCH_CORE_TxMultipleCollision_P0;
	uint8 dummy432[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_P0;
	uint8 dummy433[28];
	uint32 SWITCH_CORE_TxLateCollision_P0;
	uint8 dummy434[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_P0;
	uint8 dummy435[28];
	uint32 SWITCH_CORE_TxFrameInDisc_P0;
	uint8 dummy436[28];
	uint32 SWITCH_CORE_TxPausePkts_P0;
	uint8 dummy437[28];
	uint32 SWITCH_CORE_TxQPKTQ1_P0;
	uint8 dummy438[28];
	uint32 SWITCH_CORE_TxQPKTQ2_P0;
	uint8 dummy439[28];
	uint32 SWITCH_CORE_TxQPKTQ3_P0;
	uint8 dummy440[28];
	uint32 SWITCH_CORE_TxQPKTQ4_P0;
	uint8 dummy441[28];
	uint32 SWITCH_CORE_TxQPKTQ5_P0;
	uint8 dummy442[28];
	uint32 SWITCH_CORE_RxOctets_P0;
	uint8 dummy443[60];
	uint32 SWITCH_CORE_RxUndersizePkts_P0;
	uint8 dummy444[28];
	uint32 SWITCH_CORE_RxPausePkts_P0;
	uint8 dummy445[28];
	uint32 SWITCH_CORE_RxPkts64Octets_P0;
	uint8 dummy446[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_P0;
	uint8 dummy447[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_P0;
	uint8 dummy448[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_P0;
	uint8 dummy449[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_P0;
	uint8 dummy450[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_P0;
	uint8 dummy451[28];
	uint32 SWITCH_CORE_RxOversizePkts_P0;
	uint8 dummy452[28];
	uint32 SWITCH_CORE_RxJabbers_P0;
	uint8 dummy453[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_P0;
	uint8 dummy454[28];
	uint32 SWITCH_CORE_RxFCSErrors_P0;
	uint8 dummy455[28];
	uint32 SWITCH_CORE_RxGoodOctets_P0;
	uint8 dummy456[60];
	uint32 SWITCH_CORE_RxDropPkts_P0;
	uint8 dummy457[28];
	uint32 SWITCH_CORE_RxUnicastPkts_P0;
	uint8 dummy458[28];
	uint32 SWITCH_CORE_RxMulticastPkts_P0;
	uint8 dummy459[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_P0;
	uint8 dummy460[28];
	uint32 SWITCH_CORE_RxSAChanges_P0;
	uint8 dummy461[28];
	uint32 SWITCH_CORE_RxFragments_P0;
	uint8 dummy462[28];
	uint32 SWITCH_CORE_RxJumboPkt_P0;
	uint8 dummy463[28];
	uint32 SWITCH_CORE_RxSymblErr_P0;
	uint8 dummy464[28];
	uint32 SWITCH_CORE_InRangeErrCount_P0;
	uint8 dummy465[28];
	uint32 SWITCH_CORE_OutRangeErrCount_P0;
	uint8 dummy466[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_P0;
	uint8 dummy467[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_P0;
	uint8 dummy468[28];
	uint32 SWITCH_CORE_RxDiscard_P0;
	uint8 dummy469[60];
	uint32 SWITCH_CORE_TxQPKTQ6_P0;
	uint8 dummy470[28];
	uint32 SWITCH_CORE_TxQPKTQ7_P0;
	uint8 dummy471[28];
	uint32 SWITCH_CORE_TxPkts64Octets_P0;
	uint8 dummy472[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_P0;
	uint8 dummy473[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_P0;
	uint8 dummy474[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_P0;
	uint8 dummy475[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_P0;
	uint8 dummy476[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_P0;
	uint8 dummy477[220];
	uint32 SWITCH_CORE_TxOctets_P1;
	uint8 dummy478[60];
	uint32 SWITCH_CORE_TxDropPkts_P1;
	uint8 dummy479[28];
	uint32 SWITCH_CORE_TxQPKTQ0_P1;
	uint8 dummy480[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_P1;
	uint8 dummy481[28];
	uint32 SWITCH_CORE_TxMulticastPkts_P1;
	uint8 dummy482[28];
	uint32 SWITCH_CORE_TxUnicastPkts_P1;
	uint8 dummy483[28];
	uint32 SWITCH_CORE_TxCollisions_P1;
	uint8 dummy484[28];
	uint32 SWITCH_CORE_TxSingleCollision_P1;
	uint8 dummy485[28];
	uint32 SWITCH_CORE_TxMultipleCollision_P1;
	uint8 dummy486[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_P1;
	uint8 dummy487[28];
	uint32 SWITCH_CORE_TxLateCollision_P1;
	uint8 dummy488[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_P1;
	uint8 dummy489[28];
	uint32 SWITCH_CORE_TxFrameInDisc_P1;
	uint8 dummy490[28];
	uint32 SWITCH_CORE_TxPausePkts_P1;
	uint8 dummy491[28];
	uint32 SWITCH_CORE_TxQPKTQ1_P1;
	uint8 dummy492[28];
	uint32 SWITCH_CORE_TxQPKTQ2_P1;
	uint8 dummy493[28];
	uint32 SWITCH_CORE_TxQPKTQ3_P1;
	uint8 dummy494[28];
	uint32 SWITCH_CORE_TxQPKTQ4_P1;
	uint8 dummy495[28];
	uint32 SWITCH_CORE_TxQPKTQ5_P1;
	uint8 dummy496[28];
	uint32 SWITCH_CORE_RxOctets_P1;
	uint8 dummy497[60];
	uint32 SWITCH_CORE_RxUndersizePkts_P1;
	uint8 dummy498[28];
	uint32 SWITCH_CORE_RxPausePkts_P1;
	uint8 dummy499[28];
	uint32 SWITCH_CORE_RxPkts64Octets_P1;
	uint8 dummy500[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_P1;
	uint8 dummy501[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_P1;
	uint8 dummy502[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_P1;
	uint8 dummy503[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_P1;
	uint8 dummy504[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_P1;
	uint8 dummy505[28];
	uint32 SWITCH_CORE_RxOversizePkts_P1;
	uint8 dummy506[28];
	uint32 SWITCH_CORE_RxJabbers_P1;
	uint8 dummy507[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_P1;
	uint8 dummy508[28];
	uint32 SWITCH_CORE_RxFCSErrors_P1;
	uint8 dummy509[28];
	uint32 SWITCH_CORE_RxGoodOctets_P1;
	uint8 dummy510[60];
	uint32 SWITCH_CORE_RxDropPkts_P1;
	uint8 dummy511[28];
	uint32 SWITCH_CORE_RxUnicastPkts_P1;
	uint8 dummy512[28];
	uint32 SWITCH_CORE_RxMulticastPkts_P1;
	uint8 dummy513[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_P1;
	uint8 dummy514[28];
	uint32 SWITCH_CORE_RxSAChanges_P1;
	uint8 dummy515[28];
	uint32 SWITCH_CORE_RxFragments_P1;
	uint8 dummy516[28];
	uint32 SWITCH_CORE_RxJumboPkt_P1;
	uint8 dummy517[28];
	uint32 SWITCH_CORE_RxSymblErr_P1;
	uint8 dummy518[28];
	uint32 SWITCH_CORE_InRangeErrCount_P1;
	uint8 dummy519[28];
	uint32 SWITCH_CORE_OutRangeErrCount_P1;
	uint8 dummy520[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_P1;
	uint8 dummy521[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_P1;
	uint8 dummy522[28];
	uint32 SWITCH_CORE_RxDiscard_P1;
	uint8 dummy523[60];
	uint32 SWITCH_CORE_TxQPKTQ6_P1;
	uint8 dummy524[28];
	uint32 SWITCH_CORE_TxQPKTQ7_P1;
	uint8 dummy525[28];
	uint32 SWITCH_CORE_TxPkts64Octets_P1;
	uint8 dummy526[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_P1;
	uint8 dummy527[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_P1;
	uint8 dummy528[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_P1;
	uint8 dummy529[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_P1;
	uint8 dummy530[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_P1;
	uint8 dummy531[220];
	uint32 SWITCH_CORE_TxOctets_P2;
	uint8 dummy532[60];
	uint32 SWITCH_CORE_TxDropPkts_P2;
	uint8 dummy533[28];
	uint32 SWITCH_CORE_TxQPKTQ0_P2;
	uint8 dummy534[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_P2;
	uint8 dummy535[28];
	uint32 SWITCH_CORE_TxMulticastPkts_P2;
	uint8 dummy536[28];
	uint32 SWITCH_CORE_TxUnicastPkts_P2;
	uint8 dummy537[28];
	uint32 SWITCH_CORE_TxCollisions_P2;
	uint8 dummy538[28];
	uint32 SWITCH_CORE_TxSingleCollision_P2;
	uint8 dummy539[28];
	uint32 SWITCH_CORE_TxMultipleCollision_P2;
	uint8 dummy540[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_P2;
	uint8 dummy541[28];
	uint32 SWITCH_CORE_TxLateCollision_P2;
	uint8 dummy542[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_P2;
	uint8 dummy543[28];
	uint32 SWITCH_CORE_TxFrameInDisc_P2;
	uint8 dummy544[28];
	uint32 SWITCH_CORE_TxPausePkts_P2;
	uint8 dummy545[28];
	uint32 SWITCH_CORE_TxQPKTQ1_P2;
	uint8 dummy546[28];
	uint32 SWITCH_CORE_TxQPKTQ2_P2;
	uint8 dummy547[28];
	uint32 SWITCH_CORE_TxQPKTQ3_P2;
	uint8 dummy548[28];
	uint32 SWITCH_CORE_TxQPKTQ4_P2;
	uint8 dummy549[28];
	uint32 SWITCH_CORE_TxQPKTQ5_P2;
	uint8 dummy550[28];
	uint32 SWITCH_CORE_RxOctets_P2;
	uint8 dummy551[60];
	uint32 SWITCH_CORE_RxUndersizePkts_P2;
	uint8 dummy552[28];
	uint32 SWITCH_CORE_RxPausePkts_P2;
	uint8 dummy553[28];
	uint32 SWITCH_CORE_RxPkts64Octets_P2;
	uint8 dummy554[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_P2;
	uint8 dummy555[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_P2;
	uint8 dummy556[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_P2;
	uint8 dummy557[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_P2;
	uint8 dummy558[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_P2;
	uint8 dummy559[28];
	uint32 SWITCH_CORE_RxOversizePkts_P2;
	uint8 dummy560[28];
	uint32 SWITCH_CORE_RxJabbers_P2;
	uint8 dummy561[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_P2;
	uint8 dummy562[28];
	uint32 SWITCH_CORE_RxFCSErrors_P2;
	uint8 dummy563[28];
	uint32 SWITCH_CORE_RxGoodOctets_P2;
	uint8 dummy564[60];
	uint32 SWITCH_CORE_RxDropPkts_P2;
	uint8 dummy565[28];
	uint32 SWITCH_CORE_RxUnicastPkts_P2;
	uint8 dummy566[28];
	uint32 SWITCH_CORE_RxMulticastPkts_P2;
	uint8 dummy567[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_P2;
	uint8 dummy568[28];
	uint32 SWITCH_CORE_RxSAChanges_P2;
	uint8 dummy569[28];
	uint32 SWITCH_CORE_RxFragments_P2;
	uint8 dummy570[28];
	uint32 SWITCH_CORE_RxJumboPkt_P2;
	uint8 dummy571[28];
	uint32 SWITCH_CORE_RxSymblErr_P2;
	uint8 dummy572[28];
	uint32 SWITCH_CORE_InRangeErrCount_P2;
	uint8 dummy573[28];
	uint32 SWITCH_CORE_OutRangeErrCount_P2;
	uint8 dummy574[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_P2;
	uint8 dummy575[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_P2;
	uint8 dummy576[28];
	uint32 SWITCH_CORE_RxDiscard_P2;
	uint8 dummy577[60];
	uint32 SWITCH_CORE_TxQPKTQ6_P2;
	uint8 dummy578[28];
	uint32 SWITCH_CORE_TxQPKTQ7_P2;
	uint8 dummy579[28];
	uint32 SWITCH_CORE_TxPkts64Octets_P2;
	uint8 dummy580[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_P2;
	uint8 dummy581[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_P2;
	uint8 dummy582[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_P2;
	uint8 dummy583[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_P2;
	uint8 dummy584[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_P2;
	uint8 dummy585[220];
	uint32 SWITCH_CORE_TxOctets_P3;
	uint8 dummy586[60];
	uint32 SWITCH_CORE_TxDropPkts_P3;
	uint8 dummy587[28];
	uint32 SWITCH_CORE_TxQPKTQ0_P3;
	uint8 dummy588[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_P3;
	uint8 dummy589[28];
	uint32 SWITCH_CORE_TxMulticastPkts_P3;
	uint8 dummy590[28];
	uint32 SWITCH_CORE_TxUnicastPkts_P3;
	uint8 dummy591[28];
	uint32 SWITCH_CORE_TxCollisions_P3;
	uint8 dummy592[28];
	uint32 SWITCH_CORE_TxSingleCollision_P3;
	uint8 dummy593[28];
	uint32 SWITCH_CORE_TxMultipleCollision_P3;
	uint8 dummy594[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_P3;
	uint8 dummy595[28];
	uint32 SWITCH_CORE_TxLateCollision_P3;
	uint8 dummy596[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_P3;
	uint8 dummy597[28];
	uint32 SWITCH_CORE_TxFrameInDisc_P3;
	uint8 dummy598[28];
	uint32 SWITCH_CORE_TxPausePkts_P3;
	uint8 dummy599[28];
	uint32 SWITCH_CORE_TxQPKTQ1_P3;
	uint8 dummy600[28];
	uint32 SWITCH_CORE_TxQPKTQ2_P3;
	uint8 dummy601[28];
	uint32 SWITCH_CORE_TxQPKTQ3_P3;
	uint8 dummy602[28];
	uint32 SWITCH_CORE_TxQPKTQ4_P3;
	uint8 dummy603[28];
	uint32 SWITCH_CORE_TxQPKTQ5_P3;
	uint8 dummy604[28];
	uint32 SWITCH_CORE_RxOctets_P3;
	uint8 dummy605[60];
	uint32 SWITCH_CORE_RxUndersizePkts_P3;
	uint8 dummy606[28];
	uint32 SWITCH_CORE_RxPausePkts_P3;
	uint8 dummy607[28];
	uint32 SWITCH_CORE_RxPkts64Octets_P3;
	uint8 dummy608[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_P3;
	uint8 dummy609[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_P3;
	uint8 dummy610[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_P3;
	uint8 dummy611[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_P3;
	uint8 dummy612[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_P3;
	uint8 dummy613[28];
	uint32 SWITCH_CORE_RxOversizePkts_P3;
	uint8 dummy614[28];
	uint32 SWITCH_CORE_RxJabbers_P3;
	uint8 dummy615[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_P3;
	uint8 dummy616[28];
	uint32 SWITCH_CORE_RxFCSErrors_P3;
	uint8 dummy617[28];
	uint32 SWITCH_CORE_RxGoodOctets_P3;
	uint8 dummy618[60];
	uint32 SWITCH_CORE_RxDropPkts_P3;
	uint8 dummy619[28];
	uint32 SWITCH_CORE_RxUnicastPkts_P3;
	uint8 dummy620[28];
	uint32 SWITCH_CORE_RxMulticastPkts_P3;
	uint8 dummy621[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_P3;
	uint8 dummy622[28];
	uint32 SWITCH_CORE_RxSAChanges_P3;
	uint8 dummy623[28];
	uint32 SWITCH_CORE_RxFragments_P3;
	uint8 dummy624[28];
	uint32 SWITCH_CORE_RxJumboPkt_P3;
	uint8 dummy625[28];
	uint32 SWITCH_CORE_RxSymblErr_P3;
	uint8 dummy626[28];
	uint32 SWITCH_CORE_InRangeErrCount_P3;
	uint8 dummy627[28];
	uint32 SWITCH_CORE_OutRangeErrCount_P3;
	uint8 dummy628[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_P3;
	uint8 dummy629[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_P3;
	uint8 dummy630[28];
	uint32 SWITCH_CORE_RxDiscard_P3;
	uint8 dummy631[60];
	uint32 SWITCH_CORE_TxQPKTQ6_P3;
	uint8 dummy632[28];
	uint32 SWITCH_CORE_TxQPKTQ7_P3;
	uint8 dummy633[28];
	uint32 SWITCH_CORE_TxPkts64Octets_P3;
	uint8 dummy634[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_P3;
	uint8 dummy635[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_P3;
	uint8 dummy636[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_P3;
	uint8 dummy637[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_P3;
	uint8 dummy638[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_P3;
	uint8 dummy639[220];
	uint32 SWITCH_CORE_TxOctets_P4;
	uint8 dummy640[60];
	uint32 SWITCH_CORE_TxDropPkts_P4;
	uint8 dummy641[28];
	uint32 SWITCH_CORE_TxQPKTQ0_P4;
	uint8 dummy642[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_P4;
	uint8 dummy643[28];
	uint32 SWITCH_CORE_TxMulticastPkts_P4;
	uint8 dummy644[28];
	uint32 SWITCH_CORE_TxUnicastPkts_P4;
	uint8 dummy645[28];
	uint32 SWITCH_CORE_TxCollisions_P4;
	uint8 dummy646[28];
	uint32 SWITCH_CORE_TxSingleCollision_P4;
	uint8 dummy647[28];
	uint32 SWITCH_CORE_TxMultipleCollision_P4;
	uint8 dummy648[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_P4;
	uint8 dummy649[28];
	uint32 SWITCH_CORE_TxLateCollision_P4;
	uint8 dummy650[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_P4;
	uint8 dummy651[28];
	uint32 SWITCH_CORE_TxFrameInDisc_P4;
	uint8 dummy652[28];
	uint32 SWITCH_CORE_TxPausePkts_P4;
	uint8 dummy653[28];
	uint32 SWITCH_CORE_TxQPKTQ1_P4;
	uint8 dummy654[28];
	uint32 SWITCH_CORE_TxQPKTQ2_P4;
	uint8 dummy655[28];
	uint32 SWITCH_CORE_TxQPKTQ3_P4;
	uint8 dummy656[28];
	uint32 SWITCH_CORE_TxQPKTQ4_P4;
	uint8 dummy657[28];
	uint32 SWITCH_CORE_TxQPKTQ5_P4;
	uint8 dummy658[28];
	uint32 SWITCH_CORE_RxOctets_P4;
	uint8 dummy659[60];
	uint32 SWITCH_CORE_RxUndersizePkts_P4;
	uint8 dummy660[28];
	uint32 SWITCH_CORE_RxPausePkts_P4;
	uint8 dummy661[28];
	uint32 SWITCH_CORE_RxPkts64Octets_P4;
	uint8 dummy662[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_P4;
	uint8 dummy663[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_P4;
	uint8 dummy664[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_P4;
	uint8 dummy665[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_P4;
	uint8 dummy666[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_P4;
	uint8 dummy667[28];
	uint32 SWITCH_CORE_RxOversizePkts_P4;
	uint8 dummy668[28];
	uint32 SWITCH_CORE_RxJabbers_P4;
	uint8 dummy669[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_P4;
	uint8 dummy670[28];
	uint32 SWITCH_CORE_RxFCSErrors_P4;
	uint8 dummy671[28];
	uint32 SWITCH_CORE_RxGoodOctets_P4;
	uint8 dummy672[60];
	uint32 SWITCH_CORE_RxDropPkts_P4;
	uint8 dummy673[28];
	uint32 SWITCH_CORE_RxUnicastPkts_P4;
	uint8 dummy674[28];
	uint32 SWITCH_CORE_RxMulticastPkts_P4;
	uint8 dummy675[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_P4;
	uint8 dummy676[28];
	uint32 SWITCH_CORE_RxSAChanges_P4;
	uint8 dummy677[28];
	uint32 SWITCH_CORE_RxFragments_P4;
	uint8 dummy678[28];
	uint32 SWITCH_CORE_RxJumboPkt_P4;
	uint8 dummy679[28];
	uint32 SWITCH_CORE_RxSymblErr_P4;
	uint8 dummy680[28];
	uint32 SWITCH_CORE_InRangeErrCount_P4;
	uint8 dummy681[28];
	uint32 SWITCH_CORE_OutRangeErrCount_P4;
	uint8 dummy682[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_P4;
	uint8 dummy683[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_P4;
	uint8 dummy684[28];
	uint32 SWITCH_CORE_RxDiscard_P4;
	uint8 dummy685[60];
	uint32 SWITCH_CORE_TxQPKTQ6_P4;
	uint8 dummy686[28];
	uint32 SWITCH_CORE_TxQPKTQ7_P4;
	uint8 dummy687[28];
	uint32 SWITCH_CORE_TxPkts64Octets_P4;
	uint8 dummy688[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_P4;
	uint8 dummy689[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_P4;
	uint8 dummy690[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_P4;
	uint8 dummy691[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_P4;
	uint8 dummy692[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_P4;
	uint8 dummy693[220];
	uint32 SWITCH_CORE_TxOctets_P5;
	uint8 dummy694[60];
	uint32 SWITCH_CORE_TxDropPkts_P5;
	uint8 dummy695[28];
	uint32 SWITCH_CORE_TxQPKTQ0_P5;
	uint8 dummy696[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_P5;
	uint8 dummy697[28];
	uint32 SWITCH_CORE_TxMulticastPkts_P5;
	uint8 dummy698[28];
	uint32 SWITCH_CORE_TxUnicastPkts_P5;
	uint8 dummy699[28];
	uint32 SWITCH_CORE_TxCollisions_P5;
	uint8 dummy700[28];
	uint32 SWITCH_CORE_TxSingleCollision_P5;
	uint8 dummy701[28];
	uint32 SWITCH_CORE_TxMultipleCollision_P5;
	uint8 dummy702[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_P5;
	uint8 dummy703[28];
	uint32 SWITCH_CORE_TxLateCollision_P5;
	uint8 dummy704[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_P5;
	uint8 dummy705[28];
	uint32 SWITCH_CORE_TxFrameInDisc_P5;
	uint8 dummy706[28];
	uint32 SWITCH_CORE_TxPausePkts_P5;
	uint8 dummy707[28];
	uint32 SWITCH_CORE_TxQPKTQ1_P5;
	uint8 dummy708[28];
	uint32 SWITCH_CORE_TxQPKTQ2_P5;
	uint8 dummy709[28];
	uint32 SWITCH_CORE_TxQPKTQ3_P5;
	uint8 dummy710[28];
	uint32 SWITCH_CORE_TxQPKTQ4_P5;
	uint8 dummy711[28];
	uint32 SWITCH_CORE_TxQPKTQ5_P5;
	uint8 dummy712[28];
	uint32 SWITCH_CORE_RxOctets_P5;
	uint8 dummy713[60];
	uint32 SWITCH_CORE_RxUndersizePkts_P5;
	uint8 dummy714[28];
	uint32 SWITCH_CORE_RxPausePkts_P5;
	uint8 dummy715[28];
	uint32 SWITCH_CORE_RxPkts64Octets_P5;
	uint8 dummy716[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_P5;
	uint8 dummy717[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_P5;
	uint8 dummy718[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_P5;
	uint8 dummy719[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_P5;
	uint8 dummy720[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_P5;
	uint8 dummy721[28];
	uint32 SWITCH_CORE_RxOversizePkts_P5;
	uint8 dummy722[28];
	uint32 SWITCH_CORE_RxJabbers_P5;
	uint8 dummy723[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_P5;
	uint8 dummy724[28];
	uint32 SWITCH_CORE_RxFCSErrors_P5;
	uint8 dummy725[28];
	uint32 SWITCH_CORE_RxGoodOctets_P5;
	uint8 dummy726[60];
	uint32 SWITCH_CORE_RxDropPkts_P5;
	uint8 dummy727[28];
	uint32 SWITCH_CORE_RxUnicastPkts_P5;
	uint8 dummy728[28];
	uint32 SWITCH_CORE_RxMulticastPkts_P5;
	uint8 dummy729[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_P5;
	uint8 dummy730[28];
	uint32 SWITCH_CORE_RxSAChanges_P5;
	uint8 dummy731[28];
	uint32 SWITCH_CORE_RxFragments_P5;
	uint8 dummy732[28];
	uint32 SWITCH_CORE_RxJumboPkt_P5;
	uint8 dummy733[28];
	uint32 SWITCH_CORE_RxSymblErr_P5;
	uint8 dummy734[28];
	uint32 SWITCH_CORE_InRangeErrCount_P5;
	uint8 dummy735[28];
	uint32 SWITCH_CORE_OutRangeErrCount_P5;
	uint8 dummy736[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_P5;
	uint8 dummy737[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_P5;
	uint8 dummy738[28];
	uint32 SWITCH_CORE_RxDiscard_P5;
	uint8 dummy739[60];
	uint32 SWITCH_CORE_TxQPKTQ6_P5;
	uint8 dummy740[28];
	uint32 SWITCH_CORE_TxQPKTQ7_P5;
	uint8 dummy741[28];
	uint32 SWITCH_CORE_TxPkts64Octets_P5;
	uint8 dummy742[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_P5;
	uint8 dummy743[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_P5;
	uint8 dummy744[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_P5;
	uint8 dummy745[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_P5;
	uint8 dummy746[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_P5;
	uint8 dummy747[220];
	uint32 SWITCH_CORE_TxOctets_P6;
	uint8 dummy748[60];
	uint32 SWITCH_CORE_TxDropPkts_P6;
	uint8 dummy749[28];
	uint32 SWITCH_CORE_TxQPKTQ0_P6;
	uint8 dummy750[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_P6;
	uint8 dummy751[28];
	uint32 SWITCH_CORE_TxMulticastPkts_P6;
	uint8 dummy752[28];
	uint32 SWITCH_CORE_TxUnicastPkts_P6;
	uint8 dummy753[28];
	uint32 SWITCH_CORE_TxCollisions_P6;
	uint8 dummy754[28];
	uint32 SWITCH_CORE_TxSingleCollision_P6;
	uint8 dummy755[28];
	uint32 SWITCH_CORE_TxMultipleCollision_P6;
	uint8 dummy756[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_P6;
	uint8 dummy757[28];
	uint32 SWITCH_CORE_TxLateCollision_P6;
	uint8 dummy758[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_P6;
	uint8 dummy759[28];
	uint32 SWITCH_CORE_TxFrameInDisc_P6;
	uint8 dummy760[28];
	uint32 SWITCH_CORE_TxPausePkts_P6;
	uint8 dummy761[28];
	uint32 SWITCH_CORE_TxQPKTQ1_P6;
	uint8 dummy762[28];
	uint32 SWITCH_CORE_TxQPKTQ2_P6;
	uint8 dummy763[28];
	uint32 SWITCH_CORE_TxQPKTQ3_P6;
	uint8 dummy764[28];
	uint32 SWITCH_CORE_TxQPKTQ4_P6;
	uint8 dummy765[28];
	uint32 SWITCH_CORE_TxQPKTQ5_P6;
	uint8 dummy766[28];
	uint32 SWITCH_CORE_RxOctets_P6;
	uint8 dummy767[60];
	uint32 SWITCH_CORE_RxUndersizePkts_P6;
	uint8 dummy768[28];
	uint32 SWITCH_CORE_RxPausePkts_P6;
	uint8 dummy769[28];
	uint32 SWITCH_CORE_RxPkts64Octets_P6;
	uint8 dummy770[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_P6;
	uint8 dummy771[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_P6;
	uint8 dummy772[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_P6;
	uint8 dummy773[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_P6;
	uint8 dummy774[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_P6;
	uint8 dummy775[28];
	uint32 SWITCH_CORE_RxOversizePkts_P6;
	uint8 dummy776[28];
	uint32 SWITCH_CORE_RxJabbers_P6;
	uint8 dummy777[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_P6;
	uint8 dummy778[28];
	uint32 SWITCH_CORE_RxFCSErrors_P6;
	uint8 dummy779[28];
	uint32 SWITCH_CORE_RxGoodOctets_P6;
	uint8 dummy780[60];
	uint32 SWITCH_CORE_RxDropPkts_P6;
	uint8 dummy781[28];
	uint32 SWITCH_CORE_RxUnicastPkts_P6;
	uint8 dummy782[28];
	uint32 SWITCH_CORE_RxMulticastPkts_P6;
	uint8 dummy783[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_P6;
	uint8 dummy784[28];
	uint32 SWITCH_CORE_RxSAChanges_P6;
	uint8 dummy785[28];
	uint32 SWITCH_CORE_RxFragments_P6;
	uint8 dummy786[28];
	uint32 SWITCH_CORE_RxJumboPkt_P6;
	uint8 dummy787[28];
	uint32 SWITCH_CORE_RxSymblErr_P6;
	uint8 dummy788[28];
	uint32 SWITCH_CORE_InRangeErrCount_P6;
	uint8 dummy789[28];
	uint32 SWITCH_CORE_OutRangeErrCount_P6;
	uint8 dummy790[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_P6;
	uint8 dummy791[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_P6;
	uint8 dummy792[28];
	uint32 SWITCH_CORE_RxDiscard_P6;
	uint8 dummy793[60];
	uint32 SWITCH_CORE_TxQPKTQ6_P6;
	uint8 dummy794[28];
	uint32 SWITCH_CORE_TxQPKTQ7_P6;
	uint8 dummy795[28];
	uint32 SWITCH_CORE_TxPkts64Octets_P6;
	uint8 dummy796[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_P6;
	uint8 dummy797[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_P6;
	uint8 dummy798[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_P6;
	uint8 dummy799[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_P6;
	uint8 dummy800[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_P6;
	uint8 dummy801[220];
	uint32 SWITCH_CORE_TxOctets_P7;
	uint8 dummy802[60];
	uint32 SWITCH_CORE_TxDropPkts_P7;
	uint8 dummy803[28];
	uint32 SWITCH_CORE_TxQPKTQ0_P7;
	uint8 dummy804[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_P7;
	uint8 dummy805[28];
	uint32 SWITCH_CORE_TxMulticastPkts_P7;
	uint8 dummy806[28];
	uint32 SWITCH_CORE_TxUnicastPkts_P7;
	uint8 dummy807[28];
	uint32 SWITCH_CORE_TxCollisions_P7;
	uint8 dummy808[28];
	uint32 SWITCH_CORE_TxSingleCollision_P7;
	uint8 dummy809[28];
	uint32 SWITCH_CORE_TxMultipleCollision_P7;
	uint8 dummy810[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_P7;
	uint8 dummy811[28];
	uint32 SWITCH_CORE_TxLateCollision_P7;
	uint8 dummy812[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_P7;
	uint8 dummy813[28];
	uint32 SWITCH_CORE_TxFrameInDisc_P7;
	uint8 dummy814[28];
	uint32 SWITCH_CORE_TxPausePkts_P7;
	uint8 dummy815[28];
	uint32 SWITCH_CORE_TxQPKTQ1_P7;
	uint8 dummy816[28];
	uint32 SWITCH_CORE_TxQPKTQ2_P7;
	uint8 dummy817[28];
	uint32 SWITCH_CORE_TxQPKTQ3_P7;
	uint8 dummy818[28];
	uint32 SWITCH_CORE_TxQPKTQ4_P7;
	uint8 dummy819[28];
	uint32 SWITCH_CORE_TxQPKTQ5_P7;
	uint8 dummy820[28];
	uint32 SWITCH_CORE_RxOctets_P7;
	uint8 dummy821[60];
	uint32 SWITCH_CORE_RxUndersizePkts_P7;
	uint8 dummy822[28];
	uint32 SWITCH_CORE_RxPausePkts_P7;
	uint8 dummy823[28];
	uint32 SWITCH_CORE_RxPkts64Octets_P7;
	uint8 dummy824[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_P7;
	uint8 dummy825[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_P7;
	uint8 dummy826[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_P7;
	uint8 dummy827[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_P7;
	uint8 dummy828[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_P7;
	uint8 dummy829[28];
	uint32 SWITCH_CORE_RxOversizePkts_P7;
	uint8 dummy830[28];
	uint32 SWITCH_CORE_RxJabbers_P7;
	uint8 dummy831[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_P7;
	uint8 dummy832[28];
	uint32 SWITCH_CORE_RxFCSErrors_P7;
	uint8 dummy833[28];
	uint32 SWITCH_CORE_RxGoodOctets_P7;
	uint8 dummy834[60];
	uint32 SWITCH_CORE_RxDropPkts_P7;
	uint8 dummy835[28];
	uint32 SWITCH_CORE_RxUnicastPkts_P7;
	uint8 dummy836[28];
	uint32 SWITCH_CORE_RxMulticastPkts_P7;
	uint8 dummy837[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_P7;
	uint8 dummy838[28];
	uint32 SWITCH_CORE_RxSAChanges_P7;
	uint8 dummy839[28];
	uint32 SWITCH_CORE_RxFragments_P7;
	uint8 dummy840[28];
	uint32 SWITCH_CORE_RxJumboPkt_P7;
	uint8 dummy841[28];
	uint32 SWITCH_CORE_RxSymblErr_P7;
	uint8 dummy842[28];
	uint32 SWITCH_CORE_InRangeErrCount_P7;
	uint8 dummy843[28];
	uint32 SWITCH_CORE_OutRangeErrCount_P7;
	uint8 dummy844[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_P7;
	uint8 dummy845[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_P7;
	uint8 dummy846[28];
	uint32 SWITCH_CORE_RxDiscard_P7;
	uint8 dummy847[60];
	uint32 SWITCH_CORE_TxQPKTQ6_P7;
	uint8 dummy848[28];
	uint32 SWITCH_CORE_TxQPKTQ7_P7;
	uint8 dummy849[28];
	uint32 SWITCH_CORE_TxPkts64Octets_P7;
	uint8 dummy850[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_P7;
	uint8 dummy851[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_P7;
	uint8 dummy852[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_P7;
	uint8 dummy853[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_P7;
	uint8 dummy854[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_P7;
	uint8 dummy855[220];
	uint32 SWITCH_CORE_TxOctets_IMP;
	uint8 dummy856[60];
	uint32 SWITCH_CORE_TxDropPkts_IMP;
	uint8 dummy857[28];
	uint32 SWITCH_CORE_TxQPKTQ0_IMP;
	uint8 dummy858[28];
	uint32 SWITCH_CORE_TxBroadcastPkts_IMP;
	uint8 dummy859[28];
	uint32 SWITCH_CORE_TxMulticastPkts_IMP;
	uint8 dummy860[28];
	uint32 SWITCH_CORE_TxUnicastPkts_IMP;
	uint8 dummy861[28];
	uint32 SWITCH_CORE_TxCollisions_IMP;
	uint8 dummy862[28];
	uint32 SWITCH_CORE_TxSingleCollision_IMP;
	uint8 dummy863[28];
	uint32 SWITCH_CORE_TxMultipleCollision_IMP;
	uint8 dummy864[28];
	uint32 SWITCH_CORE_TxDeferredTransmit_IMP;
	uint8 dummy865[28];
	uint32 SWITCH_CORE_TxLateCollision_IMP;
	uint8 dummy866[28];
	uint32 SWITCH_CORE_TxExcessiveCollision_IMP;
	uint8 dummy867[28];
	uint32 SWITCH_CORE_TxFrameInDisc_IMP;
	uint8 dummy868[28];
	uint32 SWITCH_CORE_TxPausePkts_IMP;
	uint8 dummy869[28];
	uint32 SWITCH_CORE_TxQPKTQ1_IMP;
	uint8 dummy870[28];
	uint32 SWITCH_CORE_TxQPKTQ2_IMP;
	uint8 dummy871[28];
	uint32 SWITCH_CORE_TxQPKTQ3_IMP;
	uint8 dummy872[28];
	uint32 SWITCH_CORE_TxQPKTQ4_IMP;
	uint8 dummy873[28];
	uint32 SWITCH_CORE_TxQPKTQ5_IMP;
	uint8 dummy874[28];
	uint32 SWITCH_CORE_RxOctets_IMP;
	uint8 dummy875[60];
	uint32 SWITCH_CORE_RxUndersizePkts_IMP;
	uint8 dummy876[28];
	uint32 SWITCH_CORE_RxPausePkts_IMP;
	uint8 dummy877[28];
	uint32 SWITCH_CORE_RxPkts64Octets_IMP;
	uint8 dummy878[28];
	uint32 SWITCH_CORE_RxPkts65to127Octets_IMP;
	uint8 dummy879[28];
	uint32 SWITCH_CORE_RxPkts128to255Octets_IMP;
	uint8 dummy880[28];
	uint32 SWITCH_CORE_RxPkts256to511Octets_IMP;
	uint8 dummy881[28];
	uint32 SWITCH_CORE_RxPkts512to1023Octets_IMP;
	uint8 dummy882[28];
	uint32 SWITCH_CORE_RxPkts1024toMaxPktOctets_IMP;
	uint8 dummy883[28];
	uint32 SWITCH_CORE_RxOversizePkts_IMP;
	uint8 dummy884[28];
	uint32 SWITCH_CORE_RxJabbers_IMP;
	uint8 dummy885[28];
	uint32 SWITCH_CORE_RxAlignmentErrors_IMP;
	uint8 dummy886[28];
	uint32 SWITCH_CORE_RxFCSErrors_IMP;
	uint8 dummy887[28];
	uint32 SWITCH_CORE_RxGoodOctets_IMP;
	uint8 dummy888[60];
	uint32 SWITCH_CORE_RxDropPkts_IMP;
	uint8 dummy889[28];
	uint32 SWITCH_CORE_RxUnicastPkts_IMP;
	uint8 dummy890[28];
	uint32 SWITCH_CORE_RxMulticastPkts_IMP;
	uint8 dummy891[28];
	uint32 SWITCH_CORE_RxBroadcastPkts_IMP;
	uint8 dummy892[28];
	uint32 SWITCH_CORE_RxSAChanges_IMP;
	uint8 dummy893[28];
	uint32 SWITCH_CORE_RxFragments_IMP;
	uint8 dummy894[28];
	uint32 SWITCH_CORE_RxJumboPkt_IMP;
	uint8 dummy895[28];
	uint32 SWITCH_CORE_RxSymblErr_IMP;
	uint8 dummy896[28];
	uint32 SWITCH_CORE_InRangeErrCount_IMP;
	uint8 dummy897[28];
	uint32 SWITCH_CORE_OutRangeErrCount_IMP;
	uint8 dummy898[28];
	uint32 SWITCH_CORE_EEE_LPI_EVENT_IMP;
	uint8 dummy899[28];
	uint32 SWITCH_CORE_EEE_LPI_DURATION_IMP;
	uint8 dummy900[28];
	uint32 SWITCH_CORE_RxDiscard_IMP;
	uint8 dummy901[60];
	uint32 SWITCH_CORE_TxQPKTQ6_IMP;
	uint8 dummy902[28];
	uint32 SWITCH_CORE_TxQPKTQ7_IMP;
	uint8 dummy903[28];
	uint32 SWITCH_CORE_TxPkts64Octets_IMP;
	uint8 dummy904[28];
	uint32 SWITCH_CORE_TxPkts65to127Octets_IMP;
	uint8 dummy905[28];
	uint32 SWITCH_CORE_TxPkts128to255Octets_IMP;
	uint8 dummy906[28];
	uint32 SWITCH_CORE_TxPkts256to511Octets_IMP;
	uint8 dummy907[28];
	uint32 SWITCH_CORE_TxPkts512to1023Octets_IMP;
	uint8 dummy908[28];
	uint32 SWITCH_CORE_TxPkts1024toMaxPktOctets_IMP;
	uint8 dummy909[14556];
	uint32 SWITCH_CORE_QOS_GLOBAL_CTRL;
	uint8 dummy910[28];
	uint32 SWITCH_CORE_QOS_1P_EN;
	uint8 dummy911[12];
	uint32 SWITCH_CORE_QOS_EN_DIFFSERV;
	uint8 dummy912[76];
	uint32 SWITCH_CORE_PCP2TC_DEI0_P0;
	uint8 dummy913[28];
	uint32 SWITCH_CORE_PCP2TC_DEI0_P1;
	uint8 dummy914[28];
	uint32 SWITCH_CORE_PCP2TC_DEI0_P2;
	uint8 dummy915[28];
	uint32 SWITCH_CORE_PCP2TC_DEI0_P3;
	uint8 dummy916[28];
	uint32 SWITCH_CORE_PCP2TC_DEI0_P4;
	uint8 dummy917[28];
	uint32 SWITCH_CORE_PCP2TC_DEI0_P5;
	uint8 dummy918[28];
	uint32 SWITCH_CORE_PCP2TC_DEI0_P6;
	uint8 dummy919[28];
	uint32 SWITCH_CORE_PCP2TC_DEI0_P7;
	uint8 dummy920[28];
	uint32 SWITCH_CORE_PCP2TC_DEI0_IMP;
	uint8 dummy921[28];
	uint32 SWITCH_CORE_QOS_DIFF_DSCP0;
	uint8 dummy922[44];
	uint32 SWITCH_CORE_QOS_DIFF_DSCP1;
	uint8 dummy923[44];
	uint32 SWITCH_CORE_QOS_DIFF_DSCP2;
	uint8 dummy924[44];
	uint32 SWITCH_CORE_QOS_DIFF_DSCP3;
	uint8 dummy925[44];
	uint32 SWITCH_CORE_PID2TC;
	uint8 dummy926[60];
	uint32 SWITCH_CORE_TC_SEL_TABLE_P0;
	uint8 dummy927[12];
	uint32 SWITCH_CORE_TC_SEL_TABLE_P1;
	uint8 dummy928[12];
	uint32 SWITCH_CORE_TC_SEL_TABLE_P2;
	uint8 dummy929[12];
	uint32 SWITCH_CORE_TC_SEL_TABLE_P3;
	uint8 dummy930[12];
	uint32 SWITCH_CORE_TC_SEL_TABLE_P4;
	uint8 dummy931[12];
	uint32 SWITCH_CORE_TC_SEL_TABLE_P5;
	uint8 dummy932[12];
	uint32 SWITCH_CORE_TC_SEL_TABLE_P6;
	uint8 dummy933[12];
	uint32 SWITCH_CORE_TC_SEL_TABLE_P7;
	uint8 dummy934[12];
	uint32 SWITCH_CORE_TC_SEL_TABLE_IMP;
	uint8 dummy935[28];
	uint32 SWITCH_CORE_CPU2COS_MAP;
	uint8 dummy936[60];
	uint32 SWITCH_CORE_TC2COS_MAP_P0;
	uint8 dummy937[28];
	uint32 SWITCH_CORE_TC2COS_MAP_P1;
	uint8 dummy938[28];
	uint32 SWITCH_CORE_TC2COS_MAP_P2;
	uint8 dummy939[28];
	uint32 SWITCH_CORE_TC2COS_MAP_P3;
	uint8 dummy940[28];
	uint32 SWITCH_CORE_TC2COS_MAP_P4;
	uint8 dummy941[28];
	uint32 SWITCH_CORE_TC2COS_MAP_P5;
	uint8 dummy942[28];
	uint32 SWITCH_CORE_TC2COS_MAP_P6;
	uint8 dummy943[28];
	uint32 SWITCH_CORE_TC2COS_MAP_P7;
	uint8 dummy944[28];
	uint32 SWITCH_CORE_TC2COS_MAP_IMP;
	uint8 dummy945[188];
	uint32 SWITCH_CORE_QOS_REG_SPARE0;
	uint8 dummy946[28];
	uint32 SWITCH_CORE_QOS_REG_SPARE1;
	uint8 dummy947[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_P0;
	uint8 dummy948[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_P1;
	uint8 dummy949[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_P2;
	uint8 dummy950[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_P3;
	uint8 dummy951[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_P4;
	uint8 dummy952[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_P5;
	uint8 dummy953[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_P6;
	uint8 dummy954[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_P7;
	uint8 dummy955[28];
	uint32 SWITCH_CORE_PCP2TC_DEI1_IMP;
	uint8 dummy956[380];
	uint32 port_vlan_ctrl[4*9];
	uint8 dummy965[112];
	uint32 SWITCH_CORE_VLAN_REG_SPARE0;
	uint8 dummy966[28];
	uint32 SWITCH_CORE_VLAN_REG_SPARE1;
	uint8 dummy967[1756];
	uint32 SWITCH_CORE_MAC_TRUNK_CTL;
	uint8 dummy968[12];
	uint32 SWITCH_CORE_IMP0_GRP_CTL;
	uint8 dummy969[108];
	uint32 SWITCH_CORE_TRUNK_GRP_CTL0;
	uint8 dummy970[12];
	uint32 SWITCH_CORE_TRUNK_GRP_CTL1;
	uint8 dummy971[12];
	uint32 SWITCH_CORE_TRUNK_GRP_CTL2;
	uint8 dummy972[12];
	uint32 SWITCH_CORE_TRUNK_GRP_CTL3;
	uint8 dummy973[76];
	uint32 SWITCH_CORE_TRUNK_HASH_OVRRD;
	uint8 dummy974[252];
	uint32 SWITCH_CORE_TRUNK_REG_SPARE0;
	uint8 dummy975[28];
	uint32 SWITCH_CORE_TRUNK_REG_SPARE1;
	uint8 dummy976[3548];
	uint64 SWITCH_CORE_VLAN_CTRL0;
	uint64 SWITCH_CORE_VLAN_CTRL1;
	uint64 SWITCH_CORE_VLAN_CTRL2;
	uint32 SWITCH_CORE_VLAN_CTRL3;
	uint8 dummy977[12];
	uint64 SWITCH_CORE_VLAN_CTRL4;
	uint64 SWITCH_CORE_VLAN_CTRL5;
	uint32 SWITCH_CORE_VLAN_CTRL6;
	uint8 dummy978[20];
	uint32 SWITCH_CORE_VLAN_MULTI_PORT_ADDR_CTL;
	uint8 dummy979[44];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_P0;
	uint8 dummy980[12];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_P1;
	uint8 dummy981[12];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_P2;
	uint8 dummy982[12];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_P3;
	uint8 dummy983[12];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_P4;
	uint8 dummy984[12];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_P5;
	uint8 dummy985[12];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_P6;
	uint8 dummy986[12];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_P7;
	uint8 dummy987[12];
	uint32 SWITCH_CORE_DEFAULT_1Q_TAG_IMP;
	uint8 dummy988[124];
	uint32 SWITCH_CORE_DTAG_TPID;
	uint8 dummy989[12];
	uint32 SWITCH_CORE_ISP_SEL_PORTMAP;
	uint8 dummy990[108];
	uint32 SWITCH_CORE_EGRESS_VID_RMK_TBL_ACS;
	uint8 dummy991[28];
	uint32 SWITCH_CORE_EGRESS_VID_RMK_TBL_DATA;
	uint8 dummy992[92];
	uint32 SWITCH_CORE_JOIN_ALL_VLAN_EN;
	uint8 dummy993[12];
	uint32 SWITCH_CORE_PORT_IVL_SVL_CTRL;
	uint8 dummy994[108];
	uint32 SWITCH_CORE_BCM8021Q_REG_SPARE0;
	uint8 dummy995[28];
	uint32 SWITCH_CORE_BCM8021Q_REG_SPARE1;
	uint8 dummy996[3292];
	uint32 SWITCH_CORE_DOS_CTRL;
	uint8 dummy997[28];
	uint32 SWITCH_CORE_MINIMUM_TCP_HDR_SZ;
	uint8 dummy998[28];
	uint32 SWITCH_CORE_MAX_ICMPV4_SIZE_REG;
	uint8 dummy999[28];
	uint32 SWITCH_CORE_MAX_ICMPV6_SIZE_REG;
	uint8 dummy1000[28];
	uint32 SWITCH_CORE_DOS_DIS_LRN_REG;
	uint8 dummy1001[124];
	uint32 SWITCH_CORE_DOS_REG_SPARE0;
	uint8 dummy1002[28];
	uint32 SWITCH_CORE_DOS_REG_SPARE1;
	uint8 dummy1003[20196];
	uint32 SWITCH_CORE_JUMBO_PORT_MASK;
	uint8 dummy1004[28];
	uint32 SWITCH_CORE_MIB_GD_FM_MAX_SIZE;
	uint8 dummy1005[84];
	uint32 SWITCH_CORE_JUMBO_CTRL_REG_SPARE0;
	uint8 dummy1006[28];
	uint32 SWITCH_CORE_JUMBO_CTRL_REG_SPARE1;
	uint8 dummy1007[1884];
	uint32 SWITCH_CORE_COMM_IRC_CON;
	uint8 dummy1008[28];
	uint32 SWITCH_CORE_IRC_VIRTUAL_ZERO_THD;
	uint8 dummy1009[12];
	uint32 SWITCH_CORE_IRC_ALARM_THD;
	uint8 dummy1010[76];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_P0;
	uint8 dummy1011[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_P1;
	uint8 dummy1012[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_P2;
	uint8 dummy1013[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_P3;
	uint8 dummy1014[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_P4;
	uint8 dummy1015[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_P5;
	uint8 dummy1016[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_P6;
	uint8 dummy1017[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_P7;
	uint8 dummy1018[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_IMP;
	uint8 dummy1019[28];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_P0;
	uint8 dummy1020[12];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_P1;
	uint8 dummy1021[12];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_P2;
	uint8 dummy1022[12];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_P3;
	uint8 dummy1023[12];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_P4;
	uint8 dummy1024[12];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_P5;
	uint8 dummy1025[12];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_P6;
	uint8 dummy1026[12];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_P7;
	uint8 dummy1027[12];
	uint32 SWITCH_CORE_BC_SUP_RATECTRL_1_IMP;
	uint8 dummy1028[92];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_P0;
	uint8 dummy1029[28];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_P1;
	uint8 dummy1030[28];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_P2;
	uint8 dummy1031[28];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_P3;
	uint8 dummy1032[28];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_P4;
	uint8 dummy1033[28];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_P5;
	uint8 dummy1034[28];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_P6;
	uint8 dummy1035[28];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_P7;
	uint8 dummy1036[28];
	uint32 SWITCH_CORE_BC_SUP_PKTDROP_CNT_IMP;
	uint8 dummy1037[764];
	uint32 SWITCH_CORE_BC_SUPPRESS_REG_SPARE0;
	uint8 dummy1038[28];
	uint32 SWITCH_CORE_BC_SUPPRESS_REG_SPARE1;
	uint8 dummy1039[348];
	uint64 SWITCH_CORE_EAP_GLO_CON;
	uint64 SWITCH_CORE_EAP_MULTI_ADDR_CTRL;
	uint32 SWITCH_CORE_EAP_DIP0;
	uint8 dummy1040[60];
	uint32 SWITCH_CORE_EAP_DIP1;
	uint8 dummy1041[172];
	uint32 SWITCH_CORE_EAP_CON_P0;
	uint8 dummy1042[60];
	uint32 SWITCH_CORE_EAP_CON_P1;
	uint8 dummy1043[60];
	uint32 SWITCH_CORE_EAP_CON_P2;
	uint8 dummy1044[60];
	uint32 SWITCH_CORE_EAP_CON_P3;
	uint8 dummy1045[60];
	uint32 SWITCH_CORE_EAP_CON_P4;
	uint8 dummy1046[60];
	uint32 SWITCH_CORE_EAP_CON_P5;
	uint8 dummy1047[60];
	uint32 SWITCH_CORE_EAP_CON_P6;
	uint8 dummy1048[60];
	uint32 SWITCH_CORE_EAP_CON_P7;
	uint8 dummy1049[60];
	uint32 SWITCH_CORE_EAP_CON_IMP;
	uint8 dummy1050[124];
	uint32 SWITCH_CORE_IEEE8021X_REG_SPARE0;
	uint8 dummy1051[28];
	uint32 SWITCH_CORE_IEEE8021X_REG_SPARE1;
	uint8 dummy1052[1116];
	uint32 SWITCH_CORE_MST_CON;
	uint8 dummy1053[12];
	uint32 SWITCH_CORE_MST_AGE;
	uint8 dummy1054[108];
	uint32 SWITCH_CORE_MST_TAB0;
	uint8 dummy1055[28];
	uint32 SWITCH_CORE_MST_TAB1;
	uint8 dummy1056[28];
	uint32 SWITCH_CORE_MST_TAB2;
	uint8 dummy1057[28];
	uint32 SWITCH_CORE_MST_TAB3;
	uint8 dummy1058[28];
	uint32 SWITCH_CORE_MST_TAB4;
	uint8 dummy1059[28];
	uint32 SWITCH_CORE_MST_TAB5;
	uint8 dummy1060[28];
	uint32 SWITCH_CORE_MST_TAB6;
	uint8 dummy1061[28];
	uint32 SWITCH_CORE_MST_TAB7;
	uint8 dummy1062[284];
	uint32 SWITCH_CORE_SPT_MULTI_ADDR_BPS_CTRL;
	uint8 dummy1063[124];
	uint32 SWITCH_CORE_IEEE8021S_REG_SPARE0;
	uint8 dummy1064[28];
	uint32 SWITCH_CORE_IEEE8021S_REG_SPARE1;
	uint8 dummy1065[3292];
	uint32 SWITCH_CORE_SA_LIMIT_ENABLE;
	uint8 dummy1066[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_RST;
	uint8 dummy1067[12];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_RST;
	uint8 dummy1068[92];
	uint32 SWITCH_CORE_TOTAL_SA_LIMIT_CTL;
	uint8 dummy1069[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P0;
	uint8 dummy1070[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P1;
	uint8 dummy1071[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P2;
	uint8 dummy1072[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P3;
	uint8 dummy1073[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P4;
	uint8 dummy1074[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P5;
	uint8 dummy1075[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P6;
	uint8 dummy1076[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P7;
	uint8 dummy1077[12];
	uint32 SWITCH_CORE_SA_LIMIT_CTL_P8;
	uint8 dummy1078[108];
	uint32 SWITCH_CORE_TOTAL_SA_LRN_CNTR;
	uint8 dummy1079[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P0;
	uint8 dummy1080[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P1;
	uint8 dummy1081[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P2;
	uint8 dummy1082[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P3;
	uint8 dummy1083[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P4;
	uint8 dummy1084[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P5;
	uint8 dummy1085[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P6;
	uint8 dummy1086[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P7;
	uint8 dummy1087[12];
	uint32 SWITCH_CORE_SA_LRN_CNTR_P8;
	uint8 dummy1088[108];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P0;
	uint8 dummy1089[28];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P1;
	uint8 dummy1090[28];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P2;
	uint8 dummy1091[28];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P3;
	uint8 dummy1092[28];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P4;
	uint8 dummy1093[28];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P5;
	uint8 dummy1094[28];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P6;
	uint8 dummy1095[28];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P7;
	uint8 dummy1096[28];
	uint32 SWITCH_CORE_SA_OVERLIMIT_CNTR_P8;
	uint8 dummy1097[28];
	uint32 SWITCH_CORE_SA_OVER_LIMIT_COPY_REDIRECT;
	uint8 dummy1098[92];
	uint32 SWITCH_CORE_MAC_LIMIT_REG_SPARE0;
	uint8 dummy1099[28];
	uint32 SWITCH_CORE_MAC_LIMIT_REG_SPARE1;
	uint8 dummy1100[988];
	uint64 SWITCH_CORE_QOS_PRI_CTL_P0;
	uint64 SWITCH_CORE_QOS_PRI_CTL_P1;
	uint64 SWITCH_CORE_QOS_PRI_CTL_P2;
	uint64 SWITCH_CORE_QOS_PRI_CTL_P3;
	uint64 SWITCH_CORE_QOS_PRI_CTL_P4;
	uint64 SWITCH_CORE_QOS_PRI_CTL_P5;
	uint64 SWITCH_CORE_QOS_PRI_CTL_P6;
	uint64 SWITCH_CORE_QOS_PRI_CTL_P7;
	uint32 SWITCH_CORE_QOS_PRI_CTL_IMP;
	uint8 dummy1101[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_P0;
	uint8 dummy1102[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_P1;
	uint8 dummy1103[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_P2;
	uint8 dummy1104[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_P3;
	uint8 dummy1105[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_P4;
	uint8 dummy1106[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_P5;
	uint8 dummy1107[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_P6;
	uint8 dummy1108[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_P7;
	uint8 dummy1109[60];
	uint32 SWITCH_CORE_QOS_WEIGHT_IMP;
	uint8 dummy1110[124];
	uint32 SWITCH_CORE_WDRR_PENALTY_P0;
	uint8 dummy1111[12];
	uint32 SWITCH_CORE_WDRR_PENALTY_P1;
	uint8 dummy1112[12];
	uint32 SWITCH_CORE_WDRR_PENALTY_P2;
	uint8 dummy1113[12];
	uint32 SWITCH_CORE_WDRR_PENALTY_P3;
	uint8 dummy1114[12];
	uint32 SWITCH_CORE_WDRR_PENALTY_P4;
	uint8 dummy1115[12];
	uint32 SWITCH_CORE_WDRR_PENALTY_P5;
	uint8 dummy1116[12];
	uint32 SWITCH_CORE_WDRR_PENALTY_P6;
	uint8 dummy1117[28];
	uint32 SWITCH_CORE_WDRR_PENALTY_P7;
	uint8 dummy1118[12];
	uint32 SWITCH_CORE_P8_WDRR_PENALTY;
	uint8 dummy1119[108];
	uint32 SWITCH_CORE_SCHEDULER_REG_SPARE0;
	uint8 dummy1120[28];
	uint32 SWITCH_CORE_SCHEDULER_REG_SPARE1;
	uint8 dummy1121[988];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_P0;
	uint8 dummy1122[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_P1;
	uint8 dummy1123[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_P2;
	uint8 dummy1124[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_P3;
	uint8 dummy1125[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_P4;
	uint8 dummy1126[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_P5;
	uint8 dummy1127[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_P6;
	uint8 dummy1128[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_P7;
	uint8 dummy1129[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_REFRESH_IMP;
	uint8 dummy1130[124];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_P0;
	uint8 dummy1131[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_P1;
	uint8 dummy1132[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_P2;
	uint8 dummy1133[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_P3;
	uint8 dummy1134[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_P4;
	uint8 dummy1135[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_P5;
	uint8 dummy1136[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_P6;
	uint8 dummy1137[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_P7;
	uint8 dummy1138[28];
	uint32 SWITCH_CORE_PORT_SHAPER_BYTE_BASED_MAX_THD_SEL_IMP;
	uint8 dummy1139[124];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_P0;
	uint8 dummy1140[28];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_P1;
	uint8 dummy1141[28];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_P2;
	uint8 dummy1142[28];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_P3;
	uint8 dummy1143[28];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_P4;
	uint8 dummy1144[28];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_P5;
	uint8 dummy1145[28];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_P6;
	uint8 dummy1146[28];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_P7;
	uint8 dummy1147[28];
	uint32 SWITCH_CORE_PORT_SHAPER_STS_IMP;
	uint8 dummy1148[124];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_P0;
	uint8 dummy1149[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_P1;
	uint8 dummy1150[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_P2;
	uint8 dummy1151[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_P3;
	uint8 dummy1152[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_P4;
	uint8 dummy1153[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_P5;
	uint8 dummy1154[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_P6;
	uint8 dummy1155[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_P7;
	uint8 dummy1156[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_REFRESH_IMP;
	uint8 dummy1157[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_CTLREG_REG_SPARE0;
	uint8 dummy1158[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_CTLREG_REG_SPARE1;
	uint8 dummy1159[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_P0;
	uint8 dummy1160[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_P1;
	uint8 dummy1161[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_P2;
	uint8 dummy1162[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_P3;
	uint8 dummy1163[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_P4;
	uint8 dummy1164[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_P5;
	uint8 dummy1165[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_P6;
	uint8 dummy1166[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_P7;
	uint8 dummy1167[28];
	uint32 SWITCH_CORE_PORT_SHAPER_PACKET_BASED_MAX_THD_SEL_IMP;
	uint8 dummy1168[28];
	uint32 SWITCH_CORE_PORT_SHAPER_AVB_SHAPING_MODE;
	uint8 dummy1169[12];
	uint32 SWITCH_CORE_PORT_SHAPER_ENABLE;
	uint8 dummy1170[12];
	uint32 SWITCH_CORE_PORT_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1171[12];
	uint32 SWITCH_CORE_PORT_SHAPER_BLOCKING;
	uint8 dummy1172[28];
	uint32 SWITCH_CORE_IFG_BYTES;
	uint8 dummy1173[140];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_P0;
	uint8 dummy1174[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_P1;
	uint8 dummy1175[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_P2;
	uint8 dummy1176[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_P3;
	uint8 dummy1177[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_P4;
	uint8 dummy1178[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_P5;
	uint8 dummy1179[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_P6;
	uint8 dummy1180[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_P7;
	uint8 dummy1181[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_REFRESH_IMP;
	uint8 dummy1182[124];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_P0;
	uint8 dummy1183[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_P1;
	uint8 dummy1184[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_P2;
	uint8 dummy1185[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_P3;
	uint8 dummy1186[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_P4;
	uint8 dummy1187[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_P5;
	uint8 dummy1188[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_P6;
	uint8 dummy1189[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_P7;
	uint8 dummy1190[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_THD_SEL_IMP;
	uint8 dummy1191[124];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_P0;
	uint8 dummy1192[28];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_P1;
	uint8 dummy1193[28];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_P2;
	uint8 dummy1194[28];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_P3;
	uint8 dummy1195[28];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_P4;
	uint8 dummy1196[28];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_P5;
	uint8 dummy1197[28];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_P6;
	uint8 dummy1198[28];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_P7;
	uint8 dummy1199[28];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_STS_IMP;
	uint8 dummy1200[124];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_P0;
	uint8 dummy1201[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_P1;
	uint8 dummy1202[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_P2;
	uint8 dummy1203[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_P3;
	uint8 dummy1204[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_P4;
	uint8 dummy1205[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_P5;
	uint8 dummy1206[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_P6;
	uint8 dummy1207[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_P7;
	uint8 dummy1208[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_REFRESH_IMP;
	uint8 dummy1209[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q0_CONFIG_REG_SPARE0;
	uint8 dummy1210[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q0_CONFIG_REG_SPARE1;
	uint8 dummy1211[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_P0;
	uint8 dummy1212[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_P1;
	uint8 dummy1213[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_P2;
	uint8 dummy1214[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_P3;
	uint8 dummy1215[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_P4;
	uint8 dummy1216[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_P5;
	uint8 dummy1217[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_P6;
	uint8 dummy1218[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_P7;
	uint8 dummy1219[28];
	uint32 SWITCH_CORE_QUEUE0_MAX_PACKET_THD_SEL_IMP;
	uint8 dummy1220[28];
	uint32 SWITCH_CORE_QUEUE0_AVB_SHAPING_MODE;
	uint8 dummy1221[12];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_ENABLE;
	uint8 dummy1222[12];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1223[12];
	uint32 SWITCH_CORE_QUEUE0_SHAPER_BLOCKING;
	uint8 dummy1224[172];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_P0;
	uint8 dummy1225[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_P1;
	uint8 dummy1226[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_P2;
	uint8 dummy1227[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_P3;
	uint8 dummy1228[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_P4;
	uint8 dummy1229[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_P5;
	uint8 dummy1230[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_P6;
	uint8 dummy1231[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_P7;
	uint8 dummy1232[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_REFRESH_IMP;
	uint8 dummy1233[124];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_P0;
	uint8 dummy1234[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_P1;
	uint8 dummy1235[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_P2;
	uint8 dummy1236[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_P3;
	uint8 dummy1237[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_P4;
	uint8 dummy1238[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_P5;
	uint8 dummy1239[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_P6;
	uint8 dummy1240[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_P7;
	uint8 dummy1241[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_THD_SEL_IMP;
	uint8 dummy1242[124];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_P0;
	uint8 dummy1243[28];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_P1;
	uint8 dummy1244[28];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_P2;
	uint8 dummy1245[28];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_P3;
	uint8 dummy1246[28];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_P4;
	uint8 dummy1247[28];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_P5;
	uint8 dummy1248[28];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_P6;
	uint8 dummy1249[28];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_P7;
	uint8 dummy1250[28];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_STS_IMP;
	uint8 dummy1251[124];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_P0;
	uint8 dummy1252[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_P1;
	uint8 dummy1253[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_P2;
	uint8 dummy1254[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_P3;
	uint8 dummy1255[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_P4;
	uint8 dummy1256[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_P5;
	uint8 dummy1257[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_P6;
	uint8 dummy1258[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_P7;
	uint8 dummy1259[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_REFRESH_IMP;
	uint8 dummy1260[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q1_CONFIG_REG_SPARE0;
	uint8 dummy1261[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q1_CONFIG_REG_SPARE1;
	uint8 dummy1262[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_P0;
	uint8 dummy1263[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_P1;
	uint8 dummy1264[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_P2;
	uint8 dummy1265[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_P3;
	uint8 dummy1266[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_P4;
	uint8 dummy1267[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_P5;
	uint8 dummy1268[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_P6;
	uint8 dummy1269[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_P7;
	uint8 dummy1270[28];
	uint32 SWITCH_CORE_QUEUE1_MAX_PACKET_THD_SEL_IMP;
	uint8 dummy1271[28];
	uint32 SWITCH_CORE_QUEUE1_AVB_SHAPING_MODE;
	uint8 dummy1272[12];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_ENABLE;
	uint8 dummy1273[12];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1274[12];
	uint32 SWITCH_CORE_QUEUE1_SHAPER_BLOCKING;
	uint8 dummy1275[172];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_P0;
	uint8 dummy1276[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_P1;
	uint8 dummy1277[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_P2;
	uint8 dummy1278[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_P3;
	uint8 dummy1279[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_P4;
	uint8 dummy1280[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_P5;
	uint8 dummy1281[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_P6;
	uint8 dummy1282[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_P7;
	uint8 dummy1283[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_REFRESH_IMP;
	uint8 dummy1284[124];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_P0;
	uint8 dummy1285[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_P1;
	uint8 dummy1286[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_P2;
	uint8 dummy1287[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_P3;
	uint8 dummy1288[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_P4;
	uint8 dummy1289[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_P5;
	uint8 dummy1290[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_P6;
	uint8 dummy1291[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_P7;
	uint8 dummy1292[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_THD_SEL_IMP;
	uint8 dummy1293[124];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_P0;
	uint8 dummy1294[28];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_P1;
	uint8 dummy1295[28];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_P2;
	uint8 dummy1296[28];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_P3;
	uint8 dummy1297[28];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_P4;
	uint8 dummy1298[28];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_P5;
	uint8 dummy1299[28];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_P6;
	uint8 dummy1300[28];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_P7;
	uint8 dummy1301[28];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_STS_IMP;
	uint8 dummy1302[124];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_P0;
	uint8 dummy1303[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_P1;
	uint8 dummy1304[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_P2;
	uint8 dummy1305[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_P3;
	uint8 dummy1306[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_P4;
	uint8 dummy1307[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_P5;
	uint8 dummy1308[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_P6;
	uint8 dummy1309[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_P7;
	uint8 dummy1310[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_REFRESH_IMP;
	uint8 dummy1311[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q2_CONFIG_REG_SPARE0;
	uint8 dummy1312[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q2_CONFIG_REG_SPARE1;
	uint8 dummy1313[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_P0;
	uint8 dummy1314[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_P1;
	uint8 dummy1315[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_P2;
	uint8 dummy1316[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_P3;
	uint8 dummy1317[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_P4;
	uint8 dummy1318[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_P5;
	uint8 dummy1319[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_P6;
	uint8 dummy1320[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_P7;
	uint8 dummy1321[28];
	uint32 SWITCH_CORE_QUEUE2_MAX_PACKET_THD_SEL_IMP;
	uint8 dummy1322[28];
	uint32 SWITCH_CORE_QUEUE2_AVB_SHAPING_MODE;
	uint8 dummy1323[12];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_ENABLE;
	uint8 dummy1324[12];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1325[12];
	uint32 SWITCH_CORE_QUEUE2_SHAPER_BLOCKING;
	uint8 dummy1326[172];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_P0;
	uint8 dummy1327[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_P1;
	uint8 dummy1328[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_P2;
	uint8 dummy1329[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_P3;
	uint8 dummy1330[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_P4;
	uint8 dummy1331[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_P5;
	uint8 dummy1332[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_P6;
	uint8 dummy1333[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_P7;
	uint8 dummy1334[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_REFRESH_IMP;
	uint8 dummy1335[124];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_P0;
	uint8 dummy1336[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_P1;
	uint8 dummy1337[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_P2;
	uint8 dummy1338[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_P3;
	uint8 dummy1339[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_P4;
	uint8 dummy1340[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_P5;
	uint8 dummy1341[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_P6;
	uint8 dummy1342[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_P7;
	uint8 dummy1343[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_THD_SEL_IMP;
	uint8 dummy1344[124];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_P0;
	uint8 dummy1345[28];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_P1;
	uint8 dummy1346[28];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_P2;
	uint8 dummy1347[28];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_P3;
	uint8 dummy1348[28];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_P4;
	uint8 dummy1349[28];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_P5;
	uint8 dummy1350[28];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_P6;
	uint8 dummy1351[28];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_P7;
	uint8 dummy1352[28];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_STS_IMP;
	uint8 dummy1353[124];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_P0;
	uint8 dummy1354[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_P1;
	uint8 dummy1355[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_P2;
	uint8 dummy1356[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_P3;
	uint8 dummy1357[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_P4;
	uint8 dummy1358[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_P5;
	uint8 dummy1359[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_P6;
	uint8 dummy1360[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_P7;
	uint8 dummy1361[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_REFRESH_IMP;
	uint8 dummy1362[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q3_CONFIG_REG_SPARE0;
	uint8 dummy1363[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q3_CONFIG_REG_SPARE1;
	uint8 dummy1364[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_P0;
	uint8 dummy1365[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_P1;
	uint8 dummy1366[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_P2;
	uint8 dummy1367[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_P3;
	uint8 dummy1368[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_P4;
	uint8 dummy1369[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_P5;
	uint8 dummy1370[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_P6;
	uint8 dummy1371[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_P7;
	uint8 dummy1372[28];
	uint32 SWITCH_CORE_QUEUE3_MAX_PACKET_THD_SEL_IMP;
	uint8 dummy1373[28];
	uint32 SWITCH_CORE_QUEUE3_AVB_SHAPING_MODE;
	uint8 dummy1374[12];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_ENABLE;
	uint8 dummy1375[12];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1376[12];
	uint32 SWITCH_CORE_QUEUE3_SHAPER_BLOCKING;
	uint8 dummy1377[172];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_P0;
	uint8 dummy1378[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_P1;
	uint8 dummy1379[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_P2;
	uint8 dummy1380[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_P3;
	uint8 dummy1381[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_P4;
	uint8 dummy1382[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_P5;
	uint8 dummy1383[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_P6;
	uint8 dummy1384[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_P7;
	uint8 dummy1385[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_REFRESH_IMP;
	uint8 dummy1386[124];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_P0;
	uint8 dummy1387[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_P1;
	uint8 dummy1388[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_P2;
	uint8 dummy1389[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_P3;
	uint8 dummy1390[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_P4;
	uint8 dummy1391[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_P5;
	uint8 dummy1392[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_P6;
	uint8 dummy1393[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_P7;
	uint8 dummy1394[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_THD_SEL_IMP;
	uint8 dummy1395[124];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_P0;
	uint8 dummy1396[28];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_P1;
	uint8 dummy1397[28];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_P2;
	uint8 dummy1398[28];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_P3;
	uint8 dummy1399[28];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_P4;
	uint8 dummy1400[28];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_P5;
	uint8 dummy1401[28];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_P6;
	uint8 dummy1402[28];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_P7;
	uint8 dummy1403[28];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_STS_IMP;
	uint8 dummy1404[124];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_P0;
	uint8 dummy1405[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_P1;
	uint8 dummy1406[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_P2;
	uint8 dummy1407[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_P3;
	uint8 dummy1408[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_P4;
	uint8 dummy1409[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_P5;
	uint8 dummy1410[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_P6;
	uint8 dummy1411[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_P7;
	uint8 dummy1412[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_REFRESH_IMP;
	uint8 dummy1413[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q4_CONFIG_REG_SPARE0;
	uint8 dummy1414[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q4_CONFIG_REG_SPARE1;
	uint8 dummy1415[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_P0;
	uint8 dummy1416[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_P1;
	uint8 dummy1417[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_P2;
	uint8 dummy1418[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_P3;
	uint8 dummy1419[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_P4;
	uint8 dummy1420[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_P5;
	uint8 dummy1421[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_P6;
	uint8 dummy1422[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_P7;
	uint8 dummy1423[28];
	uint32 SWITCH_CORE_QUEUE4_MAX_PACKET_THD_SEL_IMP;
	uint8 dummy1424[28];
	uint32 SWITCH_CORE_QUEUE4_AVB_SHAPING_MODE;
	uint8 dummy1425[12];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_ENABLE;
	uint8 dummy1426[12];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1427[12];
	uint32 SWITCH_CORE_QUEUE4_SHAPER_BLOCKING;
	uint8 dummy1428[172];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_P0;
	uint8 dummy1429[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_P1;
	uint8 dummy1430[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_P2;
	uint8 dummy1431[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_P3;
	uint8 dummy1432[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_P4;
	uint8 dummy1433[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_P5;
	uint8 dummy1434[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_P6;
	uint8 dummy1435[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_P7;
	uint8 dummy1436[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_REFRESH_IMP;
	uint8 dummy1437[124];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_P0;
	uint8 dummy1438[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_P1;
	uint8 dummy1439[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_P2;
	uint8 dummy1440[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_P3;
	uint8 dummy1441[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_P4;
	uint8 dummy1442[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_P5;
	uint8 dummy1443[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_P6;
	uint8 dummy1444[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_P7;
	uint8 dummy1445[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_THD_SEL_IMP;
	uint8 dummy1446[124];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_P0;
	uint8 dummy1447[28];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_P1;
	uint8 dummy1448[28];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_P2;
	uint8 dummy1449[28];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_P3;
	uint8 dummy1450[28];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_P4;
	uint8 dummy1451[28];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_P5;
	uint8 dummy1452[28];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_P6;
	uint8 dummy1453[28];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_P7;
	uint8 dummy1454[28];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_STS_IMP;
	uint8 dummy1455[124];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_P0;
	uint8 dummy1456[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_P1;
	uint8 dummy1457[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_P2;
	uint8 dummy1458[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_P3;
	uint8 dummy1459[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_P4;
	uint8 dummy1460[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_P5;
	uint8 dummy1461[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_P6;
	uint8 dummy1462[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_P7;
	uint8 dummy1463[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_REFRESH_IMP;
	uint8 dummy1464[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q5_CONFIG_REG_SPARE0;
	uint8 dummy1465[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q5_CONFIG_REG_SPARE1;
	uint8 dummy1466[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_P0;
	uint8 dummy1467[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_P1;
	uint8 dummy1468[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_P2;
	uint8 dummy1469[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_P3;
	uint8 dummy1470[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_P4;
	uint8 dummy1471[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_P5;
	uint8 dummy1472[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_P6;
	uint8 dummy1473[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_P7;
	uint8 dummy1474[28];
	uint32 SWITCH_CORE_QUEUE5_MAX_PACKET_THD_SEL_IMP;
	uint8 dummy1475[28];
	uint32 SWITCH_CORE_QUEUE5_AVB_SHAPING_MODE;
	uint8 dummy1476[12];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_ENABLE;
	uint8 dummy1477[12];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1478[12];
	uint32 SWITCH_CORE_QUEUE5_SHAPER_BLOCKING;
	uint8 dummy1479[172];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_P0;
	uint8 dummy1480[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_P1;
	uint8 dummy1481[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_P2;
	uint8 dummy1482[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_P3;
	uint8 dummy1483[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_P4;
	uint8 dummy1484[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_P5;
	uint8 dummy1485[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_P6;
	uint8 dummy1486[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_P7;
	uint8 dummy1487[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_REFRESH_IMP;
	uint8 dummy1488[124];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_P0;
	uint8 dummy1489[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_P1;
	uint8 dummy1490[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_P2;
	uint8 dummy1491[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_P3;
	uint8 dummy1492[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_P4;
	uint8 dummy1493[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_P5;
	uint8 dummy1494[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_P6;
	uint8 dummy1495[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_P7;
	uint8 dummy1496[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_THD_SEL_IMP;
	uint8 dummy1497[124];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_P0;
	uint8 dummy1498[28];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_P1;
	uint8 dummy1499[28];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_P2;
	uint8 dummy1500[28];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_P3;
	uint8 dummy1501[28];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_P4;
	uint8 dummy1502[28];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_P5;
	uint8 dummy1503[28];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_P6;
	uint8 dummy1504[28];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_P7;
	uint8 dummy1505[28];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_STS_IMP;
	uint8 dummy1506[124];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_P0;
	uint8 dummy1507[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_P1;
	uint8 dummy1508[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_P2;
	uint8 dummy1509[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_P3;
	uint8 dummy1510[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_P4;
	uint8 dummy1511[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_P5;
	uint8 dummy1512[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_P6;
	uint8 dummy1513[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_P7;
	uint8 dummy1514[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_REFRESH_IMP;
	uint8 dummy1515[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q6_CONFIG_REG_SPARE0;
	uint8 dummy1516[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q6_CONFIG_REG_SPARE1;
	uint8 dummy1517[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_P0;
	uint8 dummy1518[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_P1;
	uint8 dummy1519[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_P2;
	uint8 dummy1520[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_P3;
	uint8 dummy1521[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_P4;
	uint8 dummy1522[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_P5;
	uint8 dummy1523[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_P6;
	uint8 dummy1524[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_P7;
	uint8 dummy1525[28];
	uint32 SWITCH_CORE_QUEUE6_MAX_PACKET_THD_SEL_IMP;
	uint8 dummy1526[28];
	uint32 SWITCH_CORE_QUEUE6_AVB_SHAPING_MODE;
	uint8 dummy1527[12];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_ENABLE;
	uint8 dummy1528[12];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1529[12];
	uint32 SWITCH_CORE_QUEUE6_SHAPER_BLOCKING;
	uint8 dummy1530[172];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_P0;
	uint8 dummy1531[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_P1;
	uint8 dummy1532[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_P2;
	uint8 dummy1533[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_P3;
	uint8 dummy1534[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_P4;
	uint8 dummy1535[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_P5;
	uint8 dummy1536[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_P6;
	uint8 dummy1537[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_P7;
	uint8 dummy1538[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_REFRESH_IMP;
	uint8 dummy1539[124];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_P0;
	uint8 dummy1540[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_P1;
	uint8 dummy1541[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_P2;
	uint8 dummy1542[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_P3;
	uint8 dummy1543[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_P4;
	uint8 dummy1544[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_P5;
	uint8 dummy1545[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_P6;
	uint8 dummy1546[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_P7;
	uint8 dummy1547[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_THD_SEL_IMP;
	uint8 dummy1548[124];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_P0;
	uint8 dummy1549[28];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_P1;
	uint8 dummy1550[28];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_P2;
	uint8 dummy1551[28];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_P3;
	uint8 dummy1552[28];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_P4;
	uint8 dummy1553[28];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_P5;
	uint8 dummy1554[28];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_P6;
	uint8 dummy1555[28];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_P7;
	uint8 dummy1556[28];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_STS_IMP;
	uint8 dummy1557[124];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_P0;
	uint8 dummy1558[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_P1;
	uint8 dummy1559[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_P2;
	uint8 dummy1560[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_P3;
	uint8 dummy1561[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_P4;
	uint8 dummy1562[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_P5;
	uint8 dummy1563[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_P6;
	uint8 dummy1564[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_P7;
	uint8 dummy1565[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_REFRESH_IMP;
	uint8 dummy1566[60];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q7_CONFIG_REG_SPARE0;
	uint8 dummy1567[28];
	uint32 SWITCH_CORE_EGRESS_SHAPER_Q7_CONFIG_REG_SPARE1;
	uint8 dummy1568[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_P0;
	uint8 dummy1569[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_P1;
	uint8 dummy1570[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_P2;
	uint8 dummy1571[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_P3;
	uint8 dummy1572[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_P4;
	uint8 dummy1573[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_P5;
	uint8 dummy1574[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_P6;
	uint8 dummy1575[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_P7;
	uint8 dummy1576[28];
	uint32 SWITCH_CORE_QUEUE7_MAX_PACKET_THD_SEL_IMP;
	uint8 dummy1577[28];
	uint32 SWITCH_CORE_QUEUE7_AVB_SHAPING_MODE;
	uint8 dummy1578[12];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_ENABLE;
	uint8 dummy1579[12];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_BUCKET_COUNT_SELECT;
	uint8 dummy1580[12];
	uint32 SWITCH_CORE_QUEUE7_SHAPER_BLOCKING;
	uint8 dummy1581[65708];
	uint32 SWITCH_CORE_MIB_SNAPSHOT_CTL;
	uint8 dummy1582[2044];
	uint32 SWITCH_CORE_S_TxOctets;
	uint8 dummy1583[60];
	uint32 SWITCH_CORE_S_TxDropPkts;
	uint8 dummy1584[28];
	uint32 SWITCH_CORE_S_TxQPKTQ0;
	uint8 dummy1585[28];
	uint32 SWITCH_CORE_S_TxBroadcastPkts;
	uint8 dummy1586[28];
	uint32 SWITCH_CORE_S_TxMulticastPkts;
	uint8 dummy1587[28];
	uint32 SWITCH_CORE_S_TxUnicastPkts;
	uint8 dummy1588[28];
	uint32 SWITCH_CORE_S_TxCollisions;
	uint8 dummy1589[28];
	uint32 SWITCH_CORE_S_TxSingleCollision;
	uint8 dummy1590[28];
	uint32 SWITCH_CORE_S_TxMultipleCollision;
	uint8 dummy1591[28];
	uint32 SWITCH_CORE_S_TxDeferredTransmit;
	uint8 dummy1592[28];
	uint32 SWITCH_CORE_S_TxLateCollision;
	uint8 dummy1593[28];
	uint32 SWITCH_CORE_S_TxExcessiveCollision;
	uint8 dummy1594[28];
	uint32 SWITCH_CORE_S_TxFrameInDisc;
	uint8 dummy1595[28];
	uint32 SWITCH_CORE_S_TxPausePkts;
	uint8 dummy1596[28];
	uint32 SWITCH_CORE_S_TxQPKTQ1;
	uint8 dummy1597[28];
	uint32 SWITCH_CORE_S_TxQPKTQ2;
	uint8 dummy1598[28];
	uint32 SWITCH_CORE_S_TxQPKTQ3;
	uint8 dummy1599[28];
	uint32 SWITCH_CORE_S_TxQPKTQ4;
	uint8 dummy1600[28];
	uint32 SWITCH_CORE_S_TxQPKTQ5;
	uint8 dummy1601[28];
	uint32 SWITCH_CORE_S_RxOctets;
	uint8 dummy1602[60];
	uint32 SWITCH_CORE_S_RxUndersizePkts;
	uint8 dummy1603[28];
	uint32 SWITCH_CORE_S_RxPausePkts;
	uint8 dummy1604[28];
	uint32 SWITCH_CORE_S_RxPkts64Octets;
	uint8 dummy1605[28];
	uint32 SWITCH_CORE_S_RxPkts65to127Octets;
	uint8 dummy1606[28];
	uint32 SWITCH_CORE_S_RxPkts128to255Octets;
	uint8 dummy1607[28];
	uint32 SWITCH_CORE_S_RxPkts256to511Octets;
	uint8 dummy1608[28];
	uint32 SWITCH_CORE_S_RxPkts512to1023Octets;
	uint8 dummy1609[28];
	uint32 SWITCH_CORE_S_RxPkts1024toMaxPktOctets;
	uint8 dummy1610[28];
	uint32 SWITCH_CORE_S_RxOversizePkts;
	uint8 dummy1611[28];
	uint32 SWITCH_CORE_S_RxJabbers;
	uint8 dummy1612[28];
	uint32 SWITCH_CORE_S_RxAlignmentErrors;
	uint8 dummy1613[28];
	uint32 SWITCH_CORE_S_RxFCSErrors;
	uint8 dummy1614[28];
	uint32 SWITCH_CORE_S_RxGoodOctets;
	uint8 dummy1615[60];
	uint32 SWITCH_CORE_S_RxDropPkts;
	uint8 dummy1616[28];
	uint32 SWITCH_CORE_S_RxUnicastPkts;
	uint8 dummy1617[28];
	uint32 SWITCH_CORE_S_RxMulticastPkts;
	uint8 dummy1618[28];
	uint32 SWITCH_CORE_S_RxBroadcastPkts;
	uint8 dummy1619[28];
	uint32 SWITCH_CORE_S_RxSAChanges;
	uint8 dummy1620[28];
	uint32 SWITCH_CORE_S_RxFragments;
	uint8 dummy1621[28];
	uint32 SWITCH_CORE_S_RxJumboPkt;
	uint8 dummy1622[28];
	uint32 SWITCH_CORE_S_RxSymblErr;
	uint8 dummy1623[28];
	uint32 SWITCH_CORE_S_InRangeErrCount;
	uint8 dummy1624[28];
	uint32 SWITCH_CORE_S_OutRangeErrCount;
	uint8 dummy1625[28];
	uint32 SWITCH_CORE_S_EEE_LPI_EVENT;
	uint8 dummy1626[28];
	uint32 SWITCH_CORE_S_EEE_LPI_DURATION;
	uint8 dummy1627[28];
	uint32 SWITCH_CORE_S_RxDiscard;
	uint8 dummy1628[60];
	uint32 SWITCH_CORE_S_TxQPKTQ6;
	uint8 dummy1629[28];
	uint32 SWITCH_CORE_S_TxQPKTQ7;
	uint8 dummy1630[28];
	uint32 SWITCH_CORE_S_TxPkts64Octets;
	uint8 dummy1631[28];
	uint32 SWITCH_CORE_S_TxPkts65to127Octets;
	uint8 dummy1632[28];
	uint32 SWITCH_CORE_S_TxPkts128to255Octets;
	uint8 dummy1633[28];
	uint32 SWITCH_CORE_S_TxPkts256to511Octets;
	uint8 dummy1634[28];
	uint32 SWITCH_CORE_S_TxPkts512to1023Octets;
	uint8 dummy1635[28];
	uint32 SWITCH_CORE_S_TxPkts1024toMaxPktOctets;
	uint8 dummy1636[220];
	uint32 SWITCH_CORE_LPDET_CFG;
	uint8 dummy1637[12];
	uint64 SWITCH_CORE_DF_TIMER;
	uint32 SWITCH_CORE_LED_PORTMAP;
	uint8 dummy1638[12];
	uint32 SWITCH_CORE_MODULE_ID0;
	uint8 dummy1639[44];
	uint32 SWITCH_CORE_MODULE_ID1;
	uint8 dummy1640[44];
	uint32 SWITCH_CORE_LPDET_SA;
	uint8 dummy1641[116];
	uint32 SWITCH_CORE_LPDET_REG_SPARE0;
	uint8 dummy1642[28];
	uint32 SWITCH_CORE_LPDET_REG_SPARE1;
	uint8 dummy1643[1756];
	uint64 SWITCH_CORE_BPM_CTRL;
	uint64 SWITCH_CORE_BPM_PSM_OVR_CTRL;
	uint32 SWITCH_CORE_BPM_PSM_TIME_CFG;
	uint8 dummy1644[12];
	uint32 SWITCH_CORE_BPM_PSM_THD_CFG;
	uint8 dummy1645[28];
	uint32 SWITCH_CORE_ROW_VMASK_OVR_CTRL;
	uint8 dummy1646[28];
	uint32 SWITCH_CORE_BPM_STS;
	uint8 dummy1647[28];
	uint32 SWITCH_CORE_BPM_PDA_OVR_CTRL;
	uint8 dummy1648[12];
	uint32 SWITCH_CORE_PDA_TIMEOUT_CFG;
	uint8 dummy1649[12];
	uint32 SWITCH_CORE_PDA_SETUP_TIME_CFG;
	uint8 dummy1650[12];
	uint32 SWITCH_CORE_PDA_HOLD_TIME_CFG;
	uint8 dummy1651[12];
	uint32 SWITCH_CORE_PBB_VBUFCNT_P0;
	uint8 dummy1652[12];
	uint32 SWITCH_CORE_PBB_VBUFCNT_P1;
	uint8 dummy1653[12];
	uint32 SWITCH_CORE_PBB_VBUFCNT_P2;
	uint8 dummy1654[12];
	uint32 SWITCH_CORE_RCY_TIME_CFG;
	uint8 dummy1655[12];
	uint32 SWITCH_CORE_PBB_PWRDWN_MON_CTRL;
	uint8 dummy1656[60];
	uint32 SWITCH_CORE_PBB_PWRDWN_MON0;
	uint8 dummy1657[60];
	uint32 SWITCH_CORE_PBB_PWRDWN_MON1;
	uint8 dummy1658[60];
	uint32 SWITCH_CORE_PBB_PWRDWN_MON2;
	uint8 dummy1659[316];
	uint32 SWITCH_CORE_BPM_REG_SPARE0;
	uint8 dummy1660[28];
	uint32 SWITCH_CORE_BPM_REG_SPARE1;
	uint8 dummy1661[60636];
	uint32 SWITCH_CORE_TRREG_CTRL0;
	uint8 dummy1662[28];
	uint32 SWITCH_CORE_TRREG_CTRL1;
	uint8 dummy1663[28];
	uint32 SWITCH_CORE_TRREG_CTRL2;
	uint8 dummy1664[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_P0;
	uint8 dummy1665[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_P1;
	uint8 dummy1666[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_P2;
	uint8 dummy1667[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_P3;
	uint8 dummy1668[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_P4;
	uint8 dummy1669[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_P5;
	uint8 dummy1670[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_P6;
	uint8 dummy1671[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_P7;
	uint8 dummy1672[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2PCP_MAP_IMP;
	uint8 dummy1673[124];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_P0;
	uint8 dummy1674[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_P1;
	uint8 dummy1675[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_P2;
	uint8 dummy1676[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_P3;
	uint8 dummy1677[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_P4;
	uint8 dummy1678[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_P5;
	uint8 dummy1679[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_P6;
	uint8 dummy1680[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_P7;
	uint8 dummy1681[60];
	uint32 SWITCH_CORE_EGRESS_PKT_TC2CPCP_MAP_IMP;
	uint8 dummy1682[124];
	uint32 SWITCH_CORE_TRREG_REG_SPARE0;
	uint8 dummy1683[28];
	uint32 SWITCH_CORE_TRREG_REG_SPARE1;
	uint8 dummy1684[604];
	uint32 SWITCH_CORE_EEE_EN_CTRL;
	uint8 dummy1685[12];
	uint32 SWITCH_CORE_EEE_LPI_ASSERT;
	uint8 dummy1686[12];
	uint32 SWITCH_CORE_EEE_LPI_INDICATE;
	uint8 dummy1687[12];
	uint32 SWITCH_CORE_EEE_RX_IDLE_SYMBOL;
	uint8 dummy1688[12];
	uint32 SWITCH_CORE_EEE_LPI_SYMBOL_TX_DISABLE;
	uint8 dummy1689[28];
	uint32 SWITCH_CORE_EEE_PIPELINE_TIMER;
	uint8 dummy1690[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_P0;
	uint8 dummy1691[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_P1;
	uint8 dummy1692[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_P2;
	uint8 dummy1693[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_P3;
	uint8 dummy1694[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_P4;
	uint8 dummy1695[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_P5;
	uint8 dummy1696[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_P6;
	uint8 dummy1697[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_P7;
	uint8 dummy1698[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_G_IMP;
	uint8 dummy1699[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_P0;
	uint8 dummy1700[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_P1;
	uint8 dummy1701[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_P2;
	uint8 dummy1702[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_P3;
	uint8 dummy1703[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_P4;
	uint8 dummy1704[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_P5;
	uint8 dummy1705[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_P6;
	uint8 dummy1706[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_P7;
	uint8 dummy1707[28];
	uint32 SWITCH_CORE_EEE_SLEEP_TIMER_H_IMP;
	uint8 dummy1708[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_P0;
	uint8 dummy1709[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_P1;
	uint8 dummy1710[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_P2;
	uint8 dummy1711[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_P3;
	uint8 dummy1712[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_P4;
	uint8 dummy1713[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_P5;
	uint8 dummy1714[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_P6;
	uint8 dummy1715[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_P7;
	uint8 dummy1716[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_G_IMP;
	uint8 dummy1717[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_P0;
	uint8 dummy1718[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_P1;
	uint8 dummy1719[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_P2;
	uint8 dummy1720[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_P3;
	uint8 dummy1721[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_P4;
	uint8 dummy1722[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_P5;
	uint8 dummy1723[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_P6;
	uint8 dummy1724[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_P7;
	uint8 dummy1725[28];
	uint32 SWITCH_CORE_EEE_MIN_LP_TIMER_H_IMP;
	uint8 dummy1726[28];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P0;
	uint8 dummy1727[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P1;
	uint8 dummy1728[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P2;
	uint8 dummy1729[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P3;
	uint8 dummy1730[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P4;
	uint8 dummy1731[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P5;
	uint8 dummy1732[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P6;
	uint8 dummy1733[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P7;
	uint8 dummy1734[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_G_P8;
	uint8 dummy1735[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_P0;
	uint8 dummy1736[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_P1;
	uint8 dummy1737[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_P2;
	uint8 dummy1738[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_P3;
	uint8 dummy1739[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_P4;
	uint8 dummy1740[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_P5;
	uint8 dummy1741[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_P6;
	uint8 dummy1742[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_P7;
	uint8 dummy1743[12];
	uint32 SWITCH_CORE_EEE_WAKE_TIMER_H_IMP;
	uint8 dummy1744[12];
	uint32 SWITCH_CORE_EEE_GLB_CONG_TH;
	uint8 dummy1745[12];
	uint32 SWITCH_CORE_EEE_TX_CONG_TH_Q0;
	uint8 dummy1746[12];
	uint32 SWITCH_CORE_EEE_TX_CONG_TH_Q1;
	uint8 dummy1747[12];
	uint32 SWITCH_CORE_EEE_TX_CONG_TH_Q2;
	uint8 dummy1748[12];
	uint32 SWITCH_CORE_EEE_TX_CONG_TH_Q3;
	uint8 dummy1749[12];
	uint32 SWITCH_CORE_EEE_TX_CONG_TH_Q4;
	uint8 dummy1750[12];
	uint32 SWITCH_CORE_EEE_TX_CONG_TH_Q5;
	uint8 dummy1751[20];
	uint32 SWITCH_CORE_EEE_TX_CONG_TH_Q6;
	uint8 dummy1752[12];
	uint32 SWITCH_CORE_EEE_TX_CONG_TH_Q7;
	uint8 dummy1753[44];
	uint32 SWITCH_CORE_EEE_CTL_REG_SPARE0;
	uint8 dummy1754[36];
	uint32 SWITCH_CORE_EEE_CTL_REG_SPARE1;
	uint8 dummy1755[52];
	uint64 SWITCH_CORE_EEE_DEBUG;
	uint32 SWITCH_CORE_EEE_LINK_DLY_TIMER;
	uint8 dummy1756[28];
	uint32 SWITCH_CORE_EEE_STATE;
	uint8 dummy1757[156];
	uint32 SWITCH_CORE_PORT_ENABLE;
	uint8 dummy1758[12];
	uint32 SWITCH_CORE_TX_MODE_PORT_P0;
	uint8 dummy1759[12];
	uint32 SWITCH_CORE_TX_MODE_PORT_P1;
	uint8 dummy1760[12];
	uint32 SWITCH_CORE_TX_MODE_PORT_P2;
	uint8 dummy1761[12];
	uint32 SWITCH_CORE_TX_MODE_PORT_P3;
	uint8 dummy1762[12];
	uint32 SWITCH_CORE_TX_MODE_PORT_P4;
	uint8 dummy1763[12];
	uint32 SWITCH_CORE_TX_MODE_PORT_P5;
	uint8 dummy1764[12];
	uint32 SWITCH_CORE_TX_MODE_PORT_P7;
	uint8 dummy1765[12];
	uint32 SWITCH_CORE_TX_MODE_PORT_IMP;
	uint8 dummy1766[12];
	uint32 SWITCH_CORE_RX_MODE_PORT_P0;
	uint8 dummy1767[12];
	uint32 SWITCH_CORE_RX_MODE_PORT_P1;
	uint8 dummy1768[12];
	uint32 SWITCH_CORE_RX_MODE_PORT_P2;
	uint8 dummy1769[12];
	uint32 SWITCH_CORE_RX_MODE_PORT_P3;
	uint8 dummy1770[12];
	uint32 SWITCH_CORE_RX_MODE_PORT_P4;
	uint8 dummy1771[12];
	uint32 SWITCH_CORE_RX_MODE_PORT_P5;
	uint8 dummy1772[12];
	uint32 SWITCH_CORE_RX_MODE_PORT_P7;
	uint8 dummy1773[12];
	uint32 SWITCH_CORE_RX_MODE_PORT_IMP;
	uint8 dummy1774[12];
	uint32 SWITCH_CORE_TX_TS_CAP;
	uint8 dummy1775[12];
	uint32 SWITCH_CORE_RX_TS_CAP;
	uint8 dummy1776[12];
	uint32 SWITCH_CORE_RX_TX_OPTION;
	uint8 dummy1777[12];
	uint32 SWITCH_CORE_RX_PORT_0_LINK_DELAY_LSB;
	uint8 dummy1778[12];
	uint32 SWITCH_CORE_RX_PORT_0_LINK_DELAY_MSB;
	uint8 dummy1779[12];
	uint32 SWITCH_CORE_RX_PORT_1_LINK_DELAY_LSB;
	uint8 dummy1780[12];
	uint32 SWITCH_CORE_RX_PORT_1_LINK_DELAY_MSB;
	uint8 dummy1781[12];
	uint32 SWITCH_CORE_RX_PORT_2_LINK_DELAY_LSB;
	uint8 dummy1782[12];
	uint32 SWITCH_CORE_RX_PORT_2_LINK_DELAY_MSB;
	uint8 dummy1783[12];
	uint32 SWITCH_CORE_RX_PORT_3_LINK_DELAY_LSB;
	uint8 dummy1784[12];
	uint32 SWITCH_CORE_RX_PORT_3_LINK_DELAY_MSB;
	uint8 dummy1785[12];
	uint32 SWITCH_CORE_RX_PORT_4_LINK_DELAY_LSB;
	uint8 dummy1786[12];
	uint32 SWITCH_CORE_RX_PORT_4_LINK_DELAY_MSB;
	uint8 dummy1787[12];
	uint32 SWITCH_CORE_RX_PORT_5_LINK_DELAY_LSB;
	uint8 dummy1788[12];
	uint32 SWITCH_CORE_RX_PORT_5_LINK_DELAY_MSB;
	uint8 dummy1789[12];
	uint32 SWITCH_CORE_RX_PORT_7_LINK_DELAY_LSB;
	uint8 dummy1790[12];
	uint32 SWITCH_CORE_RX_PORT_7_LINK_DELAY_MSB;
	uint8 dummy1791[12];
	uint32 SWITCH_CORE_RX_PORT_8_LINK_DELAY_LSB;
	uint8 dummy1792[12];
	uint32 SWITCH_CORE_RX_PORT_8_LINK_DELAY_MSB;
	uint8 dummy1793[12];
	uint32 SWITCH_CORE_RX_PORT_0_TS_OFFSET_LSB;
	uint8 dummy1794[12];
	uint32 SWITCH_CORE_RX_PORT_0_TS_OFFSET_MSB;
	uint8 dummy1795[12];
	uint32 SWITCH_CORE_RX_PORT_1_TS_OFFSET_LSB;
	uint8 dummy1796[12];
	uint32 SWITCH_CORE_RX_PORT_1_TS_OFFSET_MSB;
	uint8 dummy1797[12];
	uint32 SWITCH_CORE_RX_PORT_2_TS_OFFSET_LSB;
	uint8 dummy1798[12];
	uint32 SWITCH_CORE_RX_PORT_2_TS_OFFSET_MSB;
	uint8 dummy1799[12];
	uint32 SWITCH_CORE_RX_PORT_3_TS_OFFSET_LSB;
	uint8 dummy1800[12];
	uint32 SWITCH_CORE_RX_PORT_3_TS_OFFSET_MSB;
	uint8 dummy1801[12];
	uint32 SWITCH_CORE_RX_PORT_4_TS_OFFSET_LSB;
	uint8 dummy1802[12];
	uint32 SWITCH_CORE_RX_PORT_4_TS_OFFSET_MSB;
	uint8 dummy1803[12];
	uint32 SWITCH_CORE_RX_PORT_5_TS_OFFSET_LSB;
	uint8 dummy1804[12];
	uint32 SWITCH_CORE_RX_PORT_5_TS_OFFSET_MSB;
	uint8 dummy1805[12];
	uint32 SWITCH_CORE_RX_PORT_7_TS_OFFSET_LSB;
	uint8 dummy1806[12];
	uint32 SWITCH_CORE_RX_PORT_7_TS_OFFSET_MSB;
	uint8 dummy1807[12];
	uint32 SWITCH_CORE_RX_PORT_8_TS_OFFSET_LSB;
	uint8 dummy1808[12];
	uint32 SWITCH_CORE_RX_PORT_8_TS_OFFSET_MSB;
	uint8 dummy1809[12];
	uint32 SWITCH_CORE_TX_PORT_0_TS_OFFSET_LSB;
	uint8 dummy1810[12];
	uint32 SWITCH_CORE_TX_PORT_0_TS_OFFSET_MSB;
	uint8 dummy1811[12];
	uint32 SWITCH_CORE_TX_PORT_1_TS_OFFSET_LSB;
	uint8 dummy1812[12];
	uint32 SWITCH_CORE_TX_PORT_1_TS_OFFSET_MSB;
	uint8 dummy1813[12];
	uint32 SWITCH_CORE_TX_PORT_2_TS_OFFSET_LSB;
	uint8 dummy1814[12];
	uint32 SWITCH_CORE_TX_PORT_2_TS_OFFSET_MSB;
	uint8 dummy1815[12];
	uint32 SWITCH_CORE_TX_PORT_3_TS_OFFSET_LSB;
	uint8 dummy1816[12];
	uint32 SWITCH_CORE_TX_PORT_3_TS_OFFSET_MSB;
	uint8 dummy1817[12];
	uint32 SWITCH_CORE_TX_PORT_4_TS_OFFSET_LSB;
	uint8 dummy1818[12];
	uint32 SWITCH_CORE_TX_PORT_4_TS_OFFSET_MSB;
	uint8 dummy1819[12];
	uint32 SWITCH_CORE_TX_PORT_5_TS_OFFSET_LSB;
	uint8 dummy1820[12];
	uint32 SWITCH_CORE_TX_PORT_5_TS_OFFSET_MSB;
	uint8 dummy1821[12];
	uint32 SWITCH_CORE_TX_PORT_7_TS_OFFSET_LSB;
	uint8 dummy1822[12];
	uint32 SWITCH_CORE_TX_PORT_7_TS_OFFSET_MSB;
	uint8 dummy1823[12];
	uint32 SWITCH_CORE_TX_PORT_8_TS_OFFSET_LSB;
	uint8 dummy1824[12];
	uint32 SWITCH_CORE_TX_PORT_8_TS_OFFSET_MSB;
	uint8 dummy1825[12];
	uint32 SWITCH_CORE_TIME_CODE_N_P0;
	uint8 dummy1826[12];
	uint32 SWITCH_CORE_TIME_CODE_N_P1;
	uint8 dummy1827[12];
	uint32 SWITCH_CORE_TIME_CODE_N_P2;
	uint8 dummy1828[12];
	uint32 SWITCH_CORE_TIME_CODE_N_P3;
	uint8 dummy1829[12];
	uint32 SWITCH_CORE_TIME_CODE_N_P4;
	uint8 dummy1830[12];
	uint32 SWITCH_CORE_DPLL_DB_LSB;
	uint8 dummy1831[12];
	uint32 SWITCH_CORE_DPLL_DB_MSB;
	uint8 dummy1832[12];
	uint32 SWITCH_CORE_DPLL_DB_SEL;
	uint8 dummy1833[12];
	uint32 SWITCH_CORE_SHD_CTL;
	uint8 dummy1834[12];
	uint32 SWITCH_CORE_SHD_LD;
	uint8 dummy1835[12];
	uint32 SWITCH_CORE_INT_MASK;
	uint8 dummy1836[12];
	uint32 SWITCH_CORE_INT_STAT;
	uint8 dummy1837[12];
	uint32 SWITCH_CORE_TX_CTL;
	uint8 dummy1838[12];
	uint32 SWITCH_CORE_RX_CTL;
	uint8 dummy1839[12];
	uint32 SWITCH_CORE_RX_TX_CTL;
	uint8 dummy1840[12];
	uint32 SWITCH_CORE_VLAN_ITPID;
	uint8 dummy1841[12];
	uint32 SWITCH_CORE_VLAN_OTPID;
	uint8 dummy1842[12];
	uint32 SWITCH_CORE_OTHER_OTPID;
	uint8 dummy1843[12];
	uint32 SWITCH_CORE_NSE_DPLL_1;
	uint8 dummy1844[12];
	uint32 SWITCH_CORE_NSE_DPLL_2_N_P0;
	uint8 dummy1845[12];
	uint32 SWITCH_CORE_NSE_DPLL_2_N_P1;
	uint8 dummy1846[12];
	uint32 SWITCH_CORE_NSE_DPLL_2_N_P2;
	uint8 dummy1847[12];
	uint32 SWITCH_CORE_NSE_DPLL_3_N_P0;
	uint8 dummy1848[12];
	uint32 SWITCH_CORE_NSE_DPLL_3_N_P1;
	uint8 dummy1849[12];
	uint32 SWITCH_CORE_NSE_DPLL_4;
	uint8 dummy1850[12];
	uint32 SWITCH_CORE_NSE_DPLL_5;
	uint8 dummy1851[12];
	uint32 SWITCH_CORE_NSE_DPLL_6;
	uint8 dummy1852[12];
	uint32 SWITCH_CORE_NSE_DPLL_7_N_P0;
	uint8 dummy1853[12];
	uint32 SWITCH_CORE_NSE_DPLL_7_N_P1;
	uint8 dummy1854[12];
	uint32 SWITCH_CORE_NSE_DPLL_7_N_P2;
	uint8 dummy1855[12];
	uint32 SWITCH_CORE_NSE_DPLL_7_N_P3;
	uint8 dummy1856[12];
	uint32 SWITCH_CORE_NSE_NCO_1_N_P0;
	uint8 dummy1857[12];
	uint32 SWITCH_CORE_NSE_NCO_1_N_P1;
	uint8 dummy1858[12];
	uint32 SWITCH_CORE_NSE_NCO_2_N_P0;
	uint8 dummy1859[12];
	uint32 SWITCH_CORE_NSE_NCO_2_N_P1;
	uint8 dummy1860[12];
	uint32 SWITCH_CORE_NSE_NCO_2_N_P2;
	uint8 dummy1861[12];
	uint32 SWITCH_CORE_NSE_NCO_3_0;
	uint8 dummy1862[12];
	uint32 SWITCH_CORE_NSE_NCO_3_1;
	uint8 dummy1863[12];
	uint32 SWITCH_CORE_NSE_NCO_3_2;
	uint8 dummy1864[12];
	uint32 SWITCH_CORE_NSE_NCO_4;
	uint8 dummy1865[12];
	uint32 SWITCH_CORE_NSE_NCO_5_0;
	uint8 dummy1866[12];
	uint32 SWITCH_CORE_NSE_NCO_5_1;
	uint8 dummy1867[12];
	uint32 SWITCH_CORE_NSE_NCO_5_2;
	uint8 dummy1868[12];
	uint32 SWITCH_CORE_NSE_NCO_6;
	uint8 dummy1869[12];
	uint32 SWITCH_CORE_NSE_NCO_7_0;
	uint8 dummy1870[12];
	uint32 SWITCH_CORE_NSE_NCO_7_1;
	uint8 dummy1871[12];
	uint32 SWITCH_CORE_TX_COUNTER;
	uint8 dummy1872[12];
	uint32 SWITCH_CORE_RX_COUNTER;
	uint8 dummy1873[12];
	uint32 SWITCH_CORE_RX_TX_1588_COUNTER;
	uint8 dummy1874[12];
	uint32 SWITCH_CORE_TS_READ_START_END;
	uint8 dummy1875[12];
	uint32 SWITCH_CORE_HEARTBEAT_0;
	uint8 dummy1876[12];
	uint32 SWITCH_CORE_HEARTBEAT_1;
	uint8 dummy1877[140];
	uint32 SWITCH_CORE_HEARTBEAT_2;
	uint8 dummy1878[12];
	uint32 SWITCH_CORE_TIME_STAMP_N_P0;
	uint8 dummy1879[12];
	uint32 SWITCH_CORE_TIME_STAMP_N_P1;
	uint8 dummy1880[12];
	uint32 SWITCH_CORE_TIME_STAMP_N_P2;
	uint8 dummy1881[12];
	uint32 SWITCH_CORE_TIME_STAMP_INFO_N_P0;
	uint8 dummy1882[12];
	uint32 SWITCH_CORE_TIME_STAMP_INFO_N_P1;
	uint8 dummy1883[12];
	uint32 SWITCH_CORE_CNTR_DBG;
	uint8 dummy1884[12];
	uint32 SWITCH_CORE_MPLS_SPARE1;
	uint8 dummy1885[12];
	uint32 SWITCH_CORE_MPLS_SPARE2;
	uint8 dummy1886[12];
	uint32 SWITCH_CORE_MPLS_SPARE3;
	uint8 dummy1887[12];
	uint32 SWITCH_CORE_MPLS_SPARE4;
	uint8 dummy1888[12];
	uint32 SWITCH_CORE_MPLS_SPARE5;
	uint8 dummy1889[12];
	uint32 SWITCH_CORE_MPLS_SPARE6;
	uint8 dummy1890[12];
	uint32 SWITCH_CORE_MPLS_TX_CNTL;
	uint8 dummy1891[12];
	uint32 SWITCH_CORE_MPLS_RX_CNTL;
	uint8 dummy1892[12];
	uint32 SWITCH_CORE_MPLS_LABEL1_MASK_LSB;
	uint8 dummy1893[12];
	uint32 SWITCH_CORE_MPLS_LABEL1_MASK_MSB;
	uint8 dummy1894[12];
	uint32 SWITCH_CORE_MPLS_LABEL1_VALUE_LSB;
	uint8 dummy1895[12];
	uint32 SWITCH_CORE_MPLS_LABEL1_VALUE_MSB;
	uint8 dummy1896[12];
	uint32 SWITCH_CORE_MPLS_LABEL2_MASK_LSB;
	uint8 dummy1897[12];
	uint32 SWITCH_CORE_MPLS_LABEL2_MASK_MSB;
	uint8 dummy1898[12];
	uint32 SWITCH_CORE_MPLS_LABEL2_VALUE_LSB;
	uint8 dummy1899[12];
	uint32 SWITCH_CORE_MPLS_LABEL2_VALUE_MSB;
	uint8 dummy1900[12];
	uint32 SWITCH_CORE_MPLS_LABEL3_MASK_LSB;
	uint8 dummy1901[12];
	uint32 SWITCH_CORE_MPLS_LABEL3_MASK_MSB;
	uint8 dummy1902[12];
	uint32 SWITCH_CORE_MPLS_LABEL3_VALUE_LSB;
	uint8 dummy1903[12];
	uint32 SWITCH_CORE_MPLS_LABEL3_VALUE_MSB;
	uint8 dummy1904[12];
	uint32 SWITCH_CORE_MPLS_LABEL4_MASK_LSB;
	uint8 dummy1905[12];
	uint32 SWITCH_CORE_MPLS_LABEL4_MASK_MSB;
	uint8 dummy1906[12];
	uint32 SWITCH_CORE_MPLS_LABEL4_VALUE_LSB;
	uint8 dummy1907[12];
	uint32 SWITCH_CORE_MPLS_LABEL4_VALUE_MSB;
	uint8 dummy1908[12];
	uint32 SWITCH_CORE_MPLS_LABEL5_MASK_LSB;
	uint8 dummy1909[12];
	uint32 SWITCH_CORE_MPLS_LABEL5_MASK_MSB;
	uint8 dummy1910[12];
	uint32 SWITCH_CORE_MPLS_LABEL5_VALUE_LSB;
	uint8 dummy1911[12];
	uint32 SWITCH_CORE_MPLS_LABEL5_VALUE_MSB;
	uint8 dummy1912[12];
	uint32 SWITCH_CORE_MPLS_LABEL6_MASK_LSB;
	uint8 dummy1913[12];
	uint32 SWITCH_CORE_MPLS_LABEL6_MASK_MSB;
	uint8 dummy1914[12];
	uint32 SWITCH_CORE_MPLS_LABEL6_VALUE_LSB;
	uint8 dummy1915[12];
	uint32 SWITCH_CORE_MPLS_LABEL6_VALUE_MSB;
	uint8 dummy1916[12];
	uint32 SWITCH_CORE_MPLS_LABEL7_MASK_LSB;
	uint8 dummy1917[12];
	uint32 SWITCH_CORE_MPLS_LABEL7_MASK_MSB;
	uint8 dummy1918[12];
	uint32 SWITCH_CORE_MPLS_LABEL7_VALUE_LSB;
	uint8 dummy1919[12];
	uint32 SWITCH_CORE_MPLS_LABEL7_VALUE_MSB;
	uint8 dummy1920[12];
	uint32 SWITCH_CORE_MPLS_LABEL8_MASK_LSB;
	uint8 dummy1921[12];
	uint32 SWITCH_CORE_MPLS_LABEL8_MASK_MSB;
	uint8 dummy1922[12];
	uint32 SWITCH_CORE_MPLS_LABEL8_VALUE_LSB;
	uint8 dummy1923[12];
	uint32 SWITCH_CORE_MPLS_LABEL8_VALUE_MSB;
	uint8 dummy1924[12];
	uint32 SWITCH_CORE_MPLS_LABEL9_MASK_LSB;
	uint8 dummy1925[12];
	uint32 SWITCH_CORE_MPLS_LABEL9_MASK_MSB;
	uint8 dummy1926[12];
	uint32 SWITCH_CORE_MPLS_LABEL9_VALUE_LSB;
	uint8 dummy1927[12];
	uint32 SWITCH_CORE_MPLS_LABEL9_VALUE_MSB;
	uint8 dummy1928[12];
	uint32 SWITCH_CORE_MPLS_LABEL10_MASK_LSB;
	uint8 dummy1929[12];
	uint32 SWITCH_CORE_MPLS_LABEL10_MASK_MSB;
	uint8 dummy1930[12];
	uint32 SWITCH_CORE_MPLS_LABEL10_VALUE_LSB;
	uint8 dummy1931[12];
	uint32 SWITCH_CORE_MPLS_LABEL10_VALUE_MSB;
	uint8 dummy1932[12];
	uint32 SWITCH_CORE_RX_TX_1588_COUNTER1;
	uint8 dummy1933[12];
	uint32 SWITCH_CORE_RX_CF_SPEC;
	uint8 dummy1934[12];
	uint32 SWITCH_CORE_TX_CF_SPEC;
	uint8 dummy1935[12];
	uint32 SWITCH_CORE_MPLS_PACKET_ENABLE;
	uint8 dummy1936[12];
	uint32 SWITCH_CORE_TIMECODE_SEL;
	uint8 dummy1937[12];
	uint32 SWITCH_CORE_TIME_STAMP_3;
	uint8 dummy1938[12];
	uint32 SWITCH_CORE_TIME_STAMP;
	uint8 dummy1939[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_TX_CONTROL;
	uint8 dummy1940[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_RX_CONTROL;
	uint8 dummy1941[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE1;
	uint8 dummy1942[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE2;
	uint8 dummy1943[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE3;
	uint8 dummy1944[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE4;
	uint8 dummy1945[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE5;
	uint8 dummy1946[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE6;
	uint8 dummy1947[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE7;
	uint8 dummy1948[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE8;
	uint8 dummy1949[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE9;
	uint8 dummy1950[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE10;
	uint8 dummy1951[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE11;
	uint8 dummy1952[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE12;
	uint8 dummy1953[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_ETYPE13;
	uint8 dummy1954[12];
	uint32 SWITCH_CORE_DELAY_MEASUREMENT_IETF_OFFSET;
	uint8 dummy1955[12];
	uint32 SWITCH_CORE_NTP_TIME_STAMP_N_P0;
	uint8 dummy1956[12];
	uint32 SWITCH_CORE_NTP_TIME_STAMP_N_P1;
	uint8 dummy1957[12];
	uint32 SWITCH_CORE_NTP_TIME_STAMP_N_P2;
	uint8 dummy1958[12];
	uint32 SWITCH_CORE_NTP_TIME_STAMP_N_P3;
	uint8 dummy1959[12];
	uint32 SWITCH_CORE_NTP_NCO_FREQ_0;
	uint8 dummy1960[12];
	uint32 SWITCH_CORE_NTP_NCO_FREQ_1;
	uint8 dummy1961[12];
	uint32 SWITCH_CORE_NTP_DOWN_CNTER_0;
	uint8 dummy1962[12];
	uint32 SWITCH_CORE_NTP_DOWN_CNTER_1;
	uint8 dummy1963[12];
	uint32 SWITCH_CORE_NTP_ERR_LSB;
	uint8 dummy1964[12];
	uint32 SWITCH_CORE_NTP_ERR_MSB;
	uint8 dummy1965[12];
	uint32 SWITCH_CORE_DM_MAC_L1_0;
	uint8 dummy1966[12];
	uint32 SWITCH_CORE_DM_MAC_L1_1;
	uint8 dummy1967[12];
	uint32 SWITCH_CORE_DM_MAC_L1_2;
	uint8 dummy1968[12];
	uint32 SWITCH_CORE_DM_MAC_L2_0;
	uint8 dummy1969[12];
	uint32 SWITCH_CORE_DM_MAC_L2_1;
	uint8 dummy1970[12];
	uint32 SWITCH_CORE_DM_MAC_L2_2;
	uint8 dummy1971[12];
	uint32 SWITCH_CORE_DM_MAC_L3_0;
	uint8 dummy1972[12];
	uint32 SWITCH_CORE_DM_MAC_L3_1;
	uint8 dummy1973[12];
	uint32 SWITCH_CORE_DM_MAC_L3_2;
	uint8 dummy1974[12];
	uint32 SWITCH_CORE_DM_MAC_CTL_0;
	uint8 dummy1975[12];
	uint32 SWITCH_CORE_DM_MAC_CTL_1;
	uint8 dummy1976[12];
	uint32 SWITCH_CORE_DM_MAC_CTL_2;
	uint8 dummy1977[12];
	uint32 SWITCH_CORE_HEARTBEAT_3;
	uint8 dummy1978[12];
	uint32 SWITCH_CORE_HEARTBEAT_4;
	uint8 dummy1979[12];
	uint32 SWITCH_CORE_INBAND_CNTL_N_P0;
	uint8 dummy1980[12];
	uint32 SWITCH_CORE_INBAND_CNTL_N_P1;
	uint8 dummy1981[12];
	uint32 SWITCH_CORE_INBAND_CNTL_N_P2;
	uint8 dummy1982[12];
	uint32 SWITCH_CORE_INBAND_CNTL_N_P3;
	uint8 dummy1983[12];
	uint32 SWITCH_CORE_INBAND_CNTL_N_P4;
	uint8 dummy1984[12];
	uint32 SWITCH_CORE_INBAND_CNTL_N_P5;
	uint8 dummy1985[12];
	uint32 SWITCH_CORE_INBAND_CNTL_N_P6;
	uint8 dummy1986[12];
	uint32 SWITCH_CORE_INBAND_CNTL_N_P7;
	uint8 dummy1987[12];
	uint32 SWITCH_CORE_MEM_COUNTER;
	uint8 dummy1988[12];
	uint32 SWITCH_CORE_TIMESTAMP_DELTA;
	uint8 dummy1989[12];
	uint32 SWITCH_CORE_SOP_SEL;
	uint8 dummy1990[12];
	uint32 SWITCH_CORE_TIME_STAMP_INFO_3;
	uint8 dummy1991[12];
	uint32 SWITCH_CORE_TIME_STAMP_INFO_4;
	uint8 dummy1992[12];
	uint32 SWITCH_CORE_TIME_STAMP_INFO_5;
	uint8 dummy1993[12];
	uint32 SWITCH_CORE_TIME_STAMP_INFO_6;
	uint8 dummy1994[12];
	uint32 SWITCH_CORE_TIME_STAMP_INFO_7;
	uint8 dummy1995[12];
	uint32 SWITCH_CORE_TIME_STAMP_INFO_8;
	uint8 dummy1996[12];
	uint32 SWITCH_CORE_INBAND_SPARE1;
	uint8 dummy1997[140];
	uint32 SWITCH_CORE_RED_CONTROL;
	uint8 dummy1998[12];
	uint32 SWITCH_CORE_TC2RED_PROFILE_TABLE;
	uint8 dummy1999[12];
	uint32 SWITCH_CORE_RED_EGRESS_BYPASS;
	uint8 dummy2000[12];
	uint32 SWITCH_CORE_RED_AQD_CONTROL;
	uint8 dummy2001[12];
	uint32 SWITCH_CORE_RED_EXPONENT;
	uint8 dummy2002[12];
	uint32 SWITCH_CORE_RED_DROP_ADD_TO_MIB;
	uint8 dummy2003[44];
	uint32 SWITCH_CORE_RED_PROFILE_DEFAULT;
	uint8 dummy2004[28];
	uint32 SWITCH_CORE_WRED_REG_SPARE0;
	uint8 dummy2005[28];
	uint32 SWITCH_CORE_WRED_REG_SPARE1;
	uint8 dummy2006[60];
	uint32 SWITCH_CORE_RED_PROFILE0;
	uint8 dummy2007[28];
	uint32 SWITCH_CORE_RED_PROFILE1;
	uint8 dummy2008[28];
	uint32 SWITCH_CORE_RED_PROFILE2;
	uint8 dummy2009[28];
	uint32 SWITCH_CORE_RED_PROFILE3;
	uint8 dummy2010[28];
	uint32 SWITCH_CORE_RED_PROFILE4;
	uint8 dummy2011[28];
	uint32 SWITCH_CORE_RED_PROFILE5;
	uint8 dummy2012[28];
	uint32 SWITCH_CORE_RED_PROFILE6;
	uint8 dummy2013[28];
	uint32 SWITCH_CORE_RED_PROFILE7;
	uint8 dummy2014[28];
	uint32 SWITCH_CORE_RED_PROFILE8;
	uint8 dummy2015[28];
	uint32 SWITCH_CORE_RED_PROFILE9;
	uint8 dummy2016[28];
	uint32 SWITCH_CORE_RED_PROFILE10;
	uint8 dummy2017[28];
	uint32 SWITCH_CORE_RED_PROFILE11;
	uint8 dummy2018[28];
	uint32 SWITCH_CORE_RED_PROFILE12;
	uint8 dummy2019[28];
	uint32 SWITCH_CORE_RED_PROFILE13;
	uint8 dummy2020[28];
	uint32 SWITCH_CORE_RED_PROFILE14;
	uint8 dummy2021[28];
	uint32 SWITCH_CORE_RED_PROFILE15;
	uint8 dummy2022[124];
	uint32 SWITCH_CORE_RED_DROP_CNTR_RST;
	uint8 dummy2023[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_P0;
	uint8 dummy2024[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_P1;
	uint8 dummy2025[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_P2;
	uint8 dummy2026[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_P3;
	uint8 dummy2027[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_P4;
	uint8 dummy2028[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_P5;
	uint8 dummy2029[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_P6;
	uint8 dummy2030[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_P7;
	uint8 dummy2031[28];
	uint32 SWITCH_CORE_RED_PKT_DROP_CNTR_IMP;
	uint8 dummy2032[124];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_P0;
	uint8 dummy2033[60];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_P1;
	uint8 dummy2034[60];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_P2;
	uint8 dummy2035[60];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_P3;
	uint8 dummy2036[60];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_P4;
	uint8 dummy2037[60];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_P5;
	uint8 dummy2038[60];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_P6;
	uint8 dummy2039[60];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_P7;
	uint8 dummy2040[60];
	uint32 SWITCH_CORE_RED_BYTE_DROP_CNTR_IMP;
	uint8 dummy2041[20732];
	uint32 SWITCH_CORE_CFP_ACC;
	uint8 dummy2042[28];
	uint32 SWITCH_CORE_RATE_METER_GLOBAL_CTL;
	uint8 dummy2043[92];
	uint32 SWITCH_CORE_CFP_DATA0;
	uint8 dummy2044[28];
	uint32 SWITCH_CORE_CFP_DATA1;
	uint8 dummy2045[28];
	uint32 SWITCH_CORE_CFP_DATA2;
	uint8 dummy2046[28];
	uint32 SWITCH_CORE_CFP_DATA3;
	uint8 dummy2047[28];
	uint32 SWITCH_CORE_CFP_DATA4;
	uint8 dummy2048[28];
	uint32 SWITCH_CORE_CFP_DATA5;
	uint8 dummy2049[28];
	uint32 SWITCH_CORE_CFP_DATA6;
	uint8 dummy2050[28];
	uint32 SWITCH_CORE_CFP_DATA7;
	uint8 dummy2051[28];
	uint32 SWITCH_CORE_CFP_MASK0;
	uint8 dummy2052[28];
	uint32 SWITCH_CORE_CFP_MASK1;
	uint8 dummy2053[28];
	uint32 SWITCH_CORE_CFP_MASK2;
	uint8 dummy2054[28];
	uint32 SWITCH_CORE_CFP_MASK3;
	uint8 dummy2055[28];
	uint32 SWITCH_CORE_CFP_MASK4;
	uint8 dummy2056[28];
	uint32 SWITCH_CORE_CFP_MASK5;
	uint8 dummy2057[28];
	uint32 SWITCH_CORE_CFP_MASK6;
	uint8 dummy2058[28];
	uint32 SWITCH_CORE_CFP_MASK7;
	uint8 dummy2059[28];
	uint32 SWITCH_CORE_ACT_POL_DATA0;
	uint8 dummy2060[28];
	uint32 SWITCH_CORE_ACT_POL_DATA1;
	uint8 dummy2061[28];
	uint32 SWITCH_CORE_ACT_POL_DATA2;
	uint8 dummy2062[60];
	uint32 SWITCH_CORE_RATE_METER0;
	uint8 dummy2063[28];
	uint32 SWITCH_CORE_RATE_METER1;
	uint8 dummy2064[28];
	uint32 SWITCH_CORE_RATE_METER2;
	uint8 dummy2065[28];
	uint32 SWITCH_CORE_RATE_METER3;
	uint8 dummy2066[28];
	uint32 SWITCH_CORE_RATE_METER4;
	uint8 dummy2067[28];
	uint32 SWITCH_CORE_RATE_METER5;
	uint8 dummy2068[28];
	uint32 SWITCH_CORE_RATE_METER6;
	uint8 dummy2069[28];
	uint32 SWITCH_CORE_TC2COLOR;
	uint8 dummy2070[28];
	uint32 SWITCH_CORE_STAT_GREEN_CNTR;
	uint8 dummy2071[28];
	uint32 SWITCH_CORE_STAT_YELLOW_CNTR;
	uint8 dummy2072[28];
	uint32 SWITCH_CORE_STAT_RED_CNTR;
	uint8 dummy2073[188];
	uint32 SWITCH_CORE_TCAM_BIST_CONTROL;
	uint8 dummy2074[28];
	uint32 SWITCH_CORE_TCAM_BIST_STATUS;
	uint8 dummy2075[28];
	uint32 SWITCH_CORE_TCAM_TEST_COMPARE_STATUS;
	uint8 dummy2076[60];
	uint32 SWITCH_CORE_CFP_REG_SPARE0;
	uint8 dummy2077[28];
	uint32 SWITCH_CORE_CFP_REG_SPARE1;
	uint8 dummy2078[604];
	uint32 SWITCH_CORE_CFP_CTL_REG;
	uint8 dummy2079[124];
	uint64 SWITCH_CORE_UDF_0_A_0_8_0;
	uint64 SWITCH_CORE_UDF_0_A_0_8_1;
	uint64 SWITCH_CORE_UDF_0_A_0_8_2;
	uint64 SWITCH_CORE_UDF_0_A_0_8_3;
	uint64 SWITCH_CORE_UDF_0_A_0_8_4;
	uint64 SWITCH_CORE_UDF_0_A_0_8_5;
	uint64 SWITCH_CORE_UDF_0_A_0_8_6;
	uint64 SWITCH_CORE_UDF_0_A_0_8_7;
	uint32 SWITCH_CORE_UDF_0_A_0_8_8;
	uint8 dummy2080[60];
	uint64 SWITCH_CORE_UDF_1_A_0_8_0;
	uint64 SWITCH_CORE_UDF_1_A_0_8_1;
	uint64 SWITCH_CORE_UDF_1_A_0_8_2;
	uint64 SWITCH_CORE_UDF_1_A_0_8_3;
	uint64 SWITCH_CORE_UDF_1_A_0_8_4;
	uint64 SWITCH_CORE_UDF_1_A_0_8_5;
	uint64 SWITCH_CORE_UDF_1_A_0_8_6;
	uint64 SWITCH_CORE_UDF_1_A_0_8_7;
	uint32 SWITCH_CORE_UDF_1_A_0_8_8;
	uint8 dummy2081[60];
	uint64 SWITCH_CORE_UDF_2_A_0_8_0;
	uint64 SWITCH_CORE_UDF_2_A_0_8_1;
	uint64 SWITCH_CORE_UDF_2_A_0_8_2;
	uint64 SWITCH_CORE_UDF_2_A_0_8_3;
	uint64 SWITCH_CORE_UDF_2_A_0_8_4;
	uint64 SWITCH_CORE_UDF_2_A_0_8_5;
	uint64 SWITCH_CORE_UDF_2_A_0_8_6;
	uint64 SWITCH_CORE_UDF_2_A_0_8_7;
	uint32 SWITCH_CORE_UDF_2_A_0_8_8;
	uint8 dummy2082[60];
	uint64 SWITCH_CORE_UDF_0_B_0_8_0;
	uint64 SWITCH_CORE_UDF_0_B_0_8_1;
	uint64 SWITCH_CORE_UDF_0_B_0_8_2;
	uint64 SWITCH_CORE_UDF_0_B_0_8_3;
	uint64 SWITCH_CORE_UDF_0_B_0_8_4;
	uint64 SWITCH_CORE_UDF_0_B_0_8_5;
	uint64 SWITCH_CORE_UDF_0_B_0_8_6;
	uint64 SWITCH_CORE_UDF_0_B_0_8_7;
	uint32 SWITCH_CORE_UDF_0_B_0_8_8;
	uint8 dummy2083[60];
	uint64 SWITCH_CORE_UDF_1_B_0_8_0;
	uint64 SWITCH_CORE_UDF_1_B_0_8_1;
	uint64 SWITCH_CORE_UDF_1_B_0_8_2;
	uint64 SWITCH_CORE_UDF_1_B_0_8_3;
	uint64 SWITCH_CORE_UDF_1_B_0_8_4;
	uint64 SWITCH_CORE_UDF_1_B_0_8_5;
	uint64 SWITCH_CORE_UDF_1_B_0_8_6;
	uint64 SWITCH_CORE_UDF_1_B_0_8_7;
	uint32 SWITCH_CORE_UDF_1_B_0_8_8;
	uint8 dummy2084[60];
	uint64 SWITCH_CORE_UDF_2_B_0_8_0;
	uint64 SWITCH_CORE_UDF_2_B_0_8_1;
	uint64 SWITCH_CORE_UDF_2_B_0_8_2;
	uint64 SWITCH_CORE_UDF_2_B_0_8_3;
	uint64 SWITCH_CORE_UDF_2_B_0_8_4;
	uint64 SWITCH_CORE_UDF_2_B_0_8_5;
	uint64 SWITCH_CORE_UDF_2_B_0_8_6;
	uint64 SWITCH_CORE_UDF_2_B_0_8_7;
	uint32 SWITCH_CORE_UDF_2_B_0_8_8;
	uint8 dummy2085[60];
	uint64 SWITCH_CORE_UDF_0_C_0_8_0;
	uint64 SWITCH_CORE_UDF_0_C_0_8_1;
	uint64 SWITCH_CORE_UDF_0_C_0_8_2;
	uint64 SWITCH_CORE_UDF_0_C_0_8_3;
	uint64 SWITCH_CORE_UDF_0_C_0_8_4;
	uint64 SWITCH_CORE_UDF_0_C_0_8_5;
	uint64 SWITCH_CORE_UDF_0_C_0_8_6;
	uint64 SWITCH_CORE_UDF_0_C_0_8_7;
	uint32 SWITCH_CORE_UDF_0_C_0_8_8;
	uint8 dummy2086[60];
	uint64 SWITCH_CORE_UDF_1_C_0_8_0;
	uint64 SWITCH_CORE_UDF_1_C_0_8_1;
	uint64 SWITCH_CORE_UDF_1_C_0_8_2;
	uint64 SWITCH_CORE_UDF_1_C_0_8_3;
	uint64 SWITCH_CORE_UDF_1_C_0_8_4;
	uint64 SWITCH_CORE_UDF_1_C_0_8_5;
	uint64 SWITCH_CORE_UDF_1_C_0_8_6;
	uint64 SWITCH_CORE_UDF_1_C_0_8_7;
	uint32 SWITCH_CORE_UDF_1_C_0_8_8;
	uint8 dummy2087[60];
	uint64 SWITCH_CORE_UDF_2_C_0_8_0;
	uint64 SWITCH_CORE_UDF_2_C_0_8_1;
	uint64 SWITCH_CORE_UDF_2_C_0_8_2;
	uint64 SWITCH_CORE_UDF_2_C_0_8_3;
	uint64 SWITCH_CORE_UDF_2_C_0_8_4;
	uint64 SWITCH_CORE_UDF_2_C_0_8_5;
	uint64 SWITCH_CORE_UDF_2_C_0_8_6;
	uint64 SWITCH_CORE_UDF_2_C_0_8_7;
	uint32 SWITCH_CORE_UDF_2_C_0_8_8;
	uint8 dummy2088[60];
	uint64 SWITCH_CORE_UDF_0_D_0_11_0;
	uint64 SWITCH_CORE_UDF_0_D_0_11_1;
	uint64 SWITCH_CORE_UDF_0_D_0_11_2;
	uint64 SWITCH_CORE_UDF_0_D_0_11_3;
	uint64 SWITCH_CORE_UDF_0_D_0_11_4;
	uint64 SWITCH_CORE_UDF_0_D_0_11_5;
	uint64 SWITCH_CORE_UDF_0_D_0_11_6;
	uint64 SWITCH_CORE_UDF_0_D_0_11_7;
	uint64 SWITCH_CORE_UDF_0_D_0_11_8;
	uint64 SWITCH_CORE_UDF_0_D_0_11_9;
	uint64 SWITCH_CORE_UDF_0_D_0_11_10;
	uint32 SWITCH_CORE_UDF_0_D_0_11_11;
	uint8 dummy2089[29348];
	uint32 SWITCH_CORE_ARL_TCAM_ACC;
	uint8 dummy2090[28];
	uint32 SWITCH_CORE_ARL_TCAM_DATA_P0;
	uint8 dummy2091[28];
	uint32 SWITCH_CORE_ARL_TCAM_DATA_P1;
	uint8 dummy2092[92];
	uint32 SWITCH_CORE_ARL_SMEM_DATA;
	uint8 dummy2093[92];
	uint32 SWITCH_CORE_ARL_TCAM_BIST_CTRL;
	uint8 dummy2094[28];
	uint32 SWITCH_CORE_ARL_TCAM_BIST_STS;
	uint8 dummy2095[100060];
	uint32 SWITCH_CORE_PLL_NDIV_INT;
	uint8 dummy2096[12];
	uint32 SWITCH_CORE_PLL_NDIV_FRAC;
	uint8 dummy2097[28];
	uint32 SWITCH_CORE_PLL_SDMOD_CTRL;
	uint8 dummy2098[12];
	uint32 SWITCH_CORE_PLL_MOD_CTRL_0;
	uint8 dummy2099[28];
	uint32 SWITCH_CORE_PLL_MOD_CTRL_1;
	uint8 dummy2100[28];
	uint32 SWITCH_CORE_PLL_MOD_CTRL_2;
	uint8 dummy2101[28];
	uint32 SWITCH_CORE_PLL_MISC_CTRL;
	uint8 dummy2102[12];
	uint32 SWITCH_CORE_PLL_DELOCK_MIB;
	uint8 dummy2103[28];
	uint32 SWITCH_CORE_PLL_SS_CTL;
	uint8 dummy2104[44];
	uint32 SWITCH_CORE_PLL_CTRL;
	uint8 dummy2105[12];
	uint32 SWITCH_CORE_PLL_STS;
	uint8 dummy2106[60];
	uint32 SWITCH_CORE_PLL_FREQ_SEL;
	uint8 dummy2107[44];
	uint32 SWITCH_CORE_PLL_TEST_CTRL_I;
	uint8 dummy2108[12];
	uint32 SWITCH_CORE_PLL_TEST_CTRL_II;
	uint8 dummy2109[108];
	uint32 SWITCH_CORE_GREEN_MODE_DATA;
	uint8 dummy2110[60];
	uint32 SWITCH_CORE_GREEN_MODE_SELECT;
	uint8 dummy2111[444];
	uint32 SWITCH_CORE_TOP_LOW_POWER_CTRL;
	uint8 dummy2112[12];
	uint32 SWITCH_CORE_TOP_IDDQ_CTL;
	uint8 dummy2113[108];
	uint32 SWITCH_CORE_IP_PLL_BYPASS;
	uint8 dummy2114[636];
	uint32 SWITCH_CORE_TOP_MODULE_CTL_SPARE0;
	uint8 dummy2115[28];
	uint32 SWITCH_CORE_TOP_MODULE_CTL_SPARE1;
	uint8 dummy2116[220];
	uint32 SWITCH_CORE_EGPHY_CTRL;
	uint8 dummy2117[12];
	uint32 SWITCH_CORE_EGPHY_PWRMGNT;
	uint8 dummy2118[12];
	uint32 SWITCH_CORE_EGPHY_PWR_DOWN;
	uint8 dummy2119[44];
	uint32 SWITCH_CORE_EGPHY_STRAP;
	uint8 dummy2120[12];
	uint32 SWITCH_CORE_EGPHY_STS;
	uint8 dummy2121[12];
	uint32 SWITCH_CORE_EGPHY_INT_STS;
	uint8 dummy2122[12];
	uint32 SWITCH_CORE_EGPHY_MODE_STS;
	uint8 dummy2123[12];
	uint32 SWITCH_CORE_EGPHY_LPI_STS;
	uint8 dummy2124[12];
	uint32 SWITCH_CORE_EGPHY_ENG_DET_STS;
	uint8 dummy2125[12];
	uint32 SWITCH_CORE_EGPHY_ENG_DET_STS_CHG;
	uint8 dummy2126[12];
	uint32 SWITCH_CORE_EGPHY_RESET_STATUS;
	uint8 dummy2127[1596];
	uint32 SWITCH_CORE_EGPHY_CTL_SPARE0;
	uint8 dummy2128[28];
	uint32 SWITCH_CORE_EGPHY_CTL_SPARE1;
	uint8 dummy2129[220];
	uint32 SWITCH_CORE_BRPHY_CTRL;
	uint8 dummy2130[12];
	uint32 SWITCH_CORE_BRPHY_PWRMGNT;
	uint8 dummy2131[12];
	uint32 SWITCH_CORE_BRPHY_PWR_DOWN;
	uint8 dummy2132[28];
	uint32 SWITCH_CORE_BRPHY_PLL_CTRL;
	uint8 dummy2133[28];
	uint32 SWITCH_CORE_BRPHY_STS;
	uint8 dummy2134[12];
	uint32 SWITCH_CORE_BRPHY_INT_STS;
	uint8 dummy2135[44];
	uint32 SWITCH_CORE_BRPHY_ENG_DET_STS;
	uint8 dummy2136[12];
	uint32 SWITCH_CORE_BRPHY_ENG_DET_STS_CHG;
	uint8 dummy2137[12];
	uint32 SWITCH_CORE_BRPHY_RESET_STATUS;
	uint8 dummy2138[1596];
	uint32 SWITCH_CORE_BRPHY_CTL_SPARE0;
	uint8 dummy2139[28];
	uint32 SWITCH_CORE_BRPHY_CTL_SPARE1;
	uint8 dummy2140[220];
	uint32 SWITCH_CORE_STS_OVERRIDE_GMII_P0;
	uint8 dummy2141[12];
	uint32 SWITCH_CORE_STS_OVERRIDE_GMII_P1;
	uint8 dummy2142[12];
	uint32 SWITCH_CORE_STS_OVERRIDE_GMII_P2;
	uint8 dummy2143[12];
	uint32 SWITCH_CORE_STS_OVERRIDE_GMII_P3;
	uint8 dummy2144[12];
	uint32 SWITCH_CORE_STS_OVERRIDE_GMII_P4;
	uint8 dummy2145[12];
	uint32 SWITCH_CORE_STS_OVERRIDE_GMII_P5;
	uint8 dummy2146[12];
	uint32 SWITCH_CORE_STS_OVERRIDE_P6;
	uint8 dummy2147[12];
	uint32 SWITCH_CORE_STS_OVERRIDE_GMII_P7;
	uint8 dummy2148[12];
	uint32 imp_port_state;
#define ETHSW_IPS_USE_MII_HW_STS                  0x00
#define ETHSW_IPS_USE_REG_CONTENTS                0x80
#define ETHSW_IPS_GMII_SPEED_UP_NORMAL            0x00
#define ETHSW_IPS_GMII_SPEED_UP_2G                0x40
#define ETHSW_IPS_TXFLOW_NOT_PAUSE_CAPABLE        0x00
#define ETHSW_IPS_TXFLOW_PAUSE_CAPABLE            0x20
#define ETHSW_IPS_RXFLOW_NOT_PAUSE_CAPABLE        0x00
#define ETHSW_IPS_RXFLOW_PAUSE_CAPABLE            0x10
#define ETHSW_IPS_SW_PORT_SPEED_1000M_2000M       0x08
#define ETHSW_IPS_DUPLEX_MODE                     0x02
#define ETHSW_IPS_LINK_FAIL                       0x00
#define ETHSW_IPS_LINK_PASS                       0x01
	uint8 dummy2149[60];
	uint32 SWITCH_CORE_PAUSE_CAP;
	uint8 dummy2150[124];
	uint32 SWITCH_CORE_PORT4_RGMII_CTL_GP;
	uint8 dummy2151[12];
	uint32 SWITCH_CORE_PORT5_RGMII_CTL_GP;
	uint8 dummy2152[12];
	uint32 SWITCH_CORE_PORT6_RGMII_CTL_GP;
	uint8 dummy2153[28];
	uint32 SWITCH_CORE_RGMII_CTL_GP_IMP;
	uint8 dummy2154[188];
	uint32 SWITCH_CORE_P4_RGMII_TIME_DLY_GP;
	uint8 dummy2155[12];
	uint32 SWITCH_CORE_P5_RGMII_TIME_DLY_GP;
	uint8 dummy2156[12];
	uint32 SWITCH_CORE_P6_RGMII_TIME_DLY_GP;
	uint8 dummy2157[28];
	uint32 SWITCH_CORE_RGMII_TIME_DLY_GP_IMP;
	uint8 dummy2158[124];
	uint32 SWITCH_CORE_RM_PINS_DEBUG;
	uint8 dummy2159[12];
	uint32 SWITCH_CORE_MII_IDDQ_CTRL;
	uint8 dummy2160[12];
	uint32 SWITCH_CORE_MII_LOW_POWER_CTRL;
	uint8 dummy2161[12];
	uint32 SWITCH_CORE_LED_OPTIONS;
	uint8 dummy2162[972];
	uint32 SWITCH_CORE_PORT_INFO_SPARE0;
	uint8 dummy2163[28];
	uint32 SWITCH_CORE_PORT_INFO_SPARE1;
	uint8 dummy2164[220];
	uint32 SWITCH_CORE_IO_SR_CTL;
	uint8 dummy2165[28];
	uint32 SWITCH_CORE_IO_DS_SEL0;
	uint8 dummy2166[60];
	uint32 SWITCH_CORE_IO_DS_SEL2;
	uint8 dummy2167[28];
	uint32 SWITCH_CORE_GMII_IO_SR_CTL;
	uint8 dummy2168[28];
	uint32 SWITCH_CORE_GMII_IO_DS_SEL0;
	uint8 dummy2169[28];
	uint32 SWITCH_CORE_GMII_IO_DS_SEL1;
	uint8 dummy2170[28];
	uint32 SWITCH_CORE_GMII_VOL_SEL;
	uint8 dummy2171[12];
	uint32 SWITCH_CORE_PINS_DEBUG_IMP;
	uint8 dummy2172[28];
	uint32 SWITCH_CORE_BONDING_PAD_STATUS;
	uint8 dummy2173[12];
	uint32 SWITCH_CORE_STRAP_PIN_STATUS;
	uint8 dummy2174[28];
	uint32 SWITCH_CORE_DIRECT_INPUT_CTRL_VALUE;
	uint8 dummy2175[60];
	uint32 SWITCH_CORE_EMB_CPU_STATUS;
	uint8 dummy2176[1404];
	uint32 SWITCH_CORE_CHIP_CTL_SPARE0;
	uint8 dummy2177[28];
	uint32 SWITCH_CORE_CHIP_CTL_SPARE1;

} EthernetSwitchCore;

#define PBMAP_MIPS 0x100

typedef struct {
    uint32 led_ctrl;
#define SPDLNK_LED1_ACT_POL_SEL_SHIFT 7
#define SPDLNK_LED1_ACT_POL_SEL_MASK (0x1 << SPDLNK_LED1_ACT_POL_SEL_SHIFT)
#define ACT_LED_ACT_SEL_SHIFT 5
#define ACT_LED_ACT_SEL_MASK (0x1 << ACT_LED_ACT_SEL_SHIFT)
#define SPDLNK_LED0_ACT_SEL_SHIFT 2
#define SPDLNK_LED0_ACT_SEL_MASK (0x1 << SPDLNK_LED0_ACT_SEL_SHIFT)
#define RESERVED0_SHIFT 16
#define RESERVED0_MASK (0xffff << RESERVED0_SHIFT)
#define SPDLNK_LED0_ACT_POL_SEL_SHIFT 6
#define SPDLNK_LED0_ACT_POL_SEL_MASK (0x1 << SPDLNK_LED0_ACT_POL_SEL_SHIFT)
#define AGGREGATE_LED_CNTRL_RESERVED0_SHIFT 19
#define AGGREGATE_LED_CNTRL_RESERVED0_MASK (0x1fff << AGGREGATE_LED_CNTRL_RESERVED0_SHIFT)
#define AGGREGATE_LED_CNTRL_ACT_POL_SEL_SHIFT 17
#define AGGREGATE_LED_CNTRL_ACT_POL_SEL_MASK (0x1 << AGGREGATE_LED_CNTRL_ACT_POL_SEL_SHIFT)
#define SPDLNK_LED2_ACT_SEL_SHIFT 4
#define SPDLNK_LED2_ACT_SEL_MASK (0x1 << SPDLNK_LED2_ACT_SEL_SHIFT)
#define LED_SPD_OVRD_SHIFT 10
#define LED_SPD_OVRD_MASK (0x7 << LED_SPD_OVRD_SHIFT)
#define SPDLNK_LED2_ACT_POL_SEL_SHIFT 8
#define SPDLNK_LED2_ACT_POL_SEL_MASK (0x1 << SPDLNK_LED2_ACT_POL_SEL_SHIFT)
#define SPD_OVRD_EN_SHIFT 14
#define SPD_OVRD_EN_MASK (0x1 << SPD_OVRD_EN_SHIFT)
#define TX_ACT_EN_SHIFT 1
#define TX_ACT_EN_MASK (0x1 << TX_ACT_EN_SHIFT)
#define AGGREGATE_LED_CNTRL_ACT_SEL_SHIFT 16
#define AGGREGATE_LED_CNTRL_ACT_SEL_MASK (0x1 << AGGREGATE_LED_CNTRL_ACT_SEL_SHIFT)
#define LNK_STATUS_OVRD_SHIFT 13
#define LNK_STATUS_OVRD_MASK (0x1 << LNK_STATUS_OVRD_SHIFT)
#define RX_ACT_EN_SHIFT 0
#define RX_ACT_EN_MASK (0x1 << RX_ACT_EN_SHIFT)
#define AGGREGATE_LED_CNTRL_LNK_POL_SEL_SHIFT 18
#define AGGREGATE_LED_CNTRL_LNK_POL_SEL_MASK (0x1 << AGGREGATE_LED_CNTRL_LNK_POL_SEL_SHIFT)
#define SPDLNK_LED1_ACT_SEL_SHIFT 3
#define SPDLNK_LED1_ACT_SEL_MASK (0x1 << SPDLNK_LED1_ACT_SEL_SHIFT)
#define ACT_LED_POL_SEL_SHIFT 9
#define ACT_LED_POL_SEL_MASK (0x1 << ACT_LED_POL_SEL_SHIFT)
#define LNK_OVRD_EN_SHIFT 15
#define LNK_OVRD_EN_MASK (0x1 << LNK_OVRD_EN_SHIFT)
#define AGGREGATE_LED_CNTRL_PORT_EN_SHIFT 0
#define AGGREGATE_LED_CNTRL_PORT_EN_MASK (0xffff << AGGREGATE_LED_CNTRL_PORT_EN_SHIFT)
    uint32 led_encoding_sel;
    uint32 led_encoding;
#define LINK_AND_SPEED_ENCODING_M10G_ENCODE_SHIFT 15
#define LINK_AND_SPEED_ENCODING_M10G_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_M10G_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_SEL_SEL_10M_ENCODE_SHIFT 3
#define LINK_AND_SPEED_ENCODING_SEL_SEL_10M_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_SEL_SEL_10M_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_SEL_SEL_100M_ENCODE_SHIFT 6
#define LINK_AND_SPEED_ENCODING_SEL_SEL_100M_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_SEL_SEL_100M_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_SEL_SEL_10G_ENCODE_SHIFT 15
#define LINK_AND_SPEED_ENCODING_SEL_SEL_10G_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_SEL_SEL_10G_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_M10_ENCODE_SHIFT 3
#define LINK_AND_SPEED_ENCODING_M10_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_M10_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_M1000_ENCODE_SHIFT 9
#define LINK_AND_SPEED_ENCODING_M1000_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_M1000_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_SEL_SEL_2500M_ENCODE_SHIFT 12
#define LINK_AND_SPEED_ENCODING_SEL_SEL_2500M_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_SEL_SEL_2500M_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_M100_ENCODE_SHIFT 6
#define LINK_AND_SPEED_ENCODING_M100_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_M100_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_SEL_SEL_1000M_ENCODE_SHIFT 9
#define LINK_AND_SPEED_ENCODING_SEL_SEL_1000M_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_SEL_SEL_1000M_ENCODE_SHIFT)
#define LINK_AND_SPEED_ENCODING_M2500_ENCODE_SHIFT 12
#define LINK_AND_SPEED_ENCODING_M2500_ENCODE_MASK (0x7 << LINK_AND_SPEED_ENCODING_M2500_ENCODE_SHIFT)

}LED_CFG;
typedef struct EthPhyTopReg 
{
    uint32 r2pmi_lp_bcast_mode_ctrl;        /* 0x0000 */
    uint32 u2r_br_ctrl;                     /* 0x0004 */
    uint32 phy_top_rev;                     /* 0x0008 */
    uint32 qphy_rev;                        /* 0x000c */
    uint32 qphy_test_ctrl;                  /* 0x0010 */
    uint32 qphy_ctrl;                       /* 0x0014 */
#define ETHSW_QPHY_CTRL_REF_CLK_FREQ_SHIFT   17
#define ETHSW_QPHY_CTRL_REF_CLK_FREQ_MASK    (0x3<<ETHSW_QPHY_CTRL_REF_CLK_FREQ_SHIFT)
#define ETHSW_QPHY_CTRL_REF_CLK_50MHZ        (0x2<<ETHSW_QPHY_CTRL_REF_CLK_FREQ_SHIFT)
#define ETHSW_QPHY_CTRL_PHYAD_BASE_SHIFT     12
#define ETHSW_QPHY_CTRL_PHYAD_BASE_MASK      (0x1f<<ETHSW_QPHY_CTRL_PHYAD_BASE_SHIFT)
#define ETHSW_QPHY_CTRL_RESET_SHIFT          11
#define ETHSW_QPHY_CTRL_RESET_MASK           (0x1<<ETHSW_QPHY_CTRL_RESET_SHIFT )
#define ETHSW_QPHY_CTRL_CK25_DIS_SHIFT       10
#define ETHSW_QPHY_CTRL_CK25_DIS_MASK        (0x1<<ETHSW_QPHY_CTRL_CK25_DIS_SHIFT)
#define ETHSW_QPHY_CTRL_EXT_PWR_DOWN_SHIFT   1
#define ETHSW_QPHY_CTRL_EXT_PWR_DOWN_MASK    (0xf<<ETHSW_QPHY_CTRL_EXT_PWR_DOWN_SHIFT)
#define ETHSW_QPHY_CTRL_IDDQ_BIAS_SHIFT      0
#define ETHSW_QPHY_CTRL_IDDQ_BIAS_MASK       (0x1<<ETHSW_QPHY_CTRL_IDDQ_BIAS_SHIFT)
#define ETHSW_QPHY_CTRL_IDDQ_GLOBAL_PWR_SHIFT      6
#define ETHSW_QPHY_CTRL_IDDQ_GLOBAL_PWR_MASK       (0xf<<ETHSW_QPHY_CTRL_IDDQ_GLOBAL_PWR_SHIFT)

    uint32 qphy_status;                     /* 0x0018 */
    uint32 sphy_rev;                        /* 0x001c */
    uint32 sphy_test_ctrl;                  /* 0x0020 */
    uint32 sphy_ctrl;                       /* 0x0024 */
#define ETHSW_SPHY_CTRL_REF_CLK_FREQ_SHIFT   13
#define ETHSW_SPHY_CTRL_REF_CLK_FREQ_MASK    (0x3<<ETHSW_QPHY_CTRL_REF_CLK_FREQ_SHIFT)
#define ETHSW_SPHY_CTRL_REF_CLK_50MHZ        (0x2<<ETHSW_QPHY_CTRL_REF_CLK_FREQ_SHIFT)
#define ETHSW_SPHY_CTRL_PHYAD_SHIFT          8
#define ETHSW_SPHY_CTRL_PHYAD_MASK           (0x1f<<ETHSW_SPHY_CTRL_PHYAD_SHIFT)
#define ETHSW_SPHY_CTRL_RESET_SHIFT          5
#define ETHSW_SPHY_CTRL_RESET_MASK           (0x1<<ETHSW_SPHY_CTRL_RESET_SHIFT )
#define ETHSW_SPHY_CTRL_CK25_DIS_SHIFT       4
#define ETHSW_SPHY_CTRL_CK25_DIS_MASK        (0x1<<ETHSW_SPHY_CTRL_CK25_DIS_SHIFT)
#define ETHSW_SPHY_CTRL_EXT_PWR_DOWN_SHIFT   1
#define ETHSW_SPHY_CTRL_EXT_PWR_DOWN_MASK    (0x1<<ETHSW_SPHY_CTRL_EXT_PWR_DOWN_SHIFT)
#define ETHSW_SPHY_CTRL_IDDQ_BIAS_SHIFT      0
#define ETHSW_SPHY_CTRL_IDDQ_BIAS_MASK       (0x1<<ETHSW_SPHY_CTRL_IDDQ_BIAS_SHIFT)
#define ETHSW_SPHY_CTRL_IDDQ_GLOBAL_PWR_SHIFT      3
#define ETHSW_SPHY_CTRL_IDDQ_GLOBAL_PWR_MASK       (0x1<<ETHSW_SPHY_CTRL_IDDQ_GLOBAL_PWR_SHIFT)
    uint32 sphy_status;                     /* 0x0028 */
    uint32 reserved2c[53];                  /* 0x002c - 0x00fc */

    uint32 led_pwm_ctrl;                    /* 0x0100 */
    uint32 led_intensity_ctrl;              /* 0x0104 */
    LED_CFG led_ctrl[8];                    /* 0x0108 - 0x0164 */
    uint32 led_blink_rate_ctrl;             /* 0x0168 */
    uint32 reserved16c[2];      
    uint32 aggr_led_pwm_ctrl;               /* 0x0174 */
    uint32 aggr_led_intensity_ctrl;         /* 0x0178 */
    uint32 aggr_led_ctrl;                   /* 0x017c */
#define ETHSW_AGGREGATE_LED_CTRL_PORT_EN_MASK            0xffff
#define ETHSW_AGGREGATE_LED_CTRL_ACT_SEL_MASK            0x10000
#define ETHSW_AGGREGATE_LED_CTRL_ACT_POL_SEL_MASK        0x20000
#define ETHSW_AGGREGATE_LED_CTRL_LNK_POL_SEL_MASK        0x40000
    uint32 aggr_led_blink_rate_ctrl;        /* 0x0180 */
    uint32 rgmii_ctrl;                      /* 0x0184 */
#define ETHSW_RC_MODE_HV                     (1<<17)
#define ETHSW_RC_EXT_RVMII                   (1<<5)
#define ETHSW_RC_MII_MODE_MASK               (7<<2)
#define ETHSW_RC_EXT_GPHY                    (3<<2)
#define ETHSW_RC_EXT_EPHY                    (2<<2)
#define ETHSW_RC_INT_GPHY                    (1<<2)
#define ETHSW_RC_INT_EPHY                    (0<<2)
#define ETHSW_RC_ID_MODE_DIS                 (1<<1)
#define ETHSW_RC_RGMII_EN                    (1<<0)
    uint32 rgmii_ib_status;                 /* 0x0188 */
    uint32 rgmii_rx_clk_delay_ctrl;         /* 0x018c */
#define ETHSW_RXCLK_DLY_MODE_MASK            (3<<4)
#define ETHSW_RXCLK_DLY_MODE_BYPASS          (2<<4)
#define ETHSW_RXCLK_DLY_MODE_1G              (1<<4)
#define ETHSW_RXCLK_DLY_MODE_10_100          (0<<4)
} EthPhyTopReg;

#define ETHPHY_REG ((volatile EthPhyTopReg * const) ETH_PHYS_REG_BASE)

#define PHY_TEST_CTRL                                ((volatile unsigned int*)(&ETHPHY_REG->sphy_test_ctrl))
#define QPHY_TEST_CTRL                               ((volatile unsigned int*)(&ETHPHY_REG->qphy_test_ctrl))
#define SPHY_CNTRL                                   ((volatile unsigned int*)(&ETHPHY_REG->sphy_ctrl))
#define QPHY_CNTRL                                   ((volatile unsigned int*)(&ETHPHY_REG->qphy_ctrl))


typedef struct xport_reg
{
    uint8 dummy[4];
    uint32 xport_reg_xport_revision; //04
    uint8 dummy_1[0x6c];
    LED_CFG xport_led_cfg[2]; // 0x74
    uint8 dummy_2[0x48];
    uint32 xport_reg_led_blinlk_rate_cntrl; //0xd4
    uint32 xport_reg_led_serial_cntrl;//0xd8
    uint32 xport_reg_refresh_period_cntrl; //0xdc
    uint32 xport_reg_aggregate_led_cntrl;//0xe0
    uint32 xport_reg_aggregate_led_blink_rate_cntrl;//0xe4
    uint32 xport_reg_spare_cntrl;//0xe8
    uint8 dummy_3[0x114];
    uint32 xport_reg_xport_cntrl_1;//0x200
    uint32 xport_reg_crossbar_status;//0x204
    uint32 xport_reg_pon_ae_serdes_status;//0x208
}XportReg;
#define XPORT_REG ((volatile XportReg *)XPORT_REG_BASE)

typedef struct xlif_tx_if
{
    uint32 if_enable; 
    uint32 read_credits; 
    uint32 set_credits; 
    uint32 out_ctrl; 
    uint32 urun_port_enable;
    uint32 tx_threshold;
}XlifTxIf;

typedef struct xlif_reg
{
    uint32 xlif_rx_if[3]; //0x00 - 0x0b
    uint32 xlif_rsvd_1[5]; //0x0c - 0x1f
    uint32 xlif_rx_flow[2]; // 0x20 - 0x27
    uint32 xlif_rsvd_2[6]; //0x28 - 0x3f
    XlifTxIf xlif_tx_if_str; //0x40 - 0x57
    uint32 xlif_rsvd_3[2]; //0x58 - 0x5f
    uint32 xlif_tx_flow_ctrl[2]; //0x60 - 0x67
    uint32 xlif_rsvd_4[2]; //0x68 - 0x6f
    uint32 xlif_debug_bus[1]; //0x70 - 0x73
    uint32 xlif_rsvd_5[1]; //0x74 - 0x77
    uint32 xlif_eee[1]; //0x78 - 0x7b
    uint32 xlif_q_off[1]; //0x7c - 0x7f
    uint32 xlif_rsvd_6[96]; //0x080 - 0x67f
}XlifReg;
#define XLIF_REG ((volatile XlifReg *)XLIF_BASE)

#define ETH_SERDES_MISC_BASE(core)  (ETH_SERDES_0_MISC_BASE + core*ETH_SERDES_MISC_CORE_OFFSET)
typedef struct eth_serdes_0_misc_reg
{
    uint32 dir_acc_cntrl;
    uint32 indir_acc_addr;
    uint32 indir_acc_mask;
    uint32 misc_serdes_cntrl;
} eth_serdes_misc_reg;

#define ETH_SERDES_MISC_REG(core)   ((volatile eth_serdes_misc_reg *)ETH_SERDES_MISC_BASE(core))

#define ETH_SERDES_MISC_CORE_OFFSET 0x100
#define ETH_SERDES_MISC_SERDES_CNTRL(core) \
    &(((volatile eth_serdes_misc_reg *)ETH_SERDES_MISC_BASE(core))->misc_serdes_cntrl) 
    #define ETH_REG_SERDES_IDDQ         (1<<0)
    #define ETH_REG_SERDES_REFCLK_RESET (1<<1)
    #define ETH_REG_SERDES_RESET        (1<<2)
    #define ETH_REG_SERDES_REFSEL(x)    ((x)<<3)
    #define ETH_REG_SERDES_PORTAD_S     (6)
    #define ETH_REG_SERDES_PORTAD_M     (0x1f<<6)

#define ETH_SERDES_R2PMI_LP_INDIR_CNTRL(core) \
    &((volatile uint32_t *)(0x100*core + ETH_SERDES_R2PMI_LP_0_INDIR_CNTRL))

typedef struct EthernetSwitchMDIO
{
    uint32 mdio_cmd;                          /* 0x0000 */
#define ETHSW_MDIO_BUSY                       (1 << 29)
#define ETHSW_MDIO_FAIL                       (1 << 28)
#define ETHSW_MDIO_CMD_SHIFT                  26
#define ETHSW_MDIO_CMD_MASK                   (0x3<<ETHSW_MDIO_CMD_SHIFT) 
#define ETHSW_MDIO_CMD_C22_READ               2
#define ETHSW_MDIO_CMD_C22_WRITE              1
#define ETHSW_MDIO_C22_PHY_ADDR_SHIFT         21
#define ETHSW_MDIO_C22_PHY_ADDR_MASK          (0x1f<<ETHSW_MDIO_C22_PHY_ADDR_SHIFT)
#define ETHSW_MDIO_C22_PHY_REG_SHIFT          16
#define ETHSW_MDIO_C22_PHY_REG_MASK           (0x1f<<ETHSW_MDIO_C22_PHY_REG_SHIFT)
#define ETHSW_MDIO_PHY_DATA_SHIFT             0
#define ETHSW_MDIO_PHY_DATA_MASK              (0xffff<<ETHSW_MDIO_PHY_DATA_SHIFT)
    uint32 mdio_cfg;                          /* 0x0004 */
} EthernetSwitchMDIO;

#define ETHSW_MDIO ((volatile EthernetSwitchMDIO * const) SWITCH_MDIO_BASE)


typedef struct Jtag_Otp {
   uint32 ctrl0;           /* 0x00 */
#define JTAG_OTP_CTRL_ACCESS_MODE       (0x2 << 22)
#define JTAG_OTP_CTRL_PROG_EN           (1 << 21)
#define JTAG_OTP_CTRL_START             (1 << 0)
#define JTAG_OTP_CTRL_CMD_OTP_PROG_EN	(0x2 << 1)
#define JTAG_OTP_CTRL_CMD_PROG		(0xa << 1)
#define JTAG_OTP_CTRL_CMD_PROG_LOCK	(0x19 << 1)
   uint32 ctrl1;           /* 0x04 */
#define JTAG_OTP_CTRL_CPU_MODE          (1 << 0)
   uint32 ctrl2;           /* 0x08 */
   uint32 ctrl2_hi;        /* 0x0c */
   uint32 ctrl3;           /* 0x10 */
   uint32 ctrl4;           /* 0x14 */
   uint32 status0;         /* 0x18 */
   uint32 status0_hi;      /* 0x1c */
   uint32 status1;         /* 0x20 */
#define JTAG_OTP_STATUS_1_PROG_OK       (1 << 2)
#define JTAG_OTP_STATUS_1_CMD_DONE      (1 << 1)
   uint32 status2;         /* 0x24 */
   uint32 status3;         /* 0x28 */
   uint32 status4;         /* 0x2c */
   uint32 status5;         /* 0x30 */
   uint32 status6;         /* 0x34 */
   uint32 status7;         /* 0x38 */
   uint32 status8;         /* 0x3c */
   uint32 status9;         /* 0x40 */
   uint32 status10;        /* 0x44 */
   uint32 status11;        /* 0x48 */
   uint32 status12;        /* 0x4c */
   uint32 status13;        /* 0x50 */
   uint32 cpu_lock;        /* 0x54 */
#define OTP_CPU_LOCK_MASK	0x01   
#define OTP_CPU_LOCK_SHIFT	0x0   
} Jtag_Otp;

#define JTAG_OTP ((volatile Jtag_Otp * const) JTAG_OTP_BASE)

#define BTRM_OTP_READ_TIMEOUT_CNT               0x10000

/* row 8 */
#define OTP_CPU_CORE_CFG_ROW                    8
#define OTP_CPU_CORE_CFG_SHIFT                  28
#define OTP_CPU_CORE_CFG_MASK                   (0x3 << OTP_CPU_CORE_CFG_SHIFT)

/* row 9 */
#define OTP_CPU_CLOCK_FREQ_ROW                  9
#define OTP_CPU_CLOCK_FREQ_SHIFT                0
#define OTP_CPU_CLOCK_FREQ_MASK                 (0x7 << OTP_CPU_CLOCK_FREQ_SHIFT)

/* row 13 */
#define OTP_BRCM_BTRM_PRODUCTION_MODE_ROW       13
#define OTP_BRCM_BTRM_PRODUCTION_MODE_SHIFT    	0
#define OTP_BRCM_BTRM_PRODUCTION_MODE_MASK      (1 << OTP_BRCM_BTRM_PRODUCTION_MODE_SHIFT)
/* row 13 */
#define OTP_BRCM_BTRM_BOOT_ENABLE_ROW           13
#define OTP_BRCM_BTRM_BOOT_ENABLE_SHIFT        	2
#define OTP_BRCM_BTRM_BOOT_ENABLE_MASK          (1 << OTP_BRCM_BTRM_BOOT_ENABLE_SHIFT)

/* row 14 */
#define OTP_CUST_BTRM_BOOT_ENABLE_ROW           14
#define OTP_CUST_BTRM_BOOT_ENABLE_SHIFT        	28
#define OTP_CUST_BTRM_BOOT_ENABLE_MASK          (1 << OTP_CUST_BTRM_BOOT_ENABLE_SHIFT)

/* row 14 */
#define OTP_CUST_BTRM_UART_DISABLE_ROW         	14
#define OTP_CUST_BTRM_UART_DISABLE_SHIFT       	0
#define OTP_CUST_BTRM_UART_DISABLE_MASK         (1 << OTP_CUST_BTRM_UART_DISABLE_SHIFT)

/* row 14 */
#define OTP_CUST_BTRM_MSG_DISABLE_ROW         	14
#define OTP_CUST_BTRM_MSG_DISABLE_SHIFT       	27
#define OTP_CUST_BTRM_MSG_DISABLE_MASK          (1 << OTP_CUST_BTRM_MSG_DISABLE_SHIFT)
/* row 14 */
#define OTP_PCM_DISABLE_ROW                     14
#define OTP_PCM_DISABLE_SHIFT                   13
#define OTP_PCM_DISABLE_MASK                    (0x1 << OTP_PCM_DISABLE_SHIFT)

/* row 29 */
#define OTP_CUST_MFG_MRKTID_ROW                 29
#define OTP_CUST_MFG_MRKTID_SHIFT               0
#define OTP_CUST_MFG_MRKTID_MASK                (0xffff << OTP_CUST_MFG_MRKTID_SHIFT)


/* row 26 */
#define OTP_BOOT_SW_ENET_BOOT_DIS_ROW           26
#define OTP_BOOT_SW_ENET_BOOT_DIS_SHIFT         0
#define OTP_BOOT_SW_ENET_BOOT_DIS_MASK          (7 << OTP_BOOT_SW_ENET_BOOT_DIS_SHIFT)


#define OTP_BOOT_SW_ENET_BOOT_FALLBACK_SHIFT    3
#define OTP_BOOT_SW_ENET_BOOT_FALLBACK_MASK     (7 << OTP_BOOT_SW_ENET_BOOT_FALLBACK_SHIFT)


#define OTP_BOOT_SW_ENET_RGMII_SHIFT            6
#define OTP_BOOT_SW_ENET_RGMII_MASK             (7 << OTP_BOOT_SW_ENET_RGMII_SHIFT)


/*
** SAR Registers
*/

#define SAR_TXDMA_CTRL_REGS (SAR_BASE + 0x00000000) /* SAR Tx Channel Control Registers */
#define SAR_TXDMA_CTRL_REGS_SZ  0x00000040
#define SAR_TXPBUF_PAD_REGS (SAR_BASE + 0x0000005C) /* SAR TxPktBuf Pad Registers */
#define SAR_TXPBUF_PAD_REGS_SZ  0x00000004
#define SAR_TX_CTL_REGS (SAR_BASE + 0x00000060) /* SAR Tx Control Registers */
#define SAR_TX_CTL_REGS_SZ  0x00000030
#define SAR_RX_CTL_REGS (SAR_BASE + 0x00000090) /* SAR Rx Control Registers */
#define SAR_RX_CTL_REGS_SZ  0x00000040
#define SAR_TX_VPI_VCI_REGS (SAR_BASE + 0x00000100) /* SAR  Tx ATM VPI_VCI Table Reg Registers */
#define SAR_TX_VPI_VCI_REGS_SZ  0x00000040
#define SAR_RX_VCAM_REGS (SAR_BASE + 0x00000140) /* SAR  Rx ATM VPI_VCI CAM Table Reg Registers */
#define SAR_RX_VCAM_REGS_SZ  0x00000080
#define SAR_SHPR_REGS (SAR_BASE + 0x00000200) /* SAR Atm Shaper Source Shaping Table Registers */
#define SAR_SHPR_REGS_SZ  0x00000090
#define SAR_TXIF_CFG_REGS (SAR_BASE + 0x000002A0) /* SAR TxIf to XRDP BBH Config Registers */
#define SAR_TXIF_CFG_REGS_SZ  0x00000004
#define SAR_RX_PBUF_REGS (SAR_BASE + 0x00000300) /* SAR Rx Packet Buffer Control Registers */
#define SAR_RX_PBUF_REGS_SZ  0x00000060
#define SAR_MIB_REGS (SAR_BASE + 0x00000600) /* SAR  Atm MIB Counters Registers */
#define SAR_MIB_REGS_SZ  0x000000E0
#define SAR_DIAG_REGS (SAR_BASE + 0x00000700) /* SAR  Diagnostic Registers */
#define SAR_DIAG_REGS_SZ  0x00000100
#define SAR_RX_PAF_REGS (SAR_BASE + 0x00000800) /* SAR RxPaf Top Registers */
#define SAR_RX_PAF_REGS_SZ  0x00000100
#define SAR_RX_BOND_REGS (SAR_BASE + 0x00000900) /* SAR RxPaf Bonding Registers */
#define SAR_RX_BOND_REGS_SZ  0x000000D0
#define SAR_TMUEXT_REGS (SAR_BASE + 0x00001000) /* SAR Traffic Management Unit Extended Registers */
#define SAR_TMUEXT_REGS_SZ  0x00000910
#define SAR_TX_DBG_MIB_REGS (SAR_BASE + 0x00002800) /* SAR Tx MIB Counters Registers */
#define SAR_TX_DBG_MIB_REGS_SZ  0x00000090
#define SAR_TXBBH_FIFO (SAR_BASE + 0x00002900) /* SAR Tx BBH FiFo Data Registers */
#define SAR_TXBBH_FIFO_SZ  0x00000800
#define SAR_TXPKT_FIFO (SAR_BASE + 0x00003100) /* SAR Tx Pkt FiFo Data Registers */
#define SAR_TXPKT_FIFO_SZ  0x00000800
#define SAR_TXUTO_FIFO (SAR_BASE + 0x00003900) /* SAR Tx Uto FiFo Data Registers */
#define SAR_TXUTO_FIFO_SZ  0x00000200


/* Specific SAR Rx Control Registers - LED Configuration Register */
#define SAR_RX_CTL_LED_CFG (SAR_BASE + 0x000000BC) /* LED Configuration Register */
#define SARRXCTLLEDCFG ((volatile uint32 * const) (SAR_RX_CTL_LED_CFG))

#define SARLEDCFG_TEST                  0x000000100     /* LED Test bit. */
#define SARLEDCFG_BLINK_30MS            0x000000000     /* LED blink speed: 00 = 30 ms */
#define SARLEDCFG_BLINK_50MS            0x000000020     /* LED blink speed: 01 = 50 ms */
#define SARLEDCFG_BLINK_125MS           0x000000040     /* LED blink speed: 10 = 125 ms */
#define SARLEDCFG_BLINK_250MS           0x000000060     /* LED blink speed: 11 = 250 ms */
#define SARLEDCFG_LNK                   0x000000010     /* Link is established - set by software when ADSL link is established. */
#define SARLEDCFG_INT_LED               0x000000008     /* Set to enable using internal LED logic to drive INET_LED, otherwise use Periph LED logic control. */
#define SARLEDCFG_MODE_LINK             0x000000000     /* INET_LED Mode: 00 = Assert on ADSL Link Only (by setting LINK bit) */
#define SARLEDCFG_MODE_TRAFFIC          0x000000002     /* INET_LED Mode: 01 = Assert on Cell Activity */
#define SARLEDCFG_MODE_MELODYLINK       0x000000004     /* INET_LED Mode: 10 = ADSL Melody/Link Mode - blink slowly during training and solid on link */
#define SARLEDCFG_MODE_TRAFFICLINK      0x000000006     /* INET_LED Mode: 11 = Assert on ADSL Link, blink on ATM TX and RX cells traffic */
#define SARLEDCFG_LED_EN                0x000000001     /* ADSL LED Enable */

/* USB Host contorl regs */
typedef struct usb_ctrl {
    uint32 setup;
#define USBH_IPP                (1<<5)
#define USBH_IOC                (1<<4)
#define USBH_STRAP_IPP_SEL      (1<<25)
#define USB2_OC_DISABLE_PORT0   (1<<28)
#define USB2_OC_DISABLE_PORT1   (1<<29)
#define USB3_OC_DISABLE_PORT0   (1<<30)
#define USB3_OC_DISABLE_PORT1   (1<<31)

    uint32 usb_pm;
#define XHC_SOFT_RESETB         (1<<21)
#define U3XPHY_PWRDWN			(1<<23)
#define USB_PWRDWN              (1<<31)
    uint32 usb_pm_status;
    uint32 ignore1[25];
    uint32 p0_u2phy_cfg1;
#define USB2_PHY_AUTORSMENB0_P0 (1<<16)
    uint32 ignore2[21];
    uint32 p1_u2phy_cfg1;
#define USB2_PHY_AUTORSMENB0_P1	(1<<16)
    uint32 ignore3[22];
} usb_ctrl_t;

typedef struct usb_xhci_gbl {
    uint32 ignore1[11];
    uint32 guctl; 
#define XHCI_REFCLKPER_MASK 	0xffc00000
#define XHCI_REFCLKPER_125ms 	0x0A000000
    uint32 ignore2[325];
} usb_xhci_gbl_t;

typedef struct BIUCFG_Access {
    uint32 permission;        /* 0x0 */
    uint32 sbox;              /* 0x4 */
    uint32 cpu_defeature;     /* 0x8 */
    uint32 dbg_security;      /* 0xc */
    uint32 rsvd1[32];         /* 0x10 - 0x8f */
    uint64 violation[2];      /* 0x90 - 0x9f */
    uint32 ts_access[2];      /* 0xa0 - 0xa7 */
    uint32 rsvd2[22];         /* 0xa8 - 0xff */
}BIUCFG_Access;

typedef struct BIUCFG_Cluster {
    uint32 permission;        /* 0x0 */
    uint32 config;            /* 0x4 */
    uint32 status;            /* 0x8 */
    uint32 control;           /* 0xc */
    uint32 cpucfg;            /* 0x10 */
    uint32 dbgrom;            /* 0x14 */
    uint32 rsvd1[2];          /* 0x18 - 0x1f */
    uint64 rvbar_addr[4];     /* 0x20 - 0x3f */
    uint32 rsvd2[48];         /* 0x40 - 0xff */
}BIUCFG_Cluster;

typedef struct BIUCFG_Bac {
    uint32 bac_permission;    /* 0x00 */
    uint32 bac_periphbase;    /* 0x04 */
    uint32 rsvd[2];           /* 0x08 - 0x0f */
    uint32 bac_event;         /* 0x10 */
    uint32 rsvd_1[3];         /* 0x14 - 0x1f */
    uint32 bac_ccicfg;        /* 0x20 */
    uint32 bac_cciaddr;       /* 0x24 */
    uint32 rsvd_2[4];         /* 0x28 - 0x37 */
    uint32 bac_ccievs2;       /* 0x38 */
    uint32 bac_ccievs3;       /* 0x3c */
    uint32 bac_ccievs4;       /* 0x40 */
    uint32 rsvd_3[3];         /* 0x44 - 0x4f */
    uint32 bac_ccievm0;       /* 0x50 */
    uint32 bac_ccievm1;       /* 0x54 */
    uint32 rsvd_4[2];         /* 0x58 - 0x5f */
    uint32 bac_dapapbcfg;     /* 0x60 */
    uint32 bac_status;        /* 0x64 */
    uint32 rsvd_5[2];         /* 0x68 - 0x6f */
    uint32 cpu_therm_irq_cfg; /* 0x70 */
    uint32 cpu_therm_threshold_cfg; /* 0x74 */
    uint32 rsvd_6;            /* 0x78 */
    uint32 cpu_therm_temp;    /* 0x7c */
    uint32 rsvd_7[32];        /* 0x80 - 0xff */
} BIUCFG_Bac;

typedef struct BIUCFG_Aux {
    uint32 aux_permission;    /* 0x00 */
    uint32 rsvd[3];           /* 0x04 - 0x0f */
    uint32 c0_clk_control;    /* 0x10 */
    uint32 c0_clk_ramp;       /* 0x14 */
    uint32 c0_clk_pattern;    /* 0x18 */
    uint32 rsvd_1;            /* 0x1c */
    uint32 c1_clk_control;    /* 0x20 */
    uint32 c1_clk_ramp;       /* 0x24 */
    uint32 c1_clk_pattern;    /* 0x28 */
    uint32 rsvd_2[53];        /* 0x2c - 0xff */
} BIUCFG_Aux;

typedef struct BIUCFG {
    BIUCFG_Access access;         /* 0x0 - 0xff*/
    BIUCFG_Cluster cluster[2];    /* 0x100 - 0x2ff*/
    BIUCFG_Bac bac;               /* 0x300 - 0x3ff */
    uint32 anonymous[192];        /* 0x400 - 0x6ff */
    BIUCFG_Aux aux;               /* 0x700 - 0x7ff */
    uint32 anonymous_1[2560];     /* 0x800 - 0x2fff */
}BIUCFG;

#define BIUCFG ((volatile BIUCFG * const) BIUCFG_BASE)

/* System Port Block */

#define FIELD_MASK(bits, shift)  ( ( (1ULL<<(bits)) - 1 ) << shift )

#define SYSPORT_RXCHK_CONTROL_SKIP_FCS_M        FIELD_MASK(1,1)
#define SYSPORT_RXCHK_CONTROL_BRCM_TAG_EN_M     FIELD_MASK(1,3)

typedef struct sys_port_topctrl {
	uint32 SYSTEMPORT_TOPCTRL_REV_CNTL;
	uint32 SYSTEMPORT_TOPCTRL_RX_FLUSH_CNTL;
	uint32 SYSTEMPORT_TOPCTRL_TX_FLUSH_CNTL;
	uint32 SYSTEMPORT_TOPCTRL_MISC_CNTL;
}sys_port_topctrl, SYSTEMPORT_TOPCTRL;

typedef struct sys_port_sysbuscfg {
	uint32 SYSTEMPORT_SYSBUSCFG_TIER1_TX_ARB_CFG;
	uint32 SYSTEMPORT_SYSBUSCFG_TIER1_RX_ARB_CFG;
	uint32 SYSTEMPORT_SYSBUSCFG_TIER2_ARB_CFG;
	uint32 SYSTEMPORT_SYSBUSCFG_ARB_CONTROL;
	uint32 SYSTEMPORT_SYSBUSCFG_U2R_BRIDGE_CONTROL;
}sys_port_sysbuscfg, SYSTEMPORT_SYSBUSCFG ;

#define SYSPORT_INTR_REGERR_M                     FIELD_MASK(1,0)
#define SYSPORT_INTR_RBUF_OVERFLOW_M              FIELD_MASK(1,1)
#define SYSPORT_INTR_RBUF_UNDERFLOW_M             FIELD_MASK(1,2)
#define SYSPORT_INTR_MPD_M                        FIELD_MASK(1,3)
#define SYSPORT_INTR_BRCM_TAG_MATCH_M             FIELD_MASK(1,4)
#define SYSPORT_INTR_RDMA_MULTI_BUFFER_DONE_M     FIELD_MASK(1,5)
#define SYSPORT_INTR_OVER_MAX_THRESHOLD_M         FIELD_MASK(1,6)
#define SYSPORT_INTR_BELOW_HYST_THRESHOLD_M       FIELD_MASK(1,7)
#define SYSPORT_INTR_FREE_LIST_EMPTY_M            FIELD_MASK(1,8)
#define SYSPORT_INTR_TX_RING_FULL_M               FIELD_MASK(1,9)
#define SYSPORT_INTR_DESC_ALLOC_ERR_M             FIELD_MASK(1,10)
#define SYSPORT_INTR_UNEXP_PKTSIZEUPDATE_ACK_M    FIELD_MASK(1,11)

#define SYSPORT_INTR_ALL_INTR_MASK                FIELD_MASK(12,0)

typedef struct sys_port_intr {
	uint32 SYSTEMPORT_INTR_CPU_STATUS;
	uint32 SYSTEMPORT_INTR_CPU_SET;
	uint32 SYSTEMPORT_INTR_CPU_CLEAR;
	uint32 SYSTEMPORT_INTR_CPU_MASK_STATUS;
	uint32 SYSTEMPORT_INTR_CPU_MASK_SET;
	uint32 SYSTEMPORT_INTR_CPU_MASK_CLEAR;
	uint32 SYSTEMPORT_INTR_PCI_STATUS;
	uint32 SYSTEMPORT_INTR_PCI_SET;
	uint32 SYSTEMPORT_INTR_PCI_CLEAR;
	uint32 SYSTEMPORT_INTR_PCI_MASK_STATUS;
	uint32 SYSTEMPORT_INTR_PCI_MASK_SET;
	uint32 SYSTEMPORT_INTR_PCI_MASK_CLEAR;
}sys_port_intr, SYSTEMPORT_INTRL2;

typedef struct systemport_rxchk {
    uint32 SYSTEMPORT_RXCHK_CONTROL;//0x80490300
    uint32 SYSTEMPORT_RXCHK_BRCM_TAG[8];//0x80490304
    uint32 SYSTEMPORT_RXCHK_BRCM_TAG_MASK[8];//0x80490324
    uint32 SYSTEMPORT_RXCHK_BRCM_TAG_MATCH_STATUS;//0x80490344
    uint32 SYSTEMPORT_RXCHK_ETHERTYPE;//0x80490348
    uint32 SYSTEMPORT_RXCHK_BAD_CHECKSUM_PACKET_DISCARD_COUNTER;//0x80490360
    uint32 SYSTEMPORT_RXCHK_OTHER_PACKET_DISCARD_COUNTER;//0x80490364
}systemport_rxchk, SYSTEMPORT_RXCHK;

typedef struct systemport_txchk {
    uint32  SYSTEMPORT_TXCHK_TXCHK_PKT_RDY_THRESHOLD;
}systemport_txchk, SYSTEMPORT_TXCHK;

typedef struct sys_port_mib
{
    uint32 Pkts64Octets;            /* SYSTEMPORT_UMAC_GR64   */
    uint32 Pkts65to127Octets;       /* SYSTEMPORT_UMAC_GR127   */
    uint32 Pkts128to255Octets;      /* SYSTEMPORT_UMAC_GR255   */
    uint32 Pkts256to511Octets;      /* SYSTEMPORT_UMAC_GR511   */
    uint32 Pkts512to1023Octets;     /* SYSTEMPORT_UMAC_GR1023   */
    uint32 Pkts1024to1518Octets;    /* SYSTEMPORT_UMAC_GR1518   */
    uint32 Pkts1519to1522;          /* SYSTEMPORT_UMAC_GRMGV   */
    uint32 Pkts1523to2047;          /* SYSTEMPORT_UMAC_GR2047   */
    uint32 Pkts2048to4095;          /* SYSTEMPORT_UMAC_GR4095   */
    uint32 Pkts4096to8191;          /* SYSTEMPORT_UMAC_GR9216   */
    uint32 RxPkts;                  /* SYSTEMPORT_UMAC_GRPKT   */
    uint32 RxOctetsLo;              /* SYSTEMPORT_UMAC_GRBYT   */
    uint32 RxMulticastPkts;         /* SYSTEMPORT_UMAC_GRMCA   */
    uint32 RxBroadcastPkts;         /* SYSTEMPORT_UMAC_GRBCA   */
    uint32 RxFCSErrs;               /* SYSTEMPORT_UMAC_GRFCS   */
    uint32 RxCtrlFrame;             /* SYSTEMPORT_UMAC_GRXCF   */
    uint32 RxPausePkts;             /* SYSTEMPORT_UMAC_GRXPF   */
    uint32 RxUnknown;               /* SYSTEMPORT_UMAC_GRXUO   */
    uint32 RxAlignErrs;             /* SYSTEMPORT_UMAC_GRALN   */
    uint32 RxExcessSizeDisc;        /* SYSTEMPORT_UMAC_GRFLR   */
    uint32 RxSymbolError;           /* SYSTEMPORT_UMAC_GRCDE   */
    uint32 RxCarrierSenseErrs;      /* SYSTEMPORT_UMAC_GRFCR   */
    uint32 RxOversizePkts;          /* SYSTEMPORT_UMAC_GROVR   */
    uint32 RxJabbers;               /* SYSTEMPORT_UMAC_GRJBR   */
    uint32 RxMtuErrs;               /* SYSTEMPORT_UMAC_GRMTUE   */
    uint32 RxGoodPkts;              /* SYSTEMPORT_UMAC_GRPOK   */
    uint32 RxUnicastPkts;           /* SYSTEMPORT_UMAC_GRUC   */
    uint32 RxPPPPkts;               /* SYSTEMPORT_UMAC_GRPPP   */
    uint32 RxCRCMatchPkts;          /* SYSTEMPORT_UMAC_GRCRC   */
    uint32 dummy1[3];               /* uint8 dummy6[12]   */
    uint32 TxPkts64Octets;          /* SYSTEMPORT_UMAC_TR64   */
    uint32 TxPkts65to127Octets;     /* SYSTEMPORT_UMAC_TR127   */
    uint32 TxPkts128to255Octets;    /* SYSTEMPORT_UMAC_TR255   */
    uint32 TxPkts256to511Octets;    /* SYSTEMPORT_UMAC_TR511   */
    uint32 TxPkts512to1023Octets;   /* SYSTEMPORT_UMAC_TR1023   */
    uint32 TxPkts1024to1518Octets;  /* SYSTEMPORT_UMAC_TR1518   */
    uint32 TxPkts1519to1522;        /* SYSTEMPORT_UMAC_TRMGV   */
    uint32 TxPkts1523to2047;        /* SYSTEMPORT_UMAC_TR2047   */
    uint32 TxPkts2048to4095;        /* SYSTEMPORT_UMAC_TR4095   */
    uint32 TxPkts4096to8191;        /* SYSTEMPORT_UMAC_TR9216   */
    uint32 TxPkts;                  /* SYSTEMPORT_UMAC_GTPKT   */
    uint32 TxMulticastPkts;         /* SYSTEMPORT_UMAC_GTMCA   */
    uint32 TxBroadcastPkts;         /* SYSTEMPORT_UMAC_GTBCA   */
    uint32 TxPausePkts;             /* SYSTEMPORT_UMAC_GTXPF   */
    uint32 TxCtrlFrame;             /* SYSTEMPORT_UMAC_GTXCF   */
    uint32 TxFCSErrs;               /* SYSTEMPORT_UMAC_GTFCS   */
    uint32 TxOversizePkts;          /* SYSTEMPORT_UMAC_GTOVR   */
    uint32 TxDeferredTx;            /* SYSTEMPORT_UMAC_GTDRF   */
    uint32 TxExcessiveDef;          /* SYSTEMPORT_UMAC_GTEDF   */
    uint32 TxSingleCol;             /* SYSTEMPORT_UMAC_GTSCL   */
    uint32 TxMultipleCol;           /* SYSTEMPORT_UMAC_GTMCL   */
    uint32 TxLateCol;               /* SYSTEMPORT_UMAC_GTLCL   */
    uint32 TxExcessiveCol;          /* SYSTEMPORT_UMAC_GTXCL   */
    uint32 TxFragments;             /* SYSTEMPORT_UMAC_GTFRG   */
    uint32 TxCol;                   /* SYSTEMPORT_UMAC_GTNCL   */
    uint32 TxJabber;                /* SYSTEMPORT_UMAC_GTJBR   */
    uint32 TxOctetsLo;              /* SYSTEMPORT_UMAC_GTBYT   */
    uint32 TxGoodPkts;              /* SYSTEMPORT_UMAC_GTPOK   */
    uint32 TxUnicastPkts;           /* SYSTEMPORT_UMAC_GTUC   */
    uint32 dummy2[3];               /* uint8 dummy7[12]   */
    uint32 RxRuntPkts;              /* SYSTEMPORT_UMAC_RRPKT   */
    uint32 RxRuntValidFCSPkts;      /* SYSTEMPORT_UMAC_RRUND   */
    uint32 RxRuntInvalidFCSPkts;    /* SYSTEMPORT_UMAC_RRFRG   */
    uint32 RxRuntOctets;            /* SYSTEMPORT_UMAC_RRBYT   */

    /* No mapping for below counters */
    /* uint32 RxUndersizePkts; */
    /* uint32 RxFragments; */

}sys_port_mib;

#define SYSPORT_UMAC_CMD_TX_ENA_M     FIELD_MASK(1,0)
#define SYSPORT_UMAC_CMD_RX_ENA_M     FIELD_MASK(1,1)

#define SYSPORT_UMAC_MPD_CTRL_MPD_EN  FIELD_MASK(1,0)

typedef struct sys_port_umac {
	uint32 SYSTEMPORT_UMAC_UMAC_DUMMY;
	uint32 SYSTEMPORT_UMAC_HD_BKP_CNTL;
	uint32 SYSTEMPORT_UMAC_CMD;
	uint32 SYSTEMPORT_UMAC_MAC0;
	uint32 SYSTEMPORT_UMAC_MAC1;
	uint32 SYSTEMPORT_UMAC_FRM_LEN;
	uint32 SYSTEMPORT_UMAC_PAUSE_QUNAT;
	uint32 dummy1[9];
	uint32 SYSTEMPORT_UMAC_SFD_OFFSET;
	uint32 SYSTEMPORT_UMAC_MODE;
	uint32 SYSTEMPORT_UMAC_FRM_TAG0;
	uint32 SYSTEMPORT_UMAC_FRM_TAG1;
	uint32 dummy2[3];
	uint32 SYSTEMPORT_UMAC_TX_IPG_LEN;
	uint32 dummy3;
	uint32 SYSTEMPORT_UMAC_EEE_CTRL;
	uint32 SYSTEMPORT_UMAC_EEE_LPI_TIMER;
	uint32 SYSTEMPORT_UMAC_EEE_WAKE_TIMER;
	uint32 SYSTEMPORT_UMAC_EEE_REF_COUNT;
	uint32 dummy4;
	uint32 SYSTEMPORT_UMAC_RX_PKT_DROP_STATUS;
	uint32 SYSTEMPORT_UMAC_SYMMETRIC_IDLE_THRESHOLD;
	uint32 dummy5[164];
	uint32 SYSTEMPORT_UMAC_MACSEC_PROG_TX_CRC;
	uint32 SYSTEMPORT_UMAC_MACSEC_CNTRL;
	uint32 SYSTEMPORT_UMAC_TS_STATUS_CNTRL;
	uint32 SYSTEMPORT_UMAC_TX_TS_DATA;
	uint32 dummy6[4];
	uint32 SYSTEMPORT_UMAC_PAUSE_CNTRL;
	uint32 SYSTEMPORT_UMAC_TXFIFO_FLUSH;
	uint32 SYSTEMPORT_UMAC_RXFIFO_STAT;
	uint32 SYSTEMPORT_UMAC_TXFIFO_STAT;
	uint32 SYSTEMPORT_UMAC_PPP_CNTRL;
	uint32 SYSTEMPORT_UMAC_PPP_REFRESH_CNTRL;
	uint32 SYSTEMPORT_UMAC_TX_PAUSE_PREL0;
	uint32 SYSTEMPORT_UMAC_TX_PAUSE_PREL1;
	uint32 SYSTEMPORT_UMAC_TX_PAUSE_PREL2;
	uint32 SYSTEMPORT_UMAC_TX_PAUSE_PREL3;
	uint32 SYSTEMPORT_UMAC_RX_PAUSE_PREL0;
	uint32 SYSTEMPORT_UMAC_RX_PAUSE_PREL1;
	uint32 SYSTEMPORT_UMAC_RX_PAUSE_PREL2;
	uint32 SYSTEMPORT_UMAC_RX_PAUSE_PREL3;
	uint32 dummy7[38];
    sys_port_mib sp_mib;
	uint32 dummy8[28];
	uint32 SYSTEMPORT_UMAC_MIB_CNTRL;
	uint32 dummy9[32];
	uint32 SYSTEMPORT_UMAC_RXERR_MASK;
	uint32 SYSTEMPORT_UMAC_RX_MAX_PKT_SIZE;
	uint32 dummy10[5];
	uint32 SYSTEMPORT_UMAC_MPD_CTRL;
	uint32 SYSTEMPORT_UMAC_PSW_MS;
	uint32 SYSTEMPORT_UMAC_PSW_LS;
	uint32 dummy11[8];
	}sys_port_umac, SYSTEMPORT_UMAC;

#define SYSPORT_RDMA_CTRL_RDMA_EN_M             FIELD_MASK(1,0)
#define SYSPORT_RDMA_CTRL_RING_CFG_M            FIELD_MASK(1,1)
#define SYSPORT_RDMA_CTRL_DISCARD_EN_M          FIELD_MASK(1,2)
#define SYSPORT_RDMA_CTRL_DATA_OFFSET_M         FIELD_MASK(10,4)
#define SYSPORT_RDMA_CTRL_DDR_DESC_RD_EN_M      FIELD_MASK(1,14)
#define SYSPORT_RDMA_CTRL_DDR_DESC_WR_EN_M      FIELD_MASK(1,15)
#define SYSPORT_RDMA_CTRL_DDR_DESC_SWAP_M       FIELD_MASK(2,16)

#define SYSPORT_RDMA_BSRS_BUF_SIZE_LOG2_S       (0)
#define SYSPORT_RDMA_BSRS_BUF_SIZE_LOG2_M       FIELD_MASK(4,0)
#define SYSPORT_RDMA_BSRS_RING_SIZE_S           (16)
#define SYSPORT_RDMA_BSRS_RING_SIZE_M           FIELD_MASK(16,16)

#define SYSPORT_RDMA_STATUS_RDMA_DISABLED_M     FIELD_MASK(1,0)
#define SYSPORT_RDMA_STATUS_DESC_RAM_BUSY_M     FIELD_MASK(1,1)

#define SYSTEMPORT_RDMA_LOCRAM_DESCRING_SIZE_MAX  512

#define SYSPORT_RDMA_PRODUCER_INDEX_PROD_IDX_M  FIELD_MASK(16,0)   
#define SYSPORT_RDMA_CONSUMER_INDEX_CONS_IDX_M  FIELD_MASK(16,0)   
          
#define SYSTEMPORT_RDMA_DDR_DESC_RING_PUSH_TIMER_TIMEOUT_M  FIELD_MASK(16,0)

typedef struct sys_port_rdma {
	uint32 SYSTEMPORT_RDMA_DESCRIPTOR_WORD[1024];
	uint32 SYSTEMPORT_RDMA_CONTROL;
	uint32 SYSTEMPORT_RDMA_STATUS;
	uint32 SYSTEMPORT_RDMA_SYSBUS_BURST;
	uint32 SYSTEMPORT_RDMA_BSRS;
	uint32 SYSTEMPORT_RDMA_WRITE_POINTER_LOW;
	uint32 SYSTEMPORT_RDMA_WRITE_POINTER_HIGH;
	uint32 SYSTEMPORT_RDMA_PRODUCER_INDEX;
	uint32 SYSTEMPORT_RDMA_CONSUMER_INDEX;
	uint32 SYSTEMPORT_RDMA_START_ADDRESS_LOW;
	uint32 SYSTEMPORT_RDMA_START_ADDRESS_HIGH;
	uint32 SYSTEMPORT_RDMA_MULTIPLE_BUFFERS_DONE_INTERRUPT_THRESHOLD_PUSH_TIMER;
	uint32 SYSTEMPORT_RDMA_XON_XOFF_THRESHOLD;
	uint32 SYSTEMPORT_RDMA_READ_POINTER_LOW;
	uint32 SYSTEMPORT_RDMA_READ_POINTER_HIGH;
	uint32 SYSTEMPORT_RDMA_DESC_RAM_ARB_CONTROL;
	uint32 SYSTEMPORT_RDMA_DESC_RAM_RD_ARB_CFG;
	uint32 SYSTEMPORT_RDMA_DESC_RAM_WR_ARB_CFG;
	uint32 SYSTEMPORT_RDMA_DDR_DESC_RING_START_LOW;
	uint32 SYSTEMPORT_RDMA_DDR_DESC_RING_START_HIGH;
	uint32 SYSTEMPORT_RDMA_DDR_DESC_RING_SIZE;
	uint32 SYSTEMPORT_RDMA_DDR_DESC_RING_CTRL;
	uint32 SYSTEMPORT_RDMA_DDR_DESC_RING_PUSH_TIMER;
	uint32 SYSTEMPORT_RDMA_TEST;
	uint32 SYSTEMPORT_RDMA_DEBUG;

}sys_port_rdma, SYSTEMPORT_RDMA;

#define SYSPORT_RBUF_CTRL_RSB_EN_M            FIELD_MASK(1,0)
#define SYSPORT_RBUF_CTRL_4B_ALIGN_M          FIELD_MASK(1,1)
#define SYSPORT_RBUF_CTRL_BTAG_STRIP_M        FIELD_MASK(1,2)
#define SYSPORT_RBUF_CTRL_BAD_PKT_DISCARD_M   FIELD_MASK(1,3)
#define SYSPORT_RBUF_CTRL_CRC_REPLACE_M       FIELD_MASK(1,20)
#define SYSPORT_RBUF_CTRL_RSB_SWAP_M          FIELD_MASK(2,22)
#define SYSPORT_RBUF_CTRL_RSB_SWAP_S          22

#define SYSPORT_RBUF_CTRL_RSB_SWAP_NONE       0
#define SYSPORT_RBUF_CTRL_RSB_SWAP_32         1
#define SYSPORT_RBUF_CTRL_RSB_SWAP_64         2
#define SYSPORT_RBUF_CTRL_RSB_SWAP_32_64      3

typedef struct sys_port_rbuf {
	uint32 SYSTEMPORT_RBUF_RBUF_CONTROL;
	uint32 SYSTEMPORT_RBUF_RBUF_PACKET_READY_THRESHOLD;
	uint32 SYSTEMPORT_RBUF_RBUF_STATUS;
	uint32 SYSTEMPORT_RBUF_RBUF_OVERFLOW_PACKET_DISCARD_COUNT;
	uint32 SYSTEMPORT_RBUF_RBUF_ERROR_PACKET_COUNT;
}sys_port_rbuf, SYSTEMPORT_RBUF;


typedef struct sys_port_tbuf {
	uint32 SYSTEMPORT_TBUF_TBUF_CONTROL;
	uint32 SYSTEMPORT_TBUF_TBUF_STATUS;
	
}sys_port_tbuf, SYSTEMPORT_TBUF;

#define SYSTEMPORT_TDMA_DESC_RING_MAX        16
#define SYSTEMPORT_TDMA_LOCRAM_DESCRING_MAX  1024

#define SYSTEMPORT_TDMA_TIMEOUT_TICK_NSEC    4096 // For 250MHz clock: 1024 * 4ns

typedef struct systemport_tdma_descriptor_write_port
{
		uint32 SYSTEMPORT_TDMA_DESCRIPTOR_XX_WRITE_PORT_LO;
		uint32 SYSTEMPORT_TDMA_DESCRIPTOR_XX_WRITE_PORT_HI;
}systemport_tdma_descriptor_write_port;

typedef struct systemport_tdma_descriptor_read_port
{
		uint32 SYSTEMPORT_TDMA_DESCRIPTOR_XX_READ_PORT_LO;
		uint32 SYSTEMPORT_TDMA_DESCRIPTOR_XX_READ_PORT_HI;
}systemport_tdma_descriptor_read_port;

#define SYSPORT_TDMA_DESC_RING_XX_HEAD_TAIL_PTR_RING_EN_M                          FIELD_MASK(1,25)
#define SYSTEMPORT_TDMA_DESC_RING_XX_PRODUCER_CONSUMER_INDEX_CONSUMER_INDEX_S      16
#define SYSTEMPORT_TDMA_DESC_RING_XX_PRODUCER_CONSUMER_INDEX_CONSUMER_INDEX_M      FIELD_MASK(16,16)
#define SYSTEMPORT_TDMA_DESC_RING_XX_PRODUCER_CONSUMER_INDEX_PRODUCER_INDEX_M      FIELD_MASK(16,0)

#define SYSTEMPORT_TDMA_DESC_RING_XX_INTR_CONTROL_TIMEOUT_S                        16
#define SYSTEMPORT_TDMA_DESC_RING_XX_INTR_CONTROL_TIMEOUT_M                        FIELD_MASK(16,16)
#define SYSTEMPORT_TDMA_DESC_RING_XX_INTR_CONTROL_RING_EMPTY_INTR_EN_M             FIELD_MASK(1,15)
#define SYSTEMPORT_TDMA_DESC_RING_XX_INTR_CONTROL_INTR_THRESHOLD_S                 0
#define SYSTEMPORT_TDMA_DESC_RING_XX_INTR_CONTROL_INTR_THRESHOLD_M                 FIELD_MASK(15,0)

typedef struct SYSTEMPORT_TDMA_DESC
{
	uint32 SYSTEMPORT_TDMA_DESC_RING_XX_HEAD_TAIL_PTR;
	uint32 SYSTEMPORT_TDMA_DESC_RING_XX_COUNT;
	uint32 SYSTEMPORT_TDMA_DESC_RING_XX_MAX_HYST_THRESHOLD;
	uint32 SYSTEMPORT_TDMA_DESC_RING_XX_INTR_CONTROL;
	uint32 SYSTEMPORT_TDMA_DESC_RING_XX_PRODUCER_CONSUMER_INDEX;
	uint32 SYSTEMPORT_TDMA_DESC_RING_XX_MAPPING;
	uint32 SYSTEMPORT_TDMA_DESC_RING_XX_PCP_DEI_VID;
	uint32 SYSTEMPORT_TDMA_DDR_DESC_RING_XX_START_LOW;
	uint32 SYSTEMPORT_TDMA_DDR_DESC_RING_XX_START_HIGH;
	uint32 SYSTEMPORT_TDMA_DDR_DESC_RING_XX_SIZE;
	uint32 SYSTEMPORT_TDMA_DDR_DESC_RING_XX_CTRL;
	uint32 SYSTEMPORT_TDMA_DDR_DESC_RING_XX_PUSH_TIMER;
	
}systemport_tdma_desc;

#define SYSPORT_TDMA_CONTROL_TDMA_EN_M            FIELD_MASK(1,0)
#define SYSPORT_TDMA_CONTROL_TSB_EN_M             FIELD_MASK(1,1)
#define SYSPORT_TDMA_CONTROL_TSB_SWAP_M           FIELD_MASK(2,2)
#define SYSPORT_TDMA_CONTROL_ACB_ALGO_M           FIELD_MASK(1,4)
#define SYSPORT_TDMA_CONTROL_DATA_OFFSET_M        FIELD_MASK(10,5)
#define SYSPORT_TDMA_CONTROL_VLAN_EN_M            FIELD_MASK(1,15)
#define SYSPORT_TDMA_CONTROL_SW_BRCM_TAG_M        FIELD_MASK(1,16)
#define SYSPORT_TDMA_CONTROL_DDR_DESC_RING_EN_M   FIELD_MASK(1,19)
#define SYSPORT_TDMA_CONTROL_DDR_DESC_SWAP_M      FIELD_MASK(2,20)
#define SYSPORT_TDMA_CONTROL_DDR_DESC_SWAP_S      20
#define SYSPORT_TDMA_CONTROL_NO_ACB_M             FIELD_MASK(1,27)

#define SYSPORT_TDMA_CONTROL_DDR_DESC_SWAP_NONE   0
#define SYSPORT_TDMA_CONTROL_DDR_DESC_SWAP_32     1
#define SYSPORT_TDMA_CONTROL_DDR_DESC_SWAP_64     2
#define SYSPORT_TDMA_CONTROL_DDR_DESC_SWAP_32_64  3

#define SYSPORT_TDMA_STATUS_TDMA_DISABLED_M       FIELD_MASK(1,0)
#define SYSPORT_TDMA_STATUS_LL_RAM_INIT_BUSY_M    FIELD_MASK(1,1)

#define SYSTEMPORT_TDMA_DDR_DESC_RING_PUSH_TIMER_TIMEOUT_M  FIELD_MASK(16,0)

typedef struct sys_port_tdma {
	systemport_tdma_descriptor_write_port SYSTEMPORT_TDMA_DESCRIPTOR_WRITE_PORT[16];
	uint8 dummy1[128];
	systemport_tdma_descriptor_read_port  SYSTEMPORT_TDMA_DESCRIPTOR_READ_PORT[16];
	uint8 dummy2[128];
	uint32 SYSTEMPORT_TDMA_DESCRIPTOR_XX_READ_PORT_CMD[16];
	uint8 dummy3[64];
	systemport_tdma_desc SYSTEMPORT_TDMA_DESC[16]; 
	uint8 dummy4[128];
	uint32 SYSTEMPORT_TDMA_CONTROL;
	uint32 SYSTEMPORT_TDMA_STATUS;
	uint32 SYSTEMPORT_TDMA_SYSBUS_BURST;
	uint32 SYSTEMPORT_TDMA_OVER_MAX_THRESHOLD_STATUS;
	uint32 SYSTEMPORT_TDMA_OVER_HYST_THRESHOLD_STATUS;
	uint32 SYSTEMPORT_TDMA_TPID;
	uint32 SYSTEMPORT_TDMA_FREE_LIST_HEAD_TAIL_PTR;
	uint32 SYSTEMPORT_TDMA_FREE_LIST_COUNT;
	uint32 SYSTEMPORT_TDMA_TIER_2_ARBITER_CTRL;
	uint32 SYSTEMPORT_TDMA_TIER_1_ARBITER_0_CTRL;
	uint32 SYSTEMPORT_TDMA_TIER_1_ARBITER_0_QUEUE_ENABLE;
	uint32 SYSTEMPORT_TDMA_TIER_1_ARBITER_1_CTRL;
	uint32 SYSTEMPORT_TDMA_TIER_1_ARBITER_1_QUEUE_ENABLE;
	uint32 SYSTEMPORT_TDMA_TIER_1_ARBITER_2_CTRL;
	uint32 SYSTEMPORT_TDMA_TIER_1_ARBITER_2_QUEUE_ENABLE;
	uint32 SYSTEMPORT_TDMA_TIER_1_ARBITER_3_CTRL;
	uint32 SYSTEMPORT_TDMA_TIER_1_ARBITER_3_QUEUE_ENABLE;
	uint32 SYSTEMPORT_TDMA_TEST;
	uint32 SYSTEMPORT_TDMA_DEBUG;

}sys_port_tdma, SYSTEMPORT_TDMA;

#define SYSPORT_TPC     ((volatile sys_port_topctrl * const) SYSPORT_BASE)
#define SYSPORT_SYSBUSCFG     ((volatile sys_port_sysbuscfg * const) SYSPORT_SYSBUSCFG_BASE)
#define SYSPORT_INTR0   ((volatile sys_port_intr * const) SYSPORT_INTR_0_BASE)
#define SYSPORT_INTR1   ((volatile sys_port_intr * const) SYSPORT_INTR_1_BASE)
#define SYSPORT_UMAC    ((volatile sys_port_umac    * const) SYSPORT_UMAC_BASE)
#define SYSPORT_RDMA    ((volatile sys_port_rdma    * const) SYSPORT_RDMA_BASE)
#define SYSPORT_RBUF    ((volatile sys_port_rbuf    * const) SYSPORT_RBUF_BASE)
#define SYSPORT_TBUF    ((volatile sys_port_tbuf    * const) SYSPORT_TBUF_BASE)
#define SYSPORT_TDMA    ((volatile sys_port_tdma    * const) SYSPORT_TDMA_BASE)
#define SYSPORT_MIB     ((volatile sys_port_mib * const) (SYSPORT_MIB_BASE))


typedef struct sysport {
    union {
        SYSTEMPORT_TOPCTRL SYSTEMPORT_TOPCTRL;
        uint8 SYSTEMPORT_TOPCTRL_buf[64];//0x80490000
    };
    union {
        SYSTEMPORT_SYSBUSCFG SYSTEMPORT_SYSBUSCFG;
        uint8 SYSTEMPORT_SYSBUSCFG_buf[448];//0x80490040
    };
    union {
        SYSTEMPORT_INTRL2 SYSTEMPORT_INTRL2_0;
        uint8 SYSTEMPORT_INTRL2_0_buf[64];//0x8049022f
    };
    union {
        SYSTEMPORT_INTRL2 SYSTEMPORT_INTRL2_1;
        uint8 SYSTEMPORT_INTRL2_1_buf[192];//0x80490240
    };
    union {
        SYSTEMPORT_RXCHK SYSTEMPORT_RXCHK;
        uint8 SYSTEMPORT_RXCHK_buf[128];//0x80490300
    };
    union {
        SYSTEMPORT_TXCHK SYSTEMPORT_TXCHK;
        uint8 SYSTEMPORT_TXCHK_buf[128];//0x80490380
    };
    union {
        SYSTEMPORT_RBUF SYSTEMPORT_RBUF;
        uint8 SYSTEMPORT_RBUF_buf[512];//0x80490400
    };
    union {
        SYSTEMPORT_TBUF SYSTEMPORT_TBUF;
        uint8 SYSTEMPORT_TBUF_buf[512];//0x80490600
    };
    union {
        SYSTEMPORT_UMAC SYSTEMPORT_UMAC;
        uint8 SYSTEMPORT_UMAC_buf[6144];//0x80490800
    };
    union {
        SYSTEMPORT_RDMA SYSTEMPORT_RDMA;
        uint8 SYSTEMPORT_RDMA_buf[8192];//0x80492000
    };
    union {
        SYSTEMPORT_TDMA SYSTEMPORT_TDMA;
        uint8 SYSTEMPORT_TDMA_buf[8192];//0x80494000
    };
} sysport;

#define SYSPORT_0_BASE SYSPORT_BASE
#define SYSPORT(base_num)    ((volatile sysport* const) SYSPORT_##base_num##_BASE)

typedef struct CCI500_SlaveIntf {
#define SNOOP_CTRL_ENABLE_SNOOP            0x1
    uint32 snoop_ctrl;        /* 0x0 */
#define SHARE_OVR_SHAREABLE_OVR_SHIFT      0x0
#define SHARE_OVR_SHAREABLE_OVR_MASK       0x3
#define SHARE_OVR_SHAREABLE_OVR_NONSHR     0x2
#define SHARE_OVR_SHAREABLE_OVR_SHR        0x3
    uint32 share_ovr;         /* 0x4 */
    uint32 rsvd1[62];         /* 0x8 - 0xff */
    uint32 arqos_ovr;         /* 0x100 */
    uint32 awqos_ovr;         /* 0x104 */
    uint32 rsvd2[2];          /* 0x108 - 0x10f */
    uint32 qos_max_ot;        /* 0x110 */
    uint32 rsvd3[955];        /* 0x114 - 0xfff */
}CCI500_SlaveIntf;

typedef struct CCI500_EventCounter {
    uint32 sel;          /* 0x0 */
    uint32 data;         /* 0x4 */
    uint32 ctrl;         /* 0x8 */
    uint32 clr_ovfl;     /* 0xC */
    uint32 rsvd[16380];  /* 0x10 - 0xffff */
}CCI500_EventCounter;

typedef struct CCI500 {
#define CONTROL_OVERRIDE_SNOOP_DISABLE     0x1
#define CONTROL_OVERRIDE_SNOOP_FLT_DISABLE 0x4
    uint32 ctrl_ovr;        /* 0x0 */
    uint32 rsvd1;           /* 0x4 */
#define SECURE_ACCESS_UNSECURE_ENABLE      0x1
    uint32 secr_acc;        /* 0x8 */
    uint32 status;          /* 0xc */
#define STATUS_CHANGE_PENDING              0x1
    uint32 impr_err;        /* 0x10 */
    uint32 qos_threshold;   /* 0x14 */
    uint32 rsvd2[58];       /* 0x18 - 0xff */
    uint32 pmu_ctrl;        /* 0x100 */
#define DBG_CTRL_EN_INTF_MON               0x1
    uint32 debug_ctrl;      /* 0x104 */
    uint32 rsvd3[958];      /* 0x108 - 0xfff */
#define SLAVEINTF_COHERENCY_PORT           0x0
#define SLAVEINTF_CPU_CLUSTER              0x1
    CCI500_SlaveIntf si[7]; /* 0x1000 - 0x7fff */
    uint32 rsvd4[8192];     /* 0x8000 - 0xffff */
    CCI500_EventCounter evt_cntr[8]; /* 0x10000 - 0x8ffff */
}CCI500;

#define CCI500 ((volatile CCI500 * const) CCI500_BASE)

#if (CONFIG_BRCM_CHIP_REV != 0x63146A0)
typedef struct MPM_COMMON
{
	uint32 MPM_COMMON_CONFIG;//0x80023D00
	uint32 MPM_COMMON_CONTROL;//0x80023D04
#define MPM_COMMON_CONTROL_MPM_DA_POOL_EN_SHIFT    4
#define MPM_COMMON_CONTROL_MPM_DA_POOL_EN_MASK     (0xF << MPM_COMMON_CONTROL_MPM_DA_POOL_EN_SHIFT)
#define MPM_COMMON_CONTROL_MPM_MPM_FREE_EN_SHIFT   2
#define MPM_COMMON_CONTROL_MPM_MPM_FREE_EN_MASK    (0x1 << MPM_COMMON_CONTROL_MPM_MPM_FREE_EN_SHIFT)
#define MPM_COMMON_CONTROL_MPM_MPM_ALLOC_EN_SHIFT  1
#define MPM_COMMON_CONTROL_MPM_MPM_ALLOC_EN_MASK   (0x1 << MPM_COMMON_CONTROL_MPM_MPM_ALLOC_EN_SHIFT)
#define MPM_COMMON_CONTROL_MPM_INIT_SHIFT          0
#define MPM_COMMON_CONTROL_MPM_INIT_MASK           (0x1 << MPM_COMMON_CONTROL_MPM_INIT_SHIFT)
	uint32 MPM_COMMON_STATUS;//0x80023D08
#define MPM_COMMON_STATUS_CORE_READY_SHIFT        5
#define MPM_COMMON_STATUS_CORE_READY_MASK         (0x1 << MPM_COMMON_STATUS_CORE_READY_SHIFT)
#define MPM_COMMON_STATUS_CORE_PFIFOS_FULL_SHIFT  4
#define MPM_COMMON_STATUS_CORE_PFIFOS_FULL_MASK   (0x1 << MPM_COMMON_STATUS_CORE_PFIFOS_FULL_SHIFT)
#define MPM_COMMON_STATUS_DA_READY_SHIFT          3
#define MPM_COMMON_STATUS_DA_READY_MASK           (0x1 << MPM_COMMON_STATUS_DA_READY_SHIFT)
#define MPM_COMMON_STATUS_DMA_READY_SHIFT         2
#define MPM_COMMON_STATUS_DMA_READY_MASK          (0x1 << MPM_COMMON_STATUS_DMA_READY_SHIFT)
#define MPM_COMMON_STATUS_DMA_BA_INIT_RINGS_LOADED_SHIFT  1
#define MPM_COMMON_STATUS_DMA_BA_INIT_RINGS_LOADED_MASK   (0x1 << MPM_COMMON_STATUS_DMA_BA_INIT_RINGS_LOADED_SHIFT)
#define MPM_COMMON_STATUS_MPM_DISABLED_SHIFT      0
#define MPM_COMMON_STATUS_MPM_DISABLED_MASK       (0x1 << MPM_COMMON_STATUS_MPM_DISABLED_SHIFT)
	uint32 MPM_COMMON_REV;//0x80023D0C
	uint32 MPM_COMMON_TIMER_TICK_CTRL;//0x80023D10
	uint8 dummy0[12];//0x80023D10
	uint32 MPM_COMMON_POOL_CFG_0;//0x80023D20
	uint32 MPM_COMMON_POOL_CFG_1;//0x80023D24
	uint32 MPM_COMMON_POOL_CFG_2;//0x80023D28
	uint32 MPM_COMMON_POOL_CFG_3;//0x80023D2C
#define MPM_COMMON_POOL_CFG_BUF_SIZE_SHIFT  0
#define MPM_COMMON_POOL_CFG_BUF_SIZE_MASK   (0x1F << MPM_COMMON_POOL_CFG_BUF_SIZE_SHIFT)
	uint32 MPM_COMMON_DA_POOL_XOFF_CFG_0;//0x80023D30
	uint32 MPM_COMMON_DA_POOL_XOFF_CFG_1;//0x80023D34
	uint32 MPM_COMMON_DA_POOL_XOFF_CFG_2;//0x80023D38
	uint32 MPM_COMMON_DA_POOL_XOFF_CFG_3;//0x80023D3C
	uint32 MPM_COMMON_DA_POOL_XON_CFG_0;//0x80023D40
	uint32 MPM_COMMON_DA_POOL_XON_CFG_1;//0x80023D44
	uint32 MPM_COMMON_DA_POOL_XON_CFG_2;//0x80023D48
	uint32 MPM_COMMON_DA_POOL_XON_CFG_3;//0x80023D4C
	uint32 MPM_COMMON_DA_BP_ASSERT_CFG;//0x80023D50
	uint32 MPM_COMMON_DA_BP_DEASSERT_CFG;//0x80023D54
}MPM_COMMON;

#define MPM_COMMON ((volatile MPM_COMMON * const) MPM_COMMON_BASE)

typedef struct MPM_CORE
{
	uint32 MPM_CORE_CONFIG;//0x80023000
#define MPM_CORE_CONFIG_MPM_SIZE_LOG2_KB_SHIFT  4
#define MPM_CORE_CONFIG_MPM_SIZE_LOG2_KB_MASK   (0xFFFF << MPM_CORE_CONFIG_MPM_SIZE_LOG2_KB_SHIFT)
	uint32 MPM_CORE_CONTROL;//0x80023004
	uint32 MPM_CORE_STATUS;//0x80023008
	uint32 MPM_CORE_DIAG;//0x8002300C
#define MPM_CORE_DIAG_INVALID_FREE_CLIENT_ID_SHIFT  20
#define MPM_CORE_DIAG_INVALID_FREE_CLIENT_ID_MASK   (0x7 << MPM_CORE_DIAG_INVALID_FREE_CLIENT_ID_SHIFT)
#define MPM_CORE_DIAG_INVALID_FREE_INDEX_SHIFT  0
#define MPM_CORE_DIAG_INVALID_FREE_INDEX_MASK   (0xFFFFF << MPM_CORE_DIAG_INVALID_FREE_INDEX_SHIFT)
	uint8 dummy0[16];//0x8002300C
	uint32 MPM_CORE_SEARCH_ENGINE_ARBITER_CFG;//0x80023020
	uint32 MPM_CORE_SEARCH_ENGINE_ARBITER_WEIGHT_0;//0x80023024
	uint32 MPM_CORE_SEARCH_ENGINE_ARBITER_WEIGHT_1;//0x80023028
	uint32 MPM_CORE_SEARCH_ENGINE_ARBITER_WEIGHT_2;//0x8002302C
	uint32 MPM_CORE_SEARCH_ENGINE_ARBITER_WEIGHT_3;//0x80023030
	uint32 MPM_CORE_SEARCH_ENGINE_ARBITER_WEIGHT_4;//0x80023034
	uint32 MPM_CORE_PFIFO_SIZE_CFG;//0x80023038
	uint32 MPM_CORE_FFIFO_SIZE_CFG;//0x8002303C
	uint32 MPM_CORE_POOL_AVAIL_CFG_0;//0x80023040
	uint32 MPM_CORE_POOL_AVAIL_CFG_1;//0x80023044
	uint32 MPM_CORE_POOL_AVAIL_CFG_2;//0x80023048
	uint32 MPM_CORE_POOL_AVAIL_CFG_3;//0x8002304C
	uint32 MPM_CORE_SPARE_CTRL;//0x80023050
	uint32 MPM_CORE_PFIFO_FLUSH_CTRL;//0x80023054
	uint8 dummy1[24];//0x80023054
	uint32 MPM_CORE_MEMORY_POOL_POINTERS_0;//0x80023070
	uint32 MPM_CORE_MEMORY_POOL_POINTERS_1;//0x80023074
	uint32 MPM_CORE_MEMORY_POOL_POINTERS_2;//0x80023078
	uint32 MPM_CORE_MEMORY_POOL_POINTERS_3;//0x8002307C
	uint32 MPM_CORE_MEMORY_POOL_LLIST_LEN_0;//0x80023080
	uint32 MPM_CORE_MEMORY_POOL_LLIST_LEN_1;//0x80023084
	uint32 MPM_CORE_MEMORY_POOL_LLIST_LEN_2;//0x80023088
	uint32 MPM_CORE_MEMORY_POOL_LLIST_LEN_3;//0x8002308C
	uint32 MPM_CORE_MEMORY_FREE_ROW_COUNT;//0x80023090
	uint32 MPM_CORE_MEMORY_USED_ROW_COUNT;//0x80023094
	uint32 MPM_CORE_PFIFO_LEVELS;//0x80023098
	uint32 MPM_CORE_FFIFO_LEVEL;//0x8002309C
	uint32 MPM_CORE_BACKDOOR_ACCESS_CTRL;//0x800230A0
	uint32 MPM_CORE_BACKDOOR_ACCESS_RD_DATA;//0x800230A4
	uint32 MPM_CORE_BACKDOOR_ACCESS_WR_DATA;//0x800230A8
	uint8  dummy2[4];//0x800230A8
	uint32 MPM_CORE_MIB_INVALID_FREE_CNT;//0x800230B0
	uint32 MPM_CORE_MIB_INVALID_MCSI_CNT;//0x800230B4
	uint32 MPM_CORE_MIB_INVALID_MCOVF_CNT;//0x800230B8
	uint32 MPM_CORE_MIB_POOL_0_ALLOC_CNT;//0x800230BC
	uint32 MPM_CORE_MIB_POOL_1_ALLOC_CNT;//0x800230C0
	uint32 MPM_CORE_MIB_POOL_2_ALLOC_CNT;//0x800230C4
	uint32 MPM_CORE_MIB_POOL_3_ALLOC_CNT;//0x800230C8
	uint32 MPM_CORE_MIB_POOL_0_FREE_CNT;//0x800230CC
	uint32 MPM_CORE_MIB_POOL_1_FREE_CNT;//0x800230D0
	uint32 MPM_CORE_MIB_POOL_2_FREE_CNT;//0x800230D4
	uint32 MPM_CORE_MIB_POOL_3_FREE_CNT;//0x800230D8
	uint32 MPM_CORE_MIB_TOTAL_FREE_CNT;//0x800230DC
	uint32 MPM_CORE_MIB_TOTAL_MCSI_CNT;//0x800230E0
	uint32 MPM_CORE_MIB_TOTAL_ALLOC_CNT;//0x800230E4
	uint32 MPM_CORE_FREE_EBUF_CNT;//0x800230E8
	uint8  dummy3[12];//0x800230E8
	uint32 MPM_CORE_TP_RD_CTRL;//0x800230F8
	uint32 MPM_CORE_TP_RD_DATA;//0x800230FC
}MPM_CORE;

#define MPM_CORE ((volatile MPM_CORE * const) MPM_CORE_BASE)

typedef struct MPM_DMA_BALLOC
{
	uint32 MPM_DMA_BALLOC_CONFIG;//0x80020000
#define MPM_DMA_BALLOC_CONFIG_PSB_WR_EN_SHIFT  0
#define MPM_DMA_BALLOC_CONFIG_PSB_WR_EN_MASK   (0x1 << MPM_DMA_BALLOC_CONFIG_PSB_WR_EN_SHIFT)
	uint8 dummy0_0[4];//0x80020000
	uint32 MPM_DMA_BALLOC_STATUS;//0x80020008
	uint8 dummy0[16];//0x80020008
	uint32 MPM_DMA_BALLOC_RING_FULL;//0x8002001C
	uint32 MPM_DMA_BALLOC_SYSRAM_PSB_ADDR_LOW;//0x80020020
	uint32 MPM_DMA_BALLOC_SYSRAM_PSB_ADDR_HIGH;//0x80020024
	uint32 MPM_DMA_BALLOC_PSB_CFG;//0x80020028
#define MPM_DMA_BALLOC_PSB_CFG_THRESHOLD_SHIFT  0
#define MPM_DMA_BALLOC_PSB_CFG_TIMEOUT_SHIFT    16
	uint8 dummy1[4];//0x80020028
	uint32 MPM_DMA_BALLOC_TIER1_ARB_RING_EN_i;//0x80020030
	uint8 dummy2[28];//0x80020030
	uint32 MPM_DMA_BALLOC_RING_ARB_WEIGHT_i;//0x80020050
#define MPM_DMA_BALLOC_RING_ARB_WEIGHT_T1_WEIGHT_SHIFT   0
#define MPM_DMA_BALLOC_RING_ARB_WEIGHT_T1_WEIGHT_MASK    (0x1F << MPM_DMA_BALLOC_RING_ARB_WEIGHT_T1_WEIGHT_SHIFT)
#define MPM_DMA_BALLOC_RING_ARB_WEIGHT_XRR_POLICY_SHIFT  8
#define MPM_DMA_BALLOC_RING_ARB_WEIGHT_XRR_POLICY_MASK   (0x1 << MPM_DMA_BALLOC_RING_ARB_WEIGHT_XRR_POLICY_SHIFT)
	uint8 dummy3[380];//0x80020050
	uint32 MPM_DMA_BALLOC_TIER1_ARB_CFG_i;//0x800201D0
#define MPM_DMA_BALLOC_TIER1_ARB_CFG_ARB_MODE_SHIFT   0
#define MPM_DMA_BALLOC_TIER1_ARB_CFG_ARB_MODE_MASK    (0x3 << MPM_DMA_BALLOC_TIER1_ARB_CFG_ARB_MODE_SHIFT)
#define MPM_DMA_BALLOC_TIER1_ARB_CFG_T2_WEIGHT_SHIFT  4
#define MPM_DMA_BALLOC_TIER1_ARB_CFG_T2_WEIGHT_MASK   (0xF << MPM_DMA_BALLOC_TIER1_ARB_CFG_T2_WEIGHT_SHIFT)
	uint8 dummy4[12];//0x800201D0
	uint32 MPM_DMA_BALLOC_TIER2_ARB_CFG;//0x800201E0
	uint32 MPM_DMA_BALLOC_SYSBUS_BURST_CFG;//0x800201E4
	uint8 dummy5[24];//0x800201E4
	uint32 MPM_DMA_BALLOC_RING_PRODUCER_INDEX_i;//0x80020200
	uint8 dummy6[124];//0x80020200
	uint32 MPM_DMA_BALLOC_SYSRAM_RING_CONSUMER_INDEX_i;//0x80020280
	uint8 dummy7[124];//0x80020280
	uint32 MPM_DMA_BALLOC_RING_CPINDEX_COMBO_i;//0x80020300
	uint8 dummy8[124];//0x80020300
	uint32 MPM_DMA_BALLOC_RING_CFG_i;//0x80020380
#define MPM_DMA_BALLOC_RING_CFG_RING_EN_SHIFT  31
#define MPM_DMA_BALLOC_RING_CFG_RING_EN_MASK   (0x1 << MPM_DMA_BALLOC_RING_CFG_RING_EN_SHIFT)
#define MPM_DMA_BALLOC_RING_CFG_PSB_CONTRIB_EN_SHIFT  21
#define MPM_DMA_BALLOC_RING_CFG_PSB_CONTRIB_EN_MASK   (0x1 << MPM_DMA_BALLOC_RING_CFG_PSB_CONTRIB_EN_SHIFT)
#define MPM_DMA_BALLOC_RING_CFG_SWAP_SHIFT  19
#define MPM_DMA_BALLOC_RING_CFG_SWAP_MASK   (0x1 << MPM_DMA_BALLOC_RING_CFG_SWAP_SHIFT)
#define MPM_DMA_BALLOC_RING_CFG_BUF_ADDR_TYPE_SHIFT  16
#define MPM_DMA_BALLOC_RING_CFG_BUF_ADDR_TYPE_MASK   (0x7 << MPM_DMA_BALLOC_RING_CFG_BUF_ADDR_TYPE_SHIFT)
#define MPM_DMA_BALLOC_RING_CFG_SYSRAM_RING_SIZE_LOG2_SHIFT  12
#define MPM_DMA_BALLOC_RING_CFG_SYSRAM_RING_SIZE_LOG2_MASK   (0xF << MPM_DMA_BALLOC_RING_CFG_SYSRAM_RING_SIZE_LOG2_SHIFT)
#define MPM_DMA_BALLOC_RING_CFG_LOCRAM_RING_SIZE_SHIFT  1
#define MPM_DMA_BALLOC_RING_CFG_LOCRAM_RING_SIZE_MASK   (0x7FF << MPM_DMA_BALLOC_RING_CFG_LOCRAM_RING_SIZE_SHIFT)
#define MPM_DMA_BALLOC_RING_CFG_RING_MODE_SHIFT  0
#define MPM_DMA_BALLOC_RING_CFG_RING_MODE_MASK   (0x1 << MPM_DMA_BALLOC_RING_CFG_RING_MODE_SHIFT)
	uint8 dummy9[124];//0x80020380
	uint32 MPM_DMA_BALLOC_SYSRAM_RING_ADDR_LOW_i;//0x80020400
	uint8 dummy10[124];//0x80020400
	uint32 MPM_DMA_BALLOC_SYSRAM_RING_ADDR_HIGH_i;//0x80020480
	uint8 dummy11[124];//0x80020480
	uint32 MPM_DMA_BALLOC_BUFDONE_SIG_CFG_i;//0x80020500
#define MPM_DMA_BALLOC_BUFDONE_SIG_CFG_THRESHOLD_SHIFT  0
#define MPM_DMA_BALLOC_BUFDONE_SIG_CFG_TIMEOUT_SHIFT    16
	uint8 dummy12[124];//0x80020500
	uint32 MPM_DMA_BALLOC_RING_ALLOC_CFG_i;//0x80020580
#define MPM_DMA_BALLOC_RING_ALLOC_CFG_BUF2_POOL_SHIFT  8
#define MPM_DMA_BALLOC_RING_ALLOC_CFG_BUF2_POOL_MASK   (0x3 << MPM_DMA_BALLOC_RING_ALLOC_CFG_BUF2_POOL_SHIFT)
#define MPM_DMA_BALLOC_RING_ALLOC_CFG_BUF1_POOL_SHIFT  4
#define MPM_DMA_BALLOC_RING_ALLOC_CFG_BUF1_POOL_MASK   (0x3 << MPM_DMA_BALLOC_RING_ALLOC_CFG_BUF1_POOL_SHIFT)
#define MPM_DMA_BALLOC_RING_ALLOC_CFG_BUFTYPE_SHIFT    0
#define MPM_DMA_BALLOC_RING_ALLOC_CFG_BUFTYPE_MASK     (0x3 << MPM_DMA_BALLOC_RING_ALLOC_CFG_BUFTYPE_SHIFT)
	uint8 dummy13[124];//0x80020580
	uint32 MPM_DMA_BALLOC_SYSRAM_RING_WR_CFG_i;//0x80020600
#define MPM_DMA_BALLOC_SYSRAM_RING_WR_CFG_TIMEOUT_SHIFT         16
#define MPM_DMA_BALLOC_SYSRAM_RING_WR_CFG_TIMEOUT_MASK          (0xFFFF << MPM_DMA_BALLOC_SYSRAM_RING_WR_CFG_SHIFT)
#define MPM_DMA_BALLOC_SYSRAM_RING_WR_CFG_MAX_BURST_SIZE_SHIFT  8
#define MPM_DMA_BALLOC_SYSRAM_RING_WR_CFG_MIN_BURST_SIZE_SHIFT  0
	uint8 dummy14[124];//0x80020600
	uint32 MPM_DMA_BALLOC_RING_BUF1_INIT_CFG_i;//0x80020680
	uint8 dummy15[124];//0x80020680
	uint32 MPM_DMA_BALLOC_RING_BUF2_INIT_CFG_i;//0x80020700
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER2_EN_SHIFT             23
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER2_EN_MASK              (0x1 << MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER2_EN_SHIFT)
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER2_CMDLIST_START_SHIFT  16
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER2_CMDLIST_START_MASK   (0x3F << MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER2_CMDLIST_START_SHIFT)
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER1_EN_SHIFT             15
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER1_EN_MASK              (0x1 << MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER1_EN_SHIFT)
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER1_CMDLIST_START_SHIFT  8
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER1_CMDLIST_START_MASK   (0x3F << MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER1_CMDLIST_START_SHIFT)
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER0_EN_SHIFT             7
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER0_EN_MASK              (0x1 << MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER0_EN_SHIFT)
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER0_CMDLIST_START_SHIFT  0
#define MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER0_CMDLIST_START_MASK   (0x3F << MPM_DMA_BALLOC_RING_BUF_INIT_CFG_LAYER0_CMDLIST_START_SHIFT)
	uint8 dummy16[124];//0x80020700
	uint32 MPM_DMA_BALLOC_RING_XOFF_CFG_i;//0x80020780
	uint8 dummy17[124];//0x80020780
	uint32 MPM_DMA_BALLOC_RING_XON_CFG_i;//0x80020800
	uint8 dummy18[124];//0x80020800
	uint32 MPM_DMA_BALLOC_RING_INFO_i;//0x80020880
	uint8 dummy19[124];//0x80020880
	uint32 MPM_DMA_BALLOC_REGIFC_ALLOC_i;//0x80020900
	uint8 dummy20[4092];//0x80020900
	uint32 MPM_DMA_BALLOC_BUF_INIT_CMD_i;//0x80021900
}MPM_DMA_BALLOC;

#define MPM_DMA_BALLOC ((volatile MPM_DMA_BALLOC * const) MPM_DMA_BALLOC_BASE)

typedef struct MPM_DMA_BFREE
{
	uint32 MPM_DMA_BFREE_CONFIG;//0x80022000
#define MPM_DMA_BFREE_CONFIG_CSB_WR_EN_SHIFT  0
#define MPM_DMA_BFREE_CONFIG_CSB_WR_EN_MASK   (0x1 << MPM_DMA_BFREE_CONFIG_CSB_WR_EN_SHIFT)
	uint8 dummy0_0[4];//0x80022000
	uint32 MPM_DMA_BFREE_STATUS;//0x80022008
	uint8 dummy0[20];//0x80022008
	uint32 MPM_DMA_BFREE_SYSRAM_CSB_ADDR_LOW;//0x80022020
	uint32 MPM_DMA_BFREE_SYSRAM_CSB_ADDR_HIGH;//0x80022024
	uint32 MPM_DMA_BFREE_CSB_CFG;//0x80022028
#define MPM_DMA_BFREE_CSB_CFG_THRESHOLD_SHIFT  0
#define MPM_DMA_BFREE_CSB_CFG_TIMEOUT_SHIFT    16
	uint8 dummy1_1[4];//0x80022028
	uint32 MPM_DMA_BFREE_TIER1_ARB_RING_EN_i;//0x80022030
	uint8 dummy1[28];//0x80022030
	uint32 MPM_DMA_BFREE_RING_ARB_WEIGHT_i;//0x80022050
	uint8 dummy2[380];//0x80022050
	uint32 MPM_DMA_BFREE_TIER1_ARB_CFG_i;//0x800221D0
#define MPM_DMA_BFREE_TIER1_ARB_CFG_ARB_MODE_SHIFT   0
#define MPM_DMA_BFREE_TIER1_ARB_CFG_ARB_MODE_MASK    (0x3 << MPM_DMA_BFREE_TIER1_ARB_CFG_ARB_MODE_SHIFT)
#define MPM_DMA_BFREE_TIER1_ARB_CFG_T2_WEIGHT_SHIFT  4
#define MPM_DMA_BFREE_TIER1_ARB_CFG_T2_WEIGHT_MASK   (0xF << MPM_DMA_BFREE_TIER1_ARB_CFG_T2_WEIGHT_SHIFT)
	uint8 dummy3[12];//0x800221D0
	uint32 MPM_DMA_BFREE_TIER2_ARB_CFG;//0x800221E0
	uint32 MPM_DMA_BFREE_SYSBUS_BURST_CFG;//0x800221E4
	uint8 dummy4[8];//0x800221E4
	uint32 MPM_DMA_BFREE_CPU_PTR_CFG;//0x800221F0
#define MPM_DMA_BFREE_CPU_PTR_CFG_SIZE_SHIFT  0
#define MPM_DMA_BFREE_CPU_PTR_CFG_SIZE_MASK   (0x1 << MPM_DMA_BFREE_CPU_PTR_CFG_SIZE_SHIFT)
	uint32 MPM_DMA_BFREE_SKB_NEXT_PTR_OFFSET;//0x800221F4
	uint32 MPM_DMA_BFREE_SKB_HEAD_PTR_OFFSET;//0x800221F8
	uint32 MPM_DMA_BFREE_FKB_NEXT_PTR_OFFSET;//0x800221FC
	uint32 MPM_DMA_BFREE_RING_CONSUMER_INDEX_i;//0x80022200
	uint8 dummy5[124];//0x80022200
	uint32 MPM_DMA_BFREE_SYSRAM_RING_PRODUCER_INDEX_i;//0x80022280
	uint8 dummy6[124];//0x80022280
	uint32 MPM_DMA_BFREE_RING_CPINDEX_COMBO_i;//0x80022300
	uint8 dummy7[124];//0x80022300
	uint32 MPM_DMA_BFREE_RING_CFG_i;//0x80022380
#define MPM_DMA_BFREE_RING_CFG_RING_EN_SHIFT  31
#define MPM_DMA_BFREE_RING_CFG_RING_EN_MASK   (0x1 << MPM_DMA_BFREE_RING_CFG_RING_EN_SHIFT)
#define MPM_DMA_BFREE_RING_CFG_CSB_CONTRIB_EN_SHIFT  21
#define MPM_DMA_BFREE_RING_CFG_CSB_CONTRIB_EN_MASK   (0x1 << MPM_DMA_BFREE_RING_CFG_CSB_CONTRIB_EN_SHIFT)
#define MPM_DMA_BFREE_RING_CFG_SWAP_SHIFT  19
#define MPM_DMA_BFREE_RING_CFG_SWAP_MASK   (0x1 << MPM_DMA_BFREE_RING_CFG_SWAP_SHIFT)
#define MPM_DMA_BFREE_RING_CFG_BUF_ADDR_TYPE_SHIFT  16
#define MPM_DMA_BFREE_RING_CFG_BUF_ADDR_TYPE_MASK   (0x3 << MPM_DMA_BFREE_RING_CFG_BUF_ADDR_TYPE_SHIFT)
#define MPM_DMA_BFREE_RING_CFG_SYSRAM_RING_SIZE_LOG2_SHIFT  12
#define MPM_DMA_BFREE_RING_CFG_SYSRAM_RING_SIZE_LOG2_MASK   (0xF << MPM_DMA_BFREE_RING_CFG_SYSRAM_RING_SIZE_LOG2_SHIFT)
#define MPM_DMA_BFREE_RING_CFG_LOCRAM_RING_SIZE_SHIFT  1
#define MPM_DMA_BFREE_RING_CFG_LOCRAM_RING_SIZE_MASK   (0x1FF << MPM_DMA_BFREE_RING_CFG_LOCRAM_RING_SIZE_SHIFT)
#define MPM_DMA_BFREE_RING_CFG_RING_MODE_SHIFT  0
#define MPM_DMA_BFREE_RING_CFG_RING_MODE_MASK   (0x1 << MPM_DMA_BFREE_RING_CFG_RING_MODE_SHIFT)
	uint8 dummy8[124];//0x80022380
	uint32 MPM_DMA_BFREE_SYSRAM_RING_ADDR_LOW_i;//0x80022400
	uint8 dummy9[124];//0x80022400
	uint32 MPM_DMA_BFREE_SYSRAM_RING_ADDR_HIGH_i;//0x80022480
	uint8 dummy10[124];//0x80022480
	uint32 MPM_DMA_BFREE_CMDDONE_SIG_CFG_i;//0x80022500
#define MPM_DMA_BFREE_CMDDONE_SIG_CFG_THRESHOLD_SHIFT  0
#define MPM_DMA_BFREE_CMDDONE_SIG_CFG_TIMEOUT_SHIFT    16
	uint8 dummy11[252];//0x80022500
	uint32 MPM_DMA_BFREE_SYSRAM_RING_RD_CFG_i;//0x80022600
#define MPM_DMA_BFREE_SYSRAM_RING_RD_CFG_TIMEOUT_SHIFT         16
#define MPM_DMA_BFREE_SYSRAM_RING_RD_CFG_MAX_BURST_SIZE_SHIFT  8
#define MPM_DMA_BFREE_SYSRAM_RING_RD_CFG_MIN_BURST_SIZE_SHIFT  0
	uint8 dummy12[636];//0x80022600
	uint32 MPM_DMA_BFREE_LOCRAM_RING_INFO_i;//0x80022880
	uint8 dummy13[124];//0x80022880
	uint32 MPM_DMA_BFREE_REGIFC_CMD_i;//0x80022900
}MPM_DMA_BFREE;

#define MPM_DMA_BFREE ((volatile MPM_DMA_BFREE * const) MPM_DMA_BFREE_BASE)

typedef struct MPM_DMA_COMMON
{
	uint32 MPM_DMA_COMMON_CONFIG;//0x80022D00
#define MPM_DMA_COMMON_CONFIG_MPM_EBUF_SIZE_SHIFT  0
#define MPM_DMA_COMMON_CONFIG_MPM_EBUF_SIZE_MASK   (0x3 << MPM_DMA_COMMON_CONFIG_MPM_EBUF_SIZE_SHIFT)
	uint8 dummy0[12];//0x80022D00
	uint32 MPM_DMA_COMMON_MPM_PHYSICAL_BASE_ADDR;//0x80022D10
	uint32 MPM_DMA_COMMON_MPM_VIRTUAL_BASE_ADDR_LOW;//0x80022D14
	uint32 MPM_DMA_COMMON_MPM_VIRTUAL_BASE_ADDR_HIGH;//0x80022D18
	uint8 dummy1[20];//0x80022D18
	uint32 MPM_DMA_COMMON_DQM_INTERFACE_CFG;//0x80022D30
#define MPM_DMA_COMMON_DQM_INTERFACE_CFG_EN_SHIFT  0
#define MPM_DMA_COMMON_DQM_INTERFACE_CFG_EN_MASK   (0x1 << MPM_DMA_COMMON_DQM_INTERFACE_CFG_EN_SHIFT)
	uint8 dummy2[204];//0x80022D30
	uint32 MPM_DMA_COMMON_DQM_ALLOC_DEALLOC;//0x80022E00
}MPM_DMA_COMMON;
#define MPM_DMA_COMMON ((volatile MPM_DMA_COMMON * const) MPM_DMA_COMMON_BASE)

typedef struct MPM_INTRL2
{
	uint32 MPM_INTRL2_CPU_STATUS;//0x80023E00
	uint32 MPM_INTRL2_CPU_SET;//0x80023E04
	uint32 MPM_INTRL2_CPU_CLEAR;//0x80023E08
	uint32 MPM_INTRL2_CPU_MASK_STATUS;//0x80023E0C
	uint32 MPM_INTRL2_CPU_MASK_SET;//0x80023E10
	uint32 MPM_INTRL2_CPU_MASK_CLEAR;//0x80023E14
#define MPM_INTRL2_ALL_INTR_MASK  0xFFFF
#define MPM_INTRL2_MPM_POOL_EMPTY_INTR_SHIFT  12
#define MPM_INTRL2_MPM_POOL_EMPTY_INTR_MASK   (0xF << MPM_INTRL2_MPM_POOL_EMPTY_INTR_SHIFT)
#define MPM_INTRL2_REG_ERR_INTR_MASK  (0x1 << 11)
#define MPM_INTRL2_BUFINIT_ERR_INTR_MASK  (0x1 << 10)
#define MPM_INTRL2_MPM_DED_INTR_SHIFT  8
#define MPM_INTRL2_MPM_DED_INTR_MASK   (0x3 << MPM_INTRL2_MPM_DED_INTR_SHIFT)
#define MPM_INTRL2_MPM_MCOVF_ERR_INTR_MASK  (0x1 << 7)
#define MPM_INTRL2_MPM_MCSI_ERR_INTR_MASK  (0x1 << 6)
#define MPM_INTRL2_MPM_FREE_ERR_INTR_MASK  (0x1 << 5)
#define MPM_INTRL2_BFCMD_SYSRAM_RING_OVERFLOW_INTR_MASK  (0x1 << 4)
#define MPM_INTRL2_BFCMD_LOCRAM_RING_OVERFLOW_INTR_MASK  (0x1 << 3)
#define MPM_INTRL2_BFCMD_ALLOC_ERR_INTR_MASK  (0x1 << 2)
#define MPM_INTRL2_CSB_DONE_INTR_MASK  (0x1 << 1)
#define MPM_INTRL2_PSB_DONE_INTR_MASK  (0x1 << 0)
}MPM_INTRL2;

#define MPM_INTRL2 ((volatile MPM_INTRL2 * const) MPM_INTRL2_BASE)

typedef struct MPM_BB
{
	uint32 MPM_BB_FORCE;            //0x80023F00
	uint32 MPM_BB_FORCED_CTRL;      //0x80023F04
	uint32 MPM_BB_FORCED_ADDR;      //0x80023F08
	uint32 MPM_BB_FORCED_DATA;      //0x80023F0C
	uint32 MPM_BB_DECODE_CFG;       //0x80023F10
	uint32 MPM_BB_DBG_CFG;          //0x80023F14
	uint32 MPM_BB_DBG_RXFIFO_STS;   //0x80023F18
	uint32 MPM_BB_DBG_TXFIFO_STS;   //0x80023F1c
	uint32 MPM_BB_DBG_RXFIFO_DATA1; //0x80023F20
	uint32 MPM_BB_DBG_RXFIFO_DATA2; //0x80023F24
	uint32 MPM_BB_DBG_TXFIFO_DATA1; //0x80023F28
	uint32 MPM_BB_DBG_TXFIFO_DATA2; //0x80023F2c
	uint32 MPM_BB_DBG_TXFIFO_DATA3; //0x80023F30
	uint32 MPM_BB_MISC;             //0x80023F34
#define MPM_BB_MISC_OLD_TASK_NUM_MASK  (0x1 << 0)
}MPM_BB;

#define MPM_BB ((volatile MPM_BB * const) MPM_BB_BASE)

typedef struct MPM_SYSBUS_MASTER_ACCESS
{
	uint32 MPM_DMA_SYSBUS_MASTER_ACCESS_TIER1_BFREE_ARB_CFG;//0x80022F00
	uint32 MPM_DMA_SYSBUS_MASTER_ACCESS_TIER1_BALLOC_ARB_CFG;//0x80022F04
	uint32 MPM_DMA_SYSBUS_MASTER_ACCESS_TIER2_ARB_CFG;//0x80022F08
	uint32 MPM_DMA_SYSBUS_MASTER_ACCESS_ARB_EN;//0x80022F0C
	uint32 MPM_DMA_SYSBUS_MASTER_ACCESS_WR_REPLY_CFG;//0x80022F10
#define MPM_DMA_SYSBUS_MASTER_ACCESS_WR_REPLY_CFG_WRBUF_REPLY_EN_MODE_SHIFT 0
#define MPM_DMA_SYSBUS_MASTER_ACCESS_WR_REPLY_CFG_WRBUF_REPLY_EN_MODE_MASK \
    (0x3 << MPM_DMA_SYSBUS_MASTER_ACCESS_WR_REPLY_CFG_WRBUF_REPLY_EN_MODE_SHIFT)
	uint32 MPM_DMA_SYSBUS_MASTER_ACCESS_DIAGMPM;//0x80022F14
}MPM_SYSBUS_MASTER_ACCESS;

#define MPM_SYSBUS_MASTER_ACCESS ((volatile MPM_SYSBUS_MASTER_ACCESS * const) MPM_DMA_SUBSYS_MASTER_ACCESS_BASE)
#endif

typedef struct Ubus4SysModuleTop {
    uint32 unused0[16];      /* 0x0 */
    uint32 UcbData;          /* 0x40 */
    uint32 UcbHdr;           /* 0x44 */
    uint32 UcbCntl;          /* 0x48 */
    uint32 unused1;          /* 0x4c */
    uint32 ReadUcbHdr;       /* 0x50 */
    uint32 ReadUcbData;      /* 0x54 */
    uint32 ReadUcbStatus;    /* 0x58 */
    uint32 ReacUcbFifoStatus; /* 0x5c */
} Ubus4SysModuleTop;

typedef struct Ubus4ModuleClientRegistration {
    uint32 SlvStatus[16];		/* 0x0	 */
    uint32 MstStatus[ 8];    	/* 0x240 */
    uint32 RegCntl;    			/* 0x260 */
    uint32 SlvStopProgDelay;	/* 0x264 */
} Ubus4ModuleClientRegistration;

#define UBUSSYSTOP ((volatile Ubus4SysModuleTop * const) UBUS_SYS_MODULE_BASE)
#define UBUSSYSTOP_REGISTRATION  \
		((volatile Ubus4ModuleClientRegistration * const) \
		UBUS_SYS_MODULE_REGISTRATION_BASE)

typedef struct Ubus4ClkCtrlCfgRegs {
    uint32 ClockCtrl;
#define UBUS4_CLK_CTRL_EN_SHIFT    (0)
#define UBUS4_CLK_CTRL_EN_MASK     (0x1 << UBUS4_CLK_CTRL_EN_SHIFT)
#define UBUS4_CLK_BYPASS_SHIFT     (2)
#define UBUS4_CLK_BYPASS_MASK      (0x1 << UBUS4_CLK_BYPASS_SHIFT)
#define UBUS4_MIN_CLK_SEL_SHIFT    (4)
#define UBUS4_MIN_CLK_SEL_MASK     (0x7 << UBUS4_MIN_CLK_SEL_SHIFT)
#define UBUS4_MID_CLK_SEL_SHIFT    (8)
#define UBUS4_MID_CLK_SEL_MASK     (0x7 << UBUS4_MID_CLK_SEL_SHIFT)
    uint32 reserved0[3];
    uint32 Min2Mid_threshhold;
    uint32 Mid2Max_threshhold;
    uint32 Mid2Min_threshhold;
    uint32 Max2Mid_threshhold;
    uint32 ClkIntoMin;
    uint32 ClkIntoMid;
    uint32 ClkIntoMax;
    uint32 reserved1;
    uint32 ClkMinTime;
    uint32 ClkMidTime;
    uint32 ClkMaxTime;
} Ubus4ClkCtrlCfgRegs;

#define UBUS4CLK ((volatile Ubus4ClkCtrlCfgRegs * const) UBUS_MAPPED_BASE)

typedef struct WanTopRegs {
	uint32 WAN_TOP_SCRATCH;//0x80144000
	uint32 WAN_TOP_RESET;//0x80144004
#define WAN_TOP_OUT_RESET (1<<0)
	uint32 GPON_GEARBOX_0;//0x80144008
	uint32 GPON_PATTERN_CFG1;//0x8014400c
	uint32 GPON_PATTERN_CFG2;//0x80144010
	uint32 GPON_GEARBOX_2;//0x80144014
	uint32 EARLY_TXEN;//0x80144018
	uint32 WAN_TOP_RESCAL_CFG;//0x8014401c
// #define RESCAL_RSTB_SHIFT           15
// #define RESCAL_RSTB                 (0x1<<RESCAL_RSTB_SHIFT)
// #define RESCAL_PWRDN_SHIFT          13
// #define RESCAL_PWRDN                (0x1<<RESCAL_PWRDN_SHIFT)
	uint32 WAN_TOP_RESCAL_STATUS_0;//0x80144020
#define RESCAL_DONE_SHIFT           26
#define RESCAL_DONE                 (0x1<<RESCAL_DONE_SHIFT)
	uint32 WAN_TOP_RESCAL_STATUS_1;//0x80144024
	uint32 WAN_TOP_MISC_0;//0x80144028
	uint32 WAN_TOP_MISC_1;//0x8014402c
	uint32 WAN_TOP_MISC_2;//0x80144030
	uint32 WAN_TOP_MISC_3;//0x80144034
	uint32 WAN_SERDES_PLL_CTL;//0x80144038
	uint32 WAN_SERDES_TEMP_CTL;//0x8014403c
	uint32 WAN_SERDES_PRAM_CTL;//0x80144040
	uint32 WAN_SERDES_PRAM_CTL_2;//0x80144044
	uint32 WAN_SERDES_PRAM_CTL_3;//0x80144048
	uint32 WAN_TOP_PMI_LP_0;//0x8014404c
#define PMI_LP0_SERDES_WR  0x03
#define PMI_LP0_SERDES_RD  0x02
#define PMI_LP0_PCS_WR     0x05
#define PMI_LP0_PCS_RD     0x04
	uint32 WAN_TOP_PMI_LP_1;//0x80144050
	uint32 WAN_TOP_PMI_LP_2;//0x80144054
	uint32 WAN_TOP_PMI_LP_3;//0x80144058
	uint32 WAN_TOP_PMI_LP_4;//0x8014405c
#define PMI_LP3or4_PCS_PMI_LP_ERR (1<<17)
#define PMI_LP3or4_PCS_PMI_LP_ACK (1<<16)
#define PMI_LP3or4_PCS_PMI_LP_DATA_MASK (0xffff)

	uint32 WAN_TOP_TOD_CONFIG_0;//0x80144060
	uint32 WAN_TOP_TOD_CONFIG_1;//0x80144064
	uint32 WAN_TOP_TOD_CONFIG_2;//0x80144068
	uint32 WAN_TOP_TOD_CONFIG_3;//0x8014406c
	uint32 WAN_TOP_TOD_CONFIG_4;//0x80144070
	uint32 WAN_TOP_TOD_CONFIG_5;//0x80144074
	uint32 WAN_TOD_TS48_MSB;//0x80144078
	uint32 WAN_TOD_TS48_LSB;//0x8014407c
	uint32 WAN_TOD_TS64_MSB;//0x80144080
	uint32 WAN_TOD_TS64_LSB;//0x80144084
	uint32 WAN_TOP_TOD_STATUS_0;//0x80144088
	uint32 WAN_TOP_TOD_STATUS_1;//0x8014408c
	uint32 WAN_TOP_SERDES_STATUS;//0x80144090
	uint32 WAN_TOP_GPON_GEARBOX_STATUS;//0x80144094
	uint32 WAN_INT_STATUS;//0x80144098
	uint32 WAN_INT_MASK;//0x8014409c
	uint32 WAN_CLK_DEJITTER_SAMPLING_CTL_0;//0x801440a0
	uint32 WAN_CLK_DEJITTER_SAMPLING_CTL_1;//0x801440a4
	uint32 WAN_CLK_SAMPLE_COUNTER;//0x801440a8
	uint32 WAN_SYNCE_PLL_CONFIG;//0x801440ac
	uint32 WAN_TOP_GPON_GEARBOX_PRBS_CONTROL_0;//0x801440b0
	uint32 WAN_TOP_OSR_CONTROL;//0x801440b4
	uint32 WAN_TOP_GPON_GEARBOX_PRBS_CONTROL_1;//0x801440b8
	uint32 WAN_TOP_GPON_GEARBOX_PRBS_STATUS_0;//0x801440bc
	uint32 WAN_TOP_GPON_GEARBOX_PRBS_STATUS_1;//0x801440c0
	uint32 WAN_TOP_AE_GEARBOX_CONTROL_0;//0x801440c4
#define VREG_CFG_VREG_CLK_BYPASS_SHIFT 9
#define VREG_CFG_VREG_CLK_BYPASS_MASK (0x1 << VREG_CFG_VREG_CLK_BYPASS_SHIFT)
#define VREG_CFG_VREG_CLK_SRC_SHIFT 8
#define VREG_CFG_VREG_CLK_SRC_MASK (0x1 << VREG_CFG_VREG_CLK_SRC_SHIFT)
#define VREG_CFG_VREG_DIV_SHIFT 0
#define VREG_CFG_VREG_DIV_MASK (0xff << VREG_CFG_VREG_DIV_SHIFT)
	uint32 WAN_VOLTAGE_REGULATOR_DIVIDER;//0x801440c8
	uint32 WAN_CLOCK_SYNC_CONFIG;//0x801440cc
	uint32 WAN_AEPCS_IEEE_REGID;//0x801440d0
	uint32 WAN_TOP_FORCE_LBE_CONTROL;//0x801440d4
        #define LBE_CFG_FORCE_LBE           (0x1<<0)
        #define LBE_CFG_FORCE_LBE_VALUE     (0x1<<1)
        #define LBE_CFG_FORCE_LBE_OE        (0x1<<2)
        #define LBE_CFG_FORCE_LBE_OE_VALUE  (0x1<<3)
	uint32 NGPON_GEARBOX_RX_CTL_0;//0x801440d8
	uint32 NGPON_GEARBOX_RX_CTL_1;//0x801440dc
	uint32 NGPON_GEARBOX_RX_CTL_2;//0x801440e0
	uint32 NGPON_GEARBOX_RX_CTL_3;//0x801440e4
	uint32 NGPON_GEARBOX_TX_CTL;//0x801440e8
	uint32 NGPON_GEARBOX_STATUS;//0x801440ec
	uint32 EPON_10G_GEARBOX;//0x801440f0
	uint32 WAN_TOP_MISC_4;//0x801440f4
	uint32 WAN_TOP_STATUS;//0x801440f8
}WanTopRegs;

#define WAN_TOP ((volatile WanTopRegs * const) WAN_BASE)

typedef struct
{
    uint32  reserved1[131];     // 0x0 - 0x20B
#define LAN_CONFIG_EEE_EN_SHIFT 0 
#define LAN_CONFIG_EEE_EN_MASK  (0x1 << LAN_CONFIG_EEE_EN_SHIFT)
    uint32  eee;                // 0x20C
    uint32  reserved2[60];      // 0x210 - 0x2FF

} WanBBHTxLanConfig;

typedef struct
{
    uint32 common_config[64];       // 0x0 - 0xFF
    uint32 wan_config[192];         // 0x100 - 0x3FF
    WanBBHTxLanConfig lan_config;   // 0x400 - 0x6FF
    uint32 unified_config[192];     // 0x700 - 0x9FF
    uint32 debug_counters[128];     // 0xA00 - 0xbFF
    uint32 reserved[1280];          // 0xC00 - 0x1FFF

} WanBBHTxRegs;

typedef struct
{
    uint32 reserved[256];           // 0x0 - 0x3FF

} WanBBHRxRegs;

typedef struct
{
    WanBBHTxRegs txBlk[4];   // 0x80170000 
    WanBBHRxRegs rxBlk[4];   // 0x80178000

} WanBBHRegs;
#define WANBBH ((volatile WanBBHRegs * const) WANBBH_BASE)

#endif

#ifdef __cplusplus
}
#endif

#endif

