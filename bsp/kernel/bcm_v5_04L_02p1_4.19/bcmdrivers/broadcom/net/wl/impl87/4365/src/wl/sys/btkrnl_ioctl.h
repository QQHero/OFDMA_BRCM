/* FILE-CSTYLED */

/*****************************************************************************/
/*                                                                           */
/*  Name:          btkrnl_ioctl.h                                            */
/*  $Header: /ramdisk/repositories/20_cvs_clean_up/2011-02-11_sj/src/wl/sys/btkrnl_ioctl.h,v 1.4 2009-09-23 17:48:44 stafford Exp $ */
/*                                                                           */
/*  Description:   BTKRNL Win9x VXD external interface header                */
/*                 This file contains all external interface for BTKRNL VxD  */
/*                                                                           */
/*                                                                           */
/* NOTE: To be included by module of a VXD (btport.vxd) or application which */
/*       wants to send io control requests to BTKRNL.VXD                     */
/*                                                                           */
/*  Date        Modification                                                 */
/*  ------------------------                                                 */
/* 05/14/00     Satyajit Create                                              */
/*                                                                           */
/*  Copyright (c) 2000, Widcomm Inc., All Rights Reserved.                   */
/*  Widcomm Bluetooth Software. Proprietary and confidential.                */
/*****************************************************************************/

#ifndef __btkrnl_ioctl__h_
#define __btkrnl_ioctl__h_

// #include "data_types.h"
// #include "bt_types.h"
// #include "target.h"
// #include "port_api.h"
// #include "btkrnl_pan_ioctl.h"
#include "btkrnl_dev_ioctl.h"
// #include "btkrnl_l2c_ioctl.h"

#ifndef _WIN32_WCE
#include <pshpack1.h>
#endif

#define BTKRNL_DRIVER_VERSION                0x00030006

/////////////////////////////////////////////
//  DO NOT CHANGE ANY OF THESE DEFINITONS!!!!
#define BTKRNL_DRIVER_NEW_EIR_MSG_LEN        0x00030006  // extended inq results changed the message length
#define BTKRNL_DRIVER_LONG_SDP_INT_ATTR      0x00030006  // support added for 64 and 128 bit SDP int attributes, SDK needs to know when it changed
#define BTKRNL_DRIVER_NEW_SEC_ID_SIZE        0x00030005  // changed 32 bit to 64 bit for trustedmask

#define BTKRNL_DRIVER_NEW_PORT_WRITE_STRUCT  0x00030004  // closest driver rew to when BTKRNL_PORT_WRITE_PARS changed
#define BTKRNL_DRIVER_NEW_SDP_ATTR_SIZES     0x00030004  // driver rew when SDP_MAX_ATTR_LEN changed and IOCTL msg structs
                                                         // changed to dynamic

#define BTKRNL_DRIVER_NEW_IOCTL_VERSION      0x00030002  // driver rev when new DD_TYPE_BTKRNL
                                                         // definition ioctls introduced
//  end DO NOT CHANGE ANY OF THESE DEFINITONS!!!!
/////////////////////////////////////////////

#ifndef DD_TYPE_BTKRNL
#define DD_TYPE_BTKRNL      FILE_DEVICE_BUS_EXTENDER
#define DD_TYPE_BTKRNL_OLD  60500                       // old proprietary value, need to support from < 3.1 apps
#endif

#define XLATE_IOCTL_TO_NEW(x) ((x) = (((x) & 0x0000ffff) | (FILE_DEVICE_BUS_EXTENDER << 16)))

#ifndef DEVICE_TYPE_FROM_CTL_CODE
#define DEVICE_TYPE_FROM_CTL_CODE(ctrlCode)     (((DWORD)(ctrlCode & 0xffff0000)) >> 16)
#endif

// Define IOCTL codes
#define IOCTL_BTKRNL_BASE          0x100
#define IOCTL_BTKRNL_BTM_BASE      0x200

