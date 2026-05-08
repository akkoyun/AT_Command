/* *******************************************************************************
 *  Copyright (C) 2014-2026 Mehmet Gunce Akkoyun Can not be copied and/or
 *	distributed without the express permission of Mehmet Gunce Akkoyun.
 *
 *	Library				: AT_Command
 *	Version				: 02.00.01
 *
 *********************************************************************************/

#pragma once

// Include Arduino Library
#ifndef Arduino_h
	#include <Arduino.h>
#endif

// Cloud Server Parameters (string literals — keep as #define for user override)
// ------------------------------------------------------------------------------
#ifndef _PostMan_Server_
	#define _PostMan_Server_ "165.227.154.147"
#endif
#ifndef _PostMan_EndPoint_
	#define _PostMan_EndPoint_ "/"
#endif

// Numeric Config Parameters
// -------------------------
#ifndef _PostMan_Port_
	constexpr uint8_t _PostMan_Port_ = 80;
#endif
#ifndef _PostMan_CID_
	constexpr uint8_t _PostMan_CID_ = 1;
#endif
#ifndef _AT_WAIT_DELAY_
	constexpr uint8_t _AT_WAIT_DELAY_ = 20;
#endif
#ifndef _AT_UART_READ_DELAY_
	constexpr uint8_t _AT_UART_READ_DELAY_ = 5;
#endif
#ifndef _AT_SD_PROMPT_DELAY_
	constexpr uint8_t _AT_SD_PROMPT_DELAY_ = 10;
#endif

// Function Timeout Definitions (ms) — override before including this header
// --------------------------------------------------------------------------
#ifndef _TIMEOUT_AT_
	constexpr uint16_t _TIMEOUT_AT_ = 1000;
#endif
#ifndef _TIMEOUT_FCLASS_
	constexpr uint16_t _TIMEOUT_FCLASS_ = 1000;
#endif
#ifndef _TIMEOUT_CGMI_
	constexpr uint16_t _TIMEOUT_CGMI_ = 1000;
#endif
#ifndef _TIMEOUT_CGMM_
	constexpr uint16_t _TIMEOUT_CGMM_ = 1000;
#endif
#ifndef _TIMEOUT_SWPKGV_
	constexpr uint16_t _TIMEOUT_SWPKGV_ = 1000;
#endif
#ifndef _TIMEOUT_CGSN_
	constexpr uint16_t _TIMEOUT_CGSN_ = 1000;
#endif
#ifndef _TIMEOUT_CFUN_
	constexpr uint16_t _TIMEOUT_CFUN_ = 1000;
#endif
#ifndef _TIMEOUT_CMEE_
	constexpr uint16_t _TIMEOUT_CMEE_ = 1000;
#endif
#ifndef _TIMEOUT_CEER_
	constexpr uint16_t _TIMEOUT_CEER_ = 1000;
#endif
#ifndef _TIMEOUT_ATE_
	constexpr uint16_t _TIMEOUT_ATE_ = 1000;
#endif
#ifndef _TIMEOUT_K_
	constexpr uint16_t _TIMEOUT_K_ = 1000;
#endif
#ifndef _TIMEOUT_CPIN_
	constexpr uint16_t _TIMEOUT_CPIN_ = 1000;
#endif
#ifndef _TIMEOUT_CCID_
	constexpr uint16_t _TIMEOUT_CCID_ = 1000;
#endif
#ifndef _TIMEOUT_SIMDET_
	constexpr uint16_t _TIMEOUT_SIMDET_ = 1000;
#endif
#ifndef _TIMEOUT_GPIO_
	constexpr uint16_t _TIMEOUT_GPIO_ = 1000;
#endif
#ifndef _TIMEOUT_SLED_
	constexpr uint16_t _TIMEOUT_SLED_ = 1000;
#endif
#ifndef _TIMEOUT_SLEDSAV_
	constexpr uint16_t _TIMEOUT_SLEDSAV_ = 1000;
#endif
#ifndef _TIMEOUT_E2SLRI_
	constexpr uint16_t _TIMEOUT_E2SLRI_ = 1000;
#endif
#ifndef _TIMEOUT_CREG_
	constexpr uint16_t _TIMEOUT_CREG_ = 1000;
#endif
#ifndef _TIMEOUT_CGDCONT_
	constexpr uint16_t _TIMEOUT_CGDCONT_ = 1000;
#endif
#ifndef _TIMEOUT_SGACT_
	constexpr uint16_t _TIMEOUT_SGACT_ = 60000;
#endif
#ifndef _TIMEOUT_WS46_
	constexpr uint16_t _TIMEOUT_WS46_ = 1000;
#endif
#ifndef _TIMEOUT_RFSTS_
	constexpr uint16_t _TIMEOUT_RFSTS_ = 1000;
#endif
#ifndef _TIMEOUT_MONI_
	constexpr uint16_t _TIMEOUT_MONI_ = 1000;
