// Include Arduino Library
#ifndef Arduino_h
	#include <Arduino.h>
#endif

// Define Cloud Parameters
// -----------------------
#ifndef _PostMan_Server_
	#define _PostMan_Server_ 				"165.227.154.147"
#endif
#ifndef _PostMan_EndPoint_
	#define _PostMan_EndPoint_ 				"/"
#endif
#ifndef _PostMan_Port_
	#define _PostMan_Port_ 					(uint8_t)80
#endif

// Socket ID Definitions
// ---------------------
#ifndef _PostMan_Incomming_Socket_
	#define _PostMan_Incomming_Socket_		(uint8_t)3
#endif
#ifndef _PostMan_Outgoing_Socket_
	#define _PostMan_Outgoing_Socket_		(uint8_t)2
#endif

// Configuration Definitions
// -------------------------
#ifndef _AT_WAIT_DELAY_
	#define _AT_WAIT_DELAY_					(uint8_t)20
#endif

// Define Functon Timeout
// ----------------------
#ifndef _TIMEOUT_AT_
	#define _TIMEOUT_AT_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_FCLASS_
	#define _TIMEOUT_FCLASS_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CGMI_
	#define _TIMEOUT_CGMI_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CGMM_
	#define _TIMEOUT_CGMM_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SWPKGV_
	#define _TIMEOUT_SWPKGV_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CGSN_
	#define _TIMEOUT_CGSN_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CFUN_
	#define _TIMEOUT_CFUN_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CMEE_
	#define _TIMEOUT_CMEE_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CEER_
	#define _TIMEOUT_CEER_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_ATE_
	#define _TIMEOUT_ATE_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_K_
	#define _TIMEOUT_K_	    			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CPIN_
	#define _TIMEOUT_CPIN_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CCID_
	#define _TIMEOUT_CCID_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SIMDET_
	#define _TIMEOUT_SIMDET_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_GPIO_
	#define _TIMEOUT_GPIO_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SLED_
	#define _TIMEOUT_SLED_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SLEDSAV_
	#define _TIMEOUT_SLEDSAV_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_E2SLRI_
	#define _TIMEOUT_E2SLRI_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CREG_
	#define _TIMEOUT_CREG_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CGDCONT_
	#define _TIMEOUT_CGDCONT_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SGACT_
	#define _TIMEOUT_SGACT_				   	(uint16_t)60000
#endif
#ifndef _TIMEOUT_WS46_
	#define _TIMEOUT_WS46_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_RFSTS_
	#define _TIMEOUT_RFSTS_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_MONI_
	#define _TIMEOUT_MONI_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SCFG_
	#define _TIMEOUT_SCFG_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SCFGEXT_
	#define _TIMEOUT_SCFGEXT_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SCFGEXT2_
	#define _TIMEOUT_SCFGEXT2_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SCFGEXT3_
	#define _TIMEOUT_SCFGEXT3_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_FRWL_
	#define _TIMEOUT_FRWL_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_ICMP_
	#define _TIMEOUT_ICMP_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_Ping_
	#define _TIMEOUT_Ping_				   	(uint16_t)6000
#endif
#ifndef _TIMEOUT_CCLK_
	#define _TIMEOUT_CCLK_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CTZU_
	#define _TIMEOUT_CTZU_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_NITZ_
	#define _TIMEOUT_NITZ_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_CCLKMODE_
	#define _TIMEOUT_CCLKMODE_			   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SD_
	#define _TIMEOUT_SD_				   	(uint16_t)65000
#endif
#ifndef _TIMEOUT_SH_
	#define _TIMEOUT_SH_				   	(uint16_t)5000
#endif
#ifndef _TIMEOUT_SL_
	#define _TIMEOUT_SL_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SA_
	#define _TIMEOUT_SA_				   	(uint16_t)15000
#endif
#ifndef _TIMEOUT_SS_
	#define _TIMEOUT_SS_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SO_
	#define _TIMEOUT_SO_				   	(uint16_t)5000
#endif
#ifndef _TIMEOUT_SI_
	#define _TIMEOUT_SI_				   	(uint16_t)1000
#endif
#ifndef _TIMEOUT_SSEND_
	#define _TIMEOUT_SSEND_				   	(uint16_t)2000
#endif
#ifndef _TIMEOUT_SRECV_
	#define _TIMEOUT_SRECV_				   	(uint16_t)5000
#endif
#ifndef _TIMEOUT_FTPCLOSE_
	#define _TIMEOUT_FTPCLOSE_				(uint16_t)65000
#endif
#ifndef _TIMEOUT_FTPCWD_
	#define _TIMEOUT_FTPCWD_				(uint16_t)65000
#endif
#ifndef _TIMEOUT_FTPFSIZE_
	#define _TIMEOUT_FTPFSIZE_				(uint16_t)65000
#endif
#ifndef _TIMEOUT_FTPGETPKT_
	#define _TIMEOUT_FTPGETPKT_				(uint16_t)65000
#endif
#ifndef _TIMEOUT_FTPOPEN_
	#define _TIMEOUT_FTPOPEN_				(uint16_t)65000
#endif
#ifndef _TIMEOUT_FTPRECV_
	#define _TIMEOUT_FTPRECV_				(uint16_t)65000
#endif
#ifndef _TIMEOUT_FTPTO_
	#define _TIMEOUT_FTPTO_					(uint16_t)65000
#endif
#ifndef _TIMEOUT_FTPTYPE_
	#define _TIMEOUT_FTPTYPE_				(uint16_t)65000
#endif
#ifndef _TIMEOUT_Z_
	#define _TIMEOUT_Z_						(uint16_t)5000
#endif
#ifndef _TIMEOUT_SHDN_
	#define _TIMEOUT_SHDN_					(uint16_t)5000
#endif
#ifndef _TIMEOUT_CSQ_
	#define _TIMEOUT_CSQ_					(uint16_t)1000
#endif