// From COM Server or other application
//
#define IOCTL_PORT_OPEN                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x01, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_OPEN_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x01, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_EVENT_MASK            CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x02, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_EVENT_MASK_OLD        CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x02, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_CLOSE                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x03, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_CLOSE_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x03, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_CHECK_CONNECTION      CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x04, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_CHECK_CONNECTION_OLD  CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x04, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_SETSTATE              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x05, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_SETSTATE_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x05, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_GETSTATE              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x06, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_GETSTATE_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x06, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_CONTROL               CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x07, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_CONTROL_OLD           CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x07, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_GET_MODEMSTATUS       CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x08, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_GET_MODEMSTATUS_OLD   CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x08, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_CLEAR_ERROR           CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x09, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_CLEAR_ERROR_OLD       CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x09, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_SEND_ERROR            CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x0A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_SEND_ERROR_OLD        CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x0A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_GET_QUEUESTATUS       CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x0B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_GET_QUEUESTATUS_OLD   CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x0B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_PURGE                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x0C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_PURGE_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x0C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_READ                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x0D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_READ_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x0D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_WRITE                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x0E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_WRITE_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x0E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_TEST                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x0F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_TEST_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x0F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RFCOMM_ALLOC_SCN           CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RFCOMM_ALLOC_SCN_OLD       CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RFCOMM_FREE_SCN            CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RFCOMM_FREE_SCN_OLD        CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_FLOW_CONTROL          CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PORT_FLOW_CONTROL_OLD      CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x12, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define BTKRNL_IOCTL_PORT_GET_STATS           CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_PORT_GET_STATS_OLD       CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_PORT_SET_TRACE_LEVEL     CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_PORT_SET_TRACE_LEVEL_OLD CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_L2CAP_GET_STATS          CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x18, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_L2CAP_GET_STATS_OLD      CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x18, METHOD_BUFFERED, FILE_ANY_ACCESS)

// This request is used to request driver to assign and possible create a COM
// port to be used in cojunction with opened RFCOMM handle.  The
// request is issued from the application.  It is blocked until a
// COM port is assigned.  Returns DEVIOCTL_NOERROR if succeeds.
// Input buffer hold 2 bytes port_handle and a 1 byte port to be assigned.
#define BTKRNL_IOCTL_PORT_ASSIGN_COM     CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x19, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_PORT_ASSIGN_COM_OLD CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x19, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define BTKRNL_IOCTL_PORT_RESULT         CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x1A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_PORT_RESULT_OLD     CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x1A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_SET_ROLE             CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x1F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_SET_ROLE_OLD         CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x1F, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_L2CAP_READ                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x20, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_READ_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x20, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_REG                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_REG_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_DEREG                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_DEREG_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_CON_REQ              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x23, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_CON_REQ_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x23, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_CON_RSP              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x24, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_CON_RSP_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x24, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_CFG_REQ              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x25, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_CFG_REQ_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x25, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_CFG_RSP              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x26, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_CFG_RSP_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x26, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_DISC_REQ             CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x27, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_DISC_REQ_OLD         CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x27, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_DISC_RSP             CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x28, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_DISC_RSP_OLD         CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x28, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_DATA                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x29, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_DATA_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x29, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_PING_REQ             CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x2A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_PING_REQ_OLD         CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x2A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_SET_IDLE_TOUT        CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x2B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_SET_IDLE_TOUT_OLD    CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x2B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_SET_TRACE_LEVEL      CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x2C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_SET_TRACE_LEVEL_OLD  CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x2C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_ALLOC_PSM            CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x2E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_ALLOC_PSM_OLD        CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x2E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_FREE_PSM             CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x2F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_FREE_PSM_OLD         CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x2F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_ASSIGN_PSM           CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x38, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_L2CAP_ASSIGN_PSM_OLD       CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x38, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Values from 0x30 - 0x3F are reserved for btkrnl_dev_ioctl.h
//