#endif
#ifndef _TIMEOUT_SCFG_
	constexpr uint16_t _TIMEOUT_SCFG_ = 1000;
#endif
#ifndef _TIMEOUT_SCFGEXT_
	constexpr uint16_t _TIMEOUT_SCFGEXT_ = 1000;
#endif
#ifndef _TIMEOUT_SCFGEXT2_
	constexpr uint16_t _TIMEOUT_SCFGEXT2_ = 1000;
#endif
#ifndef _TIMEOUT_SCFGEXT3_
	constexpr uint16_t _TIMEOUT_SCFGEXT3_ = 1000;
#endif
#ifndef _TIMEOUT_FRWL_
	constexpr uint16_t _TIMEOUT_FRWL_ = 1000;
#endif
#ifndef _TIMEOUT_ICMP_
	constexpr uint16_t _TIMEOUT_ICMP_ = 1000;
#endif
#ifndef _TIMEOUT_Ping_
	constexpr uint16_t _TIMEOUT_Ping_ = 6000;
#endif
#ifndef _TIMEOUT_CCLK_
	constexpr uint16_t _TIMEOUT_CCLK_ = 1000;
#endif
#ifndef _TIMEOUT_CTZU_
	constexpr uint16_t _TIMEOUT_CTZU_ = 1000;
#endif
#ifndef _TIMEOUT_NITZ_
	constexpr uint16_t _TIMEOUT_NITZ_ = 1000;
#endif
#ifndef _TIMEOUT_CCLKMODE_
	constexpr uint16_t _TIMEOUT_CCLKMODE_ = 1000;
#endif
#ifndef _TIMEOUT_SD_
	constexpr uint16_t _TIMEOUT_SD_ = 65000;
#endif
#ifndef _TIMEOUT_SH_
	constexpr uint16_t _TIMEOUT_SH_ = 5000;
#endif
#ifndef _TIMEOUT_TCPMAXWIN_
	constexpr uint16_t _TIMEOUT_TCPMAXWIN_ = 1000;
#endif
#ifndef _TIMEOUT_SL_
	constexpr uint16_t _TIMEOUT_SL_ = 1000;
#endif
#ifndef _TIMEOUT_SA_
	constexpr uint16_t _TIMEOUT_SA_ = 15000;
#endif
#ifndef _TIMEOUT_SS_
	constexpr uint16_t _TIMEOUT_SS_ = 1000;
#endif
#ifndef _TIMEOUT_SO_
	constexpr uint16_t _TIMEOUT_SO_ = 5000;
#endif
#ifndef _TIMEOUT_SKTTO_
	constexpr uint16_t _TIMEOUT_SKTTO_ = 1000;
#endif
#ifndef _TIMEOUT_SI_
	constexpr uint16_t _TIMEOUT_SI_ = 1000;
#endif
#ifndef _TIMEOUT_SSEND_
	constexpr uint16_t _TIMEOUT_SSEND_ = 2000;
#endif
#ifndef _TIMEOUT_SRECV_
	constexpr uint16_t _TIMEOUT_SRECV_ = 5000;
#endif
#ifndef _TIMEOUT_DNS_
	constexpr uint16_t _TIMEOUT_DNS_ = 1000;
#endif
#ifndef _TIMEOUT_CACHEDNS_
	constexpr uint16_t _TIMEOUT_CACHEDNS_ = 1000;
#endif
#ifndef _TIMEOUT_FTPCLOSE_
	constexpr uint16_t _TIMEOUT_FTPCLOSE_ = 65000;
#endif
#ifndef _TIMEOUT_FTPCWD_
	constexpr uint16_t _TIMEOUT_FTPCWD_ = 65000;
#endif
#ifndef _TIMEOUT_FTPFSIZE_
	constexpr uint16_t _TIMEOUT_FTPFSIZE_ = 65000;
#endif
#ifndef _TIMEOUT_FTPGETPKT_
	constexpr uint16_t _TIMEOUT_FTPGETPKT_ = 65000;
#endif
#ifndef _TIMEOUT_FTPOPEN_
	constexpr uint16_t _TIMEOUT_FTPOPEN_ = 65000;
#endif
#ifndef _TIMEOUT_FTPRECV_
	constexpr uint16_t _TIMEOUT_FTPRECV_ = 65000;
#endif
#ifndef _TIMEOUT_FTPTO_
	constexpr uint16_t _TIMEOUT_FTPTO_ = 65000;
#endif
#ifndef _TIMEOUT_FTPTYPE_
	constexpr uint16_t _TIMEOUT_FTPTYPE_ = 65000;
#endif
#ifndef _TIMEOUT_Z_
	constexpr uint16_t _TIMEOUT_Z_ = 5000;
#endif
#ifndef _TIMEOUT_SHDN_
	constexpr uint16_t _TIMEOUT_SHDN_ = 5000;
#endif
#ifndef _TIMEOUT_CSQ_
	constexpr uint16_t _TIMEOUT_CSQ_ = 1000;
#endif