// IOCTLs passed from the WIN32 app to BTKRNL
#define BTKRNL_IOCTL_HCI_WRITE                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x40, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_HCI_WRITE_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x40, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_HCI_READ                   CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x41, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_HCI_READ_OLD               CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x41, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define BTKRNL_IOCTL_HCI_CMD_WITH_WAIT          CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_HCI_CMD_WITH_WAIT_OLD      CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x42, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_HCI_POWER_STATE            CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x43, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_HCI_POWER_STATE_OLD        CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x43, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define BTKRNL_IOCTL_HCI_READ_RSSI              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x45, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_HCI_READ_RSSI_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x45, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define BTKRNL_IOCTL_GET_HW_INFO                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x47, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_GET_HW_INFO_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x47, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_GET_VERSION                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x48, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_GET_VERSION_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x48, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_GET_STATS                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x49, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_GET_STATS_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x49, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_CLEAR_STATS                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x4A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_CLEAR_STATS_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x4A, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_TRACE_READ                        CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x75, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TRACE_READ_OLD                    CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x75, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TRACE_DISABLE                     CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x76, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TRACE_DISABLE_OLD                 CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x76, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TRACE_PROTOCOL_FLAGS              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x77, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TRACE_PROTOCOL_FLAGS_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x77, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SDP_ADDITIONAL_PROT_DESCR         CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x7F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADDITIONAL_PROT_DESCR_OLD     CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x7F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_SERVICE_SEARCH                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x80, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_SERVICE_SEARCH_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x80, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_CREATE_RECORD                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x81, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_CREATE_RECORD_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x81, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_DELETE_RECORD                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x82, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_DELETE_RECORD_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x82, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_ATTR                      CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x83, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_ATTR_OLD                  CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x83, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_SEQ                       CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x84, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_SEQ_OLD                   CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x84, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_UUID_SEQ                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x85, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_UUID_SEQ_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x85, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_PROT_DESCR                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x86, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_PROT_DESCR_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x86, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_PROF_DESCR                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x87, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_PROF_DESCR_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x87, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_LANG_BASE                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x88, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_LANG_BASE_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x88, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_SERV_CLASS                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x89, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_ADD_SERV_CLASS_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x89, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_DELETE_ATTR                   CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x8A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_DELETE_ATTR_OLD               CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x8A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_SET_TRACE_LEVEL               CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x8B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_SET_TRACE_LEVEL_OLD           CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x8B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_READ_REC                      CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x8C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_READ_REC_OLD                  CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x8C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_READ_ATTR                     CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x8D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_READ_ATTR_OLD                 CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x8D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_RELEASE_DB                    CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x8E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_RELEASE_DB_OLD                CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x8E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_WAIT                          CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x8F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SDP_WAIT_OLD                      CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x8F, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_VPORT_SET_EVENTCALLBACK           CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x90, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_SET_EVENTCALLBACK_OLD       CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x90, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_EVENT_MASK                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x91, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_EVENT_MASK_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x91, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_CLOSE                       CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x92, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_CLOSE_OLD                   CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x92, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_CHECK_CONNECTION            CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x93, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_CHECK_CONNECTION_OLD        CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x93, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_SETSTATE                    CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x94, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_SETSTATE_OLD                CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x94, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_GETSTATE                    CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x95, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_GETSTATE_OLD                CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x95, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_CONTROL                     CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x96, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_CONTROL_OLD                 CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x96, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_GET_MODEMSTATUS             CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x97, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_GET_MODEMSTATUS_OLD         CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x97, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_CLEAR_ERROR                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x98, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_CLEAR_ERROR_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x98, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_SEND_ERROR                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x99, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_SEND_ERROR_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x99, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_GET_QUEUESTATUS             CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x9A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_GET_QUEUESTATUS_OLD         CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x9A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_PURGE                       CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x9B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_PURGE_OLD                   CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x9B, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_READ                        CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x9C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_READ_OLD                    CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x9C, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_WRITE                       CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x9D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_WRITE_OLD                   CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x9D, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_OPEN                        CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x9E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_OPEN_OLD                    CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x9E, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_FLOW_CONTROL                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0x9F, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_FLOW_CONTROL_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0x9F, METHOD_BUFFERED, FILE_ANY_ACCESS)

// This request is sent from the LAN Access Server to open and close server session
#define IOCTL_VPORT_RFCOMM_CONNECT              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xA0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_RFCOMM_CONNECT_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xA0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_RFCOMM_DISCONNECT           CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xA1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_RFCOMM_DISCONNECT_OLD       CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xA1, METHOD_BUFFERED, FILE_ANY_ACCESS)

// This request is sent from the LAN Access Server to register for device
// status notifications
#define IOCTL_VPORT_REGISTER                    CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xB0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_REGISTER_OLD                CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xB0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_DEREGISTER                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xB1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_DEREGISTER_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xB1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_READ_BD_ADDR                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xB2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VPORT_READ_BD_ADDR_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xB2, METHOD_BUFFERED, FILE_ANY_ACCESS)

// This is request notifies BTKRNL that LAN access configuration has been changed
#define IOCTL_LAP_CONFIG_CHANGED                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xBA, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_LAP_CONFIG_CHANGED_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xBA, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_LAP_START                         CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xBB, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_LAP_START_OLD                     CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xBB, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_LAP_STOP                          CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xBC, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_LAP_STOP_OLD                      CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xBC, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Define IOCTLs that can be sent from the audio stream driver
#define BTAUDIO_IOCTL_REGISTER_STREAM           CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xC0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_REGISTER_STREAM_OLD       CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xC0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_DEREGISTER_STREAM         CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xC1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_DEREGISTER_STREAM_OLD     CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xC1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_SEND_FRAME                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xC2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_SEND_FRAME_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xC2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_START_STREAM              CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xC3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_START_STREAM_OLD          CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xC3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_STOP_STREAM               CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xC4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_STOP_STREAM_OLD           CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xC4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_GET_VOLUME                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xC5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_GET_VOLUME_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xC5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_SET_VOLUME                CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xC6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_SET_VOLUME_OLD            CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xC6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTAUDIO_IOCTL_SEND_SINK_FRAME           CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xC7, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ENABLE                            CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xCA, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENABLE_OLD                        CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xCA, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_IS_ENABLED                        CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xCB, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_IS_ENABLED_OLD                    CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xCB, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_IS_PRESENT                        CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xCC, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_IS_PRESENT_OLD                    CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xCC, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_HID_CONNECT                       CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xD0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HID_CONNECT_OLD                   CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xD0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HID_DISCONNECT                    CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xD1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HID_DISCONNECT_OLD                CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xD1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HID_IS_CONNECTED                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xD2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HID_IS_CONNECTED_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xD2, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define BTKRNL_IOCTL_HID_STATUS                 CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xD3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_HID_STATUS_OLD             CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xD3, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_AV_PLUGIN_DEVICE                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xD8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_AV_READ_DATA                      CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xD9, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_AV_CONNECT                        CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xDA, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_AV_CONNECT_OLD                    CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xDA, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_AV_DISCONNECT                     CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xDB, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_AV_DISCONNECT_OLD                 CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xDB, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_AV_IS_CONNECTED                   CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xDC, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_AV_IS_CONNECTED_OLD               CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xDC, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_AV_STATUS                  CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xDD, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define BTKRNL_IOCTL_AV_STATUS_OLD              CTL_CODE(DD_TYPE_BTKRNL_OLD, IOCTL_BTKRNL_BASE+0xDD, METHOD_BUFFERED, FILE_ANY_ACCESS)

// last pre-3.1 IOCTL code, new codes need not continue the _OLD support

#define IOCTL_AV_SUSPEND                        CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xDE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_AV_RESUME                         CTL_CODE(DD_TYPE_BTKRNL,     IOCTL_BTKRNL_BASE+0xDF, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _BTDEVICE_PARS_1_1
{
    void *DeviceHandle;  // handle BTKRNL is to use when executing
    // device's callback(s).  Opaque for BTKRNL,
    // but will typically point to either device
    // object or device extension.  Must be unique
    // within the BT instances since BTKRNL will
    // use it to identify the source of data

#define BTDEVICE_TYPE_USB     1
#define BTDEVICE_TYPE_PCMCIA  2
#define BTDEVICE_TYPE_SERIAL  3
#define BTDEVICE_TYPE_PCI     4
#define BTDEVICE_TYPE_SDIO    5

    unsigned long DeviceType;  // device type

    unsigned long DeviceNumber;  // number of the device within the
    // range supported by this driver
    // starting from 0

    // Pointer to the device send data callback
    BTDEVICE_SEND_DATA_FUNC BtDeviceSendDataFunc;

    // Pointer to the device send commands callback
    BTDEVICE_SEND_CMD_FUNC  BtDeviceSendCmdFunc;

    // TBD - chances we better pass a UNICODE description string to
    // assure WinNT/2000 compatibility?
#define BTDEVICE_MAX_DESCRIPTION_SIZE 80
    unsigned char DeviceDescription[BTDEVICE_MAX_DESCRIPTION_SIZE];

} BTDEVICE_PARS_1_1, *PBTDEVICE_PARS_1_1;

typedef struct _BTDEVICE_PARS_1_2
{
    void *DeviceHandle;  // handle BTKRNL is to use when executing
                         // device's callback(s).  Opaque for BTKRNL,
                         // but will typically point to either device
                         // object or device extension.  Must be unique
                         // within the BT instances since BTKRNL will
                         // use it to identify the source of data

    unsigned long DeviceType;  // device type

    unsigned long DeviceNumber;  // number of the device within the
                                 // range supported by this driver
                                 // starting from 0

    // Pointer to the device send data callback
    BTDEVICE_SEND_DATA_FUNC BtDeviceSendDataFunc;

    // Pointer to the device send commands callback
    BTDEVICE_SEND_CMD_FUNC  BtDeviceSendCmdFunc;

    // Pointer to the device send data callback
    BTDEVICE_SEND_VOICE_FUNC BtDeviceSendVoiceFunc;

    // Pointer to the device control callback
    BTDEVICE_SEND_CTL_FUNC BtDeviceSendCtlFunc;

    // TBD - chances we better pass a UNICODE description string to
    // assure WinNT/2000 compatibility?
#define BTDEVICE_MAX_DESCRIPTION_SIZE 80
    unsigned char DeviceDescription[BTDEVICE_MAX_DESCRIPTION_SIZE];

} BTDEVICE_PARS_1_2, *PBTDEVICE_PARS_1_2;

// Event callback type definition
typedef void (__cdecl BTPORT_EVENT_CALLBACK) (UINT32 code, UINT16 hKernelPort);

// Data callback result code
typedef int DATANOTIFYRESULT;
#define DATA_OVERRUN_ERROR    -1
#define DATA_NOTIFY_SUCCESS    0
#define DATA_EXCEED_HIGH_WATER 1

// Data callback type definition
typedef DATANOTIFYRESULT (__cdecl BTPORT_DATA_CALLBACK) (UINT16 port_handle, void *p_data, UINT16 len);

// structure for get stats
typedef struct _BTKRNL_STATS
{
    int num;
    UINT32 rfc_vpkts_rcvd;
    UINT32 rfc_vbytes_rcvd;
    UINT32 rfc_apkts_rcvd;
    UINT32 rfc_abytes_rcvd;
} BTKRNL_STATS;

// structure to return HW info
typedef struct _BTKRNL_HW_INFO
{
    unsigned long DeviceType;  // device type

    unsigned long DeviceNumber;  // number of the device within the

    unsigned char DeviceDescription[BTDEVICE_MAX_DESCRIPTION_SIZE];
} BTKRNL_HW_INFO, *PBTKRNL_HW_INFO;

// The rest is not needed for WiFi

#endif // __btkrnl_ioctl__h_
